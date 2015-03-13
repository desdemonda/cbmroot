//*-- AUTHOR : Ilse Koenig
//*-- Created : 10/11/2003

/////////////////////////////////////////////////////////////
// CbmGeoTrd
//
// Class for geometry of TRD
//
/////////////////////////////////////////////////////////////

#include "CbmGeoTrd.h"

ClassImp(CbmGeoTrd)

CbmGeoTrd::CbmGeoTrd() {
  // Constructor
  fName="trd";
  maxSectors=0;
  maxModules=6;
}

const char* CbmGeoTrd::getModuleName(Int_t m) {
  // Returns the module name of trd number m
  sprintf(modName,"trd%i",m+1);
  return modName;
}

const char* CbmGeoTrd::getEleName(Int_t m) {
  // Returns the element name of trd number m
  sprintf(eleName,"trd%i",m+1);
  return eleName;
}
