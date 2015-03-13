// -------------------------------------------------------------------------
// -----                CbmMCTestHitCompare source file             -----
// -----                  Created 18/07/08  by T.Stockmanns        -----
// -------------------------------------------------------------------------
#include "CbmMCTestHitCompare.h"

#include "CbmMCEntry.h"
#include "CbmMCMatch.h"
#include "CbmStsPoint.h"
#include "CbmStsHit.h"

// framework includes
#include "FairRootManager.h"

// Root includes
#include "TClonesArray.h"
#include "TMath.h"
#include "TH2F.h"
#include "TList.h"

// libc includes
#include <iostream>
using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
CbmMCTestHitCompare::CbmMCTestHitCompare()
  : FairTask("Creates CbmMC test"),
    fMCMatch(NULL),
    fStripHit(NULL),
    fMCPoint(NULL),
    fhHitPointCorrelation(NULL),
    fhHitPointSCorrelationSmall(NULL),
    fhHitPointSCorrelationLarge(NULL),
    fHistoList(NULL)
{
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmMCTestHitCompare::~CbmMCTestHitCompare()
{
}

// -----   Public method Init   --------------------------------------------
InitStatus CbmMCTestHitCompare::Init()
{


//  fMCMatch->InitStage(kMCTrack, "", "MCTrack");



  FairRootManager* ioman = FairRootManager::Instance();
  	if (!ioman) {
  		cout << "-E- CbmMCTestHitCompare::Init: "
  				<< "RootManager not instantiated!" << endl;
  		return kFATAL;
  	}

  	fMCMatch = (CbmMCMatch*)ioman->GetObject("MCMatch");

  	fStripHit = (TClonesArray*)ioman->GetObject("StsHit");
  	fMCPoint = (TClonesArray*)ioman->GetObject("StsPoint");

	cout << "-I- CbmMCTestHitCompare::Init: Initialization successfull" << endl;
  CreateHistos();
  Reset();

  return kSUCCESS;
}


// -------------------------------------------------------------------------
void CbmMCTestHitCompare::SetParContainers()
{
  // Get Base Container
//  FairRun* ana = FairRun::Instance();
//  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

}


// -----   Public method Exec   --------------------------------------------
void CbmMCTestHitCompare::Exec(Option_t* opt)
{
	//fMCMatch->CreateArtificialStage(kMCTrack, "", "");

  CbmMCResult myResult = fMCMatch->GetMCInfo(Cbm::kStsHit, Cbm::kStsPoint);
	cout << myResult;
	for (int i = 0; i < myResult.GetNEntries(); i++){
		CbmMCEntry myLinks = myResult.GetMCLink(i);
		CbmStsHit* myHit = (CbmStsHit*)fStripHit->At(i);

		cout << "Hit Match for hit " << i << " at (" << myHit->GetX() << "," << myHit->GetY() << "," << myHit->GetZ() << ")" << endl;
		if (myLinks.GetNLinks()<2){
		  for (int j = 0; j < myLinks.GetNLinks(); j++){
		    if (myLinks.GetLink(j).GetType() == Cbm::kStsPoint){
				
		      CbmStsPoint* myMCPoint = (CbmStsPoint*)fMCPoint->At(myLinks.GetLink(j).GetIndex());
		      //myMCTrack->Print(myLinks.GetFairLink(j).GetIndex());
		      cout << "MCPoint " << myLinks.GetLink(j).GetIndex() << " at (" << myMCPoint->GetX(myMCPoint->GetZ()) << "," << 
		      myMCPoint->GetY(myMCPoint->GetZ()) << "," << 
		      myMCPoint->GetZ() << ")" <<endl;
		      fhHitPointCorrelation -> Fill ( myHit->GetX()-myMCPoint->GetX(myMCPoint->GetZ()),
						      myHit->GetY()-myMCPoint->GetY(myMCPoint->GetZ()));
		      cout << "--------------------------------" << endl;
		    }
		  }
		}
		else {
		  for (int j = 0; j < myLinks.GetNLinks(); j++){
		    if (myLinks.GetLink(j).GetType() == Cbm::kStsPoint){
				
		      CbmStsPoint* myMCPoint = (CbmStsPoint*)fMCPoint->At(myLinks.GetLink(j).GetIndex());
		      //myMCTrack->Print(myLinks.GetFairLink(j).GetIndex());
		      cout << "MCPoint " << myLinks.GetLink(j).GetIndex() << " at (" << myMCPoint->GetX(myMCPoint->GetZ()) << "," << 
		      myMCPoint->GetY(myMCPoint->GetZ()) << "," << 
		      myMCPoint->GetZ() << ")" <<endl;
		      if ( TMath::Abs (myHit->GetX()-myMCPoint->GetX(myMCPoint->GetZ())) < 1. && TMath::Abs (myHit->GetY()-myMCPoint->GetY(myMCPoint->GetZ())) < 1.) {
		        fhHitPointSCorrelationSmall -> Fill( myHit->GetX()-myMCPoint->GetX(myMCPoint->GetZ()),
						        myHit->GetY()-myMCPoint->GetY(myMCPoint->GetZ()));
		      }
		      else {
			fhHitPointSCorrelationLarge -> Fill( myHit->GetX()-myMCPoint->GetX(myMCPoint->GetZ()),
						              myHit->GetY()-myMCPoint->GetY(myMCPoint->GetZ()));
		      }
		      cout << "--------------------------------" << endl;
		    }
		  }
		}
		cout << endl;
	}
}
// -------------------------------------------------------------------------

    


// -----   Private method CreateHistos   -----------------------------------
void CbmMCTestHitCompare::CreateHistos() {


  // Histogram list
  fHistoList = new TList();
  

  fhHitPointCorrelation  = new TH2F("hHitPointCorrelation","Hit vs point correlation",
					  2500,-1, 1,2500,-1,1);
  fhHitPointSCorrelationSmall = new TH2F("hHitPointSCorrelationSmall","Hit vs points correlation small",
					  2500,-1, 1,2500,-1,1);
  fhHitPointSCorrelationLarge = new TH2F("hHitPointSCorrelationLarge","Hit vs point correlation large",
					  2500,-50, 50,2500,-50,50);
  fHistoList->Add(fhHitPointCorrelation);
  fHistoList->Add(fhHitPointSCorrelationSmall);
  fHistoList->Add(fhHitPointSCorrelationLarge);


}
// -------------------------------------------------------------------------



// -----   Private method Reset   ------------------------------------------
void CbmMCTestHitCompare::Reset() {


  TIter next(fHistoList);
  while ( TH2* histo = ((TH2*)next()) ) histo->Reset();
//   for ( Int_t ist = 0 ; ist < fNStations ; ist++ ) {
//     TIter next0(fHistoListPS[ist]);
//     while ( TH1* histo = ((TH1*)next0()) ) histo->Reset();
//   }

}
// -------------------------------------------------------------------------
void CbmMCTestHitCompare::Finish()
{
  gDirectory->mkdir("Histos");
  gDirectory->cd("Histos");
  TIter next(fHistoList);
  while ( TH2* histo = ((TH2*)next()) ) histo->Write();


  gDirectory->cd("..");

}


ClassImp(CbmMCTestHitCompare);
