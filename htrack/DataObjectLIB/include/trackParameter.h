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
///     - base class for holding the track parameter of the transformation
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:32:42 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _TRACKPARAMETER_H
#define _TRACKPARAMETER_H


#include "../../MiscLIB/include/coordinateSystem.h"


/* **************************************************************
 * CLASS trackParameter							 				*
 * **************************************************************/

class trackParameter {

protected:

	double dim[DIMENSIONS];		/**< Variable to store the parametervector. */

public:

/**
 * Default constructor
 */

	trackParameter();

/**
 * Constructor
 * @param dim1 is the value in the first dimension
 * @param dim2 is the value in the second dimension
 * @param dim3 is the value in the third dimension
 */

	trackParameter(const trackParameter& value);
	trackParameter(double dim1, double dim2, double dim3);

/**
 * Destructor
 */

	virtual ~trackParameter();

/**
 * operator = ()
 */

	const trackParameter& operator = (const trackParameter& value);

/**
 * returns the value in the dimension
 */

	double get(unsigned short dimension);

/**
 * sets the value in the dimension
 */

	void set(double set, unsigned short dimension);

};

#endif
