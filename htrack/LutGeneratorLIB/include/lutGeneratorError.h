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
/// $Date: 2008-11-21 14:01:49 $
/// $Revision: 1.9 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _LUTGENERATORERROR_H
#define _LUTGENERATORERROR_H


#include "../../MiscLIB/include/errorHandling.h"


/* **************************************************************
 * CLASS lutGeneratorError						 				*
 * **************************************************************/

class lutGeneratorError : public errorHandling {

public:

/**
 * Default constructor
 */

	lutGeneratorError();

/**
 * Destructor
 */

	virtual ~lutGeneratorError();

};

#endif


#ifndef _NOHITERROR_H
#define _NOHITERROR_H

/**
 * CLASS noHitError
 */

class noHitError : public lutGeneratorError {

public:

/**
 * Default constructor
 */

	noHitError();

/**
 * Destructor
 */

	virtual ~noHitError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _MAGNETICFIELDFACTORCANNOTBEZEROERROR_H
#define _MAGNETICFIELDFACTORCANNOTBEZEROERROR_H

/**
 * CLASS magneticFieldFactorCannotBeZero
 */

class magneticFieldFactorCannotBeZero : public lutGeneratorError {

public:

/**
 * Default constructor
 */

	magneticFieldFactorCannotBeZero();

/**
 * Destructor
 */

	virtual ~magneticFieldFactorCannotBeZero();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _TOOBIGINDEXFORDIGITALHITLUTERROR_H
#define _TOOBIGINDEXFORDIGITALHITLUTERROR_H

/**
 * CLASS tooBigIndexForDigitalHitLutError
 */

class tooBigIndexForDigitalHitLutError : public lutGeneratorError {

private:

	unsigned long index;
	unsigned long numberOfEntries;

public:

/**
 * Default constructor
 */

	tooBigIndexForDigitalHitLutError();

/**
 * Constructor
 */

	tooBigIndexForDigitalHitLutError(unsigned long index, unsigned long numberOfEntries);

/**
 * Destructor
 */

	virtual ~tooBigIndexForDigitalHitLutError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _TOOBIGINDEXFORPRELUTERROR_H
#define _TOOBIGINDEXFORPRELUTERROR_H

/**
 * CLASS tooBigIndexForPrelutError
 */

class tooBigIndexForPrelutError : public lutGeneratorError {

private:

	unsigned long index;
	unsigned long numberOfEntries;

public:

/**
 * Default constructor
 */

	tooBigIndexForPrelutError();

/**
 * Constructor
 */

	tooBigIndexForPrelutError(unsigned long index, unsigned long numberOfEntries);

/**
 * Destructor
 */

	virtual ~tooBigIndexForPrelutError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _TOOBIGINDEXFORLUTERROR_H
#define _TOOBIGINDEXFORLUTERROR_H

/**
 * CLASS tooBigIndexForLutError
 */

class tooBigIndexForLutError : public lutGeneratorError {

private:

	unsigned long index;
	unsigned long numberOfEntries;

public:

/**
 * Default constructor
 */

	tooBigIndexForLutError();

/**
 * Constructor
 */

	tooBigIndexForLutError(unsigned long index, unsigned long numberOfEntries);

/**
 * Destructor
 */

	virtual ~tooBigIndexForLutError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSDIGITALHITLUTERROR_H
#define _CANNOTACCESSDIGITALHITLUTERROR_H

/**
 * CLASS cannotAccessDigitalHitLutError
 */

class cannotAccessDigitalHitLutError : public lutGeneratorError {

public:

/**
 * Default constructor
 */

	cannotAccessDigitalHitLutError();

/**
 * Destructor
 */

	virtual ~cannotAccessDigitalHitLutError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSPRELUTERROR_H
#define _CANNOTACCESSPRELUTERROR_H

/**
 * CLASS cannotAccessPrelutError
 */

class cannotAccessPrelutError : public lutGeneratorError {

public:

/**
 * Default constructor
 */

	cannotAccessPrelutError();

/**
 * Destructor
 */

	virtual ~cannotAccessPrelutError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSLUTERROR_H
#define _CANNOTACCESSLUTERROR_H

/**
 * CLASS cannotAccessLutError
 */

class cannotAccessLutError : public lutGeneratorError {

public:

/**
 * Default constructor
 */

	cannotAccessLutError();

/**
 * Destructor
 */

	virtual ~cannotAccessLutError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTGETDEFINITIONOFUNDEFINEDOBJECTERROR_H
#define _CANNOTGETDEFINITIONOFUNDEFINEDOBJECTERROR_H

/**
 * CLASS cannotGetDefinitionOfUndefinedObjectError
 */

class cannotGetDefinitionOfUndefinedObjectError : public lutGeneratorError {

public:

/**
 * Default constructor
 */

	cannotGetDefinitionOfUndefinedObjectError();

/**
 * Destructor
 */

	virtual ~cannotGetDefinitionOfUndefinedObjectError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTGETBORDEROFUNDEFINEDOBJECTERROR_H
#define _CANNOTGETBORDEROFUNDEFINEDOBJECTERROR_H

/**
 * CLASS cannotGetBorderOfUndefinedObjectError
 */

class cannotGetBorderOfUndefinedObjectError : public lutGeneratorError {

public:

/**
 * Default constructor
 */

	cannotGetBorderOfUndefinedObjectError();

/**
 * Destructor
 */

	virtual ~cannotGetBorderOfUndefinedObjectError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTCLEARBORDEROFUNDEFINEDOBJECTERROR_H
#define _CANNOTCLEARBORDEROFUNDEFINEDOBJECTERROR_H

/**
 * CLASS cannotClearBorderOfUndefinedObjectError
 */

class cannotClearBorderOfUndefinedObjectError : public lutGeneratorError {

public:

/**
 * Default constructor
 */

	cannotClearBorderOfUndefinedObjectError();

/**
 * Destructor
 */

	virtual ~cannotClearBorderOfUndefinedObjectError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTSETPRELUTRANGEOFUNDEFINEDOBJECTERROR_H
#define _CANNOTSETPRELUTRANGEOFUNDEFINEDOBJECTERROR_H

/**
 * CLASS cannotSetPrelutRangeOfUndefinedObjectError
 */

class cannotSetPrelutRangeOfUndefinedObjectError : public lutGeneratorError {

public:

/**
 * Default constructor
 */

	cannotSetPrelutRangeOfUndefinedObjectError();

/**
 * Destructor
 */

	virtual ~cannotSetPrelutRangeOfUndefinedObjectError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTGETMAGNETICFIELDOFUNDEFINEDOBJECTERROR_H
#define _CANNOTGETMAGNETICFIELDOFUNDEFINEDOBJECTERROR_H

/**
 * CLASS cannotGetMagneticFieldOfUndefinedObjectError
 */

class cannotGetMagneticFieldOfUndefinedObjectError : public lutGeneratorError {

public:

/**
 * Default constructor
 */

	cannotGetMagneticFieldOfUndefinedObjectError();

/**
 * Destructor
 */

	virtual ~cannotGetMagneticFieldOfUndefinedObjectError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTGETMAGNETICFIELDFACTOROFUNDEFINEDOBJECTERROR_H
#define _CANNOTGETMAGNETICFIELDFACTOROFUNDEFINEDOBJECTERROR_H

/**
 * CLASS cannotGetMagneticFieldFactorOfUndefinedObjectError
 */

class cannotGetMagneticFieldFactorOfUndefinedObjectError : public lutGeneratorError {

public:

/**
 * Default constructor
 */

	cannotGetMagneticFieldFactorOfUndefinedObjectError();

/**
 * Destructor
 */

	virtual ~cannotGetMagneticFieldFactorOfUndefinedObjectError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTSETMAGNETICFIELDOFUNDEFINEDOBJECTERROR_H
#define _CANNOTSETMAGNETICFIELDOFUNDEFINEDOBJECTERROR_H

/**
 * CLASS cannotSetMagneticFieldOfUndefinedObjectError
 */

class cannotSetMagneticFieldOfUndefinedObjectError : public lutGeneratorError {

public:

/**
 * Default constructor
 */

	cannotSetMagneticFieldOfUndefinedObjectError();

/**
 * Destructor
 */

	virtual ~cannotSetMagneticFieldOfUndefinedObjectError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTSETMAGNETICFIELDFACTOROFUNDEFINEDOBJECTERROR_H
#define _CANNOTSETMAGNETICFIELDFACTOROFUNDEFINEDOBJECTERROR_H

/**
 * CLASS cannotSetMagneticFieldFactorOfUndefinedObjectError
 */

class cannotSetMagneticFieldFactorOfUndefinedObjectError : public lutGeneratorError {

public:

/**
 * Default constructor
 */

	cannotSetMagneticFieldFactorOfUndefinedObjectError();

/**
 * Destructor
 */

	virtual ~cannotSetMagneticFieldFactorOfUndefinedObjectError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSCORRECTIONSOFUNDEFINEDOBJECTERROR_H
#define _CANNOTACCESSCORRECTIONSOFUNDEFINEDOBJECTERROR_H

/**
 * CLASS cannotAccessCorrectionsOfUndefinedObjectError
 */

class cannotAccessCorrectionsOfUndefinedObjectError : public lutGeneratorError {

public:

/**
 * Default constructor
 */

	cannotAccessCorrectionsOfUndefinedObjectError();

/**
 * Destructor
 */

	virtual ~cannotAccessCorrectionsOfUndefinedObjectError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTEVALUATEBORDEROFUNDEFINEDOBJECTERROR_H
#define _CANNOTEVALUATEBORDEROFUNDEFINEDOBJECTERROR_H

/**
 * CLASS cannotEvaluateBorderOfUndefinedObjectError
 */

class cannotEvaluateBorderOfUndefinedObjectError : public lutGeneratorError {

public:

/**
 * Default constructor
 */

	cannotEvaluateBorderOfUndefinedObjectError();

/**
 * Destructor
 */

	virtual ~cannotEvaluateBorderOfUndefinedObjectError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _WRONGLUTTYPEFORUSEDFUNCTIONALITYDETECTEDERROR_H
#define _WRONGLUTTYPEFORUSEDFUNCTIONALITYDETECTEDERROR_H

/**
 * CLASS wrongLutTypeForUsedFunctionalityDetectedError
 */

class wrongLutTypeForUsedFunctionalityDetectedError : public lutGeneratorError {

public:

/**
 * Default constructor
 */

	wrongLutTypeForUsedFunctionalityDetectedError();

/**
 * Destructor
 */

	virtual ~wrongLutTypeForUsedFunctionalityDetectedError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSRUNGEKUTTALISTERROR_H
#define _CANNOTACCESSRUNGEKUTTALISTERROR_H

/**
 * CLASS cannotAccessRungeKuttaListError
 */

class cannotAccessRungeKuttaListError : public lutGeneratorError {

public:

/**
 * Default constructor
 */

	cannotAccessRungeKuttaListError();

/**
 * Destructor
 */

	virtual ~cannotAccessRungeKuttaListError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSRUNGEKUTTAINVERTEDLISTERROR_H
#define _CANNOTACCESSRUNGEKUTTAINVERTEDLISTERROR_H

/**
 * CLASS cannotAccessRungeKuttaInvertedListError
 */

class cannotAccessRungeKuttaInvertedListError : public lutGeneratorError {

public:

/**
 * Default constructor
 */

	cannotAccessRungeKuttaInvertedListError();

/**
 * Destructor
 */

	virtual ~cannotAccessRungeKuttaInvertedListError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _WRONGENTRYINLISTERROR_H
#define _WRONGENTRYINLISTERROR_H

/**
 * CLASS wrongEntryInListError
 */

class wrongEntryInListError : public lutGeneratorError {

public:

/**
 * Default constructor
 */

	wrongEntryInListError();

/**
 * Destructor
 */

	virtual ~wrongEntryInListError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif
