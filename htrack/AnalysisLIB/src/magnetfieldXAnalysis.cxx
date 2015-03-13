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
#include "../include/magnetfieldXAnalysis.h"


/****************************************************************
 * returns the corresponding string, which is drawn on that axi,*
 * where the impulse is shown.									*
 ****************************************************************/

const char* magnetfieldXAnalysis::getTitleOfXAxi() {
	
	return XXAXITITLE;

}

/**
 * returns the corresponding modifier for the first constant
 * dimension.
 */

const char* magnetfieldXAnalysis::getDim1Modifier() {

	return DIMYMODIFIER;

}

/**
 * returns the corresponding modifier for the second constant
 * dimension.
 */

const char* magnetfieldXAnalysis::getDim2Modifier() {

	return DIMZMODIFIER;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* magnetfieldXAnalysis::getNameOfBxDisplay() {

	return xBxDisplayName;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* magnetfieldXAnalysis::getTitleOfBxDisplay() {

	return xBxDisplayTitle;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* magnetfieldXAnalysis::getNameOfByDisplay() {

	return xByDisplayName;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* magnetfieldXAnalysis::getTitleOfByDisplay() {

	return xByDisplayTitle;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* magnetfieldXAnalysis::getNameOfBzDisplay() {

	return xBzDisplayName;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* magnetfieldXAnalysis::getTitleOfBzDisplay() {

	return xBzDisplayTitle;

}

/****************************************************************
 * returns true, if the saving should be in subdirectories.		*
 ****************************************************************/

bool magnetfieldXAnalysis::getSavingInSubDirectory() {

	return XSAVINGINSUBDIRECOTRY;

}

/****************************************************************
 * returns the corresponding string for the saving subdirectory.*
 ****************************************************************/

const char* magnetfieldXAnalysis::getNameOfSavingSubDirectory() {

	return XSAVINGSUBDIRECTORY;

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

magnetfieldXAnalysis::magnetfieldXAnalysis() : magnetfieldDirectionAnalysis() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

magnetfieldXAnalysis::~magnetfieldXAnalysis() {

}
