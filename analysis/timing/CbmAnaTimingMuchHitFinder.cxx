/** CbmAnaTimingMuchHitFinder.cxx
 *@author E.Kryshen <e.kryshen@gsi.de>
 *@since 2011-01-10
 **/

#include "CbmAnaTimingMuchHitFinder.h"
#include "CbmMCTrack.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairMCPoint.h"
#include "CbmMuchGeoScheme.h"
#include "CbmMuchModuleGem.h"
#include "CbmMuchSector.h"
#include "CbmMuchPad.h"
#include "CbmMCEpoch.h"
#include "CbmMuchDigi.h"
#include "CbmMuchDigiMatch.h"
#include "TClonesArray.h"
#include "TH3D.h"
#include "TH1D.h"
#include "TVector3.h"
#include "TCanvas.h"
#include "TArc.h"
#include "CbmMuchCluster.h"
#include "TLatex.h"
#include "CbmMuchPixelHit.h"
#include "TCanvas.h"

#include <map>
#include <vector>
using std::vector;
using std::multimap;

// -----   Default constructor   -------------------------------------------
CbmAnaTimingMuchHitFinder::CbmAnaTimingMuchHitFinder():
  fEvent(0),fDigiFile("much_digi.root"),fHistoName("histo.root"),
  fGeoScheme(CbmMuchGeoScheme::Instance())
{}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmAnaTimingMuchHitFinder::CbmAnaTimingMuchHitFinder(const char* name, TString digiFile, TString histoName):
  FairTask(name),fEvent(0),fDigiFile(digiFile),fHistoName(histoName),
  fGeoScheme(CbmMuchGeoScheme::Instance())
{}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmAnaTimingMuchHitFinder::~CbmAnaTimingMuchHitFinder(){}
// -------------------------------------------------------------------------

// -----  SetParContainers -------------------------------------------------
void CbmAnaTimingMuchHitFinder::SetParContainers(){
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb = ana->GetRuntimeDb();
  rtdb->getContainer("FairBaseParSet");
  rtdb->getContainer("CbmGeoPassivePar");
  rtdb->getContainer("CbmGeoStsPar");
  rtdb->getContainer("CbmGeoMuchPar");
  rtdb->getContainer("CbmFieldPar");
}
// -------------------------------------------------------------------------


// -----   Public method Init (abstract in base class)  --------------------
InitStatus CbmAnaTimingMuchHitFinder::Init(){
  FairRootManager* fManager = FairRootManager::Instance();
  // Initialize GeoScheme
  TFile* oldfile=gFile;
  TFile* file=new TFile(fDigiFile);
  TObjArray* stations = (TObjArray*) file->Get("stations");
  file->Close();
  file->Delete();
  gFile=oldfile;
  fGeoScheme->Init(stations);
  
  fMuchDigis       = (TClonesArray*) fManager->GetObject("MuchDigi");
  fMuchDigiMatches = (TClonesArray*) fManager->GetObject("MuchDigiMatch");
  fMuchClusters    = (TClonesArray*) fManager->GetObject("MuchCluster");
  fMuchHits        = (TClonesArray*) fManager->GetObject("MuchPixelHit");
  fMcEpoch         = (CbmMCEpoch*)   fManager->GetObject("MCEpoch.");
  
  CbmMuchModuleGem* module = (CbmMuchModuleGem*) fGeoScheme->GetModule(0,0,0,4);
  vector<CbmMuchPad*> pads = module->GetPads();
  for (UInt_t i=0;i<pads.size();i++) pads[i]->SetFillColor(kYellow);
  
  for (Int_t i=0;i<NSLICES;i++){
    fModules[i] = new CbmMuchModuleGem(*module);
  }
  
  fHitTimeAll       = new TH1D("fHitTimeAll",";time [ns]; Entries",600,0,600);
  fHitTimeCorrected = new TH1D("fHitTimeCorrected",";time [ns]; Entries",600,0,600);
  fHitTimeAll->SetLineColor(kBlue);
  fHitTimeCorrected->SetLineColor(kBlue);
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void CbmAnaTimingMuchHitFinder::Exec(Option_t* opt){
//  DrawSlices();
  AnalyseHitTimeDistributions();
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void CbmAnaTimingMuchHitFinder::AnalyseHitTimeDistributions(){
  Int_t nHits = fMuchHits->GetEntriesFast();
  printf("nHits=%i\n",nHits);
  for (Int_t i=0;i<nHits;i++){
    CbmMuchPixelHit* hit = (CbmMuchPixelHit*) fMuchHits->At(i);
    Double_t t = hit->GetTime();
    TVector3 r;
    hit->Position(r);
    fHitTimeAll->Fill(t);
    fHitTimeCorrected->Fill(t-r.Mag()/29.97);
  }
}
// -------------------------------------------------------------------------


// -----   Public method Finish   ------------------------------------------
void CbmAnaTimingMuchHitFinder::Finish(){
  TCanvas* c1 = new TCanvas("c1","c1",1200,800);
  c1->Divide(1,2);
  c1->cd(1);
  fHitTimeAll->Draw();
  c1->cd(2);
  fHitTimeCorrected->Draw();
  
  TFile* f = new TFile(fHistoName.Data(),"RECREATE");
  fHitTimeAll->Write();
  fHitTimeCorrected->Write();
//  fDetEventTime->Write();
  f->Close();
  
//  CbmMuchModuleGem* module = (CbmMuchModuleGem*) fGeoScheme->GetModule(0,0,0,4);
//  Double_t r    = module->GetCutRadius();
//  TArc* cMin = new TArc(0,0,r,0,360);
//  cMin->SetFillColor(10);
//  cMin->Draw("f");

}
// -------------------------------------------------------------------------




// -----   Public method Exec   --------------------------------------------
void CbmAnaTimingMuchHitFinder::DrawSlices(){
  CbmMuchModuleGem* module = (CbmMuchModuleGem*) fGeoScheme->GetModule(0,0,0,4);
  module->ClearDigis();
  for (Int_t iDigi = 0; iDigi < fMuchDigis->GetEntriesFast(); iDigi++) {
    CbmMuchDigi* digi = (CbmMuchDigi*) fMuchDigis->At(iDigi);
    if (digi->GetDetectorId()!=module->GetDetectorId()) continue;
    Double_t time = digi->GetTime();
    module->AddDigi(time,iDigi);  
  }

  Double_t r    = module->GetCutRadius();
  TVector3 size = module->GetSize();
  TVector3 pos  = module->GetPosition();
//  printf("module:");
//  printf(" r=%.1f",r);
//  printf(" size=(%.1f,%.1f,%.1f)",size.x(),size.y(),size.z());
//  printf(" pos=(%.1f,%.1f,%.1f)",pos.x(),pos.y(),pos.z());
//  printf("\n");

  for (Int_t nSlice=0;nSlice<NSLICES;nSlice++){
    
    multimap<Double_t,Int_t> digis = module->GetDigis();
    multimap<Double_t,Int_t>::iterator it = digis.begin();
    multimap<Int_t,Double_t> time_slice;
    time_slice.clear();
//    Int_t nSlice=3;
    Int_t iSlice=0;
    
    Double_t last_time = -1.e+10;
    while (it!=digis.end()){
      if (last_time>0 && it->first - last_time > 2) {
        if (iSlice==nSlice) break;
        iSlice++;
        time_slice.clear();
      }
      last_time = it->first;
      time_slice.insert(pair<Int_t,Double_t>(it->second,it->first));
      it++;
    }
    Double_t slice_start = time_slice.begin()->second;
    Double_t slice_stop = (--time_slice.end())->second;
    
    printf("time slice: %05i",time_slice.size());
    printf(" start: %6.1f",slice_start);
    printf(" finish: %6.1f",slice_stop);
    printf("\n");


    
    TCanvas* c1 = new TCanvas(Form("c%04.0f",slice_start),Form("c%i",nSlice),1000,1000);
//    TCanvas* c1 = new TCanvas(Form("c%i",NSLICES-nSlice),Form("c%i",nSlice),2*670,2*670);
    gPad->Range(pos.x()-size.x()/2.,pos.y()-size.y()/2.,pos.x()+size.x()/2.,pos.y()+size.y()/2.);

    module->SetFillColor(kYellow);
    module->SetFillStyle(3001);
    module->Draw("f");
    module->DrawModule(kYellow);

    for (Int_t i=0;i<module->GetNSectors();i++){
      CbmMuchSector* sector = module->GetSector(i);
      for (Int_t p=0;p<sector->GetNChannels();p++){
        CbmMuchPad* pad = sector->GetPad(p);
        pad->SetFillColor(kYellow);
      }
    }

    for (Int_t iDigi=0;iDigi<fMuchDigis->GetEntriesFast();iDigi++){
      CbmMuchDigi* digi = (CbmMuchDigi*) fMuchDigis->At(iDigi);
      if (digi->GetDetectorId()!=module->GetDetectorId()) continue;
      Double_t pad_start_time = digi->GetTime();
      Double_t pad_stop_time = digi->GetTime()+digi->GetDeadTime();
      Long64_t channelId = digi->GetChannelId();
      UInt_t ADCcharge = digi->GetADCCharge();
      CbmMuchPad* pad = module->GetPad(channelId);
      if (pad_start_time<slice_start && pad_stop_time>slice_start) pad->SetFillColor(kBlack);
    }
    
    for (Int_t i=0;i<module->GetNSectors();i++){
      CbmMuchSector* sector = module->GetSector(i);
      for (Int_t p=0;p<sector->GetNChannels();p++){
        CbmMuchPad* pad = sector->GetPad(p);
        pad->DrawClone("f");
        pad->Draw();
      }
      sector->SetLineWidth(1);
      sector->SetLineColor(kGreen+4);
      sector->Draw();
    }
    
    for (Int_t iCluster=0;iCluster<fMuchClusters->GetEntriesFast();iCluster++){
      CbmMuchCluster* cluster = (CbmMuchCluster*) fMuchClusters->At(iCluster);
  //    Int_t iDigi = cluster->GetDigiIndex(0);
      for (Int_t i=0;i<cluster->GetNDigis();i++){
        Int_t iDigi = cluster->GetDigiIndex(i);
        CbmMuchDigi* digi = (CbmMuchDigi*) fMuchDigis->At(iDigi);
        if (digi->GetDetectorId()!=module->GetDetectorId()) continue;
        if (time_slice.find(iDigi)==time_slice.end()) continue;

        Long64_t channelId = digi->GetChannelId();
        UInt_t ADCcharge = digi->GetADCCharge();
        CbmMuchPad* pad = module->GetPad(channelId);
        pad->SetFired(iDigi,ADCcharge);
        pad->Draw("f");
        pad->Draw();
      }
    }
    
    TArc* cMin = new TArc(0,0,r,0,360);
    cMin->SetFillColor(10);
    cMin->Draw("f");
    TLatex* l = new TLatex(2,2,Form("Time: %.0f ns",slice_start));
    l->SetTextAlign(0);
    l->SetTextSize(0.04);
    l->Draw();
    c1->Print(".png");
    
  }
//  for (Int_t iDigi=0;iDigi<fMuchDigis->GetEntriesFast();iDigi++){
//    CbmMuchDigi* digi = (CbmMuchDigi*) fMuchDigis->At(iDigi);
//    if (digi->GetDetectorId()!=module->GetDetectorId()) continue;
//    Long64_t channelId = digi->GetChannelId();
//    UInt_t ADCcharge = digi->GetADCCharge();
//    CbmMuchPad* pad = module->GetPad(channelId);
//    pad->SetFired(iDigi,ADCcharge);
//  }

}
// -------------------------------------------------------------------------





ClassImp(CbmAnaTimingMuchHitFinder);
