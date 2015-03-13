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
//     - base class for holding the information from the look-up-tables
//       for one hit
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:33:08 $
// $Revision: 1.5 $
//
// *******************************************************************/


#include "../include/lutBorder.h"


/****************************************************************
 * CLASS lutBorder								 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

lutBorder::lutBorder() : firstBorder(), secondBorder(), hit(NULL) {

  //	firstBorder.start = 0;
  //	firstBorder.stop  = 0;
  //	secondBorder.houghCoord.clear();
  //	hit               = NULL;

}

/****************************************************************
 * Copy constructor												*
 ****************************************************************/

lutBorder::lutBorder(const lutBorder& value) 
  : firstBorder(value.firstBorder),
    secondBorder(value.secondBorder),
    hit(value.hit)
{

  //	this->firstBorder  = value.firstBorder;
  //	this->secondBorder = value.secondBorder;
  //	this->hit          = value.hit;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

lutBorder::~lutBorder() {

	secondBorder.houghCoord.clear();

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const lutBorder& lutBorder::operator = (const lutBorder& value) {

	this->firstBorder  = value.firstBorder;
	this->secondBorder = value.secondBorder;
	this->hit          = value.hit;

	return *this;

}

/****************************************************************
 * method returns the prelut border								*
 ****************************************************************/

prelutHoughBorder& lutBorder::getPrelutHoughBorder() {

	return firstBorder;

}

/****************************************************************
 * method returns the lut border								*
 ****************************************************************/

lutHoughBorder& lutBorder::getLutHoughBorder() {

	return secondBorder;

}

/****************************************************************
 * method returns the hit corresponding to the borders			*
 ****************************************************************/

trackfinderInputHit* lutBorder::getHit() {

	return hit;

}

/****************************************************************
 * method sets the prelut border								*
 ****************************************************************/

prelutHoughBorder* lutBorder::getPrelutHoughBorderPointer() {

	return &firstBorder;

}

/****************************************************************
 * method sets the lut border									*
 ****************************************************************/

lutHoughBorder* lutBorder::getLutHoughBorderPointer() {

	return &secondBorder;

}

/****************************************************************
 * method sets the prelut border								*
 ****************************************************************/

void lutBorder::setPrelutHoughBorder(prelutHoughBorder& border) {

	firstBorder = border;

}

/****************************************************************
 * method sets the lut border									*
 ****************************************************************/

void lutBorder::setLutHoughBorder(lutHoughBorder& border) {

	secondBorder = border;

}

/****************************************************************
 * method sets the hit corresponding to the borders				*
 ****************************************************************/

void lutBorder::setHit(trackfinderInputHit* _hit) {

	this->hit = _hit;

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double lutBorder::getReservedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = getReservedSizeOfPrelutData(0);
	returnValue += getReservedSizeOfLutData(0);
	returnValue += getReservedSizeOfHitData(0);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double lutBorder::getAllocatedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = getAllocatedSizeOfPrelutData(0);
	returnValue += getAllocatedSizeOfLutData(0);
	returnValue += getAllocatedSizeOfHitData(0);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double lutBorder::getUsedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = getUsedSizeOfPrelutData(0);
	returnValue += getUsedSizeOfLutData(0);
	returnValue += getUsedSizeOfHitData(0);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double lutBorder::getReservedSizeOfPrelutData(unsigned short dimension) {

	double returnValue;

	returnValue  = firstBorder.getReservedSizeOfData(0);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double lutBorder::getAllocatedSizeOfPrelutData(unsigned short dimension) {

	double returnValue;

	returnValue  = firstBorder.getAllocatedSizeOfData(0);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double lutBorder::getUsedSizeOfPrelutData(unsigned short dimension) {

	double returnValue;

	returnValue  = firstBorder.getUsedSizeOfData(0);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double lutBorder::getReservedSizeOfLutData(unsigned short dimension) {

	double returnValue;

	returnValue  = secondBorder.getReservedSizeOfData(0);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double lutBorder::getAllocatedSizeOfLutData(unsigned short dimension) {

	double returnValue;

	returnValue  = secondBorder.getAllocatedSizeOfData(0);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double lutBorder::getUsedSizeOfLutData(unsigned short dimension) {

	double returnValue;

	returnValue  = secondBorder.getUsedSizeOfData(0);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double lutBorder::getReservedSizeOfHitData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(hit);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double lutBorder::getAllocatedSizeOfHitData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double lutBorder::getUsedSizeOfHitData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(hit);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}
