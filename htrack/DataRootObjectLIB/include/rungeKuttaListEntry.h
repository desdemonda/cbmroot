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
///   class:
///     - base class for holding one entry of the first
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


#ifndef _RUNGEKUTTALISTENTRY_H
#define _RUNGEKUTTALISTENTRY_H


#include "../../DataObjectLIB/include/trackCoordinates.h"
#include "../../DataObjectLIB/include/specialList.h"
#include "../include/digitalHit.h"


/* **************************************************************
 * CLASS rungeKuttaListEntry					 				*
 * **************************************************************/

class rungeKuttaListEntry {

private:

	trackCoordinates position;
	specialList<digitalHit, true, true, true, true, true> hits;

public:

/**
 * Default constructor
 */

	rungeKuttaListEntry();

/**
 * Constructor
 */

	rungeKuttaListEntry(const rungeKuttaListEntry& value);

/**
 * Destructor
 */

	virtual ~rungeKuttaListEntry();

/**
 * operator = ()
 */

	const rungeKuttaListEntry& operator = (const rungeKuttaListEntry& value);

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

	rungeKuttaListEntry* getPointer();

/**
 * method compare two objects of this class
 * @return value > 0, if element is smaller
 * @return value = 0, if element is equal
 * @return value < 0, if element is bigger
 */

	int compare(rungeKuttaListEntry element);

/**
 * method returns the position
 */

	 trackCoordinates getPosition();

/**
 * method returns the hits
 */

	 specialList<digitalHit, true, true, true, true, true> getHits();

/**
 * method resets the active hit object
 */

	 void resetActiveHitObject();

/**
 * method returns the number of hit objects
 */

	 unsigned long getNumberOfHits();

/**
 * method returns the active hit object and makes the
 * next one active
 */

	 digitalHit readActiveHitObjectAndMakeNextOneActive();

/**
 * method sets the position
 */

	 void setPosition(trackCoordinates& value);

/**
 * method sets the hits
 */

	 void setHits(specialList<digitalHit, true, true, true, true, true>& value);

/**
 * method clears all hit
 */

	 void clearHits();

/**
 * method adds a hit
 */

	 void addHit(digitalHit& value);

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
