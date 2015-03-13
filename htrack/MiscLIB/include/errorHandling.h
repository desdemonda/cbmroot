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
///     - base class for errors
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-09-11 14:12:40 $
/// $Revision: 1.9 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _ERRORHANDLING_H
#define _ERRORHANDLING_H


#include "../include/projects.h"
#include <string>


//#define NOERRORMESSAGE	/**< If this is defined, no message would be written to the standard output. */
#undef NOERRORMESSAGE


/* **************************************************************
 * CLASS errorHandling							 				*
 * **************************************************************/

class errorHandling {

private:

	std::string errorSource;	/**< Variable to store a string representation for the project where the error occurs. */

public:

/**
 * Default constructor
 */

	errorHandling();

/**
 * Constructor
 * @param errorSource should be a string representation for the project
 *        where the error occurs
 */

	errorHandling(std::string errorSource);

/**
 * Destructor
 */

	virtual ~errorHandling();

/**
 * This method prints a message with a leading source specifier.
 * @param message should be a convincing string to handle the error
 */

	void printMsg(std::string message);

/**
 * This method displays an error message.
 */

	virtual void errorMsg() = 0;

};

#endif


#ifndef _CANNOTOPENFILEERROR_H
#define _CANNOTOPENFILEERROR_H

/**
 * CLASS cannotOpenFileError
 */

class cannotOpenFileError : public errorHandling {

private:

	std::string fileName;

public:

/**
 * Default constructor
 */

	cannotOpenFileError();

/**
 * Constructor
 * @param errorSource should be a string representation for the project
 *        where the error occurs
 * @param fileName is the name of the file
 */

	cannotOpenFileError(std::string errorSource, std::string fileName = "");
	cannotOpenFileError(std::string errorSource, const char* fileName);

/**
 * Destructor
 */

	virtual ~cannotOpenFileError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTCLOSEFILEERROR_H
#define _CANNOTCLOSEFILEERROR_H

/**
 * CLASS cannotCloseFileError
 */

class cannotCloseFileError : public errorHandling {

private:

	std::string fileName;

public:

/**
 * Default constructor
 */

	cannotCloseFileError();

/**
 * Constructor
 * @param errorSource should be a string representation for the project
 *        where the error occurs
 * @param fileName is the name of the file
 */

	cannotCloseFileError(std::string errorSource, std::string fileName = "");
	cannotCloseFileError(std::string errorSource, const char* fileName);

/**
 * Destructor
 */

	virtual ~cannotCloseFileError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTCREATEFILEERROR_H
#define _CANNOTCREATEFILEERROR_H

/**
 * CLASS cannotCreateFileError
 */

class cannotCreateFileError : public errorHandling {

private:

	std::string fileName;

public:

/**
 * Default constructor
 */

	cannotCreateFileError();

/**
 * Constructor
 * @param errorSource should be a string representation for the project
 *        where the error occurs
 * @param fileName is the name of the file
 */

	cannotCreateFileError(std::string errorSource, std::string fileName = "");
	cannotCreateFileError(std::string errorSource, const char* fileName);

/**
 * Destructor
 */

	virtual ~cannotCreateFileError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _UNEXPECTEDEOFFOUNDERROR_H
#define _UNEXPECTEDEOFFOUNDERROR_H

/**
 * CLASS unexpectedEofFoundError
 */

class unexpectedEofFoundError : public errorHandling {

private:

	std::string fileName;

public:

/**
 * Default constructor
 */

	unexpectedEofFoundError();

/**
 * Constructor
 * @param errorSource should be a string representation for the project
 *        where the error occurs
 * @param fileName is the name of the file
 */

	unexpectedEofFoundError(std::string errorSource, std::string fileName = "");
	unexpectedEofFoundError(std::string errorSource, const char* fileName);

/**
 * Destructor
 */

	virtual ~unexpectedEofFoundError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _NOFILENAMESPECIFIEDERROR_H
#define _NOFILENAMESPECIFIEDERROR_H

/**
 * CLASS noFilenameSpecifiedError
 */

class noFilenameSpecifiedError : public errorHandling {

public:

/**
 * Default constructor
 */

	noFilenameSpecifiedError();

/**
 * Constructor
 * @param errorSource should be a string representation for the project
 *        where the error occurs
 */

	noFilenameSpecifiedError(std::string errorSource);

/**
 * Destructor
 */

	virtual ~noFilenameSpecifiedError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _MEMORYALLOCATIONERROR_H
#define _MEMORYALLOCATIONERROR_H

/**
 * CLASS memoryAllocationError
 */

class memoryAllocationError : public errorHandling {

public:

/**
 * Default constructor
 */

	memoryAllocationError();

/**
 * Constructor
 * @param errorSource should be a string representation for the project
 *        where the error occurs
 */

	memoryAllocationError(std::string errorSource);

/**
 * Destructor
 */

	virtual ~memoryAllocationError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSHITSORTRACKSERROR_H
#define _CANNOTACCESSHITSORTRACKSERROR_H


/**
 * CLASS cannotAccessHitsOrTracksError
 */

class cannotAccessHitsOrTracksError : public errorHandling {

public:

/**
 * Default constructor
 */

	cannotAccessHitsOrTracksError();

/**
 * Constructor
 * @param errorSource should be a string representation for the project
 *        where the error occurs
 */

	cannotAccessHitsOrTracksError(std::string errorSource);

/**
 * Destructor
 */

	virtual ~cannotAccessHitsOrTracksError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSEVENTDATAERROR_H
#define _CANNOTACCESSEVENTDATAERROR_H


/**
 * CLASS cannotAccessEventDataError
 */

class cannotAccessEventDataError : public errorHandling {

public:

/**
 * Default constructor
 */

	cannotAccessEventDataError();

/**
 * Constructor
 * @param errorSource should be a string representation for the project
 *        where the error occurs
 */

	cannotAccessEventDataError(std::string errorSource);

/**
 * Destructor
 */

	virtual ~cannotAccessEventDataError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSHISTOGRAMDATAERROR_H
#define _CANNOTACCESSHISTOGRAMDATAERROR_H


/**
 * CLASS cannotAccessHistogramDataError
 */

class cannotAccessHistogramDataError : public errorHandling {

public:

/**
 * Default constructor
 */

	cannotAccessHistogramDataError();

/**
 * Constructor
 * @param errorSource should be a string representation for the project
 *        where the error occurs
 */

	cannotAccessHistogramDataError(std::string errorSource);

/**
 * Destructor
 */

	virtual ~cannotAccessHistogramDataError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSTRACKDATAERROR_H
#define _CANNOTACCESSTRACKDATAERROR_H


/**
 * CLASS cannotAccessTrackDataError
 */

class cannotAccessTrackDataError : public errorHandling {

public:

/**
 * Default constructor
 */

	cannotAccessTrackDataError();

/**
 * Constructor
 * @param errorSource should be a string representation for the project
 *        where the error occurs
 */

	cannotAccessTrackDataError(std::string errorSource);

/**
 * Destructor
 */

	virtual ~cannotAccessTrackDataError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSROOTMANAGERERROR_H
#define _CANNOTACCESSROOTMANAGERERROR_H


/**
 * CLASS cannotAccessRootManagerError
 */

class cannotAccessRootManagerError : public errorHandling {

public:

/**
 * Default constructor
 */

	cannotAccessRootManagerError();

/**
 * Constructor
 * @param errorSource should be a string representation for the project
 *        where the error occurs
 */

	cannotAccessRootManagerError(std::string errorSource);

/**
 * Destructor
 */

	virtual ~cannotAccessRootManagerError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _NODETECTORERROR_H
#define _NODETECTORERROR_H


/**
 * CLASS noDetectorError
 */

class noDetectorError : public errorHandling {

public:

/**
 * Default constructor
 */

	noDetectorError();

/**
 * Constructor
 * @param errorSource should be a string representation for the project
 *        where the error occurs
 */

	noDetectorError(std::string errorSource);

/**
 * Destructor
 */

	virtual ~noDetectorError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _OUTOFRANGEERROR_H
#define _OUTOFRANGEERROR_H


/**
 * CLASS outOfRangeError
 */

class outOfRangeError : public errorHandling {

public:

/**
 * Default constructor
 */

	outOfRangeError();

/**
 * Constructor
 * @param errorSource should be a string representation for the project
 *        where the error occurs
 */

	outOfRangeError(std::string errorSource);

/**
 * Destructor
 */

	virtual ~outOfRangeError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _ZERODIVISIONERROR_H
#define _ZERODIVISIONERROR_H


/**
 * CLASS zeroDivisionError
 */

class zeroDivisionError : public errorHandling {

public:

/**
 * Default constructor
 */

	zeroDivisionError();

/**
 * Constructor
 * @param errorSource should be a string representation for the project
 *        where the error occurs
 */

	zeroDivisionError(std::string errorSource);

/**
 * Destructor
 */

	virtual ~zeroDivisionError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _ZEROMODULOERROR_H
#define _ZEROMODULOERROR_H


/**
 * CLASS zeroModuloError
 */

class zeroModuloError : public errorHandling {

public:

/**
 * Default constructor
 */

	zeroModuloError();

/**
 * Constructor
 * @param errorSource should be a string representation for the project
 *        where the error occurs
 */

	zeroModuloError(std::string errorSource);

/**
 * Destructor
 */

	virtual ~zeroModuloError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSSTATIONERROR_H
#define _CANNOTACCESSSTATIONERROR_H


/**
 * CLASS cannotAccessStationError
 */

class cannotAccessStationError : public errorHandling {

public:

/**
 * Default constructor
 */

	cannotAccessStationError();

/**
 * Constructor
 * @param errorSource should be a string representation for the project
 *        where the error occurs
 */

	cannotAccessStationError(std::string errorSource);

/**
 * Destructor
 */

	virtual ~cannotAccessStationError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTREADEMPTYMEMORYERROR_H
#define _CANNOTREADEMPTYMEMORYERROR_H

/**
 * CLASS cannotReadEmptyMemoryError
 */

class cannotReadEmptyMemoryError : public errorHandling {

public:

/**
 * Default constructor
 */

	cannotReadEmptyMemoryError();

/**
 * Constructor
 * @param errorSource should be a string representation for the project
 *        where the error occurs
 */

	cannotReadEmptyMemoryError(std::string errorSource);

/**
 * Destructor
 */

	virtual ~cannotReadEmptyMemoryError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSHISTOGRAMSPACEERROR_H
#define _CANNOTACCESSHISTOGRAMSPACEERROR_H

/**
 * CLASS cannotAccessHistogramSpaceError
 */

class cannotAccessHistogramSpaceError : public errorHandling {

public:

/**
 * Default constructor
 */

	cannotAccessHistogramSpaceError();

/**
 * Constructor
 * @param errorSource should be a string representation for the project
 *        where the error occurs
 */

	cannotAccessHistogramSpaceError(std::string errorSource);

/**
 * Destructor
 */

	virtual ~cannotAccessHistogramSpaceError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSMAGNETICFIELDOBJECTERROR_H
#define _CANNOTACCESSMAGNETICFIELDOBJECTERROR_H

/**
 * CLASS cannotAccessMagneticFieldObjectError
 */

class cannotAccessMagneticFieldObjectError : public errorHandling {

public:

/**
 * Default constructor
 */

	cannotAccessMagneticFieldObjectError();

/**
 * Constructor
 * @param errorSource should be a string representation for the project
 *        where the error occurs
 */

	cannotAccessMagneticFieldObjectError(std::string errorSource);

/**
 * Destructor
 */

	virtual ~cannotAccessMagneticFieldObjectError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSTABLESERROR_H
#define _CANNOTACCESSTABLESERROR_H


/**
 * CLASS cannotAccessTablesError
 */

class cannotAccessTablesError : public errorHandling {

public:

/**
 * Default constructor
 */

	cannotAccessTablesError();

/**
 * Constructor
 * @param errorSource should be a string representation for the project
 *        where the error occurs
 */

	cannotAccessTablesError(std::string errorSource);

/**
 * Destructor
 */

	virtual ~cannotAccessTablesError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _FUNCTIONISDEPRECATEDERROR_H
#define _FUNCTIONISDEPRECATEDERROR_H


/**
 * CLASS functionIsDeprecatedError
 */

class functionIsDeprecatedError : public errorHandling {

public:

/**
 * Default constructor
 */

	functionIsDeprecatedError();

/**
 * Constructor
 * @param errorSource should be a string representation for the project
 *        where the error occurs
 */

	functionIsDeprecatedError(std::string errorSource);

/**
 * Destructor
 */

	virtual ~functionIsDeprecatedError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSLUTSERROR_H
#define _CANNOTACCESSLUTSERROR_H

/**
 * CLASS cannotAccessLutsError
 */

class cannotAccessLutsError : public errorHandling {

public:

/**
 * Default constructor
 */

	cannotAccessLutsError();

/**
 * Constructor
 * @param errorSource should be a string representation for the project
 *        where the error occurs
 */

	cannotAccessLutsError(std::string errorSource);

/**
 * Destructor
 */

	virtual ~cannotAccessLutsError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSDISTRIBUTIONMEMORYERROR_H
#define _CANNOTACCESSDISTRIBUTIONMEMORYERROR_H

/**
 * CLASS cannotAccessDistributionMemoryError
 */

class cannotAccessDistributionMemoryError : public errorHandling {

public:

/**
 * Default constructor
 */

	cannotAccessDistributionMemoryError();

/**
 * Constructor
 * @param errorSource should be a string representation for the project
 *        where the error occurs
 */

	cannotAccessDistributionMemoryError(std::string errorSource);

/**
 * Destructor
 */

	virtual ~cannotAccessDistributionMemoryError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSHISTOGRAMERROR_H
#define _CANNOTACCESSHISTOGRAMERROR_H

/**
 * CLASS cannotAccessHistogramError
 */

class cannotAccessHistogramError : public errorHandling {

public:

/**
 * Default constructor
 */

	cannotAccessHistogramError();

/**
 * Constructor
 * @param errorSource should be a string representation for the project
 *        where the error occurs
 */

	cannotAccessHistogramError(std::string errorSource);

/**
 * Destructor
 */

	virtual ~cannotAccessHistogramError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _WINDOWDIVISIONERROR_H
#define _WINDOWDIVISIONERROR_H

/**
 * CLASS windowDivisionError
 */

class windowDivisionError : public errorHandling {

public:

/**
 * Default constructor
 */

	windowDivisionError();

/**
 * Constructor
 * @param errorSource should be a string representation for the project
 *        where the error occurs
 */

	windowDivisionError(std::string errorSource);

/**
 * Destructor
 */

	virtual ~windowDivisionError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif
