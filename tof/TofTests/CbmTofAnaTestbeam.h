// ------------------------------------------------------------------
// -----                     CbmTofTests                        -----
// -----              Created 29/08/2013 by P.-A. Loizeau       -----
// ------------------------------------------------------------------
#ifndef _CBMTOFANATESTBEAM_H_
#define _CBMTOFANATESTBEAM_H_

#include "FairTask.h"

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

class CbmTofAnaTestbeam : public FairTask {

   public:
       CbmTofAnaTestbeam();
       CbmTofAnaTestbeam(const char* name, Int_t verbose = 1);
       virtual ~CbmTofAnaTestbeam();

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
       ** @brief Initialize other parameters not included in parameter classes.
       **/
      Bool_t   InitParameters();
      /**
      ** @brief Load the geometry: for now just resizing the Digis temporary vectors
      **/
      Bool_t   LoadGeometry();

      CbmTofAnaTestbeam(const CbmTofAnaTestbeam&);
      CbmTofAnaTestbeam operator=(const CbmTofAnaTestbeam&);

      Int_t         fEvents;            // Number of processed events

      // Geometry infos
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

      TClonesArray          * fTofDigisColl;      // TOF Digis
      TClonesArray          * fTofHitsColl;       // TOF hits
      TClonesArray          * fTofDigiMatchColl;  // TOF DigiMatches


      // Histograms

      // Test class performance
 
      // Rates and data rates

      // Performances check
 
      // Control
      TTimeStamp fStart;
      TTimeStamp fStop;

      ClassDef(CbmTofAnaTestbeam, 1);
};

#endif

