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
//     - base class for holding the digital hit information
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-11-21 13:59:22 $
// $Revision: 1.2 $
//
// *******************************************************************/


#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/digitalHit.h"


#define RADIX 10


/****************************************************************
 * CLASS digitalHit								 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

digitalHit::digitalHit() : hitIndex(0), stationIndex(0) {

  //	hitIndex     = 0;
  //	stationIndex = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

digitalHit::digitalHit(const digitalHit& value) : hitIndex(value.hitIndex), stationIndex(value.stationIndex)   {

  //	this->hitIndex     = value.hitIndex;
  //	this->stationIndex = value.stationIndex;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

digitalHit::~digitalHit() {

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const digitalHit& digitalHit::operator = (const digitalHit& value) {

	this->hitIndex     = value.hitIndex;
	this->stationIndex = value.stationIndex;

	return *this;

}

/****************************************************************
 * This method converts the object into a string representation.*
 ****************************************************************/

digitalHit::operator std::string(){

	return toNotIdentifiedString();

}

/****************************************************************
 * method returns a pointer to the object						*
 ****************************************************************/

digitalHit* digitalHit::getPointer() {

	return this;

}

/****************************************************************
 * method compare two objects of this class						*
 * @return value > 0, if element is smaller						*
 * @return value = 0, if element is equal						*
 * @return value < 0, if element is bigger						*
 ****************************************************************/

int digitalHit::compare(digitalHit element) {

	int returnValue;

	if (element.getHitIndex() < this->getHitIndex())
		returnValue = 1;
	else if (element.getHitIndex() == this->getHitIndex())
		returnValue = 0;
	else
		returnValue = -1;

	return returnValue;

}

/****************************************************************
 * method returns the maximuum number of digital hit			*
 * indizes which can occur										*
 ****************************************************************/

#if (ARCHITECTURE != PS3)

unsigned long digitalHit::getMaxNumberOfHitIndizes() {

	unsigned long returnValue;

#if (ARCHITECTURE == CBMROOT)

/**/

#else

	returnValue = 0;

#endif

	return returnValue;

}

#endif

/****************************************************************
 * method returns the digital hit index							*
 ****************************************************************/

unsigned long digitalHit::getHitIndex() {

	return hitIndex;

}
std::string digitalHit::getHitIndexString() {

	std::string returnValue;
	int         counts;
	int         maxCounts;
	char        buffer[longConversionDigits+1];

	returnValue  = "000";
	counts       = addRadix(RADIX, returnValue);
	maxCounts    = (int)returnValue.length();

	if (counts < maxCounts)
		returnValue.erase(counts, maxCounts - counts);

	ultos(hitIndex, buffer, RADIX, longConversionDigits);
	returnValue += buffer;

	return returnValue;

}

/****************************************************************
 * method returns the digital station index						*
 ****************************************************************/

unsigned short digitalHit::getStationIndex() {

	return stationIndex;

}
std::string digitalHit::getStationIndexString() {

	std::string returnValue;
	int         counts;
	int         maxCounts;
	char        buffer[shortConversionDigits+1];

	returnValue  = "000";
	counts       = addRadix(RADIX, returnValue);
	maxCounts    = (int)returnValue.length();

	if (counts < maxCounts)
		returnValue.erase(counts, maxCounts - counts);

	ustos(stationIndex, buffer, RADIX, shortConversionDigits);
	returnValue += buffer;

	return returnValue;

}

/****************************************************************
 * Method returns a hit object based on the digital hit index.	*
 * CAUTION: The return object contains just coordinates.		*
 * Other parts of such an object are not set.					*
 ****************************************************************/

#if (ARCHITECTURE != PS3)

trackfinderInputHit digitalHit::getHit() {

	trackfinderInputHit returnValue;

#if (ARCHITECTURE == CBMROOT)

/**/

#else

	returnValue.initDefault();

#endif

	return returnValue;

}

#endif

/****************************************************************
 * method sets the digital hit index							*
 ****************************************************************/

void digitalHit::setHitIndex(unsigned long value) {

	hitIndex = value;

}
void digitalHit::setHitIndexString(std::string& value) {

	std::string temp;
	int         radix;

	temp     = value;
	extractRadix(&radix, &temp);
	hitIndex = stoul(temp, radix);

}

/****************************************************************
 * method sets the digital station index						*
 ****************************************************************/

void digitalHit::setStationIndex(unsigned short value) {

	stationIndex = value;

}
void digitalHit::setStationIndexString(std::string& value) {

	std::string temp;
	int         radix;

	temp         = value;
	extractRadix(&radix, &temp);
	stationIndex = stous(temp, radix);

}

/****************************************************************
 * Method sets the digital hit index based on a hit object.		*
 ****************************************************************/

#if (ARCHITECTURE != PS3)

void digitalHit::setHit(trackfinderInputHit* hit) {

	if (hit == NULL)
		throw cannotAccessHitsOrTracksError(DATAROOTOBJECTLIB);

#if (ARCHITECTURE == CBMROOT)

/**/

#else

	hitIndex     = (unsigned long)hit->getHitOrder();

	if (hit->getStation() != NULL)
		if (!hit->getStation()->isMasked())
			stationIndex = hit->getStation()->getIndex();
		else
			stationIndex = (unsigned short)-1;
	else
		stationIndex = (unsigned short)-1;

#endif

}

#endif

/****************************************************************
 * This method converts the object into a string representation	*
 * without adding an identifier.								*
 ****************************************************************/

std::string digitalHit::toNotIdentifiedString() {

	std::string returnValue;

	returnValue  = "(";
	returnValue += getHitIndexString();
	returnValue += ",";
	returnValue += getStationIndexString();
	returnValue += ")";

	return returnValue;

}

/****************************************************************
 * This method converts the object into a string representation	*
 * and adds an identifier.										*
 ****************************************************************/

std::string digitalHit::toIdentifiedString() {

	std::string returnValue;

	returnValue  = "HitIndex: ";
	returnValue += getHitIndexString();
	returnValue += ", StationIndex: ";
	returnValue += getStationIndexString();

	return returnValue;

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double digitalHit::getReservedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(hitIndex);
	returnValue += sizeof(stationIndex);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double digitalHit::getAllocatedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double digitalHit::getUsedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(hitIndex);
	returnValue += sizeof(stationIndex);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}
