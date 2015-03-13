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
///     - class for structuring the dataObjects for output
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-10-07 10:36:50 $
/// $Revision: 1.8 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _TRACKFINDEROUTPUTDATA_H
#define _TRACKFINDEROUTPUTDATA_H


#include "../../MiscLIB/include/defs.h"
#if (ARCHITECTURE == STANDALONE)
	#include "../../RootFrameworkLIB/include/CbmStsTrack.h"
#elif (ARCHITECTURE == CBMROOT)
	#include "CbmStsTrack.h"
#endif
#include "../include/trackfinderInputHit.h"
#include "../../DataObjectLIB/include/trackMomentum.h"
#include "../../DataObjectLIB/include/trackFrameworkMomentum.h"
#include "TClonesArray.h"
#include "TObject.h"


/* **************************************************************
 * CLASS trackfinderOutputData					 				*
 * **************************************************************/

class trackfinderOutputData : public TObject {

protected:

	TClonesArray* tracks;			/**< Object to store all tracks. */
	CbmStsTrack*  actualTrack;		/**< Pointer to the actual track for accessing. */ 
	bool          tracksAreLocal;	/**< Variable to store if the TClonesArray tracks is locally allocated or a pointer set from outside. */ 

public:

/**
 * class definition which is needed for TObject from root.
 */

	ClassDef(trackfinderOutputData, 1);

/**
 * Default constructor
 */

	trackfinderOutputData();

/**
 * Constructor
 */

	trackfinderOutputData(const trackfinderOutputData& value);

/**
 * Destructor
 */

	virtual ~trackfinderOutputData();

/**
 * operator = ()
 */

	const trackfinderOutputData& operator = (const trackfinderOutputData& value);

/**
 * initializes the object
 */

	void init();
	void init(TClonesArray* tracks);

/**
 * method returns the number of tracks
 */

	int getNumberOfTracks();

/**
 * method returns a reference to the track data
 */

	TClonesArray* getTrackData();

/**
 * method returns a reference to a track with a specified index
 */

	CbmStsTrack* getTrackByOrder(unsigned int index);

/**
 * method adds a track to the end of the Track-TClonesArray
 */

	void addTrack(CbmStsTrack& track);
	void addTrack(trackMomentum& momentum);

/**
 * method adds a track to the end of the Track-TClonesArray
 */

	void addTrack(trackFrameworkMomentum& momentum);

/**
 * method adds a hit to the end of the Hit-TClonesArray
 */

	void addHitToActualTrack(trackfinderInputHit* hit);

/**
 * method removes all tracks from the Track-TClonesArray
 */

	void removeAllTracks();

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
