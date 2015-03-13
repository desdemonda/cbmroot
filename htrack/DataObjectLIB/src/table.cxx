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
///     - The class optimizes the classification of the peaks in the histogram
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-10-24 16:36:58 $
/// $Revision: 1.13 $
///
//////////////////////////////////////////////////////////////////////


#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/dataObjectWarningMsg.h"
#include "../include/tableFile.h"
#include "../include/table.h"


/****************************************************************
 * method reads a file to get the table							*
 ****************************************************************/

void table::addTableStringEntry(tableStringEntry& string) {

	std::string                       signature;
	std::string                       classification;
	int                               radix;
	int                               radixCheck;
	int                               signaturePos;
	bool                              unknownValueFound;
	std::basic_string<char>::iterator signaturePosition;
	bitArray                          entrySignature;
	bitArray                          entryClassification;

	signature         = string.getSignature();
	classification    = string.getClassification();

	signaturePos      = (int)signature.find_first_of("x", 0);
	if (signaturePos == (int)std::basic_string<char>::npos)
		signaturePos  = 0;
	else
		signaturePos += 1; /* + 1 to point after the 'x' */
	if ((int)entrySignature.length() > (int)signature.length() - signaturePos)
		signature.insert(signaturePos, (int)entrySignature.length() - (int)signature.length() + signaturePos, 'x');

	extractRadix(&radix, &signature);
	unknownValueFound = false;
	for (signaturePosition = signature.begin(); signaturePosition != signature.end(); signaturePosition++) {

		radixCheck = (int)ctous(*signaturePosition);
		if (radixCheck >= radix) {

			unknownValueFound = true;
			break;
		
		}

	}
	addRadix(radix, signature);

	if (unknownValueFound) {

		for (unsigned short i = 0; i < (unsigned short)radix; i++) {

			*signaturePosition = ustoc(i);
			string.setSignature(signature);
			addTableStringEntry(string);

		}

	}
	else {

		extractRadix(&radix, &classification);
		unknownValueFound = false;
		for (std::basic_string<char>::iterator classificationPosition = classification.begin(); classificationPosition != classification.end(); classificationPosition++) {

			radixCheck = (int)ctous(*classificationPosition);
			if (radixCheck >= radix) {

				unknownValueFound         = true;
				*classificationPosition   = '0';

			}

		}
		addRadix(radix, classification);

		if (unknownValueFound) {

			unknownValueInClassificationFoundWarningMsg* unknownValueInClassificationFound = new unknownValueInClassificationFoundWarningMsg();
			unknownValueInClassificationFound->warningMsg();
			if(unknownValueInClassificationFound != NULL) {
				delete unknownValueInClassificationFound;
				unknownValueInClassificationFound = NULL;
			}

		}

		entrySignature       = bitArray(signature);
		entryClassification  = bitArray(classification);
		addEntry(entrySignature, entryClassification);

	}

}

/****************************************************************
 * method which reduces the bitCombinations to minimal terms	*
 ****************************************************************/

void table::quineMcClusky(std::list<tableStringEntry>& list, bool useClassification) {

	bool                                             firstObjectFound;
	bool                                             secondObjectFound;
	std::list<tableStringEntry>                      copyList;
	std::string                                      mergedSignature;
	std::string                                      mergedClassification;
	tableStringEntry                                 mergedEntry;
	std::list<tableStringEntry>                      mergedList;
	std::list<std::list<tableStringEntry>::iterator> removeList;
	bool                                             primeImplicantFound;

	if (list.size() > 1) {

		copyList.clear();
		for (std::list<tableStringEntry>::iterator h = list.begin(); h != list.end(); h++)
			copyList.push_back(*h);

		for (unsigned short i = 0; i < list.back().getSignature().length(); i++) {

			mergedList.clear();
			removeList.clear();
			for (std::list<tableStringEntry>::iterator j = list.begin(); j != list.end(); j++) {

				for (std::list<tableStringEntry>::iterator k = j; k != list.end(); k++) {

					if (j == k)
						continue;

					if (j->canBeMergedWith(*k, mergedSignature, useClassification)) {

						firstObjectFound  = false;
						secondObjectFound = false;
						for (std::list<std::list<tableStringEntry>::iterator>::iterator l = removeList.begin(); l != removeList.end(); l++) {

							if (*l == j)
								firstObjectFound  = true;

							if (*l == k)
								secondObjectFound = true;

						}

						if(!firstObjectFound)
							removeList.push_back(j);

						if(!secondObjectFound)
							removeList.push_back(k);
						
						mergedClassification = j->getClassification();
						mergedEntry.setSignature(mergedSignature);
						mergedEntry.setClassification(mergedClassification);
						
						firstObjectFound  = false;
						for (std::list<tableStringEntry>::iterator m = mergedList.begin(); m != mergedList.end(); m++)
							if (m->getSignature() == mergedSignature)
								firstObjectFound = true;

						if (!firstObjectFound)
							mergedList.push_back(mergedEntry);

					}

				}

			}

			if (!removeList.empty()) {

				for (std::list<std::list<tableStringEntry>::iterator>::iterator n = removeList.begin(); n != removeList.end(); n++)
					list.erase(*n);

				for (std::list<tableStringEntry>::iterator o = mergedList.begin(); o != mergedList.end(); o++)
					list.push_back(*o);

			}
			else
				break;

		}

	}

	if (list.size() > 1) {

		/* loop to exclude one found prime implicant */
		for (std::list<tableStringEntry>::iterator p = list.begin(); p != list.end();) {

			/* loop to find out if the resulting prime implicants are enough */
			for (std::list<tableStringEntry>::iterator q = copyList.begin(); q != copyList.end(); q++) {
	
				primeImplicantFound = false;
				/* loop over the resulting prime implicants */
				for (std::list<tableStringEntry>::iterator r = list.begin(); r != list.end(); r++) {

					/* exclude the prime implicant */
					if (p == r)
						continue;

					/* search if this prime implicant can build the actual term */
					if (q->isImplicitelyIn(*r))
						primeImplicantFound = true;

				}

				/* actual term cannot be build by any prime implicant */
				if (!primeImplicantFound)
					break;

			}

			if (primeImplicantFound)	/* prime implicant is not neccessary */
				p = list.erase(p);
			else						/* prime implicant is neccessary */
				p++;

		}

	}

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

table::table() : members(), minimumClassification(), maximumClassification() {

	clear();

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

table::~table() {

	clear();
	
}

/****************************************************************
 * method removes all entries from the table					*
 ****************************************************************/

void table::clear() {

	members.clear();
	minimumClassification = bitArray(1);
	maximumClassification = bitArray(0);

}

/****************************************************************
 * method updates the table belonging to correct entries		*
 ****************************************************************/

void table::update() {

	tableEntry actualEntry;

	members.resetActiveObject();
	for (unsigned long i = 0; i < getNumberOfEntries(); i++) {

		actualEntry = members.readActiveObject();

		if (actualEntry.getClassification() > getMaximumClassification())
			maximumClassification = actualEntry.getClassification();

		if (actualEntry.getClassification() < getMinimumClassification())
			members.eraseActiveObject();
		else
			members.makeNextOneActive();

	}

}

/****************************************************************
 * method returns the number of entries							*
 ****************************************************************/

unsigned long table::getNumberOfEntries() {

	return members.getNumberOfEntries();

}

/****************************************************************
 * method returns the maximal number of entries					*
 ****************************************************************/

unsigned long table::getNumberOfMembers() {

	/* maximumClassification can be used here because it is a bitArray like all signatures */
	return (unsigned long)(1 << maximumClassification.length());

}

/****************************************************************
 * method returns the tableEntry for the given signature		*
 ****************************************************************/

tableEntry table::getEntry(bitArray& signature) {

	bitArray    zero;
	tableEntry  returnValue;
	tableEntry* object;
	
	zero        = bitArray(0);
	returnValue = tableEntry(signature, zero);

	if (members.isFound(returnValue, true)) {

		object      = members.getActiveObject();
		returnValue = (*object);

	}
	else {

		returnValue.setSignature(zero);

	}

	if (returnValue.getClassification() < getMinimumClassification())
		returnValue = tableEntry(zero, zero);

	return returnValue;

}

/****************************************************************
 * method returns the classification for the given signature	*
 ****************************************************************/

bitArray table::getClassification(bitArray& signature) {

	tableEntry returnValue;

	returnValue = getEntry(signature);

	return returnValue.getClassification();

}

/****************************************************************
 * method returns the member entry for the given signature		*
 ****************************************************************/

bitArray table::getMember(unsigned long index) {

	bitArray signature;

	signature = bitArray(index);

	return getClassification(signature);

}

/****************************************************************
 * method returns the maximal classification of this table		*
 ****************************************************************/

bitArray& table::getMaximumClassification() {

	return maximumClassification;

}

/****************************************************************
 * method returns the minimal classification of this table		*
 ****************************************************************/

bitArray& table::getMinimumClassification() {

	return minimumClassification;

}

/****************************************************************
 * method sets the minimal classification of this table			*
 ****************************************************************/

void table::setMinimumClassification(bitArray value) {

	minimumClassification = value;

	if(value > maximumClassification) {

		minClassIsBiggerThanMaxClassWarningMsg* minClassIsBiggerThanMaxClass = new minClassIsBiggerThanMaxClassWarningMsg();
		minClassIsBiggerThanMaxClass->warningMsg();
		if(minClassIsBiggerThanMaxClass != NULL) {
			delete minClassIsBiggerThanMaxClass;
			minClassIsBiggerThanMaxClass = NULL;
		}

	}

}

/****************************************************************
 * method adds an entry to the table							*
 ****************************************************************/

void table::addEntry(bitArray& signature, bitArray& classification) {

	tableEntry addKey;

	addKey.setSignature(signature);
	addKey.setClassification(classification);

	addEntry(addKey);

}
void table::addEntry(tableEntry& value) {

	if (value.getClassification() >= getMinimumClassification()) {

		members.push(value);

		if (value.getClassification() > getMaximumClassification())
			maximumClassification = value.getClassification();

	}

}

/****************************************************************
 * Method adds an entry to the table, if it does not exist.		*
 * If it exists the classification would be summed up to get	*
 * a higher priority.											*
 ****************************************************************/

void table::sumEntry(bitArray& signature, bitArray& classification) {

	tableEntry  actualEntry;
	tableEntry* object;
	bitArray    actualClassification;
	
	actualEntry = tableEntry(signature, classification);

	if (members.isFound(actualEntry, true)) {

		object               = members.getActiveObject();
		actualClassification = object->getClassification() + classification;
		object->setClassification(actualClassification);

		if ((actualClassification > object->getClassification()) || (classification > object->getClassification())) {

			overflowByAddingClassificationsFoundWarningMsg* overflowByAddingClassificationsFound = new overflowByAddingClassificationsFoundWarningMsg();
			overflowByAddingClassificationsFound->warningMsg();
			if(overflowByAddingClassificationsFound != NULL) {
				delete overflowByAddingClassificationsFound;
				overflowByAddingClassificationsFound = NULL;
			}

		}

		if (object->getClassification() > getMaximumClassification())
			maximumClassification = object->getClassification();

	}
	else
		addEntry(actualEntry);

}

/****************************************************************
 * method converts the complete table into a std::string		*
 * representation												*
 ****************************************************************/

std::string table::toString() {

	return members.print();

}

/****************************************************************
 * method reads a file to get the table							*
 ****************************************************************/

void table::read(std::string fileName, std::streambuf* terminal) {

	tableFile         readFile;
	tableStringEntry* readData;

	readFile.setFileName(fileName);

	readFile.setDataNum(0);

	readFile.readFile(terminal);

	readData = (tableStringEntry*)readFile.getDataPtr();

	members.clear();
	for (unsigned long i = 0; i < readFile.getDataNum(); i++)
		addTableStringEntry(readData[i]);

}

/****************************************************************
 * method writes a file representing the table					*
 ****************************************************************/

void table::write(std::string fileName, std::string name, std::string usage, bool useClassification, std::streambuf* terminal) {

	unsigned long               i;
	tableEntry                  actualEntry;
	tableStringEntry            actualString;
	std::list<tableStringEntry> stringEntries;
	tableStringEntry*           writeData;
	tableFileHeader             fileHeader;
	tableFile                   writeFile;

	stringEntries.clear();
	members.resetActiveObject();
	for (i = 0; i < members.getNumberOfEntries(); i++) {
		
		actualEntry = members.readActiveObjectAndMakeNextOneActive();
		actualString.setup(actualEntry);
		stringEntries.push_back(actualString);

	}

	quineMcClusky(stringEntries, useClassification);

	writeData                  = new tableStringEntry[(unsigned long)stringEntries.size()];

	i = 0;
	for (std::list<tableStringEntry>::iterator j = stringEntries.begin(); j != stringEntries.end(); i++, j++)
		writeData[i] = *j;

	fileHeader.name            = name;
	fileHeader.usage           = usage;
	fileHeader.numberOfEntries = (unsigned int)stringEntries.size();

	stringEntries.clear();

	writeFile.setFileName(fileName);

	writeFile.setDataNum(fileHeader.numberOfEntries);

	writeFile.setDataPtr(writeData);

	writeFile.setHeader(fileHeader);

	writeFile.writeFile(terminal);

	if (writeData != NULL) {
		delete [] writeData;
		writeData = NULL;
	}

}
