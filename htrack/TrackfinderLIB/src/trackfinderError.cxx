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
//     - base class for errors occuring during trackfinding
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:39:20 $
// $Revision: 1.2 $
//
// *******************************************************************/


#include "../include/trackfinderError.h"


/****************************************************************
 * CLASS trackfinderError						 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackfinderError::trackfinderError() : errorHandling(HISTOGRAMTRANSFORMATIONLIB) {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackfinderError::~trackfinderError() {

}


/****************************************************************
 * CLASS cannotAccessHoughMemManagerError		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessHoughMemManagerError::cannotAccessHoughMemManagerError() : trackfinderError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessHoughMemManagerError::~cannotAccessHoughMemManagerError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessHoughMemManagerError::errorMsg() {

	printMsg("The houghMemManager is not accessible!!!");

}


/****************************************************************
 * CLASS cannotAccessHistogramMemManagerError	 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessHistogramMemManagerError::cannotAccessHistogramMemManagerError() : trackfinderError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessHistogramMemManagerError::~cannotAccessHistogramMemManagerError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessHistogramMemManagerError::errorMsg() {

	printMsg("The histogramMemManager is not accessible!!!");

}


/****************************************************************
 * CLASS cannotAccessHoughTransformError		 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessHoughTransformError::cannotAccessHoughTransformError() : trackfinderError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessHoughTransformError::~cannotAccessHoughTransformError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessHoughTransformError::errorMsg() {

	printMsg("The houghTransform is not accessible!!!");

}


/****************************************************************
 * CLASS cannotAccessHistogramTransformError	 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessHistogramTransformError::cannotAccessHistogramTransformError() : trackfinderError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessHistogramTransformError::~cannotAccessHistogramTransformError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessHistogramTransformError::errorMsg() {

	printMsg("The histogramTransform is not accessible!!!");

}


/****************************************************************
 * CLASS cannotAccessAnalyserError	 							*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

cannotAccessAnalyserError::cannotAccessAnalyserError() : trackfinderError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

cannotAccessAnalyserError::~cannotAccessAnalyserError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void cannotAccessAnalyserError::errorMsg() {

	printMsg("The analyser is not accessible!!!");

}


/****************************************************************
 * CLASS impossibleFeatureWithoutAnalyserError					*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

impossibleFeatureWithoutAnalyserError::impossibleFeatureWithoutAnalyserError() : trackfinderError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

impossibleFeatureWithoutAnalyserError::~impossibleFeatureWithoutAnalyserError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void impossibleFeatureWithoutAnalyserError::errorMsg() {

	printMsg("It is impossible to use this feature without the analyser object!!!");

}


/****************************************************************
 * CLASS featureNotEnabledInAnalyserError						*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

featureNotEnabledInAnalyserError::featureNotEnabledInAnalyserError() : trackfinderError() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

featureNotEnabledInAnalyserError::~featureNotEnabledInAnalyserError() {

}

/****************************************************************
 * This method displays an error message.		 				*
 ****************************************************************/

void featureNotEnabledInAnalyserError::errorMsg() {

	printMsg("This feature can not be used if it is not enabled in the analyser object!!!");

}
