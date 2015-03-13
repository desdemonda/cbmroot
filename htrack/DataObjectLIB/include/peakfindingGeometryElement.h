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
///     - class for holding the information of an element in the
///       automatic generated filter geometry
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-10-07 10:36:23 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _PEAKFINDINGGEOMETRYELEMENT_H
#define _PEAKFINDINGGEOMETRYELEMENT_H


#include "../../MiscLIB/include/coordinateSystem.h"


/* **************************************************************
 * CLASS peakfindingGeometryElement					 			*
 * **************************************************************/

class peakfindingGeometryElement {

protected:

	short          dim[3];					/**< Variable to store the coordinatevector. */
	unsigned short value;					/**< Variable to store the value of the actual position */

public:

/**
 * Default constructor
 */

	peakfindingGeometryElement();

/**
 * Constructor
 * @param dim1 is the value in the first dimension
 * @param dim2 is the value in the second dimension
 * @param dim3 is the value in the third dimension
 */

	peakfindingGeometryElement(const peakfindingGeometryElement& value);
	peakfindingGeometryElement(short dim1, short dim2, short dim3, unsigned short value);

/**
 * Destructor
 */

	virtual ~peakfindingGeometryElement();

/**
 * operator = ()
 */

	const peakfindingGeometryElement& operator = (const peakfindingGeometryElement& value);

/**
 * returns the value in the dimension
 */

	short get(unsigned short dimension);
	std::string getString(unsigned short dimension);

/**
 * get value
 */

	unsigned short getValue();
	std::string getValueString();

/**
 * sets the value in the dimension
 */

	void set(short set, unsigned short dimension);
	void setString(std::string& set, unsigned short dimension);

/**
 * set value
 */

	void setValue(unsigned short value);
	void setValueString(std::string& value);

/**
 * increment value
 */

	void incrementValue(unsigned short increment = 1);

/**
 * This method compares to objects of this class. The result is
 * - zero, if the object is equal to the element
 * - else, if the object is not equal to the element
 */

	int compare(peakfindingGeometryElement& element);

/**
 * This method compares to objects of this class. The result is
 * - true, if the object is bigger than the element
 * - false, if the object is not bigger than the element
 */

	bool isBigger(peakfindingGeometryElement& element);

/**
 * returns a pointer to this object
 */

	peakfindingGeometryElement* getPointer();

/**
 * returns a string representation for this object
 */

	std::string toString();

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
