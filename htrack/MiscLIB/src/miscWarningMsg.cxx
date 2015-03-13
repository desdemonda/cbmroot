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
//     - base class for warnings occuring during misc operations
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:36:56 $
// $Revision: 1.2 $
//
// *******************************************************************/


#include "../../MiscLIB/include/projects.h"
#include "../include/miscWarningMsg.h"
#include "../include/conversionRoutines.h"
#include <iostream>


/****************************************************************
 * CLASS miscWarningMsg							 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

miscWarningMsg::miscWarningMsg() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

miscWarningMsg::~miscWarningMsg() {

}

/****************************************************************
 * This method prints a message.				 				*
 ****************************************************************/

void miscWarningMsg::printMsg(std::string message) {

#ifndef NOMISCWARNINGMESSAGE

	std::cerr << std::endl;
	
	std::cerr << MISCLIB << " WARNING: ";
	
	if (!message.empty())
		std::cerr << message;
	
	std::cerr << std::endl;

#endif

}

/****************************************************************
 * CLASS valueBiggerThanRadixWarningMsg			 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

valueBiggerThanRadixWarningMsg::valueBiggerThanRadixWarningMsg() : miscWarningMsg(), value(0), radix(0) {

  //	value = 0;
  //	radix = 0;

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

valueBiggerThanRadixWarningMsg::valueBiggerThanRadixWarningMsg(long _value, int _radix) : miscWarningMsg(), value(_value), radix(_radix) {

  //	this->value = _value;
  //	this->radix = _radix;

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

valueBiggerThanRadixWarningMsg::~valueBiggerThanRadixWarningMsg() {

}

/****************************************************************
 * This method displays a warning message.		 				*
 ****************************************************************/

void valueBiggerThanRadixWarningMsg::warningMsg() {

	std::string temp;
	char        longBuffer[longConversionDigits+1];
	char        intBuffer[intConversionDigits+1];

	temp = "The value '";
	ltos(value, longBuffer, 10, longConversionDigits);
	temp += longBuffer;
	temp += "' in the string-to-number-conversion is bigger than the radix ";
	itos(radix, intBuffer, 10, intConversionDigits);
	temp += intBuffer;
	temp += "'!!!";
	printMsg(temp);

}
