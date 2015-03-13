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
// $Date: 2008-08-14 12:33:07 $
// $Revision: 1.3 $
//
// *******************************************************************/


#include "../include/borderCell.h"


/****************************************************************
 * CLASS borderCell								 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

borderCell::borderCell() : layer() {

  //	layer.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

borderCell::borderCell(const borderCell& value) : layer(value.layer) {

  //	this->layer = value.layer;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

borderCell::~borderCell() {

	layer.clear();

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const borderCell& borderCell::operator = (const borderCell& value) {

	this->layer = value.layer;

	return *this;

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double borderCell::getReservedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = (double)sizeof(layer);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double borderCell::getAllocatedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;

	for (unsigned int i = 0; i < (unsigned int)layer.size(); i++) {

		returnValue += layer[i].getReservedSizeOfData(0);
		returnValue += layer[i].getAllocatedSizeOfData(0);

	}

	returnValue += ((double)layer.capacity() - (double)layer.size()) * (double)sizeof(lutBorder);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double borderCell::getUsedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;

	for (unsigned int i = 0; i < (unsigned int)layer.size(); i++) {

		returnValue += layer[i].getUsedSizeOfData(0);

	}

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double borderCell::getReservedSizeOfPrelutData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double borderCell::getAllocatedSizeOfPrelutData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;

	for (unsigned int i = 0; i < (unsigned int)layer.size(); i++) {

		returnValue += layer[i].getReservedSizeOfPrelutData(0);
		returnValue += layer[i].getAllocatedSizeOfPrelutData(0);

	}

	returnValue += ((double)layer.capacity() - (double)layer.size()) * (double)sizeof(prelutHoughBorder);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double borderCell::getUsedSizeOfPrelutData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;

	for (unsigned int i = 0; i < (unsigned int)layer.size(); i++) {

		returnValue += layer[i].getUsedSizeOfPrelutData(0);

	}

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double borderCell::getReservedSizeOfLutData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double borderCell::getAllocatedSizeOfLutData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;

	for (unsigned int i = 0; i < (unsigned int)layer.size(); i++) {

		returnValue += layer[i].getReservedSizeOfLutData(0);
		returnValue += layer[i].getAllocatedSizeOfLutData(0);

	}

	returnValue += ((double)layer.capacity() - (double)layer.size()) * (double)sizeof(lutHoughBorder);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double borderCell::getUsedSizeOfLutData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;

	for (unsigned int i = 0; i < (unsigned int)layer.size(); i++) {

		returnValue += layer[i].getUsedSizeOfLutData(0);

	}

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double borderCell::getReservedSizeOfHitData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double borderCell::getAllocatedSizeOfHitData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;

	for (unsigned int i = 0; i < (unsigned int)layer.size(); i++) {

		returnValue += layer[i].getReservedSizeOfHitData(0);
		returnValue += layer[i].getAllocatedSizeOfHitData(0);

	}

	returnValue += ((double)layer.capacity() - (double)layer.size()) * (double)sizeof(trackfinderInputHit*);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double borderCell::getUsedSizeOfHitData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;

	for (unsigned int i = 0; i < (unsigned int)layer.size(); i++) {

		returnValue += layer[i].getUsedSizeOfHitData(0);

	}

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double borderCell::getAllocatedSizeOfAddOnData(unsigned short dimension) {

	double returnValue;

	returnValue  = (double)sizeof(layer);

	returnValue += ((double)layer.capacity() - (double)layer.size()) * ((double)sizeof(lutBorder) - (double)sizeof(prelutHoughBorder) - (double)sizeof(lutHoughBorder) - (double)sizeof(trackfinderInputHit*));

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}
