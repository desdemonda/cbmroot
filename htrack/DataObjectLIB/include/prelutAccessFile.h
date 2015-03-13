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
///     - read and write access to a file
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-10-24 16:36:57 $
/// $Revision: 1.7 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _PRELUTACCESSFILE_H
#define _PRELUTACCESSFILE_H


#include "../../FileioLIB/include/io.h"


#define numberOfPrelutAccessFileCmds 8		/**< Definition of the number of parameters gettable from file. */


/*
 * struct consisting of all parameters which should be read from file
 */
struct prelutAccessFileHeader {

prelutAccessFileHeader() : name(), usage(), numberOfEntries(0), dimMin(0.), dimMax(0.), dimStep(0), dimStartEntry(0.), dimStopEntry(0.) {};
	std::string   name;						/**< Variable to store the name of the lut */
	std::string   usage;					/**< Variable to store the usage of the lut */
	unsigned long numberOfEntries;			/**< Variable to store the number of entries of the lut */
	double        dimMin;					/**< Variable to store the minimal value for the third dimension to compute the prelut value. */
	double        dimMax;					/**< Variable to store the maximal value for the third dimension to compute the prelut value. */
	int           dimStep;					/**< Variable to store the stepwidth of the third dimension. */
	double        dimStartEntry;			/**< Variable to store the minimal starting entry in the third dimension. */
	double        dimStopEntry;				/**< Variable to store the maximal stop entry in the third dimension. */
};


/* **************************************************************
 * CLASS prelutAccessFile						 				*
 * **************************************************************/

class prelutAccessFile : public io {

private:

	prelutAccessFileHeader header;											/**< Struct to store all parameters. */
	bool                   localMemory;										/**< Variable to store if the memory to read the file is local memory or not. */

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
	
	prelutAccessFile();

/**
 * Destructor
 */

	virtual ~prelutAccessFile();

/**
 * This method initializes the object.
 */

	void init();

/**
 * This method returns a reference of the data struct.
 */

	prelutAccessFileHeader& getHeaderReference();

/**
 * This method sets the data struct	to the existing one.
 */

	void setHeader(prelutAccessFileHeader& structure);

/**
 * This method returns an information string about the object.
 */

	std::string getInfo();

};

#endif
