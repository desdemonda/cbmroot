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
///     - class for errors occuring while doing some framework operations
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:38:40 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _FRAMEWORKERROR_H
#define _FRAMEWORKERROR_H


#include "../../MiscLIB/include/errorHandling.h"
#include <string>


/* **************************************************************
 * CLASS frameworkError							 				*
 * **************************************************************/

class frameworkError : public errorHandling {

public:

/**
 * Default constructor
 */

	frameworkError();

/**
 * Default destructor
 */

	virtual ~frameworkError();

};

#endif


#ifndef _FOLDERNAMENOTFOUNDERROR_H
#define _FOLDERNAMENOTFOUNDERROR_H

/**
 * CLASS folderNameNotFoundError
 */

class folderNameNotFoundError : public frameworkError {

private:

	std::string folderName;

public:

/**
 * Default constructor
 */

	folderNameNotFoundError();

/**
 * Constructor
 */

	folderNameNotFoundError(std::string actualFolderName);

/**
 * Destructor
 */

	virtual ~folderNameNotFoundError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _LISTOFFOLDERSNOTFOUNDERROR_H
#define _LISTOFFOLDERSNOTFOUNDERROR_H

/**
 * CLASS listOfFoldersNotFoundError
 */

class listOfFoldersNotFoundError : public frameworkError {

private:

	std::string folderName;

public:

/**
 * Default constructor
 */

	listOfFoldersNotFoundError();

/**
 * Constructor
 */

	listOfFoldersNotFoundError(std::string actualFolderName);

/**
 * Destructor
 */

	virtual ~listOfFoldersNotFoundError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _LISTOFBRANCHESNOTFOUNDERROR_H
#define _LISTOFBRANCHESNOTFOUNDERROR_H

/**
 * CLASS listOfBranchesNotFoundError
 */

class listOfBranchesNotFoundError : public frameworkError {

private:

	std::string branchName;

public:

/**
 * Default constructor
 */

	listOfBranchesNotFoundError();

/**
 * Constructor
 */

	listOfBranchesNotFoundError(std::string actualBranchName);

/**
 * Destructor
 */

	virtual ~listOfBranchesNotFoundError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTMAKELISTITERATORERROR_H
#define _CANNOTMAKELISTITERATORERROR_H

/**
 * CLASS cannotMakeListIteratorError
 */

class cannotMakeListIteratorError : public frameworkError {

public:

/**
 * Default constructor
 */

	cannotMakeListIteratorError();

/**
 * Destructor
 */

	virtual ~cannotMakeListIteratorError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTADDFOLDERERROR_H
#define _CANNOTADDFOLDERERROR_H

/**
 * CLASS cannotAddFolderError
 */

class cannotAddFolderError : public frameworkError {

private:

	std::string folderName;

public:

/**
 * Default constructor
 */

	cannotAddFolderError();

/**
 * Constructor
 */

	cannotAddFolderError(std::string actualFolderName);

/**
 * Destructor
 */

	virtual ~cannotAddFolderError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _TREENAMENOTFOUNDERROR_H
#define _TREENAMENOTFOUNDERROR_H

/**
 * CLASS treeNameNotFoundError
 */

class treeNameNotFoundError : public frameworkError {

private:

	std::string treeName;

public:

/**
 * Default constructor
 */

	treeNameNotFoundError();

/**
 * Constructor
 */

	treeNameNotFoundError(std::string actualTreeName);

/**
 * Destructor
 */

	virtual ~treeNameNotFoundError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSBRANCHESERROR_H
#define _CANNOTACCESSBRANCHESERROR_H

/**
 * CLASS cannotAccessBranchesError
 */

class cannotAccessBranchesError : public frameworkError {

public:

/**
 * Default constructor
 */

	cannotAccessBranchesError();

/**
 * Destructor
 */

	~cannotAccessBranchesError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _NOOUTPUTTREEERROR_H
#define _NOOUTPUTTREEERROR_H

/**
 * CLASS noOutputTreeError
 */

class noOutputTreeError : public frameworkError {

public:

/**
 * Default constructor
 */

	noOutputTreeError();

/**
 * Destructor
 */

	~noOutputTreeError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _NOOUTPUTFILEERROR_H
#define _NOOUTPUTFILEERROR_H

/**
 * CLASS noOutputFileError
 */

class noOutputFileError : public frameworkError {

public:

/**
 * Default constructor
 */

	noOutputFileError();

/**
 * Destructor
 */

	~noOutputFileError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSHITSERROR_H
#define _CANNOTACCESSHITSERROR_H


/**
 * CLASS cannotAccessHitsError
 */

class cannotAccessHitsError : public frameworkError {

public:

/**
 * Default constructor
 */

	cannotAccessHitsError();

/**
 * Destructor
 */

	virtual ~cannotAccessHitsError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTACCESSPOINTSERROR_H
#define _CANNOTACCESSPOINTSERROR_H


/**
 * CLASS cannotAccessPointError
 */

class cannotAccessPointError : public frameworkError {

public:

/**
 * Default constructor
 */

	cannotAccessPointError();

/**
 * Destructor
 */

	virtual ~cannotAccessPointError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _CANNOTSMEARERROR_H
#define _CANNOTSMEARERROR_H


/**
 * CLASS cannotSmearError
 */

class cannotSmearError : public frameworkError {

public:

/**
 * Default constructor
 */

	cannotSmearError();

/**
 * Destructor
 */

	virtual ~cannotSmearError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _NOINPUTFILEERROR_H
#define _NOINPUTFILEERROR_H

/**
 * CLASS noInputFileError
 */

class noInputFileError : public frameworkError {

public:

/**
 * Default constructor
 */

	noInputFileError();

/**
 * Destructor
 */

	~noInputFileError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _NOOUTPUTFOLDERERROR_H
#define _NOOUTPUTFOLDERERROR_H

/**
 * CLASS noOutputFolderError
 */

class noOutputFolderError : public frameworkError {

public:

/**
 * Default constructor
 */

	noOutputFolderError();

/**
 * Destructor
 */

	~noOutputFolderError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _NOTREEERROR_H
#define _NOTREEERROR_H

/**
 * CLASS noTreeError
 */

class noTreeError : public frameworkError {

public:

/**
 * Default constructor
 */

	noTreeError();

/**
 * Destructor
 */

	~noTreeError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif


#ifndef _NODETECTORTYPEERROR_H
#define _NODETECTORTYPEERROR_H

/**
 * CLASS noDetectorTypeError
 */

class noDetectorTypeError : public frameworkError {

public:

/**
 * Default constructor
 */

	noDetectorTypeError();

/**
 * Destructor
 */

	~noDetectorTypeError();

/**
 * This method displays an error message.
 */

	void errorMsg();

};

#endif
