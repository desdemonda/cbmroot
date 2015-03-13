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
///     - abstract class for all filter methods for
///		 maxMorphSearch
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:35:34 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _FILTERDIMX_H
#define _FILTERDIMX_H


#include "../../DataRootObjectLIB/include/histogramData.h"
#include "../../MiscLIB/include/bitArray.h"
#include "filterBasicStyle.h"


/* **************************************************************
 * CLASS filterDimX			 									*
 * **************************************************************/

class filterDimX {

 private:

  filterDimX(const filterDimX&);
  filterDimX& operator=(const filterDimX&);


protected:

	histogramData**   histogram;			/**< Object for accessing the histogram. */
	bitArray*         filterMem;			/**< Array to store the input values for filtering. */
	filterBasicStyle* baseFilter;			/**< Object to build the filtering strategy. */
	unsigned short    filterSize;			/**< Variable to store the complete size of the area of the filter. */
	unsigned short    filterLocalSize;		/**< Variable to store the complete size of the area of the local filter. */

public:

/**
 * Default constructor
 */

	filterDimX();

/**
 * Constructor
 * @param histogram is an object for accessing the histogram
 * @param size is the complete size of the area of the filter
 * @param localSize is the complete size of the area of the local filter
 */

	filterDimX( histogramData** histogram,
				unsigned short  size,
				unsigned short  localSize);

/**
 * Default destructor
 */

	virtual ~filterDimX();

/**
 * This method initializes the object.
 * @param histogram is an object for accessing the histogram
 * @param size is the complete size of the area of the filter
 * @param localSize is the complete size of the area of the local filter
 */

	void init( histogramData**  histogram,
			   unsigned short   size,
			   unsigned short   localSize);

/**
 * This method implements the filter.
 */

	virtual void filter() = 0;

};

#endif
