// -----------------------------------------------------------------------
// -----                     CbmTrdCreatePidLike                     -----
// -----               Created 26/02/07  by F. Uhlig                 -----
// -----------------------------------------------------------------------

#include "CbmTrdCreatePidLike.h"

#include "CbmTrdHit.h"
#include "CbmTrdPoint.h"

#include "FairRootManager.h"
#include "CbmMCTrack.h"

#include "TClonesArray.h"
#include "TH1F.h"
#include "TVector3.h"

#include <iostream>
using std::cout;
using std::endl;

// ---- Default constructor -------------------------------------------------

CbmTrdCreatePidLike::CbmTrdCreatePidLike() 
  : FairTask(),
    fTrdHitCollection(NULL),
    fTrdPointCollection(NULL),
    fMCTrackArray(NULL),
    h1dEdxEL(),
    h1dEdxPI(),
    h1dEdxKA(),
    h1dEdxPR(),
    h1dEdxMU(),
    h1momentum(NULL)
{
    PrepareHistograms();
}
// --------------------------------------------------------------------------


// ---- Standard constructor ------------------------------------------------
CbmTrdCreatePidLike::CbmTrdCreatePidLike(const char* name,
        const char* title)
  : FairTask(name),
    fTrdHitCollection(NULL),
    fTrdPointCollection(NULL),
    fMCTrackArray(NULL),
    h1dEdxEL(),
    h1dEdxPI(),
    h1dEdxKA(),
    h1dEdxPR(),
    h1dEdxMU(),
    h1momentum(NULL)
{
    PrepareHistograms();
}
// --------------------------------------------------------------------------


// ---- Destructor ----------------------------------------------------------
CbmTrdCreatePidLike::~CbmTrdCreatePidLike() {}
// --------------------------------------------------------------------------


// ---- Initialisation ------------------------------------------------------
InitStatus CbmTrdCreatePidLike::Init() {
    // Get pointer to the ROOT I/O manager
    FairRootManager* rootMgr = FairRootManager::Instance();
    if(NULL == rootMgr) {
        cout << "-E- CbmTrdCreatePidLike::Init : "
        << "ROOT manager is not instantiated !" << endl;
        return kFATAL;
    }

    // Get pointer to TRD point array
    fTrdPointCollection = (TClonesArray*) rootMgr->GetObject("TrdPoint");
    if(NULL == fTrdPointCollection) {
        cout << "-W- CbmTrdCreatePidLike::Init : "
        << "no TRD point array !" << endl;
        return kERROR;
    }

    // Get pointer to TRD hit array
    fTrdHitCollection = (TClonesArray*) rootMgr->GetObject("TrdHit");
    if(NULL == fTrdHitCollection) {
        cout << "-W- CbmTrdCreatePidLike::Init : "
        << "no TRD hit array !" << endl;
        return kERROR;
    }

    // Get MCTrack array
    fMCTrackArray  = (TClonesArray*) rootMgr->GetObject("MCTrack");
    if ( ! fMCTrackArray) {
        cout << "-E- CbmTrdCreatePidLike::Init : No MCTrack array!"
        << endl;
        return kFATAL;
    }

    return kSUCCESS;

}

// --------------------------------------------------------------------------


// ---- Task execution ------------------------------------------------------
void CbmTrdCreatePidLike::Exec(Option_t* option) {
    // Declare variables
    CbmTrdHit* trdHit = NULL;
    CbmTrdPoint* trdPoint = NULL;

    Int_t       partID    = 0;
    Int_t       motherID  = 0;
    Int_t       momBin    = 0;
    TVector3  mom;

    Double_t     momentum;
    Double_t     momentum1;
    //   Int_t        momentum;

    // Event counters
    Int_t nTrdHit = fTrdHitCollection->GetEntriesFast();

    // Loop over TRD hits
    for(Int_t iHit = 0; iHit < nTrdHit; iHit++) {

        trdHit = (CbmTrdHit*) fTrdHitCollection->At(iHit);
        if(NULL == trdHit)
            continue;

        trdPoint = (CbmTrdPoint*) fTrdPointCollection->At(trdHit->GetRefId());
        if(NULL == trdPoint)
            continue;

        motherID = (((CbmMCTrack*)fMCTrackArray->At(trdPoint->GetTrackID()))->GetMotherId());

        if (motherID == -1) {


           TVector3 mom_det;
           trdPoint->Momentum(mom_det);
           momentum1=mom_det.Mag();

            (((CbmMCTrack*)fMCTrackArray->At(trdPoint->GetTrackID()))->GetMomentum(mom));
            momentum=mom.Mag();

            partID = (((CbmMCTrack*)fMCTrackArray->At(trdPoint->GetTrackID()))->GetPdgCode());


            if ((0.24 < momentum) && (momentum  < 0.26) )
                momBin=0;
            else if ((0.49 < momentum) && (momentum  < 0.51) )
                momBin=1;
            else if ((0.99 < momentum) && (momentum  < 1.01) )
                momBin=2;
            else if ((1.49 < momentum) && (momentum  < 1.51) )
                momBin=3;
            else if ((1.99 < momentum) && (momentum  < 2.01) )
                momBin=4;
            else if ((2.99 < momentum) && (momentum  < 3.01) )
                momBin=5;
            else if ((3.99 < momentum) && (momentum  < 4.01) )
                momBin=6;
            else if ((4.99 < momentum) && (momentum  < 5.01) )
                momBin=7;
            else if ((5.99 < momentum) && (momentum  < 6.01) )
                momBin=8;
            else if ((6.99 < momentum) && (momentum  < 7.01) )
                momBin=9;
            else if ((7.99 < momentum) && (momentum  < 8.01) )
                momBin=10;
            else if ((8.99 < momentum) && (momentum  < 9.01) )
                momBin=11;
            else if ((9.99 < momentum) && (momentum  < 10.01) )
                momBin=12;
            else {
                cout << "Momentum: " << momentum << endl;
                continue;
            }


            if (TMath::Abs(partID) == 11){
                h1dEdxEL[momBin]->Fill((trdHit->GetELoss())*1000000);
                h1momentum->Fill(momentum1);
	    }
            if  (TMath::Abs(partID) == 211)
                h1dEdxPI[momBin]->Fill((trdHit->GetELoss())*1000000);
            if  (TMath::Abs(partID) == 13)
                h1dEdxMU[momBin]->Fill((trdHit->GetELoss())*1000000);
            if  (TMath::Abs(partID) == 321)
                h1dEdxKA[momBin]->Fill((trdHit->GetELoss())*1000000);
            if  (TMath::Abs(partID) == 2212)
                h1dEdxPR[momBin]->Fill((trdHit->GetELoss())*1000000);


            //	}
        }
    }


}

// --------------------------------------------------------------------------


// ---- Finish --------------------------------------------------------------
void CbmTrdCreatePidLike::Finish() {
    WriteHistograms();
}
// --------------------------------------------------------------------------


// ---- Prepare test histograms ---------------------------------------------

void CbmTrdCreatePidLike::PrepareHistograms() {

    Char_t text[200];

    h1momentum = new TH1F("momentum","momentum",1000,0,20.);

    for (Int_t imom = 0; imom < kNMom; imom++) {
        sprintf(text,"h1dEdxEL%01d",imom+1);
        h1dEdxEL[imom] = new TH1F(text,"dE/dx  Dist.",400,0,80);
        sprintf(text,"h1dEdxPI%01d",imom+1);
        h1dEdxPI[imom] = new TH1F(text,"dE/dx  Dist.",400,0,80);
        sprintf(text,"h1dEdxKA%01d",imom+1);
        h1dEdxKA[imom] = new TH1F(text,"dE/dx  Dist.",400,0,80);
        sprintf(text,"h1dEdxPR%01d",imom+1);
        h1dEdxPR[imom] = new TH1F(text,"dE/dx  Dist.",400,0,80);
        sprintf(text,"h1dEdxMU%01d",imom+1);
        h1dEdxMU[imom] = new TH1F(text,"dE/dx  Dist.",400,0,80);
    }
}
// --------------------------------------------------------------------------


// ---- Write test histograms ------------------------------------------------

void CbmTrdCreatePidLike::WriteHistograms() {
    // write the output histograms to a file


    for (Int_t imom = 0; imom < kNMom; imom++) {
        h1dEdxPI[imom]->Write();
        h1dEdxEL[imom]->Write();
        h1dEdxMU[imom]->Write();
        h1dEdxKA[imom]->Write();
        h1dEdxPR[imom]->Write();
    }
    h1momentum->Write();
    //  delete outputFile;

}

// --------------------------------------------------------------------------

ClassImp(CbmTrdCreatePidLike);

