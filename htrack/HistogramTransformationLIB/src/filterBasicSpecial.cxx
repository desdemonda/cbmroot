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
// $Revision: 1.3 $
//
// *******************************************************************/


#include "../include/filterBasicSpecial.h"
#include "../include/filterBasicSimple.h"
#include "../include/filterBasicSimpleMod.h"
#include <stdio.h>


/****************************************************************
 * Default constructor											*
 ****************************************************************/

filterBasicSpecial::filterBasicSpecial(bitArray maximumClass) 
  : filterBasicStyle(),
    maxClass(maximumClass),
    filterMax(new filterBasicSimpleMod()),
    filterElse(new filterBasicSimple())
{
  /*
	maxClass   = maximumClass;
	filterMax  = new filterBasicSimpleMod();
	filterElse = new filterBasicSimple();
  */
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

filterBasicSpecial::~filterBasicSpecial() {

	if (filterMax != NULL) {
	
		delete filterMax;
		filterMax = NULL;
	}
	if (filterElse != NULL) {
	
		delete filterElse;
		filterElse = NULL;
	}

}

/****************************************************************
 * This method implements the filter without respect to the		*
 * axis.														*
 ****************************************************************/

bitArray filterBasicSpecial::filter( bitArray* array, unsigned int length,
								  	 unsigned int small, unsigned int element) {

	bitArray returnValue;

	if (array[element] >= maxClass)
		returnValue = filterMax->filter(array, length, small, element);
	else
		returnValue = filterElse->filter(array, length, small, element);

	return returnValue;

}
