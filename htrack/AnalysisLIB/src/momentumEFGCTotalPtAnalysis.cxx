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
#include "../include/momentumEFGCTotalPtAnalysis.h"


/****************************************************************
 * resets the profiles for event and nothing for total			*
 ****************************************************************/

void momentumEFGCTotalPtAnalysis::reset() {

}

/****************************************************************
 * returns the corresponding impulse for each display entry.	*
 ****************************************************************/

double momentumEFGCTotalPtAnalysis::getEntryOfXAxi(trackMomentum& momentum) {

	return formula.evaluateAbsPt(momentum);

}

/****************************************************************
 * returns the corresponding string, which is drawn on that axi,*
 * where the impulse is shown.									*
 ****************************************************************/

const char* momentumEFGCTotalPtAnalysis::getTitleOfXAxi() {
	
	return PTXAXITITLE;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* momentumEFGCTotalPtAnalysis::getNameOfEMDisplay() {

	return emDisplayNameTotalPt;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* momentumEFGCTotalPtAnalysis::getTitleOfEMDisplay() {

	return emDisplayTitleTotalPt;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* momentumEFGCTotalPtAnalysis::getNameOfFMDisplay() {

	return fmDisplayNameTotalPt;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* momentumEFGCTotalPtAnalysis::getTitleOfFMDisplay() {

	return fmDisplayTitleTotalPt;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* momentumEFGCTotalPtAnalysis::getNameOfGMDisplay() {

	return gmDisplayNameTotalPt;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* momentumEFGCTotalPtAnalysis::getTitleOfGMDisplay() {

	return gmDisplayTitleTotalPt;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* momentumEFGCTotalPtAnalysis::getNameOfCMDisplay() {

	return cmDisplayNameTotalPt;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* momentumEFGCTotalPtAnalysis::getTitleOfCMDisplay() {

	return cmDisplayTitleTotalPt;

}

/****************************************************************
 * returns the corresponding string for the saving directory.	*
 ****************************************************************/

const char* momentumEFGCTotalPtAnalysis::getNameOfSavingDirectory() {

	return TOTALPTSAVINGDIRECOTRY;

}

/****************************************************************
 * returns true, if the saving should be in subdirectories.		*
 ****************************************************************/

bool momentumEFGCTotalPtAnalysis::getSavingInSubDirectory() {

	return TOTALPTSAVINGINSUBDIRECOTRY;

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

momentumEFGCTotalPtAnalysis::momentumEFGCTotalPtAnalysis() : momentumEFGCAnalysis() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

momentumEFGCTotalPtAnalysis::~momentumEFGCTotalPtAnalysis() {

}
