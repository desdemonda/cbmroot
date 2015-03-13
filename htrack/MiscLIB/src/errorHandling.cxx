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
//     - base class for errors
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-09-11 14:12:40 $
// $Revision: 1.9 $
//
// *******************************************************************/


#include "../include/errorHandling.h"
#include <iostream>


/****************************************************************
 * CLASS errorHandling							 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

errorHandling::errorHandling() : errorSource() {

  //	errorSource.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

errorHandling::errorHandling(std::string _errorSource) : errorSource(_errorSource) {

  //	this->errorSource = _errorSource;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

errorHandling::~errorHandling() {

}

/****************************************************************
 * This method prints a message with a leading source specifier.*
 ****************************************************************/

void errorHandling::printMsg(std::string message) {

#ifndef NOERRORMESSAGE 
	
	std::cerr << std::endl;

	if (!errorSource.empty())
		std::cerr << errorSource << " ";
	
	std::cerr << "ERROR: ";
	
	if (!message.empty())
		std::cerr << message;
	
	std::cerr << std::endl;

#endif

}

/****************************************************************
 * CLASS cannotOpenFileError							 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotOpenFileError::cannotOpenFileError() : errorHandling(), fileName()  {

  //	this->fileName.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

cannotOpenFileError::cannotOpenFileError(std::string _errorSource, std::string _fileName) : errorHandling(_errorSource), fileName(_fileName) {

  //	this->fileName = _fileName;

}
cannotOpenFileError::cannotOpenFileError(std::string _errorSource, const char* _fileName) : errorHandling(_errorSource), fileName(_fileName) {

  //	this->fileName = _fileName;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotOpenFileError::~cannotOpenFileError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotOpenFileError::errorMsg() {

	std::string temp;

	if (fileName.empty())
		temp = "The file cannot be opened!!!";
	else {

		temp  = "The file '";
		temp += fileName;
		temp += "' cannot be opened!!!";

	}

	printMsg(temp);

}


/****************************************************************
 * CLASS cannotCloseFileError							 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotCloseFileError::cannotCloseFileError() : errorHandling(), fileName() {

  //	this->fileName.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

cannotCloseFileError::cannotCloseFileError(std::string _errorSource, std::string _fileName) : errorHandling(_errorSource), fileName(_fileName) {

  //	this->fileName = _fileName;

}
cannotCloseFileError::cannotCloseFileError(std::string _errorSource, const char* _fileName) : errorHandling(_errorSource), fileName(_fileName) {

  //	this->fileName = _fileName;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotCloseFileError::~cannotCloseFileError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotCloseFileError::errorMsg() {

	std::string temp;

	if (fileName.empty())
		temp = "The file cannot be closed!!!";
	else {

		temp  = "The file '";
		temp += fileName;
		temp += "' cannot be closed!!!";

	}

	printMsg(temp);

}


/****************************************************************
 * CLASS cannotCreateFileError							 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotCreateFileError::cannotCreateFileError() : errorHandling(), fileName() {

  //	this->fileName.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

cannotCreateFileError::cannotCreateFileError(std::string _errorSource, std::string _fileName) : errorHandling(_errorSource), fileName(_fileName) {

  //	this->fileName = _fileName;

}
cannotCreateFileError::cannotCreateFileError(std::string _errorSource, const char* _fileName) : errorHandling(_errorSource), fileName(_fileName) {

  //	this->fileName = _fileName;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotCreateFileError::~cannotCreateFileError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotCreateFileError::errorMsg() {

	std::string temp;

	if (fileName.empty())
		temp = "The file cannot be created!!!";
	else {

		temp  = "The file '";
		temp += fileName;
		temp += "' cannot be created!!!";

	}

	printMsg(temp);

}


/****************************************************************
 * CLASS unexpectedEofFoundError						 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

unexpectedEofFoundError::unexpectedEofFoundError() : errorHandling(), fileName() {

  //	this->fileName.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

unexpectedEofFoundError::unexpectedEofFoundError(std::string _errorSource, std::string _fileName) : errorHandling(_errorSource), fileName(_fileName) {

  //	this->fileName = _fileName;

}
unexpectedEofFoundError::unexpectedEofFoundError(std::string _errorSource, const char* _fileName) : errorHandling(_errorSource), fileName(_fileName) {

  // this->fileName = _fileName;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

unexpectedEofFoundError::~unexpectedEofFoundError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void unexpectedEofFoundError::errorMsg() {

	std::string temp;

	if (fileName.empty())
		temp = "The EOF marker is found but not expected!!!";
	else {

		temp  = "The EOF marker in the file '";
		temp += fileName;
		temp += "' is found but not expected!!!";

	}

	printMsg(temp);

}


/****************************************************************
 * CLASS noFilenameSpecifiedError						 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

noFilenameSpecifiedError::noFilenameSpecifiedError() : errorHandling() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

noFilenameSpecifiedError::noFilenameSpecifiedError(std::string _errorSource) : errorHandling(_errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

noFilenameSpecifiedError::~noFilenameSpecifiedError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void noFilenameSpecifiedError::errorMsg() {

	printMsg("No filename is specified!!!");

}


/****************************************************************
 * CLASS memoryAllocationError							 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

memoryAllocationError::memoryAllocationError() : errorHandling() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

memoryAllocationError::memoryAllocationError(std::string _errorSource) : errorHandling(_errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

memoryAllocationError::~memoryAllocationError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void memoryAllocationError::errorMsg() {

	printMsg("Some memory allocation fails!!!");

}


/****************************************************************
 * CLASS cannotAccessHitsOrTracksError							*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessHitsOrTracksError::cannotAccessHitsOrTracksError() : errorHandling() {

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessHitsOrTracksError::cannotAccessHitsOrTracksError(std::string _errorSource) : errorHandling(_errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessHitsOrTracksError::~cannotAccessHitsOrTracksError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessHitsOrTracksError::errorMsg() {

	printMsg("The hits or the tracks are not accessible!!!");

}


/****************************************************************
 * CLASS cannotAccessEventDataError								*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessEventDataError::cannotAccessEventDataError() : errorHandling() {

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessEventDataError::cannotAccessEventDataError(std::string _errorSource) : errorHandling(_errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessEventDataError::~cannotAccessEventDataError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessEventDataError::errorMsg() {

	printMsg("The sourcedata of the event is not accessible!!!");

}


/****************************************************************
 * CLASS cannotAccessHistogramDataError							*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessHistogramDataError::cannotAccessHistogramDataError() : errorHandling() {

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessHistogramDataError::cannotAccessHistogramDataError(std::string _errorSource) : errorHandling(_errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessHistogramDataError::~cannotAccessHistogramDataError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessHistogramDataError::errorMsg() {

	printMsg("The sourcedata of the histogram is not accessible!!!");

}


/****************************************************************
 * CLASS cannotAccessTrackDataError								*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessTrackDataError::cannotAccessTrackDataError() : errorHandling() {

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessTrackDataError::cannotAccessTrackDataError(std::string _errorSource) : errorHandling(_errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessTrackDataError::~cannotAccessTrackDataError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessTrackDataError::errorMsg() {

	printMsg("The data of the evaluated tracks is not accessible!!!");

}


/****************************************************************
 * CLASS cannotAccessRootManagerError							*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessRootManagerError::cannotAccessRootManagerError() : errorHandling() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

cannotAccessRootManagerError::cannotAccessRootManagerError(std::string _errorSource) : errorHandling(_errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessRootManagerError::~cannotAccessRootManagerError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessRootManagerError::errorMsg() {

	printMsg("The root-file-manager is not accessible!!!");

}


/****************************************************************
 * CLASS noDetectorError										*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

noDetectorError::noDetectorError() : errorHandling() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

noDetectorError::noDetectorError(std::string _errorSource) : errorHandling(_errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

noDetectorError::~noDetectorError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void noDetectorError::errorMsg() {

	printMsg("There is no detector, which can be accessed!!!");

}


/****************************************************************
 * CLASS outOfRangeError										*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

outOfRangeError::outOfRangeError() : errorHandling() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

outOfRangeError::outOfRangeError(std::string _errorSource) : errorHandling(_errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

outOfRangeError::~outOfRangeError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void outOfRangeError::errorMsg() {

	printMsg("The index is out of range!!!");

}


/****************************************************************
 * CLASS zeroDivisionError										*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

zeroDivisionError::zeroDivisionError() : errorHandling() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

zeroDivisionError::zeroDivisionError(std::string _errorSource) : errorHandling(_errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

zeroDivisionError::~zeroDivisionError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void zeroDivisionError::errorMsg() {

	printMsg("The division through zero is not possible!!!");

}


/****************************************************************
 * CLASS zeroModuloError										*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

zeroModuloError::zeroModuloError() : errorHandling() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

zeroModuloError::zeroModuloError(std::string _errorSource) : errorHandling(_errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

zeroModuloError::~zeroModuloError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void zeroModuloError::errorMsg() {

	printMsg("The modulo with zero is not possible!!!");

}


/****************************************************************
 * CLASS cannotAccessStationError								*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessStationError::cannotAccessStationError() : errorHandling() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

cannotAccessStationError::cannotAccessStationError(std::string _errorSource) : errorHandling(_errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessStationError::~cannotAccessStationError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessStationError::errorMsg() {

	printMsg("The station of this hit cannot be accessed!!!");

}


/****************************************************************
 * CLASS cannotReadEmptyMemoryError				 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotReadEmptyMemoryError::cannotReadEmptyMemoryError() : errorHandling() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

cannotReadEmptyMemoryError::cannotReadEmptyMemoryError(std::string _errorSource) : errorHandling(_errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotReadEmptyMemoryError::~cannotReadEmptyMemoryError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotReadEmptyMemoryError::errorMsg() {

	printMsg("You cannot read something out of an empty memory!!!");

}


/****************************************************************
 * CLASS cannotAccessHistogramSpaceError		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessHistogramSpaceError::cannotAccessHistogramSpaceError() : errorHandling() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

cannotAccessHistogramSpaceError::cannotAccessHistogramSpaceError(std::string _errorSource) : errorHandling(_errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessHistogramSpaceError::~cannotAccessHistogramSpaceError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessHistogramSpaceError::errorMsg() {

	printMsg("The histogram space object is not accessible!!!");

}


/****************************************************************
 * CLASS cannotAccessMagneticFieldObjectError	 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessMagneticFieldObjectError::cannotAccessMagneticFieldObjectError() : errorHandling() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

cannotAccessMagneticFieldObjectError::cannotAccessMagneticFieldObjectError(std::string _errorSource) : errorHandling(_errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessMagneticFieldObjectError::~cannotAccessMagneticFieldObjectError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessMagneticFieldObjectError::errorMsg() {

	printMsg("The magnetic field object is not accessible!!!");

}


/****************************************************************
 * CLASS cannotAccessTablesError								*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessTablesError::cannotAccessTablesError() : errorHandling() {

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessTablesError::cannotAccessTablesError(std::string _errorSource) : errorHandling(_errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessTablesError::~cannotAccessTablesError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessTablesError::errorMsg() {

	printMsg("The tabels for the ratings are not accessible!!!");

}


/****************************************************************
 * CLASS functionIsDeprecatedError								*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

functionIsDeprecatedError::functionIsDeprecatedError() : errorHandling() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

functionIsDeprecatedError::functionIsDeprecatedError(std::string _errorSource) : errorHandling(_errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

functionIsDeprecatedError::~functionIsDeprecatedError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void functionIsDeprecatedError::errorMsg() {

	printMsg("The function throwing this error is deprecated and cannot be used due to some object problems!!!");

}


/****************************************************************
 * CLASS cannotAccessLutsError						 			*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessLutsError::cannotAccessLutsError() : errorHandling() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

cannotAccessLutsError::cannotAccessLutsError(std::string _errorSource) : errorHandling(_errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessLutsError::~cannotAccessLutsError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessLutsError::errorMsg() {

	printMsg("The look-up-tables for the Hough transform are not accessible!!!");

}


/****************************************************************
 * CLASS cannotAccessDistributionMemoryError		 			*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessDistributionMemoryError::cannotAccessDistributionMemoryError() : errorHandling() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

cannotAccessDistributionMemoryError::cannotAccessDistributionMemoryError(std::string _errorSource) : errorHandling(_errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessDistributionMemoryError::~cannotAccessDistributionMemoryError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessDistributionMemoryError::errorMsg() {

	printMsg("The memory to store the distribution is not accessible!!!");

}


/****************************************************************
 * CLASS cannotAccessHistogramError			 					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessHistogramError::cannotAccessHistogramError() : errorHandling() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

cannotAccessHistogramError::cannotAccessHistogramError(std::string _errorSource) : errorHandling(_errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessHistogramError::~cannotAccessHistogramError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessHistogramError::errorMsg() {

	printMsg("The histogram is not accessible!!!");

}


/****************************************************************
 * CLASS windowDivisionError							 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

windowDivisionError::windowDivisionError() : errorHandling() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

windowDivisionError::windowDivisionError(std::string _errorSource) : errorHandling(_errorSource) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

windowDivisionError::~windowDivisionError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void windowDivisionError::errorMsg() {

	printMsg("The computing of the window division is wrong!!!");

}
