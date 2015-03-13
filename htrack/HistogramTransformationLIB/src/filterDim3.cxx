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
// $Date: 2008-10-24 16:40:41 $
// $Revision: 1.6 $
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
#include "../include/filterDim3.h"
#include <stdio.h>

#ifdef __APPLE__
#include <malloc/malloc.h>
#else
#include <malloc.h>
#endif

/****************************************************************
 * Default constructor											*
 ****************************************************************/

filterDim3::filterDim3() : filterDimZ() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

filterDim3::filterDim3( trackData**    _tracks,
						unsigned short  filterArithmetic,
						unsigned short size,
						unsigned short localSize,
						bitArray maximumClass) : filterDimZ(
						_tracks, size, localSize) {

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

filterDim3::~filterDim3() {

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void filterDim3::init( trackData**    _tracks,
					   unsigned short  filterArithmetic,
					   unsigned short size,
					   unsigned short localSize,
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
	filterDimZ::init(_tracks, size, localSize);

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
 * This method implements the filter in the Dim3-axe.			*
 ****************************************************************/

void filterDim3::filter(std::streambuf* terminal) {

	unsigned short                 i;
	unsigned short                 j;
	terminalSequence               statusSequence;
	trackLayerAccess*              trackAccess;
	int                            elementCoordinate;
	int                            filterCoordinate;
	bool                           dim1Reset;
	bool                           dim2Reset;
	bitArray                       filterResult;
	bool                           firstValueIsRemoved;

	if (filterMem == NULL)
		throw cannotAccessFilterMemoryError();

	if (baseFilter == NULL)
		throw cannotAccessFilterError();

	trackAccess = new trackLayerAccess[filterSize];

	if (trackAccess == NULL)
		throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);

	createTerminalStatusSequence(&statusSequence, terminal, "Process histogram:\t\t\t\t", (unsigned int)(*tracks)->getNumberOfLayers());
	terminalInitialize(statusSequence);

	for (i = 0; i < (*tracks)->getNumberOfLayers(); i++) {

		for (j = 0; j < filterSize; j++) {

			if (((int)i - (int)(filterSize/2) + (int)j >= (int)0) && ((int)i - (int)(filterSize/2) + (int)j < (int)(*tracks)->getNumberOfLayers())) {
				trackAccess[j].begin   = (*tracks)->getTrackIteratorBeginOfHistogramLayer(i - (filterSize/2) + j);
				trackAccess[j].end     = (*tracks)->getTrackIteratorEndOfHistogramLayer(i - (filterSize/2) + j);
				trackAccess[j].isValid = true;
			}
			else
				trackAccess[j].isValid = false;

		}

		for (trackLayer::iterator element = trackAccess[filterSize/2].begin; element != trackAccess[filterSize/2].end; ) {

			for (j = 0; j < filterSize; j++) {

				filterMem[j] = bitArray(0);

				if (!trackAccess[j].isValid)
					continue;

				if (j == filterSize/2) {
					filterMem[j] = element->value;
					continue;
				}

				for (trackLayer::iterator _filter = trackAccess[j].begin; _filter != trackAccess[j].end; _filter++) {

					elementCoordinate    = element->position.get(DIM1);
					filterCoordinate     = _filter->position.get(DIM1);
					if (filterCoordinate == elementCoordinate)
						dim1Reset = true;
					else
						dim1Reset = false;
				
					if (!dim1Reset)
						continue;

					elementCoordinate    = element->position.get(DIM2);
					filterCoordinate     = _filter->position.get(DIM2);
					if (filterCoordinate == elementCoordinate)
						dim2Reset = true;
					else
						dim2Reset = false;

					if (dim1Reset && dim2Reset) {
						filterMem[j] = _filter->value;
						break;
					}

				}

			}

			filterResult = baseFilter->filter(filterMem, filterSize, filterLocalSize, filterSize/2);
			
			if (filterResult != element->value) {

/* This removing of tracks was in the space of the next computations.	*
 * But this filter should delete the values after all computations.	So	*
 * the position must be saved and the removing has to be done later.	*/

				if (element == trackAccess[filterSize/2].begin)
					firstValueIsRemoved = true;
				else
					firstValueIsRemoved = false;
				element = (*tracks)->removeTrack(element, i);
				if (firstValueIsRemoved)
					trackAccess[filterSize/2].begin = element;

			}
			else
				element++;

		}

		terminalOverwriteWithIncrement(statusSequence);

	}

	terminalFinalize(statusSequence);

	if (trackAccess != NULL) {
		delete[] trackAccess;
		trackAccess = NULL;
	}

}
