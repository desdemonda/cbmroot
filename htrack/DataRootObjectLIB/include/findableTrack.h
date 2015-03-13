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
///     - base class for holding the track coordinates and the MCTrack
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2006/07/17 11:27:32 $
/// $Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _FINDABLETRACK_H
#define _FINDABLETRACK_H


#include "../../DataObjectLIB/include/trackCoordinates.h"
#include "trackfinderInputTrack.h"


/* **************************************************************
 * CLASS findableTrack							 				*
 * **************************************************************/

class findableTrack {

private:

	trackfinderInputTrack* mcTrack;		/**< Pointer to point to the original Monte Carlo Track information. */
	trackCoordinates       position;	/**< Object to store the coordinates of the histogram for the peak for this track. */

public:

/**
 * Default constructor
 */

	findableTrack();

/**
 * Constructor
 * @param mcTrack is a pointer to the track information from Monte Carlo
 */

	findableTrack(const findableTrack& value);
	findableTrack(trackfinderInputTrack* mcTrack, trackCoordinates& position);

/**
 * Destructor
 */

	virtual ~findableTrack();

/**
 * operator = ()
 */

	const findableTrack& operator = (const findableTrack& value);

/**
 * returns a pointer to the original track information from Monte Carlo
 */

	trackfinderInputTrack* getTrack();

/**
 * returns the coordinates of the histogram for the peak for this track
 */

	trackCoordinates& getPosition();

/**
 * sets the pointer to the original track information from Monte Carlo
 * and sets the position in the Hough space
 */

	void setParameter(trackfinderInputTrack* mcTrack, trackCoordinates& position);

};

#endif
