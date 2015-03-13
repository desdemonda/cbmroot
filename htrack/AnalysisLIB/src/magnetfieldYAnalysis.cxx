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
//     - implements methods to analyze the magnetfield
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:29:26 $
// $Revision: 1.2 $
//
// *******************************************************************/


#include "../include/magnetfieldAnalysisDef.h"
#include "../include/magnetfieldYAnalysis.h"


/****************************************************************
 * returns the corresponding string, which is drawn on that axi,*
 * where the impulse is shown.									*
 ****************************************************************/

const char* magnetfieldYAnalysis::getTitleOfXAxi() {
	
	return YXAXITITLE;

}

/**
 * returns the corresponding modifier for the first constant
 * dimension.
 */

const char* magnetfieldYAnalysis::getDim1Modifier() {

	return DIMXMODIFIER;

}

/**
 * returns the corresponding modifier for the second constant
 * dimension.
 */

const char* magnetfieldYAnalysis::getDim2Modifier() {

	return DIMZMODIFIER;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* magnetfieldYAnalysis::getNameOfBxDisplay() {

	return yBxDisplayName;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* magnetfieldYAnalysis::getTitleOfBxDisplay() {

	return yBxDisplayTitle;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* magnetfieldYAnalysis::getNameOfByDisplay() {

	return yByDisplayName;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* magnetfieldYAnalysis::getTitleOfByDisplay() {

	return yByDisplayTitle;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* magnetfieldYAnalysis::getNameOfBzDisplay() {

	return yBzDisplayName;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* magnetfieldYAnalysis::getTitleOfBzDisplay() {

	return yBzDisplayTitle;

}

/****************************************************************
 * returns true, if the saving should be in subdirectories.		*
 ****************************************************************/

bool magnetfieldYAnalysis::getSavingInSubDirectory() {

	return YSAVINGINSUBDIRECOTRY;

}

/****************************************************************
 * returns the corresponding string for the saving subdirectory.*
 ****************************************************************/

const char* magnetfieldYAnalysis::getNameOfSavingSubDirectory() {

	return YSAVINGSUBDIRECTORY;

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

magnetfieldYAnalysis::magnetfieldYAnalysis() : magnetfieldDirectionAnalysis() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

magnetfieldYAnalysis::~magnetfieldYAnalysis() {

}
