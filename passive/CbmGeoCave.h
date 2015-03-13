#ifndef CBMGEOCAVE_H
#define CBMGEOCAVE_H

#include "FairGeoSet.h"

#include "TString.h"

#include <fstream>

class FairGeoMedia;

class  CbmGeoCave : public FairGeoSet {
protected:
  TString name;
public:
  CbmGeoCave();
  ~CbmGeoCave() {}
  const char* getModuleName(Int_t) {return name.Data();}  
  Bool_t read(std::fstream&,FairGeoMedia*);
  void addRefNodes();
  void write(std::fstream&);
  void print();
  ClassDef(CbmGeoCave,0) // Class for the geometry of CAVE
};

#endif  /* !CBMGEOCAVE_H */
