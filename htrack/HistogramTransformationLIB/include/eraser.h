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
///     CAUTION: This class cannot be backward compatible with Version 4.0.
///              This is because of the histogram reduction from 3D to 2D. So
///              the filter had to be reduced in the corresponding way. And
///              the reduction of the filter cannot be done bittrue.
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:35:33 $
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _ERASER_H
#define _ERASER_H


#include "histogramTransformation.h"


/* **************************************************************
 * CLASS eraser				 									*
 * **************************************************************/

class eraser : public histogramTransformation {

protected:

	unsigned short firstFilterNeighborhoodDim1ClearRadius;	/**< Variable to store the size of the region of interest for the first filter in the first dimension. */
	unsigned short firstFilterNeighborhoodDim2ClearRadius;	/**< Variable to store the size of the region of interest for the first filter in the second dimension.  */
	unsigned short secondFilterNeighborhoodDim3ClearRadius;	/**<  Variable to store the size of the region of interest for the second filter in the third dimension. */

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

/**
 * This method clears the histogramsPeaks data in a specific
 * area.
 * @param dim1 is the value of the coordinate in the first dimension
 * @param dim2 is the value of the coordinate in the second dimension
 */

	void clearPeakNeighborhood(unsigned short dim1, unsigned short dim2);

public:

/**
 * Default constructor
 */

	eraser();

/**
 * Constructor
 * @param histogram is an object for accessing the histogram
 * @param tracks is the object to access the computed tracks
 * @param ratings is the object to access the tables for the ratings
 * @param dim1ClearRadius is the region of interest for the first filter in the first dimension
 * @param dim2ClearRadius is the region of interest for the first filter in the second dimension
 * @param dim3ClearRadius is the region of interest for the second filter in the third dimension
 */

	eraser(histogramData** histogram, trackData** tracks, tables** ratings, unsigned short dim1ClearRadius, unsigned short dim2ClearRadius, unsigned short dim3ClearRadius);

/**
 * Destructor
 */

	virtual ~eraser();

/**
 * This method initializes the object.
 * @param histogram is an object for accessing the histogram
 * @param tracks is the object to access the computed tracks
 * @param ratings is the object to access the tables for the ratings
 * @param dim1ClearRadius is the region of interest for the first filter in the first dimension
 * @param dim2ClearRadius is the region of interest for the first filter in the second dimension
 * @param dim3ClearRadius is the region of interest for the second filter in the third dimension
 */

	void init(histogramData** histogram, trackData** tracks, tables** ratings, unsigned short dim1ClearRadius, unsigned short dim2ClearRadius, unsigned short dim3ClearRadius);

};

#endif
