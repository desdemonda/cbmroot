#ifndef CBMGEOSTART_H
#define CBMGEOSTART_H

#include "FairGeoSet.h"

#include "TString.h"

class  CbmGeoStart : public FairGeoSet {
protected:
  char modName[5];  // name of module
  char eleName[3];  // substring for elements in module
public:
  CbmGeoStart();
  ~CbmGeoStart() {}
  const char* getModuleName(Int_t);
  const char* getEleName(Int_t);
  inline Int_t getModNumInMod(const TString&);
  ClassDef(CbmGeoStart,0) // Class for geometry of Start detector
};

#endif  /* !CBMGEOSTART_H */

inline Int_t CbmGeoStart::getModNumInMod(const TString& name) {
  // returns the sector index retrieved from ISTx
  return (Int_t)(name[3]-'0');
}
