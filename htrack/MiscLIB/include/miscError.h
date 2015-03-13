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
///     - class for errors occuring while doing some misc operations
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:36:55 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _MISCERROR_H
#define _MISCERROR_H


#include "../include/errorHandling.h"


/* **************************************************************
 * CLASS miscError								 				*
 * **************************************************************/

class miscError : public errorHandling {

public:

/**
 * Default constructor
 */

	miscError();

/**
 * Default destructor
 */

	virtual ~miscError();

};

#endif


#ifndef _USTOCVALUETOOBIGERROR_H
#define _USTOCVALUETOOBIGERROR_H

/**
 * CLASS ustocValueTooBigError
 */

class ustocValueTooBigError : public miscError {

public:

/**
 * Default constructor
 */

	ustocValueTooBigError();

/**
 * Destructor
 */

	virtual ~ustocValueTooBigError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CTOUSVALUETOOBIGERROR_H
#define _CTOUSVALUETOOBIGERROR_H

/**
 * CLASS ctousValueTooBigError
 */

class ctousValueTooBigError : public miscError {

public:

/**
 * Default constructor
 */

	ctousValueTooBigError();

/**
 * Destructor
 */

	virtual ~ctousValueTooBigError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif
