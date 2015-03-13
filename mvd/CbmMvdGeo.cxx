// -------------------------------------------------------------------------
// -----                       CbmMvdGeo source file                   -----
// -----                  Created 06/11/06  by V. Friese               -----
// -------------------------------------------------------------------------
#include "CbmMvdGeo.h"

#include "FairGeoNode.h"

#include <iostream>

using std::cout;
using std::endl;


// -----   Constructor   ---------------------------------------------------
CbmMvdGeo::CbmMvdGeo() {
  fName      = "mvd";
  maxSectors =  0;
  maxModules = 99;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmMvdGeo::~CbmMvdGeo() { }
// -------------------------------------------------------------------------



// -----   Virtual public method getModuleName   ---------------------------
const char* CbmMvdGeo::getModuleName(Int_t m) {
  if ( m < 0 ) {
    cout <<"-E- CbmMvdGeo::getModuleName: Illegal module number " 
	 << m << endl;
    return "";
  }
  if ( m < 9 ) sprintf(modName,"mvdstation0%i",m+1);
  else         sprintf(modName,"mvdstation%i", m+1);
  return modName;
}
// -------------------------------------------------------------------------



// -----   Virtual public method getEleName   ------------------------------
const char* CbmMvdGeo::getEleName(Int_t m) {
  sprintf(eleName,"mvd%i",m+1);
  return eleName;
}
// -------------------------------------------------------------------------


ClassImp(CbmMvdGeo)
