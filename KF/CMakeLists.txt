# Create a library called "libKF" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(INCLUDE_DIRECTORIES
${CBMROOT_SOURCE_DIR}/cbmbase 
${CBMROOT_SOURCE_DIR}/cbmdata
${CBMROOT_SOURCE_DIR}/cbmdata/sts
${CBMROOT_SOURCE_DIR}/cbmdata/mvd
${CBMROOT_SOURCE_DIR}/cbmdata/trd
${CBMROOT_SOURCE_DIR}/cbmdata/rich
${CBMROOT_SOURCE_DIR}/cbmdata/tof
${CBMROOT_SOURCE_DIR}/cbmdata/ecal
${CBMROOT_SOURCE_DIR}/cbmdata/global

${CBMROOT_SOURCE_DIR}/KF
${CBMROOT_SOURCE_DIR}/KF/Interface
${CBMROOT_SOURCE_DIR}/KF/KFModelParameters
${CBMROOT_SOURCE_DIR}/KF/KFQA
${CBMROOT_SOURCE_DIR}/KF/KFParticle
${CBMROOT_SOURCE_DIR}/KF/KFParticlePerformance
${CBMROOT_SOURCE_DIR}/KF/KFParticlesFinder

${CBMROOT_SOURCE_DIR}/sts 

${CBMROOT_SOURCE_DIR}/L1
${CBMROOT_SOURCE_DIR}/L1/L1Algo
${CBMROOT_SOURCE_DIR}/L1/ParticleFinder

${CBMROOT_SOURCE_DIR}/field 
${CBMROOT_SOURCE_DIR}/passive 
${CBMROOT_SOURCE_DIR}/mvd 
${CBMROOT_SOURCE_DIR}/rich 
${CBMROOT_SOURCE_DIR}/much/geo
${CBMROOT_SOURCE_DIR}/trd/param
${CBMROOT_SOURCE_DIR}/tof/TofParam
${CBMROOT_SOURCE_DIR}/ecal
${CBMROOT_SOURCE_DIR}/stt
)

Include_Directories( ${INCLUDE_DIRECTORIES})

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${BASE_INCLUDE_DIRECTORIES} 
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
CbmKF.cxx 
CbmKFFieldMath.cxx 
CbmKFHit.cxx 
CbmKFMaterial.cxx 
CbmKFMath.cxx 
CbmKFPixelMeasurement.cxx 
CbmKFPrimaryVertexFinder.cxx 
CbmKFSecondaryVertexFinder.cxx 
CbmKFTrackInterface.cxx 
CbmKFUMeasurement.cxx 
CbmKFVertexInterface.cxx 
Interface/CbmEcalTrackExtrapolationKF.cxx
Interface/CbmKFStsHit.cxx 
Interface/CbmKFTrack.cxx 
Interface/CbmKFTrdHit.cxx 
Interface/CbmPVFinderKF.cxx 
Interface/CbmStsFitPerformanceTask.cxx 
Interface/CbmStsKFTrackFitter.cxx 
Interface/CbmStsKFSecondaryVertexFinder.cxx 
Interface/CbmTrdTrackFitterKF.cxx 
Interface/CbmGlobalTrackFitterKF.cxx
# CbmKFParticle.cxx
# CbmKFParticle_simd.cxx
# CbmKFParticleInterface.cxx
KFModelParameters/CbmThermalModelNoFlow.cxx
KFModelParameters/CbmModelsQA.cxx
KFQA/CbmKFTrErrMCPoints.cxx
KFQA/CbmKFTrackFitQA.cxx
KFQA/KFParticleMatch.cxx
# KFParticlesFinder/CbmKFParticleDatabase.cxx
KFParticle/KFParticleDatabase.cxx
KFParticle/KFParticleBase.cxx
KFParticle/KFParticle.cxx
KFParticle/KFPTrack.cxx
KFParticle/KFPVertex.cxx
KFParticle/KFPTrackVector.cxx
KFParticle/KFParticleBaseSIMD.cxx
KFParticle/KFParticleSIMD.cxx
KFParticle/KFVertex.cxx 
KFParticle/KFParticlePVReconstructor.cxx
KFParticle/KFParticleFinder.cxx
KFParticle/KFParticleTopoReconstructor.cxx
KFParticlePerformance/KFPartEfficiencies.cxx
KFParticlePerformance/KFMCVertex.cxx
KFParticlePerformance/KFMCParticle.cxx
KFParticlePerformance/KFParticlePerformanceBase.cxx
KFParticlePerformance/KFTopoPerformance.cxx
CbmKFParticleFinder.cxx
CbmKFParticleFinderPID.cxx
CbmKFParticleFinderQA.cxx
CbmKFParticleInterface.cxx
)


set(HEADERS
CbmKF.h 
CbmKFFieldMath.h
CbmKFHit.h
CbmKFMaterial.h
CbmKFMath.h
CbmKFPixelMeasurement.h
CbmKFPrimaryVertexFinder.h
CbmKFSecondaryVertexFinder.h
CbmKFTrackInterface.h
CbmKFUMeasurement.h 
CbmKFVertexInterface.h
Interface/CbmEcalTrackExtrapolationKF.h
Interface/CbmKFStsHit.h
Interface/CbmKFTrack.h 
Interface/CbmKFTrdHit.h 
Interface/CbmPVFinderKF.h
Interface/CbmStsFitPerformanceTask.h
Interface/CbmStsKFTrackFitter.h
Interface/CbmStsKFSecondaryVertexFinder.h
Interface/CbmTrdTrackFitterKF.h
Interface/CbmGlobalTrackFitterKF.h
# CbmKFParticle.h
KFModelParameters/CbmThermalModelNoFlow.h
KFModelParameters/CbmModelsQA.h
KFQA/CbmKFTrErrMCPoints.h
KFQA/CbmKFTrackFitQA.h
KFQA/CbmKFPartEfficiencies.h
KFQA/KFParticleMatch.h
# KFParticlesFinder/CbmKFParticleDatabase.h
KFParticle/KFParticleBase.h
KFParticle/KFParticle.h
KFParticle/KFVertex.h
KFParticlePerformance/KFPartEfficiencies.h
KFParticlePerformance/KFPVEfficiencies.h
KFParticlePerformance/KFPartMatch.h
KFParticlePerformance/KFMCTrack.h
KFParticlePerformance/KFMCVertex.h
KFParticlePerformance/KFMCParticle.h
KFParticlePerformance/KFParticlePerformanceBase.h
KFParticlePerformance/KFTopoPerformance.h
CbmKFParticleFinder.h
CbmKFParticleFinderPID.h
CbmKFParticleFinderQA.h
CbmKFParticleInterface.h
)

If (APPLE)
  ADD_DEFINITIONS(-Wall -Wabi -Wsign-promo  -Wno-pmf-conversions -Wctor-dtor-privacy -Wreorder -Wno-deprecated -Wstrict-null-sentinel -Wno-non-template-friend -Wno-pragmas -Wno-parentheses -DDO_TPCCATRACKER_EFF_PERFORMANCE -DNonhomogeneousField -DCBM -DUSE_TIMERS) # -Weffc++ -Wnon-virtual-dtor -Woverloaded-virtual -Wold-style-cast   : wait for other parts of cbmroot\root.
Else()
  ADD_DEFINITIONS(-Wall -Wabi -fabi-version=0 -Wsign-promo  -Wno-pmf-conversions -Wctor-dtor-privacy -Wreorder -Wno-deprecated -Wstrict-null-sentinel -Wno-non-template-friend -Wno-pragmas -Wno-parentheses -DDO_TPCCATRACKER_EFF_PERFORMANCE -DNonhomogeneousField -DCBM -DUSE_TIMERS) # -Weffc++ -Wnon-virtual-dtor -Woverloaded-virtual -Wold-style-cast   : wait for other parts of cbmroot\root.
EndIf()

IF (SSE_FOUND)
  Message(STATUS "KF will be compiled with SSE support")
  ADD_DEFINITIONS(-DHAVE_SSE)
  SET_SOURCE_FILES_PROPERTIES(${SRCS} PROPERTIES COMPILE_FLAGS 
  "-msse -O3")
ELSE (SSE_FOUND)
  MESSAGE(STATUS "KF will be compiled without SSE support")
  SET_SOURCE_FILES_PROPERTIES(${SRCS} PROPERTIES COMPILE_FLAGS 
  "-O3")
ENDIF (SSE_FOUND)

set(LINKDEF KFLinkDef.h)
Set(LIBRARY_NAME KF)
Set(DEPENDENCIES
    Sts Ecal CbmBase CbmData Base Vc.a Minuit2
)

GENERATE_LIBRARY()
