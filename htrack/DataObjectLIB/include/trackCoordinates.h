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
///     - base class for holding the track coordinates of the transformation
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-11-21 13:58:52 $
/// $Revision: 1.5 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _TRACKCOORDINATES_H
#define _TRACKCOORDINATES_H


#include "../../MiscLIB/include/coordinateSystem.h"
#include <string>


/* **************************************************************
 * CLASS trackCoordinates						 				*
 * **************************************************************/

class trackCoordinates {

protected:

	unsigned short dim[3];		/**< Variable to store the coordinatevector. */

public:

/**
 * Default constructor
 */

	trackCoordinates();

/**
 * Constructor
 * @param dim1 is the value in the first dimension
 * @param dim2 is the value in the second dimension
 * @param dim3 is the value in the third dimension
 */

	trackCoordinates(const trackCoordinates& value);
	trackCoordinates(unsigned short dim1, unsigned short dim2, unsigned short dim3);

/**
 * Destructor
 */

	virtual ~trackCoordinates();

/**
 * operator = ()
 */

	const trackCoordinates& operator = (const trackCoordinates& value);

/**
 * This method converts the object into a string representation.
 */

	operator std::string();

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
 * method returns a pointer to the object
 */

	trackCoordinates* getPointer();

/**
 * method compare two objects of this class
 * @return value > 0, if element is smaller
 * @return value = 0, if element is equal
 * @return value < 0, if element is bigger
 */

	int compare(trackCoordinates element);

/**
 * returns the value in the dimension
 */

	unsigned short get(unsigned short dimension);

/**
 * sets the value in the dimension
 */

	void set(unsigned short set, unsigned short dimension);

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
