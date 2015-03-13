//////////////////////////////////////////////////////////////////////
/// (C)opyright 2005
/// 
/// Institute of Computer Science V
/// Prof. Männer
/// University of Mannheim, Germany
/// 
/// *******************************************************************
/// 
/// Designer(s):   Steinle
/// 
/// *******************************************************************
/// 
/// Project:     Trackfinder for CBM-Project at GSI-Darmstadt, Germany
/// 
/// *******************************************************************
/// 
/// Description:
///
///   class:
///     - base class for holding the track coordinates and the MCTrack
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2006/07/17 11:27:40 $
/// $Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////


#include "../include/findableTrack.h"


/****************************************************************
 * CLASS findableTrack							 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

findableTrack::findableTrack() : mcTrack(NULL), position() {

  //	mcTrack = NULL;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

findableTrack::findableTrack(const findableTrack& value) : mcTrack(value.mcTrack), position(value.position) {

  //	this->mcTrack  = value.mcTrack;
  //	this->position = value.position;

}
findableTrack::findableTrack(trackfinderInputTrack* _mcTrack, trackCoordinates& _position) : mcTrack(_mcTrack), position(_position) {

	this->mcTrack  = _mcTrack;
	this->position = _position;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

findableTrack::~findableTrack() {

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const findableTrack& findableTrack::operator = (const findableTrack& value) {

	this->mcTrack  = value.mcTrack;
	this->position = value.position;

	return *this;

}

/****************************************************************
 * returns a pointer to the original track information from		*
 * Monte Carlo													*
 ****************************************************************/

trackfinderInputTrack* findableTrack::getTrack() {

	return mcTrack;

}

/****************************************************************
 * returns the coordinates of the histogram for the peak for	*
 * this track													*
 ****************************************************************/

trackCoordinates& findableTrack::getPosition() {

	return position;

}

/****************************************************************
 * sets the pointer to the original track information from		*
 * Monte Carlo and sets the position in the Hough space			*
 ****************************************************************/

void findableTrack::setParameter(trackfinderInputTrack* _mcTrack, trackCoordinates& _position) {

	this->mcTrack  = _mcTrack;
	this->position = _position;

}
