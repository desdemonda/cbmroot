/***********************************************************************************
 *  $Id: CbmDileptonAssignMCid.cxx 5715 2013-12-20 12:24:47Z andrey.lebedev $
 *
 *  Class : CbmDileptonAssignMCid
 *  Descripton: This is a task, derived from FairTask. This works for the simulated
 *              data stream to assign MC information to the identified electron tracks.
 *              This also generates histograms for identification QA (efficiency,
 *              purity, pion supression etc.
 *              Input: Arrays of dileptontrackreal, global, sts, trd track, rich ring,
 *              tof hit and mc arrays.
 *              Output: Array of CbmDileptonTrackSim
 *
 *  Author: Supriya Das
 *  E-mail: S.Das@gsi.de
 ***********************************************************************************/
// CBMROOT includes
#include "CbmDileptonAssignMCid.h"

#include "CbmDileptonTrackSimCollection.h"
#include "CbmDileptonTrackReal.h"

#include "CbmGlobalTrack.h"
#include "CbmStsTrack.h"
#include "CbmTrackMatch.h"
#include "CbmRichRing.h"
#include "CbmTrackMatch.h"
#include "CbmMCTrack.h"

#include "FairTrackParam.h"
#include "FairTask.h"
#include "FairRootManager.h"

// ROOT includes
#include "TClonesArray.h"
#include "TFile.h"
#include "TH1.h"
#include "TMath.h"

#include <iostream>
#include <iomanip>

using std::cout;
using std::endl;
using std::setw;
using std::setprecision;
using std::right;

CbmDileptonAssignMCid::CbmDileptonAssignMCid() 
  : FairTask("DileptonAssignMCid"),
    fRootManager(NULL),
    fTrackSimColl(new CbmDileptonTrackSimCollection()),
    fArrayDileptonTrackReal(NULL),
    fArrayGlobalTrack(NULL),
    fArrayStsTrack(NULL),
    fArrayStsTrackMatch(NULL),
    fArrayRichRing(NULL),
    fArrayRichRingMatch(NULL),
    fArrayTrdTrack(NULL),
    fArrayTofHit(NULL),
    fArrayTofPoint(NULL),
    fArrayRichProjection(NULL),
    fArrayMCTrack(NULL),
    fFitter(),
    fPrimVertex(NULL),
    fCutChiPrimary(3.),
    fCutRadialDistance(130.),
    fCutStsHit(4),
    fVerbose(0),
    fOutFile(NULL),
    fOutFileName("pid.mc.hist.root"),
    fh_momentum_id_rich(NULL),
    fh_momentum_id_rich_trd(NULL),
    fh_momentum_id_rich_tof(NULL),
    fh_momentum_id_rich_trd_tof(NULL),
    fh_momentum_id_rich_prim(NULL),
    fh_momentum_id_rich_trd_prim(NULL),
    fh_momentum_id_rich_tof_prim(NULL),
    fh_momentum_id_rich_trd_tof_prim(NULL),
    fh_momentum_projected_e_prim(NULL),
    fh_momentum_projected_e(NULL),
    fh_momentum_projected_pi(NULL),
    fh_momentum_rich_true_e(NULL),
    fh_momentum_rich_trd_true_e(NULL),
    fh_momentum_rich_tof_true_e(NULL),
    fh_momentum_rich_trd_tof_true_e(NULL),
    fh_momentum_rich_true_e_prim(NULL),
    fh_momentum_rich_trd_true_e_prim(NULL),
    fh_momentum_rich_tof_true_e_prim(NULL),
    fh_momentum_rich_trd_tof_true_e_prim(NULL),
    fh_momentum_rich_true_pi(NULL),
    fh_momentum_rich_trd_true_pi(NULL),
    fh_momentum_rich_tof_true_pi(NULL),
    fh_momentum_rich_trd_tof_true_pi(NULL),
    fh_momentum_rich_true_p(NULL),
    fh_momentum_rich_trd_true_p(NULL),
    fh_momentum_rich_tof_true_p(NULL),
    fh_momentum_rich_trd_tof_true_p(NULL),
    fh_momentum_rich_false_e(NULL),
    fh_momentum_rich_false_pion(NULL),
    fh_momentum_rich_false_proton(NULL),
    fh_momentum_rich_false_others(NULL),
    fh_momentum_rich_trd_false_e(NULL),
    fh_momentum_rich_trd_false_pion(NULL),
    fh_momentum_rich_trd_false_proton(NULL),
    fh_momentum_rich_trd_false_others(NULL),
    fh_momentum_rich_tof_false_e(NULL),
    fh_momentum_rich_tof_false_pion(NULL),
    fh_momentum_rich_tof_false_proton(NULL),
    fh_momentum_rich_tof_false_others(NULL),
    fh_momentum_rich_trd_tof_false_e(NULL),
    fh_momentum_rich_trd_tof_false_pion(NULL),
    fh_momentum_rich_trd_tof_false_proton(NULL),
    fh_momentum_rich_trd_tof_false_others(NULL),
    fh_momentum_fake_rich(NULL),
    fh_momentum_fake_rich_trd(NULL),
    fh_momentum_fake_rich_tof(NULL),
    fh_momentum_fake_rich_trd_tof(NULL),
    fTimer(),
    fNEvents(0),
    fTime(0.)
{
}

CbmDileptonAssignMCid::CbmDileptonAssignMCid(Int_t iVerbose, TString fname, const char* name) 
  : FairTask(name,iVerbose),
    fRootManager(NULL),
    fTrackSimColl(new CbmDileptonTrackSimCollection()),
    fArrayDileptonTrackReal(NULL),
    fArrayGlobalTrack(NULL),
    fArrayStsTrack(NULL),
    fArrayStsTrackMatch(NULL),
    fArrayRichRing(NULL),
    fArrayRichRingMatch(NULL),
    fArrayTrdTrack(NULL),
    fArrayTofHit(NULL),
    fArrayTofPoint(NULL),
    fArrayRichProjection(NULL),
    fArrayMCTrack(NULL),
    fFitter(),
    fPrimVertex(NULL),
    fCutChiPrimary(3.),
    fCutRadialDistance(130.),
    fCutStsHit(4),
    fVerbose(iVerbose),
    fOutFile(NULL),
    fOutFileName(fname),
    fh_momentum_id_rich(NULL),
    fh_momentum_id_rich_trd(NULL),
    fh_momentum_id_rich_tof(NULL),
    fh_momentum_id_rich_trd_tof(NULL),
    fh_momentum_id_rich_prim(NULL),
    fh_momentum_id_rich_trd_prim(NULL),
    fh_momentum_id_rich_tof_prim(NULL),
    fh_momentum_id_rich_trd_tof_prim(NULL),
    fh_momentum_projected_e_prim(NULL),
    fh_momentum_projected_e(NULL),
    fh_momentum_projected_pi(NULL),
    fh_momentum_rich_true_e(NULL),
    fh_momentum_rich_trd_true_e(NULL),
    fh_momentum_rich_tof_true_e(NULL),
    fh_momentum_rich_trd_tof_true_e(NULL),
    fh_momentum_rich_true_e_prim(NULL),
    fh_momentum_rich_trd_true_e_prim(NULL),
    fh_momentum_rich_tof_true_e_prim(NULL),
    fh_momentum_rich_trd_tof_true_e_prim(NULL),
    fh_momentum_rich_true_pi(NULL),
    fh_momentum_rich_trd_true_pi(NULL),
    fh_momentum_rich_tof_true_pi(NULL),
    fh_momentum_rich_trd_tof_true_pi(NULL),
    fh_momentum_rich_true_p(NULL),
    fh_momentum_rich_trd_true_p(NULL),
    fh_momentum_rich_tof_true_p(NULL),
    fh_momentum_rich_trd_tof_true_p(NULL),
    fh_momentum_rich_false_e(NULL),
    fh_momentum_rich_false_pion(NULL),
    fh_momentum_rich_false_proton(NULL),
    fh_momentum_rich_false_others(NULL),
    fh_momentum_rich_trd_false_e(NULL),
    fh_momentum_rich_trd_false_pion(NULL),
    fh_momentum_rich_trd_false_proton(NULL),
    fh_momentum_rich_trd_false_others(NULL),
    fh_momentum_rich_tof_false_e(NULL),
    fh_momentum_rich_tof_false_pion(NULL),
    fh_momentum_rich_tof_false_proton(NULL),
    fh_momentum_rich_tof_false_others(NULL),
    fh_momentum_rich_trd_tof_false_e(NULL),
    fh_momentum_rich_trd_tof_false_pion(NULL),
    fh_momentum_rich_trd_tof_false_proton(NULL),
    fh_momentum_rich_trd_tof_false_others(NULL),
    fh_momentum_fake_rich(NULL),
    fh_momentum_fake_rich_trd(NULL),
    fh_momentum_fake_rich_tof(NULL),
    fh_momentum_fake_rich_trd_tof(NULL),
    fTimer(),
    fNEvents(0),
    fTime(0.)
{
}

CbmDileptonAssignMCid::~CbmDileptonAssignMCid(){

}

InitStatus CbmDileptonAssignMCid::Init(){

    cout<<"-I- CbmDileptonAssignMCid::Init: Initialization started"<<endl;

    // Get pointers to root manager
    fRootManager = FairRootManager::Instance();

    // Get GlobalTrack Array
    fArrayGlobalTrack = (TClonesArray*) fRootManager->GetObject("GlobalTrack");
    if (! fArrayGlobalTrack) {
        cout << "-E- CbmDileptonAssignMCid::Init: No GlobalTrack array!" << endl;
        return kFATAL;
    }

    // Get StsTrack Array
    fArrayStsTrack = (TClonesArray*) fRootManager->GetObject("StsTrack");
    if (! fArrayStsTrack) {
        cout << "-E- CbmDileptonAssignMCid::Init: No StsTrack array!" << endl;
        return kFATAL;
    }

    // Get StsTrackMatch Array
    fArrayStsTrackMatch = (TClonesArray*) fRootManager->GetObject("StsTrackMatch");
    if (! fArrayStsTrackMatch) {
        cout << "-E- CbmDileptonAssignMCid::Init: No StsTrackMatch array!" << endl;
        return kFATAL;
    }

    // Get RichRing Array
    fArrayRichRing = (TClonesArray*) fRootManager->GetObject("RichRing");
    if (! fArrayRichRing) {
	cout << "-E- CbmDileptonAssignMCid::Init: No RichRing array!" << endl;
	return kFATAL;
    }

    // Get RichRingMatch Array
    fArrayRichRingMatch = (TClonesArray*) fRootManager->GetObject("RichRingMatch");
    if (! fArrayRichRingMatch) {
        cout << "-E- CbmDileptonAssignMCid::Init: No RichRingMatch array!" << endl;
        return kFATAL;
    }

    // Get TrdTrack Array
    fArrayTrdTrack = (TClonesArray*) fRootManager->GetObject("TrdTrack");
    if (! fArrayTrdTrack) {
        cout << "-E- CbmDileptonAssignMCid::Init: No TrdTrack array!" << endl;
        return kFATAL;
    }

    // Get TofHit Array
    fArrayTofHit = (TClonesArray*) fRootManager->GetObject("TofHit");
    if (! fArrayTofHit) {
        cout << "-E- CbmDileptonAssignMCid::Init: No TofHit array!" << endl;
        return kFATAL;
    }

    // Get TofPoint Array
    fArrayTofPoint = (TClonesArray*) fRootManager->GetObject("TofPoint");
    if (! fArrayTofPoint) {
        cout << "-E- CbmDileptonAssignMCid::Init: No TofPoint array!" << endl;
        return kFATAL;
    }

    // Get RichRingProj Array
    fArrayRichProjection = (TClonesArray*) fRootManager->GetObject("RichProjection");
    if (!fArrayRichProjection  ) {
	cout << "-E- CbmDileptonAssignMCid::Init: No RichProjection array!" << endl;
	return kFATAL;
    }

    // Get Primary Vertex
    fPrimVertex = (CbmVertex*) fRootManager->GetObject("PrimaryVertex");
    if(!fPrimVertex){
        cout<<"-E- CbmDileptonAssignMCid::Init No Primary Vertex!" <<endl;
        return kFATAL;
    }

    // Get MCTrack Array
    fArrayMCTrack = (TClonesArray*) fRootManager->GetObject("MCTrack");
    if (! fArrayMCTrack) {
        cout << "-E- CbmDileptonAssignMCid::Init: No MCTrack array!" << endl;
        return kFATAL;
    }

    // Get DileptonTrackReal Array
    fArrayDileptonTrackReal = (TClonesArray*) fRootManager->GetObject("DileptonTrackReal");
    if (! fArrayDileptonTrackReal ) {
        cout << "-E- CbmDileptonAssignMCid::Init: DileptonTrackReal No  array!" << endl;
        return kFATAL;
    }

    fRootManager->Register("DileptonTrackSim","Dilepton", fTrackSimColl->DileptonTracksSim(),kTRUE);

    // initialize KF-Fitter
    fFitter.Init();

    BookHistograms();

    cout<<"-I- CbmDileptonAssignMCid::Init: Initialization complete"<<endl;

    return kSUCCESS;
}

void  CbmDileptonAssignMCid::Exec(Option_t* option){

    fTimer.Start();

    // Clear and Delete track array to free the memory
    fTrackSimColl->deleteTrack();

    Int_t nTrackReal =  fArrayDileptonTrackReal->GetEntriesFast();

    if (fVerbose>0) cout<<" -I-  CbmDileptonAssignMCid::Exec() : real tracks : "<<nTrackReal<<endl;


    for(Int_t iTrackReal=0; iTrackReal<nTrackReal; iTrackReal++){

	Int_t MCPdg        = -99;
	Int_t MCMotherId   = -99;
	Int_t MCMotherPdg  = -99;
	Bool_t FakeTrack   = false;
	Bool_t FakeRing    = false;

        //-----------------  Assign MC info to the identified tracks  --------------

	CbmDileptonTrackReal*  trackReal = (CbmDileptonTrackReal*)fArrayDileptonTrackReal->At(iTrackReal);
	if(!trackReal) continue;

	Int_t globalTrackIndex = trackReal->GetGlobalIndex();

        if (fVerbose>0)  cout<<" -I-  CbmDileptonAssignMCid::Exec() : real track, global track: "<< iTrackReal<<" "<<globalTrackIndex<<endl;

	//Get the global track
	CbmGlobalTrack* gTrack=(CbmGlobalTrack*)fArrayGlobalTrack->At(globalTrackIndex);

	Int_t stsTrackIndex = gTrack->GetStsTrackIndex();
	Int_t richRingIndex = gTrack->GetRichRingIndex();
       // Int_t trdTrackIndex = gTrack->GetTrdTrackIndex();
       // Int_t tofHitIndex   = gTrack->GetTofHitIndex();

	if(stsTrackIndex == -1) continue;


        CbmStsTrack* stsTrack      = NULL;
        CbmTrackMatch* stsMatch = NULL;
        CbmMCTrack* mcTrack        = NULL;

	Int_t stsMCid = -99;
	TVector3 mom;

	Int_t richMCid = -99;
	CbmRichRing *richRing =  NULL;
	CbmTrackMatch *richMatch = NULL;


	stsTrack = (CbmStsTrack*)fArrayStsTrack->At(stsTrackIndex);
	if (!stsTrack) goto add;

	stsMatch = (CbmTrackMatch*)fArrayStsTrackMatch->At(stsTrackIndex);
	if (!stsMatch) goto add;

	stsMCid = stsMatch->GetMCTrackId();
        if (stsMCid == -1) goto add;

        if(((Double_t)(stsMatch->GetNofTrueHits()))/((Double_t)(stsTrack->GetNofHits()))< 0.6) FakeTrack = true;

	mcTrack = (CbmMCTrack*)fArrayMCTrack->At(stsMCid);
	if(!mcTrack) goto add;

        mcTrack->GetMomentum(mom);
	MCPdg      = mcTrack->GetPdgCode();
	MCMotherId = mcTrack->GetMotherId();

	if(MCMotherId >= 0) {
	    CbmMCTrack *mother = (CbmMCTrack*) fArrayMCTrack->At(MCMotherId);
	    if(mother) MCMotherPdg = mother->GetPdgCode();
	}

	if(richRingIndex == -1) goto add; // if there is no rich ring, the track is added to the collection here

	richRing = (CbmRichRing*)fArrayRichRing->At(richRingIndex);
	if(!richRing) goto add;

	richMatch = (CbmTrackMatch*)fArrayRichRingMatch->At(richRingIndex);
	if (!richMatch) goto add;
	richMCid = richMatch->GetMCTrackId();
	if(richMCid == -1) goto add;

	if (fVerbose>0)  cout<<" -I-  CbmDileptonAssignMCid::Exec() : stsMCid, richMCid : "<<stsMCid << " "<<richMCid <<endl;

        if((Double_t)(richMatch->GetNofTrueHits())/(Double_t)(richRing->GetNofHits())< 0.6) FakeRing = true;

        if (fVerbose>0)  cout<<" -I-  CbmDileptonAssignMCid::Exec() : "<<FakeTrack<<" "<< FakeRing <<" "<<MCPdg<<" "<<MCMotherId <<" "<<MCMotherPdg<<endl;

    add:
	//Add This track to Collection
	fTrackSimColl->addTrack(mom,FakeTrack,FakeRing,MCPdg,MCMotherId,MCMotherPdg);

	//------------------------------------------------------------------------

	//----------------------------- Fill histograms --------------------------

	Bool_t isRich = trackReal->GetRich();
	Bool_t isTrd  = trackReal->GetTrd();
	Bool_t isTof  = trackReal->GetTof();
	Double_t chi = trackReal->GetChiPrimary();
	TVector3 momentum = trackReal->GetMomentum();


        // Get identified particles


	if (chi >3.) continue;

	if(isRich) {
	    if(-1 == MCMotherId) fh_momentum_id_rich_prim->Fill(momentum.Mag());
	    else fh_momentum_id_rich->Fill(momentum.Mag());
	}
	if(isRich && isTrd) {
             if(-1 == MCMotherId) fh_momentum_id_rich_trd_prim->Fill(momentum.Mag());
	     else fh_momentum_id_rich_trd->Fill(momentum.Mag());
	}
        if(isRich && isTof) {
             if(-1 == MCMotherId) fh_momentum_id_rich_tof_prim->Fill(momentum.Mag());
	     else fh_momentum_id_rich_tof->Fill(momentum.Mag());
	}
	if(isRich && isTrd && isTof) {
	    if(-1 == MCMotherId)  fh_momentum_id_rich_trd_tof_prim->Fill(momentum.Mag());
	    else fh_momentum_id_rich_trd_tof->Fill(momentum.Mag());
	}

        // get the fake rings
	if (FakeRing){

	    if(isRich)fh_momentum_fake_rich->Fill(momentum.Mag());
	    if(isRich && isTrd)fh_momentum_fake_rich_trd->Fill(momentum.Mag());
	    if(isRich && isTof)fh_momentum_fake_rich_tof->Fill(momentum.Mag());
	    if(isRich && isTrd && isTof)fh_momentum_fake_rich_trd_tof->Fill(momentum.Mag());
	}  // fake

	// if not fake then get the electron and pions
	if(!FakeRing){

	    // for true matches

	    if (stsMCid == richMCid){

		if(isRich){
		    if (11 == TMath::Abs(MCPdg)){
			if(-1 == MCMotherId) fh_momentum_rich_true_e_prim->Fill(momentum.Mag());
			else fh_momentum_rich_true_e->Fill(momentum.Mag());
		    }
		    else if(211 == TMath::Abs(MCPdg)) fh_momentum_rich_true_pi->Fill(momentum.Mag());
		    else if(2212 == TMath::Abs(MCPdg)) fh_momentum_rich_true_p->Fill(momentum.Mag());
		}

		if(isRich && isTrd){
		    if (11 == TMath::Abs(MCPdg)){
			if ( -1 == MCMotherId )fh_momentum_rich_trd_true_e_prim->Fill(momentum.Mag());
			else fh_momentum_rich_trd_true_e->Fill(momentum.Mag());
		    }
		    else if(211 == TMath::Abs(MCPdg)) fh_momentum_rich_trd_true_pi->Fill(momentum.Mag());
		    else if(2212 == TMath::Abs(MCPdg)) fh_momentum_rich_trd_true_p->Fill(momentum.Mag());
		}

		if(isRich && isTof){
		    if (11 == TMath::Abs(MCPdg)){
			if ( -1 == MCMotherId )fh_momentum_rich_tof_true_e_prim->Fill(momentum.Mag());
			else fh_momentum_rich_tof_true_e->Fill(momentum.Mag());
		    }
		    else if(211 == TMath::Abs(MCPdg)) fh_momentum_rich_tof_true_pi->Fill(momentum.Mag());
		    else if(2212 == TMath::Abs(MCPdg)) fh_momentum_rich_tof_true_p->Fill(momentum.Mag());

		}

		if(isRich && isTrd && isTof){
		    if (11 == TMath::Abs(MCPdg)) {
			if(-1 == MCMotherId) fh_momentum_rich_trd_tof_true_e_prim->Fill(momentum.Mag());
			else fh_momentum_rich_trd_tof_true_e->Fill(momentum.Mag());
		    }
		    else if(211 == TMath::Abs(MCPdg)) fh_momentum_rich_trd_tof_true_pi->Fill(momentum.Mag());
		    else if(2212 == TMath::Abs(MCPdg)) fh_momentum_rich_trd_tof_true_p->Fill(momentum.Mag());
		}

	    }// true match

	    // for false matches

	    else {

		if(isRich) {
		    if(11 ==  TMath::Abs(MCPdg))fh_momentum_rich_false_e->Fill(momentum.Mag());
		    else if(211 ==  TMath::Abs(MCPdg))fh_momentum_rich_false_pion->Fill(momentum.Mag());
		    else if(2212 ==  TMath::Abs(MCPdg))fh_momentum_rich_false_proton->Fill(momentum.Mag());
		    else fh_momentum_rich_false_others->Fill(momentum.Mag());
		}

		if(isRich && isTrd) {
		    if(11 ==  TMath::Abs(MCPdg))fh_momentum_rich_trd_false_e->Fill(momentum.Mag());
		    else if(211 ==  TMath::Abs(MCPdg))fh_momentum_rich_trd_false_pion->Fill(momentum.Mag());
		    else if(2212 ==  TMath::Abs(MCPdg))fh_momentum_rich_trd_false_proton->Fill(momentum.Mag());
		    else fh_momentum_rich_trd_false_others->Fill(momentum.Mag());
		}

		if(isRich && isTof) {
		    if(11 ==  TMath::Abs(MCPdg))fh_momentum_rich_tof_false_e->Fill(momentum.Mag());
		    else if(211 ==  TMath::Abs(MCPdg))fh_momentum_rich_tof_false_pion->Fill(momentum.Mag());
		    else if(2212 ==  TMath::Abs(MCPdg))fh_momentum_rich_tof_false_proton->Fill(momentum.Mag());
		    else fh_momentum_rich_tof_false_others->Fill(momentum.Mag());
		}

		if(isRich && isTrd && isTof) {
		    if(11 ==  TMath::Abs(MCPdg))fh_momentum_rich_trd_tof_false_e->Fill(momentum.Mag());
		    else if(211 ==  TMath::Abs(MCPdg))fh_momentum_rich_trd_tof_false_pion->Fill(momentum.Mag());
		    else if(2212 ==  TMath::Abs(MCPdg))fh_momentum_rich_trd_tof_false_proton->Fill(momentum.Mag());
		    else fh_momentum_rich_trd_tof_false_others->Fill(momentum.Mag());
		}
	    } // false match

	} // not fake

	//----------------------  Evaluation of identification ends  ---------------------

    } // Loop over identified tracks

    // -------- Calculate total accepted tracks within RICH for efficiency/purity etc. -----------

	for (Int_t iGlobal=0; iGlobal<fArrayGlobalTrack->GetEntriesFast(); iGlobal++){


	//Get Global Track
	CbmGlobalTrack* fGlobalTrack= (CbmGlobalTrack*)fArrayGlobalTrack->At(iGlobal);

	//Get STS information
	Int_t stsTrackIndex = fGlobalTrack->GetStsTrackIndex();
	if (stsTrackIndex == -1) continue;

        CbmStsTrack* stsTrack = (CbmStsTrack*)fArrayStsTrack->At(stsTrackIndex);
	if (!stsTrack) continue;

        CbmTrackMatch *stsMatch = (CbmTrackMatch*)fArrayStsTrackMatch->At(stsTrackIndex);
	if (!stsMatch) continue;
	Int_t stsMCid = stsMatch->GetMCTrackId();
	if (stsMCid == -1)continue;

	CbmMCTrack* mcTrack = (CbmMCTrack*)fArrayMCTrack->At(stsMCid);
	if(!mcTrack) continue;

	Int_t MCPdg = mcTrack->GetPdgCode();
        Int_t MCMotherId = mcTrack->GetMotherId();

        Int_t nStsFound = stsTrack->GetNofHits() + stsTrack->GetNofMvdHits();

	Double_t chiPrimary = fFitter.GetChiToVertex(stsTrack);

	// Fit tracks to the primary vertex
	FairTrackParam VtxTrack;
	fFitter.FitToVertex(stsTrack, fPrimVertex, &VtxTrack);

	TVector3 position;
	TVector3 momentum;

	VtxTrack.Position(position);
	VtxTrack.Momentum(momentum);

	if (chiPrimary > fCutChiPrimary)continue;
	if (nStsFound < fCutStsHit) continue;

	FairTrackParam* richTrack = (FairTrackParam*)fArrayRichProjection->At(iGlobal);

	Double_t TcentreX = richTrack->GetX();
	Double_t TcentreY = richTrack->GetY();

        Double_t RadialPosition = TMath::Sqrt(TcentreX*TcentreX+TcentreY*TcentreY);

       if(RadialPosition<fCutRadialDistance)continue;

       if(11 == TMath::Abs(MCPdg)){
	   if( MCMotherId == -1) fh_momentum_projected_e_prim->Fill(momentum.Mag());
	   else  fh_momentum_projected_e->Fill(momentum.Mag());
       }
       else if (211 == TMath::Abs(MCPdg)) fh_momentum_projected_pi->Fill(momentum.Mag());

    } // Loop over Global tracks

    //-----------------  calculation of accepted electron / pion ends  ---------------------


    fTimer.Stop();
    fTime += fTimer.RealTime();

    fNEvents++;
}
//--------------------------------------------------------------------------------
void CbmDileptonAssignMCid::Finish()
{

    fOutFile = new TFile(fOutFileName,"RECREATE");
    fOutFile->cd();

    SaveHistograms();
    fOutFile->Close();

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
//-------------------------------------------------------------------------------
void CbmDileptonAssignMCid::FinishTask(){
    fTrackSimColl->clearTrack();
}
//-------------------------------------------------------------------------------
void CbmDileptonAssignMCid::SetCuts(Double_t cutChi, Double_t cutRadial, Int_t cutSts){

    fCutChiPrimary = cutChi;
    fCutRadialDistance = cutRadial;
    fCutStsHit = cutSts;

}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
void CbmDileptonAssignMCid::BookHistograms()
{

    fh_momentum_id_rich               = new TH1D("h_momentum_id_rich","",1000,0.,15.);
    fh_momentum_id_rich_trd           = new TH1D("h_momentum_id_rich_trd","",1000,0.,15.);
    fh_momentum_id_rich_tof           = new TH1D("h_momentum_id_rich_tof","",1000,0.,15.);
    fh_momentum_id_rich_trd_tof       = new TH1D("h_momentum_id_rich_trd_tof","",1000,0.,15.);

    fh_momentum_id_rich_prim          = new TH1D("h_momentum_id_rich_prim","",1000,0.,15.);
    fh_momentum_id_rich_trd_prim      = new TH1D("h_momentum_id_rich_trd_prim","",1000,0.,15.);
    fh_momentum_id_rich_tof_prim      = new TH1D("h_momentum_id_rich_tof_prim","",1000,0.,15.);
    fh_momentum_id_rich_trd_tof_prim  = new TH1D("h_momentum_id_rich_trd_tof_prim","",1000,0.,15.);


    fh_momentum_projected_e_prim      = new TH1D("h_momentum_projected_e_prim","",1000,0.,15.);
    fh_momentum_projected_e           = new TH1D("h_momentum_projected_e","",1000,0.,15.);
    fh_momentum_projected_pi          = new TH1D("h_momentum_projected_pi","",1000,0.,15.);

    fh_momentum_rich_true_e_prim         = new TH1D("h_momentum_rich_true_e_prim","",1000,0.,15.);
    fh_momentum_rich_trd_true_e_prim     = new TH1D("h_momentum_rich_trd_true_e_prim","",1000,0.,15.);
    fh_momentum_rich_tof_true_e_prim     = new TH1D("h_momentum_rich_tof_true_e_prim","",1000,0.,15.);
    fh_momentum_rich_trd_tof_true_e_prim = new TH1D("h_momentum_rich_trd_tof_true_e_prim","",1000,0.,15.);

    fh_momentum_rich_true_e           = new TH1D("h_momentum_rich_true_e","",1000,0.,15.);
    fh_momentum_rich_trd_true_e       = new TH1D("h_momentum_rich_trd_true_e","",1000,0.,15.);
    fh_momentum_rich_tof_true_e       = new TH1D("h_momentum_rich_tof_true_e","",1000,0.,15.);
    fh_momentum_rich_trd_tof_true_e   = new TH1D("h_momentum_rich_trd_tof_true_e","",1000,0.,15.);


    fh_momentum_rich_true_pi          = new TH1D("h_momentum_rich_true_pi","",1000,0.,15.);
    fh_momentum_rich_trd_true_pi      = new TH1D("h_momentum_rich_trd_true_pi","",1000,0.,15.);
    fh_momentum_rich_tof_true_pi      = new TH1D("h_momentum_rich_tof_true_pi","",1000,0.,15.);
    fh_momentum_rich_trd_tof_true_pi  = new TH1D("h_momentum_rich_trd_tof_true_pi","",1000,0.,15.);

    fh_momentum_rich_true_p           = new TH1D("h_momentum_rich_true_p","",1000,0.,15.);
    fh_momentum_rich_trd_true_p       = new TH1D("h_momentum_rich_trd_true_p","",1000,0.,15.);
    fh_momentum_rich_tof_true_p       = new TH1D("h_momentum_rich_tof_true_p","",1000,0.,15.);
    fh_momentum_rich_trd_tof_true_p   = new TH1D("h_momentum_rich_trd_tof_true_p","",1000,0.,15.);

    fh_momentum_rich_false_e          = new TH1D("h_momentum_rich_false_e","",1000,0.,15.);
    fh_momentum_rich_false_pion       = new TH1D("h_momentum_rich_false_pion","",1000,0.,15.);
    fh_momentum_rich_false_proton     = new TH1D("h_momentum_rich_false_proton","",1000,0.,15.);
    fh_momentum_rich_false_others     = new TH1D("h_momentum_rich_false_others","",1000,0.,15.);

    fh_momentum_rich_trd_false_e      = new TH1D("h_momentum_rich_trd_false_e","",1000,0.,15.);
    fh_momentum_rich_trd_false_pion   = new TH1D("h_momentum_rich_trd_false_pion","",1000,0.,15.);
    fh_momentum_rich_trd_false_proton = new TH1D("h_momentum_rich_trd_false_proton","",1000,0.,15.);
    fh_momentum_rich_trd_false_others = new TH1D("h_momentum_rich_trd_false_others","",1000,0.,15.);

    fh_momentum_rich_tof_false_e      = new TH1D("h_momentum_rich_tof_false_e","",1000,0.,15.);
    fh_momentum_rich_tof_false_pion   = new TH1D("h_momentum_rich_tof_false_pion","",1000,0.,15.);
    fh_momentum_rich_tof_false_proton = new TH1D("h_momentum_rich_tof_false_proton","",1000,0.,15.);
    fh_momentum_rich_tof_false_others = new TH1D("h_momentum_rich_tof_false_others","",1000,0.,15.);

    fh_momentum_rich_trd_tof_false_e       = new TH1D("h_momentum_rich_trd_tof_false_e","",1000,0.,15.);
    fh_momentum_rich_trd_tof_false_pion    = new TH1D("h_momentum_rich_trd_tof_false_pion","",1000,0.,15.);
    fh_momentum_rich_trd_tof_false_proton  = new TH1D("h_momentum_rich_trd_tof_false_proton","",1000,0.,15.);
    fh_momentum_rich_trd_tof_false_others  = new TH1D("h_momentum_rich_trd_tof_false_others","",1000,0.,15.);

    fh_momentum_fake_rich             = new TH1D("h_momentum_fake_rich","",1000,0.,15.);
    fh_momentum_fake_rich_trd         = new TH1D("h_momentum_fake_rich_trd","",1000,0.,15.);
    fh_momentum_fake_rich_tof         = new TH1D("h_momentum_fake_rich_tof","",1000,0.,15.);
    fh_momentum_fake_rich_trd_tof     = new TH1D("h_momentum_fake_rich_trd_tof","",1000,0.,15.);

    if (fVerbose>0)  cout<<" -I-  CbmDileptonAssignMCid::BookHistograms() : Histograms booked "<<endl;
}
//------------------------------------------------------------------------------
void CbmDileptonAssignMCid::SaveHistograms()
{
    fOutFile->cd();

    fh_momentum_id_rich->Write();
    fh_momentum_id_rich_trd->Write();
    fh_momentum_id_rich_tof->Write();
    fh_momentum_id_rich_trd_tof->Write();

    fh_momentum_id_rich_prim->Write();
    fh_momentum_id_rich_trd_prim->Write();
    fh_momentum_id_rich_tof_prim->Write();
    fh_momentum_id_rich_trd_tof_prim->Write();

    fh_momentum_projected_e_prim->Write();
    fh_momentum_projected_e->Write();
    fh_momentum_projected_pi->Write();


    fh_momentum_rich_true_e->Write();
    fh_momentum_rich_trd_true_e->Write();
    fh_momentum_rich_tof_true_e->Write();
    fh_momentum_rich_trd_tof_true_e->Write();

    fh_momentum_rich_true_e_prim->Write();
    fh_momentum_rich_trd_true_e_prim->Write();
    fh_momentum_rich_tof_true_e_prim->Write();
    fh_momentum_rich_trd_tof_true_e_prim->Write();


    fh_momentum_rich_true_pi->Write();
    fh_momentum_rich_trd_true_pi->Write();
    fh_momentum_rich_tof_true_pi->Write();
    fh_momentum_rich_trd_tof_true_pi->Write();

    fh_momentum_rich_true_p->Write();
    fh_momentum_rich_trd_true_p->Write();
    fh_momentum_rich_tof_true_p->Write();
    fh_momentum_rich_trd_tof_true_p->Write();

    fh_momentum_rich_false_e->Write();
    fh_momentum_rich_false_pion->Write();
    fh_momentum_rich_false_proton->Write();;
    fh_momentum_rich_false_others->Write();

    fh_momentum_rich_trd_false_e->Write();
    fh_momentum_rich_trd_false_pion->Write();
    fh_momentum_rich_trd_false_proton->Write();
    fh_momentum_rich_trd_false_others->Write();

    fh_momentum_rich_tof_false_e->Write();
    fh_momentum_rich_tof_false_pion->Write();
    fh_momentum_rich_tof_false_proton->Write();
    fh_momentum_rich_tof_false_others->Write();

    fh_momentum_rich_trd_tof_false_e->Write();
    fh_momentum_rich_trd_tof_false_pion->Write();
    fh_momentum_rich_trd_tof_false_proton->Write();;
    fh_momentum_rich_trd_tof_false_others->Write();

    fh_momentum_fake_rich->Write();
    fh_momentum_fake_rich_trd->Write();
    fh_momentum_fake_rich_tof->Write();
    fh_momentum_fake_rich_trd_tof->Write();

    if (fVerbose>0)  cout<<" -I-  CbmDileptonAssignMCid::SaveHistograms() : Histograms saved "<<endl;

}
