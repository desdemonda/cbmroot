/* *******************************************************************
// (C)opyright 2004
// 
// Institute of Computer Science V
// Prof. Mäner
// University of Mannheim, Germany
// 
// *******************************************************************
//
// Author(s):   Steinle
// 
// *******************************************************************
// 
// Project:     Trackfinder for CBM-Project at GSI-Darmstadt, Germany
// 
// *******************************************************************
// 
// Description:
//
//   This is the object to put the trackfinding algorithm into the
//   root framework.
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-11-21 13:58:42 $
// $Revision: 1.40 $
//
// *******************************************************************/


#include "../../MiscLIB/include/errorHandling.h"
#include "../../MiscLIB/include/terminal.h"
#include "../../DataRootObjectLIB/include/tables.h"
#include "../../AnalysisLIB/include/analysisDef.h"
#include "../../HistogramTransformationLIB/include/filterDef.h"
#include "../include/CbmHoughStsTrackFinder.h"
#include "TStopwatch.h"
#include <iostream>


#define detectorGammaOpeningAngle  0.46764	// atan((planeMaxY + planeMaxY / 100) / planeZ)
#define detectorThetaOpeningAngle  0.46764	// atan((planeMaxX + planeMaxX / 100) / planeZ)
#define momentumCutSmallerThan     1.0		// [GeV]


/****************************************************************
 * default constructor											*
 ****************************************************************/

CbmHoughStsTrackFinder::CbmHoughStsTrackFinder() 
  : CbmStsTrackFinder(),
    eventNumber(0),	       
    configurationFile(NULL),       
    isFirstEvent(false),   
    input(NULL),	       
    space(NULL),       
    eventData(NULL),       
    histogram(NULL),	       
    tracks(NULL),
    ratings(NULL),       
    luts(NULL),       
    houghTrackfinder(NULL),       
    output(NULL),  
#ifndef NOANALYSIS
    setAnalysisOutputFileName(),
    analyser(NULL)                   
#else
    setAnalysisOutputFileName()
#endif
{

	initialParameter analysisParameters;

	try {
		configurationFile      = new inf();
		switch (configurationFile->getDataReference().inputFileFormat) {
			case ASCIIFILEFORMAT:
				input          = new inputAscii(configurationFile->getDataReference().inputFileName.c_str(),
									  configurationFile->getDataReference().inputDetectorMask,
					                  configurationFile->getDataReference().trackfinderHitProducer);
				std::cerr << std::endl << "CbmHoughStsTrackFinder WARNING: The eventnumber would always start with zero, if the input is from an ascii file!!!" << std::endl;
				break;
			default:	// ROOTFILEFORMAT
				input          = new inputFRoot(configurationFile->getDataReference().inputFileNameDetector.c_str(),
									  configurationFile->getDataReference().inputNumberOfVolumesInfrontOfSTS,
									  configurationFile->getDataReference().inputDisableAutomaticDetector,
									  configurationFile->getDataReference().inputDetectorMask,
									  configurationFile->getDataReference().trackfinderReadPointsFromFile,
									  configurationFile->getDataReference().trackfinderReadHitsFromFile,
									  configurationFile->getDataReference().trackfinderReadMapsHits,
									  configurationFile->getDataReference().trackfinderReadHybridHits,
									  configurationFile->getDataReference().trackfinderReadStripHits);
				break;
		}
		space                  = new histogramSpace(configurationFile->getDataReference().trackfinderLutRadiusMin,
									  configurationFile->getDataReference().trackfinderLutRadiusMax,
									  configurationFile->getDataReference().trackfinderLutRadiusStep,
			                          configurationFile->getDataReference().trackfinderThetaMin,
									  configurationFile->getDataReference().trackfinderThetaMax,
									  configurationFile->getDataReference().trackfinderThetaStep,
									  configurationFile->getDataReference().trackfinderGammaMin,
									  configurationFile->getDataReference().trackfinderGammaMax,
									  configurationFile->getDataReference().trackfinderGammaStep);
		histogram              = new histogramData(&space);
		tracks                 = new trackData(&space);
		ratings                = new tables();
		luts                   = new lutImplementation();
		houghTrackfinder       = new trackfinder(&eventData, &histogram, &tracks, &ratings, &luts,
									  configurationFile->getDataReference().trackfinderFilterType,
									  configurationFile->getDataReference().trackfinderAutomaticFilterDataPercentage,
									  configurationFile->getDataReference().trackfinderAutomaticFilterFileName,
									  configurationFile->getDataReference().trackfinderFirstFilterGeometry,
									  configurationFile->getDataReference().trackfinderFirstFilterArithmetic,
									  configurationFile->getDataReference().trackfinderSecondFilterGeometry,
									  configurationFile->getDataReference().trackfinderSecondFilterArithmetic,
									  configurationFile->getDataReference().trackfinderFirstFilterNeighborhoodDim1ClearRadius,
									  configurationFile->getDataReference().trackfinderFirstFilterNeighborhoodDim2ClearRadius,
									  configurationFile->getDataReference().trackfinderSecondFilterNeighborhoodDim1ClearRadius,
									  configurationFile->getDataReference().trackfinderSecondFilterNeighborhoodDim2ClearRadius,
									  configurationFile->getDataReference().trackfinderSecondFilterNeighborhoodDim3ClearRadius);
		output                 = new outputFTrack(&tracks, configurationFile->getDataReference().trackfinderWriteTracksToFile);

#ifndef NOANALYSIS

		analysisParameters.eventData                       = &eventData;
		analysisParameters.histogram                       = &histogram;
		analysisParameters.tracks                          = &tracks;
		analysisParameters.ratings                         = &ratings;
		analysisParameters.space                           = &space;
		analysisParameters.luts                            = &luts;
		analysisParameters.minP                            = configurationFile->getDataReference().analysisThresholdForP;
		analysisParameters.initAutomatcFilterGeometry      = houghTrackfinder->isAutomaticFilterGeometryEnabled();
		analysisParameters.initConfiguration               = configurationFile->getDataReference().analysisInitConfiguration;
		analysisParameters.initDetector                    = configurationFile->getDataReference().analysisInitDetector;
		analysisParameters.initEvent                       = configurationFile->getDataReference().analysisInitEvent;
		analysisParameters.initClassPriority               = configurationFile->getDataReference().analysisInitClassPriority;
		analysisParameters.initMemory                      = configurationFile->getDataReference().analysisInitMemory;
		analysisParameters.initTime                        = configurationFile->getDataReference().analysisInitTime;
		analysisParameters.initPropagationEventPoint       = configurationFile->getDataReference().analysisInitTrackPropagationEventPoint;
		analysisParameters.initPropagationEventHit         = configurationFile->getDataReference().analysisInitTrackPropagationEventHit;
		analysisParameters.initPropagationTotalPoint       = configurationFile->getDataReference().analysisInitTrackPropagationTotalPoint;
		analysisParameters.initPropagationTotalHit         = configurationFile->getDataReference().analysisInitTrackPropagationTotalHit;
		analysisParameters.initPropagationDisplay          = configurationFile->getDataReference().analysisInitTrackPropagationDisplay;
		analysisParameters.initQualityEFGCEventAbsolute    = configurationFile->getDataReference().analysisInitQualityEFGCEventAbsolute;
		analysisParameters.initQualityEFGCEventRelative    = configurationFile->getDataReference().analysisInitQualityEFGCEventRelative;
		analysisParameters.initQualityEFGCTotalAbsolute    = configurationFile->getDataReference().analysisInitQualityEFGCTotalAbsolute;
		analysisParameters.initQualityEFGCTotalRelative    = configurationFile->getDataReference().analysisInitQualityEFGCTotalRelative;
		analysisParameters.initMomentumEFGCEventPzEFGC     = configurationFile->getDataReference().analysisInitMomentumEFGCEventPzEFGC;
		analysisParameters.initMomentumEFGCEventPtEFGC     = configurationFile->getDataReference().analysisInitMomentumEFGCEventPtEFGC;
		analysisParameters.initMomentumEFGCTotalPzEFGC     = configurationFile->getDataReference().analysisInitMomentumEFGCTotalPzEFGC;
		analysisParameters.initMomentumEFGCTotalPtEFGC     = configurationFile->getDataReference().analysisInitMomentumEFGCTotalPtEFGC;
		analysisParameters.initProjectionEFGCNEvent12EFGCN = configurationFile->getDataReference().analysisInitProjectionEFGCNEvent12EFGCN;
		analysisParameters.initProjectionEFGCNEvent13EFGCN = configurationFile->getDataReference().analysisInitProjectionEFGCNEvent13EFGCN;
		analysisParameters.initProjectionEFGCNEvent32EFGCN = configurationFile->getDataReference().analysisInitProjectionEFGCNEvent32EFGCN;
		analysisParameters.initProjectionEFGCNTotal12EFGCN = configurationFile->getDataReference().analysisInitProjectionEFGCNTotal12EFGCN;
		analysisParameters.initProjectionEFGCNTotal13EFGCN = configurationFile->getDataReference().analysisInitProjectionEFGCNTotal13EFGCN;
		analysisParameters.initProjectionEFGCNTotal32EFGCN = configurationFile->getDataReference().analysisInitProjectionEFGCNTotal32EFGCN;
		analysisParameters.initMomentumEvent               = configurationFile->getDataReference().analysisInitMomentumEvent;
		analysisParameters.initMomentumTotal               = configurationFile->getDataReference().analysisInitMomentumTotal;
		analysisParameters.initMomentumDisplay             = configurationFile->getDataReference().analysisInitMomentumDisplay;
		analysisParameters.initProjectionEvent             = configurationFile->getDataReference().analysisInitProjectionEvent;
		analysisParameters.initProjectionTotal             = configurationFile->getDataReference().analysisInitProjectionTotal;
		analysisParameters.initProjectionDisplay           = configurationFile->getDataReference().analysisInitProjectionDisplay;
		analysisParameters.initMagnetfieldFactors          = configurationFile->getDataReference().analysisInitMagnetfieldConstantForEachEvent || configurationFile->getDataReference().analysisInitWeightedMagnetfieldConstant;
		analysisParameters.initMagnetfieldFactorDisplay    = configurationFile->getDataReference().analysisInitMagnetfieldConstantDisplay;
		analysisParameters.initMagnetfieldX                = configurationFile->getDataReference().analysisInitMagnetfieldX;
		analysisParameters.initMagnetfieldY                = configurationFile->getDataReference().analysisInitMagnetfieldY;
		analysisParameters.initMagnetfieldZ                = configurationFile->getDataReference().analysisInitMagnetfieldZ;
		analysisParameters.initMagnetfieldDisplay          = configurationFile->getDataReference().analysisInitMagnetfieldDisplay;
		analysisParameters.initTracksPerColumn             = configurationFile->getDataReference().analysisInitNumberOfTracksPerColumn;
		analysisParameters.initTracksPerRow                = configurationFile->getDataReference().analysisInitNumberOfTracksPerRow;
		analysisParameters.initTracksPerLayer              = configurationFile->getDataReference().analysisInitNumberOfTracksPerLayer;
		analysisParameters.initHitReadoutDistribution      = configurationFile->getDataReference().analysisInitHitReadoutDistribution;
		analysisParameters.initPrelutRange                 = configurationFile->getDataReference().analysisInitPrelutRangeForEachEvent || configurationFile->getDataReference().analysisInitWeightedPrelutRange;
		analysisParameters.initPrelutRangeDisplay          = configurationFile->getDataReference().analysisInitPrelutRangeDisplay;
		analysisParameters.initPrelutRangeDisplayMode      = configurationFile->getDataReference().analysisInitPrelutRangeDisplayMode;
		analysisParameters.percentageOfHitsInSignature     = configurationFile->getDataReference().analysisInitPercentageOfHitsInSignature;
		analysisParameters.percentageOfTracksForSignature  = configurationFile->getDataReference().analysisInitPercentageOfTracksForSignature;
		analysisParameters.analysisResultWarnings          = configurationFile->getDataReference().analysisInitAnalysisResultWarnings;
		analysisParameters.analysisResultDisplays          = configurationFile->getDataReference().analysisInitAnalysisResultDisplays;
		analysisParameters.analysisMoreResultWarnings      = configurationFile->getDataReference().analysisInitAnalysisMoreResultWarnings;
		analysisParameters.analysisMoreResultDisplays      = configurationFile->getDataReference().analysisInitAnalysisMoreResultDisplays;
		analyser               = new analysis(analysisParameters,
									  configurationFile->getDataReference().analysisInitCreatedHistogramToRoot || configurationFile->getDataReference().analysisInitEncodedHistogramToRoot || configurationFile->getDataReference().analysisInitFilteredHistogramToRoot);

#else

		if ((configurationFile->getDataReference().inputCodingTableMode == LUTGOODNESSTABLE) ||
			(configurationFile->getDataReference().inputGradingPTableMode == LUTGOODNESSTABLE) ||
			(configurationFile->getDataReference().inputGradingRTableMode == LUTGOODNESSTABLE) ||
			houghTrackfinder->isAutomaticFilterGeometryEnabled() ||
			configurationFile->getDataReference().analysisInitConfiguration ||
			configurationFile->getDataReference().analysisInitDetector ||
			configurationFile->getDataReference().analysisInitEvent ||
			configurationFile->getDataReference().analysisInitClassPriority ||
			configurationFile->getDataReference().analysisInitMemory ||
			configurationFile->getDataReference().analysisInitTime ||
			configurationFile->getDataReference().analysisInitTrackPropagationEventPoint ||
			configurationFile->getDataReference().analysisInitTrackPropagationEventHit ||
			configurationFile->getDataReference().analysisInitTrackPropagationTotalPoint ||
			configurationFile->getDataReference().analysisInitTrackPropagationTotalHit ||
			configurationFile->getDataReference().analysisInitTrackPropagationDisplay ||
			configurationFile->getDataReference().analysisInitTrackPropagationToRoot ||
			configurationFile->getDataReference().analysisInitQualityEFGCEventAbsolute ||
			configurationFile->getDataReference().analysisInitQualityEFGCEventRelative ||
			configurationFile->getDataReference().analysisInitQualityEFGCTotalAbsolute ||
			configurationFile->getDataReference().analysisInitQualityEFGCTotalRelative ||
			configurationFile->getDataReference().analysisInitMomentumEFGCEventPzEFGC ||
			configurationFile->getDataReference().analysisInitMomentumEFGCEventPtEFGC ||
			configurationFile->getDataReference().analysisInitMomentumEFGCTotalPzEFGC ||
			configurationFile->getDataReference().analysisInitMomentumEFGCTotalPtEFGC ||
			configurationFile->getDataReference().analysisInitProjectionEFGCNEvent12EFGCN ||
			configurationFile->getDataReference().analysisInitProjectionEFGCNEvent13EFGCN ||
			configurationFile->getDataReference().analysisInitProjectionEFGCNEvent32EFGCN ||
			configurationFile->getDataReference().analysisInitProjectionEFGCNTotal12EFGCN ||
			configurationFile->getDataReference().analysisInitProjectionEFGCNTotal13EFGCN ||
			configurationFile->getDataReference().analysisInitProjectionEFGCNTotal32EFGCN ||
			configurationFile->getDataReference().analysisInitMomentumEvent ||
			configurationFile->getDataReference().analysisInitMomentumTotal ||
			configurationFile->getDataReference().analysisInitMomentumDisplay ||
			configurationFile->getDataReference().analysisInitMomentumToRoot ||
			configurationFile->getDataReference().analysisInitProjectionEvent ||
			configurationFile->getDataReference().analysisInitProjectionTotal ||
			configurationFile->getDataReference().analysisInitProjectionDisplay ||
			configurationFile->getDataReference().analysisInitProjectionToRoot ||
			configurationFile->getDataReference().analysisInitMagnetfieldConstantForEachEvent ||
			configurationFile->getDataReference().analysisInitWeightedMagnetfieldConstant ||
			configurationFile->getDataReference().analysisInitMagnetfieldConstantDisplay ||
			configurationFile->getDataReference().analysisInitMagnetfieldX ||
			configurationFile->getDataReference().analysisInitMagnetfieldY ||
			configurationFile->getDataReference().analysisInitMagnetfieldZ ||
			configurationFile->getDataReference().analysisInitMagnetfieldDisplay ||
			configurationFile->getDataReference().analysisInitMagnetfieldToRoot ||
			configurationFile->getDataReference().analysisInitCreatedHistogramToRoot ||
			configurationFile->getDataReference().analysisInitEncodedHistogramToRoot ||
			configurationFile->getDataReference().analysisInitFilteredHistogramToRoot ||
			configurationFile->getDataReference().analysisInitJustOneCreatedHistogramToRoot ||
			configurationFile->getDataReference().analysisInitJustOneEncodedHistogramToRoot ||
			configurationFile->getDataReference().analysisInitJustOneFilteredHistogramToRoot ||
			configurationFile->getDataReference().analysisInitCreatedHistogramToShow ||
			configurationFile->getDataReference().analysisInitEncodedHistogramToShow ||
			configurationFile->getDataReference().analysisInitFilteredHistogramToShow ||
			configurationFile->getDataReference().analysisInitNumberOfTracksPerColumn ||
			configurationFile->getDataReference().analysisInitNumberOfTracksPerRow ||
			configurationFile->getDataReference().analysisInitNumberOfTracksPerLayer ||
			configurationFile->getDataReference().analysisInitHitReadoutDistribution ||
			configurationFile->getDataReference().analysisInitReadoutColumnsInParallel ||
			configurationFile->getDataReference().analysisInitPrelutRangeForEachEvent ||
			configurationFile->getDataReference().analysisInitWeightedPrelutRange ||
			configurationFile->getDataReference().analysisInitPrelutRangeDisplay ||
			configurationFile->getDataReference().analysisInitPrelutRangeToRoot ||
			configurationFile->getDataReference().analysisChooseMainPrelutRange ||
			configurationFile->getDataReference().analysisChooseConstraintPrelutRange ||
			configurationFile->getDataReference().analysisInitTotalAnalysis ||
			(configurationFile->getDataReference().analysisInitAnalysisResultWarnings != 0) ||
			(configurationFile->getDataReference().analysisInitAnalysisResultDisplays != 0) ||
			(configurationFile->getDataReference().analysisInitAnalysisMoreResultWarnings != 0) ||
			(configurationFile->getDataReference().analysisInitAnalysisMoreResultDisplays != 0) ||
			configurationFile->getDataReference().analysisWriteCellFiles) {

				std::cout << "WARNING: The anaylsis which is enabled cannot be done because it is not added at compilation!!!" << std::endl;

		}

#endif

		setAnalysisOutputFileName.clear();

	}
	catch(errorHandling& error) {
		error.errorMsg();
	    Fatal("CbmHoughStsTrackFinder", "Fatal Error!!!");
	}
	catch(...) {
		std::cout << "Unknown error during 'CbmHoughStsTrackFinder'-object creation" << std::endl;
	    Fatal("CbmHoughStsTrackFinder", "Fatal Error!!!");
	}

}

/****************************************************************
 * constructor													*
 ****************************************************************/

CbmHoughStsTrackFinder::CbmHoughStsTrackFinder(const char* name) 
  : CbmStsTrackFinder(),
    eventNumber(0),	       
    configurationFile(NULL),       
    isFirstEvent(false),
    input(NULL),	       
    space(NULL),       
    eventData(NULL),       
    histogram(NULL),	       
    tracks(NULL),
    ratings(NULL),       
    luts(NULL),       
    houghTrackfinder(NULL),       
    output(NULL),  
#ifndef NOANALYSIS
    setAnalysisOutputFileName(),
    analyser(NULL)                   
#else
    setAnalysisOutputFileName()
#endif
{

	initialParameter analysisParameters;

	try {
		configurationFile      = new inf(name);
		switch (configurationFile->getDataReference().inputFileFormat) {
			case ASCIIFILEFORMAT:
				input          = new inputAscii(configurationFile->getDataReference().inputFileName.c_str(),
									  configurationFile->getDataReference().inputDetectorMask,
					                  configurationFile->getDataReference().trackfinderHitProducer);
				std::cerr << std::endl << "CbmHoughStsTrackFinder WARNING: The eventnumber would always start with zero, if the input is from an ascii file!!!" << std::endl;
				break;
			default:	// ROOTFILEFORMAT
				input          = new inputFRoot(configurationFile->getDataReference().inputFileNameDetector.c_str(),
									  configurationFile->getDataReference().inputNumberOfVolumesInfrontOfSTS,
									  configurationFile->getDataReference().inputDisableAutomaticDetector,
									  configurationFile->getDataReference().inputDetectorMask,
									  configurationFile->getDataReference().trackfinderReadPointsFromFile,
									  configurationFile->getDataReference().trackfinderReadHitsFromFile,
									  configurationFile->getDataReference().trackfinderReadMapsHits,
									  configurationFile->getDataReference().trackfinderReadHybridHits,
									  configurationFile->getDataReference().trackfinderReadStripHits);
				break;
		}
		space                  = new histogramSpace(configurationFile->getDataReference().trackfinderLutRadiusMin,
									  configurationFile->getDataReference().trackfinderLutRadiusMax,
									  configurationFile->getDataReference().trackfinderLutRadiusStep,
			                          configurationFile->getDataReference().trackfinderThetaMin,
									  configurationFile->getDataReference().trackfinderThetaMax,
									  configurationFile->getDataReference().trackfinderThetaStep,
									  configurationFile->getDataReference().trackfinderGammaMin,
									  configurationFile->getDataReference().trackfinderGammaMax,
									  configurationFile->getDataReference().trackfinderGammaStep);
		histogram              = new histogramData(&space);
		tracks                 = new trackData(&space);
		ratings                = new tables();
		luts                   = new lutImplementation();
		houghTrackfinder       = new trackfinder(&eventData, &histogram, &tracks, &ratings, &luts,
									  configurationFile->getDataReference().trackfinderFilterType,
									  configurationFile->getDataReference().trackfinderAutomaticFilterDataPercentage,
									  configurationFile->getDataReference().trackfinderAutomaticFilterFileName,
									  configurationFile->getDataReference().trackfinderFirstFilterGeometry,
									  configurationFile->getDataReference().trackfinderFirstFilterArithmetic,
									  configurationFile->getDataReference().trackfinderSecondFilterGeometry,
									  configurationFile->getDataReference().trackfinderSecondFilterArithmetic,
									  configurationFile->getDataReference().trackfinderFirstFilterNeighborhoodDim1ClearRadius,
									  configurationFile->getDataReference().trackfinderFirstFilterNeighborhoodDim2ClearRadius,
									  configurationFile->getDataReference().trackfinderSecondFilterNeighborhoodDim1ClearRadius,
									  configurationFile->getDataReference().trackfinderSecondFilterNeighborhoodDim2ClearRadius,
									  configurationFile->getDataReference().trackfinderSecondFilterNeighborhoodDim3ClearRadius);
		output                 = new outputFTrack(&tracks, configurationFile->getDataReference().trackfinderWriteTracksToFile);

#ifndef NOANALYSIS

		analysisParameters.eventData                       = &eventData;
		analysisParameters.histogram                       = &histogram;
		analysisParameters.tracks                          = &tracks;
		analysisParameters.ratings                         = &ratings;
		analysisParameters.space                           = &space;
		analysisParameters.luts                            = &luts;
		analysisParameters.minP                            = configurationFile->getDataReference().analysisThresholdForP;
		analysisParameters.initAutomatcFilterGeometry      = houghTrackfinder->isAutomaticFilterGeometryEnabled();
		analysisParameters.initConfiguration               = configurationFile->getDataReference().analysisInitConfiguration;
		analysisParameters.initDetector                    = configurationFile->getDataReference().analysisInitDetector;
		analysisParameters.initEvent                       = configurationFile->getDataReference().analysisInitEvent;
		analysisParameters.initClassPriority               = configurationFile->getDataReference().analysisInitClassPriority;
		analysisParameters.initMemory                      = configurationFile->getDataReference().analysisInitMemory;
		analysisParameters.initTime                        = configurationFile->getDataReference().analysisInitTime;
		analysisParameters.initPropagationEventPoint       = configurationFile->getDataReference().analysisInitTrackPropagationEventPoint;
		analysisParameters.initPropagationEventHit         = configurationFile->getDataReference().analysisInitTrackPropagationEventHit;
		analysisParameters.initPropagationTotalPoint       = configurationFile->getDataReference().analysisInitTrackPropagationTotalPoint;
		analysisParameters.initPropagationTotalHit         = configurationFile->getDataReference().analysisInitTrackPropagationTotalHit;
		analysisParameters.initPropagationDisplay          = configurationFile->getDataReference().analysisInitTrackPropagationDisplay;
		analysisParameters.initQualityEFGCEventAbsolute    = configurationFile->getDataReference().analysisInitQualityEFGCEventAbsolute;
		analysisParameters.initQualityEFGCEventRelative    = configurationFile->getDataReference().analysisInitQualityEFGCEventRelative;
		analysisParameters.initQualityEFGCTotalAbsolute    = configurationFile->getDataReference().analysisInitQualityEFGCTotalAbsolute;
		analysisParameters.initQualityEFGCTotalRelative    = configurationFile->getDataReference().analysisInitQualityEFGCTotalRelative;
		analysisParameters.initMomentumEFGCEventPzEFGC     = configurationFile->getDataReference().analysisInitMomentumEFGCEventPzEFGC;
		analysisParameters.initMomentumEFGCEventPtEFGC     = configurationFile->getDataReference().analysisInitMomentumEFGCEventPtEFGC;
		analysisParameters.initMomentumEFGCTotalPzEFGC     = configurationFile->getDataReference().analysisInitMomentumEFGCTotalPzEFGC;
		analysisParameters.initMomentumEFGCTotalPtEFGC     = configurationFile->getDataReference().analysisInitMomentumEFGCTotalPtEFGC;
		analysisParameters.initProjectionEFGCNEvent12EFGCN = configurationFile->getDataReference().analysisInitProjectionEFGCNEvent12EFGCN;
		analysisParameters.initProjectionEFGCNEvent13EFGCN = configurationFile->getDataReference().analysisInitProjectionEFGCNEvent13EFGCN;
		analysisParameters.initProjectionEFGCNEvent32EFGCN = configurationFile->getDataReference().analysisInitProjectionEFGCNEvent32EFGCN;
		analysisParameters.initProjectionEFGCNTotal12EFGCN = configurationFile->getDataReference().analysisInitProjectionEFGCNTotal12EFGCN;
		analysisParameters.initProjectionEFGCNTotal13EFGCN = configurationFile->getDataReference().analysisInitProjectionEFGCNTotal13EFGCN;
		analysisParameters.initProjectionEFGCNTotal32EFGCN = configurationFile->getDataReference().analysisInitProjectionEFGCNTotal32EFGCN;
		analysisParameters.initMomentumEvent               = configurationFile->getDataReference().analysisInitMomentumEvent;
		analysisParameters.initMomentumTotal               = configurationFile->getDataReference().analysisInitMomentumTotal;
		analysisParameters.initMomentumDisplay             = configurationFile->getDataReference().analysisInitMomentumDisplay;
		analysisParameters.initProjectionEvent             = configurationFile->getDataReference().analysisInitProjectionEvent;
		analysisParameters.initProjectionTotal             = configurationFile->getDataReference().analysisInitProjectionTotal;
		analysisParameters.initProjectionDisplay           = configurationFile->getDataReference().analysisInitProjectionDisplay;
		analysisParameters.initMagnetfieldFactors          = configurationFile->getDataReference().analysisInitMagnetfieldConstantForEachEvent || configurationFile->getDataReference().analysisInitWeightedMagnetfieldConstant;
		analysisParameters.initMagnetfieldFactorDisplay    = configurationFile->getDataReference().analysisInitMagnetfieldConstantDisplay;
		analysisParameters.initMagnetfieldX                = configurationFile->getDataReference().analysisInitMagnetfieldX;
		analysisParameters.initMagnetfieldY                = configurationFile->getDataReference().analysisInitMagnetfieldY;
		analysisParameters.initMagnetfieldZ                = configurationFile->getDataReference().analysisInitMagnetfieldZ;
		analysisParameters.initMagnetfieldDisplay          = configurationFile->getDataReference().analysisInitMagnetfieldDisplay;
		analysisParameters.initTracksPerColumn             = configurationFile->getDataReference().analysisInitNumberOfTracksPerColumn;
		analysisParameters.initTracksPerRow                = configurationFile->getDataReference().analysisInitNumberOfTracksPerRow;
		analysisParameters.initTracksPerLayer              = configurationFile->getDataReference().analysisInitNumberOfTracksPerLayer;
		analysisParameters.initHitReadoutDistribution      = configurationFile->getDataReference().analysisInitHitReadoutDistribution;
		analysisParameters.initPrelutRange                 = configurationFile->getDataReference().analysisInitPrelutRangeForEachEvent || configurationFile->getDataReference().analysisInitWeightedPrelutRange;
		analysisParameters.initPrelutRangeDisplay          = configurationFile->getDataReference().analysisInitPrelutRangeDisplay;
		analysisParameters.initPrelutRangeDisplayMode      = configurationFile->getDataReference().analysisInitPrelutRangeDisplayMode;
		analysisParameters.percentageOfHitsInSignature     = configurationFile->getDataReference().analysisInitPercentageOfHitsInSignature;
		analysisParameters.percentageOfTracksForSignature  = configurationFile->getDataReference().analysisInitPercentageOfTracksForSignature;
		analysisParameters.analysisResultWarnings          = configurationFile->getDataReference().analysisInitAnalysisResultWarnings;
		analysisParameters.analysisResultDisplays          = configurationFile->getDataReference().analysisInitAnalysisResultDisplays;
		analysisParameters.analysisMoreResultWarnings      = configurationFile->getDataReference().analysisInitAnalysisMoreResultWarnings;
		analysisParameters.analysisMoreResultDisplays      = configurationFile->getDataReference().analysisInitAnalysisMoreResultDisplays;
		analyser               = new analysis(analysisParameters,
									  configurationFile->getDataReference().analysisInitCreatedHistogramToRoot || configurationFile->getDataReference().analysisInitEncodedHistogramToRoot || configurationFile->getDataReference().analysisInitFilteredHistogramToRoot);

#else

		if ((configurationFile->getDataReference().inputCodingTableMode == LUTGOODNESSTABLE) ||
			(configurationFile->getDataReference().inputGradingPTableMode == LUTGOODNESSTABLE) ||
			(configurationFile->getDataReference().inputGradingRTableMode == LUTGOODNESSTABLE) ||
			houghTrackfinder->isAutomaticFilterGeometryEnabled() ||
			configurationFile->getDataReference().analysisInitConfiguration ||
			configurationFile->getDataReference().analysisInitDetector ||
			configurationFile->getDataReference().analysisInitEvent ||
			configurationFile->getDataReference().analysisInitClassPriority ||
			configurationFile->getDataReference().analysisInitMemory ||
			configurationFile->getDataReference().analysisInitTime ||
			configurationFile->getDataReference().analysisInitTrackPropagationEventPoint ||
			configurationFile->getDataReference().analysisInitTrackPropagationEventHit ||
			configurationFile->getDataReference().analysisInitTrackPropagationTotalPoint ||
			configurationFile->getDataReference().analysisInitTrackPropagationTotalHit ||
			configurationFile->getDataReference().analysisInitTrackPropagationDisplay ||
			configurationFile->getDataReference().analysisInitTrackPropagationToRoot ||
			configurationFile->getDataReference().analysisInitQualityEFGCEventAbsolute ||
			configurationFile->getDataReference().analysisInitQualityEFGCEventRelative ||
			configurationFile->getDataReference().analysisInitQualityEFGCTotalAbsolute ||
			configurationFile->getDataReference().analysisInitQualityEFGCTotalRelative ||
			configurationFile->getDataReference().analysisInitMomentumEFGCEventPzEFGC ||
			configurationFile->getDataReference().analysisInitMomentumEFGCEventPtEFGC ||
			configurationFile->getDataReference().analysisInitMomentumEFGCTotalPzEFGC ||
			configurationFile->getDataReference().analysisInitMomentumEFGCTotalPtEFGC ||
			configurationFile->getDataReference().analysisInitProjectionEFGCNEvent12EFGCN ||
			configurationFile->getDataReference().analysisInitProjectionEFGCNEvent13EFGCN ||
			configurationFile->getDataReference().analysisInitProjectionEFGCNEvent32EFGCN ||
			configurationFile->getDataReference().analysisInitProjectionEFGCNTotal12EFGCN ||
			configurationFile->getDataReference().analysisInitProjectionEFGCNTotal13EFGCN ||
			configurationFile->getDataReference().analysisInitProjectionEFGCNTotal32EFGCN ||
			configurationFile->getDataReference().analysisInitMomentumEvent ||
			configurationFile->getDataReference().analysisInitMomentumTotal ||
			configurationFile->getDataReference().analysisInitMomentumDisplay ||
			configurationFile->getDataReference().analysisInitMomentumToRoot ||
			configurationFile->getDataReference().analysisInitProjectionEvent ||
			configurationFile->getDataReference().analysisInitProjectionTotal ||
			configurationFile->getDataReference().analysisInitProjectionDisplay ||
			configurationFile->getDataReference().analysisInitProjectionToRoot ||
			configurationFile->getDataReference().analysisInitMagnetfieldConstantForEachEvent ||
			configurationFile->getDataReference().analysisInitWeightedMagnetfieldConstant ||
			configurationFile->getDataReference().analysisInitMagnetfieldConstantDisplay ||
			configurationFile->getDataReference().analysisInitMagnetfieldX ||
			configurationFile->getDataReference().analysisInitMagnetfieldY ||
			configurationFile->getDataReference().analysisInitMagnetfieldZ ||
			configurationFile->getDataReference().analysisInitMagnetfieldDisplay ||
			configurationFile->getDataReference().analysisInitMagnetfieldToRoot ||
			configurationFile->getDataReference().analysisInitCreatedHistogramToRoot ||
			configurationFile->getDataReference().analysisInitEncodedHistogramToRoot ||
			configurationFile->getDataReference().analysisInitFilteredHistogramToRoot ||
			configurationFile->getDataReference().analysisInitJustOneCreatedHistogramToRoot ||
			configurationFile->getDataReference().analysisInitJustOneEncodedHistogramToRoot ||
			configurationFile->getDataReference().analysisInitJustOneFilteredHistogramToRoot ||
			configurationFile->getDataReference().analysisInitCreatedHistogramToShow ||
			configurationFile->getDataReference().analysisInitEncodedHistogramToShow ||
			configurationFile->getDataReference().analysisInitFilteredHistogramToShow ||
			configurationFile->getDataReference().analysisInitNumberOfTracksPerColumn ||
			configurationFile->getDataReference().analysisInitNumberOfTracksPerRow ||
			configurationFile->getDataReference().analysisInitNumberOfTracksPerLayer ||
			configurationFile->getDataReference().analysisInitHitReadoutDistribution ||
			configurationFile->getDataReference().analysisInitReadoutColumnsInParallel ||
			configurationFile->getDataReference().analysisInitPrelutRangeForEachEvent ||
			configurationFile->getDataReference().analysisInitWeightedPrelutRange ||
			configurationFile->getDataReference().analysisInitPrelutRangeDisplay ||
			configurationFile->getDataReference().analysisInitPrelutRangeToRoot ||
			configurationFile->getDataReference().analysisChooseMainPrelutRange ||
			configurationFile->getDataReference().analysisChooseConstraintPrelutRange ||
			configurationFile->getDataReference().analysisInitTotalAnalysis ||
			(configurationFile->getDataReference().analysisInitAnalysisResultWarnings != 0) ||
			(configurationFile->getDataReference().analysisInitAnalysisResultDisplays != 0) ||
			(configurationFile->getDataReference().analysisInitAnalysisMoreResultWarnings != 0) ||
			(configurationFile->getDataReference().analysisInitAnalysisMoreResultDisplays != 0) ||
			configurationFile->getDataReference().analysisWriteCellFiles) {

				std::cout << "WARNING: The anaylsis which is enabled cannot be done because it is not added at compilation!!!" << std::endl;

		}

#endif

		setAnalysisOutputFileName.clear();

	}
	catch(errorHandling& error) {
		error.errorMsg();
	    Fatal("CbmHoughStsTrackFinder", "Fatal Error!!!");
	}
	catch(...) {
		std::cout << "Unknown error during 'CbmHoughStsTrackFinder'-object creation" << std::endl;
	    Fatal("CbmHoughStsTrackFinder", "Fatal Error!!!");
	}

}
CbmHoughStsTrackFinder::CbmHoughStsTrackFinder(bitArray inputDetectorMask,
						int inputMinTracks, int inputMaxTracks,
						unsigned short inputMagneticFieldIntegrationStepwidthPerStation,
						int trackfinderGammaStep,
						int trackfinderThetaStep, int trackfinderLutRadiusStep,
						unsigned short trackfinderFirstFilterNeighborhoodDim1ClearRadius,
						unsigned short trackfinderFirstFilterNeighborhoodDim2ClearRadius,
						unsigned short trackfinderSecondFilterNeighborhoodDim1ClearRadius,
						unsigned short trackfinderSecondFilterNeighborhoodDim2ClearRadius,
						unsigned short trackfinderSecondFilterNeighborhoodDim3ClearRadius,
						bool initStatus, const char* analysisOutputFileName) 
  : CbmStsTrackFinder(),
    eventNumber(0),	       
    configurationFile(NULL),       
    isFirstEvent(false),   
    input(NULL),	       
    space(NULL),       
    eventData(NULL),       
    histogram(NULL),	       
    tracks(NULL),
    ratings(NULL),       
    luts(NULL),       
    houghTrackfinder(NULL),       
    output(NULL),  
#ifndef NOANALYSIS
    setAnalysisOutputFileName(),
    analyser(NULL)                   
#else
    setAnalysisOutputFileName()
#endif
{

	initialParameter analysisParameters;
	InfData          configuration;

	configuration.inputFileName                                      = "";
	configuration.inputFileFormat                                    = ROOTFILEFORMAT;
	configuration.inputMinTracks                                     = inputMinTracks;
	configuration.inputMaxTracks                                     = inputMaxTracks;
	configuration.inputDetectorMask                                  = inputDetectorMask;
	configuration.inputFileNameMagneticField                         = "";
	configuration.inputMagneticFieldIsRootFile                       = true;
	configuration.inputMapNameMagneticField                          = "NewMap";
	configuration.inputMagneticFieldIntegrationStepwidthPerStation   = inputMagneticFieldIntegrationStepwidthPerStation;
	configuration.inputMagneticFieldIntegrationFactor                = 1.0;
	configuration.inputDisableAutomaticMagneticField                 = false;
	configuration.inputFileNameDetector                              = "";
	configuration.inputNumberOfVolumesInfrontOfSTS                   = 14;
	configuration.inputDisableAutomaticDetector                      = false;
	configuration.inputCodingTableMode                               = FILETABLE;
        configuration.inputGradingPTableMode                             = FILETABLE;
	configuration.inputGradingRTableMode                             = FILETABLE;
	configuration.inputCodingTableFileName                           = "./parameters/htrack/codingTable.txt";
	configuration.inputGradingPTableFileName                         = "./parameters/htrack/gradingPTable.txt";
	configuration.inputGradingRTableFileName                         = "./parameters/htrack/gradingRTable.txt";
	configuration.inputCodingTableWrite                              = false;
	configuration.inputGradingPTableWrite                            = false;
	configuration.inputGradingRTableWrite                            = false;
	configuration.trackfinderHitProducer                             = NONEHITPRODUCER;
	configuration.trackfinderReadPointsFromFile                      = true;
	configuration.trackfinderReadHitsFromFile                        = false;
	configuration.trackfinderReadMapsHits                            = false;
	configuration.trackfinderReadHybridHits                          = true;
	configuration.trackfinderReadStripHits                           = true;
	configuration.trackfinderLutsVersion                             = ANALYTICFORMULALUT;
	configuration.trackfinderPrelutFileName                          = "./parameters/htrack/prelut.txt";
	configuration.trackfinderLutFileName                             = "./parameters/htrack/lut.txt";
	configuration.trackfinderGammaMin                                = -detectorGammaOpeningAngle;
	configuration.trackfinderGammaMax                                = +detectorGammaOpeningAngle;
	configuration.trackfinderGammaStep                               = trackfinderGammaStep;
	configuration.trackfinderThetaMin                                = -detectorGammaOpeningAngle;
	configuration.trackfinderThetaMax                                = +detectorGammaOpeningAngle;
	configuration.trackfinderThetaStep                               = trackfinderThetaStep;
	configuration.trackfinderPrelutRadiusMin                         = -0.035;
	configuration.trackfinderPrelutRadiusMax                         = +0.035;
	configuration.trackfinderLutRadiusMin                            = -(1 / (momentumCutSmallerThan - momentumCutSmallerThan / 10));
	configuration.trackfinderLutRadiusMax                            = +(1 / (momentumCutSmallerThan - momentumCutSmallerThan / 10));
	configuration.trackfinderLutRadiusStep                           = trackfinderLutRadiusStep;
	configuration.trackfinderMinClassCoding                          = 1;
	configuration.trackfinderMinClassGradingP                        = 1;
	configuration.trackfinderMinClassGradingR                        = 1;
	configuration.trackfinderFilterType                              = MAXMORPHSEARCHFILTER;
	configuration.trackfinderFirstFilterGeometry                     = FIRSTFINALGEOMETRY;
	configuration.trackfinderFirstFilterArithmetic                   = FIRSTSIMPLEARITHMETIC;
	configuration.trackfinderSecondFilterGeometry                    = SECONDFINALGEOMETRY;
	configuration.trackfinderSecondFilterArithmetic                  = SECONDSIMPLEARITHMETIC;
	configuration.trackfinderFirstFilterNeighborhoodDim1ClearRadius  = trackfinderFirstFilterNeighborhoodDim1ClearRadius;
	configuration.trackfinderFirstFilterNeighborhoodDim2ClearRadius  = trackfinderFirstFilterNeighborhoodDim2ClearRadius;
	configuration.trackfinderSecondFilterNeighborhoodDim1ClearRadius = trackfinderSecondFilterNeighborhoodDim1ClearRadius;
	configuration.trackfinderSecondFilterNeighborhoodDim2ClearRadius = trackfinderSecondFilterNeighborhoodDim2ClearRadius;
	configuration.trackfinderSecondFilterNeighborhoodDim3ClearRadius = trackfinderSecondFilterNeighborhoodDim3ClearRadius;
	configuration.trackfinderWriteTracksToFile                       = false;
	configuration.analysisThresholdForP                              = momentumCutSmallerThan;
	configuration.analysisInitConfiguration                          = false;
	configuration.analysisInitDetector                               = false;
	configuration.analysisInitEvent                                  = true;
	configuration.analysisInitClassPriority                          = false;
	configuration.analysisInitMemory                                 = false;
	configuration.analysisInitTime                                   = false;
	configuration.analysisInitTrackPropagationEventPoint             = false;
	configuration.analysisInitTrackPropagationEventHit               = false;
	configuration.analysisInitTrackPropagationTotalPoint             = false;
	configuration.analysisInitTrackPropagationTotalHit               = false;
	configuration.analysisInitTrackPropagationDisplay                = false;
	configuration.analysisInitTrackPropagationToRoot                 = false;
	configuration.analysisTrackPropagationPointDisplayMask           = 0;
	configuration.analysisTrackPropagationHitDisplayMask             = 0;
	configuration.analysisInitQualityEFGCEventAbsolute               = false;
	configuration.analysisInitQualityEFGCEventRelative               = false;
	configuration.analysisInitQualityEFGCTotalAbsolute               = false;
	configuration.analysisInitQualityEFGCTotalRelative               = true;
	configuration.analysisInitMomentumEFGCEventPzEFGC                = false;
	configuration.analysisInitMomentumEFGCEventPtEFGC                = false;
	configuration.analysisInitMomentumEFGCTotalPzEFGC                = false;
	configuration.analysisInitMomentumEFGCTotalPtEFGC                = false;
	configuration.analysisInitProjectionEFGCNEvent12EFGCN            = false;
	configuration.analysisInitProjectionEFGCNEvent13EFGCN            = false;
	configuration.analysisInitProjectionEFGCNEvent32EFGCN            = false;
	configuration.analysisInitProjectionEFGCNTotal12EFGCN            = false;
	configuration.analysisInitProjectionEFGCNTotal13EFGCN            = false;
	configuration.analysisInitProjectionEFGCNTotal32EFGCN            = false;
	configuration.analysisInitMomentumEvent                          = false;
	configuration.analysisInitMomentumTotal                          = false;
	configuration.analysisInitMomentumDisplay                        = false;
	configuration.analysisInitMomentumToRoot                         = false;
	configuration.analysisInitProjectionEvent                        = false;
	configuration.analysisInitProjectionTotal                        = false;
	configuration.analysisInitProjectionDisplay                      = false;
	configuration.analysisInitProjectionToRoot                       = false;
 	configuration.analysisInitMagnetfieldX                           = false;
	configuration.analysisInitMagnetfieldY                           = false;
	configuration.analysisInitMagnetfieldZ                           = false;
	configuration.analysisInitMagnetfieldDisplay                     = false;
	configuration.analysisInitMagnetfieldToRoot                      = false;
	configuration.analysisInitMagnetfieldConstantForEachEvent        = false;
	configuration.analysisInitWeightedMagnetfieldConstant            = false;
	configuration.analysisInitMagnetfieldConstantDisplay             = false;
	configuration.analysisMagnetfieldConstantDisplayMask             = 0;
	configuration.analysisInitMagnetfieldConstantToRoot              = false;
	configuration.analysisInitMagnetfieldVSConstants                 = false;
	configuration.analysisInitPrelutGoodness                         = false;
	configuration.analysisInitLutGoodness                            = false;
	configuration.analysisInitHoughTransformGoodness                 = false;
	configuration.analysisInitQuantizationGoodness                   = false;
	configuration.analysisInitCreatedHistogramToRoot                 = false;
	configuration.analysisInitEncodedHistogramToRoot                 = false;
	configuration.analysisInitFilteredHistogramToRoot                = false;
	configuration.analysisInitJustOneCreatedHistogramToRoot          = false;
	configuration.analysisInitJustOneEncodedHistogramToRoot          = false;
	configuration.analysisInitJustOneFilteredHistogramToRoot         = false;
	configuration.analysisInitCreatedHistogramToShow                 = false;
	configuration.analysisInitEncodedHistogramToShow                 = false;
	configuration.analysisInitFilteredHistogramToShow                = false;
	configuration.analysisInitHistogramLayer                         = 100;
	configuration.analysisInitNumberOfTracksPerColumn                = false;
	configuration.analysisInitNumberOfTracksPerRow                   = false;
	configuration.analysisInitNumberOfTracksPerLayer                 = false;
	configuration.analysisInitHitReadoutDistribution                 = false;
	configuration.analysisInitReadoutColumnsInParallel               = true;
	configuration.analysisInitPrelutRangeForEachEvent                = false;
	configuration.analysisInitWeightedPrelutRange                    = false;
	configuration.analysisInitPrelutRangeDisplay                     = false;
	configuration.analysisInitPrelutRangeDisplayMode                 = CUTMAINRELATIVEDISPLAYMODE;
	configuration.analysisPrelutRangeStationDisplayMask              = 0;
	configuration.analysisPrelutRangeStationSumDisplayMask           = false;
	configuration.analysisPrelutRangeConstraintDisplayMask           = 0;
	configuration.analysisPrelutRangeConstraintSumDisplayMask        = false;
	configuration.analysisPrelutRangeRelativeDisplayMask             = false;
	configuration.analysisInitPrelutRangeToRoot                      = false;
	configuration.analysisInitPercentageOfHitsForPrelutRange         = 80;
	configuration.analysisPrelutRangeMinStart                        = -0.1;
	configuration.analysisPrelutRangeMinStop                         = +0.1;
	configuration.analysisPrelutRangeMinSteps                        = 201;
	configuration.analysisPrelutRangeMaxStart                        = -0.1;
	configuration.analysisPrelutRangeMaxStop                         = +0.1;
	configuration.analysisPrelutRangeMaxSteps                        = 201;
	configuration.analysisInitTotalAnalysis                          = false;
	configuration.analysisInitPercentageOfHitsInSignature            = 70;
	configuration.analysisInitPercentageOfTracksForSignature         = 75;
	configuration.analysisInitAnalysisResultWarnings                 = (NUMBEROFTRACKSWHICHCANNOTBEFOUND);
	configuration.analysisInitAnalysisResultDisplays                 = (TRACKWITHMOMENTAERRORDISTRIBUTION | TRACKSPERCOLUMNDISTRIBUTION | TRACKSPERLAYERDISTRIBUTION);
	configuration.analysisInitAnalysisMoreResultWarnings             = 0;
	configuration.analysisInitAnalysisMoreResultDisplays             = 0;
	configuration.analysisWriteCellFiles                             = false;
	configuration.analysisHitCellFileName                            = "./parameters/htrack/cellHits.txt";
	configuration.analysisPrelutCellFileName                         = "./parameters/htrack/cellPrelut.txt";
	configuration.analysisLutCellFileName                            = "./parameters/htrack/cellLut.txt";
	configuration.initStatus                                         = initStatus;
	configuration.analysisOutputFileName                             = analysisOutputFileName;

	try {
		configurationFile      = new inf(configuration);
		switch (configurationFile->getDataReference().inputFileFormat) {
			case ASCIIFILEFORMAT:
				input          = new inputAscii(configurationFile->getDataReference().inputFileName.c_str(),
									  configurationFile->getDataReference().inputDetectorMask,
					                  configurationFile->getDataReference().trackfinderHitProducer);
				std::cerr << std::endl << "CbmHoughStsTrackFinder WARNING: The eventnumber would always start with zero, if the input is from an ascii file!!!" << std::endl;
				break;
			default:	// ROOTFILEFORMAT
				input          = new inputFRoot(configurationFile->getDataReference().inputFileNameDetector.c_str(),
									  configurationFile->getDataReference().inputNumberOfVolumesInfrontOfSTS,
									  configurationFile->getDataReference().inputDisableAutomaticDetector,
									  configurationFile->getDataReference().inputDetectorMask,
									  configurationFile->getDataReference().trackfinderReadPointsFromFile,
									  configurationFile->getDataReference().trackfinderReadHitsFromFile,
									  configurationFile->getDataReference().trackfinderReadMapsHits,
									  configurationFile->getDataReference().trackfinderReadHybridHits,
									  configurationFile->getDataReference().trackfinderReadStripHits);
				break;
		}
		space                  = new histogramSpace(configurationFile->getDataReference().trackfinderLutRadiusMin,
									  configurationFile->getDataReference().trackfinderLutRadiusMax,
									  configurationFile->getDataReference().trackfinderLutRadiusStep,
			                          configurationFile->getDataReference().trackfinderThetaMin,
									  configurationFile->getDataReference().trackfinderThetaMax,
									  configurationFile->getDataReference().trackfinderThetaStep,
									  configurationFile->getDataReference().trackfinderGammaMin,
									  configurationFile->getDataReference().trackfinderGammaMax,
									  configurationFile->getDataReference().trackfinderGammaStep);
		histogram              = new histogramData(&space);
		tracks                 = new trackData(&space);
		ratings                = new tables();
		luts                   = new lutImplementation();
		houghTrackfinder       = new trackfinder(&eventData, &histogram, &tracks, &ratings, &luts,
									  configurationFile->getDataReference().trackfinderFilterType,
									  configurationFile->getDataReference().trackfinderAutomaticFilterDataPercentage,
									  configurationFile->getDataReference().trackfinderAutomaticFilterFileName,
									  configurationFile->getDataReference().trackfinderFirstFilterGeometry,
									  configurationFile->getDataReference().trackfinderFirstFilterArithmetic,
									  configurationFile->getDataReference().trackfinderSecondFilterGeometry,
									  configurationFile->getDataReference().trackfinderSecondFilterArithmetic,
									  configurationFile->getDataReference().trackfinderFirstFilterNeighborhoodDim1ClearRadius,
									  configurationFile->getDataReference().trackfinderFirstFilterNeighborhoodDim2ClearRadius,
									  configurationFile->getDataReference().trackfinderSecondFilterNeighborhoodDim1ClearRadius,
									  configurationFile->getDataReference().trackfinderSecondFilterNeighborhoodDim2ClearRadius,
									  configurationFile->getDataReference().trackfinderSecondFilterNeighborhoodDim3ClearRadius);
		output                 = new outputFTrack(&tracks, configurationFile->getDataReference().trackfinderWriteTracksToFile);

#ifndef NOANALYSIS

		analysisParameters.eventData                       = &eventData;
		analysisParameters.histogram                       = &histogram;
		analysisParameters.tracks                          = &tracks;
		analysisParameters.ratings                         = &ratings;
		analysisParameters.space                           = &space;
		analysisParameters.luts                            = &luts;
		analysisParameters.minP                            = configurationFile->getDataReference().analysisThresholdForP;
		analysisParameters.initAutomatcFilterGeometry      = houghTrackfinder->isAutomaticFilterGeometryEnabled();
		analysisParameters.initConfiguration               = configurationFile->getDataReference().analysisInitConfiguration;
		analysisParameters.initDetector                    = configurationFile->getDataReference().analysisInitDetector;
		analysisParameters.initEvent                       = configurationFile->getDataReference().analysisInitEvent;
		analysisParameters.initClassPriority               = configurationFile->getDataReference().analysisInitClassPriority;
		analysisParameters.initMemory                      = configurationFile->getDataReference().analysisInitMemory;
		analysisParameters.initTime                        = configurationFile->getDataReference().analysisInitTime;
		analysisParameters.initPropagationEventPoint       = configurationFile->getDataReference().analysisInitTrackPropagationEventPoint;
		analysisParameters.initPropagationEventHit         = configurationFile->getDataReference().analysisInitTrackPropagationEventHit;
		analysisParameters.initPropagationTotalPoint       = configurationFile->getDataReference().analysisInitTrackPropagationTotalPoint;
		analysisParameters.initPropagationTotalHit         = configurationFile->getDataReference().analysisInitTrackPropagationTotalHit;
		analysisParameters.initPropagationDisplay          = configurationFile->getDataReference().analysisInitTrackPropagationDisplay;
		analysisParameters.initQualityEFGCEventAbsolute    = configurationFile->getDataReference().analysisInitQualityEFGCEventAbsolute;
		analysisParameters.initQualityEFGCEventRelative    = configurationFile->getDataReference().analysisInitQualityEFGCEventRelative;
		analysisParameters.initQualityEFGCTotalAbsolute    = configurationFile->getDataReference().analysisInitQualityEFGCTotalAbsolute;
		analysisParameters.initQualityEFGCTotalRelative    = configurationFile->getDataReference().analysisInitQualityEFGCTotalRelative;
		analysisParameters.initMomentumEFGCEventPzEFGC     = configurationFile->getDataReference().analysisInitMomentumEFGCEventPzEFGC;
		analysisParameters.initMomentumEFGCEventPtEFGC     = configurationFile->getDataReference().analysisInitMomentumEFGCEventPtEFGC;
		analysisParameters.initMomentumEFGCTotalPzEFGC     = configurationFile->getDataReference().analysisInitMomentumEFGCTotalPzEFGC;
		analysisParameters.initMomentumEFGCTotalPtEFGC     = configurationFile->getDataReference().analysisInitMomentumEFGCTotalPtEFGC;
		analysisParameters.initProjectionEFGCNEvent12EFGCN = configurationFile->getDataReference().analysisInitProjectionEFGCNEvent12EFGCN;
		analysisParameters.initProjectionEFGCNEvent13EFGCN = configurationFile->getDataReference().analysisInitProjectionEFGCNEvent13EFGCN;
		analysisParameters.initProjectionEFGCNEvent32EFGCN = configurationFile->getDataReference().analysisInitProjectionEFGCNEvent32EFGCN;
		analysisParameters.initProjectionEFGCNTotal12EFGCN = configurationFile->getDataReference().analysisInitProjectionEFGCNTotal12EFGCN;
		analysisParameters.initProjectionEFGCNTotal13EFGCN = configurationFile->getDataReference().analysisInitProjectionEFGCNTotal13EFGCN;
		analysisParameters.initProjectionEFGCNTotal32EFGCN = configurationFile->getDataReference().analysisInitProjectionEFGCNTotal32EFGCN;
		analysisParameters.initMomentumEvent               = configurationFile->getDataReference().analysisInitMomentumEvent;
		analysisParameters.initMomentumTotal               = configurationFile->getDataReference().analysisInitMomentumTotal;
		analysisParameters.initMomentumDisplay             = configurationFile->getDataReference().analysisInitMomentumDisplay;
		analysisParameters.initProjectionEvent             = configurationFile->getDataReference().analysisInitProjectionEvent;
		analysisParameters.initProjectionTotal             = configurationFile->getDataReference().analysisInitProjectionTotal;
		analysisParameters.initProjectionDisplay           = configurationFile->getDataReference().analysisInitProjectionDisplay;
		analysisParameters.initMagnetfieldFactors          = configurationFile->getDataReference().analysisInitMagnetfieldConstantForEachEvent || configurationFile->getDataReference().analysisInitWeightedMagnetfieldConstant;
		analysisParameters.initMagnetfieldFactorDisplay    = configurationFile->getDataReference().analysisInitMagnetfieldConstantDisplay;
		analysisParameters.initMagnetfieldX                = configurationFile->getDataReference().analysisInitMagnetfieldX;
		analysisParameters.initMagnetfieldY                = configurationFile->getDataReference().analysisInitMagnetfieldY;
		analysisParameters.initMagnetfieldZ                = configurationFile->getDataReference().analysisInitMagnetfieldZ;
		analysisParameters.initMagnetfieldDisplay          = configurationFile->getDataReference().analysisInitMagnetfieldDisplay;
		analysisParameters.initTracksPerColumn             = configurationFile->getDataReference().analysisInitNumberOfTracksPerColumn;
		analysisParameters.initTracksPerRow                = configurationFile->getDataReference().analysisInitNumberOfTracksPerRow;
		analysisParameters.initTracksPerLayer              = configurationFile->getDataReference().analysisInitNumberOfTracksPerLayer;
		analysisParameters.initHitReadoutDistribution      = configurationFile->getDataReference().analysisInitHitReadoutDistribution;
		analysisParameters.initPrelutRange                 = configurationFile->getDataReference().analysisInitPrelutRangeForEachEvent || configurationFile->getDataReference().analysisInitWeightedPrelutRange;
		analysisParameters.initPrelutRangeDisplay          = configurationFile->getDataReference().analysisInitPrelutRangeDisplay;
		analysisParameters.initPrelutRangeDisplayMode      = configurationFile->getDataReference().analysisInitPrelutRangeDisplayMode;
		analysisParameters.percentageOfHitsInSignature     = configurationFile->getDataReference().analysisInitPercentageOfHitsInSignature;
		analysisParameters.percentageOfTracksForSignature  = configurationFile->getDataReference().analysisInitPercentageOfTracksForSignature;
		analysisParameters.analysisResultWarnings          = configurationFile->getDataReference().analysisInitAnalysisResultWarnings;
		analysisParameters.analysisResultDisplays          = configurationFile->getDataReference().analysisInitAnalysisResultDisplays;
		analysisParameters.analysisMoreResultWarnings      = configurationFile->getDataReference().analysisInitAnalysisMoreResultWarnings;
		analysisParameters.analysisMoreResultDisplays      = configurationFile->getDataReference().analysisInitAnalysisMoreResultDisplays;
		analyser               = new analysis(analysisParameters,
									  configurationFile->getDataReference().analysisInitCreatedHistogramToRoot || configurationFile->getDataReference().analysisInitEncodedHistogramToRoot || configurationFile->getDataReference().analysisInitFilteredHistogramToRoot);

#else

		if ((configurationFile->getDataReference().inputCodingTableMode == LUTGOODNESSTABLE) ||
			(configurationFile->getDataReference().inputGradingPTableMode == LUTGOODNESSTABLE) ||
			(configurationFile->getDataReference().inputGradingRTableMode == LUTGOODNESSTABLE) ||
			houghTrackfinder->isAutomaticFilterGeometryEnabled() ||
			configurationFile->getDataReference().analysisInitConfiguration ||
			configurationFile->getDataReference().analysisInitDetector ||
			configurationFile->getDataReference().analysisInitEvent ||
			configurationFile->getDataReference().analysisInitClassPriority ||
			configurationFile->getDataReference().analysisInitMemory ||
			configurationFile->getDataReference().analysisInitTime ||
			configurationFile->getDataReference().analysisInitTrackPropagationEventPoint ||
			configurationFile->getDataReference().analysisInitTrackPropagationEventHit ||
			configurationFile->getDataReference().analysisInitTrackPropagationTotalPoint ||
			configurationFile->getDataReference().analysisInitTrackPropagationTotalHit ||
			configurationFile->getDataReference().analysisInitTrackPropagationDisplay ||
			configurationFile->getDataReference().analysisInitTrackPropagationToRoot ||
			configurationFile->getDataReference().analysisInitQualityEFGCEventAbsolute ||
			configurationFile->getDataReference().analysisInitQualityEFGCEventRelative ||
			configurationFile->getDataReference().analysisInitQualityEFGCTotalAbsolute ||
			configurationFile->getDataReference().analysisInitQualityEFGCTotalRelative ||
			configurationFile->getDataReference().analysisInitMomentumEFGCEventPzEFGC ||
			configurationFile->getDataReference().analysisInitMomentumEFGCEventPtEFGC ||
			configurationFile->getDataReference().analysisInitMomentumEFGCTotalPzEFGC ||
			configurationFile->getDataReference().analysisInitMomentumEFGCTotalPtEFGC ||
			configurationFile->getDataReference().analysisInitProjectionEFGCNEvent12EFGCN ||
			configurationFile->getDataReference().analysisInitProjectionEFGCNEvent13EFGCN ||
			configurationFile->getDataReference().analysisInitProjectionEFGCNEvent32EFGCN ||
			configurationFile->getDataReference().analysisInitProjectionEFGCNTotal12EFGCN ||
			configurationFile->getDataReference().analysisInitProjectionEFGCNTotal13EFGCN ||
			configurationFile->getDataReference().analysisInitProjectionEFGCNTotal32EFGCN ||
			configurationFile->getDataReference().analysisInitMomentumEvent ||
			configurationFile->getDataReference().analysisInitMomentumTotal ||
			configurationFile->getDataReference().analysisInitMomentumDisplay ||
			configurationFile->getDataReference().analysisInitMomentumToRoot ||
			configurationFile->getDataReference().analysisInitProjectionEvent ||
			configurationFile->getDataReference().analysisInitProjectionTotal ||
			configurationFile->getDataReference().analysisInitProjectionDisplay ||
			configurationFile->getDataReference().analysisInitProjectionToRoot ||
			configurationFile->getDataReference().analysisInitMagnetfieldConstantForEachEvent ||
			configurationFile->getDataReference().analysisInitWeightedMagnetfieldConstant ||
			configurationFile->getDataReference().analysisInitMagnetfieldConstantDisplay ||
			configurationFile->getDataReference().analysisInitMagnetfieldX ||
			configurationFile->getDataReference().analysisInitMagnetfieldY ||
			configurationFile->getDataReference().analysisInitMagnetfieldZ ||
			configurationFile->getDataReference().analysisInitMagnetfieldDisplay ||
			configurationFile->getDataReference().analysisInitMagnetfieldToRoot ||
			configurationFile->getDataReference().analysisInitCreatedHistogramToRoot ||
			configurationFile->getDataReference().analysisInitEncodedHistogramToRoot ||
			configurationFile->getDataReference().analysisInitFilteredHistogramToRoot ||
			configurationFile->getDataReference().analysisInitJustOneCreatedHistogramToRoot ||
			configurationFile->getDataReference().analysisInitJustOneEncodedHistogramToRoot ||
			configurationFile->getDataReference().analysisInitJustOneFilteredHistogramToRoot ||
			configurationFile->getDataReference().analysisInitCreatedHistogramToShow ||
			configurationFile->getDataReference().analysisInitEncodedHistogramToShow ||
			configurationFile->getDataReference().analysisInitFilteredHistogramToShow ||
			configurationFile->getDataReference().analysisInitNumberOfTracksPerColumn ||
			configurationFile->getDataReference().analysisInitNumberOfTracksPerRow ||
			configurationFile->getDataReference().analysisInitNumberOfTracksPerLayer ||
			configurationFile->getDataReference().analysisInitHitReadoutDistribution ||
			configurationFile->getDataReference().analysisInitReadoutColumnsInParallel ||
			configurationFile->getDataReference().analysisInitPrelutRangeForEachEvent ||
			configurationFile->getDataReference().analysisInitWeightedPrelutRange ||
			configurationFile->getDataReference().analysisInitPrelutRangeDisplay ||
			configurationFile->getDataReference().analysisInitPrelutRangeToRoot ||
			configurationFile->getDataReference().analysisChooseMainPrelutRange ||
			configurationFile->getDataReference().analysisChooseConstraintPrelutRange ||
			configurationFile->getDataReference().analysisInitTotalAnalysis ||
			(configurationFile->getDataReference().analysisInitAnalysisResultWarnings != 0) ||
			(configurationFile->getDataReference().analysisInitAnalysisResultDisplays != 0) ||
			(configurationFile->getDataReference().analysisInitAnalysisMoreResultWarnings != 0) ||
			(configurationFile->getDataReference().analysisInitAnalysisMoreResultDisplays != 0) ||
			configurationFile->getDataReference().analysisWriteCellFiles) {

				std::cout << "WARNING: The anaylsis which is enabled cannot be done because it is not added at compilation!!!" << std::endl;

		}

#endif

		setAnalysisOutputFileName.clear();

	}
	catch(errorHandling& error) {
		error.errorMsg();
	    Fatal("CbmHoughStsTrackFinder", "Fatal Error!!!");
	}
	catch(...) {
		std::cout << "Unknown error during 'CbmHoughStsTrackFinder'-object creation" << std::endl;
	    Fatal("CbmHoughStsTrackFinder", "Fatal Error!!!");
	}

}
CbmHoughStsTrackFinder::CbmHoughStsTrackFinder(const char* name,
						bitArray inputDetectorMask,
						int inputMinTracks, int inputMaxTracks,
						unsigned short inputMagneticFieldIntegrationStepwidthPerStation,
						int trackfinderGammaStep,
						int trackfinderThetaStep, int trackfinderLutRadiusStep,
						unsigned short trackfinderFirstFilterNeighborhoodDim1ClearRadius,
						unsigned short trackfinderFirstFilterNeighborhoodDim2ClearRadius,
						unsigned short trackfinderSecondFilterNeighborhoodDim1ClearRadius,
						unsigned short trackfinderSecondFilterNeighborhoodDim2ClearRadius,
						unsigned short trackfinderSecondFilterNeighborhoodDim3ClearRadius,
						bool initStatus, const char* analysisOutputFileName) 
  : CbmStsTrackFinder(),
    eventNumber(0),	       
    configurationFile(NULL),       
    isFirstEvent(false),
    input(NULL),	       
    space(NULL),       
    eventData(NULL),       
    histogram(NULL),	       
    tracks(NULL),
    ratings(NULL),       
    luts(NULL),       
    houghTrackfinder(NULL),       
    output(NULL),  
#ifndef NOANALYSIS
    setAnalysisOutputFileName(),
    analyser(NULL)                   
#else
    setAnalysisOutputFileName()
#endif
 {

	initialParameter analysisParameters;

	try {
		configurationFile      = new inf(name);
		configurationFile->getDataReference().inputMinTracks                                     = inputMinTracks;
		configurationFile->getDataReference().inputMaxTracks                                     = inputMaxTracks;
		configurationFile->getDataReference().inputDetectorMask                                  = inputDetectorMask;
		configurationFile->getDataReference().inputMagneticFieldIntegrationStepwidthPerStation   = inputMagneticFieldIntegrationStepwidthPerStation;
		configurationFile->getDataReference().trackfinderGammaStep                               = trackfinderGammaStep;
		configurationFile->getDataReference().trackfinderThetaStep                               = trackfinderThetaStep;
		configurationFile->getDataReference().trackfinderLutRadiusStep                           = trackfinderLutRadiusStep;
		configurationFile->getDataReference().trackfinderFirstFilterNeighborhoodDim1ClearRadius  = trackfinderFirstFilterNeighborhoodDim1ClearRadius;
		configurationFile->getDataReference().trackfinderFirstFilterNeighborhoodDim2ClearRadius  = trackfinderFirstFilterNeighborhoodDim2ClearRadius;
		configurationFile->getDataReference().trackfinderSecondFilterNeighborhoodDim1ClearRadius = trackfinderSecondFilterNeighborhoodDim1ClearRadius;
		configurationFile->getDataReference().trackfinderSecondFilterNeighborhoodDim2ClearRadius = trackfinderSecondFilterNeighborhoodDim2ClearRadius;
		configurationFile->getDataReference().trackfinderSecondFilterNeighborhoodDim3ClearRadius = trackfinderSecondFilterNeighborhoodDim3ClearRadius;
		configurationFile->getDataReference().initStatus                                         = initStatus;
		configurationFile->getDataReference().analysisOutputFileName                             = analysisOutputFileName;
		switch (configurationFile->getDataReference().inputFileFormat) {
			case ASCIIFILEFORMAT:
				input          = new inputAscii(configurationFile->getDataReference().inputFileName.c_str(),
									  configurationFile->getDataReference().inputDetectorMask,
					                  configurationFile->getDataReference().trackfinderHitProducer);
				std::cerr << std::endl << "CbmHoughStsTrackFinder WARNING: The eventnumber would always start with zero, if the input is from an ascii file!!!" << std::endl;
				break;
			default:	// ROOTFILEFORMAT
				input          = new inputFRoot(configurationFile->getDataReference().inputFileNameDetector.c_str(),
									  configurationFile->getDataReference().inputNumberOfVolumesInfrontOfSTS,
									  configurationFile->getDataReference().inputDisableAutomaticDetector,
									  configurationFile->getDataReference().inputDetectorMask,
									  configurationFile->getDataReference().trackfinderReadPointsFromFile,
									  configurationFile->getDataReference().trackfinderReadHitsFromFile,
									  configurationFile->getDataReference().trackfinderReadMapsHits,
									  configurationFile->getDataReference().trackfinderReadHybridHits,
									  configurationFile->getDataReference().trackfinderReadStripHits);
				break;
		}
		space                  = new histogramSpace(configurationFile->getDataReference().trackfinderLutRadiusMin,
									  configurationFile->getDataReference().trackfinderLutRadiusMax,
									  configurationFile->getDataReference().trackfinderLutRadiusStep,
			                          configurationFile->getDataReference().trackfinderThetaMin,
									  configurationFile->getDataReference().trackfinderThetaMax,
									  configurationFile->getDataReference().trackfinderThetaStep,
									  configurationFile->getDataReference().trackfinderGammaMin,
									  configurationFile->getDataReference().trackfinderGammaMax,
									  configurationFile->getDataReference().trackfinderGammaStep);
		histogram              = new histogramData(&space);
		tracks                 = new trackData(&space);
		ratings                = new tables();
		luts                   = new lutImplementation();
		houghTrackfinder       = new trackfinder(&eventData, &histogram, &tracks, &ratings, &luts,
									  configurationFile->getDataReference().trackfinderFilterType,
									  configurationFile->getDataReference().trackfinderAutomaticFilterDataPercentage,
									  configurationFile->getDataReference().trackfinderAutomaticFilterFileName,
									  configurationFile->getDataReference().trackfinderFirstFilterGeometry,
									  configurationFile->getDataReference().trackfinderFirstFilterArithmetic,
									  configurationFile->getDataReference().trackfinderSecondFilterGeometry,
									  configurationFile->getDataReference().trackfinderSecondFilterArithmetic,
									  configurationFile->getDataReference().trackfinderFirstFilterNeighborhoodDim1ClearRadius,
									  configurationFile->getDataReference().trackfinderFirstFilterNeighborhoodDim2ClearRadius,
									  configurationFile->getDataReference().trackfinderSecondFilterNeighborhoodDim1ClearRadius,
									  configurationFile->getDataReference().trackfinderSecondFilterNeighborhoodDim2ClearRadius,
									  configurationFile->getDataReference().trackfinderSecondFilterNeighborhoodDim3ClearRadius);
		output                 = new outputFTrack(&tracks, configurationFile->getDataReference().trackfinderWriteTracksToFile);

#ifndef NOANALYSIS

		analysisParameters.eventData                       = &eventData;
		analysisParameters.histogram                       = &histogram;
		analysisParameters.tracks                          = &tracks;
		analysisParameters.ratings                         = &ratings;
		analysisParameters.space                           = &space;
		analysisParameters.luts                            = &luts;
		analysisParameters.minP                            = configurationFile->getDataReference().analysisThresholdForP;
		analysisParameters.initAutomatcFilterGeometry      = houghTrackfinder->isAutomaticFilterGeometryEnabled();
		analysisParameters.initConfiguration               = configurationFile->getDataReference().analysisInitConfiguration;
		analysisParameters.initDetector                    = configurationFile->getDataReference().analysisInitDetector;
		analysisParameters.initEvent                       = configurationFile->getDataReference().analysisInitEvent;
		analysisParameters.initClassPriority               = configurationFile->getDataReference().analysisInitClassPriority;
		analysisParameters.initMemory                      = configurationFile->getDataReference().analysisInitMemory;
		analysisParameters.initTime                        = configurationFile->getDataReference().analysisInitTime;
		analysisParameters.initPropagationEventPoint       = configurationFile->getDataReference().analysisInitTrackPropagationEventPoint;
		analysisParameters.initPropagationEventHit         = configurationFile->getDataReference().analysisInitTrackPropagationEventHit;
		analysisParameters.initPropagationTotalPoint       = configurationFile->getDataReference().analysisInitTrackPropagationTotalPoint;
		analysisParameters.initPropagationTotalHit         = configurationFile->getDataReference().analysisInitTrackPropagationTotalHit;
		analysisParameters.initPropagationDisplay          = configurationFile->getDataReference().analysisInitTrackPropagationDisplay;
		analysisParameters.initQualityEFGCEventAbsolute    = configurationFile->getDataReference().analysisInitQualityEFGCEventAbsolute;
		analysisParameters.initQualityEFGCEventRelative    = configurationFile->getDataReference().analysisInitQualityEFGCEventRelative;
		analysisParameters.initQualityEFGCTotalAbsolute    = configurationFile->getDataReference().analysisInitQualityEFGCTotalAbsolute;
		analysisParameters.initQualityEFGCTotalRelative    = configurationFile->getDataReference().analysisInitQualityEFGCTotalRelative;
		analysisParameters.initMomentumEFGCEventPzEFGC     = configurationFile->getDataReference().analysisInitMomentumEFGCEventPzEFGC;
		analysisParameters.initMomentumEFGCEventPtEFGC     = configurationFile->getDataReference().analysisInitMomentumEFGCEventPtEFGC;
		analysisParameters.initMomentumEFGCTotalPzEFGC     = configurationFile->getDataReference().analysisInitMomentumEFGCTotalPzEFGC;
		analysisParameters.initMomentumEFGCTotalPtEFGC     = configurationFile->getDataReference().analysisInitMomentumEFGCTotalPtEFGC;
		analysisParameters.initProjectionEFGCNEvent12EFGCN = configurationFile->getDataReference().analysisInitProjectionEFGCNEvent12EFGCN;
		analysisParameters.initProjectionEFGCNEvent13EFGCN = configurationFile->getDataReference().analysisInitProjectionEFGCNEvent13EFGCN;
		analysisParameters.initProjectionEFGCNEvent32EFGCN = configurationFile->getDataReference().analysisInitProjectionEFGCNEvent32EFGCN;
		analysisParameters.initProjectionEFGCNTotal12EFGCN = configurationFile->getDataReference().analysisInitProjectionEFGCNTotal12EFGCN;
		analysisParameters.initProjectionEFGCNTotal13EFGCN = configurationFile->getDataReference().analysisInitProjectionEFGCNTotal13EFGCN;
		analysisParameters.initProjectionEFGCNTotal32EFGCN = configurationFile->getDataReference().analysisInitProjectionEFGCNTotal32EFGCN;
		analysisParameters.initMomentumEvent               = configurationFile->getDataReference().analysisInitMomentumEvent;
		analysisParameters.initMomentumTotal               = configurationFile->getDataReference().analysisInitMomentumTotal;
		analysisParameters.initMomentumDisplay             = configurationFile->getDataReference().analysisInitMomentumDisplay;
		analysisParameters.initProjectionEvent             = configurationFile->getDataReference().analysisInitProjectionEvent;
		analysisParameters.initProjectionTotal             = configurationFile->getDataReference().analysisInitProjectionTotal;
		analysisParameters.initProjectionDisplay           = configurationFile->getDataReference().analysisInitProjectionDisplay;
		analysisParameters.initMagnetfieldFactors          = configurationFile->getDataReference().analysisInitMagnetfieldConstantForEachEvent || configurationFile->getDataReference().analysisInitWeightedMagnetfieldConstant;
		analysisParameters.initMagnetfieldFactorDisplay    = configurationFile->getDataReference().analysisInitMagnetfieldConstantDisplay;
		analysisParameters.initMagnetfieldX                = configurationFile->getDataReference().analysisInitMagnetfieldX;
		analysisParameters.initMagnetfieldY                = configurationFile->getDataReference().analysisInitMagnetfieldY;
		analysisParameters.initMagnetfieldZ                = configurationFile->getDataReference().analysisInitMagnetfieldZ;
		analysisParameters.initMagnetfieldDisplay          = configurationFile->getDataReference().analysisInitMagnetfieldDisplay;
		analysisParameters.initTracksPerColumn             = configurationFile->getDataReference().analysisInitNumberOfTracksPerColumn;
		analysisParameters.initTracksPerRow                = configurationFile->getDataReference().analysisInitNumberOfTracksPerRow;
		analysisParameters.initTracksPerLayer              = configurationFile->getDataReference().analysisInitNumberOfTracksPerLayer;
		analysisParameters.initHitReadoutDistribution      = configurationFile->getDataReference().analysisInitHitReadoutDistribution;
		analysisParameters.initPrelutRange                 = configurationFile->getDataReference().analysisInitPrelutRangeForEachEvent || configurationFile->getDataReference().analysisInitWeightedPrelutRange;
		analysisParameters.initPrelutRangeDisplay          = configurationFile->getDataReference().analysisInitPrelutRangeDisplay;
		analysisParameters.initPrelutRangeDisplayMode      = configurationFile->getDataReference().analysisInitPrelutRangeDisplayMode;
		analysisParameters.percentageOfHitsInSignature     = configurationFile->getDataReference().analysisInitPercentageOfHitsInSignature;
		analysisParameters.percentageOfTracksForSignature  = configurationFile->getDataReference().analysisInitPercentageOfTracksForSignature;
		analysisParameters.analysisResultWarnings          = configurationFile->getDataReference().analysisInitAnalysisResultWarnings;
		analysisParameters.analysisResultDisplays          = configurationFile->getDataReference().analysisInitAnalysisResultDisplays;
		analysisParameters.analysisMoreResultWarnings      = configurationFile->getDataReference().analysisInitAnalysisMoreResultWarnings;
		analysisParameters.analysisMoreResultDisplays      = configurationFile->getDataReference().analysisInitAnalysisMoreResultDisplays;
		analyser               = new analysis(analysisParameters,
									  configurationFile->getDataReference().analysisInitCreatedHistogramToRoot || configurationFile->getDataReference().analysisInitEncodedHistogramToRoot || configurationFile->getDataReference().analysisInitFilteredHistogramToRoot);

#else

		if ((configurationFile->getDataReference().inputCodingTableMode == LUTGOODNESSTABLE) ||
			(configurationFile->getDataReference().inputGradingPTableMode == LUTGOODNESSTABLE) ||
			(configurationFile->getDataReference().inputGradingRTableMode == LUTGOODNESSTABLE) ||
			houghTrackfinder->isAutomaticFilterGeometryEnabled() ||
			configurationFile->getDataReference().analysisInitConfiguration ||
			configurationFile->getDataReference().analysisInitDetector ||
			configurationFile->getDataReference().analysisInitEvent ||
			configurationFile->getDataReference().analysisInitClassPriority ||
			configurationFile->getDataReference().analysisInitMemory ||
			configurationFile->getDataReference().analysisInitTime ||
			configurationFile->getDataReference().analysisInitTrackPropagationEventPoint ||
			configurationFile->getDataReference().analysisInitTrackPropagationEventHit ||
			configurationFile->getDataReference().analysisInitTrackPropagationTotalPoint ||
			configurationFile->getDataReference().analysisInitTrackPropagationTotalHit ||
			configurationFile->getDataReference().analysisInitTrackPropagationDisplay ||
			configurationFile->getDataReference().analysisInitTrackPropagationToRoot ||
			configurationFile->getDataReference().analysisInitQualityEFGCEventAbsolute ||
			configurationFile->getDataReference().analysisInitQualityEFGCEventRelative ||
			configurationFile->getDataReference().analysisInitQualityEFGCTotalAbsolute ||
			configurationFile->getDataReference().analysisInitQualityEFGCTotalRelative ||
			configurationFile->getDataReference().analysisInitMomentumEFGCEventPzEFGC ||
			configurationFile->getDataReference().analysisInitMomentumEFGCEventPtEFGC ||
			configurationFile->getDataReference().analysisInitMomentumEFGCTotalPzEFGC ||
			configurationFile->getDataReference().analysisInitMomentumEFGCTotalPtEFGC ||
			configurationFile->getDataReference().analysisInitProjectionEFGCNEvent12EFGCN ||
			configurationFile->getDataReference().analysisInitProjectionEFGCNEvent13EFGCN ||
			configurationFile->getDataReference().analysisInitProjectionEFGCNEvent32EFGCN ||
			configurationFile->getDataReference().analysisInitProjectionEFGCNTotal12EFGCN ||
			configurationFile->getDataReference().analysisInitProjectionEFGCNTotal13EFGCN ||
			configurationFile->getDataReference().analysisInitProjectionEFGCNTotal32EFGCN ||
			configurationFile->getDataReference().analysisInitMomentumEvent ||
			configurationFile->getDataReference().analysisInitMomentumTotal ||
			configurationFile->getDataReference().analysisInitMomentumDisplay ||
			configurationFile->getDataReference().analysisInitMomentumToRoot ||
			configurationFile->getDataReference().analysisInitProjectionEvent ||
			configurationFile->getDataReference().analysisInitProjectionTotal ||
			configurationFile->getDataReference().analysisInitProjectionDisplay ||
			configurationFile->getDataReference().analysisInitProjectionToRoot ||
			configurationFile->getDataReference().analysisInitMagnetfieldConstantForEachEvent ||
			configurationFile->getDataReference().analysisInitWeightedMagnetfieldConstant ||
			configurationFile->getDataReference().analysisInitMagnetfieldConstantDisplay ||
			configurationFile->getDataReference().analysisInitMagnetfieldX ||
			configurationFile->getDataReference().analysisInitMagnetfieldY ||
			configurationFile->getDataReference().analysisInitMagnetfieldZ ||
			configurationFile->getDataReference().analysisInitMagnetfieldDisplay ||
			configurationFile->getDataReference().analysisInitMagnetfieldToRoot ||
			configurationFile->getDataReference().analysisInitCreatedHistogramToRoot ||
			configurationFile->getDataReference().analysisInitEncodedHistogramToRoot ||
			configurationFile->getDataReference().analysisInitFilteredHistogramToRoot ||
			configurationFile->getDataReference().analysisInitJustOneCreatedHistogramToRoot ||
			configurationFile->getDataReference().analysisInitJustOneEncodedHistogramToRoot ||
			configurationFile->getDataReference().analysisInitJustOneFilteredHistogramToRoot ||
			configurationFile->getDataReference().analysisInitCreatedHistogramToShow ||
			configurationFile->getDataReference().analysisInitEncodedHistogramToShow ||
			configurationFile->getDataReference().analysisInitFilteredHistogramToShow ||
			configurationFile->getDataReference().analysisInitNumberOfTracksPerColumn ||
			configurationFile->getDataReference().analysisInitNumberOfTracksPerRow ||
			configurationFile->getDataReference().analysisInitNumberOfTracksPerLayer ||
			configurationFile->getDataReference().analysisInitHitReadoutDistribution ||
			configurationFile->getDataReference().analysisInitReadoutColumnsInParallel ||
			configurationFile->getDataReference().analysisInitPrelutRangeForEachEvent ||
			configurationFile->getDataReference().analysisInitWeightedPrelutRange ||
			configurationFile->getDataReference().analysisInitPrelutRangeDisplay ||
			configurationFile->getDataReference().analysisInitPrelutRangeToRoot ||
			configurationFile->getDataReference().analysisChooseMainPrelutRange ||
			configurationFile->getDataReference().analysisChooseConstraintPrelutRange ||
			configurationFile->getDataReference().analysisInitTotalAnalysis ||
			(configurationFile->getDataReference().analysisInitAnalysisResultWarnings != 0) ||
			(configurationFile->getDataReference().analysisInitAnalysisResultDisplays != 0) ||
			(configurationFile->getDataReference().analysisInitAnalysisMoreResultWarnings != 0) ||
			(configurationFile->getDataReference().analysisInitAnalysisMoreResultDisplays != 0) ||
			configurationFile->getDataReference().analysisWriteCellFiles) {

				std::cout << "WARNING: The anaylsis which is enabled cannot be done because it is not added at compilation!!!" << std::endl;

		}

#endif

		setAnalysisOutputFileName.clear();

	}
	catch(errorHandling& error) {
		error.errorMsg();
	    Fatal("CbmHoughStsTrackFinder", "Fatal Error!!!");
	}
	catch(...) {
		std::cout << "Unknown error during 'CbmHoughStsTrackFinder'-object creation" << std::endl;
	    Fatal("CbmHoughStsTrackFinder", "Fatal Error!!!");
	}

}

/****************************************************************
 * destructor													*
 ****************************************************************/

CbmHoughStsTrackFinder::~CbmHoughStsTrackFinder() {

#ifndef NOANALYSIS

	if(analyser != NULL) {

		delete analyser;
		analyser = NULL;
	}

#endif

	if (output != NULL) {
		delete output;
		output = NULL;
	}
	if (houghTrackfinder != NULL) {
		delete houghTrackfinder;
		houghTrackfinder = NULL;
	}
	if (luts != NULL) {
		delete luts;
		luts = NULL;
	}
	if(tracks != NULL) {
	
		delete tracks;
		tracks = NULL;

	}
	if(histogram != NULL) {

		delete histogram;
		histogram = NULL;

	}
	if (input != NULL) {
		delete input;
		input = NULL;
	}
	if (configurationFile != NULL) {
		delete configurationFile;
		configurationFile = NULL;
	}

}

/****************************************************************
 * This method sets the output file.			 				*
 ****************************************************************/

void CbmHoughStsTrackFinder::SetOutputFile(const char* name) {

#ifndef NOANALYSIS

	setAnalysisOutputFileName = name;

#endif

}

/****************************************************************
 * This method shows the specified histogram layer if possible.	*
 ****************************************************************/

void CbmHoughStsTrackFinder::ShowHistogramLayer(unsigned int _eventNumber, unsigned short layer, const char* drawOption) {

#ifndef NOANALYSIS

	if (analyser != NULL) {

		if (analyser->isShowingCreatedHistogramLayerEnabled())
			analyser->createdHistogramLayerShow(_eventNumber, layer, drawOption);
		if (analyser->isShowingEncodedHistogramLayerEnabled())
			analyser->encodedHistogramLayerShow(_eventNumber, layer, drawOption);
		if (analyser->isShowingFilteredHistogramLayerEnabled())
			analyser->filteredHistogramLayerShow(_eventNumber, layer, drawOption);

	}
	else {

		std::cout << "WARNING: The anaylsis which is enabled cannot be done because the analysing object is not created yet!!!" << std::endl;

	}

#else

	std::cout << "WARNING: The anaylsis which is enabled cannot be done because it is not added at compilation!!!" << std::endl;

#endif

}

/****************************************************************
 * This method initializes the object.			 				*
 ****************************************************************/

void CbmHoughStsTrackFinder::Init() {

	std::streambuf* terminal;

	eventNumber = 0;
	try {

		if ((configurationFile->getDataReference().initStatus) && (fVerbose > 0))
			terminal = std::cout.rdbuf();
		else
			terminal = NULL;

		isFirstEvent = true;

		input->init();
		output->init();

#ifndef NOANALYSIS

		ratings->setAnalyser(analyser);
		houghTrackfinder->setAnalyser(analyser);

		analyser->initPrelutRangeAnalysis(configurationFile->getDataReference().analysisInitPrelutRangeForEachEvent || configurationFile->getDataReference().analysisInitWeightedPrelutRange, configurationFile->getDataReference().analysisInitPercentageOfHitsForPrelutRange,
										configurationFile->getDataReference().analysisPrelutRangeMinSteps, configurationFile->getDataReference().analysisPrelutRangeMinStart, configurationFile->getDataReference().analysisPrelutRangeMinStop,
										configurationFile->getDataReference().analysisPrelutRangeMaxSteps, configurationFile->getDataReference().analysisPrelutRangeMaxStart, configurationFile->getDataReference().analysisPrelutRangeMaxStop);

		if (setAnalysisOutputFileName.empty()) {
			analyser->initTrackPropagationAnalysisToRoot(configurationFile->getDataReference().analysisInitTrackPropagationToRoot, configurationFile->getDataReference().analysisOutputFileName.c_str());
			analyser->initMomentumAnalysisToRoot(configurationFile->getDataReference().analysisInitMomentumToRoot, configurationFile->getDataReference().analysisOutputFileName.c_str());
			analyser->initProjectionAnalysisToRoot(configurationFile->getDataReference().analysisInitProjectionToRoot, configurationFile->getDataReference().analysisOutputFileName.c_str());
			analyser->initMagnetfieldAnalysisToRoot(configurationFile->getDataReference().analysisInitMagnetfieldToRoot, configurationFile->getDataReference().analysisOutputFileName.c_str());
			analyser->initMagnetfieldConstantAnalysisToRoot(configurationFile->getDataReference().analysisInitMagnetfieldConstantToRoot, configurationFile->getDataReference().analysisOutputFileName.c_str());
			analyser->initHistogramAnalysis(configurationFile->getDataReference().analysisInitCreatedHistogramToRoot, configurationFile->getDataReference().analysisInitEncodedHistogramToRoot, configurationFile->getDataReference().analysisInitFilteredHistogramToRoot,
											configurationFile->getDataReference().analysisInitJustOneCreatedHistogramToRoot, configurationFile->getDataReference().analysisInitJustOneEncodedHistogramToRoot, configurationFile->getDataReference().analysisInitJustOneFilteredHistogramToRoot,
											configurationFile->getDataReference().analysisInitCreatedHistogramToShow, configurationFile->getDataReference().analysisInitEncodedHistogramToShow, configurationFile->getDataReference().analysisInitFilteredHistogramToShow,
											configurationFile->getDataReference().analysisInitHistogramLayer, configurationFile->getDataReference().analysisOutputFileName.c_str());
			analyser->initPrelutRangeAnalysisToRoot(configurationFile->getDataReference().analysisInitPrelutRangeToRoot, configurationFile->getDataReference().analysisOutputFileName.c_str());
		}
		else {
			analyser->initTrackPropagationAnalysisToRoot(configurationFile->getDataReference().analysisInitTrackPropagationToRoot, setAnalysisOutputFileName.c_str());
			analyser->initMomentumAnalysisToRoot(configurationFile->getDataReference().analysisInitMomentumToRoot, setAnalysisOutputFileName.c_str());
			analyser->initProjectionAnalysisToRoot(configurationFile->getDataReference().analysisInitProjectionToRoot, setAnalysisOutputFileName.c_str());
			analyser->initMagnetfieldAnalysisToRoot(configurationFile->getDataReference().analysisInitMagnetfieldToRoot, setAnalysisOutputFileName.c_str());
			analyser->initMagnetfieldConstantAnalysisToRoot(configurationFile->getDataReference().analysisInitMagnetfieldConstantToRoot, setAnalysisOutputFileName.c_str());
			analyser->initHistogramAnalysis(configurationFile->getDataReference().analysisInitCreatedHistogramToRoot, configurationFile->getDataReference().analysisInitEncodedHistogramToRoot, configurationFile->getDataReference().analysisInitFilteredHistogramToRoot,
											configurationFile->getDataReference().analysisInitJustOneCreatedHistogramToRoot, configurationFile->getDataReference().analysisInitJustOneEncodedHistogramToRoot, configurationFile->getDataReference().analysisInitJustOneFilteredHistogramToRoot,
											configurationFile->getDataReference().analysisInitCreatedHistogramToShow, configurationFile->getDataReference().analysisInitEncodedHistogramToShow, configurationFile->getDataReference().analysisInitFilteredHistogramToShow,
											configurationFile->getDataReference().analysisInitHistogramLayer, setAnalysisOutputFileName.c_str());
			analyser->initPrelutRangeAnalysisToRoot(configurationFile->getDataReference().analysisInitPrelutRangeToRoot, setAnalysisOutputFileName.c_str());
		}

		if (configurationFile->getDataReference().analysisInitTotalAnalysis)
			analyser->resetTotalAnalysis();

#endif

		input->readDetectorInfo(fDigiScheme);
		input->getInputDataPointer()->getMagneticField()->init(fField,
															   configurationFile->getDataReference().inputFileNameMagneticField,
															   configurationFile->getDataReference().inputMagneticFieldIsRootFile,
															   configurationFile->getDataReference().inputMapNameMagneticField,
															   configurationFile->getDataReference().inputMagneticFieldIntegrationStepwidthPerStation,
															   configurationFile->getDataReference().inputMagneticFieldIntegrationFactor,
															   input->getInputData().getDetector().getNumberOfStations(),
															   configurationFile->getDataReference().inputDisableAutomaticMagneticField);

		/* setup the data pointers */
		eventData         = input->getInputDataPointer();
		histogram->init((unsigned short)eventData->getDetector().getNumberOfActiveStations(), terminal);

		/* build the look-up-tables*/
		switch(configurationFile->getDataReference().trackfinderLutsVersion) {

			case RUNGEKUTTAFILELUT:				// generate LUTs and write files
				luts->initRungeKuttaFileLuts(&space,
					configurationFile->getDataReference().trackfinderPrelutRadiusMin,
					configurationFile->getDataReference().trackfinderPrelutRadiusMax,
					configurationFile->getDataReference().trackfinderPrelutFileName,
					configurationFile->getDataReference().trackfinderLutFileName,
					eventData->getDetectorPointer(),
					terminal);

				break;

			case ANALYTICFORMULAFILELUT:		// generate LUTs and write files
				luts->initAnalyticFormulaFileLuts(&space,
					configurationFile->getDataReference().trackfinderPrelutRadiusMin,
					configurationFile->getDataReference().trackfinderPrelutRadiusMax,
					configurationFile->getDataReference().trackfinderPrelutFileName,
					configurationFile->getDataReference().trackfinderLutFileName,
					eventData->getMagneticField(),
					terminal);
				break;

			case FILELUT:						// read files
				luts->initFileLuts(&space,
					configurationFile->getDataReference().trackfinderPrelutRadiusMin,
					configurationFile->getDataReference().trackfinderPrelutRadiusMax,
					configurationFile->getDataReference().trackfinderPrelutFileName,
					configurationFile->getDataReference().trackfinderLutFileName,
					terminal);
				break;

			default:							// ANALYTICFORMULALUT without files
				luts->initAnalyticFormulaLuts(&space,
					configurationFile->getDataReference().trackfinderPrelutRadiusMin,
					configurationFile->getDataReference().trackfinderPrelutRadiusMax,
					eventData->getMagneticField());
				break;

		}

		/* build the tables */
		ratings->initGradingP(configurationFile->getDataReference().inputGradingPTableMode,
								configurationFile->getDataReference().inputGradingPTableFileName);
		ratings->setGradingPTableMinimumClassification(bitArray(configurationFile->getDataReference().trackfinderMinClassGradingP));
		ratings->initGradingR(configurationFile->getDataReference().inputGradingRTableMode,
								configurationFile->getDataReference().inputGradingRTableFileName);
		ratings->setGradingRTableMinimumClassification(bitArray(configurationFile->getDataReference().trackfinderMinClassGradingR));

#ifndef NOANALYSIS

		if (analyser->isTrackPropagationEventAnalysisEnabled() || analyser->isTrackPropagationTotalAnalysisEnabled())
			analyser->initTrackPropagationNumberOfDisplays((unsigned short)eventData->getDetector().getNumberOfActiveStations());

		analyser->initPrelutRangeAnalysis(configurationFile->getDataReference().analysisInitPrelutRangeForEachEvent || configurationFile->getDataReference().analysisInitWeightedPrelutRange, (unsigned short)eventData->getDetector().getNumberOfActiveStations());

		if (fVerbose > 2) {
			if (analyser->isTrackPropagationDisplayEnabled())
				analyser->trackPropagationAnalysisDraw(configurationFile->getDataReference().analysisTrackPropagationPointDisplayMask, configurationFile->getDataReference().analysisTrackPropagationHitDisplayMask);
			else if (analyser->isPrelutRangeDisplayEnabled())
				analyser->prelutRangeAnalysisDraw(configurationFile->getDataReference().analysisPrelutRangeStationDisplayMask,
								configurationFile->getDataReference().analysisPrelutRangeStationSumDisplayMask,
								configurationFile->getDataReference().analysisPrelutRangeConstraintDisplayMask,
								configurationFile->getDataReference().analysisPrelutRangeConstraintSumDisplayMask,
								configurationFile->getDataReference().analysisPrelutRangeRelativeDisplayMask);
			else if (analyser->isMagnetfieldFactorDisplayEnabled())
				analyser->magnetfieldFactorAnalysisDraw();
			else if (analyser->isMagnetfieldDisplayEnabled())
				analyser->magnetfieldAnalysisDraw();
			else if (analyser->isProjectionDisplayEnabled())
				analyser->projectionAnalysisDraw();
			else if (analyser->isMomentumDisplayEnabled())
				analyser->momentumAnalysisDraw();
		}

#endif

#ifndef NOANALYSIS

		if ((analyser->isConfigurationAnalysisEnabled()) && (fVerbose > 3))
			std::cout << configurationFile->getInfo() << std::endl;

		if ((analyser->isDetectorAnalysisEnabled()) && (fVerbose > 3)) {

			std::cout << std::endl;
			std::cout << "Number of detector stations: \t" << eventData->getDetector().getNumberOfStations() << std::endl;
			for (unsigned int i = 0; i < eventData->getDetector().getNumberOfStations(); i++) {
				std::cout << "Station number " << i + 1 << ":" << std::endl;
				std::cout << " \tId: "       << eventData->getDetector().getStationByIndex(i).getId()        << std::endl;
				std::cout << " \tDistance: " << eventData->getDetector().getStationByIndex(i).getDistance()  << std::endl;
				std::cout << " \tisNo: "     << eventData->getDetector().getStationByIndex(i).isNoType()     << std::endl;
				std::cout << " \tisMaps: "   << eventData->getDetector().getStationByIndex(i).isMapsType()   << std::endl;
				std::cout << " \tisStrip: "  << eventData->getDetector().getStationByIndex(i).isStripType()  << std::endl;
				std::cout << " \tisHybrid: " << eventData->getDetector().getStationByIndex(i).isHybridType() << std::endl;
				std::cout << " \tMask:  "    << eventData->getDetector().getStationByIndex(i).isMasked()     << std::endl;
				std::cout << " \tIndex: "    << eventData->getDetector().getStationByIndex(i).getIndex()     << std::endl;
			}

		}

#endif

		/* analyse magnetfield */

#ifndef NOANALYSIS

	if (analyser->isMagnetfieldAnalysisEnabled()) {

		analyser->evaluateMagnetfield(terminal);

		if (analyser->isMagnetfieldToRootEnabled())
			analyser->magnetfieldAnalysisWrite();
	
		std::cout << analyser->getMagnetfieldAnalysis() << std::endl;

	}

	if (!analyser->isMomentumDisplayEnabled() && !analyser->isProjectionDisplayEnabled() && analyser->isMagnetfieldDisplayEnabled() && !analyser->isMagnetfieldFactorDisplayEnabled() && !analyser->isPrelutRangeDisplayEnabled() && !analyser->isTrackPropagationDisplayEnabled())
		analyser->magnetfieldAnalysisUpdate();

#endif

	}
	catch(errorHandling& error) {
		error.errorMsg();
	    Fatal("CbmHoughStsTrackFinder", "Fatal Error!!!");
	}
	catch(...) {
		std::cout << "Unknown error during 'CbmHoughStsTrackFinder'-object initialization" << std::endl;
	    Fatal("CbmHoughStsTrackFinder", "Fatal Error!!!");
	}

}

/****************************************************************
 * This method does the trackfinding.			 				*
 ****************************************************************/

int CbmHoughStsTrackFinder::DoFind() {
	
	TStopwatch       eventTimer;
	TStopwatch       trackfinderTimer;
	bool             disableEvent;
	std::streambuf*  terminal;
	terminalSequence statusSequenceForEvents;

	try {

		eventTimer.Reset();
		eventTimer.Start(false);
		input->readDataSource(eventNumber, fMvdHits, fStsHits);
		disableEvent = false;

		if ((configurationFile->getDataReference().initStatus) && (fVerbose > 0))
			terminal = std::cout.rdbuf();
		else
			terminal = NULL;

#ifndef NOANALYSIS

		/* analyzing of event data starts here */

		if ((analyser->isEventAnalysisEnabled()) && (fVerbose > 0)) {

			std::cout << std::endl;
			std::cout << "Number of event:                                \t" << eventData->getEventNumber()    << std::endl;
			std::cout << "Number of tracks:                               \t" << eventData->getNumberOfTracks() << std::endl;
			std::cout << "Number of hits:                                 \t" << eventData->getNumberOfHits()   << std::endl;

			if ((analyser->isClassPriorityAnalysisEnabled()) && (fVerbose > 3)) {

				unsigned int numberOfTracksInHigherClass = 0;

				std::cout << std::endl;
				std::cout << "ANALYZED SOURCE DATA" << std::endl;
				for (unsigned short k = (unsigned short)ratings->getGradingPTableMaximumClassification().toULong(); k >= (unsigned short)ratings->getGradingPTableMinimumClassification().toULong(); k--) {
					std::cout << "Actual number of tracks with class priority " << k;
					std::cout << " and P > " << configurationFile->getDataReference().analysisThresholdForP;
					std::cout << ": \t" << analyser->getNumberOfTracksWithP(k)-numberOfTracksInHigherClass << std::endl;
					numberOfTracksInHigherClass = analyser->getNumberOfTracksWithP(bitArray(k));
				}

			}

			std::cout << "Number of tracks (class priority >= " << ratings->getGradingPTableMinimumClassification().toString(10);
			std::cout << " and P > "                            << configurationFile->getDataReference().analysisThresholdForP;
			std::cout << "): \t"                                << analyser->getNumberOfTracksWithP() << std::endl;

		}

		/* analyzing of event data ends here */

		if (configurationFile->getDataReference().inputMinTracks < configurationFile->getDataReference().inputMaxTracks) {

			if (analyser->getNumberOfTracksWithP() <= configurationFile->getDataReference().inputMinTracks)
				disableEvent = true;
			if (analyser->getNumberOfTracksWithP() > configurationFile->getDataReference().inputMaxTracks)
				disableEvent = true;

		}

#endif

		if (disableEvent) {

			if  (fVerbose > 0) {

				std::cout << std::endl;
				std::cout << "This event is disabled because the number of findable tracks" << std::endl;
				std::cout << "is not in the specified range: ";
				std::cout << configurationFile->getDataReference().inputMinTracks;
				std::cout << " < tracks <= ";
				std::cout << configurationFile->getDataReference().inputMaxTracks;
				std::cout << "!!!" << std::endl;
				std::cout << std::endl;

			}

		}
		else if (configurationFile->getDataReference().analysisWriteCellFiles) {

#ifndef NOANALYSIS

			/* compare the propagated input data with the GEANE propagation */
			if (analyser->isTrackPropagationEventAnalysisEnabled() || analyser->isTrackPropagationTotalAnalysisEnabled()) {

				analyser->evaluateTrackPropagationGoodness(terminal);

#ifdef RUNROOT

				if (!analyser->isMomentumDisplayEnabled() && !analyser->isProjectionDisplayEnabled() && !analyser->isMagnetfieldDisplayEnabled() && !analyser->isMagnetfieldFactorDisplayEnabled() && !analyser->isPrelutRangeDisplayEnabled() && analyser->isTrackPropagationDisplayEnabled())
					analyser->trackPropagationAnalysisUpdate();

#endif

				if (analyser->isTrackPropagationEventAnalysisEnabled())
					if (analyser->isTrackPropagationToRootEnabled())
						analyser->trackPropagationEventAnalysisWrite(eventData->getEventNumber());

			}

			/* evaluate the CELL simulation files */
			analyser->evaluateCellSimulationFiles(configurationFile->getDataReference().analysisHitCellFileName,
												  configurationFile->getDataReference().analysisPrelutCellFileName,
												  configurationFile->getDataReference().analysisLutCellFileName,
												  eventData->getEventNumber(),
												  terminal);

#endif

			/* update the tables */
			if (isFirstEvent) {

				ratings->initCoding(configurationFile->getDataReference().inputCodingTableMode,
									configurationFile->getDataReference().inputCodingTableFileName);
				ratings->setCodingTableMinimumClassification(bitArray(configurationFile->getDataReference().trackfinderMinClassCoding));

			}
			else
				ratings->update();

		}
		else {

#ifndef NOANALYSIS

			/* compare the propagated input data with the GEANE propagation */
			if (analyser->isTrackPropagationEventAnalysisEnabled() || analyser->isTrackPropagationTotalAnalysisEnabled()) {

				analyser->evaluateTrackPropagationGoodness(terminal);

#ifdef RUNROOT

				if (!analyser->isMomentumDisplayEnabled() && !analyser->isProjectionDisplayEnabled() && !analyser->isMagnetfieldDisplayEnabled() && !analyser->isMagnetfieldFactorDisplayEnabled() && !analyser->isPrelutRangeDisplayEnabled() && analyser->isTrackPropagationDisplayEnabled())
					analyser->trackPropagationAnalysisUpdate();

#endif

				if (analyser->isTrackPropagationEventAnalysisEnabled())
					if (analyser->isTrackPropagationToRootEnabled())
						analyser->trackPropagationEventAnalysisWrite(eventData->getEventNumber());

			}
	
			/* evaluate prelut range */
			if (configurationFile->getDataReference().analysisInitPrelutRangeForEachEvent) {
				analyser->evaluatePrelutRange(isFirstEvent,
						configurationFile->getDataReference().analysisInitWeightedPrelutRange,
						configurationFile->getDataReference().analysisChooseMainPrelutRange,
						configurationFile->getDataReference().analysisChooseConstraintPrelutRange,
						terminal);

				if (!analyser->isMomentumDisplayEnabled() && !analyser->isProjectionDisplayEnabled() && !analyser->isMagnetfieldDisplayEnabled() && !analyser->isMagnetfieldFactorDisplayEnabled() && analyser->isPrelutRangeDisplayEnabled() && !analyser->isTrackPropagationDisplayEnabled())
					analyser->prelutRangeAnalysisUpdate();

				if (analyser->isPrelutRangeToRootEnabled())
					analyser->prelutRangeAnalysisWrite(eventData->getEventNumber());

			}
			else {
				/* This case is just used to evaluate the prelut range just once at the first event and not for each event separately */
				if ((configurationFile->getDataReference().analysisInitWeightedPrelutRange) && (isFirstEvent)) {
					analyser->evaluatePrelutRange(true,
						false,
						configurationFile->getDataReference().analysisChooseMainPrelutRange,
						configurationFile->getDataReference().analysisChooseConstraintPrelutRange,
						terminal);

					if (!analyser->isMomentumDisplayEnabled() && !analyser->isProjectionDisplayEnabled() && !analyser->isMagnetfieldDisplayEnabled() && !analyser->isMagnetfieldFactorDisplayEnabled() && analyser->isPrelutRangeDisplayEnabled() && !analyser->isTrackPropagationDisplayEnabled())
						analyser->prelutRangeAnalysisUpdate();

					if (analyser->isPrelutRangeToRootEnabled())
						analyser->prelutRangeAnalysisWrite(eventData->getEventNumber());

				}
			}

			/* evaluate magnetfield factors */
			if (configurationFile->getDataReference().analysisInitMagnetfieldConstantForEachEvent) {
				analyser->evaluateMagnetfieldFactors(isFirstEvent, configurationFile->getDataReference().analysisInitWeightedMagnetfieldConstant, terminal);

				if (!analyser->isMomentumDisplayEnabled() && !analyser->isProjectionDisplayEnabled() && !analyser->isMagnetfieldDisplayEnabled() && analyser->isMagnetfieldFactorDisplayEnabled() && !analyser->isPrelutRangeDisplayEnabled() && !analyser->isTrackPropagationDisplayEnabled())
					analyser->magnetfieldFactorAnalysisUpdate();

				if (analyser->isMagnetfieldFactorToRootEnabled())
					analyser->magnetfieldFactorAnalysisWrite(eventData->getEventNumber());
		
			}
			else {
				/* This case is just used to evaluate the magnetic field factors just once at the first event and not for each event separately */
				if ((configurationFile->getDataReference().analysisInitWeightedMagnetfieldConstant) && (isFirstEvent)) {
					analyser->evaluateMagnetfieldFactors(true, false, terminal);

					if (!analyser->isMomentumDisplayEnabled() && !analyser->isProjectionDisplayEnabled() && !analyser->isMagnetfieldDisplayEnabled() && analyser->isMagnetfieldFactorDisplayEnabled() && !analyser->isPrelutRangeDisplayEnabled() && !analyser->isTrackPropagationDisplayEnabled())
						analyser->magnetfieldFactorAnalysisUpdate();

					if (analyser->isMagnetfieldFactorToRootEnabled())
						analyser->magnetfieldFactorAnalysisWrite(eventData->getEventNumber());

				}
			}

			if ((isFirstEvent) && (configurationFile->getDataReference().analysisInitMagnetfieldVSConstants))
				analyser->evaluateMagnetfieldVSFactors();

#endif

#ifndef NOANALYSIS

			if (configurationFile->getDataReference().analysisInitPrelutGoodness)
				analyser->evaluatePrelutGoodness(terminal);

			if (configurationFile->getDataReference().analysisInitLutGoodness)
				analyser->evaluateLutGoodness(terminal);

			if (configurationFile->getDataReference().analysisInitHoughTransformGoodness)
				analyser->evaluateHoughTransformGoodness(terminal);

			if (configurationFile->getDataReference().analysisInitQuantizationGoodness)
				analyser->evaluateQuantizationGoodness(terminal);

			if (configurationFile->getDataReference().analysisInitPeakDistanceGoodness)
				analyser->evaluatePeakDistanceGoodness(terminal);

#endif

			/* update the tables */
			if (isFirstEvent) {

				ratings->initCoding(configurationFile->getDataReference().inputCodingTableMode,
									configurationFile->getDataReference().inputCodingTableFileName);
				ratings->setCodingTableMinimumClassification(bitArray(configurationFile->getDataReference().trackfinderMinClassCoding));

			}
			else
				ratings->update();

#ifndef NOANALYSIS

			/* evaluate the automatic filter geometry */
			if (houghTrackfinder->isFilterGeometryGenerationEnabled())
				houghTrackfinder->generateFilterGeometry(configurationFile->getDataReference().trackfinderAutomaticFilterCoverPercentage, isFirstEvent, terminal);

#endif

			/* find tracks */
			trackfinderTimer.Reset();
			trackfinderTimer.Start(false);
			houghTrackfinder->evaluate(terminal);
			trackfinderTimer.Stop();

			/* write tracks */
			output->write(terminal, fTracks);

			/* analyzing quality starts here */

#ifndef NOANALYSIS

			if (analyser->isMCTrackVisualizationAnalysisEnabled())
				std::cout << analyser->displayMCTracks(houghTrackfinder->getDebugTrackIndex()) << std::endl;
			if (analyser->isFoundTrackVisualizationAnalysisEnabled())
				std::cout << analyser->displayFoundTracks(houghTrackfinder->getDebugTrackIndex()) << std::endl;

			if (analyser->isMomentumEventAnalysisEnabled() || analyser->isMomentumTotalAnalysisEnabled() || analyser->isProjectionEventAnalysisEnabled() || analyser->isProjectionTotalAnalysisEnabled() || analyser->isQualityEventAnalysisEnabled() || analyser->isQualityTotalAnalysisEnabled())
				analyser->evaluateAlgorithm(terminal);

			if ((analyser->isQualityEventAnalysisEnabled()) && (fVerbose > 1))
				std::cout << analyser->getAbsoluteQualityEFGCEventAnalysis() << std::endl;
			if ((analyser->isQualityEventAnalysisEnabled()) && (fVerbose > 0))
				std::cout << analyser->getRelativeQualityEFGCEventAnalysis() << std::endl;

			if (analyser->isMomentumEFGCEventAnalysisEnabled()) {

				if (analyser->isMomentumToRootEnabled())
					analyser->momentumEventAnalysisWrite(eventData->getEventNumber());

				if ((analyser->isMomentumEventAnalysisEnabled()) && (fVerbose > 2))
					std::cout << analyser->getMomentumEventAnalysis() << std::endl;

			}

			if (analyser->isProjectionEFGCNEventAnalysisEnabled()) {

				if (analyser->isProjectionToRootEnabled())
					analyser->projectionEventAnalysisWrite(eventData->getEventNumber());

			}

			if ((analyser->isMomentumDisplayEnabled() && !analyser->isMagnetfieldDisplayEnabled() && !analyser->isMagnetfieldFactorDisplayEnabled() && !analyser->isPrelutRangeDisplayEnabled() && !analyser->isTrackPropagationDisplayEnabled()) && (fVerbose > 2))
				analyser->momentumAnalysisUpdate();

			if (!analyser->isMomentumDisplayEnabled() && analyser->isProjectionDisplayEnabled() && !analyser->isMagnetfieldDisplayEnabled() && !analyser->isMagnetfieldFactorDisplayEnabled() && !analyser->isPrelutRangeDisplayEnabled() && !analyser->isTrackPropagationDisplayEnabled())
				analyser->projectionAnalysisUpdate();

			if (configurationFile->getDataReference().analysisInitTotalAnalysis && luts->typeUsesCorrections())
				analyser->addNumberOfHoughTransformCorrections(luts->getNumberOfCorrections(), luts->getNumberOfCoordCorrections());

			eventTimer.Stop();

			if (fVerbose > 1) {

				std::cout << ">-----------------------------------------------<" << std::endl;
				createTerminalStatusSequence(&statusSequenceForEvents, terminal, "Actual number of events done:\t\t\t", (unsigned int)(1 + eventData->getEventNumber()));
				terminalInitialize(statusSequenceForEvents);
				terminalOverwrite(statusSequenceForEvents, (unsigned int)(1 + eventData->getEventNumber()));
				terminalFinalize(statusSequenceForEvents);

				if (analyser->isTimeAnalysisEnabled()) {

					std::cout << std::endl;
					std::cout << "Needed realtime for:" << std::endl;

					if (fVerbose > 2) {

						std::cout << analyser->formatRealTimeAnalysis() << std::endl;

					}

					std::cout << analyser->formatRealTimeSummaryAnalysis(trackfinderTimer.RealTime(), eventTimer.RealTime()) << std::endl;
					std::cout << std::endl;
					std::cout << "Needed cputime for:" << std::endl;

					if (fVerbose > 2) {

						std::cout << analyser->formatCpuTimeAnalysis() << std::endl;

					}

					std::cout << analyser->formatCpuTimeSummaryAnalysis(trackfinderTimer.CpuTime(), eventTimer.CpuTime()) << std::endl;

				}

			}

#endif

			/* analyzing quality ends here */

		}

		eventNumber++;

		isFirstEvent = false;

		return tracks->getNumberOfTracks();

	}
	catch(errorHandling& error) {
		error.errorMsg();
	    Fatal("CbmHoughStsTrackFinder", "Fatal Error!!!");
		return 0;
	}
	catch(...) {
		std::cout << "Unknown error during 'CbmHoughStsTrackFinder'-object execution" << std::endl;
	    Fatal("CbmHoughStsTrackFinder", "Fatal Error!!!");
		return 0;
	}

}

/****************************************************************
 * This method is executed after each Exec function call to		*
 * final results.								 				*
 ****************************************************************/

void CbmHoughStsTrackFinder::Finish() {

	std::streambuf* terminal;

	if ((configurationFile->getDataReference().initStatus) && (fVerbose > 0))
		terminal = std::cout.rdbuf();
	else
		terminal = NULL;

	/* write the tables */
	if (configurationFile->getDataReference().inputCodingTableWrite)
		ratings->writeCodingTable("", "", terminal);
	if (configurationFile->getDataReference().inputGradingPTableWrite)
		ratings->writeGradingPTable("", "", terminal);
	if (configurationFile->getDataReference().inputGradingRTableWrite)
		ratings->writeGradingRTable("", "", terminal);

#ifndef NOANALYSIS

	if (configurationFile->getDataReference().trackfinderAutomaticFilterWrite)
		if (houghTrackfinder->isAutomaticFilterGeometryEnabled())
			houghTrackfinder->writePeakfindingGeometry(terminal);

	if (analyser->isTrackPropagationTotalAnalysisEnabled())
		if (analyser->isTrackPropagationToRootEnabled())
			analyser->trackPropagationTotalAnalysisWrite(eventNumber);

	if (houghTrackfinder->isFilterGeometryGenerationEnabled()) {
		if (analyser->isPeakfindingGeometryDisplayEnabled())
			houghTrackfinder->drawPeakfindingGeometry();
		if (analyser->isProjectedPeakfindingGeometryDisplayEnabled())
			houghTrackfinder->drawProjectedPeakfindingGeometry();
		if (analyser->isCoveredPeakfindingGeometryDisplayEnabled())
			houghTrackfinder->drawCoveredPeakfindingGeometry();
		if (analyser->isCoveredProjectedPeakfindingGeometryDisplayEnabled())
			houghTrackfinder->drawCoveredProjectedPeakfindingGeometry();
		if (analyser->isPeakfindingGeometryMessageEnabled())
			std::cout << houghTrackfinder->peakfindingGeometryToString() << std::endl;
		if (analyser->isProjectedPeakfindingGeometryMessageEnabled())
			std::cout << houghTrackfinder->projectedPeakfindingGeometryToString() << std::endl;
		if (analyser->isCoveredPeakfindingGeometryMessageEnabled())
			std::cout << houghTrackfinder->coveredPeakfindingGeometryToString() << std::endl;
		if (analyser->isCoveredProjectedPeakfindingGeometryMessageEnabled())
			std::cout << houghTrackfinder->coveredProjectedPeakfindingGeometryToString() << std::endl;
	}

	if ((analyser->isQualityTotalAnalysisEnabled()) && (fVerbose > 1))
		std::cout << analyser->getAbsoluteQualityEFGCTotalAnalysis();
	if ((analyser->isQualityTotalAnalysisEnabled()) && (fVerbose > 0))
		std::cout << analyser->getRelativeQualityEFGCTotalAnalysis();

	if (analyser->isMomentumEFGCTotalAnalysisEnabled()) {

		if (analyser->isMomentumToRootEnabled())
			analyser->momentumTotalAnalysisWrite(eventNumber);

		if ((analyser->isMomentumTotalAnalysisEnabled()) && (fVerbose > 2))
			std::cout << analyser->getMomentumTotalAnalysis() << std::endl;

	}

	if (analyser->isProjectionEFGCNTotalAnalysisEnabled()) {

		if (analyser->isProjectionToRootEnabled())
			analyser->projectionTotalAnalysisWrite(eventNumber);

	}

	if (analyser->isNumberOfTracksInAllColumnsAnalysisEnabled())
		analyser->showNumberOfTracksInAllColumns();

	if (analyser->isNumberOfTracksInAllLayersAnalysisEnabled())
		analyser->showNumberOfTracksInAllLayers();

	if (analyser->isHitReadoutDistributionAnalysisEnabled())
		analyser->showHitReadoutDistribution(configurationFile->getDataReference().analysisInitReadoutColumnsInParallel);

	if (configurationFile->getDataReference().analysisInitTotalAnalysis)
		analyser->printTotalAnalysis();

	if (configurationFile->getDataReference().analysisInitTotalAnalysis)
		analyser->displayTotalAnalysis();

	if ((analyser->isMemoryAnalysisEnabled()) && (fVerbose > 3)) {

		std::cout << std::endl;
		if (configurationFile->getDataReference().inputFileFormat == ASCIIFILEFORMAT)
			std::cout << analyser->formatMemoryAnalysis(histogram, ((inputAscii*)input)->getReservedSizeOfData(0), ((inputAscii*)input)->getAllocatedSizeOfData(0), ((inputAscii*)input)->getUsedSizeOfData(0)) << std::endl;
		else
			std::cout << analyser->formatMemoryAnalysis(histogram, ((inputRoot*)input)->getReservedSizeOfData(0), ((inputRoot*)input)->getAllocatedSizeOfData(0), ((inputRoot*)input)->getUsedSizeOfData(0)) << std::endl;

	}

#endif

}

ClassImp(CbmHoughStsTrackFinder)
