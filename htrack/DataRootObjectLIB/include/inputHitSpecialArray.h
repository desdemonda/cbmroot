//////////////////////////////////////////////////////////////////////
/// (C)opyright 2004
/// 
/// Institute of Computer Science V
/// Prof. M�nner
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
///     - consists of the hits and a special addon for the special memory
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:33:07 $
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _INPUTHITSPECIALARRAY_H
#define _INPUTHITSPECIALARRAY_H


#include "../include/trackfinderInputHit.h"


#define minimalArraySize 30		/**< Defines the size of the constant memory. */


/* **************************************************************
 * CLASS inputHitSpecialArray					 				*
 * **************************************************************/

class inputHitSpecialArray {

protected:

	trackfinderInputHit*  specialArray[minimalArraySize];	/**< Constant memory to store the added pointers to the hits. */
	trackfinderInputHit** specialAddArray;					/**< Dynamic memory to store the added pointers to the hits. */
	unsigned short        numberOfEntries;					/**< Variable to store the number of added pointers to hits. */
	unsigned short        activeObjectPointer;				/**< Variable to access the memory. */

public:

/**
 * Default constructor
 */

	inputHitSpecialArray();

/**
 * Constructor
 */

	inputHitSpecialArray(const inputHitSpecialArray& value);

/**
 * Destructor
 */

	virtual ~inputHitSpecialArray();

/**
 * operator = ()
 */

	const inputHitSpecialArray& operator = (const inputHitSpecialArray& value);

/**
 * sets the activeObject to the initial value
 */

	void resetActiveObject();

/**
 * method clears the memory
 */

	void clear();

/**
 * method returns the number of entries
 */

	unsigned short getNumberOfEntries();

/**
 * method pushes the element at the end of the memory.
 */

	bool push(trackfinderInputHit* hit);

/**
 * returns a reference of the activeObject and set the next one
 * to the active object.
 */

	trackfinderInputHit* readActiveObjectAndMakeNextOneActive();

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
