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
//     - implements methods to analyze hardware prerequisites
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-10-24 16:35:15 $
// $Revision: 1.8 $
//
// *******************************************************************/


#include "../../MiscLIB/include/terminal.h"
#include "../include/analysisError.h"
#include "../include/hardwareAnalysis.h"
#include <stdio.h>


#define min(a, b)  (((a) < (b)) ? (a) : (b)) 


/****************************************************************
 * Default Constructor											*
 ****************************************************************/

hardwareAnalysis::hardwareAnalysis() 
 : numberOfColumns(0),
   numberOfRows(0),
   isTracksPerColumnEnabled(false),
   numberOfColumnAnalysis(0),
   minimalNumberOfTracksPerColumn(NULL),
   averageNumberOfTracksPerColumn(NULL),
   maximalNumberOfTracksPerColumn(NULL),
   numberOfFifoSizesPerColumn(NULL),
   isTracksPerRowEnabled(false),
   numberOfRowAnalysis(0),
   minimalNumberOfTracksPerRow(NULL),
   averageNumberOfTracksPerRow(NULL),
   maximalNumberOfTracksPerRow(NULL),
   numberOfFifoSizesPerRow(NULL),
   isTracksPerLayerEnabled(false),
   numberOfLayerAnalysis(0),
   numberOfTracksInAllLayerEntries(0),
   numberOfTracksPerLayer(NULL),
   numberOfTrackDensitiesPerLayer(NULL),
   isHistogramLayerDistributionEnabled(false),
   numberOfHitReadoutDistributionAnalysis(0),
   numberOfHitReadoutDistributionEntries (0),
   hitReadoutDistribution(NULL)
{
  /*
	numberOfColumns                        = 0;
	numberOfRows                           = 0;
	isTracksPerColumnEnabled               = false;
	numberOfColumnAnalysis                 = 0;
	minimalNumberOfTracksPerColumn         = NULL;
	averageNumberOfTracksPerColumn         = NULL;
	maximalNumberOfTracksPerColumn         = NULL;
	numberOfFifoSizesPerColumn             = NULL;
	isTracksPerRowEnabled                  = false;
	numberOfRowAnalysis                    = 0;
	minimalNumberOfTracksPerRow            = NULL;
	averageNumberOfTracksPerRow            = NULL;
	maximalNumberOfTracksPerRow            = NULL;
	numberOfFifoSizesPerRow                = NULL;
	isTracksPerLayerEnabled                = false;
	numberOfLayerAnalysis                  = 0;
	numberOfTracksInAllLayerEntries        = 0;
	numberOfTracksPerLayer                 = NULL;
	numberOfTrackDensitiesPerLayer         = NULL;
	isHistogramLayerDistributionEnabled    = false;
	numberOfHitReadoutDistributionAnalysis = 0;
	numberOfHitReadoutDistributionEntries  = 0;
	hitReadoutDistribution                 = NULL;
  */
}

/****************************************************************
 * Destructor													*
 ****************************************************************/

hardwareAnalysis::~hardwareAnalysis() {

	if (minimalNumberOfTracksPerColumn != NULL) {
		free(minimalNumberOfTracksPerColumn);
		minimalNumberOfTracksPerColumn = NULL;
	}

	if (averageNumberOfTracksPerColumn != NULL) {
		free(averageNumberOfTracksPerColumn);
		averageNumberOfTracksPerColumn = NULL;
	}

	if (maximalNumberOfTracksPerColumn != NULL) {
		free(maximalNumberOfTracksPerColumn);
		maximalNumberOfTracksPerColumn = NULL;
	}
	if (numberOfFifoSizesPerColumn != NULL) {
		free(numberOfFifoSizesPerColumn);
		numberOfFifoSizesPerColumn = NULL;
	}

	if (minimalNumberOfTracksPerRow != NULL) {
		free(minimalNumberOfTracksPerRow);
		minimalNumberOfTracksPerRow = NULL;
	}

	if (averageNumberOfTracksPerRow != NULL) {
		free(averageNumberOfTracksPerRow);
		averageNumberOfTracksPerRow = NULL;
	}

	if (maximalNumberOfTracksPerRow != NULL) {
		free(maximalNumberOfTracksPerRow);
		maximalNumberOfTracksPerRow = NULL;
	}
	if (numberOfFifoSizesPerRow != NULL) {
		free(numberOfFifoSizesPerRow);
		numberOfFifoSizesPerRow = NULL;
	}

	if (numberOfTracksPerLayer != NULL) {
		free(numberOfTracksPerLayer);
		numberOfTracksPerLayer = NULL;
	}

	if (numberOfTrackDensitiesPerLayer != NULL) {
		free(numberOfTrackDensitiesPerLayer);
		numberOfTrackDensitiesPerLayer = NULL;
	}

	if (hitReadoutDistribution != NULL) {
		free(hitReadoutDistribution);
		hitReadoutDistribution = NULL;
	}

}

/****************************************************************
 * method returns if the analysis for the number of				*
 * tracks in all columns of the histogram layers is				*
 * enabled or not												*
 ****************************************************************/

bool hardwareAnalysis::isNumberOfTracksInAllColumnsAnalysisEnabled() {

	return isTracksPerColumnEnabled;

}

/****************************************************************
 * method returns if the analysis for the number of				*
 * tracks in all rows of the histogram layers is				*
 * enabled or not												*
 ****************************************************************/

bool hardwareAnalysis::isNumberOfTracksInAllRowsAnalysisEnabled() {

	return isTracksPerRowEnabled;

}

/****************************************************************
 * method returns the number of actually made column anaylsis	*
 ****************************************************************/

unsigned int hardwareAnalysis::getNumberOfColumnAnalysis() {

	return numberOfColumnAnalysis;

}

/****************************************************************
 * method returns the number of actually made row anaylsis		*
 ****************************************************************/

unsigned int hardwareAnalysis::getNumberOfRowAnalysis() {

	return numberOfRowAnalysis;

}

/****************************************************************
 * method returns the number of entries in the distribution		*
 * for all number of tracks per column in all layers			*
 ****************************************************************/

unsigned int hardwareAnalysis::getNumberOfTracksInAllColumnEntries() {

	return numberOfColumns;

}

/****************************************************************
 * method returns the number of entries in the distribution		*
 * for all number of tracks per row in all layers				*
 ****************************************************************/

unsigned int hardwareAnalysis::getNumberOfTracksInAllRowEntries() {

	return numberOfRows;

}

/****************************************************************
 * method returns the distribution for the minimal number		*
 * of tracks per column in all layers							*
 ****************************************************************/

unsigned int* hardwareAnalysis::getMinimalNumberOfTracksInAllColumnsDistribution() {

	return minimalNumberOfTracksPerColumn;

}

/****************************************************************
 * method returns the distribution for the minimal number		*
 * of tracks per row in all layers								*
 ****************************************************************/

unsigned int* hardwareAnalysis::getMinimalNumberOfTracksInAllRowsDistribution() {

	return minimalNumberOfTracksPerRow;

}

/****************************************************************
 * method returns the distribution for the average number		*
 * of tracks per column in all layers							*
 ****************************************************************/

double* hardwareAnalysis::getAverageNumberOfTracksInAllColumnsDistribution() {

	return averageNumberOfTracksPerColumn;

}

/****************************************************************
 * method returns the distribution for the average number		*
 * of tracks per row in all layers								*
 ****************************************************************/

double* hardwareAnalysis::getAverageNumberOfTracksInAllRowsDistribution() {

	return averageNumberOfTracksPerRow;

}

/****************************************************************
 * method returns the distribution for the maximal number		*
 * of tracks per column in all layers							*
 ****************************************************************/

unsigned int* hardwareAnalysis::getMaximalNumberOfTracksInAllColumnsDistribution() {

	return maximalNumberOfTracksPerColumn;

}

/****************************************************************
 * method returns the distribution for the maximal number		*
 * of tracks per row in all layers								*
 ****************************************************************/

unsigned int* hardwareAnalysis::getMaximalNumberOfTracksInAllRowsDistribution() {

	return maximalNumberOfTracksPerRow;

}

/****************************************************************
 * method returns the distribution for the fifo sizes for		*
 * the tracks per column in all layers							*
 ****************************************************************/

unsigned int* hardwareAnalysis::getNumberOfFifoSizesInAllColumnsDistribution() {

	return numberOfFifoSizesPerColumn;

}

/****************************************************************
 * method returns the distribution for the fifo sizes for		*
 * the tracks per row in all layers								*
 ****************************************************************/

unsigned int* hardwareAnalysis::getNumberOfFifoSizesInAllRowsDistribution() {

	return numberOfFifoSizesPerRow;

}

/****************************************************************
 * method initializes the hardware histogram dimensions.		*
 ****************************************************************/

void hardwareAnalysis::initHardwareHistogramDimensions(unsigned short _numberOfColumns, unsigned short _numberOfRows) {

	this->numberOfColumns = _numberOfColumns;
	this->numberOfRows    = _numberOfRows;

}

/****************************************************************
 * method initializes the hardwareTracksPerColumn-Analysis.		*
 ****************************************************************/

void hardwareAnalysis::initHardwareTracksPerColumnAnalysis(bool enable) {

	this->numberOfColumnAnalysis = 0;

	if (isTracksPerColumnEnabled) {

		if (minimalNumberOfTracksPerColumn != NULL) {
			free(minimalNumberOfTracksPerColumn);
			minimalNumberOfTracksPerColumn = NULL;
		}

		if (averageNumberOfTracksPerColumn != NULL) {
			free(averageNumberOfTracksPerColumn);
			averageNumberOfTracksPerColumn = NULL;
		}

		if (maximalNumberOfTracksPerColumn != NULL) {
			free(maximalNumberOfTracksPerColumn);
			maximalNumberOfTracksPerColumn = NULL;
		}
		if (numberOfFifoSizesPerColumn != NULL) {
			free(numberOfFifoSizesPerColumn);
			numberOfFifoSizesPerColumn = NULL;
		}

	}

	isTracksPerColumnEnabled = enable;

	if (isTracksPerColumnEnabled) {

		if (this->numberOfColumns == 0)
			throw initHistogramDimensionsFirstError();

		minimalNumberOfTracksPerColumn = (unsigned int*)calloc(this->numberOfColumns, sizeof(unsigned int));
		if (minimalNumberOfTracksPerColumn == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		averageNumberOfTracksPerColumn = (double*)calloc(this->numberOfColumns, sizeof(double));
		if (averageNumberOfTracksPerColumn == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		maximalNumberOfTracksPerColumn = (unsigned int*)calloc(this->numberOfColumns, sizeof(unsigned int));
		if (maximalNumberOfTracksPerColumn == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		numberOfFifoSizesPerColumn = (unsigned int*)calloc(this->numberOfColumns, sizeof(unsigned int));
		if (numberOfFifoSizesPerColumn == NULL)
			throw memoryAllocationError(ANALYSISLIB);

	}

}

/****************************************************************
 * method initializes the hardwareTracksPerRow-Analysis.		*
 ****************************************************************/

void hardwareAnalysis::initHardwareTracksPerRowAnalysis(bool enable) {

	this->numberOfRowAnalysis = 0;

	if (isTracksPerRowEnabled) {

		if (minimalNumberOfTracksPerRow != NULL) {
			free(minimalNumberOfTracksPerRow);
			minimalNumberOfTracksPerRow = NULL;
		}

		if (averageNumberOfTracksPerRow != NULL) {
			free(averageNumberOfTracksPerRow);
			averageNumberOfTracksPerRow = NULL;
		}

		if (maximalNumberOfTracksPerRow != NULL) {
			free(maximalNumberOfTracksPerRow);
			maximalNumberOfTracksPerRow = NULL;
		}
		if (numberOfFifoSizesPerRow != NULL) {
			free(numberOfFifoSizesPerRow);
			numberOfFifoSizesPerRow = NULL;
		}

	}

	isTracksPerRowEnabled = enable;

	if (isTracksPerRowEnabled) {

		if (this->numberOfRows == 0)
			throw initHistogramDimensionsFirstError();

		minimalNumberOfTracksPerRow = (unsigned int*)calloc(this->numberOfRows, sizeof(unsigned int));
		if (minimalNumberOfTracksPerRow == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		averageNumberOfTracksPerRow = (double*)calloc(this->numberOfRows, sizeof(double));
		if (averageNumberOfTracksPerRow == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		maximalNumberOfTracksPerRow = (unsigned int*)calloc(this->numberOfRows, sizeof(unsigned int));
		if (maximalNumberOfTracksPerRow == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		numberOfFifoSizesPerRow = (unsigned int*)calloc(this->numberOfRows, sizeof(unsigned int));
		if (numberOfFifoSizesPerRow == NULL)
			throw memoryAllocationError(ANALYSISLIB);

	}

}

/****************************************************************
 * This method evaluates the minimal, the maximal and			*
 * the average number of tracks in all columns of the histogram.*
 ****************************************************************/

void hardwareAnalysis::evaluateNumberOfTracksInAllColumns(trackData* tracks, std::streambuf* terminal) {

	unsigned int     j;
	unsigned int*    actualNumberOfTracks;
	unsigned int*    averageNumberOfTracks;
	bool             noFifoNeededFlag;
	terminalSequence statusSequence;

	if (tracks == NULL)
		throw cannotAccessTrackDataError(ANALYSISLIB);
	
	if (numberOfColumns > 0) {
		actualNumberOfTracks  = (unsigned int*)calloc(numberOfColumns, sizeof(unsigned int));
		averageNumberOfTracks = (unsigned int*)calloc(numberOfColumns, sizeof(unsigned int));
	}
	else{
		actualNumberOfTracks  = NULL;
		averageNumberOfTracks = NULL;
	}
	if (actualNumberOfTracks == NULL)
		throw memoryAllocationError(ANALYSISLIB);
	if (averageNumberOfTracks == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	createTerminalStatusSequence(&statusSequence, terminal, "Analyse tracks per column:\t\t\t\t", (unsigned int)tracks->getNumberOfLayers());
	terminalInitialize(statusSequence);

	for (unsigned short i = 0; i < tracks->getNumberOfLayers(); i++) {

		for (j = 0; j < numberOfColumns; j++)
			actualNumberOfTracks[j] = 0;

		for (trackLayer::iterator actualTrack = tracks->getTrackIteratorBeginOfHistogramLayer(i); actualTrack != tracks->getTrackIteratorEndOfHistogramLayer(i); actualTrack++) {

			if (actualTrack->position.get(DIM1) < numberOfColumns)
				actualNumberOfTracks[actualTrack->position.get(DIM1)]++;

		}

		for (j = 0; j < numberOfColumns; j++) {

			if (i == 0) {

				averageNumberOfTracks[j]              = actualNumberOfTracks[j];
				
				if (numberOfColumnAnalysis == 0) {

					minimalNumberOfTracksPerColumn[j] = actualNumberOfTracks[j];

					maximalNumberOfTracksPerColumn[j] = actualNumberOfTracks[j];

				}
				else {

					if (minimalNumberOfTracksPerColumn[j] > actualNumberOfTracks[j])
						minimalNumberOfTracksPerColumn[j] = actualNumberOfTracks[j];

					if (maximalNumberOfTracksPerColumn[j] < actualNumberOfTracks[j])
						maximalNumberOfTracksPerColumn[j] = actualNumberOfTracks[j];

				}

			}
			else {

				averageNumberOfTracks[j]              += actualNumberOfTracks[j];

				if (minimalNumberOfTracksPerColumn[j] > actualNumberOfTracks[j])
					minimalNumberOfTracksPerColumn[j] = actualNumberOfTracks[j];

				if (maximalNumberOfTracksPerColumn[j] < actualNumberOfTracks[j])
					maximalNumberOfTracksPerColumn[j] = actualNumberOfTracks[j];

			}

		}

		for (j = 0; j < numberOfColumns; j++)
			actualNumberOfTracks[j] = 0;

		for (unsigned short k = 0; k < numberOfRows; k++) {

			noFifoNeededFlag = true;

			for (trackLayer::iterator actualTrack = tracks->getTrackIteratorBeginOfHistogramLayer(i); actualTrack != tracks->getTrackIteratorEndOfHistogramLayer(i); actualTrack++) {

				if (actualTrack->position.get(DIM2) == k) {

					/* add the tracks to the columns row by row */
					actualNumberOfTracks[actualTrack->position.get(DIM1)]++;

				}

			}

			for (j = 0; j < numberOfColumns; j++) {

				/* remove first occuring track in the columns, because it can be directly proccessed by the separator pipeline */
				if ((noFifoNeededFlag) && (actualNumberOfTracks[j] > 0)) {

					actualNumberOfTracks[j]--;
					noFifoNeededFlag = false;

				}

				if (actualNumberOfTracks[j] > numberOfFifoSizesPerColumn[j])
					numberOfFifoSizesPerColumn[j] = numberOfFifoSizesPerColumn[j];

			}

		}

		terminalOverwriteWithIncrement(statusSequence);

	}

	for (j = 0; j < numberOfColumns; j++) {

		if (numberOfColumnAnalysis == 0)
			averageNumberOfTracksPerColumn[j]  = (double)averageNumberOfTracks[j] / (double)tracks->getNumberOfLayers();
		else
			averageNumberOfTracksPerColumn[j] += (double)averageNumberOfTracks[j] / (double)tracks->getNumberOfLayers();

	}

	terminalFinalize(statusSequence);

	if (actualNumberOfTracks != NULL) {
		free(actualNumberOfTracks);
		actualNumberOfTracks = NULL;
	}
	if (averageNumberOfTracks != NULL) {
		free(averageNumberOfTracks);
		averageNumberOfTracks = NULL;
	}

	numberOfColumnAnalysis++;

}

/****************************************************************
 * This method evaluates the minimal, the maximal and			*
 * the average number of tracks in all rows of the histogram.	*
 ****************************************************************/

void hardwareAnalysis::evaluateNumberOfTracksInAllRows(trackData* tracks, std::streambuf* terminal) {

	unsigned int     j;
	unsigned int*    actualNumberOfTracks;
	unsigned int*    averageNumberOfTracks;
	bool             noFifoNeededFlag;
	terminalSequence statusSequence;

	if (tracks == NULL)
		throw cannotAccessTrackDataError(ANALYSISLIB);
	
	if (numberOfRows > 0) {
		actualNumberOfTracks  = (unsigned int*)calloc(numberOfRows, sizeof(unsigned int));
		averageNumberOfTracks = (unsigned int*)calloc(numberOfRows, sizeof(unsigned int));
	}
	else{
		actualNumberOfTracks  = NULL;
		averageNumberOfTracks = NULL;
	}
	if (actualNumberOfTracks == NULL)
		throw memoryAllocationError(ANALYSISLIB);
	if (averageNumberOfTracks == NULL)
		throw memoryAllocationError(ANALYSISLIB);

	createTerminalStatusSequence(&statusSequence, terminal, "Analyse tracks per row:\t\t\t\t", (unsigned int)tracks->getNumberOfLayers());
	terminalInitialize(statusSequence);

	for (unsigned short i = 0; i < tracks->getNumberOfLayers(); i++) {

		for (j = 0; j < numberOfRows; j++)
			actualNumberOfTracks[j] = 0;

		for (trackLayer::iterator actualTrack = tracks->getTrackIteratorBeginOfHistogramLayer(i); actualTrack != tracks->getTrackIteratorEndOfHistogramLayer(i); actualTrack++) {

			if (actualTrack->position.get(DIM2) < numberOfRows)
				actualNumberOfTracks[actualTrack->position.get(DIM2)]++;

		}

		for (j = 0; j < numberOfRows; j++) {

			if (i == 0) {

				averageNumberOfTracks[j]              = actualNumberOfTracks[j];
				
				if (numberOfRowAnalysis == 0) {

					minimalNumberOfTracksPerRow[j] = actualNumberOfTracks[j];

					maximalNumberOfTracksPerRow[j] = actualNumberOfTracks[j];

				}
				else {

					if (minimalNumberOfTracksPerRow[j] > actualNumberOfTracks[j])
						minimalNumberOfTracksPerRow[j] = actualNumberOfTracks[j];

					if (maximalNumberOfTracksPerRow[j] < actualNumberOfTracks[j])
						maximalNumberOfTracksPerRow[j] = actualNumberOfTracks[j];

				}

			}
			else {

				averageNumberOfTracks[j]              += actualNumberOfTracks[j];

				if (minimalNumberOfTracksPerRow[j] > actualNumberOfTracks[j])
					minimalNumberOfTracksPerRow[j] = actualNumberOfTracks[j];

				if (maximalNumberOfTracksPerRow[j] < actualNumberOfTracks[j])
					maximalNumberOfTracksPerRow[j] = actualNumberOfTracks[j];

			}

		}

		for (j = 0; j < numberOfRows; j++)
			actualNumberOfTracks[j] = 0;

		for (unsigned short k = 0; k < numberOfColumns; k++) {

			noFifoNeededFlag = true;

			for (trackLayer::iterator actualTrack = tracks->getTrackIteratorBeginOfHistogramLayer(i); actualTrack != tracks->getTrackIteratorEndOfHistogramLayer(i); actualTrack++) {

				if (actualTrack->position.get(DIM1) == k) {

					/* add the tracks to the columns row by row */
					actualNumberOfTracks[actualTrack->position.get(DIM2)]++;

				}

			}

			for (j = 0; j < numberOfRows; j++) {

				/* remove first occuring track in the columns, because it can be directly proccessed by the separator pipeline */
				if ((noFifoNeededFlag) && (actualNumberOfTracks[j] > 0)) {

					actualNumberOfTracks[j]--;
					noFifoNeededFlag = false;

				}

				if (actualNumberOfTracks[j] > numberOfFifoSizesPerRow[j])
					numberOfFifoSizesPerRow[j] = numberOfFifoSizesPerRow[j];

			}

		}

		terminalOverwriteWithIncrement(statusSequence);

	}

	for (j = 0; j < numberOfRows; j++) {

		if (numberOfColumnAnalysis == 0)
			averageNumberOfTracksPerRow[j]  = (double)averageNumberOfTracks[j] / (double)tracks->getNumberOfLayers();
		else
			averageNumberOfTracksPerRow[j] += (double)averageNumberOfTracks[j] / (double)tracks->getNumberOfLayers();

	}

	terminalFinalize(statusSequence);

	if (actualNumberOfTracks != NULL) {
		free(actualNumberOfTracks);
		actualNumberOfTracks = NULL;
	}
	if (averageNumberOfTracks != NULL) {
		free(averageNumberOfTracks);
		averageNumberOfTracks = NULL;
	}

	numberOfRowAnalysis++;

}

/****************************************************************
 * method returns if the analysis for the number of				*
 * tracks in all histogram layers is enabled or not				*
 ****************************************************************/

bool hardwareAnalysis::isNumberOfTracksInAllLayersAnalysisEnabled() {

	return isTracksPerLayerEnabled;

}

/****************************************************************
 * method returns the number of actually made column anaylsis	*
 ****************************************************************/

unsigned int hardwareAnalysis::getNumberOfLayerAnalysis() {

	return numberOfLayerAnalysis;

}

/****************************************************************
 * method returns the number of entries in the distribution		*
 * for all number of tracks in layers							*
 ****************************************************************/

unsigned int hardwareAnalysis::getNumberOfTracksInAllLayersEntries() {

	return numberOfTracksInAllLayerEntries;

}

/****************************************************************
 * method returns the distribution for the number				*
 * of tracks per layers											*
 ****************************************************************/

unsigned int* hardwareAnalysis::getNumberOfTracksPerLayerDistribution() {

	return numberOfTracksPerLayer;

}

/****************************************************************
 * method returns the distribution for the number				*
 * of track densities per layer									*
 ****************************************************************/

int* hardwareAnalysis::getNumberOfTrackDensitiesPerLayerDistribution() {

	return numberOfTrackDensitiesPerLayer;

}

/****************************************************************
 * method initializes the hardwareTracksPerLayer-Analysis.		*
 ****************************************************************/

void hardwareAnalysis::initHardwareTracksPerLayerAnalysis(unsigned int _numberOfTracksInAllLayerEntries, bool enable) {

	this->numberOfLayerAnalysis           = 0;
	this->numberOfTracksInAllLayerEntries = 0;

	if (isTracksPerLayerEnabled) {

		if (numberOfTracksPerLayer != NULL) {
			free(numberOfTracksPerLayer);
			numberOfTracksPerLayer = NULL;
		}

		if (numberOfTrackDensitiesPerLayer != NULL) {
			free(numberOfTrackDensitiesPerLayer);
			numberOfTrackDensitiesPerLayer = NULL;
		}

	}

	isTracksPerLayerEnabled = enable;

	if (isTracksPerLayerEnabled) {

		this->numberOfTracksInAllLayerEntries = _numberOfTracksInAllLayerEntries;

		numberOfTracksPerLayer = (unsigned int*)calloc(this->numberOfTracksInAllLayerEntries, sizeof(unsigned int));
		if (numberOfTracksPerLayer == NULL)
			throw memoryAllocationError(ANALYSISLIB);

		numberOfTrackDensitiesPerLayer = (int*)calloc(this->numberOfTracksInAllLayerEntries, sizeof(int));
		if (numberOfTrackDensitiesPerLayer == NULL)
			throw memoryAllocationError(ANALYSISLIB);

	}

}

/****************************************************************
 * This method evaluates the distribution of the number of		*
 * tracks in all layers of the histogram.						*
 ****************************************************************/

void hardwareAnalysis::evaluateNumberOfTracksInAllLayers(trackData* tracks, std::streambuf* terminal) {

	terminalSequence statusSequence;

	if (tracks == NULL)
		throw cannotAccessTrackDataError(ANALYSISLIB);

	createTerminalStatusSequence(&statusSequence, terminal, "Analyse tracks per layer:\t\t\t\t", (unsigned int)min(numberOfTracksInAllLayerEntries, tracks->getNumberOfLayers()));
	terminalInitialize(statusSequence);

	for (unsigned int i = 0; i < min(numberOfTracksInAllLayerEntries, tracks->getNumberOfLayers()); i++) {

		if (numberOfLayerAnalysis == 0)
			numberOfTracksPerLayer[i]          = tracks->getNumberOfTracksOfHistogramLayer(i);
		else
			numberOfTracksPerLayer[i]         += tracks->getNumberOfTracksOfHistogramLayer(i);

		if (i == 0)
			numberOfTrackDensitiesPerLayer[i]  = (int)numberOfTracksPerLayer[i] - 0;
		else
			numberOfTrackDensitiesPerLayer[i]  = (int)numberOfTracksPerLayer[i] - (int)numberOfTracksPerLayer[i - 1];

		terminalOverwriteWithIncrement(statusSequence);

	}

	terminalFinalize(statusSequence);

	numberOfLayerAnalysis++;

}

/****************************************************************
 * method returns if the analysis for the distribution how		*
 * often a hit must be inserted in a layer is enabled or not	*
 ****************************************************************/

bool hardwareAnalysis::isHitReadoutDistributionAnalysisEnabled() {

	return isHistogramLayerDistributionEnabled;

}

/****************************************************************
 * method initializes the										*
 * hardwareHistogramLayerDistributionAnalysis.					*
 ****************************************************************/

void hardwareAnalysis::initHardwareHitReadoutDistributionAnalysis(unsigned int numberOfLayers, bool enable) {

	this->numberOfHitReadoutDistributionAnalysis = 0;

	if (isHistogramLayerDistributionEnabled) {

		if (hitReadoutDistribution != NULL) {
			free(hitReadoutDistribution);
			hitReadoutDistribution = NULL;
		}

	}

	isHistogramLayerDistributionEnabled = enable;

	if (isHistogramLayerDistributionEnabled) {

		this->numberOfHitReadoutDistributionEntries = numberOfLayers;

		hitReadoutDistribution = (unsigned int*)calloc(this->numberOfHitReadoutDistributionEntries, sizeof(unsigned int));
		if (hitReadoutDistribution == NULL)
			throw memoryAllocationError(ANALYSISLIB);

	}

}

/****************************************************************
 * This method evaluates the distribution how often a hit must	*
 * be inserted in a layer.										*
 ****************************************************************/

void hardwareAnalysis::evaluateHitReadoutDistribution(histogramData* histogram, std::streambuf* terminal) {

	terminalSequence statusSequence;

	if (histogram == NULL)
		throw cannotAccessHistogramDataError(ANALYSISLIB);

	createTerminalStatusSequence(&statusSequence, terminal, "Analyse hit readout:\t\t\t\t", (unsigned int)min(numberOfHitReadoutDistributionEntries, histogram->getValueDim3()));
	terminalInitialize(statusSequence);

	for (unsigned short i = 0; i < min(numberOfHitReadoutDistributionEntries, histogram->getValueDim3()); i++) {

		for (std::vector<lutBorder>::iterator j = histogram->getBorderBeginIterator(i); j != histogram->getBorderEndIterator(i); j++) {

			hitReadoutDistribution[j->getPrelutHoughBorder().stop - i]++;

		}

		terminalOverwriteWithIncrement(statusSequence);

	}

	terminalFinalize(statusSequence);

	numberOfHitReadoutDistributionAnalysis++;

}

/****************************************************************
 * method returns the number of actually made hit readout		*
 * distribution anaylsis										*
 ****************************************************************/

unsigned int hardwareAnalysis::getNumberOfHitReadoutDistributionAnalysis() {

	return numberOfHitReadoutDistributionAnalysis;

}

/****************************************************************
 * method returns the number of entries in the hit readout		*
 * distribution													*
 ****************************************************************/

unsigned int hardwareAnalysis::getNumberOfHitReadoutDistributionEntries() {

	return numberOfHitReadoutDistributionEntries;

}

/****************************************************************
 * method returns the distribution for the hit readout. This	*
 * tells how often the hits must be preccessed for the whole	*
 * histogramming.												*
 ****************************************************************/

unsigned int* hardwareAnalysis::getHitReadoutDistribution() {

	return hitReadoutDistribution;

}
