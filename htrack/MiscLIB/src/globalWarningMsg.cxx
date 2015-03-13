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
//     - base class for warnings occuring during access of some globals
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-09-11 14:12:40 $
// $Revision: 1.5 $
//
// *******************************************************************/


#include "../include/conversionRoutines.h"
#include "../include/globalWarningMsg.h"
#include <iostream>


/****************************************************************
 * CLASS globalWarningMsg						 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

globalWarningMsg::globalWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

globalWarningMsg::~globalWarningMsg() {

}

/****************************************************************
 * This method prints a message.				 				*
 ****************************************************************/

void globalWarningMsg::printMsg(std::string source, std::string message) {

#ifndef NOGLOBALWARNINGMESSAGE

	std::cerr << std::endl;

	if (!source.empty())
		std::cerr << source << " ";

	std::cerr << "WARNING: ";

	if (!message.empty())
		std::cerr << message;
	
	std::cerr << std::endl;

#endif

}


/****************************************************************
 * CLASS gStyleNotFoundWarningMsg				 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

gStyleNotFoundWarningMsg::gStyleNotFoundWarningMsg() : globalWarningMsg(), warningSource() {

  //	warningSource.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

gStyleNotFoundWarningMsg::gStyleNotFoundWarningMsg(std::string actualWarningSource) : globalWarningMsg(), warningSource(actualWarningSource) {

  //	warningSource = actualWarningSource;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

gStyleNotFoundWarningMsg::~gStyleNotFoundWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void gStyleNotFoundWarningMsg::warningMsg() {

	printMsg(warningSource, "The object for the global style cannot be found!!!");

}


/****************************************************************
 * CLASS gDirectoryNotFoundWarningMsg			 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

gDirectoryNotFoundWarningMsg::gDirectoryNotFoundWarningMsg() : globalWarningMsg(), warningSource() {

  //	warningSource.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

gDirectoryNotFoundWarningMsg::gDirectoryNotFoundWarningMsg(std::string actualWarningSource) : globalWarningMsg(), warningSource(actualWarningSource) {

  //	warningSource = actualWarningSource;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

gDirectoryNotFoundWarningMsg::~gDirectoryNotFoundWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void gDirectoryNotFoundWarningMsg::warningMsg() {

	printMsg(warningSource, "The object for the global directory cannot be found!!!");

}


/****************************************************************
 * CLASS geoManagerNotFoundWarningMsg			 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

geoManagerNotFoundWarningMsg::geoManagerNotFoundWarningMsg() : globalWarningMsg(), warningSource() {

  //	warningSource.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

geoManagerNotFoundWarningMsg::geoManagerNotFoundWarningMsg(std::string actualWarningSource) : globalWarningMsg(), warningSource(actualWarningSource) {

  //	warningSource = actualWarningSource;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

geoManagerNotFoundWarningMsg::~geoManagerNotFoundWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void geoManagerNotFoundWarningMsg::warningMsg() {

	printMsg(warningSource, "The geoManager to read the station geometry is not found!!!");

}


/****************************************************************
 * CLASS houghLutCorrectionWarningMsg			 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

houghLutCorrectionWarningMsg::houghLutCorrectionWarningMsg() : globalWarningMsg(), warningSource(), borderCorrections(0), borderPosCorrections(0) {

  //	warningSource.clear();
  //	borderCorrections    = 0;
  //	borderPosCorrections = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

houghLutCorrectionWarningMsg::houghLutCorrectionWarningMsg(std::string actualWarningSource, unsigned long corrections, unsigned long posCorrections) : globalWarningMsg(), warningSource(actualWarningSource), borderCorrections(corrections), borderPosCorrections(posCorrections) {

  //	warningSource        = actualWarningSource;
  //	borderCorrections    = corrections;
  //	borderPosCorrections = posCorrections;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

houghLutCorrectionWarningMsg::~houghLutCorrectionWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void houghLutCorrectionWarningMsg::warningMsg() {

	std::string temp;
	char        buffer[longConversionDigits+1];
	double      mean;

	temp = "There are ";
	ultos(borderPosCorrections, buffer, 10, longConversionDigits);
	temp += buffer;
	temp += " corrections which are done while lut calculation";
	if (borderCorrections > 0) {
		temp += ". ";
		ultos(borderCorrections, buffer, 10, longConversionDigits);
		temp += buffer;
		temp += " different transformations of a hit are affected. So in the middle there are ";
		mean = borderPosCorrections / borderCorrections;
		ultos((unsigned long)(mean + 0.5), buffer, 10, longConversionDigits);
		temp += buffer;
		temp += " positions corrected per transformation of a hit";
	}
	temp += "!!!";
	printMsg(warningSource, temp);

}


/****************************************************************
 * CLASS cannotDrawWarningMsg					 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotDrawWarningMsg::cannotDrawWarningMsg() : globalWarningMsg(), warningSource() {

  //	warningSource.clear();

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

cannotDrawWarningMsg::cannotDrawWarningMsg(std::string actualWarningSource) : globalWarningMsg(), warningSource(actualWarningSource) {

  //	warningSource = actualWarningSource;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotDrawWarningMsg::~cannotDrawWarningMsg() {

}

/****************************************************************
 * This method displayDirectories a warning message.	 		*
 ****************************************************************/

void cannotDrawWarningMsg::warningMsg() {

	printMsg(warningSource, "The display can not be drawn!!!");

}
