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
///     - base class for holding the houghTransformation border from
///       the lut look-up-table
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-10-24 16:36:57 $
/// $Revision: 1.5 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _LUTHOUGHBORDER_H
#define _LUTHOUGHBORDER_H


#include "specialMemClass.h"
#include "houghBorderPosition.h"
#include "houghBorderCommand.h"


/* **************************************************************
 * CLASS lutHoughBorder							 				*
 * **************************************************************/

class lutHoughBorder {

public:

	specialMemClass<houghBorderPosition> houghCoord;		/**< Object to store all positions of the entry in the histogram in a list of coordinates. */

/**
 * Default constructor
 */

	lutHoughBorder();

/**
 * Constructor
 */

	lutHoughBorder(const lutHoughBorder& value);

/**
 * Destructor
 */

	virtual ~lutHoughBorder();

/**
 * operator = ()
 */

	const lutHoughBorder& operator = (const lutHoughBorder& value);

/**
 * This method clears the object.
 */

	void clear();

/**
 * This method initializes the object.
 */

	void init();

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

/**
 * This method returns the houghBorder command.
 * @param maxDim is the stepsize of the second dimension to have a range check 
 */

	houghBorderCommand getHoughBorderCommand(unsigned short maxDim);

/**
 * This method sets the houghBorder command.
 * @param value is the source to setup the values for the specialMem<houghBorderPosition> houghCoord
 * @param maxFirstDim is the stepsize of the first dimension to have a range check 
 * @param maxSecondDim is the stepsize of the second dimension to have a range check 
 */

	void setHoughBorderCommand(houghBorderCommand& value, unsigned short maxFirstDim, unsigned short maxSecondDim);

/**
 * This method returns the size of the reserved memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getReservedSizeOfData(unsigned short dimension = 0);

/**
 * This method returns the size of the allocated memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getAllocatedSizeOfData(unsigned short dimension = 0);

/**
 * This method returns the size of the used memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getUsedSizeOfData(unsigned short dimension = 0);

};

#endif
