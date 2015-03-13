// -------------------------------------------------------------------------
// -----                   CbmTrdFindTracks source file                -----
// -----                  Created 25/11/05  by D. Kresan               -----
// -----                 according to the CbmStsFindTracks             -----
// -------------------------------------------------------------------------

#include "CbmTrdFindTracks.h"

#include "CbmTrdTrack.h"
#include "CbmTrdTrackFinder.h"
#include "CbmTrdHit.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"

#include <iostream>
using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
CbmTrdFindTracks::CbmTrdFindTracks()
  : FairTask(),
    fFinder(NULL),
    fTrdHitArray(NULL),
    fTrackArray(NULL),
    fNofTracks(-1)
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmTrdFindTracks::CbmTrdFindTracks(const char* name,
				   const char* title,
				   CbmTrdTrackFinder* finder)
  : FairTask(name),
    fFinder(finder),
    fTrdHitArray(NULL),
    fTrackArray(NULL),
    fNofTracks(-1)
{
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmTrdFindTracks::~CbmTrdFindTracks()
{
  fTrackArray->Delete();
}
// -------------------------------------------------------------------------



// -----   Public method Init (abstract in base class)  --------------------
InitStatus CbmTrdFindTracks::Init()
{

  // Check for Track finder
  if (! fFinder) {
    cout << "-W- CbmTrdFindTracks::Init: No track finder selected!" << endl;
    return kERROR;
  }

  // Get and check FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if (! ioman) {
    cout << "-E- CbmTrdFindTracks::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  // Get TRD hit Array
  fTrdHitArray
    = (TClonesArray*) ioman->GetObject("TrdHit");
  if ( ! fTrdHitArray) {
    cout << "-W- CbmTrdFindTracks::Init: No TrdHit array!"
	 << endl;
    return kERROR;
  }

  // Create and register TrdTrack array
  fTrackArray = new TClonesArray("CbmTrdTrack",100);
  ioman->Register("TrdTrack", "TRD", fTrackArray, kTRUE);


  // Call the Init method of the track finder
  fFinder->Init();

  return kSUCCESS;

}
// -------------------------------------------------------------------------



// -----  SetParContainers -------------------------------------------------
void CbmTrdFindTracks::SetParContainers()
{
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb = ana->GetRuntimeDb();
  //  rtdb->getContainer("CbmGeoPassivePar");
 //   rtdb->getContainer("CbmGeoStsPar");
 //   rtdb->getContainer("CbmGeoTrdPar");
    rtdb->getContainer("FairBaseParSet");
    rtdb->getContainer("CbmGeoPassivePar");
    rtdb->getContainer("CbmGeoStsPar");
    rtdb->getContainer("CbmGeoRichPar");
    rtdb->getContainer("CbmGeoTrdPar");
    rtdb->getContainer("CbmFieldPar");
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmTrdFindTracks::Exec(Option_t* opt)
{
  fTrackArray->Delete();
  fNofTracks = fFinder->DoFind(fTrdHitArray, fTrackArray);
  for (Int_t iTrack=0; iTrack<fTrackArray->GetEntriesFast(); iTrack++) {
    CbmTrdTrack* track = (CbmTrdTrack*) fTrackArray->At(iTrack);
    Double_t eloss = 0;
    for (Int_t iTRD=0; iTRD < track->GetNofHits(); iTRD++){
      Int_t TRDindex = track->GetHitIndex(iTRD);
      CbmTrdHit* trdHit = (CbmTrdHit*) fTrdHitArray->At(TRDindex);
      eloss += trdHit->GetELoss();
    }
    track->SetELoss(eloss);
  }
}
// -------------------------------------------------------------------------



// -----   Public method Finish   ------------------------------------------
void CbmTrdFindTracks::Finish()
{
  fTrackArray->Clear();
}
// -------------------------------------------------------------------------




ClassImp(CbmTrdFindTracks)
