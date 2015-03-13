# Create a library called "libLxTrack" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed
# here.

Set(INCLUDE_DIRECTORIES
  ${BASE_INCLUDE_DIRECTORIES} 
  ${CBMROOT_SOURCE_DIR}/tracking/lx
  ${CBMROOT_SOURCE_DIR}/cbmbase 
  ${CBMROOT_SOURCE_DIR}/cbmdata 
  ${CBMROOT_SOURCE_DIR}/cbmdata/much 
  ${CBMROOT_SOURCE_DIR}/cbmdata/sts 
  ${CBMROOT_SOURCE_DIR}/cbmdata/global
  ${CBMROOT_SOURCE_DIR}/much/geo
  ${CBMROOT_SOURCE_DIR}/KF
  ${CBMROOT_SOURCE_DIR}/KF/Interface
  ${CBMROOT_SOURCE_DIR}/sts
  ${CBMROOT_SOURCE_DIR}/littrack/std
  ${CBMROOT_SOURCE_DIR}/littrack/cbm
)

Include_Directories( ${INCLUDE_DIRECTORIES})

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${Boost_INCLUDE_DIRS}
)

Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

Set(LINK_DIRECTORIES
  ${ROOT_LIBRARY_DIR}
  ${FAIRROOT_LIBRARY_DIR}
  ${Boost_LIBRARY_DIRS}
)
 
Link_Directories( ${LINK_DIRECTORIES})

Set(SRCS
  Lx.cxx
  LxTrackAna.cxx
)

Set(NO_DICT_SRCS
  LxCA.cxx
  LxDraw.cxx
  LxEff.cxx
  LxMC.cxx
  LxTrackAnaSegments.cxx
  LxUtils.cxx
)

set(LINKDEF LxLinkDef.h)
Set(LIBRARY_NAME LxTrack)
Set(DEPENDENCIES
    CbmBase CbmData Base boost_regex
)

GENERATE_LIBRARY()

