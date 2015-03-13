//////////////////////////////////////////////////////////////////////
/// (C)opyright 2004
/// 
/// Institute of Computer Science V
/// Prof. Männer
/// University of Mannheim, Germany
/// 
/// *******************************************************************
/// 
/// Designer(s):   Steinle
/// 
/// *******************************************************************
/// 
/// Project:     Trackfinder for CBM-Project at GSI-Darmstadt, Germany
/// 
/// *******************************************************************
/// 
/// Description:
///
///   class:
///     - class for evaluating the basic filter method for
///		 maxMorphSearch
///
/// | FILTERSTRUCTURE | FILTERSIZE
/// ---------------------------------------------------------------------------------------------------
/// |     XXXX        |
/// |     XXXX        |
/// |  XXXXXXX        |
/// |  XXXX           | 2 * { S[i=0;f1/2-1] (f2+1)/2 } + f2 = (f1/2)*(f2+1)+f2
/// |  XXXX           |
/// ---------------------------------------------------------------------------------------------------
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:35:33 $
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _FILTERDIM1DIM2MOD_H
#define _FILTERDIM1DIM2MOD_H


#include "filterDimXDimX.h"


/* **************************************************************
 * CLASS filterDim1Dim2Mod	 									*
 * **************************************************************/

class filterDim1Dim2Mod : public filterDimXDimX {

 private:

  filterDim1Dim2Mod(const filterDim1Dim2Mod&);
  filterDim1Dim2Mod& operator=(const filterDim1Dim2Mod&);

protected:

	bitArray***  filterTempMem;			/**< Array for easier constructing the geometry of the filter. */
	bitArray**   memValues;				/**< Array to store the original values of the histogram for the next cell in the filter. */
	bool**       allocaterMarker;		/**< Array to store if the actual pointer of filterTempMem is a pointer or an allocated cell. */

public:

/**
 * Default constructor
 */

	filterDim1Dim2Mod();

/**
 * Constructor
 * @param histogram is an object for accessing the histogram
 * @param filterArithmetic defines the arithmetic which is used in the applied filter
 * @param size1 is the filtersize in the first dimension
 * @param size2 is the filtersize in the second dimension
 * @param localSize1 is the local filtersize in the first dimension
 * @param localSize2 is the local filtersize in the second dimension
 * @param maximumClass is the maximal class which can occur while filtering
 */

	filterDim1Dim2Mod( histogramData** histogram,
					   unsigned short  filterArithmetic,
					   unsigned short  size1,
					   unsigned short  size2,
					   unsigned short  localSize1,
					   unsigned short  localSize2,
					   bitArray maximumClass);

/**
 * Destructor
 */

	virtual ~filterDim1Dim2Mod();

/**
 * This method initializes the object.
 * @param histogram is an object for accessing the histogram
 * @param filterArithmetic defines the arithmetic which is used in the applied filter
 * @param size1 is the filtersize in the first dimension
 * @param size2 is the filtersize in the second dimension
 * @param localSize1 is the local filtersize in the first dimension
 * @param localSize2 is the local filtersize in the second dimension
 * @param maximumClass is the maximal class which can occur while filtering
 */

	void init( histogramData** histogram,
			   unsigned short  filterArithmetic,
			   unsigned short  size1,
			   unsigned short  size2,
			   unsigned short  localSize1,
			   unsigned short  localSize2,
			   bitArray maximumClass);

/**
 * This method implements the filter.
 */

	void filter();

};

#endif
