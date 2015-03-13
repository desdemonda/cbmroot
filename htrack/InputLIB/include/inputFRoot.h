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
///     - reads the event information from a special file
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:36:16 $
/// $Revision: 1.6 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _INPUTFROOT_H
#define _INPUTFROOT_H


#include "inputRoot.h"


/* **************************************************************
 * CLASS inputFRoot								 				*
 * **************************************************************/

class inputFRoot : public inputRoot {

protected:

	bool disableAutomaticDetector;		/**< Variable to store if the detector should be used directly from the framework or not. */

/**
 * Method returns the relative path for the geometry file.
 */

	std::string getGeometryFileRelativePath();

/**
 * Method accesses objects for the dictionarys.
 */

	void addDictionaries();

/**
 * Method reads the variables based on the detector stations
 * by hand.
 */

	bool readDetectorByInterface(CbmStsDigiScheme* scheme);
	bool readMvdDetectorByInterface(CbmStsDigiScheme* scheme);
	bool readStsDetectorByInterface(CbmStsDigiScheme* scheme);

/**
 * Method reads the variables based on the detector stations
 * by hand.
 */

	bool readDetectorByHand();
	bool readMvdDetectorByHand();
	bool readStsDetectorByHand();

public:

/**
 * class definition which is needed for TObject from root.
 */

	ClassDef(inputFRoot, 1);

/**
 * Default constructor
 */

	inputFRoot();

/**
 * Constructor
 * @param name is the name of the input file to read the data
 * @param numberOfVolumesInfrontOfSTS is the number of geometric volumes which are established in front of the STS volumes. Needed to compute the correct volumeID
 * @param geometryFileDirectory is the name of the directory which consists of the geometry definition ascii files
 * @param detMask is an object representing the mask for each detector station
 * @param hitProducer defines the way to get hits from points
 * @param enableJustPoints must be true, if points should be used instead of hits
 * @param enableHitsFromFile must be true, if the hits should be read out of a file
 * @param enableMapsHits must be true if the hits from maps should be accessed
 * @param enableHybridHits must be true if the hits from hybrids should be accessed
 * @param enableStripHits must be true if the hits from strips should be accessed
 */

	inputFRoot(bitArray detMask, bool enableJustPoints, bool enableHitsFromFile, bool enableMapsHits = true, bool enableHybridHits = true, bool enableStripHits = true);
	inputFRoot(const char* name, bitArray detMask, int hitProducer, bool enableJustPoints = false, bool enableHitsFromFile = false, bool enableMapsHits = true, bool enableHybridHits = true, bool enableStripHits = true);
	inputFRoot(const char* detectorFileName, unsigned short numberOfVolumesInfrontOfSTS, bool disableAutomaticDetector, bitArray detMask, bool enableJustPoints, bool enableHitsFromFile, bool enableMapsHits = true, bool enableHybridHits = true, bool enableStripHits = true);
	inputFRoot(const char* name, const char* detectorFileName, unsigned short numberOfVolumesInfrontOfSTS, bool disableAutomaticDetector, bitArray detMask, int hitProducer, bool enableJustPoints = false, bool enableHitsFromFile = false, bool enableMapsHits = true, bool enableHybridHits = true, bool enableStripHits = true);

/**
 * Destructor
 */

	virtual ~inputFRoot();

/**
 * Method reads the variables based on the detector stations.
 * @param scheme is an object consisting of the detector
 */

	void readDetectorInfo(CbmStsDigiScheme* scheme = NULL);

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
