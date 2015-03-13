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
///     - class for the digital hit look-up-table implemented with a file
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-10-24 16:41:17 $
/// $Revision: 1.5 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _DIGITALHITACCESS_H
#define _DIGITALHITACCESS_H

#include "../../DataRootObjectLIB/include/digitalHit.h"
#include <string>


/* **************************************************************
 * CLASS digitalHitAccess							 			*
 * **************************************************************/

class digitalHitAccess {

 private:

  digitalHitAccess(const digitalHitAccess&);
  digitalHitAccess& operator=(const digitalHitAccess&);

protected:

	digitalHit*    memory;				/**< Memory which stores the whole digital hit data. */
	unsigned long  numberOfEntries;		/**< Variable to store the number of entries in the memory. */

/**
 * This method allocates new memory.
 */
	
	void allocateNewMemory(unsigned long number);

public:

/**
 * Default constructor
 */

	digitalHitAccess();

/**
 * Destructor
 */

	virtual ~digitalHitAccess();

/**
 * Method inits the variables based on the detector stations.
 */

	void init();

/**
 * This method clears the prelut table.
 */
	
	void clear();

/**
 * method returns the number of entries
 */

	unsigned long getNumberOfEntries();
	unsigned long getNumberOfMembers();

/**
 * This method returns the value from the digital hit table.
 * @param index is the actual index for the digital hit
 */

	digitalHit getEntry(unsigned long index);
	digitalHit getMember(unsigned long index);

/**
 * This method adds the value at the end of the digital hit table.
 * @param value is the actual value for the entry
 */
	
	void addEntry(digitalHit& value);

/**
 * This method converts the prelut table into a string.
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
 * @param terminal is a buffer to place the process information
 */

	void write(std::string fileName, std::string name, std::streambuf* terminal = NULL);

/**
 * This method returns the size of the reserved memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getReservedSizeOfData(unsigned short dimension = 0);

/**
 * This method returns the size of the allocated memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getAllocatedSizeOfData(unsigned short dimension = 0);

/**
 * This method returns the size of the used memory for
 * the source data.
 * @param dimension formats the returnvalue to B, kB, MB or GB
 */

	double getUsedSizeOfData(unsigned short dimension = 0);

};

#endif
