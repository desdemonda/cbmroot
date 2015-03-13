/** CbmGeoMuchPar class
 * @author  E.Kryshen
 * @version 1.0
 * @since   18.02.08
 *
 * @author  A.Kiseleva
 * @version 0.0
 * @since   13.04.06
 * 
 *  Class for geometry parameters of MUon CHambers
 *
 */

#include "CbmGeoMuchPar.h"
#include "FairParamList.h"
#include "TObjArray.h"

CbmGeoMuchPar::CbmGeoMuchPar(const char* name,const char* title,const char* context)
  : FairParGenericSet(name,title,context),
    fGeoSensNodes(new TObjArray()),
    fGeoPassNodes(new TObjArray()),
    fStations(new TObjArray())
{
}

CbmGeoMuchPar::~CbmGeoMuchPar(void) {
}

void CbmGeoMuchPar::clear(void) {
    if(fGeoSensNodes) delete fGeoSensNodes;
    if(fGeoPassNodes) delete fGeoPassNodes;
    if(fStations) delete fStations;
}

void CbmGeoMuchPar::putParams(FairParamList* l) {
  if (!l) return;
  l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
  l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
  l->addObject("CbmMuchStationOlds list",fStations);
}

Bool_t CbmGeoMuchPar::getParams(FairParamList* l) {
  if (!l) return kFALSE;
  if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
  if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes)) return kFALSE;
  if (!l->fillObject("CbmMuchStationOlds list",fStations)) return kFALSE; 
  return kTRUE;
}

ClassImp(CbmGeoMuchPar)

