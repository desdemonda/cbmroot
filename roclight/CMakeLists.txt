# Create a library called "libCbmFlibReader" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(INCLUDE_DIRECTORIES
  ${CBMROOT_SOURCE_DIR}/roclight
  ${CBMROOT_SOURCE_DIR}/roclight/roc
  ${CBMROOT_SOURCE_DIR}/roclight/src_roc
  ${CBMROOT_SOURCE_DIR}/roclight/feet
)

Set(SYSTEM_INCLUDE_DIRECTORIES
#  ${BASE_INCLUDE_DIRECTORIES} 
  ${Boost_INCLUDE_DIR} 
#  ${ROOT_INCLUDE_DIR}  
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})
 

#Set(LINK_DIRECTORIES
#  ${ROOT_LIBRARY_DIR}
#  ${FAIRROOT_LIBRARY_DIR}
#  ${Boost_LIBRARY_DIRS}
#)
 
#Link_Directories( 
#  ${LINK_DIRECTORIES}
#)

Set(SRCS
  src_roc/Message.cxx
  src_roc/Board.cxx
  src_roc/Iterator.cxx
)

#Set(HEADERS
#  roc/Message.h
#  roc/Board.h
#  roc/Iterator.h
#)

#If(NOT OldFairSource)
#  Add_Definitions(-D_NewFairSource)
#  Set(DEFINITIONS _NewFairSource)
#EndIf()

#Set(LINKDEF  RocLibLinkDef.h )
Set(LIBRARY_NAME RocLight)
#Set(DEPENDENCIES Core)

GENERATE_LIBRARY()
