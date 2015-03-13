# Create a library called "libEcal" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(INCLUDE_DIRECTORIES
${CBMROOT_SOURCE_DIR}/cbmbase

${CBMROOT_SOURCE_DIR}/cbmdata
${CBMROOT_SOURCE_DIR}/cbmdata/sts 
${CBMROOT_SOURCE_DIR}/cbmdata/ecal 
${CBMROOT_SOURCE_DIR}/cbmdata/global

${CBMROOT_SOURCE_DIR}/KF/Interface
${CBMROOT_SOURCE_DIR}/KF

${CBMROOT_SOURCE_DIR}/ecal
)

Include_Directories( ${INCLUDE_DIRECTORIES})

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${BASE_INCLUDE_DIRECTORIES} 
)

Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
${Boost_LIBRARY_DIRS}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
CbmEcal.cxx CbmEcalDetailed.cxx 	
CbmEcalHitProducer.cxx 
CbmGeoEcalPar.cxx CbmEcalDrawer.cxx 
CbmEcalContFact.cxx
CbmEcalAnalysisDS.cxx CbmEcalAnalysis.cxx 
CbmEcalModule.cxx CbmEcalStructure.cxx 
CbmEcalHitProducerFastMC.cxx  CbmEcalCellMC.cxx
CbmEcalReconstructionFastMC.cxx
CbmEcalPhotonCalibrator.cxx CbmEcalShowerCalibrator.cxx 
CbmEcalUrqmdCalibrator.cxx CbmEcalSCurveLib.cxx 
CbmEcalSCurveLibRecord.cxx CbmEcalOShowerCalibrator.cxx 
CbmEcalTrackExtrapolation.cxx
CbmEcalTracking.cxx CbmEcalStructureFiller.cxx
CbmEcalTrackExport.cxx CbmEcalStructureExport.cxx
CbmEcalQualityCheck.cxx CbmEcalClusterFinderV2.cxx
CbmEcalTrackImport.cxx CbmEcalCalibration.cxx
CbmEcalParam.cxx CbmEcalMatching.cxx
CbmEcalAnalysisPi0.cxx CbmEcalAnalysisPair.cxx CbmEcalAnalysisMaterial.cxx
CbmEcalAnalysisNeutron.cxx CbmEcalTimeDigitizer.cxx
CbmEcalLightMap.cxx CbmEcalDrawer2.cxx
CbmEcalAnalysisCluster.cxx CbmEcalAnalysisUniformity.cxx
CbmEcalAnalysisDst.cxx CbmEcalAnalysisBSh.cxx
CbmEcalCalibrationV2.cxx CbmEcalAnalysisSimple.cxx CbmEcalIdentification.cxx
CbmEcalMaximumLocator.cxx CbmEcalAnalysisJPsi.cxx
CbmEcalAnalysisSShape.cxx CbmEcalAnalysisShape.cxx
CbmEcalShLibRecord.cxx CbmEcalShLib.cxx CbmEcalShLibTable.cxx
CbmEcalClusterFinder.cxx 
CbmEcalReco.cxx 
CbmEcalShLibNN.cxx
CbmEcalAnalysisEres.cxx CbmEcalPosLib.cxx 
CbmEcalReco2.cxx
CbmEcalShLibCorr.cxx CbmEcalShLibCorrTable.cxx
CbmEcalShLibCorrRecord.cxx 
CbmEcalRecoCorr.cxx
CbmEcalDigi.cxx CbmEcalPrepare.cxx CbmEcalAnalysisCalib.cxx
CbmEcalInf.cxx
)

set(LINKDEF EcalLinkDef.h)
Set(LIBRARY_NAME Ecal)
Set(DEPENDENCIES
    #KF ????
    CbmBase CbmData Base MLP ASImage Minuit2
)

GENERATE_LIBRARY()
