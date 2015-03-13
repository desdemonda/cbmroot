#include "CbmTrdSPADIC.h"
#include "CbmTrdClusterFinderFast.h"

#include "CbmTrdDigiPar.h"
#include "CbmTrdDigi.h"

#include "CbmTrdModule.h"
#include "CbmTrdGeoHandler.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TArray.h"
#include "TH1D.h"
#include "TCanvas.h"

#include <iostream>
#include <iomanip>
#include <cmath>
using std::cout;
using std::endl;

CbmTrdSPADIC::CbmTrdSPADIC()
  :FairTask("TrdSPADIC",1),
   fSpadicResponse(NULL),
   fShaperOrder(2),
   fShapingTime(0.09),
   fPeakBin(2),
   fBitResolution(8),
   fmaxdEdx(0.0001),
   fAdcBit(fmaxdEdx/pow(2,fBitResolution)),
   fPulseShape(false),
   fSelectionMask(),
   fDigis(NULL),   
   fDigiPar(NULL),
   fModuleInfo(NULL),
   fGeoHandler(NULL),
   fMinimumChargeTH(1.0e-06)
{
}
CbmTrdSPADIC::~CbmTrdSPADIC()
{
    fDigis->Delete();
    delete fDigis;
}
void CbmTrdSPADIC::SetParContainers()
{
  fDigiPar = (CbmTrdDigiPar*)(FairRunAna::Instance()->GetRuntimeDb()->getContainer("CbmTrdDigiPar"));
}

InitStatus CbmTrdSPADIC::Init()
{
   FairRootManager* ioman = FairRootManager::Instance();

   fDigis = (TClonesArray*) ioman->GetObject("TrdDigi");
   if (fDigis == NULL) LOG(FATAL) << "CbmTrdSPADIC::Init No TrdDigi array" << FairLogger::endl;

   fGeoHandler = new CbmTrdGeoHandler();
   fGeoHandler->Init();

   InitSpadicResponseFunction();

   return kSUCCESS;
} 


// --------------------------------------------------------------------
void CbmTrdSPADIC::SetBitResolution(Int_t bit){
  fBitResolution = pow(2,bit);
}
// --------------------------------------------------------------------
void CbmTrdSPADIC::SetMaxRange(Double_t maxRange){
  fmaxdEdx = maxRange;
  fAdcBit = fmaxdEdx/fBitResolution;
}
// --------------------------------------------------------------------
void CbmTrdSPADIC::SetPulseShapeSim(Bool_t pulseShape){
  fPulseShape = pulseShape;
}
// --------------------------------------------------------------------
void CbmTrdSPADIC::SetTriggerThreshold(Double_t minCharge){
  fMinimumChargeTH = minCharge;//  To be used for test beam data processing
}
// --------------------------------------------------------------------
Bool_t CbmDigiSorter(std::pair< Int_t, Int_t/*CbmTrdDigi**/> a, std::pair< Int_t, Int_t/*CbmTrdDigi**/> b)
{return (a.first < b.first);}
// --------------------------------------------------------------------
void CbmTrdSPADIC::InitSpadicResponseFunction(){
  fSpadicResponse = new TH1D("SpadicResponseFunction","SpadicResponseFunction", fnBins, 0 -(1.8/fnBins), 1.8 -(1.8/fnBins));// 1/25MHz * 45Timebins = 1.8µs200000,0,20);
  Double_t t = 0; //[µs]
  Double_t h = 0;
  Double_t T = 0.09; //[µs]
  Double_t t_peak   = fSpadicResponse->GetBinCenter(fPeakBin);
  for (Int_t k = 1; k <= fSpadicResponse->GetNbinsX(); k++) {
    t = fSpadicResponse->GetBinCenter(k);
    //t = fSpadicResponse->GetBinCenter(k) * t_peak;
    //h = t/pow(fShapingTime,fShaperOrder) * exp(-1. * t / fShapingTime); // Tim thesis p.42
    h = (pow(t / fShapingTime,fShaperOrder) * exp(-1. * t / fShapingTime)); // with time compesation
    fSpadicResponse->SetBinContent(k,h);
  }
  fSpadicResponse->Scale(1./fSpadicResponse->Integral());
}
// --------------------------------------------------------------------
void CbmTrdSPADIC::SetSelectionMask(Bool_t mask[fnBins]){
  for (Int_t iBin = 0; iBin < fnBins; iBin++){
    fSelectionMask[iBin] = mask[iBin];
  }
}
// --------------------------------------------------------------------
void CbmTrdSPADIC::ADC(TH1D* spadicPulse){
  for (Int_t k = 1; k <= spadicPulse->GetNbinsX(); k++) { 
    if (spadicPulse->GetBinContent(k) > fmaxdEdx)
      spadicPulse->SetBinContent(k,fmaxdEdx);
    else
      spadicPulse->SetBinContent(k, Int_t(spadicPulse->GetBinContent(k) / fAdcBit) * fAdcBit);
    if (!fSelectionMask[k-1]){
      spadicPulse->SetBinContent(k,0);
    }
  }
}
// --------------------------------------------------------------------
void CbmTrdSPADIC::ADC(CbmTrdDigi* digi){ 
  if (digi->GetCharge() > fmaxdEdx)
    digi->SetCharge(fmaxdEdx);
  else
    digi->SetCharge(Int_t(digi->GetCharge() / fAdcBit) * fAdcBit);  
}
// --------------------------------------------------------------------
void CbmTrdSPADIC::CR_RC_Shaper(CbmTrdDigi* digi, TH1D* spadicPulse){
  Double_t amplitude = digi->GetCharge();
 if (amplitude <= 0.0) return;
  TH1D *event = new TH1D("deltaPulse","deltaPulse", fnBins, 0 -(1.8/fnBins), 1.8 -(1.8/fnBins));//   1/25MHz * 45Timebins = 1.8µs
  event->SetBinContent(1,0);
  for (Int_t k = fPeakBin; k <= event->GetNbinsX(); k++) {
    event->SetBinContent(k,amplitude*exp(-0.65*(k-fPeakBin)));
  }
  spadicPulse->Reset();
  Double_t t = 0; //[µs]
  Double_t x = 0;
  Double_t h = 0;
  Double_t T_0 = event->GetBinWidth(1); //[µs]
  Double_t t_peak   = event->GetBinCenter(event->GetMaximumBin());
  Double_t max_ampl = event->GetBinContent(event->GetMaximumBin());
 
  for (Int_t k = 1; k <= event->GetNbinsX(); k++) { 
    Double_t y = 0;
    for (Int_t i = 1; i <= event->GetNbinsX(); i++) {
      t = event->GetBinCenter(i);// * t_peak;
      //h = (pow(t / fShapingTime,fShaperOrder) * exp(-1. * t / fShapingTime)); // with time compesation
      h = fSpadicResponse->GetBinContent(i);
      x = event->GetBinContent(k-i);
      y += x * h;	   
    } 
    spadicPulse->SetBinContent(k, y);// * T_0 / max_ampl);
    //spadicPulse->SetBinContent(k, event->GetBinContent(k));    
  }
  spadicPulse->Scale(amplitude/spadicPulse->GetBinContent(spadicPulse->GetMaximumBin())/*spadicPulse->Integral()*/);
  ADC(spadicPulse);
  delete event;
}
  

  // ---- Exec ----------------------------------------------------------
void CbmTrdSPADIC::Exec(Option_t *option)
{
  cout << "================CbmTrdSPADIC===============" << endl;
  LOG(INFO) << "CbmTrdSPADIC::Exec : fPulseShape:    " << (Bool_t)fPulseShape << FairLogger::endl;
  LOG(INFO) << "CbmTrdSPADIC::Exec : fBitResolution: " << fBitResolution << FairLogger::endl;
  LOG(INFO) << "CbmTrdSPADIC::Exec : fShaperOrder:   " << fShaperOrder << FairLogger::endl;
  LOG(INFO) << "CbmTrdSPADIC::Exec : fShapingTime:   " << fShapingTime << " µs" << FairLogger::endl;
  LOG(INFO) << "CbmTrdSPADIC::Exec : fmaxdEdx:       " << fmaxdEdx << " GeV" << FairLogger::endl;
  LOG(INFO) << "CbmTrdSPADIC::Exec : fAdcBit:        " <<  fAdcBit << " (GeV/bit)" << FairLogger::endl;
  LOG(INFO) << "CbmTrdSPADIC::Exec : fSelectionMask:";
  for (Int_t iBin = 0; iBin < fnBins; iBin++){
    LOG(INFO) << " " << (Bool_t)fSelectionMask[iBin];
    if ((1+iBin) % 5 == 0 && iBin > 0 && (1+iBin) < fnBins){
      LOG(INFO) << FairLogger::endl;
      LOG(INFO) << "                                    ";
    }
  }
  LOG(INFO) << FairLogger::endl;
  Bool_t debug = false;
  std::map< Int_t, std::map< Int_t, Int_t> > moduleDigiNotTriggerMap; //<ModuleAddress, <combiId, digiId> >
  std::map< Int_t, std::list< std::pair< Int_t, Int_t> > > moduleDigiTriggerMap; //<ModuleAddress, <combiId, digiId> >
  Int_t nDigis = fDigis->GetEntries();
  
  TH1D* spadicPulse = new TH1D("spadicPulse","spadicPulse", fnBins, 0 -(1.8/fnBins), 1.8 -(1.8/fnBins));// 1/25MHz * 45Timebins = 1.8µs
  TCanvas* c = NULL;
  if (debug)
    c = new TCanvas("c","c",800,600);
  for (Int_t iDigi=0; iDigi < nDigis; iDigi++ ) {
    CbmTrdDigi *digi = (CbmTrdDigi*) fDigis->At(iDigi);
   
    
    Int_t digiAddress  = digi->GetAddress();
    Int_t layerId    = CbmTrdAddress::GetLayerId(digiAddress);
    Int_t moduleId = CbmTrdAddress::GetModuleId(digiAddress);
    Int_t moduleAddress = CbmTrdAddress::GetModuleAddress(digiAddress);
    fModuleInfo = fDigiPar->GetModule(CbmTrdAddress::GetModuleAddress(digiAddress));
    if (!fModuleInfo){
      printf("digi %3i digiAddress %i layer %i and modId %i  Sec%i Row:%i Col%i not found\n",
	     iDigi,digiAddress,layerId,moduleId,CbmTrdAddress::GetSectorId(digi->GetAddress()),
	     CbmTrdAddress::GetRowId(digi->GetAddress()),CbmTrdAddress::GetColumnId(digi->GetAddress())); 
      continue;
    }
    Int_t secRow = CbmTrdAddress::GetRowId(digi->GetAddress());
    Int_t iSector = CbmTrdAddress::GetSectorId(digi->GetAddress());
    Int_t globalRow = 0;
    globalRow = fModuleInfo->GetModuleRow(iSector,secRow);
    Int_t iCol = CbmTrdAddress::GetColumnId(digi->GetAddress());
  
    Int_t combiId = globalRow * (fModuleInfo->GetNofColumns() + 1) + iCol;
 
    if (digi->GetCharge() >= fMinimumChargeTH){
      digi->SetPrimeTriggerStatus(true);
      digi->SetNormalStop(true);
      moduleDigiTriggerMap[moduleAddress].push_back(std::make_pair(combiId, iDigi));
    } else {    
      moduleDigiNotTriggerMap[moduleAddress][combiId] = iDigi;
    }

  }
  CbmTrdDigi *digi = NULL;
  for (std::map< Int_t, std::list< std::pair< Int_t, Int_t> > >::iterator iModule = moduleDigiTriggerMap.begin(); iModule != moduleDigiTriggerMap.end(); ++iModule) {
    (*iModule).second.sort(CbmDigiSorter);
    Int_t moduleAddress = (*iModule).first;
    for (std::list< std::pair< Int_t, Int_t> >::iterator iDigi = (*iModule).second.begin(); iDigi != (*iModule).second.end(); iDigi++) {
      Int_t combiId = (*iDigi).first;
      Int_t digiId = (*iDigi).second;
      digi = (CbmTrdDigi*) fDigis->At(digiId);
      if (fPulseShape){
	CR_RC_Shaper( digi, spadicPulse);
	Float_t pulse[fnBins] = {0.0};
	for (Int_t k = 1; k <= spadicPulse->GetNbinsX(); k++){
	  pulse[k-1] = spadicPulse->GetBinContent(k);
	}
	digi->SetPulseShape(pulse);
	if (debug){
	  //printf("%.6E - %.6E => %.1f%% \n", digi->GetCharge(), spadicPulse->GetBinContent(spadicPulse->GetMaximumBin()), 100.*(digi->GetCharge() - spadicPulse->GetBinContent(spadicPulse->GetMaximumBin())) / digi->GetCharge());
	  c->cd();
	  spadicPulse->GetYaxis()->SetRangeUser(0-0.01*fmaxdEdx,fmaxdEdx+0.01*fmaxdEdx);
	  spadicPulse->DrawCopy();
	}
      } else {
	ADC(digi);
      }
      if (moduleDigiNotTriggerMap[moduleAddress].find(combiId-1) != moduleDigiNotTriggerMap[moduleAddress].end()){
	digi = (CbmTrdDigi*) fDigis->At(moduleDigiNotTriggerMap[moduleAddress][combiId-1]);
	if (fPulseShape){
	  CR_RC_Shaper( digi, spadicPulse);
	  Float_t pulse[fnBins] = {0.0};
	  for (Int_t k = 1; k <= spadicPulse->GetNbinsX(); k++){
	    pulse[k-1] = spadicPulse->GetBinContent(k);
	  }
	  digi->SetPulseShape(pulse);
	  if (debug){
	    c->cd();
	    spadicPulse->SetLineColor(kRed);
	    spadicPulse->GetYaxis()->SetRangeUser(0-0.01*fmaxdEdx,fmaxdEdx+0.01*fmaxdEdx);
	    spadicPulse->DrawCopy("same");
	  }
	} else {
	  ADC(digi);
	}
	digi->SetNormalStop(true);
	digi->SetFNR_TriggerStatus(true);  
	digi->AddNeighbourTriggerId(digiId);
      }
      if (moduleDigiNotTriggerMap[moduleAddress].find(combiId+1) != moduleDigiNotTriggerMap[moduleAddress].end()){
	digi = (CbmTrdDigi*) fDigis->At(moduleDigiNotTriggerMap[moduleAddress][combiId+1]);
	if (fPulseShape){
	  CR_RC_Shaper( digi, spadicPulse);
	  Float_t pulse[fnBins] = {0.0};
	  for (Int_t k = 1; k <= spadicPulse->GetNbinsX(); k++){
	    pulse[k-1] = spadicPulse->GetBinContent(k);
	  }
	  digi->SetPulseShape(pulse);
	  if (debug){
	    c->cd();
	    spadicPulse->SetLineColor(kGreen);
	    spadicPulse->GetYaxis()->SetRangeUser(0-0.01*fmaxdEdx,fmaxdEdx+0.01*fmaxdEdx);
	    spadicPulse->DrawCopy("same");
	  }
	} else {
	  ADC(digi);
	}
	digi->SetNormalStop(true);
	digi->SetFNR_TriggerStatus(true); 
	digi->AddNeighbourTriggerId(digiId);	
      }
      if (fPulseShape)
	if (debug)
	  c->Update();
    }
  }
}
ClassImp(CbmTrdSPADIC)
