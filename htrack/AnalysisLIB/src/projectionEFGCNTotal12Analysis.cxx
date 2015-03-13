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
#include "../include/projectionEFGCNTotal12Analysis.h"


/****************************************************************
 * resets the profiles for event and nothing for total			*
 ****************************************************************/

void projectionEFGCNTotal12Analysis::reset() {

}

/****************************************************************
 * returns the corresponding entry for the x-axi.				*
 ****************************************************************/

int projectionEFGCNTotal12Analysis::getEntryOfXAxi(trackCoordinates& coordinates) {

	return (int)coordinates.get(DIM1);

}

/****************************************************************
 * returns the corresponding entry for the y-axi.				*
 ****************************************************************/

int projectionEFGCNTotal12Analysis::getEntryOfYAxi(trackCoordinates& coordinates) {

	return (int)coordinates.get(DIM2);

}

/****************************************************************
 * returns the corresponding string, which is drawn on the		*
 * x-axi.														*
 ****************************************************************/

const char* projectionEFGCNTotal12Analysis::getTitleOfXAxi() {
	
	return DIM1AXITITLE;

}

/****************************************************************
 * returns the corresponding string, which is drawn on the		*
 * y-axi.														*
 ****************************************************************/

const char* projectionEFGCNTotal12Analysis::getTitleOfYAxi() {
	
	return DIM2AXITITLE;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* projectionEFGCNTotal12Analysis::getNameOfEDisplay() {

	return eDisplayNameTotal12;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* projectionEFGCNTotal12Analysis::getTitleOfEDisplay() {

	return eDisplayTitleTotal12;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* projectionEFGCNTotal12Analysis::getNameOfFDisplay() {

	return fDisplayNameTotal12;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* projectionEFGCNTotal12Analysis::getTitleOfFDisplay() {

	return fDisplayTitleTotal12;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* projectionEFGCNTotal12Analysis::getNameOfGDisplay() {

	return gDisplayNameTotal12;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* projectionEFGCNTotal12Analysis::getTitleOfGDisplay() {

	return gDisplayTitleTotal12;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* projectionEFGCNTotal12Analysis::getNameOfCDisplay() {

	return cDisplayNameTotal12;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* projectionEFGCNTotal12Analysis::getTitleOfCDisplay() {

	return cDisplayTitleTotal12;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* projectionEFGCNTotal12Analysis::getNameOfNDisplay() {

	return nDisplayNameTotal12;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* projectionEFGCNTotal12Analysis::getTitleOfNDisplay() {

	return nDisplayTitleTotal12;

}

/****************************************************************
 * returns the corresponding string for the saving directory.	*
 ****************************************************************/

const char* projectionEFGCNTotal12Analysis::getNameOfSavingDirectory() {

	return TOTALSAVINGDIRECOTRY;

}

/****************************************************************
 * returns true, if the saving should be in subdirectories.		*
 ****************************************************************/

bool projectionEFGCNTotal12Analysis::getSavingInSubDirectory() {

	return TOTALSAVINGINSUBDIRECOTRY;

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

projectionEFGCNTotal12Analysis::projectionEFGCNTotal12Analysis() : projectionEFGCNAnalysis() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

projectionEFGCNTotal12Analysis::~projectionEFGCNTotal12Analysis() {

}
