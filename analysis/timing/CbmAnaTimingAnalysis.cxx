/** CbmAnaTimingAnalysis.cxx
 *@author E.Kryshen <e.kryshen@gsi.de>
 *@since 2010-09-07
 **/

#include "CbmAnaTimingAnalysis.h"
#include "CbmMCTrack.h"
//#include "CbmStsPoint.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairMCPoint.h"

#include "TClonesArray.h"
#include "TH3D.h"
#include "TH1D.h"
#include "TVector3.h"

// -----   Default constructor   -------------------------------------------
CbmAnaTimingAnalysis::CbmAnaTimingAnalysis(){
  fEvent=0;
  fHistoName = "histo.root";
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
CbmAnaTimingAnalysis::CbmAnaTimingAnalysis(const char* name, TString histoName)
:FairTask(name){
  fEvent=0;
  fHistoName = histoName;
}

// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmAnaTimingAnalysis::~CbmAnaTimingAnalysis(){
}
// -------------------------------------------------------------------------


// -----  SetParContainers -------------------------------------------------
void CbmAnaTimingAnalysis::SetParContainers(){
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
InitStatus CbmAnaTimingAnalysis::Init()
{
  // Get and check FairRootManager
  FairRootManager* fManager = FairRootManager::Instance();
  fMCTracks         = (TClonesArray*) fManager->GetObject("MCTrack");
  fStsPoints        = (TClonesArray*) fManager->GetObject("StsPoint");
  fMuchPoints       = (TClonesArray*) fManager->GetObject("MuchPoint");
  fTofPoints        = (TClonesArray*) fManager->GetObject("TofPoint");
  fTrdPoints        = (TClonesArray*) fManager->GetObject("TrdPoint");
  fEvent=0;
  
  Int_t    nDet = 10;
  Int_t    nEvent = 1000;
  Int_t    nTimeBins = 100;
  Double_t timeMax = 100;
  
  fDetEventTime = new TH3D("hDetEventTime","",nDet,0.,nDet,nEvent,0,nEvent,nTimeBins,0,timeMax);
  fDetEventTime->GetXaxis()->SetBinLabel(1,"STS");
  fDetEventTime->GetXaxis()->SetBinLabel(2,"MCH");
  fDetEventTime->GetXaxis()->SetBinLabel(3,"TOF");
  fDetEventTime->GetXaxis()->SetBinLabel(4,"TRD");
  
  fNPrimaryTracks = new TH1D("hNPrimaryTracks","",200,0,2000);
  fNPrimaryTracksInAcceptance = new TH1D("hNPrimaryTracksInAcceptance","",100,0,1000);
  
  fStsPointsZ  = new TH1D("hStsPointsZ" ,"",1000,0,101);
  fMuchPointsZ = new TH1D("hMuchPointsZ","",1000,100,400);
  fTofPointsZ  = new TH1D("hTofPointsZ" ,"",1000,998,1002);
  fTrdPointsZ  = new TH1D("hTrdPointsZ" ,"",1000,100,960);
  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void CbmAnaTimingAnalysis::Exec(Option_t* opt){
  Int_t nMCTracks      = fMCTracks ? fMCTracks->GetEntriesFast() : 0;
  Int_t nStsPoints     = fStsPoints ? fStsPoints->GetEntriesFast() : 0;
  Int_t nMuchPoints    = fMuchPoints ? fMuchPoints->GetEntriesFast() : 0;
  Int_t nTofPoints     = fTofPoints ? fTofPoints->GetEntriesFast() : 0;
  Int_t nTrdPoints     = fTrdPoints ? fTrdPoints->GetEntriesFast() : 0;
  if (fVerbose>-1) printf(" Event: %4i",fEvent);
  if (fVerbose>0) printf(" MCtracks: %4i",nMCTracks);
  if (fVerbose>0) printf(" StsPoints: %4i",nStsPoints);
  if (fVerbose>0) printf(" MuchPoints: %4i",nMuchPoints);
  if (fVerbose>0) printf(" TofPoints: %4i",nTofPoints);
  if (fVerbose>0) printf(" TrdPoints: %4i",nTrdPoints);
  if (fVerbose>-1) printf("\n");
  
   
  for (Int_t i=0;i<nStsPoints;i++){
    FairMCPoint* point = (FairMCPoint*) fStsPoints->At(i);
    if (fVerbose>2) printf("  Sts point: %i time=%f",i,point->GetTime());
    if (fVerbose>2) printf("Detector Id: %i",point->GetDetectorID());
    if (fVerbose>2) printf("\n");
    Double_t x = point->GetX();
    Double_t y = point->GetY();
    Double_t z = point->GetZ();
    Double_t tan_theta = (z>0) ? sqrt(x*x+y*y)/z : 0;
    if (tan_theta<0.1) continue;
    if (tan_theta>0.2) continue;
    if (z>29.6 && z<30)  fDetEventTime->Fill(0.+0.1,fEvent+0.1,point->GetTime());
    if (z>99.6 && z<100) fDetEventTime->Fill(1.+0.1,fEvent+0.1,point->GetTime());
    fStsPointsZ->Fill(z);
  }
  
  for (Int_t i=0;i<nMuchPoints;i++){
    FairMCPoint* point = (FairMCPoint*) fMuchPoints->At(i);
    if (fVerbose>2) printf("  Much point: %i time=%f",i,point->GetTime());
    if (fVerbose>2) printf("Detector Id: %i",point->GetDetectorID());
    if (fVerbose>2) printf("\n");
    Double_t x = point->GetX();
    Double_t y = point->GetY();
    Double_t z = point->GetZ();
    Double_t tan_theta = (z>0) ? sqrt(x*x+y*y)/z : 0;
    if (tan_theta<0.1) continue;
    if (tan_theta>0.2) continue;
    fMuchPointsZ->Fill(z);
    if (z>127.5 && z<128.5)  fDetEventTime->Fill(2.+0.1,fEvent+0.1,point->GetTime());
    if (z>312 && z<314)      fDetEventTime->Fill(3.+0.1,fEvent+0.1,point->GetTime());
  }

  for (Int_t i=0;i<nTofPoints;i++){
    FairMCPoint* point = (FairMCPoint*) fTofPoints->At(i);
    if (fVerbose>2) printf("  Tof point: %i time=%f",i,point->GetTime());
    if (fVerbose>2) printf("Detector Id: %i",point->GetDetectorID());
    if (fVerbose>2) printf("\n");
    Double_t x = point->GetX();
    Double_t y = point->GetY();
    Double_t z = point->GetZ();
    Double_t tan_theta = (z>0) ? sqrt(x*x+y*y)/z : 0;
    if (tan_theta<0.1) continue;
    if (tan_theta>0.2) continue;
    if (z>1000 && z<1000.2)  fDetEventTime->Fill(4.+0.1,fEvent+0.1,point->GetTime());
    fTofPointsZ->Fill(z);
  }

  for (Int_t i=0;i<nTrdPoints;i++){
    FairMCPoint* point = (FairMCPoint*) fTrdPoints->At(i);
    if (fVerbose>2) printf("  Trd point: %i time=%f",i,point->GetTime());
    if (fVerbose>2) printf("Detector Id: %i",point->GetDetectorID());
    if (fVerbose>2) printf("\n");
    Double_t x = point->GetX();
    Double_t y = point->GetY();
    Double_t z = point->GetZ();
    Double_t tan_theta = (z>0) ? sqrt(x*x+y*y)/z : 0;
    if (tan_theta<0.1) continue;
    if (tan_theta>0.2) continue;
    if (z>947.5 && z<948) fDetEventTime->Fill(5.+0.1,fEvent+0.1,point->GetTime());
    if (z>947.5 && z<948) fDetEventTime->Fill(6.+0.1,fEvent+0.1,point->GetTime());
    fTrdPointsZ->Fill(z);
  }

  
  TVector3 p;
  Int_t nPrimaryTracks = 0;
  Int_t nPrimaryTracksInAcceptance = 0;
  for (Int_t i=0;i<nMCTracks;i++){
    CbmMCTrack* track = (CbmMCTrack*) fMCTracks->At(i);
    if (track->GetMotherId()>=0) continue;
    track->GetMomentum(p);
    if (fVerbose>1) printf(" MC Track: %i",i);
    if (fVerbose>1) printf("\n");
    Double_t tan_theta = (p.Pz()>0) ? p.Pt()/p.Pz() : 0;
    if (tan_theta>0.1 && tan_theta<0.5) nPrimaryTracksInAcceptance++; 
    nPrimaryTracks++;
  }
  fNPrimaryTracks->Fill(nPrimaryTracks);
  fNPrimaryTracksInAcceptance->Fill(nPrimaryTracksInAcceptance);
  fEvent++;
  
  
}
// -------------------------------------------------------------------------


// -----   Public method Finish   ------------------------------------------
void CbmAnaTimingAnalysis::Finish(){
  TFile* f = new TFile(fHistoName.Data(),"RECREATE");
  fDetEventTime->Write();
  fNPrimaryTracks->Write();
  fNPrimaryTracksInAcceptance->Write();
  fStsPointsZ->Write();
  fMuchPointsZ->Write();
  fTofPointsZ->Write();
  fTrdPointsZ->Write();
  f->Close();
}
// -------------------------------------------------------------------------

ClassImp(CbmAnaTimingAnalysis);
