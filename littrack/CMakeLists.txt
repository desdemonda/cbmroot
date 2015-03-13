# Create a library called "libLittrack".

add_subdirectory(clustering)
add_subdirectory(parallel)

INCLUDE(std/CMakeLists.txt)

set(INCLUDE_DIRECTORIES
${CBMROOT_SOURCE_DIR}/littrack
${CBMROOT_SOURCE_DIR}/littrack/std
${CBMROOT_SOURCE_DIR}/littrack/cbm

${CBMROOT_SOURCE_DIR}/cbmbase
${CBMROOT_SOURCE_DIR}/cbmbase/report
${CBMROOT_SOURCE_DIR}/cbmbase/draw
${CBMROOT_SOURCE_DIR}/cbmbase/utils

${CBMROOT_SOURCE_DIR}/cbmdata
${CBMROOT_SOURCE_DIR}/cbmdata/much
${CBMROOT_SOURCE_DIR}/cbmdata/trd
${CBMROOT_SOURCE_DIR}/cbmdata/sts
${CBMROOT_SOURCE_DIR}/cbmdata/tof
${CBMROOT_SOURCE_DIR}/cbmdata/global
${CBMROOT_SOURCE_DIR}/cbmdata/mvd
${CBMROOT_SOURCE_DIR}/cbmdata/rich

${CBMROOT_SOURCE_DIR}/much/geo

${CBMROOT_SOURCE_DIR}/sts
${CBMROOT_SOURCE_DIR}/sts/setup

${CBMROOT_SOURCE_DIR}/rich

${CBMROOT_SOURCE_DIR}/KF
${CBMROOT_SOURCE_DIR}/KF/Interface

${CBMROOT_SOURCE_DIR}/trd/data
${CBMROOT_SOURCE_DIR}/trd/param
${CBMROOT_SOURCE_DIR}/trd/tools

)

Include_Directories( ${INCLUDE_DIRECTORIES})

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${BASE_INCLUDE_DIRECTORIES}
  ${Boost_INCLUDE_DIR}
)

Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
${Boost_LIBRARY_DIRS}
)

link_directories( ${LINK_DIRECTORIES})

set(SRCS

   cbm/base/CbmLitDetectorSetup.cxx
   
   cbm/elid/CbmLitGlobalElectronId.cxx
   
   cbm/reco/CbmLitFindMvdTracks.cxx
   cbm/reco/CbmLitFitTracksParallel.cxx
   cbm/reco/CbmLitFitTracks.cxx
   
   cbm/generators/CbmLitPolarizedGenerator.cxx
   
   cbm/qa/base/CbmLitResultChecker.cxx  
   cbm/qa/material/CbmLitCheckEnergyLossMuons.cxx
   cbm/qa/material/CbmLitCheckBrem.cxx 
   cbm/qa/mc/CbmLitMCTrack.cxx
   cbm/qa/mc/CbmLitMCTrackCreator.cxx
   cbm/qa/CbmLitCreateStandaloneData.cxx
   cbm/qa/CbmLitTestMatrixMath.cxx
   ${LITTRACKSTD_SRCS}
   cbm/base/CbmLitFieldGridCreator.cxx
   cbm/base/CbmLitFieldFitter.cxx
   cbm/base/CbmLitToolFactory.cxx
   cbm/base/CbmLitTrackingGeometryConstructor.cxx
   cbm/reco/CbmLitFindSimpleTrdClusters.cxx 
   cbm/reco/CbmLitFindLWClusters.cxx 
   cbm/reco/CbmLitLWClusterManager.cxx 
   cbm/reco/CbmLitFindGlobalTracks.cxx 
   cbm/reco/CbmLitFindGlobalTracksIdeal.cxx 
   cbm/reco/CbmLitFindGlobalTracksParallel.cxx
#   cbm/utils/CbmLitClusterDistance.cxx
   cbm/utils/CbmLitConverter.cxx
   cbm/utils/CbmLitConverterParallel.cxx
   cbm/qa/tracking/CbmLitTrackingQa.cxx
   cbm/qa/tracking/CbmLitTrackingQaStudyReport.cxx            
   cbm/qa/tracking/CbmLitTrackingQaReport.cxx
   cbm/qa/clustering/CbmLitClusteringQa.cxx
   cbm/qa/clustering/CbmLitClusteringQaTrd.cxx
   cbm/qa/clustering/CbmLitClusteringQaReport.cxx
   cbm/qa/clustering/CbmLitClusteringQaTrdReport.cxx
   cbm/qa/clustering/CbmLitClusteringQaStudyReport.cxx
   cbm/qa/fit/CbmLitFitQa.cxx
   cbm/qa/fit/CbmLitFitQaReport.cxx
   cbm/qa/fit/CbmLitFitQaStudyReport.cxx
   cbm/qa/field/CbmLitFieldQa.cxx
   cbm/qa/field/CbmLitFieldQaReport.cxx
   cbm/qa/fieldapr/CbmLitFieldApproximationQa.cxx
   cbm/qa/fieldapr/CbmLitFieldApproximationQaReport.cxx
   cbm/qa/radlength/CbmLitRadLengthQa.cxx
   cbm/qa/radlength/CbmLitRadLengthQaReport.cxx
   cbm/qa/radlength/CbmLitRadLengthGenerator.cxx
   cbm/qa/tof/CbmLitTofQa.cxx
   cbm/qa/tof/CbmLitTofQaReport.cxx
)

set(HEADERS
#   cbm/utils/CbmLitClusterDistance.h
   cbm/elid/CbmLitGlobalElectronId.h
   cbm/reco/CbmLitFindGlobalTracks.h
   cbm/reco/CbmLitFindMvdTracks.h
   cbm/reco/CbmLitFindGlobalTracksIdeal.h
   cbm/reco/CbmLitFitTracksParallel.h
   cbm/reco/CbmLitFitTracks.h
   cbm/reco/CbmLitFindGlobalTracksParallel.h
   cbm/reco/CbmLitFindSimpleTrdClusters.h
   cbm/reco/CbmLitFindLWClusters.h
   cbm/reco/CbmLitLWClusterManager.h
   cbm/generators/CbmLitPolarizedGenerator.h
   cbm/qa/material/CbmLitCheckEnergyLossMuons.h 
   cbm/qa/material/CbmLitCheckBrem.h 
   cbm/qa/CbmLitCreateStandaloneData.h
   cbm/qa/CbmLitTestMatrixMath.h
   cbm/qa/tracking/CbmLitTrackingQa.h
   cbm/qa/tracking/CbmLitTrackingQaStudyReport.h
   cbm/qa/tracking/CbmLitTrackingQaReport.h
   cbm/qa/clustering/CbmLitClusteringQa.h
   cbm/qa/clustering/CbmLitClusteringQaTrd.h
   cbm/qa/clustering/CbmLitClusteringQaTrdReport.h
   cbm/qa/clustering/CbmLitClusteringQaReport.h
   cbm/qa/clustering/CbmLitClusteringQaStudyReport.h
   cbm/qa/fit/CbmLitFitQa.h 
   cbm/qa/fit/CbmLitFitQaReport.h
   cbm/qa/fit/CbmLitFitQaStudyReport.h
   cbm/qa/field/CbmLitFieldQa.h
   cbm/qa/field/CbmLitFieldQaReport.h
   cbm/qa/fieldapr/CbmLitFieldApproximationQa.h
   cbm/qa/fieldapr/CbmLitFieldApproximationQaReport.h
   cbm/qa/radlength/CbmLitRadLengthQa.h
   cbm/qa/radlength/CbmLitRadLengthQaReport.h
   cbm/qa/radlength/CbmLitRadLengthGenerator.h
   cbm/qa/tof/CbmLitTofQa.h
   cbm/qa/tof/CbmLitTofQaReport.h
)




IF (SSE_FOUND)
  ADD_DEFINITIONS(-DHAVE_SSE)
  SET_SOURCE_FILES_PROPERTIES(${SRCS} PROPERTIES COMPILE_FLAGS "-msse -O3 -w")
  Message(STATUS "LitTrack will be compiled with SSE support")
ELSE (SSE_FOUND)
  Message(STATUS "LitTrack will be compiled without SSE support")
  SET_SOURCE_FILES_PROPERTIES(${SRCS} PROPERTIES COMPILE_FLAGS "-O3 -w")
ENDIF (SSE_FOUND)

If (Boost_FOUND)
  set(DEFINITIONS BOOST_FOUND)
EndIf (Boost_FOUND)

set(LINKDEF  LittrackLinkDef.h)
Set(LIBRARY_NAME Littrack)
Set(DEPENDENCIES
    Littrackparallel KF Much Sts Rich CbmBase CbmData Base 
)

GENERATE_LIBRARY()

