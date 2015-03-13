/* *******************************************************************
// (C)opyright 2004
// 
// Institute of Computer Science V
// Prof. M�ner
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
//     - class implements the trackfinding algorithm
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-10-24 16:43:10 $
// $Revision: 1.13 $
//
// *******************************************************************/


#include "../../HistogramTransformationLIB/include/filterDef.h"
#include "../../MiscLIB/include/terminal.h"
#include "../include/trackfinderError.h"
#include "../include/trackfinder.h"
#include <stdio.h>


#define max(a, b)  (((a) > (b)) ? (a) : (b))


/****************************************************************
 * This applies the algorithm to evaluate the filter geometry	*
 ****************************************************************/

void trackfinder::evaluatePeakfindingGeometry(std::streambuf* terminal) {

#ifndef NOANALYSIS

	unsigned short           numberOfSecuredSourceData;
	double                   securedSourceDataIncrease;
	std::list<findableTrack> findableTracks;
	terminalSequence         statusSequence;
	unsigned short           trackIndex;
	trackfinderInputTrack*   track;

	if (houghTransform == NULL)
		throw cannotAccessHoughTransformError();
	if (histoTransform == NULL)
		throw cannotAccessHistogramTransformError();
	if (analyser == NULL)
		throw cannotAccessAnalyserError();

	if (isAutomaticFilterGeometryEnabled() && analyser->isAutomaticFilterGeometryAnalysisEnabled()) {

		analyser->setupFindableComputedTracks(&findableTracks, terminal);

		if (sourceDataPercentage < 100) {
			numberOfSecuredSourceData = (sourceDataPercentage * findableTracks.size()) / 100;
			securedSourceDataIncrease = (double)100 / (double)sourceDataPercentage;
		}
		else {
			numberOfSecuredSourceData = findableTracks.size();
			securedSourceDataIncrease = 1;
		}

		createTerminalStatusSequence(&statusSequence, terminal, "Generate filter geometry:\t\t\t", (unsigned int)(numberOfSecuredSourceData * houghTransform->getNumberOfHistogramLayers()), (unsigned int)houghTransform->getNumberOfHistogramLayers());
		terminalInitialize(statusSequence);

		analyser->resetPeakfindingGeometryElements();

		trackIndex = 0;
		for (std::list<findableTrack>::iterator actualTrack = findableTracks.begin(); actualTrack != findableTracks.end();  trackIndex++) {

			track = actualTrack->getTrack();
			if (track == NULL)
				throw cannotAccessHitsOrTracksError(ANALYSISLIB);

			houghTransform->resetHistogram();

			houghTransform->createBorders(track);

			analyser->resetPeakfindingLayerGeometryElements();

			for (unsigned short i = 0; i < houghTransform->getNumberOfHistogramLayers(); i++) {

				houghTransform->createHistogramLayer(i);

				histoTransform->encodeHistogramLayer(i);

				histoTransform->diagonalizeHistogramLayer(i);

				analyser->addPeakfindingLayerGeometry(i);

				houghTransform->resetHistogramLayer();

				terminalOverwriteWithIncrement(statusSequence);

			}

			analyser->updatePeakfindingGeometry(actualTrack->getPosition());

			for (unsigned short j = 0; j < max((unsigned short)(securedSourceDataIncrease * (double)(trackIndex + 1)) - (unsigned short)(securedSourceDataIncrease * (double)trackIndex), 1); j++) {

				if (actualTrack != findableTracks.end())
					actualTrack++;

			}

			if (trackIndex == numberOfSecuredSourceData - 1)
				actualTrack = findableTracks.end();

		}

		terminalFinalize(statusSequence);

	}
	else
		throw featureNotEnabledInAnalyserError();

#else

	throw impossibleFeatureWithoutAnalyserError();

#endif

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackfinder::trackfinder() 
  : houghTransform(NULL),
    histoTransform(NULL),
    filterType(NOFILTER),
    sourceDataPercentage(100),
#ifndef NOANALYSIS
    filterFileName(),
    analyser(NULL)
#else
    filterFileName()
#endif

{
  /*

	houghTransform       = NULL;
	histoTransform       = NULL;
	filterType           = NOFILTER;
	sourceDataPercentage = 100;
	filterFileName.clear();

#ifndef NOANALYSIS

	analyser             = NULL;

#endif
  */
}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackfinder::trackfinder( trackfinderInputData** eventData, histogramData** histogram,
						  trackData** tracks, tables** ratings, lutImplementation** lut,
						  unsigned short _filterType, unsigned short _sourceDataPercentage,
						  std::string _filterFileName,
						  unsigned short firstFilterGeometry, unsigned short firstFilterArithmetic,
						  unsigned short secondFilterGeometry, unsigned short secondFilterArithmetic,
						  unsigned short firstFilterDim1ClearRadius,
						  unsigned short firstFilterDim2ClearRadius,
						  unsigned short secondFilterDim1ClearRadius,
						  unsigned short secondFilterDim2ClearRadius,
						  unsigned short secondFilterDim3ClearRadius) 
  : houghTransform(new houghTransformation(eventData, histogram, lut)),
    histoTransform(NULL),
    filterType(_filterType),
    sourceDataPercentage(_sourceDataPercentage),
#ifndef NOANALYSIS
    filterFileName(_filterFileName),
    analyser(NULL)
#else
    filterFileName(_filterFileName)
#endif
{

  //	houghTransform         = new houghTransformation(eventData, histogram, lut);

	switch(_filterType) {

		case NOFILTER:
			histoTransform = new maxMorphSearch(histogram, tracks, ratings, NOFIRSTARITHMETIC, firstFilterArithmetic, NOSECONDGEOMETRY, secondFilterArithmetic, firstFilterDim1ClearRadius, firstFilterDim2ClearRadius, secondFilterDim1ClearRadius, secondFilterDim2ClearRadius, secondFilterDim3ClearRadius);
			break;
		case MAXMORPHSEARCHFILTER:
			histoTransform = new maxMorphSearch(histogram, tracks, ratings, firstFilterGeometry, firstFilterArithmetic, secondFilterGeometry, secondFilterArithmetic, firstFilterDim1ClearRadius, firstFilterDim2ClearRadius, secondFilterDim1ClearRadius, secondFilterDim2ClearRadius, secondFilterDim3ClearRadius);
			break;
		case ERASERFILTER:
			histoTransform = new eraser(histogram, tracks, ratings, firstFilterDim1ClearRadius, firstFilterDim2ClearRadius, secondFilterDim3ClearRadius);
			break;
		default:
			histoTransform = new autoFinder(histogram, tracks, ratings, firstFilterArithmetic, (firstFilterGeometry == FIRSTFINALMODGEOMETRY), secondFilterArithmetic, (secondFilterGeometry == SECONDFINALMODGEOMETRY));
			break;

	}

	//	this->filterType       = _filterType;
	if (_sourceDataPercentage < 100)
		this->sourceDataPercentage = _sourceDataPercentage;
	else
		this->sourceDataPercentage = 100;
	//	this->filterFileName   = _filterFileName;

#ifndef NOANALYSIS

	//	analyser               = NULL;

#endif

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackfinder::~trackfinder() {

	if(histoTransform != NULL) {

		delete histoTransform;
		histoTransform = NULL;
	}

	if(houghTransform != NULL) {

		delete houghTransform;
		houghTransform = NULL;
	}

}

/****************************************************************
 * Method inits the variables based on the detector stations.	*
 ****************************************************************/

void trackfinder::init(trackfinderInputData** eventData, histogramData** histogram,
					   trackData** tracks, tables** ratings, lutImplementation** lut,
					   unsigned short _filterType, unsigned short _sourceDataPercentage,
					   std::string _filterFileName,
					   unsigned short firstFilterGeometry, unsigned short firstFilterArithmetic,
					   unsigned short secondFilterGeometry, unsigned short secondFilterArithmetic,
					   unsigned short firstFilterDim1ClearRadius,
					   unsigned short firstFilterDim2ClearRadius,
					   unsigned short secondFilterDim1ClearRadius,
					   unsigned short secondFilterDim2ClearRadius,
					   unsigned short secondFilterDim3ClearRadius) {

	if(houghTransform != NULL) {

		delete houghTransform;
		houghTransform = NULL;
	}

	if(histoTransform != NULL) {

		delete histoTransform;
		histoTransform = NULL;
	}

	houghTransform->init(eventData, histogram, lut);

	switch(_filterType) {

		case NOFILTER:
			histoTransform = new maxMorphSearch(histogram, tracks, ratings, NOFIRSTGEOMETRY, firstFilterArithmetic, NOSECONDGEOMETRY, secondFilterArithmetic, firstFilterDim1ClearRadius, firstFilterDim2ClearRadius, secondFilterDim1ClearRadius, secondFilterDim2ClearRadius, secondFilterDim3ClearRadius);
			break;
		case MAXMORPHSEARCHFILTER:
			histoTransform = new maxMorphSearch(histogram, tracks, ratings, firstFilterGeometry, firstFilterArithmetic, secondFilterGeometry, secondFilterArithmetic, firstFilterDim1ClearRadius, firstFilterDim2ClearRadius, secondFilterDim1ClearRadius, secondFilterDim2ClearRadius, secondFilterDim3ClearRadius);
			break;
		case ERASERFILTER:
			histoTransform = new eraser(histogram, tracks, ratings, firstFilterDim1ClearRadius, firstFilterDim2ClearRadius, secondFilterDim3ClearRadius);
			break;
		default:
			histoTransform = new autoFinder(histogram, tracks, ratings, firstFilterArithmetic, (firstFilterGeometry == FIRSTFINALMODGEOMETRY), secondFilterArithmetic, (secondFilterGeometry == SECONDFINALMODGEOMETRY));
			break;

	}

	this->filterType       = _filterType;
	if (_sourceDataPercentage < 100)
		this->sourceDataPercentage = _sourceDataPercentage;
	else
		this->sourceDataPercentage = 100;
	this->filterFileName   = _filterFileName;

}

/****************************************************************
 * This method sets the object for analysing the trackfinding	*
 * algorithm													*
 ****************************************************************/

void trackfinder::setAnalyser(analysis* _analyser) {

#ifndef NOANALYSIS

	this->analyser = _analyser;

#endif

}

/****************************************************************
 * This applies the track-finding algorithm						*
 ****************************************************************/

void trackfinder::evaluate(std::streambuf* terminal) {

	terminalSequence statusSequence;

	if (houghTransform == NULL)
		throw cannotAccessHoughTransformError();
	if (histoTransform == NULL)
		throw cannotAccessHistogramTransformError();

	houghTransform->resetHistogram();
	histoTransform->resetTracks();


#ifndef NOANALYSIS

	if (analyser != NULL) {
		if (analyser->isTimeAnalysisEnabled()) {
			analyser->borderCreationTimerReset();
			analyser->histogramCreationTimerReset();
			analyser->histogramEncodingTimerReset();
			analyser->histogramDiagonalizingTimerReset();
			analyser->histogramPeakfindingTimerReset();
			analyser->histogramFinalizingTimerReset();
			analyser->histogramResettingTimerReset();
			analyser->trackPeakfindingTimerReset();
		}
	}

#endif

#ifndef NOANALYSIS

	if (analyser != NULL)
		if (analyser->isTimeAnalysisEnabled())
			analyser->borderCreationTimerStart(false);

#endif

#ifndef NOANALYSIS

	houghTransform->createBorders(analyser, terminal);

#else

	houghTransform->createBorders(terminal);

#endif

#ifndef NOANALYSIS

	if (analyser != NULL)
		if (analyser->isTimeAnalysisEnabled())
			analyser->borderCreationTimerStop();

#endif

	createTerminalStatusSequence(&statusSequence, terminal, "Process layers:\t\t\t\t\t", (unsigned int)houghTransform->getNumberOfHistogramLayers());
	terminalInitialize(statusSequence);

	for (unsigned short i = 0; i < houghTransform->getNumberOfHistogramLayers(); i++) {

#ifndef NOANALYSIS

		if (analyser != NULL)
			if (analyser->isTimeAnalysisEnabled())
				analyser->histogramCreationTimerStart(false);

#endif

		houghTransform->createHistogramLayer(i);

#ifndef NOANALYSIS

		if (analyser != NULL)
			if (analyser->isTimeAnalysisEnabled())
				analyser->histogramCreationTimerStop();

#endif

#ifndef NOANALYSIS

		if (analyser != NULL) {
			if (analyser->isWritingCreatedHistogramLayerEnabled()) {
				if (analyser->isWritingJustOneCreatedHistogramLayerEnabled()) {
					if (analyser->getWritingHistogramLayerIndex() == i) {
						analyser->createdHistogramLayerWrite(i);
						if (analyser->isShowingCreatedHistogramLayerEnabled())
							analyser->createdHistogramLayerShow(i);
					}
				}
				else {
					analyser->createdHistogramLayerWrite(i);
					if (analyser->getWritingHistogramLayerIndex() == i) {
						if (analyser->isShowingCreatedHistogramLayerEnabled())
							analyser->createdHistogramLayerShow(i);
					}
				}
			}
		}

#endif

#ifndef NOANALYSIS

		if (analyser != NULL)
			if (analyser->isTimeAnalysisEnabled())
				analyser->histogramEncodingTimerStart(false);

#endif

		histoTransform->encodeHistogramLayer(i);

#ifndef NOANALYSIS

		if (analyser != NULL)
			if (analyser->isTimeAnalysisEnabled())
				analyser->histogramEncodingTimerStop();

#endif

#ifndef NOANALYSIS

		if (analyser != NULL) {
			if (analyser->isWritingEncodedHistogramLayerEnabled()) {
				if (analyser->isWritingJustOneEncodedHistogramLayerEnabled()) {
					if (analyser->getWritingHistogramLayerIndex() == i) {
						analyser->encodedHistogramLayerWrite(i);
						if (analyser->isShowingEncodedHistogramLayerEnabled())
							analyser->encodedHistogramLayerShow(i);
					}
				}
				else {
					analyser->encodedHistogramLayerWrite(i);
					if (analyser->getWritingHistogramLayerIndex() == i) {
						if (analyser->isShowingEncodedHistogramLayerEnabled())
							analyser->encodedHistogramLayerShow(i);
					}
				}
			}
		}

#endif

#ifndef NOANALYSIS

		if (analyser != NULL)
			if (analyser->isTimeAnalysisEnabled())
				analyser->histogramDiagonalizingTimerStart(false);

#endif

		histoTransform->diagonalizeHistogramLayer(i);

#ifndef NOANALYSIS

		if (analyser != NULL)
			if (analyser->isTimeAnalysisEnabled())
				analyser->histogramDiagonalizingTimerStop();

#endif

#ifndef NOANALYSIS

		if (analyser != NULL)
			if (analyser->isTimeAnalysisEnabled())
				analyser->histogramPeakfindingTimerStart(false);

#endif

		histoTransform->filterHistogramLayer(i);

#ifndef NOANALYSIS

		if (analyser != NULL)
			if (analyser->isTimeAnalysisEnabled())
				analyser->histogramPeakfindingTimerStop();

#endif

#ifndef NOANALYSIS

		if (analyser != NULL) {
			if (analyser->isWritingFilteredHistogramLayerEnabled()) {
				if (analyser->isWritingJustOneFilteredHistogramLayerEnabled()) {
					if (analyser->getWritingHistogramLayerIndex() == i) {
						analyser->filteredHistogramLayerWrite(i);
						if (analyser->isShowingFilteredHistogramLayerEnabled())
							analyser->filteredHistogramLayerShow(i);
					}
				}
				else {
					analyser->filteredHistogramLayerWrite(i);
					if (analyser->getWritingHistogramLayerIndex() == i) {
						if (analyser->isShowingFilteredHistogramLayerEnabled())
							analyser->filteredHistogramLayerShow(i);
					}
				}
			}
		}

#endif

#ifndef NOANALYSIS

		if (analyser != NULL)
			if (analyser->isTimeAnalysisEnabled())
				analyser->histogramFinalizingTimerStart(false);

#endif

		histoTransform->serializeHistogramLayer(i);

#ifndef NOANALYSIS

		if (analyser != NULL)
			if (analyser->isTimeAnalysisEnabled())
				analyser->histogramFinalizingTimerStop();

#endif

#ifndef NOANALYSIS

		if (analyser != NULL)
			if (analyser->isTimeAnalysisEnabled())
				analyser->histogramResettingTimerStart(false);

#endif

		houghTransform->resetHistogramLayer();

#ifndef NOANALYSIS

		if (analyser != NULL)
			if (analyser->isTimeAnalysisEnabled())
				analyser->histogramResettingTimerStop();

#endif

		terminalOverwriteWithIncrement(statusSequence);

	}

	terminalFinalize(statusSequence);

#ifndef NOANALYSIS

	if (analyser != NULL) {

		if (analyser->isNumberOfTracksInAllColumnsAnalysisEnabled())
			analyser->evaluateNumberOfTracksInAllColumns(terminal);
		if (analyser->isNumberOfTracksInAllRowsAnalysisEnabled())
			analyser->evaluateNumberOfTracksInAllRows(terminal);
		if (analyser->isNumberOfTracksInAllLayersAnalysisEnabled())
			analyser->evaluateNumberOfTracksInAllLayers(terminal);

	}

#endif

#ifndef NOANALYSIS

	if (analyser != NULL)
		if (analyser->isMemoryAnalysisEnabled())
			analyser->evaluateSizeOfLBufferData();

#endif

#ifndef NOANALYSIS

	if (analyser != NULL)
		if (analyser->isTimeAnalysisEnabled())
			analyser->trackPeakfindingTimerStart(false);

#endif

	histoTransform->filterHistogram(terminal);

#ifndef NOANALYSIS

	if (analyser != NULL)
		if (analyser->isTimeAnalysisEnabled())
			analyser->trackPeakfindingTimerStop();

#endif

}

/****************************************************************
 * This applies the algorithm to generate the filter geometry	*
 ****************************************************************/

void trackfinder::generateFilterGeometry(unsigned short filterCoverPercentage, bool isFirstEvent, std::streambuf* terminal) {

	peakfindingGeometry evaluatedGeometry;

	if (histoTransform == NULL)
		throw cannotAccessHistogramTransformError();
	if (analyser == NULL)
		throw cannotAccessAnalyserError();

	switch (filterType) {

		case FILEFILTER:
			if (isFirstEvent) {
				((autoFinder*)histoTransform)->readPeakfindingGeometry(filterFileName, terminal);
				if (filterCoverPercentage > 0)
					((autoFinder*)histoTransform)->setPeakfindingCoverage(filterCoverPercentage);
				((autoFinder*)histoTransform)->setup();
			}
			break;

		case AUTOMATICFIRSTEVENTFILTER:
			if (isFirstEvent) {
				evaluatePeakfindingGeometry(terminal);
				evaluatedGeometry = analyser->getPeakfindingGeometry();
				((autoFinder*)histoTransform)->setPeakfindingGeometry(evaluatedGeometry);
				((autoFinder*)histoTransform)->setPeakfindingCoverage(filterCoverPercentage);
				((autoFinder*)histoTransform)->setup();
			}
			break;

		case AUTOMATICEACHEVENTFILTER:
			evaluatePeakfindingGeometry(terminal);
			evaluatedGeometry = analyser->getPeakfindingGeometry();
			((autoFinder*)histoTransform)->setPeakfindingGeometry(evaluatedGeometry);
			((autoFinder*)histoTransform)->setPeakfindingCoverage(filterCoverPercentage);
			((autoFinder*)histoTransform)->setup();
			break;

		case AUTOMATICUPDATEEVENTFILTER:
			evaluatePeakfindingGeometry(terminal);
			evaluatedGeometry = analyser->getPeakfindingGeometry();
			if (isFirstEvent) {
				((autoFinder*)histoTransform)->setPeakfindingGeometry(evaluatedGeometry);
				((autoFinder*)histoTransform)->setPeakfindingCoverage(filterCoverPercentage);
			}
			else
				((autoFinder*)histoTransform)->updatePeakfindingGeometry(evaluatedGeometry);
			((autoFinder*)histoTransform)->setup();
			break;

		default:
			break;

	}

}

/****************************************************************
 * This method writes the peakfinding geometry into a file		*
 ****************************************************************/

void trackfinder::writePeakfindingGeometry(std::streambuf* terminal) {

	if (histoTransform == NULL)
		throw cannotAccessHistogramTransformError();

	if (isAutomaticFilterGeometryEnabled())
		((autoFinder*)histoTransform)->writePeakfindingGeometry(filterFileName, terminal);

}

/****************************************************************
 * This method draws the peakfinding geometry					*
 ****************************************************************/

void trackfinder::drawPeakfindingGeometry() {

	if (histoTransform == NULL)
		throw cannotAccessHistogramTransformError();

	if (isFilterGeometryGenerationEnabled())
		((autoFinder*)histoTransform)->drawPeakfindingGeometry();

}
void trackfinder::drawProjectedPeakfindingGeometry() {

	if (histoTransform == NULL)
		throw cannotAccessHistogramTransformError();

	if (isFilterGeometryGenerationEnabled())
		((autoFinder*)histoTransform)->drawProjectedPeakfindingGeometry();

}
void trackfinder::drawCoveredPeakfindingGeometry() {

	if (histoTransform == NULL)
		throw cannotAccessHistogramTransformError();

	if (isFilterGeometryGenerationEnabled())
		((autoFinder*)histoTransform)->drawCoveredPeakfindingGeometry();

}
void trackfinder::drawCoveredProjectedPeakfindingGeometry() {

	if (histoTransform == NULL)
		throw cannotAccessHistogramTransformError();

	if (isFilterGeometryGenerationEnabled())
		((autoFinder*)histoTransform)->drawCoveredProjectedPeakfindingGeometry();

}

/****************************************************************
 * This method returns a string representation for the			*
 * peakfinding geometry											*
 ****************************************************************/

std::string trackfinder::peakfindingGeometryToString() {

	std::string returnValue;

	returnValue.clear();

	if (histoTransform == NULL)
		throw cannotAccessHistogramTransformError();

	if (isFilterGeometryGenerationEnabled())
		returnValue = ((autoFinder*)histoTransform)->peakfindingGeometryToString();

	return returnValue;

}
std::string trackfinder::projectedPeakfindingGeometryToString() {

	std::string returnValue;

	returnValue.clear();

	if (histoTransform == NULL)
		throw cannotAccessHistogramTransformError();

	if (isFilterGeometryGenerationEnabled())
		returnValue = ((autoFinder*)histoTransform)->projectedPeakfindingGeometryToString();

	return returnValue;

}
std::string trackfinder::coveredPeakfindingGeometryToString() {

	std::string returnValue;

	returnValue.clear();

	if (histoTransform == NULL)
		throw cannotAccessHistogramTransformError();

	if (isFilterGeometryGenerationEnabled())
		returnValue = ((autoFinder*)histoTransform)->coveredPeakfindingGeometryToString();

	return returnValue;

}
std::string trackfinder::coveredProjectedPeakfindingGeometryToString() {

	std::string returnValue;

	returnValue.clear();

	if (histoTransform == NULL)
		throw cannotAccessHistogramTransformError();

	if (isFilterGeometryGenerationEnabled())
		returnValue = ((autoFinder*)histoTransform)->coveredProjectedPeakfindingGeometryToString();

	return returnValue;

}

/****************************************************************
 * method returns true if the automatic generation of the filter*
 * geometry is enabled.											*
 ****************************************************************/

bool trackfinder::isAutomaticFilterGeometryEnabled() {

	return ((filterType == AUTOMATICFIRSTEVENTFILTER) || (filterType == AUTOMATICEACHEVENTFILTER) || (filterType == AUTOMATICUPDATEEVENTFILTER));

}

/****************************************************************
 * method returns true if the generation of the filter			*
 * geometry is enabled.											*
 ****************************************************************/

bool trackfinder::isFilterGeometryGenerationEnabled() {

	return ((filterType == FILEFILTER) || isAutomaticFilterGeometryEnabled());

}

/****************************************************************
 * This method returns the trackIndex of the debugged track,	*
 * if DEBUGJUSTONEGOODTRACK is enabled.							*
 ****************************************************************/

int trackfinder::getDebugTrackIndex() {

	int returnValue;

	if (houghTransform != NULL)
		returnValue = houghTransform->getDebugTrackIndex();
	else
		returnValue = INVALIDTRACKINDEX;

	return returnValue;

}
