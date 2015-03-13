//////////////////////////////////////////////////////////////////////
/// (C)opyright 2004
/// 
/// Institute of Computer Science V
/// Prof. Männer
/// University of Mannheim, Germany
/// 
/// *******************************************************************
/// 
/// Designer(s):   Steinle
/// 
/// *******************************************************************
/// 
/// Project:     Trackfinder for CBM-Project at GSI-Darmstadt, Germany
/// 
/// *******************************************************************
/// 
/// Description:
///
///   class:
///     - The class handles one classification set for the peaks in the histogram
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-11-21 13:58:53 $
/// $Revision: 1.4 $
///
//////////////////////////////////////////////////////////////////////


#include "../include/tableEntry.h"


/****************************************************************
 * Default constructor											*
 ****************************************************************/

tableEntry::tableEntry() : signature(bitArray(0)), classification(bitArray(0)) {

  //	signature      = bitArray(0);
  //	classification = bitArray(0);

}

/****************************************************************
 * Copy constructor
 ****************************************************************/

tableEntry::tableEntry(const tableEntry& value) : signature(value.signature), classification(value.classification) {

  //	signature      = value.signature;
  //	classification = value.classification;

}

/**
 * Constructor
 */

tableEntry::tableEntry(bitArray& _signature, bitArray& _classification) : signature(_signature), classification(_classification) {

  //	this->signature      = _signature;
  //	this->classification = _classification;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

tableEntry::~tableEntry() {
	
}

/****************************************************************
 * operator = ()
 ****************************************************************/

const tableEntry& tableEntry::operator = (const tableEntry& value) {

	signature      = value.signature;
	classification = value.classification;

	return *this;

}

/****************************************************************
 * This method converts the object into a string representation.*
 ****************************************************************/

tableEntry::operator std::string(){

	return toIdentifiedString();

}

/****************************************************************
 * This method converts the object into a string representation	*
 * and adds no identifiers.										*
 ****************************************************************/

std::string tableEntry::toNotIdentifiedString() {

	std::string returnValue;

	returnValue  = "{";
	returnValue += signature.toString(2);
	returnValue += ",";
	returnValue += classification.toString(2);
	returnValue += "}";

	return returnValue;

}

/****************************************************************
 * This method converts the object into a string representation	*
 * and adds identifiers.										*
 ****************************************************************/

std::string tableEntry::toIdentifiedString() {

	std::string returnValue;

	returnValue  = "Signature: ";
	returnValue += signature.toString(2);
	returnValue += ", Classification: ";
	returnValue += classification.toString(2);

	return returnValue;

}

/****************************************************************
 * method returns a pointer to the object						*
 ****************************************************************/

tableEntry* tableEntry::getPointer() {

	return this;

}

/****************************************************************
 * method compare two objects of this class						*
 * @return value > 0, if element is smaller						*
 * @return value = 0, if element is equal						*
 * @return value < 0, if element is bigger						*
 ****************************************************************/

int tableEntry::compare(tableEntry element) {

	int returnValue;

	if (element.getSignature() < this->getSignature())
		returnValue = 1;
	else if (element.getSignature() == this->getSignature())
		returnValue = 0;
	else
		returnValue = -1;

	return returnValue;

}

/****************************************************************
 * method returns the signature									*
 ****************************************************************/

bitArray& tableEntry::getSignature() {

	return signature;

}

/****************************************************************
 * method returns the classification							*
 ****************************************************************/

bitArray& tableEntry::getClassification() {

	return classification;

}

/****************************************************************
 * method sets the signature									*
 ****************************************************************/

void tableEntry::setSignature(bitArray& value) {

	signature = value;

}

/****************************************************************
 * method sets the classification								*
 ****************************************************************/

void tableEntry::setClassification(bitArray& value) {

	classification = value;

}
