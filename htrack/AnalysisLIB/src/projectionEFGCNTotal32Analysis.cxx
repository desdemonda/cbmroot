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
#include "../include/projectionEFGCNTotal32Analysis.h"


/****************************************************************
 * resets the profiles for event and nothing for total			*
 ****************************************************************/

void projectionEFGCNTotal32Analysis::reset() {

}

/****************************************************************
 * returns the corresponding entry for the x-axi.				*
 ****************************************************************/

int projectionEFGCNTotal32Analysis::getEntryOfXAxi(trackCoordinates& coordinates) {

	return (int)coordinates.get(DIM3);

}

/****************************************************************
 * returns the corresponding entry for the y-axi.				*
 ****************************************************************/

int projectionEFGCNTotal32Analysis::getEntryOfYAxi(trackCoordinates& coordinates) {

	return (int)coordinates.get(DIM2);

}

/****************************************************************
 * returns the corresponding string, which is drawn on the		*
 * x-axi.														*
 ****************************************************************/

const char* projectionEFGCNTotal32Analysis::getTitleOfXAxi() {
	
	return DIM3AXITITLE;

}

/****************************************************************
 * returns the corresponding string, which is drawn on the		*
 * y-axi.														*
 ****************************************************************/

const char* projectionEFGCNTotal32Analysis::getTitleOfYAxi() {
	
	return DIM2AXITITLE;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* projectionEFGCNTotal32Analysis::getNameOfEDisplay() {

	return eDisplayNameTotal32;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* projectionEFGCNTotal32Analysis::getTitleOfEDisplay() {

	return eDisplayTitleTotal32;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* projectionEFGCNTotal32Analysis::getNameOfFDisplay() {

	return fDisplayNameTotal32;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* projectionEFGCNTotal32Analysis::getTitleOfFDisplay() {

	return fDisplayTitleTotal32;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* projectionEFGCNTotal32Analysis::getNameOfGDisplay() {

	return gDisplayNameTotal32;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* projectionEFGCNTotal32Analysis::getTitleOfGDisplay() {

	return gDisplayTitleTotal32;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* projectionEFGCNTotal32Analysis::getNameOfCDisplay() {

	return cDisplayNameTotal32;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* projectionEFGCNTotal32Analysis::getTitleOfCDisplay() {

	return cDisplayTitleTotal32;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* projectionEFGCNTotal32Analysis::getNameOfNDisplay() {

	return nDisplayNameTotal32;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* projectionEFGCNTotal32Analysis::getTitleOfNDisplay() {

	return nDisplayTitleTotal32;

}

/****************************************************************
 * returns the corresponding string for the saving directory.	*
 ****************************************************************/

const char* projectionEFGCNTotal32Analysis::getNameOfSavingDirectory() {

	return TOTALSAVINGDIRECOTRY;

}

/****************************************************************
 * returns true, if the saving should be in subdirectories.		*
 ****************************************************************/

bool projectionEFGCNTotal32Analysis::getSavingInSubDirectory() {

	return TOTALSAVINGINSUBDIRECOTRY;

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

projectionEFGCNTotal32Analysis::projectionEFGCNTotal32Analysis() : projectionEFGCNAnalysis() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

projectionEFGCNTotal32Analysis::~projectionEFGCNTotal32Analysis() {

}
