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
#include "../include/filterBasicSimple.h"


/****************************************************************
 * Constructor													*
 ****************************************************************/

filterBasicSimple::filterBasicSimple() : filterBasicStyle() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

filterBasicSimple::~filterBasicSimple() {

}

/****************************************************************
 * This method implements the filter without respect to the		*
 * axis.														*
 ****************************************************************/

bitArray filterBasicSimple::filter( bitArray* array, unsigned int length,
								  	     unsigned int small, unsigned int element) {

	bitArray returnValue;

	if (array == NULL)
		throw cannotAccessElementsToFilterError();

	if (length <= element)
		throw middleElementIsOutOfRangeError();

	if (length < 2)
		returnValue = array[0];
	else {
		returnValue = array[element];
		for (unsigned int i = 0; i < length; i++) {
			if ((array[i] >= array[element]) && (i != element))
				returnValue = bitArray(0);
		}
	}

	return returnValue;

}
