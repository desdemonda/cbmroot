Set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR}
${CBMROOT_CURRENT_SOURCE_DIR}
${CBMROOT_SOURCE_DIR}/fles/ctrl
${CBMROOT_SOURCE_DIR}/fles/ctrl/control
${CBMROOT_SOURCE_DIR}/fles/ctrl/control/libclient
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
ControlClient.cpp
ListPar.cpp
ListSeq.cpp
)


Set(LIBRARY_NAME cbmnetcntlclient)
Set(DEPENDENCIES boost_system zmq cbmnetutil)

GENERATE_LIBRARY()

SET(HEADERS
ListSeqItem.hpp
ListSeq.hpp
ListParItem.hpp
ListPar.hpp
ControlClient.hpp
${CBMROOT_SOURCE_DIR}/fles/ctrl/control/ControlProtocol.hpp
)

Set(LINKDEF ClientLinkDef.h)

Set(LIBRARY_NAME cbmnetcntlclientroot)
Set(DEPENDENCIES cbmnetcntlclient)

GENERATE_LIBRARY()
