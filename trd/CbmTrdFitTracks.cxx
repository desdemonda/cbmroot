// -------------------------------------------------------------------------
// -----                    CbmTrdFitTracks source file                -----
// -----                  Created 28/11/05  by D. Kresan               -----
// -----                 according to the CbmStsFitTracks              -----
// -------------------------------------------------------------------------

#include "CbmTrdFitTracks.h"

#include "CbmTrdTrackFitter.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"

#include <iostream>
using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmTrdFitTracks::CbmTrdFitTracks() 
  : FairTask(),
    fFitter(NULL),
    fTrackArray(NULL),
    fNofTracks(-1)
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmTrdFitTracks::CbmTrdFitTracks(const char* name,
				 const char* title,
				 CbmTrdTrackFitter* fitter)
  : FairTask(name),
    fFitter(fitter),
    fTrackArray(NULL),
    fNofTracks(-1)
{
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmTrdFitTracks::~CbmTrdFitTracks() { }
// -------------------------------------------------------------------------



// -----  SetParContainers -------------------------------------------------
void CbmTrdFitTracks::SetParContainers()
{
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb = ana->GetRuntimeDb();
    rtdb->getContainer("CbmGeoPassivePar");
    rtdb->getContainer("CbmGeoStsPar");
    rtdb->getContainer("CbmGeoTrdPar");
}
// -------------------------------------------------------------------------



// -----   Public method Init (abstract in base class)  --------------------
InitStatus CbmTrdFitTracks::Init() {

  // Check for Track fitter
  if (! fFitter) {
    cout << "-E- CbmTrdFitTracks: No track fitter selected!" << endl;
    Fatal("CbmTrdFitTracks","No fitter selected");
    return kERROR;
  }

  // Get and check FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if (! ioman) {
    cout << "-E- CbmTrdFitTracks::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  // Get TrdTrack array
  fTrackArray  = (TClonesArray*) ioman->GetObject("TrdTrack"); //=>SG
  if ( ! fTrackArray) {
    cout << "-E- CbmTrdFitTracks::Init: No TrdTrack array!"
	 << endl;
    return kERROR;
  }

  // Call the Init method of the track fitter
  fFitter->Init();

  return kSUCCESS;

}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmTrdFitTracks::Exec(Option_t* opt) {
  if ( !fTrackArray ) return; // =>SG
  Int_t nTracks = fTrackArray->GetEntriesFast();
  for (Int_t iTrack=0; iTrack<nTracks; iTrack++) {
    CbmTrdTrack* pTrack = (CbmTrdTrack*)fTrackArray->At(iTrack);
    fFitter->DoFit(pTrack);
  }
}
// -------------------------------------------------------------------------



// -----   Public method Finish   ------------------------------------------
void CbmTrdFitTracks::Finish() { }
// -------------------------------------------------------------------------




ClassImp(CbmTrdFitTracks)
