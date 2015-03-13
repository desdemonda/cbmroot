# Build the tsclient executable  which is taken from
# git@code.compeng.uni-frankfurt.de:flesnet.git
# This executable uses the fles_ipc library to read files
# in the tca format
# This file is only called if the compiler provides c++11 support

Set(INCLUDE_DIRECTORIES
  ${CBMROOT_CURRENT_SOURCE_DIR}
)

Include_Directories( ${INCLUDE_DIRECTORIES})


Set(SRCS
evt.c
hadtu.c
hldread.c
showevt.c
subevt.c
)

Set(EXE_NAME daq_anal)
#If(UNIX AND NOT APPLE)
#  Set(DEPENDENCIES fles_ipc boost_thread boost_system boost_serialization boost_program_options ${CMAKE_THREAD_LIBS_INIT} rt)
#Else()
#  Set(DEPENDENCIES fles_ipc boost_thread boost_system boost_serialization boost_program_options ${CMAKE_THREAD_LIBS_INIT})
#  Set(DEPENDENCIES fles_ipc boost_program_options ${CMAKE_THREAD_LIBS_INIT})
#EndIf()

GENERATE_EXECUTABLE()

