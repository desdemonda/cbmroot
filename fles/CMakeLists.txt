# This file is only called if the compiler provides c++11 support


#file(GLOB _BLA RELATIVE ipc *.cpp)
#ExternalProject_Add(ipc_external
#    SOURCE_DIR ${CMAKE_SOURCE_DIR}/fles/ipc
#    PREFIX fles_ipc
#    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
#)
#Add_Library(fles_ipc UNKNOWN IMPORTED)
#Set_Property(TARGET fles_ipc PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/lib/libfles_ipc.a)
#Set_Property(TARGET fles_ipc PROPERTY SOURCES ${_BLA})
#add_dependencies(fles_ipc ipc_external)

#ExternalProject_Add(flib_dpb_external
#    SOURCE_DIR ${CMAKE_SOURCE_DIR}/fles/flib_dpb
#    PREFIX flib_dpb
#    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
#)
#Add_Library(flib_dpb UNKNOWN IMPORTED)
#Set_Property(TARGET flib_dpb PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/lib/libflib_dpb.a)
#add_dependencies(flib_dpb flib_dpb_external)
#
#ExternalProject_Add(spadic_message_external
#    SOURCE_DIR ${CMAKE_SOURCE_DIR}/fles/spadic
#    PREFIX spadic_message
#    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
#)
#Add_Library(spadicMessage STATIC IMPORTED)
#Set_Property(TARGET spadicMessage PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/lib/libspadicMessage.a)
#add_dependencies(spadicMessage spadic_message_external)

Include(Build_Fles_Ipc.cmake)
Include(Build_Flib_Dpb.cmake)
Include(Build_Spadic_Message.cmake)


Add_Subdirectory(tsclient)
Add_Subdirectory(trb)
Add_Subdirectory(softdpb)
If(NOT APPLE)
  Add_Subdirectory(ctrl)
  Add_Subdirectory(scv3)
EndIf()
Add_Subdirectory(reader)
