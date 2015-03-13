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
///     - class for handling the histogram-transformations memory space
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:33:07 $
/// $Revision: 1.8 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _TRACKDATA_H
#define _TRACKDATA_H


#include "../../DataObjectLIB/include/histogramSpace.h"
#include "../include/histogramCell.h"
#include "../include/trackLayerAccess.h"
#include "../include/trackDigitalInformation.h"
#include "../include/trackAnalogInformation.h"



//#define CORRECTTRACK		/**< This define corrects each track to be comparable with previous versions of the program. */
#undef CORRECTTRACK


/* **************************************************************
 * CLASS trackData								 				*
 * **************************************************************/

class trackData {

private:

	histogramSpace**     space;				/**< Object to store the needed values to compute the trackAnalogInformation object based on the trackDigitalInformation object. */
	trackLayer*          tracks;			/**< Memory to store the computed trackDigitalInformation objects for each layer. */
	trackLayer::iterator accessor;			/**< Iterator to access all computed trackDigitalInformation objects independant from the layer. */
	unsigned short       accessLayer;		/**< Variable to store the number of the actual layer accessed by the iterator. */

public:

/**
 * Default constructor
 */

	trackData();

/**
 * Constructor
 * @param space is the size of the histogram in the first three dimensions
 */

	trackData(const trackData& value);
	trackData(histogramSpace** space);

/**
 * Destructor
 */

	virtual ~trackData();

/**
 * operator = ()
 */

	const trackData& operator = (const trackData& value);

/**
 * This method initializes the object.
 * @param space is the size of the histogram in the first three dimensions
 */

	void init(histogramSpace** space);

/**
 * This method returns a reference to the object
 * representing the histogramSpace.
 */

	histogramSpace& getHistogramSpace();

/**
 * This method resets the object.
 */

	void reset();

/**
 * This method resets the active object.
 */

	void resetActiveObject();

/**
 * This method returns the size of the used memory for
 * histogram.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getUsedSizeOfTrackData(unsigned short dimension = 0);

/**
 * This method returns the size of the allocated memory for
 * histogram.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getAllocatedSizeOfTrackData(unsigned short dimension = 0);

/**
 * This method returns the number of layers. This value must be
 * equal to the layer dimension of the histogram.
 */

	unsigned short getNumberOfLayers();

/**
 * This method adds a track into the memory.
 * @param dim1 is the value of the coordinate in the first dimension
 * @param dim2 is the value of the coordinate in the second dimension
 * @param dim3 is the value of the coordinate in the third dimension
 * @param cell is a reference to the corresponding histogram cell consisiting of the value and the hits
 */

	void addTrack(unsigned short dim1, unsigned short dim2, unsigned short dim3, histogramCell& cell);

/**
 * This method removes a track from the memory.
 */

	trackLayer::iterator removeTrack(trackLayer::iterator position, unsigned short layer);

/**
 * This method returns the number of tracks.
 */

	unsigned short getNumberOfTracks();

/**
 * This method returns the trackInfo in an order.
 */

	void getNextTrackDigitalInfo(trackDigitalInformation* value);

/**
 * This method returns the trackAnalogInformation in an order.
 */

	void getNextTrackAnalogInfo(trackAnalogInformation* value);

/**
 * This method returns the hit information.
 */

	void getNextHitInfo(hitArray* value);

/**
 * This method returns the each trackAnalogInformation in an order.
 */

	void getTrackAnalogInfoFromTrackDigitalInfo(trackDigitalInformation digital, trackAnalogInformation* analog);

/**
 * This method returns the each trackParameter in an order.
 */

	void getTrackParameterFromTrackDigitalInfo(trackDigitalInformation digital, trackParameter* parameter);

/**
 * This method returns the number of tracks of one layer.
 */

	unsigned short getNumberOfTracksOfHistogramLayer(unsigned short layer);

/**
 * This method returns an iterator to the begin of tracks in
 * one histogram layer.
 */

	trackLayer::iterator getTrackIteratorBeginOfHistogramLayer(unsigned short layer);

/**
 * This method returns an iterator to the end of tracks in
 * one histogram layer.
 */

	trackLayer::iterator getTrackIteratorEndOfHistogramLayer(unsigned short layer);

/**
 * This method prints a the borders of a histogram into a file.
 * @param maximumClass is the maximal value for a class. This is printed just in the comptability mode instead of the correct value.
 * @param layerStart is the start index in the third dimension of the histogram
 * @param layerStop is the stop index in the third dimension of the histogram
 * @param stationStart is the start index in the fourth dimension of the histogram
 * @param stationStop is the stop index in the fourth dimension of the histogram
 * @param hits should be true if the ids of the hits of each cell have to be printed also
 * @param name is the name of the file to print
 */

	void printTracks(unsigned int maximumClass, unsigned int layerStart, unsigned int layerStop, unsigned int stationStart, unsigned int stationStop, bool hits, char* name);

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
