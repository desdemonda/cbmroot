//////////////////////////////////////////////////////////////////////
/// (C)opyright 2004
/// 
/// Institute of Computer Science V
/// Prof. M�ner
/// University of Mannheim, Germany
/// 
/// *******************************************************************
/// 
/// Designer(s):   Steinle / Gl�
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
///     - consists of the information for the hits
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-10-24 16:39:20 $
/// $Revision: 1.6 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _TRACKFINDERINPUTHIT_H
#define _TRACKFINDERINPUTHIT_H


#include "../../MiscLIB/include/defs.h"
#if (ARCHITECTURE == STANDALONE)
	#include "../../RootFrameworkLIB/include/FairHit.h"
	#include "../../RootFrameworkLIB/include/FairMCPoint.h"
#elif (ARCHITECTURE == CBMROOT)
	#include "CbmPixelHit.h"
	#include "FairMCPoint.h"
#endif
#include "trackfinderInputTrack.h"
#include "trackfinderInputStation.h"
#include "TObject.h"


class trackfinderInputTrack;


/* **************************************************************
 * CLASS trackfinderInputHit					 				*
 * **************************************************************/

class trackfinderInputHit : public TObject {

protected:

	CbmPixelHit*                  hit;			/**< Pointer to the original hit object coming from the framework. */
	FairMCPoint*              point;			/**< Pointer to the original point object belonging to this hit coming from the framework. */
	trackfinderInputTrack*   track;			/**< Pointer to the original track for this hit object coming from the framework. */
	trackfinderInputStation* station;		/**< Pointer to the station of the detector which the hit hits. */
	bool                     isPointer;		/**< Variable to store if this object is just a pointer or it has allocated memory which has to be freed. */
	int                      hitIndex;		/**< Variable to store the index of this hit. */
	unsigned int             hitOrder;		/**< Variable to store the order of this hit. */

public:

/**
 * class definition which is needed for TObject from root.
 */

	ClassDef(trackfinderInputHit, 1);

/**
 * Default constructor
 */

	trackfinderInputHit();

/**
 * Constructor
 * @see FairHit()
 */

	trackfinderInputHit(const trackfinderInputHit& value);
	trackfinderInputHit(const trackfinderInputHit& value, unsigned int order);
	trackfinderInputHit(CbmPixelHit* hitPointer, int index, unsigned int order);
	trackfinderInputHit(int detectorId, double posX, double posY, double posZ, double xError, double yError, int index, unsigned int order, int pointIndex, bool maps, bool strip, bool hybrid);
	trackfinderInputHit(int detectorId, double posX, double posY, double posZ, double xError, double yError, int index, unsigned int order, int pointIndex);

/**
 * Destructor
 */

	virtual ~trackfinderInputHit();

/**
 * operator = ()
 */

	const trackfinderInputHit& operator = (const trackfinderInputHit& value);

/**
 * method initializes the object
 * @see FairHit()
 */

	void initDefault();
	void init(CbmPixelHit* hitPointer, int index, unsigned int order);
	void init(int detectorId, double posX, double posY, double posZ, double xError, double yError, int index, unsigned int order, int pointIndex, bool maps, bool strip, bool hybrid);
	void init(int detectorId, double posX, double posY, double posZ, double xError, double yError, int index, unsigned int order, int pointIndex);

/**
 * method returns the detectorId
 */

	int getStationId();

/**
 * method returns the x position
 */

	double getPosX();

/**
 * method returns the y position
 */

	double getPosY();

/**
 * method returns the z position
 */

	double getPosZ();

/**
 * method gets the original hit-object
 */

	CbmPixelHit* getHit();

/**
 * method gets the original point-object from hit
 */

	FairMCPoint* getPoint();

/**
 * method gets the track-object from hit
 */

	trackfinderInputTrack* getTrack();

/**
 * method gets the station-object from hit
 */

	trackfinderInputStation* getStation();

/**
 * method returns true if the original hit-object is from a
 * maps detector.
 */

	bool isMapsHit();

/**
 * method returns true if the original hit-object is from a
 * strip detector.
 */

	bool isStripHit();

/**
 * method returns true if the original hit-object is from a
 * hybrid detector.
 */

	bool isHybridHit();

/**
 * method gets the index of the original hit-object
 */

	int getHitIndex();

/**
 * method gets the order of this hit-object
 */

	unsigned int getHitOrder();

/**
 * method sets the original point-object from hit
 */

	void setPoint(FairMCPoint* actualPoint);

/**
 * method sets the track-object from hit
 */

	void setTrack(trackfinderInputTrack* actualTrack);

/**
 * method sets the station-object from hit
 */

	void setStation(trackfinderInputStation* actualStation);

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
