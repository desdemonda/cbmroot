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
///     - base class for warnings occuring during misc operations
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:36:55 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _MISCWARNINGMSG_H
#define _MISCWARNINGMSG_H


#include <string>


//#define NOMISCWARNINGMESSAGE	/**< If this is defined, no message would be written to the standard output. */
#undef NOMISCWARNINGMESSAGE


/* **************************************************************
 * CLASS miscWarningMsg							 				*
 * **************************************************************/

class miscWarningMsg {

public:

/**
 * Default constructor
 */

	miscWarningMsg();

/**
 * Destructor
 */

	virtual ~miscWarningMsg();

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


#ifndef _VALUEBIGGERTHANRADIXWARNINGMSG_H
#define _VALUEBIGGERTHANRADIXWARNINGMSG_H

/**
 * CLASS valueBiggerThanRadixWarningMsg
 */

class valueBiggerThanRadixWarningMsg : public miscWarningMsg {

private:

	long value;
	int  radix;

public:

/**
 * Default constructor
 */

	valueBiggerThanRadixWarningMsg();

/**
 * Constructor
 */

	valueBiggerThanRadixWarningMsg(long value, int radix);

/**
 * Destructor
 */

	virtual ~valueBiggerThanRadixWarningMsg();

/**
 * This method displays a warning message.
 */

	void warningMsg();

};

#endif
