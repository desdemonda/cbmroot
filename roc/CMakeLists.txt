# copy files from source directory to build directory

  Add_Custom_Command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/copy_okay
                     COMMAND cmake -E copy_directory  
                             ${CMAKE_CURRENT_SOURCE_DIR}
                             ${CMAKE_CURRENT_BINARY_DIR}/roc
                     COMMAND touch copy_okay
                     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                   )

  # Unpack the dabc tarball and patch the needed files
  Add_Custom_Command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/unpack_okay
                   COMMAND tar -xzf dabcroc19.tar.gz
                   COMMAND touch ${CMAKE_CURRENT_BINARY_DIR}/unpack_okay
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/roc/dabc
                   DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/copy_okay
                   )



If(CMAKE_SYSTEM_NAME MATCHES Darwin)
  Add_Custom_Command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/patch1_okay
                   COMMAND patch -p0 < roclib_macos.patch
                   COMMAND touch ${CMAKE_CURRENT_BINARY_DIR}/patch1_okay
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/roc
                   DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/unpack_okay
                   )
  Exec_Program("sw_vers -productVersion | cut -d . -f 1-2" OUTPUT_VARIABLE MAC_OS_VERSION)
  String(COMPARE EQUAL "10.9" "${MAC_OS_VERSION}" MAC_OS_10_9)
  If(MAC_OS_10_9)
    Add_Custom_Command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/patch_okay
                  COMMAND patch -p0 < roclib_macos_109.patch
                  COMMAND touch ${CMAKE_CURRENT_BINARY_DIR}/patch_okay
                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/roc
                  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/patch1_okay
                  )
  Else(MAC_OS_10_9)
    Add_Custom_Command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/patch_okay
                  COMMAND touch ${CMAKE_CURRENT_BINARY_DIR}/patch_okay
                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/roc
                  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/patch1_okay
                  )
  EndIf(MAC_OS_10_9)
Else(CMAKE_SYSTEM_NAME MATCHES Darwin)
  Add_Custom_Command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/patch_okay
                   COMMAND patch -p0 < roclib_linux.patch
                   COMMAND touch ${CMAKE_CURRENT_BINARY_DIR}/patch_okay
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/roc
                   DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/unpack_okay
                   )
EndIf(CMAKE_SYSTEM_NAME MATCHES Darwin)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/roc/lib/libRoc.so
                   COMMAND LD_LIBRARY_PATH=${ROOT_LIBRARY_DIR}
                           ROOTSYS=${ROOTSYS} make roclib 
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/roc
                   DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/patch_okay
                  )

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/roc/lib/libDabcBase.so
                   COMMAND make dabccore 
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/roc
                   DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/roc/lib/libRoc.so
                  )

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/roc/lib/libDabcRoc.so
                          ${CMAKE_CURRENT_BINARY_DIR}/roc/lib/libDabcMbs.so
                          ${CMAKE_CURRENT_BINARY_DIR}/roc/lib/libRocDict.rootmap
                          ${CMAKE_CURRENT_BINARY_DIR}/roc/lib/libRocDict.so
                   COMMAND LD_LIBRARY_PATH=${ROOT_LIBRARY_DIR}
                           ROOTSYS=${ROOTSYS} make rocdict
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/roc
                   DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/roc/lib/libDabcBase.so
                  )

add_custom_command(OUTPUT ${LIBRARY_OUTPUT_PATH}/libRoc.so
                          ${LIBRARY_OUTPUT_PATH}/libDabcBase.so
                          ${LIBRARY_OUTPUT_PATH}/libDabcRoc.so
                          ${LIBRARY_OUTPUT_PATH}/libDabcMbs.so
                          ${LIBRARY_OUTPUT_PATH}/libRocDict.rootmap
                          ${LIBRARY_OUTPUT_PATH}/libRocDict.so
                   COMMAND cmake -E copy_if_different lib/libRoc.so ${LIBRARY_OUTPUT_PATH}
                   COMMAND cmake -E copy_if_different lib/libDabcBase.so ${LIBRARY_OUTPUT_PATH}
                   COMMAND cmake -E copy_if_different lib/libDabcRoc.so ${LIBRARY_OUTPUT_PATH}
                   COMMAND cmake -E copy_if_different lib/libDabcMbs.so ${LIBRARY_OUTPUT_PATH}
                   COMMAND cmake -E copy_if_different lib/libRocDict.rootmap ${LIBRARY_OUTPUT_PATH}
                   COMMAND cmake -E copy_if_different lib/libRocDict.so ${LIBRARY_OUTPUT_PATH}
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/roc
                   DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/roc/lib/libRoc.so 
                           ${CMAKE_CURRENT_BINARY_DIR}/roc/lib/libDabcBase.so 
                           ${CMAKE_CURRENT_BINARY_DIR}/roc/lib/libDabcRoc.so 
                           ${CMAKE_CURRENT_BINARY_DIR}/roc/lib/libDabcMbs.so 
                           ${CMAKE_CURRENT_BINARY_DIR}/roc/lib/libRocDict.rootmap 
                           ${CMAKE_CURRENT_BINARY_DIR}/roc/lib/libRocDict.so
                  )

# create a target which will always be build and does actually nothing. The target is only
# needed that the dependencies are build, f they are not up to date. If everything is up to
# dte nothing is done. This target depends on the libAfterImage.a in the library directory of the
# build directory.

add_custom_target(RocLib ALL
                  DEPENDS ${LIBRARY_OUTPUT_PATH}/libRoc.so
                  DEPENDS ${LIBRARY_OUTPUT_PATH}/libDabcBase.so
                  DEPENDS ${LIBRARY_OUTPUT_PATH}/libDabcRoc.so
                  DEPENDS ${LIBRARY_OUTPUT_PATH}/libDabcMbs.so
                  DEPENDS ${LIBRARY_OUTPUT_PATH}/libRocDict.rootmap
                  DEPENDS ${LIBRARY_OUTPUT_PATH}/libRocDict.so
                 )

# import libDabcRoc.so which is not in the scope of CMake. This step makes it possible that
# make clean will remove the library

add_library(Roc SHARED IMPORTED GLOBAL)
add_library(DabcBase SHARED IMPORTED GLOBAL)
add_library(DabcRoc SHARED IMPORTED GLOBAL)
add_library(DabcMbs SHARED IMPORTED GLOBAL)
add_library(RocDict SHARED IMPORTED GLOBAL)

# CMake knows now where to find the target Roc
# We can use now the target Roc somewhere else in the project
# In our case this is needed to be able to link the CbmRocSource
Set_Property(TARGET Roc PROPERTY IMPORTED_LOCATION ${LIBRARY_OUTPUT_PATH}/libRoc.so) 
Set_Property(TARGET DabcBase PROPERTY IMPORTED_LOCATION ${LIBRARY_OUTPUT_PATH}/libDabcBase.so) 
Set_Property(TARGET DabcRoc PROPERTY IMPORTED_LOCATION ${LIBRARY_OUTPUT_PATH}/libDabcRoc.so) 
Set_Property(TARGET DabcMbs PROPERTY IMPORTED_LOCATION ${LIBRARY_OUTPUT_PATH}/libDabcMbs.so) 
Set_Property(TARGET RocDict PROPERTY IMPORTED_LOCATION ${LIBRARY_OUTPUT_PATH}/libRocDict.so) 
     
