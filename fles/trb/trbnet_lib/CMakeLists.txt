# Build the trbnet library  which is taken from
# git://jspc29.x-matter.uni-frankfurt.de/projects/cbmfles.git
# This executable uses the fles_ipc library to read timeslices
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
#  ${ZMQ_INCLUDE_DIR}
)

Set(LINK_DIRECTORIES
  ${Boost_LIBRARY_DIRS}
#  ${ZMQ_LIBRARY_DIR}
)
 
Link_Directories( ${LINK_DIRECTORIES})

Set(SRCS
TrbBridge.cpp
TrbBridgeStatistics.cpp
TrbBridgeTrbNetHeaders.cpp
TrbBridgeFrame.cpp
TrbBridgeTransaction.cpp
)


Set(LIBRARY_NAME trbnet)
#Set(DEPENDENCIES boost_)

GENERATE_LIBRARY()

