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


#include "../include/filterDimXDimXDimX.h"


/****************************************************************
 * Default constructor											*
 ****************************************************************/

filterDimXDimXDimX::filterDimXDimXDimX() 
  : filterDimXDimX(),
    filterSize3(0)
{

  //	filterSize3 = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

filterDimXDimXDimX::filterDimXDimXDimX( histogramData** _histogram,
					unsigned short  size1,
					unsigned short  size2,
					unsigned short  size3,
					unsigned short  size,
					unsigned short  localSize) 
  : filterDimXDimX(_histogram, size1, size2, size, localSize),
    filterSize3(size3)
{

  //	filterSize3 = size3;

}

/****************************************************************
 * Default destructor											*
 ****************************************************************/

filterDimXDimXDimX::~filterDimXDimXDimX() {

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void filterDimXDimXDimX::init( histogramData** _histogram,
			       unsigned short  size1,
			       unsigned short  size2,
			       unsigned short  size3,
			       unsigned short  size,
			       unsigned short  localSize) 
{

	filterDimXDimX::init(_histogram, size1, size2, size, localSize);
	filterSize3 = size3;

}
