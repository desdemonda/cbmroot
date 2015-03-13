// ------------------------------------------------------------------
// -----                     CbmTofAnaTestbeam                        -----
// -----              Created 12/04/2014 by nh                 -----
// ------------------------------------------------------------------

#include "CbmTofAnaTestbeam.h"

// TOF Classes and includes
#include "CbmTofDigi.h"       // in cbmdata/tof
#include "CbmTofDigiExp.h"    // in cbmdata/tof
#include "CbmTofHit.h"        // in cbmdata/tof
#include "CbmTofGeoHandler.h" // in tof/TofTools
#include "CbmTofDetectorId_v12b.h" // in cbmdata/tof
#include "CbmTofDetectorId_v14a.h" // in cbmdata/tof
#include "CbmTofCell.h"       // in tof/TofData
#include "CbmTofDigiPar.h"    // in tof/TofParam
#include "CbmTofDigiBdfPar.h" // in tof/TofParam
#include "CbmTofAddress.h"    // in cbmdata/tof
#include "CbmMatch.h"

// CBMroot classes and includes


// FAIR classes and includes
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairRootManager.h"
#include "FairLogger.h"

// ROOT Classes and includes
#include "Riostream.h"
#include "TClonesArray.h"
#include "TH1.h"
#include "TH2.h"
#include "TString.h"
#include "TFile.h"
#include "TMath.h"
#include "TRandom.h"
#include "TROOT.h"

//___________________________________________________________________
//
// CbmTofAnaTestbeam
//
// Task for analysis of Testbeam data
//
// ------------------------------------------------------------------
CbmTofAnaTestbeam::CbmTofAnaTestbeam()
  : FairTask("HadronAnalysis"),
    fEvents(0),
    fGeoHandler(new CbmTofGeoHandler()),
    fTofId(NULL),
    fChannelInfo(NULL),
    iNbSmTot(0),
    fvTypeSmOffs(),
    iNbRpcTot(0),
    fvSmRpcOffs(),
    iNbChTot(0),
    fvRpcChOffs(),
    fDigiPar(NULL),
    fDigiBdfPar(NULL),
    fTofDigisColl(NULL),
  fTofDigiMatchColl(NULL),
    fTofHitsColl(NULL),
    fStart(),
    fStop()
{
  cout << "CbmTofTests: Task started " << endl;
}
// ------------------------------------------------------------------

// ------------------------------------------------------------------
CbmTofAnaTestbeam::CbmTofAnaTestbeam(const char* name, Int_t verbose)
  : FairTask(name, verbose),
    fEvents(0),
    fGeoHandler(new CbmTofGeoHandler()),
    fTofId(NULL),
    fChannelInfo(NULL),
    iNbSmTot(0),
    fvTypeSmOffs(),
    iNbRpcTot(0),
    fvSmRpcOffs(),
    iNbChTot(0),
    fvRpcChOffs(),
    fDigiPar(NULL),
    fDigiBdfPar(NULL),
    fTofDigisColl(NULL),
    fTofDigiMatchColl(NULL),
    fTofHitsColl(NULL),
    fStart(),
    fStop()
{
}
// ------------------------------------------------------------------

// ------------------------------------------------------------------
CbmTofAnaTestbeam::~CbmTofAnaTestbeam()
{
    // Destructor
}
// ------------------------------------------------------------------
/************************************************************************************/
// FairTasks inherited functions
InitStatus CbmTofAnaTestbeam::Init()
{
   if( kFALSE == RegisterInputs() )
      return kFATAL;

   //   fTofId = new ( CbmTofDetectorId )CbmTofDetectorId_v14a();
   if( kFALSE == InitParameters() )
      return kFATAL;

   if( kFALSE == LoadGeometry() )
      return kFATAL;

   if( kFALSE == CreateHistos() )
      return kFATAL;

   return kSUCCESS;
}

void CbmTofAnaTestbeam::SetParContainers()
{
   LOG(INFO)<<" CbmTofAnaTestbeam => Get the digi parameters for tof"<<FairLogger::endl;
   return;
   // Get Base Container
   FairRunAna* ana = FairRunAna::Instance();
   FairRuntimeDb* rtdb=ana->GetRuntimeDb();

   fDigiPar = (CbmTofDigiPar*) (rtdb->getContainer("CbmTofDigiPar"));

   fDigiBdfPar = (CbmTofDigiBdfPar*) (rtdb->getContainer("CbmTofDigiBdfPar"));
}

void CbmTofAnaTestbeam::Exec(Option_t * option)
{
   // Task execution

   LOG(DEBUG)<<" CbmTofAnaTestbeam => New event"<<FairLogger::endl;

   fStart.Set();
   FillHistos();
   fStop.Set();


   if( 0 == ( fEvents%100 ) && 0 < fEvents )
   {
      cout << "-I- CbmTofAnaTestbeam::Exec : "
           << "event " << fEvents << " processed." << endl;
   }
   fEvents += 1;
}

void CbmTofAnaTestbeam::Finish()
{
   // Normalisations
   cout << "CbmTofAnaTestbeam::Finish up with " << fEvents << " analyzed events " << endl;

   WriteHistos();
   // Prevent them from being sucked in by the CbmHadronAnalysis WriteHistograms method
   DeleteHistos();
}

/************************************************************************************/
// Functions common for all clusters approximations
Bool_t   CbmTofAnaTestbeam::RegisterInputs()
{
   FairRootManager *fManager = FairRootManager::Instance();

   fTofDigisColl   = (TClonesArray *) fManager->GetObject("CbmTofDigiExp");

   if( NULL == fTofDigisColl)
      fTofDigisColl = (TClonesArray *) fManager->GetObject("CbmTofDigi");

   if( NULL == fTofDigisColl)
   {
      LOG(ERROR)<<"CbmTofAnaTestbeam::RegisterInputs => Could not get the TofDigi TClonesArray!!!"<<FairLogger::endl;
      return kFALSE;
   } // if( NULL == fTofDigisColl)

   fTofHitsColl   = (TClonesArray *) fManager->GetObject("TofHit");
   if( NULL == fTofHitsColl)
   {
      LOG(ERROR)<<"CbmTofAnaTestbeam::RegisterInputs => Could not get the TofHit TClonesArray!!!"<<FairLogger::endl;
      return kFALSE;
   } // if( NULL == fTofHitsColl)

   fTofDigiMatchColl= (TClonesArray *) fManager->GetObject("TofDigiMatch");
   if( NULL == fTofDigiMatchColl)
   {
      LOG(ERROR)<<"CbmTofAnaTestbeam::RegisterInputs => Could not get the Match TClonesArray!!!"<<FairLogger::endl;
      return kFALSE;
   } // if( NULL == fTofDigiMatchColl)

   return kTRUE;
}
/************************************************************************************/
Bool_t   CbmTofAnaTestbeam::InitParameters()
{

   // Initialize the TOF GeoHandler
   Bool_t isSimulation=kFALSE;
   LOG(INFO)<<"CbmTofAnaTestbeam::InitParameters - Geometry, Mapping, ... "
             <<FairLogger::endl;
   // Get Base Container
   FairRun* ana = FairRun::Instance();
   FairRuntimeDb* rtdb=ana->GetRuntimeDb();   

   Int_t iGeoVersion = fGeoHandler->Init(isSimulation);
   if( k14a > iGeoVersion )
   {

   }

   fDigiPar = (CbmTofDigiPar*) (rtdb->getContainer("CbmTofDigiPar"));
   if( 0 == fDigiPar )
   {
      LOG(ERROR)<<"CbmTofAnaTestbeam::InitParameters => Could not obtain the CbmTofDigiPar "<<FairLogger::endl;
      return kFALSE; 
   }

   fDigiBdfPar = (CbmTofDigiBdfPar*) (rtdb->getContainer("CbmTofDigiBdfPar"));
   if( 0 == fDigiBdfPar )
   {
      LOG(ERROR)<<"CbmTofAnaTestbeam::InitParameters => Could not obtain the CbmTofDigiBdfPar "<<FairLogger::endl;
      return kFALSE; 
   }

   return kTRUE;
}
/************************************************************************************/
Bool_t   CbmTofAnaTestbeam::LoadGeometry()
{
   // Count the total number of channels and
   // generate an array with the global channel index of the first channe in each RPC

   return kTRUE;
}
/************************************************************************************/
// ------------------------------------------------------------------
Bool_t CbmTofAnaTestbeam::CreateHistos()
{
  // Create histogramms

   TDirectory * oldir = gDirectory; // <= To prevent histos from being sucked in by the param file of the TRootManager!
   gROOT->cd(); // <= To prevent histos from being sucked in by the param file of the TRootManager !

   // define histos here


   gDirectory->cd( oldir->GetPath() ); // <= To prevent histos from being sucked in by the param file of the TRootManager!

   return kTRUE;
}

// ------------------------------------------------------------------
Bool_t CbmTofAnaTestbeam::FillHistos()
{
   // Constants, TODO => put as parameter !!!

  Int_t kTOF=6;

   // Declare variables outside the loop
   CbmTofHit   *pTofHit;

   Int_t iNbTofDigis, iNbTofHits;


   iNbTofDigis   = fTofDigisColl->GetEntriesFast();
   iNbTofHits    = fTofHitsColl->GetEntriesFast();

   // Digis info
   Double_t dTotalDataSize = 0;
   if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
   {
      CbmTofDigiExp *pDigi;
      for( Int_t iDigInd = 0; iDigInd < iNbTofDigis; iDigInd++ )
      {
         pDigi = (CbmTofDigiExp*) fTofDigisColl->At( iDigInd );

         Int_t iSmType = pDigi->GetType();
         Int_t iSm     = pDigi->GetSm();
         Int_t iRpc    = pDigi->GetRpc();
         Int_t iCh     = pDigi->GetChannel();
         // First Get X/Y position info
	 /*
         CbmTofDetectorInfo xDetInfo(kTOF, iSmType, iSm, iRpc, 0, iCh + 1);
         Int_t iChId =  fTofId->SetDetectorInfo( xDetInfo );
         fChannelInfo = fDigiPar->GetCell( iChId );

         Double_t dX = fChannelInfo->GetX();
         Double_t dY = fChannelInfo->GetY();
         Double_t dZ = fChannelInfo->GetZ();
	 /*
         fhDigiMapXY->Fill(   dX, dY );
         fhDigiMapXZ->Fill(   dX, dZ );
         fhDigiMapYZ->Fill(   dY, dZ );
	 */
      } // for( Int_t iDigInd = 0; iDigInd < iNbTofDigis; iDigInd++ )
   } // if( kTRUE == fDigiBdfPar->UseExpandedDigi() )

   // Hits info
   Int_t iNbMixedHits = 0;
   for( Int_t iHitInd = 0; iHitInd < iNbTofHits; iHitInd++)
   {
      pTofHit = (CbmTofHit*) fTofHitsColl->At( iHitInd );

      // Need a method to reconvert position in a "central channel"
//      fhHitRateCh->Fill();

      Double_t dX = pTofHit->GetX();
      Double_t dY = pTofHit->GetY();
      Double_t dZ = pTofHit->GetZ();
      /*
      fhHitMapXY->Fill(   dX, dY );
      fhHitMapXZ->Fill(   dX, dZ );
      fhHitMapYZ->Fill(   dY, dZ );
      */
   } // for( Int_t iHitInd = 0; iHitInd < iNbTofHits; iHitInd++)
   return kTRUE;
}
// ------------------------------------------------------------------

Bool_t CbmTofAnaTestbeam::WriteHistos()
{
   // TODO: add sub-folders

   // Write histogramms to the file
   TDirectory * oldir = gDirectory;
   TFile *fHist = new TFile("./tofTests.hst.root","RECREATE");
   fHist->cd();

   // Mapping
   /*
   fhDigiMapXY->Write();
   fhDigiMapXZ->Write();
   fhDigiMapYZ->Write();
   fhHitMapXY->Write();
   fhHitMapXZ->Write();
   fhHitMapYZ->Write();
   */

   gDirectory->cd( oldir->GetPath() );

   fHist->Close();

   return kTRUE;
}
Bool_t   CbmTofAnaTestbeam::DeleteHistos()
{
   // Test class performance

   // Mapping
   /*
   delete fhDigiMapXY;
   delete fhDigiMapXZ;
   delete fhDigiMapYZ;

   delete fhHitMapXY;
   delete fhHitMapXZ;
   delete fhHitMapYZ;
   */

   return kTRUE;
}

ClassImp(CbmTofAnaTestbeam);
