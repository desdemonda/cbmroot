/* *******************************************************************
// (C)opyright 2004
// 
// Institute of Computer Science V
// Prof. Männer
// University of Mannheim, Germany
// 
// *******************************************************************
// 
// Designer(s):   Steinle
// 
// *******************************************************************
// 
// Project:     Trackfinder for CBM-Project at GSI-Darmstadt, Germany
// 
// *******************************************************************
// 
// Description:
//
//   class:
//     - derived from io
//     - base class for accessing a configuration file
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-10-24 16:40:01 $
// $Revision: 1.5 $
//
// *******************************************************************/


#include "../include/fileioError.h"
#include "../include/fileioWarningMsg.h"
#include "../include/configuration.h"


/****************************************************************
 * Default constructor											*
 ****************************************************************/

configuration::configuration() : fileio() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

configuration::configuration(int numberOfTchars, char** tchars, std::streambuf* terminal) : fileio() {

	init(numberOfTchars, tchars, terminal);

}
configuration::configuration(std::string name, std::streambuf* terminal) : fileio() {

	init(name, terminal);

}
configuration::configuration(char* name, std::streambuf* terminal) : fileio() {

	init(name, terminal);

}

/****************************************************************
 * Default destructor											*
 ****************************************************************/

configuration::~configuration() {

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void configuration::init(std::streambuf* terminal) {
	
	initFile(terminal);

}
void configuration::init(int numberOfTchars, char** tchars, std::streambuf* terminal) {
	
	setFileName(numberOfTchars, tchars);
	init(terminal);

}
void configuration::init(std::string name, std::streambuf* terminal) {
	
	setFileName(name);
	init(terminal);

}
void configuration::init(char* name, std::streambuf* terminal) {
	
	setFileName(name);
	init(terminal);

}

/****************************************************************
 * This method sets the name of the file.						*
 ****************************************************************/

void configuration::setFileName(int numberOfTchars, char** tchars) {

	std::string temp;

	/* no filename specified, use default */
	if ((numberOfTchars < 1) || (tchars == NULL))
		temp = getDefFileName();
	/* filename specified */
	else {
		temp = tchars[0];
		for (int i = 1; i < numberOfTchars; i++) {
			temp += ' ';
			temp += tchars[i];
		}
	}

	fileio::setFileName(temp);

}
void configuration::setFileName(std::string name) {

	fileio::setFileName(name);

}
void configuration::setFileName(char* name) {

	fileio::setFileName(name);

}

/****************************************************************
 * This method initializes the file.							*
 ****************************************************************/

void configuration::initFile(std::streambuf* terminal) {

	std::ifstream readFileStream;
	std::ofstream writeFileStream;
	cannotOpenFileWarningMsg*   cannotOpenFile;
	writeDefaultFileWarningMsg* writeDefaultFile;

	cannotOpenFile   = NULL;
	writeDefaultFile = NULL;
	if (getFileName().empty() == true)
		throw noFilenameSpecifiedError(FILEIOLIB);
	else
		readFileStream.open(getFileName().c_str());
	/* if configuration file does not exist, write one */
	if (readFileStream.fail()) {
		cannotOpenFile = new cannotOpenFileWarningMsg();
		cannotOpenFile->warningMsg();
		if(cannotOpenFile != NULL) {
			delete cannotOpenFile;
			cannotOpenFile = NULL;
		}
		fileio::setFileName(getDefFileName());
		setHeaderDefValues();
		if (getWriteDefaultHeader()) {
			writeDefaultFile =  new writeDefaultFileWarningMsg(getFileName());
			writeDefaultFile->warningMsg();
			if(writeDefaultFile != NULL) {
				delete writeDefaultFile;
				writeDefaultFile = NULL;
			}
			writeFile(terminal);
		}
	}
	/* use configuration file */
	else {
		readFileStream.close();
		if (readFileStream.fail())
			throw cannotCloseFileError(FILEIOLIB, getFileName());
		readFile(terminal);
	}

}

/****************************************************************
 * This method reads the file.									*
 ****************************************************************/

void configuration::readFile(std::streambuf* terminal) {

	std::ifstream    readFileStream;
	std::string      statusText;
	terminalSequence statusSequence;

	if (getFileName().empty() == true)
		throw noFilenameSpecifiedError(FILEIOLIB);
	else
		readFileStream.open(getFileName().c_str());
	/* if file does not exist */
	if(readFileStream.fail())
		throw cannotOpenFileError(FILEIOLIB, getFileName());
	/* read file */
	else {

		statusText = "\nRead configuration file ";
		statusText += getFileName();
		statusText += ":\t";

		createTerminalStatusSequence(&statusSequence, terminal, statusText, (unsigned int)getNumberOfCmds());
		terminalInitialize(statusSequence);

		readFileHeader(readFileStream, &statusSequence);
		readFileStream.close();

		terminalFinalize(statusSequence);

		if (readFileStream.fail())
			throw cannotCloseFileError(FILEIOLIB, getFileName());
	}

}

/****************************************************************
 * This method writes the file.									*
 ****************************************************************/

void configuration::writeFile(std::streambuf* terminal) {

	std::ofstream    writeFileStream;
	std::string      statusText;
	terminalSequence statusSequence;

	if (getFileName().empty() == true)
		throw noFilenameSpecifiedError(FILEIOLIB);
	else
		writeFileStream.open(getFileName().c_str());
	/* if file cannot be created */
	if(writeFileStream.fail())
		throw cannotCreateFileError(FILEIOLIB, getFileName());
	/* write file */
	else {

		statusText  = "\nWrite configurarion file ";
		statusText += getFileName();
		statusText += ":\t";

		createTerminalStatusSequence(&statusSequence, terminal, statusText, (unsigned int)getNumberOfCmds());
		terminalInitialize(statusSequence);

		writeFileHeader(writeFileStream, &statusSequence);
		writeFileStream.close();

		terminalFinalize(statusSequence);

		if (writeFileStream.fail())
			throw cannotCloseFileError(FILEIOLIB, getFileName());
	}

}
