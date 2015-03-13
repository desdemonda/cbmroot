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
#include "../include/projectionEFGCNTotal13Analysis.h"


/****************************************************************
 * resets the profiles for event and nothing for total			*
 ****************************************************************/

void projectionEFGCNTotal13Analysis::reset() {

}

/****************************************************************
 * returns the corresponding entry for the x-axi.				*
 ****************************************************************/

int projectionEFGCNTotal13Analysis::getEntryOfXAxi(trackCoordinates& coordinates) {

	return (int)coordinates.get(DIM1);

}

/****************************************************************
 * returns the corresponding entry for the y-axi.				*
 ****************************************************************/

int projectionEFGCNTotal13Analysis::getEntryOfYAxi(trackCoordinates& coordinates) {

	return (int)coordinates.get(DIM3);

}

/****************************************************************
 * returns the corresponding string, which is drawn on the		*
 * x-axi.														*
 ****************************************************************/

const char* projectionEFGCNTotal13Analysis::getTitleOfXAxi() {
	
	return DIM1AXITITLE;

}

/****************************************************************
 * returns the corresponding string, which is drawn on the		*
 * y-axi.														*
 ****************************************************************/

const char* projectionEFGCNTotal13Analysis::getTitleOfYAxi() {
	
	return DIM3AXITITLE;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* projectionEFGCNTotal13Analysis::getNameOfEDisplay() {

	return eDisplayNameTotal13;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* projectionEFGCNTotal13Analysis::getTitleOfEDisplay() {

	return eDisplayTitleTotal13;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* projectionEFGCNTotal13Analysis::getNameOfFDisplay() {

	return fDisplayNameTotal13;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* projectionEFGCNTotal13Analysis::getTitleOfFDisplay() {

	return fDisplayTitleTotal13;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* projectionEFGCNTotal13Analysis::getNameOfGDisplay() {

	return gDisplayNameTotal13;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* projectionEFGCNTotal13Analysis::getTitleOfGDisplay() {

	return gDisplayTitleTotal13;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* projectionEFGCNTotal13Analysis::getNameOfCDisplay() {

	return cDisplayNameTotal13;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* projectionEFGCNTotal13Analysis::getTitleOfCDisplay() {

	return cDisplayTitleTotal13;

}

/****************************************************************
 * returns the corresponding string for the name identification.*
 ****************************************************************/

const char* projectionEFGCNTotal13Analysis::getNameOfNDisplay() {

	return nDisplayNameTotal13;

}

/****************************************************************
 * returns the corresponding string for the title.				*
 ****************************************************************/

const char* projectionEFGCNTotal13Analysis::getTitleOfNDisplay() {

	return nDisplayTitleTotal13;

}

/****************************************************************
 * returns the corresponding string for the saving directory.	*
 ****************************************************************/

const char* projectionEFGCNTotal13Analysis::getNameOfSavingDirectory() {

	return TOTALSAVINGDIRECOTRY;

}

/****************************************************************
 * returns true, if the saving should be in subdirectories.		*
 ****************************************************************/

bool projectionEFGCNTotal13Analysis::getSavingInSubDirectory() {

	return TOTALSAVINGINSUBDIRECOTRY;

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

projectionEFGCNTotal13Analysis::projectionEFGCNTotal13Analysis() : projectionEFGCNAnalysis() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

projectionEFGCNTotal13Analysis::~projectionEFGCNTotal13Analysis() {

}
