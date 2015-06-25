# Create a library called "libTrd" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${BASE_INCLUDE_DIRECTORIES} 

${CBMROOT_SOURCE_DIR}/cbmdisplay 
${CBMROOT_SOURCE_DIR}/cbmdata
${CBMROOT_SOURCE_DIR}/cbmdata/trd

${CBMROOT_SOURCE_DIR}/trd/data
${CBMROOT_SOURCE_DIR}/trd/param
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
${Boost_LIBRARY_DIRS}
) 

link_directories( ${LINK_DIRECTORIES})

set(SRCS
CbmTrdDigiDraw.cxx
CbmPixelHitSetDraw.cxx
)

set(LINKDEF  CbmDisplayLinkDef.h)
Set(LIBRARY_NAME CbmDisplay)
Set(DEPENDENCIES
    Trd CbmData EventDisplay Base 
)

GENERATE_LIBRARY()
