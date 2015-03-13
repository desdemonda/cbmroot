#ifndef CBMGEOMAGNET_H
#define CBMGEOMAGNET_H

#include "FairGeoSet.h"

class  CbmGeoMagnet : public FairGeoSet {
protected:
  char modName[2];  // name of module
  char eleName[2];  // substring for elements in module
public:
  CbmGeoMagnet();
  ~CbmGeoMagnet() {}
  const char* getModuleName(Int_t) {return modName;}
  const char* getEleName(Int_t) {return eleName;}
  ClassDef(CbmGeoMagnet,0) // Class for the geometry of Magnet
};

#endif  /* !CBMGEOMAGNET_H */
