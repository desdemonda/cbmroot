
#include "CbmGeoTrdPar.h"

#include "FairParamList.h"

#include "TObjArray.h"

ClassImp(CbmGeoTrdPar)

CbmGeoTrdPar::CbmGeoTrdPar(const char* name,const char* title,const char* context)
: FairParGenericSet(name,title,context),
  fGeoSensNodes(new TObjArray()),
  fGeoPassNodes(new TObjArray())
{
}

CbmGeoTrdPar::~CbmGeoTrdPar(void) {
}

void CbmGeoTrdPar::clear(void) {
    if(fGeoSensNodes) delete fGeoSensNodes;
    if(fGeoPassNodes) delete fGeoPassNodes;
}

void CbmGeoTrdPar::putParams(FairParamList* l) {
  if (!l) return;
   l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
   l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
}

Bool_t CbmGeoTrdPar::getParams(FairParamList* l) {
    if (!l) return kFALSE;
    if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
    if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes)) return kFALSE;

  return kTRUE;
}
