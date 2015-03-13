# Build the tsclient executable  which is taken from
# git@code.compeng.uni-frankfurt.de:flesnet.git
# This executable uses the fles_ipc library to read files
# in the tca format
# This file is only called if the compiler provides c++11 support

# Add some extra compile warnings 
Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -Winit-self -Wundef -Wold-style-cast -Woverloaded-virtual -Wwrite-strings -Wnon-virtual-dtor")

Set(INCLUDE_DIRECTORIES
${CBMROOT_CURRENT_SOURCE_DIR}
${CBMROOT_SOURCE_DIR}/fles/ipc/src
)

Include_Directories( ${INCLUDE_DIRECTORIES})

Include_Directories(SYSTEM 
  ${Boost_INCLUDE_DIR}  
  ${ZMQ_INCLUDE_DIR}
)

Set(LINK_DIRECTORIES
  ${Boost_LIBRARY_DIRS}
  ${ZMQ_LIBRARY_DIR}
)
 
Link_Directories( ${LINK_DIRECTORIES})

Set(SRCS
main.cpp
Application.cpp
Parameters.cpp
TrbBridge.cpp
TrbBridgeStatistics.cpp
TrbBridgeTrbNetHeaders.cpp
TrbBridgeFrame.cpp
TrbBridgeTransaction.cpp
)

Set(EXE_NAME trbnet_client)
If(UNIX AND NOT APPLE)
  Set(DEPENDENCIES fles_ipc boost_program_options ${CMAKE_THREAD_LIBS_INIT} zmq rt)
Else()
  Set(DEPENDENCIES fles_ipc boost_program_options ${CMAKE_THREAD_LIBS_INIT} zmq)
EndIf()

GENERATE_EXECUTABLE()

