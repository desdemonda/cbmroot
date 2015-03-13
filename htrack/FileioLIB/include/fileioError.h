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
///     - base class for errors occuring during file access
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:34:44 $
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _FILEIOERROR_H
#define _FILEIOERROR_H


#include "../../MiscLIB/include/errorHandling.h"


/* **************************************************************
 * CLASS fileioError							 				*
 * **************************************************************/

class fileioError : public errorHandling {

public:

/**
 * Default constructor
 */

	fileioError();

/**
 * Destructor
 */

	virtual ~fileioError();

};

#endif


#ifndef _TOOLESSCMDSERROR_H
#define _TOOLESSCMDSERROR_H

/**
 * CLASS tooLessCmdsError
 */

class tooLessCmdsError : public fileioError {

public:

/**
 * Default constructor
 */

	tooLessCmdsError();

/**
 * Destructor
 */

	virtual ~tooLessCmdsError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _TOOLESSDATAINFILEERROR_H
#define _TOOLESSDATAINFILEERROR_H

/**
 * CLASS tooLessDataError
 */

class tooLessDataInFileError : public fileioError {

public:

/**
 * Default constructor
 */

	tooLessDataInFileError();

/**
 * Destructor
 */

	virtual ~tooLessDataInFileError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _DATAPTRERROR_H
#define _DATAPTRERROR_H

/**
 * CLASS dataPtrError
 */

class dataPtrError : public fileioError {

public:

/**
 * Default constructor
 */

	dataPtrError();

/**
 * Destructor
 */

	virtual ~dataPtrError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _DATASTRINGNULLERROR_H
#define _DATASTRINGNULLERROR_H

/**
 * CLASS dataStringNullError
 */

class dataStringNullError : public fileioError {

public:

/**
 * Default constructor
 */

	dataStringNullError();

/**
 * Destructor
 */

	virtual ~dataStringNullError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _WRONGITERATORERROR_H
#define _ERONGITERATORERROR_H

/**
 * CLASS wrongIteratorError
 */

class wrongIteratorError : public fileioError {

public:

/**
 * Default constructor
 */

	wrongIteratorError();

/**
 * Destructor
 */

	virtual ~wrongIteratorError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif
