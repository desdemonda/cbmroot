/* *******************************************************************
// (C)opyright 2005
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
//     - base class for holding the track information of the transformation
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:33:08 $
// $Revision: 1.3 $
//
// *******************************************************************/


#include "../include/trackAnalogInformation.h"


/****************************************************************
 * CLASS trackAnalogInformation					 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

#ifndef NOANALYSIS
trackAnalogInformation::trackAnalogInformation() 
: position(trackParameter(0, 0, 0)), hits() 
{
  //	position = trackParameter(0, 0, 0);
  //	hits     = hitArray();
}
#else
trackAnalogInformation::trackAnalogInformation() : position(trackParameter(0, 0, 0)) {

  //	position = trackParameter(0, 0, 0);


#endif

/****************************************************************
 * Constructor													*
 ****************************************************************/

#ifndef NOANALYSIS
trackAnalogInformation::trackAnalogInformation(const trackAnalogInformation& value) 
  : position(value.position), hits(value.hits)
{
  //	this->position = value.position;
  //	this->hits     = value.hits;
}
#else
trackAnalogInformation::trackAnalogInformation(const trackAnalogInformation& value) 
  : position(value.position),
{
  //	this->position = value.position;
#endif

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackAnalogInformation::~trackAnalogInformation() {

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const trackAnalogInformation& trackAnalogInformation::operator = (const trackAnalogInformation& value) {

	this->position = value.position;

#ifndef NOANALYSIS

	this->hits     = value.hits;

#endif

	return *this;

}
