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
///     - base class for warnings occuring during output result operations
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:37:33 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _OUTPUTRESULTWARNINGMSG_H
#define _OUTPUTRESULTWARNINGMSG_H


#include <string>


//#define NOOUTPUTRESULTWARNINGMESSAGE	/**< If this is defined, no message would be written to the standard output. */
#undef NOOUTPUTRESULTWARNINGMESSAGE


/* **************************************************************
 * CLASS outputResultWarningMsg					 				*
 * **************************************************************/

class outputResultWarningMsg {

public:

/**
 * Default constructor
 */

	outputResultWarningMsg();

/**
 * Destructor
 */

	virtual ~outputResultWarningMsg();

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


#ifndef _TRACKPOINTERISNOTSUPPORTEDWARNINGMSG_H
#define _TRACKPOINTERISNOTSUPPORTEDWARNINGMSG_H

/**
 * CLASS trackPointerIsNotSupportedWarningMsg
 */

class trackPointerIsNotSupportedWarningMsg : public outputResultWarningMsg {

public:

/**
 * Default constructor
 */

	trackPointerIsNotSupportedWarningMsg();

/**
 * Destructor
 */

	virtual ~trackPointerIsNotSupportedWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif


#ifndef _TRACKPOINTERISDISABLEDWARNINGMSG_H
#define _TRACKPOINTERISDISABLEDWARNINGMSG_H

/**
 * CLASS trackPointerIsDisabledWarningMsg
 */

class trackPointerIsDisabledWarningMsg : public outputResultWarningMsg {

public:

/**
 * Default constructor
 */

	trackPointerIsDisabledWarningMsg();

/**
 * Destructor
 */

	virtual ~trackPointerIsDisabledWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif
