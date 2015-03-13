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
//     - implements methods to analyze the track propagation's quality
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2009-01-13 15:40:12 $
// $Revision: 1.0 $
//
// *******************************************************************/


#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/trackPropagationAnalysisDef.h"
#include "../include/trackPropagationEventHitAnalysis.h"
#include <string>


/****************************************************************
 * resets the profiles for event and nothing for total			*
 ****************************************************************/

void trackPropagationEventHitAnalysis::reset() {

	resetDisplays();

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

std::string trackPropagationEventHitAnalysis::getNameOfDistanceDisplay(unsigned short index) {

	char        buffer[shortConversionDigits+1];
	std::string returnValue;

	ustos(index, buffer, 10, shortConversionDigits);

	returnValue  = distanceDisplayNameEventHit;
	returnValue += "_Station_";
	returnValue += buffer;

	return returnValue;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

std::string trackPropagationEventHitAnalysis::getTitleOfDistanceDisplay(unsigned short index) {

	char        buffer[shortConversionDigits+1];
	std::string returnValue;

	ustos(index, buffer, 10, shortConversionDigits);

	returnValue  = distanceDisplayTitleEventHit;
	returnValue += "(Station:";
	returnValue += buffer;
	returnValue += ")";

	return returnValue;

}

/****************************************************************
 * returns the corresponding string for the saving directory.	*
 ****************************************************************/

const char* trackPropagationEventHitAnalysis::getNameOfSavingDirectory() {

	return EVENTHITSAVINGDIRECOTRY;

}

/****************************************************************
 * returns true, if the saving should be in subdirectories.		*
 ****************************************************************/

bool trackPropagationEventHitAnalysis::getSavingInSubDirectory() {

	return EVENTHITSAVINGINSUBDIRECOTRY;

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackPropagationEventHitAnalysis::trackPropagationEventHitAnalysis() : trackPropagationDAnalysis() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackPropagationEventHitAnalysis::~trackPropagationEventHitAnalysis() {

}
