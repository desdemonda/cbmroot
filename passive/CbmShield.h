#ifndef CBMSHIELD_H
#define CBMSHIELD_H 1

#include "FairModule.h"

class CbmShield : public FairModule {
  public:
    CbmShield(const char * name, const char *Title="CBM Shield");
    CbmShield();
    virtual ~CbmShield();
    virtual void ConstructGeometry();
    ClassDef(CbmShield,1) 
  
};

#endif

