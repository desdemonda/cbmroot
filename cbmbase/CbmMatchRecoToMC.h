/**
 * \file CbmMatchRecoToMC.h
 * \brief FairTask for matching RECO data to MC.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 */

#ifndef CBMMATCHRECOTOMC_H_
#define CBMMATCHRECOTOMC_H_

#include "FairTask.h"
class TClonesArray;

class CbmMatchRecoToMC : public FairTask
{
public:
   /**
    * \brief Constructor.
    */
   CbmMatchRecoToMC();

   /**
    * \brief Destructor.
    */
   virtual ~CbmMatchRecoToMC();

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

private:
    /**
     * \brief Read and create data branches.
     */
    void ReadAndCreateDataBranches();

    void MatchClusters(
          const TClonesArray* digiMatches,
          const TClonesArray* clusters,
          TClonesArray* clusterMatches);

    void MatchHits(
          const TClonesArray* matches,
          const TClonesArray* hits,
          TClonesArray* hitMatches);

    void MatchHitsSts(
          const TClonesArray* matches,
          const TClonesArray* hits,
          TClonesArray* hitMatches);

    void MatchHitsMvd(
          const TClonesArray* matches,
          const TClonesArray* hits,
          TClonesArray* hitMatches);

    void MatchHitsToPoints(
          const TClonesArray* points,
          const TClonesArray* hits,
          TClonesArray* hitMatches);

    void MatchTracks(
          const TClonesArray* hitMatches,
          const TClonesArray* points,
          const TClonesArray* tracks,
          TClonesArray* trackMatches);

    // Pointers to data arrays
    TClonesArray* fMCTracks;

    // STS
    TClonesArray* fStsPoints; // CbmStsPoint array
    TClonesArray* fStsDigis; // CbmStsDigi array
    TClonesArray* fStsClusters; // CbmStsCluster array
    TClonesArray* fStsHits; // CbmStsHit array
    TClonesArray* fStsTracks; // CbmStsTrack array
    TClonesArray* fStsDigiMatches; // Output CbmMatch array
    TClonesArray* fStsClusterMatches; // Output CbmMatch array
    TClonesArray* fStsHitMatches; // Output CbmMatch array
    TClonesArray* fStsTrackMatches; // Output CbmTrackMatchNew array

    // TRD
    TClonesArray* fTrdPoints; // CbmTrdPoint array
    TClonesArray* fTrdDigis; // CbmTrdDigi array
    TClonesArray* fTrdClusters; // CbmTrdCluster array
    TClonesArray* fTrdHits; // CbmTrdHit array
    TClonesArray* fTrdTracks; // CbmTrdTrack array
    TClonesArray* fTrdDigiMatches; // Output CbmMatch array
    TClonesArray* fTrdClusterMatches; // Output CbmMatch array
    TClonesArray* fTrdHitMatches; // Output CbmMatch array
    TClonesArray* fTrdTrackMatches; // Output CbmTrackMatchNew array

    // MUCH
    TClonesArray* fMuchPoints; // CbmMuchPoint array
    TClonesArray* fMuchPixelDigis; // CbmMuchDigi array
    TClonesArray* fMuchStrawDigis; // CbmMuchStrawDigi array
    TClonesArray* fMuchClusters; // CbmMuchCluster array
    TClonesArray* fMuchPixelHits; // CbmMuchPixelHit array
    TClonesArray* fMuchStrawHits; // CbmMuchStrawHit array
    TClonesArray* fMuchTracks; // CbmMuchTrack array
    TClonesArray* fMuchPixelDigiMatches; // Output CbmMatch array
    TClonesArray* fMuchStrawDigiMatches; // Output CbmMatch array
    TClonesArray* fMuchClusterMatches; // Output CbmMatch array
    TClonesArray* fMuchPixelHitMatches; // Output CbmMatch array
    TClonesArray* fMuchStrawHitMatches; // Output CbmMatch array
    TClonesArray* fMuchTrackMatches; // Output CbmMatch array

    // MVD

    TClonesArray* fMvdHits; 		// CbmMvdHit array
    TClonesArray* fMvdDigiMatches;	// CbmMatch array
    TClonesArray* fMvdHitMatches; 	// Output CbmMatch array
    TClonesArray* fMvdCluster;       	// CbmMvdCluster array
    TClonesArray* fMvdClusterMatches;   // Output CbmMatch array



    CbmMatchRecoToMC(const CbmMatchRecoToMC&);
    CbmMatchRecoToMC& operator=(const CbmMatchRecoToMC&);

    ClassDef(CbmMatchRecoToMC, 1);
};

#endif /* CBMMATCHRECOTOMC_H_ */
