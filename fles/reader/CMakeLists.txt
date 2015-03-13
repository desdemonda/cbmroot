# Create a library called "libCbmFlibReader" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(INCLUDE_DIRECTORIES
  ${CBMROOT_SOURCE_DIR}/cbmdata
  ${CBMROOT_SOURCE_DIR}/cbmdata/raw
  ${CBMROOT_SOURCE_DIR}/cbmdata/beamtime/fhodo
  ${CBMROOT_SOURCE_DIR}/cbmbase
  ${CBMROOT_SOURCE_DIR}/fles/reader
  ${CBMROOT_SOURCE_DIR}/fles/reader/unpacker
  ${CBMROOT_SOURCE_DIR}/fles/reader/tasks
)

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${BASE_INCLUDE_DIRECTORIES} 
  ${Boost_INCLUDE_DIR} 
  ${ROOT_INCLUDE_DIR}  
  ${CBMROOT_SOURCE_DIR}/fles/ipc/src
  ${CBMROOT_SOURCE_DIR}/fles/flib_dpb
  ${CBMROOT_SOURCE_DIR}/fles/spadic
  ${CBMROOT_SOURCE_DIR}/fles/trb/trbnet_lib
  ${CBMROOT_SOURCE_DIR}/rich/prototype/beamtime

  # needed by rootcling
  ${CBMROOT_SOURCE_DIR}/fles/spadic/message/wrap/cpp
  ${CBMROOT_SOURCE_DIR}/fles/spadic/message
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})
 

Set(LINK_DIRECTORIES
  ${ROOT_LIBRARY_DIR}
  ${FAIRROOT_LIBRARY_DIR}
  ${Boost_LIBRARY_DIRS}
)
 
Link_Directories( 
  ${LINK_DIRECTORIES}
)

Set(SRCS
  CbmFlibFileSource.cxx   
#  CbmNxFlibFileSource.cxx   
  CbmFlibFileSourceNew.cxx   
  CbmDaqBuffer.cxx
  CbmFiberHodoMapping.cxx

  unpacker/CbmTSUnpack.cxx
  unpacker/CbmTSUnpackSpadic.cxx
  unpacker/CbmTSUnpackNxyter.cxx
  unpacker/CbmTSUnpackStsxyter.cxx
  unpacker/CbmTSUnpackFiberHodo.cxx
  unpacker/CbmTSUnpackDummy.cxx
  unpacker/CbmTSUnpackTrb.cxx
  unpacker/CbmGet4v1xHackDef.cxx
  unpacker/CbmTSUnpackGet4v1x.cxx

  tasks/CbmTrdRawBeamProfile.cxx
  tasks/CbmTrdOnlineDisplay.cxx
  tasks/CbmTrdEveOnlineDisplay.cxx
)

Set(NO_DICT_SRCS
 unpacker/TimesliceReader.cpp
)

If(NOT OldFairSource)
  Add_Definitions(-D_NewFairSource)
  Set(DEFINITIONS _NewFairSource)
EndIf()

Set(LINKDEF  FlibReaderLinkDef.h )
Set(LIBRARY_NAME CbmFlibReader)
Set(DEPENDENCIES
    Eve Base CbmBase Rich fles_ipc flib_dpb spadicMessage trbnet
)

GENERATE_LIBRARY()
