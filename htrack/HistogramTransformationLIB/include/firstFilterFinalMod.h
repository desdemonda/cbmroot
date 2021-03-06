//////////////////////////////////////////////////////////////////////
/// (C)opyright 2004
/// 
/// Institute of Computer Science V
/// Prof. M�nner
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
/// |     X copy      |
/// |  XXXX           | 2 * { S[i=0;f1/2-1] (f2+1)/2 } + 1 = (f1/2)*(f2+1)+1 different localFiltersize
/// |  XXXX           |
/// ---------------------------------------------------------------------------------------------------
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:35:34 $
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _FIRSTFILTERFINALMOD_H
#define _FIRSTFILTERFINALMOD_H


#include "filterDimXDimX.h"


/* **************************************************************
 * CLASS firstFilterFinal	 									*
 * **************************************************************/

class firstFilterFinalMod : public filterDimXDimX {

 private:

  firstFilterFinalMod(const firstFilterFinalMod&);
  firstFilterFinalMod& operator=(const firstFilterFinalMod&);

protected:

	unsigned short filterResultSize;	/**< Variable to store the size of the memory for storing the computed filter-values. */
	bitArray*      filterResultMem;		/**< Array to store the computed filter-values until copying back into the histogram. */

public:

/**
 * Default constructor
 */

	firstFilterFinalMod();

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

	firstFilterFinalMod( histogramData** histogram,
						 unsigned short  filterArithmetic,
						 unsigned short  size1,
						 unsigned short  size2,
						 unsigned short  localSize1,
				 		 unsigned short  localSize2,
						 bitArray maximumClass);

/**
 * Destructor
 */

	virtual ~firstFilterFinalMod();

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
