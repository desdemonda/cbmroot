/** CbmAnaEpochTest.cxx
 *@author E.Kryshen <e.kryshen@gsi.de>
 *@since 2010-09-07
 **/

#include "CbmAnaEpochTest.h"
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
#include "CbmMCEpoch.h"

// -----   Default constructor   -------------------------------------------
CbmAnaEpochTest::CbmAnaEpochTest(){
  fNepoch=0;
  fHistoName = "histo.root";
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
CbmAnaEpochTest::CbmAnaEpochTest(const char* name, TString histoName)
:FairTask(name){
  fNepoch=0;
  fHistoName = histoName;
}

// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmAnaEpochTest::~CbmAnaEpochTest(){
}
// -------------------------------------------------------------------------


// -----  SetParContainers -------------------------------------------------
void CbmAnaEpochTest::SetParContainers(){
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
InitStatus CbmAnaEpochTest::Init()
{
  // Get and check FairRootManager
  FairRootManager* fManager = FairRootManager::Instance();
  fEpoch = (CbmMCEpoch*) fManager->GetObject("MCEpoch.");
  fNepoch=0;
  
  fStsTimeFull      = new TH1D("hStsTimeFull"     ,";time,ns;Number of MC points",20000,0,10000);
  fStsTimeCentral   = new TH1D("hStsTimeCentral"  ,";time,ns;Number of MC points",20000,0,10000);
  fMuchTimeFull     = new TH1D("hMuchTimeFull"    ,";time,ns;Number of MC points",20000,0,10000);
  fMuchTimeCentral  = new TH1D("hMuchTimeCentral" ,";time,ns;Number of MC points",20000,0,10000);
  fStsTimeFull->SetLineColor(kBlue);
  fStsTimeCentral->SetLineColor(kBlue);
  fMuchTimeFull->SetLineColor(kBlue);
  fMuchTimeCentral->SetLineColor(kBlue);

  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void CbmAnaEpochTest::Exec(Option_t* opt){
  fNepoch++;
//  if (fNepoch>1) return;
  if (fVerbose>-1) printf("Epoch: %i",fNepoch);
  Int_t nStsPoints = fEpoch->GetNofPoints(kSTS);
  Int_t nMuchPoints = fEpoch->GetNofPoints(kMUCH);
  if (fVerbose>-1) printf("Sts points: %i  ",nStsPoints);
  if (fVerbose>-1) printf("Much points: %i",nMuchPoints);
  if (fVerbose>-1) printf("\n");
  
  
  for (Int_t i=0;i<nStsPoints;i++){
    FairMCPoint* point = fEpoch->GetPoint(kSTS,i);
    if (fVerbose>2) printf("  Sts point: %i time=%f",i,point->GetTime());
    if (fVerbose>2) printf("\n");
    Double_t x = point->GetX();
    Double_t y = point->GetY();
    Double_t z = point->GetZ();
    Double_t tan_theta = (z>0) ? sqrt(x*x+y*y)/z : 0;
    if (z>29.6 && z<30)  fStsTimeFull->Fill(point->GetTime());
    if (tan_theta<0.1) continue;
    if (tan_theta>0.2) continue;
    if (z>29.6 && z<30)  fStsTimeCentral->Fill(point->GetTime());
//    if (z>99.6 && z<100) fStsTime8->Fill(point->GetTime());
  }
  
  for (Int_t i=0;i<nMuchPoints;i++){
    FairMCPoint* point = fEpoch->GetPoint(kMUCH,i);
    if (fVerbose>2) printf("  Much point: %i time=%f",i,point->GetTime());
    if (fVerbose>2) printf("\n");
    Double_t x = point->GetX();
    Double_t y = point->GetY();
    Double_t z = point->GetZ();
    Double_t tan_theta = (z>0) ? sqrt(x*x+y*y)/z : 0;
    if (z>127.5 && z<128.5)  fMuchTimeFull->Fill(point->GetTime());
    if (tan_theta<0.1) continue;
    if (tan_theta>0.2) continue;
    if (z>127.5 && z<128.5)  fMuchTimeCentral->Fill(point->GetTime());
  }

}
// -------------------------------------------------------------------------


// -----   Public method Finish   ------------------------------------------
void CbmAnaEpochTest::Finish(){
  TFile* f = new TFile(fHistoName.Data(),"RECREATE");
  fStsTimeFull->Write();
  fStsTimeCentral->Write();
  fMuchTimeFull->Write();
  fMuchTimeCentral->Write();
  f->Close();
}
// -------------------------------------------------------------------------

ClassImp(CbmAnaEpochTest);
