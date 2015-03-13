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
///     - class for structuring the momentum information
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:32:42 $
/// $Revision: 1.4 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _TRACKMOMENTUM_H
#define _TRACKMOMENTUM_H


#include "../../MiscLIB/include/coordinateSystem.h"
#include "trackFrameworkMomentum.h"


class trackFrameworkMomentum;


/* **************************************************************
 * CLASS trackMomentum							 				*
 * **************************************************************/

class trackMomentum {

protected:

	double dim[DIMENSIONS];		/**< Variable to store the momentumvector. */

public:

/**
 * Default constructor
 */

	trackMomentum();

/**
 * Constructor
 * @param dim1 is the value in the first dimension
 * @param dim2 is the value in the second dimension
 * @param dim3 is the value in the third dimension
 */

	trackMomentum(const trackMomentum& value);
	trackMomentum(double dim1, double dim2, double dim3);

/**
 * Destructor
 */

	virtual ~trackMomentum();

/**
 * operator = ()
 */

	const trackMomentum& operator = (const trackMomentum& value);

/**
 * This method converts the object into a trackFrameworkMomentum object.
 */

	operator trackFrameworkMomentum();

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
