#include "CbmEcalPointLite.h"

#include <iostream>

using namespace std;

void CbmEcalPointLite::Print(const Option_t* opt) const
{
  cout << "CbmEcalPointLite: DetectorID=" << GetDetectorID() << ", TrackID=" << GetTrackID() << ", Eloss=" << GetEnergyLoss() << ", Time=" << GetTime() << endl;
}

ClassImp(CbmEcalPointLite)
