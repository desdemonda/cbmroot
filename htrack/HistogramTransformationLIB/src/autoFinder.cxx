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
//     - class for evaluating the histogram
//     - class computes the tracks
//
//     CAUTION: This class cannot be backward compatible with Version 4.0.
//              This is because of the histogram reduction from 3D to 2D. So
//              the filter had to be reduced in the corresponding way. And
//              the reduction of the filter cannot be done bittrue.
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-10-24 16:40:41 $
// $Revision: 1.5 $
//
// *******************************************************************/


#include "../../MiscLIB/include/terminal.h"
#include "../include/filterDef.h"
#include "../include/histogramTransformationError.h"
#include "../include/filterBasicNeutral.h"
#include "../include/filterBasicSimple.h"
#include "../include/filterBasicSimpleMod.h"
#include "../include/filterBasicComplex.h"
#include "../include/filterBasicComplexMod.h"
#include "../include/filterBasicSpecial.h"
#include "../include/autoFinder.h"


#define SPEEDUPPEAKFINDING		/**< This definition enables a speedup of the peakfinding because each zero element in the histogram is not filtered. */
//#undef SPEEDUPPEAKFINDING


//#define DEBUGPEAKFINDINGGEOMETRY	/**< This definition enables a print function to debug the implemented geometry. */
#undef DEBUGPEAKFINDINGGEOMETRY


/****************************************************************
 * deletes allocated and initialized memory						*
 ****************************************************************/

void autoFinder::deleteMemory() {

	deleteFirstFilterMemory();
	deleteSecondFilterMemory();

}
void autoFinder::deleteFirstFilterMemory() {

	/* first filter */
	if (firstFilterMem != NULL) {
		delete [] firstFilterMem;
		firstFilterMem = NULL;
	}
	sizeOfFirstFilterMem = 0;

	if (firstBufferMem != NULL) {
		for (unsigned short i = 0; i < sizeOfFirstBufferMem; i++) {
			if (firstBufferMem[i].isLocal) {
				delete firstBufferMem[i].pointer;
				firstBufferMem[i].pointer = NULL;
			}
		}
		free(firstBufferMem);
		firstBufferMem = NULL;
	}
	sizeOfFirstBufferMem = 0;

}
void autoFinder::deleteSecondFilterMemory() {

	/* second filter */
	if (secondFilterMem != NULL) {
		delete [] secondFilterMem;
		secondFilterMem = NULL;
	}
	sizeOfSecondFilterMem = 0;

}

/****************************************************************
 * This method inits the memory in a speciallized manner.		*
 ****************************************************************/

void autoFinder::initMemory() {

	if (useFirstFilterMod)
		initFirstFilterMemoryWithoutDirectOverwriting();
	else
		initFirstFilterMemoryWithDirectOverwriting();

	if (useSecondFilterMod)
		initSecondFilterMemoryWithoutDirectOverwriting();
	else
		initSecondFilterMemoryWithDirectOverwriting();

}
void autoFinder::initFirstFilterMemoryWithoutDirectOverwriting() {

	specialListMem<peakfindingGeometryElement> actualGeometry;
	short                                      dim1Min;
	short                                      dim1Max;
	short                                      dim3Min;
	short                                      dim3Max;
	peakfindingGeometryElement                 actualGeometryMinElement;
	peakfindingGeometryElement                 actualGeometryMaxElement;
	unsigned short                             sizeOfFirstFilterLine;
	unsigned short                             i;
	peakfindingGeometryElement                 actualGeometryElement;
	unsigned short                             actualGeometryElementPosition;

	deleteFirstFilterMemory();

	actualGeometry           = geometry.getCoveredProjectedGeometryElements();                

	geometry.getMinMaxDimensions(&dim1Min, &dim1Max, NULL, NULL, &dim3Min, &dim3Max);

	/* first filter */
	sizeOfFirstFilterMem          = (unsigned short)actualGeometry.getNumberOfEntries();
	firstFilterMem                = new bitArray[sizeOfFirstFilterMem];
	if (firstFilterMem == NULL)
		throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);

	actualGeometryMinElement = geometry.getCoveredProjectedMinElement();
	actualGeometryMaxElement = geometry.getCoveredProjectedMaxElement();

	if ((actualGeometryMaxElement.get(DIM2) < actualGeometryMinElement.get(DIM2)) || ((actualGeometryMaxElement.get(DIM2) == actualGeometryMinElement.get(DIM2)) && (actualGeometryMaxElement.get(DIM1) < actualGeometryMinElement.get(DIM1))))
		throw wrongPeakfindingGeometryError();

	sizeOfFirstFilterLine     = (*histogram)->getValueDim1() + abs(dim1Max) + abs(dim1Min);
	sizeOfFirstBufferMem      = (unsigned short)(actualGeometryMaxElement.get(DIM2) - actualGeometryMinElement.get(DIM2)) * sizeOfFirstFilterLine + (unsigned short)(actualGeometryMaxElement.get(DIM1) - actualGeometryMinElement.get(DIM1)) + 1;
	firstBufferMem            = (geometryMemoryProfile*)calloc(sizeOfFirstBufferMem, sizeof(geometryMemoryProfile));
	if (firstBufferMem == NULL)
		throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);

#ifdef DEBUGPEAKFINDINGGEOMETRY

	actualGeometry.resetActiveObject();
	for (unsigned long k = 0; k < actualGeometry.getNumberOfEntries(); k++) {
		actualGeometryElement = actualGeometry.readActiveObjectAndMakeNextOneActive();
		std::cout << actualGeometryElement.toString() << std::endl;
	}
	std::cout << "sizeOfFirstFilterLine = " << sizeOfFirstFilterLine;
	std::cout << std::endl;

#endif

	for (i = 0; i < sizeOfFirstBufferMem; i++)
		firstBufferMem[i].isLocal = true;

	actualGeometry.resetActiveObject();
	for (unsigned long j = 0; j < actualGeometry.getNumberOfEntries(); j++) {

		actualGeometryElement         = actualGeometry.readActiveObjectAndMakeNextOneActive();

		actualGeometryElementPosition = (actualGeometryElement.get(DIM2) - actualGeometryMinElement.get(DIM2)) * sizeOfFirstFilterLine + (actualGeometryElement.get(DIM1) - actualGeometryMinElement.get(DIM1));

		firstBufferMem[actualGeometryElementPosition].pointer = &firstFilterMem[j];
		if (firstBufferMem[actualGeometryElementPosition].pointer == NULL)
			throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);

		firstBufferMem[actualGeometryElementPosition].isLocal = false;

		if ((actualGeometryElement.get(DIM1) == 0) && (actualGeometryElement.get(DIM2) == 0))
			firstFilterMiddleIndex = j;

#ifdef DEBUGPEAKFINDINGGEOMETRY

		std::cout << "firstBufferMem[" << actualGeometryElementPosition << "].pointer = &firstFilterMem[" << j << "]" << std::endl;
		std::cout << " actualGeometryElement.get(DIM1) = " << actualGeometryElement.get(DIM1) << std::endl;
		std::cout << " actualGeometryElement.get(DIM2) = " << actualGeometryElement.get(DIM2) << std::endl;
		std::cout << " firstFilterMiddleIndex = " << firstFilterMiddleIndex;
		std::cout << std::endl;

#endif

	}

	for (i = 0; i < sizeOfFirstBufferMem; i++) {

		if (firstBufferMem[i].isLocal) {

			firstBufferMem[i].pointer = new bitArray();
			if (firstBufferMem[i].pointer == NULL)
				throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);

		}

	}

}
void autoFinder::initSecondFilterMemoryWithoutDirectOverwriting() {

	specialListMem<peakfindingGeometryElement> actualGeometry;
	short                                      dim3Min;
	short                                      dim3Max;

	deleteSecondFilterMemory();

	actualGeometry                 = geometry.getCoveredProjectedGeometryElements();                

	geometry.getMinMaxDimensions(NULL, NULL, NULL, NULL, &dim3Min, &dim3Max);

	/* second filter */
	sizeOfSecondFilterMem          = ((unsigned short)actualGeometry.getNumberOfEntries()) * ((unsigned short)(dim3Max - dim3Min + 1));
	secondFilterMem                = new bitArray[sizeOfFirstFilterMem];
	if (secondFilterMem == NULL)
		throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);

}
void autoFinder::initFirstFilterMemoryWithDirectOverwriting() {

	specialListMem<peakfindingGeometryElement> actualGeometry;
	short                                      dim1Min;
	short                                      dim1Max;
	short                                      dim2Min;
	short                                      dim2Max;
	short                                      dim3Min;
	short                                      dim3Max;
	unsigned short                             sizeOfFirstFilterLine;
	unsigned short                             i;
	peakfindingGeometryElement                 actualGeometryElement;
	unsigned short                             actualGeometryElementPosition;

	deleteFirstFilterMemory();

	actualGeometry           = geometry.getCoveredProjectedGeometryElements();                

	geometry.getMinMaxDimensions(&dim1Min, &dim1Max, &dim2Min, &dim2Max, &dim3Min, &dim3Max);

	/* first filter */
	sizeOfFirstFilterMem          = (unsigned short)actualGeometry.getNumberOfEntries();
	firstFilterMem                = new bitArray[sizeOfFirstFilterMem];
	if (firstFilterMem == NULL)
		throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);

	sizeOfFirstFilterLine     = (unsigned short)(dim1Max - dim1Min + 1);
	sizeOfFirstBufferMem      = (unsigned short)(dim2Max - dim2Min + 1) * sizeOfFirstFilterLine;
	firstBufferMem            = (geometryMemoryProfile*)calloc(sizeOfFirstBufferMem, sizeof(geometryMemoryProfile));
	if (firstBufferMem == NULL)
		throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);

#ifdef DEBUGPEAKFINDINGGEOMETRY

	actualGeometry.resetActiveObject();
	for (unsigned long k = 0; k < actualGeometry.getNumberOfEntries(); k++) {
		actualGeometryElement = actualGeometry.readActiveObjectAndMakeNextOneActive();
		std::cout << actualGeometryElement.toString() << std::endl;
	}
	std::cout << "sizeOfFirstFilterLine = " << sizeOfFirstFilterLine;
	std::cout << std::endl;

#endif

	for (i = 0; i < sizeOfFirstBufferMem; i++)
		firstBufferMem[i].isLocal = true;

	actualGeometry.resetActiveObject();
	for (unsigned long j = 0; j < actualGeometry.getNumberOfEntries(); j++) {

		actualGeometryElement         = actualGeometry.readActiveObjectAndMakeNextOneActive();

		actualGeometryElementPosition = (unsigned short)(actualGeometryElement.get(DIM2) - dim2Min) * sizeOfFirstFilterLine + (unsigned short)(actualGeometryElement.get(DIM1) - dim1Min);

		firstBufferMem[actualGeometryElementPosition].pointer = &firstFilterMem[j];
		if (firstBufferMem[actualGeometryElementPosition].pointer == NULL)
			throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);

		firstBufferMem[actualGeometryElementPosition].isLocal = false;

		if ((actualGeometryElement.get(DIM1) == 0) && (actualGeometryElement.get(DIM2) == 0))
			firstFilterMiddleIndex = j;

#ifdef DEBUGPEAKFINDINGGEOMETRY

		std::cout << "firstBufferMem[" << actualGeometryElementPosition << "].pointer = &firstFilterMem[" << j << "]" << std::endl;
		std::cout << " actualGeometryElement.get(DIM1) = " << actualGeometryElement.get(DIM1) << std::endl;
		std::cout << " actualGeometryElement.get(DIM2) = " << actualGeometryElement.get(DIM2) << std::endl;
		std::cout << " firstFilterMiddleIndex = " << firstFilterMiddleIndex;
		std::cout << std::endl;

#endif

	}

	for (i = 0; i < sizeOfFirstBufferMem; i++) {

		if (firstBufferMem[i].isLocal) {

			firstBufferMem[i].pointer = new bitArray();
			if (firstBufferMem[i].pointer == NULL)
				throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);

		}

	}

}
void autoFinder::initSecondFilterMemoryWithDirectOverwriting() {

	specialListMem<peakfindingGeometryElement> actualGeometry;
	short                                      dim3Min;
	short                                      dim3Max;

	deleteSecondFilterMemory();

	actualGeometry           = geometry.getCoveredProjectedGeometryElements();                

	geometry.getMinMaxDimensions(NULL, NULL, NULL, NULL, &dim3Min, &dim3Max);

	/* second filter */
	sizeOfSecondFilterMem          = ((unsigned short)actualGeometry.getNumberOfEntries()) * ((unsigned short)(dim3Max - dim3Min + 1));
	secondFilterMem                = new bitArray[sizeOfFirstFilterMem];
	if (secondFilterMem == NULL)
		throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);

}

/****************************************************************
 * This method filters the peakclasses in a speciallized manner.*
 ****************************************************************/

void autoFinder::filteringHistogramLayer() {

	if (useFirstFilterMod)
		filteringHistogramLayerWithoutDirectOverwriting();
	else
		filteringHistogramLayerWithDirectOverwriting();

}
void autoFinder::filteringHistogramLayerWithoutDirectOverwriting() {

	short                      dim1Min;
	short                      dim1Max;
	short                      dim2Min;
	short                      dim2Max;
	peakfindingGeometryElement actualGeometryMinElement;
	unsigned short             dim1Offset;
	unsigned short             dim2Offset;
	bitArray                   zeroComparator;

	if (firstFilterMem == NULL)
		throw cannotAccessFilterMemoryError();

	if (firstBufferMem == NULL)
		throw cannotAccessFilterMemoryError();

	if (firstBaseFilter == NULL)
		throw cannotAccessFilterError();

	if (histogram == NULL)
		throw cannotAccessHistogramError(HISTOGRAMTRANSFORMATIONLIB);
	if (*histogram == NULL)
		throw cannotAccessHistogramError(HISTOGRAMTRANSFORMATIONLIB);

	geometry.getMinMaxDimensions(&dim1Min, &dim1Max, &dim2Min, &dim2Max, NULL, NULL);

	actualGeometryMinElement = geometry.getCoveredProjectedMinElement();
	dim1Offset               = abs(actualGeometryMinElement.get(DIM1));
	dim2Offset               = abs(actualGeometryMinElement.get(DIM2));
	zeroComparator           = bitArray(0);

	for (int i = (*histogram)->getValueDim2() + dim2Max - 1; i >= dim2Min; i--) {
		for (int j = (*histogram)->getValueDim1() + dim1Max - 1; j >= dim1Min; j--) {

			for (unsigned short k = sizeOfFirstBufferMem - 1; k > 0; k--)
				*(firstBufferMem[k].pointer) = *(firstBufferMem[k - 1].pointer);

			if ((j < (*histogram)->getValueDim1()) && (j >= 0) && (i < (*histogram)->getValueDim2()) && (i >= 0))
				*(firstBufferMem[0].pointer) = (*histogram)->getCell(j, i)->value;
			else
				*(firstBufferMem[0].pointer) = bitArray(0);

			if ((j + dim1Offset < (*histogram)->getValueDim1()) && (j + dim1Offset >= 0) && (i + dim2Offset < (*histogram)->getValueDim2()) && (i + dim2Offset >= 0)) {

				/* do filtering */
				if ((*histogram)->getCell(j + dim1Offset, i + dim2Offset)->value != zeroComparator)
					(*histogram)->getCell(j + dim1Offset, i + dim2Offset)->value = firstBaseFilter->filter(firstFilterMem, sizeOfFirstFilterMem, sizeOfFirstFilterMem / 2, firstFilterMiddleIndex);

#ifdef DEBUGPEAKFINDINGGEOMETRY

				std::cout << "input: dim1= " << j << ", dim2= " << i << std::endl;
				std::cout << "firstFilterMem:";
				for (int a = 0; a < sizeOfFirstFilterMem; a++)
					std::cout << " " << firstFilterMem[a];
				std::cout << std::endl;
				std::cout << "output: dim1= " << j + dim1Offset << ", dim2= " << i + dim2Offset << std::endl;
				std::cout << "value= " << (*histogram)->getCell(j + dim1Offset, i + dim2Offset)->value << std::endl;
				std::cout << std::endl;

#endif

			}

		}
	}

}
void autoFinder::filteringHistogramLayerWithDirectOverwriting() {

	short          dim1Min;
	short          dim1Max;
	short          dim2Min;
	unsigned short sizeOfFirstFilterLine;

#ifdef SPEEDUPPEAKFINDING

	bitArray       zeroComparator;

#endif

	short          dim1Offset;
	short          dim2Offset;

	if (firstFilterMem == NULL)
		throw cannotAccessFilterMemoryError();

	if (firstBufferMem == NULL)
		throw cannotAccessFilterMemoryError();

	if (firstBaseFilter == NULL)
		throw cannotAccessFilterError();

	if (histogram == NULL)
		throw cannotAccessHistogramError(HISTOGRAMTRANSFORMATIONLIB);
	if (*histogram == NULL)
		throw cannotAccessHistogramError(HISTOGRAMTRANSFORMATIONLIB);

	geometry.getMinMaxDimensions(&dim1Min, &dim1Max, &dim2Min, NULL, NULL, NULL);

	sizeOfFirstFilterLine = (unsigned short)(dim1Max - dim1Min + 1);

#ifdef SPEEDUPPEAKFINDING

	zeroComparator   = bitArray(0);

#endif

	for (int i = 0; i < (*histogram)->getValueDim2(); i++) {
		for (int j = 0; j < (*histogram)->getValueDim1(); j++) {

#ifdef SPEEDUPPEAKFINDING

			if ((*histogram)->getCell(j, i)->value != zeroComparator) {

#endif

				for (unsigned short k = 0; k < sizeOfFirstBufferMem; k++) {

					dim1Offset = (short)(k % sizeOfFirstFilterLine) + dim1Min;
					dim2Offset = (short)(k / sizeOfFirstFilterLine) + dim2Min;

					if ((j + dim1Offset < (*histogram)->getValueDim1()) && (j + dim1Offset >= 0) && (i + dim2Offset < (*histogram)->getValueDim2()) && (i + dim2Offset >= 0))
						*(firstBufferMem[k].pointer) = (*histogram)->getCell(j + dim1Offset, i + dim2Offset)->value;
					else
						*(firstBufferMem[k].pointer) = bitArray(0);

				}

				/* do filtering */
				(*histogram)->getCell(j, i)->value = firstBaseFilter->filter(firstFilterMem, sizeOfFirstFilterMem, sizeOfFirstFilterMem / 2, firstFilterMiddleIndex);

#ifdef DEBUGPEAKFINDINGGEOMETRY

				std::cout << "input: dim1= " << j << ", dim2= " << i << std::endl;
				std::cout << "firstFilterMem:";
				for (int a = 0; a < sizeOfFirstFilterMem; a++)
					std::cout << " " << firstFilterMem[a];
				std::cout << std::endl;
				std::cout << "output: dim1= " << j << ", dim2= " << i << std::endl;
				std::cout << "value= " << (*histogram)->getCell(j, i)->value << std::endl;
				std::cout << std::endl;

#endif

#ifdef SPEEDUPPEAKFINDING

			}

#endif

		}
	}

}

/****************************************************************
 * This method filters the tracks in a speciallized manner		*
 * again.														*
 ****************************************************************/

void autoFinder::filteringHistogram(std::streambuf* terminal) {

	if (useSecondFilterMod)
		filteringHistogramWithoutDirectOverwriting(terminal);
	else
		filteringHistogramWithDirectOverwriting(terminal);

}
void autoFinder::filteringHistogramWithoutDirectOverwriting(std::streambuf* terminal) {

	specialListMem<peakfindingGeometryElement> actualGeometry;
	short                                      dim3Min;
	short                                      dim3Max;
	trackLayerAccess*                          trackAccess;
	terminalSequence                           statusSequence;
	unsigned short                             i;
	short                                      j;
	unsigned short                             filterCounter;
	int                                        elementCoordinate1;
	int                                        elementCoordinate2;
	int                                        filterCoordinate1;
	int                                        filterCoordinate2;
	peakfindingGeometryElement                 filterGeometryElement;
	unsigned short                             preDimEqual;
	unsigned short                             postDimEqual;
	bitArray                                   filterResult;
	removingInformation                        removeElement;
	std::list<removingInformation>             removeElements;

	if (secondFilterMem == NULL)
		throw cannotAccessFilterMemoryError();

	if (secondBaseFilter == NULL)
		throw cannotAccessFilterError();

	actualGeometry = geometry.getCoveredProjectedGeometryElements();                

	filterGeometryElement.set(0, DIM3);

	geometry.getMinMaxDimensions(NULL, NULL, NULL, NULL, &dim3Min, &dim3Max);

	trackAccess = new trackLayerAccess[(unsigned short)(dim3Max - dim3Min + 1)];
	if (trackAccess == NULL)
		throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);

	createTerminalStatusSequence(&statusSequence, terminal, "Process histogram:\t\t\t\t", (unsigned int)(*tracks)->getNumberOfLayers());
	terminalInitialize(statusSequence);

	for (i = 0; i < (*tracks)->getNumberOfLayers(); i++) {

		for (j = dim3Min; j <= dim3Max; j++) {

			if (((int)i + j >= 0) && ((int)i + j < (int)(*tracks)->getNumberOfLayers())) {
				trackAccess[j - dim3Min].begin   = (*tracks)->getTrackIteratorBeginOfHistogramLayer((unsigned short)((int)i + j));
				trackAccess[j - dim3Min].end     = (*tracks)->getTrackIteratorEndOfHistogramLayer((unsigned short)((int)i + j));
				trackAccess[j - dim3Min].isValid = true;
			}
			else
				trackAccess[j - dim3Min].isValid = false;

		}

		for (trackLayer::iterator element = trackAccess[abs(dim3Min)].begin; element != trackAccess[abs(dim3Min)].end; ) {

			filterCounter           = 0;
			secondFilterMiddleIndex = 0;

			for (j = dim3Min; j <= dim3Max; j++) {

				if (!trackAccess[j - dim3Min].isValid)
					continue;

				if (j == 0) {
					secondFilterMem[filterCounter] = element->value;
					secondFilterMiddleIndex        = filterCounter;
					filterCounter++;
					continue;
				}

				elementCoordinate1    = (int)element->position.get(DIM1);
				elementCoordinate2    = (int)element->position.get(DIM2);

				for (trackLayer::iterator filter = trackAccess[j - dim3Min].begin; filter != trackAccess[j - dim3Min].end; filter++) {

					filterCoordinate1 = elementCoordinate1 - (int)filter->position.get(DIM1);
					filterCoordinate2 = elementCoordinate2 - (int)filter->position.get(DIM2);

					filterGeometryElement.set((short)filterCoordinate1, DIM1);
					filterGeometryElement.set((short)filterCoordinate2, DIM2);

					if (actualGeometry.isFound(filterGeometryElement, false)) {

						secondFilterMem[filterCounter] = filter->value;
						filterCounter++;

						if ((filterCoordinate1 == 0) && (filterCoordinate2 == 0)) {
							if (j < 0)
								preDimEqual++;
							else
								postDimEqual++;
						}

					}

				}

			}

			filterResult = secondBaseFilter->filter(secondFilterMem, filterCounter, filterCounter / 2, secondFilterMiddleIndex);

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
void autoFinder::filteringHistogramWithDirectOverwriting(std::streambuf* terminal) {

	specialListMem<peakfindingGeometryElement> actualGeometry;
	short                                      dim3Min;
	short                                      dim3Max;
	trackLayerAccess*                          trackAccess;
	terminalSequence                           statusSequence;
	unsigned short                             i;
	short                                      j;
	unsigned short                             filterCounter;
	int                                        elementCoordinate1;
	int                                        elementCoordinate2;
	int                                        filterCoordinate1;
	int                                        filterCoordinate2;
	peakfindingGeometryElement                 filterGeometryElement;
	bitArray                                   filterResult;
	bool                                       firstValueIsRemoved;

	if (secondFilterMem == NULL)
		throw cannotAccessFilterMemoryError();

	if (secondBaseFilter == NULL)
		throw cannotAccessFilterError();

	actualGeometry = geometry.getCoveredProjectedGeometryElements();                

	filterGeometryElement.set(0, DIM3);

	geometry.getMinMaxDimensions(NULL, NULL, NULL, NULL, &dim3Min, &dim3Max);

	trackAccess = new trackLayerAccess[(unsigned short)(dim3Max - dim3Min + 1)];
	if (trackAccess == NULL)
		throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);

	createTerminalStatusSequence(&statusSequence, terminal, "Process histogram:\t\t\t\t", (unsigned int)(*tracks)->getNumberOfLayers());
	terminalInitialize(statusSequence);

	for (i = 0; i < (*tracks)->getNumberOfLayers(); i++) {

		for (j = dim3Min; j <= dim3Max; j++) {

			if (((int)i + j >= 0) && ((int)i + j < (int)(*tracks)->getNumberOfLayers())) {
				trackAccess[j - dim3Min].begin   = (*tracks)->getTrackIteratorBeginOfHistogramLayer((unsigned short)((int)i + j));
				trackAccess[j - dim3Min].end     = (*tracks)->getTrackIteratorEndOfHistogramLayer((unsigned short)((int)i + j));
				trackAccess[j - dim3Min].isValid = true;
			}
			else
				trackAccess[j - dim3Min].isValid = false;

		}

		for (trackLayer::iterator element = trackAccess[abs(dim3Min)].begin; element != trackAccess[abs(dim3Min)].end; ) {
			
			filterCounter           = 0;
			secondFilterMiddleIndex = 0;

			for (j = dim3Min; j <= dim3Max; j++) {

				if (!trackAccess[j - dim3Min].isValid)
					continue;

				if (j == 0) {
					secondFilterMem[filterCounter] = element->value;
					secondFilterMiddleIndex        = filterCounter;
					filterCounter++;
					continue;
				}

				elementCoordinate1   = (int)element->position.get(DIM1);
				elementCoordinate2   = (int)element->position.get(DIM2);

				for (trackLayer::iterator filter = trackAccess[j - dim3Min].begin; filter != trackAccess[j - dim3Min].end; filter++) {

					filterCoordinate1 = elementCoordinate1 - (int)filter->position.get(DIM1);
					filterCoordinate2 = elementCoordinate2 - (int)filter->position.get(DIM2);

					filterGeometryElement.set((short)filterCoordinate1, DIM1);
					filterGeometryElement.set((short)filterCoordinate2, DIM2);

					if (actualGeometry.isFound(filterGeometryElement, false)) {

						secondFilterMem[filterCounter] = filter->value;
						filterCounter++;

					}

				}

			}

			filterResult = secondBaseFilter->filter(secondFilterMem, filterCounter, filterCounter / 2, secondFilterMiddleIndex);
			
			if (filterResult != element->value) {

/* The removing of tracks here is in the space of the next computations. */

				if (element == trackAccess[abs(dim3Min)].begin)
					firstValueIsRemoved = true;
				else
					firstValueIsRemoved = false;
				element = (*tracks)->removeTrack(element, i);
				if (firstValueIsRemoved)
					trackAccess[abs(dim3Min)].begin = element;

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

/****************************************************************
 * Default constructor											*
 ****************************************************************/

autoFinder::autoFinder() 
  : histogramTransformation(),
    geometry(),
    useFirstFilterMod(false),
    firstFilterMiddleIndex(0),
    sizeOfFirstFilterMem(0),
    firstFilterMem(NULL),
    sizeOfFirstBufferMem(0),
    firstBufferMem(NULL),
    firstBaseFilter(NULL),
    useSecondFilterMod(false),
    secondFilterMiddleIndex(0),
    sizeOfSecondFilterMem  (0),
    secondFilterMem(NULL),
    secondBaseFilter(NULL)
{

	geometry.reset();
	/*
	useFirstFilterMod       = false;
	firstFilterMiddleIndex  = 0;
	sizeOfFirstFilterMem    = 0;
	firstFilterMem          = NULL;
	sizeOfFirstBufferMem    = 0;
	firstBufferMem          = NULL;
	firstBaseFilter         = NULL;
	useSecondFilterMod      = false;
	secondFilterMiddleIndex = 0;
	sizeOfSecondFilterMem   = 0;
	secondFilterMem         = NULL;
	secondBaseFilter        = NULL;
	*/
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

autoFinder::autoFinder(histogramData** _histogram, trackData** _tracks, tables** _ratings, unsigned short _firstFilterArithmetic, bool _useFirstFilterMod, unsigned short _secondFilterArithmetic, bool _useSecondFilterMod) 
  : histogramTransformation(histogram, tracks, ratings),
    geometry(),
    useFirstFilterMod(_useFirstFilterMod),
    firstFilterMiddleIndex(0),
    sizeOfFirstFilterMem(0),
    firstFilterMem(NULL),
    sizeOfFirstBufferMem(0),
    firstBufferMem(NULL),
    firstBaseFilter(NULL),
    useSecondFilterMod(_useSecondFilterMod),
    secondFilterMiddleIndex(0),
    sizeOfSecondFilterMem  (0),
    secondFilterMem(NULL),
    secondBaseFilter(NULL)
{

	geometry.reset();
	/*
	this->useFirstFilterMod  = _useFirstFilterMod;
	firstFilterMiddleIndex   = 0;
	sizeOfFirstFilterMem     = 0;
	firstFilterMem           = NULL;
	sizeOfFirstBufferMem     = 0;
	firstBufferMem           = NULL;
	firstBaseFilter          = NULL;
	this->useSecondFilterMod = _useSecondFilterMod;
	secondFilterMiddleIndex  = 0;
	sizeOfSecondFilterMem    = 0;
	secondFilterMem          = NULL;
	secondBaseFilter         = NULL;
	*/
	switch(_firstFilterArithmetic) {

		case FIRSTSIMPLEARITHMETIC:
			firstBaseFilter = new filterBasicSimple();
			break;

		case FIRSTSIMPLEMODARITHMETIC:
			firstBaseFilter = new filterBasicSimpleMod();
			break;

		case FIRSTCOMPLEXARITHMETIC:
			firstBaseFilter = new filterBasicComplex();
			break;

		case FIRSTCOMPLEXMODARITHMETIC:
			firstBaseFilter = new filterBasicComplexMod();
			break;

		case FIRSTSPECIALARITHMETIC:
			if (_ratings == NULL)
				throw cannotAccessTablesError(HISTOGRAMTRANSFORMATIONLIB);
			if (*_ratings == NULL)
				throw cannotAccessTablesError(HISTOGRAMTRANSFORMATIONLIB);
			firstBaseFilter = new filterBasicSpecial((*ratings)->getCodingTableMaximumClassification());
			break;

		default:
			firstBaseFilter = new filterBasicNeutral();
			break;

	}

	switch(_secondFilterArithmetic) {

		case SECONDSIMPLEARITHMETIC:
			secondBaseFilter = new filterBasicSimple();
			break;

		case SECONDSIMPLEMODARITHMETIC:
			secondBaseFilter = new filterBasicSimpleMod();
			break;

		case SECONDCOMPLEXARITHMETIC:
			secondBaseFilter = new filterBasicComplex();
			break;

		case SECONDCOMPLEXMODARITHMETIC:
			secondBaseFilter = new filterBasicComplexMod();
			break;

		case SECONDSPECIALARITHMETIC:
			secondBaseFilter = new filterBasicSpecial((*ratings)->getCodingTableMaximumClassification());
			break;

		default:
			secondBaseFilter = new filterBasicNeutral();
			break;

	}

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

autoFinder::~autoFinder() {

	geometry.reset();

	deleteMemory();

	if (firstBaseFilter != NULL) {
		delete firstBaseFilter;
		firstBaseFilter = NULL;
	}

	if (secondBaseFilter != NULL) {
		delete secondBaseFilter;
		secondBaseFilter = NULL;
	}

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void autoFinder::init(histogramData** _histogram, trackData** _tracks, tables** _ratings, unsigned short _firstFilterArithmetic, bool _useFirstFilterMod, unsigned short _secondFilterArithmetic, bool _useSecondFilterMod) {

	geometry.reset();

	deleteMemory();

	if (firstBaseFilter != NULL) {
		delete firstBaseFilter;
		firstBaseFilter = NULL;
	}

	if (secondBaseFilter != NULL) {
		delete secondBaseFilter;
		secondBaseFilter = NULL;
	}

	histogramTransformation::init(_histogram, _tracks, _ratings);

	this->useFirstFilterMod  = _useFirstFilterMod;
	firstFilterMiddleIndex   = 0;
	this->useSecondFilterMod = _useSecondFilterMod;
	secondFilterMiddleIndex  = 0;

	switch(_firstFilterArithmetic) {

		case FIRSTSIMPLEARITHMETIC:
			firstBaseFilter = new filterBasicSimple();
			break;

		case FIRSTSIMPLEMODARITHMETIC:
			firstBaseFilter = new filterBasicSimpleMod();
			break;

		case FIRSTCOMPLEXARITHMETIC:
			firstBaseFilter = new filterBasicComplex();
			break;

		case FIRSTCOMPLEXMODARITHMETIC:
			firstBaseFilter = new filterBasicComplexMod();
			break;

		case FIRSTSPECIALARITHMETIC:
			if (_ratings == NULL)
				throw cannotAccessTablesError(HISTOGRAMTRANSFORMATIONLIB);
			if (*_ratings == NULL)
				throw cannotAccessTablesError(HISTOGRAMTRANSFORMATIONLIB);
			firstBaseFilter = new filterBasicSpecial((*_ratings)->getCodingTableMaximumClassification());
			break;

		default:
			firstBaseFilter = new filterBasicNeutral();
			break;

	}

	switch(_secondFilterArithmetic) {

		case SECONDSIMPLEARITHMETIC:
			secondBaseFilter = new filterBasicSimple();
			break;

		case SECONDSIMPLEMODARITHMETIC:
			secondBaseFilter = new filterBasicSimpleMod();
			break;

		case SECONDCOMPLEXARITHMETIC:
			secondBaseFilter = new filterBasicComplex();
			break;

		case SECONDCOMPLEXMODARITHMETIC:
			secondBaseFilter = new filterBasicComplexMod();
			break;

		case SECONDSPECIALARITHMETIC:
			secondBaseFilter = new filterBasicSpecial((*_ratings)->getCodingTableMaximumClassification());
			break;

		default:
			secondBaseFilter = new filterBasicNeutral();
			break;

	}

}

/****************************************************************
 * object setup for the peakfinding process						*
 * HINT: No peakfinding would be possible without a setup		*
 * call, if the object initialization is finished				*
 ****************************************************************/

void autoFinder::setup() {

	initMemory();

}

/****************************************************************
 * get percentage of coverage for the peak finding geometry		*
 ****************************************************************/

unsigned short autoFinder::getPeakfindingCoverage() {

	return geometry.getCoverage();

}

/****************************************************************
 * set percentage of coverage for the peak finding geometry		*
 ****************************************************************/

void autoFinder::setPeakfindingCoverage(unsigned short percentageOfCoverage) {

	geometry.setCoverage(percentageOfCoverage);

}

/****************************************************************
 * method returns the peakfinding geometry						*
 ****************************************************************/

peakfindingGeometry autoFinder::getPeakfindingGeometry() {

	return geometry;

}

/****************************************************************
 * set whole other peakfinding geometry							*
 ****************************************************************/

void autoFinder::setPeakfindingGeometry(peakfindingGeometry& actualPeakfindingGeometry) {

	geometry.setGeometry(actualPeakfindingGeometry);

}

/****************************************************************
 * update whole other peakfinding geometry						*
 ****************************************************************/

void autoFinder::updatePeakfindingGeometry(peakfindingGeometry& actualPeakfindingGeometry) {

	specialListMem<peakfindingGeometryElement> temp;

	temp = actualPeakfindingGeometry.getGeometryElements();

	geometry.addGeometryElements(temp);

}

/****************************************************************
 * method reads a file to get the peak finding geometry			*
 ****************************************************************/

void autoFinder::readPeakfindingGeometry(std::string fileName, std::streambuf* terminal) {

	geometry.read(fileName, terminal);

}

/****************************************************************
 * method writes a file representing the peak finding geometry	*
 ****************************************************************/

void autoFinder::writePeakfindingGeometry(std::string fileName, std::streambuf* terminal) {

	geometry.write(fileName, "Automatic generated peak finding geometry", terminal);

}

/****************************************************************
 * This method draws the peakfinding geometry					*
 ****************************************************************/

void autoFinder::drawPeakfindingGeometry() {

	geometry.draw();

}
void autoFinder::drawProjectedPeakfindingGeometry() {

	geometry.drawProjection();

}
void autoFinder::drawCoveredPeakfindingGeometry() {

	geometry.drawCoverage();

}
void autoFinder::drawCoveredProjectedPeakfindingGeometry() {

	geometry.drawCoverageProjection();

}

/****************************************************************
 * This method returns a string representation for the			*
 * peakfinding geometry											*
 ****************************************************************/

std::string autoFinder::peakfindingGeometryToString() {

	return geometry.toString();

}
std::string autoFinder::projectedPeakfindingGeometryToString() {

	return geometry.toProjectedString();

}
std::string autoFinder::coveredPeakfindingGeometryToString() {

	return geometry.toCoveredString();

}
std::string autoFinder::coveredProjectedPeakfindingGeometryToString() {

	return geometry.toCoveredProjectedString();

}
