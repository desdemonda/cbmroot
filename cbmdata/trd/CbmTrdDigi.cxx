#include "CbmTrdDigi.h"

#include <sstream>
using std::endl;
using std::stringstream;

CbmTrdDigi::CbmTrdDigi() 
  : CbmDigi(),
    fn_FNR_Triggers(0),
    fAddress(-1),
    fCharge(-1.),
    fTime(-1.),
    fGlobalTriggered(false),
    fPrimeTriggered(false),
    fFNR_Triggered(false),
    fNormalStop(true),
    fBufferFullStop(false),
    fDoubleHitStop(false),
    fNeighbourTriggerIds(),
    fPulseShape()
{
  for (Int_t i = 0; i < 8; i++)
    fNeighbourTriggerIds[i] = 0;
  for (Int_t i = 0; i < 45; i++)
    fPulseShape[i] = 0;
}

CbmTrdDigi::CbmTrdDigi(
      Int_t address,
      Double_t charge,
      Double_t time)
  : CbmDigi(),
    fn_FNR_Triggers(0),
    fAddress(address),
    fCharge(charge),
    fTime(time),
    fGlobalTriggered(false),
    fPrimeTriggered(false),
    fFNR_Triggered(false),
    fNormalStop(true),
    fBufferFullStop(false),
    fDoubleHitStop(false),
    fNeighbourTriggerIds(),
    fPulseShape()
{
  for (Int_t i = 0; i < 8; i++)
    fNeighbourTriggerIds[i] = 0;
  for (Int_t i = 0; i < 45; i++)
    fPulseShape[i] = 0;
}

CbmTrdDigi::CbmTrdDigi(
		       Int_t address,
		       Double_t charge,
		       Double_t time, 
		       Bool_t primeTriggered, 
		       Bool_t fnr_Triggered,
		       Bool_t globalTriggered,
		       Bool_t normalStop,
		       Bool_t bufferFullStop,
		       Bool_t doubleHitStop
)
  : CbmDigi(),
    fn_FNR_Triggers(0),
    fAddress(address),
    fCharge(charge),
    fTime(time),
    fGlobalTriggered(globalTriggered),
    fPrimeTriggered(primeTriggered),
    fFNR_Triggered(fnr_Triggered),
    fNormalStop(normalStop),
    fBufferFullStop(bufferFullStop),
    fDoubleHitStop(doubleHitStop),
    fNeighbourTriggerIds(),
    fPulseShape()
{
  for (Int_t i = 0; i < 8; i++)
    fNeighbourTriggerIds[i] = 0;
  for (Int_t i = 0; i < 45; i++)
    fPulseShape[i] = 0;
}

CbmTrdDigi::~CbmTrdDigi()
{
}

string CbmTrdDigi::ToString() const {
   stringstream ss;
   ss << "CbmTrdDigi: address=" << fAddress << " charge=" << fCharge
         << " time=" << fTime << endl;
   return ss.str();
}

void CbmTrdDigi::AddNeighbourTriggerId(Int_t digiId) {
  if (fn_FNR_Triggers < 8){
    fNeighbourTriggerIds[fn_FNR_Triggers] = digiId;
  } else {
    printf("TO MUCH FNR TRIGGERS!!!! %i\n",fn_FNR_Triggers);
  }
  fn_FNR_Triggers++;
}

void CbmTrdDigi::SetPulseShape(Float_t pulse[45]) {
  for (Int_t sample = 0; sample < 45; sample++)
    fPulseShape[sample] = pulse[sample];
}

ClassImp(CbmTrdDigi)
