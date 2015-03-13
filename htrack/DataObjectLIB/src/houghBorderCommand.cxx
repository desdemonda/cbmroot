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
//     - base class for holding the houghTransformation entry command
//       for the histogram
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-11-21 13:58:52 $
// $Revision: 1.6 $
//
// *******************************************************************/


#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/dataObjectWarningMsg.h"
#include "../include/houghBorderCommand.h"


#define STARTPOSRADIX 10
#define CMDRADIX       2


/****************************************************************
 * CLASS houghBorderCommand						 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

houghBorderCommand::houghBorderCommand() : startPos(0), cmd() {

  	cmd.clear();
	//	startPos = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

houghBorderCommand::houghBorderCommand(const houghBorderCommand& value)  : startPos(value.startPos), cmd(value.cmd) {

  //	this->cmd      = value.cmd;
  //	this->startPos = value.startPos;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

houghBorderCommand::~houghBorderCommand() {

	cmd.clear();

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const houghBorderCommand& houghBorderCommand::operator = (const houghBorderCommand& value) {

	this->cmd      = value.cmd;
	this->startPos = value.startPos;

	return *this;

}

/**
 * method returns the startPos value
 */

std::string houghBorderCommand::getStartPos() {

	std::string returnValue;
	int         counts;
	int         maxCounts;
	char        buffer[shortConversionDigits+1];

	returnValue  = "000";
	counts       = addRadix(STARTPOSRADIX, returnValue);
	maxCounts    = (int)returnValue.length();

	if (counts < maxCounts)
		returnValue.erase(counts, maxCounts - counts);

	ustos(startPos, buffer, 10, shortConversionDigits);
	returnValue += buffer;

	return returnValue;

}

/**
 * method returns the startPos value
 */

std::string houghBorderCommand::getCmd() {

	std::string returnValue;
	int         counts;
	int         maxCounts;
	char        buffer[1+1];
	bool        actualCmd;

	returnValue  = "000";
	counts       = addRadix(CMDRADIX, returnValue);
	maxCounts    = (int)returnValue.length();

	if (counts < maxCounts)
		returnValue.erase(counts, maxCounts - counts);

	cmd.resetActiveObject();
	for (unsigned short i = 0; i < cmd.getNumberOfEntries(); i++) {
		
		actualCmd = cmd.readActiveObjectAndMakeNextOneActive();

		btods(actualCmd, buffer);
		returnValue += buffer;

	}

	return returnValue;

}

/****************************************************************
 * method sets the startPos value								*
 ****************************************************************/

void houghBorderCommand::setStartPos(std::string& value) {

	std::string temp;
	int         radix;

	temp     = value;
	extractRadix(&radix, &temp);
	startPos = stous(temp, radix); 

}

/****************************************************************
 * method sets the startPos value								*
 ****************************************************************/

void houghBorderCommand::setCmd(std::string& value) {

	std::string   temp;
	int           count;
	int           radix;
	unsigned long conversion;

	temp  = value;
	count = extractRadix(&radix, &temp);

	temp.erase(0, count);
	cmd.clear();

	if (radix == 2) {

		for (std::basic_string<char>::iterator i = temp.begin(); i != temp.end(); i++)
			cmd.push(dstob(*i));

	}
	else {

		if (temp.length() > longConversionDigits) {

			tooBigValueForAToLongConversionWarningMsg* tooBigValueForAToLongConversion = new tooBigValueForAToLongConversionWarningMsg(temp);
			tooBigValueForAToLongConversion->warningMsg();
			if(tooBigValueForAToLongConversion != NULL) {
				delete tooBigValueForAToLongConversion;
				tooBigValueForAToLongConversion = NULL;
			}

		}
		else {

			conversion = stoul(temp, radix);

			while (conversion) {

				if (conversion % 2)
					cmd.push(true);
				else
					cmd.push(false);

				conversion >>= 1;

			}

		}

	}

}

/****************************************************************
 * This method converts the object into a string representation	*
 * and adds no identifiers.										*
 ****************************************************************/

std::string houghBorderCommand::toNotIdentifiedString() {

	std::string returnValue;

	returnValue  = "(";
	returnValue += getStartPos();
	returnValue += ",";
	returnValue += getCmd();
	returnValue += ")";

	return returnValue;

}

/****************************************************************
 * This method converts the object into a string representation	*
 * and adds identifiers.										*
 ****************************************************************/

std::string houghBorderCommand::toIdentifiedString() {

	std::string returnValue;

	returnValue  = "StartPos: ";
	returnValue += getStartPos();
	returnValue += ", Cmd: ";
	returnValue += getCmd();

	return returnValue;

}
