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
#include "../include/trackPropagationTotalPointAnalysis.h"
#include <string>


/****************************************************************
 * resets the profiles for event and nothing for total			*
 ****************************************************************/

void trackPropagationTotalPointAnalysis::reset() {

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

std::string trackPropagationTotalPointAnalysis::getNameOfDistanceDisplay(unsigned short index) {

	char        buffer[shortConversionDigits+1];
	std::string returnValue;

	ustos(index, buffer, 10, shortConversionDigits);

	returnValue  = distanceDisplayNameTotalPoint;
	returnValue += "_Station_";
	returnValue += buffer;

	return returnValue;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

std::string trackPropagationTotalPointAnalysis::getTitleOfDistanceDisplay(unsigned short index) {

	char        buffer[shortConversionDigits+1];
	std::string returnValue;

	ustos(index, buffer, 10, shortConversionDigits);

	returnValue  = distanceDisplayTitleTotalPoint;
	returnValue += "(Station:";
	returnValue += buffer;
	returnValue += ")";

	return returnValue;

}

/****************************************************************
 * returns the corresponding string for the saving directory.	*
 ****************************************************************/

const char* trackPropagationTotalPointAnalysis::getNameOfSavingDirectory() {

	return TOTALPOINTSAVINGDIRECOTRY;

}

/****************************************************************
 * returns true, if the saving should be in subdirectories.		*
 ****************************************************************/

bool trackPropagationTotalPointAnalysis::getSavingInSubDirectory() {

	return TOTALPOINTSAVINGINSUBDIRECOTRY;

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackPropagationTotalPointAnalysis::trackPropagationTotalPointAnalysis() : trackPropagationDAnalysis() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackPropagationTotalPointAnalysis::~trackPropagationTotalPointAnalysis() {

}
