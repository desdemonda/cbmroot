#include "CbmBeamTimeFirstPar.h"

#include "FairParamList.h"
#include "FairLogger.h"

#include <iostream>

ClassImp(CbmBeamTimeFirstPar)

CbmBeamTimeFirstPar::CbmBeamTimeFirstPar(const char* name,
    const char* title,
    const char* context)
: FairParGenericSet(name,title,context),
  fDigiCut(0)
{
}

CbmBeamTimeFirstPar::~CbmBeamTimeFirstPar(void)
{
}

void CbmBeamTimeFirstPar::clear(void)
{
}

void CbmBeamTimeFirstPar::putParams(FairParamList* l)
{
  LOG(INFO) << "Filling container: " << FairLogger::endl;  
  if (!l) { return; }
   l->add("DigiCut",   fDigiCut);

}

Bool_t CbmBeamTimeFirstPar::getParams(FairParamList* l)
{
  LOG(INFO) << "Reading container: " << FairLogger::endl;  
  if (!l) { return kFALSE; }
  if ( ! l->fill("DigiCut", &fDigiCut) ) return kFALSE;
  return kTRUE;
}

void CbmBeamTimeFirstPar::SetDigiCut(Int_t val) 
{
  LOG(INFO) << "Setting DigiCut to value: " << val << FairLogger::endl;  
  fDigiCut = val;
  LOG(INFO) << "DigiCut was set to: " << fDigiCut << FairLogger::endl;  
}
