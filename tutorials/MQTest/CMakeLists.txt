# Create a library called "MQcbmExample" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.



Set(INCLUDE_DIRECTORIES
${CBMROOT_SOURCE_DIR}/fairmq
${CBMROOT_SOURCE_DIR}/MQ
${CBMROOT_SOURCE_DIR}/fairmq/zeromq
${CBMROOT_SOURCE_DIR}/cbmbase
${CBMROOT_SOURCE_DIR}/run
${CBMROOT_SOURCE_DIR}/tutorials/MQTest
${CBMROOT_SOURCE_DIR}/tutorials/MQTest/data
${CBMROOT_SOURCE_DIR}/fles/ipc/src
${CBMROOT_SOURCE_DIR}/tutorials/MQTest/MQDevices
${CBMROOT_SOURCE_DIR}/tutorials/MQTest/MQTask
${CBMROOT_SOURCE_DIR}/tutorials/MQTest/SubTask
${CBMROOT_SOURCE_DIR}/cbmdata
${CBMROOT_SOURCE_DIR}/cbmdata/beamtime
${CBMROOT_SOURCE_DIR}/cbmdata/beamtime/fhodo
${CBMROOT_SOURCE_DIR}/cbmdata/ecal
${CBMROOT_SOURCE_DIR}/cbmdata/global
${CBMROOT_SOURCE_DIR}/cbmdata/much
${CBMROOT_SOURCE_DIR}/cbmdata/mvd
${CBMROOT_SOURCE_DIR}/cbmdata/psd
${CBMROOT_SOURCE_DIR}/cbmdata/rich
${CBMROOT_SOURCE_DIR}/cbmdata/sts
${CBMROOT_SOURCE_DIR}/cbmdata/stt
${CBMROOT_SOURCE_DIR}/cbmdata/tof
${CBMROOT_SOURCE_DIR}/cbmdata/trd
)

Include_Directories(${INCLUDE_DIRECTORIES})

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${BASE_INCLUDE_DIRECTORIES}
  ${ROOT_INCLUDE_DIR}
  ${ZMQ_INCLUDE_DIR}
  ${Boost_INCLUDE_DIR}
)

Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

If (Boost_FOUND)
#  configure_file( ${CBMROOT_SOURCE_DIR}/tutorials/MQTest/macro/startPushPull_cbm.sh.in ${CMAKE_BINARY_DIR}/bin/startPushPull_cbm.sh )
  configure_file( ${CBMROOT_SOURCE_DIR}/tutorials/MQTest/macro/startMultiSamplerPushPull_cbm.sh.in ${CMAKE_BINARY_DIR}/bin/startMultiSamplerPushPull_cbm.sh )  
#  configure_file( ${CBMROOT_SOURCE_DIR}/tutorials/MQTest/macro/startCbmSampler.sh.in ${CMAKE_BINARY_DIR}/bin/startCbmSampler.sh )
#  configure_file( ${CBMROOT_SOURCE_DIR}/tutorials/MQTest/macro/startCbmMerger.sh.in ${CMAKE_BINARY_DIR}/bin/startCbmMerger.sh )
#  configure_file( ${CBMROOT_SOURCE_DIR}/tutorials/MQTest/macro/startCbmSink.sh.in ${CMAKE_BINARY_DIR}/bin/startCbmSink.sh )
EndIf (Boost_FOUND)

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${Boost_LIBRARY_DIRS}
${FAIRROOT_LIBRARY_DIR}
${CBMLIBDIR}
) 

link_directories(${LINK_DIRECTORIES})

#Set(SRCS
#  ${CBMROOT_SOURCE_DIR}/run/CbmTimeSlice.cxx
#)

If (Boost_FOUND)
  Set(NO_DICT_SRCS
    MQDevices/CbmMQFileSink.cxx
    data/CbmMicroSlice.cxx
    MQDevices/CbmMicroSliceMerger.cxx
    MQTask/CbmMicroSliceMergerTask.cxx
    SubTask/CbmDataConverterTask.cxx
    SubTask/CbmMuchDataConverter.cxx
    SubTask/CbmStsDataConverter.cxx
    SubTask/CbmDigiConverter.cxx
  )
EndIf (Boost_FOUND)




Set(LINKDEF CbmMqExampleLinkDef.h)
Set(LIBRARY_NAME MQcbmExample)

If (Boost_FOUND)
  Set(DEPENDENCIES 
    Base ParBase FairTools CbmBase Run
    CbmData Rich Trd Tof Psd CbmGenerators Field Sts
    fles_ipc FairMQ 
    boost_thread boost_system boost_serialization boost_timer Passive
    RIO Core
  )
  If (FAIRBASE) 
    Set(DEPENDENCIES ${DEPENDENCIES} FairMQSampler)
  EndIf()
Else (Boost_FOUND)
  message( FATAL_ERROR "Boost library required to compile the MQ example" )
EndIf (Boost_FOUND)

GENERATE_LIBRARY()


If (Boost_FOUND)
  Set(Exe_Names
    testCbmDetectorSampler_Boost
    testCbmStsDetectorSampler_Boost
    testCbmMuchDetectorSampler_Boost
    testCbmDetectorProcessor_Boost
    testCbmfileSink_Boost 
  )


  set(Exe_Source
    run/runTestDetectorSampler_cbmBoost.cxx
    run/runCbmStsDetectorSamplerBoost.cxx
    run/runCbmMuchDetectorSamplerBoost.cxx
    run/runTestDetectorProcessor_cbmBoost.cxx
    run/runFileSink_cbmBoost.cxx
    )


  List(LENGTH Exe_Names _length)
  Math(EXPR _length ${_length}-1)

  ForEach(_file RANGE 0 ${_length})
    List(GET Exe_Names ${_file} _name)
    List(GET Exe_Source ${_file} _src)
    Set(EXE_NAME ${_name})
    Set(SRCS ${_src})
    Set(DEPENDENCIES MQcbmExample Mvd )
    If (NOT FAIRBASE) 
      Set(DEPENDENCIES ${DEPENDENCIES} BaseMQ)
    EndIf()

    GENERATE_EXECUTABLE()
  EndForEach(_file RANGE 0 ${_length})
EndIf (Boost_FOUND)
