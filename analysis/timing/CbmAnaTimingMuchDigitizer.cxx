/** CbmAnaTimingMuchDigitizer.cxx
 *@author E.Kryshen <e.kryshen@gsi.de>
 *@since 2011-01-06
 **/

#include "CbmAnaTimingMuchDigitizer.h"
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
#include "CbmMuchStation.h"
#include "TClonesArray.h"
#include "TH3D.h"
#include "TH1D.h"
#include "TVector3.h"
#include "TCanvas.h"
#include "TArc.h"
#include "TLatex.h"

// -----   Default constructor   -------------------------------------------
CbmAnaTimingMuchDigitizer::CbmAnaTimingMuchDigitizer():
  fEvent(0),fDigiFile("much_digi.root"),fHistoName("histo.root"),
  fGeoScheme(CbmMuchGeoScheme::Instance()),fModuleId(0),fSectorId(0),fChannelId(0),fEpoch(1)
{}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmAnaTimingMuchDigitizer::CbmAnaTimingMuchDigitizer(const char* name, TString digiFile, TString histoName):
  FairTask(name),fEvent(0),fDigiFile(digiFile),fHistoName(histoName),
  fGeoScheme(CbmMuchGeoScheme::Instance()),fModuleId(0),fSectorId(0),fChannelId(0),fEpoch(1)
{}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmAnaTimingMuchDigitizer::~CbmAnaTimingMuchDigitizer(){}
// -------------------------------------------------------------------------

// -----  SetParContainers -------------------------------------------------
void CbmAnaTimingMuchDigitizer::SetParContainers(){
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
InitStatus CbmAnaTimingMuchDigitizer::Init(){
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
  if (fEpoch)
    fMcEpoch         = (CbmMCEpoch*)   fManager->GetObject("MCEpoch.");
  else 
    fMuchPoints   = (TClonesArray*) fManager->GetObject("MuchPoint");
  
//  fMCTracks = new TClonesArray("CbmMCTrack");
//  fMcChain->SetBranchAddress("MCTrack",&fMCTracks);

  CbmMuchModuleGem* module = (CbmMuchModuleGem*) fGeoScheme->GetModule(0,0,0,fModuleId);
  Double_t r    = module->GetCutRadius();
  TVector3 size = module->GetSize();
  TVector3 pos  = module->GetPosition();
  fhPointXYT = new TH3D("hPointXYT","", 64,pos.x()-size.x()/2.,pos.x()+size.x()/2.,
                                        64,pos.y()-size.y()/2.,pos.y()+size.y()/2.,
                                       1000,                  0,10000);
  fhDigiXYT = new TH3D("hDigiXYT","", 64,pos.x()-size.x()/2.,pos.x()+size.x()/2.,
                                        64,pos.y()-size.y()/2.,pos.y()+size.y()/2.,
                                       1000,                  0,10000);
  CbmMuchSector* sector = module->GetSector(fSectorId); 
  TVector3 l = sector->GetSize();
  TVector3 p = sector->GetPosition();
  for (Int_t i=0;i<sector->GetNChannels();i++){
    CbmMuchPad* pad = sector->GetPad(i);
    printf("pad %i %f %f\n",i,pad->GetX0(),pad->GetY0());
  }
  
  fhSectorPointXYT = new TH3D("hSectorPointXYT","", 8,p[0]-l[0]/2,p[0]+l[0]/2.,
                                                   8,p[1]-l[1]/2,p[1]+l[1]/2,
                                                   1000,0,10000);
  fhSectorDigiXYT = new TH3D("hSectorDigiXYT","", 8,p[0]-l[0]/2,p[0]+l[0]/2.,
                                                  8,p[1]-l[1]/2,p[1]+l[1]/2,
                                                  1000,0,10000);
  
  fhChannelHits    = new TH1D("hChannelHits","",64,0,64);
  fhChannelHitDist = new TH1D("hChannelHitDist","",6,-0.5,5.5);
  
  fhChannelT = new TH1D("hChannelT","",10000,0,10000);
  fhPointT   = new TH1D("hPointT","",10000,0,10000);
  fhModuleT = new TH1D("hModuleT","",10000,0,10000);
  
  for (Int_t i=0;i<64;i++) {
    fhhChannelT[i] = new TH1D(Form("hhChannelT%0i",i),"",10000,0,10000);
    fhhPointT[i] = new TH1D(Form("hhPointT%0i",i),"",10000,0,10000);
  }
  

  for (Int_t i=0;i<5;i++){
    CbmMuchStation* station = (CbmMuchStation*) fGeoScheme->GetStation(i);
    fhStationTotalR[i] = new TH1D(Form("hStationTotalR%i",i),"",100,0,station->GetRmax()); 
    fhStationFoundR[i] = new TH1D(Form("hStationFoundR%i",i),"",100,0,station->GetRmax());
    fhStationEffR[i]   = new TH1D(Form("hStationEffR%i",i),"",100,0,station->GetRmax());
  }
  
  fPointsTimeAll    = new TH1D("fPointsTimeAll","; time [ns]; Entries",600,0,600);
  fPointsTimeAll->SetLineColor(kBlue);

  for (Int_t i=0;i<100;i++){
    fhChargeVsTime[i] = new TH1D(Form("fhChargeVsTime%i",i),";time [ns];charge [electrons]",400,0,40);
    fhSignalShape[i]  = new TH1D(Form("fhSignalShape%i",i),";time [ns]; voltage [a.u.]",2000,0,200);
  }
  
  Int_t nShapeTimeBins=2000;
  Double_t nBinsInNs=10;
  Double_t peakingTime=20;
  Double_t fallingEdgeTime=40;
  for (Int_t i=0;i<nShapeTimeBins;i++){
    Double_t time = i/nBinsInNs;
    if (time<=peakingTime) fShape[i]=time/peakingTime;
    else fShape[i] = exp(-(time-peakingTime)/fallingEdgeTime); 
  }
 
  fhThresholdTime = new TH1D("ThresholdTime",";ns;Entries",100,0,100);
  fhMaxTime = new TH1D("MaxTime",";ns;Entries",100,0,100);
  fhMaxCharge = new TH1D("MaxCharge",";charge;Entries",100,0,1000000);

  Int_t nTotal = 0;
  Int_t nLost = 0;
  
  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void CbmAnaTimingMuchDigitizer::Exec(Option_t* opt){
//  TimeDistributions();
//  DetailedAnalysis();
  SignalShape();
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void CbmAnaTimingMuchDigitizer::SignalShape(){
  Int_t nShapeTimeBins=2000;
  Double_t nBinsInNs=10;
  Double_t fSignalShape[2000];
  for (Int_t iMatch=0;iMatch<fMuchDigiMatches->GetEntriesFast();iMatch++) {
    nTotal++;
    printf("Match: %i ",iMatch);
    CbmMuchDigiMatch* match = (CbmMuchDigiMatch*) fMuchDigiMatches->At(iMatch);
    Int_t nElectrons = match->GetNoPrimaryElectrons();
    //printf("nElectrons: %i\n",nElectrons);
    for (Int_t j=0;j<nShapeTimeBins;j++) fSignalShape[j]=0;
    for (Int_t i=0;i<nElectrons;i++) {
      Int_t charge = match->GetChargePerPrimaryElectron(i); 
      Double_t driftTime = match->GetDriftTimePerPrimaryElectron(i); 
      //printf("  electron: %i charge: %i drift time: %f\n",i,charge,driftTime);
      for (Int_t j=0;j<nShapeTimeBins;j++){
        Int_t bin = Int_t(driftTime*nBinsInNs+j);
        if (bin>nShapeTimeBins) break;
        fSignalShape[bin]+=charge*fShape[j];
      }
      if (iMatch>99) continue;
      fhChargeVsTime[iMatch]->Fill(driftTime,charge);
    }
    Double_t max = 0;
    Double_t t_max = 0;
    Double_t t_thr = -1;
    Double_t thr = 10000;
    for (Int_t j=0;j<nShapeTimeBins;j++){
      if (fSignalShape[j]>max) {
        max = fSignalShape[j];
        t_max = j/nBinsInNs;
      }
      if (t_thr<0 && fSignalShape[j]>thr) t_thr=j/nBinsInNs;
    }
    if (t_thr<0) nLost++;
    printf(" max=%f",max);
    printf(" t_max=%f",t_max);
    printf(" \n");
    fhThresholdTime->Fill(t_thr);
    fhMaxTime->Fill(t_max);
    fhMaxCharge->Fill(max);

    if (iMatch>99) continue;
    for (Int_t j=0;j<nShapeTimeBins;j++) fhSignalShape[iMatch]->Fill(j/nBinsInNs,fSignalShape[j]);
  }
  
}

// -----   Public method Exec   --------------------------------------------
void CbmAnaTimingMuchDigitizer::TimeDistributions(){
  if (fEpoch) fMuchPoints = fMcEpoch->GetPoints(kMUCH);
  
  Int_t nMuchPoints = fMuchPoints ? fMuchPoints->GetEntriesFast() : 0;

  for (Int_t i=0;i<nMuchPoints;i++){
    FairMCPoint* point = (FairMCPoint*) fMuchPoints->At(i);
    Double_t x = point->GetX();
    Double_t y = point->GetY();
    Double_t t = point->GetTime();
//    fhPointXYT->Fill(x,y,t);
    fPointsTimeAll->Fill(t);
  }
}
// -------------------------------------------------------------------------




// -----   Public method Finish   ------------------------------------------
void CbmAnaTimingMuchDigitizer::Finish(){
  printf("nTotal=%i nLost=%i Eff=%f\n",nTotal,nLost,(nTotal-nLost)/Double_t(nTotal));
  TCanvas* cChargeVsTime = new TCanvas("cChargeVsTime","Charge vs Time",1900,1000);
  cChargeVsTime->Divide(5,5);
  for (Int_t i=0;i<25;i++){
    cChargeVsTime->cd(i+1);
    fhChargeVsTime[i]->Draw();
  }  

  TCanvas* cSignalShape = new TCanvas("cSignalShape","Signal shape",1900,1000);
  cSignalShape->Divide(5,5);
  for (Int_t i=0;i<25;i++){
    cSignalShape->cd(i+1);
    fhSignalShape[i]->Draw();
  }  

  TCanvas* cDeltaShape = new TCanvas("cDeltaShape","Delta shape",1900,1000);
  TH1D* hDeltaShape = new TH1D("hDeltaShape","hDeltaShape",1000,0,1000);
  for (Int_t i=0;i<1000;i++) hDeltaShape->Fill(i,fShape[i]);
  hDeltaShape->Draw();
  
  TCanvas* cThresholdTime = new TCanvas("cThresholdTime","Threshold time",1000,800);
  fhThresholdTime->Draw();
  
  TCanvas* cMaxTime = new TCanvas("cMaxTime","Max time",1000,800);
  fhMaxTime->Draw();

  TCanvas* cMaxCharge = new TCanvas("cMaxCharge","Max charge",1000,800);
  fhMaxCharge->Draw();

  
  return;
  
  
  TCanvas* cPointsTimeAll = new TCanvas("cPointsTimeAll","PointsTime",1200,400);
  fPointsTimeAll->Draw();
  
  
//  CbmMuchModuleGem* module = (CbmMuchModuleGem*) fGeoScheme->GetModule(0,0,0,fModuleId);
//  Double_t r    = module->GetCutRadius();
//  TVector3 size = module->GetSize();
//  TVector3 pos  = module->GetPosition();
//  printf("module:");
//  printf(" r=%.1f",r);
//  printf(" size=(%.1f,%.1f,%.1f)",size.x(),size.y(),size.z());
//  printf(" pos=(%.1f,%.1f,%.1f)",pos.x(),pos.y(),pos.z());
//  printf("\n");
//  
//  TCanvas* c1 = new TCanvas("c1","c1",1000,1000);
//  gPad->Range(pos.x()-size.x()/2.,pos.y()-size.y()/2.,pos.x()+size.x()/2.,pos.y()+size.y()/2.);
//
//  module->SetFillColor(kYellow);
//  module->SetFillStyle(3001);
//  module->Draw("f");
//  module->DrawModule(kYellow);
//  for (Int_t i=0;i<module->GetNSectors();i++){
//    CbmMuchSector* sector = module->GetSector(i);
//    printf("Sector:%i %f %f\n",i,sector->GetPosition().x(),sector->GetPosition().y());
////    sector->Draw("f");
////    sector->Draw();
//    for (Int_t p=0;p<sector->GetNChannels();p++){
//      CbmMuchPad* pad = sector->GetPad(p);
//      pad->Draw("f");
//      if (i==fChannelId) pad->SetLineColor(kRed);
//      pad->Draw();
//    }
//    sector->SetLineWidth(2);
//    sector->SetLineColor(kGreen+3);
//    sector->Draw();
//  }
//
//  
//  
//  
//  
//  TArc* cMin = new TArc(0,0,r,0,360);
//  cMin->SetFillColor(10);
//  cMin->Draw("f");
//
//  TCanvas* c2 = new TCanvas("c2","c2",1000,1000);
//  fhPointXYT->SetFillColor(kBlue);
//  fhPointXYT->SetLineColor(kRed);
//  fhPointXYT->Draw("BOX");
//
//  TCanvas* c3 = new TCanvas("c3","c3",1000,1000);
//  fhSectorPointXYT->SetFillColor(kBlue);
//  fhSectorPointXYT->SetLineColor(kRed);
//  fhSectorPointXYT->Draw("BOX");
//
//  TCanvas* c4 = new TCanvas("c4","c4",1000,1000);
//  fhDigiXYT->SetFillColor(kBlue);
//  fhDigiXYT->SetLineColor(kRed);
//  fhDigiXYT->Draw("BOX");
//  
//  TCanvas* c5 = new TCanvas("c5","c5",1000,1000);
//  fhSectorDigiXYT->SetFillColor(kBlue);
//  fhSectorDigiXYT->SetLineColor(kRed);
//  fhSectorDigiXYT->Draw("BOX");
//
//  TCanvas* c6 = new TCanvas("c6","c6",1600,1000);
//  c6->Divide(1,3);
//  c6->cd(1);
//  fhChannelT->SetFillColor(kBlue);
////  fhChannelT->SetLineColor(kRed);
//  fhChannelT->Draw();
//  c6->cd(2);
//  fhPointT->SetLineColor(kRed);
//  fhPointT->Draw();
//  c6->cd(3);
//  fhModuleT->SetLineColor(kBlue);
//  fhModuleT->Draw();
//  
//  TCanvas* c6_1 = new TCanvas("c6_1","c6_1",1600,500);
//  gPad->SetLeftMargin(0.02);
//  gPad->SetRightMargin(0.02);
//  gPad->SetTopMargin(0.10);
//  gPad->SetGridx(0);
//  gPad->SetGridy(0);
//  fhChannelT->SetFillColor(kBlue);
//  fhChannelT->SetLineColor(kBlue);
//  fhChannelT->Draw();
//  fhChannelT->SetMaximum(1.2);
//  fhChannelT->SetTitle("; time [ns];");
//  fhPointT->SetLineWidth(3);
//  fhPointT->SetLineColor(kRed);
//  fhPointT->Draw("same");
//  
//  TCanvas* c7 = new TCanvas("c7","c7",1600,400);
//  fhChannelHits->SetFillColor(kBlue);
//  fhChannelHits->SetLineColor(kRed);
//  fhChannelHits->Draw();
//  
//  for (Int_t i=1;i<=64;i++){
//    gPad->SetGridx(0);
//    gPad->SetGridy(0);
//    fhChannelHitDist->Fill(fhChannelHits->GetBinContent(i)); 
//  }
//
//  TCanvas* c8 = new TCanvas("c8","c8",800,400);
//  gPad->SetGridx(0);
//  gPad->SetGridy(0);
//  fhChannelHitDist->SetStats(kTRUE);
//  fhChannelHitDist->SetFillColor(kBlue);
////  fhChannelHitDist->SetLineColor(kRed);
//  fhChannelHitDist->SetTitle(";No of times each channel was fired in 10 us;Counts");
//  fhChannelHitDist->Draw("bar");
//  
//  TCanvas* c9 = new TCanvas("c9","c9",1900,1000);
//  c9->Divide(8,8,0.001,0.001);
//  for (Int_t i=0;i<64;i++){
//    c9->cd(i+1);
//    gPad->SetGridx(0);
//    gPad->SetGridy(0);
//    gPad->SetTopMargin(0.20);
//    gPad->SetBottomMargin(0.1);
//    fhhChannelT[i]->SetFillColor(kBlue);
//    fhhChannelT[i]->SetLineColor(kBlue);
//    fhhPointT[i]->SetLineWidth(2);
//    fhhPointT[i]->SetLineColor(kRed);
////    fhhPointT[i]->SetTitle(Form("Channel %i",i));
//    fhhPointT[i]->Draw();
//    fhhChannelT[i]->Draw("same");
//    TLatex* l = new TLatex(100,1.1,Form("Channel %i",i));
//    l->SetTextAlign(0);
//    l->SetTextSize(0.15);
//    l->Draw();
//  }
//  c9->cd();
  
  TCanvas* c10 = new TCanvas("c10","c10",1900,1000);
  c10->Divide(5,3);
  

  TFile* f = new TFile(fHistoName.Data(),"RECREATE");
  for (Int_t i=0;i<5;i++){
    fhStationTotalR[i]->Sumw2();
    fhStationFoundR[i]->Sumw2();
    fhStationEffR[i]->Divide(fhStationFoundR[i],fhStationTotalR[i]);
    c10->cd(i+1);
    gPad->SetGridx(0);
    gPad->SetGridy(0);
    fhStationTotalR[i]->Draw();   
    c10->cd(i+6);
    gPad->SetGridx(0);
    gPad->SetGridy(0);
    fhStationFoundR[i]->Draw();   
    c10->cd(i+11);
    gPad->SetGridx(0);
    gPad->SetGridy(0);
    fhStationEffR[i]->Draw();
    fhStationTotalR[i]->Write();
    fhStationFoundR[i]->Write();
    fhStationEffR[i]->Write();
    
  }
  c10->cd();
  fhChannelT->Write();
  fhPointT->Write();
  fhModuleT->Write();

//  fDetEventTime->Write();

  fPointsTimeAll->Write();
  f->Close();

}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void CbmAnaTimingMuchDigitizer::DetailedAnalysis(){
  if (fEpoch) fMuchPoints = fMcEpoch->GetPoints(kMUCH);
  
  Int_t nMuchPoints = fMuchPoints ? fMuchPoints->GetEntriesFast() : 0;
  Int_t nMuchDigis  = fMuchDigis  ? fMuchDigis->GetEntriesFast() : 0;
  if (fVerbose>-1) printf(" Event: %4i",fEvent++);
  if (fVerbose>0) printf(" nMuchPoints: %4i",nMuchPoints);
  if (fVerbose>0) printf(" nMuchDigis: %4i",nMuchDigis);
  if (fVerbose>-1) printf("\n");
   
  CbmMuchSector* sector = ((CbmMuchModuleGem*) fGeoScheme->GetModule(0,0,0,fModuleId))->GetSector(fSectorId);     
  TVector3 l = sector->GetSize();
  TVector3 p = sector->GetPosition();

  for (Int_t i=0;i<nMuchPoints;i++){
    FairMCPoint* point = (FairMCPoint*) fMuchPoints->At(i);
//    if (fVerbose>2) printf("  Much point: %i time=%f",i,point->GetTime());
//    if (fVerbose>2) printf("Detector Id: %i",point->GetDetectorID());
//    if (fVerbose>2) printf("\n");
    Double_t x = point->GetX();
    Double_t y = point->GetY();
    Double_t t = point->GetTime();
    if (fGeoScheme->GetModuleIndex(point->GetDetectorID())!=fModuleId) continue;
    fhPointXYT->Fill(x,y,t);
    if (x>p[0]-l[0]/2 && x <p[0]+l[0]/2 && y>p[1]-l[1]/2 && y<p[1]+l[1]/2)  fhSectorPointXYT->Fill(x,y,t);
  }

  for (Int_t i=0;i<nMuchDigis;i++) {
    CbmMuchDigi* digi = (CbmMuchDigi*) fMuchDigis->At(i);
    Int_t    detId     = digi->GetDetectorId();
    Long64_t channelId = digi->GetChannelId();
    Double_t time      = digi->GetTime();
    Double_t dead_time = digi->GetDeadTime();
    UInt_t   adcCharge = digi->GetADCCharge(); 
    if (fGeoScheme->GetStationIndex(detId)!=0) continue;
    if (fGeoScheme->GetLayerIndex(detId)!=0) continue;
    if (fGeoScheme->GetLayerSideIndex(detId)!=0) continue;
    if (fGeoScheme->GetModuleIndex(detId)!=fModuleId) continue;
    fhModuleT->Fill(time);
    CbmMuchModuleGem* module = (CbmMuchModuleGem*) fGeoScheme->GetModuleByDetId(detId);
    Int_t iSector  = module->GetSectorIndex(channelId);
    Int_t iChannel = module->GetChannelIndex(channelId);
    CbmMuchPad* pad = module->GetPad(channelId);
    Double_t x = pad->GetX0();
    Double_t y = pad->GetY0();
    fhDigiXYT->Fill(x,y,time,digi->GetADCCharge());
    if (iSector!=fSectorId) continue;
    fhSectorDigiXYT->Fill(x,y,time,digi->GetADCCharge());
    if (time>0 && time<10000) fhChannelHits->Fill(iChannel);
    for (Double_t t=time;t<time+dead_time;t++){
      fhhChannelT[iChannel]->Fill(t,0.2);
//      fhChannelT->Fill(t,0.2);
    }
    CbmMuchDigiMatch* match = (CbmMuchDigiMatch*) fMuchDigiMatches->At(i);
    for (Int_t ipoint=0;ipoint<match->GetNPoints();ipoint++){
      FairMCPoint* point = (FairMCPoint*) fMuchPoints->At(match->GetRefIndex(ipoint));
      Double_t point_time   = point->GetTime();
      Double_t point_charge = match->GetCharge(ipoint);
//      fhPointT->Fill(point_time,point_charge);
      fhhPointT[iChannel]->Fill(point_time);
    }

    
    
    if (iChannel!=fChannelId) continue;
    printf("time=%f dead_time=%f\n",time,dead_time);
//    fhChannelT->Fill(time);
    for (Double_t t=time;t<time+dead_time;t++){
      fhChannelT->Fill(t,0.2);
    }
//    CbmMuchDigiMatch* match = (CbmMuchDigiMatch*) fMuchDigiMatches->At(i);
    for (Int_t ipoint=0;ipoint<match->GetNPoints();ipoint++){
      FairMCPoint* point = (FairMCPoint*) fMuchPoints->At(match->GetRefIndex(ipoint));
      Double_t point_time   = point->GetTime();
      Double_t point_charge = match->GetCharge(ipoint);
//      fhPointT->Fill(point_time,point_charge);
      fhPointT->Fill(point_time);
    }
    
  }

  for (Int_t i=0;i<nMuchDigis;i++) {
    CbmMuchDigiMatch* match = (CbmMuchDigiMatch*) fMuchDigiMatches->At(i);
    for (Int_t ipoint=0;ipoint<match->GetNPoints();ipoint++){
      FairMCPoint* point = (FairMCPoint*) fMuchPoints->At(match->GetRefIndex(ipoint));
      Double_t x = point->GetX();
      Double_t y = point->GetY();
      Double_t r = sqrt(x*x+y*y);
      Int_t stationId = fGeoScheme->GetStationIndex(point->GetDetectorID()); 
      fhStationTotalR[stationId]->Fill(r);
      if (ipoint!=0) continue;
      fhStationFoundR[stationId]->Fill(r);
    }
  }
}
// -------------------------------------------------------------------------


ClassImp(CbmAnaTimingMuchDigitizer);
