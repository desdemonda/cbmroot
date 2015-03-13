// ------------------------------------------------------------------
// -----                    CbmFitGlobalTracks                  -----
// -----             Created 06/03/2006 by D.Kresan             -----
// ------------------------------------------------------------------
#include "CbmFitGlobalTracks.h"

#include "CbmGlobalTrack.h"
#include "CbmGlobalTrackFitter.h"

#include "FairRootManager.h"

#include "TClonesArray.h"

#include <iostream>

using std::cout;
using std::endl;

//___________________________________________________________________
//
// CbmFitGlobalTracks
//
// Task for global track fitting. Uses algorithm, provided by concre-
// te implementation of CbmGlobalTrackFitter class. This concrete en-
// gine is to be set in standard constructor or by UseFitter method.
//


// ------------------------------------------------------------------
CbmFitGlobalTracks::CbmFitGlobalTracks()
  : FairTask(),
    fVerbose(0),
    fFitter(NULL),
    fArrayGlbTrack(NULL)
{
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
CbmFitGlobalTracks::CbmFitGlobalTracks(const char* name, Int_t verbose,
				       CbmGlobalTrackFitter* fitter)
  : FairTask(name, verbose),
    fVerbose(verbose),
    fFitter(fitter),
    fArrayGlbTrack(NULL)
{
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
CbmFitGlobalTracks::~CbmFitGlobalTracks()
{
    // Destructor
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
InitStatus CbmFitGlobalTracks::Init()
{
    // Initialisation of the task. Initialise fitter
    if(NULL == fFitter) {
	cout << "-E- CbmFitGlobalTracks::Init : "
	    << "no track fitter selected!" << endl;
        return kERROR;
    }
    fFitter->Init();
    // Get pointer to the ROOT manager
    FairRootManager* rootMgr = FairRootManager::Instance();
    if(NULL == rootMgr) {
	cout << "-E- CbmFitGlobalTracks::Init : "
	    << "CBM ROOT manager is not instantiated!" << endl;
        return kERROR;
    }
    fArrayGlbTrack = (TClonesArray*) rootMgr->GetObject("GlobalTrack");
    if(NULL == fArrayGlbTrack) {
	cout << "-W- CbmFitGlobalTracks::Init : "
            << "no global track array!" << endl;
    }
    return kSUCCESS;
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
void CbmFitGlobalTracks::Exec(Option_t* option)
{
    // Task execution. Call DoFit of the fitter for each global track
    if(NULL != fArrayGlbTrack) {
        CbmGlobalTrack* glbTrack;
	for(Int_t iGlbTrack = 0;
	    iGlbTrack < fArrayGlbTrack->GetEntriesFast();
	    iGlbTrack++) {
	    // Get pointer to the global track
	    glbTrack = (CbmGlobalTrack*) fArrayGlbTrack->At(iGlbTrack);
	    if(NULL == glbTrack) continue;
	    // Fit the global track
            fFitter->DoFit(glbTrack);
	}
    }
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
void CbmFitGlobalTracks::Finish()
{
    // Finish of the task execution
}
// ------------------------------------------------------------------


ClassImp(CbmFitGlobalTracks);


