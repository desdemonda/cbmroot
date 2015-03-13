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
// $Revision: 1.10 $
//
// *******************************************************************/


#include "../../MiscLIB/include/conversionRoutines.h"
#include "../../MiscLIB/include/defs.h"
#include "../include/dataObjectError.h"
#include "../include/dataObjectWarningMsg.h"
#include "../include/lutHoughBorder.h"


/****************************************************************
 * CLASS lutHoughBorder							 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

lutHoughBorder::lutHoughBorder() : houghCoord() {

  	houghCoord.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

lutHoughBorder::lutHoughBorder(const lutHoughBorder& value) : houghCoord(value.houghCoord) {

  //	this->houghCoord = value.houghCoord;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

lutHoughBorder::~lutHoughBorder() {

	houghCoord.clear();

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const lutHoughBorder& lutHoughBorder::operator = (const lutHoughBorder& value) {

	this->houghCoord = value.houghCoord;

	return *this;

}

/****************************************************************
 * This method clears the object.								*
 ****************************************************************/

void lutHoughBorder::clear() {

	houghCoord.clear();

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void lutHoughBorder::init() {

	clear();

}

/****************************************************************
 * This method converts the object into a string representation	*
 * and adds no identifiers.										*
 ****************************************************************/

std::string lutHoughBorder::toNotIdentifiedString() {

	std::string         returnValue;
	houghBorderPosition actualHoughCoord;

	returnValue.clear();
	houghCoord.resetActiveObject();
	for (unsigned short i = 0; i < houghCoord.getNumberOfEntries(); i++) {
		
		actualHoughCoord = houghCoord.readActiveObjectAndMakeNextOneActive();

		if (i > 0)
			returnValue += ", ";

		returnValue += actualHoughCoord.toNotIdentifiedString();

	}

	return returnValue;

}

/****************************************************************
 * This method converts the object into a string representation	*
 * and adds identifiers.										*
 ****************************************************************/

std::string lutHoughBorder::toIdentifiedString() {

	std::string returnValue;

	returnValue  = "(Pos1,Pos2): ";
	returnValue += toNotIdentifiedString();

	return returnValue;

}

/****************************************************************
 * This method returns the houghBorder command.					*
 ****************************************************************/

houghBorderCommand lutHoughBorder::getHoughBorderCommand(unsigned short maxDim) {

	houghBorderCommand  houghCmd;
	houghBorderPosition actualHoughCoord;
	houghBorderPosition previousHoughCoord;
	unsigned short      cmdCounter;

	if (!houghCoord.isEmpty()) {

		houghCoord.resetActiveObject();
		houghCmd.cmd.clear();

		cmdCounter         = 0;
		previousHoughCoord = houghCoord.readActiveObjectAndMakeNextOneActive();
		houghCmd.startPos  = previousHoughCoord.pos1;

		for (unsigned long i = 1 ; i < houghCoord.getNumberOfEntries(); i++) {

			actualHoughCoord = houghCoord.readActiveObjectAndMakeNextOneActive();

#if (LUTVERSION < 2)

			throw borderIdRuleError();

#endif

#if (LUTVERSION > 1)

			if (actualHoughCoord.pos2 - previousHoughCoord.pos2 != 1)
				throw borderIdRuleError();

			if (actualHoughCoord.pos1 - previousHoughCoord.pos1 < 0)
				throw borderIdRuleError();

#endif

#if (LUTVERSION > 2)

			if (actualHoughCoord.pos1 - previousHoughCoord.pos1 > 1)
				throw borderIdRuleError();

#endif

			if (actualHoughCoord.pos1 - previousHoughCoord.pos1) {
				
				houghCmd.cmd.push(true);
				cmdCounter += actualHoughCoord.pos1 - previousHoughCoord.pos1;

			}
			else {

				houghCmd.cmd.push(false);

			}

			previousHoughCoord = actualHoughCoord;

		}

		if (houghCmd.startPos + cmdCounter + 1 > maxDim)
			throw borderIdConversionError();

	}

	return houghCmd;

}

/****************************************************************
 * This method sets the houghBorder command.					*
 ****************************************************************/

void lutHoughBorder::setHoughBorderCommand(houghBorderCommand& value, unsigned short maxFirstDim, unsigned short maxSecondDim) {

	houghBorderPosition actualHoughCoord;
	bool                actualCommand;

	houghCoord.clear();

	actualHoughCoord.pos1 = value.startPos;
	actualHoughCoord.pos2 = 0;

	if ((actualHoughCoord.pos1 < maxFirstDim) && (actualHoughCoord.pos2 < maxSecondDim)) {

		houghCoord.push(actualHoughCoord);

	}
	else {

		rangeLutHoughBorderWarningMsg* rangeLutHoughBorder = new rangeLutHoughBorderWarningMsg(maxFirstDim, maxSecondDim, actualHoughCoord.pos1, actualHoughCoord.pos2);
		rangeLutHoughBorder->warningMsg();
		if(rangeLutHoughBorder != NULL) {
			delete rangeLutHoughBorder;
			rangeLutHoughBorder = NULL;
		}

	}

	value.cmd.resetActiveObject();
	for (unsigned long i = 0; i < value.cmd.getNumberOfEntries(); i++) {

		actualCommand = value.cmd.readActiveObjectAndMakeNextOneActive();

		actualHoughCoord.pos2++;
		if (actualCommand)
			actualHoughCoord.pos1++;

		if ((actualHoughCoord.pos1 < maxFirstDim) && (actualHoughCoord.pos2 < maxSecondDim)) {

			houghCoord.push(actualHoughCoord);

		}
		else {

			rangeLutHoughBorderWarningMsg* rangeLutHoughBorder = new rangeLutHoughBorderWarningMsg(maxFirstDim, maxSecondDim, actualHoughCoord.pos1, actualHoughCoord.pos2);
			rangeLutHoughBorder->warningMsg();
			if(rangeLutHoughBorder != NULL) {
				delete rangeLutHoughBorder;
				rangeLutHoughBorder = NULL;
			}

		}

	}

	/* missing commands are treated as leading zeros in the command value */
	actualHoughCoord.pos2++;
	for (; actualHoughCoord.pos2 < maxSecondDim; actualHoughCoord.pos2++)
		houghCoord.push(actualHoughCoord);

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double lutHoughBorder::getReservedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = houghCoord.getReservedSizeOfData(0);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double lutHoughBorder::getAllocatedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = houghCoord.getAllocatedSizeOfData(0);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double lutHoughBorder::getUsedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = houghCoord.getUsedSizeOfData(0);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}
