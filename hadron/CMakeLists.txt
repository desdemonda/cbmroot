# Create a library called "libBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(INCLUDE_DIRECTORIES
${CBMROOT_SOURCE_DIR}/cbmbase 

${CBMROOT_SOURCE_DIR}/cbmdata 
${CBMROOT_SOURCE_DIR}/cbmdata/sts
${CBMROOT_SOURCE_DIR}/cbmdata/trd
${CBMROOT_SOURCE_DIR}/cbmdata/tof
${CBMROOT_SOURCE_DIR}/cbmdata/global

${CBMROOT_SOURCE_DIR}/KF
${CBMROOT_SOURCE_DIR}/KF/Interface

${CBMROOT_SOURCE_DIR}/hadron 
${CBMROOT_SOURCE_DIR}/tof
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
CbmHadron.cxx
CbmProduceDst.cxx
CbmFillM2.cxx
CbmFitM2.cxx
CbmHadronSpectra.cxx
CbmHadronAnalysis.cxx
CbmHadronYields.cxx
CbmMixedEventsProducer.cxx
CbmEbefluctPYR.cxx
)

set(LINKDEF HadronLinkDef.h)
Set(LIBRARY_NAME Hadron)
Set(DEPENDENCIES
    KF CbmBase CbmData Base Tof
)

GENERATE_LIBRARY()
