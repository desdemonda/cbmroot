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
//     - base class for warnings occuring during output result operations
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:37:34 $
// $Revision: 1.2 $
//
// *******************************************************************/


#include "../../MiscLIB/include/projects.h"
#include "../include/outputResultWarningMsg.h"
#include <iostream>


/****************************************************************
 * CLASS outputResultWarningMsg					 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

outputResultWarningMsg::outputResultWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

outputResultWarningMsg::~outputResultWarningMsg() {

}

/****************************************************************
 * This method prints a message.				 				*
 ****************************************************************/

void outputResultWarningMsg::printMsg(std::string message) {

#ifndef NOOUTPUTRESULTWARNINGMESSAGE

	std::cerr << std::endl;
	
	std::cerr << OUTPUTRESULTLIB << " WARNING: ";
	
	if (!message.empty())
		std::cerr << message;
	
	std::cerr << std::endl;

#endif

}


/****************************************************************
 * CLASS trackPointerIsNotSupportedWarningMsg	 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackPointerIsNotSupportedWarningMsg::trackPointerIsNotSupportedWarningMsg() : outputResultWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackPointerIsNotSupportedWarningMsg::~trackPointerIsNotSupportedWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void trackPointerIsNotSupportedWarningMsg::warningMsg() {

	printMsg("The possibility to set the tracks pointer externally is unsupported!!!");

}


/****************************************************************
 * CLASS trackPointerIsDisabledWarningMsg		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackPointerIsDisabledWarningMsg::trackPointerIsDisabledWarningMsg() : outputResultWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackPointerIsDisabledWarningMsg::~trackPointerIsDisabledWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void trackPointerIsDisabledWarningMsg::warningMsg() {

	printMsg("The possibility to set the tracks pointer externally is disabled!!!");

}
