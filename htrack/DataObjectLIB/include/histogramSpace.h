//////////////////////////////////////////////////////////////////////
/// (C)opyright 2005
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
///     - base class for holding the minimal and maximal analog
///       values for the histogram space
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:32:41 $
/// $Revision: 1.4 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _HISTOGRAMSPACE_H
#define _HISTOGRAMSPACE_H


#include "../../MiscLIB/include/coordinateSystem.h"


/* **************************************************************
 * CLASS histogramSpace							 				*
 * **************************************************************/

class histogramSpace {

private:

	double         minValues[DIMENSIONS];		/**< This variable stores the minimal values of the hough space in all dimensions. */
	double         maxValues[DIMENSIONS];		/**< This variable stores the maximal values of the hough space in all dimensions. */
	unsigned short stepValues[DIMENSIONS];		/**< This variable stores the quantization values of the hough space in all dimensions. */

public:

/**
 * Default constructor
 */

	histogramSpace();

/**
 * Constructor
 * @param dim1Min is the minimal value in the first dimension
 * @param dim1Max is the maximal value in the first dimension
 * @param dim1Step is the stepwidth of the first dimension
 * @param dim2Min is the minimal value in the second dimension
 * @param dim2Max is the maximal value in the second dimension
 * @param dim2Step is the stepwidth of the second dimension
 * @param dim3Min is the minimal value in the third dimension
 * @param dim3Max is the maximal value in the third dimension
 * @param dim3Step is the stepwidth of the third dimension
 * @param radiusMin is the minimal value in the radius dimension
 * @param radiusMax is the maximal value in the radius dimension
 * @param radiusStep is the stepwidth of the radius dimension
 * @param thetaMin is the minimal value in the theta dimension
 * @param thetaMax is the maximal value in the theta dimension
 * @param thetaStep is the stepwidth of the theta dimension
 * @param gammaMin is the minimal value in the gamma dimension
 * @param gammaMax is the maximal value in the gamma dimension
 * @param gammaStep is the stepwidth of the gamma dimension
 * @param dummy is here to differentiate the constructors for the coordinate systems
 */

	histogramSpace(const histogramSpace& value);
	histogramSpace(int dim1Step, double dim1Min, double dim1Max,
				   int dim2Step, double dim2Min, double dim2Max,
				   int dim3Step, double dim3Min, double dim3Max);
	histogramSpace(double radiusMin, double radiusMax, int radiusStep,
				   double thetaMin, double thetaMax, int thetaStep,
				   double gammaMin, double gammaMax, int gammaStep);

/**
 * Destructor
 */

	virtual ~histogramSpace();

/**
 * operator = ()
 */

	const histogramSpace& operator = (const histogramSpace& value);

/**
 * returns the minimal value in the dimension
 */

	double getMin(unsigned short dimension);

/**
 * returns the maximal value in the dimension
 */

	double getMax(unsigned short dimension);

/**
 * returns the step value in the dimension
 */

	unsigned short getStep(unsigned short dimension);

/**
 * returns the increment value in the dimension
 */

	double getIncr(unsigned short dimension);

/**
 * sets the minimal value in the dimension
 */

	void setMin(double set, unsigned short dimension);

/**
 * sets the maximal value in the dimension
 */

	void setMax(double set, unsigned short dimension);

/**
 * sets the step value in the dimension
 */

	void setStep(unsigned short set, unsigned short dimension);

/**
 * returns the analog value corresponding to the cell for the dimension
 */

	double getAnalogFromCell(unsigned short cell, unsigned short dimension);

/**
 * returns the cell corresponding to the analog value for the dimension
 */

	unsigned short getCellFromAnalog(double analog, unsigned short dimension);

};

#endif
