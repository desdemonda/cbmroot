// ------------------------------------------------------------------
// -----                     CbmTofTests                        -----
// -----              Created 29/08/2013 by P.-A. Loizeau       -----
// ------------------------------------------------------------------
#ifndef _CBMTOFTESTS_H_
#define _CBMTOFTESTS_H_

#include "FairTask.h"
#include "CbmMCEventHeader.h"

class CbmTofGeoHandler;
class CbmTofCell;
// Geometry
class CbmTofGeoHandler;
class CbmTofDetectorId;
class CbmTofCell;

class CbmTofDigiPar;
class CbmTofDigiBdfPar;

class TClonesArray;
class TH1;
class TH2;
class TString;
#include "TTimeStamp.h"

class CbmTofTests : public FairTask {

   public:
       CbmTofTests();
       CbmTofTests(const char* name, Int_t verbose = 1);
       virtual ~CbmTofTests();

       virtual InitStatus Init();
       virtual void Exec(Option_t* option);
       virtual void Finish();

       /**
        ** @brief Inherited from FairTask.
        **/
       virtual void SetParContainers();

       Bool_t   RegisterInputs();

   private:
      Bool_t   CreateHistos();
      Bool_t   FillHistos();
      Bool_t   WriteHistos();
      Bool_t   DeleteHistos();

      /**
      ** @brief Load the geometry: for now just resizing the Digis temporary vectors
      **/
      Bool_t   LoadGeometry();

      CbmTofTests(const CbmTofTests&);
      CbmTofTests operator=(const CbmTofTests&);

      Int_t         fEvents;            // Number of processed events

      // Geometry infos
      CbmMCEventHeader *fMCEventHeader;
      CbmTofGeoHandler * fGeoHandler;
      CbmTofDetectorId * fTofId;
      CbmTofCell       * fChannelInfo;
      Int_t iNbSmTot;
      std::vector< Int_t >                               fvTypeSmOffs; // Offset in SM index for first SM of each SM type
      Int_t iNbRpcTot;
      std::vector< std::vector< Int_t > >                fvSmRpcOffs;  // Offset in RPC index for first RPC of each SM
      Int_t iNbChTot;
      std::vector< std::vector< std::vector< Int_t > > > fvRpcChOffs;  // Offset in channel index for first channel of each RPC
    
      // Parameters
      CbmTofDigiPar    * fDigiPar;
      CbmTofDigiBdfPar * fDigiBdfPar;

      TClonesArray          * fTofPointsColl; // TOF MC points
      TClonesArray          * fMcTracksColl;  // MC tracks
      TClonesArray          * fTofDigisColl;  // TOF Digis
      TClonesArray          * fTofHitsColl; // TOF hits
//    TClonesArray *fStsPoints;     // STS points
//    TClonesArray *fStsHits;       // STS hits
//    TClonesArray *fStsTracks;     // STS tracks
//    TClonesArray *fGlobalTracks;  // Global tracks
//    TClonesArray *fHadrons;       // Hadrons
//    TClonesArray *fRichHits;      // RICH hits
//    Bool_t        fbRichThere;    // RICH flag
//    TClonesArray *fTrdHits;       // TRD hits
//    Bool_t        fbTrdThere;     // TRD flag

      // Histograms
         // Test class performance
      TH1* fhTestingTime;
         // Mapping
      TH2 * fhPointMapXY;
      TH2 * fhPointMapXZ;
      TH2 * fhPointMapYZ;
      TH2 * fhPointMapAng;
      TH2 * fhPointMapSph;
      TH2 * fhDigiMapXY;
      TH2 * fhDigiMapXZ;
      TH2 * fhDigiMapYZ;
      TH2 * fhDigiMapAng;
      TH2 * fhDigiMapSph;
      TH2 * fhHitMapXY;
      TH2 * fhHitMapXZ;
      TH2 * fhHitMapYZ;
      TH2 * fhHitMapAng;
      TH2 * fhHitMapSph;

         // Rates and data rates
      TH2 * fhFluxMap;
      TH2 * fhDigiFluxMap;
      TH2 * fhHitFluxMap;
      TH1 * fhDigiRateCh;
      TH1 * fhDataRateCh;
      TH1 * fhDataRateRpc;
      TH1 * fhDataRateSm;
      TH1 * fhDataRateType;
      TH1 * fhTofDataPerEvt;
      TH1 * fhTofDataRedEvt;
//      TH1 * fhHitRateCh;
      TH1 * fhOptLnkRpc;
      TH1 * fhOptLnkSm;
      TH1 * fhOptLnkType;

         // Performances check
      TH1 * fhTofRes;
      TH1 * fhTofResSing;
      TH2 * fhTofPosDifX;
      TH2 * fhTofPosDifY;
      TH2 * fhTofPosDifZ;
      TH2 * fhTofPosDifSingXX;
      TH2 * fhTofPosDifSingXY;
      TH2 * fhTofPosDifSingXZ;
      TH2 * fhTofPosDifSingY;
      TH2 * fhTofPosDifSingZ;
      TH2 * fhTofPosDifXZSing;
      TH2 * fhTofBadXPosSing;
      TH2 * fhTofBadYPosSing;
      TH2 * fhTofBadZPosSing;
      TH1 * fhTofEff;
      TH1 * fhTofMixing;

      // Control
      TTimeStamp fStart;
      TTimeStamp fStop;

      ClassDef(CbmTofTests, 1);
};

#endif

