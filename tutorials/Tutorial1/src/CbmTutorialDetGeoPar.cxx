#include "CbmTutorialDetGeoPar.h"

#include "FairParamList.h"

#include "TObjArray.h"

#include <iostream>

ClassImp(CbmTutorialDetGeoPar)

CbmTutorialDetGeoPar ::CbmTutorialDetGeoPar(const char* name,
					    const char* title,
					    const char* context)
  : FairParGenericSet(name,title,context) {
  
  fGeoSensNodes = new TObjArray();
  fGeoPassNodes = new TObjArray();

}

CbmTutorialDetGeoPar::~CbmTutorialDetGeoPar(void) {
}

void CbmTutorialDetGeoPar::clear(void) {
  if(fGeoSensNodes) delete fGeoSensNodes;
  if(fGeoPassNodes) delete fGeoPassNodes;
}

void CbmTutorialDetGeoPar::putParams(FairParamList* l) {
  if (!l) return;
  l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
  l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
}

Bool_t CbmTutorialDetGeoPar::getParams(FairParamList* l) {
  if (!l) return kFALSE;
  if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
  if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes)) return kFALSE;
  return kTRUE;
}
