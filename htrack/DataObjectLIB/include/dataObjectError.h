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
///     - class for errors occuring while interacting with the luts
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:32:40 $
/// $Revision: 1.5 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _DATAOBJECTERROR_H
#define _DATAOBJECTERROR_H


#include "../../MiscLIB/include/errorHandling.h"


/* **************************************************************
 * CLASS dataObjectError						 				*
 * **************************************************************/

class dataObjectError : public errorHandling {

public:

/**
 * Default constructor
 */

	dataObjectError();

/**
 * Destructor
 */

	virtual ~dataObjectError();

};

#endif




#ifndef _BORDERIDRULEERROR_H
#define _BORDERIDRULEERROR_H

/**
 * CLASS borderIdRuleError
 */

class borderIdRuleError : public dataObjectError {

public:

/**
 * Default constructor
 */

	borderIdRuleError();

/**
 * Destructor
 */

	virtual ~borderIdRuleError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _BORDERIDCONVERSIONERROR_H
#define _BORDERIDCONVERSIONERROR_H

/**
 * CLASS borderIdConversionError
 */

class borderIdConversionError : public dataObjectError {

public:

/**
 * Default constructor
 */

	borderIdConversionError();

/**
 * Destructor
 */

	virtual ~borderIdConversionError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _TOOBIGSTARTIDERROR_H
#define _TOOBIGSTARTIDERROR_H

/**
 * CLASS tooBigStartIdError
 */

class tooBigStartIdError : public dataObjectError {

public:

/**
 * Default constructor
 */

	tooBigStartIdError();

/**
 * Destructor
 */

	virtual ~tooBigStartIdError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTFINDMIDDLESEPARATORERROR_H
#define _CANNOTFINDMIDDLESEPARATORERROR_H

/**
 * CLASS cannotFindMiddleSeparatorError
 */

class cannotFindMiddleSeparatorError : public dataObjectError {

private:

	std::string searchedString;
	std::string middleSeparator;

public:

/**
 * Default constructor
 */

	cannotFindMiddleSeparatorError();

/**
 * Constructor
 */

	cannotFindMiddleSeparatorError(std::string searchedString, std::string middleSeparator);

/**
 * Destructor
 */

	virtual ~cannotFindMiddleSeparatorError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _NOTEXISTINGDIMENSIONERROR_H
#define _NOTEXISTINGDIMENSIONERROR_H

/**
 * CLASS notExistingDimensionError
 */

class notExistingDimensionError : public dataObjectError {

private:

	unsigned short actualDimension;
	unsigned short numberOfDimensions;

public:

/**
 * Default constructor
 */

	notExistingDimensionError();

/**
 * Constructor
 */

	notExistingDimensionError(unsigned short actualDimension, unsigned short numberOfDimensions);

/**
 * Destructor
 */

	virtual ~notExistingDimensionError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _ANALOGVALUEISNOTINRANGEERROR_H
#define _ANALOGVALUEISNOTINRANGEERROR_H

/**
 * CLASS analogValueIsNotInRangeError
 */

class analogValueIsNotInRangeError : public dataObjectError {

private:

	double actualValue;
	double minValue;
	double maxValue;

public:

/**
 * Default constructor
 */

	analogValueIsNotInRangeError();

/**
 * Constructor
 */

	analogValueIsNotInRangeError(double actualValue, double minValue, double maxValue);

/**
 * Destructor
 */

	virtual ~analogValueIsNotInRangeError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _TOOBIGCELLERROR_H
#define _TOOBIGCELLERROR_H

/**
 * CLASS tooBigCellError
 */

class tooBigCellError : public dataObjectError {

private:

	unsigned short actualCell;
	unsigned short numberOfCells;

public:

/**
 * Default constructor
 */

	tooBigCellError();

/**
 * Constructor
 */

	tooBigCellError(unsigned short actualCell, unsigned short numberOfCells);

/**
 * Destructor
 */

	virtual ~tooBigCellError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _RESULTPOINTERISNOTACCESSIBLEERROR_H
#define _RESULTPOINTERISNOTACCESSIBLEERROR_H

/**
 * CLASS resultPointerIsNotAccessibleError
 */

class resultPointerIsNotAccessibleError : public dataObjectError {

public:

/**
 * Default constructor
 */

	resultPointerIsNotAccessibleError();

/**
 * Destructor
 */

	virtual ~resultPointerIsNotAccessibleError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif
