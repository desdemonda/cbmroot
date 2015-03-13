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
//     - base class for read and write access to the hdd
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-10-24 16:40:01 $
// $Revision: 1.6 $
//
// *******************************************************************/


#include "../../MiscLIB/include/defs.h"
#include "../../MiscLIB/include/conversionRoutines.h"
#include "../include/fileioError.h"
#include "../include/fileioWarningMsg.h"
#include "../include/fileio.h"

#include <cstdlib>

/****************************************************************
 * allocates memory												*
 ****************************************************************/

void fileio::allocateMemory() {

	if (commandID == NULL) {

		commandID = new bool[getNumberOfCmds()];
		if (commandID == NULL)
			throw memoryAllocationError(FILEIOLIB);

	}

}

/****************************************************************
 * deletes allocated memory										*
 ****************************************************************/

void fileio::deleteMemory() {

	if (commandID != NULL) {

		delete [] commandID;
		commandID = NULL;

	}
}

/****************************************************************
 * This method skips whitespaces in a std::string object by		*
 * modifying the given iterator.								*
 ****************************************************************/

void fileio::skipWhitespaces(std::string& buffer, std::string::size_type* iterator, std::string::size_type itRangeMin, std::string::size_type itRangeMax, bool forward) {

	int                    tempInt;
	bool                   whitespaceFound = true;
	const char*            tempChar;
	std::string            tempString;
	std::string::size_type sizeTypeMin =  0;
	std::string::size_type sizeTypeMax = (unsigned)-1;

	if (iterator == NULL)
		throw wrongIteratorError();

	if ((*iterator >= itRangeMin) && (*iterator <= itRangeMax)) {

		while (whitespaceFound) {
	
			tempString = buffer.substr(*iterator, 1);
			tempChar   = tempString.c_str();
			tempInt = (int) (*tempChar);

			switch(tempInt) {
				case 0x09: /* horizontal tab */
				case 0x0A: /* new line */
				case 0x0B: /* vertical tab */
				case 0x0C: /* new page */
				case 0x0D: /* carriage return */
				case 0x20: /* space */
					if (forward == true) {
						/* iterator smaller than maximal range */
						if (*iterator < itRangeMax)
							(*iterator)++;
						/* iterator equal to maximal range */
						else {
						/* iterator range smaller than maximal variable range: overflow protection */
							if (itRangeMax < sizeTypeMax)
								(*iterator)++;
							whitespaceFound = false;
						}
					}
					else {
						/* iterator bigger than minimal range */
						if (*iterator > itRangeMin)
							(*iterator)--;
						/* iterator equal to minimal range */
						else {
						/* iterator range bigger than minimal variable range: underflow protection */
							if (itRangeMin > sizeTypeMin)
								(*iterator)--;
							whitespaceFound = false;
						}
					}
					break;
				default:
					whitespaceFound = false;
					break;
			}
		}
	}
	
}

/****************************************************************
 * This method returns a legalized comment character.			*
 ****************************************************************/

char fileio::legalizedCommentChar(char value) {

	int  tempValue;
	char legalComment;
	
	tempValue = (int) value;

	switch(tempValue) {
		case 0x0A: /* new line */
		case 0x0B: /* vertical tab */
		case 0x0C: /* new page */
		case 0x0D: /* carriage return */
			legalComment = 0X09; /* horizontal tab*/
			break;
		default:
			legalComment = value;
			break;
	}

	return legalComment;

}


/****************************************************************
 * This method is to read the header of the file.				*
 ****************************************************************/

void fileio::readFileHeader(std::ifstream& fileStream, terminalSequence* statusSequence) {

	std::basic_string<char>::size_type lineDelimiter;
	bool                               specifierFound;
	unsigned int                       numberOfFoundSpecifiers;
	std::string                        buffer;
	std::string                        specifier;
	std::string                        value;
	std::basic_string<char>::size_type startIndex;
	std::basic_string<char>::size_type cmdIndex;
	std::basic_string<char>::size_type commentIndex;
	std::basic_string<char>::size_type disableCmdIndex;
	std::basic_string<char>::size_type stopIndex;
	std::basic_string<char>::size_type indexMin;
	std::basic_string<char>::size_type indexMax;
	missingCmdSpecWarningMsg*          missingCmdSpec;
	missingCmdValWarningMsg*           missingCmdVal;
	unknownCmdSpecValPairWarningMsg*   unknownCmdSpecValPair;

	numberOfFoundSpecifiers = 0;
	missingCmdSpec          = NULL;
	missingCmdVal           = NULL;
	unknownCmdSpecValPair   = NULL;

	resetAllHeaderLocks();

	while(std::getline(fileStream, buffer, fileLineDelimiter)) {

		lineDelimiter = buffer.find(fileLineRemover, 0);
		if (lineDelimiter != std::basic_string<char>::npos)
			buffer.erase(lineDelimiter, 1);

		cmdIndex        = buffer.find(fileCmdSeparator, 0);
		commentIndex    = buffer.find(fileCommentSeparator, 0);
		disableCmdIndex = buffer.find(fileDisableCmdSeparator, 0);

		/* fileCmdSeparator is found as first character in the actual line */
		if (cmdIndex == 0) {
			missingCmdSpec = new missingCmdSpecWarningMsg();
			missingCmdSpec->warningMsg();
			if(missingCmdSpec != NULL) {
				delete missingCmdSpec;
				missingCmdSpec = NULL;
			}
			cmdIndex = std::string::npos;
		}

		if ((disableCmdIndex == std::string::npos) || (commentIndex == std::string::npos) || (commentIndex > cmdIndex)) {

			if (cmdIndex != std::string::npos) {

			/* extract specifier */
				/* setting indexes and ranges */
				startIndex = 0;
				stopIndex  = cmdIndex - 1;
				indexMin   = startIndex;
				indexMax   = stopIndex;
				/* skips whitespeces */
				skipWhitespaces(buffer, &startIndex, indexMin, indexMax, true);
				skipWhitespaces(buffer, &stopIndex, indexMin, indexMax, false);
				/* no specifier found */
				if (stopIndex < startIndex) {
					missingCmdSpec = new missingCmdSpecWarningMsg();
					missingCmdSpec->warningMsg();
					if(missingCmdSpec != NULL) {
						delete missingCmdSpec;
						missingCmdSpec = NULL;
					}
				}
				/* evaluate specifier */
				else {
					specifier = buffer.substr(startIndex, stopIndex-startIndex+1);
			/* extract value */
				/* setting indexes and ranges */
					startIndex = cmdIndex + sizeof(fileCmdSeparator) - 1;
					/* find start of comment */
					stopIndex  = buffer.find(fileCommentSeparator, startIndex);
					/* comment found */
					if (stopIndex != std::string::npos)
						stopIndex--;
					/* no comment found */
					else
						stopIndex = buffer.length() - 1;
					indexMin = startIndex;
					indexMax = stopIndex;
				/* skips whitespaces */
					skipWhitespaces(buffer, &startIndex, indexMin, indexMax, true);
					skipWhitespaces(buffer, &stopIndex, indexMin, indexMax, false);
				/* no value found */
					if (stopIndex < startIndex) {
						missingCmdVal = new missingCmdValWarningMsg();
						missingCmdVal->warningMsg();
						if(missingCmdVal != NULL) {
							delete missingCmdVal;
							missingCmdVal = NULL;
						}
					}
				/* evaluate value */
					else {
					/* get value for specifier */
						value = buffer.substr(startIndex, stopIndex-startIndex+1);
					
					/* look for specifier and setup value */
						specifierFound = getHeaderValue(specifier, value);
	
					/* specifier-value-pair found */
						if (specifierFound == true) {
							numberOfFoundSpecifiers++;
							if (statusSequence != NULL)
								terminalOverwriteWithIncrement(*statusSequence);
						}
					/* specifier-value-pair mismatch */
						else {
							unknownCmdSpecValPair = new unknownCmdSpecValPairWarningMsg();
							unknownCmdSpecValPair->warningMsg();
							if(unknownCmdSpecValPair != NULL) {
								delete unknownCmdSpecValPair;
								unknownCmdSpecValPair = NULL;
							}
						}
					}
				}
			}
		}
		if (numberOfFoundSpecifiers == getNumberOfCmds())
			break;
	}

	if (numberOfFoundSpecifiers != getNumberOfCmds())
		throw tooLessCmdsError();

}

/****************************************************************
 * This methods write the value and the corresponding parameter	*
 * which is identified by a specifier to an ofstream.			*
 ****************************************************************/

void fileio::setHeaderValue(std::ofstream& fileStream, char* specifier, bool value, terminalSequence* statusSequence) {

	char buffer[6]; /* 6 = sizeof(false+'\0')*/

	btobs(value, buffer);
	setHeaderValue(fileStream, specifier, buffer, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, const char* specifier, bool value, terminalSequence* statusSequence) {

	char buffer[6]; /* 6 = sizeof(false+'\0')*/

	btobs(value, buffer);
	setHeaderValue(fileStream, specifier, buffer, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, std::string specifier, bool value, terminalSequence* statusSequence) {

	setHeaderValue(fileStream, specifier.c_str(), value, statusSequence);

}

void fileio::setHeaderValue(std::ofstream& fileStream, char* specifier, char* value, terminalSequence* statusSequence) {

	fileStream << specifier << ' ' << fileCmdSeparator << ' ' << value << std::endl;

	if (statusSequence != NULL)
		terminalOverwriteWithIncrement(*statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, const char* specifier, char* value, terminalSequence* statusSequence) {

	fileStream << specifier << ' ' << fileCmdSeparator << ' ' << value << std::endl;

	if (statusSequence != NULL)
		terminalOverwriteWithIncrement(*statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, std::string specifier, char* value, terminalSequence* statusSequence) {

	setHeaderValue(fileStream, specifier.c_str(), value, statusSequence);

}

void fileio::setHeaderValue(std::ofstream& fileStream, char* specifier, const char* value, terminalSequence* statusSequence) {

	fileStream << specifier << ' ' << fileCmdSeparator << ' ' << value << std::endl;

	if (statusSequence != NULL)
		terminalOverwriteWithIncrement(*statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, const char* specifier, const char* value, terminalSequence* statusSequence) {

	fileStream << specifier << ' ' << fileCmdSeparator << ' ' << value << std::endl;

	if (statusSequence != NULL)
		terminalOverwriteWithIncrement(*statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, std::string specifier, const char* value, terminalSequence* statusSequence) {

	setHeaderValue(fileStream, specifier.c_str(), value, statusSequence);

}

void fileio::setHeaderValue(std::ofstream& fileStream, char* specifier, int value, terminalSequence* statusSequence, int radix) {

	char buffer[intConversionDigits+1];

	itos(value, buffer, radix, intConversionDigits);
	setHeaderValue(fileStream, specifier, buffer, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, const char* specifier, int value, terminalSequence* statusSequence, int radix) {

	char buffer[intConversionDigits+1];

	itos(value, buffer, radix, intConversionDigits);
	setHeaderValue(fileStream, specifier, buffer, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, std::string specifier, int value, terminalSequence* statusSequence, int radix) {
	
	setHeaderValue(fileStream, specifier.c_str(), value, statusSequence, radix);

}

void fileio::setHeaderValue(std::ofstream& fileStream, char* specifier, unsigned int value, terminalSequence* statusSequence, int radix) {

	char buffer[intConversionDigits+1];

	uitos(value, buffer, radix, intConversionDigits);
	setHeaderValue(fileStream, specifier, buffer, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, const char* specifier, unsigned int value, terminalSequence* statusSequence, int radix) {

	char buffer[intConversionDigits+1];

	uitos(value, buffer, radix, intConversionDigits);
	setHeaderValue(fileStream, specifier, buffer, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, std::string specifier, unsigned int value, terminalSequence* statusSequence, int radix) {

	setHeaderValue(fileStream, specifier.c_str(), value, statusSequence, radix);

}

void fileio::setHeaderValue(std::ofstream& fileStream, char* specifier, long value, terminalSequence* statusSequence, int radix) {

	char buffer[longConversionDigits+1];

	ltos(value, buffer, radix, longConversionDigits);
	setHeaderValue(fileStream, specifier, buffer, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, const char* specifier, long value, terminalSequence* statusSequence, int radix) {

	char buffer[longConversionDigits+1];

	ltos(value, buffer, radix, longConversionDigits);
	setHeaderValue(fileStream, specifier, buffer, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, std::string specifier, long value, terminalSequence* statusSequence, int radix) {

	setHeaderValue(fileStream, specifier.c_str(), value, statusSequence, radix);

}

void fileio::setHeaderValue(std::ofstream& fileStream, char* specifier, unsigned long value, terminalSequence* statusSequence, int radix) {

	char buffer[longConversionDigits+1];

	ultos(value, buffer, radix, longConversionDigits);
	setHeaderValue(fileStream, specifier, buffer, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, const char* specifier, unsigned long value, terminalSequence* statusSequence, int radix) {

	char buffer[longConversionDigits+1];

	ultos(value, buffer, radix, longConversionDigits);
	setHeaderValue(fileStream, specifier, buffer, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, std::string specifier, unsigned long value, terminalSequence* statusSequence, int radix) {

	setHeaderValue(fileStream, specifier.c_str(), value, statusSequence, radix);

}

void fileio::setHeaderValue(std::ofstream& fileStream, char* specifier, float value, terminalSequence* statusSequence) {

	double temp;

	temp = (double) value;
	setHeaderValue(fileStream, specifier, temp, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, const char* specifier, float value, terminalSequence* statusSequence) {

	double temp;

	temp = (double) value;
	setHeaderValue(fileStream, specifier, temp, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, std::string specifier, float value, terminalSequence* statusSequence) {

	setHeaderValue(fileStream, specifier.c_str(), value, statusSequence);

}

void fileio::setHeaderValue(std::ofstream& fileStream, char* specifier, double value, terminalSequence* statusSequence) {

	char buffer[doubleConversion+1];

	dtos(value, buffer, doubleConversionDigits);
	setHeaderValue(fileStream, specifier, buffer, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, const char* specifier, double value, terminalSequence* statusSequence) {

	char buffer[doubleConversion+1];

	dtos(value, buffer, doubleConversionDigits);
	setHeaderValue(fileStream, specifier, buffer, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, std::string specifier, double value, terminalSequence* statusSequence) {

	setHeaderValue(fileStream, specifier.c_str(), value, statusSequence);

}

void fileio::setHeaderValue(std::ofstream& fileStream, char* specifier, std::string& value, terminalSequence* statusSequence) {

	char* temp;

	temp = (char*) value.c_str();
	setHeaderValue(fileStream, specifier, temp, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, const char* specifier, std::string& value, terminalSequence* statusSequence) {

	char* temp;

	temp = (char*) value.c_str();
	setHeaderValue(fileStream, specifier, temp, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, std::string specifier, std::string& value, terminalSequence* statusSequence) {

	setHeaderValue(fileStream, specifier.c_str(), value, statusSequence);

}

void fileio::setHeaderValue(std::ofstream& fileStream, char* specifier, bool value, char* comment, terminalSequence* statusSequence) {

	char buffer[6]; /* 6 = sizeof(false+'\0')*/

	btobs(value, buffer);
	setHeaderValue(fileStream, specifier, buffer, comment, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, const char* specifier, bool value, char* comment, terminalSequence* statusSequence) {

	char buffer[6]; /* 6 = sizeof(false+'\0')*/

	btobs(value, buffer);
	setHeaderValue(fileStream, specifier, buffer, comment, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, const char* specifier, bool value, const char* comment, terminalSequence* statusSequence) {

	char buffer[6]; /* 6 = sizeof(false+'\0')*/

	btobs(value, buffer);
	setHeaderValue(fileStream, specifier, buffer, comment, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, std::string specifier, bool value, std::string comment, terminalSequence* statusSequence) {

	setHeaderValue(fileStream, specifier.c_str(), value, comment.c_str(), statusSequence);

}

void fileio::setHeaderValue(std::ofstream& fileStream, char* specifier, char* value, char* comment, terminalSequence* statusSequence) {

	fileStream << specifier << ' ' << fileCmdSeparator << ' ' << value << "\t\t";
	writeComment(fileStream, comment);

	if (statusSequence != NULL)
		terminalOverwriteWithIncrement(*statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, const char* specifier, char* value, char* comment, terminalSequence* statusSequence) {

	fileStream << specifier << ' ' << fileCmdSeparator << ' ' << value << "\t\t";
	writeComment(fileStream, comment);

	if (statusSequence != NULL)
		terminalOverwriteWithIncrement(*statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, const char* specifier, char* value, const char* comment, terminalSequence* statusSequence) {

	fileStream << specifier << ' ' << fileCmdSeparator << ' ' << value << "\t\t";
	writeComment(fileStream, comment);

	if (statusSequence != NULL)
		terminalOverwriteWithIncrement(*statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, std::string specifier, char* value, std::string comment, terminalSequence* statusSequence) {

	setHeaderValue(fileStream, specifier.c_str(), value, comment.c_str(), statusSequence);

}

void fileio::setHeaderValue(std::ofstream& fileStream, char* specifier, const char* value, char* comment, terminalSequence* statusSequence) {

	fileStream << specifier << ' ' << fileCmdSeparator << ' ' << value << "\t\t";
	writeComment(fileStream, comment);

	if (statusSequence != NULL)
		terminalOverwriteWithIncrement(*statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, const char* specifier, const char* value, char* comment, terminalSequence* statusSequence) {

	fileStream << specifier << ' ' << fileCmdSeparator << ' ' << value << "\t\t";
	writeComment(fileStream, comment);

	if (statusSequence != NULL)
		terminalOverwriteWithIncrement(*statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, const char* specifier, const char* value, const char* comment, terminalSequence* statusSequence) {

	fileStream << specifier << ' ' << fileCmdSeparator << ' ' << value << "\t\t";
	writeComment(fileStream, comment);

	if (statusSequence != NULL)
		terminalOverwriteWithIncrement(*statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, std::string specifier, const char* value, std::string comment, terminalSequence* statusSequence) {

	setHeaderValue(fileStream, specifier.c_str(), value, comment.c_str(), statusSequence);

}

void fileio::setHeaderValue(std::ofstream& fileStream, char* specifier, int value, char* comment, terminalSequence* statusSequence, int radix) {

	char buffer[intConversionDigits+1];

	itos(value, buffer, radix, intConversionDigits);
	setHeaderValue(fileStream, specifier, buffer, comment, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, const char* specifier, int value, char* comment, terminalSequence* statusSequence, int radix) {

	char buffer[intConversionDigits+1];

	itos(value, buffer, radix, intConversionDigits);
	setHeaderValue(fileStream, specifier, buffer, comment, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, const char* specifier, int value, const char* comment, terminalSequence* statusSequence, int radix) {

	char buffer[intConversionDigits+1];

	itos(value, buffer, radix, intConversionDigits);
	setHeaderValue(fileStream, specifier, buffer, comment, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, std::string specifier, int value, std::string comment, terminalSequence* statusSequence, int radix) {
	
	setHeaderValue(fileStream, specifier.c_str(), value, comment.c_str(), statusSequence, radix);

}

void fileio::setHeaderValue(std::ofstream& fileStream, char* specifier, unsigned int value, char* comment, terminalSequence* statusSequence, int radix) {

	char buffer[intConversionDigits+1];

	uitos(value, buffer, radix, intConversionDigits);
	setHeaderValue(fileStream, specifier, buffer, comment, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, const char* specifier, unsigned int value, char* comment, terminalSequence* statusSequence, int radix) {

	char buffer[intConversionDigits+1];

	uitos(value, buffer, radix, intConversionDigits);
	setHeaderValue(fileStream, specifier, buffer, comment, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, const char* specifier, unsigned int value, const char* comment, terminalSequence* statusSequence, int radix) {

	char buffer[intConversionDigits+1];

	uitos(value, buffer, radix, intConversionDigits);
	setHeaderValue(fileStream, specifier, buffer, comment, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, std::string specifier, unsigned int value, std::string comment, terminalSequence* statusSequence, int radix) {

	setHeaderValue(fileStream, specifier.c_str(), value, comment.c_str(), statusSequence, radix);

}

void fileio::setHeaderValue(std::ofstream& fileStream, char* specifier, long value, char* comment, terminalSequence* statusSequence, int radix) {

	char buffer[longConversionDigits+1];

	ltos(value, buffer, radix, longConversionDigits);
	setHeaderValue(fileStream, specifier, buffer, comment, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, const char* specifier, long value, char* comment, terminalSequence* statusSequence, int radix) {

	char buffer[longConversionDigits+1];

	ltos(value, buffer, radix, longConversionDigits);
	setHeaderValue(fileStream, specifier, buffer, comment, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, const char* specifier, long value, const char* comment, terminalSequence* statusSequence, int radix) {

	char buffer[longConversionDigits+1];

	ltos(value, buffer, radix, longConversionDigits);
	setHeaderValue(fileStream, specifier, buffer, comment, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, std::string specifier, long value, std::string comment, terminalSequence* statusSequence, int radix) {

	setHeaderValue(fileStream, specifier.c_str(), value, comment.c_str(), statusSequence, radix);

}

void fileio::setHeaderValue(std::ofstream& fileStream, char* specifier, unsigned long value, char* comment, terminalSequence* statusSequence, int radix) {

	char buffer[longConversionDigits+1];

	ultos(value, buffer, radix, longConversionDigits);
	setHeaderValue(fileStream, specifier, buffer, comment, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, const char* specifier, unsigned long value, const char* comment, terminalSequence* statusSequence, int radix) {

	char buffer[longConversionDigits+1];

	ultos(value, buffer, radix, longConversionDigits);
	setHeaderValue(fileStream, specifier, buffer, comment, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, const char* specifier, unsigned long value, char* comment, terminalSequence* statusSequence, int radix) {

	char buffer[longConversionDigits+1];

	ultos(value, buffer, radix, longConversionDigits);
	setHeaderValue(fileStream, specifier, buffer, comment, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, std::string specifier, unsigned long value, std::string comment, terminalSequence* statusSequence, int radix) {

	setHeaderValue(fileStream, specifier.c_str(), value, comment.c_str(), statusSequence, radix);

}

void fileio::setHeaderValue(std::ofstream& fileStream, char* specifier, float value, char* comment, terminalSequence* statusSequence) {

	double temp;

	temp = (double) value;
	setHeaderValue(fileStream, specifier, temp, comment, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, const char* specifier, float value, char* comment, terminalSequence* statusSequence) {

	double temp;

	temp = (double) value;
	setHeaderValue(fileStream, specifier, temp, comment, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, const char* specifier, float value, const char* comment, terminalSequence* statusSequence) {

	double temp;

	temp = (double) value;
	setHeaderValue(fileStream, specifier, temp, comment, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, std::string specifier, float value, std::string comment, terminalSequence* statusSequence) {

	setHeaderValue(fileStream, specifier.c_str(), value, comment.c_str(), statusSequence);

}

void fileio::setHeaderValue(std::ofstream& fileStream, char* specifier, double value, char* comment, terminalSequence* statusSequence) {

	char buffer[doubleConversion+1];

	dtos(value, buffer, doubleConversionDigits);
	setHeaderValue(fileStream, specifier, buffer, comment, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, const char* specifier, double value, const char* comment, terminalSequence* statusSequence) {

	char buffer[doubleConversion+1];

	dtos(value, buffer, doubleConversionDigits);
	setHeaderValue(fileStream, specifier, buffer, comment, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, const char* specifier, double value, char* comment, terminalSequence* statusSequence) {

	char buffer[doubleConversion+1];

	dtos(value, buffer, doubleConversionDigits);
	setHeaderValue(fileStream, specifier, buffer, comment, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, std::string specifier, double value, std::string comment, terminalSequence* statusSequence) {

	setHeaderValue(fileStream, specifier.c_str(), value, comment.c_str(), statusSequence);

}

void fileio::setHeaderValue(std::ofstream& fileStream, char* specifier, std::string& value, char* comment, terminalSequence* statusSequence) {

	char* temp;

	temp = (char*) value.c_str();
	setHeaderValue(fileStream, specifier, temp, comment, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, const char* specifier, std::string& value, char* comment, terminalSequence* statusSequence) {

	char* temp;

	temp = (char*) value.c_str();
	setHeaderValue(fileStream, specifier, temp, comment, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, const char* specifier, std::string& value, const char* comment, terminalSequence* statusSequence) {

	char* temp;

	temp = (char*) value.c_str();
	setHeaderValue(fileStream, specifier, temp, comment, statusSequence);

}
void fileio::setHeaderValue(std::ofstream& fileStream, std::string specifier, std::string& value, std::string comment, terminalSequence* statusSequence) {

	setHeaderValue(fileStream, specifier.c_str(), value, comment.c_str(), statusSequence);

}

/****************************************************************
 * This methods writes a comment to an ofstream.				*
 ****************************************************************/

void fileio::writeComment(std::ofstream& fileStream, char* comment) {

	unsigned short index;

	fileStream << fileCommentSeparator << ' ';

	index = 0;
	while((comment[index] != '\0') && (index < fileCommentLength)) {
		
		fileStream << legalizedCommentChar(comment[index]);
		index++;

	}
	fileStream << std::endl;

}
void fileio::writeComment(std::ofstream& fileStream, const char* comment) {

	unsigned short index;

	fileStream << fileCommentSeparator << ' ';

	index = 0;
	while((comment[index] != '\0') && (index < fileCommentLength)) {
		
		fileStream << legalizedCommentChar(comment[index]);
		index++;

	}
	fileStream << std::endl;

}
void fileio::writeComment(std::ofstream& fileStream, std::string comment) {

	writeComment(fileStream, comment.c_str());

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

fileio::fileio() : fileName(), commandID(NULL) {

  //	commandID = NULL;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

fileio::fileio(std::string name) : fileName(), commandID(NULL) {

  //	commandID = NULL;

	init(name);

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

fileio::fileio(char* name) : fileName(), commandID(NULL) {

  //	commandID = NULL;

	init(name);

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

fileio::~fileio() {

	deleteMemory();

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void fileio::init(std::string name) {

	setFileName(name);

}
void fileio::init(char* name) {

	setFileName(name);

}

/****************************************************************
 * This method returns if the lock in the commandID is set.		*
 ****************************************************************/

bool fileio::isHeaderLockSet(unsigned int lockId) {

	bool returnValue = false;

	if (lockId < getNumberOfCmds())
		returnValue = commandID[lockId];
	else {

		lockOutOfRangeWarningMsg* lockOutOfRange = new lockOutOfRangeWarningMsg(lockId, getNumberOfCmds());
		lockOutOfRange->warningMsg();
		if(lockOutOfRange != NULL) {
			delete lockOutOfRange;
			lockOutOfRange = NULL;
		}

	}

	return returnValue;

}

/****************************************************************
 * This method sets the lock in the commandID.					*
 ****************************************************************/

void fileio::setHeaderLock(unsigned int lockId, bool value) {

	if (lockId < getNumberOfCmds())
		commandID[lockId] = value;
	else {

		lockOutOfRangeWarningMsg* lockOutOfRange = new lockOutOfRangeWarningMsg(lockId, getNumberOfCmds());
		lockOutOfRange->warningMsg();
		if(lockOutOfRange != NULL) {
			delete lockOutOfRange;
			lockOutOfRange = NULL;
		}

	}

}

/****************************************************************
 * This method reopens all commandID locks.						*
 ****************************************************************/

void fileio::resetAllHeaderLocks() {

	allocateMemory();

	for (unsigned int i = 0; i < getNumberOfCmds(); i++)
		setHeaderLock(i, false);

}

/****************************************************************
 * This method resets the header to the defualt values			*
 * and reopens the commandID lock.								*
 ****************************************************************/

void fileio::resetHeader() {

	resetAllHeaderLocks();

	setHeaderDefValues();

}

/****************************************************************
 * This method returns the name of the file.					*
 ****************************************************************/

std::string fileio::getFileName() {

	return fileName;

}

/****************************************************************
 * This method sets the name of the file.						*
 ****************************************************************/

void fileio::setFileName(std::string name) {

	fileName.clear();

#if (ARCHITECTURE != PS3)

	if ((name.find("../") == 0) || (name.find("./") == 0)) {

		if (getenv("VMCWORKDIR") != NULL) {

			fileName  = getenv("VMCWORKDIR");
			fileName += "/";

		}

	}

#endif

	fileName += name;

}

void fileio::setFileName(char* name) {

	std::string tempName = name;

	setFileName(tempName);

}
