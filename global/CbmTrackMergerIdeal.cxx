// -------------------------------------------------------------------------
// -----                  CbmTrackMergerIdeal source file              -----
// -----                  Created 01/12/05  by V. Friese               -----
// -------------------------------------------------------------------------
#include "CbmTrackMergerIdeal.h"

#include "CbmGlobalTrack.h"

#include "FairRootManager.h"
#include "CbmTrackMatch.h"

#include "TClonesArray.h"

#include <iostream>
#include <map>

using std::cout;
using std::endl;
using std::map;


// -----   Default constructor   -------------------------------------------
CbmTrackMergerIdeal::CbmTrackMergerIdeal() 
  : CbmTrackMerger(),
    fStsMatch(NULL),
    fTrdMatch(NULL)
{
  fVerbose = 1;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmTrackMergerIdeal::~CbmTrackMergerIdeal() { }
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
void CbmTrackMergerIdeal::Init() {

  // Get and check FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if (! ioman) {
    cout << "-E- CbmStsTrackFinderIdeal::Init: "
	 << "RootManager not instantised!" << endl;
    return;
  }

  // Get StsTrackMatch array
  fStsMatch = (TClonesArray*) ioman->GetObject("StsTrackMatch");
  if ( ! fStsMatch ) {
    cout << "-W- CbmTrackMergerIdeal::Init: "
	 << "No StsTrackMatch array!" << endl;
  }

  // Get TrdTrackMatch array
  fTrdMatch = (TClonesArray*) ioman->GetObject("TrdTrackMatch");
  if ( ! fTrdMatch ) {
    cout << "-W- CbmTrackMergerIdeal::Init: "
	 << "No TrdTrackMatch array!" << endl;
  }

}
// -------------------------------------------------------------------------




// -----   Public method DoMerge   -----------------------------------------
Int_t CbmTrackMergerIdeal::DoMerge(TClonesArray* stsTracks,
				   TClonesArray* trdTracks,
				   TClonesArray* glbTracks) {

  // Check existence of output array
  if ( ! glbTracks ) {
    cout << "-E- CbmTrackMergerIdel::DoMerge: "
	 << "No GlobalTrack array!" << endl;
    return 0;
  }

  // Create some variables outside loops
  Int_t nMerged = 0;
  Int_t nGlb    = 0;
  Int_t nSts    = 0;
  Int_t nTrd    = 0;
  Int_t iMC     = 0;
  CbmTrackMatch* stsMatch = NULL;
  CbmTrackMatch* trdMatch = NULL;

  // Create map from TrdTrack index to StsTrack index
  map<Int_t, Int_t> trdStsMap;

  // Create map from MCTrack index tp TrdTrack index
  map<Int_t, Int_t> mcTrdMap;

  // Determine number of StsTracks and StsTrackMatches
  if ( stsTracks && fStsMatch ) {
    nSts = stsTracks->GetEntriesFast();
    if (nSts != fStsMatch->GetEntriesFast() ) {
      cout << "-E- CbmTrackMergerIdeal::DoMerge: "
	   << "Unequal array sizes of StsTrack and StsTrackMatch! ("
	   << nSts << ", " << fStsMatch->GetEntriesFast() << endl;
      nSts = 0;
    }
  }

  // Determine number of TrdTracks and TrdTrackMatches
  if ( trdTracks && fTrdMatch ) {
    nTrd = trdTracks->GetEntriesFast();
    if ( nTrd != fTrdMatch->GetEntriesFast() ) {
      cout << "-E- CbmTrackMergerIdeal::DoMerge: "
	   << "Unequal array sizes of TrdTrack and TrdTrackMatch! ("
	   << nTrd << ", " << fTrdMatch->GetEntriesFast() << endl;
      nTrd = 0;
    }
  }

  if (fVerbose>1) cout << endl << "-I- CbmTrackMergerIdeal: " << nSts
		       << " StsTracks, " << nTrd << " TrdTracks" << endl;

  // Loop over TrdTracks and fill map to MCTrack index
  for (Int_t iTrd=0; iTrd<nTrd; iTrd++) {
    trdMatch = (CbmTrackMatch*) fTrdMatch->At(iTrd);
    iMC = trdMatch->GetMCTrackId();
    if ( ! trdMatch ) {
      cout << "-W- CbmTrackMergerIdeal::DoMerge: "
	   << "Empty TrdTrackMatch at position " << iTrd << endl;
      continue;
    }
    if ( mcTrdMap.find(iMC) != mcTrdMap.end() ) {
      cout << "-W- CbmTrackMergerIdeal::DoMerge: "
	   << "MCTrack " << iMC << " matched to TrdTrack "
	   << mcTrdMap[iMC] << " and " << iTrd << endl;
      continue;
    }
    mcTrdMap[iMC] = iTrd;
  }

  // Loop over STS tracks, create Global track and attach TrdTrack
  for (Int_t iSts=0; iSts<nSts; iSts++) {
    stsMatch = (CbmTrackMatch*) fStsMatch->At(iSts);
    if ( ! stsMatch ) {
      cout << "-W- CbmTrackMergerIdeal::DoMerge: "
	   << "Empty StsTrackMatch at position " << iSts << endl;
      continue;
    }
    CbmGlobalTrack* gTrack
      = new((*glbTracks)[nGlb++]) CbmGlobalTrack();
    gTrack->SetStsTrackIndex(iSts);
    iMC = stsMatch->GetMCTrackId();
    if ( mcTrdMap.find(iMC) != mcTrdMap.end() ) {
      gTrack->SetTrdTrackIndex(mcTrdMap[iMC]);
      trdStsMap[mcTrdMap[iMC]] = iSts;
      nMerged++;
    }
  }
  if (fVerbose>1) {
    cout << "-I- CbmTrackMergerIdeal: " << nGlb
	 << " GlobalTracks created from StsTracks" << endl;
    cout << "-I- CbmTrackMergerIdeal: " << nMerged
	 << " StsTracks merged with TrdTracks" << endl;
  }

  // Loop over TRD tracks and create GlobalTracks for those not used
  Int_t nTrdOnly = 0;
  for (Int_t iTrd=0; iTrd<nTrd; iTrd++) {
    if ( trdStsMap.find(iTrd) == trdStsMap.end() ) {
      CbmGlobalTrack* gTrack
	= new ((*glbTracks)[nGlb++]) CbmGlobalTrack();
      gTrack->SetTrdTrackIndex(iTrd);
      nTrdOnly++;
    }
  }
  if (fVerbose>1) {
    cout << "-I- CbmTrackMergerIdeal: " << nTrdOnly
	 << " GlobalTracks created from TrdTracks" << endl;
    cout << "-I- CbmTrackMergerIdeal: Total " << nGlb
	 << " GlobalTracks created" << endl;
  }


  return nMerged;
}
// -------------------------------------------------------------------------





