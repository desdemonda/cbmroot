/** CbmAnaDielectronTask.h
 * @author Elena Lebedeva <e.lebedeva@gsi.de>
 * @since 2010
 * @version 3.0
 **/

#ifndef CBM_ANA_DIELECTRON_TASK_H
#define CBM_ANA_DIELECTRON_TASK_H

#include "FairTask.h"
#include "FairBaseParSet.h"
#include "CbmLmvmHist.h"
#include "CbmLmvmCandidate.h"
#include "CbmLmvmKinematicParams.h"
#include "CbmVertex.h"
#include "CbmStsKFTrackFitter.h"
#include "CbmStsTrack.h"
#include "CbmTrdTrack.h"
#include "CbmMCTrack.h"
#include "CbmGlobalTrack.h"
#include "CbmRichElectronIdAnn.h"
#include "CbmKFVertex.h"
#include "cbm/qa/mc/CbmLitMCTrackCreator.h"
#include "CbmLmvmCuts.h"

#include <map>
#include <fstream>
#include <vector>
#include <string>
using std::map;
using std::vector;
using std::string;

class TClonesArray;
class TH2D;
class TH1D;
class TH2F;
class TRandom3;
class FairRootManager;


class CbmAnaDielectronTask : public FairTask {

public:
   /*
    * \brief Standard constructor.
    */
    CbmAnaDielectronTask();

    /*
     * \brief Standard destructor.
     */
    virtual ~CbmAnaDielectronTask();

    /*
     * \brief Inherited from FairTask.
     */
    virtual InitStatus Init();

    /*
     * \brief Inherited from FairTask.
     */
    virtual void Exec(
          Option_t *option);
    
    /*
     * \brief Creates 1D histograms for each analysis step.
     * \param[in,out] hist Vector if the histograms for each analysis step.
     * \param[in] name Base name of the histograms.
     * \param[in] axisX X axis title.
     * \param[in] axisY Y axis title.
     * \param[in] nBins Number of bins in the histogram.
     * \param[in] min Minimum value.
     * \param[in] max Maximum value.
     */
    void CreateAnalysisStepsH1(
          vector<TH1D*>& hist,
          const string& name,
          const string& axisX,
          const string& axisY,
          double nBins,
          double min,
          double max);


    /*
     * \brief Creates 2D histograms for each analysis step.
     * \param[in,out] hist Vector if the histograms for each analysis step.
     * \param[in] name Base name of the histograms.
     * \param[in] axisX X axis title.
     * \param[in] axisY Y axis title.
     * \param[in] axisZ Z axis title.
     * \param[in] nBinsX Number of bins for X axis in the histogram.
     * \param[in] minX Minimum value for X axis.
     * \param[in] maxX Maximum value for X axis.
     * \param[in] nBinsY Number of bins for Y axis in the histogram.
     * \param[in] minY Minimum value for Y axis.
     * \param[in] maxY Maximum value for Y axis.
     */
    void CreateAnalysisStepsH2(
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
          double maxY);

    /*
     * \brief Creates 1D histograms for different track source types.
     * \param[in,out] hist Vector if the histograms for each analysis step.
     * \param[in] name Base name of the histograms.
     * \param[in] axisX X axis title.
     * \param[in] axisY Y axis title.
     * \param[in] nBins Number of bins in the histogram.
     * \param[in] min Minimum value.
     * \param[in] max Maximum value.
     */
    void CreateSourceTypesH1(
          vector<TH1D*>& hist,
          const string& name,
          const string& axisX,
          const string& axisY,
          double nBins,
          double min,
          double max);


    /*
     * \brief Creates 2D histograms for different track source types.
     * \param[in,out] hist Vector if the histograms for each analysis step.
     * \param[in] name Base name of the histograms.
     * \param[in] axisX X axis title.
     * \param[in] axisY Y axis title.
     * \param[in] axisZ Z axis title.
     * \param[in] nBinsX Number of bins for X axis in the histogram.
     * \param[in] minX Minimum value for X axis.
     * \param[in] maxX Maximum value for X axis.
     * \param[in] nBinsY Number of bins for Y axis in the histogram.
     * \param[in] minY Minimum value for Y axis.
     * \param[in] maxY Maximum value for Y axis.
     */
    void CreateSourceTypesH2(
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
          double maxY);


    /*
     * \brief Fills histograms for pairs.
     * \param[in] candP Positive candidate.
     * \param[in] candM Negative candidate.
     * \param[in] step Enumeration AnalysisSteps, specify analysis step.
     * \param[in] parRec Kinematic parameters for reconstructed pair.
     */
    void PairSource(
    	CbmLmvmCandidate* candP,
		CbmLmvmCandidate* candM,
        CbmLmvmAnalysisSteps step,
		CbmLmvmKinematicParams* parRec);

    /*
     * \brief Fills minv, pty, mom histograms for specified analysis step.
     * \param[in] candP Positive candidate.
     * \param[in] candM Negative candidate.
     * \param[in] parMc MC kinematic parameters.
     * \param[in] parRec Reconstructed kinematic parameters.
     * \param[in] step Enumeration AnalysisSteps, specify analysis step.
     */
    void FillPairHists(
    	  CbmLmvmCandidate* candP,
		  CbmLmvmCandidate* candM,
		  CbmLmvmKinematicParams* parMc,
		  CbmLmvmKinematicParams* parRec,
          CbmLmvmAnalysisSteps step);

    void TrackSource(
    		CbmLmvmCandidate* cand,
    		CbmLmvmAnalysisSteps step,
    		Int_t pdg);

    void SingleParticleAcceptance();

    void FillRichRingNofHits();

    Bool_t IsMcTrackAccepted(
    		Int_t mcTrackInd);

    void RichPmtXY();

    void MCPairs();

    void PairMcAndAcceptance();

    void FillTopologyCandidates();

    void FillCandidates();


    void AssignMcToCandidates();

    void AssignMcToTopologyCandidates(
          vector<CbmLmvmCandidate>& cutCandidates);

    void DifferenceSignalAndBg();

    /*
     * \brief Initialize all histograms.
     */
    void InitHists();

    void SignalAndBgReco();

    void CheckGammaConvAndPi0();

    /*
     * \brief
     * \param[in] mvdStationNum MVD station number.
     * \param[in, out] hist Vector of histograms for different source types.
     */
    void CheckClosestMvdHit(
    		Int_t mvdStationNum,
    		vector<TH2D*>& hist,
         vector<TH1D*>& histQa);

    /*
     * \brief Set cut values and fill histograms for topology cut
     * \param[in] cutName ST or TT
     */
    void CheckTopologyCut(
          const string& cutName,
          const vector<CbmLmvmCandidate>& cutCandidates,
          const vector<TH2D*>& hcut,
          const vector<TH2D*>& hcutPion,
          const vector<TH2D*>& hcutTruepair,
          Double_t angleCut,
          Double_t ppCut);

    void CalculateNofTopologyPairs(
          TH1D* h_nof_pairs,
          const string& source);

    void MvdCutMcDistance();

    virtual void Finish();

    void FillElPiMomHist();


    ClassDef(CbmAnaDielectronTask,1);

private:

    CbmAnaDielectronTask(const CbmAnaDielectronTask&);
    CbmAnaDielectronTask& operator=(const CbmAnaDielectronTask&);
    
    Bool_t IsMismatch(
    		CbmLmvmCandidate* cand);

    Bool_t IsGhost(
    		CbmLmvmCandidate* cand);

    void IsElectron(
    	CbmRichRing * ring,
    	Double_t momentum,
    	CbmTrdTrack* trdTrack,
    	CbmGlobalTrack * gTrack,
		CbmLmvmCandidate* cand);

    Bool_t IsRichElectron(
        CbmRichRing* ring, 
        Double_t momentum, 
		CbmLmvmCandidate* cand);

    Bool_t IsTrdElectron(
        CbmTrdTrack* trdTrack, 
		CbmLmvmCandidate* cand);

    Bool_t IsTofElectron(
        CbmGlobalTrack* gTrack, 
        Double_t momentum, 
		CbmLmvmCandidate* cand);


    TClonesArray *fMCTracks;
    TClonesArray *fRichRings;
    TClonesArray *fRichProj;
    TClonesArray *fRichPoints;
    TClonesArray *fRichRingMatches;
    TClonesArray *fRichHits;
    TClonesArray *fGlobalTracks;
    TClonesArray *fStsTracks;
    TClonesArray *fStsTrackMatches;
    TClonesArray *fStsHits;
    TClonesArray *fMvdHits;
    TClonesArray *fMvdPoints;
    TClonesArray *fMvdHitMatches;
    TClonesArray *fTrdTracks;
    TClonesArray *fTrdHits;
    TClonesArray *fTrdTrackMatches;
    TClonesArray *fTofHits;
    TClonesArray *fTofPoints;
    CbmVertex *fPrimVertex;
    CbmKFVertex fKFVertex;
    CbmStsKFTrackFitter fKFFitter;

    CbmLitMCTrackCreator* fMCTrackCreator; // MC track creator tool

    Bool_t fUseMvd;
    Bool_t fUseRich;
    Bool_t fUseTrd;
    Bool_t fUseTof;

    vector<CbmLmvmCandidate> fCandidates;
    vector<CbmLmvmCandidate> fSTCandidates; // STCut Segmented tracks, reconstructed only in STS
    vector<CbmLmvmCandidate> fTTCandidates; // TTCut Reconstructed tracks, reconstructed in all detectors but not identified as electrons
    vector<CbmLmvmCandidate> fRTCandidates; // RTCut Reconstructed tracks, reconstructed in STS + at least in one of the  detectro (RICH, TRD, TOF)

    Double_t fWeight; //Multiplicity*BR

    Double_t fPionMisidLevel; // For the ideal particle identification cases, set to -1 for real PID
    TRandom3* fRandom3;

    //Bool_t fUseMcMomentum;

    CbmLmvmCuts fCuts; // electorn identification and analisys cuts

	CbmRichElectronIdAnn* fElIdAnn;

    vector<TH1*> fHistoList; //list of all histograms

    // Number of hits in the MC RICH ring
    std::map<Int_t, Int_t> fNofHitsInRingMap;

   TH1D* fh_mc_mother_pdg; //mother pdg code for e-/e+
   TH1D* fh_acc_mother_pdg; //mother pdg code for accepted e-/e+

   // X-Y distribution of MC pints on the RICH PMT plane
   TH2D* fh_signal_pmtXY;
   TH2D* fh_pi0_pmtXY;
   TH2D* fh_gamma_pmtXY;

   // Vertex of secondary electron from gamma conversion for different analysis step
   //Index is the analysis step: [0]-mc, [1]-acc, [2]-reco, [3]-chi2prim, [4]-elid,
   // [5]-gamma cut, [6]-mvd1cut, [7]-mvd2cut, [8]-stcut, [9]-ttcut, [10]-ptcut.
   vector<TH2D*> fh_vertex_el_gamma_xz;
   vector<TH2D*> fh_vertex_el_gamma_yz;
   vector<TH2D*> fh_vertex_el_gamma_xy;
   vector<TH2D*> fh_vertex_el_gamma_rz;//r=sqrt(x^2+y^2)

   //Index is the analysis step: [0]-mc, [1]-acc, [2]-reco, [3]-chi2prim, [4]-elid,
   // [5]-gamma cut, [6]-mvd1cut, [7]-mvd2cut, [8]-stcut, [9]-ttcut, [10]-ptcut.
   //Use AnalysisSteps enumeration for access.
   //MC and ACC histograms are not filled sometimes.
   vector<TH1D*> fh_signal_minv; // Invariant mass for Signal
   vector<TH1D*> fh_bg_minv; // Invariant mass for BG
   vector<TH1D*> fh_pi0_minv; // Invariant mass for Pi0
   vector<TH1D*> fh_eta_minv; // Invariant mass for Eta
   vector<TH1D*> fh_gamma_minv; // Invariant mass for Eta
   vector<TH1D*> fh_signal_mom; // Signal momentum distribution
   vector<TH2D*> fh_signal_pty; // Pt/y distribution for signal
   vector<TH2D*> fh_signal_minv_pt; // Invariant mass vs. MC Pt
   vector<TH2D*> fh_eta_minv_pt; // Invariant mass vs. MC Pt
   vector<TH2D*> fh_pi0_minv_pt; // Invariant mass vs. MC Pt


   vector<TH1D*> fh_bg_truematch_minv; // Invariant mass for truly matched tracks
   vector<TH1D*> fh_bg_truematch_el_minv; // Invariant mass for truly matched electron tracks
   vector<TH1D*> fh_bg_truematch_notel_minv; // Invariant mass for truly matched tracks, not 2 electrons
   vector<TH1D*> fh_bg_mismatch_minv; // Invariant mass for mis matches tracks

   //G-Gamma, P-Pi0, O-other
   //e-e+
   //[0]=G-G, [1]=P-P, [2]=O-O, [3]=G-P, [4]=G-O, [5]=P-O
   vector<vector<TH1D*> > fh_source_bg_minv; // Invariant mass for different source


   //Index is the source type: [0]-signal, [1]-bg, [2]-pi0, [3]-gamma
   //Use SourceTypes enumeration for access.
	vector<TH1D*> fh_pt; // Transverse momentum of single track distribution
   vector<TH1D*> fh_mom; //Momentum of the single track
   vector<TH1D*> fh_chi2sts; // Chi2 of the STS tracks
   vector<TH1D*> fh_chi2prim; // Chi2 of the primary vertex
   vector <TH2D*> fh_ttcut; // TT cut
   vector <TH2D*> fh_stcut; // ST cut
   vector <TH2D*> fh_rtcut; // RT cut
   vector<TH2D*> fh_mvd1cut; // MVD cut at the first station
   vector<TH2D*> fh_mvd2cut; // MVD cut at the second station
   vector<TH1D*> fh_richann; // RICH ANN
   vector<TH1D*> fh_trdann; // TRD ANN
   vector<TH2D*> fh_tofm2; // TOF m2
   vector<TH2D*> fh_ttcut_pion;
   vector<TH2D*> fh_ttcut_truepair;
   vector<TH2D*> fh_stcut_pion;
   vector<TH2D*> fh_stcut_truepair;
   vector<TH2D*> fh_rtcut_pion;
   vector<TH2D*> fh_rtcut_truepair;

   vector<TH1D*> fh_nofMvdHits; // number of MVD hits
   vector<TH1D*> fh_nofStsHits; // number of STS hits
   vector<TH2D*> fh_mvd1xy; // hit distribution in the first MVD station
   vector<TH1D*> fh_mvd1r; // r = x^2+y^2
   vector<TH2D*> fh_mvd2xy; // hit distribution in the second MVD station
   vector<TH1D*> fh_mvd2r; // r = x^2+y^2

   //Distant to MVD hit from the same  MotherId
   TH1D* fh_mvd1cut_mc_dist_gamma;
   TH1D* fh_mvd1cut_mc_dist_pi0;
   TH1D* fh_mvd2cut_mc_dist_gamma;
   TH1D* fh_mvd2cut_mc_dist_pi0;

   vector<TH1D*> fh_mvd1cut_qa; // MVD 1 cut quality
   vector<TH1D*> fh_mvd2cut_qa; // MVD 2 cut quality

   //source of BG pairs 2D.
   //second index is the analysis step: [0]-mc, [1]-acc, [2]-reco, [3]-chi2prim, [4]-elid,
   // [5]-gamma cut, [6]-mvd1cut, [7]-mvd2cut, [8]-stcut, [9]-ttcut, [10]-ptcut.
   //Use AnalysisSteps enumeration for access.
   vector<TH2D*> fh_source_pairs_epem;

   //X axis: analysis step
   //Y axis: [0]=G-G, [1]=P-P, [2]=O-O, [3]=G-P, [4]=G-O, [5]=P-O
   TH2D* fh_source_pairs;

   //store event number
   TH1D* fh_event_number;

   //nof signal and bg tracks after each cut
   TH1D* fh_nof_bg_tracks;
   TH1D* fh_nof_el_tracks;
   TH2D* fh_source_tracks;

   TH1D* fh_nof_topology_pairs_gamma;
   TH1D* fh_nof_topology_pairs_pi0;

   //nof gamma and pi0 pairs for different track categories : global, only STS or partially reconstructed
   TH1D* fh_nof_rec_pairs_gamma;
   TH1D* fh_nof_rec_pairs_pi0;

   //nof gamma and pi0 tracks for different track categories : global, only STS or partially reconstructed
   TH1D* fh_nof_rec_gamma;
   TH1D* fh_nof_rec_pi0;

   TH1D* fh_nof_mismatches;
   TH1D* fh_nof_mismatches_rich;
   TH1D* fh_nof_mismatches_trd;
   TH1D* fh_nof_mismatches_tof;
   TH1D* fh_nof_ghosts;

   //First index is the source type: [0]-signal, [1]-bg, [2]-pi0, [3]-gamma
   //Use SourceTypes enumeration for access.
   //second index is the analysis step: [0]-mc, [1]-acc, [2]-reco, [3]-chi2prim, [4]-elid,
   // [5]-gamma cut, [6]-mvd1cut, [7]-mvd2cut, [8]-stcut, [9]-ttcut, [10]-ptcut.
   //Use AnalysisSteps enumeration for access.
   //Track momentum distribution for different sources after each cut.
   vector<vector<TH1D*> > fh_source_mom;
   //Pt distribution for different sources after each cut.
   vector<vector<TH1D*> > fh_source_pt;
   //Opening angle distribution for different sources after each cut.
   vector<vector<TH1D*> > fh_opening_angle;

   //Pions vs momentum
   TH1D* fh_pi_mom_mc;
   TH1D* fh_pi_mom_acc;
   TH1D* fh_pi_mom_rec;
   TH1D* fh_pi_mom_rec_only_sts;
   TH1D* fh_pi_mom_rec_sts_rich_trd;
   TH1D* fh_pi_mom_rec_sts_rich_trd_tof;
   TH1D* fh_pi_rapidity_mc;

   //Pions vs momentum for primary pions v < 0.1 cm
   TH1D* fh_piprim_mom_mc;
   TH1D* fh_piprim_mom_acc;
   TH1D* fh_piprim_mom_rec;
   TH1D* fh_piprim_mom_rec_only_sts;
   TH1D* fh_piprim_mom_rec_sts_rich_trd;
   TH1D* fh_piprim_mom_rec_sts_rich_trd_tof;

   TH1D* fh_piprim_plus_rapidity_mc;
   TH1D* fh_piprim_minus_rapidity_mc;
   TH1D* fh_pi0prim_rapidity_mc;
   TH1D* fh_etaprim_rapidity_mc;

public:
   void SetUseMvd(Bool_t use){fUseMvd = use;};
   void SetUseRich(Bool_t use){fUseRich = use;};
   void SetUseTrd(Bool_t use){fUseTrd = use;};
   void SetUseTof(Bool_t use){fUseTof = use;};
   void SetWeight(Double_t weight){fWeight = weight;};

   // ID cuts
   void SetTrdAnnCut(Double_t par){fCuts.fTrdAnnCut = par;}

   void SetPionMisidLevel(Double_t level) {fPionMisidLevel = level;}
  // void SetMomentumCut(Double_t mom) {fMomentumCut = mom;}
};

#endif
