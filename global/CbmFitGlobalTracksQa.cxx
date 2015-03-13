//-------------------------------------------------------------------------
//-----                     CbmFitGlobalTracksQa                      -----
//-----                 Created 07/03/06 by D. Kresan                 -----
//-------------------------------------------------------------------------
#include "CbmFitGlobalTracksQa.h"

#include "CbmGlobalTrack.h"

#include "FairRootManager.h"
#include "CbmStsPoint.h"
#include "CbmTrdPoint.h"
#include "CbmStsHit.h"
#include "CbmTrdHit.h"
#include "CbmStsTrack.h"
#include "CbmTrdTrack.h"

#include "TClonesArray.h"
#include "TH1.h"
#include "TVector3.h"
#include "TMath.h"

#include <iostream>

using std::cout;
using std::endl;

//________________________________________________________________________
//
// CbmFitGlobalTracksQa
//
// Task for performance analysis of the global track fit
//


// -----------------------------------------------------------------------
CbmFitGlobalTracksQa::CbmFitGlobalTracksQa()
  : FairTask(),
    fArrayStsPoint(NULL),
    fArrayTrdPoint(NULL),
    fArrayStsHit(NULL),
    fArrayTrdHit(NULL),
    fArrayStsTrack(NULL),
    fArrayTrdTrack(NULL),
    fArrayGlbTrack(NULL),
    fEvents(0),
    fh_first_resx(NULL),
    fh_first_resy(NULL),
    fh_first_restx(NULL),
    fh_first_resty(NULL),
    fh_first_resz(NULL),
    fh_last_resx(NULL),
    fh_last_resy(NULL),
    fh_last_restx(NULL),
    fh_last_resty(NULL),
    fh_last_resz(NULL),
    fh_first_pullx(NULL),
    fh_first_pully(NULL),
    fh_first_pulltx(NULL),
    fh_first_pullty(NULL),
    fh_last_pullx(NULL),
    fh_last_pully(NULL),
    fh_last_pulltx(NULL),
    fh_last_pullty(NULL),
    fh_chi2ndf(NULL)
{
  CreateHistogramms();
}
// -----------------------------------------------------------------------


// -----------------------------------------------------------------------
CbmFitGlobalTracksQa::CbmFitGlobalTracksQa(const char* name, Int_t verbose)
  : FairTask(name, verbose),
    fArrayStsPoint(NULL),
    fArrayTrdPoint(NULL),
    fArrayStsHit(NULL),
    fArrayTrdHit(NULL),
    fArrayStsTrack(NULL),
    fArrayTrdTrack(NULL),
    fArrayGlbTrack(NULL),
    fEvents(0),
    fh_first_resx(NULL),
    fh_first_resy(NULL),
    fh_first_restx(NULL),
    fh_first_resty(NULL),
    fh_first_resz(NULL),
    fh_last_resx(NULL),
    fh_last_resy(NULL),
    fh_last_restx(NULL),
    fh_last_resty(NULL),
    fh_last_resz(NULL),
    fh_first_pullx(NULL),
    fh_first_pully(NULL),
    fh_first_pulltx(NULL),
    fh_first_pullty(NULL),
    fh_last_pullx(NULL),
    fh_last_pully(NULL),
    fh_last_pulltx(NULL),
    fh_last_pullty(NULL),
    fh_chi2ndf(NULL)
{
    CreateHistogramms();
}
// -----------------------------------------------------------------------


// -----------------------------------------------------------------------
CbmFitGlobalTracksQa::~CbmFitGlobalTracksQa()
{
    // Destructor
}
// -----------------------------------------------------------------------


// -----------------------------------------------------------------------
InitStatus CbmFitGlobalTracksQa::Init()
{
    // Initialisation of the task

    // Get pointer to the ROOT I/O manager
    FairRootManager* rootMgr = FairRootManager::Instance();
    if(NULL == rootMgr) {
	cout << "-E- CbmFitGlobalTracksQa::Init :"
	    << " ROOT manager is not instantiated" << endl;
        return kFATAL;
    }
    // Get MC points
    fArrayStsPoint = (TClonesArray*) rootMgr->GetObject("StsPoint");
    if(NULL == fArrayStsPoint) {
	cout << "-E- CbmFitGlobalTracksQa::Init : "
            << "no STS point array!" << endl;
    }
    fArrayTrdPoint = (TClonesArray*) rootMgr->GetObject("TrdPoint");
    if(NULL == fArrayTrdPoint) {
	cout << "-E- CbmFitGlobalTracksQa::Init : "
            << "no TRD point array!" << endl;
    }
    // Get hit arrays
    fArrayStsHit = (TClonesArray*) rootMgr->GetObject("StsHit");
    if(NULL == fArrayStsHit) {
	cout << "-W- CbmFitGlobalTracksQa::Init :"
            << " no STS hit array" << endl;
    }
    fArrayTrdHit = (TClonesArray*) rootMgr->GetObject("TrdHit");
    if(NULL == fArrayTrdHit) {
	cout << "-W- CbmFitGlobalTracksQa::Init :"
            << " no TRD hit array" << endl;
    }
    // Get track arrays
    fArrayStsTrack = (TClonesArray*) rootMgr->GetObject("StsTrack");
    if(NULL == fArrayStsTrack) {
	cout << "-W- CbmFitGlobalTracksQa::Init : "
            << "no STS track array!" << endl;
    }
    fArrayTrdTrack = (TClonesArray*) rootMgr->GetObject("TrdTrack");
    if(NULL == fArrayTrdTrack) {
	cout << "-W- CbmFitGlobalTracksQa::Init : "
            << "no TRD track array!" << endl;
    }
    fArrayGlbTrack = (TClonesArray*) rootMgr->GetObject("GlobalTrack");
    if(NULL == fArrayGlbTrack) {
	cout << "-W- CbmFitGlobalTracksQa::Init : "
            << "no global track array!" << endl;
    }
    return kSUCCESS;
}
// -----------------------------------------------------------------------


// -----------------------------------------------------------------------
void CbmFitGlobalTracksQa::Exec(Option_t* option)
{
    // Task execution
    if(NULL==fArrayStsPoint || NULL==fArrayTrdPoint || NULL==fArrayStsHit ||
       NULL==fArrayTrdHit ||
       NULL==fArrayStsTrack || NULL==fArrayTrdTrack || NULL==fArrayGlbTrack) return;

    // Loop over the global tracks
    CbmStsPoint* stsPoint;
    CbmTrdPoint* trdPoint;
    CbmStsHit* stsHit;
    CbmTrdHit* trdHit;
    CbmStsTrack* stsTrack;
    CbmTrdTrack* trdTrack;
    CbmGlobalTrack* glbTrack;
    Int_t stsTrackIndex;
    Int_t trdTrackIndex;
    Int_t stsHitIndex;
    Int_t trdHitIndex;
    Int_t stsPointIndex;
    Int_t trdPointIndex;
    TVector3 pos;
    TVector3 mom;

    for(Int_t iGlbTrack = 0;
	iGlbTrack < fArrayGlbTrack->GetEntriesFast();
	iGlbTrack++) {

	// Get pointer to the global track
	glbTrack = (CbmGlobalTrack*) fArrayGlbTrack->
	    At(iGlbTrack);
	if(NULL == glbTrack) continue;


	// Chi2/NDF
	fh_chi2ndf->Fill(glbTrack->GetChi2()/(Double_t)glbTrack->GetNDF());


	// Performance at the first plane
	stsTrackIndex = glbTrack->GetStsTrackIndex();
	if(stsTrackIndex >= 0) {
	    stsTrack = (CbmStsTrack*) fArrayStsTrack->
		At(stsTrackIndex);
	    if(NULL != stsTrack) {
		stsPointIndex = -1;
		if(stsTrack->GetNofHits()) {
		    stsHitIndex = stsTrack->GetHitIndex(0);
		    stsHit = (CbmStsHit*) fArrayStsHit->At(stsHitIndex);
		    if(NULL != stsHit) {
			stsPointIndex = stsHit->GetRefId();
		    }
		} else {
		    cout << "-W- CbmFitGlobalTracksQa::Exec : "
			<< "STS track " << stsTrackIndex << " is empty!" << endl;
		}

		if(stsPointIndex >= 0) {
		    stsPoint = (CbmStsPoint*) fArrayStsPoint->At(stsPointIndex);
		    if(NULL != stsPoint) {
			stsPoint->Position(pos);
			stsPoint->Momentum(mom);
			fh_first_resx->Fill(glbTrack->GetParamFirst()->GetX() - pos.X());
			fh_first_resy->Fill(glbTrack->GetParamFirst()->GetY() - pos.Y());
			fh_first_resz->Fill(glbTrack->GetParamFirst()->GetZ() - pos.Z());
			fh_first_restx->Fill(glbTrack->GetParamFirst()->GetTx() - mom.X()/mom.Z());
			fh_first_resty->Fill(glbTrack->GetParamFirst()->GetTy() - mom.Y()/mom.Z());
			fh_first_pullx->Fill((glbTrack->GetParamFirst()->GetX() - pos.X()) /
					     TMath::Sqrt(glbTrack->GetParamFirst()->GetCovariance(0,0)));
			fh_first_pully->Fill((glbTrack->GetParamFirst()->GetY() - pos.Y()) /
					     TMath::Sqrt(glbTrack->GetParamFirst()->GetCovariance(1,1)));
			fh_first_pulltx->Fill((glbTrack->GetParamFirst()->GetTx() - mom.X()/mom.Z()) /
					      TMath::Sqrt(glbTrack->GetParamFirst()->GetCovariance(2,2)));
			fh_first_pullty->Fill((glbTrack->GetParamFirst()->GetTy() - mom.Y()/mom.Z()) /
					      TMath::Sqrt(glbTrack->GetParamFirst()->GetCovariance(3,3)));
		    }
		}
	    }
	} else {
	    trdTrackIndex = glbTrack->GetTrdTrackIndex();
	    trdTrack = (CbmTrdTrack*) fArrayTrdTrack->At(trdTrackIndex);
	    if(NULL != trdTrack) {
		trdPointIndex = -1;
		if(trdTrack->GetNofHits()) {
		    trdHitIndex = trdTrack->GetHitIndex(0);
		    trdHit = (CbmTrdHit*) fArrayTrdHit->At(trdHitIndex);
		    if(NULL != trdHit) {
			trdPointIndex = trdHit->GetRefId();
		    }
		} else {
		    cout << "-W- CbmFitGlobalTracksQa::Exec : "
			<< "TRD track " << trdTrackIndex << " is empty!" << endl;
		}

		if(trdPointIndex >= 0) {
		    trdPoint = (CbmTrdPoint*) fArrayTrdPoint->At(trdPointIndex);
		    if(NULL != trdPoint) {
			trdPoint->Position(pos);
			trdPoint->Momentum(mom);
			fh_first_resx->Fill(glbTrack->GetParamFirst()->GetX() - pos.X());
			fh_first_resy->Fill(glbTrack->GetParamFirst()->GetY() - pos.Y());
			fh_first_resz->Fill(glbTrack->GetParamFirst()->GetZ() - pos.Z());
			fh_first_restx->Fill(glbTrack->GetParamFirst()->GetTx() - mom.X()/mom.Z());
			fh_first_resty->Fill(glbTrack->GetParamFirst()->GetTy() - mom.Y()/mom.Z());
			fh_first_pullx->Fill((glbTrack->GetParamFirst()->GetX() - pos.X()) /
					     TMath::Sqrt(glbTrack->GetParamFirst()->GetCovariance(0,0)));
			fh_first_pully->Fill((glbTrack->GetParamFirst()->GetY() - pos.Y()) /
					     TMath::Sqrt(glbTrack->GetParamFirst()->GetCovariance(1,1)));
			fh_first_pulltx->Fill((glbTrack->GetParamFirst()->GetTx() - mom.X()/mom.Z()) /
					      TMath::Sqrt(glbTrack->GetParamFirst()->GetCovariance(2,2)));
			fh_first_pullty->Fill((glbTrack->GetParamFirst()->GetTy() - mom.Y()/mom.Z()) /
					      TMath::Sqrt(glbTrack->GetParamFirst()->GetCovariance(3,3)));
		    }
		}
	    }
	}


	// Performance at the last plane
	trdTrackIndex = glbTrack->GetTrdTrackIndex();
	if(trdTrackIndex >= 0) {
	    trdTrack = (CbmTrdTrack*) fArrayTrdTrack->At(trdTrackIndex);
	    if(NULL != trdTrack) {
		trdPointIndex = -1;
		if(trdTrack->GetNofHits()) {
		    trdHitIndex = trdTrack->GetHitIndex(trdTrack->GetNofHits()-1);
		    trdHit = (CbmTrdHit*) fArrayTrdHit->At(trdHitIndex);
		    if(NULL != trdHit) {
			trdPointIndex = trdHit->GetRefId();
		    }
		} else {
		    cout << "-W- CbmFitGlobalTracksQa::Exec : "
			<< "TRD track " << trdTrackIndex << " is empty!" << endl;
		}

		if(trdPointIndex >= 0) {
		    trdPoint = (CbmTrdPoint*) fArrayTrdPoint->At(trdPointIndex);
		    if(NULL != trdPoint) {
			trdPoint->Position(pos);
			trdPoint->Momentum(mom);
			fh_last_resx->Fill(glbTrack->GetParamLast()->GetX() - pos.X());
			fh_last_resy->Fill(glbTrack->GetParamLast()->GetY() - pos.Y());
			fh_last_resz->Fill(glbTrack->GetParamLast()->GetZ() - pos.Z());
			fh_last_restx->Fill(glbTrack->GetParamLast()->GetTx() - mom.X()/mom.Z());
			fh_last_resty->Fill(glbTrack->GetParamLast()->GetTy() - mom.Y()/mom.Z());
			fh_last_pullx->Fill((glbTrack->GetParamLast()->GetX() - pos.X()) /
					    TMath::Sqrt(glbTrack->GetParamLast()->GetCovariance(0,0)));
			fh_last_pully->Fill((glbTrack->GetParamLast()->GetY() - pos.Y()) /
					    TMath::Sqrt(glbTrack->GetParamLast()->GetCovariance(1,1)));
			fh_last_pulltx->Fill((glbTrack->GetParamLast()->GetTx() - mom.X()/mom.Z()) /
					     TMath::Sqrt(glbTrack->GetParamLast()->GetCovariance(2,2)));
			fh_last_pullty->Fill((glbTrack->GetParamLast()->GetTy() - mom.Y()/mom.Z()) /
					     TMath::Sqrt(glbTrack->GetParamLast()->GetCovariance(3,3)));
		    }
		}
	    }
	} else {
	    stsTrackIndex = glbTrack->GetStsTrackIndex();
	    stsTrack = (CbmStsTrack*) fArrayStsTrack->
		At(stsTrackIndex);
	    if(NULL != stsTrack) {
		stsPointIndex = -1;
		if(stsTrack->GetNofHits()) {
		    stsHitIndex = stsTrack->GetHitIndex(stsTrack->GetNofHits()-1);
		    stsHit = (CbmStsHit*) fArrayStsHit->At(stsHitIndex);
		    if(NULL != stsHit) {
			stsPointIndex = stsHit->GetRefId();
		    }
		} else {
		    cout << "-W- CbmFitGlobalTracksQa::Exec : "
			<< "STS track " << stsTrackIndex << " is empty!" << endl;
		}

		if(stsPointIndex >= 0) {
		    stsPoint = (CbmStsPoint*) fArrayStsPoint->At(stsPointIndex);
		    if(NULL != stsPoint) {
			stsPoint->Position(pos);
			stsPoint->Momentum(mom);
			fh_last_resx->Fill(glbTrack->GetParamLast()->GetX() - pos.X());
			fh_last_resy->Fill(glbTrack->GetParamLast()->GetY() - pos.Y());
			fh_last_resz->Fill(glbTrack->GetParamLast()->GetZ() - pos.Z());
			fh_last_restx->Fill(glbTrack->GetParamLast()->GetTx() - mom.X()/mom.Z());
			fh_last_resty->Fill(glbTrack->GetParamLast()->GetTy() - mom.Y()/mom.Z());
			fh_last_pullx->Fill((glbTrack->GetParamLast()->GetX() - pos.X()) /
					    TMath::Sqrt(glbTrack->GetParamLast()->GetCovariance(0,0)));
			fh_last_pully->Fill((glbTrack->GetParamLast()->GetY() - pos.Y()) /
					    TMath::Sqrt(glbTrack->GetParamLast()->GetCovariance(1,1)));
			fh_last_pulltx->Fill((glbTrack->GetParamLast()->GetTx() - mom.X()/mom.Z()) /
					     TMath::Sqrt(glbTrack->GetParamLast()->GetCovariance(2,2)));
			fh_last_pullty->Fill((glbTrack->GetParamLast()->GetTy() - mom.Y()/mom.Z()) /
					     TMath::Sqrt(glbTrack->GetParamLast()->GetCovariance(3,3)));
		    }
		}
	    }
	}
    }


    fEvents += 1;
}


// -----------------------------------------------------------------------
void CbmFitGlobalTracksQa::Finish()
{
    // Finish of the task execution
    WriteHistogramms();
}
// -----------------------------------------------------------------------


// -----------------------------------------------------------------------
void CbmFitGlobalTracksQa::CreateHistogramms()
{
    // Create control histogramms

    // Residuals at first TRD layer
    fh_first_resx = new TH1F("h_glb_first_resx",
                             "x residual at first TRD layer",
                             200, -1., 1.);
    fh_first_resy = new TH1F("h_glb_first_resy",
                             "y residual at first TRD layer",
                             200, -1., 1.);
    fh_first_restx = new TH1F("h_glb_first_restx",
                              "t_{x} residual at first TRD layer",
                              200, -0.01, 0.01);
    fh_first_resty = new TH1F("h_glb_first_resty",
                              "t_{y} residual at first TRD layer",
                              200, -0.01, 0.01);
    fh_first_resz = new TH1F("h_glb_first_resz",
                             "z residual at first TRD layer",
                             100, -0.5, 0.5);

    // Residuals at last TRD layer
    fh_last_resx = new TH1F("h_glb_last_resx",
                            "x residual at last TRD layer",
                            200, -1., 1.);
    fh_last_resy = new TH1F("h_glb_last_resy",
                            "y residual at last TRD layer",
                            200, -1., 1.);
    fh_last_restx = new TH1F("h_glb_last_restx",
                             "t_{x} residual at last TRD layer",
                             200, -0.01, 0.01);
    fh_last_resty = new TH1F("h_glb_last_resty",
                             "t_{y} residual at last TRD layer",
                             200, -0.01, 0.01);
    fh_last_resz = new TH1F("h_glb_last_resz",
                            "z residual at last TRD layer",
                            100, -0.5, 0.5);

    // Pulls at first TRD layer
    fh_first_pullx = new TH1F("h_glb_first_pullx",
                              "x pull at first TRD layer",
                              200, -10., 10.);
    fh_first_pully = new TH1F("h_glb_first_pully",
                              "y pull at first TRD layer",
                              200, -10., 10.);
    fh_first_pulltx = new TH1F("h_glb_first_pulltx",
                               "t_{x} pull at first TRD layer",
                               200, -10., 10.);
    fh_first_pullty = new TH1F("h_glb_first_pullty",
                               "t_{y} pull at first TRD layer",
                               200, -10., 10.);

    // Pulls at last TRD layer
    fh_last_pullx = new TH1F("h_glb_last_pullx",
                             "x pull at last TRD layer",
                             200, -10., 10.);
    fh_last_pully = new TH1F("h_glb_last_pully",
                             "y pull at last TRD layer",
                             200, -10., 10.);
    fh_last_pulltx = new TH1F("h_glb_last_pulltx",
                              "t_{x} pull at last TRD layer",
                             200, -10., 10.);
    fh_last_pullty = new TH1F("h_glb_last_pullty",
                              "t_{y} pull at last TRD layer",
                              200, -10., 10.);

    // Chi2/NDF of track fit
    fh_chi2ndf = new TH1F("h_glb_chi2ndf",
                          "#chi^{2}/NDF of track fit",
                          500, 0., 50.);
}
// -----------------------------------------------------------------------


// -----------------------------------------------------------------------
void CbmFitGlobalTracksQa::WriteHistogramms()
{
    // Write control histogramms to file

    // Residuals at first and last TRD layer
    fh_first_resx->Scale(1./fEvents);
    fh_first_resy->Scale(1./fEvents);
    fh_first_restx->Scale(1./fEvents);
    fh_first_resty->Scale(1./fEvents);
    fh_last_resx->Scale(1./fEvents);
    fh_last_resy->Scale(1./fEvents);
    fh_last_restx->Scale(1./fEvents);
    fh_last_resty->Scale(1./fEvents);
    fh_first_resx->Write();
    fh_first_resy->Write();
    fh_first_restx->Write();
    fh_first_resty->Write();
    fh_first_resz->Write();
    fh_last_resx->Write();
    fh_last_resy->Write();
    fh_last_restx->Write();
    fh_last_resty->Write();
    fh_last_resz->Write();

    // Pulls at first and last TRD layer
    fh_first_pullx->Scale(1./fEvents);
    fh_first_pully->Scale(1./fEvents);
    fh_first_pulltx->Scale(1./fEvents);
    fh_first_pullty->Scale(1./fEvents);
    fh_last_pullx->Scale(1./fEvents);
    fh_last_pully->Scale(1./fEvents);
    fh_last_pulltx->Scale(1./fEvents);
    fh_last_pullty->Scale(1./fEvents);
    fh_first_pullx->Write();
    fh_first_pully->Write();
    fh_first_pulltx->Write();
    fh_first_pullty->Write();
    fh_last_pullx->Write();
    fh_last_pully->Write();
    fh_last_pulltx->Write();
    fh_last_pullty->Write();

    // Chi2/NDF of fit
    fh_chi2ndf->Scale(1./fEvents);
    fh_chi2ndf->Write();
}
// -----------------------------------------------------------------------


ClassImp(CbmFitGlobalTracksQa);

