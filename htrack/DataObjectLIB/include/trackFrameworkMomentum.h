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
///     - base class for holding the framework's track parameters
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:32:42 $
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _TRACKFRAMEWORKMOMENTUM_H
#define _TRACKFRAMEWORKMOMENTUM_H


#include "trackMomentum.h"


class trackMomentum;


/* **************************************************************
 * CLASS trackFrameworkMomentum					 				*
 * **************************************************************/

class trackFrameworkMomentum {

protected:

	double tx;		/**< Variable to store the momentum ratio between px and pz. */
	double ty;		/**< Variable to store the momentum ratio between py and pz. */
	double qp;		/**< Variable to store the charge over the momentum. */

public:

/**
 * Default constructor
 */

	trackFrameworkMomentum();

/**
 * Constructor
 * @param dim1 is the value in the first dimension
 * @param dim2 is the value in the second dimension
 * @param dim3 is the value in the third dimension
 */

	trackFrameworkMomentum(const trackFrameworkMomentum& value);
	trackFrameworkMomentum(double tx, double ty, double qp);

/**
 * Destructor
 */

	virtual ~trackFrameworkMomentum();

/**
 * operator = ()
 */

	const trackFrameworkMomentum& operator = (const trackFrameworkMomentum& value);

/**
 * This method converts the object into a trackMomentum object.
 */

	operator trackMomentum();

/**
 * returns the value for tx
 */

	double getTx();

/**
 * returns the value for ty
 */

	double getTy();

/**
 * returns the value for qp
 */

	double getQp();

/**
 * sets the value for tx
 */

	void setTx(double set);

/**
 * sets the value for ty
 */

	void setTy(double set);

/**
 * sets the value for qp
 */

	void setQp(double set);

};

#endif
