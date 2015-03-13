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
//     - reads the event information from a special file
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:36:16 $
// $Revision: 1.3 $
//
// *******************************************************************/


#include "../../DataRootObjectLIB/include/trackfinderInputHit.h"
#include "../../DataRootObjectLIB/include/trackfinderInputTrack.h"
#include "../../RootFrameworkLIB/include/hitProducer.h"
#include "../include/inputError.h"
#include "../include/inputData.h"
#include <stdlib.h>

#ifdef __APPLE__
#include <malloc/malloc.h>
#else
#include <malloc.h>
#endif

/****************************************************************
 * Default constructor											*
 ****************************************************************/

inputData::inputData()
  : TObject(),
    data(),
    detectorMask(0),
    typeOfHitProducer(NONEHITPRODUCER)
{

	data.removeDetector();
	data.removeAllHits();
	data.removeAllTracks();

	//	detectorMask      = 0;
	//	typeOfHitProducer = NONEHITPRODUCER;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

inputData::inputData(bitArray detMask) 
  : TObject(),
    data(),
    detectorMask(detMask),
    typeOfHitProducer(NONEHITPRODUCER)
{

	data.removeDetector();
	data.removeAllHits();
	data.removeAllTracks();

	//	detectorMask      = detMask;
	//	typeOfHitProducer = NONEHITPRODUCER;

}
inputData::inputData(int hitProducer) 
  : TObject(),
    data(),
    detectorMask(0),
    typeOfHitProducer(hitProducer)
{

	data.removeDetector();
	data.removeAllHits();
	data.removeAllTracks();

	//	detectorMask      = 0;
	//	typeOfHitProducer = hitProducer;

}
inputData::inputData(bitArray detMask, int hitProducer) 
  : TObject(),
    data(),
    detectorMask(detMask),
    typeOfHitProducer(hitProducer)
{

	data.removeDetector();
	data.removeAllHits();
	data.removeAllTracks();

	//	detectorMask      = detMask;
	//	typeOfHitProducer = hitProducer;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

inputData::~inputData() {

}

/****************************************************************
 * method initializes the object.								*
 ****************************************************************/

void inputData::init(bitArray detMask) {

	detectorMask = detMask;

}
void inputData::init(int hitProducer) {

	typeOfHitProducer = hitProducer;

}

/****************************************************************
 * method returns a reference to the data struct				*
 ****************************************************************/

trackfinderInputData& inputData::getInputData() {

	return data;

}

/****************************************************************
 * method returns a reference to the data struct				*
 ****************************************************************/

trackfinderInputData* inputData::getInputDataPointer() {

	return &data;

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double inputData::getReservedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue = 0;
	returnValue  = data.getReservedSizeOfData();
	returnValue += detectorMask.getReservedSizeOfData(0);
	returnValue += sizeof(typeOfHitProducer);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double inputData::getAllocatedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = data.getAllocatedSizeOfData();
	returnValue += detectorMask.getAllocatedSizeOfData(0);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double inputData::getUsedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = data.getUsedSizeOfData();
	returnValue += detectorMask.getUsedSizeOfData(0);
	returnValue += sizeof(typeOfHitProducer);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

ClassImp(inputData)
