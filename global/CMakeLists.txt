# Create a library called "libGlobal" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(INCLUDE_DIRECTORIES
${CBMROOT_SOURCE_DIR}/global

${CBMROOT_SOURCE_DIR}/cbmbase
${CBMROOT_SOURCE_DIR}/cbmdata
${CBMROOT_SOURCE_DIR}/cbmdata/sts
${CBMROOT_SOURCE_DIR}/cbmdata/trd
${CBMROOT_SOURCE_DIR}/cbmdata/tof
${CBMROOT_SOURCE_DIR}/cbmdata/ecal
${CBMROOT_SOURCE_DIR}/cbmdata/global

${CBMROOT_SOURCE_DIR}/KF
${CBMROOT_SOURCE_DIR}/KF/Interface
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
CbmEcalTrackingMuch.cxx
CbmFindGlobalTracks.cxx    
CbmFindGlobalTracksQa.cxx  
CbmFindPrimaryVertex.cxx   
CbmFitGlobalTracks.cxx 
CbmFitGlobalTracksQa.cxx 
CbmGlobalTrackFitterIdeal.cxx 
CbmPVFinderIdeal.cxx       
CbmTrackMergerIdeal.cxx    
CbmTofMergerIdeal.cxx 
)


set(LINKDEF GlobalLinkDef.h)
Set(LIBRARY_NAME Global)
Set(DEPENDENCIES
    KF Ecal
    CbmBase CbmData Base 
)

GENERATE_LIBRARY()

