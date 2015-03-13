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
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:35:35 $
// $Revision: 1.8 $
//
// *******************************************************************/



#include "../../MiscLIB/include/defs.h"
#include "../../MiscLIB/include/conversionRoutines.h"
#include "../../MiscLIB/include/bitArray.h"
#include "../include/histogramTransformationError.h"
#include "../include/histogramTransformation.h"
#include <stdio.h>
#ifdef DEBUGJUSTONEGOODTRACK
#include <iostream>
#endif


/****************************************************************
 * Default constructor											*
 ****************************************************************/

histogramTransformation::histogramTransformation() 
  : histogram(NULL),
    tracks(NULL),	
    ratings(NULL)
{

  //	init(NULL, NULL, NULL);

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

histogramTransformation::histogramTransformation(histogramData** _histogram, trackData** _tracks, tables** _ratings) 
  : histogram(_histogram),
    tracks(_tracks),	
    ratings(_ratings)
{

  //	init(_histogram, _tracks, _ratings);

}

/****************************************************************
 * Default destructor											*
 ****************************************************************/

histogramTransformation::~histogramTransformation() {

}

/****************************************************************
 * This method initializes the object.							*
 ****************************************************************/

void histogramTransformation::init(histogramData** _histogram, trackData** _tracks, tables** _ratings) {

	this->histogram = _histogram;
	this->tracks    = _tracks;
	this->ratings   = _ratings;

}

/****************************************************************
 * This method resets the tracks object.						*
 ****************************************************************/

void histogramTransformation::resetTracks() {

	if (tracks == NULL)
		throw cannotAccessTracksError();
	if (*tracks == NULL)
		throw cannotAccessTracksError();

	(*tracks)->reset();

}

/****************************************************************
 * This method encodes the histogram value to a special class	*
 * value.														*
 ****************************************************************/

void histogramTransformation::encodeHistogramLayer(unsigned short layer) {

	histogramCell* actualCell;
	bitArray       zeroComparator;
	bitArray       comparator;

#ifdef PRINTORIGINALHISTOGRAMLAYERSTOFILE

	std::string    origialFile;
	std::string    originalId;

#endif

#ifdef PRINTENCODEDHISTOGRAMLAYERSTOFILE

	std::string    encodedFile;
	std::string    encodedId;

#endif

	if (histogram == NULL)
		throw cannotAccessHistogramError();
	if (*histogram == NULL)
		throw cannotAccessHistogramError();
	if (ratings == NULL)
		throw cannotAccessTablesError(HISTOGRAMTRANSFORMATIONLIB);
	if (*ratings == NULL)
		throw cannotAccessTablesError(HISTOGRAMTRANSFORMATIONLIB);

#ifdef PRINTORIGINALHISTOGRAMLAYERSTOFILE

	ustos(layer, &originalId, 10, 0);
	origialFile = "originalHistogramLayer" + originalId + ".txt";
	(*histogram)->printLayer(maxClass, 0, (*histogram)->getValueDim1(), 0, (*histogram)->getValueDim2(), 0, (*histogram)->getValueDimSH(), false, (char*)origialFile.c_str());

#endif

	zeroComparator = bitArray(0);
	comparator     = (*ratings)->getCodingTable().getMinimumClassification();

	for(unsigned short i = 0; i < (*histogram)->getValueDim2(); i++) {

		for(unsigned short j = 0; j < (*histogram)->getValueDim1(); j++) {

			actualCell = (*histogram)->getCell(j, i);

			if (actualCell->value > zeroComparator) {

#ifdef SKIPFIRSTHISTOGRAMCOLUMN

				if (j == 0)
					actualCell->value = zeroComparator;
				else {

#endif

					if (actualCell->value >= comparator)
						actualCell->value = (*ratings)->getCodingTableClassification(actualCell->value);
					else
						actualCell->value = zeroComparator;

#ifdef SKIPFIRSTHISTOGRAMCOLUMN

				}

#endif

			}

		}

	}

#ifdef PRINTENCODEDHISTOGRAMLAYERSTOFILE

	ustos(layer, &encodedId, 10, 0);
	encodedFile = "encodedHistogramLayer" + encodedId + ".txt";
	(*histogram)->printLayer(maxClass, 0, (*histogram)->getValueDim1(), 0, (*histogram)->getValueDim2(), 0, (*histogram)->getValueDimSH(), false, (char*)encodedFile.c_str());

#endif

}

/****************************************************************
 * This method handles the diagonles in the histogramPeaks		*
 * array to	perform a better quality with doubled peaks.		*
 ****************************************************************/

void histogramTransformation::diagonalizeHistogramLayer(unsigned short layer) {

	histogramCell* actualCell;
	bitArray       comparator;
	histogramCell* nextCell;

#ifdef PRINTDIAGONALIZEDHISTOGRAMLAYERSTOFILE

	std::string    file;
	std::string    id;

#endif

	if (histogram == NULL)
		throw cannotAccessHistogramError();
	if (*histogram == NULL)
		throw cannotAccessHistogramError();

	comparator = bitArray(0);

	for (unsigned short i = 0; i < (*histogram)->getValueDim2()-1; i++) {

		for (unsigned short j = 0; j < (*histogram)->getValueDim1()-1; j++) {

			actualCell = (*histogram)->getCell(j, i);
			nextCell   = (*histogram)->getCell(j + 1, i + 1);

			if ((actualCell->value > comparator) && (actualCell->value == nextCell->value))
				//actualCell->value += DIAGONALIZATIONSUMMAND;
				nextCell->value += DIAGONALIZATIONSUMMAND;

		}

	}

#ifdef PRINTDIAGONALIZEDHISTOGRAMLAYERSTOFILE

	ustos(layer, &id, 10, 0);
	file = "diagonalizedHistogramLayer" + id + ".txt";
	(*histogram)->printLayer(maxClass, 0, (*histogram)->getValueDim1(), 0, (*histogram)->getValueDim2(), 0, (*histogram)->getValueDimSH(), false, (char*)file.c_str());

#endif

}

/****************************************************************
 * This method filters the peakclasses in a speciallized manner.*
 ****************************************************************/

void histogramTransformation::filterHistogramLayer(unsigned short layer) {

#ifdef PRINTFILTEREDHISTOGRAMLAYERSTOFILE

	std::string    file;
	std::string    id;

#endif

	if (histogram == NULL)
		throw cannotAccessHistogramError();
	if (*histogram == NULL)
		throw cannotAccessHistogramError();

	filteringHistogramLayer();

#ifdef PRINTFILTEREDHISTOGRAMLAYERSTOFILE

	ustos(layer, &id, 10, 0);
	file = "filteredHistogramLayer" + id + ".txt";
	(*histogram)->printLayer(maxClass, 0, (*histogram)->getValueDim1(), 0, (*histogram)->getValueDim2(), 0, (*histogram)->getValueDimSH(), false, (char*)file.c_str());

#endif

}

/****************************************************************
 * This method converts the filtered histogramPeaks array to	*
 * the correspondance to be equal to the eraser-object result.	*
 ****************************************************************/

void histogramTransformation::serializeHistogramLayer(unsigned short layer) {

	histogramCell* actualCell;
	bitArray       comparator;

#ifdef PRINTFINALIZEDHISTOGRAMLAYERSTOFILE

	std::string    file;
	std::string    id;

#endif
	if (ratings == NULL)
		throw cannotAccessTablesError(HISTOGRAMTRANSFORMATIONLIB);
	if (*ratings == NULL)
		throw cannotAccessTablesError(HISTOGRAMTRANSFORMATIONLIB);

	if (histogram == NULL)
		throw cannotAccessHistogramError();
	if (*histogram == NULL)
		throw cannotAccessHistogramError();

	if (tracks == NULL)
		throw cannotAccessHistogramError();
	if (*tracks == NULL)
		throw cannotAccessHistogramError();

	comparator = bitArray(0);

	for (unsigned short i = 0; i < (*histogram)->getValueDim2(); i++) {

		for (unsigned short j = 0; j < (*histogram)->getValueDim1(); j++) {

			actualCell = (*histogram)->getCell(j, i);

			if (actualCell->value > comparator) {

				(*tracks)->addTrack(j, i, layer, *actualCell);

			}

		}

	}

#ifdef PRINTFINALIZEDHISTOGRAMLAYERSTOFILE

	ustos(layer, &id, 10, 0);
	file = "finalizedHistogramLayer" + id + ".txt";
	(*histogram)->printLayer(maxClass, 0, (*histogram)->getValueDim1(), 0, (*histogram)->getValueDim2(), 0, (*histogram)->getValueDimSH(), false, (char*)file.c_str());

#endif

}

/****************************************************************
 * This method filters the tracks in a speciallized manner		*
 * again.														*
 ****************************************************************/

void histogramTransformation::filterHistogram(std::streambuf* terminal) {

#ifdef DEBUGJUSTONEGOODTRACK

	trackDigitalInformation actualTrack;
	trackAnalogInformation  actualParameter;
	trackMomentum           actualMomentum;
	double                  charge;
	analyticFormula         formula;

#endif

	if (tracks == NULL)
		throw cannotAccessHistogramError();
	if (*tracks == NULL)
		throw cannotAccessHistogramError();

#ifdef PRINTPREFILTEREDTRACKLAYERSTOFILE

	(*tracks)->printTracks(maxClass, 0, (*tracks)->getNumberOfLayers(), 0, (*histogram)->getValueDimSH(), false, "prefilteredTracks.txt");

#endif

	filteringHistogram(terminal);

#ifdef DEBUGJUSTONEGOODTRACK

	if ((*tracks)->getNumberOfTracks() > 0)
		std::cout << std::endl << "Position where the peak is found (dim1, dim2 , dim3; value; px ,py, pz; charge):" << std::endl;

	for (unsigned short i = 0; i < (*tracks)->getNumberOfTracks(); i++) {

		(*tracks)->getNextTrackDigitalInfo(&actualTrack);
		(*tracks)->getTrackAnalogInfoFromTrackDigitalInfo(actualTrack, &actualParameter);
		formula.evaluatePWithCare(actualParameter.position, (*tracks)->getHistogramSpace().getIncr(HRADIUS) / 2, &actualMomentum, &charge);
		std::cout << " (" << actualTrack.position.get(DIM1) << ", " << actualTrack.position.get(DIM2) << ", " << actualTrack.position.get(DIM3) << "; " << actualTrack.value.toString(2) << "; ";
		std::cout << actualMomentum.get(PX) << ", " << actualMomentum.get(PY) << ", " << actualMomentum.get(PZ) << "; " << charge << ")" << std::endl;

	}

	if ((*tracks)->getNumberOfTracks() > 0)
		std::cout << std::endl;

#endif

#ifdef PRINTFILTEREDTRACKLAYERSTOFILE

	(*tracks)->printTracks(maxClass, 0, (*tracks)->getNumberOfLayers(), 0, (*histogram)->getValueDimSH(), false, "filteredTracks.txt");

#endif

}
