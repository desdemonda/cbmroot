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
//     - base class for holding the houghTransformation border from
//       the prelut look-up-table
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-11-21 13:58:52 $
// $Revision: 1.8 $
//
// *******************************************************************/


#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/prelutHoughBorder.h"


#define RADIX 10


/****************************************************************
 * CLASS prelutHoughBorder						 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

prelutHoughBorder::prelutHoughBorder() : start(0), stop(0) {

	clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

prelutHoughBorder::prelutHoughBorder(const prelutHoughBorder& value) 
  : start(value.start), stop(value.stop)
{

  //	this->start = value.start;
  //	this->stop  = value.stop;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

prelutHoughBorder::~prelutHoughBorder() {

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const prelutHoughBorder& prelutHoughBorder::operator = (const prelutHoughBorder& value) {

	this->start = value.start;
	this->stop  = value.stop;

	return *this;

}

/****************************************************************
 * This method clears the object.								*
 ****************************************************************/

void prelutHoughBorder::clear() {

	start = 0;
	stop  = 0;

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void prelutHoughBorder::init() {

	clear();

}

/****************************************************************
 * This method converts the object into a string representation	*
 * and adds no identifiers.										*
 ****************************************************************/

std::string prelutHoughBorder::toNotIdentifiedString() {

	std::string returnValue;

	returnValue  = "(";
	returnValue += getStart();
	returnValue += ",";
	returnValue += getStop();
	returnValue += ")";

	return returnValue;

}

/****************************************************************
 * This method converts the object into a string representation	*
 * and adds identifiers.										*
 ****************************************************************/

std::string prelutHoughBorder::toIdentifiedString() {

	std::string returnValue;

	returnValue  = "Start: ";
	returnValue += getStart();
	returnValue += ", Stop: ";
	returnValue += getStop();

	return returnValue;

}

/****************************************************************
 * method returns the start value								*
 ****************************************************************/

std::string prelutHoughBorder::getStart() {

	std::string returnValue;
	int         counts;
	int         maxCounts;
	char        buffer[shortConversionDigits+1];

	returnValue  = "000";
	counts       = addRadix(RADIX, returnValue);
	maxCounts    = (int)returnValue.length();

	if (counts < maxCounts)
		returnValue.erase(counts, maxCounts - counts);

	ustos(start, buffer, RADIX, shortConversionDigits);
	returnValue += buffer;

	return returnValue;

}

/****************************************************************
 * method returns the stop value								*
 ****************************************************************/

std::string prelutHoughBorder::getStop() {

	std::string returnValue;
	int         counts;
	int         maxCounts;
	char        buffer[shortConversionDigits+1];

	returnValue  = "000";
	counts       = addRadix(RADIX, returnValue);
	maxCounts    = (int)returnValue.length();

	if (counts < maxCounts)
		returnValue.erase(counts, maxCounts - counts);

	ustos(stop, buffer, RADIX, shortConversionDigits);
	returnValue += buffer;

	return returnValue;

}

/****************************************************************
 * method sets the start value									*
 ****************************************************************/

void prelutHoughBorder::setStart(std::string& value) {

	std::string temp;
	int         radix;

	temp  = value;
	extractRadix(&radix, &temp);
	start = stous(temp, radix);

}

/****************************************************************
 * method sets the stop value									*
 ****************************************************************/

void prelutHoughBorder::setStop(std::string& value) {

	std::string temp;
	int         radix;

	temp  = value;
	extractRadix(&radix, &temp);
	stop  = stous(temp, radix);

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double prelutHoughBorder::getReservedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(start);
	returnValue += sizeof(stop);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double prelutHoughBorder::getAllocatedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double prelutHoughBorder::getUsedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(start);
	returnValue += sizeof(stop);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}
