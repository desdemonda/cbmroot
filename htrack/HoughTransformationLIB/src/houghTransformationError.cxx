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
//     - base class for errors occuring during HoughTransform
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:35:54 $
// $Revision: 1.3 $
//
// *******************************************************************/


#include "../../MiscLIB/include/projects.h"
#include "../include/houghTransformationError.h"


/****************************************************************
 * CLASS houghTransformationError				 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

houghTransformationError::houghTransformationError() : errorHandling(HOUGHTRANSFORMATIONLIB) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

houghTransformationError::~houghTransformationError() {

}


/****************************************************************
 * CLASS noGoodTrackFoundError							 		*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

noGoodTrackFoundError::noGoodTrackFoundError() : houghTransformationError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

noGoodTrackFoundError::~noGoodTrackFoundError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void noGoodTrackFoundError::errorMsg() {

	printMsg("No good track can be found in the data set!!!");

}
