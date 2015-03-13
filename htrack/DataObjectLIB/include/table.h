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
/// $Date: 2008-10-24 16:36:57 $
/// $Revision: 1.8 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _TABLE_H
#define _TABLE_H


#include "../../MiscLIB/include/bitArray.h"
#include "../include/specialList.h"
#include "../include/tableEntry.h"
#include "../include/tableStringEntry.h"
#include <list>


/* **************************************************************
 * CLASS table									 				*
 * **************************************************************/

class table {

protected:

	specialList<tableEntry, true, true, true, true, true> members;
	bitArray                                              minimumClassification;
	bitArray                                              maximumClassification;

/**
 * method reads a file to get the table
 */

	void addTableStringEntry(tableStringEntry& string);

/**
 * method which reduces the bitCombinations to minimal terms
 */

	void quineMcClusky(std::list<tableStringEntry>& list, bool useClassification = true);

public:

/**
 * Default constructor
 */

	table();

/**
 * Destructor
 */

	virtual ~table();

/**
 * method removes all entries from the table
 */

	void clear();

/**
 * method updates the table belonging to correct entries
 */

	void update();

/**
 * method returns the number of entries
 */

	unsigned long getNumberOfEntries();

/**
 * method returns the maximal number of entries
 */

	unsigned long getNumberOfMembers();

/**
 * method returns the tableEntry for the given signature
 */

	tableEntry getEntry(bitArray& signature);

/**
 * method returns the classification for the given signature
 */

	bitArray getClassification(bitArray& signature);

/**
 * method returns the member entry for the given signature
 */

	bitArray getMember(unsigned long index);

/**
 * method returns the maximal classification of this table
 */

	bitArray& getMaximumClassification();

/**
 * method returns the minimal classification of this table
 */

	bitArray& getMinimumClassification();

/**
 * method sets the minimal classification of this table
 */

	void setMinimumClassification(bitArray value);

/**
 * method adds an entry to the table
 */

	void addEntry(bitArray& signature, bitArray& classification);
	void addEntry(tableEntry& value);

/**
 * Method adds an entry to the table, if it does not exist.
 * If it exists the classification would be summed up to get
 * a higher priority.
 */

	void sumEntry(bitArray& signature, bitArray& classification);

/**
 * method converts the complete table into a std::string representation
 */

	std::string toString();

/**
 * method reads a file to get the table
 * @param fileName is the name of the file to read the data
 * @param terminal is a buffer to place the process information
 */

	void read(std::string fileName, std::streambuf* terminal = NULL);

/**
 * method writes a file representing the table
 * @param fileName is the name of the file to write the data
 * @param name is the name of the table which should be written to file
 * @param usage is a string representing the usage of the table in the program which should be written to file
 * @param useClassification is the flag which shows if the classification should be taken into acoount while doing quineMCClusky
 * @param terminal is a buffer to place the process information
 * @see void quineMcClusky(std::list<tableStringEntry>& list, bool useClassification = true)
 */

	void write(std::string fileName, std::string name, std::string usage, bool useClassification = true, std::streambuf* terminal = NULL);

};

#endif
