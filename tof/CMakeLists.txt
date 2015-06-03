# Create a library called "libBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(INCLUDE_DIRECTORIES
${CBMROOT_SOURCE_DIR}/tof
${CBMROOT_SOURCE_DIR}/tof/TofMC
${CBMROOT_SOURCE_DIR}/tof/TofParam
${CBMROOT_SOURCE_DIR}/tof/TofReco
${CBMROOT_SOURCE_DIR}/tof/TofTools
${CBMROOT_SOURCE_DIR}/tof/TofData
${CBMROOT_SOURCE_DIR}/tof/TofDigi
${CBMROOT_SOURCE_DIR}/tof/TofTests
#${CBMROOT_SOURCE_DIR}/unpack/tof/tdc
#${CBMROOT_SOURCE_DIR}/unpack/tof/mapping

${CBMROOT_SOURCE_DIR}/cbmbase 

${CBMROOT_SOURCE_DIR}/cbmdata
${CBMROOT_SOURCE_DIR}/cbmdata/tof
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
TofMC/CbmGeoTof.cxx 
TofMC/CbmTof.cxx 

TofParam/CbmGeoTofPar.cxx 
TofParam/CbmTofContFact.cxx 
TofParam/CbmTofDigiPar.cxx 
TofParam/CbmTofDigiBdfPar.cxx 
TofParam/CbmTofCreateDigiPar.cxx

TofReco/CbmTofHitProducer.cxx 
TofReco/CbmTofHitProducerIdeal.cxx 
TofReco/CbmTofHitProducerNew.cxx 

TofTools/CbmTofGeoHandler.cxx

TofData/CbmTofCell.cxx

TofDigi/CbmTofDigitizerBDF.cxx

TofReco/CbmTofSimpClusterizer.cxx 

#TofReco/CbmTofTestBeamClusterizer.cxx 

TofTests/CbmTofTests.cxx 
#TofTests/CbmTofAnaTestbeam.cxx 
)

set(LINKDEF TofLinkDef.h)
Set(LIBRARY_NAME Tof)
Set(DEPENDENCIES
    CbmBase CbmData Base 
)

GENERATE_LIBRARY()
