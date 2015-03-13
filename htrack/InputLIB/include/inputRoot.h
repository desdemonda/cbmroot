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
/// $Date: 2008-10-07 10:38:09 $
/// $Revision: 1.9 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _INPUTROOT_H
#define _INPUTROOT_H


#include "../../MiscLIB/include/defs.h"
#if (ARCHITECTURE == STANDALONE)
	#include "../../RootFrameworkLIB/include/rootManager.h"
#elif (ARCHITECTURE == CBMROOT)
	#include "FairRootManager.h"
#endif
#include "inputData.h"
#include <string>


#define ASCIIVERSION       "v05a"				/**< Defines the version of the detector geometry files */
#define STSVACUUMFODLER    "pipevac1_1"			/**< Defines the name of the folder to read the STS stations which are in the vacuum */
#define STSPOINTBRANCH     "StsPoint"			/**< Defines the name of the branch consisting of the points for all STS stations */
#define STSMAPSHITBRANCH   "StsMapsHit"			/**< Defines the name of the branch consisting of the hits for all STS maps stations */
#define STSHYBRIDHITBRANCH "StsHybridHit"		/**< Defines the name of the branch consisting of the hits for all STS hybrid stations */
#define STSSTRIPHITBRANCH  "StsStripHit"		/**< Defines the name of the branch consisting of the hits for all STS strip stations */
#define STSHITBRANCH       "StsHit"				/**< Defines the name of the branch consisting of the hits for all STS stations */
#define MCTRACKBRANCH      "MCTrack"			/**< Defines the name of the branch consisting of the tracks for all STS stations */
#define MVDPOINTBRANCH     "MvdPoint"			/**< Defines the name of the branch consisting of the points for all MVD stations */
#define MVDHITBRANCH       "MvdHit"				/**< Defines the name of the branch consisting of the hits for all MVD stations */


#if (ARCHITECTURE == STANDALONE)
	#define NEWVOLUMEFORMAT false
#elif (ARCHITECTURE == CBMROOT)
	#define NEWVOLUMEFORMAT true
#else
	#undef NEWVOLUMEFORMAT
#endif


/* **************************************************************
 * CLASS inputRoot								 				*
 * **************************************************************/

class inputRoot : public inputData {

 private:
  inputRoot(const inputRoot&);
  inputRoot& operator=(const inputRoot&);

protected:

	TClonesArray*  inputStsPoints;				/**< Object to store all STS points which are read from file. */
	TClonesArray*  inputMvdPoints;				/**< Object to store all MVD points which are read from file. */
	TClonesArray*  inputMapsHits;				/**< Object to store all hits from the maps stations which are read from file. */
	TClonesArray*  inputHybridHits;				/**< Object to store all hits from the hybrid stations which are read from file. */
	TClonesArray*  inputStripHits;				/**< Object to store all hits from the strip stations which are read from file. */
	TClonesArray*  inputMvdHits;				/**< Object to store all hits from the MVD stations which are read from file. */
	TClonesArray*  inputStsHits;				/**< Object to store all hits from the STS stations which are read from file. */
	TClonesArray*  inputTracks;					/**< Object to store all tracks which are read from file. */
	std::string    detectorFileName;			/**< Variable to store the name of the file which consists of the detector's geometry definition ascii file. */
	unsigned short numberOfVolumesInfrontOfSTS; /**< Variable to store the number of geometric volumes which are established in front of the STS volumes. Needed to compute the correct volumeID. */
	std::string    geometryFileDirectory;		/**< Variable to store the name of the directory which consists of the geometry definition ascii files. */
	std::string    topnode;						/**< Variable to store the name of the topnode of the geoManager. */
	bool           hitsProduced;				/**< Variable to store, if the hits are produced from the points.*/
	bool           readPointsFromFile;			/**< Variable to store, if the points should be read out of a file. */
	bool           readHitsFromFile;			/**< Variable to store, if the hits should be read out of a file. */
	bool           readMapsHits;				/**< Variable to store, if the hits from maps should be accessed. */
	bool           readHybridHits;				/**< Variable to store, if the hits from hybrids should be accessed. */
	bool           readStripHits;				/**< Variable to store, if the hits from strips should be accessed. */

/**
 * Method generates the name of the actual file to read.
 */

	std::string generateFileName(std::string name);

/**
 * Method generates the name of the actual file to read.
 */

	std::string generateMvdStationFileName(unsigned int stationNumber);
	std::string generateStsStationFileName(unsigned int stationNumber);

/**
 * Method generates the name of the actual volume to read.
 */

	std::string generateMvdStationVolumeName(unsigned int stationNumber, bool newFormat = NEWVOLUMEFORMAT);
	std::string generateStsStationVolumeName(unsigned int stationNumber, bool newFormat = NEWVOLUMEFORMAT);

/**
 * Method reads the variables based on the detector stations.
 */

	bool readDetector();

/**
 * Method reads the variables of the detector from ascii file.
 */

	bool readDetectorFromAsciiFile();

/**
 * Method reads the variables of the detector from ascii files.
 */

	bool readDetectorFromAsciiFiles();
	bool readMvdDetectorFromAsciiFiles();
	bool readStsDetectorFromAsciiFiles();

/**
 * Method reads the variables of the detector from TGeoManager.
 */

	bool readDetectorFromTGeoManager();
	bool readMvdDetectorFromTGeoManager();
	bool readStsDetectorFromTGeoManager();

/**
 * Method initializes special parts of the object.
 */

	void initSpecial(FairRootManager* manager);

/**
 * Method reads the data from the file.
 * @param event identifies the number of the event to read
 * @param mvdHitArray is the memory for the hits
 * @param stsHitArray is the memory for the hits
 * @param mHitArray is the memory for the mapsHits
 * @param hHitArray is the memory for the hybridHits
 * @param sHitArray is the memory for the stripHits
 */

	void read(unsigned int event, TClonesArray* mvdHitArray = NULL, TClonesArray* stsHitArray = NULL, TClonesArray* mHitArray = NULL, TClonesArray* hHitArray = NULL, TClonesArray* sHitArray = NULL);

/**
 * Method returns the relative path for the geometry file.
 * Originally this function was virtual. But this will not
 * work with the CBMROOTFRAMEOWRK. I do really not know
 * why, but this is a solution.
 */

	std::string getGeometryFileRelativePath();

/**
 * Method accesses objects for the dictionarys.
 */

	virtual void addDictionaries() = 0;

public:

/**
 * class definition which is needed for TObject from root.
 */

	ClassDef(inputRoot, 1);

/**
 * Default constructor
 */

	inputRoot();

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

	inputRoot(bitArray detMask, bool enableJustPoints, bool enableHitsFromFile, bool enableMapsHits = true, bool enableHybridHits = true, bool enableStripHits = true);
	inputRoot(const char* name, bitArray detMask, int hitProducer, bool enableJustPoints = false, bool enableHitsFromFile = false, bool enableMapsHits = true, bool enableHybridHits = true, bool enableStripHits = true);
	inputRoot(const char* detectorFileName, unsigned short numberOfVolumesInfrontOfSTS, bitArray detMask, bool enableJustPoints, bool enableHitsFromFile, bool enableMapsHits = true, bool enableHybridHits = true, bool enableStripHits = true);
	inputRoot(const char* name, const char* detectorFileName, unsigned short numberOfVolumesInfrontOfSTS, bitArray detMask, int hitProducer, bool enableJustPoints = false, bool enableHitsFromFile = false, bool enableMapsHits = true, bool enableHybridHits = true, bool enableStripHits = true);

/**
 * Destructor
 */

	virtual ~inputRoot();

/**
 * method initializes the object.
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

	void init();
	void init(const char* name, int hitProducer);
	void init(const char* detectorFileName, unsigned int numberOfVolumesInfrontOfSTS);
	void init(const char* name, int hitProducer, const char* detectorFileName, unsigned int numberOfVolumesInfrontOfSTS);
	void init(bitArray detMask, bool enableJustPoints, bool enableHitsFromFile, bool enableMapsHits = true, bool enableHybridHits = true, bool enableStripHits = true);
	void init(const char* name, bitArray detMask, int hitProducer, bool enableJustPoints, bool enableHitsFromFile, bool enableMapsHits = true, bool enableHybridHits = true, bool enableStripHits = true);
	void init(const char* detectorFileName, unsigned short numberOfVolumesInfrontOfSTS, bitArray detMask, bool enableJustPoints, bool enableHitsFromFile, bool enableMapsHits = true, bool enableHybridHits = true, bool enableStripHits = true);
	void init(const char* name, const char* detectorFileName, unsigned short numberOfVolumesInfrontOfSTS, bitArray detMask, int hitProducer, bool enableJustPoints, bool enableHitsFromFile, bool enableMapsHits = true, bool enableHybridHits = true, bool enableStripHits = true);

/**
 * Method reads the variables based on the detector stations.
 * @param scheme is an object consisting of the detector
 */

	virtual void readDetectorInfo(CbmStsDigiScheme* scheme = NULL) = 0;

/**
 * method reads the source data from a file based on the
 * CbmStsMapsHit-class, CbmStsHybridHit-class and
 * CbmStsStripHit-class
 * @param event identifies the number of the event to read
 * @param mHitArray is the memory for the mapsHits
 * @param hHitArray is the memory for the hybridHits
 * @param sHitArray is the memory for the stripHits
 */

	void readDataSource(unsigned int event, TClonesArray* mHitArray, TClonesArray* hHitArray, TClonesArray* sHitArray);

/**
 * method reads the source data from a file based on the
 * CbmStsHit-class
 * @param event identifies the number of the event to read
 * @param mvdHitArray is the memory for the MVD hits
 * @param stsHitArray is the memory for the STS hits
 */

	void readDataSource(unsigned int event, TClonesArray* mvdHitArray, TClonesArray* stsHitArray);

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
