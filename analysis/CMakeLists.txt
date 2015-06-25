# add subdirectories
add_subdirectory (papa)


Set(INCLUDE_DIRECTORIES
${CBMROOT_SOURCE_DIR}/analysis
#${CBMROOT_SOURCE_DIR}/analysis/hyperon

${CBMROOT_SOURCE_DIR}/cbmbase 
${CBMROOT_SOURCE_DIR}/cbmbase/draw
${CBMROOT_SOURCE_DIR}/cbmbase/report
${CBMROOT_SOURCE_DIR}/cbmbase/utils

${CBMROOT_SOURCE_DIR}/cbmdata
${CBMROOT_SOURCE_DIR}/cbmdata/sts
${CBMROOT_SOURCE_DIR}/cbmdata/much
${CBMROOT_SOURCE_DIR}/cbmdata/mvd
${CBMROOT_SOURCE_DIR}/cbmdata/global
${CBMROOT_SOURCE_DIR}/cbmdata/rich
${CBMROOT_SOURCE_DIR}/cbmdata/trd
${CBMROOT_SOURCE_DIR}/cbmdata/tof
${CBMROOT_SOURCE_DIR}/cbmdata/psd

${CBMROOT_SOURCE_DIR}/sts 

${CBMROOT_SOURCE_DIR}/KF
${CBMROOT_SOURCE_DIR}/KF/Interface 
${CBMROOT_SOURCE_DIR}/KF/KFQA
${CBMROOT_SOURCE_DIR}/KF/KFParticle

${CBMROOT_SOURCE_DIR}/L1
${CBMROOT_SOURCE_DIR}/L1/L1Algo
${CBMROOT_SOURCE_DIR}/L1/ParticleFinder 

${CBMROOT_SOURCE_DIR}/much/geo

${CBMROOT_SOURCE_DIR}/rich

${CBMROOT_SOURCE_DIR}/littrack

${CBMROOT_SOURCE_DIR}/cbmgenerators
#${CBMROOT_SOURCE_DIR}/analysis/flow

#needed by rootcling
#${CBMROOT_SOURCE_DIR}/analysis/dielectron
${CBMROOT_SOURCE_DIR}/analysis/conversion
)

Include_Directories( ${INCLUDE_DIRECTORIES})

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${BASE_INCLUDE_DIRECTORIES} 
  ${Boost_INCLUDE_DIR}
)

If(NOT ${ROOT_HAS_VC})
  set(SYSTEM_INCLUDE_DIRECTORIES
      ${SYSTEM_INCLUDE_DIRECTORIES}      
      ${VC_INCLUDE_DIRS}
  )
EndIf()

Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
${Boost_LIBRARY_DIRS}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
#hyperon/CbmAnaHypYPtGenerator.cxx 
#hyperon/CbmAnaHypTools.cxx
##hyperon/CbmAnaHypSecFinder.cxx 
#hyperon/CbmAnaHypSecFinderQa.cxx
#hyperon/CbmAnaHypPair.cxx 
#hyperon/CbmAnaHypLambdaCandidate.cxx
#hyperon/CbmAnaHypTrack.cxx 
#hyperon/CbmAnaHypRecEff.cxx
#hyperon/CbmAnaHypRecLambda.cxx 
#hyperon/CbmAnaHypSelectLambda.cxx
#hyperon/CbmAnaHypRecCascade.cxx
#hyperon/CbmAnaHypMix.cxx

#CbmD0Analysis.cxx
#V0/CbmV0Analysis.cxx

#msh/CbmMSHConverter.cxx
#msh/CbmMSHAnalysis.cxx

#much/CbmAnaMuch.cxx
#much/CbmAnaMuchJpsiPolarization.cxx
#much/CbmPolarizedGenerator.cxx
#much/CbmAnaMuonCandidate.cxx 
#much/CbmAnaDimuonCandidate.cxx
#much/CbmAnaDimuonAnalysis.cxx
#much/CbmAnaDimuonHisto.cxx
#much/CbmAnaAngularAcceptance.cxx
#much/CbmAnaMuchBgd.cxx

#timing/CbmAnaTimingAnalysis.cxx 
#timing/CbmAnaEpochTest.cxx
#timing/CbmAnaTimingMuchDigitizer.cxx
#timing/CbmAnaTimingMuchHitFinder.cxx
#timing/CbmAnaTimingStsDigitizer.cxx
#timing/CbmAnaTimingStsHitFinder.cxx

dielectron/CbmAnaDielectronTask.cxx
dielectron/CbmAnaDielectronTaskDraw.cxx
dielectron/CbmAnaDielectronTaskDrawAll.cxx
dielectron/CbmAnaLmvmDrawStudy.cxx
dielectron/CbmLmvmHist.cxx
dielectron/CbmAnaDielectronStudyReportAll.cxx
dielectron/CbmAnaDielectronReports.cxx
dielectron/CbmLmvmHadd.cxx

jpsiana/CbmAnaJpsiTask.cxx
jpsiana/CbmAnaJpsiReport.cxx
jpsiana/CbmAnaJpsiHist.cxx

conversion/CbmAnaConversion.cxx
conversion/CbmAnaConversionTomography.cxx
conversion/CbmAnaConversionRich.cxx
#conversion/CbmAnaConversionKF.cxx
conversion/CbmAnaConversionReco.cxx
conversion/CbmAnaConversionPhotons.cxx
conversion/CbmAnaConversionRecoFull.cxx

#photon/CbmTrdPhotonAnalysis.cxx

flow/eventPlane.cxx
flow/CbmAnaFlow.cxx

)


Set(HEADERS )

IF (SSE_FOUND)
  Message(STATUS "Analysis will be compiled with SSE support")
  ADD_DEFINITIONS(-DHAVE_SSE)
  SET_SOURCE_FILES_PROPERTIES(${SRCS} PROPERTIES COMPILE_FLAGS 
  "-msse -O3")
ELSE (SSE_FOUND)
  MESSAGE(STATUS "Analysis will be compiled without SSE support")
  SET_SOURCE_FILES_PROPERTIES(${SRCS} PROPERTIES COMPILE_FLAGS 
  "-O3")
ENDIF (SSE_FOUND)

ADD_DEFINITIONS(-DDO_TPCCATRACKER_EFF_PERFORMANCE -DNonhomogeneousField -DCBM -DUSE_TIMERS)


Set(LINKDEF AnalysisLinkDef.h)
Set(LIBRARY_NAME Analysis)
Set(DEPENDENCIES
    Littrack KF L1 Much Rich Sts CbmBase CbmData CbmGenerators Base Vc.a)

GENERATE_LIBRARY()



