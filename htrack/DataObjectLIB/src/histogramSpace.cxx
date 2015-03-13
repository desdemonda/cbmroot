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
//     - base class for holding the minimal and maximal analog
//       values for the histogram space
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-10-10 13:47:05 $
// $Revision: 1.5 $
//
// *******************************************************************/


#include "../include/dataObjectError.h"
#include "../include/histogramSpace.h"


/****************************************************************
 * CLASS histogramSpace							 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

histogramSpace::histogramSpace() {

	minValues[DIM1]  = 0;
	minValues[DIM2]  = 0;
	minValues[DIM3]  = 0;
	maxValues[DIM1]  = 0;
	maxValues[DIM2]  = 0;
	maxValues[DIM3]  = 0;
	stepValues[DIM1] = 0;
	stepValues[DIM2] = 0;
	stepValues[DIM3] = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

histogramSpace::histogramSpace(const histogramSpace& value) {

	this->minValues[DIM1]  = value.minValues[DIM1];
	this->minValues[DIM2]  = value.minValues[DIM2];
	this->minValues[DIM3]  = value.minValues[DIM3];
	this->maxValues[DIM1]  = value.maxValues[DIM1];
	this->maxValues[DIM2]  = value.maxValues[DIM2];
	this->maxValues[DIM3]  = value.maxValues[DIM3];
	this->stepValues[DIM1] = value.stepValues[DIM1];
	this->stepValues[DIM2] = value.stepValues[DIM2];
	this->stepValues[DIM3] = value.stepValues[DIM3];

}
histogramSpace::histogramSpace(int dim1Step, double dim1Min, double dim1Max,
				   int dim2Step, double dim2Min, double dim2Max,
				   int dim3Step, double dim3Min, double dim3Max) {

	minValues[DIM1]  = dim1Min;
	minValues[DIM2]  = dim2Min;
	minValues[DIM3]  = dim3Min;
	maxValues[DIM1]  = dim1Max;
	maxValues[DIM2]  = dim2Max;
	maxValues[DIM3]  = dim3Max;
	stepValues[DIM1] = dim1Step;
	stepValues[DIM2] = dim2Step;
	stepValues[DIM3] = dim3Step;

}
histogramSpace::histogramSpace(double radiusMin, double radiusMax, int radiusStep,
				   double thetaMin, double thetaMax, int thetaStep,
				   double gammaMin, double gammaMax, int gammaStep) {

	minValues[HRADIUS]  = radiusMin;
	minValues[HTHETA]   = thetaMin;
	minValues[HGAMMA]   = gammaMin;
	maxValues[HRADIUS]  = radiusMax;
	maxValues[HTHETA]   = thetaMax;
	maxValues[HGAMMA]   = gammaMax;
	stepValues[HRADIUS] = radiusStep;
	stepValues[HTHETA]  = thetaStep;
	stepValues[HGAMMA]  = gammaStep;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

histogramSpace::~histogramSpace() {

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const histogramSpace& histogramSpace::operator = (const histogramSpace& value) {

	this->minValues[DIM1]  = value.minValues[DIM1];
	this->minValues[DIM2]  = value.minValues[DIM2];
	this->minValues[DIM3]  = value.minValues[DIM3];
	this->maxValues[DIM1]  = value.maxValues[DIM1];
	this->maxValues[DIM2]  = value.maxValues[DIM2];
	this->maxValues[DIM3]  = value.maxValues[DIM3];
	this->stepValues[DIM1] = value.stepValues[DIM1];
	this->stepValues[DIM2] = value.stepValues[DIM2];
	this->stepValues[DIM3] = value.stepValues[DIM3];

	return *this;

}

/****************************************************************
 * returns the minimal value in the dimension					*
 ****************************************************************/

double histogramSpace::getMin(unsigned short dimension) {

	double returnValue = 0;

	if (dimension < DIMENSIONS) {

		returnValue = minValues[dimension];

	}
	else
		throw notExistingDimensionError(dimension, DIMENSIONS);

	return returnValue;

}

/****************************************************************
 * returns the maximal value in the dimension					*
 ****************************************************************/

double histogramSpace::getMax(unsigned short dimension) {

	double returnValue = 0;

	if (dimension < DIMENSIONS) {

		returnValue = maxValues[dimension];

	}
	else
		throw notExistingDimensionError(dimension, DIMENSIONS);

	return returnValue;

}

/****************************************************************
 * returns the step value in the dimension						*
 ****************************************************************/

unsigned short histogramSpace::getStep(unsigned short dimension) {

	unsigned short returnValue = 0;

	if (dimension < DIMENSIONS) {

		returnValue = stepValues[dimension];

	}
	else
		throw notExistingDimensionError(dimension, DIMENSIONS);

	return returnValue;

}

/****************************************************************
 * returns the increment value in the dimension					*
 ****************************************************************/

double histogramSpace::getIncr(unsigned short dimension) {

	double returnValue = 0;

	if (dimension < DIMENSIONS) {

		returnValue = (maxValues[dimension] - minValues[dimension]) / (stepValues[dimension] - 1);

	}
	else
		throw notExistingDimensionError(dimension, DIMENSIONS);

	return returnValue;

}

/****************************************************************
 * sets the minimal value in the dimension						*
 ****************************************************************/

void histogramSpace::setMin(double set, unsigned short dimension) {

	if (dimension < DIMENSIONS) {

		minValues[dimension] = set;

	}
	else
		throw notExistingDimensionError(dimension, DIMENSIONS);

}

/****************************************************************
 * sets the maximal value in the dimension						*
 ****************************************************************/

void histogramSpace::setMax(double set, unsigned short dimension) {

	if (dimension < DIMENSIONS) {

		maxValues[dimension] = set;

	}
	else
		throw notExistingDimensionError(dimension, DIMENSIONS);

}

/****************************************************************
 * sets the step value in the dimension							*
 ****************************************************************/

void histogramSpace::setStep(unsigned short set, unsigned short dimension) {

	if (dimension < DIMENSIONS) {

		stepValues[dimension] = set;

	}
	else
		throw notExistingDimensionError(dimension, DIMENSIONS);

}

/****************************************************************
 * returns the analog value corresponding to the cell for the	*
 * dimension													*
 ****************************************************************/

double histogramSpace::getAnalogFromCell(unsigned short cell, unsigned short dimension) {

	double returnValue = 0;

	if (dimension < DIMENSIONS) {

		if (cell < getStep(dimension))
			returnValue = cell * getIncr(dimension) + getMin(dimension);
		else
			throw tooBigCellError(cell, getStep(dimension));

	}
	else
		throw notExistingDimensionError(dimension, DIMENSIONS);

	return returnValue;

}

/****************************************************************
 * returns the cell corresponding to the analog value for the dimension
 ****************************************************************/

unsigned short histogramSpace::getCellFromAnalog(double analog, unsigned short dimension) {

	unsigned short returnValue = 0;

	if (dimension < DIMENSIONS) {

		if ((analog >= getMin(dimension)) && (analog <= getMax(dimension)))
			returnValue = (unsigned short)(((analog - getMin(dimension)) / getIncr(dimension)) + 0.5);
		else
			throw analogValueIsNotInRangeError(analog, getMin(dimension), getMax(dimension));

	}
	else
		throw notExistingDimensionError(dimension, DIMENSIONS);

	return returnValue;

}
