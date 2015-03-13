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
/// $Date: 2008-08-14 12:32:42 $
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _TABLESTRINGENTRY_H
#define _TABLESTRINGENTRY_H


#include "../include/tableEntry.h"
#include <string>


/* **************************************************************
 * CLASS tableStringEntry										*
 * **************************************************************/

class tableStringEntry {

protected:

	std::string signature;
	std::string classification;

public:

/**
 * Default constructor
 */

	tableStringEntry();

/**
 * Copy constructor
 */

	tableStringEntry(const tableStringEntry& value);

/**
 * Destructor
 */

	virtual ~tableStringEntry();

/**
 * operator = ()
 */

	const tableStringEntry& operator = (const tableStringEntry& value);

/**
 * method returns a pointer to the object
 */

	tableStringEntry* getPointer();

/**
 * method compare two objects of this class
 * @return value > 0, if element is smaller
 * @return value = 0, if element is equal
 * @return value < 0, if element is bigger
 */

	int compare(tableStringEntry element);

/**
 * method compare two objects of this class
 * @return true, if the object is bigger than the element
 * @return false, if the object is not bigger than the element
 */

	bool isBigger(tableStringEntry element);

/**
 * method returns the signature
 */

	std::string getSignature();

/**
 * method returns the classification
 */

	std::string getClassification();

/**
 * method sets the signature
 */

	void setSignature(std::string& value);

/**
 * method sets the classification
 */

	void setClassification(std::string& value);

/**
 * method setup from a tableEntry
 */

	void setup(tableEntry& value);

/**
 * method to try the merging of two objects
 */

	bool canBeMergedWith(tableStringEntry& value, std::string& mergedSignature, bool useClassification = true);

/**
 * method to find out if one object can be build by using the second
 */

	bool isImplicitelyIn(tableStringEntry& value);

};

#endif
