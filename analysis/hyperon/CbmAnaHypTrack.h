#ifndef CBMANAHYPTRACK_H
#define CBMANAHYPTRACK_H

#include "CbmKFTrack.h"

class CbmAnaHypTrack : public CbmKFTrack{
  public:
    CbmAnaHypTrack();
    CbmAnaHypTrack(CbmStsTrack track, Int_t rcId, Int_t mcId,
        Int_t moId, Int_t mcPdg, Int_t moPdg,
        Double_t chi, Double_t b);
    ~CbmAnaHypTrack(){};
    Int_t fRcId;
    Int_t fMcId;
    Int_t fMoId;
    Int_t fMcPdg;
    Int_t fMoPdg;
    Double_t fChi;
    Double_t fB;
};

#endif
