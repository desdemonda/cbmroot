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
//     - implements methods to display the histogram's occupancy
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:29:27 $
// $Revision: 1.2 $
//
// *******************************************************************/


#include "../include/projectionEFGCNAnalysisDef.h"
#include "../include/projectionEFGCNEvent32Analysis.h"


/****************************************************************
 * resets the profiles for event and nothing for total			*
 ****************************************************************/

void projectionEFGCNEvent32Analysis::reset() {

	resetDisplays();

}

/****************************************************************
 * returns the corresponding entry for the x-axi.				*
 ****************************************************************/

int projectionEFGCNEvent32Analysis::getEntryOfXAxi(trackCoordinates& coordinates) {

	return (int)coordinates.get(DIM3);

}

/****************************************************************
 * returns the corresponding entry for the y-axi.				*
 ****************************************************************/

int projectionEFGCNEvent32Analysis::getEntryOfYAxi(trackCoordinates& coordinates) {

	return (int)coordinates.get(DIM2);

}

/****************************************************************
 * returns the corresponding string, which is drawn on the		*
 * x-axi.														*
 ****************************************************************/

const char* projectionEFGCNEvent32Analysis::getTitleOfXAxi() {
	
	return DIM3AXITITLE;

}

/****************************************************************
 * returns the corresponding string, which is drawn on the		*
 * y-axi.														*
 ****************************************************************/

const char* projectionEFGCNEvent32Analysis::getTitleOfYAxi() {
	
	return DIM2AXITITLE;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* projectionEFGCNEvent32Analysis::getNameOfEDisplay() {

	return eDisplayNameEvent32;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* projectionEFGCNEvent32Analysis::getTitleOfEDisplay() {

	return eDisplayTitleEvent32;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* projectionEFGCNEvent32Analysis::getNameOfFDisplay() {

	return fDisplayNameEvent32;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* projectionEFGCNEvent32Analysis::getTitleOfFDisplay() {

	return fDisplayTitleEvent32;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* projectionEFGCNEvent32Analysis::getNameOfGDisplay() {

	return gDisplayNameEvent32;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* projectionEFGCNEvent32Analysis::getTitleOfGDisplay() {

	return gDisplayTitleEvent32;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* projectionEFGCNEvent32Analysis::getNameOfCDisplay() {

	return cDisplayNameEvent32;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* projectionEFGCNEvent32Analysis::getTitleOfCDisplay() {

	return cDisplayTitleEvent32;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* projectionEFGCNEvent32Analysis::getNameOfNDisplay() {

	return nDisplayNameEvent32;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* projectionEFGCNEvent32Analysis::getTitleOfNDisplay() {

	return nDisplayTitleEvent32;

}

/****************************************************************
 * returns the corresponding string for the saving directory.	*
 ****************************************************************/

const char* projectionEFGCNEvent32Analysis::getNameOfSavingDirectory() {

	return EVENTSAVINGDIRECOTRY;

}

/****************************************************************
 * returns true, if the saving should be in subdirectories.		*
 ****************************************************************/

bool projectionEFGCNEvent32Analysis::getSavingInSubDirectory() {

	return EVENTSAVINGINSUBDIRECOTRY;

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

projectionEFGCNEvent32Analysis::projectionEFGCNEvent32Analysis() : projectionEFGCNAnalysis() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

projectionEFGCNEvent32Analysis::~projectionEFGCNEvent32Analysis() {

}
