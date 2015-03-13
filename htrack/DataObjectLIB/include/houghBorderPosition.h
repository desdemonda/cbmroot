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
/// $Date: 2008-08-14 12:32:41 $
/// $Revision: 1.5 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _HOUGHBORDERPOSITION_H
#define _HOUGHBORDERPOSITION_H


/* **************************************************************
 * CLASS houghBorderPosition					 				*
 * **************************************************************/

class houghBorderPosition {

protected:

/**
 * This method divides the set-string by searching for the separater 
 */

	void partString(std::string& basicString, std::string& part1, std::string& part2);

/**
 * This method returns the basicString without the part-string 
 */

	std::string removeFrontString(std::string& basicString, std::string& part);

/**
 * This method returns the basicString without the part-string 
 */

	std::string removeBackString(std::string& basicString, std::string& part);

public:

	unsigned short pos1;		/**< Variable to store the position in the first dimension. */
	unsigned short pos2;		/**< Variable to store the position in the second dimension. */

/* **************************************************************
 * Default constructor											*
 * **************************************************************/

	houghBorderPosition();

/**
 * Constructor
 */

	houghBorderPosition(std::string& value, bool identifiedString);

/**
 * Copy constructor
 */

	houghBorderPosition(const houghBorderPosition& value);

/**
 * Destructor
 */

	virtual ~houghBorderPosition();

/**
 * operator = ()
 */

	const houghBorderPosition& operator = (const houghBorderPosition& value);

/**
 * This method sets the object based on the string representation
 * without identifiers.
 */
	
	void fromNotIdentifiedString(std::string& value);

/**
 * This method sets the object based on the string representation
 * without identifiers.
 */
	
	void fromNotIdentifiedRadixString(std::string& value);

/**
 * This method sets the object based on the string representation
 * with identifiers.
 */

	void fromIdentifiedString(std::string& value);

/**
 * This method sets the object based on the string representation
 * with identifiers.
 */

	void fromIdentifiedRadixString(std::string& value);

/**
 * This method converts the object into a string representation
 * and adds no identifiers.
 */
	
	std::string toNotIdentifiedString();

/**
 * This method converts the object into a string representation
 * and adds no identifiers.
 */
	
	std::string toNotIdentifiedRadixString();

/**
 * This method converts the object into a string representation
 * and adds identifiers.
 */

	std::string toIdentifiedString();

/**
 * This method converts the object into a string representation
 * and adds identifiers.
 */

	std::string toIdentifiedRadixString();

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
