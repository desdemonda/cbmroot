
// -------------------------------------------------------------------------
// -----                CbmPsdReactionPlaneMaker.h source file             -----
// -----                  Created 15/05/12  by     Alla &Marina                -----
// -------------------------------------------------------------------------
#include <iostream>

#include "TClonesArray.h"
#include "TMath.h"
#include "TH1F.h"

#include "FairRootManager.h"

#include "CbmPsdReactionPlaneMaker.h"
#include "CbmPsdHit.h"

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmPsdReactionPlaneMaker::CbmPsdReactionPlaneMaker() :
  FairTask("PsdReactionPlaneMaker",1),
  fHitArray(NULL),
  fCbmPsdEvent(NULL)
 { 
  //  Reset();
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmPsdReactionPlaneMaker::~CbmPsdReactionPlaneMaker() 
{
   if ( fHitArray ) {
    fHitArray->Delete();
    delete fHitArray;
  }
}
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus CbmPsdReactionPlaneMaker::Init() {

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- CbmPsdReactionPlaneMaker::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  // Get input array
  fHitArray = (TClonesArray*) ioman->GetObject("PsdHit");
  if ( ! fHitArray ) {
    cout << "-W- CbmPsdReactionPlaneMaker::Init: "
	 << "No PSD hits array!" << endl;
    return kERROR;
  }
 // Create and register output array
  fCbmPsdEvent = new CbmPsdEventData("");
  ioman->Register("PsdEvent", "PSDEvent", fCbmPsdEvent, kTRUE);


  TFile *fhist = new TFile("EdepHistos.root");
  TH1F* hModXNewEn = (TH1F*) fhist->Get("hModXNewEn");
  fMean = hModXNewEn->GetMean();

  ifstream fxypos("psd_geo_xy.txt");
  for (Int_t ii=0; ii<44; ii++) {
    fxypos>>fXi[ii]>>fYi[ii];
   
  }
  fxypos.close();

   cout << "-I- CbmPsdReactionPlaneMaker: Intialisation successfull " << kSUCCESS<< endl;
  return kSUCCESS;

}


// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmPsdReactionPlaneMaker::Exec(Option_t* opt) {


  // Reset output array
   if ( ! fHitArray ) Fatal("Exec", "No PsdHit array");
   //  Reset();
   
  // Declare some variables
  CbmPsdHit* hit = NULL;
  Float_t edep[44];
 
  for (Int_t imod=0; imod<44; imod++)  edep[imod]=0;
 
  // Loop over PsdHits
  Int_t nHits = fHitArray->GetEntriesFast();
  for (Int_t ihit=0; ihit<nHits; ihit++) {
    hit = (CbmPsdHit*) fHitArray->At(ihit);
    if ( !hit) continue;
    Int_t mod = hit->GetModuleID();
    edep[mod] = hit->GetEdep(mod);
  }// Loop over hits
    
  
  Double_t pxShift=0, pyShift=0;
  Double_t qXWeightedShift=0,qYWeightedShift=0;
  Double_t radToDeg=180./TMath::Pi();
  
     
  for (Int_t mod=0; mod<44; mod++) {
    pyShift = fYi[mod]; 
    pxShift = fXi[mod] - fMean;
    if(edep[mod]>0) {
      qXWeightedShift += pxShift*sqrt(edep[mod]);
      qYWeightedShift += pyShift*sqrt(edep[mod]); 
    }//if(edep[mod]>0)
  }//for (Int_t mod=0; mod<44; mod++)
  
  if(qXWeightedShift!=0 && qYWeightedShift!=0) {
    Float_t rpa = TMath::ATan2(qYWeightedShift,qXWeightedShift)*radToDeg;
    fCbmPsdEvent->SetRP(rpa);

  }//if(qXWeightedShift!=0 && qYWeightedShift!=0)
  
} 
// -----   Private method Reset   ------------------------------------------
void CbmPsdReactionPlaneMaker::Reset() {
  fNHits = 0;
  if ( fHitArray ) fHitArray->Delete();
  
}

ClassImp(CbmPsdReactionPlaneMaker)
