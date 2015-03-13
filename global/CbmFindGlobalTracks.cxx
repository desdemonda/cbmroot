// -------------------------------------------------------------------------
// -----                  CbmFindGlobalTracks source file              -----
// -----                  Created 01/12/05  by V. Friese               -----
// -------------------------------------------------------------------------
#include "CbmFindGlobalTracks.h"

#include "CbmTrackMerger.h"
#include "CbmRichMerger.h"
#include "CbmTofMerger.h"

#include "FairRootManager.h"

#include "TClonesArray.h"

#include <iostream>

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmFindGlobalTracks::CbmFindGlobalTracks() 
  : FairTask("Global Tracker"), 
    fTrackMerger(NULL),
    fRichMerger(NULL),
    fTofMerger(NULL),
    fStsTracks(NULL),
    fTrdTracks(NULL),
    fRichRings(NULL),
    fTofHits(NULL),
    fGlobalTracks(NULL),
    fVerbose(1),
    fNAll(0),
    fNStsOnly(0),
    fNTrdOnly(0),
    fNWithRich(0),
    fNWithTof(0)
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmFindGlobalTracks::CbmFindGlobalTracks(CbmTrackMerger* trackMerger,
					 CbmRichMerger* richMerger,
                                         CbmTofMerger* tofMerger,
					 Int_t iVerbose) 
  : FairTask("Global Tracker"),
    fTrackMerger(trackMerger),
    fRichMerger(richMerger),
    fTofMerger(tofMerger),
    fStsTracks(NULL),
    fTrdTracks(NULL),
    fRichRings(NULL),
    fTofHits(NULL),
    fGlobalTracks(NULL),
    fVerbose(iVerbose),
    fNAll(0),
    fNStsOnly(0),
    fNTrdOnly(0),
    fNWithRich(0),
    fNWithTof(0)
{  
}
// -------------------------------------------------------------------------



// -----   Constructor with name and title   -------------------------------
CbmFindGlobalTracks::CbmFindGlobalTracks(const char* name, 
					 const char* title,
					 CbmTrackMerger* trackMerger,
					 CbmRichMerger* richMerger,
                                         CbmTofMerger* tofMerger,
					 Int_t iVerbose)
  : FairTask(name),
    fTrackMerger(trackMerger),
    fRichMerger(richMerger),
    fTofMerger(tofMerger),
    fStsTracks(NULL),
    fTrdTracks(NULL),
    fRichRings(NULL),
    fTofHits(NULL),
    fGlobalTracks(NULL),
    fVerbose(iVerbose),
    fNAll(0),
    fNStsOnly(0),
    fNTrdOnly(0),
    fNWithRich(0),
    fNWithTof(0)
{
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmFindGlobalTracks::~CbmFindGlobalTracks() {
  if ( fGlobalTracks ) {
    fGlobalTracks->Delete();
    delete fGlobalTracks;
  }
}
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus CbmFindGlobalTracks::Init() {

  // Check for Track merger
  if (! fTrackMerger) {
    cout << "-W- CbmFindGlobalTracks::Init: No track merger selected!" 
	 << endl;
    return kERROR;
  }

  // Check for Rich merger
  if (! fRichMerger) {
    cout << "-W- CbmFindGlobalTracks::Init: No RICH merger selected!" 
	 << endl;
  }

  // Check for Tof merger
  if(! fTofMerger) {
      cout << "-W- CbmFindGlobalTracks::Init: No TOF merger selected!"
          << endl;
  }

  // Get and check FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if (! ioman) {
    cout << "-E- CbmFindGlobalTracks::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  // Get StsTrack array
  fStsTracks = (TClonesArray*) ioman->GetObject("StsTrack");
  if ( ! fStsTracks) {
    cout << "-W- CbmStsFindTracks::Init: No StsTrack array!"
	 << endl;
  }

  // Get TrdTrack array
  fTrdTracks = (TClonesArray*) ioman->GetObject("TrdTrack");
  if ( ! fTrdTracks) {
    cout << "-W- CbmStsFindTracks::Init: No TrdTrack array!"
	 << endl;
  }

  // Get RichRing array
  fRichRings = (TClonesArray*) ioman->GetObject("RICHRing");
  if ( ! fRichRings) {
    cout << "-W- CbmStsFindTracks::Init: No RichRing array!"
	 << endl;
  }

  // Get TofHit array
  fTofHits = (TClonesArray*) ioman->GetObject("TofHit");
  if(! fTofHits) {
      cout << "-W- CbmFindGlobalTracks::Init: No TofHit array!"
          << endl;
  }

  // Create and register GlobalTrack array
  fGlobalTracks = new TClonesArray("CbmGlobalTrack",100);
  ioman->Register("GlobalTrack", "Global", fGlobalTracks, kTRUE);


  // Set verbosities of mergers
  if ( fTrackMerger) fTrackMerger->SetVerbose(fVerbose);
  if ( fRichMerger)  fRichMerger->SetVerbose(fVerbose);
  if ( fTofMerger )  fTofMerger->SetVerbose(fVerbose);


  // Call Init method of the mergers
  if ( fTrackMerger) fTrackMerger->Init();
  if ( fRichMerger)  fRichMerger->Init();
  if ( fTofMerger )  fTofMerger->Init();

  return kSUCCESS;

}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmFindGlobalTracks::Exec(Option_t* opt) {
  fGlobalTracks->Clear();
  Int_t nSts  = 0;
  Int_t nTrd  = 0;
  Int_t nRich = 0;
  Int_t nTof  = 0;
  if ( fStsTracks ) nSts  = fStsTracks->GetEntriesFast();
  if ( fTrdTracks ) nTrd  = fTrdTracks->GetEntriesFast();
  if ( fRichRings ) nRich = fRichRings->GetEntriesFast();
  if ( fTofHits   ) nTof  = fTofHits->GetEntriesFast();
  Int_t nMergedTracks = 0;
  if ( fTrackMerger) nMergedTracks = fTrackMerger->DoMerge(fStsTracks, 
							   fTrdTracks, 
							   fGlobalTracks);
  Int_t nMergedRings = 0;
  if ( fRichMerger ) nMergedRings  = fRichMerger->DoMerge(fGlobalTracks, 
							  fRichRings);
  Int_t nMergedTofHits = 0;
  if ( fTofMerger ) nMergedTofHits = fTofMerger->DoMerge(fGlobalTracks,
							 fTofHits);
  Int_t nAll = fGlobalTracks->GetEntriesFast();
  if (fVerbose) {
    cout << endl;
    cout << "-------------------------------------------------------" 
	 << endl;
    cout << "-I-              Global Track Finding               -I-"
	 << endl;
    cout << "Reconstructed STSTracks : " << nSts << endl;
    cout << "Reconstructed TrdTracks : " << nTrd << endl;
    cout << "STS and TRD merged      : " << nMergedTracks << endl;
    cout << "Reconstructed RICHrings : " << nRich << endl;
    cout << "RICH rings attached     : " << nMergedRings << endl
  	 << "TOF hits attached       : " << nMergedTofHits << endl;
    cout << "--------------------------------------------------------" 
	 << endl;
  }
  else cout << "-I- CbmFindGlobalTracks: " << nAll << " global tracks, "
	    << nMergedTracks << " merged, " << nMergedRings 
	    << " with RICH ring, " << nMergedTofHits << " with TOF hit"
	    << endl;
}
// -------------------------------------------------------------------------



ClassImp(CbmFindGlobalTracks)

