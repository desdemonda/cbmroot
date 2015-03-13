#ifndef CBMBEAMTIMEFIRSTPAR_H
#define CBMBEAMTIMEFIRSTPAR_H

#include "FairParGenericSet.h"

class FairParamList;

class CbmBeamTimeFirstPar : public FairParGenericSet
{
  public:

    CbmBeamTimeFirstPar(const char* name="CbmBeamTimeFirstPar",
                          const char* title="CbmBeamTimeFirstPar Parameters",
                          const char* context="TestDefaultContext");
    ~CbmBeamTimeFirstPar(void);
    void clear(void);
    void putParams(FairParamList*);
    Bool_t getParams(FairParamList*);

    Int_t GetDigiCut() { return fDigiCut; }
    void SetDigiCut(Int_t);

  private:
    Int_t fDigiCut;

    CbmBeamTimeFirstPar(const CbmBeamTimeFirstPar&);
    CbmBeamTimeFirstPar& operator=(const CbmBeamTimeFirstPar&);

    ClassDef(CbmBeamTimeFirstPar,1)
};

#endif
