/**
 * \file CbmLitTrackingQa.h
 * \brief FairTask for tracking performance calculation.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2007
 */

#ifndef CBMLITTRACKINGQA_H_
#define CBMLITTRACKINGQA_H_

#include "FairTask.h"
#include "CbmDetectorList.h"
#include "cbm/base/CbmLitDetectorSetup.h"

class CbmHistManager;
class CbmTrackMatchNew;
class CbmLitMCTrackCreator;
class CbmLitGlobalElectronId;

using std::string;
using std::vector;
using std::multimap;


/**
 * \class CbmLitTrackingQa.h
 * \brief FairTask for tracking performance calculation.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2007
 */
class CbmLitTrackingQa : public FairTask
{
public:

   /**
    * \brief Constructor.
    */
   CbmLitTrackingQa();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitTrackingQa();

   /**
    * \brief Derived from FairTask.
    */
   virtual InitStatus Init();

   /**
    * \brief Derived from FairTask.
    */
   virtual void Exec(
      Option_t* opt);

   /**
    * \brief Derived from FairTask.
    */
   virtual void Finish();

   /** Setters **/
   void SetMinNofPointsSts(Int_t minNofPointsSts) { fMinNofPointsSts = minNofPointsSts; }
   void SetMinNofPointsTrd(Int_t minNofPointsTrd) { fMinNofPointsTrd = minNofPointsTrd; }
   void SetMinNofPointsMuch(Int_t minNofPointsMuch) { fMinNofPointsMuch = minNofPointsMuch; }
   void SetMinNofPointsTof(Int_t minNofPointsTof) { fMinNofPointsTof = minNofPointsTof; }
   void SetMinNofHitsRich(Int_t minNofHitsRich) { fMinNofHitsRich = minNofHitsRich; }
   void SetQuota(Double_t quota) { fQuota = quota; }
   void SetQuotaRich(Double_t quotaRich) { fQuotaRich = quotaRich; }
   void SetMinNofHitsTrd(Int_t minNofHitsTrd) { fMinNofHitsTrd = minNofHitsTrd; }
   void SetMinNofHitsMuch(Int_t minNofHitsMuch) { fMinNofHitsMuch = minNofHitsMuch; }
   void SetUseConsecutivePointsInSts(Bool_t useConsecutivePointsInSts) { fUseConsecutivePointsInSts = useConsecutivePointsInSts; }
   void SetOutputDir(const std::string& dir) { fOutputDir = dir; }
   void SetPRange(Int_t bins, Int_t min, Int_t max) {
      fPRangeBins = bins;
      fPRangeMin = min;
      fPRangeMax = max;
   }
   void SetYRange(Int_t bins, Int_t min, Int_t max) {
      fYRangeBins = bins;
      fYRangeMin = min;
      fYRangeMax = max;
   }
   void SetPtRange(Int_t bins, Int_t min, Int_t max) {
      fPtRangeBins = bins;
      fPtRangeMin = min;
      fPtRangeMax = max;
   }
   void SetAngleRange(
         Double_t min,
         Double_t max,
         Int_t nofBins) {
      fAngleRangeMin = min;
      fAngleRangeMax = max;
      fAngleRangeBins = nofBins;
   }
   void SetTrackCategories(const vector<string>& trackCategories) { fTrackCategories = trackCategories; }
   void SetRingCategories(const vector<string>& ringCategories) { fRingCategories = ringCategories; }

   void SetRichAnnCut(Double_t par){fRichAnnCut = par;}
   void SetTrdAnnCut(Double_t par){fTrdAnnCut = par;}

private:
   /**
    * \brief Read data branches from input data files.
    */
   void ReadDataBranches();

   /**
    * \brief Fill array of track categories with default values.
    */
   void FillDefaultTrackCategories();

   void FillDefaultRingCategories();

   void FillDefaultTrackPIDCategories();

   void FillDefaultPiSuppCategories();

   void FillDefaultRingPIDCategories();

   void FillTrackAndRingAcceptanceFunctions();


   void CreateH1Efficiency(
         const string& name,
         const string& parameter,
         const string& xTitle,
         Int_t nofBins,
         Double_t minBin,
         Double_t maxBin,
         const string& opt);

   void CreateH2Efficiency(
         const string& name,
         const string& parameter,
         const string& xTitle,
         const string& yTitle,
         Int_t nofBinsX,
         Double_t minBinX,
         Double_t maxBinX,
         Int_t nofBinsY,
         Double_t minBinY,
         Double_t maxBinY,
         const string& opt);

   void CreateH1PionSuppression(
         const string& name,
         const string& parameter,
         const string& xTitle,
         Int_t nofBins,
         Double_t minBin,
         Double_t maxBin);

   void CreateH1(
         const string& name,
         const string& xTitle,
         const string& yTitle,
         Int_t nofBins,
         Double_t minBin,
         Double_t maxBin);

   void CreateH2(
         const string& name,
         const string& xTitle,
         const string& yTitle,
         const string& zTitle,
         Int_t nofBinsX,
         Double_t minBinX,
         Double_t maxBinX,
         Int_t nofBinsY,
         Double_t minBinY,
         Double_t maxBinY);

   void CreateTrackHitsHistogram(
         const string& detName);

   vector<string> CreateGlobalTrackingHistogramNames(
         const vector<string>& detectors);

   vector<string> CreateGlobalTrackingHistogramNames();

   string LocalEfficiencyNormalization(
         const string& detName);

   vector<string> GlobalTrackVariants();

   vector<string> PionSuppressionVariants();

   void CreateHistograms();

   /**
    * \brief Loop over the reconstructed global tracks.
    * Check if track is correct and fill
    * multimap <MC track index, reconstructed track index>.
    */
   void ProcessGlobalTracks();

   /**
    * \brief Loop over the reconstructed RICH rings.
    * Check if ring is correct and fill
    * multimap <MC track index, reconstructed ring index>.
    */
   void ProcessRichRings();

   /**
    * \brief Check correctness attached MVD hits.
    */
   void ProcessMvd(
      Int_t stsId);

   void FillTrackQualityHistograms(
      const CbmTrackMatchNew* trackMatch,
      DetectorId detId);

   /**
    * \brief Loop over the MC tracks. Check track acceptance for different cases.
    * Fill histograms of accepted and reconstructed tracks.
    */
   void ProcessMcTracks();

//   /**
//    * \brief Calculate efficiency histograms.
//    */
//   void CalculateEfficiencyHistos();

   /**
    * \brief Fill histograms of accepted and reconstructed tracks.
    * \param[in] mcTrack MC track pointer.
    * \param[in] mcId MC track index in array.
    * \param[in] mcMap Map from MC track index to reconstructed track index. Map is filled in ProcessGlobalTrack function.
    * \param[in] hist main name of the histograms to be filled.
    * \param[in] par Value that will be added in histogram (momentum or number of points).
    */
   void FillGlobalReconstructionHistos(
         Int_t mcId,
         const multimap<Int_t, Int_t>& mcMap,
         const string& histName,
         const string& histTypeName,
         const string& effName,
         const string& catName,
         const vector<Double_t>& par);

    /**
     * \brief Fill histograms of accepted and reconstructed rings tracks.
     * \param[in] mcTrack MC track pointer.
     * \param[in] mcId MC track index in the array.
     * \param[in] mcMap Map from MC track index to reconstructed track index. Map is filled in the ProcessGlobalTrack function.
     * \param[in] hist main name of the histograms to be filled.
     * \param[in] par Value that will be added in histogram (momentum or number of points)
     */
   void FillGlobalReconstructionHistosRich(
         Int_t mcId,
         const multimap<Int_t, Int_t>& mcMap,
         const string& histName,
         const string& histTypeName,
         const string& effName,
         const string& catName,
         const vector<Double_t>& par);

   Bool_t ElectronId(
         Int_t mcId,
         const multimap<Int_t, Int_t>& mcMap,
         const string& effName);

   void PionSuppression();

   /**
    * \brief Increase number of objects counters.
    */
   void IncreaseCounters();

   CbmHistManager* fHM; // Histogram manager
   string fOutputDir; // Output directory for results
   CbmLitDetectorSetup fDet; // Detector presence information
   CbmLitMCTrackCreator* fMCTrackCreator; // MC track creator tool

   // Acceptance defined by MC points
   Int_t fMinNofPointsSts; // Minimal number of MCPoints in STS
   Int_t fMinNofPointsTrd; // Minimal number of MCPoints in TRD
   Int_t fMinNofPointsMuch; // Minimal number of MCPoints in MUCH
   Int_t fMinNofPointsTof; // Minimal number of MCPoints in TOF
   Double_t fQuota;  // True/all hits for track to be considered correctly reconstructed

   Bool_t fUseConsecutivePointsInSts; // Use consecutive MC points for STS normalization

   // Acceptance in RICH
   Int_t fMinNofHitsRich; // Minimal number of hits in MC RICH ring
   Double_t fQuotaRich; // True/all hits for ring to be considered correctly reconstructed

   // Minimal number of hits in track to be considered as accepted.
   // This is needed because the definition of the correctly reconstructed track
   // is 70% of hits in the reconstructed track are from the same MC track.
   // But this can lead to the situation when a reconstructed track has 4 hits,
   // all of them from the same MC track, which means that the track is correctly
   // reconstructed. BUT the real MC track has 12 points(hits). This is especially
   // important for the MUCH detector. To avoid such situations one has to put
   // such cut.
   Int_t fMinNofHitsTrd; // for TRD track
   Int_t fMinNofHitsMuch; // for MUCH track

   Double_t fPRangeMin; // Minimum momentum for tracks for efficiency calculation [GeV/c]
   Double_t fPRangeMax; // Maximum momentum for tracks for efficiency calculation [GeV/c]
   Int_t fPRangeBins; // Number of bins for efficiency vs. momentum histogram
   Double_t fPtRangeMin; // Minimum Pt for tracks for efficiency calculation [GeV/c]
   Double_t fPtRangeMax; // Maximum Pt for tracks for efficiency calculation [GeV/c]
   Int_t fPtRangeBins; // Number of bins for efficiency vs. Pt histogram
   Double_t fYRangeMin; // Minimum rapidity for tracks for efficiency calculation [GeV/c]
   Double_t fYRangeMax; // Maximum rapidity for tracks for efficiency calculation [GeV/c]
   Int_t fYRangeBins; // Number of bins for efficiency vs. rapidity histogram
   Double_t fAngleRangeMin; // Minimum polar angle [grad]
   Double_t fAngleRangeMax; // Maximum polar angle [grad]
   Int_t fAngleRangeBins; // Number of bins for efficiency vs. polar angle histogram

   // Pointers to data arrays
   TClonesArray* fMCTracks; // CbmMCTrack array
   TClonesArray* fGlobalTracks; // CbmGlobalTrack array
   TClonesArray* fMvdPoints; // CbmMvdPoint array
   TClonesArray* fMvdHitMatches; // CbmMvdHitMatch array
   TClonesArray* fStsTracks; // CbmStsTrack array
   TClonesArray* fStsMatches; // CbmStsTrackMatch array
   TClonesArray* fRichRings; // CbmRichRing array
   TClonesArray* fRichProjections; // CbmRichProjection array
   TClonesArray* fRichRingMatches; // CbmRichMatchesn array
   TClonesArray* fMuchMatches; // CbmTrackMatchNew array
   TClonesArray* fTrdMatches; // CbmTrackMatchNew array
   TClonesArray* fTofPoints; // CbmTofPoint array
   TClonesArray* fTofHits; // CbmTofHit array

   // Global track segment name maps to multimap <MC track index, reconstructed track index>
   map<string, multimap<Int_t, Int_t> > fMcToRecoMap;

   vector<string> fTrackCategories; // Vector of track category names
   vector<string> fRingCategories; // Vector of ring category names
   vector<string> fTrackCategoriesPID; // Vector of track category names
   vector<string> fRingCategoriesPID; // Vector of ring category names for REC+PID
   vector<string> fPiSuppCategories;; // Vector of categories for pion suppression

#ifndef  __ROOTCLING__
   typedef Bool_t (*LitTrackAcceptanceFunction)(const TClonesArray* mcTracks, Int_t index);
   typedef Bool_t (*LitRingAcceptanceFunction)(const TClonesArray* mcTracks, Int_t index, Int_t nofHitsInRing);
   typedef Bool_t (*LitPiSuppAcceptanceFunction)(const TClonesArray* globalTracks, const TClonesArray* stsMatches, const TClonesArray* richMatches, Int_t index);
   map<string, LitTrackAcceptanceFunction> fTrackAcceptanceFunctions; // maps track category name to track acceptance function
   map<string, LitRingAcceptanceFunction> fRingAcceptanceFunctions; // maps ring category name to ring acceptance function
   map<string, LitPiSuppAcceptanceFunction> fPiSuppAcceptanceFunctions; // maps pion supp. category name to piSupp acceptance function
#endif

   CbmLitGlobalElectronId* fElectronId; // Electron identification tool
   Double_t fTrdAnnCut;
   Double_t fRichAnnCut;

   CbmLitTrackingQa(const CbmLitTrackingQa&);
   CbmLitTrackingQa& operator=(const CbmLitTrackingQa&);
   
   ClassDef(CbmLitTrackingQa, 1);
};

#endif /* CBMLITTRACKINGQA_H_ */
