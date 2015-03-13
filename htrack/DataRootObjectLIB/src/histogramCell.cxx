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
//     - base class for handling the created borders of one layer
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:33:08 $
// $Revision: 1.4 $
//
// *******************************************************************/


#include "../include/histogramCell.h"


/****************************************************************
 * CLASS histogramCell							 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

#ifndef NOANALYSIS
histogramCell::histogramCell() : value(0), hits() {

  //	value = 0;


	hits.reset();


}
#else
histogramCell::histogramCell() : value(0) {

  //	value = 0;
}

#endif

/****************************************************************
 * Constructor													*
 ****************************************************************/

#ifndef NOANALYSIS
histogramCell::histogramCell(const histogramCell& _value) : value(_value.value), hits(_value.hits) {

  //	this->value = _value.value;


  //	this->hits  = _value.hits;


}
#else
histogramCell::histogramCell(const histogramCell& _value) : value(_value.value) {

  //	this->value = _value.value;

}
#endif

/****************************************************************
 * Destructor													*
 ****************************************************************/

histogramCell::~histogramCell() {

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const histogramCell& histogramCell::operator = (const histogramCell& _value) {

	this->value = _value.value;

#ifndef NOANALYSIS

	this->hits  = _value.hits;

#endif

	return *this;

}

/****************************************************************
 * Method resets the object.									*
 ****************************************************************/

void histogramCell::reset() {

	value.reset();

#ifndef NOANALYSIS

	hits.reset();

#endif

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double histogramCell::getReservedSizeOfHistogramData(unsigned short dimension) {

	double returnValue;

	returnValue  = getReservedSizeOfHistogramSignatureData(0);
	returnValue += getReservedSizeOfHistogramHitData(0);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double histogramCell::getAllocatedSizeOfHistogramData(unsigned short dimension) {

	double returnValue;

	returnValue  = getAllocatedSizeOfHistogramSignatureData(0);
	returnValue += getAllocatedSizeOfHistogramHitData(0);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double histogramCell::getUsedSizeOfHistogramData(unsigned short dimension) {

	double returnValue;

	returnValue  = getUsedSizeOfHistogramSignatureData(0);
	returnValue += getUsedSizeOfHistogramHitData(0);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double histogramCell::getReservedSizeOfHistogramSignatureData(unsigned short dimension) {

	double returnValue;

	returnValue  = value.getReservedSizeOfData(0);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double histogramCell::getAllocatedSizeOfHistogramSignatureData(unsigned short dimension) {

	double returnValue;

	returnValue  = value.getAllocatedSizeOfData(0);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double histogramCell::getUsedSizeOfHistogramSignatureData(unsigned short dimension) {

	double returnValue;

	returnValue  = value.getUsedSizeOfData(0);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double histogramCell::getReservedSizeOfHistogramHitData(unsigned short dimension) {

#ifndef NOANALYSIS

	return hits.getReservedSizeOfData(dimension);

#else

	return 0;

#endif

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double histogramCell::getAllocatedSizeOfHistogramHitData(unsigned short dimension) {

#ifndef NOANALYSIS

	return hits.getAllocatedSizeOfData(dimension);

#else

	return 0;

#endif

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double histogramCell::getUsedSizeOfHistogramHitData(unsigned short dimension) {

#ifndef NOANALYSIS

	return hits.getUsedSizeOfData(dimension);

#else

	return 0;

#endif

}
