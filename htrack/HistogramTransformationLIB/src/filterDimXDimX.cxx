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


#include "../include/filterDimXDimX.h"


/****************************************************************
 * Default constructor											*
 ****************************************************************/

filterDimXDimX::filterDimXDimX() 
  : filterDimX(),
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

filterDimXDimX::filterDimXDimX( histogramData** _histogram,
				unsigned short  size1,
				unsigned short  size2,
				unsigned short  size,
				unsigned short  localSize) 
  : filterDimX(_histogram, size, localSize),
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

filterDimXDimX::~filterDimXDimX() {

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void filterDimXDimX::init( histogramData** _histogram,
						   unsigned short  size1,
						   unsigned short  size2,
						   unsigned short  size,
						   unsigned short  localSize) {

	filterDimX::init(_histogram, size, localSize);
	filterSize1 = size1;
	filterSize2 = size2;

}
