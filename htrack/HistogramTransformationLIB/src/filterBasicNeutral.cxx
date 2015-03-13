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
//     - class for evaluating the basic filter method for
//		 maxMorphSearch
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:35:34 $
// $Revision: 1.2 $
//
// *******************************************************************/


#include "../include/histogramTransformationError.h"
#include "../include/filterBasicNeutral.h"


/****************************************************************
 * Constructor													*
 ****************************************************************/

filterBasicNeutral::filterBasicNeutral() : filterBasicStyle() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

filterBasicNeutral::~filterBasicNeutral() {

}

/****************************************************************
 * This method implements the filter without respect to the		*
 * axis.														*
 ****************************************************************/

bitArray filterBasicNeutral::filter( bitArray* array, unsigned int length,
								  	      unsigned int small, unsigned int element) {

	if (array == NULL)
		throw cannotAccessElementsToFilterError();

	if (length <= element)
		throw middleElementIsOutOfRangeError();

	return array[element];

}
