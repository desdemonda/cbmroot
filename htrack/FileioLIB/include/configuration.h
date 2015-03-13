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
///     - derived from io
///     - base class for accessing a configuration file
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-10-24 16:40:01 $
/// $Revision: 1.4 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _CONFIGURATION_H
#define _CONFIGURATION_H


#include "fileio.h"


/* **************************************************************
 * CLASS configuration							 				*
 * **************************************************************/

class configuration : public fileio {

protected:

/**
 * This method is to get the default name of the file.
 */

	virtual std::string getDefFileName() = 0;

/**
 * This method must return true if a default file should be
 * written.
 */

	virtual bool getWriteDefaultHeader() = 0;

public:

/**
 * Default constructor
 */

	configuration();

/**
 * Constructor
 * @param name is the name of the file to access
 * @param terminal is a buffer to place the process information
 */

	configuration(int numberOfTchars, char** tchars, std::streambuf* terminal = NULL);
	configuration(std::string name, std::streambuf* terminal = NULL);
	configuration(char* name, std::streambuf* terminal = NULL);

/**
 * Destructor
 */

	virtual ~configuration();

/**
 * This method initializes the object.
 * @param terminal is a buffer to place the process information
 */

	void init(std::streambuf* terminal = NULL);
	void init(int numberOfTchars, char** tchars, std::streambuf* terminal = NULL);
	void init(std::string name, std::streambuf* terminal = NULL);
	void init(char* name, std::streambuf* terminal = NULL);

/**
 * This method sets the name of the file.
 */

	void setFileName(int numberOfTchars, char** tchars);
	void setFileName(std::string name);
	void setFileName(char* name);

/**
 * This method initializes the file.
 * @param terminal is a buffer to place the process information
 */

	void initFile(std::streambuf* terminal = NULL);

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
