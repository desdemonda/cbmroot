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
// $Revision: 1.4 $
//
// *******************************************************************/


#include "../include/trackDigitalInformation.h"


/****************************************************************
 * CLASS trackDigitalInformation				 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

#ifndef NOANALYSIS
trackDigitalInformation::trackDigitalInformation() 
  : position(trackCoordinates(0, 0, 0)), value(bitArray(0)), hits(hitArray()) 
{  
}
#else
trackDigitalInformation::trackDigitalInformation() 
  : position(trackCoordinates(0, 0, 0)), value(bitArray(0))
{  
}
#endif

/****************************************************************
 * Constructor													*
 ****************************************************************/

#ifndef NOANALYSIS
trackDigitalInformation::trackDigitalInformation(const trackDigitalInformation& _value) 
  : position(_value.position), value(_value.value), hits(_value.hits) 
{
}
#else
trackDigitalInformation::trackDigitalInformation(const trackDigitalInformation& _value) 
  : position(_value.position), value(_value.value) 
{
}
#endif

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackDigitalInformation::~trackDigitalInformation() {

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const trackDigitalInformation& trackDigitalInformation::operator = (const trackDigitalInformation& _value) {

	this->position = _value.position;
	this->value    = _value.value;

#ifndef NOANALYSIS

	this->hits     = _value.hits;

#endif

	return *this;

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double trackDigitalInformation::getReservedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = position.getReservedSizeOfData(0);
	returnValue += value.getReservedSizeOfData(0);

#ifndef NOANALYSIS

	returnValue += hits.getReservedSizeOfData(0);

#endif

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double trackDigitalInformation::getAllocatedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = position.getAllocatedSizeOfData(0);
	returnValue += value.getAllocatedSizeOfData(0);

#ifndef NOANALYSIS

	returnValue += hits.getAllocatedSizeOfData(0);

#endif

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double trackDigitalInformation::getUsedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = position.getUsedSizeOfData(0);
	returnValue += value.getUsedSizeOfData(0);

#ifndef NOANALYSIS

	returnValue += hits.getUsedSizeOfData(0);

#endif

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}
