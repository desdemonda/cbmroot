/******************************************************************************
 *  $Id: CbmDileptonAssignPid.cxx 3856 2012-02-15 17:14:28Z f.uhlig $
 *
 *  Class : CbmDileptonAssignPid
 *  Descripton: This is a task, derived from FairTask. This works for the real
 *              data stream to perform electron identification.
 *              Input: Arrays of global, sts, trd track, rich ring and tof hit
 *              arrays.
 *              Output: Array of CbmDileptonTrackReal
 *
 *  Author: Supriya Das
 *  E-mail: S.Das@gsi.de
 *****************************************************************************/
// CBMROOT includes
#include "CbmDileptonAssignPid.h"

#include "CbmDileptonTrackRealCollection.h"

#include "CbmVertex.h"
#include "CbmGlobalTrack.h"
#include "CbmStsTrack.h"
#include "CbmRichRing.h"
#include "CbmTrdTrack.h"
#include "CbmTofHit.h"

#include "FairTask.h"
#include "FairRootManager.h"

//ROOT includes
#include "TH2.h"
#include "TH1.h"
#include "TFile.h"
#include "TMath.h"
#include "TClonesArray.h"

#include <iostream>
#include <iomanip>

using std::cout;
using std::endl;
using std::setw;
using std::setprecision;
using std::right;

CbmDileptonAssignPid::CbmDileptonAssignPid() 
  : FairTask("DileptonAssignPid"),
    fRootManager(NULL),
    fTrackRealColl(new CbmDileptonTrackRealCollection()),
    fArrayGlobalTrack(NULL),
    fArrayStsTrack(NULL),
    fArrayRichRing(NULL),
    fArrayTrdTrack(NULL),
    fArrayTofHit(NULL),
    fFitter(),
    fPrimVertex(NULL),
    fswitchRichMom(kTRUE),
    fswitchRichSelection(kTRUE),
    fRichDist(1.),
    fRichNN(-0.5),
    fRich2D(0.),
    fRichRadial(130.),
    fRichNHitMean(21.85),
    fRichNHitSigma(4.35),
    fRichRmean(6.17),
    fRichRsigma(0.14),
    fswitchMom(kTRUE),
    fswitchAcceptTrd(kFALSE),
    fswitchLike(kTRUE),
    fswitchWkn(kFALSE),
    fswitchAnn(kFALSE),
    fTrdMom(1.5),
    fTrdPidLikeLow(0.95),
    fTrdPidLikeHigh(1.1),
    fTrdPidWkn(11.),
    fTrdPidAnn(0.8),
    fswitchTof(kTRUE),
    fswitchAcceptTof(kFALSE),
    fTofMass2(0.01),
    fVerbose(0),
    fOutFileName(""),
    nElec(0),
    fh_rich_ring_dist_mom(NULL),
    fh_rich_ring_dist_mom_after(NULL),
    fh_rich_selection_NN(NULL),
    fh_rich_selection_NN_after(NULL),
    fh_rich_selection_2D(NULL),
    fh_rich_selection_2D_after(NULL),
    fh_rich_ring_radial(NULL),
    fh_rich_ring_radial_after(NULL),
    fh_rich_ring_xy(NULL),
    fh_rich_ring_xy_after(NULL),
    fh_rich_ring_hits_radius(NULL),
    fh_rich_ring_hits_radius_after(NULL),
    fh_rich_ring_radius_mom(NULL),
    fh_rich_ring_radius_mom_after(NULL),
    fh_trd_like_id(NULL),
    fh_trd_like_id_after(NULL),
    fh_trd_wkn_id(NULL),
    fh_trd_wkn_id_after(NULL),
    fh_trd_ann_id(NULL),
    fh_trd_ann_id_after(NULL),
    fh_tof_m2_mom(NULL),
    fh_tof_m2_mom_after(NULL),
    foutFile(NULL),
    fTimer(),
    fNEvents(0),
    fTime(0.)
{  
}

CbmDileptonAssignPid::CbmDileptonAssignPid(Int_t iVerbose, TString fname, const char* name) 
  : FairTask(name,iVerbose), 
    fRootManager(NULL),
    fTrackRealColl(new CbmDileptonTrackRealCollection()),
    fArrayGlobalTrack(NULL),
    fArrayStsTrack(NULL),
    fArrayRichRing(NULL),
    fArrayTrdTrack(NULL),
    fArrayTofHit(NULL),
    fFitter(),
    fPrimVertex(NULL),
    fswitchRichMom(kTRUE),
    fswitchRichSelection(kTRUE),
    fRichDist(1.),
    fRichNN(-0.5),
    fRich2D(0.),
    fRichRadial(130.),
    fRichNHitMean(21.85),
    fRichNHitSigma(4.35),
    fRichRmean(6.17),
    fRichRsigma(0.14),
    fswitchMom(kTRUE),
    fswitchAcceptTrd(kFALSE),
    fswitchLike(kTRUE),
    fswitchWkn(kFALSE),
    fswitchAnn(kFALSE),
    fTrdMom(1.5),
    fTrdPidLikeLow(0.95),
    fTrdPidLikeHigh(1.1),
    fTrdPidWkn(11.),
    fTrdPidAnn(0.8),
    fswitchTof(kTRUE),
    fswitchAcceptTof(kFALSE),
    fTofMass2(0.01),
    fVerbose(iVerbose),
    fOutFileName(fname),
    nElec(0),
    fh_rich_ring_dist_mom(NULL),
    fh_rich_ring_dist_mom_after(NULL),
    fh_rich_selection_NN(NULL),
    fh_rich_selection_NN_after(NULL),
    fh_rich_selection_2D(NULL),
    fh_rich_selection_2D_after(NULL),
    fh_rich_ring_radial(NULL),
    fh_rich_ring_radial_after(NULL),
    fh_rich_ring_xy(NULL),
    fh_rich_ring_xy_after(NULL),
    fh_rich_ring_hits_radius(NULL),
    fh_rich_ring_hits_radius_after(NULL),
    fh_rich_ring_radius_mom(NULL),
    fh_rich_ring_radius_mom_after(NULL),
    fh_trd_like_id(NULL),
    fh_trd_like_id_after(NULL),
    fh_trd_wkn_id(NULL),
    fh_trd_wkn_id_after(NULL),
    fh_trd_ann_id(NULL),
    fh_trd_ann_id_after(NULL),
    fh_tof_m2_mom(NULL),
    fh_tof_m2_mom_after(NULL),
    foutFile(NULL),
    fTimer(),
    fNEvents(0),
    fTime(0.)

{
}

CbmDileptonAssignPid::~CbmDileptonAssignPid() 
{
}
//--------------------------------------------------------------------------
InitStatus CbmDileptonAssignPid::Init(){

    //Get all the necessary arrays

    // Get pointers to root manager
    fRootManager = FairRootManager::Instance();

    // Get Primary Vertex
    fPrimVertex = (CbmVertex*) fRootManager->GetObject("PrimaryVertex");
    if(!fPrimVertex){
        cout<<"-E- CbmDileptonAssignPid::Init No Primary Vertex!" <<endl;
        return kFATAL;
    }

    // Get GlobalTrack Array
    fArrayGlobalTrack = (TClonesArray*) fRootManager->GetObject("GlobalTrack");
    if (! fArrayGlobalTrack) {
        cout << "-E- CbmDileptonAssignPid::Init: No GlobalTrack array!" << endl;
	return kFATAL;
    }

    // Get STSTrack Array
    fArrayStsTrack = (TClonesArray*) fRootManager->GetObject("StsTrack");
    if (! fArrayStsTrack) {
	cout << "-E- CbmDileptonAssignPid::Init: No STSTrack array!" << endl;
	return kFATAL;
    }

    // Get RichTrack Array
    fArrayRichRing = (TClonesArray*) fRootManager->GetObject("RichRing");
    if (! fArrayRichRing) {
        cout << "-E- CbmDileptonAssignPid::Init: No RichRing array!" << endl;
        return kFATAL;
    }

    // Get TrdTrack array
    fArrayTrdTrack = (TClonesArray*) fRootManager->GetObject("TrdTrack");
    if (! fArrayTrdTrack) {
        cout << "-E- CbmDileptonAssignPid::Init: No TrdTrack array!" << endl;
        return kFATAL;
    }

    // Get TofHit array
    fArrayTofHit = (TClonesArray*) fRootManager->GetObject("TofHit");
    if (! fArrayTofHit) {
        cout << "-E- CbmDileptonAssignPid::Init: No TofHit array!" << endl;
        return kFATAL;
    }

   // fTrackRealColl = (TClonesArray*) fRootManager->GetObject("CbmDileptonTrackReal");
   // if (!fTrackRealColl)
    fRootManager->Register("DileptonTrackReal","Dilepton", fTrackRealColl->DileptonTracksReal(),kTRUE);

    // initialize KF-Fitter
    fFitter.Init();

    BookHistograms();

    cout << "-I- CbmDileptonAssignPid::Init() : " << "Initialisation completed." << endl;

    return kSUCCESS;
}
//----------------------------------------------------------------------
void CbmDileptonAssignPid::Exec(Option_t* opt){

    fTimer.Start();

    // Clear the trackcollection to free memory
    fTrackRealColl->deleteTrack();

    Int_t nGlobalTrack = fArrayGlobalTrack->GetEntriesFast();

    Int_t nStsTrack    = fArrayStsTrack->GetEntriesFast();
    Int_t nRichRing    = fArrayRichRing->GetEntriesFast();
    Int_t nTrdTrack    = fArrayTrdTrack->GetEntriesFast();


    if(fVerbose>0)  {

	cout<<"-I- CbmDileptonAssignPid::Exec() , Event :"<<fNEvents<<endl;
	cout<<"\tGlobal Tracks : "<<nGlobalTrack<<endl;
	cout<<"\t  STS Tracks  : "<<nStsTrack<<endl;
	cout<<"\t  RICH Rings  : "<<nRichRing<<endl;
	cout<<"\t  TRD Tracks  : "<<nTrdTrack<<endl;

    }
   
    for(Int_t iGlobal=0; iGlobal < nGlobalTrack; iGlobal++){

	//Get Global Track
	CbmGlobalTrack* fGlobalTrack= (CbmGlobalTrack*)fArrayGlobalTrack->At(iGlobal);

	//Get STS information
	Int_t stsTrackIndex = fGlobalTrack->GetStsTrackIndex();
	if (stsTrackIndex == -1) continue;

	// Get RICH information
	Int_t richRingIndex = fGlobalTrack->GetRichRingIndex();
	//if (richRingIndex == -1) continue; // no ring assigned: Index = -1

	// Get corresponding TRD track  (later check if available)
	Int_t trdTrackIndex = fGlobalTrack->GetTrdTrackIndex();

	// Get corresponding TOF hit (later check if available)
	Int_t tofHitIndex = fGlobalTrack->GetTofHitIndex();

        if(fVerbose>1) cout<<"-I- CbmDileptonAssignPid::Exec() : GlobalTrack no. "<<iGlobal<<":"<<"got all indices"<<endl;

	CbmStsTrack* stsTrack = (CbmStsTrack*)fArrayStsTrack->At(stsTrackIndex);
	if (!stsTrack) continue;
	
	Double_t chiPrimary = fFitter.GetChiToVertex(stsTrack);

	if(fVerbose>1) cout<<"-I- CbmDileptonAssignPid::Exec() : "<<iGlobal<<" "<<chiPrimary<<endl;

	// Fit tracks to the primary vertex
	FairTrackParam VtxTrack;
	fFitter.FitToVertex(stsTrack, fPrimVertex, &VtxTrack);

	TVector3 position;
	TVector3 momentum;

	VtxTrack.Position(position);
	VtxTrack.Momentum(momentum);

	// Get the charge and energy
	Int_t charge    = (VtxTrack.GetQp() > 0) ?1 :-1;

        // Get the tracklength
	Double_t trackLength = fGlobalTrack->GetLength()/100.;

        // Get info on RICH, TRD and TOF
	Bool_t richPid = GetRichPid(fswitchRichMom, fswitchRichSelection, momentum, richRingIndex);
        Bool_t trdPid  = GetTrdPid(richPid,fswitchMom,fswitchAcceptTrd,fswitchLike, fswitchWkn, fswitchAnn, momentum, trdTrackIndex);
        Bool_t tofPid  = GetTofPid(richPid,fswitchTof, fswitchAcceptTof,momentum, tofHitIndex, trackLength);

       if(fVerbose>1) cout<<"-I- CbmDileptonAssignPid::Exec() : gTrack,rich,trd,tof : "<<iGlobal <<" "<<richPid<<" "<<trdPid<<" "<<tofPid<<endl;

        //Add This track to Collection
         fTrackRealColl->addTrack(iGlobal,charge,momentum,chiPrimary,richPid,trdPid,tofPid);

    }

    fTimer.Stop();
    fTime += fTimer.RealTime();

    fNEvents++;

}
//---------------------------------------------------------------
void CbmDileptonAssignPid::Finish(){

    foutFile = new TFile(fOutFileName,"RECREATE");
    foutFile->cd();
    SaveHistograms();
    foutFile->Close();

    cout<<"-I- CbmDileptonAssignPid::Finish() : Output histogram file closed "<<endl;

    cout << endl;
    cout << "============================================================"
	<< endl;
    cout << "=====   " << GetName() << ": Run summary " << endl;
    cout << "===== " << endl;
    cout << "===== Good events   : " << setw(6) << fNEvents << endl;
    cout << "===== Average time  : " << setprecision(4) << setw(8) << right
	<< fTime / Double_t(fNEvents)  << " s" << endl;
    cout << "============================================================"
	<< endl;
}
//----------------------------------------------------------------
void CbmDileptonAssignPid::FinishTask(){
    if(fVerbose>0) cout<<"-I- CbmDileptonAssignPid::FinishTask() : container cleared" <<endl;
    fTrackRealColl->clearTrack();
}

//---------------------------------------------------------------
Bool_t CbmDileptonAssignPid::GetRichPid(Bool_t switchRichMom, Bool_t switchRichSelection, TVector3 momentum, Int_t richRingIndex )
{

    Bool_t isDist = kFALSE;

    if(richRingIndex == -1) return kFALSE;  //No RICH ring associated with this global track
    //Get the Rich Ring
    CbmRichRing* richRing = (CbmRichRing*)fArrayRichRing->At(richRingIndex);
    if(!richRing) return kFALSE;

    //distance cut
    Double_t distance = richRing->GetDistance();

    if(fVerbose>2)  cout<<"-I- CbmDileptonAssignPid::GetRichPid(): mom, ring-track distance : "<<momentum.Mag()<<" "<<distance<<endl;

    fh_rich_ring_dist_mom->Fill(momentum.Mag(),distance);

    if (switchRichMom){
	if (momentum.Mag() < 0.5 && distance < 2. ) isDist = kTRUE;
	if (momentum.Mag() >= 0.5 && momentum.Mag() < 2. ) {
	    if (distance < (7./3.-2./3.* momentum.Mag())) isDist = kTRUE;
	}
	if (momentum.Mag() >= 2. && distance < 1.) isDist = kTRUE;
    } else {
	if (distance < fRichDist) isDist = kTRUE;
    }

    if(isDist == kTRUE) fh_rich_ring_dist_mom_after->Fill(momentum.Mag(),distance);

    else return kFALSE;

    // use ring quality cut

    fh_rich_selection_NN->Fill(richRing->GetSelectionNN());
//    fh_rich_selection_2D->Fill(richRing->GetSelection2D());

    if(switchRichSelection){
	if (fVerbose>2) cout<<"-I- CbmDileptonAssignPid::GetRichPid(): switchRichSelection : "<<switchRichSelection<<", using NN selection cut "<<endl;

	if(richRing->GetSelectionNN() > fRichNN)fh_rich_selection_NN_after->Fill(richRing->GetSelectionNN());
	else return kFALSE;
    }

    else {
	if(fVerbose>2) cout<<"-I- CbmDileptonAssignPid::GetRichPid(): switchRichSelection : "<<switchRichSelection<<",using 2D selection cut "<<endl;

//      if(richRing->GetSelection2D() == fRich2D)fh_rich_selection_2D_after->Fill(richRing->GetSelection2D());
//	else return kFALSE;
    }

    // calculate radial
    Double_t centreX = richRing->GetCenterX();
    Double_t centreY = richRing->GetCenterY();

    fh_rich_ring_xy->Fill( centreX,centreY);

    Double_t radial = TMath::Sqrt(TMath::Power(centreX,2)+TMath::Power(centreY,2));

    //cout<<"radial : "<<radial<<endl;
    fh_rich_ring_radial->Fill(radial);

    if(radial > fRichRadial) {
	fh_rich_ring_radial_after->Fill(radial);
	fh_rich_ring_xy_after->Fill(centreX,centreY);
    }
     else return kFALSE;

    if(fVerbose>2)cout<<"-I- CbmDileptonAssignPid::GetRichPid(): I'm here : after the radial cut"<<endl;
    // Hit cut
    Double_t radius = richRing->GetRadius();

    Double_t nhit = Double_t(richRing->GetNofHits());

    fh_rich_ring_hits_radius->Fill(radius,nhit);
    if(nhit > (fRichNHitMean - 3*fRichNHitSigma) && nhit < (fRichNHitMean + 2*fRichNHitSigma))fh_rich_ring_hits_radius_after->Fill(radius,nhit);
    else return kFALSE;

    //Radius cut

    fh_rich_ring_radius_mom->Fill(momentum.Mag(),radius);
    if(radius > (fRichRmean - 3*fRichRsigma) && radius < (fRichRmean + 3*fRichRsigma)) fh_rich_ring_radius_mom_after->Fill(momentum.Mag(),radius);
    else return kFALSE;
    
    if(fVerbose>2) cout<<"-I- CbmDileptonAssignPid::GetRichPid(): I'm at the end of RichID"<<endl;

    return kTRUE;
}
//----------------------------------------------------------------------
Bool_t CbmDileptonAssignPid::GetTrdPid(Bool_t isRich, Bool_t switchMom, Bool_t switchAcceptTrd, Bool_t switchLike, Bool_t switchWkn, Bool_t switchAnn, TVector3 momentum, Int_t trdTrackIndex)
{

    Bool_t tempLike = true;
    Bool_t tempAnn  = true;
    Bool_t tempWkn  = true;

    // Decide what to do with low momenta tracks
    if(switchMom && momentum.Mag() < fTrdMom) return kTRUE;

    // Decide what to do when there is no trd track associated with global (default -> returns kFALSE)
    if(trdTrackIndex == -1){

	if(switchAcceptTrd) {
	    if(isRich) return kTRUE;
	    else return kFALSE;
	}
	else return kFALSE;

    }

    // Get the Trd track
    if (trdTrackIndex == -1)  return kFALSE;
    CbmTrdTrack* trdTrack = (CbmTrdTrack*)fArrayTrdTrack->At(trdTrackIndex);
    if(!trdTrack) return kFALSE;

    if(switchLike){
	fh_trd_like_id->Fill(trdTrack->GetPidLikeEL());
	if (trdTrack->GetPidLikeEL()<fTrdPidLikeLow || trdTrack->GetPidLikeEL()>fTrdPidLikeHigh) tempLike=false;
	else fh_trd_like_id_after->Fill(trdTrack->GetPidLikeEL());
    }
    if( switchWkn ){
	fh_trd_wkn_id->Fill(trdTrack->GetPidWkn());
	if(trdTrack->GetPidWkn()<fTrdPidWkn) tempWkn=false;
	else fh_trd_wkn_id_after->Fill(trdTrack->GetPidWkn());
    }
    if( switchAnn ){
	fh_trd_ann_id->Fill(trdTrack->GetPidANN());
	if(trdTrack->GetPidANN()<fTrdPidAnn) tempAnn=false;
	else fh_trd_ann_id_after->Fill(trdTrack->GetPidANN());
    }

    return tempLike*tempWkn*tempAnn;

}
//----------------------------------------------------------------------
Bool_t CbmDileptonAssignPid::GetTofPid(Bool_t isRich, Bool_t switchTof, Bool_t switchAcceptTof, TVector3 momentum, Int_t tofHitIndex, Double_t trackLength)
{

    Double_t mass2 = -1;

    //Decide what to do when no tof hit associated with global (default -> returns kFALSE)

    if (tofHitIndex == -1){

	if(switchAcceptTof) {
	    if(isRich) return kTRUE;
	    else return kFALSE;
	}
	else return kFALSE;

    }

    // Get TofHit
    CbmTofHit* tofHit = (CbmTofHit*)fArrayTofHit->At(tofHitIndex);
    if(!tofHit) return kFALSE;

    Double_t time = 0.2998*tofHit->GetTime();          // time in ns-> transfrom to ct in m
   
    Bool_t tempTof=kFALSE;

    if(trackLength) {
	  mass2 = TMath::Power(momentum.Mag(),2.)*(TMath::Power(time/trackLength,2) - 1);
    } else {
	  return tempTof;
    }
	
    if(fVerbose>2)cout<<"-I- CbmDileptonAssignPid::GetTofPid(): momentum, mass2 : "<<momentum.Mag()<<" "<<mass2<<endl;

    fh_tof_m2_mom->Fill(momentum.Mag(),mass2);

    if(switchTof) {
	if (momentum.Mag() < 1. && mass2 < fTofMass2) tempTof=true;

	if (momentum.Mag() >= 1. ) {
	    if (mass2 < (fTofMass2 + (momentum.Mag()-1.)*0.09))tempTof=true;
	}
    } else {
	if (mass2 < fTofMass2) tempTof=true;
    }

    if(tempTof) fh_tof_m2_mom_after->Fill(momentum.Mag(),mass2);

    return tempTof;
}
//----------------------------------------------------------------------
void CbmDileptonAssignPid::SetRichCuts(Bool_t sRichMom, Bool_t sRichSelection, Float_t richCuts[])
{
    cout<<" -I-  CbmDileptonAssignPid::SetRichCuts() : Setting Rich Cuts "
	<< " ---- Changing default switch / cut value, beware of consequence -----"<<endl;

    fswitchRichMom       = sRichMom;
    fswitchRichSelection = sRichSelection;

    fRichDist      = richCuts[0];
    fRichNN        = richCuts[1];
    fRich2D        = richCuts[2];
    fRichRadial    = richCuts[3];
    fRichNHitMean  = richCuts[4];
    fRichNHitSigma = richCuts[5];
    fRichRmean     = richCuts[6];
    fRichRsigma    = richCuts[7];
}
//----------------------------------------------------------------------
void CbmDileptonAssignPid::SetTrdCuts(Bool_t sMom, Bool_t sAccept, Bool_t sLike, Bool_t sWkn, Bool_t sAnn, Float_t trdCuts[])
{
    cout<<" -I-  CbmDileptonAssignPid::SetTrdCuts() : Setting Trd Cuts "
	<< " ---- Changing default switch / cut value, beware of consequence -----"<<endl;

    fswitchMom       = sMom;
    fswitchAcceptTrd = sAccept;
    fswitchLike      = sLike;
    fswitchWkn       = sWkn;
    fswitchAnn       = sAnn;

    fTrdMom         = trdCuts[0];
    fTrdPidLikeLow  = trdCuts[1];
    fTrdPidLikeHigh = trdCuts[2];
    fTrdPidWkn      = trdCuts[3];
    fTrdPidAnn      = trdCuts[4];
}
//----------------------------------------------------------------------
void CbmDileptonAssignPid::SetTofCuts(Bool_t sTof, Bool_t sAccept, Float_t tofCut)
{
    cout<<" -I-  CbmDileptonAssignPid::SetTofCuts() : Setting Tof Cuts "
	<< " ---- Changing default switch / cut value, beware of consequence -----"<<endl;

    fswitchTof       = sTof;
    fswitchAcceptTof = sAccept;

    fTofMass2 = tofCut;

}
//----------------------------------------------------------------------
void CbmDileptonAssignPid::BookHistograms()
{
    fh_rich_ring_dist_mom          = new TH2D("h_rich_ring_dist_mom","",100, 0., 10., 100, 0., 10.);
    fh_rich_ring_dist_mom_after    = new TH2D("h_rich_ring_dist_mom_after","",100, 0., 10., 100, 0., 10.);
    fh_rich_selection_NN           = new TH1D("h_rich_selection_NN","",100,0.,1.);
    fh_rich_selection_NN_after     = new TH1D("h_rich_selection_NN_after","",100,0.,1.);
    fh_rich_selection_2D           = new TH1D("h_rich_selection_2D","",100,0.,1.);
    fh_rich_selection_2D_after     = new TH1D("h_rich_selection_2D_after","",100,0.,1.);
    fh_rich_ring_radial            = new TH1D("h_rich_ring_radial","",600,0.,300.);
    fh_rich_ring_radial_after      = new TH1D("h_rich_ring_radial_after","",600,0.,300.);
    fh_rich_ring_xy                = new TH2D("h_rich_ring_xy","",170, -170., 170., 250, -250., 250.);
    fh_rich_ring_xy_after          = new TH2D("h_rich_ring_xy_after","",170, -170., 170., 250, -250., 250.);
    fh_rich_ring_hits_radius       = new TH2D("h_rich_ring_hits_radius",  "", 100, 0., 20.,100, 0., 100.);
    fh_rich_ring_hits_radius_after = new TH2D("h_rich_ring_hits_radius_after",  "", 100, 0., 20.,100, 0., 100.);
    fh_rich_ring_radius_mom        = new TH2D("h_rich_ring_radius_mom","",100, 0., 10., 100, 0., 20.);
    fh_rich_ring_radius_mom_after  = new TH2D("h_rich_ring_radius_mom_after","",100, 0., 10., 100, 0., 20.);

    fh_trd_like_id                 = new TH1D("h_trd_like_id","",100, 0., 1.);
    fh_trd_like_id_after           = new TH1D("h_trd_like_id_after","",100, 0., 1.);
    fh_trd_wkn_id                  = new TH1D("h_trd_wkn_id","",100, 0., 50.);
    fh_trd_wkn_id_after            = new TH1D("h_trd_wkn_id_after","",100, 0., 50.);
    fh_trd_ann_id                  = new TH1D("h_trd_ann_id","",100, -2., 2.);
    fh_trd_ann_id_after            = new TH1D("h_trd_ann_id_after","",100, -2., 2.);

    fh_tof_m2_mom                  = new TH2D("h_tof_m2_mom","",60,0,15.,50,0,1.5);
    fh_tof_m2_mom_after            = new TH2D("h_tof_m2_mom_after","",60,0,15.,50,0,1.5);

    if(fVerbose>0) cout<<"-I- CbmDileptonAssignPid::BookHistograms() : Histograms booked"<<endl;
}
//---------------------------------------------------------------------
void CbmDileptonAssignPid::SaveHistograms()
{
    fh_rich_ring_dist_mom->Write();
    fh_rich_ring_dist_mom_after->Write();
    fh_rich_selection_NN->Write();
    fh_rich_selection_NN_after->Write();
    fh_rich_selection_2D->Write();
    fh_rich_selection_2D_after->Write();
    fh_rich_ring_radial ->Write();
    fh_rich_ring_radial_after->Write();
    fh_rich_ring_xy ->Write();               
    fh_rich_ring_xy_after->Write();
    fh_rich_ring_hits_radius->Write();
    fh_rich_ring_hits_radius_after->Write();
    fh_rich_ring_radius_mom->Write();        
    fh_rich_ring_radius_mom_after->Write();  
                                   
    fh_trd_like_id->Write();                 
    fh_trd_like_id_after->Write();           
    fh_trd_wkn_id->Write();                  
    fh_trd_wkn_id_after->Write();            
    fh_trd_ann_id->Write();                  
    fh_trd_ann_id_after->Write();            
                                   
    fh_tof_m2_mom->Write();                  
    fh_tof_m2_mom_after->Write();           
                                                                                                                 
    if(fVerbose>0) cout<<"-I- CbmDileptonAssignPid::SaveHistograms() : Histograms saved"<<endl;
}

ClassImp(CbmDileptonAssignPid)
