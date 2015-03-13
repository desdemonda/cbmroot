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
//     - class for the digital hit look-up-table implemented with a file
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-10-24 16:41:18 $
// $Revision: 1.5 $
//
// *******************************************************************/


#include "../../DataRootObjectLIB/include/digitalHitAccessFile.h"
#include "../include/lutGeneratorError.h"
#include "../include/lutGeneratorWarningMsg.h"
#include "../include/digitalHitAccess.h"
#include <fstream>

#ifdef __APPLE__
#include <malloc/malloc.h>
#else
#include <malloc.h>
#endif

#define standardUsage "DIGITALHIT"


/****************************************************************
 * This method allocates new memory.							*
 ****************************************************************/
	
void digitalHitAccess::allocateNewMemory(unsigned long number) {

	numberOfEntries += number;

	if (memory == NULL)
		memory = (digitalHit*)calloc(numberOfEntries, sizeof(digitalHit));
	else
		memory = (digitalHit*)realloc(memory, numberOfEntries * sizeof(digitalHit));

	if (memory == NULL)
		throw memoryAllocationError();

}

/****************************************************************
 * Default Constructor											*
 ****************************************************************/

digitalHitAccess::digitalHitAccess() 
  : memory(NULL),
    numberOfEntries(0)
{

  //	memory = NULL;

	clear();

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

digitalHitAccess::~digitalHitAccess() {

	clear();

}

/****************************************************************
 * Method inits the variables based on the detector stations.	*
 ****************************************************************/

void digitalHitAccess::init() {

	clear();

}

/****************************************************************
 * This method clears the prelut table.							*
 ****************************************************************/
	
void digitalHitAccess::clear() {

	if (memory != NULL) {
		free(memory);
		memory = NULL;
	}

	numberOfEntries = 0;

}

/****************************************************************
 * method returns the number of entries							*
 ****************************************************************/

unsigned long digitalHitAccess::getNumberOfEntries() {

	return numberOfEntries;

}
unsigned long digitalHitAccess::getNumberOfMembers() {

	return getNumberOfEntries();

}

/****************************************************************
 * This method returns the value from the prelut table.			*
 ****************************************************************/

digitalHit digitalHitAccess::getEntry(unsigned long index) {

	digitalHit returnValue;

	if (index < numberOfEntries) {

		if (memory == NULL)
			throw cannotAccessDigitalHitLutError();

		returnValue = memory[index];

	}
	else {

		throw tooBigIndexForDigitalHitLutError(index, numberOfEntries);
	
		returnValue = digitalHit();
	
	}

	return returnValue;

}
digitalHit digitalHitAccess::getMember(unsigned long index) {

	return getEntry(index);

}

/****************************************************************
 * This method adds the value at the end of the prelut table.	*
 ****************************************************************/

void digitalHitAccess::addEntry(digitalHit& value) {

	allocateNewMemory(1);

	memory[numberOfEntries - 1] = value;

}

/****************************************************************
 * This method converts the prelut table into a string.			*
 ****************************************************************/
	
std::string digitalHitAccess::toString() {

	std::string returnValue;

	returnValue.clear();

	if (numberOfEntries == 0) {

		returnValue += "\n";
		returnValue += "No entry";

	}
	else {

		if (memory == NULL)
			throw cannotAccessDigitalHitLutError();

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

void digitalHitAccess::read(std::string fileName, std::streambuf* terminal) {

	digitalHitAccessFile readFile;

	readFile.setFileName(fileName);

	readFile.readJustFileHeader();

	digitalHitAccessFileHeader& fileHeader = readFile.getHeaderReference();

	if (fileHeader.usage != standardUsage) {

		differentDigitalHitLutUsageAsFileDetectedWarningMsg* differentDigitalHitLutUsageAsFileDetected = new differentDigitalHitLutUsageAsFileDetectedWarningMsg();
		differentDigitalHitLutUsageAsFileDetected->warningMsg();
		if(differentDigitalHitLutUsageAsFileDetected != NULL) {
			delete differentDigitalHitLutUsageAsFileDetected;
			differentDigitalHitLutUsageAsFileDetected = NULL;
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

void digitalHitAccess::write(std::string fileName, std::string name, std::streambuf* terminal) {

	digitalHitAccessFileHeader fileHeader;
	digitalHitAccessFile       writeFile;

	if (numberOfEntries == 0) {

		cannotWriteEmptyDigitalHitLutWarningMsg* cannotWriteEmptyDigitalHitLut = new cannotWriteEmptyDigitalHitLutWarningMsg();
		cannotWriteEmptyDigitalHitLut->warningMsg();
		if(cannotWriteEmptyDigitalHitLut != NULL) {
			delete cannotWriteEmptyDigitalHitLut;
			cannotWriteEmptyDigitalHitLut = NULL;
		}

	}
	else {

		if (memory == NULL)
			throw cannotAccessDigitalHitLutError();

		fileHeader.name            = name;
		fileHeader.usage           = standardUsage;
		fileHeader.numberOfEntries = numberOfEntries;

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

double digitalHitAccess::getReservedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(digitalHit*);
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

double digitalHitAccess::getAllocatedSizeOfData(unsigned short dimension) {

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

double digitalHitAccess::getUsedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(digitalHit*);
	returnValue += sizeof(numberOfEntries);

	for (unsigned long i = 0; i < numberOfEntries; i++)
		returnValue += memory[i].getUsedSizeOfData(0);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}
