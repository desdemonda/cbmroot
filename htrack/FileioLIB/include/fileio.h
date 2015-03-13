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
///     - base class for read and write access to the hdd
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-10-24 16:40:01 $
/// $Revision: 1.6 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _FILEIO_H
#define _FILEIO_H


#include "../../MiscLIB/include/terminal.h"
#include <string>
#include <fstream>


/**
 * Defines the length of a single comment in characters.
 * The maximum is defined by the range of the type unsigned short
 */
#define fileCommentLength       201

/**
 * Defines the escape sequences which the file is scanned for
 */
#define fileCmdSeparator        ":="
#define fileCommentSeparator    "/*"
#define fileDisableCmdSeparator "#"
#define fileLineDelimiter       (char)(10)		// Linefeed
#define fileLineRemover         (char)(13)      // Carriage Return


/* **************************************************************
 * CLASS fileio									 				*
 * **************************************************************/

class fileio {

private:

	std::string fileName;					/**< Variable stores the name of the file to access */
	bool*       commandID;					/**< Variable to review which parameter comes from the file. */

	//       fileio(const fileio&);
	//        fileio operator=(const fileio&);

/**
 * allocates memory
 */

	void allocateMemory();

/**
 * deletes allocated memory
 */

	void deleteMemory();

/**
 * This method skips whitespaces in a std::string object by
 * modifying the given iterator.
 * @param buffer is the string to skip the whitespaces
 * @param iterator is the beginning of the text to scan for whitespaces
 * @param itRangeMin is the minimal position of the iterator to scan
 * @param itRangeMax is the maximal position of the iterator to scan
 * @param forward enables the scanning in iterator++ direction. Else
 *        the scanning is done in iterator-- direction
 * @return iterator
 */

	void skipWhitespaces(std::string& buffer, std::string::size_type* iterator, std::string::size_type itRangeMin, std::string::size_type itRangeMax, bool forward);

/**
 * This method returns a legalized comment character.
 */

	char legalizedCommentChar(char value);

	fileio(const fileio&);
	fileio& operator=(const fileio&);
protected:

/**
 * This method is to read the header of the file.
 * @param fileStream is a stream from which the data is read
 * @param statusSequence is an object to place the process information
 */

	void readFileHeader(std::ifstream& fileStream, terminalSequence* statusSequence = NULL);

/**
 * This methods write the value and the corresponding parameter
 * which is identified by a specifier to an ofstream.
 * @param fileStream is a stream to the file
 * @param specifier is the text which identifies the parameter
 * @param value is the value of the parameter
 * @param statusSequence is an object to place the process information
 */

	void setHeaderValue(std::ofstream& fileStream, char* specifier, bool value, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, const char* specifier, bool value, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, std::string specifier, bool value, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, char* specifier, char* value, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, const char* specifier, char* value, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, std::string specifier, char* value, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, char* specifier, const char* value, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, const char* specifier, const char* value, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, std::string specifier, const char* value, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, char* specifier, int value, terminalSequence* statusSequence = NULL, int radix = 10);
	void setHeaderValue(std::ofstream& fileStream, const char* specifier, int value, terminalSequence* statusSequence = NULL, int radix = 10);
	void setHeaderValue(std::ofstream& fileStream, std::string specifier, int value, terminalSequence* statusSequence = NULL, int radix = 10);
	void setHeaderValue(std::ofstream& fileStream, char* specifier, unsigned int value, terminalSequence* statusSequence = NULL, int radix = 10);
	void setHeaderValue(std::ofstream& fileStream, const char* specifier, unsigned int value, terminalSequence* statusSequence = NULL, int radix = 10);
	void setHeaderValue(std::ofstream& fileStream, std::string specifier, unsigned int value, terminalSequence* statusSequence = NULL, int radix = 10);
	void setHeaderValue(std::ofstream& fileStream, char* specifier, long value, terminalSequence* statusSequence = NULL, int radix = 10);
	void setHeaderValue(std::ofstream& fileStream, const char* specifier, long value, terminalSequence* statusSequence = NULL, int radix = 10);
	void setHeaderValue(std::ofstream& fileStream, std::string specifier, long value, terminalSequence* statusSequence = NULL, int radix = 10);
	void setHeaderValue(std::ofstream& fileStream, char* specifier, unsigned long value, terminalSequence* statusSequence = NULL, int radix = 10);
	void setHeaderValue(std::ofstream& fileStream, const char* specifier, unsigned long value, terminalSequence* statusSequence = NULL, int radix = 10);
	void setHeaderValue(std::ofstream& fileStream, std::string specifier, unsigned long value, terminalSequence* statusSequence = NULL, int radix = 10);
	void setHeaderValue(std::ofstream& fileStream, char* specifier, float value, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, const char* specifier, float value, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, std::string specifier, float value, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, char* specifier, double value, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, const char* specifier, double value, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, std::string specifier, double value, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, char* specifier, std::string& value, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, const char* specifier, std::string& value, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, std::string specifier, std::string& value, terminalSequence* statusSequence = NULL);

	void setHeaderValue(std::ofstream& fileStream, char* specifier, bool value, char* comment, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, const char* specifier, bool value, char* comment, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, const char* specifier, bool value, const char* comment, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, std::string specifier, bool value, std::string comment, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, char* specifier, char* value, char* comment, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, const char* specifier, char* value, char* comment, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, const char* specifier, char* value, const char* comment, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, std::string specifier, char* value, std::string comment, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, char* specifier, const char* value, char* comment, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, const char* specifier, const char* value, char* comment, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, const char* specifier, const char* value, const char* comment, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, std::string specifier, const char* value, std::string comment, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, char* specifier, int value, char* comment, terminalSequence* statusSequence = NULL, int radix = 10);
	void setHeaderValue(std::ofstream& fileStream, const char* specifier, int value, char* comment, terminalSequence* statusSequence = NULL, int radix = 10);
	void setHeaderValue(std::ofstream& fileStream, const char* specifier, int value, const char* comment, terminalSequence* statusSequence = NULL, int radix = 10);
	void setHeaderValue(std::ofstream& fileStream, std::string specifier, int value, std::string comment, terminalSequence* statusSequence = NULL, int radix = 10);
	void setHeaderValue(std::ofstream& fileStream, char* specifier, unsigned int value, char* comment, terminalSequence* statusSequence = NULL, int radix = 10);
	void setHeaderValue(std::ofstream& fileStream, const char* specifier, unsigned int value, char* comment, terminalSequence* statusSequence = NULL, int radix = 10);
	void setHeaderValue(std::ofstream& fileStream, const char* specifier, unsigned int value, const char* comment, terminalSequence* statusSequence = NULL, int radix = 10);
	void setHeaderValue(std::ofstream& fileStream, std::string specifier, unsigned int value, std::string comment, terminalSequence* statusSequence = NULL, int radix = 10);
	void setHeaderValue(std::ofstream& fileStream, char* specifier, long value, char* comment, terminalSequence* statusSequence = NULL, int radix = 10);
	void setHeaderValue(std::ofstream& fileStream, const char* specifier, long value, char* comment, terminalSequence* statusSequence = NULL, int radix = 10);
	void setHeaderValue(std::ofstream& fileStream, const char* specifier, long value, const char* comment, terminalSequence* statusSequence = NULL, int radix = 10);
	void setHeaderValue(std::ofstream& fileStream, std::string specifier, long value, std::string comment, terminalSequence* statusSequence = NULL, int radix = 10);
	void setHeaderValue(std::ofstream& fileStream, char* specifier, unsigned long value, char* comment, terminalSequence* statusSequence = NULL, int radix = 10);
	void setHeaderValue(std::ofstream& fileStream, const char* specifier, unsigned long value, char* comment, terminalSequence* statusSequence = NULL, int radix = 10);
	void setHeaderValue(std::ofstream& fileStream, const char* specifier, unsigned long value, const char* comment, terminalSequence* statusSequence = NULL, int radix = 10);
	void setHeaderValue(std::ofstream& fileStream, std::string specifier, unsigned long value, std::string comment, terminalSequence* statusSequence = NULL, int radix = 10);
	void setHeaderValue(std::ofstream& fileStream, char* specifier, float value, char* comment, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, const char* specifier, float value, char* comment, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, const char* specifier, float value, const char* comment, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, std::string specifier, float value, std::string comment, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, char* specifier, double value, char* comment, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, const char* specifier, double value, char* comment, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, const char* specifier, double value, const char* comment, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, std::string specifier, double value, std::string comment, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, char* specifier, std::string& value, char* comment, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, const char* specifier, std::string& value, char* comment, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, const char* specifier, std::string& value, const char* comment, terminalSequence* statusSequence = NULL);
	void setHeaderValue(std::ofstream& fileStream, std::string specifier, std::string& value, std::string comment, terminalSequence* statusSequence = NULL);

/**
 * This methods writes a comment to an ofstream.
 */

	void writeComment(std::ofstream& fileStream, char* comment);
	void writeComment(std::ofstream& fileStream, const char* comment);
	void writeComment(std::ofstream& fileStream, std::string comment);

/**
 * This method returns the number of accepted commands.
 */

	virtual unsigned int getNumberOfCmds() = 0;

/**
 * This method assigns the value to the corresponding parameter
 * which is identified by a specifier.
 */

	virtual bool getHeaderValue(std::string& specifier, std::string& value) = 0;

/**
 * This method is to write the header of the file.
 * @param fileStream is a stream to which the data is written
 * @param statusSequence is an object to place the process information
 */

	virtual void writeFileHeader(std::ofstream& fileStream, terminalSequence* statusSequence = NULL) = 0;

/**
 * This method is to set the default values for each header
 * parameter.
 */

	virtual void setHeaderDefValues() = 0;

public:

/**
 * Default constructor
 */

	fileio();

/**
 * Constructor
 * @param name is the name of the file to access
 */

	fileio(std::string name);
	fileio(char* name);

/**
 * Destructor
 */

	virtual ~fileio();

/**
 * This method initializes the object.
 * @param name is the name of the file to access
 */

	void init(std::string name);
	void init(char* name);

/**
 * This method returns if the lock in the commandID is set.
 */

	bool isHeaderLockSet(unsigned int lockId);

/**
 * This method sets the lock in the commandID.
 */

	void setHeaderLock(unsigned int lockId, bool value = true);

/**
 * This method reopens all commandID locks.
 */

	void resetAllHeaderLocks();

/**
 * This method resets the header to the defualt values
 * and reopens the commandID lock.
 */

	void resetHeader();

/**
 * This method returns the name of the file.
 */

	std::string getFileName();

/**
 * This method sets the name of the file.
 */

	void setFileName(std::string name);
	void setFileName(char* name);

/**
 * This method returns an information string about the object.										*
 */

	virtual std::string getInfo() = 0;

/**
 * This method reads the file.
 * @param terminal is a buffer to place the process information
 */

	virtual void readFile(std::streambuf* terminal = NULL) = 0;

/**
 * This method writes the file.
 * @param terminal is a buffer to place the process information
 */

	virtual void writeFile(std::streambuf* terminal = NULL) = 0;

};

#endif
