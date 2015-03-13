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
/// $Revision: 1.7 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _INPUTASCII_H
#define _INPUTASCII_H


#include "inputData.h"
#include <fstream>


/* **************************************************************
 * CLASS inputAscii								 				*
 * **************************************************************/

class inputAscii : public inputData {

protected:

	bool          dataValidation;	/**< Variable to store the goodness of the input data. */
	std::ifstream sourceFile;		/**< Variable to store the stream to the input file. */

public:

/**
 * class definition which is needed for TObject from root.
 */

	ClassDef(inputAscii, 1);

/**
 * Default constructor
 */

	inputAscii();

/**
 * Constructor
 * @param name is the name of the input file to read the data
 * @param detMask is an object representing the mask for each detector station
 * @param hitProducer defines the way to get hits from points
 */

	inputAscii(const char* name, bitArray detMask, int hitProducer);

/**
 * Destructor
 */

	virtual ~inputAscii();

/**
 * method initializes the object.
 * @param name is the name of the input file to read the data
 * @param detMask is an object representing the mask for each detector station
 * @param hitProducer defines the way to get hits from points
 */

	void init();
	void init(const char* name, int hitProducer);
	void init(const char* name, bitArray detMask, int hitProducer);

/**
 * Method reads the variables based on the detector stations.
 * @param scheme is an object consisting of the detector
 */

	void readDetectorInfo(CbmStsDigiScheme* scheme = NULL);

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
