/* *******************************************************************
// (C)opyright 2004
// 
// Institute of Computer Science V
// Prof. M�ner
// University of Mannheim, Germany
// 
// *******************************************************************
// 
// Designer(s):   Steinle / Gl�
// 
// *******************************************************************
// 
// Project:     Trackfinder for CBM-Project at GSI-Darmstadt, Germany
// 
// *******************************************************************
// 
// Description:
//
//   class:
//     - class for structuring the dataObjects for input
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-11-21 13:59:23 $
// $Revision: 1.13 $
//
// *******************************************************************/


#include "../include/dataRootObjectError.h"
#include "../include/dataRootObjectWarningMsg.h"
#include "../include/trackfinderInputData.h"


/****************************************************************
 * method adds a trackIdAccess to the array						*
 ****************************************************************/

void trackfinderInputData::addTrackToIdList(trackfinderInputTrack* track) {

	int trackIndex;
	
	if (track == NULL)
		throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);

	trackIndex = track->getTrackIndex();

	if (trackIndex >= trackFromId.maxEntry) {

		if (trackFromId.maxEntry == 0) {
			trackFromId.list = (trackfinderInputTrack**)calloc((trackIndex + 1), sizeof(trackfinderInputTrack*));
			if (trackFromId.list == NULL)
				throw memoryAllocationError(DATAROOTOBJECTLIB);
		}
		else {
			trackFromId.list = (trackfinderInputTrack**)realloc(trackFromId.list, (trackIndex + 1) * sizeof(trackfinderInputTrack*));
			if (trackFromId.list == NULL)
				throw memoryAllocationError(DATAROOTOBJECTLIB);
		}

		for (int i = trackFromId.maxEntry; i < trackIndex + 1; i++)
			trackFromId.list[i] = NULL;
		
		trackFromId.maxEntry = trackIndex + 1;

	}

	if (trackFromId.list[trackIndex] == NULL)
		trackFromId.list[trackIndex] = track;
	else
		throw twoTracksWithSameIdError(trackIndex);

}

/****************************************************************
 * method removes all trackFromId.list from the array			*
 ****************************************************************/

void trackfinderInputData::removeTrackListFromId() {

	trackFromId.maxEntry = 0;
	if (trackFromId.list != NULL) {
		free(trackFromId.list);
		trackFromId.list = NULL;
	}

}

/****************************************************************
 * method returns a reference to a track with a specified id	*
 ****************************************************************/

trackfinderInputTrack* trackfinderInputData::getTrackById(int id) {

	trackfinderInputTrack* returnValue;

	if (id < trackFromId.maxEntry) {
		returnValue = trackFromId.list[id];
	}
	else
		returnValue = NULL;

	if (returnValue == NULL) {

		numberOfHitsWithWrongTrackId++;

#ifdef EXACTNOTEXISTINGTRACKIDWARNING

		noTrackWithIdFoundWarningMsg* noTrackWithId = new noTrackWithIdFoundWarningMsg(id);
		noTrackWithId->warningMsg();
		if(noTrackWithId != NULL) {
			delete noTrackWithId;
			noTrackWithId = NULL;
		}

#endif

	}

	return returnValue;

}

/****************************************************************
 * method initializes the tracks for the object					*
 ****************************************************************/

void trackfinderInputData::initHits() {

	if (hits == NULL)
		hits = new TClonesArray("trackfinderInputHit");

	removeAllHits();

}

/****************************************************************
 * method initializes the tracks for the object					*
 ****************************************************************/

void trackfinderInputData::initTracks() {

	if (tracks == NULL)
		tracks = new TClonesArray("trackfinderInputTrack");

	removeAllTracks();

}

/****************************************************************
 * method initializes the detector for the object				*
 ****************************************************************/

void trackfinderInputData::initDetector() {

	if (detector == NULL)
		detector = new trackfinderInputDetector();

	detector->removeDetector();

}

/**
 * method initializes the magneticField for the object
 */

void trackfinderInputData::initMagneticField() {

	if (magneticField == NULL) {

#if (ARCHITECTURE == STANDALONE)

		magneticField = new trackfinderInputSMagneticField();

#elif (ARCHITECTURE == CBMROOT)

		magneticField = new trackfinderInputFMagneticField();

#endif

	}

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackfinderInputData::trackfinderInputData() 
  : TObject(),
    eventNumber(0),
    hits(NULL),
    tracks(NULL),
    detector(NULL),
    magneticField(NULL),
    trackFromId(),
    numberOfHitsWithWrongTrackId(0)
{
  /*
	eventNumber                  = 0;
	hits                         = NULL;
	tracks                       = NULL;
	detector                     = NULL;
	magneticField                = NULL;
  */
	trackFromId.maxEntry         = 0;
	trackFromId.list             = NULL;
	/*
	numberOfHitsWithWrongTrackId = 0;
  */
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackfinderInputData::trackfinderInputData(const trackfinderInputData& value) 
  : TObject(value),
    eventNumber(value.eventNumber),
    hits(NULL),
    tracks(NULL),
    detector(new trackfinderInputDetector(*value.detector)),
    magneticField(NULL),
    trackFromId(),
    numberOfHitsWithWrongTrackId(value.numberOfHitsWithWrongTrackId)
 {

	int i;

	this->initHits();
	this->initTracks();

	//	this->eventNumber                  = value.eventNumber;

	if (value.hits == NULL) {

		if (this->hits != NULL) {
			this->removeAllHits();
			delete this->hits;
			this->hits = NULL;
		}

	}
	else {

		for (i = 0; i < value.hits->GetEntries(); i++)
			this->addHit(*((trackfinderInputHit*)value.hits->At(i)));

	}

	if (value.tracks == NULL) {

		if (this->tracks != NULL) {
			this->removeAllTracks();
			delete this->tracks;
			this->tracks = NULL;
		}

	}
	else {

		for (i = 0; i < value.tracks->GetEntries(); i++)
			this->addTrack(*((trackfinderInputTrack*)value.tracks->At(i)));

	}

	//	this->detector                     = new trackfinderInputDetector(*value.detector);
	this->initMagneticField();
	*(this->magneticField)             = *value.magneticField;
	this->trackFromId.maxEntry         = value.trackFromId.maxEntry;
	this->trackFromId.list             = value.trackFromId.list;
	//	this->numberOfHitsWithWrongTrackId = value.numberOfHitsWithWrongTrackId;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackfinderInputData::~trackfinderInputData() {

	if (hits != NULL) {
		removeAllHits();
		delete hits;
		hits = NULL;
	}
	if (tracks != NULL) {
		removeAllTracks();
		delete tracks;
		tracks = NULL;
	}
	if (trackFromId.list != NULL) {
		free(trackFromId.list);
		trackFromId.list = NULL;
	}
	if (detector != NULL) {
		detector->removeDetector();
		delete detector;
		detector = NULL;
	}
	if (magneticField != NULL) {
		delete magneticField;
		magneticField = NULL;
	}

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const trackfinderInputData& trackfinderInputData::operator = (const trackfinderInputData& value) {

	int i;

	if (this->hits != NULL) {
		this->removeAllHits();
		delete this->hits;
		this->hits = NULL;
	}
	if (this->tracks != NULL) {
		this->removeAllTracks();
		delete this->tracks;
		this->tracks = NULL;
	}
	if (this->trackFromId.list != NULL) {
		free(this->trackFromId.list);
		this->trackFromId.list = NULL;
	}
	if (this->detector != NULL) {
		this->detector->removeDetector();
		delete this->detector;
		this->detector = NULL;
	}
	if (this->magneticField != NULL) {
		delete this->magneticField;
		this->magneticField = NULL;
	}

	this->initHits();
	this->initTracks();

	this->eventNumber                  = value.eventNumber;

	if (value.hits == NULL) {

		if (this->hits != NULL) {
			this->removeAllHits();
			delete this->hits;
			this->hits = NULL;
		}

	}
	else {

		for (i = 0; i < value.hits->GetEntries(); i++)
			this->addHit(*((trackfinderInputHit*)value.hits->At(i)));

	}

	if (value.tracks == NULL) {

		if (this->tracks != NULL) {
			this->removeAllTracks();
			delete this->tracks;
			this->tracks = NULL;
		}

	}
	else {

		for (i = 0; i < value.tracks->GetEntries(); i++)
			this->addTrack(*((trackfinderInputTrack*)value.tracks->At(i)));

	}

	this->detector                     = new trackfinderInputDetector(*value.detector);
	this->initMagneticField();
	*(this->magneticField)             = *value.magneticField;
	this->trackFromId.maxEntry         = value.trackFromId.maxEntry;
	this->trackFromId.list             = value.trackFromId.list;
	this->numberOfHitsWithWrongTrackId = value.numberOfHitsWithWrongTrackId;

	return *this;

}

/****************************************************************
 * method initializes the object								*
 ****************************************************************/

void trackfinderInputData::init() {

	initHits();
	initTracks();
	initDetector();
	initMagneticField();

}

/****************************************************************
 * Method inits the default detector.							*
 ****************************************************************/

void trackfinderInputData::initDefaultDetector() {

	if (detector == NULL)
		initDetector();

	return detector->initDefaultDetector();

}

/****************************************************************
 * method returns the number of detector stations				*
 ****************************************************************/

unsigned int trackfinderInputData::getNumberOfStations() {

	if (detector == NULL)
		initDetector();

	return detector->getNumberOfStations();

}

/****************************************************************
 * method returns the number of active detector stations		*
 ****************************************************************/

unsigned int trackfinderInputData::getNumberOfActiveStations() {

	if (detector == NULL)
		initDetector();

	return detector->getNumberOfActiveStations();

}

/****************************************************************
 * This method returns the actual layers id.					*
 * Error:														*
 * - cannotFindDetectorIdError									*
 ****************************************************************/

unsigned short trackfinderInputData::getStationIndex(int stationId) {

	if (detector == NULL)
		initDetector();

	return detector->getStationIndex(stationId);

}

/****************************************************************
 * method returns the number of the actual event				*
 ****************************************************************/

unsigned int trackfinderInputData::getEventNumber() {

	return eventNumber;

}

/****************************************************************
 * method returns the number of hits							*
 ****************************************************************/

int trackfinderInputData::getNumberOfHits() {

	if (hits == NULL)
		initHits();

	return hits->GetEntries();

}

/****************************************************************
 * method returns the number of tracks							*
 ****************************************************************/

int trackfinderInputData::getNumberOfTracks() {

	if (tracks == NULL)
		initTracks();

	return tracks->GetEntries();

}

/****************************************************************
 * method returns a reference to the track data					*
 ****************************************************************/

TClonesArray& trackfinderInputData::getTrackData() {

	if (tracks == NULL)
		initTracks();

	return *tracks;

}

/****************************************************************
 * method returns a reference to a track with a specified order	*
 * index														*
 ****************************************************************/

trackfinderInputTrack* trackfinderInputData::getTrackByOrder(unsigned int index) {

	trackfinderInputTrack* track;

	if (tracks == NULL)
		throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);

	track = (trackfinderInputTrack*)tracks->At(index);
	if (track == NULL)
		throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);

	return track;

}

/****************************************************************
 * method returns a reference to a track with a specified track	*
 * index														*
 ****************************************************************/

trackfinderInputTrack* trackfinderInputData::getTrackByIndex(unsigned int index) {

	trackfinderInputTrack* track;

	if (tracks == NULL)
		throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);

	for (int i = 0; i < getNumberOfTracks(); i++) {

		track = getTrackByOrder(i);
		if (track == NULL)
			throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);

		if ((unsigned int)track->getTrackIndex() == index)
			break;

	}

	return track;

}

/****************************************************************
 * method returns a reference to the point data					*
 ****************************************************************/

TClonesArray& trackfinderInputData::getHitData() {

	if (hits == NULL)
		initHits();

	return *hits;

}

/****************************************************************
 * method returns a reference to a hit with a specified index	*
 ****************************************************************/

trackfinderInputHit* trackfinderInputData::getHitByIndex(unsigned int index) {

	trackfinderInputHit* hit;

	if (hits == NULL)
		throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);

	hit = (trackfinderInputHit*)hits->At(index);
	if (hit == NULL)
		throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);

	return hit;

}

/****************************************************************
 * method gets the whole detector with all stations				*
 ****************************************************************/

trackfinderInputDetector& trackfinderInputData::getDetector() {

	if (detector == NULL)
		initDetector();

	return *detector;

}

/****************************************************************
 * method gets the whole detector with all stations				*
 ****************************************************************/

trackfinderInputDetector* trackfinderInputData::getDetectorPointer() {

	return detector;

}

/****************************************************************
 * method sets the number of the actual event					*
 ****************************************************************/

void trackfinderInputData::setEventNumber(unsigned int number) {

	eventNumber = number;

}

/****************************************************************
 * method adds a track to the end of the Track-TClonesArray		*
 ****************************************************************/

void trackfinderInputData::addTrack(trackfinderInputTrack& track) {

	trackfinderInputTrack* test;

	if (tracks == NULL)
		initTracks();

	TClonesArray& tracksReference = *tracks;
	test = new(tracksReference[tracks->GetEntries()]) trackfinderInputTrack(track);

	if (test == NULL)
		throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);

}
void trackfinderInputData::addTrack(CbmMCTrack* track, int trackId, double charge) {

	trackfinderInputTrack* test;

	if (tracks == NULL)
		initTracks();

	TClonesArray& tracksReference = *tracks;
	test = new(tracksReference[tracks->GetEntries()]) trackfinderInputTrack(track, trackId, charge);

	if (test == NULL)
		throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);

	addTrackToIdList(test);

}
void trackfinderInputData::addTrack(int pdgId, double momX, double momY, double momZ, int trackId, double charge) {

	trackfinderInputTrack* test;

	if (tracks == NULL)
		initTracks();

	TClonesArray& tracksReference = *tracks;
	test = new(tracksReference[tracks->GetEntries()]) trackfinderInputTrack(pdgId, momX, momY, momZ, trackId, charge);

	if (test == NULL)
		throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);

	addTrackToIdList(test);

}

/****************************************************************
 * method adds a hit to the end of the Hit-TClonesArray			*
 ****************************************************************/

void trackfinderInputData::addHit(trackfinderInputHit& hit) {

	int                  numberOfHits;
	trackfinderInputHit* test;

	if (hits == NULL)
		initHits();

	TClonesArray& hitsReference = *hits;

	numberOfHits = hits->GetEntries();
	test = new(hitsReference[numberOfHits]) trackfinderInputHit(hit, (unsigned int)numberOfHits);

	if (test == NULL)
		throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);

}
void trackfinderInputData::addHit(int detectorId, CbmPixelHit* hit, FairMCPoint* point, int hitIndex) {

	int                      numberOfHits;
	trackfinderInputHit*     test;
	trackfinderInputTrack*   track;
	trackfinderInputStation* station;

	if (hit == NULL)
		throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);

	if (hits == NULL)
		initHits();

	TClonesArray& hitsReference = *hits;

	if (point == NULL)
		track = NULL;
	else
		track = getTrackById(point->GetTrackID());

	if (detectorId < 0)
		station = NULL;
	else
		station = detector->getStationPointerById(detectorId);

	numberOfHits = hits->GetEntries();
	test = new(hitsReference[numberOfHits]) trackfinderInputHit(hit, hitIndex, (unsigned int)numberOfHits);

	if (test == NULL)
		throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);

	test->setPoint(point);
	test->setTrack(track);
	test->setStation(station);

	if (track != NULL)
		track->addHit(test);

#ifdef SUPEREXACTNOTEXISTINGTRACKIDWARNING

	if ((test->getPoint() != NULL) && (test->getTrack() == NULL)) {

		noTrackWithIdFoundWarningMsg* noTrackWithId = new noTrackWithIdFoundWarningMsg(test->getPoint()->GetTrackID(), test);
		noTrackWithId->warningMsg();
		if(noTrackWithId != NULL) {
			delete noTrackWithId;
			noTrackWithId = NULL;
		}

	}

#endif

}
void trackfinderInputData::addHit(int trackId, int detectorId, double hitPosX, double hitPosY, double pointPosX, double pointPosY, double posZ, int hitIndex, int pointIndex, bool maps, bool strip, bool hybrid) {

	int                      numberOfHits;
	trackfinderInputHit*     test;
	trackfinderInputTrack*   track;
	trackfinderInputStation* station;

	if (hits == NULL)
		initHits();

	TClonesArray& hitsReference = *hits;

	if (trackId < 0)
		track = NULL;
	else
		track = getTrackById(trackId);

	if (detectorId < 0)
		station = NULL;
	else
		station = detector->getStationPointerById(detectorId);

	numberOfHits = hits->GetEntries();
	test = new(hitsReference[numberOfHits]) trackfinderInputHit(detectorId, hitPosX, hitPosY, posZ, hitPosX - pointPosX, hitPosY - pointPosY, hitIndex, (unsigned int)numberOfHits, pointIndex, maps, strip, hybrid);

	if (test == NULL)
		throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);

	if (trackId < 0)
		test->setPoint(NULL);
	else
		test->setPoint(new CbmStsPoint(trackId, detectorId, TVector3(pointPosX, pointPosY, posZ), TVector3(pointPosX, pointPosY, posZ), TVector3(-1, -1, -1), TVector3(-1, -1, -1), -1, -1, -1));

	test->setTrack(track);
	test->setStation(station);

	if (track != NULL)
		track->addHit(test);

#ifdef SUPEREXACTNOTEXISTINGTRACKIDWARNING

	if ((test->getPoint() != NULL) && (test->getTrack() == NULL)) {

		noTrackWithIdFoundWarningMsg* noTrackWithId = new noTrackWithIdFoundWarningMsg(trackId, test);
		noTrackWithId->warningMsg();
		if(noTrackWithId != NULL) {
			delete noTrackWithId;
			noTrackWithId = NULL;
		}

	}

#endif

}
void trackfinderInputData::addHit(int trackId, int detectorId, double hitPosX, double hitPosY, double pointPosX, double pointPosY, double posZ, int hitIndex, int pointIndex) {

	int                      numberOfHits;
	trackfinderInputHit*     test;
	trackfinderInputTrack*   track;
	trackfinderInputStation* station;

	if (hits == NULL)
		initHits();

	TClonesArray& hitsReference = *hits;

	if (trackId < 0)
		track = NULL;
	else
		track = getTrackById(trackId);

	if (detectorId < 0)
		station = NULL;
	else
		station = detector->getStationPointerById(detectorId);

	numberOfHits = hits->GetEntries();
	test = new(hitsReference[numberOfHits]) trackfinderInputHit(detectorId, hitPosX, hitPosY, posZ, hitPosX - pointPosX, hitPosY - pointPosY, hitIndex, (unsigned int)numberOfHits, pointIndex);

	if (test == NULL)
		throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);

	if (trackId < 0)
		test->setPoint(NULL);
	else
		test->setPoint(new CbmStsPoint(trackId, detectorId, TVector3(pointPosX, pointPosY, posZ), TVector3(pointPosX, pointPosY, posZ), TVector3(-1, -1, -1), TVector3(-1, -1, -1), -1, -1, -1));

	test->setTrack(track);
	test->setStation(station);

	if (track != NULL)
		track->addHit(test);

#ifdef SUPEREXACTNOTEXISTINGTRACKIDWARNING

	if ((test->getPoint() != NULL) && (test->getTrack() == NULL)) {

		noTrackWithIdFoundWarningMsg* noTrackWithId = new noTrackWithIdFoundWarningMsg(trackId, test);
		noTrackWithId->warningMsg();
		if(noTrackWithId != NULL) {
			delete noTrackWithId;
			noTrackWithId = NULL;
		}

	}

#endif

}
/****************************************************************
 * method sets the detector information in the data struct		*
 ****************************************************************/

void trackfinderInputData::addStation(trackfinderInputStation& actualStation) {

	if (detector == NULL)
		initDetector();

	detector->addStation(actualStation);

}


/****************************************************************
 * method masks the detector stations							*
 ****************************************************************/

void trackfinderInputData::maskDetector(bitArray mask) {

	for (unsigned int i = 0; i < detector->getNumberOfStations(); i++)
		detector->getStationByIndex(i).setMask(mask[i]);

	detector->reIndex();

}

/****************************************************************
 * method removes all tracks from the Track-TClonesArray		*
 ****************************************************************/

void trackfinderInputData::removeAllTracks() {

	if (tracks == NULL)
		initTracks();

	tracks->RemoveAll();

	removeTrackListFromId();

}

/****************************************************************
 * method removes all hits from the Hit-TClonesArray			*
 ****************************************************************/

void trackfinderInputData::removeAllHits() {

	if (hits == NULL)
		initHits();

	hits->RemoveAll();

	removeTrackListFromId();

}

/****************************************************************
 * method removes all detector stations							*
 ****************************************************************/

void trackfinderInputData::removeDetector() {

	if (detector == NULL)
		initDetector();

	detector->removeDetector();

}

/****************************************************************
 * method finalizes the hit/track assignment					*
 ****************************************************************/

void trackfinderInputData::finalizeHitTrackAssignment(bool applyAssignmentCheck) {

	trackfinderInputTrack* track;
	trackfinderInputHit*   hit;

#ifdef TRACKWITHNOPOINTSWARNING

	unsigned short         numberOfPointIncorrects;

#endif

#ifdef TRACKWITHNOHITSWARNING

	unsigned short         numberOfHitIncorrects;

#endif

#ifdef TRACKWITHHITSTOPOINTSWARNING

	unsigned short         numberOfPointToHitIncorrects;

#endif

#ifdef HITWITHNOPOINTWARNING

	unsigned short         numberOfMissingPoints;

#endif

#ifdef HITWITHNOTRACKWARNING

	unsigned short         numberOfMissingTracks;

#endif

	removeTrackListFromId();

#ifdef NOTEXISTINGTRACKIDWARNING

	if (numberOfHitsWithWrongTrackId != 0) {

		tracksWithNoIdWarningMsg* tracksWithNoId = new tracksWithNoIdWarningMsg(numberOfHitsWithWrongTrackId);
		tracksWithNoId->warningMsg();
		if(tracksWithNoId != NULL) {
			delete tracksWithNoId;
			tracksWithNoId = NULL;
		}

	}

#endif

	numberOfHitsWithWrongTrackId = 0;

	/* Check if a track exists with no point/hit or a with a wrong assignment */

#ifdef TRACKWITHNOPOINTSWARNING

	numberOfPointIncorrects      = 0;

#endif

#ifdef TRACKWITHNOHITSWARNING

	numberOfHitIncorrects        = 0;

#endif

#ifdef TRACKWITHHITSTOPOINTSWARNING

	numberOfPointToHitIncorrects = 0;

#endif

	if (tracks != NULL) {

		for (int i = 0; i < tracks->GetEntries(); i++) {
			
			track = (trackfinderInputTrack*)tracks->At(i);
			if (track == NULL)
				throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);

			if (track->getNumberOfPoints() == 0) {

#ifdef TRACKWITHNOPOINTSWARNING

				numberOfPointIncorrects++;

#endif

#ifdef EXACTTRACKWITHNOPOINTSWARNING

				trackWithNoPointWarningMsg* trackWithNoPoint = new trackWithNoPointWarningMsg(track->getTrackIndex());
				trackWithNoPoint->warningMsg();
				if(trackWithNoPoint != NULL) {
					delete trackWithNoPoint;
					trackWithNoPoint = NULL;
				}

#endif

			}

			if (track->getNumberOfHits() == 0) {

#ifdef TRACKWITHNOHITSWARNING

				numberOfHitIncorrects++;

#endif

#ifdef EXACTTRACKWITHNOHITSWARNING

				trackWithNoHitWarningMsg* trackWithNoHit = new trackWithNoHitWarningMsg(track->getTrackIndex());
				trackWithNoHit->warningMsg();
				if(trackWithNoHit != NULL) {
					delete trackWithNoHit;
					trackWithNoHit = NULL;
				}

#endif

			}

			if (track->getNumberOfPoints() != track->getNumberOfHits()) {

#ifdef TRACKWITHHITSTOPOINTSWARNING

				numberOfPointToHitIncorrects++;

#endif

#ifdef EXACTTRACKWITHHITSTOPOINTSWARNING

				trackWithWrongNumberOfHitsToPointsWarningMsg* trackWithWrongNumberOfHits = new trackWithWrongNumberOfHitsToPointsWarningMsg(track->getTrackIndex(), track->getNumberOfPoints(), track->getNumberOfHits());
				trackWithWrongNumberOfHits->warningMsg();
				if(trackWithWrongNumberOfHits != NULL) {
					delete trackWithWrongNumberOfHits;
					trackWithWrongNumberOfHits = NULL;
				}

#endif

			}

		}

#ifdef TRACKWITHNOPOINTSWARNING

		if ((numberOfPointIncorrects > 0) && applyAssignmentCheck) {

			tracksWithNoPointWarningMsg* tracksWithNoPoint = new tracksWithNoPointWarningMsg(numberOfPointIncorrects);
			tracksWithNoPoint->warningMsg();
			if(tracksWithNoPoint != NULL) {
				delete tracksWithNoPoint;
				tracksWithNoPoint = NULL;
			}

		}

#endif

#ifdef TRACKWITHNOHITSWARNING

		if ((numberOfHitIncorrects > 0) && applyAssignmentCheck) {

			tracksWithNoHitWarningMsg* tracksWithNoHit = new tracksWithNoHitWarningMsg(numberOfHitIncorrects);
			tracksWithNoHit->warningMsg();
			if(tracksWithNoHit != NULL) {
				delete tracksWithNoHit;
				tracksWithNoHit = NULL;
			}

		}

#endif

#ifdef TRACKWITHHITSTOPOINTSWARNING

		/* This warning is just important if at least one hit is found or not each hit.
		 * This is because then either every hit has a point or no hit has a point.
		 * That no hit has a point is possible because then just debugging is not
		 * possible. But the algorithm still works. */
		if ((numberOfPointToHitIncorrects > 0) && applyAssignmentCheck) {

			tracksWithWrongNumberOfHitsToPointsWarningMsg* tracksWithWrongNumberOfHitsToPoints = new tracksWithWrongNumberOfHitsToPointsWarningMsg(numberOfPointToHitIncorrects);
			tracksWithWrongNumberOfHitsToPoints->warningMsg();
			if(tracksWithWrongNumberOfHitsToPoints != NULL) {
				delete tracksWithWrongNumberOfHitsToPoints;
				tracksWithWrongNumberOfHitsToPoints = NULL;
			}

		}

#endif

	}

#ifdef HITWITHNOPOINTWARNING

	numberOfMissingPoints = 0;

#endif

#ifdef HITWITHNOTRACKWARNING

	numberOfMissingTracks = 0;

#endif

	if (hits != NULL) {

		for (int i = 0; i < hits->GetEntries(); i++) {
			
			hit = (trackfinderInputHit*)hits->At(i);
			if (hit == NULL)
				throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);

			if (hit->getPoint() == NULL) {

#ifdef HITWITHNOPOINTWARNING

				numberOfMissingPoints++;

#endif

#ifdef EXACTHITWITHNOPOINTWARNING

				hitWithNoPointWarningMsg* hitWithNoPoint = new hitWithNoPointWarningMsg(hit->getHitIndex());
				hitWithNoPoint->warningMsg();
				if(hitWithNoPoint != NULL) {
					delete hitWithNoPoint;
					hitWithNoPoint = NULL;
				}

#endif

			}

			if (hit->getTrack() == NULL) {

#ifdef HITWITHNOTRACKWARNING

				numberOfMissingTracks++;

#endif

#ifdef EXACTHITWITHNOTRACKWARNING

				hitWithNoTrackWarningMsg* hitWithNoTrack = new hitWithNoTrackWarningMsg(hit->getHitIndex());
				hitWithNoTrack->warningMsg();
				if(hitWithNoTrack != NULL) {
					delete hitWithNoTrack;
					hitWithNoTrack = NULL;
				}

#endif

			}

		}

#ifdef HITWITHNOPOINTWARNING

		if ((numberOfMissingPoints > 0) && applyAssignmentCheck) {

			hitsWithNoPointWarningMsg* hitsWithNoPoint = new hitsWithNoPointWarningMsg(numberOfMissingPoints);
			hitsWithNoPoint->warningMsg();
			if(hitsWithNoPoint != NULL) {
				delete hitsWithNoPoint;
				hitsWithNoPoint = NULL;
			}

		}

#endif

#ifdef HITWITHNOTRACKWARNING

		if ((numberOfMissingTracks > 0) && applyAssignmentCheck) {

			hitsWithNoTrackWarningMsg* hitsWithNoTrack = new hitsWithNoTrackWarningMsg(numberOfMissingTracks);
			hitsWithNoTrack->warningMsg();
			if(hitsWithNoTrack != NULL) {
				delete hitsWithNoTrack;
				hitsWithNoTrack = NULL;
			}

		}

#endif

	}

	if ((tracks == NULL) || (hits == NULL))
		throw cannotFinalizeHitTrackAssignmentBeforeInitializingTracksError();

}

/****************************************************************
 * method returns the magnetic field object						*
 ****************************************************************/

#if (ARCHITECTURE == STANDALONE)

trackfinderInputSMagneticField* trackfinderInputData::getMagneticField() {

#elif (ARCHITECTURE == CBMROOT)

trackfinderInputFMagneticField* trackfinderInputData::getMagneticField() {

#endif

	if (magneticField == NULL)
		initMagneticField();

	return magneticField;

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double trackfinderInputData::getReservedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(eventNumber);
	returnValue += sizeof(hits);
	returnValue += sizeof(tracks);
	returnValue += sizeof(detector);
	returnValue += sizeof(magneticField);
	returnValue += sizeof(trackFromId);
	returnValue += sizeof(numberOfHitsWithWrongTrackId);

	returnValue = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double trackfinderInputData::getAllocatedSizeOfData(unsigned short dimension) {

	trackfinderInputHit*   hit;
	trackfinderInputTrack* track;
	double                 returnValue;

	returnValue  = 0;

	if (hits != NULL) {

		for (int i = 0; i < hits->GetEntries(); i++) {
			
			hit = (trackfinderInputHit*)hits->At(i);
			if (hit == NULL)
				throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);

			returnValue += hit->getReservedSizeOfData(0);
			returnValue += hit->getAllocatedSizeOfData(0);

		}

		returnValue += (hits->Capacity() - hits->GetEntries()) * sizeof(trackfinderInputHit);

	}

	if (tracks != NULL) {

		for (int j = 0; j < tracks->GetEntries(); j++) {
			
			track = (trackfinderInputTrack*)tracks->At(j);
			if (hit == NULL)
				throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);

			returnValue += track->getReservedSizeOfData(0);
			returnValue += track->getAllocatedSizeOfData(0);

		}

		returnValue += (tracks->Capacity() - tracks->GetEntries()) * sizeof(trackfinderInputTrack);

	}

	if (detector != NULL) {
		returnValue += detector->getReservedSizeOfData(0);
		returnValue += detector->getAllocatedSizeOfData(0);
	}
	if (magneticField != NULL) {
		returnValue += magneticField->getReservedSizeOfData(0);
		returnValue += magneticField->getAllocatedSizeOfData(0);
	}
	if (trackFromId.maxEntry > 0)
		returnValue += trackFromId.maxEntry * sizeof(trackFromId.list[0]);

	returnValue = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double trackfinderInputData::getUsedSizeOfData(unsigned short dimension) {

	trackfinderInputHit*   hit;
	trackfinderInputTrack* track;
	double                 returnValue;

	returnValue  = sizeof(eventNumber);
	if (hits != NULL) {

		for (int i = 0; i < hits->GetEntries(); i++) {
			
			hit = (trackfinderInputHit*)hits->At(i);
			if (hit == NULL)
				throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);

			returnValue += hit->getUsedSizeOfData(0);

		}

	}

	if (tracks != NULL) {

		for (int j = 0; j < tracks->GetEntries(); j++) {
			
			track = (trackfinderInputTrack*)tracks->At(j);
			if (hit == NULL)
				throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);

			returnValue += track->getUsedSizeOfData(0);

		}

	}

	if (detector != NULL)
		returnValue += detector->getUsedSizeOfData(0);
	if (magneticField != NULL)
		returnValue += magneticField->getUsedSizeOfData(0);
	returnValue += sizeof(trackFromId);
	if (trackFromId.maxEntry > 0)
		returnValue += trackFromId.maxEntry * sizeof(trackFromId.list[0]);
	returnValue += sizeof(numberOfHitsWithWrongTrackId);

	returnValue = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double trackfinderInputData::getReservedSizeOfAddOnData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double trackfinderInputData::getAllocatedSizeOfAddOnData(unsigned short dimension) {

	trackfinderInputHit*   hit;
	trackfinderInputTrack* track;
	double                 returnValue;

	returnValue  = 0;

	if (hits != NULL) {

		for (int i = 0; i < hits->GetEntries(); i++) {
			
			hit = (trackfinderInputHit*)hits->At(i);
			if (hit == NULL)
				throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);

			returnValue += hit->getReservedSizeOfAddOnData(0);
			returnValue += hit->getAllocatedSizeOfAddOnData(0);

		}

	}

	if (tracks != NULL) {

		for (int j = 0; j < tracks->GetEntries(); j++) {
			
			track = (trackfinderInputTrack*)tracks->At(j);
			if (hit == NULL)
				throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);

			returnValue += track->getReservedSizeOfAddOnData(0);
			returnValue += track->getAllocatedSizeOfAddOnData(0);

		}

	}

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double trackfinderInputData::getUsedSizeOfAddOnData(unsigned short dimension) {

	trackfinderInputHit*   hit;
	trackfinderInputTrack* track;
	double                 returnValue;

	returnValue  = sizeof(eventNumber);
	if (hits != NULL) {

		for (int i = 0; i < hits->GetEntries(); i++) {
			
			hit = (trackfinderInputHit*)hits->At(i);
			if (hit == NULL)
				throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);

			returnValue += hit->getUsedSizeOfAddOnData(0);

		}

	}

	if (tracks != NULL) {

		for (int j = 0; j < tracks->GetEntries(); j++) {
			
			track = (trackfinderInputTrack*)tracks->At(j);
			if (hit == NULL)
				throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);

			returnValue += track->getUsedSizeOfAddOnData(0);

		}

	}

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

ClassImp(trackfinderInputData)
