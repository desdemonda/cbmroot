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


#include "../include/filterDimZDimZDimZ.h"


/****************************************************************
 * Default constructor											*
 ****************************************************************/

filterDimZDimZDimZ::filterDimZDimZDimZ() 
  : filterDimZDimZ(), 
    filterSize3(0)
{

  //	filterSize3 = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

filterDimZDimZDimZ::filterDimZDimZDimZ( trackData**    _tracks,
					unsigned short size1,
					unsigned short size2,
					unsigned short size3,
					unsigned short size,
					unsigned short localSize) 
  : filterDimZDimZ(_tracks, size1, size2, size, localSize), 
    filterSize3(size3)
{

  //	filterSize3 = size3;

}

/****************************************************************
 * Default destructor											*
 ****************************************************************/

filterDimZDimZDimZ::~filterDimZDimZDimZ() {

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void filterDimZDimZDimZ::init( trackData**    _tracks,
							   unsigned short size1,
							   unsigned short size2,
							   unsigned short size3,
							   unsigned short size,
							   unsigned short localSize) {

	filterDimZDimZ::init(_tracks, size1, size2, size, localSize);
	filterSize3 = size3;

}
