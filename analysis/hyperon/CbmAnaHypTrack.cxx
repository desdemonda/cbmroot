#include "CbmAnaHypTrack.h"
#include "CbmStsTrack.h"
#include "CbmKFTrack.h"

CbmAnaHypTrack::CbmAnaHypTrack()
  : CbmKFTrack(),
    fRcId(0),
    fMcId(0),
    fMoId(0),
    fMcPdg(0),
    fMoPdg(0),
    fChi(0.),
    fB(0.)
{
}    

CbmAnaHypTrack::CbmAnaHypTrack(CbmStsTrack track, Int_t rcId, Int_t mcId, Int_t moId,
                  Int_t mcPdg, Int_t moPdg, Double_t chi, Double_t b)
  : CbmKFTrack(track),
    fRcId(rcId),
    fMcId(mcId),
    fMoId(moId),
    fMcPdg(mcPdg),
    fMoPdg(moPdg),
    fChi(chi),
    fB(b)
{
}

