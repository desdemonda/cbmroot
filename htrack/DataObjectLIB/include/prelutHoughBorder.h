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
///       the prelut look-up-table
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-10-24 16:36:57 $
/// $Revision: 1.5 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _PRELUTHOUGHBORDER_H
#define _PRELUTHOUGHBORDER_H


/* **************************************************************
 * CLASS prelutHoughBorder						 				*
 * **************************************************************/

class prelutHoughBorder {

public:

	unsigned short start;		/**< Variable to store the starting index of the border. */
	unsigned short stop;		/**< Variable to store the end index of the border. */

/**
 * Default constructor
 */

	prelutHoughBorder();

/**
 * Constructor
 */

	prelutHoughBorder(const prelutHoughBorder& value);

/**
 * Destructor
 */

	virtual ~prelutHoughBorder();

/**
 * operator = ()
 */

	const prelutHoughBorder& operator = (const prelutHoughBorder& value);

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
 * method returns the start value
 */

	std::string getStart();

/**
 * method returns the stop value
 */

	std::string getStop();

/**
 * method sets the start value
 */

	void setStart(std::string& value);

/**
 * method sets the stop value
 */

	void setStop(std::string& value);

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
