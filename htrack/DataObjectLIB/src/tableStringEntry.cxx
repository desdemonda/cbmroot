//////////////////////////////////////////////////////////////////////
/// (C)opyright 2004
/// 
/// Institute of Computer Science V
/// Prof. M�ner
/// University of Mannheim, Germany
/// 
/// *******************************************************************
/// 
/// Designer(s):   Steinle / Gl�
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
/// $Date: 2007-06-21 15:17:25 $
/// $Revision: 1.4 $
///
//////////////////////////////////////////////////////////////////////


#include "../include/dataObjectWarningMsg.h"
#include "../include/tableStringEntry.h"


/****************************************************************
 * Default constructor											*
 ****************************************************************/

tableStringEntry::tableStringEntry() : signature(), classification() {

  //	signature.clear();
  //	classification.clear();

}

/****************************************************************
 * Copy constructor
 ****************************************************************/

tableStringEntry::tableStringEntry(const tableStringEntry& value) : signature(value.signature), classification(value.classification) {

  //	signature      = value.signature;
  //	classification = value.classification;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

tableStringEntry::~tableStringEntry() {
	
}

/****************************************************************
 * operator = ()
 ****************************************************************/

const tableStringEntry& tableStringEntry::operator = (const tableStringEntry& value) {

	signature      = value.signature;
	classification = value.classification;

	return *this;

}

/****************************************************************
 * method returns a pointer to the object						*
 ****************************************************************/

tableStringEntry* tableStringEntry::getPointer() {

	return this;

}

/****************************************************************
 * method compare two objects of this class						*
 * @return value > 0, if element is smaller						*
 * @return value = 0, if element is equal						*
 * @return value < 0, if element is bigger						*
 ****************************************************************/

int tableStringEntry::compare(tableStringEntry element) {

	int returnValue;

	if (this->getSignature() > element.getSignature())
		returnValue = 1;
	else if (this->getSignature() == element.getSignature())
		returnValue = 0;
	else
		returnValue = -1;

	return returnValue;

}

/****************************************************************
 * method compare two objects of this class						*
 * @return true, if the object is bigger than the element		*
 * @return false, if the object is not bigger than the element	*
 ****************************************************************/

bool tableStringEntry::isBigger(tableStringEntry element) {

	bool returnValue;

	if (this->getSignature() > element.getSignature())
		returnValue = true;
	else
		returnValue = false;

	return returnValue;

}

/****************************************************************
 * method returns the signature									*
 ****************************************************************/

std::string tableStringEntry::getSignature() {

	return signature;

}

/****************************************************************
 * method returns the classification							*
 ****************************************************************/

std::string tableStringEntry::getClassification() {

	return classification;

}

/****************************************************************
 * method sets the signature									*
 ****************************************************************/

void tableStringEntry::setSignature(std::string& value) {

	signature = value;

}

/****************************************************************
 * method sets the classification								*
 ****************************************************************/

void tableStringEntry::setClassification(std::string& value) {

	classification = value;

}

/****************************************************************
 * method setup from a tableEntry								*
 ****************************************************************/

void tableStringEntry::setup(tableEntry& value) {

	std::string _signature;
	std::string _classification;

	_signature      = value.getSignature().toString(2);
	_classification = value.getClassification().toString(10);
	
	setSignature(signature);
	setClassification(classification);

}

/****************************************************************
 * method to try the merging of two objects						*
 ****************************************************************/

bool tableStringEntry::canBeMergedWith(tableStringEntry& value, std::string& mergedSignature, bool useClassification) {

	bool                              returnValue;
	unsigned short                    numberOfDifferences;
	std::basic_string<char>::iterator iterator1;
	std::basic_string<char>::iterator iterator2;
	std::string                       signature1 = this->getSignature();
	std::string                       signature2 = value.getSignature();

	returnValue = false;

	if (signature1.length() != signature2.length()) {
		cannotDoQuineMcCluskyWithDifferentLengthWarningMsg* cannotDoQuineMcCluskyWithDifferentLength = new cannotDoQuineMcCluskyWithDifferentLengthWarningMsg((unsigned int)signature1.length(), (unsigned int)signature2.length());
		cannotDoQuineMcCluskyWithDifferentLength->warningMsg();
		if(cannotDoQuineMcCluskyWithDifferentLength != NULL) {
			delete cannotDoQuineMcCluskyWithDifferentLength;
			cannotDoQuineMcCluskyWithDifferentLength = NULL;
		}
	}
	else if ((signature1.substr(0, 2) != "2x") || (signature2.substr(0, 2) != "2x")) {
		cannotDoQuineMcCluskyWithWrongFormatWarningMsg* cannotDoQuineMcCluskyWithWrongFormat = new cannotDoQuineMcCluskyWithWrongFormatWarningMsg();
		cannotDoQuineMcCluskyWithWrongFormat->warningMsg();
		if(cannotDoQuineMcCluskyWithWrongFormat != NULL) {
			delete cannotDoQuineMcCluskyWithWrongFormat;
			cannotDoQuineMcCluskyWithWrongFormat = NULL;
		}
	}
	else {

		mergedSignature.clear();
		numberOfDifferences = 0;
		iterator1           = signature1.begin();
		iterator2           = signature2.begin();

		for (; iterator1 != signature1.end(); iterator1++, iterator2++) {
		
			if (*iterator1 != *iterator2) {

				numberOfDifferences++;
				mergedSignature += "x";

			}
			else
				mergedSignature += *iterator1;

		}

		if (numberOfDifferences != 1) {
			mergedSignature.clear();
		}
		else
			returnValue = true;

	}

	if (useClassification)
		if (this->getClassification() != value.getClassification())
			returnValue = false;

	return returnValue;

}

/****************************************************************
 * method to find out if one object can be build by using the	*
 * second														*
 ****************************************************************/

bool tableStringEntry::isImplicitelyIn(tableStringEntry& value) {

	bool                              returnValue;
	std::basic_string<char>::iterator iterator1;
	std::basic_string<char>::iterator iterator2;
	std::string                       signature1 = this->getSignature();
	std::string                       signature2 = value.getSignature();

	returnValue = false;

	if (signature1.length() != signature2.length()) {
		cannotDoQuineMcCluskyWithDifferentLengthWarningMsg* cannotDoQuineMcCluskyWithDifferentLength = new cannotDoQuineMcCluskyWithDifferentLengthWarningMsg((unsigned int)signature1.length(), (unsigned int)signature2.length());
		cannotDoQuineMcCluskyWithDifferentLength->warningMsg();
		if(cannotDoQuineMcCluskyWithDifferentLength != NULL) {
			delete cannotDoQuineMcCluskyWithDifferentLength;
			cannotDoQuineMcCluskyWithDifferentLength = NULL;
		}
	}
	else if ((signature1.substr(0, 2) != "2x") || (signature2.substr(0, 2) != "2x")) {
		cannotDoQuineMcCluskyWithWrongFormatWarningMsg* cannotDoQuineMcCluskyWithWrongFormat = new cannotDoQuineMcCluskyWithWrongFormatWarningMsg();
		cannotDoQuineMcCluskyWithWrongFormat->warningMsg();
		if(cannotDoQuineMcCluskyWithWrongFormat != NULL) {
			delete cannotDoQuineMcCluskyWithWrongFormat;
			cannotDoQuineMcCluskyWithWrongFormat = NULL;
		}
	}
	else {

		returnValue         = true;
		iterator1           = signature1.begin();
		iterator2           = signature2.begin();

		for (; iterator1 != signature1.end(); iterator1++, iterator2++) {
		
			if (*iterator1 != *iterator2) {
				
				if (((*iterator1 == '0') || (*iterator1 == '1')) && ((*iterator2 == '0') || (*iterator2 == '1'))) {

					returnValue = false;
					break;

				}

			}

		}

	}

	return returnValue;

}
