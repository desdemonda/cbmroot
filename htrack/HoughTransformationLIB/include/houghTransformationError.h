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
///     - derived from errorHandling
///     - base class for errors occuring during HoughTransform
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:35:54 $
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _HOUGHTRANSFORMATIONERROR_H
#define _HOUGHTRANSFORMATIONERROR_H


#include "../../MiscLIB/include/errorHandling.h"


/* **************************************************************
 * CLASS houghTransformationError				 				*
 * **************************************************************/

class houghTransformationError : public errorHandling {

public:

/**
 * Default constructor
 */

	houghTransformationError();

/**
 * Destructor
 */

	virtual ~houghTransformationError();

};

#endif


#ifndef _NOGOODTRACKFOUNDERROR_H
#define _NOGOODTRACKFOUNDERROR_H

/**
 * CLASS noGoodTrackFoundError
 */

class noGoodTrackFoundError : public houghTransformationError {

public:

/**
 * Default constructor
 */

	noGoodTrackFoundError();

/**
 * Destructor
 */

	virtual ~noGoodTrackFoundError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif
