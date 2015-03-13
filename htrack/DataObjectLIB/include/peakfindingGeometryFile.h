//////////////////////////////////////////////////////////////////////
/// (C)opyright 2004
/// 
/// Institute of Computer Science V
/// Prof. M�nner
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
///     - read and write access to a file
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-10-24 16:36:57 $
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _PEAKFINDINGGEOMETRYFILE_H
#define _PEAKFINDINGGEOMETRYFILE_H


#include "../../FileioLIB/include/io.h"


#define numberOfPeakfindingGeometryFileCmds 3		/**< Definition of the number of parameters gettable from file. */


/*
 * struct consisting of all parameters which should be read from file
 */
struct peakfindingGeometryFileHeader {
peakfindingGeometryFileHeader() : name(), numberOfEntries(0), percentageOfCoverage(0) {};
	std::string    name;					/**< Variable to store the name of the peakfinding geometry */
	unsigned long  numberOfEntries;			/**< Variable to store the number of entries of the lut */
	unsigned short percentageOfCoverage;	/**< Variable to store the percentage of coverage for the peak finding geometry. */
};


/* **************************************************************
 * CLASS peakfindingGeometryFile				 				*
 * **************************************************************/

class peakfindingGeometryFile : public io {

private:

	peakfindingGeometryFileHeader header;											/**< Struct to store all parameters. */
	bool                          localMemory;										/**< Variable to store if the memory to read the file is local memory or not. */

protected:

/**
 * This method deletes the local memory if it exists.
 */

	void deleteLocalMemory();

/**
 * This method returns the number of accepted commands.
 */

	unsigned int getNumberOfCmds();

/**
 * This method assigns the value to the corresponding parameter
 * which is identified by a specifier.
 */

	bool getHeaderValue(std::string& specifier, std::string& value);

/**
 * This method sets the number of the data structures
 * to the value that all data is read from file. Normally
 * this value should be in the header of the file as one
 * command.
 */

	void readAllData();

/**
 * This method assigns the data to the corresponding structure
 * The whole data structure to read must be in one single line
 * in the file.
 */

	bool getDataValue(std::string& buffer, unsigned long index);

/**
 * This method writes one data structure to the stream.
 * The whole data structure to write must be in one single line
 * in the file.
 */

	std::string setDataValue(unsigned long index);

/**
 * This method is to write the header of the file.
 * @param fileStream is a stream to which the data is written
 * @param statusSequence is an object to place the process information
 */

	void writeFileHeader(std::ofstream& fileStream, terminalSequence* statusSequence = NULL);

/**
 * This method is to set the default values for each header
 * parameter.
 */

	void setHeaderDefValues();

public:

/**
 * Default constructor
 */
	
	peakfindingGeometryFile();

/**
 * Destructor
 */

	virtual ~peakfindingGeometryFile();

/**
 * This method initializes the object.
 */

	void init();

/**
 * This method returns a reference of the data struct.
 */

	peakfindingGeometryFileHeader& getHeaderReference();

/**
 * This method sets the data struct	to the existing one.
 */

	void setHeader(peakfindingGeometryFileHeader& structure);

/**
 * This method returns an information string about the object.
 */

	std::string getInfo();

};

#endif
