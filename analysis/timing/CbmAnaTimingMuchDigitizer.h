/** CbmAnaTimingMuchDigitizer.h
 *@author E.Kryshen <e.kryshen@gsi.de>
 *@since 2011-01-06
 **
 **/

#ifndef CbmAnaTimingMuchDigitizer_H_
#define CbmAnaTimingMuchDigitizer_H_ 1

#include "FairTask.h"
#include "TString.h"
class TH3D;
class TH1D;
class CbmMuchGeoScheme;
class TClonesArray;
class CbmMCEpoch;

class CbmAnaTimingMuchDigitizer : public FairTask{
public:
  CbmAnaTimingMuchDigitizer();
  CbmAnaTimingMuchDigitizer(const char* name, TString digiFile, TString histoName);
  virtual ~CbmAnaTimingMuchDigitizer();
  virtual InitStatus Init();
  virtual void Exec(Option_t* opt);
  virtual void Finish();
  virtual void SetParContainers();
  void SetChannelId(Int_t id) { fChannelId = id; }
  void SetSectorId(Int_t id) {fSectorId = id; }
  void SetModuleId(Int_t id) {fModuleId = id; }
  void SetEpoch(Bool_t epoch) {fEpoch = epoch; }
  
private:
  void TimeDistributions();
  void DetailedAnalysis();
  void SignalShape();
  
  Int_t         fEvent;             //!
  CbmMuchGeoScheme* fGeoScheme;     //!
  TString       fHistoName;
  TString       fDigiFile;         
  TClonesArray* fMuchPoints;        //!
  TClonesArray* fMuchDigis;         //!
  TClonesArray* fMuchDigiMatches;   //!
  CbmMCEpoch*   fMcEpoch;           //!
  TH3D* fhPointXYT;        //!
  TH3D* fhSectorPointXYT;  //!
  TH3D* fhDigiXYT;         //!
  TH3D* fhSectorDigiXYT;   //!
  TH1D* fhChannelT;        //!
  TH1D* fhChannelHits;     //!
  TH1D* fhChannelHitDist;  //!
  TH1D* fhPointT;          //!
  TH1D* fhModuleT;         //!
  TH1D* fhhChannelT[64];   //!
  TH1D* fhhPointT[64];   //!
  TH1D* fhStationTotalR[5]; //!
  TH1D* fhStationFoundR[5]; //!
  TH1D* fhStationEffR[5]; //!
  TH1D* fhChargeVsTime[100];
  TH1D* fhSignalShape[100];
  Double_t fShape[2000];
  TH1D* fPointsTimeAll; //!
  TH1D* fhThresholdTime;
  TH1D* fhMaxTime;
  TH1D* fhMaxCharge;
  
  Int_t fChannelId;
  Int_t fSectorId;
  Int_t fModuleId;
  Bool_t fEpoch;
  
  Int_t nTotal;
  Int_t nLost;
  ClassDef(CbmAnaTimingMuchDigitizer,1)
};

#endif
