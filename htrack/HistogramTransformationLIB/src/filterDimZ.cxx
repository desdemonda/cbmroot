/* *******************************************************************
// (C)opyright 2005
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


#include "../include/filterDimZ.h"
#include <stdio.h>


/****************************************************************
 * Default constructor											*
 ****************************************************************/

filterDimZ::filterDimZ() 
  : tracks(NULL),
    filterMem(NULL),
    baseFilter(NULL),
    filterSize(0),
    filterLocalSize(0)
{
  /*
	tracks          = NULL;
	filterMem       = NULL;	// allocation in derived class 
	baseFilter      = NULL;	// allocation in derived class 
	filterSize      = 0;
	filterLocalSize = 0;
*/
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

filterDimZ::filterDimZ( trackData**    _tracks,
			unsigned short size,
			unsigned short localSize) 
  : tracks(NULL),
    filterMem(NULL),
    baseFilter(NULL),
    filterSize(0),
    filterLocalSize(0)
{

	init(_tracks, size, localSize);
	//	filterMem       = NULL;
	//	baseFilter      = NULL;

}

/****************************************************************
 * Default destructor											*
 ****************************************************************/

filterDimZ::~filterDimZ() {

	if(filterMem != NULL) {
	
		delete[] filterMem;
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

void filterDimZ::init( trackData**    _tracks,
					   unsigned short size,
					   unsigned short localSize) {

	this->tracks    = _tracks;
	filterSize      = size;
	filterLocalSize = localSize;

}
