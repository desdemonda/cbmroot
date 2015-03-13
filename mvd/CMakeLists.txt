# Create a library called "libBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(INCLUDE_DIRECTORIES
${CBMROOT_SOURCE_DIR}/mvd 

#needed by rootcling
${CBMROOT_SOURCE_DIR}/mvd/old
#${CBMROOT_SOURCE_DIR}/mvd/qa
#${CBMROOT_SOURCE_DIR}/mvd/tools
#${CBMROOT_SOURCE_DIR}/mvd/plugins
${CBMROOT_SOURCE_DIR}/mvd/plugins/tasks
${CBMROOT_SOURCE_DIR}/mvd/plugins/buffers
${CBMROOT_SOURCE_DIR}/mvd/SensorDataSheets

#${CBMROOT_SOURCE_DIR}/cbmbase 

${CBMROOT_SOURCE_DIR}/cbmdata
#${CBMROOT_SOURCE_DIR}/cbmdata/sts

${CBMROOT_SOURCE_DIR}/cbmdata/mvd
#${CBMROOT_SOURCE_DIR}/cbmdata/global


#${CBMROOT_SOURCE_DIR}/sts

#${CBMROOT_SOURCE_DIR}/KF
#${CBMROOT_SOURCE_DIR}/KF/Interface 

#${CBMROOT_SOURCE_DIR}/much/geo

#${CBMROOT_SOURCE_DIR}/rich

#${CBMROOT_SOURCE_DIR}/littrack
#${CBMROOT_SOURCE_DIR}/littrack/cbm
#${CBMROOT_SOURCE_DIR}/littrack/std/

)

Include_Directories( ${INCLUDE_DIRECTORIES})

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${BASE_INCLUDE_DIRECTORIES} 
  ${CLHEP_INCLUDE_DIR}
  ${GSL_INCLUDE_DIRS}
)

Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${GSL_LIBRARY_DIRS}
${FAIRROOT_LIBRARY_DIR}
${CLHEP_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

set(SRCS
plugins/tasks/CbmMvdSensorTask.cxx
plugins/tasks/CbmMvdSensorDigitizerTask.cxx
plugins/tasks/CbmMvdSensorFindHitTask.cxx
plugins/tasks/CbmMvdSensorHitfinderTask.cxx
plugins/tasks/CbmMvdSensorClusterfinderTask.cxx

plugins/buffers/CbmMvdSensorBuffer.cxx
plugins/buffers/CbmMvdSensorFrameBuffer.cxx
plugins/buffers/CbmMvdSensorTrackingBuffer.cxx
 
plugins/CbmMvdSensorPlugin.cxx

SensorDataSheets/CbmMvdMimosa26AHR.cxx
SensorDataSheets/CbmMvdMimosa34.cxx
SensorDataSheets/CbmMvdSensorDataSheet.cxx  

tools/CbmMvdGeoHandler.cxx

#qa/CbmMvdQa.cxx

CbmMvd.cxx 
CbmMvdContFact.cxx 
CbmMvdGeo.cxx 
CbmMvdGeoPar.cxx 
CbmMvdDigitizer.cxx  
CbmMvdPileupManager.cxx  
CbmMvdPixelCharge.cxx  
CbmMvdHitfinder.cxx
CbmMvdStationPar.cxx
CbmMvdDetector.cxx
CbmMvdSensor.cxx
CbmMvdClusterfinder.cxx
 
old/CbmMvdStation.cxx 
old/CbmMvdFindHits.cxx
old/CbmMvdDigitizeL.cxx 
old/CbmMvdHitProducer.cxx 
old/MyG4UniversalFluctuationForSi.cxx


)
#CbmMvdMatchTracks.cxx 


set(LINKDEF MvdLinkDef.h)
Set(LIBRARY_NAME Mvd)
Set(DEPENDENCIES
    CbmBase CbmData Base G4clhep
#    CbmBase CbmData Base G4clhep KF Littrack
)

GENERATE_LIBRARY()
