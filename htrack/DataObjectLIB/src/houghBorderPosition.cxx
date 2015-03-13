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
//     - base class for holding the houghTransformation border
//       position for all lut look-up-table positions
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-11-21 13:58:52 $
// $Revision: 1.8 $
//
// *******************************************************************/


#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/dataObjectError.h"
#include "../include/houghBorderPosition.h"


#define CLASSMEMBERSTRINGSEPARATOR  ","
#define IDENTIFIEDFRONTSTRING1      "Pos1: "
#define IDENTIFIEDFRONTSTRING2      " Pos2: "
#define NOTIDENTIFIEDFRONTSTRING    "("
#define NOTIDENTIFIEDBACKSTRING     ")"
#define STRINGRADIX                 10


/****************************************************************
 * CLASS houghBorderPosition					 				*
 ****************************************************************/

/****************************************************************
 * This method divides the set-string by searching for the		*
 * separater													*
 ****************************************************************/

void houghBorderPosition::partString(std::string& basicString, std::string& part1, std::string& part2) {

	std::basic_string<char>::size_type middleSeparator;

	middleSeparator = basicString.find(CLASSMEMBERSTRINGSEPARATOR, 0);

	if (middleSeparator == std::basic_string<char>::npos)
		throw cannotFindMiddleSeparatorError(basicString, CLASSMEMBERSTRINGSEPARATOR);

	part1 = basicString.substr(0, middleSeparator);
	part2 = basicString.substr(middleSeparator + 1, std::basic_string<char>::npos);

}

/****************************************************************
 * This method returns the basicString without the part-string	*
 ****************************************************************/

std::string houghBorderPosition::removeFrontString(std::string& basicString, std::string& part) {

	std::string                        returnValue;
	std::basic_string<char>::size_type position;

	returnValue = basicString;
	position    = basicString.find_first_of(part, 0);

	if (position != std::basic_string<char>::npos)
		returnValue.erase(position, part.length());

	return returnValue;

}

/****************************************************************
 * This method returns the basicString without the part-string	*
 ****************************************************************/

std::string houghBorderPosition::removeBackString(std::string& basicString, std::string& part) {

	std::string                        returnValue;
	std::basic_string<char>::size_type position;

	returnValue = basicString;
	position    = basicString.find_last_of(part, 0);

	if (position != std::basic_string<char>::npos)
		returnValue.erase(position, part.length());

	return returnValue;

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

houghBorderPosition::houghBorderPosition() : pos1(0), pos2(0) {

  //	pos1 = 0;
  //	pos2 = 0;
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

houghBorderPosition::houghBorderPosition(std::string& value, bool identifiedString) : pos1(0), pos2(0) {

	if (identifiedString)
		fromIdentifiedString(value);
	else
		fromNotIdentifiedString(value);

}

/****************************************************************
 * Copy constructor												*
 ****************************************************************/

houghBorderPosition::houghBorderPosition(const houghBorderPosition& value) : pos1(value.pos1), pos2(value.pos2) {

  //	this->pos1 = value.pos1;
  //	this->pos2 = value.pos2;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

houghBorderPosition::~houghBorderPosition() {

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const houghBorderPosition& houghBorderPosition::operator = (const houghBorderPosition& value) {

	this->pos1 = value.pos1;
	this->pos2 = value.pos2;

	return *this;

}

/****************************************************************
 * This method sets the object based on the string				*
 * representation without identifiers.							*
 ****************************************************************/

void houghBorderPosition::fromNotIdentifiedString(std::string& value) {

	std::string removeFrontPart;
	std::string removeBackPart;
	std::string part1;
	std::string part2;
	std::string number1;
	std::string number2;

	removeFrontPart = NOTIDENTIFIEDFRONTSTRING;
	removeBackPart  = NOTIDENTIFIEDBACKSTRING;

	partString(value, part1, part2);

	number1 = removeFrontString(part1, removeFrontPart);
	number2 = removeBackString(part2, removeBackPart);

	pos1 = stous(number1, STRINGRADIX);
	pos2 = stous(number2, STRINGRADIX);

}

/****************************************************************
 * This method sets the object based on the string				*
 * representation without identifiers.							*
 ****************************************************************/

void houghBorderPosition::fromNotIdentifiedRadixString(std::string& value) {

	std::string removeFrontPart;
	std::string removeBackPart;
	std::string part1;
	std::string part2;
	std::string number1;
	std::string number2;
	int         radix;

	removeFrontPart = NOTIDENTIFIEDFRONTSTRING;
	removeBackPart  = NOTIDENTIFIEDBACKSTRING;

	partString(value, part1, part2);

	number1 = removeFrontString(part1, removeFrontPart);
	number2 = removeBackString(part2, removeBackPart);

	extractRadix(&radix, &number1);
	pos1  = stous(number1, radix);
	extractRadix(&radix, &number2);
	pos2  = stous(number2, radix);

}

/****************************************************************
 * This method sets the object based on the string				*
 * representation with identifiers.								*
 ****************************************************************/

void houghBorderPosition::fromIdentifiedString(std::string& value) {

	std::string removePart1;
	std::string removePart2;
	std::string part1;
	std::string part2;
	std::string number1;
	std::string number2;

	removePart1 = IDENTIFIEDFRONTSTRING1;
	removePart2 = IDENTIFIEDFRONTSTRING2;

	partString(value, part1, part2);

	number1 = removeFrontString(part1, removePart1);
	number2 = removeFrontString(part2, removePart2);

	pos1 = stous(number1, STRINGRADIX);
	pos2 = stous(number2, STRINGRADIX);

}

/****************************************************************
 * This method sets the object based on the string				*
 * representation with identifiers.								*
 ****************************************************************/

void houghBorderPosition::fromIdentifiedRadixString(std::string& value) {

	std::string removePart1;
	std::string removePart2;
	std::string part1;
	std::string part2;
	std::string number1;
	std::string number2;
	int         radix;

	removePart1 = IDENTIFIEDFRONTSTRING1;
	removePart2 = IDENTIFIEDFRONTSTRING2;

	partString(value, part1, part2);

	number1 = removeFrontString(part1, removePart1);
	number2 = removeFrontString(part2, removePart2);

	extractRadix(&radix, &number1);
	pos1  = stous(number1, radix);
	extractRadix(&radix, &number2);
	pos2  = stous(number2, radix);

}

/****************************************************************
 * This method converts the object into a string representation	*
 ****************************************************************/
	
std::string houghBorderPosition::toNotIdentifiedString() {

	std::string returnValue;
	char        buffer[shortConversionDigits+1];

	returnValue  = NOTIDENTIFIEDFRONTSTRING;
	ustos(pos1, buffer, STRINGRADIX, shortConversionDigits);
	returnValue += buffer;
	returnValue += CLASSMEMBERSTRINGSEPARATOR;
	ustos(pos2, buffer, STRINGRADIX, shortConversionDigits);
	returnValue += buffer;
	returnValue += NOTIDENTIFIEDBACKSTRING;

	return returnValue;

}

/****************************************************************
 * This method converts the object into a string representation	*
 ****************************************************************/
	
std::string houghBorderPosition::toNotIdentifiedRadixString() {

	std::string radix;
	int         counts;
	int         maxCounts;
	std::string returnValue;
	char        buffer[shortConversionDigits+1];

	radix        = "000";
	counts       = addRadix(STRINGRADIX, radix);
	maxCounts    = (int)radix.length();

	if (counts < maxCounts)
		radix.erase(counts, maxCounts - counts);

	returnValue  = NOTIDENTIFIEDFRONTSTRING;
	returnValue += radix;
	ustos(pos1, buffer, STRINGRADIX, shortConversionDigits);
	returnValue += buffer;
	returnValue += CLASSMEMBERSTRINGSEPARATOR;
	returnValue += radix;
	ustos(pos2, buffer, STRINGRADIX, shortConversionDigits);
	returnValue += buffer;
	returnValue += NOTIDENTIFIEDBACKSTRING;

	return returnValue;

}

/****************************************************************
 * This method converts the object into a string representation	*
 ****************************************************************/
	
std::string houghBorderPosition::toIdentifiedString() {

	std::string returnValue;
	char        buffer[shortConversionDigits+1];

	returnValue  = IDENTIFIEDFRONTSTRING1;
	ustos(pos1, buffer, STRINGRADIX, shortConversionDigits);
	returnValue += buffer;
	returnValue += CLASSMEMBERSTRINGSEPARATOR;
	returnValue += IDENTIFIEDFRONTSTRING2;
	ustos(pos2, buffer, STRINGRADIX, shortConversionDigits);
	returnValue += buffer;

	return returnValue;

}

/****************************************************************
 * This method converts the object into a string representation	*
 ****************************************************************/
	
std::string houghBorderPosition::toIdentifiedRadixString() {

	std::string radix;
	int         counts;
	int         maxCounts;
	std::string returnValue;
	char        buffer[shortConversionDigits+1];

	radix        = "000";
	counts       = addRadix(STRINGRADIX, radix);
	maxCounts    = (int)radix.length();

	if (counts < maxCounts)
		radix.erase(counts, maxCounts - counts);

	returnValue  = IDENTIFIEDFRONTSTRING1;
	returnValue += radix;
	ustos(pos1, buffer, STRINGRADIX, shortConversionDigits);
	returnValue += buffer;
	returnValue += CLASSMEMBERSTRINGSEPARATOR;
	returnValue += IDENTIFIEDFRONTSTRING2;
	returnValue += radix;
	ustos(pos2, buffer, STRINGRADIX, shortConversionDigits);
	returnValue += buffer;

	return returnValue;

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double houghBorderPosition::getReservedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(pos1);
	returnValue += sizeof(pos2);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double houghBorderPosition::getAllocatedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double houghBorderPosition::getUsedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(pos1);
	returnValue += sizeof(pos2);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}
