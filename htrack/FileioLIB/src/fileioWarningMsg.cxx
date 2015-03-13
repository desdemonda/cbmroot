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
//     - base class for warnings occuring during file access
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-10-10 13:47:23 $
// $Revision: 1.3 $
//
// *******************************************************************/


#include "../../MiscLIB/include/projects.h"
#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/fileioWarningMsg.h"
#include <iostream>


/****************************************************************
 * CLASS fileioWarningMsg						 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

fileioWarningMsg::fileioWarningMsg() {

}

/****************************************************************
 * Dstructor													*
 ****************************************************************/

fileioWarningMsg::~fileioWarningMsg() {

}

/****************************************************************
 * This method prints a message.				 				*
 ****************************************************************/

void fileioWarningMsg::printMsg(std::string message) {

#ifndef NOFILEIOWARNINGMESSAGE 

	std::cerr << std::endl;
	
	std::cerr << FILEIOLIB << " WARNING: ";
	
	if (!message.empty())
		std::cerr << message;
	
	std::cerr << std::endl;

#endif

}


/****************************************************************
 * CLASS missingCmdValWarningMsg				 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

missingCmdValWarningMsg::missingCmdValWarningMsg() : fileioWarningMsg() {

}

/****************************************************************
 * Dstructor													*
 ****************************************************************/

missingCmdValWarningMsg::~missingCmdValWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void missingCmdValWarningMsg::warningMsg() {

	printMsg("Missing specifier value in the file detected!!!");

}


/****************************************************************
 * CLASS missingCmdSpecWarningMsg				 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

missingCmdSpecWarningMsg::missingCmdSpecWarningMsg() : fileioWarningMsg() {

}

/****************************************************************
 * Dstructor													*
 ****************************************************************/

missingCmdSpecWarningMsg::~missingCmdSpecWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void missingCmdSpecWarningMsg::warningMsg() {

	printMsg("Missing command specifier in the file detected!!!");

}


/****************************************************************
 * CLASS unknownCmdSpecValPairWarningMsg		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

unknownCmdSpecValPairWarningMsg::unknownCmdSpecValPairWarningMsg() : fileioWarningMsg() {

}

/****************************************************************
 * Dstructor													*
 ****************************************************************/

unknownCmdSpecValPairWarningMsg::~unknownCmdSpecValPairWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void unknownCmdSpecValPairWarningMsg::warningMsg() {

	printMsg("The value cannot be assigned to the given specifier!!!");

}


/****************************************************************
 * CLASS cannotOpenFileWarning					 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotOpenFileWarningMsg::cannotOpenFileWarningMsg() : fileioWarningMsg() {

}

/****************************************************************
 * Dstructor													*
 ****************************************************************/

cannotOpenFileWarningMsg::~cannotOpenFileWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void cannotOpenFileWarningMsg::warningMsg() {

	printMsg("The file cannot be opened!!!");

}


/****************************************************************
 * CLASS writeDefaultFileWarningMsg				 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

writeDefaultFileWarningMsg::writeDefaultFileWarningMsg() : fileioWarningMsg(), fileName() {

  //	fileName.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

writeDefaultFileWarningMsg::writeDefaultFileWarningMsg(std::string name) : fileioWarningMsg(), fileName(name) {

  //	fileName = name;

}

/****************************************************************
 * Dstructor													*
 ****************************************************************/

writeDefaultFileWarningMsg::~writeDefaultFileWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void writeDefaultFileWarningMsg::warningMsg() {

	std::string temp;

	temp  = "The default configuration file '";
	temp += fileName;
	temp += "' will be written!!!";
	printMsg(temp);

}


/****************************************************************
 * CLASS dataCommentWarningMsg					 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

dataCommentWarningMsg::dataCommentWarningMsg() : fileioWarningMsg() {

}

/****************************************************************
 * Dstructor													*
 ****************************************************************/

dataCommentWarningMsg::~dataCommentWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void dataCommentWarningMsg::warningMsg() {

	printMsg("A comment line is detected in the data area of the file!!!");

}


/****************************************************************
 * CLASS tooMuchDataWarningMsg					 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

tooMuchDataWarningMsg::tooMuchDataWarningMsg() : fileioWarningMsg() {

}

/****************************************************************
 * Dstructor													*
 ****************************************************************/

tooMuchDataWarningMsg::~tooMuchDataWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void tooMuchDataWarningMsg::warningMsg() {

	printMsg("Too much data is detected in the file!!!");

}


/****************************************************************
 * CLASS lockOutOfRangeWarningMsg					 			*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

lockOutOfRangeWarningMsg::lockOutOfRangeWarningMsg() : fileioWarningMsg(), lockId(0), numberOfLocks(0)  {

  //	lockId        = 0;
  //	numberOfLocks = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

lockOutOfRangeWarningMsg::lockOutOfRangeWarningMsg(unsigned int _lockId, unsigned int _numberOfLocks) : fileioWarningMsg(), lockId(_lockId), numberOfLocks(_numberOfLocks) {

  //	this->lockId        = _lockId;
  //	this->numberOfLocks = _numberOfLocks;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

lockOutOfRangeWarningMsg::~lockOutOfRangeWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void lockOutOfRangeWarningMsg::warningMsg() {

	std::string temp;
	char buffer[intConversionDigits+1];

	temp += "There are just ";
	uitos(numberOfLocks, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += " locks.";
	temp = " So the lockId ";
	uitos(lockId, buffer, 10, intConversionDigits);
	temp += buffer;
	temp += " is out of range!!!";
	printMsg(temp);

}
