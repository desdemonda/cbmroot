//////////////////////////////////////////////////////////////////////
/// (C)opyright 2005
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
///     - base class for holding the houghTransformation entry command
///       for the histogram
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:32:41 $
/// $Revision: 1.4 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _HOUGHBORDERCOMMAND_H
#define _HOUGHBORDERCOMMAND_H


#include "specialMem.h"


/* **************************************************************
 * CLASS houghBorderCommand						 				*
 * **************************************************************/

class houghBorderCommand {

public:

	unsigned short   startPos;		/**< Variable to store the position for the first entry in x-dimension. The position of the y-dimension is zero */
	specialMem<bool> cmd;			/**< Object to store the difference of the position in x-dimension for the next histogram entry in y-dimension. The accepted values are 0 and 1. */

/**
 * Default constructor
 */

	houghBorderCommand();

/**
 * Constructor
 */

	houghBorderCommand(const houghBorderCommand& value);

/**
 * Destructor
 */

	virtual ~houghBorderCommand();

/**
 * operator = ()
 */

	const houghBorderCommand& operator = (const houghBorderCommand& value);

/**
 * method returns the startPos value
 */
	
	std::string getStartPos();

/**
 * method returns the startPos value
 */
	
	std::string getCmd();

/**
 * method sets the startPos value
 */
	
	void setStartPos(std::string& value);

/**
 * method sets the startPos value
 */
	
	void setCmd(std::string& value);

/**
 * This method converts the object into a string representation
 * and adds no identifiers.
 */
	
	std::string toNotIdentifiedString();

/**
 * This method converts the object into a string representation
 * and adds identifiers.
 */

	std::string toIdentifiedString();

};

#endif
