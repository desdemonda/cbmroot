# The name of our project is "CBMROOT".  CMakeLists files in this project can
# refer to the root source directory of the project as ${CBMROOT_SOURCE_DIR} or
# as ${CMAKE_SOURCE_DIR} and to the root binary directory of the project as 
# ${CBMROOT_BINARY_DIR} or ${CMAKE_BINARY_DIR}.
# This difference is important for the base classes which are in CBMROOT 
# and PANDAROOT.

# Check if cmake has the required version
CMAKE_MINIMUM_REQUIRED(VERSION 2.4.3 FATAL_ERROR)

# Set name of our project to "CBMROOT". Has to be done
# after check of cmake version                        
project(CBMROOT)

FIND_PATH(FAIRBASE NAMES FairRun.h  PATHS
  ${CMAKE_SOURCE_DIR}/base/steer
  NO_DEFAULT_PATH
)

If (FAIRBASE)
  Message(STATUS "Found FAIRBASE")
  SET(FAIRBASE ${FAIRBASE})
Else (FAIRBASE)
  Message(STATUS "NOT Found FAIRBASE")
  IF(NOT DEFINED ENV{FAIRROOTPATH})
     MESSAGE(FATAL_ERROR "You did not define the environment variable FAIRROOTPATH which is needed to find FairRoot. Please set this variable and execute cmake again.")
  ENDIF(NOT DEFINED ENV{FAIRROOTPATH})
  SET(FAIRROOTPATH $ENV{FAIRROOTPATH})

EndIf (FAIRBASE)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ 
# is checked
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")
set(CMAKE_MODULE_PATH "${FAIRROOTPATH}/share/fairbase/cmake/modules"  ${CMAKE_MODULE_PATH})
set(CMAKE_MODULE_PATH "${FAIRROOTPATH}/share/fairbase/cmake/modules_old" ${CMAKE_MODULE_PATH})

# Check if the compiler support specific C++11 features
# Up to now this is only a check since the code does not use
# any of the features of the new standard

IF(FAIRROOTPATH)
  Set(CheckSrcDir "${FAIRROOTPATH}/share/fairbase/cmake/checks")
ELSE(FAIRROOTPATH)
  Set(CheckSrcDir "${CMAKE_SOURCE_DIR}/cmake/checks")
ENDIF(FAIRROOTPATH)

if(FAIRROOTPATH)
  find_package(FairRoot)
endif(FAIRROOTPATH)

include(CheckCXX11Features)

# Load some basic macros which are needed later on
include(FairMacros)
include(WriteConfigFile)
include(CTest)
include(CheckCompiler)
Include(ExternalProject)

#Check the compiler and set the compile and link flags
Check_Compiler()

Set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
Set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
Set(INCLUDE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/include")

Set(VMCWORKDIR ${CBMROOT_SOURCE_DIR})

Option(USE_PATH_INFO "Information from PATH and LD_LIBRARY_PATH are used." OFF)
If(USE_PATH_INFO)
  Set(PATH "$PATH")
  If (APPLE)
    Set(LD_LIBRARY_PATH $ENV{DYLD_LIBRARY_PATH})
  Else (APPLE)
    Set(LD_LIBRARY_PATH $ENV{LD_LIBRARY_PATH})
  EndIf (APPLE)
Else(USE_PATH_INFO)
  STRING(REGEX MATCHALL "[^:]+" PATH $ENV{PATH})
EndIf(USE_PATH_INFO)

# Check if the user wants to build the project in the source
# directory
CHECK_OUT_OF_SOURCE_BUILD()

# Check if we are on an UNIX system. If not stop with an error
# message
IF(NOT UNIX)
  MESSAGE(FATAL_ERROR "You're not on an UNIX system. The project was up to now only tested on UNIX systems, so we break here. If you want to go on please edit the CMakeLists.txt in the source directory.")
ENDIF(NOT UNIX)  

#Check if necessary environment variables are set
#If not stop execution
if(NOT CBMROOT_MINIMAL)
  IF(NOT DEFINED ENV{SIMPATH})
    MESSAGE(FATAL_ERROR "You did not define the environment variable SIMPATH which is nedded to find the external packages. Please set this variable and execute cmake again.")
  ENDIF(NOT DEFINED ENV{SIMPATH})
  SET(SIMPATH $ENV{SIMPATH})
else(NOT CBMROOT_MINIMAL)
  if(NOT DEFINED ENV{ROOTSYS})
    MESSAGE(FATAL_ERROR "You did not define the environment variable ROOTSYS which is nedded for the installation of the minimal version of CbmRoot. Please set this variable and execute cmake again.")
  endif(NOT DEFINED ENV{ROOTSYS})
endif(NOT CBMROOT_MINIMAL)


# Check if the external packages are installed into a separate install
# directory
CHECK_EXTERNAL_PACKAGE_INSTALL_DIR()

# searches for needed packages
# REQUIRED means that cmake will stop if this packages are not found
# For example the framework can run without GEANT4, but ROOT is
# mandatory
if(CBMROOT_MINIMAL)
  MESSAGE("******* Install minimal CbmRoot Version ********")
  find_package(ROOT 5.32.00 REQUIRED)
else(CBMROOT_MINIMAL)
  find_package(ROOT 5.32.00 REQUIRED)
  find_package(PLUTO REQUIRED)
  find_package(GENERATORS REQUIRED)
  find_package(GEANT3 REQUIRED)
  find_package(GEANT4)
  find_package(GEANT4DATA)
  find_package(GEANT4VMC)
  find_package(CLHEP)
  find_package(TBB)
  find_package(SSE)
  find_package(IWYU)
  find_package(ZeroMQ)
# switch of the checking for the RuleChecker, since there is a problem
# with JAVA on some systems which resulting in endless hanging of CMake 
#  find_package(RuleChecker)


  Set(Boost_NO_SYSTEM_PATHS TRUE)
  Set(Boost_NO_BOOST_CMAKE TRUE)
#  set(Boost_DEBUG TRUE)
  If(${ROOT_LIBRARY_DIR} MATCHES /lib/root)
    set(BOOST_ROOT ${SIMPATH})
    set(GSL_DIR ${SIMPATH})
  Else(${ROOT_LIBRARY_DIR} MATCHES /lib/root)
    set(BOOST_ROOT ${SIMPATH}/basics/boost)
    set(GSL_DIR ${SIMPATH}/basics/gsl)
  EndIf(${ROOT_LIBRARY_DIR} MATCHES /lib/root)
  Message("-- Looking for Boost ...")
  # If an older version of boost is found both of the variables below are
  # cached and in a second cmake run, a good boost version is found even 
  # if the version is to old. 
  # To overcome this problem both variables are cleared before checking
  # for boost.
  Unset(Boost_INCLUDE_DIR CACHE)
  Unset(Boost_LIBRARY_DIRS CACHE)
  find_package(Boost 1.48 COMPONENTS program_options thread system timer serialization regex)
  If (Boost_FOUND)
    Set(Boost_Avail 1)
  Else (Boost_FOUND)
    Set(Boost_Avail 0)
  EndIf (Boost_FOUND)
  Message("-- Looking for GSL ...")
  find_package(GSL)

endif(CBMROOT_MINIMAL)

# set a variable which should be used in all CMakeLists.txt
# Defines all basic include directories from fairbase
SetBasicVariables()

Set(BASE_INCLUDE_DIRECTORIES
${BASE_INCLUDE_DIRECTORIES} 
${SYSTEM_INCLUDE_DIRECTORIES} 
)

# Set the library version in the main CMakeLists.txt
SET(CBMROOT_MAJOR_VERSION 0)
SET(CBMROOT_MINOR_VERSION 0)
SET(CBMROOT_PATCH_VERSION 0)
SET(CBMROOT_VERSION "${CBMROOT_MAJOR_VERSION}.${CBMROOT_MINOR_VERSION}.${CBMROOT_PATCH_VERSION}")
If(NOT ROOT_FOUND_VERSION OR ROOT_FOUND_VERSION LESS 59999)
  SET(FAIRROOT_LIBRARY_PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES}
      VERSION "${CBMROOT_VERSION}"
      SOVERSION "${CBMROOT_MAJOR_VERSION}"
      SUFFIX ".so"
  )
Else()
  SET(FAIRROOT_LIBRARY_PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES}
      VERSION "${CBMROOT_VERSION}"
      SOVERSION "${CBMROOT_MAJOR_VERSION}"
  )
EndIf()

SET(CBMLIBDIR ${CMAKE_BINARY_DIR}/lib)  
SET(LD_LIBRARY_PATH  ${CBMLIBDIR} ${Boost_LIBRARY_DIRS} ${LD_LIBRARY_PATH})


Generate_Version_Info()

# Recurse into the given subdirectories.  This does not actually
# cause another cmake executable to run.  The same process will walk through
# the project's entire directory structure.

set(BUILD_BEAMTIME FALSE CACHE BOOL "Build everything needed (DABC, RocLib ...) to read testbeam data.")
If(BUILD_BEAMTIME)
  Set(BUILD_MBS FALSE)
EndIf(BUILD_BEAMTIME)

IF(NOT FAIRROOT_FOUND)

add_subdirectory (fairtools)
add_subdirectory (MbsAPI)
add_subdirectory (base)
add_subdirectory (dbase)
add_subdirectory (geobase)
add_subdirectory (parbase) 
add_subdirectory (generators)
add_subdirectory (geane)
add_subdirectory (trackbase)
ENDIF (NOT FAIRROOT_FOUND)

Execute_Process(COMMAND ${ROOT_CONFIG_EXECUTABLE} --has-vc
                OUTPUT_VARIABLE ROOT_HAS_VC
               )
String(STRIP ${ROOT_HAS_VC} ROOT_HAS_VC)

If(NOT ${ROOT_HAS_VC})
  Message(STATUS "Root does not support VC. Compile the internal version.")
  ExternalProject_Add(vc
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/vc_074
    PREFIX vc
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
  )
  Add_Library(Vc STATIC IMPORTED)
  Set_Property(TARGET Vc PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/lib/libVc.a)
  Set(VC_INCLUDE_DIRS       
      ${CBMROOT_SOURCE_DIR}/vc_074/ 
      ${CBMROOT_SOURCE_DIR}/vc_074/include/
     )
  Install(FILES $${CMAKE_BINARY_DIR}/lib/libVc.a DESTINATION lib)
EndIf()

If(ROOT_VERSION_MAJOR VERSION_LESS 6 )
  # Test is we use the new FairSource. The difference is the signature.
  # Compile test code. If the code compiles we use the new FairSource.
  Message(STATUS "Checking for FairSource version")
  Execute_process(COMMAND ${CMAKE_CXX_COMPILER} -c 
                          ${CMAKE_SOURCE_DIR}/test/testcode/CbmTestSource.cxx
                          -I${ROOT_INCLUDE_DIR} -I${FAIRROOTPATH}/include
                          -I${CMAKE_SOURCE_DIR}/base/source
                  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                  RESULT_VARIABLE OldFairSource
                  ERROR_QUIET
                 )
Else()
  Set(ROOT6 TRUE)
  Set(OldFairSource FALSE)
EndIf()

If(OldFairSource)
  Message(STATUS "Found old FairSource")
Else()
  Message(STATUS "Found new FairSource")
EndIf()


add_subdirectory (cbmbase)
add_subdirectory (cbmdata)
add_subdirectory (field)  
add_subdirectory (passive)
add_subdirectory (rich) 
add_subdirectory (mvd) 
add_subdirectory (sts) 
add_subdirectory (tof) 
add_subdirectory (trd) 
add_subdirectory (ecal) 
add_subdirectory (psd)
add_subdirectory (much)
#add_subdirectory (lhetrack) 
add_subdirectory (littrack) 
add_subdirectory (htrack)
add_subdirectory (global) 
add_subdirectory (KF)
add_subdirectory (L1)
add_subdirectory (macro)
add_subdirectory (dilept)
#add_subdirectory (stt)
#####add_subdirectory (trdcatrack)
#add_subdirectory (tutorials/Tutorial1/src)
#add_subdirectory (tutorials/Tutorial2/src)
add_subdirectory (cbmgenerators)
add_subdirectory (run)
add_subdirectory (hadron)
add_subdirectory (analysis)
#add_subdirectory (vis)
add_subdirectory (tracking)

# Check if the compilation flag -std=c++11 is set
# In this case the code is compiled with the new standard and it is possible
# to compile the fles_ipc library
If(NOT CMAKE_CXX_FLAGS)
  Message(STATUS "No C++11 support found. FLES_IPC library will not be build.")
Else()
  String(FIND ${CMAKE_CXX_FLAGS} "-std=c++11" POS_C++11)
  If(${POS_C++11} EQUAL -1)
    Message(STATUS "No C++11 support found. FLES_IPC library will not be build.")
  Else()
    Message(STATUS "C++11 support found. Build FLES_IPC library.")
    add_subdirectory(fles)
    Find_Library(BaseMQ_FOUND NAME BaseMQ PATHS ${FAIRROOTPATH}/lib)
    If(BaseMQ_FOUND)
      Message(STATUS "BaseMQ library found. Build MQTest tutorials.")
      add_subdirectory(tutorials/MQTest)
    EndIf()
  EndIf()
EndIf()


Option(BUILD_UNITTESTS "Build all unit tests and add them as new tests" OFF)
If(BUILD_UNITTESTS)

  Message("Build the unit tests.")

  Set(GTEST_ROOT ${SIMPATH})
  Find_Package(GTest)

  If(GTEST_FOUND)
    Message("Add tests library and tests.")
    Add_Subdirectory(test)
  EndIf(GTEST_FOUND)
EndIf(BUILD_UNITTESTS)

Execute_Process(COMMAND ${ROOT_CONFIG_EXECUTABLE} --has-opengl
                OUTPUT_VARIABLE EveIsBuild
                OUTPUT_STRIP_TRAILING_WHITESPACE
               )

If(${EveIsBuild} STREQUAL yes)
  Message(STATUS "Build the eventdisplay.")
IF(NOT FAIRROOT_FOUND)
  add_subdirectory (eventdisplay)
ENDIF(NOT FAIRROOT_FOUND)

  add_subdirectory (cbmdisplay)

Else(${EveIsBuild} STREQUAL yes)
  Message(STATUS "The eventdisplay is not build, there is no OpenGL support.")
EndIf(${EveIsBuild} STREQUAL yes)

#set(BUILD_BEAMTIME FALSE CACHE BOOL "Build everything needed (DABC, RocLib ...) to read testbeam data.")
If(BUILD_BEAMTIME)
#  add_subdirectory (roclight)
  add_subdirectory (roc)
  add_subdirectory (beamtime)
EndIf(BUILD_BEAMTIME)


Option(BUILD_DOXYGEN "Build Doxygen" OFF)
if(BUILD_DOXYGEN)
  MESSAGE(STATUS "*** Building the Doxygen documentaion ***")
  ADD_SUBDIRECTORY(doxygen)
endif(BUILD_DOXYGEN)

if(RULE_CHECKER_FOUND)

  ADD_CUSTOM_TARGET(RULES 
     COMMAND ${RULE_CHECKER_SCRIPT1} ${CMAKE_BINARY_DIR} viol > violations.html
     DEPENDS $ENV{ALL_RULES})

endif(RULE_CHECKER_FOUND)

if(IWYU_FOUND)

  ADD_CUSTOM_TARGET(checkHEADERS
     DEPENDS $ENV{ALL_HEADER_RULES}
  )

endif(IWYU_FOUND)

if(NOT CBMROOT_MINIMAL)
  WRITE_CONFIG_FILE(config.sh)
  WRITE_CONFIG_FILE(config.csh)

  SET(VMCWORKDIR ${CMAKE_INSTALL_PREFIX}/share/cbmroot)
  WRITE_CONFIG_FILE(config.sh_install)
  WRITE_CONFIG_FILE(config.csh_install)

else(NOT CBMROOT_MINIMAL)
  SET(FULL_CONFIG_FILE "false")
  WRITE_CONFIG_FILE(config.sh)
endif(NOT CBMROOT_MINIMAL)


Install(FILES ${CMAKE_BINARY_DIR}/config.sh_install
        DESTINATION bin
        RENAME CbmRootConfig.sh
       )

Install(FILES ${CMAKE_BINARY_DIR}/check_system.sh
        DESTINATION bin
       )

Install(FILES ${CMAKE_BINARY_DIR}/config.csh_install
        DESTINATION bin
        RENAME CbmRootConfig.csh
       )


Install(DIRECTORY gconfig geometry input parameters
        DESTINATION share/cbmroot
        PATTERN ".svn" EXCLUDE)
