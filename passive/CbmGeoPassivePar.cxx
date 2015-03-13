//*-- AUTHOR : Denis Bertini
//*-- Created : 21/06/2005

#include "CbmGeoPassivePar.h"

#include "FairParamList.h"

#include "TObjArray.h"

ClassImp(CbmGeoPassivePar)

CbmGeoPassivePar::CbmGeoPassivePar(const char* name,const char* title,const char* context)
: FairParGenericSet(name,title,context), 
  fGeoSensNodes(new TObjArray()),
  fGeoPassNodes(new TObjArray())
{
}

CbmGeoPassivePar::~CbmGeoPassivePar(void) {
}

void CbmGeoPassivePar::clear(void) {
    if(fGeoSensNodes) delete fGeoSensNodes;
    if(fGeoPassNodes) delete fGeoPassNodes;
}

void CbmGeoPassivePar::putParams(FairParamList* l) {
  if (!l) return;
   l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
   l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
}

Bool_t CbmGeoPassivePar::getParams(FairParamList* l) {
    if (!l) return kFALSE;
    if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
    if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes)) return kFALSE;

  return kTRUE;
}
