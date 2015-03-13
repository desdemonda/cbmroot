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
#include "../include/momentumEFGCEventPzAnalysis.h"


/****************************************************************
 * resets the profiles for event and nothing for total			*
 ****************************************************************/

void momentumEFGCEventPzAnalysis::reset() {

	resetDisplays();

}

/****************************************************************
 * returns the corresponding impulse for each display entry.	*
 ****************************************************************/

double momentumEFGCEventPzAnalysis::getEntryOfXAxi(trackMomentum& momentum) {

	return momentum.get(PZ);

}

/****************************************************************
 * returns the corresponding string, which is drawn on that axi,*
 * where the impulse is shown.									*
 ****************************************************************/

const char* momentumEFGCEventPzAnalysis::getTitleOfXAxi() {
	
	return PZXAXITITLE;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* momentumEFGCEventPzAnalysis::getNameOfEMDisplay() {

	return emDisplayNameEventPz;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* momentumEFGCEventPzAnalysis::getTitleOfEMDisplay() {

	return emDisplayTitleEventPz;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* momentumEFGCEventPzAnalysis::getNameOfFMDisplay() {

	return fmDisplayNameEventPz;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* momentumEFGCEventPzAnalysis::getTitleOfFMDisplay() {

	return fmDisplayTitleEventPz;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* momentumEFGCEventPzAnalysis::getNameOfGMDisplay() {

	return gmDisplayNameEventPz;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* momentumEFGCEventPzAnalysis::getTitleOfGMDisplay() {

	return gmDisplayTitleEventPz;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* momentumEFGCEventPzAnalysis::getNameOfCMDisplay() {

	return cmDisplayNameEventPz;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* momentumEFGCEventPzAnalysis::getTitleOfCMDisplay() {

	return cmDisplayTitleEventPz;

}

/****************************************************************
 * returns the corresponding string for the saving directory.	*
 ****************************************************************/

const char* momentumEFGCEventPzAnalysis::getNameOfSavingDirectory() {

	return EVENTPZSAVINGDIRECOTRY;

}

/****************************************************************
 * returns true, if the saving should be in subdirectories.		*
 ****************************************************************/

bool momentumEFGCEventPzAnalysis::getSavingInSubDirectory() {

	return EVENTPZSAVINGINSUBDIRECOTRY;

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

momentumEFGCEventPzAnalysis::momentumEFGCEventPzAnalysis() : momentumEFGCAnalysis() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

momentumEFGCEventPzAnalysis::~momentumEFGCEventPzAnalysis() {

}
