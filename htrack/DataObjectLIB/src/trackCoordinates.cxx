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
//     - base class for holding the track coordinates of the transformation
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-11-21 13:58:53 $
// $Revision: 1.5 $
//
// *******************************************************************/


#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/dataObjectError.h"
#include "../include/trackCoordinates.h"


#define RADIX 10


/****************************************************************
 * CLASS trackCoordinates						 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackCoordinates::trackCoordinates() {

	for (unsigned short i = 0; i < DIMENSIONS; i++)
		dim[i] = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackCoordinates::trackCoordinates(const trackCoordinates& value) {

	for (unsigned short i = 0; i < DIMENSIONS; i++)
		this->dim[i] = value.dim[i];

}
trackCoordinates::trackCoordinates(unsigned short dim1, unsigned short dim2, unsigned short dim3) {

	set(dim1, DIM1);
	set(dim2, DIM2);
	set(dim3, DIM3);

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackCoordinates::~trackCoordinates() {

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const trackCoordinates& trackCoordinates::operator = (const trackCoordinates& value) {

	for (unsigned short i = 0; i < DIMENSIONS; i++)
		this->dim[i] = value.dim[i];

	return *this;

}

/****************************************************************
 * This method converts the object into a string representation.*
 ****************************************************************/

trackCoordinates::operator std::string(){

	return toNotIdentifiedString();

}

/****************************************************************
 * This method converts the object into a string representation	*
 * and adds no identifiers.										*
 ****************************************************************/

std::string trackCoordinates::toNotIdentifiedString() {

	std::string radixString;
	int         counts;
	int         maxCounts;
	std::string returnValue;
	char        buffer[shortConversionDigits+1];

	radixString  = "000";
	counts       = addRadix(RADIX, radixString);
	maxCounts    = (int)radixString.length();

	if (counts < maxCounts)
		radixString.erase(counts, maxCounts - counts);

	returnValue  = "{";
	for (unsigned short i = 0; i < DIMENSIONS; i++) {

		if (i > 0)
			returnValue += ", ";
		returnValue += radixString;
		ustos(dim[i], buffer, RADIX, shortConversionDigits);
		returnValue += buffer;

	}
	returnValue += "}";

	return returnValue;

}

/****************************************************************
 * This method converts the object into a string representation	*
 * and adds identifiers.										*
 ****************************************************************/

std::string trackCoordinates::toIdentifiedString() {

	std::string radixString;
	int         counts;
	int         maxCounts;
	std::string returnValue;
	char        buffer[shortConversionDigits+1];

	radixString  = "000";
	counts       = addRadix(RADIX, radixString);
	maxCounts    = (int)radixString.length();

	if (counts < maxCounts)
		radixString.erase(counts, maxCounts - counts);

	returnValue  = "{";
	for (unsigned short i = 0; i < DIMENSIONS; i++) {

		if (i > 0)
			returnValue += ", ";
		returnValue += "dim";
		ustos(i, buffer, 10, shortConversionDigits);
		returnValue += buffer;
		returnValue += " = ";
		returnValue += radixString;
		ustos(dim[i], buffer, RADIX, shortConversionDigits);
		returnValue += buffer;

	}
	returnValue += "}";

	return returnValue;

}

/****************************************************************
 * method returns a pointer to the object						*
 ****************************************************************/

trackCoordinates* trackCoordinates::getPointer() {

	return this;

}

/****************************************************************
 * method compare two objects of this class						*
 * @return value > 0, if element is smaller						*
 * @return value = 0, if element is equal						*
 * @return value < 0, if element is bigger						*
 ****************************************************************/

int trackCoordinates::compare(trackCoordinates element) {

	int returnValue;

	if ((element.get(DIM2) < this->get(DIM2)) || ((element.get(DIM2) == this->get(DIM2)) && (element.get(DIM1) < this->get(DIM1))) || ((element.get(DIM2) == this->get(DIM2)) && (element.get(DIM1) == this->get(DIM1)) && (element.get(DIM3) < this->get(DIM3))))
		returnValue = 1;
	else if ((element.get(DIM1) == this->get(DIM1)) && (element.get(DIM2) == this->get(DIM2)) && (element.get(DIM3) == this->get(DIM3)))
		returnValue = 0;
	else
		returnValue = -1;

	return returnValue;

}

/****************************************************************
 * returns the value in the dimension							*
 ****************************************************************/

unsigned short trackCoordinates::get(unsigned short dimension) {

	unsigned short returnValue = 0;

	if (dimension < DIMENSIONS)
		returnValue = dim[dimension];
	else
		throw notExistingDimensionError(dimension, DIMENSIONS);

	return returnValue;

}

/****************************************************************
 * sets the value in the dimension								*
 ****************************************************************/

void trackCoordinates::set(unsigned short _set, unsigned short dimension) {

	if (dimension < DIMENSIONS)
		dim[dimension] = _set;
	else
		throw notExistingDimensionError(dimension, DIMENSIONS);

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double trackCoordinates::getReservedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(dim);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double trackCoordinates::getAllocatedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double trackCoordinates::getUsedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(dim);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}
