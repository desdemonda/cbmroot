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
///     - consists of the information for the detector
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:33:07 $
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _TRACKFINDERINPUTDETECTOR_H
#define _TRACKFINDERINPUTDETECTOR_H


#include "trackfinderInputStation.h"
#include "TObject.h"


/* **************************************************************
 * CLASS trackfinderInputDetector				 				*
 * **************************************************************/

class trackfinderInputDetector : public TObject {

private:

/**
 * method searches the detector for a specific station with id.
 */

	trackfinderInputStation* searchStation(int stationId);

protected:

	unsigned short           numberOfStations;			/**< Variable to store the number of stations of the detector. */
	unsigned short           numberOfActiveStations;	/**< Variable to store the number of stations which are not masked. That means which should be taken into the computation. */
	trackfinderInputStation* detector;					/**< Memory to store all stations of the detector. */

public:

/**
 * class definition which is needed for TObject from root.
 */

	ClassDef(trackfinderInputDetector, 1);

/**
 * Default constructor
 */

	trackfinderInputDetector();

/**
 * Constructor
 */

	trackfinderInputDetector(const trackfinderInputDetector& value);

/**
 * Destructor
 */

	virtual ~trackfinderInputDetector();

/**
 * operator = ()
 */

	const trackfinderInputDetector& operator = (const trackfinderInputDetector& value);

/**
 * method inits the default detector.
 */

	void initDefaultDetector();

/**
 * method returns the number of detector stations
 */

	unsigned short getNumberOfStations();

/**
 * method returns the number of active detector stations
 */

	unsigned short getNumberOfActiveStations();

/**
 * This method returns the actual layers id.
 */

	unsigned short getStationIndex(int stationId);

/**
 * method gets the detector station with a specific stationId
 */

	trackfinderInputStation& getStationById(int stationId);

/**
 * method gets the detector station with stationIndex
 * @see trackfinderInputStation::index
 */

	trackfinderInputStation& getStationByIndex(unsigned int stationIndex);

/**
 * method gets the detector station with a specific stationId
 */

	trackfinderInputStation* getStationPointerById(int stationId);

/**
 * method gets the detector station with stationIndex
 * @see trackfinderInputStation::index
 */

	trackfinderInputStation* getStationPointerByIndex(unsigned int stationIndex);

/**
 * method adds the detector station
 */

	void addStation(trackfinderInputStation station);

/**
 * method removes the detector station with a specific stationId
 */

	void removeStation(int stationId);

/**
 * method removes all detector stations
 */

	void removeDetector();

/**
 * method sorts the detector stations
 */

	void sortDetector();

/**
 * method sets the indexes for all detector stations
 */

	void reIndex();

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
