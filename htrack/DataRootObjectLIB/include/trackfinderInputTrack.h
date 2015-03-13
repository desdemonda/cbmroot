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
///     - consists of the information for the tracks
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-10-07 10:36:50 $
/// $Revision: 1.4 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _TRACKFINDERINPUTTRACK_H
#define _TRACKFINDERINPUTTRACK_H


#include "../../MiscLIB/include/defs.h"
#include "../../DataObjectLIB/include/specialMem.h"
#if (ARCHITECTURE == STANDALONE)
	#include "../../RootFrameworkLIB/include/CbmMCTrack.h"
#elif (ARCHITECTURE == CBMROOT)
	#include "CbmMCTrack.h"
#endif
#include "trackfinderInputHit.h"
#include "TObject.h"


class trackfinderInputHit;


/**
 * The struct consists of the information how often a track is found and with how many hits.
 * This struct is used just for internal analysis.
 */
struct trackfindingTrackInfo{

trackfindingTrackInfo() : frequency(0), maximumNumberOfHits(0) {};
	unsigned short frequency;				/**< Variable to store the number how often this track is found by the trackfinder. */
	unsigned short maximumNumberOfHits;		/**< Variable to store the number of hits from that peak with the biggest number. */

};


/* **************************************************************
 * CLASS trackfinderInputTrack					 				*
 * **************************************************************/

class trackfinderInputTrack : public TObject {

protected:

	CbmMCTrack*                      track;			/**< Pointer to the original track object coming from the framework. */
	specialMem<trackfinderInputHit*> hits;			/**< Object to store all pointers to the hits belonging to the track. */
	trackfindingTrackInfo            info;			/**< Struct to store some internal analysis infomration. */
	bool                             isPointer;		/**< Variable to store if this object is just a pointer or it has allocated memory which has to be freed. */
	int                              trackIndex;	/**< Variable to store the index of this track. */
	double                           charge;		/**< Variable to store the charge of this track. */

public:

/**
 * class definition which is needed for TObject from root.
 */

	ClassDef(trackfinderInputTrack, 1);

/**
 * Default constructor
 */

	trackfinderInputTrack();

/**
 * Constructor
 * @see CbmMCTrack()
 */

	trackfinderInputTrack(const trackfinderInputTrack& value);
	trackfinderInputTrack(CbmMCTrack* mcTrack, int newTrackId, double newCharge);
	trackfinderInputTrack(int pdgId, double momX, double momY, double momZ, int newTrackId, double newCharge);
	
/**
 * Destructor
 */

	virtual ~trackfinderInputTrack();

/**
 * operator = ()
 */

	const trackfinderInputTrack& operator = (const trackfinderInputTrack& value);

/**
 * method returns the trackIndex
 */

	int getTrackIndex();

/**
 * method returns the charge
 */

	double getCharge();

/**
 * method returns the x momentum
 */

	double getMomX();

/**
 * method returns the y momentum
 */

	double getMomY();

/**
 * method returns the z momentum
 */

	double getMomZ();

/**
 * method returns the pdg code
 */

	int getPdgCode();

/**
 * method removes all hits
 */

	void removeHits();

/**
 * method resets the hit pointer to the beginning
 */

	void resetHitPointer();

/**
 * method returns the number of points
 */

	unsigned short getNumberOfPoints();

/**
 * method returns the number of hits
 */

	unsigned short getNumberOfHits();

/**
 * method returns a hit
 */

	trackfinderInputHit* getHit();

/**
 * method adds a hit
 */

	void addHit(trackfinderInputHit* hit);

/**
 * gets the frequency how often the track is found
 */

	unsigned short getFrequency();

/**
 * increments the frequency how often the track is found
 */

	void incrementFrequency();

/**
 * gets the maximum number of hits belonging to the track
 */

	unsigned short getMaximumNumberOfHits();

/**
 * modifies the maximum number of hits belonging to the track,
 * if the value is bigger than the existing one.
 */

	void modifyMaximumNumberOfHits(unsigned short actualMaximumNumberOfHits);

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

/**
 * This method returns the size of the reserved memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getReservedSizeOfAddOnData(unsigned short dimension = 0);

/**
 * This method returns the size of the allocated memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getAllocatedSizeOfAddOnData(unsigned short dimension = 0);

/**
 * This method returns the size of the used memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getUsedSizeOfAddOnData(unsigned short dimension = 0);

};

#endif
