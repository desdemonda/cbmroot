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
//     - derived from errorHandling
//     - class for errors occuring while interacting with the luts
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:32:43 $
// $Revision: 1.5 $
//
// *******************************************************************/


#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/dataObjectError.h"


/****************************************************************
 * CLASS dataObjectError						 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

dataObjectError::dataObjectError() : errorHandling(DATAOBJECTLIB) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

dataObjectError::~dataObjectError() {

}


/****************************************************************
 * CLASS borderIdRuleError						 				*
 ****************************************************************/

borderIdRuleError::borderIdRuleError() : dataObjectError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

borderIdRuleError::~borderIdRuleError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void borderIdRuleError::errorMsg() {

	printMsg("There is an error in the lut which conflicts with the defined rules!!!");

}


/****************************************************************
 * CLASS borderIdConversionError				 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

borderIdConversionError::borderIdConversionError() : dataObjectError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

borderIdConversionError::~borderIdConversionError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void borderIdConversionError::errorMsg() {

	printMsg("There is a conversion error in the lut which is based on the border!!!");

}


/****************************************************************
 * CLASS tooBigStartIdError						 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

tooBigStartIdError::tooBigStartIdError() : dataObjectError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

tooBigStartIdError::~tooBigStartIdError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void tooBigStartIdError::errorMsg() {

	printMsg("The given startId is bigger than the maximum startId!!!");

}


/****************************************************************
 * CLASS cannotFindMiddleSeparatorError			 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotFindMiddleSeparatorError::cannotFindMiddleSeparatorError() : dataObjectError(), searchedString(), middleSeparator() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

cannotFindMiddleSeparatorError::cannotFindMiddleSeparatorError(std::string _searchedString, std::string _middleSeparator) : dataObjectError(), searchedString(_searchedString), middleSeparator(_middleSeparator)  {

  //	this->searchedString  = _searchedString;
  //	this->middleSeparator = _middleSeparator;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotFindMiddleSeparatorError::~cannotFindMiddleSeparatorError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotFindMiddleSeparatorError::errorMsg() {

	std::string temp;

	temp  = "The string separator '";
	temp += middleSeparator;
	temp += "' to divide the string '";
	temp += searchedString;
	temp += "' into a part for each member cannot be found!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS notExistingDimensionError				 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

notExistingDimensionError::notExistingDimensionError() : dataObjectError(), actualDimension(0), numberOfDimensions(0) {

  //	actualDimension    = 0;
  //	numberOfDimensions = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

notExistingDimensionError::notExistingDimensionError(unsigned short _actualDimension, unsigned short _numberOfDimensions) : dataObjectError(), actualDimension(_actualDimension), numberOfDimensions(_numberOfDimensions)   {

  //	this->actualDimension    = _actualDimension;
  //	this->numberOfDimensions = _numberOfDimensions;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

notExistingDimensionError::~notExistingDimensionError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void notExistingDimensionError::errorMsg() {

	std::string temp;
	char        buffer[shortConversionDigits+1];

	temp =  "The dimension ";
	ustos(actualDimension, buffer, 10, shortConversionDigits);
	temp += buffer;
	temp += " is out of range [0, ";
	ustos(numberOfDimensions - 1, buffer, shortConversionDigits);
	temp += buffer;
	temp += "]!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS analogValueIsNotInRangeError			 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

analogValueIsNotInRangeError::analogValueIsNotInRangeError() : dataObjectError(), actualValue(0), minValue(0), maxValue(0) {

  //	actualValue = 0;
  //	minValue    = 0;
  //	maxValue    = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

analogValueIsNotInRangeError::analogValueIsNotInRangeError(double _actualValue, double _minValue, double _maxValue) : dataObjectError(), actualValue(_actualValue), minValue(_minValue), maxValue(_maxValue)  {

  //	this->actualValue = _actualValue;
  //	this->minValue    = _minValue;
  //	this->maxValue    = _maxValue;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

analogValueIsNotInRangeError::~analogValueIsNotInRangeError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void analogValueIsNotInRangeError::errorMsg() {

	std::string temp;
	char        buffer[doubleConversion+1];

	temp =  "The analog value ";
	dtos(actualValue, buffer, doubleConversionDigits);
	temp += buffer;
	temp += " is out of range [";
	dtos(minValue, buffer, doubleConversionDigits);
	temp += buffer;
	temp += ", ";
	dtos(maxValue, buffer, doubleConversionDigits);
	temp += buffer;
	temp += "]!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS tooBigCellError						 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

tooBigCellError::tooBigCellError() : dataObjectError(), actualCell(0), numberOfCells(0) {

  //	actualCell    = 0;
  //	numberOfCells = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

tooBigCellError::tooBigCellError(unsigned short _actualCell, unsigned short _numberOfCells) : dataObjectError(), actualCell(_actualCell), numberOfCells(_numberOfCells)  {

  //	this->actualCell    = _actualCell;
  //	this->numberOfCells = _numberOfCells;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

tooBigCellError::~tooBigCellError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void tooBigCellError::errorMsg() {

	std::string temp;
	char        buffer[shortConversionDigits+1];

	temp =  "The cell ";
	ustos(actualCell, buffer, 10, shortConversionDigits);
	temp += buffer;
	temp += " is out of range [0, ";
	ustos(numberOfCells - 1, buffer, shortConversionDigits);
	temp += buffer;
	temp += "]!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS resultPointerIsNotAccessibleError		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

resultPointerIsNotAccessibleError::resultPointerIsNotAccessibleError() : dataObjectError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

resultPointerIsNotAccessibleError::~resultPointerIsNotAccessibleError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void resultPointerIsNotAccessibleError::errorMsg() {

	printMsg("The pointer to place the result is not accessible!!!");

}
