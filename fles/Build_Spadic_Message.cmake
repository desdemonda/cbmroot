# Build the spadic message reader library.
# The code is external in github as defined in the svn:extrenals.
# Add some extra compile warnings 

Set(CMAKE_CXX_FLAGS_BAK ${CMAKE_CXX_FLAGS})

Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -Winit-self -Wundef -Wold-style-cast -Woverloaded-virtual -Wwrite-strings -Wnon-virtual-dtor")

Set(INCLUDE_DIRECTORIES
${CBMROOT_SOURCE_DIR}/fles/spadic/message
#${CBMROOT_SOURCE_DIR}/fles/spadic/timeslice
${CBMROOT_SOURCE_DIR}/fles/spadic/message/wrap/cpp
${CBMROOT_SOURCE_DIR}/fles/ipc
${CBMROOT_SOURCE_DIR}/fles/flib_dpb
)

Set(SYSTEM_INCLUDE_DIRECTORIES
${Boost_INCLUDE_DIR}
)

Include_Directories( ${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

Set(LINK_DIRECTORIES
${Boost_LIBRARY_DIRS}
)
 
Link_Directories( ${LINK_DIRECTORIES})

Set(SRCS
spadic/message/message.c
spadic/message/message_reader.c
spadic/message/wrap/cpp/Message.cpp
)


Set(LIBRARY_NAME spadicMessage)
Set(DEPENDENCIES)

GENERATE_LIBRARY()

