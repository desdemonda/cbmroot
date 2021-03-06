# Create a library called "libTutorial1" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${INCLUDE_DIRECTORIES} 
#put here all directories where header files are located
${CBMROOT_SOURCE_DIR}/tutorials/Tutorial1/src
${CBMROOT_SOURCE_DIR}/base
${CBMROOT_SOURCE_DIR}/cbmbase
${CBMROOT_SOURCE_DIR}/parbase
${CBMROOT_SOURCE_DIR}/geobase
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
) 

link_directories( ${LINK_DIRECTORIES})

set(TUTORIAL1_SRCS
#Put here your sourcefiles
CbmTutorialDet.cxx
CbmTutorialDetContFact.cxx
CbmTutorialDetGeo.cxx
CbmTutorialDetGeoPar.cxx
CbmTutorialDetPoint.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h TUTORIAL1_HEADERS "${TUTORIAL1_SRCS}")

set(TUTORIAL1_LINKDEF Tutorial1LinkDef.h)
set(TUTORIAL1_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/CbmTutorial1Dict.cxx) 

ROOT_GENERATE_DICTIONARY("${TUTORIAL1_HEADERS}" "${TUTORIAL1_LINKDEF}" "${TUTORIAL1_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

set(TUTORIAL1_SRCS ${TUTORIAL1_SRCS} ${TUTORIAL1_DICTIONARY})

add_library(Tutorial1 SHARED ${TUTORIAL1_SRCS})
target_link_libraries(Tutorial1 ${ROOT_LIBRARIES})
set_target_properties(Tutorial1 PROPERTIES  ${FAIRROOT_LIBRARY_PROPERTIES})

################ install ###################
install(TARGETS Tutorial1 DESTINATION ${CMAKE_BINARY_DIR}/lib)
