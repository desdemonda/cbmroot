#ifndef PIPE_H
#define PIPE_H

#include "FairModule.h"

class CbmPipe : public FairModule {
  public:
    CbmPipe(const char * name, const char *Title="CBM Pipe");
    CbmPipe();

    virtual ~CbmPipe();
    virtual void ConstructGeometry();

    virtual void ConstructAsciiGeometry();

    virtual Bool_t CheckIfSensitive(std::string name) { return kFALSE; }
   
  ClassDef(CbmPipe,1) //CBMPIPE

};

#endif //PIPE_H

