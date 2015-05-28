
#include "CbmAnaJpsiTask.h"
#include <iostream>
#include "TCanvas.h"
#include "CbmMCTrack.h"
#include "CbmStsTrack.h"
#include "CbmRichRing.h"
#include "CbmTrdTrack.h"
#include "CbmDrawHist.h"
#include "CbmStsHit.h"
#include "CbmRichHit.h"
#include "CbmRichPoint.h"
#include "CbmTrdHit.h"
#include "CbmTofHit.h"
#include "CbmHistManager.h"
#include "TFile.h"
#include "TMath.h"
#include "CbmGlobalTrack.h"
#include "CbmAnaJpsiCandidate.h"
#include "CbmAnaJpsiUtils.h"
#include "CbmAnaJpsiHist.h"
#include "CbmTrackMatchNew.h"
#include "CbmAnaJpsiKinematicParams.h"
#include "CbmAnaJpsiCuts.h"


using namespace std;

CbmAnaJpsiTask::CbmAnaJpsiTask()
    : FairTask("CbmAnaJpsiTask"),
	  fEventNum(0),
	  fMcTracks(NULL),
	  fStsPoints(NULL),
	  fStsHits(NULL),
	  fStsTracks(NULL),
	  fStsTrackMatches(NULL),
	  fRichPoints(NULL),
	  fRichHits(NULL),
	  fRichRings(NULL),
	  fRichRingMatches(NULL),
	  fTrdPoints(NULL),
	  fTrdHits(NULL),
	  fTrdTracks(NULL),
	  fTrdTrackMatches(NULL),
	  fTofPoints(NULL),
	  fTofHits(NULL),
	  fGlobalTracks(NULL),
	  fPrimVertex(NULL),
	  fKFVertex(),
	  fCandidates(),
	  fElIdAnn(NULL),
	  fHM(NULL),
	  fCuts()
{
}

CbmAnaJpsiTask::~CbmAnaJpsiTask()
{

}

InitStatus CbmAnaJpsiTask::Init()
{
   //cout << "CbmRichUrqmdTest::Init"<<endl;
   FairRootManager* ioman = FairRootManager::Instance();
   if (NULL == ioman) { Fatal("CbmAnaJpsiTask::Init","RootManager not instantised!"); }
   
   fMcTracks = (TClonesArray*) ioman->GetObject("MCTrack");
   if ( NULL == fMcTracks) {Fatal("CbmAnaJpsiTask::Init","No MCtrack Array! "); }
   
   fStsPoints = (TClonesArray*) ioman->GetObject("StsPoint");
   if ( NULL == fStsPoints) {Fatal("CbmAnaJpsiTask::Init","No StsPoint Array! "); }
   
   fRichPoints = (TClonesArray*) ioman->GetObject("RichPoint");
   if ( NULL == fRichPoints) {Fatal("CbmAnaJpsiTask::Init","No RichPoint Array! "); }
   
   fTrdPoints = (TClonesArray*) ioman->GetObject("TrdPoint");
   if ( NULL == fTrdPoints) {Fatal("CbmAnaJpsiTask::Init","No TrdPoint Array! "); }
   
   fTofPoints = (TClonesArray*) ioman->GetObject("TofPoint");
   if ( NULL == fTofPoints) {Fatal("CbmAnaJpsiTask::Init","No TofPoint Array! "); }
   
   fStsHits = (TClonesArray*) ioman->GetObject("StsHit");
   if ( NULL == fStsHits) {Fatal("CbmAnaJpsiTask::Init","No StsHit Array! "); }
   
   fRichHits = (TClonesArray*) ioman->GetObject("RichHit");
   if ( NULL == fRichHits) {Fatal("CbmAnaJpsiTask::Init","No RichHits Array! ");}
   
   fTrdHits = (TClonesArray*) ioman->GetObject("TrdHit");
   if ( NULL == fTrdHits) {Fatal("CbmAnaJpsiTask::Init","No TrdHits Array! ");}
   
   fTofHits = (TClonesArray*) ioman->GetObject("TofHit");
   if ( NULL == fTofHits) {Fatal("CbmAnaJpsiTask::Init","No TofHits Array! ");}
   
	fTrdTrackMatches = (TClonesArray*) ioman->GetObject("TrdTrackMatch");
	if (NULL == fTrdTrackMatches) { Fatal("CbmAnaDielectronTask::Init","No TrdTrackMatch array!"); }

   fStsTracks = (TClonesArray*) ioman->GetObject("StsTrack");
   if ( NULL == fStsTracks) {Fatal("CbmAnaJpsiTask::Init","No StsTracks Array! ");}
   
   fRichRings = (TClonesArray*) ioman->GetObject("RichRing");
   if ( NULL == fRichRings) {Fatal("CbmAnaJpsiTask::Init","No RichRings Array! ");}
   
   fRichRingMatches = (TClonesArray*) ioman->GetObject("RichRingMatch");
   if (NULL == fRichRingMatches) { Fatal("CbmAnaJpsiTask::Init","No RichRingMatch array!"); }

   fTrdTracks = (TClonesArray*) ioman->GetObject("TrdTrack");
   if ( NULL == fTrdTracks ) {Fatal("CbmAnaJpsiTask::Init","No TrdTracks Array!");}
   
   fGlobalTracks = (TClonesArray*) ioman->GetObject("GlobalTrack");
   if ( NULL == fGlobalTracks ) {Fatal("CbmAnaJpsiTask::Init","No GlobalTracks Array!");}

   fPrimVertex = (CbmVertex*) ioman->GetObject("PrimaryVertex");
   if (NULL == fPrimVertex) { Fatal("CbmAnaJpsiTask::Init","No PrimaryVertex array!"); }

   fStsTrackMatches = (TClonesArray*) ioman->GetObject("StsTrackMatch");
   if ( NULL == fStsTrackMatches ) {Fatal("CbmAnaJpsiTask::Init","No StsTrackMatches Array!");}

   InitHist();

   if (fCuts.fUseRichAnn){
      fElIdAnn = new CbmRichElectronIdAnn();
      fElIdAnn->Init();
   }

   return kSUCCESS;
}

void CbmAnaJpsiTask::CreateSourceTypesH1(
      const string& name,
      const string& axisX,
      const string& axisY,
      double nBins,
      double min,
      double max)
{
   for (Int_t i = 0; i < CbmAnaJpsiHist::fNofSourceTypes; i++){
      string hname = name + "_"+ CbmAnaJpsiHist::fSourceTypes[i];
      fHM->Create1<TH1D>(hname, hname+";"+axisX+";"+axisY, nBins, min, max);
   }
}

void CbmAnaJpsiTask::CreateSourceTypesH2(
      const string& name,
      const string& axisX,
      const string& axisY,
      const string& axisZ,
      double nBinsX,
      double minX,
      double maxX,
      double nBinsY,
      double minY,
      double maxY)
{
   string hname = "";
   for (Int_t i = 0; i < CbmAnaJpsiHist::fNofSourceTypes; i++){
      hname = name + "_"+ CbmAnaJpsiHist::fSourceTypes[i];
      fHM->Create2<TH2D>(hname, hname+";"+axisX+";"+axisY+";"+axisZ, nBinsX, minX, maxX, nBinsY, minY, maxY);
   }
}

void CbmAnaJpsiTask::InitHist()
{
	fHM = new CbmHistManager();

   //RICH PMT plane XY
   CreateSourceTypesH2("fhRichPmtXY","X [cm]","Y [cm]","Yield", 220, -110, 110, 400, -200, 200);

   //distributions of ID and analysis cuts
   CreateSourceTypesH1("fhChi2PrimEl","#chi^{2}_{prim}", "Yield", 200, 0., 20.);
   CreateSourceTypesH1("fhMomEl","P [GeV/c]", "Yield", 160, 0., 16.);
   CreateSourceTypesH1("fhChi2StsEl","#chi^{2}_{STS}", "Yield", 80, 0., 8.);
   CreateSourceTypesH1("fhRapidityEl","Rapidity", "Yield", 40, 0., 4.);
   CreateSourceTypesH1("fhPtEl","P_{t} [GeV/c]", "Yield", 40, 0., 4.);
   CreateSourceTypesH1("fhRichAnn","RichAnn", "Yield", 20, 0., 2.);
   CreateSourceTypesH1("fhTrdAnn","TrdAnn", "Yield", 20, 0., 2.);
   CreateSourceTypesH2("fhTofM2","m^{2} [(GeV/c^{2})^{2}]","p [GeV/c]","Yield", 40, 0., 4., 280, -0.2, 1.2);

   //e+/- Mc
   fHM->Create2<TH2D>("fhMcEpmRapidityPt","fhMcEpmRapidityPt;P_{t} [GeV/c];y;Entries",40,0,4,30,0,3);
   fHM->Create1<TH1D>("fhMcEpmMinv","fhMcEpmMinv;m_{inv} [GeV/c^{2}];Yield",500,0,5); //invariant mass

   //e+/- Accepted
   fHM->Create2<TH2D>("fhAccEpmRapidityPt","fhAccEpmRapidityPt;P_{t} [GeV/c];y;Entries",40,0,4,30,0,3);
   fHM->Create1<TH1D>("fhAccEpmMinv","fhAccEpmMinv;m_{inv} [GeV/c^{2}];Yield",500,0,5);//invariant mass
   fHM->Create1<TH1D>("fhAccEpmMomentum","fhAccEpmMomentum;P [GeV/c];Yield",200,0,20);
   fHM->Create1<TH1D>("fhAccEpmRapidity","fhAccEpmRapidity;y;Yield",10,0,10);
   fHM->Create1<TH1D>("fhAccEpmPt","fhAccEpmPt;P_{t} [GeV/c];Yield",40,0,4);

   //e+/- Candidate
   fHM->Create2<TH2D>("fhCandMcEpmPtY","fhCandMcEpmPtY;P_{t} [GeV/c];y;Entries",40,0,4,30,0,3);
   fHM->Create1<TH1D>("fhCandEpmMinv","fhCandEpmMinv;m_{inv} [GeV/c^{2}];Yield",500,0,5);// reconstructed invariant mass

   //e+/- Candidate with Chi2primCut
   fHM->Create2<TH2D>("fhCandMcEpmPtYChi2PrimCut","fhCandMcEpmPtYChi2PrimCut;P_{t} [GeV/c];y;Entries",40,0,4,30,0,3);
   fHM->Create1<TH1D>("fhCandEpmMinvChi2PrimCut","fhCandEpmMinvChi2PrimCut;m_{inv} [GeV/c^{2}];Yield",500,0,5);// reconstructed invariant mass
   fHM->Create1<TH1D>("fhCandEpmMomentumChi2PrimCut","fhCandEpmMomentumChi2PrimCut;P [GeV/c];Yield",200,0,20);
   fHM->Create1<TH1D>("fhCandMcEpmRapidityChi2PrimCut","fhCandMcEpmRapidityChi2PrimCut;y;Yield",10,0,10);
   fHM->Create1<TH1D>("fhCandMcEpmPtChi2PrimCut","fhCandMcEpmPtChi2PrimCut;P_{t} [GeV/c];Yield",40,0,4);

   //e+/- Candidate with Chi2primCut Reconstructed
   fHM->Create2<TH2D>("fhRecoCandEpmPtYChi2PrimCut","fhRecoCandMcEpmPtYChi2PrimCut;P_{t} [GeV/c];y;Entries",40,0,4,30,0,3);
   fHM->Create1<TH1D>("fhRecoCandEpmMinvChi2PrimCut","fhRecoCandEpmMinvChi2PrimCut;m_{inv} [GeV/c^{2}];Yield",500,0,5);// reconstructed invariant mass
   fHM->Create1<TH1D>("fhRecoCandEpmMomentumChi2PrimCut","fhRecoCandEpmMomentumChi2PrimCut;P [GeV/c];Yield",200,0,20);
   fHM->Create1<TH1D>("fhRecoCandMcEpmRapidityChi2PrimCut","fhRecoCandMcEpmRapidityChi2PrimCut;y;Yield",10,0,10);
   fHM->Create1<TH1D>("fhRecoCandMcEpmPtChi2PrimCut","fhRecoCandMcEpmPtChi2PrimCut;P_{t} [GeV/c];Yield",40,0,4);

   // Number of candidates after Cuts
   fHM->Create2<TH2D>("fhNofCandidatesAfterCuts","fhNofCandidatesAfterCuts;Cut;Charge;Candidates",4,0,4,3,-1,2);

   //M_inv of ep- from GammaConv and from Pi0
   fHM->Create1<TH1D>("fhRecoGammaConvEpmMinv","fhRecoGammaConvEpmMinv;m_{inv} [GeV/c^{2}];Yield",35,0,3.5);
   fHM->Create1<TH1D>("fhRecoPi0EpmMinv","fhRecoPi0EpmMinv;m_{inv} [GeV/c^{2}];Yield",10,0,1);

   //background identification right/wrong
   fHM->Create1<TH1D>("fhBgIdentificationRightWrong","fhBgIdentificationRightWrong;right, Mismatch, Ghost;Yield",3,0,3);

}


void CbmAnaJpsiTask::Exec(
      Option_t* option)
{
   fEventNum++;
   cout << "CbmAnaJpsiTask, event No. " <<  fEventNum << endl;
   
   if (fPrimVertex != NULL){
      fKFVertex = CbmKFVertex(*fPrimVertex);
   } else {
      Fatal("CbmAnaJpsiTask::Exec","No PrimaryVertex array!");
   }

  if (fPrimVertex != NULL){
     fKFVertex = CbmKFVertex(*fPrimVertex);
  } else {
     Fatal("CbmAnaDielectronTask::Exec","No PrimaryVertex array!");
  }

  RichPmtXY();

  FillCandidates();

  AssignMcToCandidates();

  DifferenceSignalAndBg();

  PairMcAndAcceptance();

  NofCandidatesAfterCuts();

  BgIdentification();
}


void CbmAnaJpsiTask::FillCandidates()
{
	fCandidates.clear();
	Int_t nofGlobalTracks = fGlobalTracks->GetEntriesFast();

	for (Int_t i=0;i<nofGlobalTracks;i++) {
		// create candidate, in which we will store some parameters of reconstructed global track
    	CbmAnaJpsiCandidate cand;

		//get GlobalTrack from array
		CbmGlobalTrack* globalTrack = (CbmGlobalTrack*) fGlobalTracks->At(i);
		if (NULL == globalTrack) continue;

		// get StsTrack from global track
		cand.fStsInd = globalTrack->GetStsTrackIndex();
		if (cand.fStsInd < 0) continue;
		CbmStsTrack* stsTrack = (CbmStsTrack*) fStsTracks->At(cand.fStsInd);
		if (NULL == stsTrack) continue;

		//calculate and get track parameters
        CbmAnaJpsiUtils::CalculateAndSetTrackParamsToCandidate(&cand, stsTrack, fKFVertex);

        // RICH
        cand.fRichInd = globalTrack->GetRichRingIndex();
        if (cand.fRichInd < 0) continue;
        CbmRichRing* richRing = (CbmRichRing*) fRichRings->At(cand.fRichInd);
        if (NULL == richRing) continue;

        // TRD
        cand.fTrdInd = globalTrack->GetTrdTrackIndex();
        if (cand.fTrdInd < 0) continue;
        CbmTrdTrack* trdTrack = (CbmTrdTrack*) fTrdTracks->At(cand.fTrdInd);
        if (trdTrack == NULL) continue;

        // ToF
        cand.fTofInd = globalTrack->GetTofHitIndex();
        if (cand.fTofInd < 0) continue;
        CbmTofHit* tofHit = (CbmTofHit*) fTofHits->At(cand.fTofInd);
        if (tofHit == NULL) continue;

        IsElectron(richRing, cand.fMomentum.Mag(), trdTrack, globalTrack, &cand);

        // push candidate to the array
        // we store only candidate which have all local segments: STS, RICH, TRD, TOF
        fCandidates.push_back(cand);
	}
}

void CbmAnaJpsiTask::AssignMcToCandidates()
{
   int nCand = fCandidates.size();
   for (int i = 0; i < nCand; i++){
	   //reset MC information
      fCandidates[i].ResetMcParams();

      //STS
      //MCTrackId of the candidate is defined by STS track
      int stsInd = fCandidates[i].fStsInd;
      CbmTrackMatchNew* stsMatch  = (CbmTrackMatchNew*) fStsTrackMatches->At(stsInd);
      if (stsMatch == NULL) continue;
      fCandidates[i].fStsMcTrackId = stsMatch->GetMatchedLink().GetIndex();
      if (fCandidates[i].fStsMcTrackId < 0) continue;
      CbmMCTrack* mcTrack1 = (CbmMCTrack*) fMcTracks->At(fCandidates[i].fStsMcTrackId);
      if (mcTrack1 == NULL) continue;
      int pdg = TMath::Abs(mcTrack1->GetPdgCode());
      int motherId = mcTrack1->GetMotherId();

      // set MC signature for candidate
      if (pdg == 11 && motherId == -1) fCandidates[i].fIsMcSignalElectron = true;
      if (motherId >=0){
         CbmMCTrack* mct1 = (CbmMCTrack*) fMcTracks->At(motherId);
         int motherPdg = mct1->GetPdgCode();
         //Pi0 electron
         if (mct1 != NULL && motherPdg == 111 && pdg == 11) {
            fCandidates[i].fIsMcPi0Electron = true;
         }
         //Gamma conversion electron
         if (mct1 != NULL && motherPdg == 22 && pdg == 11){
            fCandidates[i].fIsMcGammaElectron = true;
         }
      }

      // RICH
      int richInd = fCandidates[i].fRichInd;
      CbmTrackMatchNew* richMatch  = (CbmTrackMatchNew*) fRichRingMatches->At(richInd);
      if (richMatch == NULL) continue;
      fCandidates[i].fRichMcTrackId = richMatch->GetMatchedLink().GetIndex();

      // TRD
      int trdInd = fCandidates[i].fTrdInd;
      CbmTrackMatchNew* trdMatch = (CbmTrackMatchNew*) fTrdTrackMatches->At(trdInd);
      if (trdMatch == NULL) continue;
      fCandidates[i].fTrdMcTrackId = trdMatch->GetMatchedLink().GetIndex();

      // ToF
      int tofInd = fCandidates[i].fTofInd;
      if (tofInd < 0) continue;
      CbmTofHit* tofHit = (CbmTofHit*) fTofHits->At(tofInd);
      if (tofHit == NULL) continue;
      Int_t tofPointIndex = tofHit->GetRefId();
      if (tofPointIndex < 0) continue;
      FairMCPoint* tofPoint = (FairMCPoint*) fTofPoints->At(tofPointIndex);
      if (tofPoint == NULL) continue;
      fCandidates[i].fTofMcTrackId = tofPoint->GetTrackID();
   }// candidates
}

void CbmAnaJpsiTask::DifferenceSignalAndBg()
{
    Int_t nCand = fCandidates.size();
    for (Int_t i = 0; i < nCand; i++){
        if (fCandidates[i].fIsMcSignalElectron){
            fHM->H1("fhChi2PrimEl_" + CbmAnaJpsiHist::fSourceTypes[kJpsiSignal])->Fill(fCandidates[i].fChi2Prim);
            fHM->H1("fhMomEl_" + CbmAnaJpsiHist::fSourceTypes[kJpsiSignal])->Fill(fCandidates[i].fMomentum.Mag());
            fHM->H1("fhChi2StsEl_" + CbmAnaJpsiHist::fSourceTypes[kJpsiSignal])->Fill(fCandidates[i].fChi2sts);
            fHM->H1("fhRapidityEl_" + CbmAnaJpsiHist::fSourceTypes[kJpsiSignal])->Fill(fCandidates[i].fRapidity);
            fHM->H1("fhPtEl_" + CbmAnaJpsiHist::fSourceTypes[kJpsiSignal])->Fill(fCandidates[i].fMomentum.Perp());
            fHM->H1("fhRichAnn_" + CbmAnaJpsiHist::fSourceTypes[kJpsiSignal])->Fill(fCandidates[i].fRichAnn);
            fHM->H1("fhTrdAnn_" + CbmAnaJpsiHist::fSourceTypes[kJpsiSignal])->Fill(fCandidates[i].fTrdAnn);
            fHM->H1("fhTofM2_" + CbmAnaJpsiHist::fSourceTypes[kJpsiSignal])->Fill(fCandidates[i].fMomentum.Mag(), fCandidates[i].fMass2);
        } else {
        	fHM->H1("fhChi2PrimEl_" + CbmAnaJpsiHist::fSourceTypes[kJpsiBg])->Fill(fCandidates[i].fChi2Prim);
        	fHM->H1("fhMomEl_" + CbmAnaJpsiHist::fSourceTypes[kJpsiBg])->Fill(fCandidates[i].fMomentum.Mag());
        	fHM->H1("fhChi2StsEl_" + CbmAnaJpsiHist::fSourceTypes[kJpsiBg])->Fill(fCandidates[i].fChi2sts);
        	fHM->H1("fhRapidityEl_" + CbmAnaJpsiHist::fSourceTypes[kJpsiBg])->Fill(fCandidates[i].fRapidity);
        	fHM->H1("fhPtEl_" + CbmAnaJpsiHist::fSourceTypes[kJpsiBg])->Fill(fCandidates[i].fMomentum.Perp());
        	fHM->H1("fhRichAnn_" + CbmAnaJpsiHist::fSourceTypes[kJpsiBg])->Fill(fCandidates[i].fRichAnn);
            fHM->H1("fhTrdAnn_" + CbmAnaJpsiHist::fSourceTypes[kJpsiBg])->Fill(fCandidates[i].fTrdAnn);
            fHM->H1("fhTofM2_" + CbmAnaJpsiHist::fSourceTypes[kJpsiBg])->Fill(fCandidates[i].fMomentum.Mag(), fCandidates[i].fMass2);
        }
        if (fCandidates[i].fIsMcGammaElectron){
        	fHM->H1("fhChi2PrimEl_" + CbmAnaJpsiHist::fSourceTypes[kJpsiGamma])->Fill(fCandidates[i].fChi2Prim);
        	fHM->H1("fhMomEl_" + CbmAnaJpsiHist::fSourceTypes[kJpsiGamma])->Fill(fCandidates[i].fMomentum.Mag());
        	fHM->H1("fhChi2StsEl_" + CbmAnaJpsiHist::fSourceTypes[kJpsiGamma])->Fill(fCandidates[i].fChi2sts);
        	fHM->H1("fhRapidityEl_" + CbmAnaJpsiHist::fSourceTypes[kJpsiGamma])->Fill(fCandidates[i].fRapidity);
        	fHM->H1("fhPtEl_" + CbmAnaJpsiHist::fSourceTypes[kJpsiGamma])->Fill(fCandidates[i].fMomentum.Perp());
        	fHM->H1("fhRichAnn_" + CbmAnaJpsiHist::fSourceTypes[kJpsiGamma])->Fill(fCandidates[i].fRichAnn);
            fHM->H1("fhTrdAnn_" + CbmAnaJpsiHist::fSourceTypes[kJpsiGamma])->Fill(fCandidates[i].fTrdAnn);
            fHM->H1("fhTofM2_" + CbmAnaJpsiHist::fSourceTypes[kJpsiGamma])->Fill(fCandidates[i].fMomentum.Mag(), fCandidates[i].fMass2);
        }
        if (fCandidates[i].fIsMcPi0Electron) {
        	fHM->H1("fhChi2PrimEl_" + CbmAnaJpsiHist::fSourceTypes[kJpsiPi0])->Fill(fCandidates[i].fChi2Prim);
        	fHM->H1("fhMomEl_" + CbmAnaJpsiHist::fSourceTypes[kJpsiPi0])->Fill(fCandidates[i].fMomentum.Mag());
        	fHM->H1("fhChi2StsEl_" + CbmAnaJpsiHist::fSourceTypes[kJpsiPi0])->Fill(fCandidates[i].fChi2sts);
        	fHM->H1("fhRapidityEl_" + CbmAnaJpsiHist::fSourceTypes[kJpsiPi0])->Fill(fCandidates[i].fRapidity);
        	fHM->H1("fhPtEl_" + CbmAnaJpsiHist::fSourceTypes[kJpsiPi0])->Fill(fCandidates[i].fMomentum.Perp());
        	fHM->H1("fhRichAnn_" + CbmAnaJpsiHist::fSourceTypes[kJpsiPi0])->Fill(fCandidates[i].fRichAnn);
            fHM->H1("fhTrdAnn_" + CbmAnaJpsiHist::fSourceTypes[kJpsiPi0])->Fill(fCandidates[i].fTrdAnn);
            fHM->H1("fhTofM2_" + CbmAnaJpsiHist::fSourceTypes[kJpsiPi0])->Fill(fCandidates[i].fMomentum.Mag(), fCandidates[i].fMass2);
        }
    } // loop over candidates
}

Bool_t CbmAnaJpsiTask::IsMcTrackAccepted(
		Int_t mcTrackInd)
{
	CbmMCTrack* tr = (CbmMCTrack*) fMcTracks->At(mcTrackInd);
	if (tr == NULL) return false;
	//Int_t nRichPoints = fNofHitsInRingMap[mcTrackInd];
	return (tr->GetNPoints(kMVD) + tr->GetNPoints(kSTS) >= 4);// && nRichPoints >= 7 && tr->GetNPoints(kTRD) >= 2 && tr->GetNPoints(kTOF) > 0) ;
}

void CbmAnaJpsiTask::PairMcAndAcceptance()
{
	Int_t nMcTracks = fMcTracks->GetEntries();
	for (Int_t iP = 0; iP < nMcTracks; iP++) {
		CbmMCTrack* mctrackP = (CbmMCTrack*) fMcTracks->At(iP);
		Int_t motherIdP = mctrackP->GetMotherId();
		Int_t pdgP = mctrackP->GetPdgCode();
		if ( pdgP != 11 ) continue;
		Bool_t isAccP = IsMcTrackAccepted(iP);
		for (Int_t iM = 0; iM < nMcTracks; iM++) {
			if (iP == iM) continue;
			CbmMCTrack* mctrackM = (CbmMCTrack*) fMcTracks->At(iM);
			Int_t motherIdM = mctrackM->GetMotherId();
			Int_t pdgM = mctrackM->GetPdgCode();
			if ( pdgM != -11 ) continue;
			Bool_t isAccM = IsMcTrackAccepted(iM);
			CbmAnaJpsiKinematicParams p = CbmAnaJpsiKinematicParams::KinematicParamsWithMcTracks(mctrackP,mctrackM);

			// e+/- from signal
			if (motherIdM == -1 && pdgM == -11 && motherIdP == -1 && pdgP == 11) {

				fHM->H2("fhMcEpmRapidityPt")->Fill(p.fRapidity,p.fPt);
				fHM->H1("fhMcEpmMinv")->Fill(p.fMinv);

				//accepted e+/-
				if (isAccP && isAccM) {
					fHM->H2("fhAccEpmRapidityPt")->Fill(p.fRapidity,p.fPt);
					fHM->H1("fhAccEpmMinv")->Fill(p.fMinv);
					fHM->H1("fhAccEpmMomentum")->Fill(p.fMomentumMag);
					fHM->H1("fhAccEpmRapidity")->Fill(p.fRapidity);
					fHM->H1("fhAccEpmPt")->Fill(p.fPt);
				}
			}
		}//iM
	}//iP

	// find e+/- in Candidates
	Int_t nCand = fCandidates.size();
	for (Int_t iM=0 ; iM<nCand ; iM++) //loop over electrons
	{
		if (fCandidates[iM].fIsMcSignalElectron && fCandidates[iM].fCharge < 0) // All signal electrons (no positrons)
		{
			for (Int_t iP=0 ; iP<nCand ; iP++) //loop over positrons
			{	if (iP==iM) continue;

				if (fCandidates[iP].fIsMcSignalElectron && fCandidates[iP].fCharge > 0)
				{
					//get McId and McTracks
					Int_t CandMcIdM= fCandidates[iM].fStsMcTrackId;
					Int_t CandMcIdP= fCandidates[iP].fStsMcTrackId;
					CbmMCTrack* CandMcTrackM = (CbmMCTrack*) fMcTracks->At(CandMcIdM);
					CbmMCTrack* CandMcTrackP = (CbmMCTrack*) fMcTracks->At(CandMcIdP);

					//get McParameters
					CbmAnaJpsiKinematicParams cMc = CbmAnaJpsiKinematicParams::KinematicParamsWithMcTracks(CandMcTrackM,CandMcTrackP);

					//get reconstructed parameters
					CbmAnaJpsiKinematicParams cRec = CbmAnaJpsiKinematicParams::KinematicParamsWithCandidates(&fCandidates[iM],&fCandidates[iP]);

					//Fill histograms
					fHM->H2("fhCandMcEpmPtY")->Fill(cMc.fRapidity,cMc.fPt);//histogram Rapidity vs. transv. Momentum
					fHM->H1("fhCandEpmMinv")->Fill(cRec.fMinv); //histogram invariant mass

					//Chi2Prim Cut
					if (fCandidates[iP].fChi2Prim >= 3) continue;
					if (fCandidates[iM].fChi2Prim >= 3) continue;

					//Fill histograms
					fHM->H2("fhCandMcEpmPtYChi2PrimCut")->Fill(cMc.fRapidity,cMc.fPt);//histogram Rapidity vs. transv. Momentum
					fHM->H1("fhCandEpmMinvChi2PrimCut")->Fill(cRec.fMinv); //histogram invariant mass
					fHM->H1("fhCandEpmMomentumChi2PrimCut")->Fill(cRec.fMomentumMag);
					fHM->H1("fhCandMcEpmRapidityChi2PrimCut")->Fill(cMc.fRapidity);
					fHM->H1("fhCandMcEpmPtChi2PrimCut")->Fill(cMc.fPt);

				}//Signal positrons
			}//loop over positrons

		}//SignalEl


		//reconstructed Electron
		if (fCandidates[iM].fIsElectron && fCandidates[iM].fCharge < 0)
		{
			if (fCandidates[iM].fIsMcSignalElectron)
			{
			  for (Int_t iP=0 ; iP<nCand ; iP++) //loop over positrons
			  {
				if (iP==iM) continue;

				if (fCandidates[iP].fIsElectron && fCandidates[iP].fCharge > 0)
				{
					if (fCandidates[iP].fIsMcSignalElectron==false) continue;

					CbmAnaJpsiKinematicParams candidateRec = CbmAnaJpsiKinematicParams::KinematicParamsWithCandidates(&fCandidates[iM],&fCandidates[iP]);

					//Chi2Prim Cut
					if (fCandidates[iP].fChi2Prim >= fCuts.fChiPrimCut) continue;
					if (fCandidates[iM].fChi2Prim >= fCuts.fChiPrimCut) continue;

					//Fill histograms
					fHM->H2("fhRecoCandEpmPtYChi2PrimCut")->Fill(candidateRec.fRapidity,candidateRec.fPt);//histogram Rapidity vs. transv. Momentum
					fHM->H1("fhRecoCandEpmMinvChi2PrimCut")->Fill(candidateRec.fMinv); //histogram invariant mass
					fHM->H1("fhRecoCandEpmMomentumChi2PrimCut")->Fill(candidateRec.fMomentumMag);
					fHM->H1("fhRecoCandMcEpmRapidityChi2PrimCut")->Fill(candidateRec.fRapidity);
					fHM->H1("fhRecoCandMcEpmPtChi2PrimCut")->Fill(candidateRec.fPt);
				}
			  }
			}//iM IsMcSignalEelctron
		}//select e-

		//M_inv of e+- from Gamma Conversion
		if (fCandidates[iM].fIsMcGammaElectron && fCandidates[iM].fCharge < 0)
		{
			for (Int_t iP=0; iP<nCand;iP++)
			{
				if (iP==iM) continue;

				if (fCandidates[iP].fIsMcGammaElectron && fCandidates[iP].fCharge > 0)
				{
					CbmAnaJpsiKinematicParams gammaReco = CbmAnaJpsiKinematicParams::KinematicParamsWithCandidates(&fCandidates[iM],&fCandidates[iP]);

					fHM->H1("fhRecoGammaConvEpmMinv")->Fill(gammaReco.fMinv);

				} //Is a GammaConv Positron
			}//loop over positrons
		}//Is a GammaConv Electron


		//M_inv of e+- from Pi0
		if (fCandidates[iM].fIsMcPi0Electron && fCandidates[iM].fCharge < 0)
				{
					for (Int_t iP=0; iP<nCand;iP++)
					{
						if (iP==iM) continue;

						if (fCandidates[iP].fIsMcPi0Electron && fCandidates[iP].fCharge > 0)
						{
							CbmAnaJpsiKinematicParams gammaReco = CbmAnaJpsiKinematicParams::KinematicParamsWithCandidates(&fCandidates[iM],&fCandidates[iP]);

							fHM->H1("fhRecoPi0EpmMinv")->Fill(gammaReco.fMinv);

						} //Is a GammaConv Positron
					}//loop over positrons
				}//Is a GammaConv Electron


	}//cand | loop over electrons


} // PairsAcceptance

void CbmAnaJpsiTask::IsElectron(
		CbmRichRing* ring,
		Double_t momentum,
		CbmTrdTrack* trdTrack,
	    CbmGlobalTrack * gTrack,
		CbmAnaJpsiCandidate* cand)
{
	Bool_t richEl = IsRichElectron(ring, momentum, cand);
	Bool_t trdEl = (trdTrack != NULL)?IsTrdElectron(trdTrack, cand):true;
	Double_t annRich = cand->fRichAnn;
	Double_t annTrd = cand->fTrdAnn;
	Bool_t tofEl = IsTofElectron(gTrack, momentum, cand);
	//Bool_t momCut = (fCuts.fMomentumCut > 0.)?(momentum < fCuts.fMomentumCut):true;

	if (richEl && trdEl && tofEl)
	{
	     cand->fIsElectron = true;
	} else
	{
	     cand->fIsElectron = false;
	}

} // IsElectron



Bool_t CbmAnaJpsiTask::IsRichElectron(CbmRichRing* ring,
		Double_t momentum,
		CbmAnaJpsiCandidate* cand)
{
	if (fCuts.fUseRichAnn == false)
	{
		Bool_t axisA= ring->GetAaxis();
		Bool_t axisB= ring->GetBaxis();
		Double_t dist= ring->GetDistance();
		if ( fabs(axisA-fCuts.fMeanA) < fCuts.fRmsCoeff*fCuts.fRmsA &&fabs(axisB-fCuts.fMeanB) < fCuts.fRmsCoeff*fCuts.fRmsB && dist < fCuts.fDistCut){
		            return true;
		        } else {
		            return false;
		        }
	 } else
	 {
	    Double_t ann = fElIdAnn->DoSelect(ring, momentum);
	    cand->fRichAnn = ann;
	    if (ann > fCuts.fRichAnnCut) return true;
	    else  return false;
	 }
}//IsRichElectron



Bool_t CbmAnaJpsiTask::IsTrdElectron(
      CbmTrdTrack* trdTrack,
	  CbmAnaJpsiCandidate* cand)
{
    Double_t ann = trdTrack->GetPidANN();
    cand->fTrdAnn = ann;
    if (ann > fCuts.fTrdAnnCut) return true;
    else return false;

}


Bool_t CbmAnaJpsiTask::IsTofElectron(
      CbmGlobalTrack* gTrack,
      Double_t momentum,
	  CbmAnaJpsiCandidate* cand)
{
	Double_t trackLength = gTrack->GetLength() / 100. ;

	//calculate Time Of Flight of TOF Hit
	Int_t tofIndex = gTrack->GetTofHitIndex();
	CbmTofHit* tofHit = (CbmTofHit*) fTofHits->At(tofIndex);
	if (tofHit == NULL)
	{
		cand->fMass2 = 100.;
		return false;
	}

	Double_t time= 0.2998 * tofHit->GetTime(); // time in ns -> transfrom to ct in m

    // Calculate mass squared
	Double_t mass2 = TMath::Power(momentum,2.)* (TMath::Power(time/ trackLength, 2) - 1);
	cand->fMass2 = mass2;

	if (momentum >= 1.)
	{
		if (mass2 < (0.013*momentum - 0.003))
			{
		       return true;
		    }
	} else
		{
			if (mass2 < 0.01)
			{
		      return true; //fTofM2
		    }
		}
	return false;
}


Bool_t CbmAnaJpsiTask::IsMismatch(
		CbmAnaJpsiCandidate* cand)
{
	if (cand->fStsMcTrackId == cand->fRichMcTrackId && cand->fStsMcTrackId == cand->fTrdMcTrackId &&
	       cand->fStsMcTrackId == cand->fTofMcTrackId && cand->fStsMcTrackId !=-1) return false;
	return true;
}

Bool_t CbmAnaJpsiTask::IsGhost(
		CbmAnaJpsiCandidate* cand)
{
   if (cand->fStsMcTrackId == -1 || cand->fRichMcTrackId == -1 || cand->fTrdMcTrackId == -1 ||
         cand->fTofMcTrackId == -1) return true;
   return false;
}

void CbmAnaJpsiTask::NofCandidatesAfterCuts()
{	// # of Candidates after Cuts WITHOUT Signal Electrons
	Int_t nCand = fCandidates.size();

	for (Int_t i=0; i<nCand; i++)
	{	//reconstructed Candidates (Non SignalElectrons)
		if (fCandidates[i].fIsMcSignalElectron) continue;
		fHM->H2("fhNofCandidatesAfterCuts")->Fill(0.,1.);//NofCandReco++;

			//Electrons and Positrons
			if (fCandidates[i].fCharge > 0)
			{
				fHM->H2("fhNofCandidatesAfterCuts")->Fill(0., 0.);
			}
			else if (fCandidates[i].fCharge < 0)
			{
				fHM->H2("fhNofCandidatesAfterCuts")->Fill(0., -1.);
			}

		//IdCut
		if (fCandidates[i].fIsElectron==false) continue;
		fHM->H2("fhNofCandidatesAfterCuts")->Fill(1.,1);//NofCandIdCut++;

			if (fCandidates[i].fCharge > 0)
			{
				fHM->H2("fhNofCandidatesAfterCuts")->Fill(1., 0.);
			}
			else if (fCandidates[i].fCharge < 0)
			{
				fHM->H2("fhNofCandidatesAfterCuts")->Fill(1., -1.);
			}

		//ChiPrimCut
		if (fCandidates[i].fChi2Prim > fCuts.fChiPrimCut) continue;
		fHM->H2("fhNofCandidatesAfterCuts")->Fill(2.,1.);//NofCandChi2PrimCut++;

			if (fCandidates[i].fCharge > 0)
			{
				fHM->H2("fhNofCandidatesAfterCuts")->Fill(2., 0.);
			}
			else if (fCandidates[i].fCharge < 0)
			{
				fHM->H2("fhNofCandidatesAfterCuts")->Fill(2., -1.);
			}

		//fPtCut
		if(fCandidates[i].fMomentum.Perp() > fCuts.fPtCut ) continue;
		fHM->H2("fhNofCandidatesAfterCuts")->Fill(3.,1.);//NofCandPtCut++;
			if (fCandidates[i].fCharge > 0)
			{
				fHM->H2("fhNofCandidatesAfterCuts")->Fill(3., 0.);
			}
			else if (fCandidates[i].fCharge < 0)
			{
				fHM->H2("fhNofCandidatesAfterCuts")->Fill(3., -1.);
			}
	}//loop

}

void CbmAnaJpsiTask::BgIdentification()
{//background identification right/wrong
 Int_t nCand = fCandidates.size();

 for (Int_t i=0;i<nCand;i++)
 {
	 if (IsMismatch(&fCandidates[i]))
	 {
		 fHM->H1("fhBgIdentificationRightWrong")->Fill(1.);

		 //Fill Mismatch

	 }
	 if (IsGhost(&fCandidates[i]))
	 {
		 fHM->H1("fhBgIdentificationRightWrong")->Fill(2.);//Fill Ghosts
	 }
	 if (IsMismatch(&fCandidates[i])==false && IsGhost(&fCandidates[i])==false)
	 {
		 fHM->H1("fhBgIdentificationRightWrong")->Fill(0.);//Fill right identificated
	 }

 }//Loop over Cand

}//BgIdent

void CbmAnaJpsiTask::RichPmtXY() {
	Int_t nofRichHits = fRichHits->GetEntriesFast();
	for (Int_t i = 0; i < nofRichHits; i++) {
		// get the RichHit from array
		CbmRichHit* richHit = (CbmRichHit*) fRichHits->At(i);
		if (NULL == richHit) continue;
		Int_t PointInd = richHit->GetRefId();
		if (PointInd < 0) continue;

		// get the McRichPoint of the RichHit
		CbmRichPoint* richMcPoint = (CbmRichPoint*) fRichPoints->At(PointInd);
		if (NULL == richMcPoint) continue;

		// get the RICH photon MC Track
		Int_t photonMcTrackId = richMcPoint->GetTrackID();
		if (photonMcTrackId == -1) continue;
		CbmMCTrack* photonMcTrack = (CbmMCTrack*) fMcTracks->At(photonMcTrackId);
		if (NULL == photonMcTrack) continue;

		// get photon mother MC Track (electron, pion etc.)
		Int_t photonMotherId = photonMcTrack->GetMotherId();
		if (photonMotherId == -1) continue;
		CbmMCTrack* photonMotherMcTrack = (CbmMCTrack*) fMcTracks->At( photonMotherId);
		if (NULL == photonMotherMcTrack) continue;
		Int_t photonMotherPdgCode = photonMotherMcTrack->GetPdgCode();

		// check that the photon was produced by electron
		if (TMath::Abs(11) == photonMotherPdgCode) {
			Int_t photonGrandmotherId = photonMotherMcTrack->GetMotherId();

			// if primary signal electron
			if (TMath::Abs(11) == photonMotherPdgCode || photonGrandmotherId == -1) {
				fHM->H2("fhRichPmtXY_" + CbmAnaJpsiHist::fSourceTypes[kJpsiSignal])->Fill( richHit->GetX(), richHit->GetY());
			}

			if (photonGrandmotherId != -1) {
				CbmMCTrack* photonGrandmotherMcTrack = (CbmMCTrack*) fMcTracks->At(photonGrandmotherId);
				if (NULL == photonGrandmotherMcTrack) continue;
				Int_t photonGrandmotherPdgCode = photonGrandmotherMcTrack->GetPdgCode();

				if (photonGrandmotherPdgCode == 22) {
					fHM->H2("fhRichPmtXY_" + CbmAnaJpsiHist::fSourceTypes[kJpsiGamma])->Fill(richHit->GetX(), richHit->GetY());
				}

				if (photonGrandmotherPdgCode == 111) {
					fHM->H2("fhRichPmtXY_"+ CbmAnaJpsiHist::fSourceTypes[kJpsiPi0])->Fill(richHit->GetX(), richHit->GetY());
				}
			}
		}
	}
}





void CbmAnaJpsiTask::Finish()
{
	fHM->WriteToFile();
}


ClassImp(CbmAnaJpsiTask)

