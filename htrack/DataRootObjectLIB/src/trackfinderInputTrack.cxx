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
//     - consists of the information for the tracks
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2007-10-19 14:33:12 $
// $Revision: 1.3 $
//
// *******************************************************************/


#include "../include/dataRootObjectError.h"
#include "../include/trackfinderInputTrack.h"


/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackfinderInputTrack::trackfinderInputTrack() 
  : TObject(),
    track(NULL),
    hits(),
    info(),
    isPointer(true),
    trackIndex(0),
    charge(0)
{

  //	track                    = NULL;
  //	hits.clear();
  //	info.frequency           = 0;
  //	info.maximumNumberOfHits = 0;
  //	isPointer                = true;
  //	trackIndex               = 0;
  //	charge                   = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackfinderInputTrack::trackfinderInputTrack(const trackfinderInputTrack& value) 
  : TObject(value), 
    track(NULL),
    hits(value.hits),
    info(),
    isPointer(value.isPointer),
    trackIndex(value.trackIndex),
    charge(value.charge)
{

	if (value.isPointer)
		this->track                = value.track;
	else
		this->track                = new CbmMCTrack(*value.track);

	//	this->hits                     = value.hits;
	this->info.frequency           = value.info.frequency;
	this->info.maximumNumberOfHits = value.info.maximumNumberOfHits;
	//	this->isPointer                = value.isPointer;
	//	this->trackIndex               = value.trackIndex;
	//	this->charge                   = value.charge;

}
trackfinderInputTrack::trackfinderInputTrack(CbmMCTrack* mcTrack, int newTrackIndex, double newCharge) 
  : TObject(), 
    track(mcTrack),
    hits(),
    info(),
    isPointer(true),
    trackIndex(newTrackIndex),
    charge(newCharge)
{

  //	track                    = mcTrack;
	if (track == NULL)
		throw cannotAccessHitsOrTracksError(DATAOBJECTLIB);
	/*
	hits.clear();
	info.frequency           = 0;
	info.maximumNumberOfHits = 0;
	isPointer                = true;
	trackIndex               = newTrackIndex;
	charge                   = newCharge;
	*/
}
trackfinderInputTrack::trackfinderInputTrack(int pdgId, double momX, double momY, double momZ, int newTrackIndex, double newCharge) 
  : TObject(),
    track(new CbmMCTrack(pdgId, -1, momX, momY, momZ, -1.0, -1.0, -1.0, -1.0, 0)),
    hits(),
    info(),
    isPointer(false),
    trackIndex(newTrackIndex),
    charge(newCharge)
{

  //	track                    = new CbmMCTrack(pdgId, -1, momX, momY, momZ, -1.0, -1.0, -1.0, -1.0, 0);
	if (track == NULL)
		throw cannotAccessHitsOrTracksError(DATAOBJECTLIB);
	/*
	hits.clear();
	info.frequency           = 0;
	info.maximumNumberOfHits = 0;
	isPointer                = false;
	trackIndex               = newTrackIndex;
	charge                   = newCharge;
	*/
}


/****************************************************************
 * Destructor													*
 ****************************************************************/

trackfinderInputTrack::~trackfinderInputTrack() {

	if (!isPointer)
		if (track != NULL)
			delete track;
	track = NULL;

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const trackfinderInputTrack& trackfinderInputTrack::operator = (const trackfinderInputTrack& value) {

	if (!this->isPointer)
		if (this->track != NULL)
			delete this->track;

	if (value.isPointer)
		this->track                = value.track;
	else
		this->track                = new CbmMCTrack(*value.track);
	this->hits                     = value.hits;
	this->info.frequency           = value.info.frequency;
	this->info.maximumNumberOfHits = value.info.maximumNumberOfHits;
	this->isPointer                = value.isPointer;
	this->trackIndex               = value.trackIndex;
	this->charge                   = value.charge;

	return *this;

}

/****************************************************************
 * method returns the trackIndex								*
 ****************************************************************/

int trackfinderInputTrack::getTrackIndex() {

	return trackIndex;

}

/****************************************************************
 * method returns the charge									*
 ****************************************************************/

double trackfinderInputTrack::getCharge() {

	return charge;

}

/****************************************************************
 * method returns the x momentum								*
 ****************************************************************/

double trackfinderInputTrack::getMomX() {

	double   returnValue = 0;
	TVector3 momentum;
	
	if (track != NULL) {

		track->GetMomentum(momentum);
		returnValue = momentum.X();

	}
	else
		throw cannotAccessHitsOrTracksError(DATAOBJECTLIB);
	
	return returnValue;

}

/****************************************************************
 * method returns the y momentum								*
 ****************************************************************/

double trackfinderInputTrack::getMomY() {

	double   returnValue = 0;
	TVector3 momentum;
	
	if (track != NULL) {

		track->GetMomentum(momentum);
		returnValue = momentum.Y();

	}
	else
		throw cannotAccessHitsOrTracksError(DATAOBJECTLIB);
	
	return returnValue;

}

/****************************************************************
 * method returns the z momentum								*
 ****************************************************************/

double trackfinderInputTrack::getMomZ() {

	double   returnValue = 0;
	TVector3 momentum;
	
	if (track != NULL) {

		track->GetMomentum(momentum);
		returnValue = momentum.Z();

	}
	else
		throw cannotAccessHitsOrTracksError(DATAOBJECTLIB);
	
	return returnValue;

}

/****************************************************************
 * method returns the pdg code									*
 ****************************************************************/

int trackfinderInputTrack::getPdgCode() {

	int returnValue = 0;
	
	if (track != NULL)
		returnValue = track->GetPdgCode();
	else
		throw cannotAccessHitsOrTracksError(DATAOBJECTLIB);
	
	return returnValue;

}

/****************************************************************
 * method removes all hits										*
 ****************************************************************/

void trackfinderInputTrack::removeHits() {

	hits.clear();

}

/****************************************************************
 * method resets the hit pointer to the beginning				*
 ****************************************************************/

void trackfinderInputTrack::resetHitPointer() {

	hits.resetActiveObject();

}

/****************************************************************
 * method returns the number of points							*
 ****************************************************************/

unsigned short trackfinderInputTrack::getNumberOfPoints() {

	unsigned short returnValue = 0;
	
	if (track != NULL)
		returnValue = (unsigned short)track->GetNPoints(kSTS);
	else
		throw cannotAccessHitsOrTracksError(DATAOBJECTLIB);
	
	return returnValue;

}

/****************************************************************
 * method returns the number of hits							*
 ****************************************************************/

unsigned short trackfinderInputTrack::getNumberOfHits() {

	return (unsigned short)hits.getNumberOfEntries();

}

/****************************************************************
 * method returns a hit											*
 ****************************************************************/

trackfinderInputHit* trackfinderInputTrack::getHit() {

	return hits.readActiveObjectAndMakeNextOneActive();

}

/****************************************************************
 * method adds a hit											*
 ****************************************************************/

void trackfinderInputTrack::addHit(trackfinderInputHit* hit) {

	hits.push(hit);

}

/****************************************************************
 * gets the frequency how often the track is found				*
 ****************************************************************/

unsigned short trackfinderInputTrack::getFrequency() {

	return info.frequency;

}

/****************************************************************
 * increments the frequency how often the track is found		*
 ****************************************************************/

void trackfinderInputTrack::incrementFrequency() {

	info.frequency++;

}

/****************************************************************
 * gets the maximum number of hits belonging to the track		*
 ****************************************************************/

unsigned short trackfinderInputTrack::getMaximumNumberOfHits() {

	return info.maximumNumberOfHits;

}

/****************************************************************
 * modifies the maximum number of hits belonging to the track,	*
 * if the value is bigger than the existing one.				*
 ****************************************************************/

void trackfinderInputTrack::modifyMaximumNumberOfHits(unsigned short actualMaximumNumberOfHits) {

	if (actualMaximumNumberOfHits > info.maximumNumberOfHits)
		info.maximumNumberOfHits = actualMaximumNumberOfHits;

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double trackfinderInputTrack::getReservedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(track);
	returnValue += hits.getReservedSizeOfData(0);
	returnValue += sizeof(info);
	returnValue += sizeof(isPointer);
	returnValue += sizeof(trackIndex);
	returnValue += sizeof(charge);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double trackfinderInputTrack::getAllocatedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;
	if (track != NULL)
		returnValue += sizeof(*track);
	returnValue += hits.getAllocatedSizeOfData(0);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double trackfinderInputTrack::getUsedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(track);
	if (track != NULL)
		returnValue += sizeof(*track);
	returnValue += sizeof(hits);
	returnValue += hits.getUsedSizeOfData(0);
	returnValue += sizeof(info);
	returnValue += sizeof(isPointer);
	returnValue += sizeof(trackIndex);
	returnValue += sizeof(charge);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double trackfinderInputTrack::getReservedSizeOfAddOnData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(track);
	returnValue += hits.getReservedSizeOfData(0);
	returnValue += sizeof(info);
	returnValue += sizeof(isPointer);
	returnValue += sizeof(trackIndex);
	returnValue += sizeof(charge);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double trackfinderInputTrack::getAllocatedSizeOfAddOnData(unsigned short dimension) {

	double returnValue;

	returnValue  = hits.getAllocatedSizeOfData(0);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double trackfinderInputTrack::getUsedSizeOfAddOnData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(track);
	returnValue += sizeof(hits);
	returnValue += hits.getUsedSizeOfData(0);
	returnValue += sizeof(info);
	returnValue += sizeof(isPointer);
	returnValue += sizeof(trackIndex);
	returnValue += sizeof(charge);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * Class implementation for the dictionary						*
 ****************************************************************/

ClassImp(trackfinderInputTrack)
