/* *******************************************************************
// (C)opyright 2005
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
//     - base class for holding one entry of the second
//       list in the Runge-Kutta algorithm which is used to generate
//       the look-up-tables
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-11-21 13:59:23 $
// $Revision: 1.1 $
//
// *******************************************************************/


#include "../include/dataRootObjectWarningMsg.h"
#include "../include/rungeKuttaInvertedListEntry.h"


/****************************************************************
 * CLASS rungeKuttaInvertedListEntry			 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

rungeKuttaInvertedListEntry::rungeKuttaInvertedListEntry() : hit(), positions() 
{

  //	hit.setHitIndex(0);
  //	positions.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

rungeKuttaInvertedListEntry::rungeKuttaInvertedListEntry(const rungeKuttaInvertedListEntry& value) : hit(value.hit), positions(value.positions) {

  //	this->hit       = value.hit;
  //	this->positions = value.positions;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

rungeKuttaInvertedListEntry::~rungeKuttaInvertedListEntry() {

	positions.clear();

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const rungeKuttaInvertedListEntry& rungeKuttaInvertedListEntry::operator = (const rungeKuttaInvertedListEntry& value) {

	this->hit       = value.hit;
	this->positions = value.positions;

	return *this;

}

/****************************************************************
 * This method converts the object into a string representation.*
 ****************************************************************/

rungeKuttaInvertedListEntry::operator std::string(){

	return toNotIdentifiedString();

}

/****************************************************************
 * This method converts the object into a string representation	*
 * and adds no identifiers.										*
 ****************************************************************/

std::string rungeKuttaInvertedListEntry::toNotIdentifiedString() {

	std::string      returnValue;
	trackCoordinates position;

	returnValue  = hit.toNotIdentifiedString();

	positions.resetActiveObject();
	for (unsigned long i = 0; i < positions.getNumberOfEntries(); i++) {

		position     = positions.readActiveObjectAndMakeNextOneActive();
		returnValue += position.toNotIdentifiedString();

	}

	return returnValue;

}

/****************************************************************
 * This method converts the object into a string representation	*
 * and adds identifiers.										*
 ****************************************************************/

std::string rungeKuttaInvertedListEntry::toIdentifiedString() {

	std::string      returnValue;
	trackCoordinates position;

	returnValue  = hit.toIdentifiedString();

	positions.resetActiveObject();
	for (unsigned long i = 0; i < positions.getNumberOfEntries(); i++) {

		position     = positions.readActiveObjectAndMakeNextOneActive();
		returnValue += position.toIdentifiedString();

	}

	return returnValue;

}

/****************************************************************
 * method returns a pointer to the object						*
 ****************************************************************/

rungeKuttaInvertedListEntry* rungeKuttaInvertedListEntry::getPointer() {

	return this;

}

/****************************************************************
 * method compare two objects of this class						*
 * @return value > 0, if element is smaller						*
 * @return value = 0, if element is equal						*
 * @return value < 0, if element is bigger						*
 ****************************************************************/

int rungeKuttaInvertedListEntry::compare(rungeKuttaInvertedListEntry element) {

	return getHit().compare(element.getHit());

}

/****************************************************************
 * method returns the hit										*
 ****************************************************************/

digitalHit rungeKuttaInvertedListEntry::getHit() {

	return hit;

}

/****************************************************************
 * method returns the minimal layer based on the positions		*
 ****************************************************************/

unsigned short rungeKuttaInvertedListEntry::getMinLayer() {

	trackCoordinates actualPosition;
	unsigned short   returnValue = 0;

	if (positions.getNumberOfEntries() > 0) {

		positions.resetActiveObject();
		actualPosition      = positions.readActiveObjectAndMakeNextOneActive();
		returnValue         = actualPosition.get(DIM3);
		for (unsigned long i = 1; i < positions.getNumberOfEntries(); i++) {

			actualPosition  = positions.readActiveObjectAndMakeNextOneActive();
			if (actualPosition.get(DIM3) < returnValue)
				returnValue = actualPosition.get(DIM3);

		}

	}
	else {

		noEntryInListWarningMsg* noEntryInList = new noEntryInListWarningMsg();
		noEntryInList->warningMsg();
		if(noEntryInList != NULL) {
			delete noEntryInList;
			noEntryInList = NULL;
		}

	}

	return returnValue;

}

/****************************************************************
 * method returns the maximal layer based on the positions		*
 ****************************************************************/

unsigned short rungeKuttaInvertedListEntry::getMaxLayer() {

	trackCoordinates actualPosition;
	unsigned short   returnValue = 0;

	if (positions.getNumberOfEntries() > 0) {

		positions.resetActiveObject();
		actualPosition      = positions.readActiveObjectAndMakeNextOneActive();
		returnValue         = actualPosition.get(DIM3);
		for (unsigned long i = 1; i < positions.getNumberOfEntries(); i++) {

			actualPosition  = positions.readActiveObjectAndMakeNextOneActive();
			if (actualPosition.get(DIM3) > returnValue)
				returnValue = actualPosition.get(DIM3);

		}

	}
	else {

		noEntryInListWarningMsg* noEntryInList = new noEntryInListWarningMsg();
		noEntryInList->warningMsg();
		if(noEntryInList != NULL) {
			delete noEntryInList;
			noEntryInList = NULL;
		}

	}

	return returnValue;

}

/****************************************************************
 * method returns the positions									*
 ****************************************************************/

specialList<trackCoordinates, true, true, true, true, true> rungeKuttaInvertedListEntry::getPositions() {

	return positions;

}

/****************************************************************
 * method resets the active position object						*
 ****************************************************************/

void rungeKuttaInvertedListEntry::resetActivePositionObject() {

	positions.resetActiveObject();

}

/****************************************************************
 * method returns the number of position objects				*
 ****************************************************************/

unsigned long rungeKuttaInvertedListEntry::getNumberOfPositions() {

	return positions.getNumberOfEntries();

}

/****************************************************************
 * method returns the active position object					*
 ****************************************************************/

trackCoordinates rungeKuttaInvertedListEntry::readActivePositionObject() {

	return positions.readActiveObject();

}

/****************************************************************
 * method makes the next position object active					*
 ****************************************************************/

void rungeKuttaInvertedListEntry::makeNextPositionObjectActive() {

	positions.makeNextOneActive();

}

/****************************************************************
 * method returns the active position object and makes the		*
 * next one active												*
 ****************************************************************/

trackCoordinates rungeKuttaInvertedListEntry::readActivePositionObjectAndMakeNextOneActive() {

	return positions.readActiveObjectAndMakeNextOneActive();

}

/****************************************************************
 * method updates the active position object					*
 ****************************************************************/

void rungeKuttaInvertedListEntry::updateActivePositionObject(trackCoordinates& value) {

	for (unsigned short i = 0; i < DIMENSIONS; i++)
		positions.getActiveObject()->set(value.get(i), i);

}

/****************************************************************
 * method sets the hit											*
 ****************************************************************/

void rungeKuttaInvertedListEntry::setHit(digitalHit& value) {

	hit = value;

}

/****************************************************************
 * method sets the positions									*
 ****************************************************************/

void rungeKuttaInvertedListEntry::setPositions(specialList<trackCoordinates, true, true, true, true, true>& value) {

	positions = value;

}

/****************************************************************
 * method clears all positions									*
 ****************************************************************/

void rungeKuttaInvertedListEntry::clearPositions() {

	positions.clear();

}

/****************************************************************
 * method adds a position										*
 ****************************************************************/

void rungeKuttaInvertedListEntry::addPosition(trackCoordinates& value) {

	positions.push(value);

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double rungeKuttaInvertedListEntry::getReservedSizeOfData(unsigned short dimension) {

	double           returnValue;
	trackCoordinates position;

	returnValue  = hit.getReservedSizeOfData();

	positions.resetActiveObject();
	for (unsigned long i = 0; i < positions.getNumberOfEntries(); i++) {

		position     = positions.readActiveObjectAndMakeNextOneActive();
		returnValue += position.getReservedSizeOfData();

	}

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double rungeKuttaInvertedListEntry::getAllocatedSizeOfData(unsigned short dimension) {

	double           returnValue;
	trackCoordinates position;

	returnValue  = hit.getAllocatedSizeOfData();

	positions.resetActiveObject();
	for (unsigned long i = 0; i < positions.getNumberOfEntries(); i++) {

		position     = positions.readActiveObjectAndMakeNextOneActive();
		returnValue += position.getAllocatedSizeOfData();

	}

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double rungeKuttaInvertedListEntry::getUsedSizeOfData(unsigned short dimension) {

	double           returnValue;
	trackCoordinates position;

	returnValue  = hit.getUsedSizeOfData();

	positions.resetActiveObject();
	for (unsigned long i = 0; i < positions.getNumberOfEntries(); i++) {

		position     = positions.readActiveObjectAndMakeNextOneActive();
		returnValue += position.getUsedSizeOfData();

	}

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}
