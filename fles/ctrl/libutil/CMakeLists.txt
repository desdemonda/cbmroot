# Build the fles_ipc library which is tacken from git@code.compeng.uni-frankfurt.de:flesnet.git
# This library provides the interface to the FLIB board or to files in tca
# format written by the FLIB 
#
# This file is only called if the compiler provides c++11 support

# Add some extra compile warnings 
#Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -Winit-self -Wundef -Wold-style-cast -Woverloaded-virtual -Wwrite-strings -Wnon-virtual-dtor")

Set(INCLUDE_DIRECTORIES
${CBMROOT_CURRENT_SOURCE_DIR}
${CBMROOT_SOURCE_DIR}/fles/ctrl
)

Set(SYSTEM_INCLUDE_DIRECTORIES
${Boost_INCLUDE_DIR}
${ZMQ_INCLUDE_DIR}
)

Include_Directories( ${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

Set(LINK_DIRECTORIES
${Boost_LIBRARY_DIRS}
)
 
Link_Directories( ${LINK_DIRECTORIES})

Set(SRCS
Rbits.cpp
RerrMsg.cpp
ReventFd.cpp
Rexception.cpp
RiosState.cpp
RlogFile.cpp
RlogFileDefault.cpp
RlogMsg.cpp
RosFill.cpp
RosPrintBvi.cpp
RosPrintfBase.cpp
RosPrintfS.cpp
Util.cpp
)


Set(LIBRARY_NAME cbmnetutil)
Set(DEPENDENCIES boost_system zmq)

GENERATE_LIBRARY()
