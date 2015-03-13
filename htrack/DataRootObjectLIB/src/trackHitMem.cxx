/* *******************************************************************
// (C)opyright 2004
// 
// Institute of Computer Science V
// Prof. Männer
// University of Mannheim, Germany
// 
// *******************************************************************
// 
// Designer(s):   Steinle
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
//     - consists of a summary for the peak which represents the quality of finding
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:33:08 $
// $Revision: 1.2 $
//
// *******************************************************************/


#include "../include/trackHitMem.h"


/****************************************************************
 * returns the trackId											*
 ****************************************************************/

int trackHitMem::getTrackIndex() {

	int returnValue = -1;

	if (hits.readActiveObject()->getTrack() != NULL)
		returnValue = hits.readActiveObject()->getTrack()->getTrackIndex();

	return returnValue;

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackHitMem::trackHitMem() 
  : hits(),
    trackPattern()
{

	hits.clear();
	trackPattern.reset();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackHitMem::trackHitMem(const trackHitMem& value) 
  : hits(value.hits),
    trackPattern(value.trackPattern)
{
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackHitMem::~trackHitMem() {

}

/**
 * operator = ()
 */

const trackHitMem& trackHitMem::operator = (const trackHitMem& value) {

	hits         = value.hits;
	trackPattern = value.trackPattern;

	return *this;

}

/****************************************************************
 * method removes all hits										*
 ****************************************************************/

void trackHitMem::removeAllHits() {

	hits.clear();
	trackPattern.reset();

}

/****************************************************************
 * method returns the number of hits							*
 ****************************************************************/

unsigned long trackHitMem::getNumberOfHits() {

	return hits.getNumberOfEntries();

}

/****************************************************************
 * method returns a hit and activates the next one				*
 ****************************************************************/

trackfinderInputHit* trackHitMem::getHitAndMakeNextActive() {

	return hits.readActiveObjectAndMakeNextOneActive();

}

/****************************************************************
 * method adds a hit											*
 ****************************************************************/

void trackHitMem::addHit(trackfinderInputHit* hit) {

	hits.push(hit);
	if (hit->getStation() != NULL)
		trackPattern[hit->getStation()->getIndex()] = true;

}

/****************************************************************
 * method resets the pointer to the hits						*
 ****************************************************************/

void trackHitMem::resetHitPointer() {

	hits.resetActiveObject();

}

/****************************************************************
 * This method compares to objects of this class. The result is	*
 * - zero, if the object is equal to the element				*
 * - else, if the object is not equal to the element			*
 ****************************************************************/

int trackHitMem::compare(trackHitMem& element) {

	int returnValue = -1;

	if ((this->getTrackIndex() != -1) && (element.getTrackIndex() != -1))
		returnValue = this->getTrackIndex() - element.getTrackIndex();

	return returnValue;

}

/****************************************************************
 * This method compares to objects of this class. The result is	*
 * - true, if the object is bigger than the element				*
 * - false, if the object is not bigger than the element		*
 ****************************************************************/

bool trackHitMem::isBigger(trackHitMem& element) {

	bool returnValue;

	if (this->getNumberOfHits() > element.getNumberOfHits())
		returnValue = true;
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * returns a pointer to this object								*
 ****************************************************************/

trackHitMem* trackHitMem::getPointer() {

	return this;

}

/****************************************************************
 * method returns the trackPattern								*
 ****************************************************************/

bitArray trackHitMem::getTrackPattern() {

	return trackPattern;

}

/****************************************************************
 * method returns the real track itself							*
 ****************************************************************/

trackfinderInputTrack* trackHitMem::getRealTrack() {

	trackfinderInputTrack* returnValue;

	if (hits.size() == 0)
		returnValue = NULL;
	else
		returnValue = hits.readActiveObject()->getTrack();

	return returnValue;

}
