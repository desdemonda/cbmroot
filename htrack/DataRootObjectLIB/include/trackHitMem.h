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
///     - consists of a memory holding all hits corresonding to the same track
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:33:07 $
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _TRACKHITMEM_H
#define _TRACKHITMEM_H


#include "../../DataObjectLIB/include/specialMem.h"
#include "../../MiscLIB/include/bitArray.h"
#include "trackfinderInputHit.h"
#include <string>


/* **************************************************************
 * CLASS trackHitMem							 				*
 * **************************************************************/

class trackHitMem {

protected:

	specialMem<trackfinderInputHit*> hits;			/**< Object to store hit objects for one corresponding track. */
	bitArray                         trackPattern;	/**< Object to store the pattern which detector station has a hit. A hit is coded with a one. Else the bit in the pattern is zero. */

/**
 * returns the trackId
 */

	int getTrackIndex();

public:

/**
 * Default constructor
 */

	trackHitMem();

/**
 * Constructor
 */

	trackHitMem(const trackHitMem& value);

/**
 * Destructor
 */

	virtual ~trackHitMem();

/**
 * operator = ()
 */

	const trackHitMem& operator = (const trackHitMem& value);

/**
 * method removes all hits
 */

	void removeAllHits();

/**
 * method returns the number of hits
 */

	unsigned long getNumberOfHits();

/**
 * method returns a hit and activates the next one
 */

	trackfinderInputHit* getHitAndMakeNextActive();

/**
 * method adds a hit
 */

	void addHit(trackfinderInputHit* hit);

/**
 * method resets the pointer to the hits
 */

	void resetHitPointer();

/**
 * This method compares to objects of this class. The result is
 * - zero, if the object is equal to the element
 * - else, if the object is not equal to the element
 */

	int compare(trackHitMem& element);

/**
 * This method compares to objects of this class. The result is
 * - true, if the object is bigger than the element
 * - false, if the object is not bigger than the element
 */

	bool isBigger(trackHitMem& element);

/**
 * returns a pointer to this object
 */

	trackHitMem* getPointer();

/**
 * method returns the trackPattern
 */

	bitArray getTrackPattern();

/**
 * method returns the real track itself
 */

	trackfinderInputTrack* getRealTrack();

};

#endif
