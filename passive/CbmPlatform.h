#ifndef PLATFORM_H
#define PLATFORM_H

#include "FairModule.h"

class CbmPlatform : public FairModule {
  public:
    CbmPlatform();
    CbmPlatform(const char * name, const char *Title="CBM Platform");
    virtual ~CbmPlatform();
    virtual void ConstructGeometry();
    virtual void ConstructAsciiGeometry();

    ClassDef(CbmPlatform,1) //CBMPLATFORM

};

#endif //PLATFORM_H

