Set(INCLUDE_DIRECTORIES

${CBMROOT_SOURCE_DIR}/analysis
${CBMROOT_SOURCE_DIR}/analysis/papa


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
${CBMROOT_SOURCE_DIR}/rich 

${CBMROOT_SOURCE_DIR}/KF
${CBMROOT_SOURCE_DIR}/KF/Interface 
${CBMROOT_SOURCE_DIR}/KF/KFQA
${CBMROOT_SOURCE_DIR}/KF/KFParticle

#${CBMROOT_SOURCE_DIR}/L1
#${CBMROOT_SOURCE_DIR}/L1/ParticleFinder
#${CBMROOT_SOURCE_DIR}/L1/L1Algo
#${CBMROOT_SOURCE_DIR}/L1/vectors

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

./AnalysisCuts.cxx
./AnalysisFilter.cxx
./PairAnalysisCutGroup.cxx
./PairAnalysisTrack.cxx
./PairAnalysisEvent.cxx
./PairAnalysisPair.cxx
#./PairAnalysisPairKF.cxx
./PairAnalysisPairLV.cxx
./PairAnalysisCutQA.cxx
./PairAnalysisPairLegCuts.cxx
./PairAnalysisVarCuts.cxx
./PairAnalysisHelper.cxx
./PairAnalysisStyler.cxx
./PairAnalysisVarManager.cxx

./AnalysisTaskMultiPairAnalysis.cxx
#./PairAnalysisCF.cxx
#./PairAnalysisCFdraw.cxx
./PairAnalysis.cxx
#./PairAnalysisDebugTree.cxx
#./PairAnalysisEventCuts.cxx
#./PairAnalysisMixedEvent.cxx
./PairAnalysisHF.cxx
###./PairAnalysisHFhelper.cxx
./PairAnalysisHistos.cxx
./PairAnalysisMC.cxx
#./PairAnalysisMixingHandler.cxx
./PairAnalysisVarCutsCombi.cxx
#./PairAnalysisSignalBase.cxx
#./PairAnalysisSignalExt.cxx
#./PairAnalysisSignalFunc.cxx
./PairAnalysisSignalMC.cxx
#./PairAnalysisSpectrum.cxx
#./PairAnalysisTrackCuts.cxx
./PairAnalysisTrackRotator.cxx
#./PairAnalysisV0Cuts.cxx
#./PairAnalysisVarContainer.cxx
)


Set(HEADERS )

IF (SSE_FOUND)
  Message(STATUS "Papa will be compiled with SSE support")
  ADD_DEFINITIONS(-DHAVE_SSE)
  SET_SOURCE_FILES_PROPERTIES(${SRCS} PROPERTIES COMPILE_FLAGS 
  "-msse -O3")
ELSE (SSE_FOUND)
  MESSAGE(STATUS "Papa will be compiled without SSE support")
  SET_SOURCE_FILES_PROPERTIES(${SRCS} PROPERTIES COMPILE_FLAGS 
  "-O3")
ENDIF (SSE_FOUND)

If(${MAC_OS_VERSION} MATCHES 10.6)
  ADD_DEFINITIONS(-DDO_TPCCATRACKER_EFF_PERFORMANCE -DNonhomogeneousField -DCBM -DUSE_TIMERS -DOLD_MAC)
  Set(DEFINITIONS OLD_MAC)
Else()
  ADD_DEFINITIONS(-DDO_TPCCATRACKER_EFF_PERFORMANCE -DNonhomogeneousField -DCBM -DUSE_TIMERS)
EndIf()

Set(LINKDEF PapaLinkDef.h)
Set(LIBRARY_NAME Papa)
Set(DEPENDENCIES
    KF Sts CbmBase CbmData Base Vc.a)

GENERATE_LIBRARY()



