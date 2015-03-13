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
// $Revision: 1.6 $
//
// *******************************************************************/


#include "../../MiscLIB/include/terminal.h"
#include "../include/histogramTransformationError.h"
#include "../include/eraser.h"


/****************************************************************
 * This method filters the peakclasses in a speciallized manner.*
 ****************************************************************/

void eraser::filteringHistogramLayer() {

	histogramCell* actualCell;
	bitArray       comparator;

	if (histogram == NULL)
		throw cannotAccessHistogramError(HISTOGRAMTRANSFORMATIONLIB);
	if (*histogram == NULL)
		throw cannotAccessHistogramError(HISTOGRAMTRANSFORMATIONLIB);
	if (ratings == NULL)
		throw cannotAccessTablesError(HISTOGRAMTRANSFORMATIONLIB);
	if (*ratings == NULL)
		throw cannotAccessTablesError(HISTOGRAMTRANSFORMATIONLIB);

	for (unsigned short i = (unsigned short)(*ratings)->getCodingTableMaximumClassification().toULong() + DIAGONALIZATIONSUMMAND; i >= (unsigned short)(*ratings)->getCodingTableMinimumClassification().toULong(); i--) {

		for(unsigned short j = 0; j < (*histogram)->getValueDim2(); j++) {

			for(unsigned short k = 0; k < (*histogram)->getValueDim1(); k++) {

				actualCell = (*histogram)->getCell(k, j);

				comparator = bitArray(i);

				if (actualCell->value == comparator) {

					clearPeakNeighborhood(k, j);
					actualCell->value = (*ratings)->getCodingTableMaximumClassification();

				}

			}

		}

	}

}

/****************************************************************
 * This method filters the tracks in a speciallized manner		*
 * again.														*
 ****************************************************************/

void eraser::filteringHistogram(std::streambuf* terminal) {

	unsigned short    maxClass;
	unsigned short    minClass;
	unsigned short    k;
	terminalSequence  statusSequence;
	trackLayerAccess* trackAccess;
	bitArray          comparator;
	int               elementCoordinate;
	int               filterCoordinate;
	int               filterHelpCoordinate;
	bool              dim1Reset;
	bool              dim2Reset;
	bool              dim3Reset;
	bool              firstValueIsRemoved;

	if (ratings == NULL)
		throw cannotAccessTablesError(HISTOGRAMTRANSFORMATIONLIB);
	if (*ratings == NULL)
		throw cannotAccessTablesError(HISTOGRAMTRANSFORMATIONLIB);

	trackAccess = new trackLayerAccess[2 * secondFilterNeighborhoodDim3ClearRadius + 1];

	if (trackAccess == NULL)
		throw memoryAllocationError(HISTOGRAMTRANSFORMATIONLIB);

	maxClass = (unsigned short)(*ratings)->getCodingTableMaximumClassification().toULong();
	minClass = (unsigned short)(*ratings)->getCodingTableMinimumClassification().toULong();

	createTerminalStatusSequence(&statusSequence, terminal, "Process histogram:\t\t\t\t", (unsigned int)((maxClass + DIAGONALIZATIONSUMMAND - minClass + 1) * (*tracks)->getNumberOfLayers()));
	terminalInitialize(statusSequence);

	for (unsigned short i = maxClass + DIAGONALIZATIONSUMMAND; i >= minClass; i--) {

		for (unsigned short j = 0; j < (*tracks)->getNumberOfLayers(); j++) {

			for (k = 0; k <= 2 * secondFilterNeighborhoodDim3ClearRadius; k++) {

				if (((int)j - (int)secondFilterNeighborhoodDim3ClearRadius + (int)k >= (int)0) && ((int)j - (int)secondFilterNeighborhoodDim3ClearRadius + (int)k < (int)(*tracks)->getNumberOfLayers())) {
					trackAccess[k].begin   = (*tracks)->getTrackIteratorBeginOfHistogramLayer(j - secondFilterNeighborhoodDim3ClearRadius + k);
					trackAccess[k].end     = (*tracks)->getTrackIteratorEndOfHistogramLayer(j - secondFilterNeighborhoodDim3ClearRadius + k);
					trackAccess[k].isValid = true;
				}
				else
					trackAccess[k].isValid = false;

			}

			for (trackLayer::iterator element = trackAccess[secondFilterNeighborhoodDim3ClearRadius].begin; element != trackAccess[secondFilterNeighborhoodDim3ClearRadius].end; element++) {

				comparator = bitArray(i);

				if (element->value != comparator)
					continue;

				for (k = 0; k <= 2 * secondFilterNeighborhoodDim3ClearRadius; k++) {

					if ((k == secondFilterNeighborhoodDim3ClearRadius) || (!trackAccess[k].isValid))
						continue;

					for (trackLayer::iterator filter = trackAccess[k].begin; filter != trackAccess[k].end;) {

						elementCoordinate = element->position.get(DIM1);
						filterCoordinate  = filter->position.get(DIM1);
						if ((filterCoordinate >= elementCoordinate - firstFilterNeighborhoodDim1ClearRadius) && (filterCoordinate <= elementCoordinate + firstFilterNeighborhoodDim1ClearRadius))
							dim1Reset = true;
						else
							dim1Reset = false;

						if (!dim1Reset) {
							filter++;
							continue;
						}

						elementCoordinate = element->position.get(DIM2);
						filterCoordinate  = filter->position.get(DIM2);
						if ((filterCoordinate >= elementCoordinate - firstFilterNeighborhoodDim2ClearRadius) && (filterCoordinate <= elementCoordinate + firstFilterNeighborhoodDim2ClearRadius))
							dim2Reset = true;
						else
							dim2Reset = false;

						if (!dim2Reset) {
							filter++;
							continue;
						}

						elementCoordinate    = element->position.get(DIM1);
						filterCoordinate     = filter->position.get(DIM3);
						filterHelpCoordinate = filter->position.get(DIM1);
						if ((filterCoordinate >= j - secondFilterNeighborhoodDim3ClearRadius + abs(filterHelpCoordinate - elementCoordinate)) && (filterCoordinate <= j + secondFilterNeighborhoodDim3ClearRadius - abs(filterHelpCoordinate - elementCoordinate)))
							dim3Reset = true;
						else
							dim3Reset = false;
	
						if (dim1Reset && dim2Reset && dim3Reset) {
							if (filter == trackAccess[k].begin)
								firstValueIsRemoved = true;
							else
								firstValueIsRemoved = false;
							filter = (*tracks)->removeTrack(filter, j - secondFilterNeighborhoodDim3ClearRadius + k);
							if (firstValueIsRemoved)
								trackAccess[k].begin = filter;
						}
						else
							filter++;

					}

				}

			}

			terminalOverwriteWithIncrement(statusSequence);

		}

	}

	terminalFinalize(statusSequence);

	if (trackAccess != NULL) {
		delete trackAccess;
		trackAccess = NULL;
	}

}

/****************************************************************
 * This method clears the histogramsPeaks data in a specific	*
 * area.														*
 ****************************************************************/

void eraser::clearPeakNeighborhood(unsigned short dim1, unsigned short dim2) {

	if (histogram == NULL)
		throw cannotAccessHistogramError(HISTOGRAMTRANSFORMATIONLIB);
	if (*histogram == NULL)
		throw cannotAccessHistogramError(HISTOGRAMTRANSFORMATIONLIB);

	for(int i = dim2 - firstFilterNeighborhoodDim2ClearRadius; i <= dim2 + firstFilterNeighborhoodDim2ClearRadius; i++) {

		if ((i >= 0) && (i < (*histogram)->getValueDim2())) {

			for(int j = dim1 - firstFilterNeighborhoodDim1ClearRadius; j <= dim1 + firstFilterNeighborhoodDim1ClearRadius; j++) {

				if ((j >= 0) && (j < (*histogram)->getValueDim1())) {

					if (abs(i - dim2) <= secondFilterNeighborhoodDim3ClearRadius)
						(*histogram)->getCell(j, i)->value = bitArray(0);

				}

			}

		}

	}

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

eraser::eraser() 
  : histogramTransformation(),
    firstFilterNeighborhoodDim1ClearRadius(0),
    firstFilterNeighborhoodDim2ClearRadius(0),
    secondFilterNeighborhoodDim3ClearRadius(0)
{

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

eraser::eraser(histogramData** _histogram, trackData** _tracks, tables** _ratings, unsigned short dim1ClearRadius, unsigned short dim2ClearRadius, unsigned short dim3ClearRadius) 
  : histogramTransformation(_histogram, _tracks, _ratings),
    firstFilterNeighborhoodDim1ClearRadius(dim1ClearRadius),
    firstFilterNeighborhoodDim2ClearRadius(dim2ClearRadius),
    secondFilterNeighborhoodDim3ClearRadius(dim3ClearRadius)
{
  /*
	firstFilterNeighborhoodDim1ClearRadius  = dim1ClearRadius;
	firstFilterNeighborhoodDim2ClearRadius  = dim2ClearRadius;
	secondFilterNeighborhoodDim3ClearRadius = dim3ClearRadius;
  */
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

eraser::~eraser() {

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void eraser::init(histogramData** _histogram, trackData** _tracks, tables** _ratings, unsigned short dim1ClearRadius, unsigned short dim2ClearRadius, unsigned short dim3ClearRadius) {

	histogramTransformation::init(_histogram, _tracks, _ratings);

	firstFilterNeighborhoodDim1ClearRadius  = dim1ClearRadius;
	firstFilterNeighborhoodDim2ClearRadius  = dim2ClearRadius;
	secondFilterNeighborhoodDim3ClearRadius = dim3ClearRadius;

}
