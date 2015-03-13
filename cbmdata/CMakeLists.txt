# Create a library called "libCbmData" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(INCLUDE_DIRECTORIES
${CBMROOT_SOURCE_DIR}/cbmbase 
${CBMROOT_SOURCE_DIR}/cbmdata 
# needed by rootcling
${CBMROOT_SOURCE_DIR}/cbmdata/raw
${CBMROOT_SOURCE_DIR}/cbmdata/beamtime/fhodo
${CBMROOT_SOURCE_DIR}/cbmdata/mvd
${CBMROOT_SOURCE_DIR}/cbmdata/much
${CBMROOT_SOURCE_DIR}/cbmdata/trd
${CBMROOT_SOURCE_DIR}/cbmdata/tof
${CBMROOT_SOURCE_DIR}/cbmdata/ecal
)
Include_Directories( ${INCLUDE_DIRECTORIES})

Set(SYSTEM_INCLUDE_DIRECTORIES
${BASE_INCLUDE_DIRECTORIES} 
)

Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}

)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
CbmDetectorList.cxx

CbmBaseHit.cxx
CbmDigi.cxx
CbmHit.cxx
CbmPixelHit.cxx
CbmStack.cxx
CbmStripHit.cxx
CbmTrack.cxx
CbmTrackMatch.cxx
CbmCluster.cxx

CbmMCEpoch.cxx
CbmMCEventHeader.cxx
CbmMCTrack.cxx
CbmMCEventData.cxx

CbmLink.cxx
CbmMatch.cxx
CbmTrackMatchNew.cxx

mvd/CbmMvdCluster.cxx 
mvd/CbmMvdDetectorId.cxx
mvd/CbmMvdDigi.cxx  
mvd/CbmMvdHit.cxx 
mvd/CbmMvdHitMatch.cxx 
mvd/CbmMvdDigiMatch.cxx 
mvd/CbmMvdPoint.cxx

sts/CbmStsAddress.cxx
sts/CbmStsCluster.cxx 
sts/CbmStsDigi.cxx 
sts/CbmStsHit.cxx 
sts/CbmStsPoint.cxx 
sts/CbmStsTrack.cxx 
sts/CbmStsEventData.cxx

rich/CbmRichRing.cxx	
rich/CbmRichHit.cxx 
rich/CbmRichPoint.cxx
rich/CbmRichTrbDigi.cxx

much/CbmMuchPixelHit.cxx 
much/CbmMuchPoint.cxx
much/CbmMuchCluster.cxx 
much/CbmMuchDigi.cxx
much/CbmMuchBeamTimeDigi.cxx
much/CbmMuchDigiMatch.cxx
much/CbmMuchTrack.cxx 
much/CbmMuchStrawHit.cxx
much/CbmMuchStrawDigi.cxx
much/CbmMuchAddress.cxx

trd/CbmTrdHit.cxx		 
trd/CbmTrdTrack.cxx		 
trd/CbmTrdPoint.cxx	
trd/CbmTrdDigi.cxx  
trd/CbmTrdAddress.cxx
trd/CbmTrdCluster.cxx

tof/CbmTofHit.cxx 
tof/CbmTofPoint.cxx
tof/CbmTofDetectorId.cxx
tof/CbmTofDetectorId_v07a.cxx
tof/CbmTofDetectorId_v12b.cxx
tof/CbmTofDetectorId_v14a.cxx
tof/CbmTofAddress.cxx
tof/CbmTofDigi.cxx
tof/CbmTofDigiExp.cxx

ecal/CbmEcalHit.cxx 
ecal/CbmEcalPoint.cxx
ecal/CbmEcalPointLite.cxx 
ecal/CbmEcalHitFastMC.cxx 
ecal/CbmEcalCell.cxx
ecal/CbmEcalMaximum.cxx
ecal/CbmEcalCluster.cxx
ecal/CbmEcalRecParticle.cxx
ecal/CbmEcalIdParticle.cxx

psd/CbmPsdDigi.cxx
psd/CbmPsdHit.cxx
psd/CbmPsdPoint.cxx
psd/CbmPsdEventData.cxx

stt/CbmSttPoint.cxx
stt/CbmSttHit.cxx
stt/CbmSttTrack.cxx
stt/CbmSttTrackMatch.cxx

global/CbmGlobalTrack.cxx
global/CbmVertex.cxx
global/CbmTofTrack.cxx

beamtime/CbmAuxDigi.cxx
beamtime/CbmTbEvent.cxx
beamtime/fhodo/CbmFiberHodoDigi.cxx
beamtime/fhodo/CbmFiberHodoCluster.cxx
beamtime/fhodo/CbmFiberHodoAddress.cxx

raw/CbmRawMessage.cxx
raw/CbmSpadicRawMessage.cxx
raw/CbmNxyterRawMessage.cxx
raw/CbmNxyterRawSyncMessage.cxx
raw/CbmStsxyterRawHitMessage.cxx
raw/CbmTrbRawMessage.cxx
)

set(LINKDEF  DataLinkDef.h)
Set(LIBRARY_NAME CbmData)
Set(DEPENDENCIES
    Base
)

GENERATE_LIBRARY()


