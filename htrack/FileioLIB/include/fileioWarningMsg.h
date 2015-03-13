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
///     - base class for warnings occuring during file access
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-10-10 13:47:23 $
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _FILEIOWARNINGMSG_H
#define _FILEIOWARNINGMSG_H


#include <string>


//#define NOFILEIOWARNINGMESSAGE	/**< If this is defined, no message would be written to the standard output. */
#undef NOFILEIOWARNINGMESSAGE


/* **************************************************************
 * CLASS fileioWarningMsg						 				*
 * **************************************************************/

class fileioWarningMsg {

public:

/**
 * Default constructor
 */

	fileioWarningMsg();

/**
 * Destructor
 */

	virtual ~fileioWarningMsg();

/**
 * This method prints a message.
 */

	void printMsg(std::string message);

/**
 * This method displays a warning message.
 */

	virtual void warningMsg() = 0;

};

#endif


#ifndef _MISSINGCMDVALWARNINGMSG_H
#define _MISSINGCMDVALWARNINGMSG_H

/**
 * CLASS missingCmdValWarningMsg
 */

class missingCmdValWarningMsg : public fileioWarningMsg {

public:

/**
 * Default constructor
 */

	missingCmdValWarningMsg();

/**
 * Destructor
 */

	virtual ~missingCmdValWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _MISSINGCMDSPECWARNINGMSG_H
#define _MISSINGCMDSPECWARNINGMSG_H

/**
 * CLASS missingCmdSpecWarningMsg
 */

class missingCmdSpecWarningMsg : public fileioWarningMsg {

public:

/**
 * Default constructor
 */

	missingCmdSpecWarningMsg();

/**
 * Destructor
 */

	virtual ~missingCmdSpecWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _UNKNOWNCMDSPECVALPAIRWARNINGMSG_H
#define _UNKNOWNCMDSPECVALPAIRWARNINGMSG_H

/**
 * CLASS unknownCmdSpecValPairWarningMsg
 */

class unknownCmdSpecValPairWarningMsg : public fileioWarningMsg {

public:

/**
 * Default constructor
 */

	unknownCmdSpecValPairWarningMsg();

/**
 * Destructor
 */

	virtual ~unknownCmdSpecValPairWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _CANNOTOPENFILEWARNINGMSG_H
#define _CANNOTOPENFILEWARNINGMSG_H

/**
 * CLASS cannotOpenFileWarningMsg
 */

class cannotOpenFileWarningMsg : public fileioWarningMsg {

public:

/**
 * Default constructor
 */

	cannotOpenFileWarningMsg();

/**
 * Destructor
 */

	virtual ~cannotOpenFileWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _WRITEDEFAULTFILEWARNINGMSG_H
#define _WRITEDEFAULTFILEWARNINGMSG_H


/**
 * CLASS writeDefaultFileWarningMsg
 */

class writeDefaultFileWarningMsg : public fileioWarningMsg {

private:

	std::string fileName;

public:

/**
 * Default constructor
 */

	writeDefaultFileWarningMsg();

/**
 * Constructor
 */

	writeDefaultFileWarningMsg(std::string name);

/**
 * Destructor
 */

	virtual ~writeDefaultFileWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _DATACOMMENTWARNINGMSG_H
#define _DATACOMMENTWARNINGMSG_H

/**
 * CLASS dataCommentWarningMsg
 */

class dataCommentWarningMsg : public fileioWarningMsg {

public:

/**
 * Default constructor
 */

	dataCommentWarningMsg();

/**
 * Destructor
 */

	virtual ~dataCommentWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TOOMUCHDATAWARNINGMSG_H
#define _TOOMUCHDATAWARNINGMSG_H

/**
 * CLASS tooMuchDataWarningMsg
 */

class tooMuchDataWarningMsg : public fileioWarningMsg {

public:

/**
 * Default constructor
 */

	tooMuchDataWarningMsg();

/**
 * Destructor
 */

	virtual ~tooMuchDataWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _LOCKOUTOFRANGEWARNINGMSG_H
#define _LOCKOUTOFRANGEWARNINGMSG_H

/**
 * CLASS lockOutOfRangeWarningMsg
 */

class lockOutOfRangeWarningMsg : public fileioWarningMsg {

private:

	unsigned int lockId;
	unsigned int numberOfLocks;	

public:

/**
 * Default constructor
 */

	lockOutOfRangeWarningMsg();

/**
 * Constructor
 */

	lockOutOfRangeWarningMsg(unsigned int lockId, unsigned int numberOfLocks);

/**
 * Destructor
 */

	virtual ~lockOutOfRangeWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif
