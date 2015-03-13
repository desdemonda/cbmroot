// -------------------------------------------------------------------------
// -----                    CbmSttTrackMatch source file               -----
// -----                  Created 01/12/05  by V. Friese               -----
// -----          Adapted for STT 13/03/08  by A. Zinchenko            -----
// -------------------------------------------------------------------------

#include "CbmSttTrackMatch.h"

// -----   Default constructor   -------------------------------------------
CbmSttTrackMatch::CbmSttTrackMatch() 
  : TObject(),
    fMCTrackID(-1),
    fNofTrueHits(0),
    fNofWrongHits(0),
    fNofFakeHits(0),
    fNofMCTracks(0)
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmSttTrackMatch::CbmSttTrackMatch(Int_t mcTrackID, Int_t nTrue,
				   Int_t nWrong, Int_t nFake, 
				   Int_t nTracks) 
  : TObject(),
    fMCTrackID(mcTrackID),
    fNofTrueHits(nTrue),
    fNofWrongHits(nWrong),
    fNofFakeHits(nFake),
    fNofMCTracks(nTracks)
{
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmSttTrackMatch::~CbmSttTrackMatch() {}
// -------------------------------------------------------------------------

ClassImp(CbmSttTrackMatch)
