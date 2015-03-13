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
///     - base class for errors occuring during HistogramTransform
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-10-07 10:37:46 $
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _HISTOGRAMTRANSFORMATIONERROR_H
#define _HISTOGRAMTRANSFORMATIONERROR_H


#include "../../MiscLIB/include/errorHandling.h"


/* **************************************************************
 * CLASS histogramTransformationError				 			*
 * **************************************************************/

class histogramTransformationError : public errorHandling {

public:

/**
 * Default constructor
 */

	histogramTransformationError();

/**
 * Destructor
 */

	virtual ~histogramTransformationError();

};

#endif


#ifndef _CANNOTACCESSELEMENTSTOFILTERERROR_H
#define _CANNOTACCESSELEMENTSTOFILTERERROR_H

/**
 * CLASS cannotAccessElementsToFilterError
 */

class cannotAccessElementsToFilterError : public histogramTransformationError {

public:

/**
 * Default constructor
 */

	cannotAccessElementsToFilterError();

/**
 * Destructor
 */

	virtual ~cannotAccessElementsToFilterError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _MIDDLEELEMENTISOUTOFRANGEERROR_H
#define _MIDDLEELEMENTISOUTOFRANGEERROR_H

/**
 * CLASS middleElementIsOutOfRangeError
 */

class middleElementIsOutOfRangeError : public histogramTransformationError {

public:

/**
 * Default constructor
 */

	middleElementIsOutOfRangeError();

/**
 * Destructor
 */

	virtual ~middleElementIsOutOfRangeError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSFILTERERROR_H
#define _CANNOTACCESSFILTERERROR_H

/**
 * CLASS cannotAccessFilterError
 */

class cannotAccessFilterError : public histogramTransformationError {

public:

/**
 * Default constructor
 */

	cannotAccessFilterError();

/**
 * Destructor
 */

	virtual ~cannotAccessFilterError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSFILTERMEMORYERROR_H
#define _CANNOTACCESSFILTERMEMORYERROR_H

/**
 * CLASS cannotAccessFilterMemoryError
 */

class cannotAccessFilterMemoryError : public histogramTransformationError {

public:

/**
 * Default constructor
 */

	cannotAccessFilterMemoryError();

/**
 * Destructor
 */

	virtual ~cannotAccessFilterMemoryError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSPEAKHISTOGRAMERROR_H
#define _CANNOTACCESSPEAKHISTOGRAMERROR_H

/**
 * CLASS cannotAccessPeakHistogramError
 */

class cannotAccessPeakHistogramError : public histogramTransformationError {

public:

/**
 * Default constructor
 */

	cannotAccessPeakHistogramError();

/**
 * Destructor
 */

	virtual ~cannotAccessPeakHistogramError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSTRACKSERROR_H
#define _CANNOTACCESSTRACKSERROR_H

/**
 * CLASS cannotAccessTracksError
 */

class cannotAccessTracksError : public histogramTransformationError {

public:

/**
 * Default constructor
 */

	cannotAccessTracksError();

/**
 * Destructor
 */

	virtual ~cannotAccessTracksError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _WRONGPEAKFINDINGGEOMETRYERROR_H
#define _WRONGPEAKFINDINGGEOMETRYERROR_H

/**
 * CLASS wrongPeakfindingGeometryError
 */

class wrongPeakfindingGeometryError : public histogramTransformationError {

public:

/**
 * Default constructor
 */

	wrongPeakfindingGeometryError();

/**
 * Destructor
 */

	virtual ~wrongPeakfindingGeometryError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif
