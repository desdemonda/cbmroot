/*
 * \file CbmLitClusteringQaTrd.h
 * \brief FairTask for clustering performance calculation.
 * \author Jonathan Pieper <j.pieper@gsi.de>
 * \date Dec 1, 2014
 */

#ifndef CBMLITCLUSTERINGQATRD_H_
#define CBMLITCLUSTERINGQATRD_H_

#include "FairTask.h"
#include "../cbm/base/CbmLitDetectorSetup.h"
#include "CbmMuchGeoScheme.h"
#include <string>
#include <vector>
using std::string;
using std::vector;
using std::map;
class CbmHistManager;
class TClonesArray;
class TCanvas;
class CbmTrdModule;
class CbmTrdDigiPar;

class CbmLitClusteringQaTrd : public FairTask
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitClusteringQaTrd();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitClusteringQaTrd();

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

    /** Setters */
    void SetOutputDir(const string& outputDir) { fOutputDir = outputDir; }
    void SetMuchDigiFileName(const string& digiFileName) { fMuchDigiFileName = digiFileName; }

private:

    Int_t GetStationId(
          Int_t address,
          DetectorId detId);

    /**
     * \brief Read data branches.
     */
    void ReadDataBranches();

    void ProcessSectorHistos();

    void FillModuleDigis(const string nr, Int_t col, Int_t row, Int_t charge);

    void InitMuchGeoScheme(const string& digiFileName);

    void ProcessPoints(
          const TClonesArray* points,
          const string& detName,
          DetectorId detId);

    void ProcessDigis(
          const TClonesArray* digis,
          const TClonesArray* digiMatches,
          const string& detName,
          DetectorId detId);

    void ProcessClusters(
          const TClonesArray* clusters,
          const TClonesArray* clusterMatches,
          const string& detName,
          DetectorId detId);

    void ProcessHits(
          const TClonesArray* hits,
          const TClonesArray* hitMatches,
          const string& detName,
          DetectorId detId);

    /**
     *
     */
    void FillEventCounterHistograms();

    /**
     *
     */
    void CreateHistograms();

    /**
     *
     */
    void CreateNofObjectsHistograms(
          DetectorId detId,
          const string& detName);

    /**
     *
     */
    void CreateNofObjectsHistograms(
          DetectorId detId,
          const string& detName,
          const string& parameter,
          const string& xTitle);

    void CreateClusterParametersHistograms(
          DetectorId detId,
          const string& detName);

    void FillResidualAndPullHistograms(
          const TClonesArray* points,
          const TClonesArray* hits,
          const TClonesArray* hitMatches,
          const string& detName,
          DetectorId detId);

    void FillHitEfficiencyHistograms(
          const TClonesArray* points,
          const TClonesArray* hits,
          const TClonesArray* hitMatches,
          const string& detName,
          DetectorId detId);

    void CreateHitEfficiencyHistograms(
          DetectorId detId,
          const string& detName,
          const string& parameter,
          const string& xTitle,
          Int_t nofBins,
          Double_t minBin,
          Double_t maxBin);

    CbmHistManager* fHM; // Histogram manager
    string fOutputDir; // Output directory for results
    CbmLitDetectorSetup fDet; // For detector setup determination

    // Pointers to data arrays
    TClonesArray* fMCTracks;

    TClonesArray* fMvdPoints; // CbmMvdPoint array
    TClonesArray* fMvdDigis; // CbmMvdDigi array
    TClonesArray* fMvdClusters; // CbmMvdClusters array
    TClonesArray* fMvdHits; // CbmMvdHit array

    TClonesArray* fStsPoints; // CbmStsPoint array
    TClonesArray* fStsDigis; // CbmStsDigi array
    TClonesArray* fStsClusters; // CbmStsCluster array
    TClonesArray* fStsHits; // CbmStsHit array
    TClonesArray* fStsDigiMatches; // CbmMatch array
    TClonesArray* fStsClusterMatches; // CbmMatch array
    TClonesArray* fStsHitMatches; // CbmMatch array

    TClonesArray* fRichPoints; // CbmRichPoint array
    TClonesArray* fRichHits; // CbmRichHits array

    TClonesArray* fMuchPoints; // CbmMuchPoint array
    TClonesArray* fMuchDigis; // CbmMuchDigi array
    TClonesArray* fMuchClusters; // CbmMuchCluster array
    TClonesArray* fMuchPixelHits; // CbmMuchPixelHits array
    TClonesArray* fMuchStrawHits; // CbmMuchStrawHits array
    TClonesArray* fMuchDigiMatches; // CbmMatch array
    TClonesArray* fMuchClusterMatches; // CbmMatch array
    TClonesArray* fMuchPixelHitMatches; // CbmMatch array
    TClonesArray* fMuchStrawHitMatches; // CbmMatch array

    TClonesArray* fTrdPoints; // CbmTrdPoint array
    TClonesArray* fTrdDigis; // CbmTrdDigi array
    TClonesArray* fTrdClusters; // CbmTrdCluster array
    TClonesArray* fTrdHits; // CbmTrdHit array
    TClonesArray* fTrdDigiMatches; // CbmMatch array
    TClonesArray* fTrdClusterMatches; // CbmMatch array
    TClonesArray* fTrdHitMatches; // CbmMatches array

    TClonesArray* fTofPoints; // CbmTofPoint array
    TClonesArray* fTofHits; // CbmTofHit array

    CbmTrdDigiPar *fDigiPar;
    CbmTrdModule  *fModuleInfo;

    string fMuchDigiFileName;
    map<UInt_t, TCanvas*> fCanvases;

    CbmLitClusteringQaTrd(const CbmLitClusteringQaTrd&);
    CbmLitClusteringQaTrd& operator=(const CbmLitClusteringQaTrd&);

    ClassDef(CbmLitClusteringQaTrd, 1);
};

#endif /* CBMLITCLUSTERINGQATRD_H_ */
