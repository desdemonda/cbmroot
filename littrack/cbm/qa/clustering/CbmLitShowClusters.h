/*
 * CbmLitShowClusters.h
 *
 *  Created on: 18.05.2015
 *      Author: jonathan
 */

#ifndef CBMLITSHOWCLUSTERS_H_
#define CBMLITSHOWCLUSTERS_H_

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

class CbmLitShowClusters : public FairTask
{
public:
   /**
    * \brief Constructor.
    */
  CbmLitShowClusters();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitShowClusters();

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

    /**
     *
     */
    void CreateHistograms();


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
    //map<UInt_t, map<Int_t, CbmTrdDigi*>*> fMarker;

    CbmLitShowClusters(const CbmLitShowClusters&);
    CbmLitShowClusters& operator=(const CbmLitShowClusters&);

    ClassDef(CbmLitShowClusters, 1);
};



#endif /* CBMLITSHOWCLUSTERS_H_ */
