# CMakeList file for library libSts.
# Last update: V. Friese, 17.02.2014


# -----   Specify library name   --------------------------
Set(LIBRARY_NAME Sts)
# ---------------------------------------------------------


# -----  Specify sources for compilation   ----------------
# --- Sources in setup
set(SRCS_SETUP
#setup/CbmStsAddress.cxx
setup/CbmStsElement.cxx
setup/CbmStsModule.cxx
setup/CbmStsSensor.cxx
setup/CbmStsSensorConditions.cxx
setup/CbmStsSensorPoint.cxx
setup/CbmStsSensorType.cxx
setup/CbmStsSetup.cxx
setup/CbmStsStation.cxx
)
# --- Sources in mc
set(SRCS_MC
mc/CbmStsMC.cxx
mc/CbmStsSensorFactory.cxx
)
# --- Sources in digitize
set (SRCS_DIGITIZE
digitize/CbmStsDigitize.cxx
digitize/CbmStsPhysics.cxx
digitize/CbmStsSensorTypeDssd.cxx
digitize/CbmStsSensorTypeDssdIdeal.cxx;
digitize/CbmStsSensorTypeDssdReal.cxx;
digitize/CbmStsSignal.cxx;
)
# --- Sources in reco
set (SRCS_RECO
reco/CbmStsClusterFinderSimple.cxx
reco/CbmStsFindClusters.cxx
reco/CbmStsFindHits.cxx
)
# --- Legacy sources
set (SRCS_LEGACY
legacy/CbmStsClusterFinder_old.cxx 
legacy/CbmStsDigitize_old.cxx 
legacy/CbmStsFindHits_old.cxx 
legacy/CbmStsSensor_old.cxx
legacy/CbmStsStation_old.cxx
# --- Legacy, not compiled any longer
# legacy/CbmStsIdealDigitize.cxx 
# legacy/CbmStsIdealFindHits.cxx 
# legacy/CbmStsRealClusterFinder.cxx 
# legacy/CbmStsRealDigitize.cxx 
# legacy/CbmStsRealFindHits.cxx 
)
# --- All sources
set(SRCS
${SRCS_SETUP}
${SRCS_MC}
${SRCS_DIGITIZE}
${SRCS_RECO}
${SRCS_LEGACY}
CbmGeoSts.cxx 
CbmGeoStsPar.cxx 
CbmStsContFact.cxx 
CbmStsDigiPar.cxx 
CbmStsDigiScheme.cxx 
CbmStsDigitizeTb.cxx 
CbmStsFindTracks.cxx 
CbmStsFindTracksQa.cxx 
CbmStsFitTracks.cxx 
CbmStsHitProducerIdeal.cxx 
CbmStsIdealMatchHits.cxx 
CbmStsMatchHits.cxx 
CbmStsMatchTracks.cxx 
CbmStsParAsciiFileIo.cxx 
CbmStsParRootFileIo.cxx 
CbmStsRadTool.cxx
CbmStsSensorDigiPar.cxx 
CbmStsSector.cxx 
CbmStsSectorDigiPar.cxx 
CbmStsStationDigiPar.cxx 
CbmStsTrackFinderIdeal.cxx 
CbmStsTrackFitterIdeal.cxx 
CbmStsSimulationQa.cxx 
CbmStsFindHitsQa.cxx
CbmStsReconstructionQa.cxx
CbmStsDigitizeQa.cxx
)
# -----  End of sources   ---------------------------------



# ----  Specify include directories -----------------------
set(INCLUDE_DIRECTORIES
${CBMROOT_SOURCE_DIR}/sts
${CBMROOT_SOURCE_DIR}/sts/setup
${CBMROOT_SOURCE_DIR}/sts/mc
${CBMROOT_SOURCE_DIR}/sts/digitize
${CBMROOT_SOURCE_DIR}/sts/legacy

# needed by MatchHits???
${CBMROOT_SOURCE_DIR}/passive 

${CBMROOT_SOURCE_DIR}/cbmbase 

${CBMROOT_SOURCE_DIR}/run 

${CBMROOT_SOURCE_DIR}/cbmdata 
${CBMROOT_SOURCE_DIR}/cbmdata/sts 

# needed by run/CbmMCBuffer.h
${CBMROOT_SOURCE_DIR}/cbmdata/mvd 
${CBMROOT_SOURCE_DIR}/cbmdata/rich 
${CBMROOT_SOURCE_DIR}/cbmdata/much 
${CBMROOT_SOURCE_DIR}/cbmdata/trd 
${CBMROOT_SOURCE_DIR}/cbmdata/tof 
${CBMROOT_SOURCE_DIR}/cbmdata/ecal 
${CBMROOT_SOURCE_DIR}/cbmdata/psd 
)

set(SYSTEM_INCLUDE_DIRECTORIES
${BASE_INCLUDE_DIRECTORIES} 
)
# ----  End of include directories ------------------------


# ----  Specify link directories --------------------------
set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
${Boost_LIBRARY_DIRS}
)
# -----  End of link directories   ------------------------


# -----   Specify library dependences   -------------------
Set(DEPENDENCIES
    Passive Run CbmBase CbmData Base 
)
# ---------------------------------------------------------


# -----  Specify LinkDef file   ---------------------------
set(LINKDEF ${LIBRARY_NAME}LinkDef.h)
# ---------------------------------------------------------


# -----  Let cmake do the job   ---------------------------
include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})
link_directories( ${LINK_DIRECTORIES})
GENERATE_LIBRARY()
# ---------------------------------------------------------
