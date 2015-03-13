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
///     - class for evaluating the histogram
///     - class computes the tracks
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:35:34 $
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _MAXMORPHSEARCH_H
#define _MAXMORPHSEARCH_H


#include "histogramTransformation.h"
#include "filterDimX.h"
#include "filterDimZ.h"
#include "filterDimXDimX.h"


/* **************************************************************
 * CLASS maxMorphSearch		 									*
 * **************************************************************/

class maxMorphSearch : public histogramTransformation {

 private:

  maxMorphSearch(const maxMorphSearch&);
  maxMorphSearch& operator=(const maxMorphSearch&);

protected:

	filterDimX*     firstFilter;								/**< Object to filter the histogram layer at first. */
	filterDimX*     secondFilter;								/**< Object to filter the histogram layer at second. */
	filterDimXDimX* combiFilter;								/**< Object to filter the histogram layer instead of firstFilter and secondFilter. */
	filterDimZ*     thirdFilter;								/**< Object to filter the tracks coming from the filtered histogram layer. */

/**
 * This method filters the peakclasses in a speciallized manner.
 */

	void filteringHistogramLayer();

/**
 * This method filters the tracks in a speciallized manner
 * again.
 * @param terminal is a buffer to place the process information
 */

	void filteringHistogram(std::streambuf* terminal = NULL);

public:

/**
 * Default constructor
 */

	maxMorphSearch();

/**
 * Constructor
 * @param histogram is an object for accessing the histogram
 * @param tracks is the object to access the computed tracks
 * @param ratings is the object to access the tables for the ratings
 * @param firstFilterGeometry defines the geometry of the first applied filter
 * @param firstFilterArithmetic defines the arithmetic which is used in the first applied filter
 * @param secondFilterGeometry defines the geometry of the second applied filter
 * @param secondFilterArithmetic defines the arithmetic which is used in the second applied filter
 * @param firstFilterDim1ClearRadius is the region of interest for the first filter in the first dimension
 * @param firstFilterDim2ClearRadius is the region of interest for the first filter in the second dimension
 * @param secondFilterDim1ClearRadius is the region of interest for the second filter in the first dimension
 * @param secondFilterDim2ClearRadius is the region of interest for the second filter in the second dimension
 * @param secondFilterDim3ClearRadius is the region of interest for the second filter in the third dimension
 */

	maxMorphSearch( histogramData** histogram, trackData** tracks,
					tables** ratings,
					unsigned short firstFilterGeometry, unsigned short firstFilterArithmetic,
					unsigned short secondFilterGeometry, unsigned short secondFilterArithmetic,
					unsigned short firstFilterDim1ClearRadius,
					unsigned short firstFilterDim2ClearRadius,
					unsigned short secondFilterDim1ClearRadius,
					unsigned short secondFilterDim2ClearRadius,
					unsigned short secondFilterDim3ClearRadius);

/**
 * Destructor
 */

	virtual ~maxMorphSearch();

/**
 * This method initializes the object.
 * @param histogram is an object for accessing the histogram
 * @param tracks is the object to access the computed tracks
 * @param ratings is the object to access the tables for the ratings
 * @param firstFilterGeometry defines the geometry of the first applied filter
 * @param firstFilterArithmetic defines the arithmetic which is used in the first applied filter
 * @param secondFilterGeometry defines the geometry of the second applied filter
 * @param secondFilterArithmetic defines the arithmetic which is used in the second applied filter
 * @param firstFilterDim1ClearRadius is the region of interest for the first filter in the first dimension
 * @param firstFilterDim2ClearRadius is the region of interest for the first filter in the second dimension
 * @param secondFilterDim1ClearRadius is the region of interest for the second filter in the first dimension
 * @param secondFilterDim2ClearRadius is the region of interest for the second filter in the second dimension
 * @param secondFilterDim3ClearRadius is the region of interest for the second filter in the third dimension
 */

void init( histogramData** histogram, trackData** tracks,
		   tables** ratings,
		   unsigned short firstFilterGeometry, unsigned short firstFilterArithmetic,
		   unsigned short secondFilterGeometry, unsigned short secondFilterArithmetic,
		   unsigned short firstFilterDim1ClearRadius,
		   unsigned short firstFilterDim2ClearRadius,
		   unsigned short secondFilterDim1ClearRadius,
		   unsigned short secondFilterDim2ClearRadius,
		   unsigned short secondFilterDim3ClearRadius);

};

#endif
