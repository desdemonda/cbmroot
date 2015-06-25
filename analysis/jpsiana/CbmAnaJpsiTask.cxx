
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

void CbmAnaJpsiTask::CreateSourceTypesAnalysisStepsH1(
      const string& name,
      const string& axisX,
      const string& axisY,
      double nBins,
      double min,
      double max
      )
{
	for (Int_t i = 0; i < CbmAnaJpsiHist::fNofSourceTypes; i++)
	{
	         for (Int_t step = 0; step < CbmAnaJpsiHist::fNofAnaSteps; step++)
	         {

	            string hname = name + "_" + CbmAnaJpsiHist::fSourceTypes[i] + "_" + CbmAnaJpsiHist::fAnaSteps[step] ;
	            fHM->Create1<TH1D>(hname,hname+";"+ axisY + ";" + axisY,nBins,min, max);
	         }
	}
}

void CbmAnaJpsiTask::CreateAnalysisStepsH1(
      const string& name,
      const string& axisX,
      const string& axisY,
      double nBins,
      double min,
      double max
      )
{
   for (Int_t i = 0; i < CbmAnaJpsiHist::fNofAnaSteps; i++)
   {
      string hname = name + "_"+ CbmAnaJpsiHist::fAnaSteps[i];
      fHM->Create1<TH1D>(hname, hname+";"+axisX+";"+axisY, nBins, min, max);
   }
}

void CbmAnaJpsiTask::CreateAnalysisStepsH2(
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
   for (Int_t i = 0; i < CbmAnaJpsiHist::fNofAnaSteps; i++)
   {
      string hname = name + "_"+ CbmAnaJpsiHist::fAnaSteps[i];
      fHM->Create2<TH2D>(hname, hname+";"+axisX+";"+axisY+";"+axisZ, nBinsX, minX, maxX, nBinsY, minY, maxY);
   }
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

   //vertex of the secondary electrons from gamma conversion
   CreateAnalysisStepsH2("fh_vertex_el_gamma_xz","Z [cm]", "X [cm]", "Counter per event", 200, -10., 190., 400, -130.,130.);
   CreateAnalysisStepsH2("fh_vertex_el_gamma_yz","Z [cm]", "Y [cm]", "Counter per event", 200, -10., 190., 400, -130., 130.);
   CreateAnalysisStepsH2("fh_vertex_el_gamma_xy","X [cm]", "Y [cm]", "Counter per event",  400, -130.,130., 400, -130., 130.);
   CreateAnalysisStepsH2("fh_vertex_el_gamma_rz","Z [cm]", "#sqrt{X^{2}+Y^{2}} [cm]", "Counter per event",  300, -10., 190., 300, 0., 150.);

   // Number of BG and signal tracks after each cut
   fHM->Create1<TH1D>("fh_nof_bg_tracks","fh_nof_bg_tracks;Analysis steps;Tracks/event", CbmAnaJpsiHist::fNofAnaSteps, 0., CbmAnaJpsiHist::fNofAnaSteps);
   //fHistoList.push_back(fh_nof_bg_tracks);
   fHM->Create1<TH1D>("fh_nof_el_tracks","fh_nof_el_tracks;Analysis steps;Tracks/event", CbmAnaJpsiHist::fNofAnaSteps, 0., CbmAnaJpsiHist::fNofAnaSteps);
   fHM->Create2<TH2D>("fh_source_tracks","fh_source_tracks;Analysis steps;Particle", CbmAnaJpsiHist::fNofAnaSteps, 0., CbmAnaJpsiHist::fNofAnaSteps, 7, 0., 7.);

   CreateSourceTypesAnalysisStepsH1("fh_source_mom", "p [GeV/C]", "Yield", 300, 0., 15.);
   CreateSourceTypesAnalysisStepsH1("fh_source_pt", "p [GeV/C]", "Yield", 100, 0., 5.);

   //Create invariant mass histograms
   CreateAnalysisStepsH1("fh_signal_minv", "M_{ee} [GeV/c^{2}]", "Yield", 4000, 0. , 4.);
   CreateAnalysisStepsH1("fh_bg_minv", "M_{ee} [GeV/c^{2}]", "Yield", 4000, 0. , 4.);
   CreateAnalysisStepsH1("fh_pi0_minv", "M_{ee} [GeV/c^{2}]", "Yield", 4000, 0. , 4.);
   CreateAnalysisStepsH1("fh_gamma_minv", "M_{ee} [GeV/c^{2}]", "Yield", 4000, 0. , 4.);

   // minv for true matched and mismatched tracks
   CreateAnalysisStepsH1("fh_bg_truematch_minv", "M_{ee} [GeV/c^{2}]", "Yield", 4000, 0. , 4.);
   CreateAnalysisStepsH1("fh_bg_truematch_el_minv", "M_{ee} [GeV/c^{2}]", "Yield", 4000, 0. , 4.);
   CreateAnalysisStepsH1("fh_bg_truematch_notel_minv", "M_{ee} [GeV/c^{2}]", "Yield", 4000, 0. , 4.);
   CreateAnalysisStepsH1("fh_bg_mismatch_minv", "M_{ee} [GeV/c^{2}]", "Yield", 4000, 0. , 4.);

   //Invariant mass vs. Mc Pt
   CreateAnalysisStepsH2("fh_signal_minv_pt","M_{ee} [GeV/c^{2}]", "P_{t} [GeV/c]", "Yield", 100, 0., 4., 20, 0., 2.);
   CreateAnalysisStepsH2("fh_pi0_minv_pt", "M_{ee} [GeV/c^{2}]", "P_{t} [GeV/c]", "Yield", 100, 0., 4., 20, 0., 2.);

   // Momentum distribution of the signal
   CreateAnalysisStepsH1("fh_signal_mom", "P [GeV/c]", "Yield", 100, 0., 15.);
   //Pt/y distibution of the signal
   CreateAnalysisStepsH2("fh_signal_pty","Rapidity", "P_{t} [GeV/c]", "Yield", 40, 0., 4., 20, 0., 2.);
   // Pt distribution of the signal
   CreateAnalysisStepsH1("fh_signal_pt", "P_{t} [GeV/c]", "Yield", 40, 0., 4.);
   // Rapidity distribution of the signal
   CreateAnalysisStepsH1("fh_signal_rapidity", "Rapidity [GeV/c]", "Yield", 50, 0., 5.);

   //Number of mismatches and ghosts after each cut
   fHM->Create1<TH1D>("fh_nof_mismatches","fh_nof_mismatches;Analysis steps;Tracks/event", CbmAnaJpsiHist::fNofAnaSteps, 0., CbmAnaJpsiHist::fNofAnaSteps);
   fHM->Create1<TH1D>("fh_nof_mismatches_rich","fh_nof_mismatches_rich;Analysis steps;Tracks/event", CbmAnaJpsiHist::fNofAnaSteps, 0., CbmAnaJpsiHist::fNofAnaSteps);
   fHM->Create1<TH1D>("fh_nof_mismatches_trd","fh_nof_mismatches_trd;Analysis steps;Tracks/event", CbmAnaJpsiHist::fNofAnaSteps, 0., CbmAnaJpsiHist::fNofAnaSteps);
   fHM->Create1<TH1D>("fh_nof_mismatches_tof","fh_nof_mismatches_tof;Analysis steps;Tracks/event", CbmAnaJpsiHist::fNofAnaSteps, 0., CbmAnaJpsiHist::fNofAnaSteps);
   fHM->Create1<TH1D>("fh_nof_ghosts","fh_nof_ghosts;Analysis steps;Tracks/event", CbmAnaJpsiHist::fNofAnaSteps, 0., CbmAnaJpsiHist::fNofAnaSteps);

   ///////////////////////////////////////////

   // Number of candidates after Cuts
   fHM->Create2<TH2D>("fhNofCandidatesAfterCuts","fhNofCandidatesAfterCuts;Cut;Charge;Candidates",4,0,4,3,-1,2);

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

  MCPairs();

  RichPmtXY();

  FillCandidates();

  AssignMcToCandidates();

  DifferenceSignalAndBg();

  SingleParticleAcceptance();

  PairMcAndAcceptance();

  NofCandidatesAfterCuts();

  BgIdentification();

  SignalAndBgReco();
}

void CbmAnaJpsiTask::MCPairs()
{
    Int_t nMcTracks = fMcTracks->GetEntries();
    for (Int_t i = 0; i < nMcTracks; i++){
        CbmMCTrack* mctrack = (CbmMCTrack*) fMcTracks->At(i);
        Int_t motherId = mctrack->GetMotherId();
        Int_t pdg = TMath::Abs(mctrack->GetPdgCode());
        Double_t mom = mctrack->GetP();

        // mother pdg of e-/e+
        Int_t mcMotherPdg = 0;
        if (pdg == 11) {
           // momentum distribution for electrons from signal
           if (motherId == -1)
        	   {
        	   fHM->H1("fh_source_mom_"  + CbmAnaJpsiHist::fSourceTypes[kJpsiSignal] + "_" + CbmAnaJpsiHist::fAnaSteps[kJpsiMc])->Fill(mom);
        	   fHM->H1("fh_source_pt_"  + CbmAnaJpsiHist::fSourceTypes[kJpsiSignal] + "_" + CbmAnaJpsiHist::fAnaSteps[kJpsiMc])->Fill(mctrack->GetPt());
        	   }

           // secondary electrons
           if (motherId != -1){
                CbmMCTrack* mother = (CbmMCTrack*) fMcTracks->At(motherId);
                if (NULL != mother) mcMotherPdg = mother->GetPdgCode();
                // vertex of secondary electron from gamma conversion
                if (mcMotherPdg == 22) {
                    TVector3 v;
                    mctrack->GetStartVertex(v);
                    fHM->H2("fh_vertex_el_gamma_xz_" + CbmAnaJpsiHist::fAnaSteps[kJpsiMc])->Fill(v.Z(),v.X());
                    fHM->H2("fh_vertex_el_gamma_yz_" + CbmAnaJpsiHist::fAnaSteps[kJpsiMc])->Fill(v.Z(),v.Y());
                    fHM->H2("fh_vertex_el_gamma_xy_" + CbmAnaJpsiHist::fAnaSteps[kJpsiMc])->Fill(v.X(),v.Y());
                    fHM->H2("fh_vertex_el_gamma_rz_" + CbmAnaJpsiHist::fAnaSteps[kJpsiMc])->Fill( v.Z(), sqrt(v.X()*v.X()+v.Y()*v.Y()) );
                }
            }else {
                mcMotherPdg = 0;
            }

        }
    } // nMcTracks
} //MC Pairs

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
      fCandidates[i].fMcMotherId = motherId;
      fCandidates[i].fMcPdg = pdg;

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

void CbmAnaJpsiTask::SingleParticleAcceptance()
{
    Int_t nMcTracks = fMcTracks->GetEntries();
    for (Int_t i = 0; i < nMcTracks; i++) {
        CbmMCTrack* mctrack = (CbmMCTrack*) fMcTracks->At(i);
        Int_t motherId = mctrack->GetMotherId();
        Int_t pdg = TMath::Abs(mctrack->GetPdgCode());
        Bool_t isAcc = IsMcTrackAccepted(i);

        if (pdg ==11 && motherId == -1)
        {
        fHM->H1("fh_nof_el_tracks")->Fill(0.5);
        }

        Int_t mcMotherPdg = 0;
        if (pdg == 11 && isAcc) {
           if (motherId == -1)
         	   {
        	   fHM->H1("fh_nof_el_tracks")->Fill(1.5);
         	   fHM->H1("fh_source_mom_" + CbmAnaJpsiHist::fSourceTypes[kJpsiSignal] + "_" + CbmAnaJpsiHist::fAnaSteps[kJpsiAcc])->Fill(mctrack->GetP());
         	   fHM->H1("fh_source_pt_" + CbmAnaJpsiHist::fSourceTypes[kJpsiSignal] + "_" + CbmAnaJpsiHist::fAnaSteps[kJpsiAcc])->Fill(mctrack->GetPt());
         	   }

           if (motherId != -1){
               CbmMCTrack* mother = (CbmMCTrack*) fMcTracks->At(motherId);
               if (NULL != mother) mcMotherPdg = mother->GetPdgCode();
               // vertex of secondary electron from gamma conversion
               if (mcMotherPdg == 22) {
                  TVector3 v;
                  mctrack->GetStartVertex(v);
                  fHM->H2("fh_vertex_el_gamma_xz_" + CbmAnaJpsiHist::fAnaSteps[kJpsiAcc])->Fill(v.Z(),v.X());
                  fHM->H2("fh_vertex_el_gamma_yz_" + CbmAnaJpsiHist::fAnaSteps[kJpsiAcc])->Fill(v.Z(),v.Y());
                  fHM->H2("fh_vertex_el_gamma_xy_" + CbmAnaJpsiHist::fAnaSteps[kJpsiAcc])->Fill(v.X(),v.Y());
                  fHM->H2("fh_vertex_el_gamma_rz_" + CbmAnaJpsiHist::fAnaSteps[kJpsiAcc])->Fill( v.Z(), sqrt(v.X()*v.X()+v.Y()*v.Y()) );
               }

            }else {
                mcMotherPdg = 0;
            }

        }
    }
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

				fHM->H2("fh_signal_pty_"+CbmAnaJpsiHist::fAnaSteps[kJpsiMc])->Fill(p.fRapidity, p.fPt);
				fHM->H1("fh_signal_mom_"+CbmAnaJpsiHist::fAnaSteps[kJpsiMc])->Fill(p.fMomentumMag);
				fHM->H1("fh_signal_minv_"+CbmAnaJpsiHist::fAnaSteps[kJpsiMc])->Fill(p.fMinv);
				fHM->H2("fh_signal_minv_pt_"+CbmAnaJpsiHist::fAnaSteps[kJpsiMc])->Fill(p.fMinv, p.fPt);
				fHM->H1("fh_signal_pt_"+CbmAnaJpsiHist::fAnaSteps[kJpsiMc])->Fill(p.fPt);
				fHM->H1("fh_signal_rapidity_"+CbmAnaJpsiHist::fAnaSteps[kJpsiMc])->Fill(p.fRapidity);

				//accepted e+/-
				if (isAccP && isAccM) {

					//no FillPairHists because we have not declared KinematicParamCand jet
					fHM->H2("fh_signal_pty_"+CbmAnaJpsiHist::fAnaSteps[kJpsiAcc])->Fill(p.fRapidity, p.fPt);
					fHM->H1("fh_signal_mom_"+CbmAnaJpsiHist::fAnaSteps[kJpsiAcc])->Fill(p.fMomentumMag);
					fHM->H1("fh_signal_minv_"+CbmAnaJpsiHist::fAnaSteps[kJpsiAcc])->Fill(p.fMinv);
					fHM->H2("fh_signal_minv_pt_"+CbmAnaJpsiHist::fAnaSteps[kJpsiAcc])->Fill(p.fMinv, p.fPt);
					fHM->H1("fh_signal_pt_"+CbmAnaJpsiHist::fAnaSteps[kJpsiAcc])->Fill(p.fPt);
					fHM->H1("fh_signal_rapidity_"+CbmAnaJpsiHist::fAnaSteps[kJpsiAcc])->Fill(p.fRapidity);

				}
			}

			if (motherIdP >=0 && motherIdM >=0){
						    CbmMCTrack* mct1P = (CbmMCTrack*) fMcTracks->At(motherIdP);
						    Int_t motherPdgP = mct1P->GetPdgCode();

						    CbmMCTrack* mct1M = (CbmMCTrack*) fMcTracks->At(motherIdM);
						    Int_t motherPdgM = mct1M->GetPdgCode();

						    Bool_t isPi0 = (motherPdgP == 111 && pdgP == 11 && motherPdgM == 111 && pdgM == -11 && motherIdP == motherIdM);

						    if (isPi0)
						    {
						    	fHM->H1("fh_pi0_minv_"+CbmAnaJpsiHist::fAnaSteps[kJpsiMc])->Fill(p.fMinv);
						    	fHM->H2("fh_pi0_minv_pt_"+CbmAnaJpsiHist::fAnaSteps[kJpsiMc])->Fill(p.fMinv,p.fRapidity);
						    }

						    if (isAccP && isAccM && isPi0)
						    {
						    	fHM->H1("fh_pi0_minv_"+CbmAnaJpsiHist::fAnaSteps[kJpsiAcc])->Fill(p.fMinv);
						    	fHM->H2("fh_pi0_minv_pt_"+CbmAnaJpsiHist::fAnaSteps[kJpsiAcc])->Fill(p.fMinv,p.fRapidity);
						    }

			}

		}//iM
	}//iP
} // PairsAcceptance


void CbmAnaJpsiTask::TrackSource(
		CbmAnaJpsiCandidate* cand,
    	CbmAnaJpsiAnalysisSteps step,
		Int_t pdg)
{
	int binNum = (double) step + 0.5;
	Double_t mom = cand->fMomentum.Mag();
	Double_t pt = cand->fMomentum.Perp();


    if (cand->fIsMcSignalElectron)
	{
		fHM->H1("fh_nof_el_tracks")->Fill(binNum);
		fHM->H1("fh_source_mom_" + CbmAnaJpsiHist::fSourceTypes[kJpsiSignal] + "_" + CbmAnaJpsiHist::fAnaSteps[step])->Fill(mom);
		fHM->H1("fh_source_pt_" + CbmAnaJpsiHist::fSourceTypes[kJpsiSignal] + "_"+ CbmAnaJpsiHist::fAnaSteps[step])->Fill(pt);
	}
	else
	{
		if (IsMismatch(cand)) fHM->H1("fh_nof_mismatches")->Fill(binNum);
		if (cand->fStsMcTrackId != cand->fRichMcTrackId) fHM->H1("fh_nof_mismatches_rich")->Fill(binNum);
		if (cand->fStsMcTrackId != cand->fTrdMcTrackId) fHM->H1("fh_nof_mismatches_trd")->Fill(binNum);
		if (cand->fStsMcTrackId != cand->fTofMcTrackId) fHM->H1("fh_nof_mismatches_tof")->Fill(binNum);
  	    if (IsGhost(cand)) fHM->H1("fh_nof_ghosts")->Fill(binNum);
  	    fHM->H1("fh_nof_bg_tracks")->Fill(binNum);
  	    fHM->H1("fh_source_mom_" + CbmAnaJpsiHist::fSourceTypes[kJpsiBg] + "_" + CbmAnaJpsiHist::fAnaSteps[step])->Fill(mom);
  	    fHM->H1("fh_source_pt_" + CbmAnaJpsiHist::fSourceTypes[kJpsiBg] + "_" + CbmAnaJpsiHist::fAnaSteps[step])->Fill(pt);


  	    if (cand->fIsMcGammaElectron)
  	    {
  	  		fHM->H2("fh_source_tracks")->Fill(binNum, 0.5);
  	  		fHM->H1("fh_source_mom_" + CbmAnaJpsiHist::fSourceTypes[kJpsiGamma] + "_" + CbmAnaJpsiHist::fAnaSteps[step])->Fill(mom);
  	  		fHM->H1("fh_source_pt_" + CbmAnaJpsiHist::fSourceTypes[kJpsiGamma] + "_" + CbmAnaJpsiHist::fAnaSteps[step])->Fill(pt);

  	  		// e+/- from gamma conversion vertex
  	  		int mcTrackId = cand->fStsMcTrackId;
  	  		CbmMCTrack* mctrack = (CbmMCTrack*) fMcTracks->At(mcTrackId);
  	  		if (NULL != mctrack)
  	  		{
  	  			TVector3 v;
  	  			mctrack->GetStartVertex(v);
  	  			fHM->H2("fh_vertex_el_gamma_xz_" + CbmAnaJpsiHist::fAnaSteps[step])->Fill(v.Z(),v.X());
  	  			fHM->H2("fh_vertex_el_gamma_yz_" + CbmAnaJpsiHist::fAnaSteps[step])->Fill(v.Z(),v.Y());
  	  		    fHM->H2("fh_vertex_el_gamma_xy_" + CbmAnaJpsiHist::fAnaSteps[step])->Fill(v.X(),v.Y());
  	  		    fHM->H2("fh_vertex_el_gamma_rz_" + CbmAnaJpsiHist::fAnaSteps[step])->Fill( v.Z(), sqrt(v.X()*v.X()+v.Y()*v.Y()) );
  	  		}
  	    }
  	    else if (cand->fIsMcPi0Electron)
  	    	{
  	    		fHM->H2("fh_source_tracks")->Fill(binNum, 1.5);
  	  	  		fHM->H1("fh_source_mom_" + CbmAnaJpsiHist::fSourceTypes[kJpsiPi0] + "_" + CbmAnaJpsiHist::fAnaSteps[step])->Fill(mom);
  	  	  		fHM->H1("fh_source_pt_" + CbmAnaJpsiHist::fSourceTypes[kJpsiPi0] + "_" + CbmAnaJpsiHist::fAnaSteps[step])->Fill(pt);
  	  		}
  	    else if (pdg == 211 || pdg ==-211) //Pi+-
  	  		{
	    		fHM->H2("fh_source_tracks")->Fill(binNum, 2.5);
  	  		}
  	    else if (pdg == 2212) //P
  	    	{
	    		fHM->H2("fh_source_tracks")->Fill(binNum, 3.5);
  	  		}
  	    else if (pdg == 321 || pdg == -321)
  	    	{
    			fHM->H2("fh_source_tracks")->Fill(binNum, 4.5);
  	  		}
  	    else if ( (pdg == 11 || pdg == -11) && !cand->fIsMcGammaElectron
  	  		      && !cand->fIsMcPi0Electron && !cand->fIsMcSignalElectron)
  	    	{
    			fHM->H2("fh_source_tracks")->Fill(binNum, 5.5);
  	    	}
  	    else{
	    		fHM->H2("fh_source_tracks")->Fill(binNum, 6.5);
  	  		}

	}//Signal or not

}//TrackSource



void CbmAnaJpsiTask::FillPairHists(
	  CbmAnaJpsiCandidate* candP,
      CbmAnaJpsiCandidate* candM,
	  CbmAnaJpsiKinematicParams* parMc,
	  CbmAnaJpsiKinematicParams* parRec,
      CbmAnaJpsiAnalysisSteps step)
{
	Bool_t isSignal = candP->fIsMcSignalElectron && candM->fIsMcSignalElectron;
	Bool_t isPi0 = (candP->fIsMcPi0Electron && candM->fIsMcPi0Electron && candP->fMcMotherId == candM->fMcMotherId);
	Bool_t isGamma = (candP->fIsMcGammaElectron && candM->fIsMcGammaElectron && candP->fMcMotherId == candM->fMcMotherId);
    Bool_t isBG = (!isGamma) && (!isPi0) && (!(candP->fIsMcSignalElectron || candM->fIsMcSignalElectron));
	Bool_t isMismatch = (IsMismatch(candP) || IsMismatch(candM));

	if (isSignal) fHM->H2("fh_signal_pty_"+CbmAnaJpsiHist::fAnaSteps[step])->Fill(parMc->fRapidity, parMc->fPt);
	if (isSignal) fHM->H1("fh_signal_mom_"+CbmAnaJpsiHist::fAnaSteps[step])->Fill(parMc->fMomentumMag);
	if (isSignal) fHM->H1("fh_signal_minv_"+CbmAnaJpsiHist::fAnaSteps[step])->Fill(parRec->fMinv);
	if (isSignal) fHM->H2("fh_signal_minv_pt_"+CbmAnaJpsiHist::fAnaSteps[step])->Fill(parRec->fMinv, parMc->fPt);
	if (isSignal) fHM->H1("fh_signal_pt_"+CbmAnaJpsiHist::fAnaSteps[step])->Fill(parMc->fPt);
	if (isSignal) fHM->H1("fh_signal_rapidity_"+CbmAnaJpsiHist::fAnaSteps[step])->Fill(parMc->fRapidity);
	if (isBG) fHM->H1("fh_bg_minv_"+CbmAnaJpsiHist::fAnaSteps[step])->Fill(parRec->fMinv);
	//PairSource(candP, candM, step, parRec);
	if (isPi0) fHM->H1("fh_pi0_minv_"+CbmAnaJpsiHist::fAnaSteps[step])->Fill(parRec->fMinv);
	if (isPi0) fHM->H2("fh_pi0_minv_pt_"+CbmAnaJpsiHist::fAnaSteps[step])->Fill(parRec->fMinv, parMc->fPt);
	if (isGamma) fHM->H1("fh_gamma_minv_"+CbmAnaJpsiHist::fAnaSteps[step])->Fill(parRec->fMinv);
	if (isBG && isMismatch) fHM->H1("fh_bg_mismatch_minv_"+CbmAnaJpsiHist::fAnaSteps[step])->Fill(parRec->fMinv);
	if (isBG && !isMismatch)
	{
		fHM->H1("fh_bg_truematch_minv_"+CbmAnaJpsiHist::fAnaSteps[step])->Fill(parRec->fMinv);
		if (candP->fMcPdg == 11 && candM->fMcPdg == 11) fHM->H1("fh_bg_truematch_el_minv_"+CbmAnaJpsiHist::fAnaSteps[step])->Fill(parRec->fMinv);
		if (candP->fMcPdg != 11 || candM->fMcPdg != 11) fHM->H1("fh_bg_truematch_notel_minv_"+CbmAnaJpsiHist::fAnaSteps[step])->Fill(parRec->fMinv);
	}

}



void CbmAnaJpsiTask::SignalAndBgReco()
{
	Int_t ncand = fCandidates.size();

	for (Int_t i=0;i<ncand;i++)
	{
		Int_t pdg = 0;
		if (fCandidates[i].fStsMcTrackId > 0)
		{
			CbmMCTrack* mcTrack = (CbmMCTrack*) fMcTracks->At(fCandidates[i].fStsMcTrackId);
			if (NULL != mcTrack) pdg = mcTrack->GetPdgCode();
		}

	     Bool_t isChi2Prim = (fCandidates[i].fChi2Prim < fCuts.fChiPrimCut);
	     Bool_t isEl = (fCandidates[i].fIsElectron);
	     Bool_t isPtCut = (fCandidates[i].fMomentum.Perp() > fCuts.fPtCut);


	     TrackSource(&fCandidates[i], kJpsiReco, pdg);
	     if (isChi2Prim) TrackSource(&fCandidates[i], kJpsiChi2Prim, pdg);
	     if (isChi2Prim && isEl) TrackSource(&fCandidates[i], kJpsiElId, pdg);
	     if (isChi2Prim && isEl && isPtCut) TrackSource(&fCandidates[i], kJpsiPtCut, pdg);
	}

	for (Int_t iP=0;iP<ncand;iP++)
	{
		if (fCandidates[iP].fCharge < 0) continue;
		CbmMCTrack* mctrackP = NULL;
		if (fCandidates[iP].fStsMcTrackId >=0) mctrackP = (CbmMCTrack*) fMcTracks->At(fCandidates[iP].fStsMcTrackId);

		for (Int_t iM=0;iM<ncand;iM++)
		{
			if (fCandidates[iM].fCharge > 0) continue;
			CbmMCTrack* mctrackM = NULL;
			if (fCandidates[iM].fStsMcTrackId >=0) mctrackM = (CbmMCTrack*) fMcTracks->At(fCandidates[iM].fStsMcTrackId);
			if (iM == iP ) continue;

			CbmAnaJpsiKinematicParams pMC;
			if (mctrackP != NULL && mctrackM != NULL) pMC = CbmAnaJpsiKinematicParams::KinematicParamsWithMcTracks(mctrackP, mctrackM);

			CbmAnaJpsiKinematicParams pRec = CbmAnaJpsiKinematicParams::KinematicParamsWithCandidates(&fCandidates[iP],&fCandidates[iM]);

		    Bool_t isChiPrimary = (fCandidates[iP].fChi2Prim < fCuts.fChiPrimCut && fCandidates[iM].fChi2Prim < fCuts.fChiPrimCut);
		    Bool_t isEl = (fCandidates[iP].fIsElectron && fCandidates[iM].fIsElectron);
		    Bool_t isPtCut = (fCandidates[iP].fMomentum.Perp() > fCuts.fPtCut && fCandidates[iM].fMomentum.Perp() > fCuts.fPtCut);

		    FillPairHists(&fCandidates[iP], &fCandidates[iM], &pMC, &pRec, kJpsiReco);
		    if (isChiPrimary)
		    {
		    	FillPairHists(&fCandidates[iP], &fCandidates[iM], &pMC, &pRec, kJpsiChi2Prim);
		    }
		    if (isChiPrimary && isEl)
		    {
		    	FillPairHists(&fCandidates[iP], &fCandidates[iM], &pMC, &pRec, kJpsiElId);
		    }
		    if (isChiPrimary && isEl && isPtCut)
		    {
		    	FillPairHists(&fCandidates[iP], &fCandidates[iM], &pMC, &pRec, kJpsiPtCut);
		    }
		}//iM
	}//iP
}

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

