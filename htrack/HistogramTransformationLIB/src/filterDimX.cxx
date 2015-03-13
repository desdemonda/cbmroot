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
//     - abstract class for all filter methods for
//		 maxMorphSearch
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:35:35 $
// $Revision: 1.2 $
//
// *******************************************************************/


#include "../include/filterDimX.h"
#include <stdio.h>


/****************************************************************
 * Default constructor											*
 ****************************************************************/

filterDimX::filterDimX() 
  : histogram(NULL),
    filterMem(NULL),
    baseFilter(NULL),
    filterSize(0),
    filterLocalSize(0)
{
  /*
	histogram       = NULL;
	filterMem       = NULL;	// allocation in derived class 
  baseFilter      = NULL;	// allocation in derived class 
	filterSize      = 0;
	filterLocalSize = 0;
*/
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

filterDimX::filterDimX( histogramData** _histogram,
			unsigned short  size,
			unsigned short  localSize) 
  : histogram(NULL),
    filterMem(NULL),
    baseFilter(NULL),
    filterSize(0),
    filterLocalSize(0)
{
  /*
	init(_histogram, size, localSize);
	filterMem       = NULL;
	baseFilter      = NULL;
  */
}

/****************************************************************
 * Default destructor											*
 ****************************************************************/

filterDimX::~filterDimX() {

	if(filterMem != NULL) {
	
		delete [] filterMem;
		filterMem = NULL;
	}
	if(baseFilter != NULL) {
	
		delete baseFilter;
		baseFilter = NULL;
	}

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void filterDimX::init( histogramData** _histogram,
					   unsigned short  size,
					   unsigned short  localSize) {

	this->histogram = _histogram;
	filterSize      = size;
	filterLocalSize = localSize;

}
