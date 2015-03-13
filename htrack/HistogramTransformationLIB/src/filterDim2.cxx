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
#include "../include/filterDim2.h"
#include <stdio.h>

#ifdef __APPLE__
#include <malloc/malloc.h>
#else
#include <malloc.h>
#endif

/****************************************************************
 * Default constructor											*
 ****************************************************************/

filterDim2::filterDim2() : filterDimX() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

filterDim2::filterDim2( histogramData** _histogram,
						unsigned short  filterArithmetic,
						unsigned short  size,
						unsigned short  localSize,
						bitArray maximumClass) : filterDimX(
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

filterDim2::~filterDim2() {

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void filterDim2::init( histogramData** _histogram,
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
 * This method implements the filter in the Dim2-axe.			*
 ****************************************************************/

void filterDim2::filter() {

	unsigned short valueDim1;
	unsigned short valueDim2;
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

	for (unsigned short i = 0; i < valueDim1; i++) {
		for (k = 0; k < filterSize; k++) {
			filterMem[k] = bitArray(0);
		}
		for (unsigned short j = 0; j < valueDim2 + filterSize / 2; j++) {
			for (k = 1; k < filterSize; k++) {
				filterMem[k-1] = filterMem[k];
			}
			if (j < valueDim2) {
				filterMem[filterSize - 1] = (*histogram)->getCell(i, j)->value;
			}
			else {
				filterMem[filterSize - 1] = bitArray(0);
			}
			/* normal mode */
			if (j >= filterSize / 2) {
				(*histogram)->getCell(i, j - filterSize / 2)->value = baseFilter->filter(filterMem, filterSize, filterLocalSize, filterSize / 2);
			}
		}
	}

}
