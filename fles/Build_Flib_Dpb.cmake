
Set(LIB_SOURCES flib_dpb/MicrosliceContents.cpp)
Set(LIB_HEADERS flib_dpb/MicrosliceContents.hpp)

Add_Library(flib_dpb SHARED ${LIB_SOURCES} ${LIB_HEADERS})
Set_Target_Properties(flib_dpb PROPERTIES  SUFFIX ".so")
