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
#include "../include/projectionEFGCNEvent13Analysis.h"


/****************************************************************
 * resets the profiles for event and nothing for total			*
 ****************************************************************/

void projectionEFGCNEvent13Analysis::reset() {

	resetDisplays();

}

/****************************************************************
 * returns the corresponding entry for the x-axi.				*
 ****************************************************************/

int projectionEFGCNEvent13Analysis::getEntryOfXAxi(trackCoordinates& coordinates) {

	return (int)coordinates.get(DIM1);

}

/****************************************************************
 * returns the corresponding entry for the y-axi.				*
 ****************************************************************/

int projectionEFGCNEvent13Analysis::getEntryOfYAxi(trackCoordinates& coordinates) {

	return (int)coordinates.get(DIM3);

}

/****************************************************************
 * returns the corresponding string, which is drawn on the		*
 * x-axi.														*
 ****************************************************************/

const char* projectionEFGCNEvent13Analysis::getTitleOfXAxi() {
	
	return DIM1AXITITLE;

}

/****************************************************************
 * returns the corresponding string, which is drawn on the		*
 * y-axi.														*
 ****************************************************************/

const char* projectionEFGCNEvent13Analysis::getTitleOfYAxi() {
	
	return DIM3AXITITLE;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* projectionEFGCNEvent13Analysis::getNameOfEDisplay() {

	return eDisplayNameEvent13;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* projectionEFGCNEvent13Analysis::getTitleOfEDisplay() {

	return eDisplayTitleEvent13;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* projectionEFGCNEvent13Analysis::getNameOfFDisplay() {

	return fDisplayNameEvent13;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* projectionEFGCNEvent13Analysis::getTitleOfFDisplay() {

	return fDisplayTitleEvent13;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* projectionEFGCNEvent13Analysis::getNameOfGDisplay() {

	return gDisplayNameEvent13;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* projectionEFGCNEvent13Analysis::getTitleOfGDisplay() {

	return gDisplayTitleEvent13;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* projectionEFGCNEvent13Analysis::getNameOfCDisplay() {

	return cDisplayNameEvent13;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* projectionEFGCNEvent13Analysis::getTitleOfCDisplay() {

	return cDisplayTitleEvent13;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* projectionEFGCNEvent13Analysis::getNameOfNDisplay() {

	return nDisplayNameEvent13;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* projectionEFGCNEvent13Analysis::getTitleOfNDisplay() {

	return nDisplayTitleEvent13;

}

/****************************************************************
 * returns the corresponding string for the saving directory.	*
 ****************************************************************/

const char* projectionEFGCNEvent13Analysis::getNameOfSavingDirectory() {

	return EVENTSAVINGDIRECOTRY;

}

/****************************************************************
 * returns true, if the saving should be in subdirectories.		*
 ****************************************************************/

bool projectionEFGCNEvent13Analysis::getSavingInSubDirectory() {

	return EVENTSAVINGINSUBDIRECOTRY;

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

projectionEFGCNEvent13Analysis::projectionEFGCNEvent13Analysis() : projectionEFGCNAnalysis() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

projectionEFGCNEvent13Analysis::~projectionEFGCNEvent13Analysis() {

}
