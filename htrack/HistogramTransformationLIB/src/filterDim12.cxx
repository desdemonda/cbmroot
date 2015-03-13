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
// $Revision: 1.4 $
//
// *******************************************************************/


#include "../../MiscLIB/include/errorHandling.h"
#include "../../MiscLIB/include/terminal.h"
#include "../include/filterDef.h"
#include "../include/histogramTransformationError.h"
#include "../include/filterBasicNeutral.h"
#include "../include/filterBasicSimple.h"
#include "../include/filterBasicSimpleMod.h"
#include "../include/filterBasicComplex.h"
#include "../include/filterBasicComplexMod.h"
#include "../include/filterBasicSpecial.h"
#include "../include/filterDim12.h"
#include <stdio.h>

#ifdef __APPLE__
#include <malloc/malloc.h>
#else
#include <malloc.h>
#endif

#define min(a, b)  (((a) < (b)) ? (a) : (b)) 



/****************************************************************
 * Default constructor											*
 ****************************************************************/

filterDim12::filterDim12() : filterDimX() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

filterDim12::filterDim12( histogramData** _histogram,
						  unsigned short  filterArithmetic,
						  unsigned short  size,
						  unsigned short  localSize,
						  bitArray maximumClass) :
						  filterDimX(
						  _histogram, size, localSize) {

	switch(filterArithmetic) {

		case FIRSTSIMPLEARITHMETIC:
			baseFilter      = new filterBasicSimple();
			break;

		case FIRSTSIMPLEMODARITHMETIC:
			baseFilter      = new filterBasicSimpleMod();
			break;

		case FIRSTCOMPLEXARITHMETIC:
			baseFilter      = new filterBasicComplex();
			break;

		case FIRSTCOMPLEXMODARITHMETIC:
			baseFilter      = new filterBasicComplexMod();
			break;

		case FIRSTSPECIALARITHMETIC:
			baseFilter      = new filterBasicSpecial(maximumClass);
			break;

		default:
			baseFilter      = new filterBasicNeutral();
			break;

	}

	filterMem       = new bitArray[filterSize];

	if (filterMem == NULL)
		throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

filterDim12::~filterDim12() {

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void filterDim12::init( histogramData** _histogram,
						unsigned short  filterArithmetic,
					    unsigned short   size,
					    unsigned short   localSize,
					    bitArray maximumClass) {
					  
	/* free the old allocated space */
	if (baseFilter != NULL) {
		delete baseFilter;
		baseFilter = NULL;
	}
	if (filterMem != NULL) {
		delete[] filterMem;
		filterMem = NULL;
	}

	/* set new parameter */
	filterDimX::init(_histogram, size, localSize);

	switch(filterArithmetic) {

		case FIRSTSIMPLEARITHMETIC:
			baseFilter      = new filterBasicSimple();
			break;

		case FIRSTSIMPLEMODARITHMETIC:
			baseFilter      = new filterBasicSimpleMod();
			break;

		case FIRSTCOMPLEXARITHMETIC:
			baseFilter      = new filterBasicComplex();
			break;

		case FIRSTCOMPLEXMODARITHMETIC:
			baseFilter      = new filterBasicComplexMod();
			break;

		case FIRSTSPECIALARITHMETIC:
			baseFilter      = new filterBasicSpecial(maximumClass);
			break;

		default:
			baseFilter      = new filterBasicNeutral();
			break;

	}

	filterMem       = new bitArray[filterSize];

	if (filterMem == NULL)
		throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);

}

/****************************************************************
 * This method implements the filter in the Dim12-axe.			*
 ****************************************************************/

void filterDim12::filter() {

	unsigned short valueDim1;
	unsigned short valueDim2;
	unsigned short i;
	unsigned short j;
	unsigned short k;

	if (filterMem == NULL)
		throw cannotAccessFilterMemoryError();

	if (histogram == NULL)
		throw cannotAccessHistogramError(HISTOGRAMTRANSFORMATIONLIB);
	if (*histogram == NULL)
		throw cannotAccessHistogramError(HISTOGRAMTRANSFORMATIONLIB);

	if (baseFilter == NULL)
		throw cannotAccessFilterError();

	valueDim1 = (*histogram)->getValueDim1();
	valueDim2 = (*histogram)->getValueDim2();

	/* loop for one half of the histogram */
	for (i = 0; i < valueDim2; i++) {
		for (j = 0; j < filterSize; j++) {
			filterMem[j] = bitArray(0);
		}
		/* loop to reach the end of the actual skewed-filter-row */
		for (j = 0; j < min(valueDim2 - i, valueDim1) + filterSize/2; j++) {
			for (k = 1; k < filterSize; k++) {
				filterMem[k-1] = filterMem[k];
			}
			if (j < min(valueDim2 - i, valueDim1)) {
				filterMem[filterSize-1] = (*histogram)->getCell(j, i + j)->value;
			}
			else {
				filterMem[filterSize - 1] = bitArray(0);
			}
			/* end mode */
			if (j >= min(valueDim2 - i, valueDim1)) {
				(*histogram)->getCell(j - filterSize / 2, i + j - filterSize / 2)->value = baseFilter->filter(filterMem, filterSize - ( j - min(valueDim2 - i, valueDim1)) - 1, filterLocalSize, filterSize / 2);
			}
			/* normal mode */
			else if (j >= filterSize - 1) {
				(*histogram)->getCell(j - filterSize / 2, i + j - filterSize / 2)->value = baseFilter->filter(filterMem, filterSize, filterLocalSize, filterSize / 2);
			}
			/* start mode */
			else if (j >= filterSize/2) {
				(*histogram)->getCell(j - filterSize / 2, i + j - filterSize / 2)->value = baseFilter->filter(&filterMem[filterSize - 1 - i - j], i + j + 1, filterLocalSize, i + j - filterSize / 2);
			}
		}
	}
	/* loop for the other half of the histogram */
	for (i = 1; i < valueDim1; i++) {
		for (j = 0; j < filterSize; j++) {
			filterMem[j] = bitArray(0);
		}
		for (j = 0; j < min(valueDim1 - i, valueDim2) + filterSize/2; j++) {
			for (k = 1; k < filterSize; k++) {
				filterMem[k-1] = filterMem[k];
			}
			if (j <  min(valueDim1 - i, valueDim2)) {
				filterMem[filterSize-1] = (*histogram)->getCell(i + j, j)->value;
			}
			else {
				filterMem[filterSize-1] = bitArray(0);
			}
			/* end mode */
			if (j >= min(valueDim1 - i, valueDim2)) {
				(*histogram)->getCell(i + j - filterSize / 2, j - filterSize / 2)->value = baseFilter->filter(filterMem, filterSize - (j - min(valueDim1 - i, valueDim2)) - 1, filterLocalSize, filterSize / 2);
			}
			/* normal mode */
			if (j >= filterSize-1) {
				(*histogram)->getCell(i + j - filterSize / 2, j - filterSize / 2)->value = baseFilter->filter(filterMem, filterSize, filterLocalSize, filterSize / 2);
			}
			/* start mode */
			else if (j >= filterSize / 2) {
				(*histogram)->getCell(i + j - filterSize / 2, j - filterSize / 2)->value = baseFilter->filter(&filterMem[filterSize - 1 - j], j + 1, filterLocalSize, j - filterSize / 2);
			}
		}
	}

}
