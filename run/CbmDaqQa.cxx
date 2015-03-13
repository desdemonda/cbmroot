/** @file CbmDaqQaQa.h
 ** @author Evgeny Kryshen <e.kryshen@gsi.de>
 ** @date 7 May 2013
 **/

#include "CbmDaqQa.h"
#include "CbmTimeSlice.h"
#include "CbmMuchDigi.h"
#include "CbmMuchDigiMatch.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "CbmMuchPixelHit.h"
#include "TChain.h"
#include "CbmMuchCluster.h"
#include "TClonesArray.h"

CbmDaqQa::CbmDaqQa(TChain* mcChain) 
 : FairTask("DaqQa"),
   fMCChain(mcChain),
   fTimeSlice(NULL),
   fMuchHits(NULL),
   fClusters(NULL),
   fTimeSTS(NULL),
   fTimeMUCH(NULL),
   fSignalShape(NULL),         
   fTimeMUCHhits(NULL),        
   fTimeOverThreshold(NULL),   
   fChargeVsTOT(NULL),         
   fTotChargeVsTOT(NULL),      
   fTotChargeVsAMP(NULL),      
   fTimeMUCHmc(NULL),          
   fTimeDiff(NULL), 
   fTimeDiff2(NULL)
{
}

InitStatus CbmDaqQa::Init() {
  // Get FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) fLogger->Fatal(MESSAGE_ORIGIN,"No FairRootManager");

  // Get event header
  fTimeSlice = (CbmTimeSlice*) ioman->GetObject("TimeSlice.");
  fMuchHits = (TClonesArray*) ioman->GetObject("MuchPixelHit");
  fClusters = (TClonesArray*) ioman->GetObject("MuchCluster");

  fTimeSTS = new TH1D("fTimeSTS",";t (ns);Number of digis",1200,0,1200);
  fTimeMUCH = new TH1D("fTimeMUCH",";t (ns);Number of digis",1200,0,1200);
  fTimeMUCHhits = new TH1D("fTimeMUCHhits",";t (ns);Number of hits",1200,0,1200);
  fSignalShape = new TH1D*[20];
  fTimeOverThreshold = new TH1D("fTimeOverThreshold",";time over threshold (ns); Number of digis",5000,0,5000);
  fChargeVsTOT = new TH2D("fChargeVsTOT",";;",100,0,1000,100,0,300);
  fTotChargeVsTOT = new TH2D("fTotChargeVsTOT",";Total charge (10^{6} electrons);Time over threshold, ns",300,0,3,300,0,300);
  fTotChargeVsAMP = new TH2D("fTotChargeVsAMP",";Total charge (10^{6} electrons);Max amplitude (a.u.)",100,0,1,100,0,500);
  fTimeMUCHmc = new TH1D("fTimeMUCH",";t (ns);Number of digis",1200,0,1200);
  for (Int_t i=0;i<20;i++) fSignalShape[i] = new TH1D(Form("fSignalShape%i",i),Form("%i",i),3000,0,3000);
  fTimeDiff = new TH1D("fTimeDiff",";RECO time - MC time (ns); Hits",100,-10,100);
  fTimeDiff2 = new TH2D("fTimeDiff",";RECO time - MC time (ns);Total charge (10^{6} el);",50,-10,100,50,0,1);
  return kSUCCESS;
}

void CbmDaqQa::Exec(Option_t* opt) {
  printf("Slice ... start time: %f\n",fTimeSlice->GetStartTime());
  printf("   number of STS points = %i\n",fTimeSlice->GetDataSize(kSTS));
  printf("   number of MUCH points = %i\n",fTimeSlice->GetDataSize(kMUCH));
  for (Int_t iDigi=0;iDigi<fTimeSlice->GetDataSize(kSTS);iDigi++){
    CbmMuchDigi* digi = (CbmMuchDigi*) fTimeSlice->GetData(kSTS,iDigi);
    fTimeSTS->Fill(digi->GetTime());
  }
  for (Int_t iDigi=0;iDigi<fTimeSlice->GetDataSize(kMUCH);iDigi++){
    CbmMuchDigi* digi = (CbmMuchDigi*) fTimeSlice->GetData(kMUCH,iDigi);
    fTimeMUCH->Fill(digi->GetTime());
    fTimeOverThreshold->Fill(digi->GetAdc());
    CbmMuchDigiMatch* match = digi->GetMatch();
    Double_t tot = match->GetTimeOverThreshold(10000);
    fChargeVsTOT->Fill(digi->GetAdc(),tot);
    fTotChargeVsTOT->Fill(match->GetTotalWeight()/1000000.,tot);
    fTotChargeVsAMP->Fill(match->GetTotalWeight()/1000000.,digi->GetAdc());
    if (iDigi>=20) continue;
//    printf("match=%p\n",match);
    if (!match) printf("No match!!!\n");
    TArrayD shape = match->GetSignalShape();
//    printf("%i\n",match->GetTotalCharge());
//    printf("%i\n",match->GetNoPrimaryElectrons());
    for (Int_t i=0;i<shape.GetSize();i++){
//      printf("%f\n",shape[i]);
      fSignalShape[iDigi]->SetBinContent(i,shape[i]);
    }
  }
  for (Int_t iHit=0;iHit<fMuchHits->GetEntriesFast();iHit++){
    CbmMuchPixelHit* hit = (CbmMuchPixelHit*) fMuchHits->At(iHit);
    fTimeMUCHhits->Fill(hit->GetTime());
  }
  
  for (Int_t iHit=0;iHit<fMuchHits->GetEntriesFast();iHit++){
    CbmMuchPixelHit* hit = (CbmMuchPixelHit*) fMuchHits->At(iHit);
    Double_t hitTime = hit->GetTime();
    Int_t iCluster = hit->GetRefId();
    CbmMuchCluster* cluster = (CbmMuchCluster*) fClusters->At(iCluster);
    Int_t mcRef=-1;
    Double_t mcTime = -1;
    for (Int_t iDigi=0;iDigi<cluster->GetNofDigis();iDigi++){
      Int_t iDigiIndex = cluster->GetDigi(iDigi);
//    for (Int_t iDigi=0;iDigi<fTimeSlice->GetDataSize(kMUCH);iDigi++){
      CbmMuchDigi* digi = (CbmMuchDigi*) fTimeSlice->GetData(kMUCH,iDigiIndex);
      Double_t t = digi->GetTime();
      CbmMuchDigiMatch* match = digi->GetMatch();
      for (Int_t el=0;el<match->GetNoPrimaryElectrons();el++){
        Int_t mcRefTmp = match->GetRefIndexPerPrimaryElectron(el);
        if (mcRef<0) mcRef=mcRefTmp;
        else if (mcRef!=mcRefTmp) {
          mcRef=-1;
          break;
        }
      }
      if (mcRef==-1) break;
      mcTime = match->GetMCtimePerPrimaryElectron(0);
      fTimeDiff2->Fill(t-mcTime,match->GetTotalWeight()/1000000.);
    }
    if (mcTime<0) continue;
    fTimeMUCHmc->Fill(mcTime);
    fTimeDiff->Fill(hitTime-mcTime);
  }
}

void CbmDaqQa::Finish() {
  gStyle->SetOptStat(0);
  TCanvas* ctime = new TCanvas("ctime","ctime",1200,800);
  gPad->SetRightMargin(0.02);
  gPad->SetTopMargin(0.02);
  fTimeSTS->SetLineColor(kRed);
  fTimeSTS->SetFillColor(kRed);
  fTimeMUCH->SetLineColor(kBlue);
  fTimeMUCH->SetFillColor(kBlue);
//  fTimeSTS->Draw();
//  fTimeMUCH->Draw("same");
  fTimeMUCH->Draw();
  fTimeMUCHmc->SetLineColor(kRed);
  fTimeMUCHmc->SetFillColor(kRed);
  fTimeMUCHmc->Draw("same");
  
  TCanvas* cSignalShapes = new TCanvas("cSignalShapes","cSignalShapes",1800,1000);
  cSignalShapes->Divide(5,4);
  for(Int_t ipad = 0;ipad<20;ipad++){
    cSignalShapes->cd(ipad+1);
    fSignalShape[ipad]->Draw();
  }

  
  TCanvas* ctimehits = new TCanvas("ctimehits","ctimehits",1200,600);
  gPad->SetRightMargin(0.02);
  gPad->SetTopMargin(0.02);
  fTimeMUCHhits->SetLineColor(kBlue);
  fTimeMUCHhits->SetFillColor(kBlue);
  fTimeMUCHhits->Draw();

  
  TCanvas* cTOT = new TCanvas("cTOT","cTOT",1200,600);
  gPad->SetRightMargin(0.02);
  gPad->SetTopMargin(0.02);
  fTimeOverThreshold->Draw();

  TCanvas* cChargeVsTOT = new TCanvas("cChargeVsTOT","cChargeVsTOT",1200,600);
  gPad->SetTopMargin(0.02);
  fChargeVsTOT->Draw("colz");

  TCanvas* cTotChargeVsTOT = new TCanvas("cTotChargeVsTOT","cTotChargeVsTOT",700,600);
  gPad->SetTopMargin(0.02);
  gPad->SetLogz();
  fTotChargeVsTOT->GetYaxis()->SetTitleOffset(1.2);
  fTotChargeVsTOT->Draw("colz");
  
  
  TCanvas* cTotChargeVsAMP = new TCanvas("cTotChargeVsAMP","cTotChargeVsAMP",700,600);
  gPad->SetTopMargin(0.02);
  gPad->SetLogz();
  fTotChargeVsAMP->GetYaxis()->SetTitleOffset(1.2);
  fTotChargeVsAMP->Draw("colz");
  
  TCanvas* cTimeDiff = new TCanvas("cTimeDiff","cTimeDiff",800,600);
  gPad->SetRightMargin(0.02);
  gPad->SetTopMargin(0.02);
  fTimeDiff->Draw();
  
  TCanvas* cTimeDiff2 = new TCanvas("cTimeDiff2","cTimeDiff2",800,600);
  gPad->SetTopMargin(0.02);
  fTimeDiff2->Draw("colz");
}

CbmDaqQa::~CbmDaqQa() {
}


ClassImp(CbmDaqQa)

