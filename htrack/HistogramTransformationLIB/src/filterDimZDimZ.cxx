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


#include "../include/filterDimZDimZ.h"


/****************************************************************
 * Default constructor											*
 ****************************************************************/

filterDimZDimZ::filterDimZDimZ() 
  : filterDimZ(), 
    filterSize1(0),
    filterSize2(0)
{
  /*
	filterSize1 = 0;
	filterSize2 = 0;
  */
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

filterDimZDimZ::filterDimZDimZ( trackData**    _tracks,
				unsigned short size1,
				unsigned short size2,
				unsigned short size,
				unsigned short localSize) 
  : filterDimZ(_tracks, size, localSize),
    filterSize1(size1),
    filterSize2(size2)
{
  /*
	filterSize1 = size1;
	filterSize2 = size2;
  */
}

/****************************************************************
 * Default destructor											*
 ****************************************************************/

filterDimZDimZ::~filterDimZDimZ() {

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void filterDimZDimZ::init( trackData**    _tracks,
						   unsigned short size1,
						   unsigned short size2,
						   unsigned short size,
						   unsigned short localSize) {

	filterDimZ::init(_tracks, size, localSize);
	filterSize1 = size1;
	filterSize2 = size2;

}
