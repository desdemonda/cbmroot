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
// $Revision: 1.8 $
//
// *******************************************************************/


#include "../../MiscLIB/include/errorHandling.h"
#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/lutHoughBorder.h"
#include "../include/lutAccessFile.h"


#define max(a, b)  (((a) > (b)) ? (a) : (b)) 


/* Defines the separator which is used in the file to delimit	*
 * a part of the struct which should be written in one line.	*
 * Important is that the newline character is forbidden			*
 */
#define fileDataSeparator '\t'

/*
 * default values for the initialization of each parameter
 */
#define defValName                  "lut"
#define defValUsage                 "LUT"
#define defValFormat                HARDWAREFORMAT
#define defValNumberOfEntries       0
#define defValDim1Min               0
#define defValDim1Max               0
#define defValDim1Step              0
#define defValDim2Min               0
#define defValDim2Max               0
#define defValDim2Step              0
#define defValStructureSeparator    "TABULATOR"
#define defValBlockSeparator        "NEWLINE"
#define defValContentSoftware       "(pos1, pos2) ... (pos1, pos2)"
#define defValFormatSoftware        "([Radix]x[Number],[Radix]x[Number])" defValStructureSeparator "..." defValStructureSeparator "([Radix]x[Number],[Radix]x[Number])" defValBlockSeparator
#define defValContentHardware       "startPos & Command"
#define defValFormatHardware        "[Radix]x[Number]" defValStructureSeparator "[Radix]x[Number]" defValBlockSeparator


/*
 * stringcommands which represent the parameters read from file
 */
#define stringCmdName               "name"
#define stringCmdUsage              "usage"
#define stringCmdFormat             "format"
#define stringCmdDim1Min            "dim1Min"
#define stringCmdDim1Max            "dim1Max"
#define stringCmdDim1Step           "dim1Step"
#define stringCmdDim2Min            "dim2Min"
#define stringCmdDim2Max            "dim2Max"
#define stringCmdDim2Step           "dim2Step"
#define stringCmdNumberOfEntries    "numberOfEntries"
#define stringCmdStructureSeparator "/**/ structureSeparator"
#define stringCmdBlockSeparator     "/**/ blockSeparator"
#define stringCmdContent            "/**/ content"
#define stringCmdFormatString       "/**/ format"


/*
 * index of each parameter for the commandID table
 */
#define idCmdName                   0
#define idCmdUsage                  1
#define idCmdFormat                 2
#define idCmdNumberOfEntries        3
#define idCmdDim1Min                4
#define idCmdDim1Max                5
#define idCmdDim1Step               6
#define idCmdDim2Min                7
#define idCmdDim2Max                8
#define idCmdDim2Step               9


/****************************************************************
 * This method deletes the local memory if it exists.			*
 ****************************************************************/

void lutAccessFile::deleteLocalMemory() {

	if (localMemory) {

/********************************************************/
/* make code changes for allocation number of data here*/
		lutHoughBorder* typeCastedData = (lutHoughBorder*)getDataPtr();
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

unsigned int lutAccessFile::getNumberOfCmds() {

	return numberOfLutAccessFileCmds;

}

/****************************************************************
 * This method assigns the value to the corresponding parameter	*
 * which is identified by a specifier.							*
 ****************************************************************/

bool lutAccessFile::getHeaderValue(std::string& specifier, std::string& value) {

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
	else if (specifier.compare(stringCmdFormat) == 0) {
		if (!isHeaderLockSet(idCmdFormat)) {
			header.format = stous((char*)value.c_str(), 10);
			setHeaderLock(idCmdFormat, true);
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
	else if (specifier.compare(stringCmdDim1Min) == 0) {
		if (!isHeaderLockSet(idCmdDim1Min)) {
			header.dim1Min = stod((char*)value.c_str());
			setHeaderLock(idCmdDim1Min, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdDim1Max) == 0) {
		if (!isHeaderLockSet(idCmdDim1Max)) {
			header.dim1Max = stod((char*)value.c_str());
			setHeaderLock(idCmdDim1Max, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdDim1Step) == 0) {
		if (!isHeaderLockSet(idCmdDim1Step)) {
			header.dim1Step = stoi((char*)value.c_str(), 10);
			setHeaderLock(idCmdDim1Step, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdDim2Min) == 0) {
		if (!isHeaderLockSet(idCmdDim2Min)) {
			header.dim2Min = stod((char*)value.c_str());
			setHeaderLock(idCmdDim2Min, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdDim2Max) == 0) {
		if (!isHeaderLockSet(idCmdDim2Max)) {
			header.dim2Max = stod((char*)value.c_str());
			setHeaderLock(idCmdDim2Max, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdDim2Step) == 0) {
		if (!isHeaderLockSet(idCmdDim2Step)) {
			header.dim2Step = stoi((char*)value.c_str(), 10);
			setHeaderLock(idCmdDim2Step, true);
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

void lutAccessFile::readAllData() {

	if ((getDataNum() == 0) && (getDataPtr() == NULL)) {

/********************************************************/
/* make code changes for allocation number of data here*/
		lutHoughBorder* typeCastedData = new lutHoughBorder[header.numberOfEntries];
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

bool lutAccessFile::getDataValue(std::string& buffer, unsigned long index) {

	std::string       temp;
	bool              noError = true;
/********************************************************/
/* make code changes for a different data object here	*/
	lutHoughBorder*     typeCastedData = (lutHoughBorder*) getDataPtr();
	houghBorderCommand  actualCommand;
	unsigned long       coordCounter;
	houghBorderPosition actualPosition;
/********************************************************/

/********************************************************/
/* make code changes for a different data object here	*/
	switch(header.format) {
		case HARDWAREFORMAT:
			temp = getDataString(buffer, 0, fileDataSeparator);
			if (temp.empty())
				noError = false;
			else
				actualCommand.setStartPos(temp);
			temp = getDataString(buffer, 1, fileDataSeparator);
			if (temp.empty())
				noError = false;
			else
				actualCommand.setCmd(temp);
			if (noError)
				typeCastedData[index].setHoughBorderCommand(actualCommand, header.dim1Step, header.dim2Step);
			break;
		default:
			typeCastedData[index].houghCoord.clear();
			coordCounter = 0;
			while (noError) {
				temp = getDataString(buffer, coordCounter, fileDataSeparator);
				if (temp.empty())
					noError = false;
				else {
					actualPosition.fromNotIdentifiedRadixString(temp);
					typeCastedData[index].houghCoord.push(actualPosition);
					coordCounter++;
				}
			}
			if (coordCounter > 0)
				noError = true;
			break;
	}
/********************************************************/
	return noError;

}

/****************************************************************
 * This method writes one data structure to the stream.			*
 * The whole data structure to write must be in one single line	*
 * in the file.													*
 ****************************************************************/

std::string lutAccessFile::setDataValue(unsigned long index) {

	std::string         returnValue;
/********************************************************/
/* make code changes for a different data object here	*/
	lutHoughBorder*     typeCastedData = (lutHoughBorder*) getDataPtr();
	houghBorderCommand  actualCommand;
	houghBorderPosition actualPosition;
/********************************************************/

/********************************************************/
/* make code changes for a different data object here	*/
	returnValue.clear();
	switch (header.format) {
		case HARDWAREFORMAT:
			actualCommand = typeCastedData[index].getHoughBorderCommand((unsigned short)header.dim1Step);
			returnValue += actualCommand.getStartPos();
			returnValue += fileDataSeparator;
			returnValue += actualCommand.getCmd();
			break;
		default:
			typeCastedData[index].houghCoord.resetActiveObject();
			for (unsigned long i = 0; i < typeCastedData[index].houghCoord.getNumberOfEntries(); i++) {
				actualPosition   = typeCastedData[index].houghCoord.readActiveObjectAndMakeNextOneActive();
				if (i > 0)
					returnValue += fileDataSeparator;
				returnValue     += actualPosition.toNotIdentifiedRadixString();
			}
			break;
	}
/********************************************************/

	return returnValue;

}

/****************************************************************
 * This method is to write the header of the file.				*
 * Dependencies:												*
 * - Fileio::setHeaderValue										*
 ****************************************************************/

void lutAccessFile::writeFileHeader(std::ofstream& fileStream, terminalSequence* statusSequence) {

/********************************************************/
/* make code changes for a different configuration here */
	setHeaderValue(fileStream, stringCmdName,               header.name,              "Name of the look up table", statusSequence);
	setHeaderValue(fileStream, stringCmdUsage,              header.usage,             "Usage of the look up table", statusSequence);
	setHeaderValue(fileStream, stringCmdFormat,             header.format,            "Format of the look up table", statusSequence);
	setHeaderValue(fileStream, stringCmdNumberOfEntries,    header.numberOfEntries,   "Number of entries in the look up table", statusSequence);
	setHeaderValue(fileStream, stringCmdDim1Min,            header.dim1Min,           "Minimum value in the first dimension of the LUT configuration during creation", statusSequence);
	setHeaderValue(fileStream, stringCmdDim1Max,            header.dim1Max,           "Maximum value in the first dimension of the LUT configuration during creation", statusSequence);
	setHeaderValue(fileStream, stringCmdDim1Step,           header.dim1Step,          "Number of steps for the value range in the first dimension of the LUT configuration during creation", statusSequence);
	setHeaderValue(fileStream, stringCmdDim2Min,            header.dim2Min,           "Minimum value in the second dimension of the LUT configuration during creation", statusSequence);
	setHeaderValue(fileStream, stringCmdDim2Max,            header.dim2Max,           "Maximum value in the second dimension of the LUT configuration during creation", statusSequence);
	setHeaderValue(fileStream, stringCmdDim2Step,           header.dim2Step,          "Number of steps for the value range in the second dimension of the LUT configuration during creation", statusSequence);
	setHeaderValue(fileStream, stringCmdStructureSeparator, defValStructureSeparator, "The separator for the members of the data structure", statusSequence);
	setHeaderValue(fileStream, stringCmdBlockSeparator,     defValBlockSeparator,     "The separator for the blocks of the data", statusSequence);
	
	switch(header.format){
	
		case HARDWAREFORMAT:
			setHeaderValue(fileStream, stringCmdContent,            defValContentHardware,    "The content of the file", statusSequence);
			setHeaderValue(fileStream, stringCmdFormatString,       defValFormatHardware,     "The format for an entry in the file", statusSequence);
			break;

		default:
			setHeaderValue(fileStream, stringCmdContent,            defValContentSoftware,    "The content of the file", statusSequence);
			setHeaderValue(fileStream, stringCmdFormatString,       defValFormatSoftware,     "The format for an entry in the file", statusSequence);
			break;

	}
/********************************************************/

}

/****************************************************************
 * This method is to set the default values for each header		*
 * parameter.
 ****************************************************************/

void lutAccessFile::setHeaderDefValues() {

/********************************************************/
/* make code changes for a different configuration here */
	header.name            = defValName;
	header.usage           = defValUsage;
	header.format          = defValFormat;
	header.numberOfEntries = defValNumberOfEntries;
	header.dim1Min         = defValDim1Min;
	header.dim1Max         = defValDim1Max;
	header.dim1Step        = defValDim1Step;
	header.dim2Min         = defValDim2Min;
	header.dim2Max         = defValDim2Max;
	header.dim2Step        = defValDim2Step;
/********************************************************/

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

lutAccessFile::lutAccessFile() : io(), header(), localMemory(false) {

  //	localMemory = false;

	init();

}

/****************************************************************
 * destructor													*
 ****************************************************************/

lutAccessFile::~lutAccessFile() {

	if (localMemory)
		deleteLocalMemory();

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void lutAccessFile::init() {

	if (localMemory)
		deleteLocalMemory();

	resetHeader();

}

/****************************************************************
 * This method returns a reference of the data struct.			*
 ****************************************************************/

lutAccessFileHeader& lutAccessFile::getHeaderReference() {

	return header;

}

/****************************************************************
 * This method sets the data struct	to the existing one.		*
 ****************************************************************/

void lutAccessFile::setHeader(lutAccessFileHeader& structure) {

	header.name                     = structure.name;
	header.usage                    = structure.usage;
	header.format                   = structure.format;
	header.numberOfEntries          = structure.numberOfEntries;
	header.dim1Min                  = structure.dim1Min;
	header.dim1Max                  = structure.dim1Max;
	header.dim1Step                 = structure.dim1Step;
	header.dim2Min                  = structure.dim2Min;
	header.dim2Max                  = structure.dim2Max;
	header.dim2Step                 = structure.dim2Step;
	setHeaderLock(idCmdName, true);
	setHeaderLock(idCmdUsage, true);
	setHeaderLock(idCmdFormat, true);
	setHeaderLock(idCmdNumberOfEntries, true);
	setHeaderLock(idCmdDim1Min, true);
	setHeaderLock(idCmdDim1Max, true);
	setHeaderLock(idCmdDim1Step, true);
	setHeaderLock(idCmdDim2Min, true);
	setHeaderLock(idCmdDim2Max, true);
	setHeaderLock(idCmdDim2Step, true);

}

/****************************************************************
 * This method returns an information string about the object.	*
 ****************************************************************/

std::string lutAccessFile::getInfo() {

	std::string message;
	char        shortBuffer[shortConversionDigits+1];
	char        intBuffer[intConversionDigits + 1];
	char        longBuffer[longConversionDigits + 1];
	char        doubleBuffer[doubleConversion + 1];

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
	message += stringCmdFormat;
	message += "\t: ";
	ustos(header.format, shortBuffer, 10, shortConversionDigits);
	message += shortBuffer;
	message += "\n";
	message += "Number of Data";
	message += "\t: ";
	ultos(getDataNum(), longBuffer, 10, longConversionDigits);
	message += longBuffer;
	message += "\n";
	message += stringCmdDim1Min;
	message += "\t: ";
	dtos(header.dim1Min, doubleBuffer, doubleConversionDigits);
	message += doubleBuffer;
	message += "\n";
	message += stringCmdDim1Max;
	message += "\t: ";
	dtos(header.dim1Max, doubleBuffer, doubleConversionDigits);
	message += doubleBuffer;
	message += "\n";
	message += stringCmdDim1Step;
	message += "\t: ";
	itos(header.dim1Step, intBuffer, 10, intConversionDigits);
	message += intBuffer;
	message += "\n";
	message += stringCmdDim2Min;
	message += "\t: ";
	dtos(header.dim2Min, doubleBuffer, doubleConversionDigits);
	message += doubleBuffer;
	message += "\n";
	message += stringCmdDim2Max;
	message += "\t: ";
	dtos(header.dim2Max, doubleBuffer, doubleConversionDigits);
	message += doubleBuffer;
	message += "\n";
	message += stringCmdDim2Step;
	message += "\t: ";
	itos(header.dim2Step, intBuffer, 10, intConversionDigits);
	message += intBuffer;
	message += "\n";
	message += "\n";
	message += "Setup for the look-up-table file:\n";
	message += "--------------------------------------\n";
	message += "CommandID";
	message += "\t: ";
	for (unsigned int i = 0; i < getNumberOfCmds(); i++) {
		btods(isHeaderLockSet(i), longBuffer);
		message += longBuffer;
	}
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
	switch (header.format) {
		case HARDWAREFORMAT:
			message += defValContentHardware;
			break;
		default:
			message += defValContentSoftware;
			break;
	}
	message += "\n";
	message += stringCmdFormatString;
	message += "\t: ";
	switch (header.format) {
		case HARDWAREFORMAT:
			message += defValFormatHardware;
			break;
		default:
			message += defValFormatSoftware;
			break;
	}
	message += "\n";
/********************************************************/

	return message;

}
