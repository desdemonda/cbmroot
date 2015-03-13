# Create a library called "libBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(INCLUDE_DIRECTORIES
  ${CBMROOT_SOURCE_DIR}/field 
)

Include_Directories( ${INCLUDE_DIRECTORIES})

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${BASE_INCLUDE_DIRECTORIES} 
)

Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})


set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}

)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
CbmFieldConst.cxx     
CbmFieldContFact.cxx    
CbmFieldMap.cxx
CbmFieldMapCreator.cxx         
CbmFieldMapData.cxx     
CbmFieldMapSym2.cxx     
CbmFieldMapSym3.cxx     
CbmFieldPar.cxx         
CbmBsField.cxx 
CbmFieldCreator.cxx 
CbmFieldMapDistorted.cxx       
CbmFieldMapSym1.cxx     
)

set(LINKDEF FieldLinkDef.h)
Set(LIBRARY_NAME Field)
Set(DEPENDENCIES
    Base 
)

GENERATE_LIBRARY()

