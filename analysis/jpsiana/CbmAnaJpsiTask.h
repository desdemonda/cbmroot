
#ifndef CBMJPSIANATASK
#define CBMJPSIANATASK

#include "FairTask.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TClonesArray.h"
#include "CbmHistManager.h"
#include "CbmKFVertex.h"
#include "CbmVertex.h"
#include "CbmAnaJpsiCandidate.h"
#include "CbmAnaJpsiCuts.h"
#include "CbmAnaJpsiHist.h"
#include "CbmAnaJpsiKinematicParams.h"
#include "CbmRichRing.h"
#include "CbmRichElectronIdAnn.h"
#include "CbmTrdTrack.h"
#include "CbmGlobalTrack.h"




using namespace std;

class CbmAnaJpsiTask : public FairTask
{

public:
   /**
    * \brief Standard constructor.
    */
	CbmAnaJpsiTask();

   /**
    * \brief Standard destructor.
    */
   virtual ~CbmAnaJpsiTask();

   /**
    * \brief Inherited from FairTask.
    */
   virtual InitStatus Init();

   /**
    * \brief Inherited from FairTask.
    */
   virtual void Exec(
		   Option_t* option);

   /**
    * \brief Inherited from FairTask.
    */
   virtual void Finish();
   



private:
  Int_t fEventNum;
  
  TClonesArray* fMcTracks;
  TClonesArray* fStsPoints;
  TClonesArray* fStsHits;
  TClonesArray* fStsTracks;
  TClonesArray* fStsTrackMatches;
  TClonesArray* fRichPoints; 
  TClonesArray* fRichHits;
  TClonesArray* fRichRings;
  TClonesArray* fRichRingMatches;
  TClonesArray* fTrdPoints; 
  TClonesArray* fTrdHits;
  TClonesArray* fTrdTracks;
  TClonesArray* fTrdTrackMatches;
  TClonesArray* fTofPoints; 
  TClonesArray* fTofHits;
  TClonesArray* fGlobalTracks;
  
  CbmVertex* fPrimVertex;
  CbmKFVertex fKFVertex;

  vector<CbmAnaJpsiCandidate> fCandidates; // array of JPsi candidates

  CbmAnaJpsiCuts fCuts; //electron identification and analysis cuts

  CbmRichElectronIdAnn* fElIdAnn;


  CbmHistManager* fHM;


  //First index is the source type: [0]-signal, [1]-bg, [2]-pi0, [3]-gamma
  //Use SourceTypes enumeration for access.
  //second index is the analysis step: [0]-mc, [1]-acc, [2]-reco, [3]-chi2prim, [4]-elid,
  // [5]-gamma cut, [6]-mvd1cut, [7]-mvd2cut, [8]-stcut, [9]-ttcut, [10]-ptcut.
  //Use AnalysisSteps enumeration for access.

  //Track momentum distribution for different sources after each cut.
  vector<vector<TH1D*> > fh_source_mom;
  //Pt distribution for different sources after each cut.
  vector<vector<TH1D*> > fh_source_pt;


  void CreateSourceTypesAnalysisStepsH1(
        const string& name,
        const string& axisX,
        const string& axisY,
        double nBins,
        double min,
        double max);

  /*
       * \brief Creates 1D histograms for each analysis step.
       * \param[in] name Base name of the histograms.
       * \param[in] axisX X axis title.
       * \param[in] axisY Y axis title.
       * \param[in] nBins Number of bins for X axis in the histogram.
       * \param[in] min Minimum value for X axis.
       * \param[in] max Maximum value for X axis.
       */
  void CreateAnalysisStepsH1(
        const string& name,
        const string& axisX,
        const string& axisY,
        double nBins,
        double min,
        double max);

  /*
       * \brief Creates 2D histograms for each analysis step.
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
   * \param[in] name Base name of the histograms.
   * \param[in] axisX X axis title.
   * \param[in] axisY Y axis title.
   * \param[in] nBins Number of bins in the histogram.
   * \param[in] min Minimum value.
   * \param[in] max Maximum value.
   */
  void CreateSourceTypesH1(
        const string& name,
        const string& axisX,
        const string& axisY,
        double nBins,
        double min,
        double max);


  /*
   * \brief Creates 2D histograms for different track source types.
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

    void InitHist();

    void MCPairs();

    void RichPmtXY();

    /**
     * \brief Fill fCandidates array with JPsiCandidates. Candidate should have STS, RICH, TRD, TOF local segments.
     *
     */
    void FillCandidates();

    /**
     * \brief Assign MC info to the candidates.
     */
    void AssignMcToCandidates();

    /**
     * \brief Return true if MC track is in detector acceptance
     */
    Bool_t IsMcTrackAccepted(
    		Int_t mcTrackInd);

    void SingleParticleAcceptance();

    /**
     * \brief Fill histograms for MC and Acc pairs
     */
    void PairMcAndAcceptance();



    void TrackSource(
    		CbmAnaJpsiCandidate* cand,
			CbmAnaJpsiAnalysisSteps step,
			Int_t pdg);

    void FillPairHists(
    	  CbmAnaJpsiCandidate* candP,
          CbmAnaJpsiCandidate* candM,
    	  CbmAnaJpsiKinematicParams* parMc,
    	  CbmAnaJpsiKinematicParams* parRec,
          CbmAnaJpsiAnalysisSteps step);

    void SignalAndBgReco();

    /**
         * \brief  Identifies particle as Electron (or not)
         */
    void IsElectron(
    		CbmRichRing* ring,
    		Double_t momentum,
    		CbmTrdTrack* trdTrack,
    	    CbmGlobalTrack* gTrack,
    		CbmAnaJpsiCandidate* cand);

    /**
     * \brief  find rich electron
     */
    Bool_t IsRichElectron(
    		CbmRichRing* ring,
			Double_t momentum,
			CbmAnaJpsiCandidate* cand);

    Bool_t IsTrdElectron(
          CbmTrdTrack* trdTrack,
    	  CbmAnaJpsiCandidate* cand);


    Bool_t IsTofElectron(
    	      CbmGlobalTrack* gTrack,
    	      Double_t momentum,
    		  CbmAnaJpsiCandidate* cand);

    Bool_t IsMismatch(
    		CbmAnaJpsiCandidate* cand);

    Bool_t IsGhost(
    		CbmAnaJpsiCandidate* cand);


    void BgIdentification();

    /**
     * \brief Fill histograms for signal and BG electrons
     */
    void DifferenceSignalAndBg();


    /**
     * \brief Calculate the number of Candidates after every Cut.
     */
    void NofCandidatesAfterCuts();

   /**
    * \brief Copy constructor.
    */
   CbmAnaJpsiTask(const CbmAnaJpsiTask&);

   /**
    * \brief Assignment operator.
    */
   CbmAnaJpsiTask& operator=(const CbmAnaJpsiTask&);


	ClassDef(CbmAnaJpsiTask,1)
};

#endif

