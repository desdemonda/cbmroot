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
//     - consists of the information for the detector
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:33:08 $
// $Revision: 1.4 $
//
// *******************************************************************/


#include "../include/dataRootObjectError.h"
#include "../include/dataRootObjectWarningMsg.h"
#include "../include/trackfinderInputDetector.h"
#include <stdlib.h>


/****************************************************************
 * method which compares the detectorstation related to its		*
 * id															*
 ****************************************************************/

int compareStationId(const void* element1, const void* element2) {

	return ((trackfinderInputStation*)element1)->getId() - ((trackfinderInputStation*)element2)->getId();

}

/****************************************************************
 * method searches the detector for a specific station with id.	*
 ****************************************************************/

trackfinderInputStation* trackfinderInputDetector::searchStation(int stationId) {

	trackfinderInputStation  key;
	trackfinderInputStation* returnValue = NULL;

	if (detector != NULL) {

		key.setId(stationId);
		returnValue = (trackfinderInputStation*)bsearch((void*)(&key), (void*)detector, numberOfStations, sizeof(trackfinderInputStation), compareStationId);

	}
	else
		throw detectorIsNotDefinedError();

	if (returnValue == NULL)
		throw cannotFindStationIdError(stationId);

	return returnValue;

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackfinderInputDetector::trackfinderInputDetector() 
  : TObject(), 
    numberOfStations(0),
    numberOfActiveStations(0),
    detector(NULL)
{
  /*
	numberOfStations       = 0;
	numberOfActiveStations = 0;
	detector               = NULL;
  */
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackfinderInputDetector::trackfinderInputDetector(const trackfinderInputDetector& value) 
  : TObject(value), 
    numberOfStations(value.numberOfStations),
    numberOfActiveStations( value.numberOfActiveStations),
    detector(NULL)
{

  //	this->numberOfStations       = value.numberOfStations;
  //	this->numberOfActiveStations = value.numberOfActiveStations;
	this->detector = (trackfinderInputStation*)calloc(this->numberOfStations, sizeof(trackfinderInputStation));
	for (unsigned int i = 0; i < this->numberOfStations; i++)
		this->detector[i] = value.detector[i];

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackfinderInputDetector::~trackfinderInputDetector() {

	if (detector != NULL) {
		free(detector);
		detector = NULL;
	}

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const trackfinderInputDetector& trackfinderInputDetector::operator = (const trackfinderInputDetector& value) {

	if (this->detector != NULL) {
		free(this->detector);
		this->detector = NULL;
	}
	this->numberOfStations       = value.numberOfStations;
	this->numberOfActiveStations = value.numberOfActiveStations;
	this->detector = (trackfinderInputStation*)calloc(this->numberOfStations, sizeof(trackfinderInputStation));
	for (unsigned int i = 0; i < this->numberOfStations; i++)
		this->detector[i] = value.detector[i];

	return *this;

}

/****************************************************************
 * Method inits the default detector.							*
 ****************************************************************/

void trackfinderInputDetector::initDefaultDetector() {

	trackfinderInputStation actualStation;

	removeDetector();

	actualStation.setId(1);
	actualStation.setDistance(0.05);
	actualStation.removeType();
	actualStation.setMask(false);
	addStation(actualStation);

	actualStation.setId(2);
	actualStation.setDistance(0.10);
	actualStation.removeType();
	actualStation.setMask(false);
	addStation(actualStation);

	actualStation.setId(3);
	actualStation.setDistance(0.20);
	actualStation.removeType();
	actualStation.setMask(false);
	addStation(actualStation);

	actualStation.setId(4);
	actualStation.setDistance(0.40);
	actualStation.removeType();
	actualStation.setMask(false);
	addStation(actualStation);

	actualStation.setId(5);
	actualStation.setDistance(0.60);
	actualStation.removeType();
	actualStation.setMask(false);
	addStation(actualStation);

	actualStation.setId(6);
	actualStation.setDistance(0.80);
	actualStation.removeType();
	actualStation.setMask(false);
	addStation(actualStation);

	actualStation.setId(7);
	actualStation.setDistance(1.00);
	actualStation.removeType();
	actualStation.setMask(false);
	addStation(actualStation);

}

/****************************************************************
 * method returns the number of detector stations				*
 ****************************************************************/

unsigned short trackfinderInputDetector::getNumberOfStations() {

	return numberOfStations;

}

/****************************************************************
 * method returns the number of detector stations				*
 ****************************************************************/

unsigned short trackfinderInputDetector::getNumberOfActiveStations() {

	return numberOfActiveStations;

}

/****************************************************************
 * This method returns the actual layers id.					*
 * Error:														*
 * - cannotFindDetectorIdError									*
 ****************************************************************/

unsigned short trackfinderInputDetector::getStationIndex(int stationId) {

	trackfinderInputStation* hit;
	unsigned short           returnValue;

	hit = searchStation(stationId);

	if (hit != NULL)
		returnValue = hit->getIndex();
	else
		returnValue = INVALIDSTATIONTYPE;

	return returnValue;

}

/****************************************************************
 * method gets the detector station with a specific detectorId	*
 ****************************************************************/

trackfinderInputStation& trackfinderInputDetector::getStationById(int stationId) {

	trackfinderInputStation* returnValue;

	returnValue = searchStation(stationId);

	return *returnValue;

}

/****************************************************************
 * method gets the detector station with stationIndex			*
 ****************************************************************/

trackfinderInputStation& trackfinderInputDetector::getStationByIndex(unsigned int stationIndex) {

	trackfinderInputStation* returnValue;

	if (detector == NULL)
		throw detectorIsNotDefinedError();

	if (stationIndex < numberOfStations)
		returnValue = &detector[stationIndex];
	else
		throw tooBigStationOrderNumberError(stationIndex);

	return *returnValue;

}

/****************************************************************
 * method gets the detector station with a specific detectorId	*
 ****************************************************************/

trackfinderInputStation* trackfinderInputDetector::getStationPointerById(int stationId) {

	trackfinderInputStation* returnValue;

	returnValue = searchStation(stationId);

	return returnValue;

}

/****************************************************************
 * method gets the detector station with stationIndex			*
 ****************************************************************/

trackfinderInputStation* trackfinderInputDetector::getStationPointerByIndex(unsigned int stationIndex) {

	trackfinderInputStation* returnValue;

	if (detector == NULL)
		throw detectorIsNotDefinedError();

	if (stationIndex < numberOfStations)
		returnValue = &detector[stationIndex];
	else
		throw tooBigStationOrderNumberError(stationIndex);

	return returnValue;

}

/****************************************************************
 * method adds the detector station								*
 ****************************************************************/

void trackfinderInputDetector::addStation(trackfinderInputStation station) {

	numberOfStations++;
	if (numberOfStations == 1)
		detector = (trackfinderInputStation*)calloc(numberOfStations, sizeof(trackfinderInputStation));
	else
		detector = (trackfinderInputStation*)realloc(detector, numberOfStations * sizeof(trackfinderInputStation));

	if (detector == NULL)
		throw memoryAllocationError(DATAROOTOBJECTLIB);

	detector[numberOfStations-1].setId(station.getId());
	detector[numberOfStations-1].setDistance(station.getDistance());
	detector[numberOfStations-1].removeType();
	detector[numberOfStations-1].setMapsType(station.isMapsType());
	detector[numberOfStations-1].setHybridType(station.isHybridType());
	detector[numberOfStations-1].setStripType(station.isStripType());
	detector[numberOfStations-1].setMask(false);

	sortDetector();

}

/****************************************************************
 * method removes the detector station with a specific detectorId*
 ****************************************************************/

void trackfinderInputDetector::removeStation(int stationId) {

	unsigned short index;

	index = getStationIndex(stationId);

	numberOfStations--;
	for (; index < numberOfStations; index++) {
		detector[index].setId(detector[index+1].getId());
		detector[index].setDistance(detector[index+1].getDistance());
		detector[index].removeType();
		detector[index].setMapsType(detector[index+1].isMapsType());
		detector[index].setHybridType(detector[index+1].isHybridType());
		detector[index].setStripType(detector[index+1].isStripType());
		detector[index].setMask(detector[index+1].isMasked());
	}
	detector = (trackfinderInputStation*)realloc(detector, numberOfStations * sizeof(trackfinderInputStation));

	if (detector == NULL)
		throw detectorIsNotDefinedError();

	reIndex();

}

/****************************************************************
 * method removes all detector stations							*
 ****************************************************************/

void trackfinderInputDetector::removeDetector() {

	if (detector != NULL) {
		free(detector);
		detector = NULL;
	}
	numberOfStations       = 0;
	numberOfActiveStations = 0;

}

/****************************************************************
 * method sorts the detector stations							*
 ****************************************************************/

void trackfinderInputDetector::sortDetector() {

	if (detector != NULL)
		qsort((void*)(detector), numberOfStations, sizeof(trackfinderInputStation), compareStationId);
	else
		throw detectorIsNotDefinedError();

	reIndex();

}

/****************************************************************
 * method sets the indexes for all detector stations			*
 ****************************************************************/

void trackfinderInputDetector::reIndex() {

	numberOfActiveStations = 0;

	if (detector == NULL)
		throw detectorIsNotDefinedError();

	for (unsigned int i = 0; i < numberOfStations; i++) {

		if (detector[i].isMasked()) {

			detector[i].setIndex(INVALIDSTATIONTYPE);

		}
		else {

			detector[i].setIndex(numberOfActiveStations);
			numberOfActiveStations++;

		}

	}

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double trackfinderInputDetector::getReservedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(numberOfStations);
	returnValue += sizeof(numberOfActiveStations);
	returnValue += sizeof(detector);

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double trackfinderInputDetector::getAllocatedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = 0;

	if (detector != NULL) {

		for (unsigned short i = 0; i < numberOfStations; i++) {

			returnValue += detector[i].getReservedSizeOfData(0);
			returnValue += detector[i].getAllocatedSizeOfData(0);

		}

	}

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double trackfinderInputDetector::getUsedSizeOfData(unsigned short dimension) {

	double returnValue;

	returnValue  = sizeof(numberOfStations);
	returnValue += sizeof(numberOfActiveStations);
	returnValue += sizeof(detector);

	if (detector != NULL) {

		for (unsigned short i = 0; i < numberOfActiveStations; i++) {

			returnValue += detector[i].getUsedSizeOfData(0);

		}

	}

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

ClassImp(trackfinderInputDetector)
