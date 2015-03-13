// -------------------------------------------------------------------------
// -----                    CbmMvdTrackMatch source file               -----
// -----                  Derived from CbmStsTrackMatch by V. Friese
// -----                  Created 16/06/09  by M. Deveaux              -----
// -------------------------------------------------------------------------


#include "CbmMvdTrackMatch.h"



// -----   Default constructor   -------------------------------------------
CbmMvdTrackMatch::CbmMvdTrackMatch() {
  fMCTrackId    = -1;
  fNofTrueHits  =  0;
  fNofWrongHits =  0;
  fNofFakeHits  =  0;
  fNofMCTracks  =  0;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmMvdTrackMatch::CbmMvdTrackMatch(Int_t mcTrackId, Int_t nTrue,
				   Int_t nWrong, Int_t nFake, 
				   Int_t nTracks) {
  fMCTrackId    = mcTrackId;
  fNofTrueHits  = nTrue;
  fNofWrongHits = nWrong;
  fNofFakeHits  = nFake;
  fNofMCTracks  = nTracks;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmMvdTrackMatch::~CbmMvdTrackMatch() {}
// -------------------------------------------------------------------------


ClassImp(CbmMvdTrackMatch)
