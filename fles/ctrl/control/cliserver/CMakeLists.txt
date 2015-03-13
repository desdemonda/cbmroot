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
cliserver.cpp
)


Set(EXE_NAME cliserver)
Set(DEPENDENCIES boost_system boost_thread zmq cbmnetutil cbmnetcntlserver)

GENERATE_EXECUTABLE()
