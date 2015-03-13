/***********************************************************************************
 *  $Id: CbmJpsiTrigger.cxx 1536 2007-09-27 09:41:06Z uhlig $
 *
 *  Class : CbmJpsiTrigger
 ***********************************************************************************/
// CBMROOT includes
#include "CbmJpsiTrigger.h"
#include "CbmJpsiTrackCollection.h"
#include "CbmJpsiTriggerElectron.h"
#include "FairTask.h"
#include "FairRootManager.h"
#include "CbmTrdTrack.h"
#include "CbmGlobalTrack.h"
#include "CbmStsTrack.h"
#include "CbmMCTrack.h"
#include "FairTrackParam.h"

// ROOT includes
#include "TClonesArray.h"
#include "TFile.h"
#include "TH1F.h"
#include "TMath.h"
#include "TRandom.h"
#include "TVector3.h"
#include <iostream>
#include <iomanip>
using std::cout;
using std::endl;

ClassImp(CbmJpsiTrigger);

CbmJpsiTrigger::CbmJpsiTrigger() : FairTask("CbmJpsiTrigger"){

  fJpsiColl = new CbmJpsiTrackCollection();

  fCutPt = 1.;
  
  fVerbose = 0;
  
  fNEvents = 0;
  fElectrons=0;
  fOutFileName = "trigger.jpsi.mc.root";
}

CbmJpsiTrigger::CbmJpsiTrigger(Int_t iVerbose, TString fname, const char* name) : FairTask(name,iVerbose){


 fJpsiColl = new CbmJpsiTrackCollection();
 // fJpsiColl = new CbmJpsiTriggerElectron();

  fCutPt = 1.;
  
  fVerbose = iVerbose;
  
  fNEvents = 0;
  fOutFileName = fname;
  fElectrons=0;
}

CbmJpsiTrigger::~CbmJpsiTrigger(){

}

InitStatus CbmJpsiTrigger::Init()
{
  
  cout<<"-I- CbmJpsiTrigger::Init: Initialization started"<<endl;
  
  // Get pointers to root manager
  fRootManager = FairRootManager::Instance();
  
  
  
  // Get MCTrack Array
  fMCStack = (TClonesArray*) fRootManager->GetObject("MCTrack");
  if (! fMCStack) {
    cout << "-E- CbmJpsiTrigger::Init: No MCTrack array!" << endl;
    return kFATAL;
  }
  
     fGlobalTrack = (TClonesArray*) fRootManager->GetObject("GlobalTrack");
    if (! fGlobalTrack) {
        cout << "-E- CbmDileptonAssignPid::Init: No GlobalTrack array!" << endl;
	return kFATAL;
    }

    // Get STSTrack Array
    fStsTrack = (TClonesArray*) fRootManager->GetObject("StsTrack");
    if (! fStsTrack) {
	cout << "-E- CbmDileptonAssignPid::Init: No STSTrack array!" << endl;
	return kFATAL;
    }

    // Get TrdTrack array
    fTrdTrack = (TClonesArray*) fRootManager->GetObject("TrdTrack");
    if (! fTrdTrack) {
        cout << "-E- CbmDileptonAssignPid::Init: No TrdTrack array!" << endl;
        return kFATAL;
    }
 
  // Get TRD tracks Array
  fTrdTrack = (TClonesArray*) fRootManager->GetObject("TrdTrack");
  
  // fJpsiEl = new TClonesArray("CbmJpsiTriggerElectron",1000); 
  //    fRootManager->Register("CbmJpsiTriggerElectron","Jpsi electrons canditates",fJpsiEl ,kTRUE);
    fRootManager->Register("CbmJpsiTriggerElectron","Jpsi electrons canditates",
			 fJpsiColl->JpsiTracks() ,kTRUE);
  
  
  
    cout<<"-I- CbmJpsiTrigger::Init: Initialization complete"<<endl;
    
    return kSUCCESS;
}

void  CbmJpsiTrigger::Exec(Option_t* option)
{

  fTimer.Start();
  
  if (fVerbose>0) cout<<" -I-  CbmJpsiTrigger::Exec() : "<<endl;
  TLorentzVector  momMC;
  
  fJpsiColl->deleteTrack();

  TVector3 mom;
  fNEvents++;  

  fElectrons = 0;

  Int_t nparticles = fGlobalTrack->GetEntriesFast();
  
  Int_t nStsTrack    = fStsTrack->GetEntriesFast();
  Int_t nTrdTrack    = fTrdTrack->GetEntriesFast();
  for (int j=0; j<nparticles; j++ ) {
    
    //Get Global Track
    CbmGlobalTrack* fGlTrack = (CbmGlobalTrack*)fGlobalTrack->At(j);
    
    //Get STS information
    Int_t stsTrackIndex = fGlTrack->GetStsTrackIndex();
    if (stsTrackIndex == -1) continue;
    
    // Get corresponding TRD track  (later check if available)
    if (stsTrackIndex == -1) continue;
    CbmStsTrack* stsTrack = (CbmStsTrack*)fStsTrack->At(stsTrackIndex);
    if (!stsTrack) continue;
    
    if(stsTrack->GetNDF() == 0) continue;
    Double_t chi2= (stsTrack->GetChi2())/Double_t(stsTrack->GetNDF());
    	  cout<<"sts chi2 "<<chi2<<endl;
    if (chi2>3) continue;
    FairTrackParam *parf = stsTrack->GetParamFirst();
    parf->Momentum(mom);
    if (mom.Pt() < 1. ) continue;
    Double_t charge= parf->GetQp();
 	
    // Get the Trd track
    Int_t trdTrackIndex = fGlTrack->GetTrdTrackIndex();
    if (trdTrackIndex == -1)  continue;
    CbmTrdTrack* trdtr = (CbmTrdTrack*)fTrdTrack->At(trdTrackIndex);
    cout<<" trdTrackIndex "<<trdTrackIndex<<" trdtr "<<trdtr<<endl;
    if(!trdtr) continue;
     
    Double_t wkn=trdtr->GetPidWkn();
    Double_t ann=trdtr->GetPidANN();
    Double_t like= trdtr->GetPidLikeEL();
    //    if(wkn == -1 ) continue;
    //   if(like == -1 ) continue;
    //    if(ann == -2 ) continue;
    Int_t partPdgMC = 11; // zaglushka!!!!
    // TClonesArray& clref = *fTracks;
    // new ( clref[clref.GetEntriesFast()] ) CbmJpsiTriggerElectron(mom,pdg, ann, like, wkn);
  //  cout<<"CbmJpsiTrackCollection>> "<<clref.GetEntriesFast()<<endl; 
    //  fNTracks++;
    fJpsiColl->AddTrack(mom, partPdgMC, ann, like, wkn);
        cout<<fNEvents<<" "<<j<<"nel "<<fElectrons<<" Pt"<<mom.Pt()<<
      " pdgMC "<<partPdgMC<<" ann "<<ann<<" like "<<like<<" wkn "<<wkn<<endl;
    fElectrons++;
  }

  cout<<" altogether electrons "<<fElectrons<<endl;

}
//--------------------------------------------------------------------------- 
void CbmJpsiTrigger::SetCuts(Double_t cutPt)
{

    fCutPt = cutPt;
}
//-------------------------------------------------------------------------------
