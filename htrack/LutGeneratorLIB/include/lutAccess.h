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
///     - derived from lut
///     - class for the second look-up-table implemented with a file
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-10-24 16:41:17 $
/// $Revision: 1.7 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _lutAccess_H
#define _lutAccess_H


#include "../../MiscLIB/include/defs.h"
#include "../../DataRootObjectLIB/include/digitalHit.h"
#include "../../DataObjectLIB/include/lutAccessFile.h"
#include "lut.h"
#include <string>


/* **************************************************************
 * CLASS lutAccess								 				*
 * **************************************************************/

class lutAccess : public lut {

 private:

  lutAccess(const lutAccess&);
  lutAccess& operator=(const lutAccess&);

protected:

	lutHoughBorder* memory;					/**< Memory which stores the whole look up table. */
	unsigned long   numberOfEntries;		/**< Variable to store the number of entries in the memory. */

/**
 * This method allocates new memory.
 */
	
	void allocateNewMemory(unsigned long number);

public:

/**
 * Default constructor
 */

	lutAccess();

/**
 * Constructor
 * @param dim1Min is the minimal value of the first dimension
 * @param dim1Max is the maximal value of the first dimension
 * @param dim1Step is the stepwidth of the first dimension to build a grid
 * @param dim2Min is the minimal value of the second dimension
 * @param dim2Max is the maximal value of the second dimension
 * @param dim2Step is the stepwidth of the second dimension to build a grid
 */

	lutAccess(double dim1Min, double dim1Max, int dim1Step, double dim2Min, double dim2Max, int dim2Step);

/**
 * Destructor
 */

	virtual ~lutAccess();

/**
 * Method inits the variables based on the detector stations.
 * @param dim1Min is the minimal value of the first dimension
 * @param dim1Max is the maximal value of the first dimension
 * @param dim1Step is the stepwidth of the first dimension to build a grid
 * @param dim2Min is the minimal value of the second dimension
 * @param dim2Max is the maximal value of the second dimension
 * @param dim2Step is the stepwidth of the second dimension to build a grid
 */

	void init(double dim1Min, double dim1Max, int dim1Step, double dim2Min, double dim2Max, int dim2Step);

/**
 * This method returns the magnetic field to use.
 * Caution: This function here has no useful
 * implementations in some subclasses. So be
 * careful when using.
 */

#if (ARCHITECTURE != PS3)

	trackfinderInputMagneticField* getMagneticField();

#endif

/**
 * This method returns the magnetic field factor to use instead of
 * the magneticField.
 * Caution: This function here has no useful
 * implementations in some subclasses. So be
 * careful when using.
 */

	double getMagneticFieldFactor();

/**
 * This method sets the magnetic field to use.
 * Caution: This function here has no useful
 * implementation. It is just here because in
 * another subclass of lut, this function can
 * be used and is very important. This function
 * is virtual in the base class.
 */

#if (ARCHITECTURE != PS3)

	void setMagneticField(trackfinderInputMagneticField* magneticField);

#endif

/**
 * This method sets the magnetic field factor to use instead of
 * the magneticField.
 * Caution: This function here has no useful
 * implementation. It is just here because in
 * another subclass of lut, this function can
 * be used and is very important. This function
 * is virtual in the base class.
 */

	void setMagneticFieldFactor(double magneticFieldFactor);

/**
 * This method returns the number of corrections.
 * Caution: This function here has no useful
 * implementation. It is just here because in
 * another subclass of lut, this function can
 * be used and is very important. This function
 * is virtual in the base class.
 */

	unsigned long getNumberOfCorrections();

/**
 * This method returns the number of coord corrections.
 * Caution: This function here has no useful
 * implementation. It is just here because in
 * another subclass of lut, this function can
 * be used and is very important. This function
 * is virtual in the base class.
 */

	unsigned long getNumberOfCoordCorrections();

/**
 * This method resets both correction counters.
 * Caution: This function here has no useful
 * implementation. It is just here because in
 * another subclass of lut, this function can
 * be used and is very important. This function
 * is virtual in the base class.
 */

	void resetCorrectionCounter();

/**
 * This method evaluates the value from the lut table.
 * @param hit is the actual hit-object to compute the borders
 * @param if borderPointer is null, the computation would be done in an internal object
 * @return if borderPointer is not null, this object would consist of the computed results
 */

#if (ARCHITECTURE != PS3)

	void evaluate(trackfinderInputHit* hit, lutHoughBorder* borderPointer = NULL);

#endif

	void evaluate(digitalHit* hit, lutHoughBorder* borderPointer = NULL);

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
 * This method returns the value from the lut table.
 * @param hit contains the actual index for the borders
 * @param index is the actual index for the borders
 */
	
	lutHoughBorder getEntry(digitalHit* hit);
	lutHoughBorder getEntry(unsigned long index);
	lutHoughBorder getMember(unsigned long index);

/**
 * This method adds the value at the end of the lut table.
 * @param value is the actual value for the entry
 * @param index is the actual index for the borders
 */
	
	void addEntry(lutHoughBorder& value);
	void addEntry(lutHoughBorder& value, unsigned long index);
	void addEntry(lutHoughBorder& value, digitalHit hit);

/**
 * This method converts the lut table into a string.
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
 * @param format is the specifier for the format in which the information is written to file
 * @param terminal is a buffer to place the process information
 */

	void write(std::string fileName, std::string name, unsigned short format = SOFTWAREFORMAT, std::streambuf* terminal = NULL);

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
