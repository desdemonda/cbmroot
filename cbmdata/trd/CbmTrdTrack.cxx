// -------------------------------------------------------------------------
// -----                      CbmStsTrack source file                  -----
// -----                  Created 11/07/05  by M. Kalisky              -----
// -----                  Modified 04/06/09 by A. Lebedev               -----
// -------------------------------------------------------------------------
#include "CbmTrdTrack.h"

// -----   Default constructor   -------------------------------------------
CbmTrdTrack::CbmTrdTrack()
  : CbmTrack(),
    fPidWkn(-1.),
    fPidANN(-1.),
    fPidLikeEL(-1.),
    fPidLikePI(-1.),
    fPidLikeKA(-1.),
    fPidLikePR(-1.),
    fPidLikeMU(-1.),
    fELoss(0.)
{
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmTrdTrack::~CbmTrdTrack()
{
}
// -------------------------------------------------------------------------

ClassImp(CbmTrdTrack);
