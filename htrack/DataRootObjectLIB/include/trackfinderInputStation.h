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
///     - consists of the information for one detector station
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:33:07 $
/// $Revision: 1.5 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _TRACKFINDERINPUTSTATION_H
#define _TRACKFINDERINPUTSTATION_H


#include "TObject.h"


/**
 * Defines all possible types for the station
 */
enum{NOSTATIONTYPE, MAPSSTATIONTYPE, HYBRIDSTATIONTYPE, STRIPSTATIONTYPE, INVALIDSTATIONTYPE};


/* **************************************************************
 * CLASS trackfinderInputStation				 				*
 * **************************************************************/

class trackfinderInputStation : public TObject {

protected:

	int            id;				/**< Variable to store the id of the station. */
	double         distance;		/**< Variable to store the distance of the station. */
	unsigned char  type;			/**< Variable to store the type of the station. */
	bool           masked;			/**< Variable to store the mask of the station. If this is true, the station is not taken into the computation of the algorithm. This is done by not accepting the hits of the station and by not indexing. */
	unsigned short index;			/**< Variable to store the index of the station with regard to the whole detector. This index is the order of the station in the detector. By changing the type of this variable, you have to change also the definition of INVALIDSTATIONTYPE. */

public:

/**
 * class definition which is needed for TObject from root.
 */

	ClassDef(trackfinderInputStation, 1);

/**
 * Default constructor
 */

	trackfinderInputStation();

/**
 * Constructor
 */

	trackfinderInputStation(const trackfinderInputStation& value);
	trackfinderInputStation(double stationDistance,	int stationId,
							bool stationMaps, bool stationHybrid,
							bool stationStrip, bool mask = false);

/**
 * Destructor
 */

	virtual ~trackfinderInputStation();

/**
 * operator = ()
 */

	const trackfinderInputStation& operator = (const trackfinderInputStation& value);

/**
 * This method returns the id of the station.
 */

	int getId();

/**
 * This method returns the distance of the station.
 */

	double getDistance();

/**
 * This method returns if the station is of no type.
 */

	bool isNoType();

/**
 * This method returns if the station is of type MAPS.
 */

	bool isMapsType();

/**
 * This method returns if the station is of type HYBRID.
 */

	bool isHybridType();

/**
 * This method returns if the station is of type STRIP.
 */

	bool isStripType();

/**
 * This method returns if the station is masked.
 */

	bool isMasked();

/**
 * This method returns the index of the station.
 */

	unsigned short getIndex();

/**
 * This method sets the id of the station.
 */

	void setId(int stationId);

/**
 * This method sets the distance of the station.
 */

	void setDistance(double stationDistance);

/**
 * This method sets if the station is of no type.
 */

	void removeType();

/**
 * This method sets if the station is of type MAPS.
 */

	void setMapsType(bool stationMaps = true);

/**
 * This method sets if the station is of type HYBRID.
 */

	void setHybridType(bool stationHybrid = true);

/**
 * This method sets if the station is of type STRIP.
 */

	void setStripType(bool stationStrip = true);

/**
 * This method sets the mask of the station.
 */

	void setMask(bool stationMask = true);

/**
 * This method sets the index of the station.
 */

	void setIndex(unsigned short stationIndex);

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
