// ------------------------------------------------------------------
// -----                     CbmTofTests                        -----
// -----              Created 29/08/2013 by pal                 -----
// ------------------------------------------------------------------

#include "CbmTofTests.h"

// TOF Classes and includes
#include "CbmTofPoint.h"      // in cbmdata/tof
#include "CbmTofDigi.h"       // in cbmdata/tof
#include "CbmTofDigiExp.h"    // in cbmdata/tof
#include "CbmTofHit.h"        // in cbmdata/tof
#include "CbmTofGeoHandler.h" // in tof/TofTools
#include "CbmTofDetectorId_v12b.h" // in cbmdata/tof
#include "CbmTofCell.h"       // in tof/TofData
#include "CbmTofDigiPar.h"    // in tof/TofParam
#include "CbmTofDigiBdfPar.h" // in tof/TofParam
#include "CbmTofAddress.h"    // in cbmdata/tof

// CBMroot classes and includes
#include "CbmMCEventHeader.h"
#include "CbmMCTrack.h"
/*
#include "CbmStsTrack.h"
#include "CbmStsHit.h"
#include "CbmStsPoint.h"
#include "CbmHadron.h"
#include "CbmGlobalTrack.h"
*/

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
// CbmTofTests
//
// Task for analysis of hadron spectra
//
// ------------------------------------------------------------------
CbmTofTests::CbmTofTests()
  : FairTask("HadronAnalysis"),
    fEvents(0),
    fMCEventHeader(NULL),
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
    fTofPointsColl(NULL),
    fMcTracksColl(NULL),
    fTofDigisColl(NULL),
    fTofHitsColl(NULL),
    fhTestingTime(NULL),
    fhPointMapXY(NULL),
    fhPointMapXZ(NULL),
    fhPointMapYZ(NULL),
    fhPointMapAng(NULL),
    fhPointMapSph(NULL),
    fhDigiMapXY(NULL),
    fhDigiMapXZ(NULL),
    fhDigiMapYZ(NULL),
    fhDigiMapAng(NULL),
    fhDigiMapSph(NULL),
    fhHitMapXY(NULL),
    fhHitMapXZ(NULL),
    fhHitMapYZ(NULL),
    fhHitMapAng(NULL),
    fhHitMapSph(NULL),
    fhFluxMap(NULL),
    fhDigiFluxMap(NULL),
    fhHitFluxMap(NULL),
    fhDigiRateCh(NULL),
    fhDataRateCh(NULL),
    fhDataRateRpc(NULL),
    fhDataRateSm(NULL),
    fhDataRateType(NULL),
    fhTofDataPerEvt(NULL),
    fhTofDataRedEvt(NULL),
    fhOptLnkRpc(NULL),
    fhOptLnkSm(NULL),
    fhOptLnkType(NULL),
    fhTofRes(NULL),
    fhTofResSing(NULL),
    fhTofPosDifX(NULL),
    fhTofPosDifY(NULL),
    fhTofPosDifZ(NULL),
    fhTofPosDifSingXX(NULL),
    fhTofPosDifSingXY(NULL),
    fhTofPosDifSingXZ(NULL),
    fhTofPosDifSingY(NULL),
    fhTofPosDifSingZ(NULL),
    fhTofPosDifXZSing(NULL),
    fhTofBadXPosSing(NULL),
    fhTofBadYPosSing(NULL),
    fhTofBadZPosSing(NULL),
    fhTofEff(NULL),
    fhTofMixing(NULL),
    fStart(),
    fStop()
{
  cout << "CbmTofTests: Task started " << endl;
}
// ------------------------------------------------------------------

// ------------------------------------------------------------------
CbmTofTests::CbmTofTests(const char* name, Int_t verbose)
  : FairTask(name, verbose),
    fEvents(0),
    fMCEventHeader(NULL),
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
    fTofPointsColl(NULL),
    fMcTracksColl(NULL),
    fTofDigisColl(NULL),
    fTofHitsColl(NULL),
    fhTestingTime(NULL),
    fhPointMapXY(NULL),
    fhPointMapXZ(NULL),
    fhPointMapYZ(NULL),
    fhPointMapAng(NULL),
    fhPointMapSph(NULL),
    fhDigiMapXY(NULL),
    fhDigiMapXZ(NULL),
    fhDigiMapYZ(NULL),
    fhDigiMapAng(NULL),
    fhDigiMapSph(NULL),
    fhHitMapXY(NULL),
    fhHitMapXZ(NULL),
    fhHitMapYZ(NULL),
    fhHitMapAng(NULL),
    fhHitMapSph(NULL),
    fhFluxMap(NULL),
    fhDigiFluxMap(NULL),
    fhHitFluxMap(NULL),
    fhDigiRateCh(NULL),
    fhDataRateCh(NULL),
    fhDataRateRpc(NULL),
    fhDataRateSm(NULL),
    fhDataRateType(NULL),
    fhTofDataPerEvt(NULL),
    fhTofDataRedEvt(NULL),
    fhOptLnkRpc(NULL),
    fhOptLnkSm(NULL),
    fhOptLnkType(NULL),
    fhTofRes(NULL),
    fhTofResSing(NULL),
    fhTofPosDifX(NULL),
    fhTofPosDifY(NULL),
    fhTofPosDifZ(NULL),
    fhTofPosDifSingXX(NULL),
    fhTofPosDifSingXY(NULL),
    fhTofPosDifSingXZ(NULL),
    fhTofPosDifSingY(NULL),
    fhTofPosDifSingZ(NULL),
    fhTofPosDifXZSing(NULL),
    fhTofBadXPosSing(NULL),
    fhTofBadYPosSing(NULL),
    fhTofBadZPosSing(NULL),
    fhTofEff(NULL),
    fhTofMixing(NULL),
    fStart(),
    fStop()
{
}
// ------------------------------------------------------------------

// ------------------------------------------------------------------
CbmTofTests::~CbmTofTests()
{
    // Destructor
}
// ------------------------------------------------------------------
/************************************************************************************/
// FairTasks inherited functions
InitStatus CbmTofTests::Init()
{
   if( kFALSE == RegisterInputs() )
      return kFATAL;

   fTofId = new CbmTofDetectorId_v12b();

   if( kFALSE == LoadGeometry() )
      return kFATAL;

   if( kFALSE == CreateHistos() )
      return kFATAL;

   return kSUCCESS;
}

void CbmTofTests::SetParContainers()
{
   LOG(INFO)<<" CbmTofTests => Get the digi parameters for tof"<<FairLogger::endl;

   // Get Base Container
   FairRunAna* ana = FairRunAna::Instance();
   FairRuntimeDb* rtdb=ana->GetRuntimeDb();

   fDigiPar = (CbmTofDigiPar*) (rtdb->getContainer("CbmTofDigiPar"));

   fDigiBdfPar = (CbmTofDigiBdfPar*) (rtdb->getContainer("CbmTofDigiBdfPar"));
}

void CbmTofTests::Exec(Option_t * option)
{
   // Task execution

   LOG(DEBUG)<<" CbmTofTests => New event"<<FairLogger::endl;

   fStart.Set();
   FillHistos();
   fStop.Set();
   fhTestingTime->Fill( fStop.GetSec() - fStart.GetSec()
                       + (fStop.GetNanoSec() - fStart.GetNanoSec())/1e9 );

   if( 0 == ( fEvents%100 ) && 0 < fEvents )
   {
      cout << "-I- CbmTofTests::Exec : "
           << "event " << fEvents << " processed." << endl;
   }
   fEvents += 1;
}

void CbmTofTests::Finish()
{
   // Normalisations
   cout << "CbmTofTests::Finish up with " << fEvents << " analyzed events " << endl;

   Double_t dEvtRate     = 1./fEvents; // [1/events]
   Double_t dIntRate     = 1.E7;       // [interactions/s]
   Double_t dOptLinkCapa = 2.5;        // Nominal bandwidth [GByte/s]
   Double_t dOptLinkAvBw = 0.8;        // Bandwidth available for data transport [Nominal bandwidth ratio]
   Double_t dKiloByte    = 1024;
   Double_t dMegaByte    = 1024*dKiloByte;
   Double_t dGigaByte    = 1024*dMegaByte;

   cout << "<I> Normalisation factors " << dIntRate*dEvtRate << " [1/s], " << dOptLinkCapa*dOptLinkAvBw <<" [GByte/s]"<< endl;

   fhFluxMap->Scale(dIntRate*dEvtRate);
   fhDigiFluxMap->Scale(dIntRate*dEvtRate);
   fhHitFluxMap->Scale(dIntRate*dEvtRate);
   fhDigiRateCh->Scale(dIntRate*dEvtRate);
   fhDataRateCh->Scale(dIntRate*dEvtRate/dMegaByte);
   fhDataRateRpc->Scale(dIntRate*dEvtRate/dGigaByte);
   fhDataRateSm->Scale(dIntRate*dEvtRate/dGigaByte);
   fhDataRateType->Scale(dIntRate*dEvtRate/dGigaByte);
   fhTofDataPerEvt->Scale(1/dKiloByte);
//   fhHitRateCh->Scale(dIntRate*dEvtRate);

   fhOptLnkRpc->Scale(dIntRate*dEvtRate/dGigaByte/(dOptLinkCapa*dOptLinkAvBw));
   fhOptLnkSm->Scale(dIntRate*dEvtRate/dGigaByte/(dOptLinkCapa*dOptLinkAvBw));
   fhOptLnkType->Scale(dIntRate*dEvtRate/dGigaByte/(dOptLinkCapa*dOptLinkAvBw));

   WriteHistos();
   // Prevent them from being sucked in by the CbmHadronAnalysis WriteHistograms method
   DeleteHistos();
}

/************************************************************************************/
// Functions common for all clusters approximations
Bool_t   CbmTofTests::RegisterInputs()
{
   FairRootManager *fManager = FairRootManager::Instance();
/*
   fMCEventHeader = (CbmMCEventHeader*) fManager->GetObject("MCEventHeader");
   if( NULL == fMCEventHeader)
   {
      LOG(ERROR)<<"CbmTofTests::RegisterInputs => Could not get the MCEventHeader TClonesArray!!!"<<FairLogger::endl;
      return kFALSE;
   } // if( NULL == fMCEventHeader)
*/
   fTofPointsColl  = (TClonesArray *) fManager->GetObject("TofPoint");
   if( NULL == fTofPointsColl)
   {
      LOG(ERROR)<<"CbmTofTests::RegisterInputs => Could not get the TofPoint TClonesArray!!!"<<FairLogger::endl;
      return kFALSE;
   } // if( NULL == fTofPointsColl)

   fMcTracksColl   = (TClonesArray *) fManager->GetObject("MCTrack");
   if( NULL == fMcTracksColl)
   {
      LOG(ERROR)<<"CbmTofTests::RegisterInputs => Could not get the MCTrack TClonesArray!!!"<<FairLogger::endl;
      return kFALSE;
   } // if( NULL == fMcTracksColl)

   fTofDigisColl   = (TClonesArray *) fManager->GetObject("TofDigi");
   if( NULL == fTofDigisColl)
   {
      LOG(ERROR)<<"CbmTofTests::RegisterInputs => Could not get the TofDigi TClonesArray!!!"<<FairLogger::endl;
      return kFALSE;
   } // if( NULL == fTofDigisColl)

   fTofHitsColl   = (TClonesArray *) fManager->GetObject("TofHit");
   if( NULL == fTofHitsColl)
   {
      LOG(ERROR)<<"CbmTofTests::RegisterInputs => Could not get the TofHit TClonesArray!!!"<<FairLogger::endl;
      return kFALSE;
   } // if( NULL == fTofHitsColl)

/*
   fGlobalTracks = (TClonesArray*) fManager->GetObject("GlobalTrack");
   if( NULL == fGlobalTracks)
   {
      LOG(ERROR)<<"CbmTofTests::RegisterInputs => Could not get the GlobalTrack TClonesArray!!!"<<FairLogger::endl;
      return kFALSE;
   } // if( NULL == fGlobalTracks)

   fHadrons = (TClonesArray*) fManager->GetObject("Hadron");
   if( NULL == fHadrons)
   {
      LOG(ERROR)<<"CbmTofTests::RegisterInputs => Could not get the Hadron TClonesArray!!!"<<FairLogger::endl;
      return kFALSE;
   } // if( NULL == fHadrons)

   fStsTracks = (TClonesArray*) fManager->GetObject("StsTrack");
   if( NULL == fStsTracks)
   {
      LOG(ERROR)<<"CbmTofTests::RegisterInputs => Could not get the StsTrack TClonesArray!!!"<<FairLogger::endl;
      return kFALSE;
   } // if( NULL == fStsTracks)
   fStsHits = (TClonesArray*) fManager->GetObject("StsHit");
   if( NULL == fStsHits)
   {
      LOG(ERROR)<<"CbmTofTests::RegisterInputs => Could not get the StsHit TClonesArray!!!"<<FairLogger::endl;
      return kFALSE;
   } // if( NULL == fStsHits)
   fStsPoints = (TClonesArray*) fManager->GetObject("StsPoint");
   if( NULL == fStsPoints)
   {
      LOG(ERROR)<<"CbmTofTests::RegisterInputs => Could not get the StsPoint TClonesArray!!!"<<FairLogger::endl;
      return kFALSE;
   } // if( NULL == fStsPoints)

   // PAL modif
   fRichHits = (TClonesArray*) fManager->GetObject("RichHit");
   if( NULL == fRichHits)
   {
      LOG(ERROR)<<"CbmTofTests::RegisterInputs => Could not get the RichHit TClonesArray!!!"<<FairLogger::endl;
      fbRichThere = kFALSE;
   } // if( NULL == fRichHits)
      else fbRichThere = kTRUE;

   fTrdHits = (TClonesArray*) fManager->GetObject("TrdHit");
   if( NULL == fTrdHits)
   {
      LOG(ERROR)<<"CbmTofTests::RegisterInputs => Could not get the TrdHit TClonesArray!!!"<<FairLogger::endl;
      fbTrdThere = kFALSE;
   } // if( NULL == fTrdHits)
      else fbTrdThere = kTRUE;
*/

   return kTRUE;
}
/************************************************************************************/
Bool_t   CbmTofTests::LoadGeometry()
{
   /*
     Type 0: 5 RPC/SM,  24 SM, 32 ch/RPC =>  3840 ch          , 120 RPC         ,
     Type 1: 5 RPC/SM, 142 SM, 32 ch/RPC => 22720 ch => 26560 , 710 RPC =>  830 , => 166
     Type 3: 3 RPC/SM,  50 SM, 56 ch/RPC =>  8400 ch => 34960 , 150 RPC =>  980 , => 216
     Type 4: 5 RPC/SM,   8 SM, 96 ch/RPC =>  3840 ch => 38800 ,  40 RPC => 1020 , => 224
     Type 5: 5 RPC/SM,   8 SM, 96 ch/RPC =>  3840 ch => 42640 ,  40 RPC => 1060 , => 232
     Type 6: 2 RPC/SM,  10 SM, 96 ch/RPC =>  1920 ch => 44560 ,  20 RPC => 1080 , => 242
   */

   // Count the total number of channels and
   // generate an array with the global channel index of the first channe in each RPC
   Int_t iNbSmTypes = fDigiBdfPar->GetNbSmTypes();
   fvTypeSmOffs.resize( iNbSmTypes );
   fvSmRpcOffs.resize( iNbSmTypes );
   fvRpcChOffs.resize( iNbSmTypes );
   iNbSmTot  = 0;
   iNbRpcTot = 0;
   iNbChTot  = 0;
   for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
   {
      Int_t iNbSm  = fDigiBdfPar->GetNbSm(  iSmType);
      Int_t iNbRpc = fDigiBdfPar->GetNbRpc( iSmType);

      fvTypeSmOffs[iSmType] = iNbSmTot;
      iNbSmTot += iNbSm;

      fvSmRpcOffs[iSmType].resize( iNbSm );
      fvRpcChOffs[iSmType].resize( iNbSm );

      for( Int_t iSm = 0; iSm < iNbSm; iSm++ )
      {
         fvSmRpcOffs[iSmType][iSm] = iNbRpcTot;
         iNbRpcTot += iNbRpc;

         fvRpcChOffs[iSmType][iSm].resize( iNbRpc );
         for( Int_t iRpc = 0; iRpc < iNbRpc; iRpc++ )
         {
            fvRpcChOffs[iSmType][iSm][iRpc] = iNbChTot;
            iNbChTot += fDigiBdfPar->GetNbChan( iSmType, iRpc );
         } // for( Int_t iRpc = 0; iRpc < iNbRpc; iRpc++ )
      } // for( Int_t iSm = 0; iSm < iNbSm; iSm++ )
   } // for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )

   return kTRUE;
}
/************************************************************************************/
// ------------------------------------------------------------------
Bool_t CbmTofTests::CreateHistos()
{
  // Create histogramms

   TDirectory * oldir = gDirectory; // <= To prevent histos from being sucked in by the param file of the TRootManager!
   gROOT->cd(); // <= To prevent histos from being sucked in by the param file of the TRootManager !

   // Test class performance
   fhTestingTime = new TH1I( "TofDigiBdf_TestingTime", "Time needed to for the test processing in each event; Time [s]",
                                  4000, 0.0, 4.0 );

   Double_t ymin=-1.;
   Double_t ymax=4.;
   Double_t ptmmax=2.5;
   Int_t    ptm_nbx=30;
   Int_t    ptm_nby=30;

   Double_t v1_nbx=20.;
   Double_t v1_nby=20.;
   Double_t yvmax=1.3;

   // xy - hit densities and rates
   Int_t nbinx=1500;
   Int_t nbiny=1000;
   Int_t nbinz=1500;
   Double_t xrange=750.;
   Double_t yrange=500.;
   Double_t zmin  =950.;
   Double_t zmax  =1100.;

   // angular densities for overlap check
   Int_t iNbBinThetaX  = 1200;
   Double_t dThetaXMin = -  60.0;
   Double_t dThetaXMax =    60.0;
   Int_t iNbBinThetaY  = 800;
   Double_t dThetaYMin = -  40.0;
   Double_t dThetaYMax =    40.0;
   /*
   Int_t iNbBinPhi     = 100;
   Double_t dPhiMin    =   0;
   Double_t dPhiMax    =  10;
   Int_t iNbBinTheta   = 360;
   Double_t dThetaMin  = - 180.0;
   Double_t dThetaMax  =   180.0;
   */
   Int_t iNbBinPhi     = 180;
   Double_t dPhiMin    =   0;
   Double_t dPhiMax    =  TMath::Pi()*90/180;
   Int_t iNbBinTheta   = 180;
   Double_t dThetaMin  = - TMath::Pi();
   Double_t dThetaMax  =   TMath::Pi();

   // Mapping
      // points
   fhPointMapXY     = new TH2D("TofTests_PointsMapXY",  "Position of the Tof Points; X[cm]; Y[cm]; # [Points]",
                              nbinx,-xrange,xrange,nbiny,-yrange,yrange);
   fhPointMapXZ     = new TH2D("TofTests_PointsMapXZ",  "Position of the Tof Points; X[cm]; Z[cm]; # [Points]",
                              nbinx,-xrange,xrange,nbinz,zmin,zmax);
   fhPointMapYZ     = new TH2D("TofTests_PointsMapYZ",  "Position of the Tof Points; Y[cm]; Z[cm]; # [Points]",
                              nbiny,-yrange,yrange,nbinz,zmin,zmax);
   fhPointMapAng    = new TH2D("TofTests_PointsMapAng",  "Position of the Tof Points; #theta_{x}[Deg.]; #theta_{y}[Deg.]; # [Points]",
                              iNbBinThetaX, dThetaXMin, dThetaXMax,
                              iNbBinThetaY, dThetaYMin, dThetaYMax);
   fhPointMapSph    = new TH2D("TofTests_PointsMapSph",  "Position of the Tof Points; #theta[rad.]; #phi[rad.]; # [Points]",
                              iNbBinTheta, dThetaMin, dThetaMax,
                              iNbBinPhi,   dPhiMin,   dPhiMax);
      // Digis
   fhDigiMapXY     = new TH2D("TofTests_DigisMapXY",  "Position of the Tof Digis; X[cm]; Y[cm]; # [Digi]",
                              nbinx,-xrange,xrange,nbiny,-yrange,yrange);
   fhDigiMapXZ     = new TH2D("TofTests_DigisMapXZ",  "Position of the Tof Digis; X[cm]; Z[cm]; # [Digi]",
                              nbinx,-xrange,xrange,nbinz,zmin,zmax);
   fhDigiMapYZ     = new TH2D("TofTests_DigisMapYZ",  "Position of the Tof Digis; Y[cm]; Z[cm]; # [Digi]",
                              nbiny,-yrange,yrange,nbinz,zmin,zmax);
   fhDigiMapAng    = new TH2D("TofTests_DigisMapAng",  "Position of the Tof Digis; #theta_{x}[Deg.]; #theta_{y}[Deg.]; # [Digi]",
                              iNbBinThetaX, dThetaXMin, dThetaXMax,
                              iNbBinThetaY, dThetaYMin, dThetaYMax);
   fhDigiMapSph    = new TH2D("TofTests_DigisMapSph",  "Position of the Tof Digis; #theta[rad.]; #phi[rad.]; # [Points]",
                              iNbBinTheta, dThetaMin, dThetaMax,
                              iNbBinPhi,   dPhiMin,   dPhiMax);

      // Hits
   fhHitMapXY      = new TH2D("TofTests_HitsMapXY",   "Position of the Tof Hits; X[cm]; Y[cm]; # [Hits]",
                              nbinx,-xrange,xrange,nbiny,-yrange,yrange);
   fhHitMapXZ      = new TH2D("TofTests_HitsMapXZ",   "Position of the Tof Hits; X[cm]; Z[cm]; # [Hits]",
                              nbinx,-xrange,xrange,nbinz,zmin,zmax);
   fhHitMapYZ      = new TH2D("TofTests_HitsMapYZ",   "Position of the Tof Hits; Y[cm]; Z[cm]; # [Hits]",
                              nbiny,-yrange,yrange,nbinz,zmin,zmax);
   fhHitMapAng     = new TH2D("TofTests_HitsMapAng",  "Position of the Tof Hits; #theta_{x}[Deg.]; #theta_{y}[Deg.]; # [Hits]",
                              iNbBinThetaX, dThetaXMin, dThetaXMax,
                              iNbBinThetaY, dThetaYMin, dThetaYMax);
   fhHitMapSph     = new TH2D("TofTests_HitsMapSph",  "Position of the Tof Hits; #theta[rad.]; #phi[rad.]; # [Points]",
                              iNbBinTheta, dThetaMin, dThetaMax,
                              iNbBinPhi,   dPhiMin,   dPhiMax);

   // Rates and data rates
   Int_t iNbSmTypes = fDigiBdfPar->GetNbSmTypes();
   fhFluxMap       = new TH2D("TofTests_FluxMap",  "Tof Point rate as function of position; X[cm]; Y[cm]; Flux [1/(s*cm^2)]",
                              nbinx,-xrange,xrange,nbiny,-yrange,yrange);
   fhDigiFluxMap   = new TH2D("TofTests_DigisFluxMap",  "Tof Hit rate as function of position; X[cm]; Y[cm]; Flux [Digi/(s*cm^2)]",
                              nbinx,-xrange,xrange,nbiny,-yrange,yrange);
   fhHitFluxMap    = new TH2D("TofTests_HitsFluxMap", "Tof Hit rate as function of position; X[cm]; Y[cm]; Flux [Hits/(s*cm^2)]",
                              nbinx,-xrange,xrange,nbiny,-yrange,yrange);
   fhDigiRateCh    = new TH1D("TofTests_DigiRateCh","Digi rate per channel; Channel []; Rate [1/s]",
                               2*iNbChTot, 0, 2*iNbChTot);
   fhDataRateCh    = new TH1D("TofTests_DataRateCh","Data rate per channel; Channel []; Data Rate [kB/s]",
                               2*iNbChTot, 0, 2*iNbChTot);
   fhDataRateRpc   = new TH1D("TofTests_DataRateRpc","Data rate per RPC; RPC []; Rate [GB/s]",
                               iNbRpcTot, 0, iNbRpcTot);
   fhDataRateSm    = new TH1D("TofTests_DataRateSm","Data rate per SM; SM []; Rate [GB/s]",
                               iNbSmTot, 0, iNbSmTot);
   fhDataRateType  = new TH1D("TofTests_DataRateType","Data rate per SM; SM Type[]; Rate [GB/s]",
                               iNbSmTypes, 0, iNbSmTypes);
   fhTofDataPerEvt = new TH1D("TofTests_DataPerEvt","Data per event for the full wall; Event Size [kB]",
                              1000, 0, 1000.0);
   fhTofDataRedEvt = new TH1D("TofTests_DataRedEvt","Possible data reduction per event for the full wall; Event Size Reduction [kB]",
                              iNbChTot, 0, iNbChTot);
//   fhHitRateCh     = new TH1D("TofTests_HitRateCh", "Tof Hit rate per channel; Channel []; Rate [1/s]",
//                              iNbChTot, 0, iNbChTot);
   fhOptLnkRpc     = new TH1D("TofTests_OptLnkRpc","Number of optical links needed per RPC; RPC []",
                               iNbRpcTot, 0, iNbRpcTot);
   fhOptLnkSm      = new TH1D("TofTests_OptLnkSm","Number of optical links needed per SM; SM []; Optical Links []",
                               iNbSmTot, 0, iNbSmTot);
   fhOptLnkType    = new TH1D("TofTests_OptLnkType","Number of optical links needed per SM type; SM Type[]; Optical Links []",
                               iNbSmTypes, 0, iNbSmTypes);

   // Performances check
   fhTofRes = new TH1I( "TofTests_TimeRes", "Time difference between TofHits and TofPoint; tMcPoint -tTofHit [ns]",
                        5000, -5.0, 5.0 );
   fhTofResSing = new TH1I( "TofTests_TimeResSing", "Time difference between TofHits and TofPoint, only 1 MC Track/Point per Hit; tMcPoint -tTofHit [ns]",
                        5000, -5.0, 5.0 );

   fhTofPosDifX= new TH2D("TofTests_PosDifX",   "Position Accuracy of the Tof Hits in X; X[cm]; PtX - HitX[cm]; # [Hits]",
         nbinx,-xrange,xrange, 200,-50,50);
   fhTofPosDifY= new TH2D("TofTests_PosDifY",   "Position Accuracy of the Tof Hits in Y; Y[cm]; PtY - HitY[cm]; # [Hits]",
         nbiny,-yrange,yrange, 200,-50,50);
   fhTofPosDifZ= new TH2D("TofTests_PosDifZ",   "Position Accuracy of the Tof Hits in Z; Z[cm]; PtZ - HitZ[cm]; # [Hits]",
         600, 950, 1100, 200,-10,10);
   fhTofPosDifSingXX= new TH2D("TofTests_PosDifSingXX",
         "Position Accuracy of the Tof Hits in X, hits from single MC point; Pt X[cm]; PtX - HitX[cm]; # [Hits]",
         nbinx,-xrange,xrange, 500,-50,50);
   fhTofPosDifSingXY= new TH2D("TofTests_PosDifSingXY",
         "Position Accuracy of the Tof Hits in X, hits from single MC point; Pt Y[cm]; PtX - HitX[cm]; # [Hits]",
         nbiny,-yrange,yrange, 500,-50,50);
   fhTofPosDifSingXZ= new TH2D("TofTests_PosDifSingXZ",
         "Position Accuracy of the Tof Hits in X, hits from single MC point; Pt Z[cm]; PtX - HitX[cm]; # [Hits]",
         nbinz,zmin,zmax, 500,-50,50);
   fhTofPosDifSingY= new TH2D("TofTests_PosDifSingY",
         "Position Accuracy of the Tof Hits in Y, hits from single MC point; Pt Y[cm]; PtY - HitY[cm]; # [Hits]",
         nbiny,-yrange,yrange, 500,-50,50);
   fhTofPosDifSingZ= new TH2D("TofTests_PosDifSingZ",
         "Position Accuracy of the Tof Hits in Z, hits from single MC point; Pt Z[cm]; PtZ - HitZ[cm]; # [Hits]",
         600, 950, 1100, 200,-10,10);
   fhTofPosDifXZSing= new TH2D("TofTests_PosDifXZSing",
         "Position Accuracy of the Tof Hits in Z, hits from single MC point; PtX - HitX[cm]; PtZ - HitZ[cm]; # [Hits]",
         200,-50,50, 200,-10,10);

   fhTofBadXPosSing = new TH2D("TofTests_BadXPosSing",   "Position of the Tof Hits; X[cm]; Y[cm]; # [Hits]",
                              nbinx,-xrange,xrange,nbiny,-yrange,yrange);
   fhTofBadYPosSing = new TH2D("TofTests_BadYPosSing",   "Position of the Tof Hits; X[cm]; Y[cm]; # [Hits]",
                              nbinx,-xrange,xrange,nbiny,-yrange,yrange);
   fhTofBadZPosSing = new TH2D("TofTests_BadZPosSing",   "Position of the Tof Hits; X[cm]; Y[cm]; # [Hits]",
                              nbinx,-xrange,xrange,nbiny,-yrange,yrange);

   fhTofEff = new TH1I( "TofTests_TofEff", "Fraction of tracks reaching Tof producing a non mixed TofHit; # non-mixed Hits/ # Tof Tracks []",
                        1000, 0.0, 1.0 );
   fhTofMixing = new TH1I( "TofTests_TofMix", "Fraction of TofHits built from more than 1 McTrack; # mixed Hits/ # Hits []",
                        1000, 0.0, 0.1 );

   gDirectory->cd( oldir->GetPath() ); // <= To prevent histos from being sucked in by the param file of the TRootManager!

   return kTRUE;
}

// ------------------------------------------------------------------
Bool_t CbmTofTests::FillHistos()
{
   // Constants, TODO => put as parameter !!!
   Int_t iDataSizeHit = 48; // [bits]

   // Declare variables outside the loop
   CbmMCTrack  *pMcTrk;
   CbmTofPoint *pTofPoint;
   CbmTofHit   *pTofHit;

   Int_t iNbTracks, iNbTofPts, iNbTofDigis, iNbTofHits;

   iNbTracks     = fMcTracksColl->GetEntriesFast();
   iNbTofPts     = fTofPointsColl->GetEntriesFast();
   iNbTofDigis   = fTofDigisColl->GetEntriesFast();
   iNbTofHits    = fTofHitsColl->GetEntriesFast();

   // Tracks Info
   Int_t iNbTofTracks     = 0;
   Int_t iNbTofTracksPrim = 0;
   for(Int_t iTrkInd = 0; iTrkInd < iNbTracks; iTrkInd++)
   {
      pMcTrk = (CbmMCTrack*) fMcTracksColl->At( iTrkInd );

      if( 0 < pMcTrk->GetNPoints(kTOF) )
      {
         iNbTofTracks++;
      }
      if( 0 < pMcTrk->GetNPoints(kTOF) && -1 == pMcTrk->GetMotherId() )
         iNbTofTracksPrim++;

   } // for(Int_t iTrkInd = 0; iTrkInd < nMcTracks; iTrkInd++)

   // Points info
   for (Int_t iPntInd = 0; iPntInd < iNbTofPts; iPntInd++ )
   {
      // Get a pointer to the TOF point
      pTofPoint = (CbmTofPoint*) fTofPointsColl->At( iPntInd );

      // Obtain position
      TVector3 vPntPos;
      pTofPoint->Position( vPntPos );

      Double_t dX = vPntPos.X();
      Double_t dY = vPntPos.Y();
      Double_t dZ = vPntPos.Z();

      fhPointMapXY->Fill( dX, dY );
      fhPointMapXZ->Fill( dX, dZ );
      fhPointMapYZ->Fill( dY, dZ );
      fhFluxMap->Fill(    dX, dY );

      Double_t dThetaX = TMath::ATan2( dX, dZ )*180.0/TMath::Pi();
      Double_t dThetaY = TMath::ATan2( dY, dZ )*180.0/TMath::Pi();
      fhPointMapAng->Fill( dThetaX, dThetaY );

      Double_t dPhi    = TMath::ATan2( TMath::Sqrt( dX*dX + dY*dY ), dZ );//*180.0/TMath::Pi();
      Double_t dTheta  = TMath::ATan2( dY, dX );//*180.0/TMath::Pi();
      fhPointMapSph->Fill( dTheta, dPhi );
   } // for (Int_t iPntInd = 0; iPntInd < nTofPoint; iPntInd++ )

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
         CbmTofDetectorInfo xDetInfo(kTOF, iSmType, iSm, iRpc, 0, iCh + 1);
         Int_t iChId =  fTofId->SetDetectorInfo( xDetInfo );
         fChannelInfo = fDigiPar->GetCell( iChId );

         Double_t dX = fChannelInfo->GetX();
         Double_t dY = fChannelInfo->GetY();
         Double_t dZ = fChannelInfo->GetZ();

         fhDigiMapXY->Fill(   dX, dY );
         fhDigiMapXZ->Fill(   dX, dZ );
         fhDigiMapYZ->Fill(   dY, dZ );
         fhDigiFluxMap->Fill( dX, dY  );

         Double_t dThetaX = TMath::ATan2( dX, dZ )*180.0/TMath::Pi();
         Double_t dThetaY = TMath::ATan2( dY, dZ )*180.0/TMath::Pi();
         fhDigiMapAng->Fill( dThetaX, dThetaY );

         Double_t dPhi    = TMath::ATan2( TMath::Sqrt( dX*dX + dY*dY ), dZ );//*180.0/TMath::Pi();
         Double_t dTheta  = TMath::ATan2( dY, dX );//*180.0/TMath::Pi();
         fhDigiMapSph->Fill( dTheta, dPhi );

         Int_t iGlobalChan = iCh  + fvRpcChOffs[iSmType][iSm][iRpc];
         Int_t iGlobalRpc  = iRpc + fvSmRpcOffs[iSmType][iSm];
         Int_t iGlobalSm   = iSm  + fvTypeSmOffs[iSmType];
         fhDigiRateCh->Fill(  iGlobalChan + iNbChTot*pDigi->GetSide() );
         fhDataRateCh->Fill(  iGlobalChan + iNbChTot*pDigi->GetSide() , iDataSizeHit );
         fhDataRateRpc->Fill( iGlobalRpc,  iDataSizeHit );
         fhDataRateSm->Fill(  iGlobalSm,   iDataSizeHit );
         fhDataRateType->Fill(  iSmType,   iDataSizeHit );
         fhOptLnkRpc->Fill( iGlobalRpc,  iDataSizeHit );
         fhOptLnkSm->Fill(  iGlobalSm,   iDataSizeHit );
         fhOptLnkType->Fill(  iSmType,   iDataSizeHit );
         dTotalDataSize += iDataSizeHit;
      } // for( Int_t iDigInd = 0; iDigInd < iNbTofDigis; iDigInd++ )
   } // if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
      else
      {
         CbmTofDigi *pDigi;
         for( Int_t iDigInd = 0; iDigInd < iNbTofDigis; iDigInd++ )
         {
            pDigi = (CbmTofDigi*) fTofDigisColl->At( iDigInd );

            Int_t iSmType = pDigi->GetType();
            Int_t iSm     = pDigi->GetSm();
            Int_t iRpc    = pDigi->GetRpc();
            Int_t iCh     = pDigi->GetChannel();
            // First Get X/Y position info
            CbmTofDetectorInfo xDetInfo(kTOF, iSmType, iSm, iRpc, 0, iCh + 1);
            Int_t iChId = fTofId->SetDetectorInfo( xDetInfo );
            fChannelInfo = fDigiPar->GetCell( iChId );

            Double_t dX = fChannelInfo->GetX();
            Double_t dY = fChannelInfo->GetY();
            Double_t dZ = fChannelInfo->GetZ();

            fhDigiMapXY->Fill(   dX, dY );
            fhDigiMapXZ->Fill(   dX, dZ );
            fhDigiMapYZ->Fill(   dY, dZ );
            fhDigiFluxMap->Fill( dX, dY  );

            Double_t dThetaX = TMath::ATan2( dX, dZ )*180.0/TMath::Pi();
            Double_t dThetaY = TMath::ATan2( dY, dZ )*180.0/TMath::Pi();
            fhDigiMapAng->Fill( dThetaX, dThetaY );

            Double_t dPhi    = TMath::ATan2( TMath::Sqrt( dX*dX + dY*dY ), dZ );//*180.0/TMath::Pi();
            Double_t dTheta  = TMath::ATan2( dY, dX );//*180.0/TMath::Pi();
            fhDigiMapSph->Fill( dTheta, dPhi );

            Int_t iGlobalChan = iCh  + fvRpcChOffs[iSmType][iSm][iRpc];
            Int_t iGlobalRpc  = iRpc + fvSmRpcOffs[iSmType][iSm];
            Int_t iGlobalSm   = iSm  + fvTypeSmOffs[iSmType];
            fhDigiRateCh->Fill(  iGlobalChan + iNbChTot*pDigi->GetSide() );
            fhDataRateCh->Fill(  iGlobalChan + iNbChTot*pDigi->GetSide() , iDataSizeHit );
            fhDataRateRpc->Fill( iGlobalRpc,  iDataSizeHit );
            fhDataRateSm->Fill(  iGlobalSm,   iDataSizeHit );
            fhDataRateType->Fill(  iSmType,   iDataSizeHit );
            fhOptLnkRpc->Fill( iGlobalRpc,  iDataSizeHit );
            fhOptLnkSm->Fill(  iGlobalSm,   iDataSizeHit );
            fhOptLnkType->Fill(  iSmType,   iDataSizeHit );
            dTotalDataSize += iDataSizeHit;
         } // for( Int_t iDigInd = 0; iDigInd < iNbTofDigis; iDigInd++ )
      } // else of if( kTRUE == fDigiBdfPar->UseExpandedDigi() )

   fhTofDataPerEvt->Fill(dTotalDataSize);

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

      fhHitMapXY->Fill(   dX, dY );
      fhHitMapXZ->Fill(   dX, dZ );
      fhHitMapYZ->Fill(   dY, dZ );
      fhHitFluxMap->Fill( dX, dY );

      Double_t dThetaX = TMath::ATan2( dX, dZ )*180.0/TMath::Pi();
      Double_t dThetaY = TMath::ATan2( dY, dZ )*180.0/TMath::Pi();
      fhHitMapAng->Fill( dThetaX, dThetaY );

      Double_t dPhi    = TMath::ATan2( TMath::Sqrt( dX*dX + dY*dY ), dZ );//*180.0/TMath::Pi();
      Double_t dTheta  = TMath::ATan2( dY, dX );//*180.0/TMath::Pi();
      fhHitMapSph->Fill( dPhi, dTheta );

      // Using the SetLinks/GetLinks of the TofHit class seems to conflict
      // with something in littrack QA
//      CbmTofPoint* pPt = (CbmTofPoint*)(pTofHit->GetLinks());
      // Use instead the index
      // outdated TBD
      return kTRUE;

      CbmTofPoint* pPt = (CbmTofPoint*)fTofPointsColl->At( pTofHit->GetRefId() );

      fhTofRes->Fill( pTofHit->GetTime() - pPt->GetTime() );

      fhTofPosDifX->Fill( pPt->GetX(), pPt->GetX() - pTofHit->GetX() );
      fhTofPosDifY->Fill( pPt->GetY(), pPt->GetY() - pTofHit->GetY() );
      fhTofPosDifZ->Fill( pPt->GetZ(), pPt->GetZ() - pTofHit->GetZ() );

      if( 1 == pTofHit->GetFlag() )
      {
         fhTofPosDifSingXX->Fill( pPt->GetX(), pPt->GetX() - pTofHit->GetX() );
         fhTofPosDifSingXY->Fill( pPt->GetY(), pPt->GetX() - pTofHit->GetX() );
         fhTofPosDifSingXZ->Fill( pPt->GetZ(), pPt->GetX() - pTofHit->GetX() );
         fhTofPosDifSingY->Fill( pPt->GetY(), pPt->GetY() - pTofHit->GetY() );
         fhTofPosDifSingZ->Fill( pPt->GetZ(), pPt->GetZ() - pTofHit->GetZ() );
         fhTofPosDifXZSing->Fill( pPt->GetX() - pTofHit->GetX(), pPt->GetZ() - pTofHit->GetZ() );

         if( pPt->GetX() - pTofHit->GetX() < -0.7 ||
               0.7 < pPt->GetX() - pTofHit->GetX() )
            fhTofBadXPosSing->Fill( pPt->GetX(), pPt->GetY() );
//            fhTofBadXPosSing->Fill( pTofHit->GetX(), pTofHit->GetY() );
         if( pPt->GetY() - pTofHit->GetY() < -2.0 ||
               2.0 < pPt->GetY() - pTofHit->GetY() )
            fhTofBadYPosSing->Fill( pPt->GetX(), pPt->GetY() );
//            fhTofBadYPosSing->Fill( pTofHit->GetX(), pTofHit->GetY() );
         if( pPt->GetZ() - pTofHit->GetZ() < -0.6 ||
               0.6 < pPt->GetZ() - pTofHit->GetZ() )
            fhTofBadZPosSing->Fill( pPt->GetX(), pPt->GetY() );
//            fhTofBadZPosSing->Fill( pTofHit->GetX(), pTofHit->GetY() );

         fhTofResSing->Fill( pTofHit->GetTime() - pPt->GetTime() );
      } // if( 1 == pTofHit->GetFlag() )
         else iNbMixedHits++;
   } // for( Int_t iHitInd = 0; iHitInd < iNbTofHits; iHitInd++)
   if( 0 <  iNbTofHits )
      fhTofMixing->Fill(iNbMixedHits/iNbTofHits);

   return kTRUE;
}
// ------------------------------------------------------------------

Bool_t CbmTofTests::WriteHistos()
{
   // TODO: add sub-folders

   // Write histogramms to the file
   TDirectory * oldir = gDirectory;
   TFile *fHist = new TFile("./tofTests.hst.root","RECREATE");
   fHist->cd();

   // Test class performance
   fhTestingTime->Write();

   // Mapping
   fhPointMapXY->Write();
   fhPointMapXZ->Write();
   fhPointMapYZ->Write();
   fhPointMapAng->Write();
   fhPointMapSph->Write();
   fhDigiMapXY->Write();
   fhDigiMapXZ->Write();
   fhDigiMapYZ->Write();
   fhDigiMapAng->Write();
   fhDigiMapSph->Write();
   fhHitMapXY->Write();
   fhHitMapXZ->Write();
   fhHitMapYZ->Write();
   fhHitMapAng->Write();
   fhHitMapSph->Write();

   // Rates and data rates
   fhFluxMap->Write();
   fhDigiFluxMap->Write();
   fhHitFluxMap->Write();
   fhDigiRateCh->Write();
   fhDataRateCh->Write();
   fhDataRateRpc->Write();
   fhDataRateSm->Write();
   fhDataRateType->Write();
   fhTofDataPerEvt->Write();
   fhTofDataRedEvt->Write();
//   fhHitRateCh->Write();
   fhOptLnkRpc->Write();
   fhOptLnkSm->Write();
   fhOptLnkType->Write();

   // Performances check
   fhTofRes->Write();
   fhTofResSing->Write();
   fhTofPosDifX->Write();
   fhTofPosDifY->Write();
   fhTofPosDifZ->Write();
   fhTofPosDifSingXX->Write();
   fhTofPosDifSingXY->Write();
   fhTofPosDifSingXZ->Write();

   fhTofPosDifSingY->Write();
   fhTofPosDifSingZ->Write();
   fhTofPosDifXZSing->Write();

   fhTofBadXPosSing->Write();
   fhTofBadYPosSing->Write();
   fhTofBadZPosSing->Write();

   fhTofEff->Write();
   fhTofMixing->Write();

   gDirectory->cd( oldir->GetPath() );

   fHist->Close();

   return kTRUE;
}
Bool_t   CbmTofTests::DeleteHistos()
{
   // Test class performance
   delete fhTestingTime;

   // Mapping
   delete fhPointMapXY;
   delete fhPointMapXZ;
   delete fhPointMapYZ;
   delete fhPointMapAng;
   delete fhPointMapSph;
   delete fhDigiMapXY;
   delete fhDigiMapXZ;
   delete fhDigiMapYZ;
   delete fhDigiMapAng;
   delete fhDigiMapSph;
   delete fhHitMapXY;
   delete fhHitMapXZ;
   delete fhHitMapYZ;
   delete fhHitMapAng;
   delete fhHitMapSph;

   // Rates and data rates
   delete fhFluxMap;
   delete fhDigiFluxMap;
   delete fhHitFluxMap;
   delete fhDigiRateCh;
   delete fhDataRateCh;
   delete fhDataRateRpc;
   delete fhDataRateSm;
   delete fhDataRateType;
   delete fhTofDataPerEvt;
   delete fhTofDataRedEvt;
//   delete fhHitRateCh;
   delete fhOptLnkRpc;
   delete fhOptLnkSm;
   delete fhOptLnkType;

   // Performances check
   delete fhTofRes;
   delete fhTofResSing;
   delete fhTofPosDifX;
   delete fhTofPosDifY;
   delete fhTofPosDifZ;

   delete fhTofPosDifSingXX;
   delete fhTofPosDifSingXY;
   delete fhTofPosDifSingXZ;

   delete fhTofPosDifSingY;
   delete fhTofPosDifSingZ;
   delete fhTofPosDifXZSing;

   delete fhTofBadXPosSing;
   delete fhTofBadYPosSing;
   delete fhTofBadZPosSing;

   delete fhTofEff;
   delete fhTofMixing;

   return kTRUE;
}


ClassImp(CbmTofTests);
