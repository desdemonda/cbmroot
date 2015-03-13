set(INCLUDE_DIRECTORIES
 ${ROOT_INCLUDE_DIR}
 ${GTEST_INCLUDE_DIRS} 
 ${CMAKE_SOURCE_DIR}/cbmdata/trd
 ${CMAKE_SOURCE_DIR}/cbmbase
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

###############################
Set ( Sources
    _GTestCbmTrdDetectorId.cxx
    ${CMAKE_SOURCE_DIR}/cbmdata/trd/CbmTrdDetectorId.cxx
    )  

add_executable(_GTestCbmTrdDetectorId ${Sources})
target_link_libraries(_GTestCbmTrdDetectorId ${ROOT_LIBRARIES} ${GTEST_BOTH_LIBRARIES})
add_test(_GTestCbmTrdDetectorId ${PROJECT_BINARY_DIR}/bin/_GTestCbmTrdDetectorId)

###############################
#Set ( Sources
#    _GTestCbmTrdDetectorId_simple.cxx
#    ${CMAKE_SOURCE_DIR}/cbmdata/trd/CbmTrdDetectorId.cxx
#    )  
#
#add_executable(_GTestCbmTrdDetectorId_simple ${Sources})
#target_link_libraries(_GTestCbmTrdDetectorId_simple ${ROOT_LIBRARIES} ${GTEST_BOTH_LIBRARIES})
#add_test(_GTestCbmTrdDetectorId_simple ${PROJECT_BINARY_DIR}/bin/_GTestCbmTrdDetectorId_simple)
#
################################
#Set ( Sources
#    _GTestCbmTrdDetectorId_simple1.cxx
#    ${CMAKE_SOURCE_DIR}/cbmdata/trd/CbmTrdDetectorId.cxx
#    )  
#
#add_executable(_GTestCbmTrdDetectorId_simple1 ${Sources})
#target_link_libraries(_GTestCbmTrdDetectorId_simple1 ${ROOT_LIBRARIES} ${GTEST_BOTH_LIBRARIES})
#add_test(_GTestCbmTrdDetectorId_simple1 ${PROJECT_BINARY_DIR}/bin/_GTestCbmTrdDetectorId_simple1)
#
################################
#Set ( Sources
#    _GTestCbmTrdDetectorId_simple2.cxx
#    ${CMAKE_SOURCE_DIR}/cbmdata/trd/CbmTrdDetectorId.cxx
#    )  
#
#add_executable(_GTestCbmTrdDetectorId_simple2 ${Sources})
#target_link_libraries(_GTestCbmTrdDetectorId_simple2 ${ROOT_LIBRARIES} ${GTEST_BOTH_LIBRARIES})
#add_test(_GTestCbmTrdDetectorId_simple2 ${PROJECT_BINARY_DIR}/bin/_GTestCbmTrdDetectorId_simple2)
#
################################
#Set ( Sources
#    _GTestCbmTrdDetectorId_simple3.cxx
#    ${CMAKE_SOURCE_DIR}/cbmdata/trd/CbmTrdDetectorId.cxx
#    )  
#
#add_executable(_GTestCbmTrdDetectorId_simple3 ${Sources})
#target_link_libraries(_GTestCbmTrdDetectorId_simple3 ${ROOT_LIBRARIES} ${GTEST_BOTH_LIBRARIES})
#add_test(_GTestCbmTrdDetectorId_simple3 ${PROJECT_BINARY_DIR}/bin/_GTestCbmTrdDetectorId_simple3)




