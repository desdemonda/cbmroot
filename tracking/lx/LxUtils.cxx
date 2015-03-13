#include "Lx.h"

using namespace std;

// Used for building of the background on an invariant mass.
void LxFinder::SaveEventTracks()
{
  for (list<LxTrack*>::iterator i = caSpace.tracks.begin(); i != caSpace.tracks.end(); ++i)
  {
    LxTrack* firstTrack = *i;

    if (0 == firstTrack->externalTrack)
      continue;

    CbmStsTrack t = *firstTrack->externalTrack->track;

    if (t.GetParamLast()->GetQp() > 0)
      extFitter.DoFit(&t, -13);
    else
      extFitter.DoFit(&t, 13);

    Double_t chi2Prim = extFitter.GetChiToVertex(&t, fPrimVtx);
    FairTrackParam params;
    extFitter.Extrapolate(&t, fPrimVtx->GetZ(), &params);

    Double_t p = 1 / params.GetQp();
    Double_t p2 = p * p;

    if (p2 < 9)
      continue;

    Double_t tx2 = params.GetTx() * params.GetTx();
    Double_t ty2 = params.GetTy() * params.GetTy();
    Double_t pt2 = p2 * (tx2 + ty2) / (1 + tx2 + ty2);

    if (pt2 < 1)
      continue;

    t.SetParamFirst(&params);
    *superEventData = t;
    superEventTracks->Fill();
  }
}
