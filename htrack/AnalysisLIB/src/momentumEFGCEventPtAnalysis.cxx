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
#include "../include/momentumEFGCEventPtAnalysis.h"


/****************************************************************
 * resets the profiles for event and nothing for total			*
 ****************************************************************/

void momentumEFGCEventPtAnalysis::reset() {

	resetDisplays();

}

/****************************************************************
 * returns the corresponding impulse for each display entry.	*
 ****************************************************************/

double momentumEFGCEventPtAnalysis::getEntryOfXAxi(trackMomentum& momentum) {

	return formula.evaluateAbsPt(momentum);

}

/****************************************************************
 * returns the corresponding string, which is drawn on that axi,*
 * where the impulse is shown.									*
 ****************************************************************/

const char* momentumEFGCEventPtAnalysis::getTitleOfXAxi() {
	
	return PTXAXITITLE;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* momentumEFGCEventPtAnalysis::getNameOfEMDisplay() {

	return emDisplayNameEventPt;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* momentumEFGCEventPtAnalysis::getTitleOfEMDisplay() {

	return emDisplayTitleEventPt;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* momentumEFGCEventPtAnalysis::getNameOfFMDisplay() {

	return fmDisplayNameEventPt;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* momentumEFGCEventPtAnalysis::getTitleOfFMDisplay() {

	return fmDisplayTitleEventPt;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* momentumEFGCEventPtAnalysis::getNameOfGMDisplay() {

	return gmDisplayNameEventPt;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* momentumEFGCEventPtAnalysis::getTitleOfGMDisplay() {

	return gmDisplayTitleEventPt;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* momentumEFGCEventPtAnalysis::getNameOfCMDisplay() {

	return cmDisplayNameEventPt;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* momentumEFGCEventPtAnalysis::getTitleOfCMDisplay() {

	return cmDisplayTitleEventPt;

}

/****************************************************************
 * returns the corresponding string for the saving directory.	*
 ****************************************************************/

const char* momentumEFGCEventPtAnalysis::getNameOfSavingDirectory() {

	return EVENTPTSAVINGDIRECOTRY;

}

/****************************************************************
 * returns true, if the saving should be in subdirectories.		*
 ****************************************************************/

bool momentumEFGCEventPtAnalysis::getSavingInSubDirectory() {

	return EVENTPTSAVINGINSUBDIRECOTRY;

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

momentumEFGCEventPtAnalysis::momentumEFGCEventPtAnalysis() : momentumEFGCAnalysis() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

momentumEFGCEventPtAnalysis::~momentumEFGCEventPtAnalysis() {

}
