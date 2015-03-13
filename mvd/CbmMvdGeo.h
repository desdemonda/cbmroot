// -------------------------------------------------------------------------
// -----                       CbmMvdGeo header file                   -----
// -----                  Created 06/11/06  by V. Friese               -----
// -------------------------------------------------------------------------


/**  CbmMvdGeo.h
 *@author V.Friese <v.friese@gsi.de>
 *
 * Geometry structure of Detector MVD. I am not very sure what this class
 * does, so I just copied from STS.
 *
 **/


#ifndef CBMMVDGEO_H
#define CBMMVDGEO_H 1


#include "FairGeoSet.h"


class  CbmMvdGeo : public FairGeoSet {

 public:

  /** Constructor **/
  CbmMvdGeo();


  /** Destructor **/
  virtual ~CbmMvdGeo();


  /** Name of modules (stations) **/
  virtual const char* getModuleName(Int_t);


  /** Name of elements (whatever that may be) **/
  virtual const char* getEleName(Int_t);


  /** Module index from module name **/
  virtual inline Int_t getModNumInMod(const TString&);


 protected:
  
  char modName[20];  // name of module
  char eleName[20];  // substring for elements in module


  ClassDef(CbmMvdGeo,0);

};


inline Int_t CbmMvdGeo::getModNumInMod(const TString& name) {
  return (Int_t)(name[3]-'0')-1;
}


#endif
