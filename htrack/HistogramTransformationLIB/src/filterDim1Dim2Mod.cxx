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
#include "../include/filterDim1Dim2Mod.h"
#include <stdio.h>

#ifdef __APPLE__
#include <malloc/malloc.h>
#else
#include <malloc.h>
#endif

/****************************************************************
 * Default constructor											*
 ****************************************************************/

filterDim1Dim2Mod::filterDim1Dim2Mod() 
  : filterDimXDimX(),
    filterTempMem(NULL),
    memValues(NULL),
    allocaterMarker(NULL)
{
  /*
	filterTempMem     = NULL;
	memValues         = NULL;
	allocaterMarker   = NULL;
  */
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

filterDim1Dim2Mod::filterDim1Dim2Mod( histogramData** _histogram,
				      unsigned short  filterArithmetic,
				      unsigned short  size1,
				      unsigned short  size2,
				      unsigned short  localSize1,
				      unsigned short  localSize2,
				      bitArray maximumClass) 
  : filterDimXDimX(_histogram, size1, size2, (size1+1)*(size2/2)+size1, (localSize1+1)*(localSize2/2)+localSize1),
    filterTempMem(NULL),
    memValues(NULL),
    allocaterMarker(NULL)
{

	int          m;
	int          n;
	unsigned int filterTempCounter;

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

	filterTempMem = new bitArray**[filterSize2];

	if (filterTempMem == NULL)
		throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);

	for (m = 0; m < filterSize2; m++) {
		filterTempMem[m] = new bitArray*[filterSize1];

		if (filterTempMem[m] == NULL)
			throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);

		for (n = 0; n < filterSize1; n++) {
			filterTempMem[m][n] = new bitArray[1];

			if (filterTempMem[m][n] == NULL)
				throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);

		}
	}

	allocaterMarker = new bool*[filterSize2];

	if (allocaterMarker == NULL)
		throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);

	for (m = 0; m < filterSize2; m++) {
		allocaterMarker[m] = new bool[filterSize1];

		if (allocaterMarker[m] == NULL)
			throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);

	}

	filterTempCounter = 0;
	for (n = 0; n < filterSize1; n++) {
		for (m = 0; m < filterSize2; m++) {
			if (m < filterSize2/2) {
				if (n <= filterSize1/2) {
					allocaterMarker[m][n] = true;
					if(filterTempMem[m][n] != NULL) {
						delete[] filterTempMem[m][n];
						filterTempMem[m][n] = NULL;
					}
					filterTempMem[m][n] = &filterMem[filterTempCounter];
					filterTempCounter++;
				}
			}
			else if (m == filterSize2/2) {
					allocaterMarker[m][n] = true;
					if(filterTempMem[m][n] != NULL) {
						delete[] filterTempMem[m][n];
						filterTempMem[m][n] = NULL;
					}
					filterTempMem[m][n] = &filterMem[filterTempCounter];
					filterTempCounter++;
			}
			else {
				if (n >= filterSize1/2) {
					allocaterMarker[m][n] = true;
					if(filterTempMem[m][n] != NULL) {
						delete[] filterTempMem[m][n];
						filterTempMem[m][n] = NULL;
					}
					filterTempMem[m][n] = &filterMem[filterTempCounter];
					filterTempCounter++;
				}
			}
		}
	}

	memValues  = new bitArray*[filterSize2/2];

	if (memValues == NULL)
		throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);

	for (m = 0; m < filterSize2/2; m++) {
		memValues[m] = new bitArray[(*histogram)->getValueDim1() - filterSize1 / 2];

		if (memValues[m] == NULL)
			throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);

	}

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

filterDim1Dim2Mod::~filterDim1Dim2Mod() {

	int m;

	if(filterTempMem != NULL) {
		for (m = 0; m < filterSize2; m++) {
			if(filterTempMem[m] != NULL) {
				for (int n = 0; n < filterSize1; n++) {
					if (allocaterMarker[m][n] == false)
						if(filterTempMem[m][n] != NULL) {
							delete[] filterTempMem[m][n];
							filterTempMem[m][n] = NULL;
						}
				}
				delete[] filterTempMem[m];
				filterTempMem[m] = NULL;
			}
		}
		delete[] filterTempMem;
		filterTempMem = NULL;
	}
	if(memValues != NULL) {
		for (m = 0; m < filterSize2/2; m++) {
			if(memValues[m] != NULL) {
				delete[] memValues[m];
				memValues[m] = NULL;
			}
		}
		delete[] memValues;
		memValues = NULL;
	}
	if(allocaterMarker != NULL) {
		for (m = 0; m < filterSize2; m++) {
			if(allocaterMarker[m] != NULL) {
				delete[] allocaterMarker[m];
				allocaterMarker[m] = NULL;
			}
		}
		delete[] allocaterMarker;
		allocaterMarker = NULL;
	}

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void filterDim1Dim2Mod::init( histogramData** _histogram,
							  unsigned short  filterArithmetic,
							  unsigned short  size1,
							  unsigned short  size2,
							  unsigned short  localSize1,
							  unsigned short  localSize2,
							  bitArray maximumClass) {

	int          m;
	int          n;
	unsigned int filterTempCounter = 0;

	/* free the old allocated space */
	if (baseFilter != NULL) {
		delete baseFilter;
		baseFilter = NULL;
	}
	if (filterMem != NULL) {
		delete[] filterMem;
		filterMem = NULL;
	}
	if (filterTempMem != NULL) {
		for (m = 0; m < filterSize2; m++) {
			if (filterTempMem[m] != NULL) {
				for (n = 0; n < filterSize1; n++) {
					if (filterTempMem[m][n] != NULL) {
						delete[] filterTempMem[m][n];
						filterTempMem[m][n] = NULL;
					}
				}
				delete[] filterTempMem[m];
				filterTempMem[m] = NULL;
			}
		}
		delete[] filterTempMem;
		filterTempMem = NULL;
	}
	if (allocaterMarker != NULL) {
		for (m = 0; m < filterSize2; m++) {
			if (allocaterMarker[m] != NULL) {
				delete[] allocaterMarker[m];
				allocaterMarker[m] = NULL;
			}
		}
		delete[] allocaterMarker;
		allocaterMarker = NULL;
	}
	if (memValues != NULL) {
		for (m = 0; m < filterSize2/2; m++) {
			if (memValues[m] != NULL) {
				delete[] memValues[m];
				memValues[m] = NULL;
			}
		}
		delete[] memValues;
		memValues = NULL;
	}

	/* set new parameter */
	filterDimXDimX::init(_histogram, size1, size2, (size1+1)*(size2/2)+size1, (localSize1+1)*(localSize2/2)+localSize1);

	/* allocate new space */

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

	filterTempMem = new bitArray**[filterSize2];

	if (filterTempMem == NULL)
		throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);

	for (m = 0; m < filterSize2; m++) {
		filterTempMem[m] = new bitArray*[filterSize1];

		if (filterTempMem[m] == NULL)
			throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);

		for (n = 0; n < filterSize1; n++) {
			filterTempMem[m][n] = new bitArray[1];

			if (filterTempMem[m][n] == NULL)
				throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);

		}
	}

	allocaterMarker = new bool*[filterSize2];

	if (allocaterMarker == NULL)
		throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);

	for (m = 0; m < filterSize2; m++) {
		allocaterMarker[m] = new bool[filterSize1];

		if (allocaterMarker[m] == NULL)
			throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);

	}

	filterTempCounter = 0;
	for (n = 0; n < filterSize1; n++) {
		for (m = 0; m < filterSize2; m++) {
			if (m < filterSize2/2) {
				if (n <= filterSize1/2) {
					allocaterMarker[m][n] = true;
					if(filterTempMem[m][n] != NULL) {
						delete[] filterTempMem[m][n];
						filterTempMem[m][n] = NULL;
					}
					filterTempMem[m][n] = &filterMem[filterTempCounter];
					filterTempCounter++;
				}
			}
			else if (m == filterSize2/2) {
					allocaterMarker[m][n] = true;
					if(filterTempMem[m][n] != NULL) {
						delete[] filterTempMem[m][n];
						filterTempMem[m][n] = NULL;
					}
					filterTempMem[m][n] = &filterMem[filterTempCounter];
					filterTempCounter++;
			}
			else {
				if (n >= filterSize1/2) {
					allocaterMarker[m][n] = true;
					if(filterTempMem[m][n] != NULL) {
						delete[] filterTempMem[m][n];
						filterTempMem[m][n] = NULL;
					}
					filterTempMem[m][n] = &filterMem[filterTempCounter];
					filterTempCounter++;
				}
			}
		}
	}

	memValues  = new bitArray*[filterSize2/2];

	if (memValues == NULL)
		throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);

	for (m = 0; m < filterSize2/2; m++) {
		memValues[m] = new bitArray[(*_histogram)->getValueDim1() - filterSize1 / 2];

		if (memValues[m] == NULL)
			throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);

	}

}

/****************************************************************
 * This method implements the filter in the Dim1- and Dim2-axe.	*
 ****************************************************************/

void filterDim1Dim2Mod::filter() {

	unsigned short valueDim1;
	unsigned short valueDim2;
	int            k;
	int            l;
	unsigned int   filterIndexHelp;

	if (filterTempMem == NULL)
		throw cannotAccessFilterMemoryError();

	if (memValues == NULL)
		throw cannotAccessFilterMemoryError();

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

	for (int i = 0; i < valueDim2; i++) {
		for (k = 0; k < filterSize2; k++) {
			for (l = 0; l < filterSize1; l++) {
				filterTempMem[k][l][0] = bitArray(0);
			}
		}
		for (int j = 0; j < valueDim1 + filterSize1 / 2; j++) {
			/* store the temporary values which should not be overwritten in a special memory */
			for (k = 0; k < filterSize2 / 2; k++) {
				for (l = 1; l < valueDim1 - filterSize1 / 2; l++) {
					memValues[k][l - 1] = memValues[k][l];
				}
				memValues[k][valueDim1 - filterSize1 / 2 - 1] = filterTempMem[k + 1][0][0];
			}
			/* initialize the filter values */
			for (k = 0; k < filterSize2; k++) {
				for (l = 1; l < filterSize1; l++) {
					filterTempMem[k][l - 1][0] = filterTempMem[k][l][0];
				}
				if ((i + k - (filterSize2 / 2) >= 0) && (i + k - (filterSize2 / 2) < valueDim2) && (j < valueDim1))
					if (k < filterSize2 / 2)
						filterTempMem[k][filterSize1 - 1][0] = memValues[k][0];
					else
						filterTempMem[k][filterSize1 - 1][0] = (*histogram)->getCell(j, i + k - (filterSize2 / 2))->value;
				else
					filterTempMem[k][filterSize1 - 1][0] = bitArray(0);
			}
			/* end mode of filtering */
			if (j >= valueDim1) {
				filterIndexHelp = (j + 1 - valueDim1) * ((filterSize2 + 1) / 2);
				(*histogram)->getCell(j - filterSize1 / 2, i)->value = baseFilter->filter(filterMem, filterSize - filterIndexHelp, filterLocalSize, filterSize / 2);
			}
			/* normal mode of filtering */
			else if (j >= filterSize1-1) {
				(*histogram)->getCell(j - filterSize1 / 2, i)->value = baseFilter->filter(filterMem, filterSize, filterLocalSize, filterSize / 2);
			}
			/* start mode of filtering */
			else if (j >= filterSize1 / 2) {
				filterIndexHelp = (j + 1 - filterSize1 / 2) * ((filterSize2 + 1) / 2);
				(*histogram)->getCell(j - filterSize1 / 2, i)->value = baseFilter->filter(&filterMem[filterSize / 2 - filterIndexHelp], 1 + filterSize / 2 + filterIndexHelp, filterLocalSize, filterIndexHelp);
			}
		}
	}

}
