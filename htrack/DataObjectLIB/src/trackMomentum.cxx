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
//     - class for structuring the momentum information
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:32:44 $
// $Revision: 1.4 $
//
// *******************************************************************/


#include "../include/dataObjectError.h"
#include "../include/trackMomentum.h"
#include <math.h>


#define sqr(a)  ((a) * (a)) 


/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackMomentum::trackMomentum() {

	for (unsigned short i = 0; i < DIMENSIONS; i++)
		dim[i] = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackMomentum::trackMomentum(const trackMomentum& value) {

	for (unsigned short i = 0; i < DIMENSIONS; i++)
		this->dim[i] = value.dim[i];

}
trackMomentum::trackMomentum(double dim1, double dim2, double dim3) {

	set(dim1, DIM1);
	set(dim2, DIM2);
	set(dim3, DIM3);

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackMomentum::~trackMomentum() {

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const trackMomentum& trackMomentum::operator = (const trackMomentum& value) {

	for (unsigned short i = 0; i < DIMENSIONS; i++)
		this->dim[i] = value.dim[i];

	return *this;

}

/****************************************************************
 * This method converts the object into a						*
 * trackFrameworkMomentum object.								*
 ****************************************************************/

trackMomentum::operator trackFrameworkMomentum() {

	trackFrameworkMomentum returnValue;

	returnValue.setTx(get(PX) / get(PZ));
	returnValue.setTy(get(PY) / get(PZ));
	returnValue.setQp(sqrt(sqr(get(PX)) + sqr(get(PY)) + sqr(get(PZ))));

	return returnValue;

}

/****************************************************************
 * returns the value in the dimension							*
 ****************************************************************/

double trackMomentum::get(unsigned short dimension) {

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

void trackMomentum::set(double _set, unsigned short dimension) {

	if (dimension < DIMENSIONS)
		dim[dimension] = _set;
	else
		throw notExistingDimensionError(dimension, DIMENSIONS);

}

