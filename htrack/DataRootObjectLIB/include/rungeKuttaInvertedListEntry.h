//////////////////////////////////////////////////////////////////////
/// (C)opyright 2005
/// 
/// Institute of Computer Science V
/// Prof. M�ner
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
///     - base class for holding one entry of the second
///       list in the Runge-Kutta algorithm which is used to generate
///       the look-up-tables
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-11-21 13:59:22 $
/// $Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _RUNGEKUTTAINVERTEDLISTENTRY_H
#define _RUNGEKUTTAINVERTEDLISTENTRY_H


#include "../../MiscLIB/include/defs.h"
#include "../../DataObjectLIB/include/specialList.h"
#include "../../DataObjectLIB/include/trackCoordinates.h"
#include "../include/digitalHit.h"


/* **************************************************************
 * CLASS rungeKuttaInvertedListEntry			 				*
 * **************************************************************/

class rungeKuttaInvertedListEntry {

private:

	digitalHit                                                  hit;
	specialList<trackCoordinates, true, true, true, true, true> positions;

public:

/**
 * Default constructor
 */

	rungeKuttaInvertedListEntry();

/**
 * Constructor
 */

	rungeKuttaInvertedListEntry(const rungeKuttaInvertedListEntry& value);

/**
 * Destructor
 */

	virtual ~rungeKuttaInvertedListEntry();

/**
 * operator = ()
 */

	const rungeKuttaInvertedListEntry& operator = (const rungeKuttaInvertedListEntry& value);

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

	rungeKuttaInvertedListEntry* getPointer();

/**
 * method compare two objects of this class
 * @return value > 0, if element is smaller
 * @return value = 0, if element is equal
 * @return value < 0, if element is bigger
 */

	int compare(rungeKuttaInvertedListEntry element);

/**
 * method returns the hit
 */

	 digitalHit getHit();

/**
 * method returns the minimal layer based on the positions
 */

	 unsigned short getMinLayer();

/**
 * method returns the maximal layer based on the positions
 */

	 unsigned short getMaxLayer();

/**
 * method returns the positions
 */

	 specialList<trackCoordinates, true, true, true, true, true> getPositions();

/**
 * method resets the active position object
 */

	 void resetActivePositionObject();

/**
 * method returns the number of position objects
 */

	 unsigned long getNumberOfPositions();

/**
 * method returns the active position object
 */

	 trackCoordinates readActivePositionObject();

/**
 * method makes the next position object active
 */

	 void makeNextPositionObjectActive();

/**
 * method returns the active position object and makes the
 * next one active
 */

	 trackCoordinates readActivePositionObjectAndMakeNextOneActive();

/**
 * method updates the active position object
 */

	 void updateActivePositionObject(trackCoordinates& value);

/**
 * method sets the hit
 */

	 void setHit(digitalHit& value);

/**
 * method sets the positions
 */

	 void setPositions(specialList<trackCoordinates, true, true, true, true, true>& value);

/**
 * method clears all positions
 */

	 void clearPositions();

/**
 * method adds a position
 */

	 void addPosition(trackCoordinates& value);

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
