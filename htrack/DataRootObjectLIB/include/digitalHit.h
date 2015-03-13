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
///     - base class for holding the digital hit information
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-11-21 13:59:22 $
/// $Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _DIGITALHIT_H
#define _DIGITALHIT_H


#include "../../MiscLIB/include/defs.h"

#if (ARCHITECTURE != PS3)

#include "trackfinderInputHit.h"

#endif

#include <string>


/* **************************************************************
 * CLASS digitalHit								 				*
 * **************************************************************/

class digitalHit {

private:

	unsigned long  hitIndex;			/**< Variable to store the digital hit data which can be used as index for the look-up-tables. */
	unsigned short stationIndex;		/**< Variable to store the index for the detector station in which the digital hit occurs. */

public:

/**
 * Default constructor
 */

	digitalHit();

/**
 * Constructor
 */

	digitalHit(const digitalHit& value);

/**
 * Destructor
 */

	virtual ~digitalHit();

/**
 * operator = ()
 */

	const digitalHit& operator = (const digitalHit& value);

/**
 * This method converts the object into a string representation.
 */

	operator std::string();

/**
 * method returns a pointer to the object
 */

	digitalHit* getPointer();

/**
 * method compare two objects of this class
 * @return value > 0, if element is smaller
 * @return value = 0, if element is equal
 * @return value < 0, if element is bigger
 */

	int compare(digitalHit element);

/**
 * method returns the maximuum number of digital hit
 * indizes which can occur
 */

#if (ARCHITECTURE != PS3)

	unsigned long getMaxNumberOfHitIndizes();

#endif

/**
 * method returns the digital hit index
 */

	unsigned long getHitIndex();
	std::string getHitIndexString();

/**
 * method returns the digital station index
 */

	unsigned short getStationIndex();
	std::string getStationIndexString();

/**
 * Method returns a hit object based on the digital hit index.
 * CAUTION: The return object contains just coordinates.
 * Other parts of such an object are not set.
 */

#if (ARCHITECTURE != PS3)

	trackfinderInputHit getHit();

#endif

/**
 * method sets the digital hit index
 */

	void setHitIndex(unsigned long value);
	void setHitIndexString(std::string& value);

/**
 * method sets the digital station index
 */

	void setStationIndex(unsigned short value);
	void setStationIndexString(std::string& value);

/**
 * Method sets the digital hit index based on a hit object.
 */

#if (ARCHITECTURE != PS3)

	void setHit(trackfinderInputHit* hit);

#endif

/**
 * This method converts the object into a string representation
 * without adding an identifier.
 */
	
	std::string toNotIdentifiedString();

/**
 * This method converts the object into a string representation
 * and adds an identifier.
 */

	std::string toIdentifiedString();

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
