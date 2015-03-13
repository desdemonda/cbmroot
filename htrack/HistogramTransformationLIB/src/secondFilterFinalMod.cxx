/* *******************************************************************
// (C)opyright 2005
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
// $Revision: 1.8 $
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
#include "../include/secondFilterFinalMod.h"
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

secondFilterFinalMod::secondFilterFinalMod() : filterDimZDimZDimZ() {

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

secondFilterFinalMod::secondFilterFinalMod( trackData**    _tracks,
									  unsigned short  filterArithmetic,
									  unsigned short size1,
									  unsigned short size2,
									  unsigned short size3,
									  unsigned short localSize1,
									  unsigned short localSize2,
									  unsigned short localSize3,
									  bitArray maximumClass) : filterDimZDimZDimZ() {

	unsigned short size;
	unsigned short localSize;

	size      = (size3 - 1) * ((size1 / 2) * (size2 + 1) + 1) + 1;
	localSize = (2 * ( min( localSize1, localSize2) * min( localSize1, localSize2) + min( localSize1, localSize2)) + 1) * (localSize3 - 1);
	if (localSize > size)
		localSize = size;

	filterDimZDimZDimZ::init(_tracks, size1, size2, size3, size, localSize);

	switch(filterArithmetic) {

		case SECONDSIMPLEARITHMETIC:
			baseFilter      = new filterBasicSimple();
			break;

		case SECONDSIMPLEMODARITHMETIC:
			baseFilter      = new filterBasicSimpleMod();
			break;

		case SECONDCOMPLEXARITHMETIC:
			baseFilter      = new filterBasicComplex();
			break;

		case SECONDCOMPLEXMODARITHMETIC:
			baseFilter      = new filterBasicComplexMod();
			break;

		case SECONDSPECIALARITHMETIC:
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

secondFilterFinalMod::~secondFilterFinalMod() {

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void secondFilterFinalMod::init( trackData**    _tracks,
								 unsigned short  filterArithmetic,
								 unsigned short   size1,
								 unsigned short   size2,
								 unsigned short   size3,
								 unsigned short   localSize1,
								 unsigned short   localSize2,
								 unsigned short   localSize3,
								 bitArray maximumClass) {
					  
	unsigned short size;
	unsigned short localSize;

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
	/* set new parameter */
	size      = (size3 - 1) * ((size1 / 2) * (size2 + 1) + 1) + 1;
	localSize = (2 * ( min( localSize1, localSize2) * min( localSize1, localSize2) + min( localSize1, localSize2)) + 1) * (localSize3 - 1);
	if (localSize > size)
		localSize = size;

	filterDimZDimZDimZ::init(_tracks, size1, size2, size3, size, localSize);

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

void secondFilterFinalMod::filter(std::streambuf* terminal) {

	unsigned short                 i;
	unsigned short                 j;
	unsigned short                 filterCounter;
	unsigned short                 filterMiddleIndex;
	unsigned short                 preDimEqual;
	unsigned short                 postDimEqual;
	terminalSequence               statusSequence;
	trackLayerAccess*              trackAccess;
	int                            elementCoordinate1;
	int                            elementCoordinate2;
	int                            filterCoordinate1;
	int                            filterCoordinate2;
	bool                           dim1Reset;
	bool                           dim2Reset;
	bool                           dim3Reset;
	bitArray                       filterResult;
	removingInformation            removeElement;
	std::list<removingInformation> removeElements;

	if (filterMem == NULL)
		throw cannotAccessFilterMemoryError();

	if (baseFilter == NULL)
		throw cannotAccessFilterError();

	trackAccess = new trackLayerAccess[filterSize3];

	if (trackAccess == NULL)
		throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);

	createTerminalStatusSequence(&statusSequence, terminal, "Process histogram:\t\t\t\t", (unsigned int)(*tracks)->getNumberOfLayers());
	terminalInitialize(statusSequence);

	for (i = 0; i < (*tracks)->getNumberOfLayers(); i++) {

		for (j = 0; j < filterSize3; j++) {

			if (((int)i - (int)(filterSize3/2) + (int)j >= (int)0) && ((int)i - (int)(filterSize3/2) + (int)j < (int)(*tracks)->getNumberOfLayers())) {
				trackAccess[j].begin   = (*tracks)->getTrackIteratorBeginOfHistogramLayer(i - (filterSize3/2) + j);
				trackAccess[j].end     = (*tracks)->getTrackIteratorEndOfHistogramLayer(i - (filterSize3/2) + j);
				trackAccess[j].isValid = true;
			}
			else
				trackAccess[j].isValid = false;

		}

		for (trackLayer::iterator element = trackAccess[filterSize3/2].begin; element != trackAccess[filterSize3/2].end; ) {

			filterCounter     = 0;
			filterMiddleIndex = 0;
			preDimEqual       = 0;
			postDimEqual      = 0;

			for (j = 0; j < filterSize3; j++) {

				if (!trackAccess[j].isValid)
					continue;

				if (j == filterSize3/2) {
					filterMem[filterCounter] = element->value;
					filterMiddleIndex        = filterCounter;
					filterCounter++;
					continue;
				}

				elementCoordinate1   = element->position.get(DIM1);
				elementCoordinate2   = element->position.get(DIM2);

				for (trackLayer::iterator _filter = trackAccess[j].begin; _filter != trackAccess[j].end; _filter++) {

					filterCoordinate1    = _filter->position.get(DIM1);
					filterCoordinate2    = _filter->position.get(DIM2);

					if ((filterCoordinate1 <= elementCoordinate1) && (filterCoordinate1 >= elementCoordinate1 - filterSize1/2) && (filterCoordinate2 < elementCoordinate2) && (filterCoordinate2 >= elementCoordinate2 - filterSize2/2))
						dim1Reset = true;
					else
						dim1Reset = false;

					if ((filterCoordinate1 == elementCoordinate1) && (filterCoordinate2 == elementCoordinate2)) {
						dim2Reset = true;
						if (j < filterSize3/2)
							preDimEqual++;
						else
							postDimEqual++;
					}
					else
						dim2Reset = false;

					if ((filterCoordinate1 >= elementCoordinate1) && (filterCoordinate1 <= elementCoordinate1 + filterSize1/2) && (filterCoordinate2 > elementCoordinate2) && (filterCoordinate2 <= elementCoordinate2 + filterSize2/2))
						dim3Reset = true;
					else
						dim3Reset = false;

					if (dim1Reset || dim2Reset || dim3Reset) {
						filterMem[filterCounter] = _filter->value;
						filterCounter++;
					}

				}

			}

			filterResult = baseFilter->filter(filterMem, filterCounter, filterLocalSize, filterMiddleIndex);

			if (filterResult != element->value) {

/* Here the saving of the position for the later removing process is	*
 * done.																*/

				removeElement.element = element;
				removeElement.layer   = i;
				if ((preDimEqual > 0) && (postDimEqual > 0)) {
					if ((preDimEqual + postDimEqual) % 2 == 0) {
						if (preDimEqual != postDimEqual)
							removeElements.push_back(removeElement);
					}
					else {
						if (preDimEqual != postDimEqual + 1)
							removeElements.push_back(removeElement);
					}
				}
				else
					removeElements.push_back(removeElement);
				element++;

			}
			else
				element++;

		}

		terminalOverwriteWithIncrement(statusSequence);

	}

	terminalFinalize(statusSequence);

/* Here the removing of the tracks is done if inplace is undefined		*/

	createTerminalStatusSequence(&statusSequence, terminal, "Remove filtered tracks:\t\t", (unsigned int)removeElements.size());
	terminalInitialize(statusSequence);

	while (removeElements.size() > 0) {

		removeElement = removeElements.back();
		(*tracks)->removeTrack(removeElement.element, removeElement.layer);
		removeElements.pop_back();
		terminalOverwriteWithIncrement(statusSequence);

	}

	terminalFinalize(statusSequence);

	if (trackAccess != NULL) {
		delete [] trackAccess;
		trackAccess = NULL;
	}

}
