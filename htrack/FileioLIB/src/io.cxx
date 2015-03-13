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
//     - base class for io with the hdd
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-10-24 16:40:01 $
// $Revision: 1.8 $
//
// *******************************************************************/


#include "../include/fileioError.h"
#include "../include/fileioWarningMsg.h"
#include "../include/io.h"


/****************************************************************
 * This method legalizes a data string.							*
 ****************************************************************/

void legalizeDataString(std::string* data) {

	int         tempValue;
	const char* tempChar;
	
	if (data == NULL)
		throw dataStringNullError();

	tempChar  = data->c_str();

	for (unsigned int i = 0; i < data->length(); i++) {

		tempValue = (int)tempChar[i];

		switch(tempValue) {
			case 0x0A: /* new line */
				data->replace(i, 1 , 1, 0x09); /* horizontal tab*/
				break;
			default:
				break;
		}
	}

}

/****************************************************************
 * This method is to read the data from the file.				*
 ****************************************************************/

void io::readFileData(std::ifstream& fileStream, terminalSequence* statusSequence) {

	std::basic_string<char>::size_type lineDelimiter;
	unsigned int                       numberOfFoundData = 0;
	bool                               dataFound;
	std::string                        buffer;
	std::basic_string<char>::size_type comment_index;
	dataCommentWarningMsg*             dataComment;
	tooMuchDataWarningMsg*             tooMuchData;

	dataComment = NULL;
	tooMuchData = NULL;

	while(std::getline(fileStream, buffer, fileLineDelimiter)) {

		lineDelimiter = buffer.find(fileLineRemover, 0);
		if (lineDelimiter != std::basic_string<char>::npos)
			buffer.erase(lineDelimiter, 1);

		comment_index = buffer.find(fileCommentSeparator, 0);

		/* fileCommentSeparator is found as first character in the actual line */
		if (comment_index == 0) {
			if ((numberOfFoundData != 0) && (numberOfFoundData != numberOfData)) {
				dataComment = new dataCommentWarningMsg();
				dataComment->warningMsg();
				if(dataComment != NULL) {
					delete dataComment;
					dataComment = NULL;
				}
			}
		}
		/* fileCommentSeparator is not found as first character in the actual line */
		else {
			if (data == NULL)
				throw dataPtrError();
			if (numberOfFoundData == numberOfData) {
				tooMuchData = new tooMuchDataWarningMsg();
				tooMuchData->warningMsg();
				if(tooMuchData != NULL) {
					delete tooMuchData;
					tooMuchData = NULL;
				}
				break;
			}
			else {
				dataFound = getDataValue(buffer, numberOfFoundData);
				if (dataFound) {
					numberOfFoundData++;
					if (statusSequence != NULL)
						terminalOverwriteWithIncrement(*statusSequence);
				}
			}
		}

	}

	if (numberOfFoundData != numberOfData)
		throw tooLessDataInFileError();

}

/****************************************************************
 * This method is to write the data to the file.				*
 ****************************************************************/

void io::writeFileData(std::ofstream& fileStream, terminalSequence* statusSequence) {

	std::string buffer;

	if (data == NULL)
		throw dataPtrError();

	for(unsigned long i = 0; i < numberOfData; i++) {
		
		buffer = setDataValue(i);
		legalizeDataString(&buffer);
		buffer += '\n';
		fileStream << buffer;

		if (statusSequence != NULL)
			terminalOverwriteWithIncrement(*statusSequence);

	}

}

/****************************************************************
 * This method legalizes a data string.							*
 ****************************************************************/

std::string io::getDataString(std::string& _data, int index, char delimiter) {

	char                                legalDelimiter;
	std::string                         returnValue;
	std::basic_string <char>::size_type start_index;
	std::basic_string <char>::size_type stop_index;

	/* newline delimiter is not written into file. So here one	*
	 * has to deal with											*/
	switch((int)delimiter) {
		case 0x0A: /* new line */
			legalDelimiter = 0x09; /* horizontal tab*/
			break;
		default:
			legalDelimiter = delimiter;
			break;
	}

	start_index = 0;
	/* search for the start of the substring at position index	*/
	for (int i = 0; i < index; i++) {

		start_index = _data.find(legalDelimiter, start_index);
		/* index is out of range */
		if (start_index == std::basic_string<char>::npos)
			break;
		/* index is found */
		else
			start_index++;
	}

	stop_index = 0;
	/* search for the end of the substring at position index,	*
	 * if start_index is not out of range						*/
	if (start_index != std::basic_string<char>::npos)
		stop_index = _data.find(legalDelimiter, start_index);
	/* if stop index is out of range, the actual index is the	*
	 * last accepted one										*/
	if (stop_index == std::basic_string<char>::npos)
		stop_index = _data.length();
	
	/* if index is out of range, an empty string is returned	*/
	if (start_index == std::basic_string<char>::npos)
		returnValue.clear();
	/* if index is not out of range, the substring is returned	*/
	else
		returnValue = _data.substr(start_index, stop_index - start_index);

	return returnValue;

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

io::io() : fileio(), data(NULL), numberOfData(0) {

	init();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

io::io(std::string name) : fileio(name), data(NULL), numberOfData(0)  {

	init();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

io::io(char* name) : fileio(name), data(NULL), numberOfData(0)  {

	init();

}

/****************************************************************
 * Default destructor											*
 ****************************************************************/

io::~io() {

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void io::init() {

	setDataPtr(NULL);
	setDataNum(0);

}

/****************************************************************
 * This method gets the void pointer to the data structure.		*
 ****************************************************************/

void* io::getDataPtr() {

	return data;

}

/****************************************************************
 * This method sets the void pointer to the data structure.		*
 ****************************************************************/

void io::setDataPtr(void* pointer) {

	data = pointer;

}

/****************************************************************
 * This method gets the number of the data structures.			*
 ****************************************************************/

unsigned long io::getDataNum() {

	return numberOfData;

}

/****************************************************************
 * This method sets the number of the data structures.			*
 ****************************************************************/

void io::setDataNum(unsigned long number) {

	numberOfData = number;

}

/****************************************************************
 * This method reads just the fileHeader.						*
 ****************************************************************/

void io::readJustFileHeader() {

	std::ifstream readFileStream;

	if (getFileName().empty() == true)
		throw noFilenameSpecifiedError(FILEIOLIB);
	else
		readFileStream.open(getFileName().c_str());
	/* if file does not exist */
	if(readFileStream.fail())
		throw cannotOpenFileError(FILEIOLIB, getFileName());
	/* read file */
	else {
		readFileHeader(readFileStream);
		readFileStream.close();
		if (readFileStream.is_open())
			throw cannotCloseFileError(FILEIOLIB, getFileName());
	}

}

/****************************************************************
 * This method reads the file.									*
 ****************************************************************/

void io::readFile(std::streambuf* terminal) {

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

		statusText  = "\nRead data file ";
		statusText += getFileName();
		statusText += ":\t";

		createTerminalStatusSequence(&statusSequence, terminal, statusText, (unsigned int)(getNumberOfCmds() + numberOfData));
		terminalInitialize(statusSequence);

		readFileHeader(readFileStream, &statusSequence);
		if ((getDataNum() == 0) && (getDataPtr() == NULL))
			readAllData();
		readFileData(readFileStream, &statusSequence);
		readFileStream.close();

		terminalFinalize(statusSequence);

		if (readFileStream.is_open())
			throw cannotCloseFileError(FILEIOLIB, getFileName());
	}

}

/****************************************************************
 * This method writes the file.									*
 ****************************************************************/

void io::writeFile(std::streambuf* terminal) {

	std::ofstream    writeFileStream;
	std::string      statusText;
	terminalSequence statusSequence;

	if (getFileName().empty())
		throw noFilenameSpecifiedError(FILEIOLIB);
	else
		writeFileStream.open(getFileName().c_str());
	/* if file cannot be created */
	if(writeFileStream.fail())
		throw cannotCreateFileError(FILEIOLIB, getFileName());
	/* write file */
	else {

		statusText  = "\nWrite data file ";
		statusText += getFileName();
		statusText += ":\t";

		createTerminalStatusSequence(&statusSequence, terminal, statusText, (unsigned int)(getNumberOfCmds() + numberOfData));
		terminalInitialize(statusSequence);

		writeComment(writeFileStream ,"FILE HEADER STARTS");
		writeFileHeader(writeFileStream, &statusSequence);
		writeComment(writeFileStream ,"FILE HEADER STOPS");
		writeComment(writeFileStream , "");
		writeComment(writeFileStream ,"FILE DATA STARTS");
		writeFileData(writeFileStream, &statusSequence);
		writeComment(writeFileStream ,"FILE DATA STOPS");
		writeFileStream.close();

		terminalFinalize(statusSequence);

		if (writeFileStream.fail())
			throw cannotCloseFileError(FILEIOLIB, getFileName());

	}

}
