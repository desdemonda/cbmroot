#ifndef Cave_H
#define Cave_H

#include "FairModule.h"


class CbmCave : public FairModule {
  public:
    CbmCave(const char * name, const char *Title="Exp Cave");
    CbmCave();
    virtual ~CbmCave();
    virtual void ConstructGeometry();
    

private:
     Double_t world[3];
     ClassDef(CbmCave,1) //CBMCaveSD
};

#endif //Cave_H

