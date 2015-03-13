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
//     - consists of the hits and a special addon for the special memory
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:33:08 $
// $Revision: 1.3 $
//
// *******************************************************************/


#include "../../MiscLIB/include/errorHandling.h"
#include "../include/inputHitSpecialArray.h"


#define min(a, b)  (((a) < (b)) ? (a) : (b)) 
#define max(a, b)  (((a) > (b)) ? (a) : (b)) 


/****************************************************************
 * Default constructor											*
 ****************************************************************/

inputHitSpecialArray::inputHitSpecialArray() : specialAddArray(NULL), numberOfEntries(0), activeObjectPointer() {

  //	specialAddArray = NULL;
  //	numberOfEntries = 0;

	resetActiveObject();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

inputHitSpecialArray::inputHitSpecialArray(const inputHitSpecialArray& value) 
  : specialAddArray(NULL), numberOfEntries(0), activeObjectPointer() 
{

	unsigned short numberOfCopyingEntries = value.numberOfEntries;

	if (numberOfCopyingEntries > minimalArraySize) {
		this->specialAddArray = (trackfinderInputHit**)calloc((numberOfCopyingEntries - minimalArraySize), sizeof(trackfinderInputHit*));
		if (this->specialAddArray == NULL)
			throw memoryAllocationError(DATAROOTOBJECTLIB);
	}

	for (unsigned short i = 0; i < numberOfCopyingEntries; i++) {

		if (i < minimalArraySize)
			this->specialArray[i]    = value.specialArray[i];
		else
			this->specialAddArray[i] = value.specialAddArray[i];

	}
	
	this->numberOfEntries            = numberOfCopyingEntries;
	this->resetActiveObject();

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

inputHitSpecialArray::~inputHitSpecialArray() {

	if (specialAddArray != NULL) {
		free(specialAddArray);
		specialAddArray = NULL;
	}

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const inputHitSpecialArray& inputHitSpecialArray::operator = (const inputHitSpecialArray& value) {

	unsigned short i;
	unsigned short numberOfCopyingEntries = value.numberOfEntries;

	clear();

	if (numberOfCopyingEntries > minimalArraySize) {
		this->specialAddArray    = (trackfinderInputHit**)calloc((numberOfCopyingEntries - minimalArraySize), sizeof(trackfinderInputHit*));
		if (this->specialAddArray == NULL)
			throw memoryAllocationError(DATAROOTOBJECTLIB);
	}

	for (i = 0; i < min(minimalArraySize, numberOfCopyingEntries); i++)
		this->specialArray[i]    = value.specialArray[i];

	for (i = 0; i < max(numberOfCopyingEntries - minimalArraySize, 0); i++)
		this->specialAddArray[i] = value.specialAddArray[i];
	
	this->numberOfEntries        = numberOfCopyingEntries;
	this->resetActiveObject();

	return *this;

}

/****************************************************************
 * sets the activeObject to the initial value					*
 ****************************************************************/

void inputHitSpecialArray::resetActiveObject() {

	activeObjectPointer = 0;

}

/****************************************************************
 * method clears the memory										*
 ****************************************************************/

void inputHitSpecialArray::clear() {

	if (specialAddArray != NULL) {
		free(specialAddArray);
		specialAddArray = NULL;
	}
	numberOfEntries = 0;
	resetActiveObject();

}

/****************************************************************
 * method returns the number of entries							*
 ****************************************************************/

unsigned short inputHitSpecialArray::getNumberOfEntries() {

	return numberOfEntries;

}

/****************************************************************
 * method pushes the element at the end of the memory.			*
 ****************************************************************/

bool inputHitSpecialArray::push(trackfinderInputHit* hit) {

	bool           returnValue;
	bool           alreadyInMem;
	unsigned short i;

	if (numberOfEntries == (unsigned short)-1)
		returnValue = false;
	else {

		returnValue = true;

		alreadyInMem = false;
		for (i = 0; i < min(minimalArraySize, numberOfEntries); i++) {
			if (specialArray[i] == hit) {
				alreadyInMem = true;
				break;
			}
		}
		for (i = 0; i < max(numberOfEntries - minimalArraySize, 0); i++) {
			if (specialAddArray[i] == hit) {
				alreadyInMem = true;
				break;
			}
		}

		if (!alreadyInMem) {

			activeObjectPointer = numberOfEntries;
			numberOfEntries++;

			if (activeObjectPointer < minimalArraySize) {
				specialArray[activeObjectPointer] = hit;
			}
			else {
				if (numberOfEntries == minimalArraySize + 1)
					specialAddArray = (trackfinderInputHit**)calloc(numberOfEntries - minimalArraySize, sizeof(trackfinderInputHit*));
				else {
					specialAddArray = (trackfinderInputHit**)realloc(specialAddArray, (numberOfEntries - minimalArraySize) * sizeof(trackfinderInputHit*));
				}
				if (specialAddArray == NULL)
					throw memoryAllocationError(DATAROOTOBJECTLIB);
				specialAddArray[activeObjectPointer - minimalArraySize] = hit;
			}

		}

	}

	return returnValue;

}

/****************************************************************
 * returns a reference of the activeObject and set the next one	*
 * to the active object.										*
 ****************************************************************/

trackfinderInputHit* inputHitSpecialArray::readActiveObjectAndMakeNextOneActive() {

	trackfinderInputHit* returnValue;

	if (activeObjectPointer < minimalArraySize)
		returnValue = specialArray[activeObjectPointer];
	else
		returnValue = specialAddArray[activeObjectPointer - minimalArraySize];

	activeObjectPointer++;
	if (activeObjectPointer == numberOfEntries)
		resetActiveObject();

	return returnValue;

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double inputHitSpecialArray::getReservedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = minimalArraySize * sizeof(trackfinderInputHit*);
	returnValue += sizeof(specialAddArray);
	returnValue += sizeof(numberOfEntries);
	returnValue += sizeof(activeObjectPointer);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double inputHitSpecialArray::getAllocatedSizeOfData(unsigned short dimension) {

	double returnValue;

	if (numberOfEntries > minimalArraySize)
		returnValue  = (numberOfEntries - minimalArraySize) * sizeof(trackfinderInputHit*);
	else
		returnValue  = 0;

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double inputHitSpecialArray::getUsedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = minimalArraySize * sizeof(trackfinderInputHit*);
	returnValue += sizeof(specialAddArray);
	if (numberOfEntries > minimalArraySize)
		returnValue += (numberOfEntries - minimalArraySize) * sizeof(trackfinderInputHit*);
	returnValue += sizeof(numberOfEntries);
	returnValue += sizeof(activeObjectPointer);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}
