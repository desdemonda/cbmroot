# Create a library called "libCbmBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${BASE_INCLUDE_DIRECTORIES} 
  ${Boost_INCLUDE_DIR}
)

Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

Set(INCLUDE_DIRECTORIES
  ${CBMROOT_SOURCE_DIR}/cbmbase 
  ${CBMROOT_SOURCE_DIR}/cbmdata
  ${CBMROOT_SOURCE_DIR}/cbmdata/sts
  # needed by rootcling
  ${CBMROOT_SOURCE_DIR}/cbmbase/report
)
Include_Directories( ${INCLUDE_DIRECTORIES})

Set(LINK_DIRECTORIES
  ${ROOT_LIBRARY_DIR}
  ${FAIRROOT_LIBRARY_DIR}
  ${Boost_LIBRARY_DIRS}
#  ${CMAKE_SOURCE_DIR}/base
)
 
link_directories( ${LINK_DIRECTORIES})

Set(SRCS
CbmAddress.cxx
CbmModuleList.cxx

#CbmDigiManager.cxx
CbmPropagator.cxx

CbmTrdTrackFinder.cxx  
CbmTrdTrackFitter.cxx  

CbmStsTrackFinder.cxx 
CbmStsTrackFitter.cxx 

CbmMuchTrackFinder.cxx

CbmRichRingFinder.cxx 
CbmRichRingFitter.cxx 
CbmRichTrackExtrapolation.cxx 
CbmRichRingTrackAssign.cxx 
CbmRichRingSelect.cxx 

CbmGlobalTrackFitter.cxx
CbmPrimaryVertexFinder.cxx
CbmRichMerger.cxx
CbmTofMerger.cxx
CbmTrackMerger.cxx

CbmMCMatch.cxx
CbmMCMatchCreatorTask.cxx
CbmMCMatchLoaderTask.cxx
CbmMCMatchSelectorTask.cxx
CbmMCObject.cxx
CbmMCStage.cxx
CbmMCResult.cxx
#CbmMCTestMomentumCompare.cxx
CbmMCTestHitCompare.cxx
CbmMCEntry.cxx
CbmMCList.cxx
CbmMCDataArray.cxx
CbmMCDataManager.cxx

CbmModule.cxx

CbmHistManager.cxx
draw/CbmDrawHist.cxx
report/CbmReport.cxx      
report/CbmStudyReport.cxx
report/CbmSimulationReport.cxx
report/CbmTextReportElement.cxx
report/CbmHtmlReportElement.cxx       
report/CbmLatexReportElement.cxx

utils/CbmUtils.cxx

CbmMatchRecoToMC.cxx
)

# Better way to check for presence of print or Print
# Does not work with trunk version of fairbase
#Include(CheckIncludeFiles)
#Set(CMAKE_REQUIRED_INCLUDES ${INCLUDE_DIRECTORIES})
#Check_Include_Files(CbmMCEntry.h HAVE_LOWERCASE_PRINT)

If(EXISTS ${CMAKE_SOURCE_DIR}/base/event/FairMultiLinkedData.h)
  Execute_Process(COMMAND grep -c print ${CMAKE_SOURCE_DIR}/base/event/FairMultiLinkedData.h 
                  OUTPUT_VARIABLE NR_OF_print 
                 )  

  If(NR_OF_print GREATER 0)
    Set(HAVE_LOWERCASE_PRINT TRUE)
  EndIf(NR_OF_print GREATER 0)

  If(${HAVE_LOWERCASE_PRINT})
    Message(STATUS "FairMultiLinkedData has lowercase print function")
    Add_Definitions(-DHAVE_LOWERCASE_PRINT)
  Else(${HAVE_LOWERCASE_PRINT})
    Message(STATUS "FairMultiLinkedData has upercase print function")
  EndIf(${HAVE_LOWERCASE_PRINT})
Else()
  Message(STATUS "FairMultiLinkedData has upercase print function")
EndIf()

Execute_Process(COMMAND ${ROOT_CONFIG_EXECUTABLE} --has-gdml
                OUTPUT_VARIABLE ROOT_HAS_GDML 
)

If ( ${ROOT_HAS_GDML} STREQUAL yes )
  Message(STATUS "Build CbmModule with Gdml support")
  Add_Definitions(-DROOT_HAS_GDML)
EndIf ( ${ROOT_HAS_GDML} STREQUAL yes )



Set(LINKDEF CbmBaseLinkDef.h)
Set(LIBRARY_NAME CbmBase)
Set(DEPENDENCIES
    CbmData Base boost_regex
)

If ( ${ROOT_HAS_GDML} STREQUAL yes )
 Set (DEPENDENCIES ${DEPENDENCIES} Gdml)
EndIf ( ${ROOT_HAS_GDML} STREQUAL yes )

GENERATE_LIBRARY()


