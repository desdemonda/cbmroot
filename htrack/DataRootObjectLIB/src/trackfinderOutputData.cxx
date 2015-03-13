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
//     - class for structuring the dataObjects for output
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-10-07 10:36:51 $
// $Revision: 1.11 $
//
// *******************************************************************/


#include "../../MiscLIB/include/defs.h"
#if (ARCHITECTURE == STANDALONE)
	#include "../../RootFrameworkLIB/include/FairTrackParam.h"
#elif (ARCHITECTURE == CBMROOT)
	#include "FairTrackParam.h"
#endif
#include "../include/dataRootObjectError.h"
#include "../include/trackfinderOutputData.h"


/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackfinderOutputData::trackfinderOutputData() 
  : TObject(), 
    tracks(NULL),
    actualTrack(NULL),
    tracksAreLocal(true)
{
  /*
	tracks         = NULL;
	actualTrack    = NULL;
	tracksAreLocal = true;
*/
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackfinderOutputData::trackfinderOutputData(const trackfinderOutputData& value) 
  : TObject(value),
    tracks(NULL),
    actualTrack(NULL),
    tracksAreLocal(true)
{

	int index;

	if (value.tracks == NULL) {

		this->tracks         = NULL;
		this->actualTrack    = NULL;
		this->tracksAreLocal = true;

	}
	else {

		if (value.tracksAreLocal) {

			init();
			for (int i = 0; i < value.tracks->GetEntries(); i++)
				this->addTrack(*((CbmStsTrack*)value.tracks->At(i)));

		}
		else
			init(value.tracks);

		index                = value.tracks->IndexOf(value.actualTrack);
		this->actualTrack    = (CbmStsTrack*)this->tracks->At(index);
		this->tracksAreLocal = value.tracksAreLocal;
	}

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackfinderOutputData::~trackfinderOutputData() {

	if (tracksAreLocal) {

		if (tracks != NULL) {
			removeAllTracks();
			delete tracks;
			tracks = NULL;
		}

	}

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const trackfinderOutputData& trackfinderOutputData::operator = (const trackfinderOutputData& value) {

	int index;

	if (tracksAreLocal) {

		if (tracks != NULL) {
			removeAllTracks();
			delete tracks;
			tracks = NULL;
		}

	}

	if (value.tracks == NULL) {

		this->tracks         = NULL;
		this->actualTrack    = NULL;
		this->tracksAreLocal = true;

	}
	else {

		if (value.tracksAreLocal) {

			init();
			for (int i = 0; i < value.tracks->GetEntries(); i++)
				this->addTrack(*((CbmStsTrack*)value.tracks->At(i)));

		}
		else
			init(value.tracks);

		index                = value.tracks->IndexOf(value.actualTrack);
		this->actualTrack    = (CbmStsTrack*)this->tracks->At(index);
		this->tracksAreLocal = value.tracksAreLocal;

	}

	return *this;
	
}

/****************************************************************
 * initializes the object										*
 ****************************************************************/

void trackfinderOutputData::init() {

	if (tracksAreLocal) {

		if (tracks != NULL) {
			removeAllTracks();
			delete tracks;
			tracks = NULL;
		}

	}

	tracks         = new TClonesArray("CbmStsTrack");
	if (tracks == NULL)
		throw cannotAccessHitsOrTracksError(DATAOBJECTLIB);

	tracksAreLocal = true;

}
void trackfinderOutputData::init(TClonesArray* _tracks) {

	if (tracksAreLocal) {

		if (this->tracks != NULL) {
			removeAllTracks();
			delete this->tracks;
			this->tracks = NULL;
		}

	}

	this->tracks   = _tracks;

	tracksAreLocal = false;

}

/****************************************************************
 * method returns the number of tracks							*
 ****************************************************************/

int trackfinderOutputData::getNumberOfTracks() {

	int returnValue;

	if (tracks == NULL)
		init();

	returnValue = tracks->GetEntries();
	
	return returnValue;

}

/****************************************************************
 * method returns a reference to the track data					*
 ****************************************************************/

TClonesArray* trackfinderOutputData::getTrackData() {

	TClonesArray* returnValue;

	if (tracks == NULL)
		init();

	returnValue = tracks;

	return returnValue;

}

/****************************************************************
 * method returns a reference to a track with a specified index	*
 ****************************************************************/

CbmStsTrack* trackfinderOutputData::getTrackByOrder(unsigned int index) {

	CbmStsTrack* track;

	if (tracks == NULL)
		throw cannotAccessHitsOrTracksError(DATAOBJECTLIB);

	if (index < (unsigned int)getNumberOfTracks())
		track = (CbmStsTrack*)tracks->At(index);
	else
		throw tooBigTrackIndexError();

	if (track == NULL)
		throw cannotAccessHitsOrTracksError(DATAOBJECTLIB);

	return track;

}

/****************************************************************
 * method adds a track to the end of the Track-TClonesArray		*
 ****************************************************************/

void trackfinderOutputData::addTrack(CbmStsTrack& track) {

	CbmStsTrack* test;

	if (tracks == NULL)
		init();

	TClonesArray& tracksReference = *tracks;

	test = new(tracksReference[tracks->GetEntries()]) CbmStsTrack();
	if (test == NULL)
		throw cannotAccessHitsOrTracksError(DATAOBJECTLIB);

	*test = track;

}
void trackfinderOutputData::addTrack(trackMomentum& momentum) {

	trackFrameworkMomentum p = (trackFrameworkMomentum)momentum;
	
	addTrack(p);

}

/****************************************************************
 * method adds a track to the end of the Track-TClonesArray		*
 ****************************************************************/

void trackfinderOutputData::addTrack(trackFrameworkMomentum& momentum) {

	FairTrackParam trackParameter;

	if (tracks == NULL)
		init();

	TClonesArray& tracksReference = *tracks;

	actualTrack = new(tracksReference[tracks->GetEntries()]) CbmStsTrack();
	if (actualTrack == NULL)
		throw cannotAccessHitsOrTracksError(DATAOBJECTLIB);

	trackParameter.SetTx(momentum.getTx());
	trackParameter.SetTy(momentum.getTy());
	trackParameter.SetQp(momentum.getQp());
	trackParameter.SetX(-1);
	trackParameter.SetY(-1);
	trackParameter.SetZ(-1);

	actualTrack->SetParamFirst(&trackParameter);
	actualTrack->SetParamLast(&trackParameter);

}

/****************************************************************
 * method adds a hit to the end of the Hit-TClonesArray			*
 ****************************************************************/

void trackfinderOutputData::addHitToActualTrack(trackfinderInputHit* hit) {

	FairTrackParam firstTrackParameter;
	FairTrackParam lastTrackParameter;

	if (actualTrack != NULL) {

		firstTrackParameter = *actualTrack->GetParamFirst();
		lastTrackParameter  = *actualTrack->GetParamLast();
		
		/* first input for trackParameter has to delete the dummies */
		if (lastTrackParameter.GetZ() < 0) {

			firstTrackParameter.SetX(hit->getPosX());
			firstTrackParameter.SetY(hit->getPosY());
			firstTrackParameter.SetZ(hit->getPosZ());
			lastTrackParameter.SetX(hit->getPosX());
			lastTrackParameter.SetY(hit->getPosY());
			lastTrackParameter.SetZ(hit->getPosZ());

		}
		/* not first input for trackParameter */
		else {

			if (hit->getPosZ() < firstTrackParameter.GetZ()) {

				firstTrackParameter.SetX(hit->getPosX());
				firstTrackParameter.SetY(hit->getPosY());
				firstTrackParameter.SetZ(hit->getPosZ());

			}
			if (hit->getPosZ() > lastTrackParameter.GetZ()) {

				lastTrackParameter.SetX(hit->getPosX());
				lastTrackParameter.SetY(hit->getPosY());
				lastTrackParameter.SetZ(hit->getPosZ());

			}

		}

#ifdef HITCOMPATIBILITY

		if (hit->isMapsHit())
			actualTrack->AddMapsHit(hit->getHitIndex(), (CbmStsMapsHit*)hit->getHit());
		else if (hit->isHybridHit())
			actualTrack->AddHybridHit(hit->getHitIndex(), (CbmStsHybridHit*)hit->getHit());
		else if (hit->isStripHit())
			actualTrack->AddStripHit(hit->getHitIndex(), (CbmStsStripHit*)hit->getHit());
		else
			actualTrack->AddHit(hit->getHitIndex(), (CbmStsHit*)hit->getHit());

#else

		if (hit->isMapsHit())
			actualTrack->AddMvdHit(hit->getHitIndex());//, (FairHit*)hit->getHit());
		else
			actualTrack->AddHit(hit->getHitIndex(), kSTSHIT);//, (FairHit*)hit->getHit());

#endif

		actualTrack->SetParamFirst(&firstTrackParameter);
		actualTrack->SetParamLast(&lastTrackParameter);

	}
	else
		throw cannotAccessHitsOrTracksError(DATAOBJECTLIB);

}

/****************************************************************
 * method removes all tracks from the Track-TClonesArray		*
 ****************************************************************/

void trackfinderOutputData::removeAllTracks() {

	if (tracks == NULL)
		init();

	tracks->RemoveAll();

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double trackfinderOutputData::getReservedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(tracks);
	returnValue += sizeof(actualTrack);
	returnValue += sizeof(tracksAreLocal);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double trackfinderOutputData::getAllocatedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;

	if (tracks != NULL)
		returnValue += tracks->Capacity() * sizeof(CbmStsTrack);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double trackfinderOutputData::getUsedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(tracks);
	if (tracks != NULL)
		returnValue += tracks->GetEntries() * sizeof(CbmStsTrack);
	returnValue += sizeof(actualTrack);
	returnValue += sizeof(tracksAreLocal);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

ClassImp(trackfinderOutputData)
