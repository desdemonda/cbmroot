

set(CUDA_BUILD_TYPE "Device")
#set(CUDA_BUILD_TYPE "Emulation")
# Load some basic macros which are needed later on
include(${CMAKE_SOURCE_DIR}/cmake/cuda/FindCuda.cmake)


# Add current directory to the nvcc include line.
CUDA_INCLUDE_DIRECTORIES(
   ${CMAKE_CURRENT_SOURCE_DIR}
   ${CUDA_CUT_INCLUDE}
   ${ROOT_INCLUDE_DIR}
   ${CUDA_INCLUDE}
)

set(CUDA_SRCS
    DeviceInfo.cu
    Test.cu
    LitCudaTracking.cu
    LitCudaField.cu
    LitCudaTrackFitter.cu
    FitTracks.cu
    LitCudaMath.cu
   )

set(LINK_DIRECTORIES
    $ {ROOT_LIBRARY_DIR}

   )

CUDA_ADD_LIBRARY(cudaimpl $ {CUDA_SRCS})

# Something else to link to.
#ADD_EXECUTABLE(cuda_example
#  main.cc
#)

# Specify the dependency.
#TARGET_LINK_LIBRARIES(
#  ${CUDA_TARGET_LINK}
#  ${CUDA_CUT_TARGET_LINK}
#  ${ROOT_LIBRARIES}
#)

##############################################################################
# Use the CUDA_COMPILE macro.
CUDA_COMPILE(CUDA_FILES)

IF(UNIX AND NOT WIN32)
FIND_PROGRAM(CMAKE_UNAME uname /bin /usr/bin /usr/local/bin )
IF(CMAKE_UNAME)
EXEC_PROGRAM(uname ARGS -m OUTPUT_VARIABLE CMAKE_SYSTEM_PROCESSOR)
SET(CMAKE_SYSTEM_PROCESSOR $ {CMAKE_SYSTEM_PROCESSOR} CACHE INTERNAL
    "processor type (i386 and x86_64)")
IF(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
ADD_DEFINITIONS(-fPIC)
ENDIF(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
ENDIF(CMAKE_UNAME)
ENDIF(UNIX AND NOT WIN32)

#############################################################################
set(INCLUDE_DIRECTORIES
    $ {ROOT_INCLUDE_DIR}
    $ {CUDA_INCLUDE}
   )

include_directories(
   $ {INCLUDE_DIRECTORIES}
)

#set(LINK_DIRECTORIES
#  ${ROOT_LIBRARY_DIR}
#
#)

#link_directories(
#   $ {LINK_DIRECTORIES}
#)

set(CUDALITTRACK_SRCS
    LitCudaTest.cxx
   )

set(CUDALITTRACK_HEADERS
    $ {CMAKE_CURRENT_SOURCE_DIR}/LitCudaTest.h
   )

#set(CUDA_LINKDEF  ${CMAKE_CURRENT_SOURCE_DIR}/cudaLinkDef.h)
#set(CUDALITTRACK_DICTIONARY "${CMAKE_CURRENT_BINARY_DIR}/cudaDict.cxx")
#ROOT_GENERATE_DICTIONARY("${INTERFACE_HEADERS}" "${INTERFACE_LINKDEF}" "${INTERFACE_DICTIONARY}" "${INCLUDE_DIRECTORIES}")
SET(CUDALITTRACK_SRCS $ {CUDALITTRACK_SRCS} $ {CUDALITTRACK_DICTIONARY})

ADD_LIBRARY(cudalittrack SHARED
            $ {CUDALITTRACK_SRCS}
           )

############### build the library #####################
#add_library(cudalittrack SHARED $ {CUDALITTRACK_SRCS})

target_link_libraries(
   cudalittrack $ {ROOT_LIBRARIES}
#  "${CMAKE_BINARY_DIR}/lib/libcudaimpl.a"
   "${CUDA_TARGET_LINK}"
   "${CUDA_CUT_TARGET_LINK}" )

set_target_properties(cudalittrack PROPERTIES $ {FAIRROOT_LIBRARY_PROPERTIES})

############### install the library #############
######
#install(TARGETS cudalittrack DESTINATION $ {CMAKE_BINARY_DIR}/lib)
