set(INCLUDE_DIRECTORIES
 ${ROOT_INCLUDE_DIR}
 ${GTEST_INCLUDE_DIRS} 
 ${CMAKE_SOURCE_DIR}/trd
 ${CMAKE_SOURCE_DIR}/cbmdata/trd
 ${CMAKE_SOURCE_DIR}/cbmbase
 ${CMAKE_SOURCE_DIR}/fairtools
 ${CMAKE_SOURCE_DIR}/test/mock
 ${CMAKE_CURRENT_SOURCE_DIR}
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

Set(Sources
    ${CMAKE_SOURCE_DIR}/cbmbase/CbmDetectorList.cxx	    
    ${CMAKE_SOURCE_DIR}/trd/CbmTrdGeoHandler.cxx
)

CHANGE_FILE_EXTENSION(*.cxx *.h HDRS "${Sources}")
set(LINKDEF TrdGeoHandlerLinkDef.h)
set(DICTIONARY TrdGeoHandlerDict.cxx)
ROOT_GENERATE_DICTIONARY()

set(Sources ${Sources} ${DICTIONARY}
    _GTestTrdGeoHandler.cxx
    ${CMAKE_SOURCE_DIR}/cbmdata/trd/CbmTrdDetectorId.cxx
    )  
 
############### build the test #####################
  add_executable(_GTestTrdGeoHandler ${Sources})
  target_link_libraries(_GTestTrdGeoHandler ${ROOT_LIBRARIES} Geom GeomPainter VMC ${GTEST_BOTH_LIBRARIES} FairTools FairMock)

  Generate_Exe_Script(${CMAKE_CURRENT_SOURCE_DIR} _GTestTrdGeoHandler)
  add_test(_GTestTrdGeoHandler ${PROJECT_CURRENT_BINARY_DIR}/_GTestTrdGeoHandler.sh)
#  add_test(run ${CBMROOT_BINARY_DIR}/macro/run/run.sh)
#SET_TESTS_PROPERTIES(run PROPERTIES TIMEOUT "300")
#SET_TESTS_PROPERTIES(run PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All







