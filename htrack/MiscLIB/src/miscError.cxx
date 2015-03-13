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
//     - derived from errorHandling
//     - class for errors occuring while doing some misc operations
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:36:56 $
// $Revision: 1.2 $
//
// *******************************************************************/


#include "../include/miscError.h"


/****************************************************************
 * CLASS miscError								 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

miscError::miscError() : errorHandling(MISCLIB) {

}

/****************************************************************
 * Default destructor											*
 ****************************************************************/

miscError::~miscError() {

}


/****************************************************************
 * CLASS ustocValueTooBigError							 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

ustocValueTooBigError::ustocValueTooBigError() : miscError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

ustocValueTooBigError::~ustocValueTooBigError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void ustocValueTooBigError::errorMsg() {

	printMsg("The value given to the unsigned-short-to-char function is too big!!!");

}


/****************************************************************
 * CLASS ctousValueTooBigError							 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

ctousValueTooBigError::ctousValueTooBigError() : miscError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

ctousValueTooBigError::~ctousValueTooBigError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void ctousValueTooBigError::errorMsg() {

	printMsg("The value given to the char-to-unsigned-short function is too big!!!");

}
