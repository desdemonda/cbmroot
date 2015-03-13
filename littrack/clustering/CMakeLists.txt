# Create a library called "libClustering".

Set(INCLUDE_DIRECTORIES
${CBMROOT_SOURCE_DIR}/littrack/clustering

${CBMROOT_SOURCE_DIR}/cbmbase
${CBMROOT_SOURCE_DIR}/cbmbase/draw

${CBMROOT_SOURCE_DIR}/cbmdata
${CBMROOT_SOURCE_DIR}/cbmdata/sts
${CBMROOT_SOURCE_DIR}/cbmdata/much
#${CBMROOT_SOURCE_DIR}/cbmdata/mvd

${CBMROOT_SOURCE_DIR}/much/geo

#${CBMROOT_SOURCE_DIR}/mvd
)

Include_Directories( ${INCLUDE_DIRECTORIES})

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${BASE_INCLUDE_DIRECTORIES}
)

Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
${GEANT4_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

set(SRCS
CbmClusteringGeometry.cxx
CbmMuchClustering.cxx
CbmClusteringA1.cxx
CbmClusteringSL.cxx
CbmClusteringWard.cxx
CbmMuchTest.cxx
#CbmMvdGeoCl.cxx
#CbmMvdClustering.cxx
#CbmMvdClusteringNoCharges.cxx
)

set(HEADERS
#CbmClusteringGeometry.h
CbmMuchClustering.h
#CbmClusteringA1.h
#CbmClusteringSL.h
#CbmClusteringWard.h
CbmMuchTest.h
#CbmMvdGeoCl.h
#CbmMvdClustering.h
#CbmMvdClusteringNoCharges.h
)

IF (SSE_FOUND)
  ADD_DEFINITIONS(-DHAVE_SSE)
  SET_SOURCE_FILES_PROPERTIES(${SRCS} PROPERTIES COMPILE_FLAGS 
  "-msse -O3 -w")
  Message(STATUS "Clustering will be compiled with SSE support")
ELSE (SSE_FOUND)
  Message(STATUS "Clustering will be compiled without SSE support")
  SET_SOURCE_FILES_PROPERTIES(${SRCS} PROPERTIES COMPILE_FLAGS 
  "-O3 -w")
ENDIF (SSE_FOUND)

If (Boost_FOUND)
  set(DEFINITIONS BOOST_FOUND)
EndIf (Boost_FOUND)

set(LINKDEF ClusteringLinkDef.h)
Set(LIBRARY_NAME Clustering)
Set(DEPENDENCIES
    Much Mvd CbmBase CbmData Base 
)

GENERATE_LIBRARY()


