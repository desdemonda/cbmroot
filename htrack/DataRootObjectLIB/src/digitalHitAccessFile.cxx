/* *******************************************************************
// (C)opyright 2004
// 
// Institute of Computer Science V
// Prof. Männer
// University of Mannheim, Germany
// 
// *******************************************************************
// 
// Designer(s):   Steinle
// 
// *******************************************************************
// 
// Project:     Trackfinder for CBM-Project at GSI-Darmstadt, Germany
// 
// *******************************************************************
// 
// Description:
//
//   class:
//     - read and write access to a file
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-11-21 13:59:22 $
// $Revision: 1.2 $
//
// *******************************************************************/


#include "../../MiscLIB/include/errorHandling.h"
#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/digitalHit.h"
#include "../include/digitalHitAccessFile.h"


#define max(a, b)  (((a) > (b)) ? (a) : (b)) 


/* Defines the separator which is used in the file to delimit	*
 * a part of the struct which should be written in one line.	*
 * Important is that the newline character is forbidden			*
 */
#define fileDataSeparator '\t'

/*
 * default values for the initialization of each parameter
 */
#define defValName                  "digitalHit"
#define defValUsage                 "DIGITALHIT"
#define defValNumberOfEntries       0
#define defValStructureSeparator    "TABULATOR"
#define defValBlockSeparator        "NEWLINE"
#define defValContent               "Identifier"
#define defValFormat                "[Radix]x[Number]defValBlockSeparator"


/*
 * stringcommands which represent the parameters read from file
 */
#define stringCmdName               "name"
#define stringCmdUsage              "usage"
#define stringCmdNumberOfEntries    "numberOfEntries"
#define stringCmdStructureSeparator "/**/ structureSeparator"
#define stringCmdBlockSeparator     "/**/ blockSeparator"
#define stringCmdContent            "/**/ content"
#define stringCmdFormat             "/**/ format"


/*
 * index of each parameter for the commandID table
 */
#define idCmdName                   0
#define idCmdUsage                  1
#define idCmdNumberOfEntries        2


/****************************************************************
 * This method deletes the local memory if it exists.			*
 ****************************************************************/

void digitalHitAccessFile::deleteLocalMemory() {

	if (localMemory) {

/********************************************************/
/* make code changes for allocation number of data here*/
		digitalHit* typeCastedData = (digitalHit*)getDataPtr();
/********************************************************/

		if (typeCastedData != NULL) {
			delete [] typeCastedData;
			setDataNum(0);
			setDataPtr(NULL);
		}

		localMemory = false;

	}

}

/****************************************************************
 * This method returns the number of accepted commands.			*
 ****************************************************************/

unsigned int digitalHitAccessFile::getNumberOfCmds() {

	return numberOfDigitalHitAccessFileCmds;

}


/****************************************************************
 * This method assigns the value to the corresponding parameter	*
 * which is identified by a specifier.							*
 ****************************************************************/

bool digitalHitAccessFile::getHeaderValue(std::string& specifier, std::string& value) {

	bool specifierFound = false;

/********************************************************/
/* make code changes for a different configuration here */
	if (specifier.compare(stringCmdName) == 0) {
		if (!isHeaderLockSet(idCmdName)) {
			header.name = value;
			setHeaderLock(idCmdName, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdUsage) == 0) {
		if (!isHeaderLockSet(idCmdUsage)) {
			header.usage = value;
			setHeaderLock(idCmdUsage, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdNumberOfEntries) == 0) {
		if (!isHeaderLockSet(idCmdNumberOfEntries)) {
			header.numberOfEntries = stoul((char*)value.c_str(), 10);
			setHeaderLock(idCmdNumberOfEntries, true);
			specifierFound = true;
		}
	}
/********************************************************/

	return specifierFound;

}

/****************************************************************
 * This method sets the number of the data structures			*
 * to the value that all data is read from file. Normally		*
 * this value should be in the header of the file as one		*
 * command.														*
 ****************************************************************/

void digitalHitAccessFile::readAllData() {

	if ((getDataNum() == 0) && (getDataPtr() == NULL)) {

/********************************************************/
/* make code changes for allocation number of data here*/
		digitalHit* typeCastedData = new digitalHit[header.numberOfEntries];
/********************************************************/

		setDataNum(header.numberOfEntries);
		setDataPtr(typeCastedData);

		if (typeCastedData == NULL)
			throw memoryAllocationError(DATAOBJECTLIB);
		else
			localMemory = true;

	}

}

/****************************************************************
 * This method assigns the data to the corresponding structure.	*
 * The whole data structure to read must be in one single line	*
 * in the file.													*
 ****************************************************************/

bool digitalHitAccessFile::getDataValue(std::string& buffer, unsigned long index) {

	std::string       temp;
	bool              noError = true;
/********************************************************/
/* make code changes for a different data object here	*/
	digitalHit* typeCastedData = (digitalHit*) getDataPtr();
/********************************************************/

/********************************************************/
/* make code changes for a different data object here	*/
	temp = getDataString(buffer, 0, fileDataSeparator);
	if (temp.empty())
		noError = false;
	else
		typeCastedData[index].setHitIndexString(temp);
	temp = getDataString(buffer, 1, fileDataSeparator);
	if (temp.empty())
		noError = false;
	else
		typeCastedData[index].setStationIndexString(temp);
/********************************************************/
	return noError;

}

/****************************************************************
 * This method writes one data structure to the stream.			*
 * The whole data structure to write must be in one single line	*
 * in the file.													*
 ****************************************************************/

std::string digitalHitAccessFile::setDataValue(unsigned long index) {

	std::string returnValue;
/********************************************************/
/* make code changes for a different data object here	*/
	digitalHit* typeCastedData = (digitalHit*) getDataPtr();
/********************************************************/

/********************************************************/
/* make code changes for a different data object here	*/
	returnValue  = typeCastedData[index].getHitIndexString();
	returnValue += fileDataSeparator;
	returnValue += typeCastedData[index].getStationIndexString();
/********************************************************/

	return returnValue;

}

/****************************************************************
 * This method is to write the header of the file.				*
 * Dependencies:												*
 * - Fileio::setHeaderValue										*
 ****************************************************************/

void digitalHitAccessFile::writeFileHeader(std::ofstream& fileStream, terminalSequence* statusSequence) {

/********************************************************/
/* make code changes for a different configuration here */
	setHeaderValue(fileStream, stringCmdName,               header.name,              "Name of the look up table", statusSequence);
	setHeaderValue(fileStream, stringCmdUsage,              header.usage,             "Usage of the look up table", statusSequence);
	setHeaderValue(fileStream, stringCmdNumberOfEntries,    header.numberOfEntries,   "Number of entries in the look up table", statusSequence);
	setHeaderValue(fileStream, stringCmdBlockSeparator,     defValBlockSeparator,     "The separator for the blocks of the data", statusSequence);
	setHeaderValue(fileStream, stringCmdContent,            defValContent,            "The content of the file", statusSequence);
	setHeaderValue(fileStream, stringCmdFormat,             defValFormat,             "The format for an entry in the file", statusSequence);
/********************************************************/

}

/****************************************************************
 * This method is to set the default values for each header		*
 * parameter.													*
 ****************************************************************/

void digitalHitAccessFile::setHeaderDefValues() {

/********************************************************/
/* make code changes for a different configuration here */
	header.name            = defValName;
	header.usage           = defValUsage;
	header.numberOfEntries = defValNumberOfEntries;
/********************************************************/

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

digitalHitAccessFile::digitalHitAccessFile() : io(), header(), localMemory(false) {

  //	localMemory = false;

	init();

}

/****************************************************************
 * destructor													*
 ****************************************************************/

digitalHitAccessFile::~digitalHitAccessFile() {

	if (localMemory)
		deleteLocalMemory();

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void digitalHitAccessFile::init() {

	if (localMemory)
		deleteLocalMemory();

	resetHeader();

}

/****************************************************************
 * This method returns a reference of the data struct.			*
 ****************************************************************/

digitalHitAccessFileHeader& digitalHitAccessFile::getHeaderReference() {

	return header;

}

/****************************************************************
 * This method sets the data struct	to the existing one.		*
 ****************************************************************/

void digitalHitAccessFile::setHeader(digitalHitAccessFileHeader& structure) {

	header.name                     = structure.name;
	header.usage                    = structure.usage;
	header.numberOfEntries          = structure.numberOfEntries;
	setHeaderLock(idCmdName, true);
	setHeaderLock(idCmdUsage, true);
	setHeaderLock(idCmdNumberOfEntries, true);

}

/****************************************************************
 * This method returns an information string about the object.	*
 ****************************************************************/

std::string digitalHitAccessFile::getInfo() {

	std::string message;
	char        longBuffer[longConversionDigits + 1];

/********************************************************/
/* make code changes for a different information here	*/
	message += "Setup for the file access:\n";
	message += "------------------------------------------\n";
	message += "Filename";
	message += "\t: ";
	message += getFileName();
	message += "\n";
	message += "Data-pointer";
	message += "\t: ";
	if (getDataPtr() == NULL)
		message += "FAILED ; ADDRESS = NULL";
	else {
		message += "OK ; ADDRESS = ";
		sprintf(longBuffer, "%p", getDataPtr());
		message += longBuffer;
	}
	message += "\n";
	message += "Usage";
	message += "\t";
	message += header.usage;
	message += "\n";
	message += "Number of Data";
	message += "\t: ";
	ultos(getDataNum(), longBuffer, 10, longConversionDigits);
	message += longBuffer;
	message += "\n";
	message += "\n";
	message += "Setup for the pre-look-up-table file:\n";
	message += "------------------------------------------\n";
	message += "CommandID";
	message += "\t: ";
	for (unsigned int i = 0; i < getNumberOfCmds(); i++) {
		btods(isHeaderLockSet(i), longBuffer);
		message += longBuffer;
	}
	message += "\n";
	message += stringCmdBlockSeparator;
	message += "\t: ";
	message += defValBlockSeparator;
	message += "\n";
	message += stringCmdContent;
	message += "\t: ";
	message += defValContent;
	message += "\n";
	message += stringCmdFormat;
	message += "\t: ";
	message += defValFormat;
	message += "\n";
/********************************************************/

	return message;

}
