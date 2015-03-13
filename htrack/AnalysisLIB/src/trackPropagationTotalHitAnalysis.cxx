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
#include "../include/trackPropagationTotalHitAnalysis.h"
#include <string>


/****************************************************************
 * resets the profiles for event and nothing for total			*
 ****************************************************************/

void trackPropagationTotalHitAnalysis::reset() {

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

std::string trackPropagationTotalHitAnalysis::getNameOfDistanceDisplay(unsigned short index) {

	char        buffer[shortConversionDigits+1];
	std::string returnValue;

	ustos(index, buffer, 10, shortConversionDigits);

	returnValue  = distanceDisplayNameTotalHit;
	returnValue += "_Station_";
	returnValue += buffer;

	return returnValue;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

std::string trackPropagationTotalHitAnalysis::getTitleOfDistanceDisplay(unsigned short index) {

	char        buffer[shortConversionDigits+1];
	std::string returnValue;

	ustos(index, buffer, 10, shortConversionDigits);

	returnValue  = distanceDisplayTitleTotalHit;
	returnValue += "(Station:";
	returnValue += buffer;
	returnValue += ")";

	return returnValue;

}

/****************************************************************
 * returns the corresponding string for the saving directory.	*
 ****************************************************************/

const char* trackPropagationTotalHitAnalysis::getNameOfSavingDirectory() {

	return TOTALHITSAVINGDIRECOTRY;

}

/****************************************************************
 * returns true, if the saving should be in subdirectories.		*
 ****************************************************************/

bool trackPropagationTotalHitAnalysis::getSavingInSubDirectory() {

	return TOTALHITSAVINGINSUBDIRECOTRY;

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackPropagationTotalHitAnalysis::trackPropagationTotalHitAnalysis() : trackPropagationDAnalysis() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackPropagationTotalHitAnalysis::~trackPropagationTotalHitAnalysis() {

}
