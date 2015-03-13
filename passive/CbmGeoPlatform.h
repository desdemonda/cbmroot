#ifndef CBMGEOPLATFORM_H
#define CBMGEOPLATFORM_H

#include "FairGeoSet.h"

class  CbmGeoPlatform : public FairGeoSet {
protected:
  char modName[2];  // name of module
  char eleName[2];  // substring for elements in module
public:
  CbmGeoPlatform();
  ~CbmGeoPlatform() {}
  const char* getModuleName(Int_t) {return modName;}
  const char* getEleName(Int_t)    {return eleName;}
  ClassDef(CbmGeoPlatform,0) // Class for the geometry of Platform
};

#endif  /* !CBMGEOPLATFORM_H */
