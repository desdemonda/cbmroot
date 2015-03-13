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
//     - base class for warnings occuring during dataObject-access
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:32:43 $
// $Revision: 1.7 $
//
// *******************************************************************/


#include "../../MiscLIB/include/projects.h"
#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/dataObjectWarningMsg.h"
#include <iostream>


/****************************************************************
 * CLASS dataObjectWarningMsg					 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

dataObjectWarningMsg::dataObjectWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

dataObjectWarningMsg::~dataObjectWarningMsg() {

}

/****************************************************************
 * This method prints a message.				 				*
 ****************************************************************/

void dataObjectWarningMsg::printMsg(std::string message) {

#ifndef NODATAOBJECTWARNINGMESSAGE 

	std::cerr << std::endl;
	
	std::cerr << DATAOBJECTLIB << " WARNING: ";
	
	if (!message.empty())
		std::cerr << message;
	
	std::cerr << std::endl;

#endif

}


/****************************************************************
 * CLASS cannotDoQuineMcCluskyWithDifferentLengthWarningMsg		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotDoQuineMcCluskyWithDifferentLengthWarningMsg::cannotDoQuineMcCluskyWithDifferentLengthWarningMsg() : dataObjectWarningMsg(), length1(0), length2(0) {

  //	length1 = 0;
  //	length2 = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

cannotDoQuineMcCluskyWithDifferentLengthWarningMsg::cannotDoQuineMcCluskyWithDifferentLengthWarningMsg(unsigned int _length1, unsigned int _length2) : dataObjectWarningMsg(), length1(_length1), length2(_length2)  {

  //	this->length1 = _length1;
  //	this->length2 = _length2;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotDoQuineMcCluskyWithDifferentLengthWarningMsg::~cannotDoQuineMcCluskyWithDifferentLengthWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void cannotDoQuineMcCluskyWithDifferentLengthWarningMsg::warningMsg() {

	std::string  temp;
	char         intBuffer[intConversionDigits+1];

	temp = "You cannot do the quine-mc-clusky with different length!!!\nThe appearing lengths are ";
	uitos(length1, intBuffer, 10, intConversionDigits);
	temp += intBuffer;
	temp += " and ";
	uitos(length2, intBuffer, 10, intConversionDigits);
	temp += intBuffer;
	temp += ".";

	printMsg(temp);

}


/****************************************************************
 * CLASS cannotDoQuineMcCluskyWithWrongFormatWarningMsg			*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotDoQuineMcCluskyWithWrongFormatWarningMsg::cannotDoQuineMcCluskyWithWrongFormatWarningMsg() : dataObjectWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotDoQuineMcCluskyWithWrongFormatWarningMsg::~cannotDoQuineMcCluskyWithWrongFormatWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void cannotDoQuineMcCluskyWithWrongFormatWarningMsg::warningMsg() {

	printMsg("You cannot do the quine-mc-clusky with this format!!!\nThe format must be '2x' followed by the bit-combinations.");

}


/****************************************************************
 * CLASS unknownValueInClassificationFoundWarningMsg			*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

unknownValueInClassificationFoundWarningMsg::unknownValueInClassificationFoundWarningMsg() : dataObjectWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

unknownValueInClassificationFoundWarningMsg::~unknownValueInClassificationFoundWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void unknownValueInClassificationFoundWarningMsg::warningMsg() {

	printMsg("There is an unknown value in a classification!!! This value is replaced by zero.");

}


/****************************************************************
 * CLASS minClassIsBiggerThanMaxClassWarningMsg					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

minClassIsBiggerThanMaxClassWarningMsg::minClassIsBiggerThanMaxClassWarningMsg() : dataObjectWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

minClassIsBiggerThanMaxClassWarningMsg::~minClassIsBiggerThanMaxClassWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void minClassIsBiggerThanMaxClassWarningMsg::warningMsg() {

	printMsg("The minimum classification of the table is set bigger than the maximum classification.");

}


/****************************************************************
 * CLASS rangeLutHoughBorderWarningMsg							*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

rangeLutHoughBorderWarningMsg::rangeLutHoughBorderWarningMsg() : dataObjectWarningMsg(), maxFirstDim(0), maxSecondDim(0), actualFirstDim(0), actualSecondDim(0)  {

  //	maxFirstDim     = 0;
  //	maxSecondDim    = 0;
  //	actualFirstDim  = 0;
  //	actualSecondDim = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

rangeLutHoughBorderWarningMsg::rangeLutHoughBorderWarningMsg(unsigned short _maxFirstDim, unsigned short _maxSecondDim, unsigned short _actualFirstDim, unsigned short _actualSecondDim) : dataObjectWarningMsg(), maxFirstDim(_maxFirstDim), maxSecondDim(_maxSecondDim), actualFirstDim(_actualFirstDim), actualSecondDim(_actualSecondDim)  {

  //	this->maxFirstDim     = _maxFirstDim;
  //	this->maxSecondDim    = _maxSecondDim;
  //	this->actualFirstDim  = _actualFirstDim;
  //	this->actualSecondDim = _actualSecondDim;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

rangeLutHoughBorderWarningMsg::~rangeLutHoughBorderWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void rangeLutHoughBorderWarningMsg::warningMsg() {

	std::string  temp;
	char         buffer[shortConversionDigits+1];

	temp = "The maximum range of one histogram layer is (";
	ustos(maxFirstDim, buffer, 10, shortConversionDigits);
	temp += buffer;
	temp += ", ";
	ustos(maxSecondDim, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += "). So the cell (";
	ustos(actualFirstDim, buffer, 10, shortConversionDigits);
	temp += buffer;
	temp += ", ";
	ustos(actualSecondDim, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += ") cannot be set!!!";

	printMsg(temp);

}


/****************************************************************
 * CLASS overflowByAddingClassificationsFoundWarningMsg		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

overflowByAddingClassificationsFoundWarningMsg::overflowByAddingClassificationsFoundWarningMsg() : dataObjectWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

overflowByAddingClassificationsFoundWarningMsg::~overflowByAddingClassificationsFoundWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void overflowByAddingClassificationsFoundWarningMsg::warningMsg() {

	printMsg("There is an overflow detected by adding classifications in a table!!!");

}


/****************************************************************
 * CLASS tooBigValueForAToLongConversionWarningMsg				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

tooBigValueForAToLongConversionWarningMsg::tooBigValueForAToLongConversionWarningMsg() : dataObjectWarningMsg(), value() {

  //	value.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

tooBigValueForAToLongConversionWarningMsg::tooBigValueForAToLongConversionWarningMsg(std::string _value) : dataObjectWarningMsg(), value(_value) {

  //	this->value = _value;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

tooBigValueForAToLongConversionWarningMsg::~tooBigValueForAToLongConversionWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void tooBigValueForAToLongConversionWarningMsg::warningMsg() {

	std::string temp;
	char        buffer[longConversionDigits + 1];

	temp =  "The occuring string: '";
	temp += value;
	temp += "' of length: ";
	ultos((unsigned long)temp.length(), buffer, 10, longConversionDigits);
	temp += buffer;
	temp += " is too long to be converted into an unsigned long value (max length: ";
	ultos(longConversionDigits, buffer, 10, longConversionDigits);
	temp += buffer;
	temp += ")!!!";

	printMsg(temp);

}
