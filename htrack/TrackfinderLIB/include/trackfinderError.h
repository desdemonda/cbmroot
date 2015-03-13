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
///     - base class for errors occuring during trackfinding
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:39:20 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _TRACKFINDERERROR_H
#define _TRACKFINDERERROR_H


#include "../../MiscLIB/include/errorHandling.h"


/* **************************************************************
 * CLASS trackfinderError						 				*
 * **************************************************************/

class trackfinderError : public errorHandling {

public:

/**
 * Default constructor
 */

	trackfinderError();

/**
 * Destructor
 */

	virtual ~trackfinderError();

};

#endif


#ifndef _CANNOTACCESSHOUGHMEMMANAGERERROR_H
#define _CANNOTACCESSHOUGHMEMMANAGERERROR_H


/**
 * CLASS cannotAccessHoughMemManagerError
 */

class cannotAccessHoughMemManagerError : public trackfinderError {

public:

/**
 * Default constructor
 */

	cannotAccessHoughMemManagerError();

/**
 * Destructor
 */

	virtual ~cannotAccessHoughMemManagerError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSHISTOGRAMMEMMANAGERERROR_H
#define _CANNOTACCESSHISTOGRAMMEMMANAGERERROR_H


/**
 * CLASS cannotAccessHistogramMemManagerError
 */

class cannotAccessHistogramMemManagerError : public trackfinderError {

public:

/**
 * Default constructor
 */

	cannotAccessHistogramMemManagerError();

/**
 * Destructor
 */

	virtual ~cannotAccessHistogramMemManagerError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSHOUGHTRANSFORMERROR_H
#define _CANNOTACCESSHOUGHTRANSFORMERROR_H


/**
 * CLASS cannotAccessHoughTransformError
 */

class cannotAccessHoughTransformError : public trackfinderError {

public:

/**
 * Default constructor
 */

	cannotAccessHoughTransformError();

/**
 * Destructor
 */

	virtual ~cannotAccessHoughTransformError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSHISTOGRAMTRANSFORMERROR_H
#define _CANNOTACCESSHISTOGRAMTRANSFORMERROR_H


/**
 * CLASS cannotAccessHistogramTransformError
 */

class cannotAccessHistogramTransformError : public trackfinderError {

public:

/**
 * Default constructor
 */

	cannotAccessHistogramTransformError();

/**
 * Destructor
 */

	virtual ~cannotAccessHistogramTransformError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSANALYSERERROR_H
#define _CANNOTACCESSANALYSERERROR_H


/**
 * CLASS cannotAccessAnalyserError
 */

class cannotAccessAnalyserError : public trackfinderError {

public:

/**
 * Default constructor
 */

	cannotAccessAnalyserError();

/**
 * Destructor
 */

	virtual ~cannotAccessAnalyserError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _IMPOSSIBLEFEATUREWITHOUTANALYSERERROR_H
#define _IMPOSSIBLEFEATUREWITHOUTANALYSERERROR_H


/**
 * CLASS impossibleFeatureWithoutAnalyserError
 */

class impossibleFeatureWithoutAnalyserError : public trackfinderError {

public:

/**
 * Default constructor
 */

	impossibleFeatureWithoutAnalyserError();

/**
 * Destructor
 */

	virtual ~impossibleFeatureWithoutAnalyserError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _FEATURENOTENABLEDINANALYSERERROR_H
#define _FEATURENOTENABLEDINANALYSERERROR_H


/**
 * CLASS featureNotEnabledInAnalyserError
 */

class featureNotEnabledInAnalyserError : public trackfinderError {

public:

/**
 * Default constructor
 */

	featureNotEnabledInAnalyserError();

/**
 * Destructor
 */

	virtual ~featureNotEnabledInAnalyserError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif
