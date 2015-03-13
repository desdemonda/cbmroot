// ------------------------------------------------------------------
// -----                      CbmProduceDst                     -----
// -----             Created 2008-01-18 by D.Kresan             -----
// ------------------------------------------------------------------

#include <iostream>
using namespace std;

#include "TClonesArray.h"
#include "TMath.h"
#include "TH2F.h"

#include "FairRootManager.h"
#include "CbmMCTrack.h"
#include "CbmTofPoint.h"
#include "CbmStsTrack.h"
#include "CbmTrackMatch.h"
#include "CbmTrdTrack.h"
#include "CbmTofHit.h"
#include "CbmGlobalTrack.h"
#include "CbmVertex.h"
#include "CbmStsKFTrackFitter.h"
#include "CbmHadron.h"
#include "CbmProduceDst.h"


// ------------------------------------------------------------------
CbmProduceDst::CbmProduceDst()
  : FairTask("CbmProduceDst"),
    fEvents(0),
    fHadrons(0),
    fArrayMCTrack(NULL),
    fArrayTofPoint(NULL),
    fArrayStsTrack(NULL),
    fArrayStsTrackM(NULL),
    fArrayTrdTrack(NULL),
    fArrayTrdTrackM(NULL),
      fArrayTofHit(NULL),
    fArrayGlobalTrack(NULL),
    fPrimVertex(NULL),
    fTrackFitter(new CbmStsKFTrackFitter()),
    fArrayHadron(new TClonesArray("CbmHadron")),
    fh_pdl(NULL)
{
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
CbmProduceDst::CbmProduceDst(Int_t iVerbose)
  : FairTask("CbmProduceDst", iVerbose),
    fEvents(0),
    fHadrons(0),
    fArrayMCTrack(NULL),
    fArrayTofPoint(NULL),
    fArrayStsTrack(NULL),
    fArrayStsTrackM(NULL),
    fArrayTrdTrack(NULL),
    fArrayTrdTrackM(NULL),
      fArrayTofHit(NULL),
    fArrayGlobalTrack(NULL),
    fPrimVertex(NULL),
    fTrackFitter(new CbmStsKFTrackFitter()),
    fArrayHadron(new TClonesArray("CbmHadron")),
    fh_pdl(NULL)
{
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
CbmProduceDst::~CbmProduceDst()
{
    fEvents = 0;
    fHadrons = 0;
    fArrayMCTrack = fArrayTofPoint = fArrayStsTrack = fArrayStsTrackM =
	fArrayTrdTrack = fArrayTrdTrackM = fArrayTofHit = fArrayGlobalTrack = NULL;
    fPrimVertex = NULL;
    delete fTrackFitter;
    delete fArrayHadron;
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
InitStatus CbmProduceDst::Init()
{
    // Get pointer to the CBM root manager
    FairRootManager *rootMgr = FairRootManager::Instance();

    fArrayMCTrack = (TClonesArray*) rootMgr->GetObject("MCTrack");
    if(NULL == fArrayMCTrack) {
	cout << "-E- CbmProduceDst::Init : "
	    << "no MCTrack array!" << endl;
        return kERROR;
    }

    fArrayTofPoint = (TClonesArray*) rootMgr->GetObject("TofPoint");
    if(NULL == fArrayTofPoint) {
	cout << "-E- CbmProduceDst::Init : "
	    << "no TOFPoint array!" << endl;
        return kERROR;
    }

    fArrayStsTrack = (TClonesArray*) rootMgr->GetObject("StsTrack");
    if(NULL == fArrayStsTrack) {
	cout << "-E- CbmProduceDst::Init : "
	    << "no STSTrack array!" << endl;
        return kERROR;
    }

    fArrayStsTrackM = (TClonesArray*) rootMgr->GetObject("StsTrackMatch");
    if(NULL == fArrayStsTrackM) {
	cout << "-E- CbmProduceDst::Init : "
	    << "no STSTrackMatch array!" << endl;
        return kERROR;
    }

    fArrayTrdTrack = (TClonesArray*) rootMgr->GetObject("TrdTrack");
    if(NULL == fArrayTrdTrack) {
	cout << "-W- CbmProduceDst::Init : "
	    << "no TRDTrack array!" << endl;
    }

    fArrayTrdTrackM = (TClonesArray*) rootMgr->GetObject("TrdTrackMatch");
    if(NULL == fArrayTrdTrackM) {
	cout << "-W- CbmProduceDst::Init : "
	    << "no TRDTrackMatch array!" << endl;
    }

    fArrayTofHit = (TClonesArray*) rootMgr->GetObject("TofHit");
    if(NULL == fArrayTofHit) {
	cout << "-E- CbmProduceDst::Init : "
	    << "no TofHit array!" << endl;
        return kERROR;
    }

    fArrayGlobalTrack = (TClonesArray*) rootMgr->GetObject("GlobalTrack");
    if(NULL == fArrayGlobalTrack) {
	cout << "-E- CbmProduceDst::Init : "
	    << "no GlobalTrack array!" << endl;
        return kERROR;
    }

    fPrimVertex = (CbmVertex*) rootMgr->GetObject("PrimaryVertex");
    if(NULL == fPrimVertex) {
	cout << "-E- CbmProduceDst::Init : "
	    << "no PrimaryVertex!" << endl;
        return kERROR;
    }

    rootMgr->Register("Hadron", "Hadrons", fArrayHadron, kTRUE);

    fh_pdl = new TH2F("h_pdl", "Length resolution vs. momentum",
		      100, 0., 10., 200, -10., 10.);

    return kSUCCESS;
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
InitStatus CbmProduceDst::ReInit()
{
    return Init();
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
void CbmProduceDst::SetParContainers()
{
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
void CbmProduceDst::Exec(Option_t *option)
{
    if(NULL == fArrayTofPoint || NULL == fArrayStsTrack ||
       NULL == fArrayTofHit || NULL == fArrayGlobalTrack) {
	return;
    }


    fHadrons = 0;
    fArrayHadron->Clear();


    // Loop over Global tracks
    CbmGlobalTrack *track;
    CbmStsTrack *stsTrack;
    CbmTrackMatch *stsTrackM;
    CbmTrdTrack *trdTrack;
    CbmTrackMatch *trdTrackM;
    CbmTofHit *tofHit;
    Int_t refIndex;
    CbmTofPoint *tofPoint;
    CbmMCTrack *mcTrack;
    Bool_t ghost;
    Bool_t tdh;
    Int_t pdg;
    Double_t qp;
    Int_t charge;
    FairTrackParam paramExtr;
    Double_t p;
    Double_t pt;
    Double_t pz;
    Int_t nMvdHits;
    Int_t nStsHits;
    Int_t nTrdHits;
    Double_t b;
    Double_t tof;
    Double_t l;
    Double_t mass2;
    for(Int_t iGlobal = 0;
	iGlobal < fArrayGlobalTrack->GetEntriesFast();
	iGlobal++) {
	// Get pointer to the Global track
	track = (CbmGlobalTrack*) fArrayGlobalTrack->At(iGlobal);
	if(NULL == track) continue;

	// Get pointer to the STS track
	if(track->GetStsTrackIndex() < 0) continue;
	stsTrack = (CbmStsTrack*) fArrayStsTrack->
	    At(track->GetStsTrackIndex());
	if(NULL == stsTrack) continue;
	nMvdHits = stsTrack->GetNofMvdHits();
        nStsHits = stsTrack->GetNofHits();

	// Get pointer to the STS track match
	if(track->GetStsTrackIndex() < 0) continue;
	stsTrackM = (CbmTrackMatch*) fArrayStsTrackM->
	    At(track->GetStsTrackIndex());
	if(NULL == stsTrackM) continue;

	// Get pointer to the TRD track and TRD track match
	if(track->GetTrdTrackIndex() > -1) {
	    trdTrack = (CbmTrdTrack*) fArrayTrdTrack->
		At(track->GetTrdTrackIndex());
	    if(NULL == trdTrack) continue;
	    nTrdHits = trdTrack->GetNofHits();
	    trdTrackM = (CbmTrackMatch*) fArrayTrdTrackM->
		At(track->GetTrdTrackIndex());
	    if(NULL == trdTrackM) continue;
	} else {
	    nTrdHits = 0;
	}

        // Get pointer to the TOF hit
	if(track->GetTofHitIndex() < 0) continue;
	tofHit = (CbmTofHit*) fArrayTofHit->
	    At(track->GetTofHitIndex());
	if(NULL == tofHit) continue;

	// Get pointer to the TOF point
	refIndex = tofHit->GetRefId();
	if(refIndex < 0) continue;
	tofPoint = (CbmTofPoint*) fArrayTofPoint->At(refIndex);
	if(NULL == tofPoint) continue;

	// Check if ghost
	if(track->GetTrdTrackIndex() > -1) {
	    if(stsTrackM->GetMCTrackId() > -1 &&
	       stsTrackM->GetMCTrackId() == trdTrackM->GetMCTrackId() &&
	       stsTrackM->GetMCTrackId() == tofPoint->GetTrackID()) {
		ghost = kFALSE;
		mcTrack = (CbmMCTrack*) fArrayMCTrack->At(stsTrackM->GetMCTrackId());
		if(NULL == mcTrack) continue;
		pdg = mcTrack->GetPdgCode();
	    } else {
		ghost = kTRUE;
		pdg = 0;
	    }
	} else {
	    if(stsTrackM->GetMCTrackId() > -1 &&
	       stsTrackM->GetMCTrackId() == tofPoint->GetTrackID()) {
		ghost = kFALSE;
		mcTrack = (CbmMCTrack*) fArrayMCTrack->At(stsTrackM->GetMCTrackId());
		if(NULL == mcTrack) continue;
		pdg = mcTrack->GetPdgCode();
	    } else {
		ghost = kTRUE;
		pdg = 0;
	    }
	}

	// Check Tof Double Hit
	if(1 == tofHit->GetFlag()) {
            tdh = kFALSE;
	} else {
            tdh = kTRUE;
	}

	// Extrapolate to the primary vertex
        fTrackFitter->Extrapolate(stsTrack, fPrimVertex->GetZ(), &paramExtr);
	// Get momentum and mass
	qp = paramExtr.GetQp();
	if(qp > 0) {
            charge = 1;
	} else {
            charge = -1;
	}
	if(TMath::Abs(qp) < 1e-8) continue;
	p = 1./TMath::Abs(qp);
	pz = TMath::Sqrt( p*p /
			 ( TMath::Power(paramExtr.GetTx(), 2) +
			  TMath::Power(paramExtr.GetTy(), 2) + 1 ) );
	pt = TMath::Sqrt(TMath::Power(paramExtr.GetTx()*pz, 2) +
			 TMath::Power(paramExtr.GetTy()*pz, 2));
        b = fTrackFitter->GetChiToVertex(stsTrack, fPrimVertex);
	tof = tofHit->GetTime();
	l = track->GetLength();
	Double_t val = (tof*29.9792458)/l;
	mass2 = p*p*(val*val-1.);

	fh_pdl->Fill(p, l - tofPoint->GetLength());

	// Create hadron
	new ((*fArrayHadron)[fHadrons]) CbmHadron(charge, p, pt, pz,
                                                  nMvdHits, nStsHits, nTrdHits,
						  b, l, mass2,
						  pdg, ghost, tdh);
	fHadrons += 1;
    }


    // Control output
    if(0 == (fEvents%1)) {
	cout << "-I- CbmProduceDst::Exec : "
	    << "event " << fEvents << " is processed." << endl;
    }
    fEvents += 1;
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
void CbmProduceDst::Finish()
{
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
void CbmProduceDst::WriteHisto()
{
  fh_pdl->Write();
}
// ------------------------------------------------------------------


ClassImp(CbmProduceDst);

