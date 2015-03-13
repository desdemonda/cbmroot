set(INCLUDE_DIRECTORIES
 ${ROOT_INCLUDE_DIR}
 ${GTEST_INCLUDE_DIRS} 
 ${CMAKE_SOURCE_DIR}/cbmbase
 ${CMAKE_CURRENT_SOURCE_DIR}
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

Set ( Sources
    ${CMAKE_SOURCE_DIR}/cbmbase/CbmDetectorList.cxx
    )  

CHANGE_FILE_EXTENSION(*.cxx *.h HDRS "${Sources}")
set(LINKDEF CbmDetectorListLinkDef.h)
set(DICTIONARY CbmDetectorListDict.cxx)
ROOT_GENERATE_DICTIONARY()

Set(Sources ${Sources} ${DICTIONARY} _GTestCbmDetectorList.cxx)
 

############### build the test #####################
add_executable(_GTestCbmDetectorList ${Sources})
target_link_libraries(_GTestCbmDetectorList ${ROOT_LIBRARIES} ${GTEST_BOTH_LIBRARIES})
add_test(_GTestCbmDetectorList ${PROJECT_BINARY_DIR}/bin/_GTestCbmDetectorList)




