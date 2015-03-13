// ----------------------------------------------------------------------------
// -----                   CbmRichProtPrepareExtrapolation                -----
// -----                   Created 26-07-2010 by D.Kresan                 -----
// ----------------------------------------------------------------------------



#include <iostream>
using namespace std;

#include "TClonesArray.h"
#include "TRandom.h"
#include "TMath.h"

#include "FairRootManager.h"
#include "FairTrackParam.h"
#include "CbmMCTrack.h"
#include "CbmStsTrack.h"
#include "CbmGlobalTrack.h"
#include "CbmRichProtPrepareExtrapolation.h"



// ----------------------------------------------------------------------------
CbmRichProtPrepareExtrapolation::CbmRichProtPrepareExtrapolation()
    : FairTask("CbmRichProtPrepareExtrapolation"),
      fPosReso(0.),
      fThetaReso(0.),
      fArrayMCTracks(NULL),
      fArrayStsTracks(NULL),
      fArrayGlobalTracks(NULL)
{
}
// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------
CbmRichProtPrepareExtrapolation::CbmRichProtPrepareExtrapolation(Int_t verbose)
    : FairTask("CbmRichProtPrepareExtrapolation"),
      fPosReso(0.),
      fThetaReso(0.),
      fArrayMCTracks(NULL),
      fArrayStsTracks(NULL),
      fArrayGlobalTracks(NULL)
{
    fVerbose = verbose;
}
// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------
CbmRichProtPrepareExtrapolation::~CbmRichProtPrepareExtrapolation()
{
}
// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------
void CbmRichProtPrepareExtrapolation::SetParContainers()
{
}
// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------
InitStatus CbmRichProtPrepareExtrapolation::Init()
{
    FairRootManager *mgr = FairRootManager::Instance();
    if(NULL == mgr) {
        return kFATAL;
    }

    fArrayMCTracks = (TClonesArray*) mgr->GetObject("MCTrack");
    if(NULL == fArrayMCTracks) {
        return kFATAL;
    }

    fArrayStsTracks = new TClonesArray("CbmStsTrack");
    mgr->Register("StsTrack", "STS", fArrayStsTracks, kTRUE);

    fArrayGlobalTracks = new TClonesArray("CbmGlobalTrack");
    mgr->Register("GlobalTrack", "Global", fArrayGlobalTracks, kTRUE);

    return kSUCCESS;
}
// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------
void CbmRichProtPrepareExtrapolation::Exec(Option_t *option)
{
    if(NULL == fArrayMCTracks || NULL == fArrayStsTracks ||
       NULL == fArrayGlobalTracks) {
        return;
    }


    Int_t nMCTracks = fArrayMCTracks->GetEntriesFast();
    CbmMCTrack *mcTrack;
    Int_t motherId;
    Int_t pdg;
    Float_t x, y;
    Float_t pt;
    Float_t theta, phi;
    Float_t p;
    Float_t px, py, pz;
    Float_t tx, ty;
    Float_t qp;
    Int_t nStsTracks = 0;
    CbmStsTrack *stsTrack;
    Int_t charge;
    CbmGlobalTrack *track;
    for(Int_t i = 0; i < nMCTracks; i++) {
	mcTrack = (CbmMCTrack*) fArrayMCTracks->At(i);
	if(NULL == mcTrack) {
            continue;
	}

	motherId = mcTrack->GetMotherId();
	if(-1 != motherId) {
            continue;
	}

	pdg = mcTrack->GetPdgCode();
	if(11 != pdg && -11 != pdg &&
	   211 != pdg && -211 != pdg) {
            continue;
	}

        x = gRandom->Gaus(mcTrack->GetStartX(), fPosReso);
	y = gRandom->Gaus(mcTrack->GetStartY(), fPosReso);
	pt = TMath::Sqrt(TMath::Power(mcTrack->GetPx(),2)+
			 TMath::Power(mcTrack->GetPy(),2));
	p = TMath::Sqrt(TMath::Power(pt,2)+
			TMath::Power(mcTrack->GetPz(),2));
	theta = TMath::ATan2(pt, mcTrack->GetPz());
        theta = gRandom->Gaus(theta, fThetaReso);
	phi = TMath::ATan2(mcTrack->GetPy(), mcTrack->GetPx());
	pz = p*TMath::Cos(theta);
	pt = p*TMath::Sin(theta);
	px = pt*TMath::Cos(phi);
	py = pt*TMath::Sin(phi);

	new ((*fArrayStsTracks)[nStsTracks]) CbmStsTrack();
	stsTrack = (CbmStsTrack*) fArrayStsTracks->At(nStsTracks);
	if(NULL == stsTrack) {
            continue;
	}
	FairTrackParam paramLast(*stsTrack->GetParamLast());
	//paramLast = stsTrack->GetParamLast();
	paramLast.SetX(x);
	paramLast.SetY(y);
	paramLast.SetZ(mcTrack->GetStartZ());
        paramLast.SetTx(px/pz);
	paramLast.SetTy(py/pz);
        /*paramLast->SetCovariance(0, 0, 2.56);
        paramLast->SetCovariance(1, 1, 0.5476);
        paramLast->SetCovariance(2, 2, 0.0308);
        paramLast->SetCovariance(2, 2, 0.0308);*/
	if(11 == pdg || -211 == pdg) {
            charge = -1;
	} else {
            charge = 1;
	}
	if(p > 0) {
	    paramLast.SetQp((Double_t)charge/p);
	}
	new ((*fArrayGlobalTracks)[nStsTracks]) CbmGlobalTrack();
	track = (CbmGlobalTrack*) fArrayGlobalTracks->At(nStsTracks);
	if(NULL == track) {
            continue;
	}
        track->SetStsTrackIndex(nStsTracks);
	nStsTracks += 1;
        track->SetParamLast(&paramLast);
    }


    if(fVerbose) {
	cout << "-I- CbmRichProtPrepareExtrapolation : " << nStsTracks
	    << " STS tracks are created" << endl;
    }
}
// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------
void CbmRichProtPrepareExtrapolation::Finish()
{
}
// ----------------------------------------------------------------------------



ClassImp(CbmRichProtPrepareExtrapolation)



