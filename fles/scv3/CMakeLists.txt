Set(INCLUDE_DIRECTORIES
${CBMROOT_CURRENT_SOURCE_DIR}
${CBMROOT_SOURCE_DIR}/fles/ctrl
)

Include_Directories( ${INCLUDE_DIRECTORIES})
 
Link_Directories( ${LINK_DIRECTORIES})

Set(SRCS
cbmnet_registers.h
scv3_rf.cpp
dump_cnet_rf.cpp
)


Set(EXE_NAME dump_cnet_rf)
Set(DEPENDENCIES cbmnetcntlclient)

GENERATE_EXECUTABLE()
