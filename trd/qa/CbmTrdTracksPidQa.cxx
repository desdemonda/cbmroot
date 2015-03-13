// -----------------------------------------------------------------------
// -----                     CbmTrdTracksPidQa                        -----
// -----               Created 24.02.07  by F. Uhlig                 -----
// -----------------------------------------------------------------------

#include "CbmTrdTracksPidQa.h"

#include "CbmTrdHit.h"
#include "CbmTrdPoint.h"
#include "CbmTrdTrack.h"

#include "FairRootManager.h"
#include "CbmMCTrack.h"

#include "TClonesArray.h"
#include "TVector3.h"
#include "TMath.h"
#include "TH1F.h"
#include "TH2F.h"

#include <iostream>
using std::cout;
using std::endl;

// ---- Default constructor -------------------------------------------------

CbmTrdTracksPidQa::CbmTrdTracksPidQa()
  : FairTask(),
    fTrdTrackCollection(NULL),
    fTrdHitCollection(NULL),
    fMCPointArray(NULL),
    fMCTrackArray(NULL),
    PartID(NULL),
    NrTRDHits(NULL),
    ELossPI(NULL),
    ELossEL(NULL),
    ELossALL(NULL),
    MomPI(NULL),
    MomEL(NULL),
    MomALL(NULL),
    MOMvsELossEL(NULL),
    MOMvsELossPI(NULL),
    MOMvsELossALL(NULL),
    WknPI(NULL),
    WknEL(NULL),
    WknALL(NULL),
    WknLowPI(NULL),
    WknLowEL(NULL),
    WknLowALL(NULL),
    WknHighPI(NULL),
    WknHighEL(NULL),
    WknHighALL(NULL),
    AnnPI(NULL),
    AnnEL(NULL),
    AnnALL(NULL),
    AnnLowPI(NULL),
    AnnLowEL(NULL),
    AnnLowALL(NULL),
    AnnHighPI(NULL),
    AnnHighEL(NULL),
    AnnHighALL(NULL),
    LikePI(NULL),
    LikeEL(NULL),
    LikeALL(NULL),
    LikeHighPI(NULL),
    LikeHighEL(NULL),
    LikeHighALL(NULL),
    LikeLowPI(NULL),
    LikeLowEL(NULL),
    LikeLowALL(NULL)
{
}
// --------------------------------------------------------------------------


// ---- Standard constructor ------------------------------------------------
CbmTrdTracksPidQa::CbmTrdTracksPidQa(const char* name,
					 const char* title)
  : FairTask(name),
    fTrdTrackCollection(NULL),
    fTrdHitCollection(NULL),
    fMCPointArray(NULL),
    fMCTrackArray(NULL),
    PartID(NULL),
    NrTRDHits(NULL),
    ELossPI(NULL),
    ELossEL(NULL),
    ELossALL(NULL),
    MomPI(NULL),
    MomEL(NULL),
    MomALL(NULL),
    MOMvsELossEL(NULL),
    MOMvsELossPI(NULL),
    MOMvsELossALL(NULL),
    WknPI(NULL),
    WknEL(NULL),
    WknALL(NULL),
    WknLowPI(NULL),
    WknLowEL(NULL),
    WknLowALL(NULL),
    WknHighPI(NULL),
    WknHighEL(NULL),
    WknHighALL(NULL),
    AnnPI(NULL),
    AnnEL(NULL),
    AnnALL(NULL),
    AnnLowPI(NULL),
    AnnLowEL(NULL),
    AnnLowALL(NULL),
    AnnHighPI(NULL),
    AnnHighEL(NULL),
    AnnHighALL(NULL),
    LikePI(NULL),
    LikeEL(NULL),
    LikeALL(NULL),
    LikeHighPI(NULL),
    LikeHighEL(NULL),
    LikeHighALL(NULL),
    LikeLowPI(NULL),
    LikeLowEL(NULL),
    LikeLowALL(NULL)
{
}
// --------------------------------------------------------------------------


// ---- Destructor ----------------------------------------------------------
CbmTrdTracksPidQa::~CbmTrdTracksPidQa()
{
}
// --------------------------------------------------------------------------


// ---- Initialisation ------------------------------------------------------
InitStatus CbmTrdTracksPidQa::Init()
{
    // Get pointer to the ROOT I/O manager
    FairRootManager* rootMgr = FairRootManager::Instance();
    if(NULL == rootMgr) {
	cout << "-E- CbmTrdTrackPidQa::Init : "
	    << "ROOT manager is not instantiated !" << endl;
	return kFATAL;
    }


    // Get pointer to TRD point array
    fMCPointArray = (TClonesArray*) rootMgr->GetObject("TrdPoint");
    if(NULL == fMCPointArray) {
	cout << "-W- CbmTrdTracksPidQa::Init : "
	    << "no MC point array !" << endl;
	return kERROR;
    }

    // Get pointer to TRD hit array
    fTrdHitCollection = (TClonesArray*) rootMgr->GetObject("TrdHit");
    if(NULL == fTrdHitCollection) {
	cout << "-W- CbmTrdTracksPidQa::Init : "
	    << "no TRD hit array !" << endl;
	return kERROR;
    }

    // Get pointer to TRD track array
    fTrdTrackCollection = (TClonesArray*) rootMgr->GetObject("TrdTrack");
    if(NULL == fTrdTrackCollection) {
	cout << "-W- CbmTrdTracksPidQa::Init : "
	    << "no TRD hit array !" << endl;
	return kERROR;
    }

    // Get MCTrack array
    fMCTrackArray  = (TClonesArray*) rootMgr->GetObject("MCTrack");
    if ( ! fMCTrackArray) {
      cout << "-E- CbmTrdTracksPidQa::Init : No MCTrack array!"
         << endl;
      return kFATAL;
    }


    PrepareHistograms();


    return kSUCCESS;

}

// --------------------------------------------------------------------------


// ---- Task execution ------------------------------------------------------
void CbmTrdTracksPidQa::Exec(Option_t* option)
{
    // Declare variables
    CbmTrdTrack* trdTrack = NULL;
    CbmTrdPoint* mcPoint = NULL;
    CbmTrdHit* trdHit = NULL;

    Int_t partID;
    Int_t nrtrdpoints;
    TVector3 mom;
    Double_t momentum;

    // Event counters
    Int_t nTrdTrack = fTrdTrackCollection->GetEntriesFast();

    // Loop over TRD tracks
    for(Int_t iTrack = 0; iTrack < nTrdTrack; iTrack++){
	trdTrack = (CbmTrdTrack*) fTrdTrackCollection->At(iTrack);
	if(NULL == trdTrack) continue;

        nrtrdpoints = trdTrack->GetNofHits();

        trdHit = (CbmTrdHit*) fTrdHitCollection->At(trdTrack->GetHitIndex(1));
	if(NULL == trdHit) continue;

        mcPoint = (CbmTrdPoint*) fMCPointArray->At(trdHit->GetRefId());
	if(NULL == mcPoint) continue;


        NrTRDHits->Fill(nrtrdpoints);

        if (nrtrdpoints == 12) {

          partID = (((CbmMCTrack*)fMCTrackArray->At(mcPoint->GetTrackID()))->GetPdgCode());
          (((CbmMCTrack*)fMCTrackArray->At(mcPoint->GetTrackID()))->GetMomentum(mom));
          momentum=mom.Mag();

          PartID->Fill(partID);
          if (TMath::Abs(partID) == 11){
            WknEL->Fill(trdTrack->GetPidWkn());
            AnnEL->Fill(trdTrack->GetPidANN());
            LikeEL->Fill(trdTrack->GetPidLikeEL());

            ELossEL->Fill(trdTrack->GetELoss()*1000000.);
            MomEL->Fill(momentum);
            MOMvsELossEL->Fill(trdTrack->GetELoss()*1000000.,momentum);

            if (momentum < 2.) {
              LikeLowEL->Fill(trdTrack->GetPidLikeEL());
              WknLowEL->Fill(trdTrack->GetPidWkn());
              AnnLowEL->Fill(trdTrack->GetPidANN());
            } else {
              LikeHighEL->Fill(trdTrack->GetPidLikeEL());
              WknHighEL->Fill(trdTrack->GetPidWkn());
              AnnHighEL->Fill(trdTrack->GetPidANN());
	    }
	  }
          if (TMath::Abs(partID) == 211){
            WknPI->Fill(trdTrack->GetPidWkn());
            AnnPI->Fill(trdTrack->GetPidANN());
            LikePI->Fill(trdTrack->GetPidLikeEL());

            ELossPI->Fill(trdTrack->GetELoss()*1000000.);
            MomPI->Fill(momentum);
            MOMvsELossPI->Fill(trdTrack->GetELoss()*1000000.,momentum);
            if (momentum < 2.) {
              LikeLowPI->Fill(trdTrack->GetPidLikeEL());
              WknLowPI->Fill(trdTrack->GetPidWkn());
              AnnLowPI->Fill(trdTrack->GetPidANN());
            } else {
              LikeHighPI->Fill(trdTrack->GetPidLikeEL());
              WknHighPI->Fill(trdTrack->GetPidWkn());
              AnnHighPI->Fill(trdTrack->GetPidANN());
	    }
          }
          if (!(TMath::Abs(partID) == 11)){
            WknALL->Fill(trdTrack->GetPidWkn());
            AnnALL->Fill(trdTrack->GetPidANN());
            LikeALL->Fill(trdTrack->GetPidLikeEL());

            ELossALL->Fill(trdTrack->GetELoss()*1000000.);
            MomALL->Fill(momentum);
            MOMvsELossALL->Fill(trdTrack->GetELoss()*1000000.,momentum);

            if (momentum < 2.) {
              LikeLowALL->Fill(trdTrack->GetPidLikeEL());
              WknLowALL->Fill(trdTrack->GetPidWkn());
              AnnLowALL->Fill(trdTrack->GetPidANN());
            } else {
              LikeHighALL->Fill(trdTrack->GetPidLikeEL());
              WknHighALL->Fill(trdTrack->GetPidWkn());
              AnnHighALL->Fill(trdTrack->GetPidANN());
	    }
	  }
	}
    }
}

// --------------------------------------------------------------------------


// ---- Finish --------------------------------------------------------------
void CbmTrdTracksPidQa::Finish()
{
  WriteHistograms();
}
// --------------------------------------------------------------------------


// ---- Prepare test histograms ---------------------------------------------

void CbmTrdTracksPidQa::PrepareHistograms()
{

    WknPI     = new TH1F("WknPI","Wkn for pions ",10000,0.,2000.);
    WknEL      = new TH1F("WknEL","Wkn for electrons ",10000,0.,2000.);
    WknALL    = new TH1F("WknALL","Wkn for all particles except electrons ",10000,0.,2000.);
    WknLowPI     = new TH1F("WknLowPI","Wkn for pions ",10000,0.,2000.);
    WknLowEL      = new TH1F("WknLowEL","Wkn for electrons ",10000,0.,2000.);
    WknLowALL    = new TH1F("WknLowALL","Wkn for all particles except electrons ",10000,0.,2000.);
    WknHighPI     = new TH1F("WknHighPI","Wkn for pions ",10000,0.,2000.);
    WknHighEL      = new TH1F("WknHighEL","Wkn for electrons ",10000,0.,2000.);
    WknHighALL    = new TH1F("WknHighALL","Wkn for all particles except electrons ",10000,0.,2000.);


    AnnPI    = new TH1F("AnnPI","Ann for pions ",400,-2.,2.);
    AnnEL    = new TH1F("AnnEL","Ann for electrons ",400,-2.,2.);
    AnnALL   = new TH1F("AnnALL","Ann for all except electrons ",400,-2.,2.);
    AnnHighPI    = new TH1F("AnnHighPI","Ann for pions ",400,-2.,2.);
    AnnHighEL    = new TH1F("AnnHighEL","Ann for electrons ",400,-2.,2.);
    AnnHighALL   = new TH1F("AnnHighALL","Ann for all except electrons ",400,-2.,2.);
    AnnLowPI    = new TH1F("AnnLowPI","Ann for pions ",400,-2.,2.);
    AnnLowEL    = new TH1F("AnnLowEL","Ann for electrons ",400,-2.,2.);
    AnnLowALL   = new TH1F("AnnLowALL","Ann for all except electrons ",400,-2.,2.);


    LikePI    = new TH1F("LikePI","Likelihood for pions ",400,-2.,2.);
    LikeEL    = new TH1F("LikeEL","Likelihood for electrons ",400,-2.,2.);
    LikeALL   = new TH1F("LikeALL","Likelihood for all except electrons ",400,-2.,2.);
    LikeHighPI    = new TH1F("LikeHighPI","Likelihood for pions ",400,-2.,2.);
    LikeHighEL    = new TH1F("LikeHighEL","Likelihood for electrons ",400,-2.,2.);
    LikeHighALL   = new TH1F("LikeHighALL","Likelihood for all except electrons ",400,-2.,2.);
    LikeLowPI    = new TH1F("LikeLowPI","Likelihood for pions ",400,-2.,2.);
    LikeLowEL    = new TH1F("LikeLowEL","Likelihood for electrons ",400,-2.,2.);
    LikeLowALL   = new TH1F("LikeLowALL","Likelihood for all except electrons ",400,-2.,2.);

    PartID    = new TH1F("PartID","Particle ID of the Track",4600,-2300.,2300.);
    NrTRDHits = new TH1F("NrTRDHits","Nr. of TRD Hits for track",20,0.,20.);

    ELossPI   = new TH1F("ELossPI","summed ELoss for pions",2000, 0.,200.);
    ELossEL   = new TH1F("ELossEL","summed ELoss for electrons",2000, 0.,200.);
    ELossALL  = new TH1F("ELossALL","summed ELoss for all except electrons",2000, 0.,200.);

    MomPI   = new TH1F("MomPI","summed Momentum for pions",110, 0.,11.);
    MomEL   = new TH1F("MomEL","summed Momentum for electrons",110, 0.,11.);
    MomALL  = new TH1F("MomALL","summed Momentum for all except electrons",110, 0.,11.);

    MOMvsELossPI   = new TH2F("MomvsElossPI","Momentum vs Eloss EL",2000, 0.,200.,110, 0.,11.);
    MOMvsELossEL   = new TH2F("MomvsElossEL","Momentum vs Eloss PI",2000, 0.,200.,110, 0.,11.);
    MOMvsELossALL  = new TH2F("MomvsElossALL","Momentum vs Eloss EL",2000, 0.,200.,110, 0.,11.);

}
// --------------------------------------------------------------------------


// ---- Write test histograms ------------------------------------------------

void CbmTrdTracksPidQa::WriteHistograms()
{

  // Write histos to output
  gDirectory->mkdir("TrdTracksPidQA");
  gDirectory->cd("TrdTracksPidQA");

  if(WknPI)         WknPI->Write();
  if(WknEL)         WknEL->Write();
  if(WknALL)        WknALL->Write();
  if(WknLowPI)      WknLowPI->Write();
  if(WknLowEL)      WknLowEL->Write();
  if(WknLowALL)     WknLowALL->Write();
  if(WknHighPI)     WknHighPI->Write();
  if(WknHighEL)     WknHighEL->Write();
  if(WknHighALL)    WknHighALL->Write();

  if(AnnPI)         AnnPI->Write();
  if(AnnEL)         AnnEL->Write();
  if(AnnALL)        AnnALL->Write();
  if(AnnLowPI)      AnnLowPI->Write();
  if(AnnLowEL)      AnnLowEL->Write();
  if(AnnLowALL)     AnnLowALL->Write();
  if(AnnHighPI)     AnnHighPI->Write();
  if(AnnHighEL)     AnnHighEL->Write();
  if(AnnHighALL)    AnnHighALL->Write();

  if(LikePI)        LikePI->Write();
  if(LikeEL)        LikeEL->Write();
  if(LikeALL)       LikeALL->Write();
  if(LikeHighPI)    LikeHighPI->Write();
  if(LikeHighEL)    LikeHighEL->Write();
  if(LikeHighALL)   LikeHighALL->Write();
  if(LikeLowPI)     LikeLowPI->Write();
  if(LikeLowEL)     LikeLowEL->Write();
  if(LikeLowALL)    LikeLowALL->Write();

  if(PartID)        PartID->Write();
  if(NrTRDHits)     NrTRDHits->Write();
  if(ELossPI)       ELossPI->Write();
  if(ELossEL)       ELossEL->Write();
  if(ELossALL)      ELossALL->Write();
  if(MomPI)         MomPI->Write();
  if(MomEL)         MomEL->Write();
  if(MomALL)        MomALL->Write();
  if(MOMvsELossPI)  MOMvsELossPI->Write();
  if(MOMvsELossEL)  MOMvsELossEL->Write();
  if(MOMvsELossALL) MOMvsELossALL->Write();

  gDirectory->cd("..");
}

// --------------------------------------------------------------------------

ClassImp(CbmTrdTracksPidQa);

