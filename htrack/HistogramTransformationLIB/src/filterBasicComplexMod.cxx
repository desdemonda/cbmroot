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
#include "../include/filterBasicComplexMod.h"


#define min(a, b)  (((a) < (b)) ? (a) : (b)) 


/****************************************************************
 * Constructor													*
 ****************************************************************/

filterBasicComplexMod::filterBasicComplexMod() : filterBasicStyle() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

filterBasicComplexMod::~filterBasicComplexMod() {

}

/****************************************************************
 * This method implements the filter without respect to the		*
 * axis.														*
 ****************************************************************/

bitArray filterBasicComplexMod::filter( bitArray* array, unsigned int length,
											 unsigned int small, unsigned int element) {

	bitArray     globalMaximum;
	bool         localMaximum;
	unsigned int localStartSearch;
	unsigned int localStopSearch;
	unsigned int i;
	bitArray     returnValue;

	if (array == NULL)
		throw cannotAccessElementsToFilterError();

	if (length <= element)
		throw middleElementIsOutOfRangeError();

	if (length < 2)
		returnValue = array[0];
	else {
		globalMaximum  = array[0];
		for (i = 1; i < length; i++) {
			if (array[i] > globalMaximum)
				globalMaximum = array[i];
		}
	
		if (globalMaximum != array[element])	/* element is not global maximum */
			returnValue = bitArray(0);
		else {
			localMaximum = true;
			if (small / 2 > element)
				localStartSearch = 0;
			else
				localStartSearch = element - small / 2;
			localStopSearch  = min(element + 1 + small / 2, length);
			for (i = localStartSearch; i < localStopSearch; i++) {
				if ((i != element) && (array[i] == globalMaximum))
					localMaximum = false;
			}
			if (localMaximum)					/* element is local maximum */
				returnValue = array[element];
			else
				returnValue = bitArray(0);
		}
	}

	return returnValue;

}
