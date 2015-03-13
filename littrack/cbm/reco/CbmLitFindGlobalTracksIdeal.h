/**
 * \file CbmLitFindGlobalTracksIdeal.h
 * \brief FairTask for ideal global track reconstruction.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 */

#ifndef CBMLITFINDGLOBALTRACKSIDEAL_H_
#define CBMLITFINDGLOBALTRACKSIDEAL_H_

#include "base/CbmLitDetectorSetup.h"
#include "FairTask.h"

#include <map>

class TClonesArray;

/**
 * \class CbmLitFindGlobalTracksIdeal
 * \brief FairTask for ideal global track reconstruction.
 *
 * FairTask class for ideal global track reconstruction.
 * Combines local track segments to a global track based on MC information.
 * Local ideal or real tracking has to be performed in advance.
 * Also track matching tasks has to be executed in advance.
 * Produces objects of type CbmGlobalTrack.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 *
 */
class CbmLitFindGlobalTracksIdeal : public FairTask
{
public:
   /*
    * \brief Default constructor.
    */
   CbmLitFindGlobalTracksIdeal();

   /*
    * \brief Destructor.
    */
   virtual ~CbmLitFindGlobalTracksIdeal();

   /**
    * \brief Derived from FairTask.
    */
   virtual InitStatus Init();

   /**
     * \brief Derived from FairTask.
     */
   virtual void Exec(Option_t* opt);

private:
   /**
    * \brief Derived from FairTask.
    */
   virtual void Finish();

   /**
    * \brief Read necessary data branches from the input data files.
    */
   void ReadDataBranches();

   /**
     * \brief Fill map from <MC track index> to <reconstructed track index>.
     */
   void FillTrackMap(
      std::map<Int_t, Int_t>& mcMap,
      const TClonesArray* matches);

   /**
    * \brief Fill map from <MC track index> to <TOF hit index>.
    */
   void FillMapTof();

   /**
    * \brief Create output CbmGlobalTracks and write them to output array.
    */
   void CreateGlobalTracks();

   CbmLitDetectorSetup fDet;

   // Pointers to data arrays
   TClonesArray* fMCTracks; // CbmMCTrack
   TClonesArray* fStsMatches; // CbmTrackMatchNew
   TClonesArray* fMuchMatches; // CbmTrackMatchNew
   TClonesArray* fTrdMatches; // CbmTrackMatchNew
   TClonesArray* fTofMCPoints; // CbmTofPoint
   TClonesArray* fTofHits; // CbmTofHit
   TClonesArray* fGlobalTracks; //output: CbmGlobalTrack

   // Maps for reconstructed tracks
   // <MC track index, reconstructed track index>
   std::map<Int_t, Int_t> fMcStsMap; // STS
   std::map<Int_t, Int_t> fMcTrdMap; // TRD
   std::map<Int_t, Int_t> fMcMuchMap; // MUCH
   std::map<Int_t, Int_t> fMcTofMap; //TOF

   Int_t fEventNo; //event counter

   CbmLitFindGlobalTracksIdeal(const CbmLitFindGlobalTracksIdeal&);
   CbmLitFindGlobalTracksIdeal& operator=(const CbmLitFindGlobalTracksIdeal&);
   
   ClassDef(CbmLitFindGlobalTracksIdeal, 1);
};

#endif /* CBMLITFINDGLOBALTRACKSIDEAL_H_ */
