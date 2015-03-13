/* *******************************************************************
// (C)opyright 2004
// 
// Institute of Computer Science V
// Prof. Männer
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
//   class:
//     - reads some parameters from a configuration file
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-10-24 16:35:25 $
// $Revision: 1.36 $
//
// *******************************************************************/


#include "../../MiscLIB/include/conversionRoutines.h"
#include "../../DataRootObjectLIB/include/tables.h"
#include "../../RootFrameworkLIB/include/hitProducer.h"
#include "../../LutGeneratorLIB/include/lutImplementation.h"
#include "../../MiscLIB/include/memoryDef.h"
#include "../../HistogramTransformationLIB/include/filterDef.h"
#include "../../AnalysisLIB/include/analysisDef.h"
#include "../../AnalysisLIB/include/prelutRangeLayerAnalysis.h"
#include "../include/inf.h"


/*
 * default filename
 */
#define defFileName "CbmHoughStsTrackFinder.ini"

/*
 * true if a default configuration-file should be written.
 */
#define writeDefaultHeader false

/*
 * default values for each parameter
 */
#define detectorGammaOpeningAngle                                0.46764	// atan((planeMaxY + planeMaxY / 100) / planeZ)
#define detectorThetaOpeningAngle                                0.46764	// atan((planeMaxX + planeMaxX / 100) / planeZ)
#define momentumCutSmallerThan                                   1.0		// [GeV]

#define defValInputFileName                                      "./input/*.root"
#define defValInputFileFormat                                    ROOTFILEFORMAT
#define defValInputMinTracks                                     0
#define defValInputMaxTracks                                     0
#define defValInputDetectorMask                                  0
#define defValInputFileNameMagneticField                         "./input/FieldActive.root"
#define defValInputMagneticFieldIsRootFile                       true
#define defValInputMapNameMagneticField                          "NewMap"
#define defValInputMagneticFieldIntegrationStepwidthPerStation   10
#define defValInputMagneticFieldIntegrationFactor                1.0
#define defValInputDisableAutomaticMagneticField                 false
#define defValInputFileNameDetector                              "./geometry/*.dat"
#define defValInputNumberOfVolumesInfrontOfSTS                   10
#define defValInputDisableAutomaticDetector                      false
#define defValInputCodingTableMode                               FILETABLE
#define defValInputGradingPTableMode                             FILETABLE
#define defValInputGradingRTableMode                             FILETABLE
#define defValInputCodingTableFileName                           "./parameters/htrack/codingTable.txt"
#define defValInputGradingPTableFileName                         "./parameters/htrack/gradingPTable.txt"
#define defValInputGradingRTableFileName                         "./parameters/htrack/gradingRTable.txt"
#define defValInputCodingTableWrite                              false
#define defValInputGradingPTableWrite                            false
#define defValInputGradingRTableWrite                            false
#define defValTrackfinderHitProducer                             NONEHITPRODUCER
#define defValTrackfinderReadPointsFromFile                      true
#define defValTrackfinderReadHitsFromFile                        false
#define defValTrackfinderReadMapsHits                            false
#define defValTrackfinderReadHybridHits                          true
#define defValTrackfinderReadStripHits                           true
#define defValTrackfinderLutsVersion                             ANALYTICFORMULALUT
#define defValTrackfinderPrelutFileName                          "./parameters/htrack/prelut.txt"
#define defValTrackfinderLutFileName                             "./parameters/htrack/lut.txt"
#define defValTrackfinderGammaMin                                -detectorGammaOpeningAngle
#define defValTrackfinderGammaMax                                +detectorGammaOpeningAngle
#ifndef CALLOC_HISTOGRAM_RUNTIME
	#define defValTrackfinderGammaStep                           maxDimGamma
#else
	#define defValTrackfinderGammaStep                           191
#endif
#define defValTrackfinderThetaMin                                -detectorThetaOpeningAngle
#define defValTrackfinderThetaMax                                +detectorThetaOpeningAngle
#ifndef CALLOC_HISTOGRAM_RUNTIME
	#define defValTrackfinderThetaStep                           maxDimTheta
#else
	#define defValTrackfinderThetaStep                           383
#endif
#define defValTrackfinderPrelutRadiusMin                         -0.035
#define defValTrackfinderPrelutRadiusMax                         +0.035
#define defValTrackfinderLutRadiusMin                            -(1 / (momentumCutSmallerThan - momentumCutSmallerThan / 10))
#define defValTrackfinderLutRadiusMax                            +(1 / (momentumCutSmallerThan - momentumCutSmallerThan / 10))
#ifndef CALLOC_HISTOGRAM_RUNTIME
	#define defValTrackfinderLutRadiusStep                       maxDimRadius
#else
	#define defValTrackfinderLutRadiusStep                       127
#endif
#define defValTrackfinderMinClassCoding                          1
#define defValTrackfinderMinClassGradingP                        1
#define defValTrackfinderMinClassGradingR                        1
#define defValTrackfinderFilterType                              MAXMORPHSEARCHFILTER
#define defValTrackfinderFirstFilterGeometry                     FIRSTFINALGEOMETRY
#define defValTrackfinderFirstFilterArithmetic                   FIRSTSIMPLEARITHMETIC
#define defValTrackfinderSecondFilterGeometry                    SECONDFINALGEOMETRY
#define defValTrackfinderSecondFilterArithmetic                  SECONDSIMPLEARITHMETIC
#define defValTrackfinderFirstFilterNeighborhoodDim1ClearRadius  4
#define defValTrackfinderFirstFilterNeighborhoodDim2ClearRadius  1
#define defValTrackfinderSecondFilterNeighborhoodDim1ClearRadius 1
#define defValTrackfinderSecondFilterNeighborhoodDim2ClearRadius 1
#define defValTrackfinderSecondFilterNeighborhoodDim3ClearRadius 1
#define defValTrackfinderAutomaticFilterCoverPercentage          100
#define defValTrackfinderAutomaticFilterDataPercentage           10
#define defValTrackfinderAutomaticFilterWrite                    false
#define defValTrackfinderAutomaticFilterFileName                 "./parameters/htrack/filterGeometry.txt"
#define defValAnalysisGraphicOutputFileName                      "../Results/trackfinderAnalysisOutput.root"
#define defValTrackfinderWriteTracksToFile                       false
#define defValAnalysisThresholdForP                              momentumCutSmallerThan
#define defValAnalysisInitConfiguration                          false
#define defValAnalysisInitDetector                               false
#define defValAnalysisInitEvent                                  true
#define defValAnalysisInitClassPriority                          false
#define defValAnalysisInitMemory                                 false
#define defValAnalysisInitTime                                   false
#define defValAnalysisInitTrackPropagationEventPoint             false
#define defValAnalysisInitTrackPropagationEventHit               false
#define defValAnalysisInitTrackPropagationTotalPoint             false
#define defValAnalysisInitTrackPropagationTotalHit               false
#define defValAnalysisInitTrackPropagationDisplay                false
#define defValAnalysisInitTrackPropagationToRoot                 false
#define defValAnalysisTrackPropagationPointDisplayMask           0
#define defValAnalysisTrackPropagationHitDisplayMask             0
#define defValAnalysisInitQualityEFGCEventAbsolute               false
#define defValAnalysisInitQualityEFGCEventRelative               false
#define defValAnalysisInitQualityEFGCTotalAbsolute               false
#define defValAnalysisInitQualityEFGCTotalRelative               true
#define defValAnalysisInitMomentumEFGCEventPzEFGC                false
#define defValAnalysisInitMomentumEFGCEventPtEFGC                false
#define defValAnalysisInitMomentumEFGCTotalPzEFGC                false
#define defValAnalysisInitMomentumEFGCTotalPtEFGC                false
#define defValAnalysisInitProjectionEFGCNEvent12EFGCN            false
#define defValAnalysisInitProjectionEFGCNEvent13EFGCN            false
#define defValAnalysisInitProjectionEFGCNEvent32EFGCN            false
#define defValAnalysisInitProjectionEFGCNTotal12EFGCN            false
#define defValAnalysisInitProjectionEFGCNTotal13EFGCN            false
#define defValAnalysisInitProjectionEFGCNTotal32EFGCN            false
#define defValAnalysisInitMomentumEvent                          false
#define defValAnalysisInitMomentumTotal                          false
#define defValAnalysisInitMomentumDisplay                        false
#define defValAnalysisInitMomentumToRoot                         false
#define defValAnalysisInitProjectionEvent                        false
#define defValAnalysisInitProjectionTotal                        false
#define defValAnalysisInitProjectionDisplay                      false
#define defValAnalysisInitProjectionToRoot                       false
#define defValAnalysisInitMagnetfieldX                           false
#define defValAnalysisInitMagnetfieldY                           false
#define defValAnalysisInitMagnetfieldZ                           false
#define defValAnalysisInitMagnetfieldDisplay                     false
#define defValAnalysisInitMagnetfieldToRoot                      false
#define defValAnalysisInitMagnetfieldConstantForEachEvent        false
#define defValAnalysisInitWeightedMagnetfieldConstant            false
#define defValAnalysisInitMagnetfieldConstantDisplay             false
#define defValAnalysisMagnetfieldConstantDisplayMask             0
#define defValAnalysisInitMagnetfieldConstantToRoot              false
#define defValAnalysisInitMagnetfieldVSConstants                 false
#define defValAnalysisInitPrelutGoodness                         false
#define defValAnalysisInitLutGoodness                            false
#define defValAnalysisInitHoughTransformGoodness                 false
#define defValAnalysisInitQuantizationGoodness                   false
#define defValAnalysisInitPeakDistanceGoodness                   false
#define defValAnalysisInitCreatedHistogramToRoot                 false
#define defValAnalysisInitEncodedHistogramToRoot                 false
#define defValAnalysisInitFilteredHistogramToRoot                false
#define defValAnalysisInitJustOneCreatedHistogramToRoot          false
#define defValAnalysisInitJustOneEncodedHistogramToRoot          false
#define defValAnalysisInitJustOneFilteredHistogramToRoot         false
#define defValAnalysisInitCreatedHistogramToShow                 false
#define defValAnalysisInitEncodedHistogramToShow                 false
#define defValAnalysisInitFilteredHistogramToShow                false
#define defValAnalysisInitHistogramLayer                         100
#define defValAnalysisInitNumberOfTracksPerColumn                false
#define defValAnalysisInitNumberOfTracksPerRow                   false
#define defValAnalysisInitNumberOfTracksPerLayer                 false
#define defValAnalysisInitHitReadoutDistribution                 false
#define defValAnalysisInitReadoutColumnsInParallel               true
#define defValAnalysisInitPrelutRangeForEachEvent                false
#define defValAnalysisInitWeightedPrelutRange                    false
#define defValAnalysisInitPrelutRangeDisplay                     false
#define defValAnalysisInitPrelutRangeDisplayMode                 CUTMAINRELATIVEDISPLAYMODE
#define defValAnalysisPrelutRangeStationDisplayMask              0
#define defValAnalysisPrelutRangeStationSumDisplayMask           false
#define defValAnalysisPrelutRangeConstraintDisplayMask           0
#define defValAnalysisPrelutRangeConstraintSumDisplayMask        false
#define defValAnalysisPrelutRangeRelativeDisplayMask             false
#define defValAnalysisInitPrelutRangeToRoot                      false
#define defValAnalysisInitPercentageOfHitsForPrelutRange         80
#define defValAnalysisPrelutRangeMinStart                        -0.1
#define defValAnalysisPrelutRangeMinStop                         +0.1
#define defValAnalysisPrelutRangeMinSteps                        201
#define defValAnalysisPrelutRangeMaxStart                        -0.1
#define defValAnalysisPrelutRangeMaxStop                         +0.1
#define defValAnalysisPrelutRangeMaxSteps                        201
#define defValAnalysisChooseMainPrelutRange                      true
#define defValAnalysisChooseConstraintPrelutRange                false
#define defValAnalysisInitTotalAnalysis                          false
#define defValAnalysisInitPercentageOfHitsInSignature            70
#define defValAnalysisInitPercentageOfTracksForSignature         75
#define defValAnalysisInitAnalysisResultWarnings                 (NUMBEROFTRACKSWHICHCANNOTBEFOUND)
#define defValAnalysisInitAnalysisResultDisplays                 (TRACKWITHMOMENTAERRORDISTRIBUTION | TRACKSPERCOLUMNDISTRIBUTION | TRACKSPERLAYERDISTRIBUTION)
#define defValAnalysisInitAnalysisMoreResultWarnings             0
#define defValAnalysisInitAnalysisMoreResultDisplays             0
#define defValAnalysisWriteCellFiles                             false
#define defValAnalysisHitCellFileName						     "./parameters/htrack/cellHits.txt"
#define defValAnalysisPrelutCellFileName						 "./parameters/htrack/cellPrelut.txt"
#define defValAnalysisLutCellFileName						     "./parameters/htrack/cellLut.txt"
#define defValInitStatus                                         true
#define defValAnalysisOutputFileName                             "analysisOutput.root"

/*
 * stringcommands which represent the parameters read from file
 */
#define stringCmdInputFileName                                      "inputFileName"
#define stringCmdInputFileFormat                                    "inputFileFormat"
#define stringCmdInputMinTracks                                     "inputMinTracks"
#define stringCmdInputMaxTracks                                     "inputMaxTracks"
#define stringCmdInputDetectorMask                                  "inputDetectorMask"
#define stringCmdInputFileNameMagneticField                         "inputFileNameMagneticField"
#define stringCmdInputMagneticFieldIsRootFile                       "inputMagneticFieldIsRootFile"
#define stringCmdInputMapNameMagneticField                          "inputMapNameMagneticField"
#define stringCmdInputMagneticFieldIntegrationStepwidthPerStation   "inputMagneticFieldIntegrationStepwidthPerStation"
#define stringCmdInputMagneticFieldIntegrationFactor                "inputMagneticFieldIntegrationFactor"
#define stringCmdInputDisableAutomaticMagneticField                 "inputDisableAutomaticMagneticField"
#define stringCmdInputFileNameDetector                              "inputFileNameDetector"
#define stringCmdInputNumberOfVolumesInfrontOfSTS                   "inputNumberOfVolumesInfrontOfSTS"
#define stringCmdInputDisableAutomaticDetector                      "inputDisableAutomaticDetector"
#define stringCmdInputCodingTableMode                               "inputCodingTableMode"
#define stringCmdInputGradingPTableMode                             "inputGradingPTableMode"
#define stringCmdInputGradingRTableMode                             "inputGradingRTableMode"
#define stringCmdInputCodingTableFileName                           "inputCodingTableFileName"
#define stringCmdInputGradingPTableFileName                         "inputGradingPTableFileName"
#define stringCmdInputGradingRTableFileName                         "inputGradingRTableFileName"
#define stringCmdInputCodingTableWrite                              "inputCodingTableWrite"
#define stringCmdInputGradingPTableWrite                            "inputGradingPTableWrite"
#define stringCmdInputGradingRTableWrite                            "inputGradingRTableWrite"
#define stringCmdTrackfinderHitProducer                             "trackfinderHitProducer"
#define stringCmdTrackfinderReadPointsFromFile                      "trackfinderReadPointsFromFile"
#define stringCmdTrackfinderReadHitsFromFile                        "trackfinderReadHitsFromFile"
#define stringCmdTrackfinderReadMapsHits                            "trackfinderReadMapsHits"
#define stringCmdTrackfinderReadHybridHits                          "trackfinderReadHybridHits"
#define stringCmdTrackfinderReadStripHits                           "trackfinderReadStripHits"
#define stringCmdTrackfinderLutsVersion                             "trackfinderLutsVersion"
#define stringCmdTrackfinderPrelutFileName                          "trackfinderPrelutFileName"
#define stringCmdTrackfinderLutFileName                             "trackfinderLutFileName"
#define stringCmdTrackfinderGammaMin                                "trackfinderGammaMin"
#define stringCmdTrackfinderGammaMax                                "trackfinderGammaMax"
#define stringCmdTrackfinderGammaStep                               "trackfinderGammaStep"
#define stringCmdTrackfinderThetaMin                                "trackfinderThetaMin"
#define stringCmdTrackfinderThetaMax                                "trackfinderThetaMax"
#define stringCmdTrackfinderThetaStep                               "trackfinderThetaStep"
#define stringCmdTrackfinderPrelutRadiusMin                         "trackfinderPrelutRadiusMin"
#define stringCmdTrackfinderPrelutRadiusMax                         "trackfinderPrelutRadiusMax"
#define stringCmdTrackfinderLutRadiusMin                            "trackfinderLutRadiusMin"
#define stringCmdTrackfinderLutRadiusMax                            "trackfinderLutRadiusMax"
#define stringCmdTrackfinderLutRadiusStep                           "trackfinderLutRadiusStep"
#define stringCmdTrackfinderMinClassCoding                          "trackfinderMinClassCoding"
#define stringCmdTrackfinderMinClassGradingP                        "trackfinderMinClassGradingP"
#define stringCmdTrackfinderMinClassGradingR                        "trackfinderMinClassGradingR"
#define stringCmdTrackfinderFilterType                              "trackfinderFilterType"
#define stringCmdTrackfinderFirstFilterGeometry                     "trackfinderFirstFilterGeometry"
#define stringCmdTrackfinderFirstFilterArithmetic                   "trackfinderFirstFilterArithmetic"
#define stringCmdTrackfinderSecondFilterGeometry                    "trackfinderSecondFilterGeometry"
#define stringCmdTrackfinderSecondFilterArithmetic                  "trackfinderSecondFilterArithmetic"
#define stringCmdTrackfinderFirstFilterNeighborhoodDim1ClearRadius  "trackfinderFirstFilterNeighborhoodDim1ClearRadius"
#define stringCmdTrackfinderFirstFilterNeighborhoodDim2ClearRadius  "trackfinderFirstFilterNeighborhoodDim2ClearRadius"
#define stringCmdTrackfinderSecondFilterNeighborhoodDim1ClearRadius "trackfinderSecondFilterNeighborhoodDim1ClearRadius"
#define stringCmdTrackfinderSecondFilterNeighborhoodDim2ClearRadius "trackfinderSecondFilterNeighborhoodDim2ClearRadius"
#define stringCmdTrackfinderSecondFilterNeighborhoodDim3ClearRadius "trackfinderSecondFilterNeighborhoodDim3ClearRadius"
#define stringCmdTrackfinderAutomaticFilterCoverPercentage          "trackfinderAutomaticFilterCoverPercentage"
#define stringCmdTrackfinderAutomaticFilterDataPercentage           "trackfinderAutomaticFilterDataPercentage"
#define stringCmdTrackfinderAutomaticFilterWrite                    "trackfinderAutomaticFilterWrite"
#define stringCmdTrackfinderAutomaticFilterFileName                 "trackfinderAutomaticFilterFileName"
#define stringCmdTrackfinderWriteTracksToFile                       "trackfinderWriteTracksToFile"
#define stringCmdAnalysisOutputFileName                             "analysisOutputFileName"
#define stringCmdAnalysisThresholdForP                              "analysisThresholdForP"
#define stringCmdAnalysisInitConfiguration                          "analysisInitConfiguration"
#define stringCmdAnalysisInitDetector                               "analysisInitDetector"
#define stringCmdAnalysisInitEvent                                  "analysisInitEvent"
#define stringCmdAnalysisInitClassPriority                          "analysisInitClassPriority"
#define stringCmdAnalysisInitMemory                                 "analysisInitMemory"
#define stringCmdAnalysisInitTime                                   "analysisInitTime"
#define stringCmdAnalysisInitTrackPropagationEventPoint             "analysisInitTrackPropagationEventPoint"
#define stringCmdAnalysisInitTrackPropagationEventHit               "analysisInitTrackPropagationEventHit"
#define stringCmdAnalysisInitTrackPropagationTotalPoint             "analysisInitTrackPropagationTotalPoint"
#define stringCmdAnalysisInitTrackPropagationTotalHit               "analysisInitTrackPropagationTotalHit"
#define stringCmdAnalysisInitTrackPropagationDisplay                "analysisInitTrackPropagationDisplay"
#define stringCmdAnalysisInitTrackPropagationToRoot                 "analysisInitTrackPropagationToRoot"
#define stringCmdAnalysisTrackPropagationPointDisplayMask           "analysisTrackPropagationPointDisplayMask"
#define stringCmdAnalysisTrackPropagationHitDisplayMask             "analysisTrackPropagationHitDisplayMask"
#define stringCmdAnalysisInitQualityEFGCEventAbsolute               "analysisInitQualityEFGCEventAbsolute"
#define stringCmdAnalysisInitQualityEFGCEventRelative               "analysisInitQualityEFGCEventRelative"
#define stringCmdAnalysisInitQualityEFGCTotalAbsolute               "analysisInitQualityEFGCTotalAbsolute"
#define stringCmdAnalysisInitQualityEFGCTotalRelative               "analysisInitQualityEFGCTotalRelative"
#define stringCmdAnalysisInitMomentumEFGCEventPzEFGC                "analysisInitMomentumEFGCEventPzEFGC"
#define stringCmdAnalysisInitMomentumEFGCEventPtEFGC                "analysisInitMomentumEFGCEventPtEFGC"
#define stringCmdAnalysisInitMomentumEFGCTotalPzEFGC                "analysisInitMomentumEFGCTotalPzEFGC"
#define stringCmdAnalysisInitMomentumEFGCTotalPtEFGC                "analysisInitMomentumEFGCTotalPtEFGC"
#define stringCmdAnalysisInitProjectionEFGCNEvent12EFGCN            "analysisInitProjectionEFGCNEvent12EFGCN"
#define stringCmdAnalysisInitProjectionEFGCNEvent13EFGCN            "analysisInitProjectionEFGCNEvent13EFGCN"
#define stringCmdAnalysisInitProjectionEFGCNEvent32EFGCN            "analysisInitProjectionEFGCNEvent32EFGCN"
#define stringCmdAnalysisInitProjectionEFGCNTotal12EFGCN            "analysisInitProjectionEFGCNTotal12EFGCN"
#define stringCmdAnalysisInitProjectionEFGCNTotal13EFGCN            "analysisInitProjectionEFGCNTotal13EFGCN"
#define stringCmdAnalysisInitProjectionEFGCNTotal32EFGCN            "analysisInitProjectionEFGCNTotal32EFGCN"
#define stringCmdAnalysisInitMomentumEvent                          "analysisInitMomentumEvent"
#define stringCmdAnalysisInitMomentumTotal                          "analysisInitMomentumTotal"
#define stringCmdAnalysisInitMomentumDisplay                        "analysisInitMomentumDisplay"
#define stringCmdAnalysisInitMomentumToRoot                         "analysisInitMomentumToRoot"
#define stringCmdAnalysisInitProjectionEvent                        "analysisInitProjectionEvent"
#define stringCmdAnalysisInitProjectionTotal                        "analysisInitProjectionTotal"
#define stringCmdAnalysisInitProjectionDisplay                      "analysisInitProjectionDisplay"
#define stringCmdAnalysisInitProjectionToRoot                       "analysisInitProjectionToRoot"
#define stringCmdAnalysisInitMagnetfieldX                           "analysisInitMagnetfieldX"
#define stringCmdAnalysisInitMagnetfieldY                           "analysisInitMagnetfieldY"
#define stringCmdAnalysisInitMagnetfieldZ                           "analysisInitMagnetfieldZ"
#define stringCmdAnalysisInitMagnetfieldDisplay                     "analysisInitMagnetfieldDisplay"
#define stringCmdAnalysisInitMagnetfieldToRoot                      "analysisInitMagnetfieldToRoot"
#define stringCmdAnalysisInitMagnetfieldConstantForEachEvent        "analysisInitMagnetfieldConstantForEachEvent"
#define stringCmdAnalysisInitWeightedMagnetfieldConstant            "analysisInitWeightedMagnetfieldConstant"
#define stringCmdAnalysisInitMagnetfieldConstantDisplay             "analysisInitMagnetfieldConstantDisplay"
#define stringCmdAnalysisMagnetfieldConstantDisplayMask             "analysisMagnetfieldConstantDisplayMask"
#define stringCmdAnalysisInitMagnetfieldConstantToRoot              "analysisInitMagnetfieldConstantToRoot"
#define stringCmdAnalysisInitMagnetfieldVSConstants                 "analysisInitMagnetfieldVSConstants"
#define stringCmdAnalysisInitPrelutGoodness                         "analysisInitPrelutGoodness"
#define stringCmdAnalysisInitLutGoodness                            "analysisInitLutGoodness"
#define stringCmdAnalysisInitHoughTransformGoodness                 "analysisInitHoughTransformGoodness"
#define stringCmdAnalysisInitQuantizationGoodness                   "analysisInitQuantizationGoodness"
#define stringCmdAnalysisInitPeakDistanceGoodness                   "analysisInitPeakDistanceGoodness"
#define stringCmdAnalysisInitCreatedHistogramToRoot                 "analysisInitCreatedHistogramToRoot"
#define stringCmdAnalysisInitEncodedHistogramToRoot                 "analysisInitEncodedHistogramToRoot"
#define stringCmdAnalysisInitFilteredHistogramToRoot                "analysisInitFilteredHistogramToRoot"
#define stringCmdAnalysisInitJustOneCreatedHistogramToRoot          "analysisInitJustOneCreatedHistogramToRoot"
#define stringCmdAnalysisInitJustOneEncodedHistogramToRoot          "analysisInitJustOneEncodedHistogramToRoot"
#define stringCmdAnalysisInitJustOneFilteredHistogramToRoot         "analysisInitJustOneFilteredHistogramToRoot"
#define stringCmdAnalysisInitCreatedHistogramToShow                 "analysisInitCreatedHistogramToShow"
#define stringCmdAnalysisInitEncodedHistogramToShow                 "analysisInitEncodedHistogramToShow"
#define stringCmdAnalysisInitFilteredHistogramToShow                "analysisInitFilteredHistogramToShow"
#define stringCmdAnalysisInitHistogramLayer                         "analysisInitHistogramLayer"
#define stringCmdAnalysisInitNumberOfTracksPerColumn				"analysisInitNumberOfTracksPerColumn"
#define stringCmdAnalysisInitNumberOfTracksPerRow    				"analysisInitNumberOfTracksPerRow"
#define stringCmdAnalysisInitNumberOfTracksPerLayer                 "analysisInitNumberOfTracksPerLayer"
#define stringCmdAnalysisInitHitReadoutDistribution                 "analysisInitHitReadoutDistribution"
#define stringCmdAnalysisInitReadoutColumnsInParallel               "analysisInitReadoutColumnsInParallel"
#define stringCmdAnalysisInitPrelutRangeForEachEvent                "analysisInitPrelutRangeForEachEvent"
#define stringCmdAnalysisInitWeightedPrelutRange                    "analysisInitWeightedPrelutRange"
#define stringCmdAnalysisInitPrelutRangeDisplay                     "analysisInitPrelutRangeDisplay"
#define stringCmdAnalysisInitPrelutRangeDisplayMode                 "analysisInitPrelutRangeDisplayMode"
#define stringCmdAnalysisPrelutRangeStationDisplayMask              "analysisPrelutRangeStationDisplayMask"
#define stringCmdAnalysisPrelutRangeStationSumDisplayMask           "analysisPrelutRangeStationSumDisplayMask"
#define stringCmdAnalysisPrelutRangeConstraintDisplayMask           "analysisPrelutRangeConstraintDisplayMask"
#define stringCmdAnalysisPrelutRangeConstraintSumDisplayMask        "analysisPrelutRangeConstraintSumDisplayMask"
#define stringCmdAnalysisPrelutRangeRelativeDisplayMask             "analysisPrelutRangeRelativeDisplayMask"
#define stringCmdAnalysisInitPrelutRangeToRoot                      "analysisInitPrelutRangeToRoot"
#define stringCmdAnalysisInitPercentageOfHitsForPrelutRange         "analysisInitPercentageOfHitsForPrelutRange"
#define stringCmdAnalysisPrelutRangeMinStart                        "analysisPrelutRangeMinStart"
#define stringCmdAnalysisPrelutRangeMinStop                         "analysisPrelutRangeMinStop"
#define stringCmdAnalysisPrelutRangeMinSteps                        "analysisPrelutRangeMinSteps"
#define stringCmdAnalysisPrelutRangeMaxStart                        "analysisPrelutRangeMaxStart"
#define stringCmdAnalysisPrelutRangeMaxStop                         "analysisPrelutRangeMaxStop"
#define stringCmdAnalysisPrelutRangeMaxSteps                        "analysisPrelutRangeMaxSteps"
#define stringCmdAnalysisChooseMainPrelutRange                      "analysisChooseMainPrelutRange"
#define stringCmdAnalysisChooseConstraintPrelutRange                "analysisChooseConstraintPrelutRange"
#define stringCmdAnalysisInitTotalAnalysis                          "analysisInitTotalAnalysis"
#define stringCmdAnalysisInitPercentageOfHitsInSignature            "analysisInitPercentageOfHitsInSignature"
#define stringCmdAnalysisInitPercentageOfTracksForSignature         "analysisInitPercentageOfTracksForSignature"
#define stringCmdAnalysisInitAnalysisResultWarnings                 "analysisInitAnalysisResultWarnings"
#define stringCmdAnalysisInitAnalysisResultDisplays                 "analysisInitAnalysisResultDisplays"
#define stringCmdAnalysisInitAnalysisMoreResultWarnings             "analysisInitAnalysisMoreResultWarnings"
#define stringCmdAnalysisInitAnalysisMoreResultDisplays             "analysisInitAnalysisMoreResultDisplays"
#define stringCmdAnalysisWriteCellFiles                             "analysisWriteCellFiles"
#define stringCmdAnalysisHitCellFileName						    "analysisHitCellFileName"
#define stringCmdAnalysisPrelutCellFileName						    "analysisPrelutCellFileName"
#define stringCmdAnalysisLutCellFileName						    "analysisLutCellFileName"
#define stringCmdInitStatus                                         "initStatus"

/*
 * index of each parameter for the commandID table
 */
#define idCmdInputFileName                                        0
#define idCmdInputFileFormat                                      1
#define idCmdInputMinTracks                                       2
#define idCmdInputMaxTracks                                       3
#define idCmdInputDetectorMask                                    4
#define idCmdInputFileNameMagneticField                           5
#define idCmdInputMagneticFieldIsRootFile                         6
#define idCmdInputMapNameMagneticField                            7
#define idCmdInputMagneticFieldIntegrationStepwidthPerStation     8
#define idCmdInputMagneticFieldIntegrationFactor                  9
#define idCmdInputDisableAutomaticMagneticField                  10
#define idCmdInputFileNameDetector                               11
#define idCmdInputNumberOfVolumesInfrontOfSTS                    12
#define idCmdInputDisableAutomaticDetector                       13
#define idCmdInputCodingTableMode                                14
#define idCmdInputGradingPTableMode                              15
#define idCmdInputGradingRTableMode                              16
#define idCmdInputCodingTableFileName                            17
#define idCmdInputGradingPTableFileName                          18
#define idCmdInputGradingRTableFileName                          19
#define idCmdInputCodingTableWrite                               20
#define idCmdInputGradingPTableWrite                             21
#define idCmdInputGradingRTableWrite                             22
#define idCmdTrackfinderHitProducer                              23
#define idCmdTrackfinderReadPointsFromFile                       24
#define idCmdTrackfinderReadHitsFromFile                         25
#define idCmdTrackfinderReadMapsHits                             26
#define idCmdTrackfinderReadHybridHits                           27
#define idCmdTrackfinderReadStripHits                            28
#define idCmdTrackfinderLutsVersion                              29
#define idCmdTrackfinderPrelutFileName                           30
#define idCmdTrackfinderLutFileName                              31
#define idCmdTrackfinderGammaMin                                 32
#define idCmdTrackfinderGammaMax                                 33
#define idCmdTrackfinderGammaStep                                34
#define idCmdTrackfinderThetaMin                                 35
#define idCmdTrackfinderThetaMax                                 36
#define idCmdTrackfinderThetaStep                                37
#define idCmdTrackfinderPrelutRadiusMin                          38
#define idCmdTrackfinderPrelutRadiusMax                          39
#define idCmdTrackfinderLutRadiusMin                             40
#define idCmdTrackfinderLutRadiusMax                             41
#define idCmdTrackfinderLutRadiusStep                            42
#define idCmdTrackfinderMinClassCoding                           43
#define idCmdTrackfinderMinClassGradingP                         44
#define idCmdTrackfinderMinClassGradingR                         45
#define idCmdTrackfinderFilterType                               46
#define idCmdTrackfinderFirstFilterGeometry                      47
#define idCmdTrackfinderFirstFilterArithmetic                    48
#define idCmdTrackfinderSecondFilterGeometry                     49
#define idCmdTrackfinderSecondFilterArithmetic                   50
#define idCmdTrackfinderFirstFilterNeighborhoodDim1ClearRadius   51
#define idCmdTrackfinderFirstFilterNeighborhoodDim2ClearRadius   52
#define idCmdTrackfinderSecondFilterNeighborhoodDim1ClearRadius  53
#define idCmdTrackfinderSecondFilterNeighborhoodDim2ClearRadius  54
#define idCmdTrackfinderSecondFilterNeighborhoodDim3ClearRadius  55
#define idCmdTrackfinderAutomaticFilterCoverPercentage           56
#define idCmdTrackfinderAutomaticFilterDataPercentage            57
#define idCmdTrackfinderAutomaticFilterWrite                     58
#define idCmdTrackfinderAutomaticFilterFileName                  69
#define idCmdTrackfinderWriteTracksToFile                        60
#define idCmdAnalysisOutputFileName                              61
#define idCmdAnalysisThresholdForP                               62
#define idCmdAnalysisInitConfiguration                           63
#define idCmdAnalysisInitDetector                                64
#define idCmdAnalysisInitEvent                                   65
#define idCmdAnalysisInitClassPriority                           66
#define idCmdAnalysisInitMemory                                  67
#define idCmdAnalysisInitTime                                    68
#define idCmdAnalysisInitTrackPropagationEventPoint              69
#define idCmdAnalysisInitTrackPropagationEventHit                70
#define idCmdAnalysisInitTrackPropagationTotalPoint              71
#define idCmdAnalysisInitTrackPropagationTotalHit                72
#define idCmdAnalysisInitTrackPropagationDisplay                 73
#define idCmdAnalysisInitTrackPropagationToRoot                  74
#define idCmdAnalysisTrackPropagationPointDisplayMask            75
#define idCmdAnalysisTrackPropagationHitDisplayMask              76
#define idCmdAnalysisInitQualityEFGCEventAbsolute                77
#define idCmdAnalysisInitQualityEFGCEventRelative                78
#define idCmdAnalysisInitQualityEFGCTotalAbsolute                79
#define idCmdAnalysisInitQualityEFGCTotalRelative                80
#define idCmdAnalysisInitMomentumEFGCEventPzEFGC                 81
#define idCmdAnalysisInitMomentumEFGCEventPtEFGC                 82
#define idCmdAnalysisInitMomentumEFGCTotalPzEFGC                 83
#define idCmdAnalysisInitMomentumEFGCTotalPtEFGC                 84
#define idCmdAnalysisInitProjectionEFGCNEvent12EFGCN             85
#define idCmdAnalysisInitProjectionEFGCNEvent13EFGCN             86
#define idCmdAnalysisInitProjectionEFGCNEvent32EFGCN             87
#define idCmdAnalysisInitProjectionEFGCNTotal12EFGCN             88
#define idCmdAnalysisInitProjectionEFGCNTotal13EFGCN             89
#define idCmdAnalysisInitProjectionEFGCNTotal32EFGCN             90
#define idCmdAnalysisInitMomentumEvent                           91
#define idCmdAnalysisInitMomentumTotal                           92
#define idCmdAnalysisInitMomentumDisplay                         93
#define idCmdAnalysisInitMomentumToRoot                          94
#define idCmdAnalysisInitProjectionEvent                         95
#define idCmdAnalysisInitProjectionTotal                         96
#define idCmdAnalysisInitProjectionDisplay                       97
#define idCmdAnalysisInitProjectionToRoot                        98
#define idCmdAnalysisInitMagnetfieldX                            99
#define idCmdAnalysisInitMagnetfieldY                           100
#define idCmdAnalysisInitMagnetfieldZ                           101
#define idCmdAnalysisInitMagnetfieldDisplay                     102
#define idCmdAnalysisInitMagnetfieldToRoot                      103
#define idCmdAnalysisInitMagnetfieldConstantForEachEvent        104
#define idCmdAnalysisInitWeightedMagnetfieldConstant            105
#define idCmdAnalysisInitMagnetfieldConstantDisplay             106
#define idCmdAnalysisMagnetfieldConstantDisplayMask             107
#define idCmdAnalysisInitMagnetfieldConstantToRoot              108
#define idCmdAnalysisInitMagnetfieldVSConstants                 109
#define idCmdAnalysisInitPrelutGoodness                         110
#define idCmdAnalysisInitLutGoodness                            111
#define idCmdAnalysisInitHoughTransformGoodness                 112
#define idCmdAnalysisInitQuantizationGoodness                   113
#define idCmdAnalysisInitPeakDistanceGoodness                   114
#define idCmdAnalysisInitCreatedHistogramToRoot                 115
#define idCmdAnalysisInitEncodedHistogramToRoot                 116
#define idCmdAnalysisInitFilteredHistogramToRoot                117
#define idCmdAnalysisInitJustOneCreatedHistogramToRoot          118
#define idCmdAnalysisInitJustOneEncodedHistogramToRoot          119
#define idCmdAnalysisInitJustOneFilteredHistogramToRoot         120
#define idCmdAnalysisInitCreatedHistogramToShow                 121
#define idCmdAnalysisInitEncodedHistogramToShow                 122
#define idCmdAnalysisInitFilteredHistogramToShow                123
#define idCmdAnalysisInitHistogramLayer                         124
#define idCmdAnalysisInitNumberOfTracksPerColumn                125
#define idCmdAnalysisInitNumberOfTracksPerRow                   126
#define idCmdAnalysisInitNumberOfTracksPerLayer                 127
#define idCmdAnalysisInitHitReadoutDistribution                 128
#define idCmdAnalysisInitReadoutColumnsInParallel               129
#define idCmdAnalysisInitPrelutRangeForEachEvent                130
#define idCmdAnalysisInitWeightedPrelutRange                    131
#define idCmdAnalysisInitPrelutRangeDisplay                     132
#define idCmdAnalysisInitPrelutRangeDisplayMode                 133
#define idCmdAnalysisPrelutRangeStationDisplayMask              134
#define idCmdAnalysisPrelutRangeStationSumDisplayMask           135
#define idCmdAnalysisPrelutRangeConstraintDisplayMask           136
#define idCmdAnalysisPrelutRangeConstraintSumDisplayMask        137
#define idCmdAnalysisPrelutRangeRelativeDisplayMask             138
#define idCmdAnalysisInitPrelutRangeToRoot                      139
#define idCmdAnalysisInitPercentageOfHitsForPrelutRange         140
#define idCmdAnalysisPrelutRangeMinStart                        141
#define idCmdAnalysisPrelutRangeMinStop                         142
#define idCmdAnalysisPrelutRangeMinSteps                        143
#define idCmdAnalysisPrelutRangeMaxStart                        144
#define idCmdAnalysisPrelutRangeMaxStop                         145
#define idCmdAnalysisPrelutRangeMaxSteps                        146
#define idCmdAnalysisChooseMainPrelutRange                      147
#define idCmdAnalysisChooseConstraintPrelutRange                148
#define idCmdAnalysisInitTotalAnalysis                          149
#define idCmdAnalysisInitPercentageOfHitsInSignature            150
#define idCmdAnalysisInitPercentageOfTracksForSignature         151
#define idCmdAnalysisInitAnalysisResultWarnings                 152
#define idCmdAnalysisInitAnalysisResultDisplays                 153
#define idCmdAnalysisInitAnalysisMoreResultWarnings             154
#define idCmdAnalysisInitAnalysisMoreResultDisplays             155
#define idCmdAnalysisWriteCellFiles                             156
#define idCmdAnalysisHitCellFileName						    157
#define idCmdAnalysisPrelutCellFileName						    158
#define idCmdAnalysisLutCellFileName						    159
#define idCmdInitStatus                                         160


#define max(a, b)  (((a) > (b)) ? (a) : (b)) 


/****************************************************************
 * This method assigns the value to the corresponding parameter	*
 * which is identified by a specifier.							*
 ****************************************************************/

bool inf::getInputHeaderValue(std::string& specifier, std::string& value) {

	bool specifierFound = false;
	
	if (specifier.compare(stringCmdInputFileName) == 0) {
		if (!isHeaderLockSet(idCmdInputFileName)) {
			config.inputFileName = value;
			setHeaderLock(idCmdInputFileName, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputFileFormat) == 0) {
		if (!isHeaderLockSet(idCmdInputFileFormat)) {
			config.inputFileFormat = stoi((char*)value.c_str(), 10);
			setHeaderLock(idCmdInputFileFormat, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputMinTracks) == 0) {
		if (!isHeaderLockSet(idCmdInputMinTracks)) {
			config.inputMinTracks = stoi((char*)value.c_str(), 10);
			setHeaderLock(idCmdInputMinTracks, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputMaxTracks) == 0) {
		if (!isHeaderLockSet(idCmdInputMaxTracks)) {
			config.inputMaxTracks = stoi((char*)value.c_str(), 10);
			setHeaderLock(idCmdInputMaxTracks, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputDetectorMask) == 0) {
		if (!isHeaderLockSet(idCmdInputDetectorMask)) {
			config.inputDetectorMask = bitArray(value);
			setHeaderLock(idCmdInputDetectorMask, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputFileNameDetector) == 0) {
		if (!isHeaderLockSet(idCmdInputFileNameDetector)) {
			config.inputFileNameDetector = value;
			setHeaderLock(idCmdInputFileNameDetector, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputNumberOfVolumesInfrontOfSTS) == 0) {
		if (!isHeaderLockSet(idCmdInputNumberOfVolumesInfrontOfSTS)) {
			config.inputNumberOfVolumesInfrontOfSTS = stous((char*)value.c_str(), 10);
			setHeaderLock(idCmdInputNumberOfVolumesInfrontOfSTS, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputDisableAutomaticDetector) == 0) {
		if (!isHeaderLockSet(idCmdInputDisableAutomaticDetector)) {
			config.inputDisableAutomaticDetector = bstob((char*)value.c_str());
			if (!config.inputDisableAutomaticDetector)
				config.inputDisableAutomaticDetector = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdInputDisableAutomaticDetector, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputCodingTableMode) == 0) {
		if (!isHeaderLockSet(idCmdInputCodingTableMode)) {
			config.inputCodingTableMode = stoi((char*)value.c_str(), 10);
			setHeaderLock(idCmdInputCodingTableMode, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputGradingPTableMode) == 0) {
		if (!isHeaderLockSet(idCmdInputGradingPTableMode)) {
			config.inputGradingPTableMode = stoi((char*)value.c_str(), 10);
			setHeaderLock(idCmdInputGradingPTableMode, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputGradingRTableMode) == 0) {
		if (!isHeaderLockSet(idCmdInputGradingRTableMode)) {
			config.inputGradingRTableMode = stoi((char*)value.c_str(), 10);
			setHeaderLock(idCmdInputGradingRTableMode, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputCodingTableFileName) == 0) {
		if (!isHeaderLockSet(idCmdInputCodingTableFileName)) {
			config.inputCodingTableFileName = value;
			setHeaderLock(idCmdInputCodingTableFileName, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputGradingPTableFileName) == 0) {
		if (!isHeaderLockSet(idCmdInputGradingPTableFileName)) {
			config.inputGradingPTableFileName = value;
			setHeaderLock(idCmdInputGradingPTableFileName, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputGradingRTableFileName) == 0) {
		if (!isHeaderLockSet(idCmdInputGradingRTableFileName)) {
			config.inputGradingRTableFileName = value;
			setHeaderLock(idCmdInputGradingRTableFileName, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputCodingTableWrite) == 0) {
		if (!isHeaderLockSet(idCmdInputCodingTableWrite)) {
			config.inputCodingTableWrite = bstob((char*)value.c_str());
			if (!config.inputCodingTableWrite)
				config.inputCodingTableWrite = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdInputCodingTableWrite, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputGradingPTableWrite) == 0) {
		if (!isHeaderLockSet(idCmdInputGradingPTableWrite)) {
			config.inputGradingPTableWrite = bstob((char*)value.c_str());
			if (!config.inputGradingPTableWrite)
				config.inputGradingPTableWrite = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdInputGradingPTableWrite, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputGradingRTableWrite) == 0) {
		if (!isHeaderLockSet(idCmdInputGradingRTableWrite)) {
			config.inputGradingRTableWrite = bstob((char*)value.c_str());
			if (!config.inputGradingRTableWrite)
				config.inputGradingRTableWrite = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdInputGradingRTableWrite, true);
			specifierFound = true;
		}
	}

	return specifierFound;

}

/****************************************************************
 * This method assigns the value to the corresponding parameter	*
 * which is identified by a specifier.							*
 ****************************************************************/

bool inf::getTrackfinderHeaderValue(std::string& specifier, std::string& value) {

	bool specifierFound = false;
	
	if (specifier.compare(stringCmdTrackfinderHitProducer) == 0) {
		if (!isHeaderLockSet(idCmdTrackfinderHitProducer)) {
			config.trackfinderHitProducer = stoi((char*)value.c_str(), 10);
			setHeaderLock(idCmdTrackfinderHitProducer, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderReadPointsFromFile) == 0) {
		if (!isHeaderLockSet(idCmdTrackfinderReadPointsFromFile)) {
			config.trackfinderReadPointsFromFile = bstob((char*)value.c_str());
			if (!config.trackfinderReadPointsFromFile)
				config.trackfinderReadPointsFromFile = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdTrackfinderReadPointsFromFile, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderReadHitsFromFile) == 0) {
		if (!isHeaderLockSet(idCmdTrackfinderReadHitsFromFile)) {
			config.trackfinderReadHitsFromFile = bstob((char*)value.c_str());
			if (!config.trackfinderReadHitsFromFile)
				config.trackfinderReadHitsFromFile = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdTrackfinderReadHitsFromFile, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderReadMapsHits) == 0) {
		if (!isHeaderLockSet(idCmdTrackfinderReadMapsHits)) {
			config.trackfinderReadMapsHits = bstob((char*)value.c_str());
			if (!config.trackfinderReadMapsHits)
				config.trackfinderReadMapsHits = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdTrackfinderReadMapsHits, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderReadHybridHits) == 0) {
		if (!isHeaderLockSet(idCmdTrackfinderReadHybridHits)) {
			config.trackfinderReadHybridHits = bstob((char*)value.c_str());
			if (!config.trackfinderReadHybridHits)
				config.trackfinderReadHybridHits = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdTrackfinderReadHybridHits, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderReadStripHits) == 0) {
		if (!isHeaderLockSet(idCmdTrackfinderReadStripHits)) {
			config.trackfinderReadStripHits = bstob((char*)value.c_str());
			if (!config.trackfinderReadStripHits)
				config.trackfinderReadStripHits = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdTrackfinderReadStripHits, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderLutsVersion) == 0) {
		if (!isHeaderLockSet(idCmdTrackfinderLutsVersion)) {
			config.trackfinderLutsVersion = stous((char*)value.c_str(), 10);
			setHeaderLock(idCmdTrackfinderLutsVersion, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderPrelutFileName) == 0) {
		if (!isHeaderLockSet(idCmdTrackfinderPrelutFileName)) {
			config.trackfinderPrelutFileName = value;
			setHeaderLock(idCmdTrackfinderPrelutFileName, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderLutFileName) == 0) {
		if (!isHeaderLockSet(idCmdTrackfinderLutFileName)) {
			config.trackfinderLutFileName = value;
			setHeaderLock(idCmdTrackfinderLutFileName, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderGammaMin) == 0) {
		if (!isHeaderLockSet(idCmdTrackfinderGammaMin)) {
			config.trackfinderGammaMin = stod((char*)value.c_str());
			setHeaderLock(idCmdTrackfinderGammaMin, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderGammaMax) == 0) {
		if (!isHeaderLockSet(idCmdTrackfinderGammaMax)) {
			config.trackfinderGammaMax = stod((char*)value.c_str());
			setHeaderLock(idCmdTrackfinderGammaMax, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderGammaStep) == 0) {
		if (!isHeaderLockSet(idCmdTrackfinderGammaStep)) {
			config.trackfinderGammaStep = stoi((char*)value.c_str(), 10);
			setHeaderLock(idCmdTrackfinderGammaStep, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderThetaMin) == 0) {
		if (!isHeaderLockSet(idCmdTrackfinderThetaMin)) {
			config.trackfinderThetaMin = stod((char*)value.c_str());
			setHeaderLock(idCmdTrackfinderThetaMin, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderThetaMax) == 0) {
		if (!isHeaderLockSet(idCmdTrackfinderThetaMax)) {
			config.trackfinderThetaMax = stod((char*)value.c_str());
			setHeaderLock(idCmdTrackfinderThetaMax, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderThetaStep) == 0) {
		if (!isHeaderLockSet(idCmdTrackfinderThetaStep)) {
			config.trackfinderThetaStep = stoi((char*)value.c_str(), 10);
			setHeaderLock(idCmdTrackfinderThetaStep, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderPrelutRadiusMin) == 0) {
		if (!isHeaderLockSet(idCmdTrackfinderPrelutRadiusMin)) {
			config.trackfinderPrelutRadiusMin = stod((char*)value.c_str());
			setHeaderLock(idCmdTrackfinderPrelutRadiusMin, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderPrelutRadiusMax) == 0) {
		if (!isHeaderLockSet(idCmdTrackfinderPrelutRadiusMax)) {
			config.trackfinderPrelutRadiusMax = stod((char*)value.c_str());
			setHeaderLock(idCmdTrackfinderPrelutRadiusMax, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderLutRadiusMin) == 0) {
		if (!isHeaderLockSet(idCmdTrackfinderLutRadiusMin)) {
			config.trackfinderLutRadiusMin = stod((char*)value.c_str());
			setHeaderLock(idCmdTrackfinderLutRadiusMin, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderLutRadiusMax) == 0) {
		if (!isHeaderLockSet(idCmdTrackfinderLutRadiusMax)) {
			config.trackfinderLutRadiusMax = stod((char*)value.c_str());
			setHeaderLock(idCmdTrackfinderLutRadiusMax, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderLutRadiusStep) == 0) {
		if (!isHeaderLockSet(idCmdTrackfinderLutRadiusStep)) {
			config.trackfinderLutRadiusStep = stoi((char*)value.c_str(), 10);
			setHeaderLock(idCmdTrackfinderLutRadiusStep, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderMinClassCoding) == 0) {
		if (!isHeaderLockSet(idCmdTrackfinderMinClassCoding)) {
			config.trackfinderMinClassCoding = stous((char*)value.c_str(), 10);
			setHeaderLock(idCmdTrackfinderMinClassCoding, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderMinClassGradingP) == 0) {
		if (!isHeaderLockSet(idCmdTrackfinderMinClassGradingP)) {
			config.trackfinderMinClassGradingP = stous((char*)value.c_str(), 10);
			setHeaderLock(idCmdTrackfinderMinClassGradingP, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderMinClassGradingR) == 0) {
		if (!isHeaderLockSet(idCmdTrackfinderMinClassGradingR)) {
			config.trackfinderMinClassGradingR = stous((char*)value.c_str(), 10);
			setHeaderLock(idCmdTrackfinderMinClassGradingR, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderFilterType) == 0) {
		if (!isHeaderLockSet(idCmdTrackfinderFilterType)) {
			config.trackfinderFilterType = stous((char*)value.c_str(), 10);
			setHeaderLock(idCmdTrackfinderFilterType, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderFirstFilterGeometry) == 0) {
		if (!isHeaderLockSet(idCmdTrackfinderFirstFilterGeometry)) {
			config.trackfinderFirstFilterGeometry = stous((char*)value.c_str(), 10);
			setHeaderLock(idCmdTrackfinderFirstFilterGeometry, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderFirstFilterArithmetic) == 0) {
		if (!isHeaderLockSet(idCmdTrackfinderFirstFilterArithmetic)) {
			config.trackfinderFirstFilterArithmetic = stous((char*)value.c_str(), 10);
			setHeaderLock(idCmdTrackfinderFirstFilterArithmetic, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderSecondFilterGeometry) == 0) {
		if (!isHeaderLockSet(idCmdTrackfinderSecondFilterGeometry)) {
			config.trackfinderSecondFilterGeometry = stous((char*)value.c_str(), 10);
			setHeaderLock(idCmdTrackfinderSecondFilterGeometry, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderSecondFilterArithmetic) == 0) {
		if (!isHeaderLockSet(idCmdTrackfinderSecondFilterArithmetic)) {
			config.trackfinderSecondFilterArithmetic = stous((char*)value.c_str(), 10);
			setHeaderLock(idCmdTrackfinderSecondFilterArithmetic, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderFirstFilterNeighborhoodDim1ClearRadius) == 0) {
		if (!isHeaderLockSet(idCmdTrackfinderFirstFilterNeighborhoodDim1ClearRadius)) {
			config.trackfinderFirstFilterNeighborhoodDim1ClearRadius = stous((char*)value.c_str(), 10);
			setHeaderLock(idCmdTrackfinderFirstFilterNeighborhoodDim1ClearRadius, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderFirstFilterNeighborhoodDim2ClearRadius) == 0) {
		if (!isHeaderLockSet(idCmdTrackfinderFirstFilterNeighborhoodDim2ClearRadius)) {
			config.trackfinderFirstFilterNeighborhoodDim2ClearRadius = stous((char*)value.c_str(), 10);
			setHeaderLock(idCmdTrackfinderFirstFilterNeighborhoodDim2ClearRadius, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderSecondFilterNeighborhoodDim1ClearRadius) == 0) {
		if (!isHeaderLockSet(idCmdTrackfinderSecondFilterNeighborhoodDim1ClearRadius)) {
			config.trackfinderSecondFilterNeighborhoodDim1ClearRadius = stous((char*)value.c_str(), 10);
			setHeaderLock(idCmdTrackfinderSecondFilterNeighborhoodDim1ClearRadius, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderSecondFilterNeighborhoodDim2ClearRadius) == 0) {
		if (!isHeaderLockSet(idCmdTrackfinderSecondFilterNeighborhoodDim2ClearRadius)) {
			config.trackfinderSecondFilterNeighborhoodDim2ClearRadius = stous((char*)value.c_str(), 10);
			setHeaderLock(idCmdTrackfinderSecondFilterNeighborhoodDim2ClearRadius, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderSecondFilterNeighborhoodDim3ClearRadius) == 0) {
		if (!isHeaderLockSet(idCmdTrackfinderSecondFilterNeighborhoodDim3ClearRadius)) {
			config.trackfinderSecondFilterNeighborhoodDim3ClearRadius = stous((char*)value.c_str(), 10);
			setHeaderLock(idCmdTrackfinderSecondFilterNeighborhoodDim3ClearRadius, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderAutomaticFilterCoverPercentage) == 0) {
		if (!isHeaderLockSet(idCmdTrackfinderAutomaticFilterCoverPercentage)) {
			config.trackfinderAutomaticFilterCoverPercentage = stous((char*)value.c_str(), 10);
			if (config.trackfinderAutomaticFilterCoverPercentage > 100)
				config.trackfinderAutomaticFilterCoverPercentage = 100;
			setHeaderLock(idCmdTrackfinderAutomaticFilterCoverPercentage, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderAutomaticFilterDataPercentage) == 0) {
		if (!isHeaderLockSet(idCmdTrackfinderAutomaticFilterDataPercentage)) {
			config.trackfinderAutomaticFilterDataPercentage = stous((char*)value.c_str(), 10);
			if (config.trackfinderAutomaticFilterDataPercentage > 100)
				config.trackfinderAutomaticFilterDataPercentage = 100;
			setHeaderLock(idCmdTrackfinderAutomaticFilterDataPercentage, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderAutomaticFilterWrite) == 0) {
		if (!isHeaderLockSet(idCmdTrackfinderAutomaticFilterWrite)) {
			config.trackfinderAutomaticFilterWrite = bstob((char*)value.c_str());
			if (!config.trackfinderAutomaticFilterWrite)
				config.trackfinderAutomaticFilterWrite = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdTrackfinderAutomaticFilterWrite, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderAutomaticFilterFileName) == 0) {
		if (!isHeaderLockSet(idCmdTrackfinderAutomaticFilterFileName)) {
			config.trackfinderAutomaticFilterFileName = value;
			setHeaderLock(idCmdTrackfinderAutomaticFilterFileName, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdTrackfinderWriteTracksToFile) == 0) {
		if (!isHeaderLockSet(idCmdTrackfinderWriteTracksToFile)) {
			config.trackfinderWriteTracksToFile = bstob((char*)value.c_str());
			if (!config.trackfinderWriteTracksToFile)
				config.trackfinderWriteTracksToFile = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdTrackfinderWriteTracksToFile, true);
			specifierFound = true;
		}
	}

	return specifierFound;

}

/****************************************************************
 * This method assigns the value to the corresponding parameter	*
 * which is identified by a specifier.							*
 ****************************************************************/

bool inf::getInitialisationHeaderValue(std::string& specifier, std::string& value) {

	bool specifierFound = false;

	if (specifier.compare(stringCmdAnalysisThresholdForP) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisThresholdForP)) {
			config.analysisThresholdForP = stod((char*)value.c_str());
			setHeaderLock(idCmdAnalysisThresholdForP, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitConfiguration) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitConfiguration)) {
			config.analysisInitConfiguration = bstob((char*)value.c_str());
			if (!config.analysisInitConfiguration)
				config.analysisInitConfiguration = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitConfiguration, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitDetector) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitDetector)) {
			config.analysisInitDetector = bstob((char*)value.c_str());
			if (!config.analysisInitDetector)
				config.analysisInitDetector = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitDetector, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitEvent) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitEvent)) {
			config.analysisInitEvent = bstob((char*)value.c_str());
			if (!config.analysisInitEvent)
				config.analysisInitEvent = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitEvent, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitClassPriority) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitClassPriority)) {
			config.analysisInitClassPriority = bstob((char*)value.c_str());
			if (!config.analysisInitClassPriority)
				config.analysisInitClassPriority = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitClassPriority, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitMemory) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitMemory)) {
			config.analysisInitMemory = bstob((char*)value.c_str());
			if (!config.analysisInitMemory)
				config.analysisInitMemory = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitMemory, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitTime) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitTime)) {
			config.analysisInitTime = bstob((char*)value.c_str());
			if (!config.analysisInitTime)
				config.analysisInitTime = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitTime, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitTrackPropagationEventPoint) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitTrackPropagationEventPoint)) {
			config.analysisInitTrackPropagationEventPoint = bstob((char*)value.c_str());
			if (!config.analysisInitTrackPropagationEventPoint)
				config.analysisInitTrackPropagationEventPoint = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitTrackPropagationEventPoint, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitTrackPropagationEventHit) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitTrackPropagationEventHit)) {
			config.analysisInitTrackPropagationEventHit = bstob((char*)value.c_str());
			if (!config.analysisInitTrackPropagationEventHit)
				config.analysisInitTrackPropagationEventHit = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitTrackPropagationEventHit, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitTrackPropagationTotalPoint) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitTrackPropagationTotalPoint)) {
			config.analysisInitTrackPropagationTotalPoint = bstob((char*)value.c_str());
			if (!config.analysisInitTrackPropagationTotalPoint)
				config.analysisInitTrackPropagationTotalPoint = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitTrackPropagationTotalPoint, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitTrackPropagationTotalHit) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitTrackPropagationTotalHit)) {
			config.analysisInitTrackPropagationTotalHit = bstob((char*)value.c_str());
			if (!config.analysisInitTrackPropagationTotalHit)
				config.analysisInitTrackPropagationTotalHit = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitTrackPropagationTotalHit, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitTrackPropagationDisplay) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitTrackPropagationDisplay)) {
			config.analysisInitTrackPropagationDisplay = bstob((char*)value.c_str());
			if (!config.analysisInitTrackPropagationDisplay)
				config.analysisInitTrackPropagationDisplay = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitTrackPropagationDisplay, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitTrackPropagationToRoot) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitTrackPropagationToRoot)) {
			config.analysisInitTrackPropagationToRoot = bstob((char*)value.c_str());
			if (!config.analysisInitTrackPropagationToRoot)
				config.analysisInitTrackPropagationToRoot = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitTrackPropagationToRoot, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisTrackPropagationPointDisplayMask) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisTrackPropagationPointDisplayMask)) {
			config.analysisTrackPropagationPointDisplayMask = bitArray(value);
			setHeaderLock(idCmdAnalysisTrackPropagationPointDisplayMask, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisTrackPropagationHitDisplayMask) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisTrackPropagationHitDisplayMask)) {
			config.analysisTrackPropagationHitDisplayMask = bitArray(value);
			setHeaderLock(idCmdAnalysisTrackPropagationHitDisplayMask, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInitStatus) == 0) {
		if (!isHeaderLockSet(idCmdInitStatus)) {
			config.initStatus = bstob((char*)value.c_str());
			if (!config.initStatus)
				config.initStatus = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdInitStatus, true);
			specifierFound = true;
		}
	}

	return specifierFound;

}

/****************************************************************
 * This method assigns the value to the corresponding parameter	*
 * which is identified by a specifier.							*
 ****************************************************************/

bool inf::getQualityHeaderValue(std::string& specifier, std::string& value) {

	bool specifierFound = false;

	if (specifier.compare(stringCmdAnalysisInitQualityEFGCEventAbsolute) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitQualityEFGCEventAbsolute)) {
			config.analysisInitQualityEFGCEventAbsolute = bstob((char*)value.c_str());
			if (!config.analysisInitQualityEFGCEventAbsolute)
				config.analysisInitQualityEFGCEventAbsolute = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitQualityEFGCEventAbsolute, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitQualityEFGCEventRelative) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitQualityEFGCEventRelative)) {
			config.analysisInitQualityEFGCEventRelative = bstob((char*)value.c_str());
			if (!config.analysisInitQualityEFGCEventRelative)
				config.analysisInitQualityEFGCEventRelative = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitQualityEFGCEventRelative, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitQualityEFGCTotalAbsolute) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitQualityEFGCTotalAbsolute)) {
			config.analysisInitQualityEFGCTotalAbsolute = bstob((char*)value.c_str());
			if (!config.analysisInitQualityEFGCTotalAbsolute)
				config.analysisInitQualityEFGCTotalAbsolute = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitQualityEFGCTotalAbsolute, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitQualityEFGCTotalRelative) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitQualityEFGCTotalRelative)) {
			config.analysisInitQualityEFGCTotalRelative = bstob((char*)value.c_str());
			if (!config.analysisInitQualityEFGCTotalRelative)
				config.analysisInitQualityEFGCTotalRelative = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitQualityEFGCTotalRelative, true);
			specifierFound = true;
		}
	}

	return specifierFound;

}

/****************************************************************
 * This method assigns the value to the corresponding parameter	*
 * which is identified by a specifier.							*
 ****************************************************************/

bool inf::getMomentumHeaderValue(std::string& specifier, std::string& value) {

	bool specifierFound = false;

	if (specifier.compare(stringCmdAnalysisInitMomentumEFGCEventPzEFGC) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitMomentumEFGCEventPzEFGC)) {
			config.analysisInitMomentumEFGCEventPzEFGC = bstob((char*)value.c_str());
			if (!config.analysisInitMomentumEFGCEventPzEFGC)
				config.analysisInitMomentumEFGCEventPzEFGC = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitMomentumEFGCEventPzEFGC, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitMomentumEFGCEventPtEFGC) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitMomentumEFGCEventPtEFGC)) {
			config.analysisInitMomentumEFGCEventPtEFGC = bstob((char*)value.c_str());
			if (!config.analysisInitMomentumEFGCEventPtEFGC)
				config.analysisInitMomentumEFGCEventPtEFGC = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitMomentumEFGCEventPtEFGC, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitMomentumEFGCTotalPzEFGC) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitMomentumEFGCTotalPzEFGC)) {
			config.analysisInitMomentumEFGCTotalPzEFGC = bstob((char*)value.c_str());
			if (!config.analysisInitMomentumEFGCTotalPzEFGC)
				config.analysisInitMomentumEFGCTotalPzEFGC = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitMomentumEFGCTotalPzEFGC, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitMomentumEFGCTotalPtEFGC) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitMomentumEFGCTotalPtEFGC)) {
			config.analysisInitMomentumEFGCTotalPtEFGC = bstob((char*)value.c_str());
			if (!config.analysisInitMomentumEFGCTotalPtEFGC)
				config.analysisInitMomentumEFGCTotalPtEFGC = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitMomentumEFGCTotalPtEFGC, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitMomentumEvent) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitMomentumEvent)) {
			config.analysisInitMomentumEvent = bstob((char*)value.c_str());
			if (!config.analysisInitMomentumEvent)
				config.analysisInitMomentumEvent = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitMomentumEvent, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitMomentumTotal) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitMomentumTotal)) {
			config.analysisInitMomentumTotal = bstob((char*)value.c_str());
			if (!config.analysisInitMomentumTotal)
				config.analysisInitMomentumTotal = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitMomentumTotal, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitMomentumDisplay) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitMomentumDisplay)) {
			config.analysisInitMomentumDisplay = bstob((char*)value.c_str());
			if (!config.analysisInitMomentumDisplay)
				config.analysisInitMomentumDisplay = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitMomentumDisplay, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitMomentumToRoot) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitMomentumToRoot)) {
			config.analysisInitMomentumToRoot = bstob((char*)value.c_str());
			if (!config.analysisInitMomentumToRoot)
				config.analysisInitMomentumToRoot = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitMomentumToRoot, true);
			specifierFound = true;
		}
	}

	return specifierFound;

}

/****************************************************************
 * This method assigns the value to the corresponding parameter	*
 * which is identified by a specifier.							*
 ****************************************************************/

bool inf::getProjectionHeaderValue(std::string& specifier, std::string& value) {

	bool specifierFound = false;

	if (specifier.compare(stringCmdAnalysisInitProjectionEFGCNEvent12EFGCN) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitProjectionEFGCNEvent12EFGCN)) {
			config.analysisInitProjectionEFGCNEvent12EFGCN = bstob((char*)value.c_str());
			if (!config.analysisInitProjectionEFGCNEvent12EFGCN)
				config.analysisInitProjectionEFGCNEvent12EFGCN = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitProjectionEFGCNEvent12EFGCN, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitProjectionEFGCNEvent13EFGCN) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitProjectionEFGCNEvent13EFGCN)) {
			config.analysisInitProjectionEFGCNEvent13EFGCN = bstob((char*)value.c_str());
			if (!config.analysisInitProjectionEFGCNEvent13EFGCN)
				config.analysisInitProjectionEFGCNEvent13EFGCN = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitProjectionEFGCNEvent13EFGCN, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitProjectionEFGCNEvent32EFGCN) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitProjectionEFGCNEvent32EFGCN)) {
			config.analysisInitProjectionEFGCNEvent32EFGCN = bstob((char*)value.c_str());
			if (!config.analysisInitProjectionEFGCNEvent32EFGCN)
				config.analysisInitProjectionEFGCNEvent32EFGCN = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitProjectionEFGCNEvent32EFGCN, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitProjectionEFGCNTotal12EFGCN) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitProjectionEFGCNTotal12EFGCN)) {
			config.analysisInitProjectionEFGCNTotal12EFGCN = bstob((char*)value.c_str());
			if (!config.analysisInitProjectionEFGCNTotal12EFGCN)
				config.analysisInitProjectionEFGCNTotal12EFGCN = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitProjectionEFGCNTotal12EFGCN, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitProjectionEFGCNTotal13EFGCN) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitProjectionEFGCNTotal13EFGCN)) {
			config.analysisInitProjectionEFGCNTotal13EFGCN = bstob((char*)value.c_str());
			if (!config.analysisInitProjectionEFGCNTotal13EFGCN)
				config.analysisInitProjectionEFGCNTotal13EFGCN = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitProjectionEFGCNTotal13EFGCN, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitProjectionEFGCNTotal32EFGCN) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitProjectionEFGCNTotal32EFGCN)) {
			config.analysisInitProjectionEFGCNTotal32EFGCN = bstob((char*)value.c_str());
			if (!config.analysisInitProjectionEFGCNTotal32EFGCN)
				config.analysisInitProjectionEFGCNTotal32EFGCN = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitProjectionEFGCNTotal32EFGCN, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitProjectionEvent) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitProjectionEvent)) {
			config.analysisInitProjectionEvent = bstob((char*)value.c_str());
			if (!config.analysisInitProjectionEvent)
				config.analysisInitProjectionEvent = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitProjectionEvent, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitProjectionTotal) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitProjectionTotal)) {
			config.analysisInitProjectionTotal = bstob((char*)value.c_str());
			if (!config.analysisInitProjectionTotal)
				config.analysisInitProjectionTotal = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitProjectionTotal, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitProjectionDisplay) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitProjectionDisplay)) {
			config.analysisInitProjectionDisplay = bstob((char*)value.c_str());
			if (!config.analysisInitProjectionDisplay)
				config.analysisInitProjectionDisplay = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitProjectionDisplay, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitProjectionToRoot) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitProjectionToRoot)) {
			config.analysisInitProjectionToRoot = bstob((char*)value.c_str());
			if (!config.analysisInitProjectionToRoot)
				config.analysisInitProjectionToRoot = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitProjectionToRoot, true);
			specifierFound = true;
		}
	}

	return specifierFound;

}

/****************************************************************
 * This method assigns the value to the corresponding parameter	*
 * which is identified by a specifier.							*
 ****************************************************************/

bool inf::getMagnetfieldHeaderValue(std::string& specifier, std::string& value) {

	bool specifierFound = false;
	
	if (specifier.compare(stringCmdInputFileNameMagneticField) == 0) {
		if (!isHeaderLockSet(idCmdInputFileNameMagneticField)) {
			config.inputFileNameMagneticField = value;
			setHeaderLock(idCmdInputFileNameMagneticField, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputMagneticFieldIsRootFile) == 0) {
		if (!isHeaderLockSet(idCmdInputMagneticFieldIsRootFile)) {
			config.inputMagneticFieldIsRootFile = bstob((char*)value.c_str());
			if (!config.inputMagneticFieldIsRootFile)
				config.inputMagneticFieldIsRootFile = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdInputMagneticFieldIsRootFile, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputMapNameMagneticField) == 0) {
		if (!isHeaderLockSet(idCmdInputMapNameMagneticField)) {
			config.inputMapNameMagneticField = value;
			setHeaderLock(idCmdInputMapNameMagneticField, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputMagneticFieldIntegrationStepwidthPerStation) == 0) {
		if (!isHeaderLockSet(idCmdInputMagneticFieldIntegrationStepwidthPerStation)) {
			config.inputMagneticFieldIntegrationStepwidthPerStation = stous((char*)value.c_str(), 10);
			setHeaderLock(idCmdInputMagneticFieldIntegrationStepwidthPerStation, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputMagneticFieldIntegrationFactor) == 0) {
		if (!isHeaderLockSet(idCmdInputMagneticFieldIntegrationFactor)) {
			config.inputMagneticFieldIntegrationFactor = stod((char*)value.c_str());
			setHeaderLock(idCmdInputMagneticFieldIntegrationFactor, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdInputDisableAutomaticMagneticField) == 0) {
		if (!isHeaderLockSet(idCmdInputDisableAutomaticMagneticField)) {
			config.inputDisableAutomaticMagneticField = bstob((char*)value.c_str());
			if (!config.inputDisableAutomaticMagneticField)
				config.inputDisableAutomaticMagneticField = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdInputDisableAutomaticMagneticField, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitMagnetfieldX) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitMagnetfieldX)) {
			config.analysisInitMagnetfieldX = bstob((char*)value.c_str());
			if (!config.analysisInitMagnetfieldX)
				config.analysisInitMagnetfieldX = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitMagnetfieldX, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitMagnetfieldY) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitMagnetfieldY)) {
			config.analysisInitMagnetfieldY = bstob((char*)value.c_str());
			if (!config.analysisInitMagnetfieldY)
				config.analysisInitMagnetfieldY = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitMagnetfieldY, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitMagnetfieldZ) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitMagnetfieldZ)) {
			config.analysisInitMagnetfieldZ = bstob((char*)value.c_str());
			if (!config.analysisInitMagnetfieldZ)
				config.analysisInitMagnetfieldZ = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitMagnetfieldZ, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitMagnetfieldDisplay) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitMagnetfieldDisplay)) {
			config.analysisInitMagnetfieldDisplay = bstob((char*)value.c_str());
			if (!config.analysisInitMagnetfieldDisplay)
				config.analysisInitMagnetfieldDisplay = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitMagnetfieldDisplay, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitMagnetfieldToRoot) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitMagnetfieldToRoot)) {
			config.analysisInitMagnetfieldToRoot = bstob((char*)value.c_str());
			if (!config.analysisInitMagnetfieldToRoot)
				config.analysisInitMagnetfieldToRoot = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitMagnetfieldToRoot, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitMagnetfieldConstantForEachEvent) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitMagnetfieldConstantForEachEvent)) {
			config.analysisInitMagnetfieldConstantForEachEvent = bstob((char*)value.c_str());
			if (!config.analysisInitMagnetfieldConstantForEachEvent)
				config.analysisInitMagnetfieldConstantForEachEvent = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitMagnetfieldConstantForEachEvent, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitWeightedMagnetfieldConstant) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitWeightedMagnetfieldConstant)) {
			config.analysisInitWeightedMagnetfieldConstant = bstob((char*)value.c_str());
			if (!config.analysisInitWeightedMagnetfieldConstant)
				config.analysisInitWeightedMagnetfieldConstant = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitWeightedMagnetfieldConstant, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitMagnetfieldConstantDisplay) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitMagnetfieldConstantDisplay)) {
			config.analysisInitMagnetfieldConstantDisplay = bstob((char*)value.c_str());
			if (!config.analysisInitMagnetfieldConstantDisplay)
				config.analysisInitMagnetfieldConstantDisplay = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitMagnetfieldConstantDisplay, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisMagnetfieldConstantDisplayMask) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisMagnetfieldConstantDisplayMask)) {
			config.analysisMagnetfieldConstantDisplayMask = bitArray(value);
			setHeaderLock(idCmdAnalysisMagnetfieldConstantDisplayMask, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitMagnetfieldConstantToRoot) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitMagnetfieldConstantToRoot)) {
			config.analysisInitMagnetfieldConstantToRoot = bstob((char*)value.c_str());
			if (!config.analysisInitMagnetfieldConstantToRoot)
				config.analysisInitMagnetfieldConstantToRoot = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitMagnetfieldConstantToRoot, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitMagnetfieldVSConstants) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitMagnetfieldVSConstants)) {
			config.analysisInitMagnetfieldVSConstants = bstob((char*)value.c_str());
			if (!config.analysisInitMagnetfieldVSConstants)
				config.analysisInitMagnetfieldVSConstants = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitMagnetfieldVSConstants, true);
			specifierFound = true;
		}
	}

	return specifierFound;

}

/****************************************************************
 * This method assigns the value to the corresponding parameter	*
 * which is identified by a specifier.							*
 ****************************************************************/

bool inf::getGoodnessHeaderValue(std::string& specifier, std::string& value) {

	bool specifierFound = false;
	
	if (specifier.compare(stringCmdAnalysisInitPrelutGoodness) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitPrelutGoodness)) {
			config.analysisInitPrelutGoodness = bstob((char*)value.c_str());
			if (!config.analysisInitPrelutGoodness)
				config.analysisInitPrelutGoodness = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitPrelutGoodness, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitLutGoodness) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitLutGoodness)) {
			config.analysisInitLutGoodness = bstob((char*)value.c_str());
			if (!config.analysisInitLutGoodness)
				config.analysisInitLutGoodness = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitLutGoodness, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitHoughTransformGoodness) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitHoughTransformGoodness)) {
			config.analysisInitHoughTransformGoodness = bstob((char*)value.c_str());
			if (!config.analysisInitHoughTransformGoodness)
				config.analysisInitHoughTransformGoodness = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitHoughTransformGoodness, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitQuantizationGoodness) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitQuantizationGoodness)) {
			config.analysisInitQuantizationGoodness = bstob((char*)value.c_str());
			if (!config.analysisInitQuantizationGoodness)
				config.analysisInitQuantizationGoodness = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitQuantizationGoodness, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitPeakDistanceGoodness) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitPeakDistanceGoodness)) {
			config.analysisInitPeakDistanceGoodness = bstob((char*)value.c_str());
			if (!config.analysisInitPeakDistanceGoodness)
				config.analysisInitPeakDistanceGoodness = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitPeakDistanceGoodness, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitPercentageOfHitsInSignature) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitPercentageOfHitsInSignature)) {
			config.analysisInitPercentageOfHitsInSignature = stoul((char*)value.c_str(), 10);
			if (config.analysisInitPercentageOfHitsInSignature > 100)
				config.analysisInitPercentageOfHitsInSignature = 100;
			setHeaderLock(idCmdAnalysisInitPercentageOfHitsInSignature, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitPercentageOfTracksForSignature) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitPercentageOfTracksForSignature)) {
			config.analysisInitPercentageOfTracksForSignature = stoul((char*)value.c_str(), 10);
			if (config.analysisInitPercentageOfTracksForSignature > 100)
				config.analysisInitPercentageOfTracksForSignature = 100;
			setHeaderLock(idCmdAnalysisInitPercentageOfTracksForSignature, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitAnalysisResultWarnings) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitAnalysisResultWarnings)) {
			config.analysisInitAnalysisResultWarnings = stoul((char*)value.c_str(), 10);
			setHeaderLock(idCmdAnalysisInitAnalysisResultWarnings, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitAnalysisResultDisplays) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitAnalysisResultDisplays)) {
			config.analysisInitAnalysisResultDisplays = stoul((char*)value.c_str(), 10);
			setHeaderLock(idCmdAnalysisInitAnalysisResultDisplays, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitAnalysisMoreResultWarnings) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitAnalysisMoreResultWarnings)) {
			config.analysisInitAnalysisMoreResultWarnings = stoul((char*)value.c_str(), 10);
			setHeaderLock(idCmdAnalysisInitAnalysisMoreResultWarnings, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitAnalysisMoreResultDisplays) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitAnalysisMoreResultDisplays)) {
			config.analysisInitAnalysisMoreResultDisplays = stoul((char*)value.c_str(), 10);
			setHeaderLock(idCmdAnalysisInitAnalysisMoreResultDisplays, true);
			specifierFound = true;
		}
	}

	return specifierFound;

}

/****************************************************************
 * This method assigns the value to the corresponding parameter	*
 * which is identified by a specifier.							*
 ****************************************************************/

bool inf::getHistogramHeaderValue(std::string& specifier, std::string& value) {

	bool specifierFound = false;
	
	if (specifier.compare(stringCmdAnalysisInitCreatedHistogramToRoot) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitCreatedHistogramToRoot)) {
			config.analysisInitCreatedHistogramToRoot = bstob((char*)value.c_str());
			if (!config.analysisInitCreatedHistogramToRoot)
				config.analysisInitCreatedHistogramToRoot = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitCreatedHistogramToRoot, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitEncodedHistogramToRoot) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitEncodedHistogramToRoot)) {
			config.analysisInitEncodedHistogramToRoot = bstob((char*)value.c_str());
			if (!config.analysisInitEncodedHistogramToRoot)
				config.analysisInitEncodedHistogramToRoot = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitEncodedHistogramToRoot, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitFilteredHistogramToRoot) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitFilteredHistogramToRoot)) {
			config.analysisInitFilteredHistogramToRoot = bstob((char*)value.c_str());
			if (!config.analysisInitFilteredHistogramToRoot)
				config.analysisInitFilteredHistogramToRoot = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitFilteredHistogramToRoot, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitJustOneCreatedHistogramToRoot) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitJustOneCreatedHistogramToRoot)) {
			config.analysisInitJustOneCreatedHistogramToRoot = bstob((char*)value.c_str());
			if (!config.analysisInitJustOneCreatedHistogramToRoot)
				config.analysisInitJustOneCreatedHistogramToRoot = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitJustOneCreatedHistogramToRoot, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitJustOneEncodedHistogramToRoot) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitJustOneEncodedHistogramToRoot)) {
			config.analysisInitJustOneEncodedHistogramToRoot = bstob((char*)value.c_str());
			if (!config.analysisInitJustOneEncodedHistogramToRoot)
				config.analysisInitJustOneEncodedHistogramToRoot = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitJustOneEncodedHistogramToRoot, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitJustOneFilteredHistogramToRoot) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitJustOneFilteredHistogramToRoot)) {
			config.analysisInitJustOneFilteredHistogramToRoot = bstob((char*)value.c_str());
			if (!config.analysisInitJustOneFilteredHistogramToRoot)
				config.analysisInitJustOneFilteredHistogramToRoot = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitJustOneFilteredHistogramToRoot, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitCreatedHistogramToShow) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitCreatedHistogramToShow)) {
			config.analysisInitCreatedHistogramToShow = bstob((char*)value.c_str());
			if (!config.analysisInitCreatedHistogramToShow)
				config.analysisInitCreatedHistogramToShow = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitCreatedHistogramToShow, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitEncodedHistogramToShow) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitEncodedHistogramToShow)) {
			config.analysisInitEncodedHistogramToShow = bstob((char*)value.c_str());
			if (!config.analysisInitEncodedHistogramToShow)
				config.analysisInitEncodedHistogramToShow = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitEncodedHistogramToShow, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitFilteredHistogramToShow) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitFilteredHistogramToShow)) {
			config.analysisInitFilteredHistogramToShow = bstob((char*)value.c_str());
			if (!config.analysisInitFilteredHistogramToShow)
				config.analysisInitFilteredHistogramToShow = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitFilteredHistogramToShow, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitHistogramLayer) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitHistogramLayer)) {
			config.analysisInitHistogramLayer = stous((char*)value.c_str(), 10);
			setHeaderLock(idCmdAnalysisInitHistogramLayer, true);
			specifierFound = true;
		}
	}

	return specifierFound;

}

/****************************************************************
 * This method assigns the value to the corresponding parameter	*
 * which is identified by a specifier.							*
 ****************************************************************/

bool inf::getHardwareHeaderValue(std::string& specifier, std::string& value) {

	bool specifierFound = false;
	
	if (specifier.compare(stringCmdAnalysisInitNumberOfTracksPerColumn) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitNumberOfTracksPerColumn)) {
			config.analysisInitNumberOfTracksPerColumn = bstob((char*)value.c_str());
			if (!config.analysisInitNumberOfTracksPerColumn)
				config.analysisInitNumberOfTracksPerColumn = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitNumberOfTracksPerColumn, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitNumberOfTracksPerRow) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitNumberOfTracksPerRow)) {
			config.analysisInitNumberOfTracksPerRow = bstob((char*)value.c_str());
			if (!config.analysisInitNumberOfTracksPerRow)
				config.analysisInitNumberOfTracksPerRow = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitNumberOfTracksPerRow, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitNumberOfTracksPerLayer) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitNumberOfTracksPerLayer)) {
			config.analysisInitNumberOfTracksPerLayer = bstob((char*)value.c_str());
			if (!config.analysisInitNumberOfTracksPerLayer)
				config.analysisInitNumberOfTracksPerLayer = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitNumberOfTracksPerLayer, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitHitReadoutDistribution) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitHitReadoutDistribution)) {
			config.analysisInitHitReadoutDistribution = bstob((char*)value.c_str());
			if (!config.analysisInitHitReadoutDistribution)
				config.analysisInitHitReadoutDistribution = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitHitReadoutDistribution, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitReadoutColumnsInParallel) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitReadoutColumnsInParallel)) {
			config.analysisInitReadoutColumnsInParallel = bstob((char*)value.c_str());
			if (!config.analysisInitReadoutColumnsInParallel)
				config.analysisInitReadoutColumnsInParallel = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitReadoutColumnsInParallel, true);
			specifierFound = true;
		}
	}

	return specifierFound;

}

/****************************************************************
 * This method assigns the value to the corresponding parameter	*
 * which is identified by a specifier.							*
 ****************************************************************/

bool inf::getPrelutRangeHeaderValue(std::string& specifier, std::string& value) {

	bool specifierFound = false;
	
	if (specifier.compare(stringCmdAnalysisInitPrelutRangeForEachEvent) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitPrelutRangeForEachEvent)) {
			config.analysisInitPrelutRangeForEachEvent = bstob((char*)value.c_str());
			if (!config.analysisInitPrelutRangeForEachEvent)
				config.analysisInitPrelutRangeForEachEvent = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitPrelutRangeForEachEvent, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitWeightedPrelutRange) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitWeightedPrelutRange)) {
			config.analysisInitWeightedPrelutRange = bstob((char*)value.c_str());
			if (!config.analysisInitWeightedPrelutRange)
				config.analysisInitWeightedPrelutRange = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitWeightedPrelutRange, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitPrelutRangeDisplay) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitPrelutRangeDisplay)) {
			config.analysisInitPrelutRangeDisplay = bstob((char*)value.c_str());
			if (!config.analysisInitPrelutRangeDisplay)
				config.analysisInitPrelutRangeDisplay = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitPrelutRangeDisplay, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitPrelutRangeDisplayMode) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitPrelutRangeDisplayMode)) {
			config.analysisInitPrelutRangeDisplayMode = stous((char*)value.c_str(), 10);
			setHeaderLock(idCmdAnalysisInitPrelutRangeDisplayMode, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisPrelutRangeStationDisplayMask) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisPrelutRangeStationDisplayMask)) {
			config.analysisPrelutRangeStationDisplayMask = bitArray(value);
			setHeaderLock(idCmdAnalysisPrelutRangeStationDisplayMask, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisPrelutRangeStationSumDisplayMask) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisPrelutRangeStationSumDisplayMask)) {
			config.analysisPrelutRangeStationSumDisplayMask = bstob((char*)value.c_str());
			if (!config.analysisPrelutRangeStationSumDisplayMask)
				config.analysisPrelutRangeStationSumDisplayMask = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisPrelutRangeStationSumDisplayMask, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisPrelutRangeConstraintDisplayMask) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisPrelutRangeConstraintDisplayMask)) {
			config.analysisPrelutRangeConstraintDisplayMask = bitArray(value);
			setHeaderLock(idCmdAnalysisPrelutRangeConstraintDisplayMask, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisPrelutRangeConstraintSumDisplayMask) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisPrelutRangeConstraintSumDisplayMask)) {
			config.analysisPrelutRangeConstraintSumDisplayMask = bstob((char*)value.c_str());
			if (!config.analysisPrelutRangeConstraintSumDisplayMask)
				config.analysisPrelutRangeConstraintSumDisplayMask = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisPrelutRangeConstraintSumDisplayMask, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisPrelutRangeRelativeDisplayMask) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisPrelutRangeRelativeDisplayMask)) {
			config.analysisPrelutRangeRelativeDisplayMask = bstob((char*)value.c_str());
			if (!config.analysisPrelutRangeRelativeDisplayMask)
				config.analysisPrelutRangeRelativeDisplayMask = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisPrelutRangeRelativeDisplayMask, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitPrelutRangeToRoot) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitPrelutRangeToRoot)) {
			config.analysisInitPrelutRangeToRoot = bstob((char*)value.c_str());
			if (!config.analysisInitPrelutRangeToRoot)
				config.analysisInitPrelutRangeToRoot = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitPrelutRangeToRoot, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisInitPercentageOfHitsForPrelutRange) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitPercentageOfHitsForPrelutRange)) {
			config.analysisInitPercentageOfHitsForPrelutRange = stous((char*)value.c_str(), 10);
			if (config.analysisInitPercentageOfHitsForPrelutRange > 100)
				config.analysisInitPercentageOfHitsForPrelutRange = 100;
			setHeaderLock(idCmdAnalysisInitPercentageOfHitsForPrelutRange, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisPrelutRangeMinStart) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisPrelutRangeMinStart)) {
			config.analysisPrelutRangeMinStart = stod((char*)value.c_str());
			setHeaderLock(idCmdAnalysisPrelutRangeMinStart, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisPrelutRangeMinStop) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisPrelutRangeMinStop)) {
			config.analysisPrelutRangeMinStop = stod((char*)value.c_str());
			setHeaderLock(idCmdAnalysisPrelutRangeMinStop, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisPrelutRangeMinSteps) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisPrelutRangeMinSteps)) {
			config.analysisPrelutRangeMinSteps = stous((char*)value.c_str(), 10);
			setHeaderLock(idCmdAnalysisPrelutRangeMinSteps, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisPrelutRangeMaxStart) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisPrelutRangeMaxStart)) {
			config.analysisPrelutRangeMaxStart = stod((char*)value.c_str());
			setHeaderLock(idCmdAnalysisPrelutRangeMaxStart, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisPrelutRangeMaxStop) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisPrelutRangeMaxStop)) {
			config.analysisPrelutRangeMaxStop = stod((char*)value.c_str());
			setHeaderLock(idCmdAnalysisPrelutRangeMaxStop, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisPrelutRangeMaxSteps) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisPrelutRangeMaxSteps)) {
			config.analysisPrelutRangeMaxSteps = stous((char*)value.c_str(), 10);
			setHeaderLock(idCmdAnalysisPrelutRangeMaxSteps, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisChooseMainPrelutRange) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisChooseMainPrelutRange)) {
			config.analysisChooseMainPrelutRange = bstob((char*)value.c_str());
			if (!config.analysisChooseMainPrelutRange)
				config.analysisChooseMainPrelutRange = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisChooseMainPrelutRange, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisChooseConstraintPrelutRange) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisChooseConstraintPrelutRange)) {
			config.analysisChooseConstraintPrelutRange = bstob((char*)value.c_str());
			if (!config.analysisChooseConstraintPrelutRange)
				config.analysisChooseConstraintPrelutRange = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisChooseConstraintPrelutRange, true);
			specifierFound = true;
		}
	}

	return specifierFound;

}

/****************************************************************
 * This method assigns the value to the corresponding parameter	*
 * which is identified by a specifier.							*
 ****************************************************************/

bool inf::getTotalAnalysisHeaderValue(std::string& specifier, std::string& value) {

	bool specifierFound = false;
	
	if (specifier.compare(stringCmdAnalysisInitTotalAnalysis) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisInitTotalAnalysis)) {
			config.analysisInitTotalAnalysis = bstob((char*)value.c_str());
			if (!config.analysisInitTotalAnalysis)
				config.analysisInitTotalAnalysis = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisInitTotalAnalysis, true);
			specifierFound = true;
		}
	}

	return specifierFound;

}

/****************************************************************
 * This method assigns the value to the corresponding parameter	*
 * which is identified by a specifier.							*
 ****************************************************************/

bool inf::getCellAnalysisHeaderValue(std::string& specifier, std::string& value) {

	bool specifierFound = false;
	
	if (specifier.compare(stringCmdAnalysisWriteCellFiles) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisWriteCellFiles)) {
			config.analysisWriteCellFiles = bstob((char*)value.c_str());
			if (!config.analysisWriteCellFiles)
				config.analysisWriteCellFiles = dstob(*((char*)value.c_str()));
			setHeaderLock(idCmdAnalysisWriteCellFiles, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisHitCellFileName) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisHitCellFileName)) {
			config.analysisHitCellFileName = value;
			setHeaderLock(idCmdAnalysisHitCellFileName, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisPrelutCellFileName) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisPrelutCellFileName)) {
			config.analysisPrelutCellFileName = value;
			setHeaderLock(idCmdAnalysisPrelutCellFileName, true);
			specifierFound = true;
		}
	}
	else if (specifier.compare(stringCmdAnalysisLutCellFileName) == 0) {
		if (!isHeaderLockSet(idCmdAnalysisLutCellFileName)) {
			config.analysisLutCellFileName = value;
			setHeaderLock(idCmdAnalysisLutCellFileName, true);
			specifierFound = true;
		}
	}

	return specifierFound;

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

inf::inf() 
  : configuration(),
    config()
{

	resetHeader();

	init(getDefFileName());

}

/****************************************************************
 * Default constructor											*
 ****************************************************************/

inf::inf(InfData& setup) 
  : configuration(),
    config()
{

	resetHeader();

/********************************************************/
/* make code changes for a different configuration here */
	config.inputFileName                                      = setup.inputFileName;
	config.inputFileFormat                                    = setup.inputFileFormat;
	config.inputMinTracks                                     = setup.inputMinTracks;
	config.inputMaxTracks                                     = setup.inputMaxTracks;
	config.inputDetectorMask                                  = setup.inputDetectorMask;
	config.inputFileNameMagneticField                         = setup.inputFileNameMagneticField;
	config.inputMagneticFieldIsRootFile                       = setup.inputMagneticFieldIsRootFile;
	config.inputMapNameMagneticField                          = setup.inputMapNameMagneticField;
	config.inputMagneticFieldIntegrationStepwidthPerStation   = setup.inputMagneticFieldIntegrationStepwidthPerStation;
	config.inputMagneticFieldIntegrationFactor                = setup.inputMagneticFieldIntegrationFactor;
	config.inputDisableAutomaticMagneticField                 = setup.inputDisableAutomaticMagneticField;
	config.inputFileNameDetector                              = setup.inputFileNameDetector;
	config.inputNumberOfVolumesInfrontOfSTS                   = setup.inputNumberOfVolumesInfrontOfSTS;
	config.inputDisableAutomaticDetector                      = setup.inputDisableAutomaticDetector;
	config.inputCodingTableMode                               = setup.inputCodingTableMode;
	config.inputGradingPTableMode                             = setup.inputGradingPTableMode;
	config.inputGradingRTableMode                             = setup.inputGradingRTableMode;
	config.inputCodingTableFileName                           = setup.inputCodingTableFileName;
	config.inputGradingPTableFileName                         = setup.inputGradingPTableFileName;
	config.inputGradingRTableFileName                         = setup.inputGradingRTableFileName;
	config.inputCodingTableWrite                              = setup.inputCodingTableWrite;
	config.inputGradingPTableWrite                            = setup.inputGradingPTableWrite;
	config.inputGradingRTableWrite                            = setup.inputGradingRTableWrite;
	config.trackfinderHitProducer                             = setup.trackfinderHitProducer;
	config.trackfinderReadPointsFromFile                      = setup.trackfinderReadPointsFromFile;
	config.trackfinderReadHitsFromFile                        = setup.trackfinderReadHitsFromFile;
	config.trackfinderReadMapsHits                            = setup.trackfinderReadMapsHits;
	config.trackfinderReadHybridHits                          = setup.trackfinderReadHybridHits;
	config.trackfinderReadStripHits                           = setup.trackfinderReadStripHits;
	config.trackfinderLutsVersion                             = setup.trackfinderLutsVersion;
	config.trackfinderPrelutFileName                          = setup.trackfinderPrelutFileName;
	config.trackfinderLutFileName                             = setup.trackfinderLutFileName;
	config.trackfinderGammaMin                                = setup.trackfinderGammaMin;
	config.trackfinderGammaMax                                = setup.trackfinderGammaMax;
	config.trackfinderGammaStep                               = setup.trackfinderGammaStep;
	config.trackfinderThetaMin                                = setup.trackfinderThetaMin;
	config.trackfinderThetaMax                                = setup.trackfinderThetaMax;
	config.trackfinderThetaStep                               = setup.trackfinderThetaStep;
	config.trackfinderPrelutRadiusMin                         = setup.trackfinderPrelutRadiusMin;
	config.trackfinderPrelutRadiusMax                         = setup.trackfinderPrelutRadiusMax;
	config.trackfinderLutRadiusMin                            = setup.trackfinderLutRadiusMin;
	config.trackfinderLutRadiusMax                            = setup.trackfinderLutRadiusMax;
	config.trackfinderLutRadiusStep                           = setup.trackfinderLutRadiusStep;
	config.trackfinderMinClassCoding                          = setup.trackfinderMinClassCoding;
	config.trackfinderMinClassGradingP                        = setup.trackfinderMinClassGradingP;
	config.trackfinderMinClassGradingR                        = setup.trackfinderMinClassGradingR;
	config.trackfinderFilterType                              = setup.trackfinderFilterType;
	config.trackfinderFirstFilterGeometry                     = setup.trackfinderFirstFilterGeometry;
	config.trackfinderFirstFilterArithmetic                   = setup.trackfinderFirstFilterArithmetic;
	config.trackfinderSecondFilterGeometry                    = setup.trackfinderSecondFilterGeometry;
	config.trackfinderSecondFilterArithmetic                  = setup.trackfinderSecondFilterArithmetic;
	config.trackfinderFirstFilterNeighborhoodDim1ClearRadius  = setup.trackfinderFirstFilterNeighborhoodDim1ClearRadius;
	config.trackfinderFirstFilterNeighborhoodDim2ClearRadius  = setup.trackfinderFirstFilterNeighborhoodDim2ClearRadius;
	config.trackfinderSecondFilterNeighborhoodDim1ClearRadius = setup.trackfinderSecondFilterNeighborhoodDim1ClearRadius;
	config.trackfinderSecondFilterNeighborhoodDim2ClearRadius = setup.trackfinderSecondFilterNeighborhoodDim2ClearRadius;
	config.trackfinderSecondFilterNeighborhoodDim3ClearRadius = setup.trackfinderSecondFilterNeighborhoodDim3ClearRadius;
	config.trackfinderAutomaticFilterCoverPercentage          = setup.trackfinderAutomaticFilterCoverPercentage;
	if (config.trackfinderAutomaticFilterCoverPercentage > 100)
		config.trackfinderAutomaticFilterCoverPercentage = 100;
	config.trackfinderAutomaticFilterDataPercentage           = setup.trackfinderAutomaticFilterDataPercentage;
	if (config.trackfinderAutomaticFilterDataPercentage > 100)
		config.trackfinderAutomaticFilterDataPercentage = 100;
	config.trackfinderAutomaticFilterWrite                    = setup.trackfinderAutomaticFilterWrite;
	config.trackfinderAutomaticFilterFileName                 = setup.trackfinderAutomaticFilterFileName;
	config.trackfinderWriteTracksToFile                       = setup.trackfinderWriteTracksToFile;
	config.analysisOutputFileName                             = setup.analysisOutputFileName;
	config.analysisThresholdForP                              = setup.analysisThresholdForP;
	config.analysisInitConfiguration                          = setup.analysisInitConfiguration;
	config.analysisInitDetector                               = setup.analysisInitDetector;
	config.analysisInitEvent                                  = setup.analysisInitEvent;
	config.analysisInitClassPriority                          = setup.analysisInitClassPriority;
	config.analysisInitMemory                                 = setup.analysisInitMemory;
	config.analysisInitTime                                   = setup.analysisInitTime;
	config.analysisInitTrackPropagationEventPoint             = setup.analysisInitTrackPropagationEventPoint;
	config.analysisInitTrackPropagationEventHit               = setup.analysisInitTrackPropagationEventHit;
	config.analysisInitTrackPropagationTotalPoint             = setup.analysisInitTrackPropagationTotalPoint;
	config.analysisInitTrackPropagationTotalHit               = setup.analysisInitTrackPropagationTotalHit;
	config.analysisInitTrackPropagationDisplay                = setup.analysisInitTrackPropagationDisplay;
	config.analysisInitTrackPropagationToRoot                 = setup.analysisInitTrackPropagationToRoot;
	config.analysisTrackPropagationPointDisplayMask           = setup.analysisTrackPropagationPointDisplayMask;
	config.analysisTrackPropagationHitDisplayMask             = setup.analysisTrackPropagationHitDisplayMask;
	config.analysisInitQualityEFGCEventAbsolute               = setup.analysisInitQualityEFGCEventAbsolute;
	config.analysisInitQualityEFGCEventRelative               = setup.analysisInitQualityEFGCEventRelative;
	config.analysisInitQualityEFGCTotalAbsolute               = setup.analysisInitQualityEFGCTotalAbsolute;
	config.analysisInitQualityEFGCTotalRelative               = setup.analysisInitQualityEFGCTotalRelative;
	config.analysisInitMomentumEFGCEventPzEFGC                = setup.analysisInitMomentumEFGCEventPzEFGC;
	config.analysisInitMomentumEFGCEventPtEFGC                = setup.analysisInitMomentumEFGCEventPtEFGC;
	config.analysisInitMomentumEFGCTotalPzEFGC                = setup.analysisInitMomentumEFGCTotalPzEFGC;
	config.analysisInitMomentumEFGCTotalPtEFGC                = setup.analysisInitMomentumEFGCTotalPtEFGC;
	config.analysisInitProjectionEFGCNEvent12EFGCN            = setup.analysisInitProjectionEFGCNEvent12EFGCN;
	config.analysisInitProjectionEFGCNEvent13EFGCN            = setup.analysisInitProjectionEFGCNEvent13EFGCN;
	config.analysisInitProjectionEFGCNEvent32EFGCN            = setup.analysisInitProjectionEFGCNEvent32EFGCN;
	config.analysisInitProjectionEFGCNTotal12EFGCN            = setup.analysisInitProjectionEFGCNTotal12EFGCN;
	config.analysisInitProjectionEFGCNTotal13EFGCN            = setup.analysisInitProjectionEFGCNTotal13EFGCN;
	config.analysisInitProjectionEFGCNTotal32EFGCN            = setup.analysisInitProjectionEFGCNTotal32EFGCN;
	config.analysisInitMomentumEvent                          = setup.analysisInitMomentumEvent;
	config.analysisInitMomentumTotal                          = setup.analysisInitMomentumTotal;
	config.analysisInitMomentumDisplay                        = setup.analysisInitMomentumDisplay;
	config.analysisInitMomentumToRoot                         = setup.analysisInitMomentumToRoot;
	config.analysisInitProjectionEvent                        = setup.analysisInitProjectionEvent;
	config.analysisInitProjectionTotal                        = setup.analysisInitProjectionTotal;
	config.analysisInitProjectionDisplay                      = setup.analysisInitProjectionDisplay;
	config.analysisInitProjectionToRoot                       = setup.analysisInitProjectionToRoot;
	config.analysisInitMagnetfieldX                           = setup.analysisInitMagnetfieldX;
	config.analysisInitMagnetfieldY                           = setup.analysisInitMagnetfieldY;
	config.analysisInitMagnetfieldZ                           = setup.analysisInitMagnetfieldZ;
	config.analysisInitMagnetfieldDisplay                     = setup.analysisInitMagnetfieldDisplay;
	config.analysisInitMagnetfieldToRoot                      = setup.analysisInitMagnetfieldToRoot;
	config.analysisInitMagnetfieldConstantForEachEvent        = setup.analysisInitMagnetfieldConstantForEachEvent;
	config.analysisInitWeightedMagnetfieldConstant            = setup.analysisInitWeightedMagnetfieldConstant;
	config.analysisInitMagnetfieldConstantDisplay             = setup.analysisInitMagnetfieldConstantDisplay;
	config.analysisMagnetfieldConstantDisplayMask             = setup.analysisMagnetfieldConstantDisplayMask;
	config.analysisInitMagnetfieldConstantToRoot              = setup.analysisInitMagnetfieldConstantToRoot;
	config.analysisInitMagnetfieldVSConstants                 = setup.analysisInitMagnetfieldVSConstants;
	config.analysisInitPrelutGoodness                         = setup.analysisInitPrelutGoodness;
	config.analysisInitLutGoodness                            = setup.analysisInitLutGoodness;
	config.analysisInitHoughTransformGoodness                 = setup.analysisInitHoughTransformGoodness;
	config.analysisInitQuantizationGoodness                   = setup.analysisInitQuantizationGoodness;
	config.analysisInitPeakDistanceGoodness                   = setup.analysisInitPeakDistanceGoodness;
	config.analysisInitCreatedHistogramToRoot                 = setup.analysisInitCreatedHistogramToRoot;
	config.analysisInitEncodedHistogramToRoot                 = setup.analysisInitEncodedHistogramToRoot;
	config.analysisInitFilteredHistogramToRoot                = setup.analysisInitFilteredHistogramToRoot;
	config.analysisInitJustOneCreatedHistogramToRoot          = setup.analysisInitJustOneCreatedHistogramToRoot;
	config.analysisInitJustOneEncodedHistogramToRoot          = setup.analysisInitJustOneEncodedHistogramToRoot;
	config.analysisInitJustOneFilteredHistogramToRoot         = setup.analysisInitJustOneFilteredHistogramToRoot;
	config.analysisInitCreatedHistogramToShow                 = setup.analysisInitCreatedHistogramToShow;
	config.analysisInitEncodedHistogramToShow                 = setup.analysisInitEncodedHistogramToShow;
	config.analysisInitFilteredHistogramToShow                = setup.analysisInitFilteredHistogramToShow;
	config.analysisInitHistogramLayer                         = setup.analysisInitHistogramLayer;
	config.analysisInitNumberOfTracksPerColumn                = setup.analysisInitNumberOfTracksPerColumn;
	config.analysisInitNumberOfTracksPerRow                   = setup.analysisInitNumberOfTracksPerRow;
	config.analysisInitNumberOfTracksPerLayer                 = setup.analysisInitNumberOfTracksPerLayer;
	config.analysisInitHitReadoutDistribution                 = setup.analysisInitHitReadoutDistribution;
	config.analysisInitReadoutColumnsInParallel               = setup.analysisInitReadoutColumnsInParallel;
	config.analysisInitPrelutRangeForEachEvent                = setup.analysisInitPrelutRangeForEachEvent;
	config.analysisInitWeightedPrelutRange                    = setup.analysisInitWeightedPrelutRange;
	config.analysisInitPrelutRangeDisplay                     = setup.analysisInitPrelutRangeDisplay;
	config.analysisPrelutRangeStationDisplayMask              = setup.analysisPrelutRangeStationDisplayMask;
	config.analysisPrelutRangeStationSumDisplayMask           = setup.analysisPrelutRangeStationSumDisplayMask;
	config.analysisPrelutRangeConstraintDisplayMask           = setup.analysisPrelutRangeConstraintDisplayMask;
	config.analysisPrelutRangeConstraintSumDisplayMask        = setup.analysisPrelutRangeConstraintSumDisplayMask;
	config.analysisInitPrelutRangeDisplayMode                 = setup.analysisInitPrelutRangeDisplayMode;
	config.analysisPrelutRangeRelativeDisplayMask             = setup.analysisPrelutRangeRelativeDisplayMask;
	config.analysisInitPrelutRangeToRoot                      = setup.analysisInitPrelutRangeToRoot;
	config.analysisInitPercentageOfHitsForPrelutRange         = setup.analysisInitPercentageOfHitsForPrelutRange;
	if (config.analysisInitPercentageOfHitsForPrelutRange > 100)
		config.analysisInitPercentageOfHitsForPrelutRange = 100;
	config.analysisPrelutRangeMinStart                        = setup.analysisPrelutRangeMinStart;
	config.analysisPrelutRangeMinStop                         = setup.analysisPrelutRangeMinStop;
	config.analysisPrelutRangeMinSteps                        = setup.analysisPrelutRangeMinSteps;
	config.analysisPrelutRangeMaxStart                        = setup.analysisPrelutRangeMaxStart;
	config.analysisPrelutRangeMaxStop                         = setup.analysisPrelutRangeMaxStop;
	config.analysisPrelutRangeMaxSteps                        = setup.analysisPrelutRangeMaxSteps;
	config.analysisChooseMainPrelutRange                      = setup.analysisChooseMainPrelutRange;
	config.analysisChooseConstraintPrelutRange                = setup.analysisChooseConstraintPrelutRange;
	config.analysisInitTotalAnalysis                          = setup.analysisInitTotalAnalysis;
	config.analysisInitPercentageOfHitsInSignature            = setup.analysisInitPercentageOfHitsInSignature;
	if (config.analysisInitPercentageOfHitsInSignature > 100)
		config.analysisInitPercentageOfHitsInSignature = 100;
	config.analysisInitPercentageOfTracksForSignature         = setup.analysisInitPercentageOfTracksForSignature;
	if (config.analysisInitPercentageOfTracksForSignature > 100)
		config.analysisInitPercentageOfTracksForSignature = 100;
	config.analysisInitAnalysisResultWarnings                 = setup.analysisInitAnalysisResultWarnings;
	config.analysisInitAnalysisResultDisplays                 = setup.analysisInitAnalysisResultDisplays;
	config.analysisInitAnalysisMoreResultWarnings             = setup.analysisInitAnalysisMoreResultWarnings;
	config.analysisInitAnalysisMoreResultDisplays             = setup.analysisInitAnalysisMoreResultDisplays;
	config.analysisWriteCellFiles                             = setup.analysisWriteCellFiles;
	config.analysisHitCellFileName                            = setup.analysisHitCellFileName;
	config.analysisPrelutCellFileName                         = setup.analysisPrelutCellFileName;
	config.analysisLutCellFileName                            = setup.analysisLutCellFileName;
	config.initStatus                                         = setup.initStatus;
/********************************************************/

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

inf::inf(int numberOfTchars, char** tchars) 
  : configuration(),
    config()
{

	resetHeader();

	init(numberOfTchars, tchars);

}
inf::inf(std::string name) 
  : configuration(),
    config()
{

	resetHeader();

	init(name);

}
inf::inf(char* name) 
  : configuration(),
    config()
{

	resetHeader();

	init(name);

}

/****************************************************************
 * destructor													*
 ****************************************************************/

inf::~inf() {

}

/****************************************************************
 * This method returns the number of accepted commands.			*
 ****************************************************************/

unsigned int inf::getNumberOfCmds() {

	return numberOfCmds;

}

/****************************************************************
 * This method is to get the default name of the file.			*
 ****************************************************************/

std::string inf::getDefFileName() {

	return defFileName;

}

/****************************************************************
 * This method must return true if a default file should be		*
 * written.														*
 ****************************************************************/

bool inf::getWriteDefaultHeader() {

	return writeDefaultHeader;

}

/****************************************************************
 * This method is to set the default values for each header		*
 * parameter.													*
 ****************************************************************/

void inf::setHeaderDefValues() {

/********************************************************/
/* make code changes for a different configuration here */
	config.inputFileName                                      = defValInputFileName;
	config.inputFileFormat                                    = defValInputFileFormat;
	config.inputMinTracks                                     = defValInputMinTracks;
	config.inputMaxTracks                                     = defValInputMaxTracks;
	config.inputDetectorMask                                  = defValInputDetectorMask;
	config.inputFileNameMagneticField                         = defValInputFileNameMagneticField;
	config.inputMagneticFieldIsRootFile                       = defValInputMagneticFieldIsRootFile;
	config.inputMapNameMagneticField                          = defValInputMapNameMagneticField;
	config.inputMagneticFieldIntegrationStepwidthPerStation   = defValInputMagneticFieldIntegrationStepwidthPerStation;
	config.inputMagneticFieldIntegrationFactor                = defValInputMagneticFieldIntegrationFactor;
	config.inputDisableAutomaticMagneticField                 = defValInputDisableAutomaticMagneticField;
	config.inputFileNameDetector                              = defValInputFileNameDetector;
	config.inputNumberOfVolumesInfrontOfSTS                   = defValInputNumberOfVolumesInfrontOfSTS;
	config.inputDisableAutomaticDetector                      = defValInputDisableAutomaticDetector;
	config.inputCodingTableMode                               = defValInputCodingTableMode;
	config.inputGradingPTableMode                             = defValInputGradingPTableMode;
	config.inputGradingRTableMode                             = defValInputGradingRTableMode;
	config.inputCodingTableFileName                           = defValInputCodingTableFileName;
	config.inputGradingPTableFileName                         = defValInputGradingPTableFileName;
	config.inputGradingRTableFileName                         = defValInputGradingRTableFileName;
	config.inputCodingTableWrite                              = defValInputCodingTableWrite;
	config.inputGradingPTableWrite                            = defValInputGradingPTableWrite;
	config.inputGradingRTableWrite                            = defValInputGradingRTableWrite;
	config.trackfinderHitProducer                             = defValTrackfinderHitProducer;
	config.trackfinderReadPointsFromFile                      = defValTrackfinderReadPointsFromFile;
	config.trackfinderReadHitsFromFile                        = defValTrackfinderReadHitsFromFile;
	config.trackfinderReadMapsHits                            = defValTrackfinderReadMapsHits;
	config.trackfinderReadHybridHits                          = defValTrackfinderReadHybridHits;
	config.trackfinderReadStripHits                           = defValTrackfinderReadStripHits;
	config.trackfinderLutsVersion                             = defValTrackfinderLutsVersion;
	config.trackfinderPrelutFileName                          = defValTrackfinderPrelutFileName;
	config.trackfinderLutFileName                             = defValTrackfinderLutFileName;
	config.trackfinderGammaMin                                = defValTrackfinderGammaMin;
	config.trackfinderGammaMax                                = defValTrackfinderGammaMax;
	config.trackfinderGammaStep                               = defValTrackfinderGammaStep;
	config.trackfinderThetaMin                                = defValTrackfinderThetaMin;
	config.trackfinderThetaMax                                = defValTrackfinderThetaMax;
	config.trackfinderThetaStep                               = defValTrackfinderThetaStep;
	config.trackfinderPrelutRadiusMin                         = defValTrackfinderPrelutRadiusMin;
	config.trackfinderPrelutRadiusMax                         = defValTrackfinderPrelutRadiusMax;
	config.trackfinderLutRadiusMin                            = defValTrackfinderLutRadiusMin;
	config.trackfinderLutRadiusMax                            = defValTrackfinderLutRadiusMax;
	config.trackfinderLutRadiusStep                           = defValTrackfinderLutRadiusStep;
	config.trackfinderMinClassCoding                          = defValTrackfinderMinClassCoding;
	config.trackfinderMinClassGradingP                        = defValTrackfinderMinClassGradingP;
	config.trackfinderMinClassGradingR                        = defValTrackfinderMinClassGradingR;
	config.trackfinderFilterType                              = defValTrackfinderFilterType;
	config.trackfinderFirstFilterGeometry                     = defValTrackfinderFirstFilterGeometry;
	config.trackfinderFirstFilterArithmetic                   = defValTrackfinderFirstFilterArithmetic;
	config.trackfinderSecondFilterGeometry                    = defValTrackfinderSecondFilterGeometry;
	config.trackfinderSecondFilterArithmetic                  = defValTrackfinderSecondFilterArithmetic;
	config.trackfinderFirstFilterNeighborhoodDim1ClearRadius  = defValTrackfinderFirstFilterNeighborhoodDim1ClearRadius;
	config.trackfinderFirstFilterNeighborhoodDim2ClearRadius  = defValTrackfinderFirstFilterNeighborhoodDim2ClearRadius;
	config.trackfinderSecondFilterNeighborhoodDim1ClearRadius = defValTrackfinderSecondFilterNeighborhoodDim1ClearRadius;
	config.trackfinderSecondFilterNeighborhoodDim2ClearRadius = defValTrackfinderSecondFilterNeighborhoodDim2ClearRadius;
	config.trackfinderSecondFilterNeighborhoodDim3ClearRadius = defValTrackfinderSecondFilterNeighborhoodDim3ClearRadius;
	config.trackfinderAutomaticFilterCoverPercentage          = defValTrackfinderAutomaticFilterCoverPercentage;
	if (config.trackfinderAutomaticFilterCoverPercentage > 100)
		config.trackfinderAutomaticFilterCoverPercentage = 100;
	config.trackfinderAutomaticFilterDataPercentage           = defValTrackfinderAutomaticFilterDataPercentage;
	if (config.trackfinderAutomaticFilterDataPercentage > 100)
		config.trackfinderAutomaticFilterDataPercentage = 100;
	config.trackfinderAutomaticFilterWrite                    = defValTrackfinderAutomaticFilterWrite;
	config.trackfinderAutomaticFilterFileName                 = defValTrackfinderAutomaticFilterFileName;
	config.trackfinderWriteTracksToFile                       = defValTrackfinderWriteTracksToFile;
	config.analysisThresholdForP                              = defValAnalysisThresholdForP;
	config.analysisInitConfiguration                          = defValAnalysisInitConfiguration;
	config.analysisInitDetector                               = defValAnalysisInitDetector;
	config.analysisInitEvent                                  = defValAnalysisInitEvent;
	config.analysisInitClassPriority                          = defValAnalysisInitClassPriority;
	config.analysisInitMemory                                 = defValAnalysisInitMemory;
	config.analysisInitTime                                   = defValAnalysisInitTime;
	config.analysisInitTrackPropagationEventPoint             = defValAnalysisInitTrackPropagationEventPoint;
	config.analysisInitTrackPropagationEventHit               = defValAnalysisInitTrackPropagationEventHit;
	config.analysisInitTrackPropagationTotalPoint             = defValAnalysisInitTrackPropagationTotalPoint;
	config.analysisInitTrackPropagationTotalHit               = defValAnalysisInitTrackPropagationTotalHit;
	config.analysisInitTrackPropagationDisplay                = defValAnalysisInitTrackPropagationDisplay;
	config.analysisInitTrackPropagationToRoot                 = defValAnalysisInitTrackPropagationToRoot;
	config.analysisTrackPropagationPointDisplayMask           = defValAnalysisTrackPropagationPointDisplayMask;
	config.analysisTrackPropagationHitDisplayMask             = defValAnalysisTrackPropagationHitDisplayMask;
	config.analysisInitQualityEFGCEventAbsolute               = defValAnalysisInitQualityEFGCEventAbsolute;
	config.analysisInitQualityEFGCEventRelative               = defValAnalysisInitQualityEFGCEventRelative;
	config.analysisInitQualityEFGCTotalAbsolute               = defValAnalysisInitQualityEFGCTotalAbsolute;
	config.analysisInitQualityEFGCTotalRelative               = defValAnalysisInitQualityEFGCTotalRelative;
	config.analysisInitMomentumEFGCEventPzEFGC                = defValAnalysisInitMomentumEFGCEventPzEFGC;
	config.analysisInitMomentumEFGCEventPtEFGC                = defValAnalysisInitMomentumEFGCEventPtEFGC;
	config.analysisInitMomentumEFGCTotalPzEFGC                = defValAnalysisInitMomentumEFGCTotalPzEFGC;
	config.analysisInitMomentumEFGCTotalPtEFGC                = defValAnalysisInitMomentumEFGCTotalPtEFGC;
	config.analysisInitProjectionEFGCNEvent12EFGCN            = defValAnalysisInitProjectionEFGCNEvent12EFGCN;
	config.analysisInitProjectionEFGCNEvent13EFGCN            = defValAnalysisInitProjectionEFGCNEvent13EFGCN;
	config.analysisInitProjectionEFGCNEvent32EFGCN            = defValAnalysisInitProjectionEFGCNEvent32EFGCN;
	config.analysisInitProjectionEFGCNTotal12EFGCN            = defValAnalysisInitProjectionEFGCNTotal12EFGCN;
	config.analysisInitProjectionEFGCNTotal13EFGCN            = defValAnalysisInitProjectionEFGCNTotal13EFGCN;
	config.analysisInitProjectionEFGCNTotal32EFGCN            = defValAnalysisInitProjectionEFGCNTotal32EFGCN;
	config.analysisInitMomentumEvent                          = defValAnalysisInitMomentumEvent;
	config.analysisInitMomentumTotal                          = defValAnalysisInitMomentumTotal;
	config.analysisInitMomentumDisplay                        = defValAnalysisInitMomentumDisplay;
	config.analysisInitMomentumToRoot                         = defValAnalysisInitMomentumToRoot;
	config.analysisInitProjectionEvent                        = defValAnalysisInitProjectionEvent;
	config.analysisInitProjectionTotal                        = defValAnalysisInitProjectionTotal;
	config.analysisInitProjectionDisplay                      = defValAnalysisInitProjectionDisplay;
	config.analysisInitProjectionToRoot                       = defValAnalysisInitProjectionToRoot;
	config.analysisInitMagnetfieldX                           = defValAnalysisInitMagnetfieldX;
	config.analysisInitMagnetfieldY                           = defValAnalysisInitMagnetfieldY;
	config.analysisInitMagnetfieldZ                           = defValAnalysisInitMagnetfieldZ;
	config.analysisInitMagnetfieldDisplay                     = defValAnalysisInitMagnetfieldDisplay;
	config.analysisInitMagnetfieldToRoot                      = defValAnalysisInitMagnetfieldToRoot;
	config.analysisInitMagnetfieldConstantForEachEvent        = defValAnalysisInitMagnetfieldConstantForEachEvent;
	config.analysisInitWeightedMagnetfieldConstant            = defValAnalysisInitWeightedMagnetfieldConstant;
	config.analysisInitMagnetfieldConstantDisplay             = defValAnalysisInitMagnetfieldConstantDisplay;
	config.analysisMagnetfieldConstantDisplayMask             = defValAnalysisMagnetfieldConstantDisplayMask;
	config.analysisInitMagnetfieldConstantToRoot              = defValAnalysisInitMagnetfieldConstantToRoot;
	config.analysisInitMagnetfieldVSConstants                 = defValAnalysisInitMagnetfieldVSConstants;
	config.analysisInitPrelutGoodness                         = defValAnalysisInitPrelutGoodness;
	config.analysisInitLutGoodness                            = defValAnalysisInitLutGoodness;
	config.analysisInitHoughTransformGoodness                 = defValAnalysisInitHoughTransformGoodness;
	config.analysisInitQuantizationGoodness                   = defValAnalysisInitQuantizationGoodness;
	config.analysisInitPeakDistanceGoodness                   = defValAnalysisInitPeakDistanceGoodness;
	config.analysisInitCreatedHistogramToRoot                 = defValAnalysisInitCreatedHistogramToRoot;
	config.analysisInitEncodedHistogramToRoot                 = defValAnalysisInitEncodedHistogramToRoot;
	config.analysisInitFilteredHistogramToRoot                = defValAnalysisInitFilteredHistogramToRoot;
	config.analysisInitJustOneCreatedHistogramToRoot          = defValAnalysisInitJustOneCreatedHistogramToRoot;
	config.analysisInitJustOneEncodedHistogramToRoot          = defValAnalysisInitJustOneEncodedHistogramToRoot;
	config.analysisInitJustOneFilteredHistogramToRoot         = defValAnalysisInitJustOneFilteredHistogramToRoot;
	config.analysisInitCreatedHistogramToShow                 = defValAnalysisInitCreatedHistogramToShow;
	config.analysisInitEncodedHistogramToShow                 = defValAnalysisInitEncodedHistogramToShow;
	config.analysisInitFilteredHistogramToShow                = defValAnalysisInitFilteredHistogramToShow;
	config.analysisInitHistogramLayer                         = defValAnalysisInitHistogramLayer;
	config.analysisInitNumberOfTracksPerColumn                = defValAnalysisInitNumberOfTracksPerColumn;
	config.analysisInitNumberOfTracksPerRow                   = defValAnalysisInitNumberOfTracksPerRow;
	config.analysisInitNumberOfTracksPerLayer                 = defValAnalysisInitNumberOfTracksPerLayer;
	config.analysisInitHitReadoutDistribution                 = defValAnalysisInitHitReadoutDistribution;
	config.analysisInitReadoutColumnsInParallel               = defValAnalysisInitReadoutColumnsInParallel;
	config.analysisInitPrelutRangeForEachEvent                = defValAnalysisInitPrelutRangeForEachEvent;
	config.analysisInitWeightedPrelutRange                    = defValAnalysisInitWeightedPrelutRange;
	config.analysisInitPrelutRangeDisplay                     = defValAnalysisInitPrelutRangeDisplay;
	config.analysisInitPrelutRangeDisplayMode                 = defValAnalysisInitPrelutRangeDisplayMode;
	config.analysisPrelutRangeStationDisplayMask              = defValAnalysisPrelutRangeStationDisplayMask;
	config.analysisPrelutRangeStationSumDisplayMask           = defValAnalysisPrelutRangeStationSumDisplayMask;
	config.analysisPrelutRangeConstraintDisplayMask           = defValAnalysisPrelutRangeConstraintDisplayMask;
	config.analysisPrelutRangeConstraintSumDisplayMask        = defValAnalysisPrelutRangeConstraintSumDisplayMask;
	config.analysisPrelutRangeRelativeDisplayMask             = defValAnalysisPrelutRangeRelativeDisplayMask;
	config.analysisInitPrelutRangeToRoot                      = defValAnalysisInitPrelutRangeToRoot;
	config.analysisInitPercentageOfHitsForPrelutRange         = defValAnalysisInitPercentageOfHitsForPrelutRange;
	if (config.analysisInitPercentageOfHitsForPrelutRange > 100)
		config.analysisInitPercentageOfHitsForPrelutRange = 100;
	config.analysisPrelutRangeMinStart                        = defValAnalysisPrelutRangeMinStart;
	config.analysisPrelutRangeMinStop                         = defValAnalysisPrelutRangeMinStop;
	config.analysisPrelutRangeMinSteps                        = defValAnalysisPrelutRangeMinSteps;
	config.analysisPrelutRangeMaxStart                        = defValAnalysisPrelutRangeMaxStart;
	config.analysisPrelutRangeMaxStop                         = defValAnalysisPrelutRangeMaxStop;
	config.analysisPrelutRangeMaxSteps                        = defValAnalysisPrelutRangeMaxSteps;
	config.analysisChooseMainPrelutRange                      = defValAnalysisChooseMainPrelutRange;
	config.analysisChooseConstraintPrelutRange                = defValAnalysisChooseConstraintPrelutRange;
	config.analysisInitTotalAnalysis                          = defValAnalysisInitTotalAnalysis;
	config.analysisInitPercentageOfHitsInSignature            = defValAnalysisInitPercentageOfHitsInSignature;
	if (config.analysisInitPercentageOfHitsInSignature > 100)
		config.analysisInitPercentageOfHitsInSignature = 100;
	config.analysisInitPercentageOfTracksForSignature         = defValAnalysisInitPercentageOfTracksForSignature;
	if (config.analysisInitPercentageOfTracksForSignature > 100)
		config.analysisInitPercentageOfTracksForSignature = 100;
	config.analysisInitAnalysisResultWarnings                 = defValAnalysisInitAnalysisResultWarnings;
	config.analysisInitAnalysisResultDisplays                 = defValAnalysisInitAnalysisResultDisplays;
	config.analysisInitAnalysisMoreResultWarnings             = defValAnalysisInitAnalysisMoreResultWarnings;
	config.analysisInitAnalysisMoreResultDisplays             = defValAnalysisInitAnalysisMoreResultDisplays;
	config.analysisWriteCellFiles                             = defValAnalysisWriteCellFiles;
	config.analysisHitCellFileName                            = defValAnalysisHitCellFileName;
	config.analysisPrelutCellFileName                         = defValAnalysisPrelutCellFileName;
	config.analysisLutCellFileName                            = defValAnalysisLutCellFileName;
	config.initStatus                                         = defValInitStatus;
	config.analysisOutputFileName                             = defValAnalysisOutputFileName;
/********************************************************/

}

/****************************************************************
 * This method assigns the value to the corresponding parameter	*
 * which is identified by a specifier.
 ****************************************************************/

bool inf::getHeaderValue(std::string& specifier, std::string& value) {

	bool specifierFound = false;

/********************************************************/
/* make code changes for a different configuration here */
	if (!specifierFound)
		specifierFound = getInputHeaderValue(specifier, value);
	if (!specifierFound)
		specifierFound = getTrackfinderHeaderValue(specifier, value);
	if (!specifierFound)
		specifierFound = getInitialisationHeaderValue(specifier, value);
	if (!specifierFound)
		specifierFound = getQualityHeaderValue(specifier, value);
	if (!specifierFound)
		specifierFound = getMomentumHeaderValue(specifier, value);
	if (!specifierFound)
		specifierFound = getProjectionHeaderValue(specifier, value);
	if (!specifierFound)
		specifierFound = getMagnetfieldHeaderValue(specifier, value);
	if (!specifierFound)
		specifierFound = getGoodnessHeaderValue(specifier, value);
	if (!specifierFound)
		specifierFound = getHistogramHeaderValue(specifier, value);
	if (!specifierFound)
		specifierFound = getHardwareHeaderValue(specifier, value);
	if (!specifierFound)
		specifierFound = getPrelutRangeHeaderValue(specifier, value);
	if (!specifierFound)
		specifierFound = getTotalAnalysisHeaderValue(specifier, value);
	if (!specifierFound)
		specifierFound = getCellAnalysisHeaderValue(specifier, value);
	if (!specifierFound) {
		if (specifier.compare(stringCmdAnalysisOutputFileName) == 0) {
			if (!isHeaderLockSet(idCmdAnalysisOutputFileName)) {
				config.analysisOutputFileName = value;
				setHeaderLock(idCmdAnalysisOutputFileName, true);
				specifierFound = true;
			}
		}
	}
/********************************************************/

	return specifierFound;

}

/****************************************************************
 * This method is to write the header of the file.				*
 ****************************************************************/

void inf::writeFileHeader(std::ofstream& fileStream, terminalSequence* statusSequence) {

/********************************************************/
/* make code changes for a different configuration here */
	char buffer[300];
	char conversionBuffer[intConversionDigits + 1];

	writeComment(fileStream, "[PARAMETERS FOR THE INPUT OBJECT]");

	setHeaderValue(fileStream, stringCmdInputFileName,                                               config.inputFileName,                                             "Filename of the file which consists of the input data", statusSequence);

	strcpy(buffer, "Fileformat of the file which consists of the input data -> (");
	itos(ASCIIFILEFORMAT, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":ASCII, ");
	itos(ROOTFILEFORMAT, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":Standalone ROOT)");

	setHeaderValue(fileStream, stringCmdInputFileFormat,                                             config.inputFileFormat,                                           buffer, statusSequence);
	setHeaderValue(fileStream, stringCmdInputMinTracks,                                              config.inputMinTracks,                                            "Number of minimum tracks to be considered in the program", statusSequence);
	setHeaderValue(fileStream, stringCmdInputMaxTracks,                                              config.inputMaxTracks,                                            "Number of maximum tracks to be considered in the program", statusSequence);
	setHeaderValue(fileStream, stringCmdInputDetectorMask,                                           config.inputDetectorMask.toString().c_str(),                      "Number which inhibits corresponding detector stations from computation", statusSequence);
	setHeaderValue(fileStream, stringCmdInputFileNameMagneticField,                                  config.inputFileNameMagneticField,                                "Filename of the file which consists of the magnetic field map", statusSequence);
	setHeaderValue(fileStream, stringCmdInputMagneticFieldIsRootFile,                                config.inputMagneticFieldIsRootFile,                              "True if the file which consists of the magnetic field map is a root-file", statusSequence);
	setHeaderValue(fileStream, stringCmdInputMapNameMagneticField,                                   config.inputMapNameMagneticField,                                 "Name of the map in the root-file which consists of the magnetic field map", statusSequence);
	setHeaderValue(fileStream, stringCmdInputMagneticFieldIntegrationStepwidthPerStation,            config.inputMagneticFieldIntegrationStepwidthPerStation,          "Number of points which are inserted between each detector station to integrate the magnetic field", statusSequence);
	setHeaderValue(fileStream, stringCmdInputMagneticFieldIntegrationFactor,                         config.inputMagneticFieldIntegrationFactor,                       "Factor to correct the integrated magnetic field", statusSequence);
	setHeaderValue(fileStream, stringCmdInputDisableAutomaticMagneticField,                          config.inputDisableAutomaticMagneticField,                        "True if the magnetic field should be used directly from the framework or not", statusSequence);
	setHeaderValue(fileStream, stringCmdInputFileNameDetector,                                       config.inputFileNameDetector,                                     "Filename of the file which consists of the detector information", statusSequence);
	setHeaderValue(fileStream, stringCmdInputNumberOfVolumesInfrontOfSTS,                            config.inputNumberOfVolumesInfrontOfSTS,                          "Number of geometric volumes which are established infront of the detector. Needed to compute the correct volumeID", statusSequence);
	setHeaderValue(fileStream, stringCmdInputDisableAutomaticDetector,                               config.inputDisableAutomaticDetector,                             "False if the detector should be used directly from the framework or not", statusSequence);

	strcpy(buffer, "Version of the used tables -> (");
	itos(NOTABLE, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":NOTABLE, ");
	itos(CROSSFOOTTABLE, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":CROSSFOOTTABLE, ");
	itos(LUTGOODNESSTABLE, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":LUTGOODNESSTABLE, ");
	itos(FILETABLE, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":FILETABLE, ");
	itos(ONLINETABLE, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":ONLINETABLE)");

	setHeaderValue(fileStream, stringCmdInputCodingTableMode,                                        config.inputCodingTableMode,                                      buffer, statusSequence);
	setHeaderValue(fileStream, stringCmdInputGradingPTableMode,                                      config.inputGradingPTableMode,                                    buffer, statusSequence);
	setHeaderValue(fileStream, stringCmdInputGradingRTableMode,                                      config.inputGradingRTableMode,                                    buffer, statusSequence);
	setHeaderValue(fileStream, stringCmdInputCodingTableFileName,                                    config.inputCodingTableFileName,                                  "Name of the file which consists the coding table if enabled", statusSequence);
	setHeaderValue(fileStream, stringCmdInputGradingPTableFileName,                                  config.inputGradingPTableFileName,                                "Name of the file which consists the gradingP table if enabled", statusSequence);
	setHeaderValue(fileStream, stringCmdInputGradingRTableFileName,                                  config.inputGradingRTableFileName,                                "Name of the file which consists the gradingR table if enabled", statusSequence);
	setHeaderValue(fileStream, stringCmdInputCodingTableWrite,                                       config.inputCodingTableWrite,                                     "Initializes that the coding table is written to file", statusSequence);
	setHeaderValue(fileStream, stringCmdInputGradingPTableWrite,                                     config.inputGradingPTableWrite,                                   "Initializes that the gradingP table is written to file", statusSequence);
	setHeaderValue(fileStream, stringCmdInputGradingRTableWrite,                                     config.inputGradingRTableWrite,                                   "Initializes that the gradingR table is written to file", statusSequence);

	writeComment(fileStream, "");
	writeComment(fileStream, "[PARAMETERS FOR THE TRACKFINDER OBJECT]");

	strcpy(buffer, "Type of the hitProducer -> (");
	itos(NONEHITPRODUCER, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":NONE, ");
	itos(BINNINGHITPRODUCER, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":Binning, ");
	itos(SMEARINGHITPRODUCER, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":Smearing)");

	setHeaderValue(fileStream, stringCmdTrackfinderHitProducer,                                      config.trackfinderHitProducer,                                    buffer, statusSequence);
	setHeaderValue(fileStream, stringCmdTrackfinderReadPointsFromFile,                               config.trackfinderReadPointsFromFile,                             "Just points are read out of the root file", statusSequence);
	setHeaderValue(fileStream, stringCmdTrackfinderReadHitsFromFile,                                 config.trackfinderReadHitsFromFile,                               "The hits are read out of the root file", statusSequence);
	setHeaderValue(fileStream, stringCmdTrackfinderReadMapsHits,                                     config.trackfinderReadMapsHits,                                   "Maps hits are read", statusSequence);
	setHeaderValue(fileStream, stringCmdTrackfinderReadHybridHits,                                   config.trackfinderReadHybridHits,                                 "Hybrid hits are read", statusSequence);
	setHeaderValue(fileStream, stringCmdTrackfinderReadStripHits,                                    config.trackfinderReadStripHits,                                  "Strip Hits are read", statusSequence);

	strcpy(buffer, "Version of the used look-up-tables -> (");
	itos(RUNGEKUTTAFILELUT, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":Generate Runge-Kutta-Lut, ");
	itos(ANALYTICFORMULAFILELUT, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":Generate Formula-Lut, ");
	itos(FILELUT, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":Use File-Lut, ");
	itos(ANALYTICFORMULALUT, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":Use Formula)");

	setHeaderValue(fileStream, stringCmdTrackfinderLutsVersion,                                      config.trackfinderLutsVersion,                                      buffer, statusSequence);
	setHeaderValue(fileStream, stringCmdTrackfinderPrelutFileName,                                   config.trackfinderPrelutFileName,                                   "Name of the file which consists the prelut", statusSequence);
	setHeaderValue(fileStream, stringCmdTrackfinderLutFileName,                                      config.trackfinderLutFileName,                                      "Name of the file which consists the lut", statusSequence);
	setHeaderValue(fileStream, stringCmdTrackfinderGammaMin,                                         config.trackfinderGammaMin,                                         "Minimal gamma angle value for the hough transformation", statusSequence);
	setHeaderValue(fileStream, stringCmdTrackfinderGammaMax,                                         config.trackfinderGammaMax,                                         "Maximal gamma angle value for the hough transformation", statusSequence);
	setHeaderValue(fileStream, stringCmdTrackfinderGammaStep,                                        config.trackfinderGammaStep,                                        "Quantization steps for the gamma angle", statusSequence);
	setHeaderValue(fileStream, stringCmdTrackfinderThetaMin,                                         config.trackfinderThetaMin,                                         "Minimal theta angle value for the hough transformation", statusSequence);
	setHeaderValue(fileStream, stringCmdTrackfinderThetaMax,                                         config.trackfinderThetaMax,                                         "Maximal theta angle value for the hough transformation", statusSequence);
	setHeaderValue(fileStream, stringCmdTrackfinderThetaStep,                                        config.trackfinderThetaStep,                                        "Quantization steps for the theta angle", statusSequence);
	setHeaderValue(fileStream, stringCmdTrackfinderPrelutRadiusMin,                                  config.trackfinderPrelutRadiusMin,                                  "Minimal PrelutRadius for the hough transformation", statusSequence);
	setHeaderValue(fileStream, stringCmdTrackfinderPrelutRadiusMax,                                  config.trackfinderPrelutRadiusMax,                                  "Maximal PrelutRadius for the hough transformation", statusSequence);
	setHeaderValue(fileStream, stringCmdTrackfinderLutRadiusMin,                                     config.trackfinderLutRadiusMin,                                     "Minimal LutRadius for the hough transformation", statusSequence);
	setHeaderValue(fileStream, stringCmdTrackfinderLutRadiusMax,                                     config.trackfinderLutRadiusMax,                                     "Maximal LutRadius for the hough transformation", statusSequence);
	setHeaderValue(fileStream, stringCmdTrackfinderLutRadiusStep,                                    config.trackfinderLutRadiusStep,                                    "Quantization steps for the LutRadius", statusSequence);
	setHeaderValue(fileStream, stringCmdTrackfinderMinClassCoding,                                   config.trackfinderMinClassCoding,                                   "Minimal allowed classification in the coding table", statusSequence);
	setHeaderValue(fileStream, stringCmdTrackfinderMinClassGradingP,                                 config.trackfinderMinClassGradingP,                                 "Minimal allowed classification in the gradingP table", statusSequence);
	setHeaderValue(fileStream, stringCmdTrackfinderMinClassGradingR,                                 config.trackfinderMinClassGradingR,                                 "Minimal allowed classification in the gradingR table", statusSequence);

	strcpy(buffer, "Select the general filter -> (");
	itos(NOFILTER, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":No filter, ");
	itos(MAXMORPHSEARCHFILTER, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":MaxMorphSearch filter, ");
	itos(ERASERFILTER, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":Eraser filter, ");
	itos(FILEFILTER, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":Read filter from file, ");
	itos(AUTOMATICFIRSTEVENTFILTER, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":Automatic first event filter, ");
	itos(AUTOMATICEACHEVENTFILTER, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":Automatic each event filter, ");
	itos(AUTOMATICUPDATEEVENTFILTER, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":Automatic update event filter)");

	setHeaderValue(fileStream, stringCmdTrackfinderFilterType,                                       config.trackfinderFilterType,                                       buffer, statusSequence);

	strcpy(buffer, "Select the first filter geometry -> (");
	itos(NOFIRSTGEOMETRY, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":No geometry, ");
	itos(FIRST21GEOMETRY, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":FilterDim2 and filterDim1, ");
	itos(FIRST122GEOMETRY, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":FilterDim12 and filterDim2, ");
	itos(FIRST12GEOMETRY, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":FilterDim1Dim2, ");
	itos(FIRST12MODGEOMETRY, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":FilterDim1Dim2Mod, ");
	itos(FIRST121GEOMETRY, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":FilterDim12Dim1, ");
	itos(FIRSTFINALGEOMETRY, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":FirstFilterFinal, ");
	itos(FIRSTFINALMODGEOMETRY, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":FirstFilterFinalMod)");

	setHeaderValue(fileStream, stringCmdTrackfinderFirstFilterGeometry,                              config.trackfinderFirstFilterGeometry,                              buffer, statusSequence);

	strcpy(buffer, "Select the first filter arithmetic -> (");
	itos(NOFIRSTARITHMETIC, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":No arithmetic, ");
	itos(FIRSTSIMPLEARITHMETIC, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":simple arithmetic, ");
	itos(FIRSTSIMPLEMODARITHMETIC, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":simple modified arithmetic, ");
	itos(FIRSTCOMPLEXARITHMETIC, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":complex arithmetic, ");
	itos(FIRSTCOMPLEXMODARITHMETIC, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":complex modified arithmetic, ");
	itos(FIRSTSPECIALARITHMETIC, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, "special arithmetic:)");

	setHeaderValue(fileStream, stringCmdTrackfinderFirstFilterArithmetic,                            config.trackfinderFirstFilterArithmetic,                            buffer, statusSequence);

	strcpy(buffer, "Select the second filter geometry -> (");
	itos(NOSECONDGEOMETRY, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":No geometry, ");
	itos(SECOND3MODGEOMETRY, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":FilterDim3Mod, ");
	itos(SECONDFINALGEOMETRY, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":SecondFilterFinal, ");
	itos(SECONDFINALMODGEOMETRY, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":SecondFilterFinalMod, ");
	itos(SECOND3GEOMETRY, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":FilterDim3)");

	setHeaderValue(fileStream, stringCmdTrackfinderSecondFilterGeometry,                             config.trackfinderSecondFilterGeometry,                             buffer, statusSequence);

	strcpy(buffer, "Select the second filter arithmetic -> (");
	itos(NOSECONDARITHMETIC, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":No arithmetic, ");
	itos(SECONDSIMPLEARITHMETIC, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":simple arithmetic, ");
	itos(SECONDSIMPLEMODARITHMETIC, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":simple modified arithmetic, ");
	itos(SECONDCOMPLEXARITHMETIC, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":complex arithmetic, ");
	itos(SECONDCOMPLEXMODARITHMETIC, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":complex modified arithmetic, ");
	itos(SECONDSPECIALARITHMETIC, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, "special arithmetic:)");

	setHeaderValue(fileStream, stringCmdTrackfinderSecondFilterArithmetic,                           config.trackfinderSecondFilterArithmetic,                           buffer, statusSequence);
	setHeaderValue(fileStream, stringCmdTrackfinderFirstFilterNeighborhoodDim1ClearRadius,           config.trackfinderFirstFilterNeighborhoodDim1ClearRadius,           "FirstFilter's Dim1-radius which is cleared around a peak", statusSequence);
	setHeaderValue(fileStream, stringCmdTrackfinderFirstFilterNeighborhoodDim2ClearRadius,           config.trackfinderFirstFilterNeighborhoodDim2ClearRadius,           "FirstFilter's Dim2-radius which is cleared around a peak", statusSequence);
	setHeaderValue(fileStream, stringCmdTrackfinderSecondFilterNeighborhoodDim1ClearRadius,          config.trackfinderSecondFilterNeighborhoodDim1ClearRadius,          "SecondFilter's Dim1-radius which is cleared around a peak", statusSequence);
	setHeaderValue(fileStream, stringCmdTrackfinderSecondFilterNeighborhoodDim2ClearRadius,          config.trackfinderSecondFilterNeighborhoodDim2ClearRadius,          "SecondFilter's Dim2-radius which is cleared around a peak", statusSequence);
	setHeaderValue(fileStream, stringCmdTrackfinderSecondFilterNeighborhoodDim3ClearRadius,          config.trackfinderSecondFilterNeighborhoodDim3ClearRadius,          "SecondFilter's Dim3-radius which is cleared around a peak", statusSequence);
	setHeaderValue(fileStream, stringCmdTrackfinderAutomaticFilterCoverPercentage,                   config.trackfinderAutomaticFilterCoverPercentage,                   "Percentage of occurence frequency for a geometry element to be used", statusSequence);
	setHeaderValue(fileStream, stringCmdTrackfinderAutomaticFilterDataPercentage,                    config.trackfinderAutomaticFilterDataPercentage,                    "Percentage of source data which should be used to generate the automatic filter", statusSequence);
	setHeaderValue(fileStream, stringCmdTrackfinderAutomaticFilterWrite,                             config.trackfinderAutomaticFilterWrite,                             "True, if the automatic filter has to be written into a file", statusSequence);
	setHeaderValue(fileStream, stringCmdTrackfinderAutomaticFilterFileName,                          config.trackfinderAutomaticFilterFileName,                          "Name of the file which contains the automatic filter geometry for writing or reading", statusSequence);
	setHeaderValue(fileStream, stringCmdTrackfinderWriteTracksToFile,                                config.trackfinderWriteTracksToFile,                                "False if the output should be placed directly into the framework interface", statusSequence);

	writeComment(fileStream, "");
	writeComment(fileStream, "[PARAMETERS FOR THE ANALYSIS OBJECT]");

	setHeaderValue(fileStream, stringCmdAnalysisOutputFileName,                                      config.analysisOutputFileName,                                      "Filename of the file which consists of the analyis graphic output data", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisThresholdForP,                                       config.analysisThresholdForP,                                       "Threshold for P to be a track", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitConfiguration,                                   config.analysisInitConfiguration,                                   "Initialize analysis for the configuration", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitDetector,                                        config.analysisInitDetector,                                        "Initialize analysis for the detector", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitEvent,                                           config.analysisInitEvent,                                           "Initialize analysis for the event", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitClassPriority,                                   config.analysisInitClassPriority,                                   "Initialize analysis for the classPriority", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitMemory,                                          config.analysisInitMemory,                                          "Initialize analysis for the memory", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitTime,                                            config.analysisInitTime,                                            "Initialize analysis for the time", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitTrackPropagationEventPoint,                      config.analysisInitTrackPropagationEventPoint,                      "Initialize analysis for the goodness of the input point data for each event", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitTrackPropagationEventHit,                        config.analysisInitTrackPropagationEventHit,                        "Initialize analysis for the goodness of the input hit data for each event", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitTrackPropagationTotalPoint,                      config.analysisInitTrackPropagationTotalPoint,                      "Initialize analysis for the goodness of the input point data for all events", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitTrackPropagationTotalHit,                        config.analysisInitTrackPropagationTotalHit,                        "Initialize analysis for the goodness of the input hit data for all events", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitTrackPropagationDisplay,                         config.analysisInitTrackPropagationDisplay,                         "Initialize analysis for the graphic display", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitTrackPropagationToRoot,                          config.analysisInitTrackPropagationToRoot,                          "Initialize analysis for the graphic-to-root-file functionality", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisTrackPropagationPointDisplayMask,                    config.analysisTrackPropagationPointDisplayMask.toString().c_str(), "Number which inhibits corresponding input point data results from drawing", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisTrackPropagationHitDisplayMask,                      config.analysisTrackPropagationHitDisplayMask.toString().c_str(),   "Number which inhibits corresponding input hit data results from drawing", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitQualityEFGCEventAbsolute,                        config.analysisInitQualityEFGCEventAbsolute,                        "Initialize analysis for the absolute EFGC-quality for the event", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitQualityEFGCEventRelative,                        config.analysisInitQualityEFGCEventRelative,                        "Initialize analysis for the relative EFGC-quality for the event", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitQualityEFGCTotalAbsolute,                        config.analysisInitQualityEFGCTotalAbsolute,                        "Initialize analysis for the absolute EFGC-quality for all events", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitQualityEFGCTotalRelative,                        config.analysisInitQualityEFGCTotalRelative,                        "Initialize analysis for the relative EFGC-quality for all events", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitMomentumEFGCEventPzEFGC,                         config.analysisInitMomentumEFGCEventPzEFGC,                         "Initialize analysis for the EFGC-Pz-quality for the event", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitMomentumEFGCEventPtEFGC,                         config.analysisInitMomentumEFGCEventPtEFGC,                         "Initialize analysis for the EFGC-Pt-quality for the event", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitMomentumEFGCTotalPzEFGC,                         config.analysisInitMomentumEFGCTotalPzEFGC,                         "Initialize analysis for the EFGC-Pz-quality for all events", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitMomentumEFGCTotalPtEFGC,                         config.analysisInitMomentumEFGCTotalPtEFGC,                         "Initialize analysis for the EFGC-Pt-quality for all events", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitProjectionEFGCNEvent12EFGCN,                     config.analysisInitProjectionEFGCNEvent12EFGCN,                     "Initialize analysis for the EFGCN-12-quality for the event", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitProjectionEFGCNEvent13EFGCN,                     config.analysisInitProjectionEFGCNEvent13EFGCN,                     "Initialize analysis for the EFGCN-13-quality for the event", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitProjectionEFGCNEvent32EFGCN,                     config.analysisInitProjectionEFGCNEvent32EFGCN,                     "Initialize analysis for the EFGCN-32-quality for the event", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitProjectionEFGCNTotal12EFGCN,                     config.analysisInitProjectionEFGCNTotal12EFGCN,                     "Initialize analysis for the EFGCN-12-quality for all events", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitProjectionEFGCNTotal13EFGCN,                     config.analysisInitProjectionEFGCNTotal13EFGCN,                     "Initialize analysis for the EFGCN-13-quality for all events", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitProjectionEFGCNTotal32EFGCN,                     config.analysisInitProjectionEFGCNTotal32EFGCN,                     "Initialize analysis for the EFGCN-32-quality for all events", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitMomentumEvent,                                   config.analysisInitMomentumEvent,                                   "Initialize analysis for the graphic quality for the event", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitMomentumTotal,                                   config.analysisInitMomentumTotal,                                   "Initialize analysis for the graphic quality for all events", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitMomentumDisplay,                                 config.analysisInitMomentumDisplay,                                 "Initialize analysis for the graphic display", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitMomentumToRoot,                                  config.analysisInitMomentumToRoot,                                  "Initialize analysis for the graphic-to-root-file functionality", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitProjectionEvent,                                 config.analysisInitProjectionEvent,                                 "Initialize analysis for the graphic quality for the event", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitProjectionTotal,                                 config.analysisInitProjectionTotal,                                 "Initialize analysis for the graphic quality for all events", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitProjectionDisplay,                               config.analysisInitProjectionDisplay,                               "Initialize analysis for the graphic display", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitProjectionToRoot,                                config.analysisInitProjectionToRoot,                                "Initialize analysis for the graphic-to-root-file functionality", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitMagnetfieldX,                                    config.analysisInitMagnetfieldX,                                    "Initialize analysis for the magnetfield in the x dimension", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitMagnetfieldY,                                    config.analysisInitMagnetfieldY,                                    "Initialize analysis for the magnetfield in the y dimension", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitMagnetfieldZ,                                    config.analysisInitMagnetfieldZ,                                    "Initialize analysis for the magnetfield in the z dimension", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitMagnetfieldDisplay,                              config.analysisInitMagnetfieldDisplay,                              "Initialize analysis for the magnetfield display", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitMagnetfieldToRoot,                               config.analysisInitMagnetfieldToRoot,                               "Initialize analysis for the magnetfield-to-root-file functionality", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitMagnetfieldConstantForEachEvent,                 config.analysisInitMagnetfieldConstantForEachEvent,                 "Initialize analysis for the magnetfield factors for each event", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitWeightedMagnetfieldConstant,                     config.analysisInitWeightedMagnetfieldConstant,                     "Initialize analysis for the magnetfield factors to be weighted for the events", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitMagnetfieldConstantDisplay,                      config.analysisInitMagnetfieldConstantDisplay,                      "Initialize analysis for the magnetfield constant display", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisMagnetfieldConstantDisplayMask,                      config.analysisMagnetfieldConstantDisplayMask.toString().c_str(),   "Number which inhibits corresponding magnetfield constants from drawing", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitMagnetfieldConstantToRoot,                       config.analysisInitMagnetfieldConstantToRoot,                       "Initialize analysis for the magnetfield-constant-to-root-file functionality", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitMagnetfieldVSConstants,                          config.analysisInitMagnetfieldVSConstants,                          "Initialize analysis for the comparisson between the magnetfield and the constants", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitPrelutGoodness,                                  config.analysisInitPrelutGoodness,                                  "Initialize analysis for the prelut", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitLutGoodness,                                     config.analysisInitLutGoodness,                                     "Initialize analysis for the lut", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitHoughTransformGoodness,                          config.analysisInitHoughTransformGoodness,                          "Initialize analysis for the Hough transform", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitQuantizationGoodness,                            config.analysisInitQuantizationGoodness,                            "Initialize analysis for the quantization", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitPeakDistanceGoodness,                            config.analysisInitPeakDistanceGoodness,                            "Initialize analysis for the peak distance", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitCreatedHistogramToRoot,                          config.analysisInitCreatedHistogramToRoot,                          "Initialize analysis for the created histogram", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitEncodedHistogramToRoot,                          config.analysisInitEncodedHistogramToRoot,                          "Initialize analysis for the encoded histogram", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitFilteredHistogramToRoot,                         config.analysisInitFilteredHistogramToRoot,                         "Initialize analysis for the filtered histogram", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitJustOneCreatedHistogramToRoot,                   config.analysisInitJustOneCreatedHistogramToRoot,                   "Initialize analysis for just one created histogram", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitJustOneEncodedHistogramToRoot,                   config.analysisInitJustOneEncodedHistogramToRoot,                   "Initialize analysis for just one encoded histogram", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitJustOneFilteredHistogramToRoot,                  config.analysisInitJustOneFilteredHistogramToRoot,                  "Initialize analysis for just one filtered histogram", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitCreatedHistogramToShow,                          config.analysisInitCreatedHistogramToShow,                          "Initialize analysis for showing the created histogram", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitEncodedHistogramToShow,                          config.analysisInitEncodedHistogramToShow,                          "Initialize analysis for showing the encoded histogram", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitFilteredHistogramToShow,                         config.analysisInitFilteredHistogramToShow,                         "Initialize analysis for showing the filtered histogram", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitHistogramLayer,                                  config.analysisInitHistogramLayer,                                  "Initialize the index of the layer to make just one histogram accessible for showing and/or writing", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitNumberOfTracksPerColumn,                         config.analysisInitNumberOfTracksPerColumn,                         "Initialize analysis for evaluating the number of tracks per column in all histogram layers", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitNumberOfTracksPerRow,                            config.analysisInitNumberOfTracksPerRow,                            "Initialize analysis for evaluating the number of tracks per row in all histogram layers", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitNumberOfTracksPerLayer,                          config.analysisInitNumberOfTracksPerLayer,                          "Initialize analysis for evaluating the number of tracks per histogram layer", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitHitReadoutDistribution,                          config.analysisInitHitReadoutDistribution,                          "Initialize analysis for evaluating the distribution how often a hit must be read out of the memory and processed to build all histogram layers", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitReadoutColumnsInParallel,                        config.analysisInitReadoutColumnsInParallel,                        "Initialize the readout modi of the histogram", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitPrelutRangeForEachEvent,                         config.analysisInitPrelutRangeForEachEvent,                         "Initialize analysis for the prelut ranges for each event", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitWeightedPrelutRange,                             config.analysisInitWeightedPrelutRange,                             "Initialize analysis for the prelut ranges to be weighted for the events", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitPrelutRangeDisplay,                              config.analysisInitPrelutRangeDisplay,                              "Initialize the prelut range analysis display", statusSequence);

	strcpy(buffer, "Mode of the prelut range analysis display -> (");
	itos(CORRECTRELATIVEDISPLAYMODE, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":Correct layer relative, ");
	itos(CUTCORRECTRELATIVEDISPLAYMODE, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":Correct layer relative and cut, ");
	itos(NORMALCORRECTDISPLAYMODE, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":Correct layer, ");
	itos(MAINRELATIVEDISPLAYMODE, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":Main layer relative, ");
	itos(CUTMAINRELATIVEDISPLAYMODE, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":Main layer relative and cut, ");
	itos(NORMALMAINDISPLAYMODE, conversionBuffer, 10, intConversionDigits);
	strcat(buffer, conversionBuffer);
	strcat(buffer, ":Main layer)");

	setHeaderValue(fileStream, stringCmdAnalysisInitPrelutRangeDisplayMode,                          config.analysisInitPrelutRangeDisplayMode,                          buffer, statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisPrelutRangeStationDisplayMask,                       config.analysisPrelutRangeStationDisplayMask.toString().c_str(),    "Number which inhibits prelut range displays for each station result from drawing", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisPrelutRangeStationSumDisplayMask,                    config.analysisPrelutRangeStationSumDisplayMask,                    "Initialize the inhibition of the prelut range display for the station sum result from drawing", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisPrelutRangeConstraintDisplayMask,                    config.analysisPrelutRangeConstraintDisplayMask.toString().c_str(), "Number which inhibits prelut range displays for each constraint result from drawing", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisPrelutRangeConstraintSumDisplayMask,                 config.analysisPrelutRangeConstraintSumDisplayMask,                 "Initialize the inhibition of the prelut range display for the constraint sum result from drawing", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisPrelutRangeRelativeDisplayMask,                      config.analysisPrelutRangeRelativeDisplayMask,                      "Initialize the inhibition of the prelut range display for the relative result from drawing", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitPrelutRangeToRoot,                               config.analysisInitPrelutRangeToRoot,                               "Initialize analysis for the prelut-range-to-root-file functionality", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitPercentageOfHitsForPrelutRange,                  config.analysisInitPercentageOfHitsForPrelutRange,                  "Initialize the percentage of the number of hits which must be in the prelut range to be an acceptable range", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisPrelutRangeMinStart,                                 config.analysisPrelutRangeMinStart,                                 "Minimal value for the minimum of the prelut range", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisPrelutRangeMinStop,                                  config.analysisPrelutRangeMinStop,                                  "Maximal value for the minimum of the prelut range", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisPrelutRangeMinSteps,                                 config.analysisPrelutRangeMinSteps,                                 "Stepwidth for the minimum prelut range", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisPrelutRangeMaxStart,                                 config.analysisPrelutRangeMaxStart,                                 "Minimal value for the maximum of the prelut range", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisPrelutRangeMaxStop,                                  config.analysisPrelutRangeMaxStop,                                  "Maximal value for the maximum of the prelut range", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisPrelutRangeMaxSteps,                                 config.analysisPrelutRangeMaxSteps,                                 "Stepwidth for the maximum prelut range", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisChooseMainPrelutRange,                               config.analysisChooseMainPrelutRange,                               "Choose range of main or correct layer analysis", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisChooseConstraintPrelutRange,                         config.analysisChooseConstraintPrelutRange,                         "Choose range with regard to constraint or just maximum", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitTotalAnalysis,                                   config.analysisInitTotalAnalysis,                                   "Initialize the total analysis package", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitPercentageOfHitsInSignature,                     config.analysisInitPercentageOfHitsInSignature,                     "Initialize the percentage of the number of hits which must be found to accept the signature", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitPercentageOfTracksForSignature,                  config.analysisInitPercentageOfTracksForSignature,                  "Initialize the percentage of the real tracks which must be found with the accepted signatures", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitAnalysisResultWarnings,                          config.analysisInitAnalysisResultWarnings,                          "Initialize the printing of the special analysis results to standard output", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitAnalysisResultDisplays,                          config.analysisInitAnalysisResultDisplays,                          "Initialize the displays of the special analysis results in graphical mode", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitAnalysisMoreResultWarnings,                      config.analysisInitAnalysisMoreResultWarnings,                      "Initialize the printing of the special analysis results to standard output", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisInitAnalysisMoreResultDisplays,                      config.analysisInitAnalysisMoreResultDisplays,                      "Initialize the displays of the special analysis results in graphical mode", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisWriteCellFiles,                                      config.analysisWriteCellFiles,                                      "Enables the production of the neccessary files for the CELL BE implementation simulation", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisHitCellFileName,                                     config.analysisHitCellFileName,                                     "Name of the file containing the hits in a format which is used in the CELL BE simulation", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisPrelutCellFileName,                                  config.analysisPrelutCellFileName,                                  "Name of the file containing the prelut in a format which is used in the CELL BE simulation", statusSequence);
	setHeaderValue(fileStream, stringCmdAnalysisLutCellFileName,                                     config.analysisLutCellFileName,                                     "Name of the file containing the lut in a format which is used in the CELL BE simulation", statusSequence);

	writeComment(fileStream, "");
	writeComment(fileStream, "[GLOBAL PARAMETERS]");

	setHeaderValue(fileStream, stringCmdInitStatus,                                                  config.initStatus,                                                  "Initialize the status to be shown", statusSequence);
/********************************************************/

}

/****************************************************************
 * This method returns a reference of the data struct.			*
 ****************************************************************/

InfData& inf::getDataReference() {

	return config;

}

/****************************************************************
 * This method returns an information string about the object.	*
 ****************************************************************/

std::string inf::getInfo() {

	std::string message;
	char buffer[max(max(intConversionDigits, doubleConversionDigits), 6)+1]; /* 6 = sizeof(false+'/0')*/

/********************************************************/
/* make code changes for a different information here	*/
	message += "Setup for the Hough tracking:\n";
	message += "------------------------------------\n";
	message += "CommandID";
	message += "\t\t: ";
	for (unsigned int i = 0; i < getNumberOfCmds(); i++) {
		btods(isHeaderLockSet(i), buffer);
		message += buffer;
	}
	message += "\n\n";
	message += "[PARAMETERS FOR THE INPUT OBJECT]";
	message += "\n";
	message += stringCmdInputFileName;
	message += "\t\t\t\t: ";
	message += config.inputFileName;
	message += "\n";
	message += stringCmdInputFileFormat;
	message += "\t\t\t\t: ";
	itos(config.inputFileFormat, buffer, 10, intConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdInputMinTracks;
	message += "\t\t\t\t: ";
	itos(config.inputMinTracks, buffer, 10, intConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdInputMaxTracks;
	message += "\t\t\t\t: ";
	itos(config.inputMaxTracks, buffer, 10, intConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdInputDetectorMask;
	message += "\t\t\t: ";
	message += config.inputDetectorMask.toString();
	message += "\n";
	message += stringCmdInputFileNameMagneticField;
	message += "\t\t: ";
	message += config.inputFileNameMagneticField;
	message += "\n";
	message += stringCmdInputMagneticFieldIsRootFile;
	message += "\t\t: ";
	btobs(config.inputMagneticFieldIsRootFile, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdInputMapNameMagneticField;
	message += "\t\t: ";
	message += config.inputMapNameMagneticField;
	message += "\n";
	message += stringCmdInputMagneticFieldIntegrationStepwidthPerStation;
	message += "\t: ";
	ustos(config.inputMagneticFieldIntegrationStepwidthPerStation, buffer, 10, intConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdInputMagneticFieldIntegrationFactor;
	message += "\t: ";
	dtos(config.inputMagneticFieldIntegrationFactor, buffer, doubleConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdInputDisableAutomaticMagneticField;
	message += "\t\t: ";
	message += config.inputDisableAutomaticMagneticField;
	message += "\n";
	message += stringCmdInputFileNameDetector;
	message += "\t\t\t: ";
	message += config.inputFileNameDetector;
	message += "\n";
	message += stringCmdInputNumberOfVolumesInfrontOfSTS;
	message += "\t: ";
	itos(config.inputNumberOfVolumesInfrontOfSTS, buffer, 10, intConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdInputDisableAutomaticDetector;
	message += "\t\t: ";
	message += config.inputDisableAutomaticDetector;
	message += "\n";
	message += stringCmdInputCodingTableMode;
	message += "\t\t\t: ";
	itos(config.inputCodingTableMode, buffer, 10, intConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdInputGradingPTableMode;
	message += "\t\t\t: ";
	itos(config.inputGradingPTableMode, buffer, 10, intConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdInputGradingRTableMode;
	message += "\t\t\t: ";
	itos(config.inputGradingRTableMode, buffer, 10, intConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdInputCodingTableFileName;
	message += "\t\t: ";
	message += config.inputCodingTableFileName;
	message += "\n";
	message += stringCmdInputGradingPTableFileName;
	message += "\t\t: ";
	message += config.inputGradingPTableFileName;
	message += "\n";
	message += stringCmdInputGradingRTableFileName;
	message += "\t\t: ";
	message += config.inputGradingRTableFileName;
	message += "\n";
	message += stringCmdInputCodingTableWrite;
	message += "\t\t\t: ";
	btobs(config.inputCodingTableWrite, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdInputGradingPTableWrite;
	message += "\t\t\t: ";
	btobs(config.inputGradingPTableWrite, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdInputGradingRTableWrite;
	message += "\t\t\t: ";
	btobs(config.inputGradingRTableWrite, buffer);
	message += buffer;
	message += "\n\n";
	message += "[PARAMETERS FOR THE TRACKFINDER OBJECT]";
	message += "\n";
	message += stringCmdTrackfinderHitProducer;
	message += "\t\t\t: ";
	itos(config.trackfinderHitProducer, buffer, 10, intConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderReadPointsFromFile;
	message += "\t\t: ";
	btobs(config.trackfinderReadPointsFromFile, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderReadHitsFromFile;
	message += "\t\t: ";
	btobs(config.trackfinderReadHitsFromFile, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderReadMapsHits;
	message += "\t\t\t: ";
	btobs(config.trackfinderReadMapsHits, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderReadHybridHits;
	message += "\t\t: ";
	btobs(config.trackfinderReadHybridHits, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderReadStripHits;
	message += "\t\t: ";
	btobs(config.trackfinderReadStripHits, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderLutsVersion;
	message += "\t\t\t: ";
	itos(config.trackfinderLutsVersion, buffer, 10, intConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderPrelutFileName;
	message += "\t\t: ";
	message += config.trackfinderPrelutFileName;
	message += "\n";
	message += stringCmdTrackfinderLutFileName;
	message += "\t\t\t: ";
	message += config.trackfinderLutFileName;
	message += "\n";
	message += stringCmdTrackfinderGammaMin;
	message += "\t\t\t: ";
	dtos(config.trackfinderGammaMin, buffer, doubleConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderGammaMax;
	message += "\t\t\t: ";
	dtos(config.trackfinderThetaMax, buffer, doubleConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderGammaStep;
	message += "\t\t\t: ";
	itos(config.trackfinderGammaStep, buffer, 10, intConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderThetaMin;
	message += "\t\t\t: ";
	dtos(config.trackfinderThetaMin, buffer, doubleConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderThetaMax;
	message += "\t\t\t: ";
	dtos(config.trackfinderThetaMax, buffer, doubleConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderThetaStep;
	message += "\t\t\t: ";
	itos(config.trackfinderThetaStep, buffer, 10, intConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderPrelutRadiusMin;
	message += "\t\t: ";
	dtos(config.trackfinderPrelutRadiusMin, buffer, doubleConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderPrelutRadiusMax;
	message += "\t\t: ";
	dtos(config.trackfinderPrelutRadiusMax, buffer, doubleConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderLutRadiusMin;
	message += "\t\t\t: ";
	dtos(config.trackfinderLutRadiusMin, buffer, doubleConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderLutRadiusMax;
	message += "\t\t\t: ";
	dtos(config.trackfinderLutRadiusMax, buffer, doubleConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderLutRadiusStep;
	message += "\t\t: ";
	itos(config.trackfinderLutRadiusStep, buffer, 10, intConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderMinClassCoding;
	message += "\t\t\t: ";
	ustos(config.trackfinderMinClassCoding, buffer, 10, shortConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderMinClassGradingP;
	message += "\t\t\t: ";
	ustos(config.trackfinderMinClassGradingP, buffer, 10, shortConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderMinClassGradingR;
	message += "\t\t\t: ";
	ustos(config.trackfinderMinClassGradingR, buffer, 10, shortConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderFilterType;
	message += "\t\t\t: ";
	ustos(config.trackfinderFilterType, buffer, 10, shortConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderFirstFilterGeometry;
	message += "\t\t\t: ";
	ustos(config.trackfinderFirstFilterGeometry, buffer, 10, shortConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderFirstFilterArithmetic;
	message += "\t\t\t: ";
	ustos(config.trackfinderFirstFilterArithmetic, buffer, 10, shortConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderSecondFilterGeometry;
	message += "\t\t\t: ";
	ustos(config.trackfinderSecondFilterGeometry, buffer, 10, shortConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderSecondFilterArithmetic;
	message += "\t\t\t: ";
	ustos(config.trackfinderSecondFilterArithmetic, buffer, 10, shortConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderFirstFilterNeighborhoodDim1ClearRadius;
	message += "\t: ";
	ustos(config.trackfinderFirstFilterNeighborhoodDim1ClearRadius, buffer, 10, shortConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderFirstFilterNeighborhoodDim2ClearRadius;
	message += "\t: ";
	ustos(config.trackfinderFirstFilterNeighborhoodDim2ClearRadius, buffer, 10, shortConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderSecondFilterNeighborhoodDim1ClearRadius;
	message += "\t: ";
	ustos(config.trackfinderSecondFilterNeighborhoodDim1ClearRadius, buffer, 10, shortConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderSecondFilterNeighborhoodDim2ClearRadius;
	message += "\t: ";
	ustos(config.trackfinderSecondFilterNeighborhoodDim2ClearRadius, buffer, 10, shortConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderSecondFilterNeighborhoodDim3ClearRadius;
	message += "\t: ";
	ustos(config.trackfinderSecondFilterNeighborhoodDim3ClearRadius, buffer, 10, shortConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderAutomaticFilterCoverPercentage;
	message += "\t: ";
	ustos(config.trackfinderAutomaticFilterCoverPercentage, buffer, 10, shortConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderAutomaticFilterDataPercentage;
	message += "\t: ";
	ustos(config.trackfinderAutomaticFilterDataPercentage, buffer, 10, shortConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderAutomaticFilterWrite;
	message += "\t\t: ";
	btobs(config.trackfinderAutomaticFilterWrite, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdTrackfinderAutomaticFilterFileName;
	message += "\t\t: ";
	message += config.trackfinderAutomaticFilterFileName;
	message += "\n";
	message += stringCmdTrackfinderWriteTracksToFile;
	message += "\t\t: ";
	btobs(config.trackfinderWriteTracksToFile, buffer);
	message += buffer;
	message += "\n\n";
	message += "[PARAMETERS FOR THE ANALYSIS OBJECT]";
	message += "\n";
	message += stringCmdAnalysisOutputFileName;
	message += "\t\t: ";
	message += config.analysisOutputFileName;
	message += "\n";
	message += stringCmdAnalysisThresholdForP;
	message += "\t\t\t\t: ";
	dtos(config.analysisThresholdForP, buffer, doubleConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitConfiguration;
	message += "\t\t\t: ";
	btobs(config.analysisInitConfiguration, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitDetector;
	message += "\t\t\t\t: ";
	btobs(config.analysisInitDetector, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitEvent;
	message += "\t\t\t\t: ";
	btobs(config.analysisInitEvent, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitMemory;
	message += "\t\t\t\t: ";
	btobs(config.analysisInitMemory, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitTime;
	message += "\t\t\t\t: ";
	btobs(config.analysisInitTime, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitTrackPropagationEventPoint;
	message += "\t\t\t: ";
	btobs(config.analysisInitTrackPropagationEventPoint, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitTrackPropagationEventHit;
	message += "\t\t\t: ";
	btobs(config.analysisInitTrackPropagationEventHit, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitTrackPropagationTotalPoint;
	message += "\t\t\t: ";
	btobs(config.analysisInitTrackPropagationTotalPoint, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitTrackPropagationTotalHit;
	message += "\t\t\t: ";
	btobs(config.analysisInitTrackPropagationTotalHit, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitTrackPropagationDisplay;
	message += "\t\t\t: ";
	btobs(config.analysisInitTrackPropagationDisplay, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitTrackPropagationToRoot;
	message += "\t\t\t: ";
	btobs(config.analysisInitTrackPropagationToRoot, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisTrackPropagationPointDisplayMask;
	message += "\t\t: ";
	message += config.analysisTrackPropagationPointDisplayMask.toString();
	message += "\n";
	message += stringCmdAnalysisTrackPropagationHitDisplayMask;
	message += "\t\t: ";
	message += config.analysisTrackPropagationHitDisplayMask.toString();
	message += "\n";
	message += stringCmdAnalysisInitClassPriority;
	message += "\t\t\t: ";
	btobs(config.analysisInitClassPriority, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitQualityEFGCEventAbsolute;
	message += "\t\t: ";
	btobs(config.analysisInitQualityEFGCEventAbsolute, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitQualityEFGCEventRelative;
	message += "\t\t: ";
	btobs(config.analysisInitQualityEFGCEventRelative, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitQualityEFGCTotalAbsolute;
	message += "\t\t: ";
	btobs(config.analysisInitQualityEFGCTotalAbsolute, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitQualityEFGCTotalRelative;
	message += "\t\t: ";
	btobs(config.analysisInitQualityEFGCTotalRelative, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitMomentumEFGCEventPzEFGC;
	message += "\t\t: ";
	btobs(config.analysisInitMomentumEFGCEventPzEFGC, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitMomentumEFGCEventPtEFGC;
	message += "\t\t: ";
	btobs(config.analysisInitMomentumEFGCEventPtEFGC, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitMomentumEFGCTotalPzEFGC;
	message += "\t\t: ";
	btobs(config.analysisInitMomentumEFGCTotalPzEFGC, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitMomentumEFGCTotalPtEFGC;
	message += "\t\t: ";
	btobs(config.analysisInitMomentumEFGCTotalPtEFGC, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitProjectionEFGCNEvent12EFGCN;
	message += "\t\t: ";
	btobs(config.analysisInitProjectionEFGCNEvent12EFGCN, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitProjectionEFGCNEvent13EFGCN;
	message += "\t\t: ";
	btobs(config.analysisInitProjectionEFGCNEvent13EFGCN, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitProjectionEFGCNEvent32EFGCN;
	message += "\t\t: ";
	btobs(config.analysisInitProjectionEFGCNEvent32EFGCN, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitProjectionEFGCNTotal12EFGCN;
	message += "\t\t: ";
	btobs(config.analysisInitProjectionEFGCNTotal12EFGCN, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitProjectionEFGCNTotal13EFGCN;
	message += "\t\t: ";
	btobs(config.analysisInitProjectionEFGCNTotal13EFGCN, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitProjectionEFGCNTotal32EFGCN;
	message += "\t\t: ";
	btobs(config.analysisInitProjectionEFGCNTotal32EFGCN, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitMomentumEvent;
	message += "\t\t\t: ";
	btobs(config.analysisInitMomentumEvent, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitMomentumTotal;
	message += "\t\t\t: ";
	btobs(config.analysisInitMomentumTotal, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitMomentumDisplay;
	message += "\t\t\t: ";
	btobs(config.analysisInitMomentumDisplay, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitMomentumToRoot;
	message += "\t\t\t: ";
	btobs(config.analysisInitMomentumToRoot, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitProjectionEvent;
	message += "\t\t\t: ";
	btobs(config.analysisInitProjectionEvent, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitProjectionTotal;
	message += "\t\t\t: ";
	btobs(config.analysisInitProjectionTotal, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitProjectionDisplay;
	message += "\t\t\t: ";
	btobs(config.analysisInitProjectionDisplay, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitProjectionToRoot;
	message += "\t\t\t: ";
	btobs(config.analysisInitProjectionToRoot, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitMagnetfieldX;
	message += "\t\t\t: ";
	btobs(config.analysisInitMagnetfieldX, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitMagnetfieldY;
	message += "\t\t\t: ";
	btobs(config.analysisInitMagnetfieldY, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitMagnetfieldZ;
	message += "\t\t\t: ";
	btobs(config.analysisInitMagnetfieldZ, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitMagnetfieldDisplay;
	message += "\t\t\t: ";
	btobs(config.analysisInitMagnetfieldDisplay, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitMagnetfieldToRoot;
	message += "\t\t\t: ";
	btobs(config.analysisInitMagnetfieldToRoot, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitMagnetfieldConstantForEachEvent;
	message += "\t: ";
	btobs(config.analysisInitMagnetfieldConstantForEachEvent, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitWeightedMagnetfieldConstant;
	message += "\t\t: ";
	btobs(config.analysisInitWeightedMagnetfieldConstant, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitMagnetfieldConstantDisplay;
	message += "\t\t: ";
	btobs(config.analysisInitMagnetfieldConstantDisplay, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisMagnetfieldConstantDisplayMask;
	message += "\t\t: ";
	message += config.analysisMagnetfieldConstantDisplayMask.toString();
	message += "\n";
	message += stringCmdAnalysisInitMagnetfieldConstantToRoot;
	message += "\t\t: ";
	btobs(config.analysisInitMagnetfieldConstantToRoot, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitMagnetfieldVSConstants;
	message += "\t\t: ";
	btobs(config.analysisInitMagnetfieldVSConstants, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitPrelutGoodness;
	message += "\t\t\t: ";
	btobs(config.analysisInitPrelutGoodness, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitLutGoodness;
	message += "\t\t\t\t: ";
	btobs(config.analysisInitLutGoodness, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitHoughTransformGoodness;
	message += "\t\t: ";
	btobs(config.analysisInitHoughTransformGoodness, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitQuantizationGoodness;
	message += "\t\t: ";
	btobs(config.analysisInitQuantizationGoodness, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitPeakDistanceGoodness;
	message += "\t\t: ";
	btobs(config.analysisInitPeakDistanceGoodness, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitCreatedHistogramToRoot;
	message += "\t\t: ";
	btobs(config.analysisInitCreatedHistogramToRoot, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitEncodedHistogramToRoot;
	message += "\t\t: ";
	btobs(config.analysisInitEncodedHistogramToRoot, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitFilteredHistogramToRoot;
	message += "\t\t: ";
	btobs(config.analysisInitFilteredHistogramToRoot, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitJustOneCreatedHistogramToRoot;
	message += "\t: ";
	btobs(config.analysisInitJustOneCreatedHistogramToRoot, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitJustOneEncodedHistogramToRoot;
	message += "\t: ";
	btobs(config.analysisInitJustOneEncodedHistogramToRoot, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitJustOneFilteredHistogramToRoot;
	message += "\t: ";
	btobs(config.analysisInitJustOneFilteredHistogramToRoot, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitCreatedHistogramToShow;
	message += "\t\t: ";
	btobs(config.analysisInitCreatedHistogramToShow, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitEncodedHistogramToShow;
	message += "\t\t: ";
	btobs(config.analysisInitEncodedHistogramToShow, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitFilteredHistogramToShow;
	message += "\t\t: ";
	btobs(config.analysisInitFilteredHistogramToShow, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitHistogramLayer;
	message += "\t\t\t: ";
	ustos(config.analysisInitHistogramLayer, buffer, 10, shortConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitNumberOfTracksPerColumn;
	message += "\t\t: ";
	btobs(config.analysisInitNumberOfTracksPerColumn, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitNumberOfTracksPerRow;
	message += "\t\t: ";
	btobs(config.analysisInitNumberOfTracksPerRow, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitNumberOfTracksPerLayer;
	message += "\t\t: ";
	btobs(config.analysisInitNumberOfTracksPerLayer, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitHitReadoutDistribution;
	message += "\t\t\t: ";
	btobs(config.analysisInitHitReadoutDistribution, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitReadoutColumnsInParallel;
	message += "\t: ";
	btobs(config.analysisInitReadoutColumnsInParallel, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitPrelutRangeForEachEvent;
	message += "\t\t\t: ";
	btobs(config.analysisInitPrelutRangeForEachEvent, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitWeightedPrelutRange;
	message += "\t\t\t: ";
	btobs(config.analysisInitWeightedPrelutRange, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitPrelutRangeDisplay;
	message += "\t\t\t: ";
	btobs(config.analysisInitPrelutRangeDisplay, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitPrelutRangeDisplayMode;
	message += "\t: ";
	ustos(config.analysisInitPrelutRangeDisplayMode, buffer, 10, shortConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisPrelutRangeStationDisplayMask;
	message += "\t\t: ";
	message += config.analysisPrelutRangeStationDisplayMask.toString();
	message += "\n";
	message += stringCmdAnalysisPrelutRangeStationSumDisplayMask;
	message += "\t\t\t: ";
	btobs(config.analysisPrelutRangeStationSumDisplayMask, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisPrelutRangeConstraintDisplayMask;
	message += "\t\t: ";
	message += config.analysisPrelutRangeConstraintDisplayMask.toString();
	message += "\n";
	message += stringCmdAnalysisPrelutRangeConstraintSumDisplayMask;
	message += "\t\t\t: ";
	btobs(config.analysisPrelutRangeConstraintSumDisplayMask, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisPrelutRangeRelativeDisplayMask;
	message += "\t\t\t: ";
	btobs(config.analysisPrelutRangeRelativeDisplayMask, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitPrelutRangeToRoot;
	message += "\t\t\t: ";
	btobs(config.analysisInitPrelutRangeToRoot, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitPercentageOfHitsForPrelutRange;
	message += "\t: ";
	ustos(config.analysisInitPercentageOfHitsForPrelutRange, buffer, 10, shortConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisPrelutRangeMinStart;
	message += "\t: ";
	dtos(config.analysisPrelutRangeMinStart, buffer, doubleConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisPrelutRangeMinStop;
	message += "\t: ";
	dtos(config.analysisPrelutRangeMinStop, buffer, doubleConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisPrelutRangeMinSteps;
	message += "\t: ";
	ustos(config.analysisPrelutRangeMinSteps, buffer, 10, shortConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisPrelutRangeMaxStart;
	message += "\t: ";
	dtos(config.analysisPrelutRangeMaxStart, buffer, doubleConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisPrelutRangeMaxStop;
	message += "\t: ";
	dtos(config.analysisPrelutRangeMaxStop, buffer, doubleConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisPrelutRangeMaxSteps;
	message += "\t: ";
	ustos(config.analysisPrelutRangeMaxSteps, buffer, 10, shortConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisChooseMainPrelutRange;
	message += "\t\t\t: ";
	btobs(config.analysisChooseMainPrelutRange, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisChooseConstraintPrelutRange;
	message += "\t\t\t: ";
	btobs(config.analysisChooseConstraintPrelutRange, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitTotalAnalysis;
	message += "\t\t\t: ";
	btobs(config.analysisInitTotalAnalysis, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitPercentageOfHitsInSignature;
	message += "\t\t: ";
	ustos(config.analysisInitPercentageOfHitsInSignature, buffer, 10, shortConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitPercentageOfTracksForSignature;
	message += "\t: ";
	ustos(config.analysisInitPercentageOfTracksForSignature, buffer, 10, shortConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitAnalysisResultWarnings;
	message += "\t\t: ";
	ultos(config.analysisInitAnalysisResultWarnings, buffer, 10, longConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitAnalysisResultDisplays;
	message += "\t\t: ";
	ultos(config.analysisInitAnalysisResultDisplays, buffer, 10, longConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitAnalysisMoreResultWarnings;
	message += "\t\t: ";
	ultos(config.analysisInitAnalysisMoreResultWarnings, buffer, 10, longConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisInitAnalysisMoreResultDisplays;
	message += "\t\t: ";
	ultos(config.analysisInitAnalysisMoreResultDisplays, buffer, 10, longConversionDigits);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisWriteCellFiles;
	message += "\t\t: ";
	btobs(config.analysisWriteCellFiles, buffer);
	message += buffer;
	message += "\n";
	message += stringCmdAnalysisHitCellFileName;
	message += "\t\t: ";
	message += config.analysisHitCellFileName;
	message += "\n";
	message += stringCmdAnalysisPrelutCellFileName;
	message += "\t\t: ";
	message += config.analysisPrelutCellFileName;
	message += "\n";
	message += stringCmdAnalysisLutCellFileName;
	message += "\t\t: ";
	message += config.analysisLutCellFileName;
	message += "\n\n";
	message += "[GLOBAL PARAMETERS]";
	message += "\n";
	message += stringCmdInitStatus;
	message += "\t\t\t\t\t: ";
	btobs(config.initStatus, buffer);
	message += buffer;
/********************************************************/

	return message;

}
