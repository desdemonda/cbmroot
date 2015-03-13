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
//     - implements methods to analyze the trackfinding algorithm graphically
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:29:26 $
// $Revision: 1.3 $
//
// *******************************************************************/


#include "../include/momentumEFGCAnalysisDef.h"
#include "../include/momentumEFGCTotalPzAnalysis.h"


/****************************************************************
 * resets the profiles for event and nothing for total			*
 ****************************************************************/

void momentumEFGCTotalPzAnalysis::reset() {

}

/****************************************************************
 * returns the corresponding impulse for each display entry.	*
 ****************************************************************/

double momentumEFGCTotalPzAnalysis::getEntryOfXAxi(trackMomentum& momentum) {

	return momentum.get(PZ);

}

/****************************************************************
 * returns the corresponding string, which is drawn on that axi,*
 * where the impulse is shown.									*
 ****************************************************************/

const char* momentumEFGCTotalPzAnalysis::getTitleOfXAxi() {
	
	return PZXAXITITLE;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* momentumEFGCTotalPzAnalysis::getNameOfEMDisplay() {

	return emDisplayNameTotalPz;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* momentumEFGCTotalPzAnalysis::getTitleOfEMDisplay() {

	return emDisplayTitleTotalPz;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* momentumEFGCTotalPzAnalysis::getNameOfFMDisplay() {

	return fmDisplayNameTotalPz;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* momentumEFGCTotalPzAnalysis::getTitleOfFMDisplay() {

	return fmDisplayTitleTotalPz;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* momentumEFGCTotalPzAnalysis::getNameOfGMDisplay() {

	return gmDisplayNameTotalPz;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* momentumEFGCTotalPzAnalysis::getTitleOfGMDisplay() {

	return gmDisplayTitleTotalPz;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* momentumEFGCTotalPzAnalysis::getNameOfCMDisplay() {

	return cmDisplayNameTotalPz;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* momentumEFGCTotalPzAnalysis::getTitleOfCMDisplay() {

	return cmDisplayTitleTotalPz;

}


/****************************************************************
 * returns the corresponding string for the saving directory.	*
 ****************************************************************/

const char* momentumEFGCTotalPzAnalysis::getNameOfSavingDirectory() {

	return TOTALPZSAVINGDIRECOTRY;

}

/****************************************************************
 * returns true, if the saving should be in subdirectories.		*
 ****************************************************************/

bool momentumEFGCTotalPzAnalysis::getSavingInSubDirectory() {

	return TOTALPZSAVINGINSUBDIRECOTRY;

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

momentumEFGCTotalPzAnalysis::momentumEFGCTotalPzAnalysis() : momentumEFGCAnalysis() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

momentumEFGCTotalPzAnalysis::~momentumEFGCTotalPzAnalysis() {

}
