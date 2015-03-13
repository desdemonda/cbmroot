# Create a library called "libCbmGenerators" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(INCLUDE_DIRECTORIES
${CBMROOT_SOURCE_DIR}/cbmdata
${CBMROOT_SOURCE_DIR}/cbmgenerators
)

Include_Directories( ${INCLUDE_DIRECTORIES})

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${BASE_INCLUDE_DIRECTORIES} 
  ${PLUTO_INCLUDE_DIR}
)

Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
${PLUTO_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
CbmDirectPhotonGenerator.cxx
CbmEcalGenerator.cxx
CbmHsdLvmGenerator.cxx
CbmHsdGenerator.cxx
CbmShieldGenerator.cxx
CbmUnigenGenerator.cxx
CbmUrqmdGenerator.cxx
URun.cxx
UEvent.cxx
UParticle.cxx
CbmIonGenerator.cxx
CbmPlutoGenerator.cxx
CbmPlutoReactionGenerator.cxx
)


set(LINKDEF  CbmGeneratorsLinkDef.h)
Set(LIBRARY_NAME CbmGenerators)
Set(DEPENDENCIES
    CbmData Base Pluto
)

GENERATE_LIBRARY()
