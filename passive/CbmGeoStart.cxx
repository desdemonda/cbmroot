//*-- AUTHOR : Ilse Koenig
//*-- Created : 12/12/2003

/////////////////////////////////////////////////////////////
// CbmGeoStart
//
// Class for geometry of START
//
/////////////////////////////////////////////////////////////

#include "CbmGeoStart.h"

ClassImp(CbmGeoStart)

CbmGeoStart::CbmGeoStart() 
: FairGeoSet(),
  modName(),
  eleName()
{
  // Constructor
  fName="start";
  maxSectors=0;
  maxModules=6;
}

const char* CbmGeoStart::getModuleName(Int_t m) {
  // Return the module name in module m
  sprintf(modName,"IST%i",m);
  return modName;
}

const char* CbmGeoStart::getEleName(Int_t m) {
  // Return the element name in module m
  sprintf(eleName,"I%i",m);
  return eleName;
}
