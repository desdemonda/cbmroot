/** CbmAnaDielectronTask.cxx
 * based on class by T.Galatyuk
 * @author Elena Lebedeva <e.lebedeva@gsi.de>
 * @since 2010
 * @version 3.0
 **/

#include "CbmAnaDielectronTask.h" 

#include "FairTask.h"
#include "FairRootManager.h"
#include "CbmVertex.h"
#include "CbmGlobalTrack.h"
#include "CbmStsKFTrackFitter.h"
#include "FairMCPoint.h"

#include "CbmMCTrack.h"

#include "CbmStsTrack.h"
#include "CbmRichRing.h"
#include "CbmTrackMatchNew.h"
#include "CbmTrdTrack.h"
#include "CbmTrdHit.h"
#include "CbmTofHit.h"

#include "FairTrackParam.h"
#include "CbmKF.h"

#include "CbmRichPoint.h"
#include "CbmRichHit.h"
#include "CbmTofPoint.h"

#include "TClonesArray.h"
#include "TObjArray.h"
#include "TGraph.h"
#include "TVector3.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TF1.h"
#include "TMath.h"
#include "TLorentzVector.h"
#include "TProfile.h"
#include "TRandom3.h"
#include "TObject.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"
#include "FairGeoVolume.h"
#include "FairGeoTransform.h"
#include "FairGeoVector.h"
#include "FairGeoMedium.h"
#include "FairGeoNode.h"
#include "TDatabasePDG.h"

#include "CbmStsHit.h"
#include "CbmMvdHit.h"
#include "CbmMvdHitMatch.h"

#include "TString.h"
#include "TSystem.h"
#include "TStopwatch.h"
#include "L1Field.h"
#include "CbmL1PFFitter.h"


#include "CbmLmvmUtils.h"

#include <sstream>

using namespace std;

ClassImp(CbmAnaDielectronTask);


void CbmAnaDielectronTask::CreateAnalysisStepsH1(
      vector<TH1D*>& hist,
      const string& name,
      const string& axisX,
      const string& axisY,
      double nBins,
      double min,
      double max)
{
   string hname = "";
   hist.resize(CbmLmvmHist::fNofAnaSteps);
   for (Int_t i = 0; i < CbmLmvmHist::fNofAnaSteps; i++){
      hname = name + "_"+ CbmLmvmHist::fAnaSteps[i];
      hist[i] = new TH1D(hname.c_str(), hname.c_str(), nBins, min, max);
      hist[i]->GetXaxis()->SetTitle(axisX.c_str());
      hist[i]->GetYaxis()->SetTitle(axisY.c_str());
      fHistoList.push_back(hist[i]);
   }
}

void CbmAnaDielectronTask::CreateAnalysisStepsH2(
      vector<TH2D*>& hist,
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
   hist.resize(CbmLmvmHist::fNofAnaSteps);
   for (Int_t i = 0; i < CbmLmvmHist::fNofAnaSteps; i++){
      hname = name + "_"+ CbmLmvmHist::fAnaSteps[i];
      hist[i] = new TH2D(hname.c_str(), hname.c_str(), nBinsX, minX, maxX, nBinsY, minY, maxY);
      hist[i]->GetXaxis()->SetTitle(axisX.c_str());
      hist[i]->GetYaxis()->SetTitle(axisY.c_str());
      hist[i]->GetZaxis()->SetTitle(axisZ.c_str());
      fHistoList.push_back(hist[i]);
   }
}

void CbmAnaDielectronTask::CreateSourceTypesH1(
      vector<TH1D*>& hist,
      const string& name,
      const string& axisX,
      const string& axisY,
      double nBins,
      double min,
      double max)
{
   string hname = "";
   hist.resize(CbmLmvmHist::fNofSourceTypes);
   for (Int_t i = 0; i < CbmLmvmHist::fNofSourceTypes; i++){
      hname = name + "_"+ CbmLmvmHist::fSourceTypes[i];
      hist[i] = new TH1D(hname.c_str(), hname.c_str(), nBins, min, max);
      hist[i]->GetXaxis()->SetTitle(axisX.c_str());
      hist[i]->GetYaxis()->SetTitle(axisY.c_str());
      fHistoList.push_back(hist[i]);
   }
}

void CbmAnaDielectronTask::CreateSourceTypesH2(
      vector<TH2D*>& hist,
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
   hist.resize(CbmLmvmHist::fNofSourceTypes);
   for (Int_t i = 0; i < CbmLmvmHist::fNofSourceTypes; i++){
      hname = name + "_"+ CbmLmvmHist::fSourceTypes[i];
      hist[i] = new TH2D(hname.c_str(), hname.c_str(), nBinsX, minX, maxX, nBinsY, minY, maxY);
      hist[i]->GetXaxis()->SetTitle(axisX.c_str());
      hist[i]->GetYaxis()->SetTitle(axisY.c_str());
      hist[i]->GetZaxis()->SetTitle(axisZ.c_str());
      fHistoList.push_back(hist[i]);
   }
}

CbmAnaDielectronTask::CbmAnaDielectronTask()
  : FairTask("CbmAnaDielectronTask"),
    fMCTracks(NULL),
    fRichRings(NULL),
    fRichProj(NULL),
    fRichPoints(NULL),
    fRichRingMatches(NULL),
    fRichHits(NULL),
    fGlobalTracks(NULL),
    fStsTracks(NULL),
    fStsTrackMatches(NULL),
    fStsHits(NULL),
    fMvdHits(NULL),
    fMvdPoints(NULL),
    fMvdHitMatches(NULL),
    fTrdTracks(NULL),
    fTrdHits(NULL),
    fTrdTrackMatches(NULL),
    fTofHits(NULL),
    fTofPoints(NULL),
    fPrimVertex(NULL),
    fKFVertex(),
    fKFFitter(),
    fMCTrackCreator(NULL),
    fUseMvd(kFALSE),
    fUseRich(kTRUE),
    fUseTrd(kTRUE),
    fUseTof(kTRUE),
    fCandidates(),
    fSTCandidates(),
    fTTCandidates(),
    fRTCandidates(),
    fWeight(0.),
    fPionMisidLevel(-1.),
    fRandom3(new TRandom3(0)),
    fElIdAnn(NULL),
    fHistoList(),
    fNofHitsInRingMap(),
    fh_mc_mother_pdg(NULL),
    fh_acc_mother_pdg(NULL),
	fh_signal_pmtXY(NULL),
	fh_pi0_pmtXY(NULL),
	fh_gamma_pmtXY(NULL),
    fh_vertex_el_gamma_xz(),
    fh_vertex_el_gamma_yz(),
    fh_vertex_el_gamma_xy(),
    fh_vertex_el_gamma_rz(),
    fh_signal_minv(),
    fh_bg_minv(),
    fh_pi0_minv(),
    fh_eta_minv(),
    fh_gamma_minv(),
    fh_signal_mom(),
    fh_signal_pty(),
    fh_signal_minv_pt(),
    fh_eta_minv_pt(),
    fh_pi0_minv_pt(),
    fh_bg_truematch_minv(),
    fh_bg_truematch_el_minv(),
    fh_bg_truematch_notel_minv(),
    fh_bg_mismatch_minv(),
    fh_source_bg_minv(),
    fh_pt(),
    fh_mom(),
    fh_chi2sts(),
    fh_chi2prim(),
    fh_ttcut(),
    fh_stcut(),
    fh_rtcut(),
    fh_mvd1cut(),
    fh_mvd2cut(),
    fh_richann(),
    fh_trdann(),
    fh_tofm2(),
    fh_ttcut_pion(),
    fh_ttcut_truepair(),
    fh_stcut_pion(),
    fh_stcut_truepair(),
    fh_rtcut_pion(),
    fh_rtcut_truepair(),
    fh_nofMvdHits(),
    fh_nofStsHits(),
    fh_mvd1xy(),
    fh_mvd1r(),
    fh_mvd2xy(),
    fh_mvd2r(),   
    fh_mvd1cut_mc_dist_gamma(NULL),
    fh_mvd1cut_mc_dist_pi0(NULL),
    fh_mvd2cut_mc_dist_gamma(NULL),
    fh_mvd2cut_mc_dist_pi0(NULL),
    fh_mvd1cut_qa(),
    fh_mvd2cut_qa(),
    fh_source_pairs_epem(),
    fh_source_pairs(NULL),
    fh_event_number(NULL),
    fh_nof_bg_tracks(NULL),
    fh_nof_el_tracks(NULL),
    fh_nof_topology_pairs_gamma(NULL),
    fh_nof_topology_pairs_pi0(NULL),
    fh_source_tracks(NULL),
    fh_nof_rec_pairs_gamma(NULL),
    fh_nof_rec_pairs_pi0(NULL),
    fh_nof_rec_gamma(NULL),
    fh_nof_rec_pi0(NULL),
    fh_nof_mismatches(NULL),
    fh_nof_mismatches_rich(NULL),
    fh_nof_mismatches_trd(NULL),
    fh_nof_mismatches_tof(NULL),
    fh_nof_ghosts(NULL),
    fh_source_mom(),
    fh_source_pt(),
    fh_opening_angle(),
    fh_pi_mom_mc(NULL),
    fh_pi_mom_acc(NULL),
    fh_pi_mom_rec(NULL),
    fh_pi_mom_rec_only_sts(NULL),
    fh_pi_mom_rec_sts_rich_trd(NULL),
    fh_pi_mom_rec_sts_rich_trd_tof(NULL),
    fh_pi_rapidity_mc(NULL),
    fh_piprim_mom_mc(NULL),
    fh_piprim_mom_acc(NULL),
    fh_piprim_mom_rec(NULL),
    fh_piprim_mom_rec_only_sts(NULL),
    fh_piprim_mom_rec_sts_rich_trd(NULL),
    fh_piprim_mom_rec_sts_rich_trd_tof(NULL),
    fh_piprim_minus_rapidity_mc(NULL),
    fh_piprim_plus_rapidity_mc(NULL),
    fh_pi0prim_rapidity_mc(NULL),
    fh_etaprim_rapidity_mc(NULL)
{
   // weight for rho0 = 0.001081; omega_ee = 0.0026866; omega_dalitz = 0.02242; phi = 0.00039552; pi0 = 4.38   ------ for 25 GeV
   fWeight = 0.0;
   fUseRich = true;
   fUseTrd = true;
   fUseTof = true;
   fPionMisidLevel = -1.;
   fRandom3 = new TRandom3(0);

   fCuts.SetDefaultCuts();
}

CbmAnaDielectronTask::~CbmAnaDielectronTask()
{

}

void CbmAnaDielectronTask::InitHists()
{
   fHistoList.clear();

   // Mother PDG
   fh_mc_mother_pdg = new TH1D("fh_mc_mother_pdg", "fh_mc_mother_pdg; Pdg code; Particles per event", 7000, -3500., 3500.);
   fHistoList.push_back(fh_mc_mother_pdg);
   fh_acc_mother_pdg = new TH1D("fh_acc_mother_pdg","fh_acc_mother_pdg; Pdg code; Particles per event", 7000, -3500., 3500.);
   fHistoList.push_back(fh_acc_mother_pdg);

   //X-Y distribution of MC points on PMT
   fh_signal_pmtXY = new TH2D("fh_signal_pmtXY", "fh_signal_pmtXY;X [cm];Y [cm];Counter", 110, -110, 110, 200, -200, 200);
   fHistoList.push_back(fh_signal_pmtXY);
   fh_pi0_pmtXY = new TH2D("fh_pi0_pmtXY", "fh_pi0_pmtXY;X [cm];Y [cm];Counter", 110, -110, 110, 200, -200, 200);
   fHistoList.push_back(fh_pi0_pmtXY);
   fh_gamma_pmtXY = new TH2D("fh_gamma_pmtXY", "fh_gamma_pmtXY;X [cm];Y [cm];Counter", 110, -110, 110, 200, -200, 200);
   fHistoList.push_back(fh_gamma_pmtXY);

   //vertex of the secondary electrons from gamma conversion
   CreateAnalysisStepsH2(fh_vertex_el_gamma_xz, "fh_vertex_el_gamma_xz","Z [cm]", "X [cm]", "Counter per event", 200, -10., 190., 400, -130.,130.);
   CreateAnalysisStepsH2(fh_vertex_el_gamma_yz, "fh_vertex_el_gamma_yz","Z [cm]", "Y [cm]", "Counter per event", 200, -10., 190., 400, -130., 130.);
   CreateAnalysisStepsH2(fh_vertex_el_gamma_xy, "fh_vertex_el_gamma_xy","X [cm]", "Y [cm]", "Counter per event",  400, -130.,130., 400, -130., 130.);
   CreateAnalysisStepsH2(fh_vertex_el_gamma_rz, "fh_vertex_el_gamma_rz","Z [cm]", "#sqrt{X^{2}+Y^{2}} [cm]", "Counter per event",  300, -10., 190., 300, 0., 150.);

   // Number of BG and signal tracks after each cut
   fh_nof_bg_tracks = new TH1D("fh_nof_bg_tracks","fh_nof_bg_tracks;Analysis steps;Tracks/event", CbmLmvmHist::fNofAnaSteps, 0., CbmLmvmHist::fNofAnaSteps);
   fHistoList.push_back(fh_nof_bg_tracks);
   fh_nof_el_tracks = new TH1D("fh_nof_el_tracks","fh_nof_el_tracks;Analysis steps;Tracks/event", CbmLmvmHist::fNofAnaSteps, 0., CbmLmvmHist::fNofAnaSteps);
   fHistoList.push_back(fh_nof_el_tracks);
   fh_source_tracks = new TH2D("fh_source_tracks","fh_source_tracks;Analysis steps;Particle", CbmLmvmHist::fNofAnaSteps, 0., CbmLmvmHist::fNofAnaSteps, 7, 0., 7.);
   fHistoList.push_back(fh_source_tracks);

   fh_nof_topology_pairs_gamma = new TH1D("fh_nof_topology_pairs_gamma","fh_nof_topology_pairs_gamma;Pair type;Pairs/event", 8, 0., 8);
   fHistoList.push_back(fh_nof_topology_pairs_gamma);

   fh_nof_topology_pairs_pi0 = new TH1D("fh_nof_topology_pairs_pi0","fh_nof_topology_pairs_pi0;Pair type;Pairs/event", 8, 0., 8);
   fHistoList.push_back(fh_nof_topology_pairs_pi0);

   //Number of mismatches and ghosts after each cut
   fh_nof_mismatches = new TH1D("fh_nof_mismatches","fh_nof_mismatches;Analysis steps;Tracks/event", CbmLmvmHist::fNofAnaSteps, 0., CbmLmvmHist::fNofAnaSteps);
   fHistoList.push_back(fh_nof_mismatches);
   fh_nof_mismatches_rich = new TH1D("fh_nof_mismatches_rich","fh_nof_mismatches_rich;Analysis steps;Tracks/event", CbmLmvmHist::fNofAnaSteps, 0., CbmLmvmHist::fNofAnaSteps);
   fHistoList.push_back(fh_nof_mismatches_rich);
   fh_nof_mismatches_trd = new TH1D("fh_nof_mismatches_trd","fh_nof_mismatches_trd;Analysis steps;Tracks/event", CbmLmvmHist::fNofAnaSteps, 0., CbmLmvmHist::fNofAnaSteps);
   fHistoList.push_back(fh_nof_mismatches_trd);
   fh_nof_mismatches_tof = new TH1D("fh_nof_mismatches_tof","fh_nof_mismatches_tof;Analysis steps;Tracks/event", CbmLmvmHist::fNofAnaSteps, 0., CbmLmvmHist::fNofAnaSteps);
   fHistoList.push_back(fh_nof_mismatches_tof);
   fh_nof_ghosts = new TH1D("fh_nof_ghosts","fh_nof_ghosts;Analysis steps;Tracks/event", CbmLmvmHist::fNofAnaSteps, 0., CbmLmvmHist::fNofAnaSteps);
   fHistoList.push_back(fh_nof_ghosts);

   // BG pair source
   fh_source_pairs = new TH2D("fh_source_pairs","fh_source_pairs;Analysis steps;Pair", CbmLmvmHist::fNofAnaSteps, 0., CbmLmvmHist::fNofAnaSteps, CbmLmvmHist::fNofBgPairSources, 0., CbmLmvmHist::fNofBgPairSources);
   fHistoList.push_back(fh_source_pairs);

   // Event number counter
   fh_event_number = new TH1D("fh_event_number","fh_event_number", 1, 0, 1.);
   fHistoList.push_back(fh_event_number);

   CreateSourceTypesH1(fh_richann, "fh_richann", "ANN output", "Yield", 100, -1.1, 1.1);
   CreateSourceTypesH1(fh_trdann, "fh_trdann", "ANN output", "Yield", 100, -1.1, 1.1);
   CreateSourceTypesH2(fh_tofm2, "fh_tofm2", "P [GeV/c]", "m^{2} [GeV/c^{2}]^{2}", "Yield", 100, 0., 4., 600, -0.2, 1.2);

   // Distributions of analysis cuts.
   // Transverse momentum of tracks.
   CreateSourceTypesH1(fh_pt, "fh_pt", "P_{t} [GeV/c]", "Yield", 200, 0., 2.);
   // Momentum of tracks
   CreateSourceTypesH1(fh_mom, "fh_mom", "P [GeV/c]", "Yield", 200, 0., 10.);
   // Chi2 of the STS tracks
   CreateSourceTypesH1(fh_chi2sts, "fh_chi2sts", "#chi^{2}", "Yield", 200, 0., 20.);
   // Chi2 of the primary vertex
   CreateSourceTypesH1(fh_chi2prim, "fh_chi2prim", "#chi^{2}_{prim}", "Yield", 200, 0., 20.);
   // TT cut
   CreateSourceTypesH2(fh_ttcut, "fh_ttcut", "#sqrt{p_{e^{#pm}} p_{rec}} [GeV/c]", "#theta_{e^{+},e^{-}} [deg]", "Yield", 100, 0., 5., 100, 0., 5.);
   // ST cut
   CreateSourceTypesH2(fh_stcut, "fh_stcut", "#sqrt{p_{e^{#pm}} p_{rec}} [GeV/c]", "#theta_{e^{#pm},rec} [deg]", "Yield", 100, 0., 5., 100, 0., 5.);
   // RT cut
   CreateSourceTypesH2(fh_rtcut, "fh_rtcut", "#sqrt{p_{e^{#pm}} p_{rec}} [GeV/c]", "#theta_{e^{#pm},rec} [deg]", "Yield", 100, 0., 5., 100, 0., 5.);
   // MVD cut at the first station
   CreateSourceTypesH2(fh_mvd1cut, "fh_mvd1cut", "d_{MVD} [cm]", "P_{e} [GeV/c]", "Yield", 100, 0., 1., 100, 0., 5.);
   // MVD cut at the second station
   CreateSourceTypesH2(fh_mvd2cut, "fh_mvd2cut", "d_{MVD} [cm]", "P_{e} [GeV/c]", "Yield", 100, 0., 1., 100, 0., 5.);

   CreateSourceTypesH2(fh_ttcut_pion, "fh_ttcut_pion", "#sqrt{p_{e^{#pm}} p_{rec}} [GeV/c]", "#theta_{e^{+},e^{-}} [deg]", "Yield", 100, 0., 5., 100, 0., 5.);
   CreateSourceTypesH2(fh_ttcut_truepair, "fh_ttcut_truepair", "#sqrt{p_{e^{#pm}} p_{rec}} [GeV/c]", "#theta_{e^{+},e^{-}} [deg]", "Yield", 100, 0., 5., 100, 0., 5.);
   CreateSourceTypesH2(fh_stcut_pion, "fh_stcut_pion", "#sqrt{p_{e^{#pm}} p_{rec}} [GeV/c]", "#theta_{e^{+},e^{-}} [deg]", "Yield", 100, 0., 5., 100, 0., 5.);
   CreateSourceTypesH2(fh_stcut_truepair, "fh_stcut_truepair", "#sqrt{p_{e^{#pm}} p_{rec}} [GeV/c]", "#theta_{e^{+},e^{-}} [deg]", "Yield", 100, 0., 5., 100, 0., 5.);
   CreateSourceTypesH2(fh_rtcut_pion, "fh_rtcut_pion", "#sqrt{p_{e^{#pm}} p_{rec}} [GeV/c]", "#theta_{e^{+},e^{-}} [deg]", "Yield", 100, 0., 5., 100, 0., 5.);
   CreateSourceTypesH2(fh_rtcut_truepair, "fh_rtcut_truepair", "#sqrt{p_{e^{#pm}} p_{rec}} [GeV/c]", "#theta_{e^{+},e^{-}} [deg]", "Yield", 100, 0., 5., 100, 0., 5.);

   CreateSourceTypesH1(fh_nofMvdHits, "fh_nofMvdHits", "Number of hits in MVD", "Yield", 5, -0.5, 4.5);
   CreateSourceTypesH1(fh_nofStsHits, "fh_nofStsHits", "Number of hits in STS", "Yield", 9, -0.5, 8.5);
   CreateSourceTypesH2(fh_mvd1xy, "fh_mvd1xy", "X [cm]", "Y [cm]", "Yield", 60, -3., 3., 60, -3., 3.);
   CreateSourceTypesH1(fh_mvd1r, "fh_mvd1r", "#sqrt{X^{2}+Y^{2}} [cm]", "Yield", 60, 0., 3.);
   CreateSourceTypesH2(fh_mvd2xy, "fh_mvd2xy", "X [cm]", "Y [cm]", "Yield", 60, -6., 6., 60, -6., 6.);
   CreateSourceTypesH1(fh_mvd2r, "fh_mvd2r", "#sqrt{X^{2}+Y^{2}} [cm]", "Yield", 60, 0., 6.);

   // Check MVD cut quality. [0.5]-correct, [1.5]-wrong
   CreateSourceTypesH1(fh_mvd1cut_qa, "fh_mvd1cut_qa", "MVD hit assignment", "Yield", 2, 0., 2.);
   CreateSourceTypesH1(fh_mvd2cut_qa, "fh_mvd2cut_qa", "MVD hit assignment", "Yield", 2, 0., 2.);

   //Create invariant mass histograms
   CreateAnalysisStepsH1(fh_signal_minv, "fh_signal_minv", "M_{ee} [GeV/c^{2}]", "Yield", 4000, 0. , 4.);
   CreateAnalysisStepsH1(fh_bg_minv, "fh_bg_minv", "M_{ee} [GeV/c^{2}]", "Yield", 4000, 0. , 4.);
   CreateAnalysisStepsH1(fh_pi0_minv, "fh_pi0_minv", "M_{ee} [GeV/c^{2}]", "Yield", 4000, 0. , 4.);
   CreateAnalysisStepsH1(fh_eta_minv, "fh_eta_minv", "M_{ee} [GeV/c^{2}]", "Yield", 4000, 0. , 4.);
   CreateAnalysisStepsH1(fh_gamma_minv, "fh_gamma_minv", "M_{ee} [GeV/c^{2}]", "Yield", 4000, 0. , 4.);
   // minv for true matched and mismatched tracks
   CreateAnalysisStepsH1(fh_bg_truematch_minv, "fh_bg_truematch_minv", "M_{ee} [GeV/c^{2}]", "Yield", 4000, 0. , 4.);
   CreateAnalysisStepsH1(fh_bg_truematch_el_minv, "fh_bg_truematch_el_minv", "M_{ee} [GeV/c^{2}]", "Yield", 4000, 0. , 4.);
   CreateAnalysisStepsH1(fh_bg_truematch_notel_minv, "fh_bg_truematch_notel_minv", "M_{ee} [GeV/c^{2}]", "Yield", 4000, 0. , 4.);
   CreateAnalysisStepsH1(fh_bg_mismatch_minv, "fh_bg_mismatch_minv", "M_{ee} [GeV/c^{2}]", "Yield", 4000, 0. , 4.);
   // Minv for different sources
   fh_source_bg_minv.resize(CbmLmvmHist::fNofBgPairSources);
   for (int i = 0; i < CbmLmvmHist::fNofBgPairSources; i++){
      stringstream ss;
      ss << "fh_source_bg_minv_" << i;
      CreateAnalysisStepsH1(fh_source_bg_minv[i], ss.str(), "M_{ee} [GeV/c^{2}]", "Yield", 4000, 0. , 4.);
   }
   //Invariant mass vs. Mc Pt
   CreateAnalysisStepsH2(fh_signal_minv_pt, "fh_signal_minv_pt","M_{ee} [GeV/c^{2}]", "P_{t} [GeV/c]", "Yield", 100, 0., 4., 20, 0., 2.);
   CreateAnalysisStepsH2(fh_pi0_minv_pt, "fh_pi0_minv_pt", "M_{ee} [GeV/c^{2}]", "P_{t} [GeV/c]", "Yield", 100, 0., 4., 20, 0., 2.);
   CreateAnalysisStepsH2(fh_eta_minv_pt, "fh_eta_minv_pt", "M_{ee} [GeV/c^{2}]", "P_{t} [GeV/c]", "Yield", 100, 0., 4., 20, 0., 2.);

   // Momentum distribution of the signal
   CreateAnalysisStepsH1(fh_signal_mom, "fh_signal_mom", "P [GeV/c]", "Yield", 100, 0., 15.);
   //Pt/y distibution of the signal
   CreateAnalysisStepsH2(fh_signal_pty, "fh_signal_pty","Rapidity", "P_{t} [GeV/c]", "Yield", 40, 0., 4., 20, 0., 2.);
   //Sources pairs 2D
   CreateAnalysisStepsH2(fh_source_pairs_epem, "fh_source_pairs_epem","mother particle e+", "mother particle e-", "Yield", 3, 0., 3., 3, 0., 3.);

   fh_opening_angle.resize(CbmLmvmHist::fNofSourceTypes);
   fh_source_mom.resize(CbmLmvmHist::fNofSourceTypes);
   fh_source_pt.resize(CbmLmvmHist::fNofSourceTypes);
   for (Int_t i = 0; i < CbmLmvmHist::fNofSourceTypes; i++){
      fh_opening_angle[i].resize(CbmLmvmHist::fNofAnaSteps);
      fh_source_mom[i].resize(CbmLmvmHist::fNofAnaSteps);
      fh_source_pt[i].resize(CbmLmvmHist::fNofAnaSteps);
   }

   for (Int_t i = 0; i < CbmLmvmHist::fNofSourceTypes; i++){
      for (Int_t step = 0; step < CbmLmvmHist::fNofAnaSteps; step++){
         string hname = "", htitle = "";
         hname = "fh_opening_angle_" + CbmLmvmHist::fAnaSteps[step] + "_" + CbmLmvmHist::fSourceTypes[i];
         htitle = hname + ";#Theta_{1,2} [deg];Yield";
         fh_opening_angle[i][step] = new TH1D(hname.c_str(), htitle.c_str(), 160, 0., 80.);
         fHistoList.push_back(fh_opening_angle[i][step]);

         hname = "fh_source_mom_" + CbmLmvmHist::fAnaSteps[step] + "_" + CbmLmvmHist::fSourceTypes[i];
         htitle = hname + ";P [GeV/c];Yield";
         fh_source_mom[i][step] = new TH1D(hname.c_str(), htitle.c_str(), 300, 0., 15.);
         fHistoList.push_back(fh_source_mom[i][step]);

         hname = "fh_source_pt_" + CbmLmvmHist::fAnaSteps[step] + "_" + CbmLmvmHist::fSourceTypes[i];
         htitle = hname + ";P_{t} [GeV/c];Yield";
         fh_source_pt[i][step] = new TH1D(hname.c_str(), htitle.c_str(), 100, 0., 5.);
         fHistoList.push_back(fh_source_pt[i][step]);
      }
   }

   //pions vs momentum
   fh_pi_mom_mc = new TH1D("fh_pi_mom_mc", "fh_pi_mom_mc;p [GeV/c];dN/dP [1/GeV/c]", 30, 0., 3.);
   fHistoList.push_back(fh_pi_mom_mc);
   fh_pi_mom_acc = new TH1D("fh_pi_mom_acc", "fh_pi_mom_acc;p [GeV/c];dN/dP [1/GeV/c]", 30, 0., 3.);
   fHistoList.push_back(fh_pi_mom_acc);
   fh_pi_mom_rec = new TH1D("fh_pi_mom_rec", "fh_pi_mom_rec;p [GeV/c];dN/dP [1/GeV/c]", 30, 0., 3.);
   fHistoList.push_back(fh_pi_mom_rec);
   fh_pi_mom_rec_only_sts = new TH1D("fh_pi_mom_rec_only_sts", "fh_pi_mom_rec_only_sts;p [GeV/c];dN/dP [1/GeV/c]", 30, 0., 3.);
   fHistoList.push_back(fh_pi_mom_rec_only_sts);
   fh_pi_mom_rec_sts_rich_trd = new TH1D("fh_pi_mom_rec_sts_rich_trd", "fh_pi_mom_rec_sts_rich_trd;p [GeV/c];dN/dP [1/GeV/c]", 30, 0., 3.);
   fHistoList.push_back(fh_pi_mom_rec_sts_rich_trd);
   fh_pi_mom_rec_sts_rich_trd_tof = new TH1D("fh_pi_mom_rec_sts_rich_trd_tof", "fh_pi_mom_rec_sts_rich_trd_tof;p [GeV/c];dN/dP [1/GeV/c]", 30, 0., 3.);
   fHistoList.push_back(fh_pi_mom_rec_sts_rich_trd_tof);
   fh_pi_rapidity_mc = new TH1D("fh_pi_rapidity_mc", "fh_pi_rapidity_mc;Rapidity;dN/dY", 400, 0., 4.);
   fHistoList.push_back(fh_pi_rapidity_mc);


   //pions vs momentum for primary pions
   fh_piprim_mom_mc = new TH1D("fh_piprim_mom_mc", "fh_piprim_mom_mc;p [GeV/c];dN/dP [1/GeV/c]", 30, 0., 3.);
   fHistoList.push_back(fh_piprim_mom_mc);
   fh_piprim_mom_acc = new TH1D("fh_piprim_mom_acc", "fh_piprim_mom_acc;p [GeV/c];dN/dP [1/GeV/c]", 30, 0., 3.);
   fHistoList.push_back(fh_piprim_mom_acc);
   fh_piprim_mom_rec = new TH1D("fh_piprim_mom_rec", "fh_piprim_mom_rec;p [GeV/c];dN/dP [1/GeV/c]", 30, 0., 3.);
   fHistoList.push_back(fh_piprim_mom_rec);
   fh_piprim_mom_rec_only_sts = new TH1D("fh_piprim_mom_rec_only_sts", "fh_piprim_mom_rec_only_sts;p [GeV/c];dN/dP [1/GeV/c]", 30, 0., 3.);
   fHistoList.push_back(fh_piprim_mom_rec_only_sts);
   fh_piprim_mom_rec_sts_rich_trd = new TH1D("fh_piprim_mom_rec_sts_rich_trd", "fh_piprim_mom_rec_sts_rich_trd;p [GeV/c];dN/dP [1/GeV/c]", 30, 0., 3.);
   fHistoList.push_back(fh_piprim_mom_rec_sts_rich_trd);
   fh_piprim_mom_rec_sts_rich_trd_tof = new TH1D("fh_piprim_mom_rec_sts_rich_trd_tof", "fh_piprim_mom_rec_sts_rich_trd_tof;p [GeV/c];dN/dP [1/GeV/c]", 30, 0., 3.);
   fHistoList.push_back(fh_piprim_mom_rec_sts_rich_trd_tof);

   fh_piprim_plus_rapidity_mc = new TH1D("fh_piprim_plus_rapidity_mc", "fh_piprim_plus_rapidity_mc;Rapidity;dN/dY", 400, 0., 4.);
   fHistoList.push_back(fh_piprim_plus_rapidity_mc);
   fh_piprim_minus_rapidity_mc = new TH1D("fh_piprim_minus_rapidity_mc", "fh_piprim_minus_rapidity_mc;Rapidity;dN/dY", 400, 0., 4.);
   fHistoList.push_back(fh_piprim_minus_rapidity_mc);
   fh_pi0prim_rapidity_mc = new TH1D("fh_pi0prim_rapidity_mc", "fh_pi0prim_rapidity_mc;Rapidity;dN/dY", 400, 0., 4.);
   fHistoList.push_back(fh_pi0prim_rapidity_mc);
   fh_etaprim_rapidity_mc = new TH1D("fh_etaprim_rapidity_mc", "fh_etaprim_rapidity_mc;Rapidity;dN/dY", 400, 0., 4.);
   fHistoList.push_back(fh_etaprim_rapidity_mc);


   fh_nof_rec_pairs_gamma = new TH1D("fh_nof_rec_pairs_gamma", "fh_nof_rec_pairs_gamma;Pair category; Number per event", 3, -0.5, 2.5);
   fHistoList.push_back(fh_nof_rec_pairs_gamma);
   fh_nof_rec_pairs_pi0 = new TH1D("fh_nof_rec_pairs_pi0", "fh_nof_rec_pairs_pi0;Pair category; Number per event", 3, -0.5, 2.5);
   fHistoList.push_back(fh_nof_rec_pairs_pi0);

   fh_nof_rec_gamma = new TH1D("fh_nof_rec_gamma", "fh_nof_rec_gamma;Track category; Number per event", 3, -0.5, 2.5);
   fHistoList.push_back(fh_nof_rec_gamma);
   fh_nof_rec_pi0 = new TH1D("fh_nof_rec_pi0", "fh_nof_rec_pi0;Track category; Number per event", 3, -0.5, 2.5);
   fHistoList.push_back(fh_nof_rec_pi0);
}

InitStatus CbmAnaDielectronTask::Init()
{
	cout << "InitStatus CbmAnaDielectronTask::Init"<< endl;

	FairRootManager* ioman = FairRootManager::Instance();
	if (NULL == ioman) { Fatal("CbmAnaDielectronTask::Init","No FairRootManager!"); }

	fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
	if (NULL == fMCTracks) { Fatal("CbmAnaDielectronTask::Init","No MCTrack array!"); }

	if (fUseRich == true){
		fRichHits = (TClonesArray*) ioman->GetObject("RichHit");
		if (NULL == fRichHits) { Fatal("CbmAnaDielectronTask::Init","No RichHit array!"); }

		fRichRings = (TClonesArray*) ioman->GetObject("RichRing");
		if (NULL == fRichRings) { Fatal("CbmAnaDielectronTask::Init","No RichRing array!"); }

		fRichPoints = (TClonesArray*) ioman->GetObject("RichPoint");
		if (NULL == fRichPoints) { Fatal("CbmAnaDielectronTask::Init","No RichPoint array!"); }

		fRichRingMatches = (TClonesArray*) ioman->GetObject("RichRingMatch");
		if (NULL == fRichRingMatches) { Fatal("CbmAnaDielectronTask::Init","No RichRingMatch array!"); }

		fRichProj = (TClonesArray*) ioman->GetObject("RichProjection");
		if (NULL == fRichProj) { Fatal("CbmAnaDielectronTask::Init","No RichProjection array!"); }
	}//fUseRich

	fStsTrackMatches = (TClonesArray*) ioman->GetObject("StsTrackMatch");
	if (NULL == fStsTrackMatches) { Fatal("CbmAnaDielectronTask::Init","No StsTrackMatch array!"); }

	fStsTracks = (TClonesArray*) ioman->GetObject("StsTrack");
	if (NULL == fStsTracks) { Fatal("CbmAnaDielectronTask::Init","No StsTrack array!"); }

   fStsHits = (TClonesArray*) ioman->GetObject("StsHit");
   if (NULL == fStsHits) { Fatal("CbmAnaDielectronTask::Init","No StsHit array!"); }

   if (fUseMvd){
      fMvdHits = (TClonesArray*) ioman->GetObject("MvdHit");
      if (NULL == fMvdHits) { Fatal("CbmAnaDielectronTask::Init","No MvdHit array!"); }

      fMvdPoints = (TClonesArray*) ioman->GetObject("MvdPoint");
      if (NULL == fMvdPoints) { Fatal("CbmAnaDielectronTask::Init",": No MvdPoint array!"); }

      fMvdHitMatches = (TClonesArray*) ioman->GetObject("MvdHitMatch");
      if (NULL == fMvdHitMatches) { Fatal("CbmAnaDielectronTask::Init",": No MvdHitMatch array!"); }
   }

	fGlobalTracks = (TClonesArray*) ioman->GetObject("GlobalTrack");
	if (NULL == fGlobalTracks) { Fatal("CbmAnaDielectronTask::Init","No GlobalTrack array!"); }

	if (fUseTrd == true){
		fTrdTracks = (TClonesArray*) ioman->GetObject("TrdTrack");
		if (NULL == fTrdTracks) { Fatal("CbmAnaDielectronTask::Init","No TrdTrack array!"); }

		fTrdTrackMatches = (TClonesArray*) ioman->GetObject("TrdTrackMatch");
		if (NULL == fTrdTrackMatches) { Fatal("CbmAnaDielectronTask::Init","No TrdTrackMatch array!"); }
	}//fUseTrd

	if (fUseTof == true){
		fTofPoints = (TClonesArray*) ioman->GetObject("TofPoint");
		if (NULL == fTofPoints) { Fatal("CbmAnaDielectronTask::Init","No TofPoint array!"); }

		fTofHits = (TClonesArray*) ioman->GetObject("TofHit");
		if (NULL == fTofHits) { Fatal("CbmAnaDielectronTask::Init","No TofHit array!"); }
   }//fUseTof

   fPrimVertex = (CbmVertex*) ioman->GetObject("PrimaryVertex");
   if (NULL == fPrimVertex) { Fatal("CbmAnaDielectronTask::Init","No PrimaryVertex array!"); }

   InitHists();

   fKFFitter.Init();

   fMCTrackCreator = CbmLitMCTrackCreator::Instance();

   if (fCuts.fUseRichAnn){
      fElIdAnn = new CbmRichElectronIdAnn();
      fElIdAnn->Init();
   }

   // if TRD detector us not used the momentum cut at 5.5GeV/c are used
   if (!fUseTrd){
      fCuts.fMomentumCut = 5.5;
   }

   return kSUCCESS;
}

void CbmAnaDielectronTask::Exec(
      Option_t *option)
{
    fh_event_number->Fill(0.5);

    cout << "-I- CbmAnaDielectronTask, event number " << fh_event_number->GetEntries() << endl;
    fCuts.Print();
    cout << "fWeight = " << fWeight << endl;

    if (fPrimVertex != NULL){
       fKFVertex = CbmKFVertex(*fPrimVertex);
    } else {
       Fatal("CbmAnaDielectronTask::Exec","No PrimaryVertex array!");
    }

    fMCTrackCreator->Create();

    FillRichRingNofHits();
    MCPairs();   
    RichPmtXY();
    SingleParticleAcceptance();
    PairMcAndAcceptance();
    FillTopologyCandidates();
    FillCandidates();
    CalculateNofTopologyPairs(fh_nof_topology_pairs_gamma, "gamma");
    CalculateNofTopologyPairs(fh_nof_topology_pairs_pi0, "pi0");
    DifferenceSignalAndBg();
    SignalAndBgReco();
    FillElPiMomHist();
}// Exec

void CbmAnaDielectronTask::FillRichRingNofHits()
{
	fNofHitsInRingMap.clear();
    Int_t nofRichHits = fRichHits->GetEntriesFast();
    for (Int_t iHit=0; iHit < nofRichHits; iHit++) {
        CbmRichHit* hit = static_cast<CbmRichHit*>(fRichHits->At(iHit));
        if (NULL == hit) continue;

        Int_t iPoint = hit->GetRefId();
        if (iPoint < 0) continue;

        FairMCPoint* point = static_cast<FairMCPoint*>(fRichPoints->At(iPoint));
        if (NULL == point) continue;

        Int_t iMCTrack = point->GetTrackID();
        CbmMCTrack* track = static_cast<CbmMCTrack*>(fMCTracks->At(iMCTrack));
        if (NULL == track) continue;

        Int_t iMother = track->GetMotherId();
        if (iMother == -1) continue;

        fNofHitsInRingMap[iMother]++;
    }
}

void CbmAnaDielectronTask::MCPairs()
{
    Int_t nMcTracks = fMCTracks->GetEntries();
    for (Int_t i = 0; i < nMcTracks; i++){
        CbmMCTrack* mctrack = (CbmMCTrack*) fMCTracks->At(i);
        Int_t motherId = mctrack->GetMotherId();
        Int_t pdg = TMath::Abs(mctrack->GetPdgCode());
        Double_t mom = mctrack->GetP();

        // mother pdg of e-/e+
        Int_t mcMotherPdg = 0;
        if (pdg == 11) {
           // momentum distribution for electrons from signal
           if (motherId == -1) fh_source_mom[kSignal][kMc]->Fill(mom, fWeight);

           // secondary electrons
           if (motherId != -1){
                CbmMCTrack* mother = (CbmMCTrack*) fMCTracks->At(motherId);
                if (NULL != mother) mcMotherPdg = mother->GetPdgCode();
                // vertex of secondary electron from gamma conversion
                if (mcMotherPdg == 22) {
                    TVector3 v;
                    mctrack->GetStartVertex(v);
                    fh_vertex_el_gamma_xz[kMc]->Fill(v.Z(),v.X());
                    fh_vertex_el_gamma_yz[kMc]->Fill(v.Z(),v.Y());
                    fh_vertex_el_gamma_xy[kMc]->Fill(v.X(),v.Y());
                    fh_vertex_el_gamma_rz[kMc]->Fill( v.Z(), sqrt(v.X()*v.X()+v.Y()*v.Y()) );
                }
            }else {
                mcMotherPdg = 0;
            }
            fh_mc_mother_pdg->Fill(mcMotherPdg);
        }
    } // nMcTracks
} //MC Pairs

void CbmAnaDielectronTask::RichPmtXY()
{
    Int_t nofRichHits = fRichHits->GetEntriesFast();
    for (Int_t iH=0; iH < nofRichHits; iH++) {
    	CbmRichHit* richHit = static_cast<CbmRichHit*>(fRichHits->At(iH));
    	if ( richHit == NULL ) continue;
    	Int_t pointInd =  richHit->GetRefId();
    	if (pointInd < 0) continue;

        FairMCPoint* pointPhoton = static_cast<FairMCPoint*>(fRichPoints->At(pointInd));
        if (NULL == pointPhoton) continue;

        Int_t iMCTrackPhoton = pointPhoton->GetTrackID();
        CbmMCTrack* trackPhoton = static_cast<CbmMCTrack*>(fMCTracks->At(iMCTrackPhoton));
        if (NULL == trackPhoton) continue;

        Int_t iMCTrack = trackPhoton->GetMotherId();
        if (iMCTrack == -1) continue;

        CbmMCTrack* mctrack = static_cast<CbmMCTrack*>(fMCTracks->At(iMCTrack));
        if (NULL == mctrack) continue;

        Int_t pdg = TMath::Abs(mctrack->GetPdgCode());
        Int_t motherId = mctrack->GetMotherId();
        TVector3 v;
		mctrack->GetStartVertex(v);
		Bool_t isPrim = (v.Z() < 2.);

		if (pdg == 11 && motherId == -1) { //signal
			fh_signal_pmtXY->Fill(richHit->GetX(), richHit->GetY(), fWeight);
		}
		if (motherId >=0 && isPrim){

			CbmMCTrack* mct1 = (CbmMCTrack*) fMCTracks->At(motherId);
			int motherPdg = mct1->GetPdgCode();
			if (mct1 != NULL && motherPdg == 111 && pdg == 11) { //pi0
				fh_pi0_pmtXY->Fill(richHit->GetX(), richHit->GetY());
			}
			if (mct1 != NULL && motherPdg == 22 && pdg == 11){ //gamma
				fh_gamma_pmtXY->Fill(richHit->GetX(), richHit->GetY());
			}
		}
    }
}


Bool_t CbmAnaDielectronTask::IsMcTrackAccepted(
		Int_t mcTrackInd)
{
	CbmMCTrack* tr = (CbmMCTrack*) fMCTracks->At(mcTrackInd);
	if (tr == NULL) return false;
	Int_t nRichPoints = fNofHitsInRingMap[mcTrackInd];
	return (tr->GetNPoints(kMVD) + tr->GetNPoints(kSTS) >= 4 && nRichPoints >= 7 && tr->GetNPoints(kTRD) >= 2 && tr->GetNPoints(kTOF) > 0) ;
}

void CbmAnaDielectronTask::SingleParticleAcceptance()
{
    Int_t nMcTracks = fMCTracks->GetEntries();
    for (Int_t i = 0; i < nMcTracks; i++) {
        CbmMCTrack* mctrack = (CbmMCTrack*) fMCTracks->At(i);
        Int_t motherId = mctrack->GetMotherId();
        Int_t pdg = TMath::Abs(mctrack->GetPdgCode());
        Int_t nMvdPoints = mctrack->GetNPoints(kMVD);
        Int_t nStsPoints = mctrack->GetNPoints(kSTS);
        Int_t nRichPoints = fNofHitsInRingMap[i];

        Bool_t isAcc = ( nMvdPoints+nStsPoints >= 4 && nRichPoints >= 7);

        Int_t mcMotherPdg = 0;
        if (pdg == 11 && isAcc) {
           if (motherId != -1){
               CbmMCTrack* mother = (CbmMCTrack*) fMCTracks->At(motherId);
               if (NULL != mother) mcMotherPdg = mother->GetPdgCode();
               // vertex of secondary electron from gamma conversion
               if (mcMotherPdg == 22) {
                  TVector3 v;
                  mctrack->GetStartVertex(v);
                  fh_vertex_el_gamma_xz[kAcc]->Fill(v.Z(),v.X());
                  fh_vertex_el_gamma_yz[kAcc]->Fill(v.Z(),v.Y());
                  fh_vertex_el_gamma_xy[kAcc]->Fill(v.X(),v.Y());
                  fh_vertex_el_gamma_rz[kAcc]->Fill( v.Z(), sqrt(v.X()*v.X()+v.Y()*v.Y()) );
               }

            }else {
                mcMotherPdg = 0;
            }
            fh_acc_mother_pdg->Fill(mcMotherPdg);
        }
    }
}

void CbmAnaDielectronTask::PairMcAndAcceptance()
{
	Int_t nMcTracks = fMCTracks->GetEntries();
	for (Int_t iP = 0; iP < nMcTracks; iP++) {
		CbmMCTrack* mctrackP = (CbmMCTrack*) fMCTracks->At(iP);
		Int_t motherIdP = mctrackP->GetMotherId();
		Int_t pdgP = mctrackP->GetPdgCode();
		if ( pdgP != 11 ) continue;
		Bool_t isAccP = IsMcTrackAccepted(iP);
		for (Int_t iM = 0; iM < nMcTracks; iM++) {
			if (iP == iM) continue;
			CbmMCTrack* mctrackM = (CbmMCTrack*) fMCTracks->At(iM);
			Int_t motherIdM = mctrackM->GetMotherId();
			Int_t pdgM = mctrackM->GetPdgCode();
			if ( pdgM != -11 ) continue;
			Bool_t isAccM = IsMcTrackAccepted(iM);
			CbmLmvmKinematicParams p = CbmLmvmKinematicParams::KinematicParamsWithMcTracks(mctrackP,mctrackM);

			// e+/- from signal
			if (motherIdM == -1 && pdgM == -11 && motherIdP == -1 && pdgP == 11) {
				fh_signal_pty[kMc]->Fill(p.fRapidity,p.fPt, fWeight);
				fh_signal_mom[kMc]->Fill(p.fMomentumMag, fWeight);
				fh_signal_minv[kMc]->Fill(p.fMinv, fWeight);

				if (isAccP && isAccM) {
					fh_signal_pty[kAcc]->Fill(p.fRapidity, p.fPt, fWeight);
					fh_signal_mom[kAcc]->Fill(p.fMomentumMag, fWeight);
					fh_signal_minv[kAcc]->Fill(p.fMinv, fWeight);
				}
			}

			if (motherIdP >=0 && motherIdM >=0){
			    CbmMCTrack* mct1P = (CbmMCTrack*) fMCTracks->At(motherIdP);
			    Int_t motherPdgP = mct1P->GetPdgCode();

			    CbmMCTrack* mct1M = (CbmMCTrack*) fMCTracks->At(motherIdM);
			    Int_t motherPdgM = mct1M->GetPdgCode();

			    Bool_t isPi0 = (motherPdgP == 111 && pdgP == 11 && motherPdgM == 111 && pdgM == -11 && motherIdP == motherIdM);
			    Bool_t isEta = (motherPdgP == 221 && pdgP == 11 && motherPdgM == 221 && pdgM == -11 && motherIdP == motherIdM);

			    if (isPi0) fh_pi0_minv[kMc]->Fill(p.fMinv);
			    if (isEta) fh_eta_minv[kMc]->Fill(p.fMinv);
			    if (isAccP && isAccM && isPi0) fh_pi0_minv[kAcc]->Fill(p.fMinv);
			    if (isAccP && isAccM && isEta) fh_eta_minv[kAcc]->Fill(p.fMinv);
			}
		}//iM
	}//iP
} // PairsAcceptance


void CbmAnaDielectronTask::FillElPiMomHist()
{
   Int_t nMcTracks = fMCTracks->GetEntries();
   for (Int_t i = 0; i < nMcTracks; i++) {
       CbmMCTrack* mctrack = (CbmMCTrack*) fMCTracks->At(i);
       Int_t motherId = mctrack->GetMotherId();
       Int_t pdg = TMath::Abs(mctrack->GetPdgCode());
       double momentum = mctrack->GetP();
       double rapidity = mctrack->GetRapidity();
       Int_t nMvdPoints = mctrack->GetNPoints(kMVD);
       Int_t nStsPoints = mctrack->GetNPoints(kSTS);
       Bool_t isAcc = ( nMvdPoints+nStsPoints >= 4);
       TVector3 vertex;
       mctrack->GetStartVertex(vertex);

       if (pdg == 211){
          fh_pi_mom_mc->Fill(momentum);
          fh_pi_rapidity_mc->Fill(rapidity);
          if (isAcc) fh_pi_mom_acc->Fill(momentum);

          if (vertex.Mag() < 0.1) {
             fh_piprim_mom_mc->Fill(momentum);
             if (mctrack->GetPdgCode() == 211) fh_piprim_plus_rapidity_mc->Fill(rapidity);
             if (mctrack->GetPdgCode() == -211) fh_piprim_minus_rapidity_mc->Fill(rapidity);
             if (isAcc) fh_piprim_mom_acc->Fill(momentum);
          }
       }

       if (pdg == 111 && vertex.Mag() < 0.1) {
          fh_pi0prim_rapidity_mc->Fill(rapidity);
       }

       if (pdg == 221 && vertex.Mag() < 0.1) {
          fh_etaprim_rapidity_mc->Fill(rapidity);
       }
   }

   Int_t ngTracks = fGlobalTracks->GetEntriesFast();
   for (Int_t i = 0; i < ngTracks; i++) {
       CbmGlobalTrack* gTrack = (CbmGlobalTrack*) fGlobalTracks->At(i);
       if(NULL == gTrack) continue;
       int stsInd = gTrack->GetStsTrackIndex();
       int richInd = gTrack->GetRichRingIndex();
       int trdInd = gTrack->GetTrdTrackIndex();
       int tofInd = gTrack->GetTofHitIndex();

       if (stsInd < 0) continue;
       CbmStsTrack* stsTrack = (CbmStsTrack*) fStsTracks->At(stsInd);
       if (stsTrack == NULL) continue;
       CbmTrackMatchNew* stsMatch  = (CbmTrackMatchNew*) fStsTrackMatches->At(stsInd);
       if (stsMatch == NULL) continue;
       int stsMcTrackId = stsMatch->GetMatchedLink().GetIndex();
       if (stsMcTrackId < 0) continue;
       CbmMCTrack* mcTrack1 = (CbmMCTrack*) fMCTracks->At(stsMcTrackId);
       if (mcTrack1 == NULL) continue;
       int pdg = TMath::Abs(mcTrack1->GetPdgCode());
       int motherId = mcTrack1->GetMotherId();
       double momentum = mcTrack1->GetP();

       TVector3 vertex;
       mcTrack1->GetStartVertex(vertex);

       if (pdg == 211) {
          fh_pi_mom_rec->Fill(momentum);
          if (richInd < 0 && trdInd < 0 && tofInd < 0) {
             fh_pi_mom_rec_only_sts->Fill(momentum);
          }
          if (richInd >= 0 && trdInd >= 0) {
               fh_pi_mom_rec_sts_rich_trd->Fill(momentum);
           }
          if (richInd >= 0 && trdInd >= 0 && tofInd >= 0) {
             fh_pi_mom_rec_sts_rich_trd_tof->Fill(momentum);
          }

          if (vertex.Mag() < 0.1) {
             fh_piprim_mom_rec->Fill(momentum);
             if (richInd < 0 && trdInd < 0 && tofInd < 0) {
                fh_piprim_mom_rec_only_sts->Fill(momentum);
             }
             if (richInd >= 0 && trdInd >= 0) {
                  fh_piprim_mom_rec_sts_rich_trd->Fill(momentum);
              }
             if (richInd >= 0 && trdInd >= 0 && tofInd >= 0) {
                fh_piprim_mom_rec_sts_rich_trd_tof->Fill(momentum);
             }
          }
       }
   }//gTracks
}


void CbmAnaDielectronTask::FillTopologyCandidates()
{
    fSTCandidates.clear();
    fRTCandidates.clear();
    Int_t ngTracks = fGlobalTracks->GetEntriesFast();

    for (Int_t i = 0; i < ngTracks; i++) {
    	CbmLmvmCandidate cand;
    
        CbmGlobalTrack* gTrack  = (CbmGlobalTrack*) fGlobalTracks->At(i);
        if(NULL == gTrack) continue;

        cand.fStsInd = gTrack->GetStsTrackIndex();
        if (cand.fStsInd < 0) continue;
        CbmStsTrack* stsTrack = (CbmStsTrack*) fStsTracks->At(cand.fStsInd);
        if (stsTrack == NULL) continue;

        cand.fRichInd = gTrack->GetRichRingIndex();
        cand.fTrdInd = gTrack->GetTrdTrackIndex();
        cand.fTofInd = gTrack->GetTofHitIndex();

        CbmLmvmUtils::CalculateAndSetTrackParamsToCandidate(&cand, stsTrack, fKFVertex);

        // select tracks from vertex
        if (cand.fChi2Prim > fCuts.fChiPrimCut) continue;

        // ST cut candidates, only STS
        if (cand.fRichInd < 0 && cand.fTrdInd < 0 && cand.fTofInd < 0) fSTCandidates.push_back(cand);

        // RT cut candidates, STS + at least one detector (RICH, TRD, TOF) but not all
        // Candidates must be identified as electron in registered detectors:
        // if it is registered in RICH it must be identified in the RICH as electron
        // RICH
        Bool_t isRichRT = (cand.fRichInd < 0)?false:true;
        if ( isRichRT ){
           CbmRichRing* richRing = (CbmRichRing*) fRichRings->At(cand.fRichInd);
           if (richRing == NULL) isRichRT = false;
           if ( isRichRT ) isRichRT = IsRichElectron(richRing, cand.fMomentum.Mag(), &cand);
        }

         // TRD
        Bool_t isTrdRT = (cand.fTrdInd < 0)?false:true;
        if ( isTrdRT ) isTrdRT = fUseTrd;
        if ( isTrdRT ) {
           CbmTrdTrack* trdTrack = (CbmTrdTrack*) fTrdTracks->At(cand.fTrdInd);
           if (trdTrack == NULL) isTrdRT = false;
           if ( isTrdRT ) isTrdRT = IsTrdElectron(trdTrack, &cand);
         }

         // ToF
        Bool_t isTofRT = (cand.fTofInd < 0)?false:true;
        if (isTofRT) {
           CbmTofHit* tofHit = (CbmTofHit*) fTofHits->At(cand.fTofInd);
           if (tofHit == NULL) isTofRT = false;
           if (isTofRT) isTofRT = IsTofElectron(gTrack, cand.fMomentum.Mag(), &cand);
        }

        if (isRichRT || isTrdRT || isTofRT ) {
           if ( !(cand.fRichInd >= 0 && cand.fTrdInd >= 0 && cand.fTofInd >= 0) ) {
              fRTCandidates.push_back(cand);
           }
        }
    }//gTracks
    cout << "fSTCandidates.size() = " << fSTCandidates.size() << endl;
    cout << "fRTCandidates.size() = " << fRTCandidates.size() << endl;

    AssignMcToTopologyCandidates(fSTCandidates);
    AssignMcToTopologyCandidates(fRTCandidates);
}

void CbmAnaDielectronTask::FillCandidates()
{
   fCandidates.clear();
   fTTCandidates.clear();
   Int_t nGTracks = fGlobalTracks->GetEntriesFast();
   fCandidates.reserve(nGTracks);

   for (Int_t i = 0; i < nGTracks; i++){
	  CbmLmvmCandidate cand;
      cand.fIsElectron = false;
      cand.fIsGamma = false;
      cand.fIsStCutElectron = false;
      cand.fIsTtCutElectron = false;
      cand.fIsRtCutElectron = false;
      cand.fIsMvd1CutElectron = true;
      cand.fIsMvd2CutElectron = true;

      CbmGlobalTrack* gTrack  = (CbmGlobalTrack*) fGlobalTracks->At(i);
      if(NULL == gTrack) continue;
      // STS
      cand.fStsInd = gTrack->GetStsTrackIndex();
      if (cand.fStsInd < 0) continue;
      CbmStsTrack* stsTrack = (CbmStsTrack*) fStsTracks->At(cand.fStsInd);
      if (stsTrack == NULL) continue;

      CbmLmvmUtils::CalculateAndSetTrackParamsToCandidate(&cand, stsTrack, fKFVertex);

      // Add all pions from STS for pion misidentification level study
      if (fPionMisidLevel >= 0.0){
    	  CbmTrackMatchNew* stsMatch  = (CbmTrackMatchNew*) fStsTrackMatches->At(cand.fStsInd);
         if (stsMatch == NULL) continue;
         cand.fStsMcTrackId = stsMatch->GetMatchedLink().GetIndex();
         if (cand.fStsMcTrackId < 0) continue;
         CbmMCTrack* mcTrack1 = (CbmMCTrack*) fMCTracks->At(cand.fStsMcTrackId);
         if (mcTrack1 == NULL) continue;
         Int_t pdg = TMath::Abs(mcTrack1->GetPdgCode());

         //check that pion track has track projection onto the photodetector plane
         const FairTrackParam* richProjection = (FairTrackParam*)(fRichProj->At(i));
         if (richProjection == NULL || richProjection->GetX() == 0 || richProjection->GetY() == 0) continue;

         if (pdg == 211){
            IsElectron(NULL, cand.fMomentum.Mag(), NULL, gTrack, &cand);
            fCandidates.push_back(cand);
            continue;
         }
      }

      // RICH
      cand.fRichInd = gTrack->GetRichRingIndex();
      if (cand.fRichInd < 0) continue;
      CbmRichRing* richRing = (CbmRichRing*) fRichRings->At(cand.fRichInd);

      // TRD
      CbmTrdTrack* trdTrack = NULL;
      if (fUseTrd == true) {
         cand.fTrdInd = gTrack->GetTrdTrackIndex();
         if (cand.fTrdInd < 0) continue;
         trdTrack = (CbmTrdTrack*) fTrdTracks->At(cand.fTrdInd);
         if (trdTrack == NULL) continue;
      }

      // ToF
      cand.fTofInd = gTrack->GetTofHitIndex();
      if (cand.fTofInd < 0) continue;
      CbmTofHit* tofHit = (CbmTofHit*) fTofHits->At(cand.fTofInd);
      if (tofHit == NULL) continue;

      IsElectron(richRing, cand.fMomentum.Mag(), trdTrack, gTrack, &cand);

      fCandidates.push_back(cand);
      if (!cand.fIsElectron && cand.fChi2Prim < fCuts.fChiPrimCut) fTTCandidates.push_back(cand);

   }// global tracks
   cout << "fCandidates.size() = " << fCandidates.size() << endl;
   cout << "fTTCandidates.size() = " << fTTCandidates.size() << endl;

   AssignMcToCandidates();
   AssignMcToTopologyCandidates(fTTCandidates);
}

void CbmAnaDielectronTask::AssignMcToCandidates()
{
   int nCand = fCandidates.size();
   for (int i = 0; i < nCand; i++){
      fCandidates[i].ResetMcParams();

      //STS
      //MCTrackId of the candidate is defined by STS track
      int stsInd = fCandidates[i].fStsInd;
      CbmTrackMatchNew* stsMatch  = (CbmTrackMatchNew*) fStsTrackMatches->At(stsInd);
      if (stsMatch == NULL) continue;
      fCandidates[i].fStsMcTrackId = stsMatch->GetMatchedLink().GetIndex();
      if (fCandidates[i].fStsMcTrackId < 0) continue;
      CbmMCTrack* mcTrack1 = (CbmMCTrack*) fMCTracks->At(fCandidates[i].fStsMcTrackId);
      if (mcTrack1 == NULL) continue;
      int pdg = TMath::Abs(mcTrack1->GetPdgCode());
      int motherId = mcTrack1->GetMotherId();
      fCandidates[i].fMcMotherId = motherId;
      fCandidates[i].fMcPdg = pdg;

      if (pdg == 211 && fPionMisidLevel >= 0.) continue;

      // RICH
      int richInd = fCandidates[i].fRichInd;
      CbmTrackMatchNew* richMatch  = (CbmTrackMatchNew*) fRichRingMatches->At(richInd);
      if (richMatch == NULL) continue;
      fCandidates[i].fRichMcTrackId = richMatch->GetMatchedLink().GetIndex();
      //if (fCandidates[i].richMcTrackId < 0) continue;
      //CbmMCTrack* mcTrack2 = (CbmMCTrack*) fMCTracks->At(fCandidates[i].richMcTrackId);
      //if (mcTrack2 == NULL) continue;

      if (pdg == 11 && motherId == -1) fCandidates[i].fIsMcSignalElectron = true;

      if (motherId >=0){
         CbmMCTrack* mct1 = (CbmMCTrack*) fMCTracks->At(motherId);
         int motherPdg = mct1->GetPdgCode();
         if (mct1 != NULL && motherPdg == 111 && pdg == 11) {
            fCandidates[i].fIsMcPi0Electron = true;
         }
         if (mct1 != NULL && motherPdg == 22 && pdg == 11){
            fCandidates[i].fIsMcGammaElectron = true;
         }
         if(mct1 != NULL && motherPdg == 221 && pdg == 11){
            fCandidates[i].fIsMcEtaElectron = true;
         }
      }

      // TRD
      CbmTrdTrack* trdTrack = NULL;
      if (fUseTrd == true) {
         int trdInd = fCandidates[i].fTrdInd;
         CbmTrackMatchNew* trdMatch = (CbmTrackMatchNew*) fTrdTrackMatches->At(trdInd);
         if (trdMatch == NULL) continue;
         fCandidates[i].fTrdMcTrackId = trdMatch->GetMatchedLink().GetIndex();
         //if (fCandidates[i].trdMcTrackId < 0) continue;
         //CbmMCTrack* mcTrack3 = (CbmMCTrack*) fMCTracks->At(fCandidates[i].trdMcTrackId);
         //if (mcTrack3 == NULL) continue;
      }

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
    //  if (cand.tofMcTrackId < 0) continue;
    //  CbmMCTrack* mcTrack4 = (CbmMCTrack*) fMCTracks->At(cand.tofMcTrackId);
    //  if (mcTrack4 == NULL) continue;
   }// candidates
}

void CbmAnaDielectronTask::AssignMcToTopologyCandidates(
      vector<CbmLmvmCandidate>& cutCandidates)
{
   int nCand = cutCandidates.size();
   for (int i = 0; i < nCand; i++){
      cutCandidates[i].ResetMcParams();

      int stsInd = cutCandidates[i].fStsInd;
      if (stsInd < 0) continue;
      CbmTrackMatchNew* stsMatch  = (CbmTrackMatchNew*) fStsTrackMatches->At(stsInd);
      if (stsMatch == NULL) continue;
      int stsMcTrackId = stsMatch->GetMatchedLink().GetIndex();
      cutCandidates[i].fStsMcTrackId = stsMcTrackId;
      if (stsMcTrackId < 0) continue;
      CbmMCTrack* mcTrack1 = (CbmMCTrack*) fMCTracks->At(stsMcTrackId);
      if (mcTrack1 == NULL) continue;
      int pdg = TMath::Abs(mcTrack1->GetPdgCode());
      int motherId = mcTrack1->GetMotherId();
      cutCandidates[i].fMcMotherId = motherId;
      cutCandidates[i].fMcPdg = pdg;

      if (pdg == 11 && motherId == -1) cutCandidates[i].fIsMcSignalElectron = true;

      if (motherId >=0){
         CbmMCTrack* mct1 = (CbmMCTrack*) fMCTracks->At(motherId);
         int motherPdg = mct1->GetPdgCode();
         if (mct1 != NULL && motherPdg == 111 && pdg == 11) {
            cutCandidates[i].fIsMcPi0Electron = true;
         }
         if (mct1 != NULL && motherPdg == 22 && pdg == 11){
            cutCandidates[i].fIsMcGammaElectron = true;
         }
         if(mct1 != NULL && motherPdg == 221 && pdg == 11){
            cutCandidates[i].fIsMcEtaElectron = true;
         }
      }
   }
}

void CbmAnaDielectronTask::PairSource(
	  CbmLmvmCandidate* candP,
	  CbmLmvmCandidate* candM,
      CbmLmvmAnalysisSteps step,
	  CbmLmvmKinematicParams* parRec)
{
   Bool_t isSignal = candP->fIsMcSignalElectron && candM->fIsMcSignalElectron;
   Bool_t isPi0 = (candP->fIsMcPi0Electron && candM->fIsMcPi0Electron && candP->fMcMotherId == candM->fMcMotherId);
   Bool_t isEta = (candP->fIsMcEtaElectron && candM->fIsMcEtaElectron && candP->fMcMotherId == candM->fMcMotherId);
   Bool_t isGamma = (candP->fIsMcGammaElectron && candM->fIsMcGammaElectron && candP->fMcMotherId == candM->fMcMotherId);
   Bool_t isBg = (!isEta) && (!isGamma) && (!isPi0) && (!(candP->fIsMcSignalElectron || candM->fIsMcSignalElectron));


   if (isSignal) fh_opening_angle[kSignal][step]->Fill(parRec->fAngle, fWeight);
   if (isBg) fh_opening_angle[kBg][step]->Fill(parRec->fAngle);
   if (isPi0) fh_opening_angle[kPi0][step]->Fill(parRec->fAngle);
   if (isGamma)fh_opening_angle[kGamma][step]->Fill(parRec->fAngle);


   int binNum = (double)step + 0.5;
   // Fill BG source pair histograms
   if (isBg){
      TH2D* hsp = fh_source_pairs_epem[step];
      if (candM->fIsMcGammaElectron) {
         if (candP->fIsMcGammaElectron && candP->fMcMotherId != candM->fMcMotherId){
            hsp->Fill(0.5, 0.5);
            fh_source_bg_minv[0][step]->Fill(parRec->fMinv);//g-g
            fh_source_pairs->Fill(binNum, 0.5);
         }else if (candP->fIsMcPi0Electron){
            hsp->Fill(1.5, 0.5);
            fh_source_bg_minv[3][step]->Fill(parRec->fMinv);//g-p
            fh_source_pairs->Fill(binNum, 3.5);
         }else{
            hsp->Fill(2.5, 0.5);
            fh_source_bg_minv[4][step]->Fill(parRec->fMinv);//g-o
            fh_source_pairs->Fill(binNum, 4.5);
         }
      }else if (candM->fIsMcPi0Electron) {
        if (candP->fIsMcGammaElectron){
           hsp->Fill(0.5, 1.5);
           fh_source_bg_minv[3][step]->Fill(parRec->fMinv);//g-p
           fh_source_pairs->Fill(binNum, 3.5);
        } else if (candP->fIsMcPi0Electron && candP->fMcMotherId != candM->fMcMotherId){
           hsp->Fill(1.5, 1.5);
           fh_source_bg_minv[1][step]->Fill(parRec->fMinv);//p-p
           fh_source_pairs->Fill(binNum, 1.5);
        } else {
           hsp->Fill(2.5, 1.5);
           fh_source_bg_minv[5][step]->Fill(parRec->fMinv);//p-o
           fh_source_pairs->Fill(binNum, 5.5);
        }
      }else {
         if (candP->fIsMcGammaElectron){
            hsp->Fill(0.5, 2.5);
            fh_source_bg_minv[4][step]->Fill(parRec->fMinv);//g-o
            fh_source_pairs->Fill(binNum, 4.5);
         }else if (candP->fIsMcPi0Electron){
            hsp->Fill(1.5, 2.5);
            fh_source_bg_minv[5][step]->Fill(parRec->fMinv);//p-o
            fh_source_pairs->Fill(binNum, 5.5);
         }else {
            hsp->Fill(2.5, 2.5);
            fh_source_bg_minv[2][step]->Fill(parRec->fMinv);//o-o
            fh_source_pairs->Fill(binNum, 2.5);
         }
      }
   }
}

void CbmAnaDielectronTask::TrackSource(
		CbmLmvmCandidate* cand,
		CbmLmvmAnalysisSteps step,
		Int_t pdg)
{
   int binNum = (double)step + 0.5;
	Double_t mom = cand->fMomentum.Mag();
	Double_t pt = cand->fMomentum.Perp();
	if (cand->fIsMcSignalElectron) {
		fh_nof_el_tracks->Fill(binNum, fWeight);
		fh_source_mom[kSignal][step]->Fill(mom, fWeight);
		fh_source_pt[kSignal][step]->Fill(pt, fWeight);
	} else {
	   if (IsMismatch(cand)) fh_nof_mismatches->Fill(binNum);
	   if (cand->fStsMcTrackId != cand->fRichMcTrackId) fh_nof_mismatches_rich->Fill(binNum);
	   if (fUseTrd && cand->fStsMcTrackId != cand->fTrdMcTrackId) fh_nof_mismatches_trd->Fill(binNum);
	   if (cand->fStsMcTrackId != cand->fTofMcTrackId) fh_nof_mismatches_tof->Fill(binNum);
	   if (IsGhost(cand)) fh_nof_ghosts->Fill(binNum);
		fh_nof_bg_tracks->Fill(binNum);
		fh_source_mom[kBg][step]->Fill(mom);
		fh_source_pt[kBg][step]->Fill(pt);
		if (cand->fIsMcGammaElectron) {
		   fh_source_tracks->Fill(binNum, 0.5);
		   fh_source_mom[kGamma][step]->Fill(mom);
		   fh_source_pt[kGamma][step]->Fill(pt);

		   // e+/- from gamma conversion vertex
		   int mcTrackId = cand->fStsMcTrackId;
		   CbmMCTrack* mctrack = (CbmMCTrack*) fMCTracks->At(mcTrackId);
		   if (NULL != mctrack){
            TVector3 v;
            mctrack->GetStartVertex(v);
            fh_vertex_el_gamma_xz[step]->Fill(v.Z(),v.X());
            fh_vertex_el_gamma_yz[step]->Fill(v.Z(),v.Y());
            fh_vertex_el_gamma_xy[step]->Fill(v.X(),v.Y());
            fh_vertex_el_gamma_rz[step]->Fill( v.Z(), sqrt(v.X()*v.X()+v.Y()*v.Y()) );
		   }
		}else if (cand->fIsMcPi0Electron) {
		   fh_source_tracks->Fill(binNum, 1.5);
		   fh_source_mom[kPi0][step]->Fill(mom);
		   fh_source_pt[kPi0][step]->Fill(pt);
		}else if (pdg == 211 || pdg ==-211) {
		   fh_source_tracks->Fill(binNum, 2.5);
		}else if (pdg == 2212) {
		   fh_source_tracks->Fill(binNum, 3.5);
		}else if (pdg == 321 || pdg == -321) {
		   fh_source_tracks->Fill(binNum, 4.5);
		}else if ( (pdg == 11 || pdg == -11) && !cand->fIsMcGammaElectron
		      && !cand->fIsMcPi0Electron && !cand->fIsMcSignalElectron){
		   fh_source_tracks->Fill(binNum, 5.5);
		}else{
		   fh_source_tracks->Fill(binNum, 6.5);
		}
	}
}

void CbmAnaDielectronTask::FillPairHists(
	  CbmLmvmCandidate* candP,
      CbmLmvmCandidate* candM,
	  CbmLmvmKinematicParams* parMc,
	  CbmLmvmKinematicParams* parRec,
      CbmLmvmAnalysisSteps step)
{
   Bool_t isSignal = candP->fIsMcSignalElectron && candM->fIsMcSignalElectron;
   Bool_t isPi0 = (candP->fIsMcPi0Electron && candM->fIsMcPi0Electron && candP->fMcMotherId == candM->fMcMotherId);
   Bool_t isEta = (candP->fIsMcEtaElectron && candM->fIsMcEtaElectron && candP->fMcMotherId == candM->fMcMotherId);
   Bool_t isGamma = (candP->fIsMcGammaElectron && candM->fIsMcGammaElectron && candP->fMcMotherId == candM->fMcMotherId);
   Bool_t isBG = (!isEta) && (!isGamma) && (!isPi0) && (!(candP->fIsMcSignalElectron || candM->fIsMcSignalElectron));
   Bool_t isMismatch = (IsMismatch(candP) || IsMismatch(candM));

   if (isSignal) fh_signal_pty[step]->Fill(parMc->fRapidity, parMc->fPt, fWeight);
   if (isSignal) fh_signal_mom[step]->Fill(parMc->fMomentumMag, fWeight);
   if (isSignal) fh_signal_minv[step]->Fill(parRec->fMinv, fWeight);
   if (isSignal) fh_signal_minv_pt[step]->Fill(parRec->fMinv, parMc->fPt, fWeight);
   if (isBG) fh_bg_minv[step]->Fill(parRec->fMinv);
   PairSource(candP, candM, step, parRec);
   if (isPi0) fh_pi0_minv[step]->Fill(parRec->fMinv);
   if (isEta) fh_eta_minv[step]->Fill(parRec->fMinv);
   if (isPi0) fh_pi0_minv_pt[step]->Fill(parRec->fMinv, parMc->fPt);
   if (isEta) fh_eta_minv_pt[step]->Fill(parRec->fMinv, parMc->fPt);
   if (isGamma) fh_gamma_minv[step]->Fill(parRec->fMinv);
   if (isBG && isMismatch) fh_bg_mismatch_minv[step]->Fill(parRec->fMinv);
   if (isBG && !isMismatch){
      fh_bg_truematch_minv[step]->Fill(parRec->fMinv);
      if (candP->fMcPdg == 11 && candM->fMcPdg == 11) fh_bg_truematch_el_minv[step]->Fill(parRec->fMinv);
      if (candP->fMcPdg != 11 || candM->fMcPdg != 11) fh_bg_truematch_notel_minv[step]->Fill(parRec->fMinv);
   }

}

void CbmAnaDielectronTask::SignalAndBgReco()
{
   CheckGammaConvAndPi0();
   CheckTopologyCut("ST", fSTCandidates, fh_stcut, fh_stcut_pion, fh_stcut_truepair, fCuts.fStCutAngle, fCuts.fStCutPP);
   CheckTopologyCut("TT", fTTCandidates, fh_ttcut, fh_ttcut_pion, fh_ttcut_truepair, fCuts.fTtCutAngle, fCuts.fTtCutPP);
   CheckTopologyCut("RT", fRTCandidates, fh_rtcut, fh_rtcut_pion, fh_rtcut_truepair, fCuts.fRtCutAngle, fCuts.fRtCutPP);
   if (fUseMvd){
      CheckClosestMvdHit(1, fh_mvd1cut, fh_mvd1cut_qa);
      CheckClosestMvdHit(2, fh_mvd2cut, fh_mvd2cut_qa);
   }

   Int_t ncand = fCandidates.size();
   for (Int_t i = 0; i < ncand; i++){
      Int_t pdg = 0;
      if (fCandidates[i].fStsMcTrackId > 0){
         CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(fCandidates[i].fStsMcTrackId);
         if (NULL != mcTrack) pdg = mcTrack->GetPdgCode();
      }

      Bool_t isChi2Prim = (fCandidates[i].fChi2Prim < fCuts.fChiPrimCut);
      Bool_t isEl = (fCandidates[i].fIsElectron);
      Bool_t isGammaCut = (!fCandidates[i].fIsGamma);
      Bool_t isMvd1Cut = fCandidates[i].fIsMvd1CutElectron;
      Bool_t isMvd2Cut = fCandidates[i].fIsMvd2CutElectron;
      Bool_t isStCut = (fCandidates[i].fIsStCutElectron);
      Bool_t isTtCut = (fCandidates[i].fIsTtCutElectron);
      Bool_t isPtCut = (fCandidates[i].fMomentum.Perp() > fCuts.fPtCut);
      if (!fUseMvd) isMvd1Cut = true;
      if (!fUseMvd) isMvd2Cut = true;

		TrackSource(&fCandidates[i], kReco, pdg);
		if (isChi2Prim) TrackSource(&fCandidates[i], kChi2Prim, pdg);
		if (isChi2Prim && isEl) TrackSource(&fCandidates[i], kElId, pdg);
		if (isChi2Prim && isEl && isGammaCut) TrackSource(&fCandidates[i], kGammaCut, pdg);
      if (isChi2Prim && isEl && isGammaCut && isMvd1Cut) TrackSource(&fCandidates[i], kMvd1Cut, pdg);
      if (isChi2Prim && isEl && isGammaCut && isMvd1Cut && isMvd2Cut)TrackSource(&fCandidates[i], kMvd2Cut, pdg);
		if (isChi2Prim && isEl && isGammaCut && isMvd1Cut && isMvd2Cut && isStCut) TrackSource(&fCandidates[i], kStCut, pdg);
		if (isChi2Prim && isEl && isGammaCut && isMvd1Cut && isMvd2Cut && isStCut && isTtCut) TrackSource(&fCandidates[i], kTtCut, pdg);
		if (isChi2Prim && isEl && isGammaCut && isMvd1Cut && isMvd2Cut && isStCut && isTtCut && isPtCut) TrackSource(&fCandidates[i], kPtCut, pdg);
    }

    for (Int_t iP = 0; iP < ncand; iP++){
        if (fCandidates[iP].fCharge < 0) continue;
	CbmMCTrack* mctrackP = NULL;
	if (fCandidates[iP].fStsMcTrackId >=0) mctrackP = (CbmMCTrack*) fMCTracks->At(fCandidates[iP].fStsMcTrackId);
        for (Int_t iM = 0; iM < ncand; iM++){
            if (fCandidates[iM].fCharge > 0) continue;
	    CbmMCTrack* mctrackM = NULL;
	    if (fCandidates[iM].fStsMcTrackId >=0) mctrackM = (CbmMCTrack*) fMCTracks->At(fCandidates[iM].fStsMcTrackId);
            if (iM == iP ) continue;

        CbmLmvmKinematicParams pMC;
	    if (mctrackP != NULL && mctrackM != NULL) pMC = CbmLmvmKinematicParams::KinematicParamsWithMcTracks(mctrackP, mctrackM);

	        CbmLmvmKinematicParams pRec = CbmLmvmKinematicParams::KinematicParamsWithCandidates(&fCandidates[iP],&fCandidates[iM]);

            Bool_t isChiPrimary = (fCandidates[iP].fChi2Prim < fCuts.fChiPrimCut && fCandidates[iM].fChi2Prim < fCuts.fChiPrimCut);
            Bool_t isEl = (fCandidates[iP].fIsElectron && fCandidates[iM].fIsElectron);
            Bool_t isGammaCut = (!fCandidates[iP].fIsGamma && !fCandidates[iM].fIsGamma);
            Bool_t isStCut = (fCandidates[iP].fIsStCutElectron && fCandidates[iM].fIsStCutElectron);
            Bool_t isTtCut = (fCandidates[iP].fIsTtCutElectron && fCandidates[iM].fIsTtCutElectron);
            Bool_t isPtCut = (fCandidates[iP].fMomentum.Perp() > fCuts.fPtCut && fCandidates[iM].fMomentum.Perp() > fCuts.fPtCut);
            Bool_t isAngleCut = (pRec.fAngle > fCuts.fAngleCut);
            Bool_t isMvd1Cut = (fCandidates[iP].fIsMvd1CutElectron && fCandidates[iM].fIsMvd1CutElectron);
            Bool_t isMvd2Cut = (fCandidates[iP].fIsMvd2CutElectron && fCandidates[iM].fIsMvd2CutElectron);
            if (!fUseMvd) isMvd1Cut = true;
            if (!fUseMvd) isMvd2Cut = true;

            FillPairHists(&fCandidates[iP], &fCandidates[iM], &pMC, &pRec, kReco);
            if (isChiPrimary){
               FillPairHists(&fCandidates[iP], &fCandidates[iM], &pMC, &pRec, kChi2Prim);
            }
            if (isChiPrimary && isEl){
               FillPairHists(&fCandidates[iP], &fCandidates[iM], &pMC, &pRec, kElId);
            }
            if (isChiPrimary && isEl && isGammaCut){
               FillPairHists(&fCandidates[iP], &fCandidates[iM], &pMC, &pRec, kGammaCut);
            }
            if (isChiPrimary && isEl && isGammaCut && isMvd1Cut){
               FillPairHists(&fCandidates[iP], &fCandidates[iM], &pMC, &pRec, kMvd1Cut);
            }
            if (isChiPrimary && isEl && isGammaCut && isMvd1Cut && isMvd2Cut){
               FillPairHists(&fCandidates[iP], &fCandidates[iM], &pMC, &pRec, kMvd2Cut);
            }
            if (isChiPrimary && isEl && isGammaCut && isMvd1Cut && isMvd2Cut && isStCut){
               FillPairHists(&fCandidates[iP], &fCandidates[iM], &pMC, &pRec, kStCut);
            }
            if (isChiPrimary && isEl && isGammaCut && isMvd1Cut && isMvd2Cut && isStCut && isTtCut){
               FillPairHists(&fCandidates[iP], &fCandidates[iM], &pMC, &pRec, kTtCut);
            }
            if (isChiPrimary && isEl && isGammaCut && isMvd1Cut && isMvd2Cut && isStCut && isTtCut && isPtCut){
               FillPairHists(&fCandidates[iP], &fCandidates[iM], &pMC, &pRec, kPtCut);
            }
        } // iM
    } // iP
}

void CbmAnaDielectronTask::CheckGammaConvAndPi0()
{
   Int_t ncand = fCandidates.size();
   for (Int_t iP = 0; iP < ncand; iP++){
      if (fCandidates[iP].fCharge < 0) continue;
      for (Int_t iM = 0; iM < ncand; iM++){
         if (fCandidates[iM].fCharge > 0) continue;
         if (iM == iP ) continue;
         if ((fCandidates[iP].fChi2Prim < fCuts.fChiPrimCut && fCandidates[iP].fIsElectron) &&
         (fCandidates[iM].fChi2Prim < fCuts.fChiPrimCut && fCandidates[iM].fIsElectron )) {
        	CbmLmvmKinematicParams pRec = CbmLmvmKinematicParams::KinematicParamsWithCandidates(&fCandidates[iP],&fCandidates[iM]);
            if (pRec.fMinv < fCuts.fGammaCut) {
               fCandidates[iM].fIsGamma = true;
               fCandidates[iP].fIsGamma = true;
            }
         }
      }
   }
}

void CbmAnaDielectronTask::CheckTopologyCut(
      const string& cutName,
      const vector<CbmLmvmCandidate>& cutCandidates,
      const vector<TH2D*>& hcut,
      const vector<TH2D*>& hcutPion,
      const vector<TH2D*>& hcutTruepair,
      Double_t angleCut,
      Double_t ppCut)
{
   vector<Double_t> angles, mom, candInd;
   Int_t nCand = fCandidates.size();
   Int_t nCutCand = cutCandidates.size();
   for (Int_t iP = 0; iP < nCand; iP++){
      if (fCandidates[iP].fChi2Prim < fCuts.fChiPrimCut && fCandidates[iP].fIsElectron){
         angles.clear();
         mom.clear();
         candInd.clear();
         for (Int_t iM = 0; iM < nCutCand; iM++){
            // different charges, charge Im != charge iP
            if (cutCandidates[iM].fCharge != fCandidates[iP].fCharge){
               CbmLmvmKinematicParams pRec = CbmLmvmKinematicParams::KinematicParamsWithCandidates(&fCandidates[iP], &cutCandidates[iM]);
               angles.push_back(pRec.fAngle);
               mom.push_back(cutCandidates[iM].fMomentum.Mag());
               candInd.push_back(iM);
            } // if
         }// iM
         //find min opening angle
         Double_t minAng = 360.;
         Int_t minInd = -1;
         for (Int_t i = 0; i < angles.size(); i++){
            if (minAng > angles[i]){
               minAng = angles[i];
               minInd = i;
            }
         }
         if (minInd == -1){
            if (cutName == "TT") fCandidates[iP].fIsTtCutElectron = true;
            if (cutName == "ST") fCandidates[iP].fIsStCutElectron = true;
            if (cutName == "RT") fCandidates[iP].fIsRtCutElectron = true;
            continue;
         }
         Double_t sqrt_mom = TMath::Sqrt(fCandidates[iP].fMomentum.Mag()*mom[minInd]);
         Double_t val = -1.*(angleCut/ppCut)*sqrt_mom + angleCut;
         if ( !(sqrt_mom < ppCut && val > minAng) ) {
            if (cutName == "TT") fCandidates[iP].fIsTtCutElectron = true;
            if (cutName == "ST") fCandidates[iP].fIsStCutElectron = true;
            if (cutName == "RT") fCandidates[iP].fIsRtCutElectron = true;
         }

         int stsInd = cutCandidates[ candInd[minInd] ].fStsInd;
         if (stsInd < 0) continue;
         int pdg =  TMath::Abs(cutCandidates[ candInd[minInd] ].fMcPdg);
         int motherId = cutCandidates[ candInd[minInd] ].fMcMotherId;

         if (fCandidates[iP].fIsMcSignalElectron){
            hcut[kSignal]->Fill(sqrt_mom, minAng, fWeight);
            if (pdg == 211) hcutPion[kSignal]->Fill(sqrt_mom, minAng, fWeight);
            if (motherId == fCandidates[iP].fMcMotherId) hcutTruepair[kSignal]->Fill(sqrt_mom, minAng, fWeight);
         } else{
            hcut[kBg]->Fill(sqrt_mom, minAng);
            if (pdg == 211) hcutPion[kBg]->Fill(sqrt_mom, minAng);;
            if (motherId != -1 && motherId == fCandidates[iP].fMcMotherId) hcutTruepair[kBg]->Fill(sqrt_mom, minAng);;
         }
         if (fCandidates[iP].fIsMcPi0Electron){
            hcut[kPi0]->Fill(sqrt_mom, minAng);
            if (pdg == 211) hcutPion[kPi0]->Fill(sqrt_mom, minAng);;
            if (motherId != -1 && motherId == fCandidates[iP].fMcMotherId) hcutTruepair[kPi0]->Fill(sqrt_mom, minAng);;
         }
         if (fCandidates[iP].fIsMcGammaElectron){
            hcut[kGamma]->Fill(sqrt_mom, minAng);
            if (pdg == 211) hcutPion[kGamma]->Fill(sqrt_mom, minAng);;
            if (motherId != -1 && motherId == fCandidates[iP].fMcMotherId) hcutTruepair[kGamma]->Fill(sqrt_mom, minAng);;
         }
      }//if electron
   } //iP
}

void CbmAnaDielectronTask::CalculateNofTopologyPairs(
      TH1D* h_nof_pairs,
      const string& source)
{
   Int_t nCand = fCandidates.size();
   vector<bool> isAdded;
   isAdded.resize(nCand);
   for (Int_t iP = 0; iP < nCand; iP++){
	   isAdded[iP] = false;
   }
   for (Int_t iP = 0; iP < nCand; iP++){
      if (fCandidates[iP].fMcMotherId == -1) continue;
      if ( source == "pi0" && !fCandidates[iP].fIsMcPi0Electron) continue;
      if ( source == "gamma" && !fCandidates[iP].fIsMcGammaElectron) continue;

      if ( !(fCandidates[iP].fChi2Prim < fCuts.fChiPrimCut && fCandidates[iP].fIsElectron) ) continue;

      if (isAdded[iP]) continue;

      for (Int_t iM = 0; iM < fSTCandidates.size(); iM++){
         if (fSTCandidates[iM].fMcMotherId == fCandidates[iP].fMcMotherId){
            h_nof_pairs->Fill(4.5);
            isAdded[iP] = true;
            break;
         }
      }
      if (isAdded[iP]) continue;

      for (Int_t iM = 0; iM < fRTCandidates.size(); iM++){
         if (fRTCandidates[iM].fMcMotherId == fCandidates[iP].fMcMotherId){
            h_nof_pairs->Fill(5.5);
            isAdded[iP] = true;
            break;
         }
      }
      if (isAdded[iP]) continue;

      for (Int_t iM = 0; iM < fTTCandidates.size(); iM++){
         if (fTTCandidates[iM].fMcMotherId == fCandidates[iP].fMcMotherId){
            h_nof_pairs->Fill(6.5);
            isAdded[iP] = true;
            break;
         }
      }
      if (isAdded[iP]) continue;

      for (Int_t iM = 0; iM < fCandidates.size(); iM++){
         if (iM != iP && fCandidates[iM].fMcMotherId == fCandidates[iP].fMcMotherId && fCandidates[iM].fChi2Prim < fCuts.fChiPrimCut && fCandidates[iM].fIsElectron) {
            h_nof_pairs->Fill(7.5);
            isAdded[iP] = true;
            isAdded[iM] = true;
            break;
         }
      }

      if (isAdded[iP]) continue;
      Int_t nofPointsSts = 0;
      Int_t nofMcTracks = fMCTracks->GetEntriesFast();
      for (Int_t iMCTrack = 0; iMCTrack < nofMcTracks; iMCTrack++) {
    	  const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(fMCTracks->At(iMCTrack));
    	  if (mcTrack->GetMotherId() != fCandidates[iP].fMcMotherId) continue;
    	  if (iMCTrack == fCandidates[iP].fStsMcTrackId) continue;

		  if (!fMCTrackCreator->TrackExists(iMCTrack)) continue;
		  const CbmLitMCTrack& litMCTrack = fMCTrackCreator->GetTrack(iMCTrack);
		  nofPointsSts = litMCTrack.GetNofPointsInDifferentStations(kSTS);
		  break;
      }
      if (nofPointsSts == 0) h_nof_pairs->Fill(0.5);
      if (nofPointsSts >= 1 && nofPointsSts <=3) h_nof_pairs->Fill(1.5);
      if (nofPointsSts >= 4 && nofPointsSts <=5) h_nof_pairs->Fill(2.5);
      if (nofPointsSts >= 6) h_nof_pairs->Fill(3.5);

   }
}

Bool_t CbmAnaDielectronTask::IsMismatch(
		CbmLmvmCandidate* cand)
{
   bool isTrdOk = (fUseTrd) ? (cand->fStsMcTrackId == cand->fTrdMcTrackId) : true;
   if (cand->fStsMcTrackId == cand->fRichMcTrackId && isTrdOk &&
       cand->fStsMcTrackId == cand->fTofMcTrackId && cand->fStsMcTrackId !=-1) return false;
   return true;
}

Bool_t CbmAnaDielectronTask::IsGhost(
		CbmLmvmCandidate* cand)
{
   if (cand->fStsMcTrackId == -1 || cand->fRichMcTrackId == -1 || cand->fTrdMcTrackId == -1 ||
         cand->fTofMcTrackId == -1) return true;
   return false;
}

void CbmAnaDielectronTask::IsElectron(
      CbmRichRing * ring,
      Double_t momentum,
      CbmTrdTrack* trdTrack,
      CbmGlobalTrack * gTrack,
	  CbmLmvmCandidate* cand)
{
   if (fPionMisidLevel < 0.){
      Bool_t richEl = IsRichElectron(ring, momentum, cand);
      Bool_t trdEl = (trdTrack != NULL)?IsTrdElectron(trdTrack, cand):true;
      Double_t annRich = cand->fRichAnn;
      Double_t annTrd = cand->fTrdAnn;
      Bool_t tofEl = IsTofElectron(gTrack, momentum, cand);
      Bool_t momCut = (fCuts.fMomentumCut > 0.)?(momentum < fCuts.fMomentumCut):true;

      if (richEl && trdEl && tofEl && momCut) {
         cand->fIsElectron = true;
      } else {
         cand->fIsElectron = false;
      }
   } else {
      // PID using MC information, a certain pi supression level can be set
       if (cand->fStsMcTrackId < 0 || cand->fStsMcTrackId >= fMCTracks->GetEntries()){
	   cand->fIsElectron = false;
       } else {
		   CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(cand->fStsMcTrackId);
		   Int_t pdg = mcTrack->GetPdgCode();
		   if (pdg == 11 || pdg == -11){
			   cand->fIsElectron = true;
		   } else {
			   Double_t r = fRandom3->Rndm();
			   if (r < fPionMisidLevel){
			   cand->fIsElectron = true;
			   } else {
			   cand->fIsElectron = false;
			   }
		   }
       }
   }
}

Bool_t CbmAnaDielectronTask::IsRichElectron(
      CbmRichRing *ring,
      Double_t momentum,
	  CbmLmvmCandidate* cand)
{
    if (fCuts.fUseRichAnn == false){
        Double_t axisA = ring->GetAaxis();
        Double_t axisB = ring->GetBaxis();
        Double_t dist = ring->GetDistance();
        if ( fabs(axisA-fCuts.fMeanA) < fCuts.fRmsCoeff*fCuts.fRmsA &&fabs(axisB-fCuts.fMeanB) < fCuts.fRmsCoeff*fCuts.fRmsB && dist < fCuts.fDistCut){
            return true;
        } else {
            return false;
        }
    } else {
        Double_t ann = fElIdAnn->DoSelect(ring, momentum);
        cand->fRichAnn = ann;
        if (ann > fCuts.fRichAnnCut) return true;
        else  return false;
    }
}


Bool_t CbmAnaDielectronTask::IsTrdElectron(
      CbmTrdTrack* trdTrack,
	  CbmLmvmCandidate* cand)
{
    Double_t ann = trdTrack->GetPidANN();
    cand->fTrdAnn = ann;
    if (ann > fCuts.fTrdAnnCut) return true;
    else return false;

}

Bool_t CbmAnaDielectronTask::IsTofElectron(
      CbmGlobalTrack* gTrack,
      Double_t momentum,
	  CbmLmvmCandidate* cand)
{
    Double_t trackLength = gTrack->GetLength() / 100.;

    // Calculate time of flight from TOF hit
    Int_t tofIndex = gTrack->GetTofHitIndex();
    CbmTofHit* tofHit = (CbmTofHit*) fTofHits->At(tofIndex);
    if (NULL == tofHit){
        cand->fMass2 = 100.;
        return false;
    }
    Double_t time = 0.2998 * tofHit->GetTime(); // time in ns -> transfrom to ct in m

    // Calculate mass squared
    Double_t mass2 = TMath::Power(momentum, 2.) * (TMath::Power(time/ trackLength, 2) - 1);
    cand->fMass2 = mass2;

    if (momentum >= 1.) {
        if (mass2 < (0.013*momentum - 0.003)){
            return true;
        }
    } else {
        if (mass2 < 0.01){
            return true;//fTofM2
        }
    }
    return false;
}

void CbmAnaDielectronTask::DifferenceSignalAndBg()
{
//ID cuts
    Int_t nCand = fCandidates.size(); 
    for (Int_t i = 0; i < nCand; i++){
        if (fCandidates[i].fIsMcSignalElectron){
            fh_chi2prim[kSignal]->Fill(fCandidates[i].fChi2Prim, fWeight);
        } else {
            fh_chi2prim[kBg]->Fill(fCandidates[i].fChi2Prim);
        }
	if (fCandidates[i].fIsMcGammaElectron){
	    fh_chi2prim[kGamma]->Fill(fCandidates[i].fChi2Prim);
	}
        if (fCandidates[i].fIsMcPi0Electron) fh_chi2prim[kPi0]->Fill(fCandidates[i].fChi2Prim);

        if (fCandidates[i].fChi2Prim > fCuts.fChiPrimCut ) continue;

        if (fCandidates[i].fIsMcSignalElectron){
            fh_richann[kSignal]->Fill(fCandidates[i].fRichAnn, fWeight);
            fh_trdann[kSignal]->Fill(fCandidates[i].fTrdAnn, fWeight);
            fh_tofm2[kSignal]->Fill(fCandidates[i].fMomentum.Mag(), fCandidates[i].fMass2, fWeight);
        } else {
            fh_richann[kBg]->Fill(fCandidates[i].fRichAnn);
            fh_trdann[kBg]->Fill(fCandidates[i].fTrdAnn);
            fh_tofm2[kBg]->Fill(fCandidates[i].fMomentum.Mag(), fCandidates[i].fMass2);
       }
       if (fCandidates[i].fIsMcGammaElectron){
          fh_richann[kGamma]->Fill(fCandidates[i].fRichAnn);
          fh_trdann[kGamma]->Fill(fCandidates[i].fTrdAnn);
          fh_tofm2[kGamma]->Fill(fCandidates[i].fMomentum.Mag(), fCandidates[i].fMass2);
       }
       if (fCandidates[i].fIsMcPi0Electron){
          fh_richann[kPi0]->Fill(fCandidates[i].fRichAnn);
          fh_trdann[kPi0]->Fill(fCandidates[i].fTrdAnn);
          fh_tofm2[kPi0]->Fill(fCandidates[i].fMomentum.Mag(), fCandidates[i].fMass2);
       }
    } // loop over candidates

    for (Int_t i = 0; i < nCand; i++){
        if (!(fCandidates[i].fChi2Prim < fCuts.fChiPrimCut && fCandidates[i].fIsElectron )) continue;

        if (fCandidates[i].fIsMcSignalElectron){
            fh_pt[kSignal]->Fill(fCandidates[i].fMomentum.Perp(), fWeight);
            fh_mom[kSignal]->Fill(fCandidates[i].fMomentum.Mag(), fWeight);
            fh_chi2sts[kSignal]->Fill(fCandidates[i].fChi2sts, fWeight);
        } else {
            fh_pt[kBg]->Fill(fCandidates[i].fMomentum.Perp());
            fh_mom[kBg]->Fill(fCandidates[i].fMomentum.Mag());
            fh_chi2sts[kBg]->Fill(fCandidates[i].fChi2sts);
        }
        if (fCandidates[i].fIsMcGammaElectron){
           fh_pt[kGamma]->Fill(fCandidates[i].fMomentum.Perp());
           fh_mom[kGamma]->Fill(fCandidates[i].fMomentum.Mag());
           fh_chi2sts[kGamma]->Fill(fCandidates[i].fChi2sts);
        }
        if (fCandidates[i].fIsMcPi0Electron){
           fh_pt[kPi0]->Fill(fCandidates[i].fMomentum.Perp());
           fh_mom[kPi0]->Fill(fCandidates[i].fMomentum.Mag());
           fh_chi2sts[kPi0]->Fill(fCandidates[i].fChi2sts);
        }
    } // loop over candidates

   if (fUseMvd){
      //number of STS and MVD hits and distributions for the MVD
      for (int i = 0; i < nCand; i++){
         if (!(fCandidates[i].fChi2Prim < fCuts.fChiPrimCut && fCandidates[i].fIsElectron )) continue;

         CbmStsTrack *track = (CbmStsTrack*) fStsTracks->At(fCandidates[i].fStsInd);
         if (NULL == track) continue;
         int nofMvdHits = track->GetNofMvdHits();
         int nofStsHits = track->GetNofHits();
         double mvd1x = 0., mvd1y = 0., mvd2x = 0., mvd2y = 0.;

         for(Int_t ith = 0; ith < nofMvdHits; ith++) {
            Int_t iHit = track->GetMvdHitIndex(ith);
            CbmMvdHit *pmh = (CbmMvdHit*) fMvdHits->At(iHit);
            Int_t stationNum = pmh->GetStationNr();
            if(NULL == pmh) continue;
            if (stationNum == 1){
               mvd1x = pmh->GetX();
               mvd1y = pmh->GetY();
            } else if (stationNum == 2){
               mvd2x = pmh->GetX();
               mvd2y = pmh->GetY();
            }
         }
         double mvd1r = sqrt(mvd1x*mvd1x + mvd1y*mvd1y);
         double mvd2r = sqrt(mvd2x*mvd2x + mvd2y*mvd2y);

         if (fCandidates[i].fIsMcSignalElectron){
            fh_nofMvdHits[kSignal]->Fill(nofMvdHits, fWeight);
            fh_nofStsHits[kSignal]->Fill(nofStsHits, fWeight);
            fh_mvd1xy[kSignal]->Fill(mvd1x, mvd1y, fWeight);
            fh_mvd1r[kSignal]->Fill(mvd1r, fWeight);
            fh_mvd2xy[kSignal]->Fill(mvd2x, mvd2y, fWeight);
            fh_mvd2r[kSignal]->Fill(mvd2r, fWeight);
         } else {
            fh_nofMvdHits[kBg]->Fill(nofMvdHits);
            fh_nofStsHits[kBg]->Fill(nofStsHits);
            fh_mvd1xy[kBg]->Fill(mvd1x, mvd1y);
            fh_mvd1r[kBg]->Fill(mvd1r);
            fh_mvd2xy[kBg]->Fill(mvd2x, mvd2y);
            fh_mvd2r[kBg]->Fill(mvd2r);
         }
         if (fCandidates[i].fIsMcGammaElectron){
            fh_nofMvdHits[kGamma]->Fill(nofMvdHits);
            fh_nofStsHits[kGamma]->Fill(nofStsHits);
            fh_mvd1xy[kGamma]->Fill(mvd1x, mvd1y);
            fh_mvd1r[kGamma]->Fill(mvd1r);
            fh_mvd2xy[kGamma]->Fill(mvd2x, mvd2y);
            fh_mvd2r[kGamma]->Fill(mvd2r);
         }
         if (fCandidates[i].fIsMcPi0Electron){
            fh_nofMvdHits[kPi0]->Fill(nofMvdHits);
            fh_nofStsHits[kPi0]->Fill(nofStsHits);
            fh_mvd1xy[kPi0]->Fill(mvd1x, mvd1y);
            fh_mvd1r[kPi0]->Fill(mvd1r);
            fh_mvd2xy[kPi0]->Fill(mvd2x, mvd2y);
            fh_mvd2r[kPi0]->Fill(mvd2r);
         }
      }
   }
}

void CbmAnaDielectronTask::CheckClosestMvdHit(
		Int_t mvdStationNum,
		vector<TH2D*>& hist,
		vector<TH1D*>& histQa)
{
   vector<float> mvdX;
   vector<float> mvdY;
   vector<int> mvdInd;
   vector<float> candX;
   vector<float> candY;
   vector<int> candInd;

   CbmKF *KF = CbmKF::Instance();

   Int_t nMvdHit = fMvdHits->GetEntriesFast();
   for(Int_t iHit = 0; iHit < nMvdHit; iHit++) {
      CbmMvdHit *pmh = (CbmMvdHit*) fMvdHits->At(iHit);
      if(NULL == pmh) continue;
      Int_t stationNum = pmh->GetStationNr();
      if (stationNum == mvdStationNum){
		mvdX.push_back(pmh->GetX());
		mvdY.push_back(pmh->GetY());
		mvdInd.push_back(iHit);
      }
   }

   Int_t nCand = fCandidates.size();
   for (Int_t i = 0; i < nCand; i++){
      if (fCandidates[i].fChi2Prim < fCuts.fChiPrimCut && fCandidates[i].fIsElectron){
         CbmStsTrack *track = (CbmStsTrack*) fStsTracks->At(fCandidates[i].fStsInd);
         if(NULL == track) continue;
         Int_t nhits = track->GetNofMvdHits();
         for(Int_t ith = 0; ith < nhits; ith++) {
            Int_t iHit = track->GetMvdHitIndex(ith);
            CbmMvdHit *pmh = (CbmMvdHit*) fMvdHits->At(iHit);
            Int_t stationNum = pmh->GetStationNr();
            if(NULL == pmh) continue;
            if (stationNum == mvdStationNum){
			  candX.push_back(pmh->GetX());
			  candY.push_back(pmh->GetY());
			  candInd.push_back(i);
            }
         }
      }
   }// iCand

   for(Int_t iT = 0; iT < candInd.size(); iT++) {
      Float_t mind = 9999999.;
      Int_t minMvdInd = -1;
      for(Int_t iH = 0; iH < mvdX.size(); iH++) {
         Float_t dx = mvdX[iH] - candX[iT];
         Float_t dy = mvdY[iH] - candY[iT];
         Float_t d2 = dx*dx + dy*dy;
         if(d2 < 1.e-9) continue;
         if(d2 < mind){
            minMvdInd = mvdInd[iH];
            mind = d2;
         }
      } // iHit
      Double_t dmvd = sqrt(mind);

      // Check MVD cut quality
      double bin = -1.;
      const CbmMvdHitMatch* hitMatch = static_cast<const CbmMvdHitMatch*>(fMvdHitMatches->At(minMvdInd));
      if (NULL != hitMatch){
         int mcMvdHitId = hitMatch->GetTrackId();
         CbmMCTrack* mct1 = (CbmMCTrack*) fMCTracks->At(mcMvdHitId);
         int mcMvdHitPdg = TMath::Abs(mct1->GetPdgCode());
         int mvdMotherId = mct1->GetMotherId();

	 int stsMcTrackId = fCandidates[candInd[iT]].fStsMcTrackId;
         int stsMotherId = -2;
	 if (stsMcTrackId >=0){
	     CbmMCTrack* mct2 = (CbmMCTrack*) fMCTracks->At(stsMcTrackId);
	     stsMotherId = mct2->GetMotherId();
	 }

         //cout << mvdStationNum << " " << mvdMotherId << " " << stsMotherId << endl;
         if ( mvdMotherId != -1 && mvdMotherId == stsMotherId){
            bin = 0.5; // correct assignment
         } else {
            bin = 1.5; // not correct assignment
         }

         if (fCandidates[candInd[iT]].fIsMcSignalElectron){
            if (mvdMotherId == stsMotherId && mcMvdHitPdg == 11){
               bin = 0.5; // correct assignment
            }else{
               bin = 1.5;
            }
         }
      }
      //cout << "MVD cut correctness " << bin << endl;

      // Fill histograms
      fCandidates[candInd[iT]].fDSts = dmvd;
      if (fCandidates[candInd[iT]].fIsMcSignalElectron){
         hist[kSignal]->Fill(dmvd, fCandidates[candInd[iT]].fMomentum.Mag(), fWeight);
         histQa[kSignal]->Fill(bin, fWeight);
      }else {
         hist[kBg]->Fill(dmvd, fCandidates[candInd[iT]].fMomentum.Mag());
         histQa[kBg]->Fill(bin);
      }
      if (fCandidates[candInd[iT]].fIsMcGammaElectron){
         hist[kGamma]->Fill(dmvd, fCandidates[candInd[iT]].fMomentum.Mag());
         histQa[kGamma]->Fill(bin);
      }
      if (fCandidates[candInd[iT]].fIsMcPi0Electron){
         hist[kPi0]->Fill(dmvd, fCandidates[candInd[iT]].fMomentum.Mag());
         histQa[kPi0]->Fill(bin);
      }

      // Apply MVD cut
      if (mvdStationNum == 1){
         Double_t mom = fCandidates[candInd[iT]].fMomentum.Mag();
         Double_t val = -1.*(fCuts.fMvd1CutP/fCuts.fMvd1CutD)*dmvd + fCuts.fMvd1CutP;
         if ( !(dmvd < fCuts.fMvd1CutD && val > mom) ) {
            fCandidates[candInd[iT]].fIsMvd1CutElectron = true;
         } else {
            fCandidates[candInd[iT]].fIsMvd1CutElectron = false;
         }
      }
      if (mvdStationNum == 2){
         Double_t mom = fCandidates[candInd[iT]].fMomentum.Mag();
         Double_t val = -1.*(fCuts.fMvd2CutP/fCuts.fMvd2CutD)*dmvd + fCuts.fMvd2CutP;
         if ( !(dmvd < fCuts.fMvd2CutD && val > mom) ){
            fCandidates[candInd[iT]].fIsMvd2CutElectron = true;
         }else {
            fCandidates[candInd[iT]].fIsMvd2CutElectron = false;
         }
      }
   } // iTrack
}

void CbmAnaDielectronTask::MvdCutMcDistance()
{
   if (!fUseMvd) return;
   Int_t nCand = fCandidates.size();
   for (Int_t i = 0; i < nCand; i++){
      if (fCandidates[i].fChi2Prim < fCuts.fChiPrimCut && fCandidates[i].fIsElectron){
         CbmStsTrack *track = (CbmStsTrack*) fStsTracks->At(fCandidates[i].fStsInd);
         if(NULL == track) continue;
         int stsMcTrackId = fCandidates[i].fStsMcTrackId;
         Int_t nhits = track->GetNofMvdHits();
         for(Int_t ith = 0; ith < nhits; ith++) {
            Int_t iHit = track->GetMvdHitIndex(ith);
            CbmMvdHit *pmh1 = (CbmMvdHit*) fMvdHits->At(iHit);
            if (NULL == pmh1) continue;
            Int_t stationNum = pmh1->GetStationNr();

            int nofMvdHits = fMvdHitMatches->GetEntriesFast();
            for (int iMvd = 0; iMvd < nofMvdHits; iMvd++){
               const CbmMvdHitMatch* hitMatch = static_cast<const CbmMvdHitMatch*>(fMvdHitMatches->At(iMvd));
               if (NULL == hitMatch) continue;
               int mcMvdHitId = hitMatch->GetTrackId();
               if (stsMcTrackId != mcMvdHitId) continue;
               CbmMvdHit *pmh2 = (CbmMvdHit*) fMvdHits->At(iMvd);
               if ( pmh2->GetStationNr() != stationNum) continue;
               double dx = pmh1->GetX() - pmh2->GetX();
               double dy = pmh1->GetY() - pmh2->GetY();
               double d = sqrt(dx*dx + dy*dy);
               if (stationNum == 1){
                  if (fCandidates[i].fIsMcGammaElectron) fh_mvd1cut_mc_dist_gamma->Fill(d);
                  if (fCandidates[i].fIsMcPi0Electron) fh_mvd1cut_mc_dist_pi0->Fill(d);
               }else if (stationNum == 1){
                  if (fCandidates[i].fIsMcGammaElectron) fh_mvd2cut_mc_dist_gamma->Fill(d);
                  if (fCandidates[i].fIsMcPi0Electron) fh_mvd2cut_mc_dist_pi0->Fill(d);
               }

            }
         }
      }
   }// iCan
}

void CbmAnaDielectronTask::Finish()
{
   // Write histograms to a file
   for (Int_t i = 0; i < fHistoList.size(); i++){
     fHistoList[i]->Write();
   }
}
