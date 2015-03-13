// -----------------------------------------------------------------------
// -----                     CbmTrdHitProducerQa                     -----
// -----               Created 13/12/05  by M. Kalisky               -----
// -----------------------------------------------------------------------

#include "CbmTrdHitProducerQa.h"

#include "CbmTrdHit.h"
#include "CbmTrdDigi.h"
#include "CbmTrdPoint.h"
#include "CbmGeoTrdPar.h"
#include "CbmMatch.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairBaseParSet.h"
#include "FairRuntimeDb.h"
#include "FairDetector.h"
#include "CbmMCTrack.h"

#include "TClonesArray.h"
#include "TObjArray.h"
#include "TMath.h"
#include "TH1F.h"

#include <iostream>
using std::cout;
using std::endl;

// ---- Default constructor -------------------------------------------------

CbmTrdHitProducerQa::CbmTrdHitProducerQa() 
  : FairTask(),
    fTrdHitCollection(NULL),
    fTrdDigiCollection(NULL),
    fTrdDigiMatchCollection(NULL),
    fTrdPointCollection(NULL),
    fMCTrackArray(NULL),
    fNoTrdStations(-1),
    fNoTrdPerStation(-1),
    fHitPoolsX(NULL),
    fHitPoolsY(NULL),
    S1L1eTR15(NULL),
    S1L1edEdx15(NULL),
    S1L1edE15(NULL),
    S1L1edEall(NULL),
    S1L1pidE15(NULL),
    S1L1pidEall(NULL),
    S3L4eTR15(NULL),
    S3L4edEdx15(NULL),
    S3L4edE15(NULL),
    S3L4edEall(NULL),
    S3L4pidE15(NULL),
    S3L4pidEall(NULL)
{
    PrepareHistograms();
}
// --------------------------------------------------------------------------


// ---- Standard constructor ------------------------------------------------
CbmTrdHitProducerQa::CbmTrdHitProducerQa(const char* name,
					 const char* title)
  : FairTask(name),
    fTrdHitCollection(NULL),
    fTrdDigiCollection(NULL),
    fTrdDigiMatchCollection(NULL),
    fTrdPointCollection(NULL),
    fMCTrackArray(NULL),
    fNoTrdStations(-1),
    fNoTrdPerStation(-1),
    fHitPoolsX(NULL),
    fHitPoolsY(NULL),
    S1L1eTR15(NULL),
    S1L1edEdx15(NULL),
    S1L1edE15(NULL),
    S1L1edEall(NULL),
    S1L1pidE15(NULL),
    S1L1pidEall(NULL),
    S3L4eTR15(NULL),
    S3L4edEdx15(NULL),
    S3L4edE15(NULL),
    S3L4edEall(NULL),
    S3L4pidE15(NULL),
    S3L4pidEall(NULL)
{
    PrepareHistograms();
}
// --------------------------------------------------------------------------


// ---- Destructor ----------------------------------------------------------
CbmTrdHitProducerQa::~CbmTrdHitProducerQa()
{
}
// --------------------------------------------------------------------------


// ---- Initialisation ------------------------------------------------------
InitStatus CbmTrdHitProducerQa::Init()
{
    // Get pointer to the ROOT I/O manager
    FairRootManager* rootMgr = FairRootManager::Instance();
    if(NULL == rootMgr) {
	cout << "-E- CbmTrdHitProducerQa::Init : "
	    << "ROOT manager is not instantiated !" << endl;
	return kFATAL;
    }

    // Get pointer to TRD point array
    fTrdPointCollection = (TClonesArray*) rootMgr->GetObject("TrdPoint");
    if(NULL == fTrdPointCollection) {
	cout << "-W- CbmTrdHitProducerQa::Init : "
	    << "no TRD point array !" << endl;
	return kERROR;
    }

    // Get pointer to TRD digi array
    fTrdDigiCollection = (TClonesArray*) rootMgr->GetObject("TrdDigi");
    if(NULL == fTrdDigiCollection) {
	cout << "-W- CbmTrdHitProducerQa::Init : "
	    << "no TRD digi array !" << endl;
	return kERROR;
    }

    // Get pointer to TRD digi array
    fTrdDigiMatchCollection = (TClonesArray*) rootMgr->GetObject("TrdDigiMatch");
    if(NULL == fTrdDigiMatchCollection) {
	cout << "-W- CbmTrdHitProducerQa::Init : "
	    << "no TRD digi match array !" << endl;
	return kERROR;
    }

    // Get pointer to TRD hit array
    fTrdHitCollection = (TClonesArray*) rootMgr->GetObject("TrdHit");
    if(NULL == fTrdHitCollection) {
	cout << "-W- CbmTrdHitProducerQa::Init : "
	    << "no TRD hit array !" << endl;
	return kERROR;
    }

    // Get MCTrack array
    fMCTrackArray  = (TClonesArray*) rootMgr->GetObject("MCTrack");
    if ( ! fMCTrackArray) {
      cout << "-E- CbmTrdHitProducerQa::Init : No MCTrack array!"
         << endl;
      return kFATAL;
    }

    /*

    // Get the pointer to the singleton FairRunAna object
    FairRunAna* ana = FairRunAna::Instance();
    if(NULL == ana) {
        cout << "-E- CbmTrdFindTracksQa::Init :"
            <<" no FairRunAna object!" << endl;
        return kERROR;
    }

    // Get the pointer to run-time data base
    FairRuntimeDb* rtdb = ana->GetRuntimeDb();
    if(NULL == rtdb) {
        cout << "-E- CbmTrdFindTracksQa::Init :"
            <<" no runtime database!" << endl;
        return kERROR;
    }

    // Get the pointer to container of base parameters
    FairBaseParSet* baseParSet =
        (FairBaseParSet*) rtdb->getContainer("FairBaseParSet");
    if(NULL == baseParSet) {
        cout << "-E- CbmTrdFindTracksQa::Init :"
            <<" no container of base parameters!" << endl;
        return kERROR;
    }
    // Get the pointer to detector list
    TObjArray *detList = baseParSet->GetDetList();
    if(NULL == detList) {
        cout << "-E- CbmTrdFindTracksQa::Init :"
            << " no detector list!" << endl;
        return kERROR;
    }

    // Find TRD detector
    FairDetector* trd = (FairDetector*) detList->FindObject("TRD");
    if(NULL == trd) {
        cout << "-E- CbmTrdFindTracksQa::Init :"
            << " no TRD detector!" << endl;
        return kERROR;
    }

    // Determine the geometry version
    CbmGeoTrdPar* par=(CbmGeoTrdPar*)(rtdb->getContainer("CbmGeoTrdPar"));
    TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
    FairGeoNode *fm1= (FairGeoNode *)fSensNodes->FindObject("trd3gas#4");
    FairGeoNode *fm2= (FairGeoNode *)fSensNodes->FindObject("trd3gas#3");
    FairGeoNode *fm3= (FairGeoNode *)fSensNodes->FindObject("trd6gas#2");
    FairGeoNode *fm4= (FairGeoNode *)fSensNodes->FindObject("trd1mod1gas");

    if (fm1) {
      cout << "-I CbmTrdFindTracksQa::Init : TRD geometry : 3x4" << endl;
      fNoTrdStations = 3;
      fNoTrdPerStation = 4;
    }
    else if (fm2) {
      cout << " -I  CbmTrdFindTracksQa::Init : TRD geometry : 3x3" << endl;
      fNoTrdStations = 3;
      fNoTrdPerStation = 3;
    }
    else if (fm3) {
      cout << " -I CbmTrdFindTracksQa::Init : TRD geometry : 6x2" << endl;
      fNoTrdStations = 6;
      fNoTrdPerStation = 2;
    }
    else if (fm4) {
      cout << "-I CbmTrdFindTracksQa::Init : TRD geometry : 3x4 new segmented geometry" << endl;
      fNoTrdStations = 3;
      fNoTrdPerStation = 4;
    }
    else {
      cout << "***************************************" <<endl;
      cout << "                                       " <<endl;
      cout << " - FATAL ERROR Unknown geometry version" <<endl;
      cout << "   in CbmTrdHitProducerQa              " <<endl;
      cout << "                                       " <<endl;
      cout << "***************************************" <<endl;
      return kFATAL;
    }
    */

    return kSUCCESS;

}

// --------------------------------------------------------------------------


// ---- Task execution ------------------------------------------------------
void CbmTrdHitProducerQa::Exec(Option_t* option)
{
    // Declare variables
    CbmTrdHit* trdHit = NULL;
    CbmTrdDigi* trdDigi = NULL;
    CbmMatch* trdDigiMatch = NULL;
    CbmTrdPoint* trdPoint = NULL;
    //    CbmMCTrack* mctrack = NULL;


    Float_t  hitPoolX = 0;
    Float_t  hitPoolY = 0;
    Float_t  hitPosX  = 0;
    Float_t  hitPosY  = 0;
    Float_t  hitErrX  = 0;
    Float_t  hitErrY  = 0;

    Float_t     pointPosX = 0;
    Float_t     pointPosY = 0;

    Int_t       plane     = 0;
    Int_t       partID    = 0;

    Float_t     momentum;


    // Event counters
    Int_t nTrdHit = fTrdHitCollection->GetEntriesFast();

    // Loop over TRD hits
    for(Int_t iHit = 0; iHit < nTrdHit; iHit++){
	trdHit = (CbmTrdHit*) fTrdHitCollection->At(iHit);
	if(NULL == trdHit) continue;

	// This will have to change in the future, when the creation of the poin
	// will not be necessarily connected to existence of tyhe point

	trdDigiMatch = (CbmMatch*) fTrdDigiMatchCollection->At(trdHit->GetRefId());
	if(NULL == trdDigiMatch) continue;

	trdPoint = (CbmTrdPoint*) fTrdPointCollection->At(trdDigiMatch->GetMatchedLink().GetIndex());
	if(NULL == trdPoint) continue;

        plane = trdHit->GetPlaneId();

        if (plane ==1 ) {

          partID = (((CbmMCTrack*)fMCTrackArray->At(trdPoint->GetTrackID()))->GetPdgCode());

          momentum = TMath::Sqrt((trdPoint->GetPx()*trdPoint->GetPx())+
                                 (trdPoint->GetPy()*trdPoint->GetPy())+
                                 (trdPoint->GetPz()*trdPoint->GetPz()));

	  if ( (TMath::Abs(partID) == 11) && (momentum > 1.25) && (momentum < 1.75) ) {

            S1L1eTR15->Fill((trdHit->GetELossTR())*1000000);
            S1L1edEdx15->Fill((trdHit->GetELossdEdX())*1000000);
            S1L1edE15->Fill((trdHit->GetELoss())*1000000);

	  }
          if (TMath::Abs(partID) == 11){
            S1L1edEall->Fill((trdHit->GetELoss())*1000000);
	  }
          if ((TMath::Abs(partID) == 211) && (momentum > 1.25) && (momentum < 1.75)){
            S1L1pidE15->Fill((trdHit->GetELoss())*1000000);
	  }
          if (TMath::Abs(partID) == 211){
            S1L1pidEall->Fill((trdHit->GetELoss())*1000000);
	  }

	}

       if (plane ==12 ) {

          partID = (((CbmMCTrack*)fMCTrackArray->At(trdPoint->GetTrackID()))->GetPdgCode());

          momentum = TMath::Sqrt((trdPoint->GetPx()*trdPoint->GetPx())+
                                 (trdPoint->GetPy()*trdPoint->GetPy())+
                                 (trdPoint->GetPz()*trdPoint->GetPz()));

          if (TMath::Abs(partID) == 11){
            S3L4edEall->Fill((trdHit->GetELoss())*1000000);
	  }
          if (TMath::Abs(partID) == 211){
            S3L4pidEall->Fill((trdHit->GetELoss())*1000000);
	  }

	}



        if (plane ==1 ) {
	// compute the Hit pools for X and Y coordinate
	hitPosX   = trdHit->GetX();
	hitErrX   = trdHit->GetDx();
	//        hitErrX  /= 10000;           // micrometers to centimeters
	pointPosX = trdPoint->GetX();
	hitPoolX  = (hitPosX - pointPosX);///hitErrX;



	hitPosY   = trdHit->GetY();
	hitErrY   = trdHit->GetDy();
	//        hitErrY  /= 10000;          // micrometers to centimeters
	pointPosY = trdPoint->GetY();
	hitPoolY  = (hitPosY - pointPosY);///hitErrY;


       	// fill histograms
	fHitPoolsX->Fill(hitPoolX);
	fHitPoolsY->Fill(hitPoolY);

	}
    }


}

// --------------------------------------------------------------------------


// ---- Finish --------------------------------------------------------------
void CbmTrdHitProducerQa::Finish()
{
  WriteHistograms();
}
// --------------------------------------------------------------------------


// ---- Prepare test histograms ---------------------------------------------

void CbmTrdHitProducerQa::PrepareHistograms()
{

    fHitPoolsX=NULL;
    fHitPoolsY=NULL; 

    S1L1eTR15=NULL; 
    S1L1edEdx15=NULL;
    S1L1edE15=NULL; 
    S1L1edEall=NULL;
    S1L1pidE15=NULL;
    S1L1pidEall=NULL;

    S3L4eTR15=NULL;
    S3L4edEdx15=NULL;
    S3L4edE15=NULL;
    S3L4edEall=NULL;
    S3L4pidE15=NULL;
    S3L4pidEall=NULL;

    fHitPoolsX = new TH1F("fHitPoolsX", "", 500, -50, 50);
    fHitPoolsY = new TH1F("fHitPoolsY", "", 500, -50, 50);

    S1L1eTR15   = new TH1F("S1L1eTR15","TR of e- for first layer ",600,0.,60.);
    S1L1edEdx15 = new TH1F("S1L1edEdx15","dEdx of e- for first layer ",600,0.,60.);
    S1L1edE15 = new TH1F("S1L1edE15","dEdx+TR of e- for first layer ",600,0.,60.);
    S1L1edEall = new TH1F("S1L1edEall","dEdx+TR of e- for first layer ",600,0.,60.);
    S1L1pidE15 = new TH1F("S1L1pidE15","dEdx+TR of pi- for first layer ",600,0.,60.);
    S1L1pidEall = new TH1F("S1L1pidEall","dEdx+TR of pi- for first layer ",600,0.,60.);
    S3L4edEall = new TH1F("S3L4edEall","dEdx+TR of e- for layer 12",600,0.,60.);
    S3L4pidEall = new TH1F("S3L4pidEall","dEdx+TR of pi- for layer 12",600,0.,60.);

}
// --------------------------------------------------------------------------


// ---- Write test histograms ------------------------------------------------

void CbmTrdHitProducerQa::WriteHistograms()
{
    if(fHitPoolsX)    fHitPoolsX->Write();
    if(fHitPoolsY)    fHitPoolsY->Write();

    if(S1L1eTR15)      S1L1eTR15->Write();
    if(S1L1edEdx15)    S1L1edEdx15->Write();
    if(S1L1edE15)      S1L1edE15->Write();
    if(S1L1edEall)      S1L1edEall->Write();
    if(S1L1pidE15)     S1L1pidE15->Write();
    if(S1L1pidEall)     S1L1pidEall->Write();

    if(S3L4eTR15)      S3L4eTR15->Write();
    if(S3L4edEdx15)    S3L4edEdx15->Write();
    if(S3L4edE15)      S3L4edE15->Write();
    if(S3L4edEall)      S3L4edEall->Write();
    if(S3L4pidE15)     S3L4pidE15->Write();
    if(S3L4pidEall)     S3L4pidEall->Write();
}

// --------------------------------------------------------------------------

ClassImp(CbmTrdHitProducerQa);

