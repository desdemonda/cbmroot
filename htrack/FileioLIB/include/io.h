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
///     - base class for io with the hdd
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-10-24 16:40:01 $
/// $Revision: 1.5 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _IO_H
#define _IO_H


#include "fileio.h"


/* **************************************************************
 * CLASS io										 				*
 * **************************************************************/

class io : public fileio {

private:

	void*          data;			/**< Pointer to the data which should be accessed */
	unsigned long  numberOfData;	/**< Variable to store the size of the data */

	//	io(const io&);
	//	io operator=(const io&);

protected:

/**
 * This method is to read the data from the file.
 * @param fileStream is a stream from which the data is read
 * @param statusSequence is an object to place the process information
 */

	void readFileData(std::ifstream& fileStream, terminalSequence* statusSequence = NULL);

/**
 * This method is to write the data to the file.
 * @param fileStream is a stream to which the data is written
 * @param statusSequence is an object to place the process information
 */

	void writeFileData(std::ofstream& fileStream, terminalSequence* statusSequence = NULL);

/**
 * This method legalizes a data string.
 */

	std::string getDataString(std::string& data, int index, char delimiter);

/**
 * This method sets the number of the data structures
 * to the value that all data is read from file. Normally
 * this value should be in the header of the file as one
 * command.
 */

	virtual void readAllData() = 0;

/**
 * This method assigns the data to the corresponding structure
 * The whole data structure to read must be in one single line
 * in the file.
 */

	virtual bool getDataValue(std::string& buffer, unsigned long index) = 0;

/**
 * This method writes one data structure to the stream.
 * The whole data structure to write must be in one single line
 * in the file.
 */

	virtual std::string setDataValue(unsigned long index) = 0;

	io(const io&);
	io& operator=(const io&);
public:

/**
 * Default constructor
 */

	io();

/**
 * Constructor
 * @param name is the name of the file to access
 */

	io(std::string name);
	io(char* name);

/**
 * Destructor
 */

	virtual ~io();

/**
 * This method initializes the object.
 */

	void init();

/**
 * This method gets the void pointer to the data structure.
 */

	void* getDataPtr();

/**
 * This method sets the void pointer to the data structure.
 */

	void setDataPtr(void* pointer);

/**
 * This method gets the number of the data structures.
 */

	unsigned long getDataNum();

/**
 * This method sets the number of the data structures.
 */

	void setDataNum(unsigned long number);

/**
 * This method reads just the fileHeader.
 */

	void readJustFileHeader();

/**
 * This method reads the file.
 * @param terminal is a buffer to place the process information
 */

	void readFile(std::streambuf* terminal = NULL);

/**
 * This method writes the file.
 * @param terminal is a buffer to place the process information
 */

	void writeFile(std::streambuf* terminal = NULL);

};

#endif
