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
//     - base class for holding the track parameter of the transformation
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:32:44 $
// $Revision: 1.4 $
//
// *******************************************************************/


#include "../include/dataObjectError.h"
#include "../include/trackParameter.h"


/****************************************************************
 * CLASS trackParameter							 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackParameter::trackParameter() {

	for (unsigned short i = 0; i < DIMENSIONS; i++)
		dim[i] = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackParameter::trackParameter(const trackParameter& value) {

	for (unsigned short i = 0; i < DIMENSIONS; i++)
		this->dim[i] = value.dim[i];

}
trackParameter::trackParameter(double dim1, double dim2, double dim3) {

	set(dim1, DIM1);
	set(dim2, DIM2);
	set(dim3, DIM3);

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackParameter::~trackParameter() {

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const trackParameter& trackParameter::operator = (const trackParameter& value) {

	for (unsigned short i = 0; i < DIMENSIONS; i++)
		this->dim[i] = value.dim[i];

	return *this;

}

/****************************************************************
 * returns the value in the dimension							*
 ****************************************************************/

double trackParameter::get(unsigned short dimension) {

	double returnValue = 0;

	if (dimension < DIMENSIONS)
		returnValue = dim[dimension];
	else
		throw notExistingDimensionError(dimension, DIMENSIONS);

	return returnValue;

}

/****************************************************************
 * sets the value in the dimension								*
 ****************************************************************/

void trackParameter::set(double _set, unsigned short dimension) {

	if (dimension < DIMENSIONS)
		dim[dimension] = _set;
	else
		throw notExistingDimensionError(dimension, DIMENSIONS);

}
