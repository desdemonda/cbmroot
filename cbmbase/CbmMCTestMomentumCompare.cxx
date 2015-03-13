// -------------------------------------------------------------------------
// -----                CbmMCTestMomentumCompare source file             -----
// -----                  Created 18/07/08  by T.Stockmanns        -----
// -------------------------------------------------------------------------
// libc includes
#include <iostream>

// Root includes
#include "TROOT.h"
#include "TClonesArray.h"

// framework includes
#include "FairRootManager.h"
#include "CbmMCTestMomentumCompare.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairHit.h"
#include "FairMultiLinkedData.h"

#include "CbmMCTrack.h"
#include "CbmTrack.h"
#include "CbmMCEntry.h"


// -----   Default constructor   -------------------------------------------
CbmMCTestMomentumCompare::CbmMCTestMomentumCompare()
	: FairTask("Creates CbmMC test")
{
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmMCTestMomentumCompare::~CbmMCTestMomentumCompare()
{
}

// -----   Public method Init   --------------------------------------------
InitStatus CbmMCTestMomentumCompare::Init()
{


//  fMCMatch->InitStage(kMCTrack, "", "MCTrack");



  FairRootManager* ioman = FairRootManager::Instance();
  	if (!ioman) {
  		std::cout << "-E- CbmMCTestMomentumCompare::Init: "
  				<< "RootManager not instantiated!" << std::endl;
  		return kFATAL;
  	}

  	fMCMatch = (CbmMCMatch*)ioman->GetObject("MCMatch");

  	fTrack = (TClonesArray*)ioman->GetObject("LheGenTrack");
  	fMCTrack = (TClonesArray*)ioman->GetObject("MCTrack");

	std::cout << "-I- CbmMCTestMomentumCompare::Init: Initialization successfull" << std::endl;


  return kSUCCESS;
}


// -------------------------------------------------------------------------
void CbmMCTestMomentumCompare::SetParContainers()
{
  // Get Base Container
//  FairRun* ana = FairRun::Instance();
//  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

}


// -----   Public method Exec   --------------------------------------------
void CbmMCTestMomentumCompare::Exec(Option_t* opt)
{
// 	//fMCMatch->CreateArtificialStage(kMCTrack, "", "");
// 
// 	CbmMCResult myResult = fMCMatch->GetMCInfo(kTrack, kMCTrack);
// 	std::cout << myResult;
// 	for (int i = 0; i < myResult.GetNEntries(); i++){
// 		CbmMCEntry myLinks = myResult.GetMCLink(i);
// 		CbmTrack* myTrack = (CbmTrack*)fTrack->At(i);
// 		std::cout << "TrackMatch for Track " << i << std::endl;
// 		//myTrack->Print();
// 		std::cout << "P: " << myTrack->GetParamFirst().GetSDMomentum().Mag() << std::endl;
// 		std::cout << "Belongs to: " << std::endl;
// 		for (int j = 0; j < myLinks.GetNLinks(); j++){
// 			if (myLinks.GetLink(j).GetType() == kMCTrack){
// 				std::cout << "MCTrack " << myLinks.GetLink(j).GetIndex() << std::endl;
// 				CbmMCTrack* myMCTrack = (CbmMCTrack*)fMCTrack->At(myLinks.GetLink(j).GetIndex());
// 				//myMCTrack->Print(myLinks.GetFairLink(j).GetIndex());
// 				std::cout << "P: " << myMCTrack->GetMomentum().Mag() << " PID: " << myMCTrack->GetPdgCode() << std::endl;
// 				std::cout << "--------------------------------" << std::endl;
// 			}
// 		}
// 		std::cout << std::endl;
// 	}
}

void CbmMCTestMomentumCompare::Finish()
{
}


ClassImp(CbmMCTestMomentumCompare);
