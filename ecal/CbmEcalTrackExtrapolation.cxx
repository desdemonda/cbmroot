#include "CbmEcalTrackExtrapolation.h"

#include "FairRootManager.h"

void CbmEcalTrackExtrapolation::Init()
{
  FairRootManager* ioman=FairRootManager::Instance();
  if (!ioman)
    Fatal("CbmEcalTrackExtrapolationKF::Init()", "Can't instantise Rootmanager");
  ioman->Register("EcalTrackExtrapolation", "ECAL", this, kFALSE);
  fMap=new Short_t[32000]; 
}

ClassImp(CbmEcalTrackExtrapolation)
