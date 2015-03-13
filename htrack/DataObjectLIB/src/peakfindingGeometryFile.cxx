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
// $Date: 2008-10-24 16:36:58 $
// $Revision: 1.3 $
//
// *******************************************************************/


#include "../../MiscLIB/include/errorHandling.h"
#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/peakfindingGeometryElement.h"
#include "../include/peakfindingGeometryFile.h"


#define max(a, b)  (((a) > (b)) ? (a) : (b)) 


/* Defines the separator which is used in the file to delimit	*
 * a part of the struct which should be written in one line.	*
 * Important is that the newline character is forbidden			*
 */
#define fileDataSeparator '\t'

/*
 * default values for the initialization of each parameter
 */
#define defValName                  "peakfinding geometry"
#define defValNumberOfEntries       0
#define defValPercentageOfCoverage  100
#define defValStructureSeparator    "TABULATOR"
#define defValBlockSeparator        "NEWLINE"
#define defValContent               "Position of the peakfinding geometry element & frequency of occurence during automatic generation"
#define defValFormat                "[Radix]x[Number]" defValStructureSeparator "[Radix]x[Number]" defValStructureSeparator "[Radix]x[Number]" defValStructureSeparator "[Radix]x[Number]" defValBlockSeparator


/*
 * stringcommands which represent the parameters read from file
 */
#define stringCmdName                 "name"
#define stringCmdNumberOfEntries      "numberOfEntries"
#define stringCmdPercentageOfCoverage "percentageOfCoverage"
#define stringCmdStructureSeparator   "/**/ structureSeparator"
#define stringCmdBlockSeparator       "/**/ blockSeparator"
#define stringCmdContent              "/**/ content"
#define stringCmdFormat               "/**/ format"


/*
 * index of each parameter for the commandID table
 */
#define idCmdName                   0
#define idCmdNumberOfEntries        1
#define idCmdPercentageOfCoverage   2


/****************************************************************
 * This method deletes the local memory if it exists.			*
 ****************************************************************/

void peakfindingGeometryFile::deleteLocalMemory() {

	if (localMemory) {

/********************************************************/
/* make code changes for allocation number of data here*/
		peakfindingGeometryElement* typeCastedData = (peakfindingGeometryElement*)getDataPtr();
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

unsigned int peakfindingGeometryFile::getNumberOfCmds() {

	return numberOfPeakfindingGeometryFileCmds;

}

/****************************************************************
 * This method assigns the value to the corresponding parameter	*
 * which is identified by a specifier.							*
 ****************************************************************/

bool peakfindingGeometryFile::getHeaderValue(std::string& specifier, std::string& value) {

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
	else if (specifier.compare(stringCmdNumberOfEntries) == 0) {
		if (!isHeaderLockSet(idCmdNumberOfEntries)) {
			header.numberOfEntries = stoul((char*)value.c_str(), 10);
			setHeaderLock(idCmdNumberOfEntries, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdPercentageOfCoverage) == 0) {
		if (!isHeaderLockSet(idCmdPercentageOfCoverage)) {
			header.percentageOfCoverage = stous((char*)value.c_str(), 10);
			if (header.percentageOfCoverage > 100)
				header.percentageOfCoverage = 100;
			setHeaderLock(idCmdPercentageOfCoverage, true);
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

void peakfindingGeometryFile::readAllData() {

	if ((getDataNum() == 0) && (getDataPtr() == NULL)) {

/********************************************************/
/* make code changes for allocation number of data here*/
		peakfindingGeometryElement* typeCastedData = new peakfindingGeometryElement[header.numberOfEntries];
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

bool peakfindingGeometryFile::getDataValue(std::string& buffer, unsigned long index) {

	std::string       temp;
	bool              noError = true;
/********************************************************/
/* make code changes for a different data object here	*/
	peakfindingGeometryElement* typeCastedData = (peakfindingGeometryElement*) getDataPtr();
/********************************************************/

/********************************************************/
/* make code changes for a different data object here	*/
	temp = getDataString(buffer, 0, fileDataSeparator);
	if (temp.empty())
		noError = false;
	else
		typeCastedData[index].setString(temp, DIM1);
	temp = getDataString(buffer, 1, fileDataSeparator);
	if (temp.empty())
		noError = false;
	else
		typeCastedData[index].setString(temp, DIM2);
	temp = getDataString(buffer, 2, fileDataSeparator);
	if (temp.empty())
		noError = false;
	else
		typeCastedData[index].setString(temp, DIM3);
	temp = getDataString(buffer, 3, fileDataSeparator);
	if (temp.empty())
		noError = false;
	else
		typeCastedData[index].setValueString(temp);
/********************************************************/
	return noError;

}

/****************************************************************
 * This method writes one data structure to the stream.			*
 * The whole data structure to write must be in one single line	*
 * in the file.													*
 ****************************************************************/

std::string peakfindingGeometryFile::setDataValue(unsigned long index) {

	std::string returnValue;
/********************************************************/
/* make code changes for a different data object here	*/
	peakfindingGeometryElement* typeCastedData = (peakfindingGeometryElement*) getDataPtr();
/********************************************************/

/********************************************************/
/* make code changes for a different data object here	*/
	returnValue  = typeCastedData[index].getString(DIM1);
	returnValue += fileDataSeparator;
	returnValue += typeCastedData[index].getString(DIM2);
	returnValue += fileDataSeparator;
	returnValue += typeCastedData[index].getString(DIM3);
	returnValue += fileDataSeparator;
	returnValue += typeCastedData[index].getValueString();
/********************************************************/

	return returnValue;

}

/****************************************************************
 * This method is to write the header of the file.				*
 * Dependencies:												*
 * - Fileio::setHeaderValue										*
 ****************************************************************/

void peakfindingGeometryFile::writeFileHeader(std::ofstream& fileStream, terminalSequence* statusSequence) {

/********************************************************/
/* make code changes for a different configuration here */
	setHeaderValue(fileStream, stringCmdName,                 header.name,                 "Name of the look up table", statusSequence);
	setHeaderValue(fileStream, stringCmdNumberOfEntries,      header.numberOfEntries,      "Number of entries in the look up table", statusSequence);
	setHeaderValue(fileStream, stringCmdPercentageOfCoverage, header.percentageOfCoverage, "Percentage of coverage for the peak finding geometry", statusSequence);
	setHeaderValue(fileStream, stringCmdStructureSeparator,   defValStructureSeparator,    "The separator for the members of the data structure", statusSequence);
	setHeaderValue(fileStream, stringCmdBlockSeparator,       defValBlockSeparator,        "The separator for the blocks of the data", statusSequence);
	setHeaderValue(fileStream, stringCmdContent,              defValContent,               "The content of the file", statusSequence);
	setHeaderValue(fileStream, stringCmdFormat,               defValFormat,                "The format for an entry in the file", statusSequence);
/********************************************************/

}

/****************************************************************
 * This method is to set the default values for each header		*
 * parameter.
 ****************************************************************/

void peakfindingGeometryFile::setHeaderDefValues() {

/********************************************************/
/* make code changes for a different configuration here */
	header.name                 = defValName;
	header.numberOfEntries      = defValNumberOfEntries;
	header.percentageOfCoverage = defValPercentageOfCoverage;
/********************************************************/

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

peakfindingGeometryFile::peakfindingGeometryFile() : io(), header(), localMemory(false) {

  //	localMemory = false;

	init();

}

/****************************************************************
 * destructor													*
 ****************************************************************/

peakfindingGeometryFile::~peakfindingGeometryFile() {

	if (localMemory)
		deleteLocalMemory();

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void peakfindingGeometryFile::init() {

	if (localMemory)
		deleteLocalMemory();

	resetHeader();

}

/****************************************************************
 * This method returns a reference of the data struct.			*
 ****************************************************************/

peakfindingGeometryFileHeader& peakfindingGeometryFile::getHeaderReference() {

	return header;

}

/****************************************************************
 * This method sets the data struct	to the existing one.		*
 ****************************************************************/

void peakfindingGeometryFile::setHeader(peakfindingGeometryFileHeader& structure) {

	header.name                          = structure.name;
	header.numberOfEntries               = structure.numberOfEntries;
	header.percentageOfCoverage          = structure.percentageOfCoverage;
	setHeaderLock(idCmdName, true);
	setHeaderLock(idCmdNumberOfEntries, true);
	setHeaderLock(idCmdPercentageOfCoverage, true);

}

/****************************************************************
 * This method returns an information string about the object.	*
 ****************************************************************/

std::string peakfindingGeometryFile::getInfo() {

	std::string message;
	char        shortBuffer[shortConversionDigits + 1];
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
	message += "Number of Data";
	message += "\t: ";
	ultos(getDataNum(), longBuffer, 10, longConversionDigits);
	message += longBuffer;
	message += "\n";
	message += stringCmdPercentageOfCoverage;
	message += "\t: ";
	ustos(header.percentageOfCoverage, shortBuffer, shortConversionDigits);
	message += shortBuffer;
	message += "\n";
	message += "\n";
	message += "Setup for the peakfinding geometry file:\n";
	message += "------------------------------------------\n";
	message += "CommandID";
	message += "\t: ";
	for (unsigned int i = 0; i < getNumberOfCmds(); i++) {
		btods(isHeaderLockSet(i), longBuffer);
		message += longBuffer;
	}
	message += "\n";
	message += stringCmdStructureSeparator;
	message += "\t: ";
	message += defValStructureSeparator;
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
