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
#include "../include/filterBasicComplex.h"


#define min(a, b)  (((a) < (b)) ? (a) : (b)) 


/****************************************************************
 * Default constructor											*
 ****************************************************************/

filterBasicComplex::filterBasicComplex() : filterBasicStyle() {

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

filterBasicComplex::~filterBasicComplex() {

}

/****************************************************************
 * This method implements the filter without respect to the		*
 * axis.														*
 ****************************************************************/

bitArray filterBasicComplex::filter( bitArray* array, unsigned int length,
										  unsigned int small, unsigned int element) {

	bitArray     globalMaximum;
	bool         localMaximum;
	unsigned int localStartSearch;
	unsigned int localStopSearch;
	int          preCounter;
	int          postCounter;
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
			preCounter = 0;						/* compute number of equal values to the element in front of the element */
			for (i = 0; i < element; i++) {
				if (array[i] == array[element])
					preCounter++;
			}
			postCounter = 0;					/* compute number of equal values to the element behind of the element */	
			for (i = element + 1; i < length; i++) {
				if (array[i] == array[element])
					postCounter++;
			}
			localMaximum     = true;			/* evaluate the local maxima criteria */
			if (small / 2 > element)
				localStartSearch = 0;
			else
				localStartSearch = element - small / 2;
			localStopSearch  = min(element + 1 + small / 2, length);
			for (i = localStartSearch; i < localStopSearch; i++) {
				if ((i != element) && (array[i] == array[element]))
					localMaximum = false;
			}
			/* if the index is in the middle of the equal values or the local maxima criteria is fullfilled, take the value,
			 * else delete it */
//			if (((preCounter == postCounter) && (preCounter + postCounter != length - 1)) || (preCounter == postCounter+1) || (localMaximum == true))
			if ((preCounter == postCounter) || (preCounter == postCounter+1) || (localMaximum == true))
				returnValue = array[element];
			else
				returnValue = bitArray(0);
		}
	}

	return returnValue;

}
