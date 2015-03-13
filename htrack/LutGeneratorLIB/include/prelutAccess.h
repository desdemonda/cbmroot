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
///     - derived from prelut
///     - class for the first look-up-table implemented with a file
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-10-24 16:41:18 $
/// $Revision: 1.7 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _PRELUTACCESS_H
#define _PRELUTACCESS_H


#include "../../MiscLIB/include/defs.h"
#include "../../DataRootObjectLIB/include/digitalHit.h"
#include "../../DataObjectLIB/include/prelutAccessFile.h"
#include "prelut.h"
#include <string>


/* **************************************************************
 * CLASS prelutAccess								 			*
 * **************************************************************/

class prelutAccess : public prelut {

 private:

  prelutAccess(const prelutAccess&);
  prelutAccess& operator=(const prelutAccess&);

protected:

	prelutHoughBorder* memory;				/**< Memory which stores the whole look up table. */
	unsigned long      numberOfEntries;		/**< Variable to store the number of entries in the memory. */

/**
 * This method allocates new memory.
 */
	
	void allocateNewMemory(unsigned long number);

public:

/**
 * Default constructor
 */

	prelutAccess();

/**
 * Constructor
 * @param dim3Min is the minimal value of the first dimension
 * @param dim3Max is the maximal value of the first dimension
 * @param dim3Step is the stepwidth of the first dimension to build a grid
 * @param dim3StartEntry is the minimal value of the second dimension
 * @param dim3StopEntry is the maximal value of the second dimension
 */

	prelutAccess(double dim3Min, double dim3Max, int dim3Step, double dim3StartEntry, double dim3StopEntry);

/**
 * Destructor
 */

	virtual ~prelutAccess();

/**
 * Method inits the variables based on the detector stations.
 * @param dim3Min is the minimal value of the first dimension
 * @param dim3Max is the maximal value of the first dimension
 * @param dim3Step is the stepwidth of the first dimension to build a grid
 * @param dim3StartEntry is the minimal value of the second dimension
 * @param dim3StopEntry is the maximal value of the second dimension
 */

	void init(double dim3Min, double dim3Max, int dim3Step, double dim3StartEntry, double dim3StopEntry);

/**
 * This method evaluates the value from the prelut table.
 * @param hit is the actual hit-object to compute the borders
 * @param if borderPointer is null, the computation would be done in an internal object
 * @return if borderPointer is not null, this object would consist of the computed results
 */

#if (ARCHITECTURE != PS3)

	void evaluate(trackfinderInputHit* hit, prelutHoughBorder* borderPointer = NULL);

#endif

	void evaluate(digitalHit* hit, prelutHoughBorder* borderPointer = NULL);

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
 * This method returns the value from the prelut table.
 * @param hit contains the actual index for the borders
 * @param index is the actual index for the borders
 */
	
	prelutHoughBorder getEntry(digitalHit* hit);
	prelutHoughBorder getEntry(unsigned long index);
	prelutHoughBorder getMember(unsigned long index);

/**
 * This method adds the value at the end of the prelut table.
 * @param value is the actual value for the entry
 * @param index is the actual index for the borders
 */
	
	void addEntry(prelutHoughBorder& value);
	void addEntry(prelutHoughBorder& value, unsigned long index);
	void addEntry(prelutHoughBorder& value, digitalHit hit);

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
