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
///     - consists of the hits and a special addon for the special memory
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:33:07 $
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _INPUTHITSPECIALMEM_H
#define _INPUTHITSPECIALMEM_H


#include "../../MiscLIB/include/memoryDef.h"
#include "inputHitSpecialArray.h"
#include "inputHitSpecialList.h"


#define numberOfMemories maxDimSH				/**< Defines the number of memories in parallel. Ordinary there is one memory for each station of the detector. */
#define typeOfMemories   inputHitSpecialArray	/**< Defines the object which is used to implement one memory. */


/* **************************************************************
 * CLASS inputHitSpecialMem						 				*
 * **************************************************************/

class inputHitSpecialMem {

protected:

	typeOfMemories specialMem[numberOfMemories];		/**< Array to store the hits in the memory of the corresponding detector station. */

public:

/**
 * Default constructor
 */

	inputHitSpecialMem();

/**
 * Constructor
 */

	inputHitSpecialMem(const inputHitSpecialMem& value);

/**
 * Destructor
 */

	virtual ~inputHitSpecialMem();

/**
 * operator = ()
 */

	const inputHitSpecialMem& operator = (const inputHitSpecialMem& value);

/**
 * method resets the memory
 */

	void reset();

/**
 * method returns the number of memories
 */

	unsigned short getNumberOfMemories();

/**
 * operator []
 */

	typeOfMemories& operator [] (size_t index);

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
