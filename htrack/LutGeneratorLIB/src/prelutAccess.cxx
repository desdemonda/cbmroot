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
//     - derived from prelut
//     - class for the first look-up-table implemented with a file
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-11-21 14:01:50 $
// $Revision: 1.8 $
//
// *******************************************************************/


#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/lutGeneratorError.h"
#include "../include/lutGeneratorWarningMsg.h"
#include "../include/prelutAccess.h"
#include <fstream>

#ifdef __APPLE__
#include <malloc/malloc.h>
#else
#include <malloc.h>
#endif

#define standardUsage "PRELUT"


/****************************************************************
 * This method allocates new memory.							*
 ****************************************************************/
	
void prelutAccess::allocateNewMemory(unsigned long number) {

	numberOfEntries += number;

	if (memory == NULL)
		memory = (prelutHoughBorder*)calloc(numberOfEntries, sizeof(prelutHoughBorder));
	else
		memory = (prelutHoughBorder*)realloc(memory, numberOfEntries * sizeof(prelutHoughBorder));

	if (memory == NULL)
		throw memoryAllocationError();

	for (unsigned long i = numberOfEntries - number; i < numberOfEntries; i++)
		memory[i].init();

}

/****************************************************************
 * Default Constructor											*
 ****************************************************************/

prelutAccess::prelutAccess() 
  : prelut(), 
    memory(NULL),
    numberOfEntries(0)
{

  //	memory = NULL;

	clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

prelutAccess::prelutAccess(double dim3Min, double dim3Max, int dim3Step, double dim3StartEntry, double dim3StopEntry) 
  : prelut(dim3Min, dim3Max, dim3Step, dim3StartEntry, dim3StopEntry),
    memory(NULL),
    numberOfEntries(0)
 {

   //	memory = NULL;

	clear();

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

prelutAccess::~prelutAccess() {

	clear();

}

/****************************************************************
 * Method inits the variables based on the detector stations.	*
 ****************************************************************/

void prelutAccess::init(double dim3Min, double dim3Max, int dim3Step, double dim3StartEntry, double dim3StopEntry) {

	clear();
	prelut::init(dim3Min, dim3Max, dim3Step, dim3StartEntry, dim3StopEntry);

}

/****************************************************************
 * This method evaluates the value from the prelut table.		*
 ****************************************************************/

#if (ARCHITECTURE != PS3)

void prelutAccess::evaluate(trackfinderInputHit* hit, prelutHoughBorder* borderPointer) {

	digitalHit temp;

	if (hit == NULL)
		throw noHitError();

	temp.setHit(hit);

	evaluate(&temp, borderPointer);

}

#endif

void prelutAccess::evaluate(digitalHit* hit, prelutHoughBorder* borderPointer) {

	if (hit == NULL)
		throw noHitError();

	if (borderPointer != NULL)
		*borderPointer = getEntry(hit);
	else
		border         = getEntry(hit);

}

/****************************************************************
 * This method clears the prelut table.							*
 ****************************************************************/
	
void prelutAccess::clear() {

	if (memory != NULL) {
		free(memory);
		memory = NULL;
	}

	numberOfEntries = 0;

}

/****************************************************************
 * method returns the number of entries							*
 ****************************************************************/

unsigned long prelutAccess::getNumberOfEntries() {

	return numberOfEntries;

}
unsigned long prelutAccess::getNumberOfMembers() {

	return getNumberOfEntries();

}

/****************************************************************
 * This method returns the value from the prelut table.			*
 ****************************************************************/
	
prelutHoughBorder prelutAccess::getEntry(digitalHit* hit) {

	if (hit == NULL)
		throw noHitError();

	return getEntry(hit->getHitIndex());

}
prelutHoughBorder prelutAccess::getEntry(unsigned long index) {

	prelutHoughBorder returnValue;

	if (index < numberOfEntries) {

		if (memory == NULL)
			throw cannotAccessPrelutError();

		returnValue = memory[index];

	}
	else {

		throw tooBigIndexForPrelutError(index, numberOfEntries);
	
		returnValue = prelutHoughBorder();
	
	}

	return returnValue;

}
prelutHoughBorder prelutAccess::getMember(unsigned long index) {

	return getEntry(index);

}

/****************************************************************
 * This method adds the value at the end of the prelut table.	*
 ****************************************************************/
	
void prelutAccess::addEntry(prelutHoughBorder& value) {

	allocateNewMemory(1);

	memory[numberOfEntries - 1] = value;

}
void prelutAccess::addEntry(prelutHoughBorder& value, unsigned long index) {

	if (index >= numberOfEntries)
		allocateNewMemory(index + 1 - numberOfEntries);

	memory[index] = value;

}
void prelutAccess::addEntry(prelutHoughBorder& value, digitalHit hit) {

	addEntry(value, hit.getHitIndex());

}

/****************************************************************
 * This method converts the prelut table into a string.			*
 ****************************************************************/
	
std::string prelutAccess::toString() {

	std::string returnValue;

	returnValue.clear();
	returnValue += def.toString();

	if (numberOfEntries == 0) {

		returnValue += "\n";
		returnValue += "No entry";

	}
	else {

		if (memory == NULL)
			throw cannotAccessPrelutError();

		for (unsigned long i = 0; i < numberOfEntries; i++) {

			returnValue += "\n";
			returnValue += memory[i].toIdentifiedString();

		}

	}

	return returnValue;

}

/****************************************************************
 * method reads a file to get the table							*
 ****************************************************************/

void prelutAccess::read(std::string fileName, std::streambuf* terminal) {

	unsigned short   countDifferentPrelutDefinitionAsFile;
	prelutAccessFile readFile;
	std::string      definitionString;
	std::string      fileHeaderString;

	countDifferentPrelutDefinitionAsFile = 0;

	readFile.setFileName(fileName);

	readFile.readJustFileHeader();

	prelutAccessFileHeader& fileHeader = readFile.getHeaderReference();

	if (fileHeader.usage != standardUsage) {

		differentPrelutUsageAsFileDetectedWarningMsg* differentPrelutUsageAsFileDetected = new differentPrelutUsageAsFileDetectedWarningMsg();
		differentPrelutUsageAsFileDetected->warningMsg();
		if(differentPrelutUsageAsFileDetected != NULL) {
			delete differentPrelutUsageAsFileDetected;
			differentPrelutUsageAsFileDetected = NULL;
		}

	}

	dtos(def.dim3Min,       &definitionString, doubleConversionDigits);
	dtos(fileHeader.dimMin, &fileHeaderString, doubleConversionDigits);
	if (definitionString != fileHeaderString) {
		countDifferentPrelutDefinitionAsFile++;
		def.dim3Min        = fileHeader.dimMin;
	}
	dtos(def.dim3Max,       &definitionString, doubleConversionDigits);
	dtos(fileHeader.dimMax, &fileHeaderString, doubleConversionDigits);
	if (definitionString != fileHeaderString) {
		countDifferentPrelutDefinitionAsFile++;
		def.dim3Max        = fileHeader.dimMax;
	}
	if (def.dim3Step != fileHeader.dimStep) {
		countDifferentPrelutDefinitionAsFile++;
		def.dim3Step       = fileHeader.dimStep;
	}
	dtos(def.dim3StartEntry,       &definitionString, doubleConversionDigits);
	dtos(fileHeader.dimStartEntry, &fileHeaderString, doubleConversionDigits);
	if (definitionString != fileHeaderString) {
		countDifferentPrelutDefinitionAsFile++;
		def.dim3StartEntry = fileHeader.dimStartEntry;
	}
	dtos(def.dim3StopEntry,       &definitionString, doubleConversionDigits);
	dtos(fileHeader.dimStopEntry, &fileHeaderString, doubleConversionDigits);
	if (definitionString != fileHeaderString) {
		countDifferentPrelutDefinitionAsFile++;
		def.dim3StopEntry  = fileHeader.dimStopEntry;
	}

	if (countDifferentPrelutDefinitionAsFile != 0) {

		differentPrelutDefinitionAsFileDetectedWarningMsg* differentPrelutDefinitionAsFileDetected = new differentPrelutDefinitionAsFileDetectedWarningMsg(countDifferentPrelutDefinitionAsFile);
		differentPrelutDefinitionAsFileDetected->warningMsg();
		if(differentPrelutDefinitionAsFileDetected != NULL) {
			delete differentPrelutDefinitionAsFileDetected;
			differentPrelutDefinitionAsFileDetected = NULL;
		}

	}

	clear();

	allocateNewMemory(fileHeader.numberOfEntries);

	readFile.setDataNum(numberOfEntries);

	readFile.setDataPtr(memory);

	readFile.readFile(terminal);

}

/****************************************************************
 * method writes a file representing the table					*
 ****************************************************************/

void prelutAccess::write(std::string fileName, std::string name, std::streambuf* terminal) {

	prelutAccessFileHeader fileHeader;
	prelutAccessFile       writeFile;

	if (numberOfEntries == 0) {

		cannotWriteEmptyPrelutWarningMsg* cannotWriteEmptyPrelut = new cannotWriteEmptyPrelutWarningMsg();
		cannotWriteEmptyPrelut->warningMsg();
		if(cannotWriteEmptyPrelut != NULL) {
			delete cannotWriteEmptyPrelut;
			cannotWriteEmptyPrelut = NULL;
		}

	}
	else {

		if (memory == NULL)
			throw cannotAccessPrelutError();

		fileHeader.name            = name;
		fileHeader.usage           = standardUsage;
		fileHeader.numberOfEntries = numberOfEntries;
		fileHeader.dimMin          = def.dim3Min;
		fileHeader.dimMax          = def.dim3Max;
		fileHeader.dimStep         = def.dim3Step;
		fileHeader.dimStartEntry   = def.dim3StartEntry;
		fileHeader.dimStopEntry    = def.dim3StopEntry;

		writeFile.setFileName(fileName);

		writeFile.setDataNum(fileHeader.numberOfEntries);

		writeFile.setDataPtr(memory);

		writeFile.setHeader(fileHeader);

		writeFile.writeFile(terminal);

	}

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double prelutAccess::getReservedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(prelutHoughBorder*);
	returnValue += sizeof(numberOfEntries);

	for (unsigned long i = 0; i < numberOfEntries; i++)
		returnValue += memory[i].getReservedSizeOfData(0);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double prelutAccess::getAllocatedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;

	for (unsigned long i = 0; i < numberOfEntries; i++)
		returnValue += memory[i].getAllocatedSizeOfData(0);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double prelutAccess::getUsedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(prelutHoughBorder*);
	returnValue += sizeof(numberOfEntries);

	for (unsigned long i = 0; i < numberOfEntries; i++)
		returnValue += memory[i].getUsedSizeOfData(0);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}
