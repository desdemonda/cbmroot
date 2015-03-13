/* *******************************************************************
// (C)opyright 2005
// 
// Institute of Computer Science V
// Prof. M�ner
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
//     - base class for holding one entry of the first
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


#include "../include/rungeKuttaListEntry.h"


/****************************************************************
 * CLASS rungeKuttaListEntry					 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

rungeKuttaListEntry::rungeKuttaListEntry() : position(), hits() {

  //	position.set(0, DIM1);
  //	position.set(0, DIM2);
  //	position.set(0, DIM3);
  //	hits.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

rungeKuttaListEntry::rungeKuttaListEntry(const rungeKuttaListEntry& value) 
 : position(value.position), hits(value.hits) {

	this->position = value.position;
	this->hits     = value.hits;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

rungeKuttaListEntry::~rungeKuttaListEntry() {

	hits.clear();

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const rungeKuttaListEntry& rungeKuttaListEntry::operator = (const rungeKuttaListEntry& value) {

	this->position = value.position;
	this->hits     = value.hits;

	return *this;

}

/****************************************************************
 * This method converts the object into a string representation.*
 ****************************************************************/

rungeKuttaListEntry::operator std::string(){

	return toNotIdentifiedString();

}

/****************************************************************
 * This method converts the object into a string representation	*
 * and adds no identifiers.										*
 ****************************************************************/

std::string rungeKuttaListEntry::toNotIdentifiedString() {

	std::string returnValue;
	digitalHit  hit;

	returnValue  = position.toNotIdentifiedString();

	hits.resetActiveObject();
	for (unsigned long i = 0; i < hits.getNumberOfEntries(); i++) {

		hit          = hits.readActiveObjectAndMakeNextOneActive();
		returnValue += hit.toNotIdentifiedString();

	}

	return returnValue;

}

/****************************************************************
 * This method converts the object into a string representation	*
 * and adds identifiers.										*
 ****************************************************************/

std::string rungeKuttaListEntry::toIdentifiedString() {

	std::string returnValue;
	digitalHit  hit;

	returnValue  = position.toIdentifiedString();

	hits.resetActiveObject();
	for (unsigned long i = 0; i < hits.getNumberOfEntries(); i++) {

		hit          = hits.readActiveObjectAndMakeNextOneActive();
		returnValue += hit.toIdentifiedString();

	}

	return returnValue;

}

/****************************************************************
 * method returns a pointer to the object						*
 ****************************************************************/

rungeKuttaListEntry* rungeKuttaListEntry::getPointer() {

	return this;

}

/****************************************************************
 * method compare two objects of this class						*
 * @return value > 0, if element is smaller						*
 * @return value = 0, if element is equal						*
 * @return value < 0, if element is bigger						*
 ****************************************************************/

int rungeKuttaListEntry::compare(rungeKuttaListEntry element) {

	return getPosition().compare(element.getPosition());

}

/****************************************************************
 * method returns the position									*
 ****************************************************************/

trackCoordinates rungeKuttaListEntry::getPosition() {

	return position;

}

/****************************************************************
 * method returns the hits										*
 ****************************************************************/

specialList<digitalHit, true, true, true, true, true> rungeKuttaListEntry::getHits() {

	return hits;

}

/****************************************************************
 * method resets the active hit object							*
 ****************************************************************/

void rungeKuttaListEntry::resetActiveHitObject() {

	hits.resetActiveObject();

}

/****************************************************************
 * method returns the number of hit objects						*
 ****************************************************************/

unsigned long rungeKuttaListEntry::getNumberOfHits() {

	return hits.getNumberOfEntries();

}

/****************************************************************
 * method returns the active hit object and makes the			*
 * next one active												*
 ****************************************************************/

digitalHit rungeKuttaListEntry::readActiveHitObjectAndMakeNextOneActive() {

	return hits.readActiveObjectAndMakeNextOneActive();

}

/****************************************************************
 * method sets the position										*
 ****************************************************************/

void rungeKuttaListEntry::setPosition(trackCoordinates& value) {

	position = value;

}

/****************************************************************
 * method sets the hits											*
 ****************************************************************/

void rungeKuttaListEntry::setHits(specialList<digitalHit, true, true, true, true, true>& value) {

	hits = value;

}

/****************************************************************
 * method clears all hit										*
 ****************************************************************/

void rungeKuttaListEntry::clearHits() {

	hits.clear();

}

/****************************************************************
 * method adds a hit											*
 ****************************************************************/

void rungeKuttaListEntry::addHit(digitalHit& value) {

	hits.push(value);

}

/****************************************************************
 * This method returns the size of the reserved memory for		*
 * the source data.												*
 ****************************************************************/

double rungeKuttaListEntry::getReservedSizeOfData(unsigned short dimension) {

	double     returnValue;
	digitalHit hit;

	returnValue  = position.getReservedSizeOfData();

	hits.resetActiveObject();
	for (unsigned long i = 0; i < hits.getNumberOfEntries(); i++) {

		hit          = hits.readActiveObjectAndMakeNextOneActive();
		returnValue += hit.getReservedSizeOfData();

	}

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the allocated memory for		*
 * the source data.												*
 ****************************************************************/

double rungeKuttaListEntry::getAllocatedSizeOfData(unsigned short dimension) {

	double     returnValue;
	digitalHit hit;

	returnValue  = position.getAllocatedSizeOfData();

	hits.resetActiveObject();
	for (unsigned long i = 0; i < hits.getNumberOfEntries(); i++) {

		hit          = hits.readActiveObjectAndMakeNextOneActive();
		returnValue += hit.getAllocatedSizeOfData();

	}

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}

/****************************************************************
 * This method returns the size of the used memory for			*
 * the source data.												*
 ****************************************************************/

double rungeKuttaListEntry::getUsedSizeOfData(unsigned short dimension) {

	double     returnValue;
	digitalHit hit;

	returnValue  = position.getUsedSizeOfData();

	hits.resetActiveObject();
	for (unsigned long i = 0; i < hits.getNumberOfEntries(); i++) {

		hit          = hits.readActiveObjectAndMakeNextOneActive();
		returnValue += hit.getUsedSizeOfData();

	}

	returnValue  = (returnValue / (1 << (10 * dimension)));

	return returnValue;

}
