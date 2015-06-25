/** @file CbmTofDigitizerBDF.cxx
 ** @author Pierre-Alain Loizeau <loizeau@physi.uni-heidelberg.de>
 ** @date 19.07.2013
 **/

#include "CbmTofDigitizerBDF.h"

// TOF Classes and includes
#include "CbmTofPoint.h"      // in cbmdata/tof
#include "CbmTofDigi.h"       // in cbmdata/tof
#include "CbmTofDigiExp.h"    // in cbmdata/tof
#include "CbmTofGeoHandler.h" // in tof/TofTools
#include "CbmTofDetectorId_v12b.h" // in cbmdata/tof
#include "CbmTofDetectorId_v14a.h" // in cbmdata/tof
#include "CbmTofCell.h"       // in tof/TofData
#include "CbmTofDigiPar.h"    // in tof/TofParam
#include "CbmTofDigiBdfPar.h" // in tof/TofParam
#include "CbmTofAddress.h"    // in cbmdata/tof

// CBMroot classes and includes
#include "CbmLink.h"
#include "CbmMCTrack.h"
#include "CbmMatch.h"

// FAIR classes and includes
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

// ROOT Classes and includes
#include "TClonesArray.h"
#include "TMath.h"
#include "TLine.h"
#include "TRandom3.h"
#include "TF2.h"
#include "TVector3.h"
#include "TH1.h"
#include "TH2.h"
#include "TDirectory.h"
#include "TROOT.h"
#include "TGeoManager.h"

// C++ Classes and includes

// Gauss Integration Constants
const Int_t    kiNbIntPts = 2;
Double_t TofChargeDistributions::Gauss1D( Double_t *px, Double_t *par )
{
   Double_t x = px[0];
   Double_t res = par[0]*TMath::Gaus(x,par[1],par[2]);
   return res;
}
Double_t TofChargeDistributions::Gauss2D( Double_t *px, Double_t *par )
{
   Double_t x = px[0];
   Double_t y = px[1];
   Double_t res = par[0]*TMath::Gaus(x,par[1],par[2])*TMath::Gaus(y,par[3],par[4]);
   return res;
}

// If enabled add an offset depending on the strip length by making the full propagation time
// to each strips end
// If disabled just the time to the center is used
// TODO: Add this as parameter
//#define FULL_PROPAGATION_TIME

/************************************************************************************/
CbmTofDigitizerBDF::CbmTofDigitizerBDF():
   FairTask("CbmTofDigitizerBDF"),
   fdFeeGainSigma(0.),
   fdFeeTotThr(0.),
   fdTimeResElec(0.),
   fdSignalPropSpeed(0.),
   fh1ClusterSizeProb(),
   fh1ClusterTotProb(),
   fdChannelGain(),
   fGeoHandler(new CbmTofGeoHandler()),
   fTofId(NULL),
   fDigiPar(NULL),
   fChannelInfo(NULL),
   fDigiBdfPar(NULL),
   fiNbElecChTot(0),
   fvRpcChOffs(),
   fTofPointsColl(NULL),
   fMcTracksColl(NULL),
   fStorDigi(),
   fStorDigiExp(),
   fStorDigiMatch(),
   fvlTrckChAddr(),
   fRandStart(NULL),
   fRandEff(NULL),  
   fRandRadius(NULL),
   fRandCharge(NULL),
   fRandRes(NULL),  
   fbWriteDigisInOut(kTRUE),
   fTofDigisColl(NULL),
   fTofDigiMatchPointsColl(NULL),
   fiNbDigis(0),
   fVerbose(1),
   fhTofPointsPerTrack(NULL),
   fhTofPtsInTrkVsGapInd(NULL),
   fhTofPtsInTrkVsGapIndPrm(NULL),
   fhTofPtsInTrkVsGapIndSec(NULL),
   fhTofPtsPosVsGap(),
   fhEvtProcTime(NULL),
   fhDigiMergeTime(NULL),
   fhDigiNbElecCh(NULL),
   fhProcTimeEvtSize(NULL),
   fhMeanDigiPerTrack(NULL),
   fhMeanFiredPerTrack(NULL),
   fhPtTime(NULL),
   fhDigiTime(NULL),
   fhDigiTimeRes(NULL),
   fhDigiTimeResB(NULL),
   fhToTDist(NULL),
   fhElecChOccup(NULL),
   fhMultiDigiEvtElCh(NULL),
   fhFiredEvtElCh(NULL),
   fhMultiProbElCh(NULL),
   fStart(),
   fStop(),
   fdDigitizeTime(0.),
   fdMergeTime(0.),
   fsBeamInputFile("")
{

}

CbmTofDigitizerBDF::CbmTofDigitizerBDF(const char *name, Int_t verbose, Bool_t writeDataInOut):
   FairTask(TString(name),verbose),
   fdFeeGainSigma(0.),
   fdFeeTotThr(0.),
   fdTimeResElec(0.),
   fdSignalPropSpeed(0.),
   fh1ClusterSizeProb(),
   fh1ClusterTotProb(),
   fdChannelGain(),
   fGeoHandler(new CbmTofGeoHandler()),
   fTofId(NULL),
   fDigiPar(NULL),
   fChannelInfo(NULL),
   fDigiBdfPar(NULL),
   fiNbElecChTot(0),
   fvRpcChOffs(),
   fTofPointsColl(NULL),
   fMcTracksColl(NULL),
   fStorDigi(),
   fStorDigiExp(),
   fStorDigiMatch(),
   fvlTrckChAddr(),
   fRandStart(NULL),
   fRandEff(NULL),  
   fRandRadius(NULL),
   fRandCharge(NULL),
   fRandRes(NULL),  
   fbWriteDigisInOut(writeDataInOut),
   fTofDigisColl(NULL),
   fTofDigiMatchPointsColl(NULL),
   fiNbDigis(0),
   fVerbose(verbose),
   fhTofPointsPerTrack(NULL),
   fhTofPtsInTrkVsGapInd(NULL),
   fhTofPtsInTrkVsGapIndPrm(NULL),
   fhTofPtsInTrkVsGapIndSec(NULL),
   fhTofPtsPosVsGap(),
   fhEvtProcTime(NULL),
   fhDigiMergeTime(NULL),
   fhDigiNbElecCh(NULL),
   fhProcTimeEvtSize(NULL),
   fhMeanDigiPerTrack(NULL),
   fhMeanFiredPerTrack(NULL),
   fhPtTime(NULL),
   fhDigiTime(NULL),
   fhDigiTimeRes(NULL),
   fhDigiTimeResB(NULL),
   fhToTDist(NULL),
   fhElecChOccup(NULL),
   fhMultiDigiEvtElCh(NULL),
   fhFiredEvtElCh(NULL),
   fhMultiProbElCh(NULL),
   fStart(),
   fStop(),
   fdDigitizeTime(0.),
   fdMergeTime(0.),
   fsBeamInputFile("")
{

}

CbmTofDigitizerBDF::~CbmTofDigitizerBDF()
{
   if( fGeoHandler )
      delete fGeoHandler;
      delete fRandStart;
      delete fRandEff;
      delete fRandRadius;
      delete fRandCharge;
      delete fRandRes;
//   DeleteHistos(); // <-- if needed  ?
}

/************************************************************************************/
// FairTasks inherited functions
InitStatus CbmTofDigitizerBDF::Init()
{
   if( kFALSE == RegisterInputs() )
      return kFATAL;

   if( kFALSE == RegisterOutputs() )
      return kFATAL;

   if( kFALSE == InitParameters() )
      return kFATAL;

   if( kFALSE == LoadBeamtimeValues() )
      return kFATAL;

   if( kFALSE == CreateHistos() )
      return kFATAL;

   fRandStart  = new TRandom3();
   fRandStart->SetSeed(0);
   fRandEff    = new TRandom3();
   fRandEff->SetSeed(0);
   fRandRadius = new TRandom3();
   fRandRadius->SetSeed(0);
   fRandCharge = new TRandom3();
   fRandCharge->SetSeed(0);
   fRandRes    = new TRandom3();
   fRandRes->SetSeed(0);

   return kSUCCESS;
}

void CbmTofDigitizerBDF::SetParContainers()
{
   LOG(INFO)<<" CbmTofDigitizerBDF => Get the digi parameters for tof"<<FairLogger::endl;

   // Get Base Container
   FairRunAna* ana = FairRunAna::Instance();
   FairRuntimeDb* rtdb=ana->GetRuntimeDb();

   fDigiPar = (CbmTofDigiPar*)
              (rtdb->getContainer("CbmTofDigiPar"));

   fDigiBdfPar = (CbmTofDigiBdfPar*)
              (rtdb->getContainer("CbmTofDigiBdfPar"));
}

void CbmTofDigitizerBDF::Exec(Option_t * option)
{
   fTofDigisColl->Clear("C");
   fTofDigiMatchPointsColl->Clear("C");

   fiNbDigis = 0;

   LOG(DEBUG)<<" CbmTofDigitizerBDF => New event"<<FairLogger::endl;
   fStart.Set();
   switch( fDigiBdfPar->GetClusterModel() )
   {
      case 0:
         DigitizeDirectClusterSize();
      case 1:
         DigitizeFlatDisc();
         break;
      case 2:
         DigitizeGaussCharge();
         break;
      default:
         DigitizeDirectClusterSize();
         break;
   }

   fStop.Set();
   fdDigitizeTime = fStop.GetSec() - fStart.GetSec()
                    + (fStop.GetNanoSec() - fStart.GetNanoSec())/1e9;

   MergeSameChanDigis();
   fStart.Set();
   fdMergeTime    = fStart.GetSec() - fStop.GetSec()
                    + (fStart.GetNanoSec() - fStop.GetNanoSec())/1e9;

   FillHistos();
}

void CbmTofDigitizerBDF::Finish()
{
   WriteHistos();
   // Prevent them from being sucked in by the CbmHadronAnalysis WriteHistograms method
   DeleteHistos();

   // Clear the cluster size and efficiency histos copies inside the parameter
   fDigiBdfPar->ClearHistos();
}

/************************************************************************************/
// Functions common for all clusters approximations
Bool_t   CbmTofDigitizerBDF::RegisterInputs()
{
   FairRootManager *fManager = FairRootManager::Instance();

   fTofPointsColl  = (TClonesArray *) fManager->GetObject("TofPoint");
   if( NULL == fTofPointsColl)
   {
      LOG(ERROR)<<"CbmTofDigitizerBDF::RegisterInputs => Could not get the TofPoint TClonesArray!!!"<<FairLogger::endl;
      return kFALSE;
   } // if( NULL == fTofPointsColl)

   fMcTracksColl   = (TClonesArray *) fManager->GetObject("MCTrack");
   if( NULL == fMcTracksColl)
   {
      LOG(ERROR)<<"CbmTofDigitizerBDF::RegisterInputs => Could not get the MCTrack TClonesArray!!!"<<FairLogger::endl;
      return kFALSE;
   } // if( NULL == fMcTracksColl)

   return kTRUE;
}
Bool_t   CbmTofDigitizerBDF::RegisterOutputs()
{
   FairRootManager* rootMgr = FairRootManager::Instance();
   if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
   {
      fTofDigisColl = new TClonesArray("CbmTofDigiExp");
   } // if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
      else
      {
         fTofDigisColl = new TClonesArray("CbmTofDigi");
      } // else of if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
   fTofDigiMatchPointsColl = new TClonesArray("CbmMatch",100000);

   // Flag check to control whether digis are written in ouput root file
   rootMgr->Register( "TofDigi","Tof", fTofDigisColl, fbWriteDigisInOut);
   rootMgr->Register( "TofDigiMatchPoints","Tof", fTofDigiMatchPointsColl, fbWriteDigisInOut);

   return kTRUE;
}
Bool_t   CbmTofDigitizerBDF::InitParameters()
{
   // Initialize the TOF GeoHandler
   Bool_t isSimulation=kFALSE;
   Int_t iGeoVersion = fGeoHandler->Init(isSimulation);
   if( k12b > iGeoVersion )
   {
      LOG(ERROR)<<"CbmTofDigitizerBDF::InitParameters => Only compatible with geometries after v12b !!!"
                <<FairLogger::endl;
      return kFALSE;
   }

   LOG(INFO)<<"CbmTofDigitizerBDF::InitParameters: GeoVersion "<<iGeoVersion<<FairLogger::endl;

   switch(iGeoVersion){
       case k12b:
	 fTofId = new CbmTofDetectorId_v12b();
	 break;
       case k14a:
	 fTofId = new CbmTofDetectorId_v14a();
	 break;
       default:
	 LOG(ERROR)<<"CbmTofDigitizerBDF::InitParameters: Invalid Detector ID "<<iGeoVersion<<FairLogger::endl;
   }
   return kTRUE;
}
Bool_t   CbmTofDigitizerBDF::LoadBeamtimeValues()
{
   LOG(INFO)<<"CbmTofDigitizerBDF::LoadBeamtimeValues from "<<fsBeamInputFile
 	    <<FairLogger::endl;

   fDigiBdfPar->SetInputFile(fsBeamInputFile);

   // Printout option for crosscheck
   LOG(INFO)<<"  CbmTofDigitizerBDF::LoadBeamtimeValues Digi Par contains " 
            << fDigiPar->GetNrOfModules() << " cells " <<FairLogger::endl;
   fDigiBdfPar->printParams();

   if(kFALSE == fDigiBdfPar->LoadBeamtimeHistos()){
     LOG(FATAL)<<"CbmTofDigitizerBDF::LoadBeamtimeValues: Cluster properties from testbeam could not be loaded! "<<FairLogger::endl;
     return kFATAL;
   }

//   // Printout option for crosscheck
//   fDigiBdfPar->printParams();

   // Obtain some of the constants
   fdFeeGainSigma    = fDigiBdfPar->GetFeeGainSigma();
   fdFeeTotThr       = fDigiBdfPar->GetFeeThreshold();
   fdTimeResElec     = fDigiBdfPar->GetFeeTimeRes();
   fdSignalPropSpeed = fDigiBdfPar->GetSignalSpeed();

   // Generate the gain/fee channel matrix
   Int_t iNbSmTypes = fDigiBdfPar->GetNbSmTypes();

   LOG(DEBUG2)<<"CbmTofDigitizerBDF::LoadBeamtimeValues: ini gain values for SmTypes "
	      << iNbSmTypes
	      << FairLogger::endl;

   fdChannelGain.resize( iNbSmTypes );

   TRandom3 randFeeGain;
   randFeeGain.SetSeed(0);

   // Save the total number of electronic channels for the "Occupancy" estimation
   // and the first channel of each RPC for the multiple signals estimation
   fiNbElecChTot = 0;
   fvRpcChOffs.resize( iNbSmTypes );

   for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
   {
      LOG(DEBUG2)<<"CbmTofDigitizerBDF::LoadBeamtimeValues => Gain for SM type "<<iSmType<<FairLogger::endl;
      Int_t iNbSm  = fDigiBdfPar->GetNbSm(  iSmType);
      Int_t iNbRpc = fDigiBdfPar->GetNbRpc( iSmType);

      fdChannelGain[iSmType].resize( iNbSm*iNbRpc );
      fvRpcChOffs[iSmType].resize( iNbSm );

      for( Int_t iSm = 0; iSm < iNbSm; iSm++ )
      {
         fvRpcChOffs[iSmType][iSm].resize( iNbRpc );

         for( Int_t iRpc = 0; iRpc < iNbRpc; iRpc++ )
         {
            LOG(DEBUG2)<<"CbmTofDigitizerBDF::LoadBeamtimeValues => Gain for SM/Rpc "<<iSm<<"/"<<iRpc<<FairLogger::endl;
            Int_t iNbCh = fDigiBdfPar->GetNbChan( iSmType, iRpc );
            Int_t iNbSides = 2 - fDigiBdfPar->GetChanType( iSmType, iRpc );

            // Save the total number of electronic channels for the "Occupancy" estimation
            // and the first channel of each RPC for the multiple signals estimation
            fiNbElecChTot += iNbCh*iNbSides;
            fvRpcChOffs[iSmType][iSm][iRpc] = fiNbElecChTot;

            fdChannelGain[iSmType][iSm*iNbRpc + iRpc].resize( iNbCh*iNbSides );

            for( Int_t iCh = 0; iCh < iNbCh; iCh++ )
               for( Int_t iSide = 0; iSide < iNbSides; iSide++ )
                  if( 0 < fdFeeGainSigma )
               {
                  fdChannelGain[iSmType][iSm*iNbRpc + iRpc][iCh*iNbSides + iSide] =
                        randFeeGain.Gaus( 1.0, fdFeeGainSigma);
                  if( fdChannelGain[iSmType][iSm*iNbRpc + iRpc][iCh*iNbSides + iSide] <0.0 )
                     LOG(ERROR)<<"CbmTofDigitizerBDF::LoadBeamtimeValues => Neg. Gain for SM/Rpc "<<iSm<<"/"<<iRpc
                               <<" "<<fdChannelGain[iSmType][iSm*iNbRpc + iRpc][iCh*iNbSides + iSide]<<FairLogger::endl;
               } // if( 0 < fdFeeGainSigma )
                  else fdChannelGain[iSmType][iSm*iNbRpc + iRpc][iCh*iNbSides + iSide] = 1;
         } // for( Int_t iRpc = 0; iRpc < iNbRpc; iRpc++ )
      } // for( Int_t iSm = 0; iSm < iNbSm; iSm++ )
   } // for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )

   TDirectory * oldir = gDirectory;
   gROOT->cd();
   // Generate the Probability conversion histograms for Cluster size
   // and cluster charge
   fh1ClusterSizeProb.resize( iNbSmTypes );
   fh1ClusterTotProb.resize( iNbSmTypes );
   for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
   {
      LOG(DEBUG2)<<"CbmTofDigitizerBDF::LoadBeamtimeValues => SM type "<<iSmType<<FairLogger::endl;

      if( 0 == fDigiBdfPar->GetClusterModel() )
      {
         // Generating the Cluster Radius Probability from the Cluster Size distribution
         fh1ClusterSizeProb[iSmType]  = new TH1D( Form("ClusterSizeProb%03d", iSmType),
                                                        "Random throw to Cluster size mapping",
                                                        10000, 0.0, 1.0 );
         TH1     *hClusterSize = fDigiBdfPar->GetClustSizeHist(iSmType);
         Int_t    iNbBinsSize  = hClusterSize->GetNbinsX();
         Double_t dNbBinsProb = fh1ClusterSizeProb[iSmType]->GetNbinsX();
         Int_t    iProbBin   = 1;
         Double_t dIntegral  = 0;
         Double_t dIntSize    = hClusterSize->GetEntries();
         Double_t dSizeVal;

         for( Int_t iBin = 1; iBin <= iNbBinsSize; iBin++ )
         {
            dIntegral += hClusterSize->GetBinContent( iBin );
            if( (Double_t)iProbBin/dNbBinsProb < dIntegral/dIntSize )
               dSizeVal    = hClusterSize->GetBinCenter( iBin );
            while( (Double_t)iProbBin/dNbBinsProb < dIntegral/dIntSize )
            {
               fh1ClusterSizeProb[iSmType]->SetBinContent( iProbBin, dSizeVal );
               iProbBin++;
            } // while( (Double_t)iProbBin/dNbBinsProb < dIntegral/dIntTot )
         } // for( Int_t iBin = 1; iBin <= iNbBinsTot; iBin++ )
         fh1ClusterSizeProb[iSmType]->SetBinContent( iProbBin, dSizeVal );
      } // if( 0 == fDigiBdfPar->GetClusterModel() )
      else
         switch( fDigiBdfPar->GetClusterRadiusModel() )
         {
            case 0:
            {
               // Single value using the beamtime cluster size distribution mean
               // => Just copy pointer to the beamtime histogram
               fh1ClusterSizeProb[iSmType] = fDigiBdfPar->GetClustSizeHist(iSmType);
               LOG(INFO)<<"CbmTofDigitizerBDF::LoadBeamtimeValues => SM type "<<iSmType
                          <<" Mean Cluster Size "<<(fh1ClusterSizeProb[iSmType]->GetMean())<<FairLogger::endl;
               break;
            }
            case 1:
            {
               // from beamtime cluster size distribution
               // Ingo tip: use Landau distribution until it matchs
               fh1ClusterSizeProb[iSmType] = fDigiBdfPar->GetClustSizeHist(iSmType);
               LOG(INFO)<<"CbmTofDigitizerBDF::LoadBeamtimeValues => SM type "<<iSmType
                          <<" Mean Cluster Size "<<(fh1ClusterSizeProb[iSmType]->GetMean())<<FairLogger::endl;
            }
            default:
            {
               // Should not happen !
               fh1ClusterSizeProb[iSmType] = 0;
               break;
            }
         } // switch( fDigiBdfPar->GetClusterRadiusModel() )


      fh1ClusterTotProb[iSmType]  = new TH1D( Form("ClusterTotProb%03d", iSmType),
                                               "Random throw to Cluster Tot mapping",
                                               10000, 0.0, 1.0 );
      TH1     *hClusterTot = fDigiBdfPar->GetClustTotHist(iSmType);
      Int_t    iNbBinsTot  = hClusterTot->GetNbinsX();
      Double_t dNbBinsProb = fh1ClusterTotProb[iSmType]->GetNbinsX();
      Int_t    iProbBin   = 1;
      Double_t dIntegral  = 0;
      Double_t dIntTot    = hClusterTot->GetEntries();
      Double_t dTotVal;
      Double_t dPsToNs = 1e3; // FIXME? Most histograms from analysis are in ps, simulation used ns !!

      for( Int_t iBin = 1; iBin <= iNbBinsTot; iBin++ )
      {
         dIntegral += hClusterTot->GetBinContent( iBin );
         if( (Double_t)iProbBin/dNbBinsProb < dIntegral/dIntTot )
            dTotVal    = hClusterTot->GetBinLowEdge( iBin )/dPsToNs;
         while( (Double_t)iProbBin/dNbBinsProb < dIntegral/dIntTot )
         {
            fh1ClusterTotProb[iSmType]->SetBinContent( iProbBin, dTotVal );
            iProbBin++;
         } // while( (Double_t)iProbBin/dNbBinsProb < dIntegral/dIntTot )
      } // for( Int_t iBin = 1; iBin <= iNbBinsTot; iBin++ )
      fh1ClusterTotProb[iSmType]->SetBinContent( iProbBin, dTotVal );
   } // for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
   gDirectory->cd( oldir->GetPath() );

   // Prepare the intermediate Digi storage
   if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
   {
      fStorDigiExp.resize( iNbSmTypes );
      fStorDigiMatch.resize( iNbSmTypes );
      for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
      {
         Int_t iNbSm  = fDigiBdfPar->GetNbSm(  iSmType);
         Int_t iNbRpc = fDigiBdfPar->GetNbRpc( iSmType);
         fStorDigiExp[iSmType].resize( iNbSm*iNbRpc );
         fStorDigiMatch[iSmType].resize( iNbSm*iNbRpc );
         for( Int_t iSm = 0; iSm < iNbSm; iSm++ )
            for( Int_t iRpc = 0; iRpc < iNbRpc; iRpc++ )
            {
               Int_t iNbCh = fDigiBdfPar->GetNbChan( iSmType, iRpc );
               Int_t iNbSides = 2 - fDigiBdfPar->GetChanType( iSmType, iRpc );
               fStorDigiExp[iSmType][iSm*iNbRpc + iRpc].resize( iNbCh*iNbSides );
               fStorDigiMatch[iSmType][iSm*iNbRpc + iRpc].resize( iNbCh*iNbSides );
            } // for each (Sm, rpc) pair
      } // for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
   } // if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
      else
      {
         fStorDigi.resize( iNbSmTypes );
	 fStorDigiMatch.resize( iNbSmTypes );
	 for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
         {
            Int_t iNbSm  = fDigiBdfPar->GetNbSm(  iSmType);
            Int_t iNbRpc = fDigiBdfPar->GetNbRpc( iSmType);
            fStorDigi[iSmType].resize( iNbSm*iNbRpc );
	    fStorDigiMatch[iSmType].resize( iNbSm*iNbRpc );
            for( Int_t iSm = 0; iSm < iNbSm; iSm++ )
               for( Int_t iRpc = 0; iRpc < iNbRpc; iRpc++ )
               {
                  Int_t iNbCh = fDigiBdfPar->GetNbChan( iSmType, iRpc );
                  Int_t iNbSides = 2 - fDigiBdfPar->GetChanType( iSmType, iRpc );
                  fStorDigi[iSmType][iSm*iNbRpc + iRpc].resize( iNbCh*iNbSides );
		  fStorDigiMatch[iSmType][iSm*iNbRpc + iRpc].resize( iNbCh*iNbSides );
               } // for each (Sm, rpc) pair
         } // for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
      } // else of if( kTRUE == fDigiBdfPar->UseExpandedDigi() )

   cout<<"CbmTofDigitizerBDF::LoadBeamtimeValues: GeoVersion "<<fGeoHandler->GetGeoVersion()<<endl;

   // TEST stupid stuffs
   Int_t iNbGeantChannels = fDigiPar->GetNrOfModules();
   Int_t iMinSmType =  17; // 4b for sm type in ID v12b normally
   Int_t iMaxSmType =  -1; // 4b for sm type in ID v12b normally
   Int_t iMinSm     = 257; // 8b for sm type in ID v12b normally
   Int_t iMaxSm     =  -1; // 8b for sm type in ID v12b normally
   Int_t iMinRpc    =   9; // 3b for sm type in ID v12b normally
   Int_t iMaxRpc    =  -1; // 3b for sm type in ID v12b normally
   Int_t iMinGap    =  17; // 4b for sm type in ID v12b normally
   Int_t iMaxGap    =  -1; // 4b for sm type in ID v12b normally
   Int_t iMinCell   = 257; // 8b for sm type in ID v12b normally
   Int_t iMaxCell   =  -1; // 4b for sm type in ID v12b normally
   for(Int_t iCellInd = 0; iCellInd < iNbGeantChannels; iCellInd++)
   {
      Int_t iCellId = fDigiPar->GetCellId( iCellInd );

      if( iMinSmType > fTofId->GetSMType(iCellId) )
         iMinSmType = fTofId->GetSMType(iCellId);
      if( iMaxSmType < fTofId->GetSMType(iCellId) )
         iMaxSmType = fTofId->GetSMType(iCellId);

      if( iMinSm > fTofId->GetSModule(iCellId) )
         iMinSm = fTofId->GetSModule(iCellId);
      if( iMaxSm < fTofId->GetSModule(iCellId) )
         iMaxSm = fTofId->GetSModule(iCellId);

      if( iMinRpc > fTofId->GetCounter(iCellId) )
         iMinRpc = fTofId->GetCounter(iCellId);
      if( iMaxRpc < fTofId->GetCounter(iCellId) )
         iMaxRpc = fTofId->GetCounter(iCellId);

      if( iMinGap > fTofId->GetGap(iCellId) )
         iMinGap = fTofId->GetGap(iCellId);
      if( iMaxGap < fTofId->GetGap(iCellId) )
         iMaxGap = fTofId->GetGap(iCellId);

      if( iMinCell > fTofId->GetCell(iCellId) )
         iMinCell = fTofId->GetCell(iCellId);
      if( iMaxCell < fTofId->GetCell(iCellId) )
         iMaxCell = fTofId->GetCell(iCellId);
   }
   cout<<"SM type min "<<iMinSmType<<" max "<<iMaxSmType<<endl;
   cout<<"SM      min "<<iMinSm<<" max "<<iMaxSm<<endl;
   cout<<"Rpc     min "<<iMinRpc<<" max "<<iMaxRpc<<endl;
   cout<<"Gap     min "<<iMinGap<<" max "<<iMaxGap<<endl;
   cout<<"Chan    min "<<iMinCell<<" max "<<iMaxCell<<endl;
   //
   return kTRUE;
}
/************************************************************************************/
// Histogramming functions
Bool_t   CbmTofDigitizerBDF::CreateHistos()
{
   TDirectory * oldir = gDirectory; // <= To prevent histos from being sucked in by the param file of the TRootManager!
   gROOT->cd(); // <= To prevent histos from being sucked in by the param file of the TRootManager !

   fhTofPointsPerTrack = new TH1I( "TofDigiBdf_TofPtPerTrk", "Number of Tof Points per MC track reaching the TOF; Nb Tof Points in Tracks []",
                                   16, 0.0, 16.0 );
   fhTofPtsInTrkVsGapInd = new TH2I( "TofDigiBdf_TofPtInTrkGap", "Gap index vs Number of Tof Points in corresponding MC Track; Nb Tof Pts in Track []; Gap Index []",
                                    16, 0.0, 16.0,
                                    10, -0.5, 9.5 );
   fhTofPtsInTrkVsGapIndPrm = new TH2I( "TofDigiBdf_TofPtInTrkGapPrm", "Gap index vs Number of Tof Points in corresponding MC Track, primaries; Nb Tof Pts in Track []; Gap Index []",
                                    16, 0.0, 16.0,
                                    10, -0.5, 9.5 );
   fhTofPtsInTrkVsGapIndSec = new TH2I( "TofDigiBdf_TofPtInTrkGapSec", "Gap index vs Number of Tof Points in corresponding MC Track, secondaries; Nb Tof Pts in Track []; Gap Index []",
                                    16, 0.0, 16.0,
                                    10, -0.5, 9.5 );
   for(Int_t iGap = 0; iGap < 10; iGap++)
      fhTofPtsPosVsGap[iGap] = new TH2I( Form("TofDigiBdf_fhTofPtsPosVsGap%d", iGap),
            Form("Tof Point positions in gap %d; X [cm]; Y [cm]", iGap),
            750, -750, 750,
            500, -500, 500 );
   /*
   fhTofPointsPerTrackVsPdg = new TH2I( "TofDigiBdf_TofPtPerTrkVsPdg", "Number of Tof Points per MC track in each event; PDG []; Nb Points/Nb Tracks []",
         800, -4000, 4000,
         16, 0.0, 16.0 );
   fhMeanPointPerTrack = new TH1I( "TofDigiBdf_PtPerTrk", "Mean Number of Tof Points per MC track in each event; Nb Points/Nb Tracks []",
                               800, 0.0, 16.0 );
   fhMeanPointPerTrack2d = new TH2I( "TofDigiBdf_PtPerTrk2d", "Mean Number of Tof Points per MC track in each event; Nb Tracks []; Nb Tof Points/Nb Tracks []",
         300, 0.0, 3000.0,
         800, 0.0, 16.0 );
   fhMeanDigiPerPoint  = new TH1I( "TofDigiBdf_DigiPerPt", "Mean Number of ToF Digi per Tof Point in each event; Nb Digi/Nb Points []",
                               500, 0.0, 10.0 );
   fhMeanFiredPerPoint = new TH1I( "TofDigiBdf_FirPerPt", "Mean Number of fired channel per Tof Point in each event; Nb Fired/Nb Points []",
                               500, 0.0, 10.0 );
*/
   fhEvtProcTime = new TH1I( "TofDigiBdf_EvtProcTime", "Time needed to process each event; Time [s]",
                              40000, 0.0, 40.0 );
   fhDigiMergeTime = new TH1I( "TofDigiBdf_DigiMergeTime", "Time needed to merge the digis for each event; Time [s]",
                              4000, 0.0, 0.4 );
   fhDigiNbElecCh = new TH1I( "TofDigiBdf_DigiNbElecCh", "Nb of digis per channel before merging; Nb Digis in same elec. ch []",
         50, 0.0, 50 );
   fhProcTimeEvtSize = new TH2I( "TofDigiBdf_ProcTimeEvtSize", "Time needed to process each event vs Event Size; Event Size [TofPoints]; Time [s]",
                               600, 0.0, 12000.0,
                               400, 0.0,     4.0 );
   fhMeanDigiPerTrack  = new TH1I( "TofDigiBdf_DigiPerTrk", "Mean Number of ToF Digi per MC track in each event; Nb Digi/Nb Tracks []",
                               500, 0.0, 10.0 );
   fhMeanFiredPerTrack = new TH1I( "TofDigiBdf_FirPerTrk", "Mean Number of fired channel per MC track in each event; Nb Fired/Nb Tracks []",
                               500, 0.0, 10.0 );
   fhPtTime = new TH1I( "TofDigiBdf_PtTime", "TofPoints Time distribution, summed for all Pts/channels; Time [ns]",
         10000, 0.0, 10000.0 );
   fhDigiTime = new TH1I( "TofDigiBdf_DigiTime", "Time distribution, summed for all digis/channels; Time [ns]",
         10000, 0.0, 10000.0 );
   fhDigiTimeRes = new TH1I( "TofDigiBdf_DigiTimeRes", "Time to True time distribution, summed for all digis/channels; Time Digi - Time Pt [ns]",
         10000, -50.0, 50.0 );
   fhDigiTimeResB = new TH2I( "TofDigiBdf_DigiTimeResB", "Time to True time distribution, summed for all digis/channels; Time Digi - Time Pt [ns]",
         10000, -50.0, 50.0,
         6, 0, 6);
   fhToTDist = new TH1I( "TofDigiBdf_ToTDist", "ToT distribution, summed for all digis/channels; Tot [ns]",
                                500, 0.0, 50.0 );

   fhElecChOccup = new TH1I( "TofDigiBdf_ElecChOccup", "Percentage of ToF electronic channels fired per event; Elect. chan occupancy [%]",
         1000, 0.0, 100.0 );
   fhMultiDigiEvtElCh = new TH1D( "TofDigiBdf_MultiDigiEvtElCh",
         "Number of events with multiple digi (~MC track) per electronic channel; Elect. chan index []",
         fiNbElecChTot, 0.0, fiNbElecChTot );
   fhFiredEvtElCh = new TH1D( "TofDigiBdf_FiredEvtElCh",
         "Number of events with at least 1 digi per electronic channel; Elect. chan index []",
         fiNbElecChTot, 0.0, fiNbElecChTot );
   fhMultiProbElCh = new TH1D( "TofDigiBdf_MultiProbElCh",
         "Probability of having a multiple digi (~MC track) event per electronic channel; Elect. chan index []; Multiple signal prob. [%]",
         fiNbElecChTot, 0.0, fiNbElecChTot );

   gDirectory->cd( oldir->GetPath() ); // <= To prevent histos from being sucked in by the param file of the TRootManager!

   return kTRUE;
}
Bool_t   CbmTofDigitizerBDF::FillHistos()
{
   Int_t nTofPoint = fTofPointsColl->GetEntries();
   Int_t nTracks   = fMcTracksColl->GetEntries();
   Int_t nTofDigi  = fTofDigisColl->GetEntries();
   Double_t nTofFired  = 0;
   Double_t dProcessTime = fdDigitizeTime + fdMergeTime;
   // Trakcs Info
   Int_t iNbTofTracks     = 0;
   Int_t iNbTofTracksPrim = 0;
   CbmMCTrack  *pMcTrk;
   for(Int_t iTrkInd = 0; iTrkInd < nTracks; iTrkInd++)
   {
      pMcTrk = (CbmMCTrack*) fMcTracksColl->At( iTrkInd );
      // Jump all tracks not making 8 points for test
//      if( 8 != pMcTrk->GetNPoints(kTOF) )
//         continue;
      if( 0 < pMcTrk->GetNPoints(kTOF) )
      {
         iNbTofTracks++;
         fhTofPointsPerTrack->Fill( pMcTrk->GetNPoints(kTOF) );
         /*
         fhTofPointsPerTrackVsPdg->Fill( pMcTrk->GetPdgCode(), pMcTrk->GetNPoints(kTOF) );
         */
      }
      if( 0 < pMcTrk->GetNPoints(kTOF) && -1 == pMcTrk->GetMotherId() )
         iNbTofTracksPrim++;

   } // for(Int_t iTrkInd = 0; iTrkInd < nMcTracks; iTrkInd++)

   // Tof Points info
   CbmTofPoint  *pTofPt;
   for(Int_t iPtInd = 0; iPtInd < nTofPoint; iPtInd++)
   {
      pTofPt = (CbmTofPoint*) fTofPointsColl->At( iPtInd );
      fhPtTime->Fill( pTofPt->GetTime() );

      Int_t iDetId  = pTofPt->GetDetectorID();
      Int_t iGap    = fGeoHandler->GetGap(iDetId);
      pMcTrk = (CbmMCTrack*) fMcTracksColl->At( pTofPt->GetTrackID() );

      fhTofPtsInTrkVsGapInd->Fill( pMcTrk->GetNPoints(kTOF), iGap );
      if( -1 == pMcTrk->GetMotherId() )
         fhTofPtsInTrkVsGapIndPrm->Fill( pMcTrk->GetNPoints(kTOF), iGap );
      else if( 11 != pMcTrk->GetPdgCode() ) // Remove electrons
         fhTofPtsInTrkVsGapIndSec->Fill( pMcTrk->GetNPoints(kTOF), iGap );

      // Get TofPoint Position
      TVector3 vPntPos;
      pTofPt->Position( vPntPos );
      if( 8-pMcTrk->GetNPoints(kTOF) <= iGap &&
            pMcTrk->GetNPoints(kTOF) < 8 &&
            -1 != pMcTrk->GetMotherId() )
      fhTofPtsPosVsGap[iGap]->Fill(  vPntPos.X(),  vPntPos.Y() );
   }

   fhDigiMergeTime->Fill( fdMergeTime );
   fhEvtProcTime->Fill( dProcessTime );
   fhProcTimeEvtSize->Fill( nTofPoint, dProcessTime );
   fhMeanDigiPerTrack->Fill( (Double_t)nTofDigi/(Double_t)iNbTofTracks );
//   fhMeanDigiPerPoint->Fill( (Double_t)nTofDigi/(Double_t)nTofPoint );
//   fhMeanPointPerTrack->Fill( (Double_t)nTofPoint/(Double_t)iNbTofTracks);
//   fhMeanPointPerTrack2d->Fill( iNbTofTracks, (Double_t)nTofPoint/(Double_t)iNbTofTracks);

   // Assume for the occupancy that we can only have one Digi per electronic channel
   // No Multiple hits/digi in same event!
   fhElecChOccup->Fill( 100.0*(Double_t)nTofDigi/(Double_t)fiNbElecChTot );

   if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
   {
      CbmTofDigiExp *pDigi;
      for( Int_t iDigInd = 0; iDigInd < nTofDigi; iDigInd++ )
      {
         pDigi = (CbmTofDigiExp*) fTofDigisColl->At( iDigInd );
         CbmMatch* digiMatch=(CbmMatch *)fTofDigiMatchPointsColl->At(iDigInd);	
	 CbmLink L0 = digiMatch->GetLink(0); 
	 CbmTofPoint* point = (CbmTofPoint*)fTofPointsColl->At( L0.GetIndex()); 

         if( pDigi->GetTot() < 0 )
            cout<<iDigInd<<"/"<<nTofDigi<<" "<<pDigi->GetTot()<<endl;
         fhDigiTime->Fill( pDigi->GetTime() );
         fhDigiTimeRes->Fill( pDigi->GetTime() - point->GetTime() );
         fhDigiTimeResB->Fill( pDigi->GetTime() - point->GetTime(),
               pDigi->GetType() );
         fhToTDist->Fill( pDigi->GetTot() );

         nTofFired += 1.0/( 2.0 - fDigiBdfPar->GetChanType( pDigi->GetType(), pDigi->GetRpc() ) );
      } // for( Int_t iDigInd = 0; iDigInd < nTofDigi; iDigInd++ )
   } // if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
      else
      {
         CbmTofDigi *pDigi;
         for( Int_t iDigInd = 0; iDigInd < nTofDigi; iDigInd++ )
         {
            pDigi = (CbmTofDigi*) fTofDigisColl->At( iDigInd );
	    CbmMatch* digiMatch=(CbmMatch *)fTofDigiMatchPointsColl->At(iDigInd);	
	    CbmLink L0 = digiMatch->GetLink(0); 
	    CbmTofPoint* point = (CbmTofPoint*)fTofPointsColl->At( L0.GetIndex()); 
	 //			CbmTofPoint* point = (CbmTofPoint*)fTofPointsColl->At(pDigi->GetMatch()->GetMatchedLink().GetIndex()); 
            if( pDigi->GetTot() < 0 )
               cout<<iDigInd<<"/"<<nTofDigi<<" "<<pDigi->GetTot()<<endl;
            fhDigiTime->Fill( pDigi->GetTime() );
            fhDigiTimeRes->Fill( pDigi->GetTime() - point->GetTime() );
            fhToTDist->Fill( pDigi->GetTot() );

            nTofFired += 1.0/( 2.0 - fDigiBdfPar->GetChanType( pDigi->GetType(), pDigi->GetRpc() ) );
         } // for( Int_t iDigInd = 0; iDigInd < nTofDigi; iDigInd++ )
      } // else of if( kTRUE == fDigiBdfPar->UseExpandedDigi() )

//   fhMeanFiredPerPoint->Fill( nTofFired / (Double_t)nTofPoint);
   fhMeanFiredPerTrack->Fill( nTofFired/(Double_t)iNbTofTracks );

   return kTRUE;
}
Bool_t   CbmTofDigitizerBDF::WriteHistos()
{
   TDirectory * oldir = gDirectory;
   TFile *fHist = new TFile("./tofDigiBdf.hst.root","RECREATE");

   fHist->cd();

   fhTofPointsPerTrack->Write();
   fhTofPtsInTrkVsGapInd->Write();
   fhTofPtsInTrkVsGapIndPrm->Write();
   fhTofPtsInTrkVsGapIndSec->Write();
   for(Int_t iGap = 0; iGap < 10; iGap++)
      fhTofPtsPosVsGap[iGap]->Write();
   /*
   fhTofPointsPerTrackVsPdg->Write();
   fhMeanDigiPerPoint->Write();
   fhMeanFiredPerPoint->Write();
   fhMeanPointPerTrack->Write();
   fhMeanPointPerTrack2d->Write();
*/
   fhEvtProcTime->Write();
   fhDigiMergeTime->Write();
   fhDigiNbElecCh->Write();
   fhProcTimeEvtSize->Write();
   fhMeanDigiPerTrack->Write();
   fhMeanFiredPerTrack->Write();
   fhPtTime->Write();
   fhDigiTime->Write();
   fhDigiTimeRes->Write();
   fhDigiTimeResB->Write();
   fhToTDist->Write();

   fhElecChOccup->Write();
   fhMultiDigiEvtElCh->Write();
   fhFiredEvtElCh->Write();
   fhMultiProbElCh->Divide(fhMultiDigiEvtElCh, fhFiredEvtElCh);
   fhMultiProbElCh->Scale( 100.0 );
   fhMultiProbElCh->Write();

   // Uncomment if need to control the Cluster Size and ToT proba
   Int_t iNbSmTypes = fDigiBdfPar->GetNbSmTypes();
   for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
   {
      if( 0 == fDigiBdfPar->GetClusterModel() )
         fh1ClusterSizeProb[iSmType]->Write();
      fh1ClusterTotProb[iSmType]->Write();
   }

   gDirectory->cd( oldir->GetPath() );

   fHist->Close();

   return kTRUE;
}
Bool_t   CbmTofDigitizerBDF::DeleteHistos()
{

   delete fhTofPointsPerTrack;
   delete fhTofPtsInTrkVsGapInd;
   delete fhTofPtsInTrkVsGapIndPrm;
   delete fhTofPtsInTrkVsGapIndSec;
   for(Int_t iGap = 0; iGap < 10; iGap++)
      delete fhTofPtsPosVsGap[iGap];
   /*
   delete fhTofPointsPerTrackVsPdg;
   delete fhMeanDigiPerPoint;
   delete fhMeanFiredPerPoint;
   delete fhMeanPointPerTrack;
   delete fhMeanPointPerTrack2d;
*/
   delete fhEvtProcTime;
   delete fhDigiMergeTime;
   delete fhDigiNbElecCh;
   delete fhProcTimeEvtSize;
   delete fhMeanDigiPerTrack;
   delete fhMeanFiredPerTrack;
   delete fhPtTime;
   delete fhDigiTime;
   delete fhDigiTimeRes;
   delete fhDigiTimeResB;
   delete fhToTDist;

   delete fhElecChOccup;
   delete fhMultiDigiEvtElCh;
   delete fhFiredEvtElCh;
   delete fhMultiProbElCh;

   Int_t iNbSmTypes = fDigiBdfPar->GetNbSmTypes();
   for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
   {
      if( 0 == fDigiBdfPar->GetClusterModel() )
         delete fh1ClusterSizeProb[iSmType];
      delete fh1ClusterTotProb[iSmType];
   }

   return kTRUE;
}
/************************************************************************************/
// Functions for the merging of "gap digis" and "multiple hits digis" into "channel digis"
// TODO: FEE double hit discrimination capability (using Time distance between Digis)
// TODO: Charge summing up
Bool_t   CbmTofDigitizerBDF::MergeSameChanDigis()
{
   Int_t iNbSmTypes = fDigiBdfPar->GetNbSmTypes();
   if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
   {
      // loop over each (Smtype, Sm, Rpc, Channel, Side)
      for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
      {
         Int_t iNbSm  = fDigiBdfPar->GetNbSm(  iSmType);
         Int_t iNbRpc = fDigiBdfPar->GetNbRpc( iSmType);

         for( Int_t iSm = 0; iSm < iNbSm; iSm++ )
            for( Int_t iRpc = 0; iRpc < iNbRpc; iRpc++ )
            {
               Int_t iNbCh = fDigiBdfPar->GetNbChan( iSmType, iRpc );
               Int_t iNbSides = 2 - fDigiBdfPar->GetChanType( iSmType, iRpc );

               for( Int_t iCh = 0; iCh < iNbCh; iCh++ )
                  for( Int_t iSide = 0; iSide < iNbSides; iSide++ )
                  {
                     Int_t iNbDigis = fStorDigiExp[iSmType][iSm*iNbRpc + iRpc][iNbSides*iCh+iSide].size();
                     if( 0 < iNbDigis )
                     {
                        fhDigiNbElecCh->Fill(iNbDigis);

                        fhFiredEvtElCh->Fill( fvRpcChOffs[iSmType][iSm][iRpc] + iNbSides*iCh + iSide );
                        if( 1 < iNbDigis )
                           fhMultiDigiEvtElCh->Fill( fvRpcChOffs[iSmType][iSm][iRpc] + iNbSides*iCh + iSide );

                        Int_t iChosenDigi = -1;
                        Double_t dMinTime = 1e18;
			Int_t    ivDigiInd[iNbDigis];
			Double_t dvDigiTime[iNbDigis];
                        for( Int_t iDigi = 0; iDigi < iNbDigis; iDigi++)
                          if( fStorDigiExp[iSmType][iSm*iNbRpc + iRpc][iNbSides*iCh+iSide][iDigi]->GetTime()
                                                                                          < dMinTime )
                          {
                              iChosenDigi = iDigi;
                              dMinTime = fStorDigiExp[iSmType][iSm*iNbRpc + iRpc][iNbSides*iCh+iSide][iDigi]->GetTime();
                          }

		        if (0 == fStorDigiMatch[iSmType][iSm*iNbRpc + iRpc][iNbSides*iCh+iSide].size()){
			  LOG(ERROR)<<Form(" cannot add digiMatch for (%d,%d,%d,%d,%d) at pos  %d",
					   iSmType,iSm,iRpc,iCh,iSide,fiNbDigis)<<FairLogger::endl;
			  break;
			}

                        new((*fTofDigisColl)[fiNbDigis]) CbmTofDigiExp(
                              *fStorDigiExp[iSmType][iSm*iNbRpc + iRpc][iNbSides*iCh+iSide][iChosenDigi] );

			LOG(DEBUG)<<Form("Add digi %d (%zu) match of (%d,%d,%d,%d,%d) at pos %d", 
					iChosenDigi, fStorDigiMatch[iSmType][iSm*iNbRpc + iRpc][iNbSides*iCh+iSide].size(),
					iSmType,iSm,iRpc,iCh,iSide,fiNbDigis)<<FairLogger::endl;


			CbmMatch* digiMatch = new CbmMatch();
			digiMatch->AddLink(CbmLink(1.,fStorDigiMatch[iSmType][iSm*iNbRpc + iRpc][iNbSides*iCh+iSide][iChosenDigi] ));
			
			new((*fTofDigiMatchPointsColl)[fiNbDigis]) CbmMatch(*digiMatch);
			CbmLink LP = digiMatch->GetLink(0); 
			Int_t lp=LP.GetIndex();
			delete digiMatch;

			LOG(DEBUG)<<"CbmTofDigitizerBDF:: TofDigiMatchColl entry "
				  <<fTofDigiMatchPointsColl->GetEntries()-1
				  <<", Poi: "<<fStorDigiMatch[iSmType][iSm*iNbRpc + iRpc][iNbSides*iCh+iSide][iChosenDigi]
				  <<", lp: "<<lp
				  <<", MCt: "<<((CbmTofPoint*) fTofPointsColl->At(lp))->GetTrackID()
				  <<FairLogger::endl;
                        if(lp != fStorDigiMatch[iSmType][iSm*iNbRpc + iRpc][iNbSides*iCh+iSide][iChosenDigi] )
			  LOG(ERROR)<<Form("CbmTofDigitizerBDF::MergeSameChanDigis inconsistent links: %d <-> %d for (%d,%d,%d,%d,%d)",
				    lp, fStorDigiMatch[iSmType][iSm*iNbRpc + iRpc][iNbSides*iCh+iSide][iChosenDigi],
				    iSmType,iSm,iRpc,iCh,iSide)<<FairLogger::endl;
                        fiNbDigis++;
			for( Int_t iDigi = 0; iDigi < iNbDigis; iDigi++){
                           delete fStorDigiExp[iSmType][iSm*iNbRpc + iRpc][iNbSides*iCh+iSide][iDigi];
			   //delete fStorDigiMatch[iSmType][iSm*iNbRpc + iRpc][iNbSides*iCh+iSide][iDigi];
			}
                        fStorDigiExp[iSmType][iSm*iNbRpc + iRpc][iNbSides*iCh+iSide].clear();
                        fStorDigiMatch[iSmType][iSm*iNbRpc + iRpc][iNbSides*iCh+iSide].clear();
                     } // if( 0 < iNbDigis )
                  } // for each (Ch, Side) pair
            } // for each (Sm, rpc) pair
      } // for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
   } // if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
   else
   {
      LOG(ERROR)<<Form(" skip compressed digi section ")<<FairLogger::endl;
      return 0;

      // loop over each (Smtype, Sm, Rpc, Channel, Side)
      for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
      {
         Int_t iNbSm  = fDigiBdfPar->GetNbSm(  iSmType);
         Int_t iNbRpc = fDigiBdfPar->GetNbRpc( iSmType);
         for( Int_t iSm = 0; iSm < iNbSm; iSm++ )
            for( Int_t iRpc = 0; iRpc < iNbRpc; iRpc++ )
            {
               Int_t iNbCh = fDigiBdfPar->GetNbChan( iSmType, iRpc );
               Int_t iNbSides = 2 - fDigiBdfPar->GetChanType( iSmType, iRpc );

               for( Int_t iCh = 0; iCh < iNbCh; iCh++ )
                  for( Int_t iSide = 0; iSide < iNbSides; iSide++ )
                  {
                     Int_t iNbDigis = fStorDigi[iSmType][iSm*iNbRpc + iRpc][iNbSides*iCh+iSide].size();
                     if( 0 < iNbDigis )
                     {
                        fhDigiNbElecCh->Fill(iNbDigis);

                        fhFiredEvtElCh->Fill( fvRpcChOffs[iSmType][iSm][iRpc] + iNbSides*iCh + iSide );
                        if( 1 < iNbDigis )
                           fhMultiDigiEvtElCh->Fill( fvRpcChOffs[iSmType][iSm][iRpc] + iNbSides*iCh + iSide );

                        Int_t iChosenDigi = -1;
                        Double_t dMinTime = 1e18;
                        for( Int_t iDigi = 0; iDigi < iNbDigis; iDigi++)
                           if( fStorDigi[iSmType][iSm*iNbRpc + iRpc][iNbSides*iCh+iSide][iDigi]->GetTime()
                                                                                          < dMinTime )
                           {
                              iChosenDigi = iDigi;
                              dMinTime = fStorDigi[iSmType][iSm*iNbRpc + iRpc][iNbSides*iCh+iSide][iDigi]->GetTime();
                           }

			LOG(DEBUG)<<Form(" New Tof Digi ")<<FairLogger::endl;

                        new((*fTofDigisColl)[fiNbDigis]) CbmTofDigi(
                              *fStorDigi[iSmType][iSm*iNbRpc + iRpc][iNbSides*iCh+iSide][iChosenDigi] );

			CbmMatch* digiMatch = new CbmMatch();
			digiMatch->AddLink(CbmLink(0.,fStorDigiMatch[iSmType][iSm*iNbRpc + iRpc][iNbSides*iCh+iSide][iChosenDigi] ));
			new((*fTofDigiMatchPointsColl)[fiNbDigis]) CbmMatch(*digiMatch);
			delete digiMatch;
                        fiNbDigis++;

                        for( Int_t iDigi = 0; iDigi < iNbDigis; iDigi++){
                           delete fStorDigi[iSmType][iSm*iNbRpc + iRpc][iNbSides*iCh+iSide][iDigi];
			   //                           delete fStorDigiMatch[iSmType][iSm*iNbRpc + iRpc][iNbSides*iCh+iSide][iDigi];
			}
                        fStorDigi[iSmType][iSm*iNbRpc + iRpc][iNbSides*iCh+iSide].clear();
                        fStorDigiMatch[iSmType][iSm*iNbRpc + iRpc][iNbSides*iCh+iSide].clear();
                     } // if( 0 < iNbDigis )
                  } // for each (Ch, Side) pair
            } // for each (Sm, rpc) pair
      } // for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
   }// else of if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
   return kTRUE;
}
/************************************************************************************/
// Functions for the Cluster Radius generation
Double_t CbmTofDigitizerBDF::GenerateClusterRadius( Int_t iSmType, Int_t iRpc )
{
   Double_t dClusterRadius = 0;
   Int_t    iChType = fDigiBdfPar->GetChanType( iSmType, iRpc );

   switch( fDigiBdfPar->GetClusterRadiusModel() )
   {
      case -1:
      {
         // Fixed value at 0.0002 to get a cluster size as close to 1 as possible
         dClusterRadius = 0.0002;
         break;
      }
      case 0:
      {
         // Single value using the beamtime cluster size distribution mean
         if( 0 == iChType )
         {
            // Simple linear relation mean cluster size = 2* radius +1 in [strips]
            // Cf "RadiusToMeanClusterAll" histogram
            // Come from simple geometry of cluster center position if one
            // neglect RPC and extremities edge effects
            dClusterRadius = (fh1ClusterSizeProb[iSmType]->GetMean() - 1.0)/2.0;
            // Convert to [cm]
            if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
               // Horizontal channel => strip width along Y
               dClusterRadius *= fChannelInfo->GetSizey();
               // Vertical channel => strip width along X
               else dClusterRadius *= fChannelInfo->GetSizex();
         } // if( 0 == iChType)
            else
            {
               LOG(ERROR)<<"CbmTofDigitizerBDF::GenerateClusterRadius => Cluster Radius "
                     <<"obtention from cluster size not implemented for pads, Sm type "
                     <<iSmType<<" Rpc "<<iRpc<<FairLogger::endl;
               LOG(ERROR)<<"CbmTofDigitizerBDF::GenerateClusterRadius => Test "
                     <<" Sm type "<<iSmType<<" Rpc "<<iRpc
                     <<" Type "<<iChType<<" Type "<<(Int_t)iChType<<FairLogger::endl;
               return kFALSE;
            } // else of if( 0 == iChType)
         break;
      } // case 0:
      case 1:
      case 2:
      {
         if( 0 == iChType)
         {
            // from beamtime cluster size distribution
            Double_t dStripSize = 0;
            // Convert to [cm]
            if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
               // Horizontal channel => strip width along Y
               dStripSize = fChannelInfo->GetSizey();
               // Vertical channel => strip width along X
               else dStripSize = fChannelInfo->GetSizex();

            // Obtain the Landau peak position and sigma scale factor
            // from the parameter object. It should take care of getting the values
            // either from the ASCII parameter file or from a fit on the beam data
            // TODO: cross-check the exact value of the second parameter, here it is set so that probability of
            // negative radius is really low
            Double_t dPeakPos  = fDigiBdfPar->GetLandauMpv(iSmType);
            Double_t dSigmScal = fDigiBdfPar->GetLandauSigma(iSmType); // empirical best

            // Get the actual Cluster radius
            dClusterRadius = dStripSize*fRandRadius->Landau( dPeakPos, dSigmScal ) ;
            if( dClusterRadius < 0 )
               dClusterRadius = 0;
         } // if( 0 == iChType)
            else
            {
               LOG(ERROR)<<"CbmTofDigitizerBDF::GenerateClusterRadius => Cluster Radius "
                     <<"obtention from cluster size not implemented for pads, Sm type "
                     <<iSmType<<" Rpc "<<iRpc<<FairLogger::endl;
               LOG(ERROR)<<"CbmTofDigitizerBDF::DigitizeFlatDisc => Test 2"
                     <<" Sm type "<<iSmType<<" Rpc "<<iRpc
                     <<" Type "<<iChType<<FairLogger::endl;
               return kFALSE;
            } // else of if( 0 == iChType)
         break;
      } // case 1:
      default:
      {
         LOG(ERROR)<<"CbmTofDigitizerBDF::GenerateClusterRadius => Wrong Cluster Radius method , Sm type "
                     <<iSmType<<" Rpc "<<iRpc<<FairLogger::endl;
         dClusterRadius = 0;
         break;
      } // default:
   } // switch( fDigiBdfPar->GetClusterRadiusModel() )
   return dClusterRadius;
}
/************************************************************************************/
// Functions for a direct use of the cluster size
Bool_t   CbmTofDigitizerBDF::DigitizeDirectClusterSize()
{
   // Uniform distribution in ]0;x]
   // gRandom->Uniform(x);
   // Gauss distribution in of mean m, sigma s
   // gRandom->Gauss(m, s);

   CbmTofPoint *pPoint;
   CbmMCTrack  *pMcTrk;

   Int_t nTofPoint = fTofPointsColl->GetEntries();
   Int_t nMcTracks = fMcTracksColl ->GetEntries();

   // Debug Info printout
   Int_t iNbTofTracks     = 0;
   Int_t iNbTofTracksPrim = 0;

   LOG(DEBUG) << "CbmTofDigitizerBDF::DigitizeDirectClusterSize: " << nTofPoint
              << " points in Tof for this event with " << nMcTracks
              << " MC tracks "<< FairLogger::endl;

   // Prepare the temporary storing of the Track/Point/Digi info
   if( kTRUE == fDigiBdfPar->UseOneGapPerTrk() )
      fvlTrckChAddr.resize( nMcTracks );
   for(Int_t iTrkInd = 0; iTrkInd < nMcTracks; iTrkInd++)
   {
      if( kTRUE == fDigiBdfPar->UseOneGapPerTrk() )
         fvlTrckChAddr[iTrkInd].clear();
      pMcTrk = (CbmMCTrack*) fMcTracksColl->At( iTrkInd );
      if( 0 < pMcTrk->GetNPoints(kTOF) )
         iNbTofTracks++;
      if( 0 < pMcTrk->GetNPoints(kTOF) && -1 == pMcTrk->GetMotherId() )
         iNbTofTracksPrim++;
   } // for(Int_t iTrkInd = 0; iTrkInd < nMcTracks; iTrkInd++)

   //Some numbers on TOF distributions
   LOG(DEBUG) << "CbmTofDigitizerBDF::DigitizeDirectClusterSize: " << iNbTofTracks
              << " tracks in Tof " << FairLogger::endl;
   LOG(DEBUG) << "CbmTofDigitizerBDF::DigitizeDirectClusterSize: " << iNbTofTracksPrim
              << " tracks in Tof from vertex" << FairLogger::endl;

   // Tof Point Info
   Int_t    iDetId, iSmType, iSM, iRpc, iChannel, iGap;
   Int_t    iTrackID, iChanId;
   TVector3 vPntPos;
   // Cluster Info
   Int_t    iClusterSize;
   Double_t dClustCharge;
   // Digi
//   CbmTofDigi * pTofDigi;    // <- Compressed digi (64 bits)
//   CbmTofDigi * pTofDigiExp; // <- Expanded digi
   // General geometry info
   Int_t    iNbSmTypes = fDigiBdfPar->GetNbSmTypes();
   Int_t    iNbSm, iNbRpc, iNbCh;
   Int_t    iChType;

   // Start jitter: Same contrib. for all points in same event
   Double_t dStartJitter = fRandStart->Gaus( 0.0, fDigiBdfPar->GetStartTimeRes() );

   for (Int_t iPntInd = 0; iPntInd < nTofPoint; iPntInd++ )
   {
      // Get a pointer to the TOF point
      pPoint = (CbmTofPoint*) fTofPointsColl->At( iPntInd );
      if( NULL == pPoint )
      {
         LOG(WARNING)<<"CbmTofDigitizerBDF::DigitizeDirectClusterSize => Be careful: hole in the CbmTofPoint TClonesArray!"
                     <<FairLogger::endl;
         continue;
      } // if( pPoint )

      // Get all channel info
      iDetId  = pPoint->GetDetectorID();
      iSmType = fGeoHandler->GetSMType(iDetId);
      iRpc     = fGeoHandler->GetCounter(iDetId);

      iChannel = fGeoHandler->GetCell(iDetId);
      iChannel --; // Again, channel going from 1 to nbCh instead of 0 to nbCh - 1 ?!?!?
      iGap     = fGeoHandler->GetGap(iDetId);
      iSM      = fGeoHandler->GetSModule(iDetId);
      iChanId  = fGeoHandler->GetCellId(iDetId);
      iTrackID = pPoint->GetTrackID();

      iNbSm   = fDigiBdfPar->GetNbSm(  iSmType);
      iNbRpc  = fDigiBdfPar->GetNbRpc( iSmType);
      iNbCh   = fDigiBdfPar->GetNbChan( iSmType, iRpc );
      iChType = fDigiBdfPar->GetChanType( iSmType, iRpc );

      // Get pointer to the MC-Track info
      Int_t iTrkId = pPoint->GetTrackID();
      pMcTrk = (CbmMCTrack*) fMcTracksColl->At( iTrkId );

      // Keep only primaries
      if( kTRUE == fDigiBdfPar->UseOnlyPrimaries() )
         if( -1 != pMcTrk->GetMotherId() )
            continue;

      if(   iSmType  < 0. || iNbSmTypes <= iSmType
         || iSM      < 0. || iNbSm      <= iSM
         || iRpc     < 0. || iNbRpc     <= iRpc
         || iChannel < 0. || iNbCh      <= iChannel )
      {
	 LOG(ERROR)<<Form("CbmTofDigitizerBDF => det ID 0x%08x",iDetId) <<" SMType: "<<iSmType;
         LOG(ERROR)<<" SModule: "<<iSM<<" of "<<iNbSm+1;
         LOG(ERROR)<<" Module: "<<iRpc<<" of "<<iNbRpc+1;
         LOG(ERROR)<<" Gap: "<<iGap;
         LOG(FATAL)<<" Cell: "<<iChannel<<" of "<<iNbCh+1 <<FairLogger::endl;
         continue;
      } // if error on channel data

      // Check if there was already a Digi from the same track created
      // with this channel as main channel
      ULong64_t uAddr = CbmTofAddress::GetUniqueAddress( iSM, iRpc, iChannel, 0, iSmType);
      if( kTRUE == fDigiBdfPar->UseOneGapPerTrk() )
      {
         Bool_t bFoundIt = kFALSE;
         for( Int_t iTrkMainCh = 0; iTrkMainCh < fvlTrckChAddr[iTrkId].size(); iTrkMainCh ++)
            if( uAddr == fvlTrckChAddr[iTrkId][iTrkMainCh])
            {
               bFoundIt = kTRUE;
               break;
            }
         // If it is the case, no need to redo the full stuff for this gap
         if( kTRUE == bFoundIt)
            continue;
      } // if( kTRUE == fDigiBdfPar->UseOneGapPerTrk() )

      // Probability that the RPC detect the hit
//      if( fDigiBdfPar->GetEfficiency(iSmType) < gRandom->Uniform(1) )
//         continue;

      // Probability that the gap detect the hit
      // For now use a simple gap treatment (cf CbmTofDigiBdfPar):
      // - a single fired gap fires the channel
      // - all gaps have exactly equivalent efficiency/firing probability
      // - independent gap firing (no charge accumulation or such)
      // The probability for a hit not to fire at all is then
      //    (1-p)^NGaps with p = gap efficiency and Ngaps the number of gaps in this RPC
      // This results in the relation: p = 1 - (1 - P)^(1/Ngaps)
      //         with P = RPC efficiency from beamtime
      if( fDigiBdfPar->GetGapEfficiency(iSmType, iRpc) < fRandEff->Uniform(1) )
         continue;

      // Save the address in vector so that this channel is not redone later for the
      // eventual other gaps TofPoint
      if( kTRUE == fDigiBdfPar->UseOneGapPerTrk() )
         fvlTrckChAddr[iTrkId].push_back(uAddr);

      // Get TofPoint Position
      pPoint->Position( vPntPos );
      fChannelInfo = fDigiPar->GetCell(iChanId);

      if( 1 == iChType)
      {
         LOG(ERROR)<<"CbmTofDigitizerBDF::DigitizeDirectClusterSize => This method "
               <<"is not available for pads!!"<<FairLogger::endl;
         return kFALSE;
      } // if( 1 == iChType)

      // Cluster size from beamtime distribution in [strips]
      iClusterSize = fh1ClusterSizeProb[iSmType]->GetBinContent(
                        fh1ClusterSizeProb[iSmType]->FindBin(
                           fRandRadius->Uniform(1) ) );

      Int_t iNbStripsSideA;
      Int_t iNbStripsSideB;
      if( 1 == iClusterSize%2 )
      {
         // odd => a center strip and symetric side strips
         iNbStripsSideA = (iClusterSize - 1)/2;
         iNbStripsSideB = (iClusterSize - 1)/2;
      } // if odd cluster size
         else
         {
            // even => asymetric, the side getting more strips depends
            // on the cluster center position
            iNbStripsSideA = iClusterSize/2; // left/bottom
            iNbStripsSideB = iClusterSize/2; // right/top

            if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
            {
               // Horizontal strips
               if( vPntPos.Y() - fChannelInfo->GetY() < fChannelInfo->GetSizey()/2.0 )
                  iNbStripsSideB --; // less strips on top
               else iNbStripsSideA --; // less strips on bottom
            } // if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
            else if( vPntPos.X() - fChannelInfo->GetX() < fChannelInfo->GetSizex()/2.0 )
               iNbStripsSideB --; // less strips on right
            else iNbStripsSideA --; // less strips on left
         } // if even cluster size

      // Generate Cluster charge as ToT[ps]
      dClustCharge = fh1ClusterTotProb[iSmType]->GetBinContent(
                        fh1ClusterTotProb[iSmType]->FindBin(
                           fRandCharge->Uniform(1) ) );

      // Calculate the time for the central channel
      Double_t dCentralTime =  pPoint->GetTime()
                             + fRandRes->Gaus( 0.0, fDigiBdfPar->GetResolution(iSmType ) )
                             + dStartJitter; // Same contrib. for all points in same event

      if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
      {
         // Horizontal channel: A = Right and B = Left of the channel

         // Assume the width (sigma) of the gaussian is 0.5*dClusterSize/3
         // => 99% of the charge is in "dClusterSize"
         TF1 * f1ChargeGauss = new TF1( "f1ChargeDist", "[0]*TMath::Gaus(x,[1],[2])",
                                          vPntPos.Y() - 2*iClusterSize,
                                          vPntPos.Y() + 2*iClusterSize);
//         TofChargeDistributions * fptr = new TofChargeDistributions();
//         TF1 * f1ChargeGauss = new TF1( "f1ChargeDist", fptr,
//                                          &TofChargeDistributions::Gauss1D,
//                                          vPntPos.Y() - 2*iClusterSize,
//                                          vPntPos.Y() + 2*iClusterSize,
//                                          3
//                                          );
         f1ChargeGauss->SetParameters( dClustCharge/( TMath::Sqrt( 2.0*TMath::Pi()*iClusterSize/6.0 )),
                                       vPntPos.Y(), 0.5*iClusterSize/3.0);

         double *x=new double[kiNbIntPts];
         double *w=new double[kiNbIntPts];
         f1ChargeGauss->CalcGaussLegendreSamplingPoints( kiNbIntPts, x, w, 1e-12);
         // Loop over strips, get the share of the charge each get, compute
         // the times it measure and create the digis
         for( Int_t iStripInd = iChannel - iNbStripsSideA;
               iStripInd <= iChannel + iNbStripsSideB;
               iStripInd++ )
            if( 0 <= iStripInd && iStripInd < iNbCh )
            {
               CbmTofDetectorInfo xDetInfo(kTOF, iSmType, iSM, iRpc, 0, iStripInd + 1);
               Int_t iSideChId = fTofId->SetDetectorInfo( xDetInfo );
               fChannelInfo = fDigiPar->GetCell( iSideChId );

               Double_t dStripCharge = f1ChargeGauss->IntegralFast( kiNbIntPts, x, w,
                                          fChannelInfo->GetY() - fChannelInfo->GetSizey()/2.0,
                                          fChannelInfo->GetY() + fChannelInfo->GetSizey()/2.0);

               // Fee Threshold on charge, each side gets half, each side has a gain
               if( fDigiBdfPar->GetFeeThreshold() <
                     dStripCharge*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][2*iStripInd+1]/2.0 )
               {
                  Double_t dTimeA = dCentralTime;
                  dTimeA +=  fRandRes->Gaus( 0.0, fdTimeResElec)
#ifdef FULL_PROPAGATION_TIME
                           + TMath::Abs( vPntPos.X() - ( fChannelInfo->GetX() + fChannelInfo->GetSizex()/2.0 ) )
#else
                           + ( vPntPos.X() - fChannelInfo->GetX() )
#endif
                             /fdSignalPropSpeed;
                  // Switch between Digi and DigiExp
                  if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
                  {
                     CbmTofDigiExp * tofDigi = new CbmTofDigiExp( iSM, iRpc, iChannel, dTimeA,
                           dStripCharge*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][2*iChannel+1]/2.0,
                           1, iSmType );
		     //					 tofDigi->GetMatch()->AddLink(1., iPntInd);
                     fStorDigiExp[iSmType][iSM*iNbRpc + iRpc][2*iChannel+1].push_back( tofDigi );
		     fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iChannel+1].push_back( iPntInd );

		     LOG(INFO)<<Form("Digimatch (%d,%d,%d,%d): size %zu, val %d, MCt %d",
				iSmType,iSM,iRpc,iChannel,fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iChannel+1].size(),iPntInd,iTrackID)
			      <<FairLogger::endl;
                  } // if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
                  else {
                     CbmTofDigi * tofDigi = new CbmTofDigi( iSM, iRpc, iChannel, dTimeA,
                           dStripCharge*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][2*iChannel+1]/2.0,
                           1, iSmType );
		     //					 tofDigi->GetMatch()->AddLink(1., iPntInd);
                     fStorDigi[iSmType][iSM*iNbRpc + iRpc][2*iChannel+1].push_back( tofDigi );
		     fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iChannel+1].push_back( iPntInd );
                  } // else of if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
               } // if Side A above threshold
               if( fDigiBdfPar->GetFeeThreshold() <
                     dStripCharge*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][2*iStripInd]/2.0 )
               {
                  Double_t dTimeB = dCentralTime;
                  dTimeB +=  fRandRes->Gaus( 0.0, fdTimeResElec)
#ifdef FULL_PROPAGATION_TIME
                           + TMath::Abs( vPntPos.X() - ( fChannelInfo->GetX() - fChannelInfo->GetSizex()/2.0 ) )
#else
                           - ( vPntPos.X() - fChannelInfo->GetX() )
#endif
                             /fdSignalPropSpeed;
                  // Switch between Digi and DigiExp
                  if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
                  {
                     CbmTofDigiExp * tofDigi = new CbmTofDigiExp( iSM, iRpc, iChannel, dTimeB,
                           dStripCharge*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][2*iChannel]/2.0,
                           0, iSmType );
		     //					 tofDigi->GetMatch()->AddLink(1., iPntInd);
                     fStorDigiExp[iSmType][iSM*iNbRpc + iRpc][2*iChannel].push_back( tofDigi );
		     fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iChannel].push_back( iPntInd );
		     LOG(DEBUG)<<Form("Digimatch (%d,%d,%d,%d): size %zu, val %d, MCt %d",
				     iSmType,iSM,iRpc,iChannel,fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iChannel+1].size(),iPntInd,iTrackID)
			      <<FairLogger::endl;                  } // if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
                  else {
                     CbmTofDigi * tofDigi = new CbmTofDigi( iSM, iRpc, iChannel, dTimeB,
                           dStripCharge*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][2*iChannel]/2.0,
                           0, iSmType );
		     //					 tofDigi->GetMatch()->AddLink(1., iPntInd);
                     fStorDigi[iSmType][iSM*iNbRpc + iRpc][2*iChannel].push_back( tofDigi );
		     fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iChannel].push_back( iPntInd );
                  } // else of if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
               } // if Side B above threshold
            } // if( 0 <= iStripInd && iStripInd < iNbCh )
         delete f1ChargeGauss;
//         delete []x;
//         delete []w;
      } // if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
      else
      {
         // Vertical channel: A = Top and B = Bottom of the channel

         // Assume the width (sigma) of the gaussian is 0.5*dClusterSize/3
         // => 99% of the charge is in "dClusterSize"
         TF1 * f1ChargeGauss = new TF1( "f1ChargeDist", "[0]*TMath::Gaus(x,[1],[2])",
                                          vPntPos.X() - 2*iClusterSize,
                                          vPntPos.X() + 2*iClusterSize);

//         TofChargeDistributions * fptr = new TofChargeDistributions();
//         TF1 * f1ChargeGauss = new TF1( "f1ChargeDist", fptr,
//                                          &TofChargeDistributions::Gauss1D,
//                                          vPntPos.Y() - 2*iClusterSize,
//                                          vPntPos.Y() + 2*iClusterSize,
//                                          3
//                                          );
         f1ChargeGauss->SetParameters( dClustCharge/( TMath::Sqrt( 2.0*TMath::Pi() )*iClusterSize/6.0 ),
                                       vPntPos.X(), 0.5*iClusterSize/3.0);

         double *x=new double[kiNbIntPts];
         double *w=new double[kiNbIntPts];
         f1ChargeGauss->CalcGaussLegendreSamplingPoints( kiNbIntPts, x, w, 1e-12);
         // Loop over strips, get the share of the charge each get, compute
         // the times it measure and create the digis
         for( Int_t iStripInd = iChannel - iNbStripsSideA;
               iStripInd <= iChannel + iNbStripsSideB;
               iStripInd++ )
            if( 0 <= iStripInd && iStripInd < iNbCh )
            {
               CbmTofDetectorInfo xDetInfo(kTOF, iSmType, iSM, iRpc, 0, iStripInd + 1);
               Int_t iSideChId = fTofId->SetDetectorInfo( xDetInfo );
               fChannelInfo = fDigiPar->GetCell( iSideChId );

               Double_t dStripCharge = f1ChargeGauss->IntegralFast( kiNbIntPts, x, w,
                                          fChannelInfo->GetX() - fChannelInfo->GetSizex()/2.0,
                                          fChannelInfo->GetX() + fChannelInfo->GetSizex()/2.0);

               // Fee Threshold on charge, each side gets half, each side has a gain
               if( fDigiBdfPar->GetFeeThreshold() <
                     dStripCharge*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][2*iStripInd+1]/2.0 )
               {
                  Double_t dTimeA = dCentralTime;
                  dTimeA +=  fRandRes->Gaus( 0.0, fdTimeResElec)
#ifdef FULL_PROPAGATION_TIME
                           + TMath::Abs( vPntPos.Y() - ( fChannelInfo->GetY() + fChannelInfo->GetSizey()/2.0 ) )
#else
                           + ( vPntPos.Y() - fChannelInfo->GetY() )
#endif
                             /fdSignalPropSpeed;
                  // Switch between Digi and DigiExp
                  if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
                  {
                     CbmTofDigiExp * tofDigi = new CbmTofDigiExp( iSM, iRpc, iStripInd, dTimeA,
                           dStripCharge*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][2*iStripInd+1]/2.0,
                           1, iSmType );
		     //					 tofDigi->GetMatch()->AddLink(1., iPntInd);
                     fStorDigiExp[iSmType][iSM*iNbRpc + iRpc][2*iStripInd+1].push_back( tofDigi );
		     fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iStripInd+1].push_back( iPntInd );
		     LOG(DEBUG)<<Form("Digimatch (%d,%d,%d,%d): size %zu, val %d, MCt %d",
				     iSmType,iSM,iRpc,iChannel,fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iChannel+1].size(),iPntInd,iTrackID)
			      <<FairLogger::endl;
                  } // if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
                  else {
                     CbmTofDigi * tofDigi = new CbmTofDigi( iSM, iRpc, iStripInd, dTimeA,
                           dStripCharge*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][2*iStripInd+1]/2.0,
                           1, iSmType );
		     //					 tofDigi->GetMatch()->AddLink(1., iPntInd);
                     fStorDigi[iSmType][iSM*iNbRpc + iRpc][2*iStripInd+1].push_back( tofDigi );
		     fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iStripInd+1].push_back( iPntInd );
                  } // else of if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
               } // if Side A above threshold
               if( fDigiBdfPar->GetFeeThreshold() <
                     dStripCharge*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][2*iStripInd]/2.0 )
               {
                  Double_t dTimeB = dCentralTime;
                  dTimeB +=  fRandRes->Gaus( 0.0, fdTimeResElec)
#ifdef FULL_PROPAGATION_TIME
                           + TMath::Abs( vPntPos.Y() - ( fChannelInfo->GetY() - fChannelInfo->GetSizey()/2.0 ) )
#else
                           - ( vPntPos.Y() - fChannelInfo->GetY() )
#endif
                             /fdSignalPropSpeed;
                  // Switch between Digi and DigiExp
                  if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
                  {
                     CbmTofDigiExp * tofDigi = new CbmTofDigiExp( iSM, iRpc, iStripInd, dTimeB,
                           dStripCharge*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][2*iStripInd]/2.0,
                           0, iSmType );
		     //					 tofDigi->GetMatch()->AddLink(1., iPntInd);
                     fStorDigiExp[iSmType][iSM*iNbRpc + iRpc][2*iStripInd].push_back( tofDigi );
		     fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iStripInd].push_back( iPntInd );
		     LOG(DEBUG)<<Form("Digimatch (%d,%d,%d,%d): size %zu, val %d, MCt %d",
				     iSmType,iSM,iRpc,iChannel,fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iChannel+1].size(),iPntInd,iTrackID)
			      <<FairLogger::endl;
                  } // if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
                  else {
                     CbmTofDigi * tofDigi = new CbmTofDigi( iSM, iRpc, iStripInd, dTimeB,
                           dStripCharge*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][2*iStripInd]/2.0,
                           0, iSmType );
		     //					 tofDigi->GetMatch()->AddLink(1., iPntInd);
                     fStorDigi[iSmType][iSM*iNbRpc + iRpc][2*iStripInd].push_back( tofDigi );
		     fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iStripInd].push_back( iPntInd );
                  } // else of if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
               } // if Side B above threshold
            } // if( 0 <= iStripInd && iStripInd < iNbCh )
         delete f1ChargeGauss;
         delete []x;
         delete []w;
      } // else of if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
   } // for (Int_t iPntInd = 0; iPntInd < nTofPoint; iPntInd++ )

   // Clear the Track to channel temporary storage
   if( kTRUE == fDigiBdfPar->UseOneGapPerTrk() )
   {
      for(Int_t iTrkInd = 0; iTrkInd < nMcTracks; iTrkInd++)
         fvlTrckChAddr[iTrkInd].clear();
      fvlTrckChAddr.clear();
   } // if( kTRUE == fDigiBdfPar->UseOneGapPerTrk() )

   return kTRUE;
}
/************************************************************************************/
// Functions for a simple "Flat disc" cluster approximation
Bool_t   CbmTofDigitizerBDF::DigitizeFlatDisc()
{
   // Uniform distribution in ]0;x]
   // gRandom->Uniform(x);
   // Gauss distribution in of mean m, sigma s
   // gRandom->Gauss(m, s);

   CbmTofPoint *pPoint;
   CbmMCTrack  *pMcTrk;

   Int_t nTofPoint = fTofPointsColl->GetEntries();
   Int_t nMcTracks = fMcTracksColl ->GetEntries();

   // Debug Info printout
   Int_t iNbTofTracks     = 0;
   Int_t iNbTofTracksPrim = 0;

   LOG(DEBUG) << "CbmTofDigitizerBDF::DigitizeFlatDisc: " << nTofPoint
              << " points in Tof for this event with " << nMcTracks
              << " MC tracks "<< FairLogger::endl;

   // Prepare the temporary storing of the Track/Point/Digi info
   if( kTRUE == fDigiBdfPar->UseOneGapPerTrk() )
      fvlTrckChAddr.resize( nMcTracks );
   for(Int_t iTrkInd = 0; iTrkInd < nMcTracks; iTrkInd++)
   {
      if( kTRUE == fDigiBdfPar->UseOneGapPerTrk() )
         fvlTrckChAddr[iTrkInd].clear();
      pMcTrk = (CbmMCTrack*) fMcTracksColl->At( iTrkInd );
      if( 0 < pMcTrk->GetNPoints(kTOF) )
         iNbTofTracks++;
      if( 0 < pMcTrk->GetNPoints(kTOF) && -1 == pMcTrk->GetMotherId() )
         iNbTofTracksPrim++;
   } // for(Int_t iTrkInd = 0; iTrkInd < nMcTracks; iTrkInd++)

   //Some numbers on TOF distributions
   LOG(DEBUG) << "CbmTofDigitizerBDF::DigitizeFlatDisc: " << iNbTofTracks
              << " tracks in Tof " << FairLogger::endl;
   LOG(DEBUG) << "CbmTofDigitizerBDF::DigitizeFlatDisc: " << iNbTofTracksPrim
              << " tracks in Tof from vertex" << FairLogger::endl;

   // Tof Point Info
   Int_t    iDetId, iSmType, iSM, iRpc, iChannel, iGap;
   Int_t    iTrackID, iChanId;
   TVector3 vPntPos;
   // Cluster Info
   Double_t dClusterSize;
   Double_t dClustCharge;
   // Digi
//   CbmTofDigi * pTofDigi;    // <- Compressed digi (64 bits)
//   CbmTofDigi * pTofDigiExp; // <- Expanded digi
   // General geometry info
   Int_t    iNbSmTypes = fDigiBdfPar->GetNbSmTypes();
   Int_t    iNbSm, iNbRpc, iNbCh;
   Int_t    iChType;

   // Start jitter: Same contrib. for all points in same event
   Double_t dStartJitter = fRandStart->Gaus( 0.0, fDigiBdfPar->GetStartTimeRes() );

   for (Int_t iPntInd = 0; iPntInd < nTofPoint; iPntInd++ )
   {
      // Get a pointer to the TOF point
      pPoint = (CbmTofPoint*) fTofPointsColl->At( iPntInd );
      if( NULL == pPoint )
      {
         LOG(WARNING)<<"CbmTofDigitizerBDF::DigitizeFlatDisc => Be careful: hole in the CbmTofPoint TClonesArray!"<<endl;
         continue;
      } // if( pPoint )

      // Get all channel info
      iDetId  = pPoint->GetDetectorID();
      iSmType = fGeoHandler->GetSMType(iDetId);
      iRpc     = fGeoHandler->GetCounter(iDetId);
      iChannel = fGeoHandler->GetCell(iDetId);
      if(fGeoHandler->GetGeoVersion() < k14a)
      iChannel --; // Again, channel going from 1 to nbCh instead of 0 to nbCh - 1 ?!?!?
      iGap     = fGeoHandler->GetGap(iDetId);
      iSM      = fGeoHandler->GetSModule(iDetId);
      iChanId  = fGeoHandler->GetCellId(iDetId);
      iTrackID = pPoint->GetTrackID();

      iNbSm   = fDigiBdfPar->GetNbSm(  iSmType);
      iNbRpc  = fDigiBdfPar->GetNbRpc( iSmType);
      iNbCh   = fDigiBdfPar->GetNbChan( iSmType, iRpc );
      iChType = fDigiBdfPar->GetChanType( iSmType, iRpc );

      // Get pointer to the MC-Track info
      Int_t iTrkId = pPoint->GetTrackID();
      pMcTrk = (CbmMCTrack*) fMcTracksColl->At( iTrkId );

      // Jump all tracks not making 8 points for test
//      if( 8 != pMcTrk->GetNPoints(kTOF) )
//         continue;

      // Keep only primaries
      if( kTRUE == fDigiBdfPar->UseOnlyPrimaries() )
         if( -1 != pMcTrk->GetMotherId() )
            continue;

      if(   iSmType  < 0. || iNbSmTypes <= iSmType
         || iSM      < 0. || iNbSm      <= iSM
         || iRpc     < 0. || iNbRpc     <= iRpc
         || iChannel < 0. || iNbCh      <= iChannel )
      {
	 LOG(ERROR)<<Form("CbmTofDigitizerBDF => det ID 0x%08x",iDetId) 
		   <<", GeoVersion "<< fGeoHandler->GetGeoVersion()<<", SMType: "<<iSmType;
         LOG(ERROR)<<" SModule: "<<iSM<<" of "<<iNbSm+1;
         LOG(ERROR)<<" Module: "<<iRpc<<" of "<<iNbRpc+1;
         LOG(ERROR)<<" Gap: "<<iGap;
         LOG(FATAL)<<" Cell: "<<iChannel<<" of "<<iNbCh+1 <<FairLogger::endl;
         continue;
      } // if error on channel data
      LOG(DEBUG2)<<"CbmTofDigitizerBDF::DigitizeFlatDisc => Check Point "<<iPntInd
            <<" Sm type "<<iSmType<<" SM "<<iSM<<" Rpc "<<iRpc<<" Channel "<<iChannel
            <<" Type "<<iChType<<" Orient. "<<fDigiBdfPar->GetChanOrient( iSmType, iRpc )<<FairLogger::endl;

      // Check if there was already a Digi from the same track created
      // with this channel as main channel
      ULong64_t uAddr = CbmTofAddress::GetUniqueAddress( iSM, iRpc, iChannel, 0, iSmType);
      if( kTRUE == fDigiBdfPar->UseOneGapPerTrk() )
      {
         Bool_t bFoundIt = kFALSE;
         for( Int_t iTrkMainCh = 0; iTrkMainCh < fvlTrckChAddr[iTrkId].size(); iTrkMainCh ++)
            if( uAddr == fvlTrckChAddr[iTrkId][iTrkMainCh])
            {
               bFoundIt = kTRUE;
               break;
            }
         // If it is the case, no need to redo the full stuff for this gap
         if( kTRUE == bFoundIt)
            continue;
      } // if( kTRUE == fDigiBdfPar->UseOneGapPerTrk() )

      // Probability that the RPC detect the hit
//      if( fDigiBdfPar->GetEfficiency(iSmType) < gRandom->Uniform(1) )
//         continue;

      // Probability that the gap detect the hit
      // For now use a simple gap treatment (cf CbmTofDigiBdfPar):
      // - a single fired gap fires the channel
      // - all gaps have exactly equivalent efficiency/firing probability
      // - independent gap firing (no charge accumulation or such)
      // The probability for a hit not to fire at all is then
      //    (1-p)^NGaps with p = gap efficiency and Ngaps the number of gaps in this RPC
      // This results in the relation: p = 1 - (1 - P)^(1/Ngaps)
      //         with P = RPC efficiency from beamtime
      LOG(DEBUG2)<<"CbmTofDigitizerBDF::DigitizeFlatDisc => Eff = "
		 <<fDigiBdfPar->GetGapEfficiency(iSmType, iRpc) 
		 <<FairLogger::endl;

      if( fDigiBdfPar->GetGapEfficiency(iSmType, iRpc) < fRandEff->Uniform(1) )
         continue;

      // Save the address in vector so that this channel is not redone later for the
      // eventual other gaps TofPoint
      if( kTRUE == fDigiBdfPar->UseOneGapPerTrk() )
         fvlTrckChAddr[iTrkId].push_back(uAddr);

      // Get TofPoint Position
      pPoint->Position( vPntPos );
      fChannelInfo = fDigiPar->GetCell(iChanId);

      // Generate a Cluster radius in [cm]
      dClusterSize = GenerateClusterRadius( iSmType, iRpc );
      // Cut on the higher radius because otherwise the big clusters at the edge
      // of the counter generate hits totally off (because the main strip do not get
      // more charge than the side strips anymore). Cut value fully empirical.
      while( dClusterSize < 0.0001 ||
              3.0 < dClusterSize )
         // Should not happen without error message
         // But Landau can give really small values
         dClusterSize = GenerateClusterRadius( iSmType, iRpc );

      // Generate Cluster charge as ToT[ps]
      dClustCharge = fh1ClusterTotProb[iSmType]->GetBinContent(
                        fh1ClusterTotProb[iSmType]->FindBin(
                           fRandCharge->Uniform(1) ) );

      //Total cluster area (used to calculate the charge fraction in each channel)
      Double_t dClustArea = dClusterSize * dClusterSize * TMath::Pi();

      // Calculate the fraction of the charge in central channel
      Double_t dChargeCentral = dClustCharge * ComputeClusterAreaOnChannel(
                                    iChanId, dClusterSize, vPntPos.X(), vPntPos.Y());
      LOG(DEBUG2)<<"CbmTofDigitizerBDF::DigitizeFlatDisc: ChargeCentral "<<dChargeCentral
		 <<", "<<dClustCharge
		 <<", "<<iChanId<<", "<<dClusterSize<<", "<<vPntPos.X()<<", "<<vPntPos.Y()<<", "<<vPntPos.Z()
		 <<FairLogger::endl;

      dChargeCentral /= dClustArea;
      if( dClustCharge +0.0000001 < dChargeCentral )
      {
         LOG(ERROR)<<"CbmTofDigitizerBDF::DigitizeFlatDisc => Central Charge "
                   <<dChargeCentral<<" "<<dClustCharge<<" "<<dClustCharge-dChargeCentral
                   <<" "<<(ComputeClusterAreaOnChannel(iChanId, dClusterSize, vPntPos.X(), vPntPos.Y()))<<" "<<dClustArea
                   <<FairLogger::endl;
         LOG(ERROR)<<"CbmTofDigitizerBDF::DigitizeFlatDisc => Check Point "<<iPntInd
                     <<" Sm type "<<iSmType<<" SM "<<iSM<<" Rpc "<<iRpc<<" Channel "<<iChannel
                     <<" Type "<<iChType<<" Orient. "<<fDigiBdfPar->GetChanOrient( iSmType, iRpc )<<FairLogger::endl;
      } // if( dClustCharge +0.0000001 < dChargeCentral )

      // Calculate the time for the central channel
      Double_t dCentralTime =  pPoint->GetTime()
                             + fRandRes->Gaus( 0.0, fDigiBdfPar->GetResolution(iSmType ) )
                             + dStartJitter; // Same contrib. for all points in same event

      // FIXME: not sure if this limit does not destroy rate estimates and such
//      if( 1e6 < dCentralTime )
//         continue;

      // Calculate propagation time(s) to the readout point(s)
      if( 0 == iChType)
      {
         // Strips (readout on both side)
         // Assume that the bottom/left strip have lower channel index
         // E.g:  ... | i | i+1 | ...
         //     or      ...         y
         //            -----        ^
         //             i+1         |
         //            -----         --> x
         //              i
         //            -----
         //             ...

         Double_t dTimeA = dCentralTime;
         Double_t dTimeB = dCentralTime;
         if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
         {
            // Horizontal channel: A = Right and B = Left of the channel
            dTimeA +=  fRandRes->Gaus( 0.0, fdTimeResElec)
#ifdef FULL_PROPAGATION_TIME
                     + TMath::Abs( vPntPos.X() - ( fChannelInfo->GetX() + fChannelInfo->GetSizex()/2.0 ) )
#else
                     + ( vPntPos.X() - fChannelInfo->GetX() )
#endif
                       /fdSignalPropSpeed;
            dTimeB +=  fRandRes->Gaus( 0.0, fdTimeResElec)
#ifdef FULL_PROPAGATION_TIME
                     + TMath::Abs( vPntPos.X() - ( fChannelInfo->GetX() - fChannelInfo->GetSizex()/2.0 ) )
#else
                     - ( vPntPos.X() - fChannelInfo->GetX() )
#endif
                       /fdSignalPropSpeed;
         } // if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
            else
            {

	      TGeoNode *fNode=        // prepare global->local trafo
			gGeoManager->FindNode(fChannelInfo->GetX(),fChannelInfo->GetY(),fChannelInfo->GetZ());
	      LOG(DEBUG)<<Form(" TofDigitizerBDF:: (%3d,%3d,%3d,%3d) - node at (%6.1f,%6.1f,%6.1f) : 0x%p",
			      iSmType,iSM,iRpc,iChannel,
			       fChannelInfo->GetX(),fChannelInfo->GetY(),fChannelInfo->GetZ(),fNode, 
			       vPntPos.X(), vPntPos.Y(), vPntPos.Z())
			<<FairLogger::endl;
	      TGeoNode*	cNode= gGeoManager->GetCurrentNode();
	      TGeoHMatrix* cMatrix = gGeoManager->GetCurrentMatrix();
	      Double_t hitpos[3]={fChannelInfo->GetX(),vPntPos.Y(),fChannelInfo->GetZ()};
	      Double_t hitpos_local[3];
	      gGeoManager->MasterToLocal(hitpos, hitpos_local);
               // Vertical channel: A = Top and B = Bottom of the channel
               dTimeA +=  fRandRes->Gaus( 0.0, fdTimeResElec)
#ifdef FULL_PROPAGATION_TIME
                        + TMath::Abs( vPntPos.Y() - ( fChannelInfo->GetY() + fChannelInfo->GetSizey()/2.0 ) )
#else
		 //                        + ( vPntPos.Y() - fChannelInfo->GetY() )
                        + hitpos_local[1]
#endif
                          /fdSignalPropSpeed;
               dTimeB +=  fRandRes->Gaus( 0.0, fdTimeResElec)
#ifdef FULL_PROPAGATION_TIME
                        + TMath::Abs( vPntPos.Y() - ( fChannelInfo->GetY() - fChannelInfo->GetSizey()/2.0 ) )
#else
		 //                        - ( vPntPos.Y() - fChannelInfo->GetY() )
                        - hitpos_local[1]
#endif
                          /fdSignalPropSpeed;
            } // else of if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
	 LOG(DEBUG)<<Form(" TofDigitizerBDF:: chrg %7.1f, gain %7.1f, thr %7.1f ",dChargeCentral,
			  fdChannelGain[iSmType][iSM*iNbRpc + iRpc][2*iChannel+1],fDigiBdfPar->GetFeeThreshold())
		   <<FairLogger::endl;
         // Switch between Digi and DigiExp
         if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
         {
            if( fDigiBdfPar->GetFeeThreshold() <=
                  dChargeCentral*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][2*iChannel+1]/2.0 )
            {
               CbmTofDigiExp * tofDigi = new CbmTofDigiExp( iSM, iRpc, iChannel, dTimeA,
                     dChargeCentral*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][2*iChannel+1]/2.0,
                     1, iSmType );
	       //nh,crashes  
	       //	       tofDigi->GetMatch()->AddLink(1., iPntInd);
               fStorDigiExp[iSmType][iSM*iNbRpc + iRpc][2*iChannel+1].push_back( tofDigi );
	       fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iChannel+1].push_back( iPntInd );
		     LOG(DEBUG)<<Form("Digimatch (%d,%d,%d,%d): size %zu, val %d, MCt %d",
				     iSmType,iSM,iRpc,iChannel,fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iChannel+1].size(),iPntInd,iTrackID)
			      <<FairLogger::endl;

            } // charge ok
            if( fDigiBdfPar->GetFeeThreshold() <=
                  dChargeCentral*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][2*iChannel]/2.0 )
            {
               CbmTofDigiExp * tofDigi = new CbmTofDigiExp( iSM, iRpc, iChannel, dTimeB,
                     dChargeCentral*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][2*iChannel]/2.0,
                     0, iSmType );
	       //			   tofDigi->GetMatch()->AddLink(1., iPntInd);
               fStorDigiExp[iSmType][iSM*iNbRpc + iRpc][2*iChannel].push_back( tofDigi );
	       fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iChannel].push_back( iPntInd );
            } // charge ok
         } // if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
         else {
            if( fDigiBdfPar->GetFeeThreshold() <=
                  dChargeCentral*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][2*iChannel+1]/2.0 )
            {
               CbmTofDigi * tofDigi = new CbmTofDigi( iSM, iRpc, iChannel, dTimeA,
                     dChargeCentral*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][2*iChannel+1]/2.0,
                     1, iSmType );
	       //			   tofDigi->GetMatch()->AddLink(1., iPntInd);
               fStorDigi[iSmType][iSM*iNbRpc + iRpc][2*iChannel+1].push_back( tofDigi );
	       fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iChannel+1].push_back( iPntInd );
		     LOG(DEBUG)<<Form("Digimatch (%d,%d,%d,%d): size %zu, val %d, MCt %d",
				     iSmType,iSM,iRpc,iChannel,fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iChannel+1].size(),iPntInd,iTrackID)
			      <<FairLogger::endl;
            } // charge ok
            if( fDigiBdfPar->GetFeeThreshold() <=
                  dChargeCentral*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][2*iChannel]/2.0 )
            {
               CbmTofDigi * tofDigi = new CbmTofDigi( iSM, iRpc, iChannel, dTimeB,
                     dChargeCentral*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][2*iChannel]/2.0,
                     0, iSmType );
	       //			   tofDigi->GetMatch()->AddLink(1., iPntInd);
               fStorDigi[iSmType][iSM*iNbRpc + iRpc][2*iChannel].push_back( tofDigi );
	       fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iChannel].push_back( iPntInd );
            } // charge ok
         } // else of if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
      } // if( 0 == iChType)
         else
         {
            // Pad (Single side readout)
            // Assume that the bottom/left pads have lower channel index
            // E.g: for a 2*4 pads counter, pads 0-3 are the bottom/left ones and 4-7 the
            // top/right one with reversed numbering:
            //               -----------------
            //  row 1        | 7 | 6 | 5 | 4 |          y
            //               -----------------          ^
            //  row 0        | 0 | 1 | 2 | 3 |          |
            //               -----------------           --> x
            // or            ---------
            //               | 4 | 3 |
            //               ---------
            //               | 5 | 2 |
            //               ---------
            //               | 6 | 1 |
            //               ---------
            //               | 7 | 0 |
            //               ---------
            //        row      1   0
            // Also assume that the readout happens at the middle of the readout edge

            Double_t dClustToReadout = 0.0;
            Double_t dPadTime = dCentralTime;
            // First calculate the propagation time to the center of the pad base
            if( iChannel < iNbCh/2.0 )
            {
               // First row
               if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
                  // Vertical => base = right edge
                  dClustToReadout = TMath::Sqrt(  TMath::Power( vPntPos.Y() - fChannelInfo->GetY() , 2)
                         + TMath::Power( vPntPos.X() - ( fChannelInfo->GetX() + fChannelInfo->GetSizex()/2.0 ), 2) );
                  else  // Horizontal => base = bottom edge
                     dClustToReadout = TMath::Sqrt(  TMath::Power( vPntPos.X() - fChannelInfo->GetX() , 2)
                         + TMath::Power( vPntPos.Y() - ( fChannelInfo->GetY() - fChannelInfo->GetSizey()/2.0 ), 2) );
            } // if( iChannel < iNbCh/2.0 )
               else
               {
                  // Second row
                  if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
                     // Vertical => base = left edge
                     dClustToReadout = TMath::Sqrt(  TMath::Power( vPntPos.Y() - fChannelInfo->GetY() , 2)
                            + TMath::Power( vPntPos.X() - ( fChannelInfo->GetX() - fChannelInfo->GetSizex()/2.0 ), 2) );
                     else  // Horizontal => base = upper edge
                        dClustToReadout = TMath::Sqrt(  TMath::Power( vPntPos.X() - fChannelInfo->GetX() , 2)
                            + TMath::Power( vPntPos.Y() - ( fChannelInfo->GetY() + fChannelInfo->GetSizey()/2.0 ), 2) );
               } // else of if( iChannel < iNbCh/2.0 )

            dPadTime += fRandRes->Gaus( 0.0, fdTimeResElec) + dClustToReadout/fdSignalPropSpeed;

            if( fDigiBdfPar->GetFeeThreshold() <=
                  dChargeCentral*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][iChannel] )
            {
               // Switch between Digi and DigiExp
               if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
               {
                  CbmTofDigiExp * tofDigi = new CbmTofDigiExp( iSM, iRpc, iChannel, dPadTime,
                        dChargeCentral*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][iChannel],
                        0, iSmType );
		  //				  tofDigi->GetMatch()->AddLink(1., iPntInd);
                  fStorDigiExp[iSmType][iSM*iNbRpc + iRpc][iChannel].push_back( tofDigi );
		  fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][iChannel].push_back( iPntInd );
		     LOG(DEBUG)<<Form("Digimatch (%d,%d,%d,%d): size %zu, val %d, MCt %d",
				     iSmType,iSM,iRpc,iChannel,fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iChannel+1].size(),iPntInd,iTrackID)
			      <<FairLogger::endl;

               } // if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
               else {
                  CbmTofDigi * tofDigi = new CbmTofDigi( iSM, iRpc, iChannel, dPadTime,
                        dChargeCentral*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][iChannel],
                        0, iSmType );
		  //				  tofDigi->GetMatch()->AddLink(1., iPntInd);
                  fStorDigi[iSmType][iSM*iNbRpc + iRpc][iChannel].push_back( tofDigi );
		  fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][iChannel].push_back( iPntInd );
               } // else of if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
            } // charge ok
         } // else of if( 0 == iChType)

      // Loop over neighboring channel to find if they have overlap with
      // the cluster ( and if yes which fraction of the total charge they
      // got)
      if( 0 == iChType)
      {
         // Strips (readout on both side)
         // Loop in decreasing channel index until a channel with right/upper edge farther
         // from cluster center than cluster radius is found
         // Loop in increasing channel index until a channel with left/lower edge farther
         // from cluster center than cluster radius is found

         Int_t iMinChanInd = iChannel - 1;
         Int_t iMaxChanInd = iChannel + 1;

         Double_t dClusterDist = 0.0;
         if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
         {
            // Horizontal channels: First go down, then up
            while( 0 <= iMinChanInd )
            {
               dClusterDist = TMath::Abs( vPntPos.Y()
                               - ( fChannelInfo->GetY()
                                   + fChannelInfo->GetSizey()*( iMinChanInd- iChannel + 0.5 ) ) );
               if( dClusterDist < dClusterSize )
                  iMinChanInd --;
                  else break;
            }
            while( iMaxChanInd < iNbCh )
            {
               dClusterDist = TMath::Abs( vPntPos.Y()
                               - ( fChannelInfo->GetY()
                                   + fChannelInfo->GetSizey()*( iMaxChanInd- iChannel - 0.5 ) ) );
               if( dClusterDist < dClusterSize )
                  iMaxChanInd ++;
                  else break;
            }
         } // if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
            else
            {
               // Vertical channels: First go to the left, then to the right
               while( 0 <= iMinChanInd )
               {
                  dClusterDist = TMath::Abs( vPntPos.X()
                                  - ( fChannelInfo->GetX()
                                      + fChannelInfo->GetSizex()*( iMinChanInd- iChannel + 0.5 ) ) );
                  if( dClusterDist < dClusterSize )
                     iMinChanInd --;
                     else break;
               }
               while( iMaxChanInd < iNbCh )
               {
                  dClusterDist = TMath::Abs( vPntPos.X()
                                  - ( fChannelInfo->GetX()
                                      + fChannelInfo->GetSizex()*( iMaxChanInd- iChannel - 0.5 ) ) );
                  if( dClusterDist < dClusterSize )
                     iMaxChanInd ++;
                     else break;
               }
            } // else of if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )

         // Loop over all channels inside the interval ]iMinChanInd;iMaxChanInd[ except central channel,
         // using the overlap area with cluster to get the charge and adding a Digi for all channels
         for( Int_t iChanInd = iMinChanInd + 1; iChanInd < iMaxChanInd; iChanInd++ )
         {
            if( iChanInd == iChannel )
               continue;

            // Channel index in this UID is in [1,nbCh] instead of [0, nbCh[
            // ... don't ask me why ...

	    //            CbmTofDetectorInfo xDetInfo(kTOF, iSmType, iSM, iRpc, 0, iChanInd + 1);
            Int_t iCh1=iChanInd;
	    if(fGeoHandler->GetGeoVersion() < k14a) iCh1= iCh1+1; //FIXME
	    CbmTofDetectorInfo xDetInfo(kTOF, iSmType, iSM, iRpc, 0, iCh1); 
            Int_t iSideChId = fTofId->SetDetectorInfo( xDetInfo );

            fChannelInfo = fDigiPar->GetCell( iSideChId );

	    if(NULL == fChannelInfo){
	      LOG(ERROR)<<"CbmTofDigitizerBDF::DigitizeFlatDisc: Invalid channel "
			<<iSideChId<<" "<<iSmType<<" "<<iSM<<" "<<iRpc<<" "<<iChanInd+1
			<<FairLogger::endl;
	      continue;
	    }

            // Calculate the fraction of the charge in this channel
            Double_t dChargeSideCh = dClustCharge * ComputeClusterAreaOnChannel(
                                              iSideChId, dClusterSize, vPntPos.X(), vPntPos.Y());
            dChargeSideCh /= dClustArea;
            if( dClustCharge  + 0.0000001 < dChargeSideCh )
            {
               LOG(ERROR)<<"CbmTofDigitizerBDF::DigitizeFlatDisc => Side Charge "
                         <<dChargeSideCh<<" "<<dClustCharge<<" "<<dClustArea<<FairLogger::endl;
               LOG(ERROR)<<"CbmTofDigitizerBDF::DigitizeFlatDisc => Check Point "<<iPntInd
                           <<" Sm type "<<iSmType<<" SM "<<iSM<<" Rpc "<<iRpc<<" Channel "<<iChanInd
                           <<" Type "<<iChType<<" Orient. "<<fDigiBdfPar->GetChanOrient( iSmType, iRpc )<<FairLogger::endl;
            } // if( dClustCharge  + 0.0000001 < dChargeSideCh )

            // Strips = readout on both side
            Double_t dTimeA = dCentralTime;
            Double_t dTimeB = dCentralTime;
            if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
            {
               // Horizontal channel: A = Right and B = Left of the channel
               dTimeA +=  fRandRes->Gaus( 0.0, fdTimeResElec)
#ifdef FULL_PROPAGATION_TIME
                        + TMath::Abs( vPntPos.X() - ( fChannelInfo->GetX() + fChannelInfo->GetSizex()/2.0 ) )
#else
                        + ( vPntPos.X() - fChannelInfo->GetX() )
#endif
                          /fdSignalPropSpeed;
               dTimeB +=  fRandRes->Gaus( 0.0, fdTimeResElec)
#ifdef FULL_PROPAGATION_TIME
                        + TMath::Abs( vPntPos.X() - ( fChannelInfo->GetX() - fChannelInfo->GetSizex()/2.0 ) )
#else
                        - ( vPntPos.X() - fChannelInfo->GetX() )
#endif
                          /fdSignalPropSpeed;
            } // if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
               else
               {
		 TGeoNode *fNode=        // prepare global->local trafo
			gGeoManager->FindNode(fChannelInfo->GetX(),fChannelInfo->GetY(),fChannelInfo->GetZ());
		 LOG(DEBUG)<<Form(" TofDigitizerBDF:: (%3.d,%3d,%3d,%3d)-node at (%6.1f,%6.1f,%6.1f) : 0x%p",
			      iSmType,iSM,iRpc,iChanInd,
			      fChannelInfo->GetX(),fChannelInfo->GetY(),fChannelInfo->GetZ(),fNode)
			<<FairLogger::endl;
		 TGeoNode*	cNode= gGeoManager->GetCurrentNode();
		 TGeoHMatrix* cMatrix = gGeoManager->GetCurrentMatrix();
		 Double_t hitpos[3]={fChannelInfo->GetX(),vPntPos.Y(),fChannelInfo->GetZ()};
		 Double_t hitpos_local[3];
		 gGeoManager->MasterToLocal(hitpos, hitpos_local);

                  // Vertical channel: A = Top and B = Bottom of the channel
                  dTimeA +=  fRandRes->Gaus( 0.0, fdTimeResElec)
#ifdef FULL_PROPAGATION_TIME
                           + TMath::Abs( vPntPos.Y() - ( fChannelInfo->GetY() + fChannelInfo->GetSizey()/2.0 ) )
#else
		    //                           + ( vPntPos.Y() - fChannelInfo->GetY() )
		           + hitpos_local[1]
#endif
                             /fdSignalPropSpeed;
                  dTimeB +=  fRandRes->Gaus( 0.0, fdTimeResElec)
#ifdef FULL_PROPAGATION_TIME
                           + TMath::Abs( vPntPos.Y() - ( fChannelInfo->GetY() - fChannelInfo->GetSizey()/2.0 ) )
#else
		    //                           - ( vPntPos.Y() - fChannelInfo->GetY() )
		           - hitpos_local[1]
#endif
                             /fdSignalPropSpeed;
               } // else of if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )

            // Fee Threshold on charge
            if( fDigiBdfPar->GetFeeThreshold() <=
                  dChargeSideCh*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][2*iChanInd+1]/2.0 )
            {
               // Switch between Digi and DigiExp
               if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
               {
                  CbmTofDigiExp * tofDigi = new CbmTofDigiExp( iSM, iRpc, iChanInd, dTimeA,
                        dChargeSideCh*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][2*iChanInd+1]/2.0,
                        1, iSmType );
		  //				  tofDigi->GetMatch()->AddLink(1., iPntInd);
                  fStorDigiExp[iSmType][iSM*iNbRpc + iRpc][2*iChanInd+1].push_back( tofDigi );
		  fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iChanInd+1].push_back( iPntInd );
		     LOG(DEBUG)<<Form("Digimatch (%d,%d,%d,%d): size %zu, val %d, MCt %d",
				     iSmType,iSM,iRpc,iChannel,fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iChannel+1].size(),iPntInd,iTrackID)
			      <<FairLogger::endl;

               } // if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
               else {
                  CbmTofDigi * tofDigi = new CbmTofDigi( iSM, iRpc, iChanInd, dTimeA,
                        dChargeSideCh*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][2*iChanInd+1]/2.0,
                        1, iSmType );
		  //				  tofDigi->GetMatch()->AddLink(1., iPntInd);
                  fStorDigi[iSmType][iSM*iNbRpc + iRpc][2*iChanInd+1].push_back( tofDigi );
		  fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iChanInd+1].push_back( iPntInd );
               } // else of if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
            } // if( charge above threshold)
            if( fDigiBdfPar->GetFeeThreshold() <=
                  dChargeSideCh*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][2*iChanInd]/2.0 )
            {
               // Switch between Digi and DigiExp
               if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
               {
                  CbmTofDigiExp * tofDigi = new CbmTofDigiExp( iSM, iRpc, iChanInd, dTimeB,
                        dChargeSideCh*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][2*iChanInd]/2.0,
                        0, iSmType );
		  //				  tofDigi->GetMatch()->AddLink(1., iPntInd);
                  fStorDigiExp[iSmType][iSM*iNbRpc + iRpc][2*iChanInd].push_back( tofDigi );
		  fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iChanInd].push_back( iPntInd );
		     LOG(DEBUG)<<Form("Digimatch (%d,%d,%d,%d): size %zu, val %d, MCt %d",
				     iSmType,iSM,iRpc,iChannel,fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iChannel+1].size(),iPntInd,iTrackID)
			      <<FairLogger::endl;

               } // if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
               else {
                  CbmTofDigi * tofDigi = new CbmTofDigi( iSM, iRpc, iChanInd, dTimeB,
                        dChargeSideCh*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][2*iChanInd]/2.0,
                        0, iSmType );
		  //				  tofDigi->GetMatch()->AddLink(1., iPntInd);
                  fStorDigi[iSmType][iSM*iNbRpc + iRpc][2*iChanInd].push_back( tofDigi );
		  fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iChanInd].push_back( iPntInd );
               } // else of if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
            } // if( charge above threshold)
         } // for( Int_t iChanInd = iMinChanInd + 1; iChanInd < iMaxChanInd; iChanInd++ )
      } // if( 0 == iChType)
         else
         {
            // Pad (Single side readout)
            // Assume that the bottom/left pads have lower channel index
            // E.g: for a 2*4 pads counter, pads 0-3 are the bottom/left ones and 4-7 the
            // top/right one with reversed numbering
            //               -----------------
            //  row 1        | 7 | 6 | 5 | 4 |          y
            //               -----------------          ^
            //  row 0        | 0 | 1 | 2 | 3 |          |
            //               -----------------           --> x
            // or            ---------
            //               | 4 | 3 |
            //               ---------
            //               | 5 | 2 |
            //               ---------
            //               | 6 | 1 |
            //               ---------
            //               | 7 | 0 |
            //               ---------
            //        row      1   0

            // Loop in decreasing channel index in same row, find min = 1st 0 channel
            // Loop in increasing channel index in same row, find max = 1st 0 channel
            Int_t iMinChanInd = iChannel;
            Int_t iMaxChanInd = iChannel;

            Double_t dClusterDist = 0;
            Int_t    iRow;
            Bool_t   bCheckOtherRow = kFALSE;
            if( iChannel < iNbCh/2.0 )
               iRow = 0;
               else iRow = 1;

            if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
            {
               // Vertical => base = right edge for row 0 and left edge for row 1
               while( dClusterDist < dClusterSize &&
                       iRow*iNbCh/2.0 < iMinChanInd )
               {
                  iMinChanInd --;
                  dClusterDist = TMath::Abs( vPntPos.Y()
                                  - ( fChannelInfo->GetY()
                                      + fChannelInfo->GetSizey()*( iMinChanInd - iChannel + ( 1-2*iRow )/2 ) ) );
               } // while upper/lower edge inside cluster and index not out of rpc
               dClusterDist = 0;
               while( dClusterDist < dClusterSize &&
                       iMaxChanInd < (1 + iRow)*iNbCh/2.0 - 1 )
               {
                  iMaxChanInd ++;
                  dClusterDist = TMath::Abs( vPntPos.Y()
                                  - ( fChannelInfo->GetY()
                                      + fChannelInfo->GetSizey()*( iMaxChanInd - iChannel - ( 1-2*iRow )/2 ) ) );
               } // while lower/upper edge inside cluster and index not out of rpc

               // Test if Pad in diff row but same column as central pad has cluster overlap
               // if Yes => Loop from min+1 to max-1 equivalents in the opposite row
               dClusterDist = TMath::Abs( vPntPos.X()
                                 - ( fChannelInfo->GetX()
                                     + fChannelInfo->GetSizex()*( 2*iRow - 1 )/2 ) );
               if( dClusterDist < dClusterSize )
                  bCheckOtherRow = kTRUE;
            } // if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
               else
               {
                  // Horizontal => base = lower edge for row 0 and upper edge for row 1
                  while( dClusterDist < dClusterSize &&
                        iRow*iNbCh/2.0 < iMinChanInd )
                  {
                     iMinChanInd --;
                     dClusterDist = TMath::Abs( vPntPos.X()
                                     - ( fChannelInfo->GetX()
                                         + fChannelInfo->GetSizex()*( iMinChanInd - iChannel + ( 1-2*iRow )/2 ) ) );
                  } // while right/left edge inside cluster and index not out of rpc
                  dClusterDist = 0;
                  while( dClusterDist < dClusterSize &&
                          iMaxChanInd < (1 + iRow)*iNbCh/2.0 - 1 )
                  {
                     iMaxChanInd ++;
                     dClusterDist = TMath::Abs( vPntPos.X()
                                     - ( fChannelInfo->GetX()
                                         + fChannelInfo->GetSizex()*( iMaxChanInd - iChannel - ( 1-2*iRow )/2 ) ) );
                  } // while left/right edge inside cluster and index not out of rpc

                  // Test if Pad in diff row but same column as central pad has cluster overlap
                  // if Yes => Loop from min+1 to max-1 equivalents in the opposite row
                  dClusterDist = TMath::Abs( vPntPos.Y()
                                    - ( fChannelInfo->GetY()
                                        + fChannelInfo->GetSizey()*( 1 - 2*iRow )/2 ) );
                  if( dClusterDist < dClusterSize )
                     bCheckOtherRow = kTRUE;
               } // else of if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )

            // Loop over all channels inside the interval ]iMinChanInd;iMaxChanInd[ except central channel,
            // using the overlap area with cluster to get the charge and adding a Digi for all channels
            for( Int_t iChanInd = iMinChanInd + 1; iChanInd < iMaxChanInd; iChanInd++ )
            {
               if( iChanInd == iChannel )
                  continue;

               // Channel index in this UID is in [1,nbCh] instead of [0, nbCh[
               // ... don't ask me why ...
               CbmTofDetectorInfo xDetInfo(kTOF, iSmType, iSM, iRpc, 0, iChanInd + 1);

               Int_t iSideChId =fTofId->SetDetectorInfo( xDetInfo );

               fChannelInfo = fDigiPar->GetCell( iSideChId );

               // Calculate the fraction of the charge in this channel
               Double_t dChargeSideCh = dClustCharge * ComputeClusterAreaOnChannel(
                                                 iSideChId, dClusterSize, vPntPos.X(), vPntPos.Y());
               dChargeSideCh /= dClustArea;

               // Fee Threshold on charge
               if( dChargeSideCh*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][iChanInd]
                     < fDigiBdfPar->GetFeeThreshold() )
                  continue;

               Double_t dClustToReadout = 0.0;
               Double_t dPadTime = dCentralTime;
               // First calculate the propagation time to the center of the pad base
               if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
                  // Vertical => base = right/left edge
                  dClustToReadout = TMath::Sqrt(  TMath::Power( vPntPos.Y() - fChannelInfo->GetY() , 2)
                         + TMath::Power( vPntPos.X() - ( fChannelInfo->GetX() + (1-2*iRow)*fChannelInfo->GetSizex()/2.0 ), 2) );
                  else  // Horizontal => base = bottom/upper edge
                     dClustToReadout = TMath::Sqrt(  TMath::Power( vPntPos.X() - fChannelInfo->GetX() , 2)
                         + TMath::Power( vPntPos.Y() - ( fChannelInfo->GetY() - (1-2*iRow)*fChannelInfo->GetSizey()/2.0 ), 2) );

               dPadTime += fRandRes->Gaus( 0.0, fdTimeResElec) + dClustToReadout/fdSignalPropSpeed;

               // Switch between Digi and DigiExp
               if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
               {
                  CbmTofDigiExp * tofDigi = new CbmTofDigiExp( iSM, iRpc, iChanInd, dPadTime,
                        dChargeSideCh*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][iChanInd],
                        0, iSmType );
		  //				  tofDigi->GetMatch()->AddLink(1., iPntInd);
                  fStorDigiExp[iSmType][iSM*iNbRpc + iRpc][iChanInd].push_back( tofDigi );
		  fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][iChanInd].push_back( iPntInd );
		     LOG(DEBUG)<<Form("Digimatch (%d,%d,%d,%d): size %zu, val %d, MCt %d",
				     iSmType,iSM,iRpc,iChannel,fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iChannel+1].size(),iPntInd,iTrackID)
			      <<FairLogger::endl;
               } // if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
               else {
                  CbmTofDigi * tofDigi = new CbmTofDigi( iSM, iRpc, iChanInd, dPadTime,
                        dChargeSideCh*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][iChanInd],
                        0, iSmType );
		  //				  tofDigi->GetMatch()->AddLink(1., iPntInd);
                  fStorDigi[iSmType][iSM*iNbRpc + iRpc][iChanInd].push_back( tofDigi );
		  fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][iChanInd].push_back( iPntInd );
               } // else of if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
            } // for( Int_t iChanInd = iMinChanInd + 1; iChanInd < iMaxChanInd; iChanInd++ )

            // Loop over channels on the other row equivalent to the interval ]iMinChanInd;iMaxChanInd[
            if( kTRUE == bCheckOtherRow )
               for( Int_t iChanInd = iMinChanInd + 1 + (1-2*iRow)*iNbCh/2.0;
                          iChanInd < iMaxChanInd + (1-2*iRow)*iNbCh/2.0;
                          iChanInd++ )
               {
                  // Channel index in this UID is in [1,nbCh] instead of [0, nbCh[
                  // ... don't ask me why ...
                  CbmTofDetectorInfo xDetInfo(kTOF, iSmType, iSM, iRpc, 0, iChanInd + 1);

                  Int_t iSideChId =fTofId->SetDetectorInfo( xDetInfo );

                  fChannelInfo = fDigiPar->GetCell( iSideChId );

                  // Calculate the fraction of the charge in this channel
                  Double_t dChargeSideCh = dClustCharge * ComputeClusterAreaOnChannel(
                                                    iSideChId, dClusterSize, vPntPos.X(), vPntPos.Y());

                  // Fee Threshold on charge
                  if( dChargeSideCh*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][iChanInd]
                        < fDigiBdfPar->GetFeeThreshold() )
                     continue;

                  Double_t dClustToReadout = 0.0;
                  Double_t dPadTime = dCentralTime;
                  // First calculate the propagation time to the center of the pad base
                  if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
                     // Vertical => base = right/left edge
                     dClustToReadout = TMath::Sqrt(  TMath::Power( vPntPos.Y() - fChannelInfo->GetY() , 2)
                            + TMath::Power( vPntPos.X() - ( fChannelInfo->GetX() + (1-2*iRow)*fChannelInfo->GetSizex()/2.0 ), 2) );
                     else  // Horizontal => base = bottom/upper edge
                        dClustToReadout = TMath::Sqrt(  TMath::Power( vPntPos.X() - fChannelInfo->GetX() , 2)
                            + TMath::Power( vPntPos.Y() - ( fChannelInfo->GetY() - (1-2*iRow)*fChannelInfo->GetSizey()/2.0 ), 2) );
		  
                  dPadTime += fRandRes->Gaus( 0.0, fdTimeResElec) + dClustToReadout/fdSignalPropSpeed;

                  // Switch between Digi and DigiExp
                  if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
                  {
                     CbmTofDigiExp * tofDigi = new CbmTofDigiExp( iSM, iRpc, iChanInd, dPadTime,
                           dChargeSideCh*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][iChanInd],
                           0, iSmType );
		     //					 tofDigi->GetMatch()->AddLink(1., iPntInd);
                     fStorDigiExp[iSmType][iSM*iNbRpc + iRpc][iChanInd].push_back( tofDigi );
		     fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][iChanInd].push_back( iPntInd );
		     LOG(DEBUG)<<Form("Digimatch (%d,%d,%d,%d): size %zu, val %d, MCt %d",
				     iSmType,iSM,iRpc,iChannel,fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iChannel+1].size(),iPntInd,iTrackID)
			      <<FairLogger::endl;

                  } // if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
                  else {
                     CbmTofDigi * tofDigi = new CbmTofDigi( iSM, iRpc, iChanInd, dPadTime,
                           dChargeSideCh*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][iChanInd],
                           0, iSmType );
		     //					 tofDigi->GetMatch()->AddLink(1., iPntInd);
                     fStorDigi[iSmType][iSM*iNbRpc + iRpc][iChanInd].push_back( tofDigi );
		     fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][iChanInd].push_back( iPntInd );
                  } // else of if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
               } // for( Int_t iChanInd = iMinChanInd + 1; iChanInd < iMaxChanInd; iChanInd++ )
         } // else of if( 0 == iChType)

   } // for (Int_t iPntInd = 0; iPntInd < nTofPoint; iPntInd++ )

   if( kTRUE == fDigiBdfPar->UseOneGapPerTrk() )
   {
      // Clear the Track to channel temporary storage
      for(Int_t iTrkInd = 0; iTrkInd < nMcTracks; iTrkInd++)
         fvlTrckChAddr[iTrkInd].clear();
      fvlTrckChAddr.clear();
   } // if( kTRUE == fDigiBdfPar->UseOneGapPerTrk() )

   return kTRUE;
}
/************************************************************************************/
// Functions for a 2D Gauss cluster approximation
Bool_t CbmTofDigitizerBDF::DigitizeGaussCharge()
{
   // Uniform distribution in ]0;x]
   // gRandom->Uniform(x);
   // Gauss distribution in of mean m, sigma s
   // gRandom->Gauss(m, s);

   CbmTofPoint *pPoint;
   CbmMCTrack  *pMcTrk;

   Int_t nTofPoint = fTofPointsColl->GetEntries();
   Int_t nMcTracks = fMcTracksColl ->GetEntries();

   // Debug Info printout
   Int_t iNbTofTracks     = 0;
   Int_t iNbTofTracksPrim = 0;

   LOG(DEBUG) << "CbmTofDigitizerBDF::DigitizeGaussCharge: " << nTofPoint
              << " points in Tof for this event with " << nMcTracks
              << " MC tracks "<< FairLogger::endl;

   // Prepare the temporary storing of the Track/Point/Digi info
   if( kTRUE == fDigiBdfPar->UseOneGapPerTrk() )
      fvlTrckChAddr.resize( nMcTracks );
   for(Int_t iTrkInd = 0; iTrkInd < nMcTracks; iTrkInd++)
   {
      if( kTRUE == fDigiBdfPar->UseOneGapPerTrk() )
         fvlTrckChAddr[iTrkInd].clear();
      pMcTrk = (CbmMCTrack*) fMcTracksColl->At( iTrkInd );
      if( 0 < pMcTrk->GetNPoints(kTOF) )
         iNbTofTracks++;
      if( 0 < pMcTrk->GetNPoints(kTOF) && -1 == pMcTrk->GetMotherId() )
         iNbTofTracksPrim++;
   } // for(Int_t iTrkInd = 0; iTrkInd < nMcTracks; iTrkInd++)

   //Some numbers on TOF distributions
   LOG(DEBUG) << "CbmTofDigitizerBDF::DigitizeGaussCharge: " << iNbTofTracks
              << " tracks in Tof " << FairLogger::endl;
   LOG(DEBUG) << "CbmTofDigitizerBDF::DigitizeGaussCharge: " << iNbTofTracksPrim
              << " tracks in Tof from vertex" << FairLogger::endl;

   // Tof Point Info
   Int_t    iDetId, iSmType, iSM, iRpc, iChannel, iGap;
   Int_t    iTrackID, iChanId;
   TVector3 vPntPos;
   // Cluster Info
   Double_t dClusterSize;
   Double_t dClustCharge;
   // Digi
//   CbmTofDigi * pTofDigi;    // <- Compressed digi (64 bits)
//   CbmTofDigi * pTofDigiExp; // <- Expanded digi
   // General geometry info
   Int_t    iNbSmTypes = fDigiBdfPar->GetNbSmTypes();
   Int_t    iNbSm, iNbRpc, iNbCh;
   Int_t    iChType;

   // Start jitter: Same contrib. for all points in same event
   Double_t dStartJitter = fRandStart->Gaus( 0.0, fDigiBdfPar->GetStartTimeRes() );

   for (Int_t iPntInd = 0; iPntInd < nTofPoint; iPntInd++ )
   {
      // Get a pointer to the TOF point
      pPoint = (CbmTofPoint*) fTofPointsColl->At( iPntInd );
      if( NULL == pPoint )
      {
         LOG(WARNING)<<"CbmTofDigitizerBDF::DigitizeGaussCharge => Be careful: hole in the CbmTofPoint TClonesArray!"<<endl;
         continue;
      } // if( pPoint )

      // Get all channel info
      iDetId   = pPoint->GetDetectorID();
      iSmType  = fGeoHandler->GetSMType(iDetId);
      iRpc     = fGeoHandler->GetCounter(iDetId);

      iChannel = fGeoHandler->GetCell(iDetId);
      iGap     = fGeoHandler->GetGap(iDetId);
      iSM      = fGeoHandler->GetSModule(iDetId);
      iChanId  = fGeoHandler->GetCellId(iDetId);
      iTrackID = pPoint->GetTrackID();

      iNbSm   = fDigiBdfPar->GetNbSm(  iSmType);
      iNbRpc  = fDigiBdfPar->GetNbRpc( iSmType);
      iNbCh   = fDigiBdfPar->GetNbChan( iSmType, iRpc );
      iChType = fDigiBdfPar->GetChanType( iSmType, iRpc );

      // Get pointer to the MC-Track info
      Int_t iTrkId = pPoint->GetTrackID();
      pMcTrk = (CbmMCTrack*) fMcTracksColl->At( iTrkId );

      // Keep only primaries
      if( kTRUE == fDigiBdfPar->UseOnlyPrimaries() )
         if( -1 != pMcTrk->GetMotherId() )
            continue;

      if(   iSmType  < 0. || iNbSmTypes < iSmType
         || iSM      < 0. || iNbSm      < iSM
         || iRpc     < 0. || iNbRpc     < iRpc
         || iChannel < 0. || iNbCh      < iChannel )
      {
	 LOG(ERROR)<<Form("CbmTofDigitizerBDF => det ID 0x%08x",iDetId) <<" SMType: "<<iSmType;
         LOG(ERROR)<<" SModule: "<<iSM<<" of "<<iNbSm+1;
         LOG(ERROR)<<" Module: "<<iRpc<<" of "<<iNbRpc+1;
         LOG(ERROR)<<" Gap: "<<iGap;
         LOG(ERROR)<<" Cell: "<<iChannel<<" of "<<iNbCh+1 <<FairLogger::endl;
         continue;
      } // if error on channel data

      // Check if there was already a Digi from the same track created
      // with this channel as main channel
      ULong64_t uAddr = CbmTofAddress::GetUniqueAddress( iSM, iRpc, iChannel, 0, iSmType);
      if( kTRUE == fDigiBdfPar->UseOneGapPerTrk() )
      {
         Bool_t bFoundIt = kFALSE;
         for( Int_t iTrkMainCh = 0; iTrkMainCh < fvlTrckChAddr[iTrkId].size(); iTrkMainCh ++)
            if( uAddr == fvlTrckChAddr[iTrkId][iTrkMainCh])
            {
               bFoundIt = kTRUE;
               break;
            }
         // If it is the case, no need to redo the full stuff for this gap
         if( kTRUE == bFoundIt)
            continue;
      } // if( kTRUE == fDigiBdfPar->UseOneGapPerTrk() )

      // Probability that the RPC detect the hit
//      if( fDigiBdfPar->GetEfficiency(iSmType) < fRandEff->Uniform(1) )
//         continue;

      // Probability that the gap detect the hit
      // For now use a simple gap treatment (cf CbmTofDigiBdfPar):
      // - a single fired gap fires the channel
      // - all gaps have exactly equivalent efficiency/firing probability
      // - independent gap firing (no charge accumulation or such)
      // The probability for a hit not to fire at all is then
      //    (1-p)^NGaps with p = gap efficiency and Ngaps the number of gaps in this RPC
      // This results in the relation: p = 1 - (1 - P)^(1/Ngaps)
      //         with P = RPC efficiency from beamtime
      if( fDigiBdfPar->GetGapEfficiency(iSmType, iRpc) < fRandEff->Uniform(1) )
         continue;

      // Save the address in vector so that this channel is not redone later for the
      // eventual other gaps TofPoint
      if( kTRUE == fDigiBdfPar->UseOneGapPerTrk() )
         fvlTrckChAddr[iTrkId].push_back(uAddr);

      // Get TofPoint Position
      pPoint->Position( vPntPos );
      fChannelInfo = fDigiPar->GetCell(iChanId);

      // Generate a Cluster radius in [cm]
      dClusterSize = GenerateClusterRadius( iSmType, iRpc );
      while( dClusterSize < 0.0001 )
         // Should not happen without error message
         // But Landau can give really small values
         dClusterSize = GenerateClusterRadius( iSmType, iRpc );

      // Generate Cluster charge as ToT[ps]
      dClustCharge = fh1ClusterTotProb[iSmType]->GetBinContent(
                        fh1ClusterTotProb[iSmType]->FindBin(
                           fRandCharge->Uniform(1) ) );

      // Assume the width (sigma) of the gaussian in both direction is dClusterSize/3
      // => 99% of the charge is in "dClusterSize"
      TF2* f2ChargeDist = new TF2("ChargeDist", "[0]*TMath::Gaus(x,[1],[2])*TMath::Gaus(y,[3],[4])",
                                   -5.0*dClusterSize, 5.0*dClusterSize,
                                   -5.0*dClusterSize, 5.0*dClusterSize);
      f2ChargeDist->SetParameters( dClustCharge/( TMath::Sqrt( 2.0*TMath::Pi() )*dClusterSize/3.0),
                                     vPntPos.X(), dClusterSize/3.0,
                                     vPntPos.Y(), dClusterSize/3.0);

      // Calculate the fraction of the charge in central channel
      Double_t dChargeCentral = f2ChargeDist->Integral( fChannelInfo->GetX() - fChannelInfo->GetSizex()/2.0,
                                                        fChannelInfo->GetX() + fChannelInfo->GetSizex()/2.0,
                                                        fChannelInfo->GetY() - fChannelInfo->GetSizey()/2.0,
                                                        fChannelInfo->GetY() + fChannelInfo->GetSizey()/2.0 );

      // Calculate the time for the central channel
      Double_t dCentralTime =  pPoint->GetTime()
                             + fRandRes->Gaus( 0.0, fDigiBdfPar->GetResolution(iSmType ) )
                             + dStartJitter; // Same contrib. for all points in same event

      // Calculate propagation time(s) to the readout point(s)
      if( 0 == iChType)
      {
         // Strips (readout on both side)
         // Assume that the bottom/left strip have lower channel index
         // E.g:  ... | i | i+1 | ...
         //     or      ...         y
         //            -----        ^
         //             i+1         |
         //            -----         --> x
         //              i
         //            -----
         //             ...

         Double_t dTimeA = dCentralTime;
         Double_t dTimeB = dCentralTime;
         if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
         {
            // Horizontal channel: A = Right and B = Left of the channel
            dTimeA +=  fRandRes->Gaus( 0.0, fdTimeResElec)
#ifdef FULL_PROPAGATION_TIME
                     + TMath::Abs( vPntPos.X() - ( fChannelInfo->GetX() + fChannelInfo->GetSizex()/2.0 ) )
#else
                     + ( vPntPos.X() - fChannelInfo->GetX() )
#endif
                       /fdSignalPropSpeed;
            dTimeB +=  fRandRes->Gaus( 0.0, fdTimeResElec)
#ifdef FULL_PROPAGATION_TIME
                     + TMath::Abs( vPntPos.X() - ( fChannelInfo->GetX() - fChannelInfo->GetSizex()/2.0 ) )
#else
                     - ( vPntPos.X() - fChannelInfo->GetX() )
#endif
                       /fdSignalPropSpeed;
         } // if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
            else
            {
               // Vertical channel: A = Top and B = Bottom of the channel
               dTimeA +=  fRandRes->Gaus( 0.0, fdTimeResElec)
#ifdef FULL_PROPAGATION_TIME
                        + TMath::Abs( vPntPos.Y() - ( fChannelInfo->GetY() + fChannelInfo->GetSizey()/2.0 ) )
#else
                        + ( vPntPos.Y() - fChannelInfo->GetY() )
#endif
                          /fdSignalPropSpeed;
               dTimeB +=  fRandRes->Gaus( 0.0, fdTimeResElec)
#ifdef FULL_PROPAGATION_TIME
                        + TMath::Abs( vPntPos.Y() - ( fChannelInfo->GetY() - fChannelInfo->GetSizey()/2.0 ) )
#else
                        - ( vPntPos.Y() - fChannelInfo->GetY() )
#endif
                          /fdSignalPropSpeed;
            } // else of if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )

         // Switch between Digi and DigiExp
         if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
         {
            CbmTofDigiExp * tofDigi = new CbmTofDigiExp( iSM, iRpc, iChannel, dTimeA,
                  dChargeCentral*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][2*iChannel+1]/2.0,
                  1, iSmType );
	    //			tofDigi->GetMatch()->AddLink(1., iPntInd);
            fStorDigiExp[iSmType][iSM*iNbRpc + iRpc][2*iChannel+1].push_back( tofDigi );
            fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iChannel+1].push_back( iPntInd );
		     LOG(DEBUG)<<Form("Digimatch (%d,%d,%d,%d): size %zu, val %d, MCt %d",
				     iSmType,iSM,iRpc,iChannel,fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iChannel+1].size(),iPntInd,iTrackID)
			      <<FairLogger::endl;

            tofDigi = new CbmTofDigiExp( iSM, iRpc, iChannel, dTimeB,
                  dChargeCentral*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][2*iChannel]/2.0,
                  0, iSmType );
	    //			tofDigi->GetMatch()->AddLink(1., iPntInd);
            fStorDigiExp[iSmType][iSM*iNbRpc + iRpc][2*iChannel].push_back( tofDigi );
            fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iChannel].push_back( iPntInd );
		     LOG(DEBUG)<<Form("Digimatch (%d,%d,%d,%d): size %zu, val %d, MCt %d",
				     iSmType,iSM,iRpc,iChannel,fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iChannel].size(),iPntInd,iTrackID)
			      <<FairLogger::endl;
         } // if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
         else {
            CbmTofDigi * tofDigi = new CbmTofDigi( iSM, iRpc, iChannel, dTimeA,
                  dChargeCentral*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][2*iChannel+1]/2.0,
                  1, iSmType );
	    //			tofDigi->GetMatch()->AddLink(1., iPntInd);
            fStorDigi[iSmType][iSM*iNbRpc + iRpc][2*iChannel+1].push_back( tofDigi );
            fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iChannel+1].push_back( iPntInd );
            tofDigi = new CbmTofDigi( iSM, iRpc, iChannel, dTimeB,
                  dChargeCentral*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][2*iChannel]/2.0,
                  0, iSmType );
	    //			tofDigi->GetMatch()->AddLink(1., iPntInd);
            fStorDigi[iSmType][iSM*iNbRpc + iRpc][2*iChannel].push_back( tofDigi );
            fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iChannel].push_back( iPntInd );
         } // else of if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
      } // if( 0 == iChType)
         else
         {
            // Pad (Single side readout)
            // Assume that the bottom/left pads have lower channel index
            // E.g: for a 2*4 pads counter, pads 0-3 are the bottom/left ones and 4-7 the
            // top/right one with reversed numbering:
            //               -----------------
            //  row 1        | 7 | 6 | 5 | 4 |          y
            //               -----------------          ^
            //  row 0        | 0 | 1 | 2 | 3 |          |
            //               -----------------           --> x
            // or            ---------
            //               | 4 | 3 |
            //               ---------
            //               | 5 | 2 |
            //               ---------
            //               | 6 | 1 |
            //               ---------
            //               | 7 | 0 |
            //               ---------
            //        row      1   0
            // Also assume that the readout happens at the middle of the readout edge

            Double_t dClustToReadout = 0.0;
            Double_t dPadTime = dCentralTime;
            // First calculate the propagation time to the center of the pad base
            if( iChannel < iNbCh/2.0 )
            {
               // First row
               if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
                  // Vertical => base = right edge
                  dClustToReadout = TMath::Sqrt(  TMath::Power( vPntPos.Y() - fChannelInfo->GetY() , 2)
                         + TMath::Power( vPntPos.X() - ( fChannelInfo->GetX() + fChannelInfo->GetSizex()/2.0 ), 2) );
                  else  // Horizontal => base = bottom edge
                     dClustToReadout = TMath::Sqrt(  TMath::Power( vPntPos.X() - fChannelInfo->GetX() , 2)
                         + TMath::Power( vPntPos.Y() - ( fChannelInfo->GetY() - fChannelInfo->GetSizey()/2.0 ), 2) );
            } // if( iChannel < iNbCh/2.0 )
               else
               {
                  // Second row
                  if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
                     // Vertical => base = left edge
                     dClustToReadout = TMath::Sqrt(  TMath::Power( vPntPos.Y() - fChannelInfo->GetY() , 2)
                            + TMath::Power( vPntPos.X() - ( fChannelInfo->GetX() - fChannelInfo->GetSizex()/2.0 ), 2) );
                     else  // Horizontal => base = upper edge
                        dClustToReadout = TMath::Sqrt(  TMath::Power( vPntPos.X() - fChannelInfo->GetX() , 2)
                            + TMath::Power( vPntPos.Y() - ( fChannelInfo->GetY() + fChannelInfo->GetSizey()/2.0 ), 2) );
               } // else of if( iChannel < iNbCh/2.0 )

            dPadTime += fRandRes->Gaus( 0.0, fdTimeResElec) + dClustToReadout/fdSignalPropSpeed;

            // TODO: Check on fee threshold ?
            // Switch between Digi and DigiExp
            if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
            {
               CbmTofDigiExp * tofDigi = new CbmTofDigiExp( iSM, iRpc, iChannel, dPadTime,
                     dChargeCentral*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][iChannel],
                     0, iSmType );
	       //			   tofDigi->GetMatch()->AddLink(1., iPntInd);
               fStorDigiExp[iSmType][iSM*iNbRpc + iRpc][iChannel].push_back( tofDigi );
	       fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][iChannel].push_back( iPntInd );
		     LOG(DEBUG)<<Form("Digimatch (%d,%d,%d,%d): size %zu, val %d, MCt %d",
				     iSmType,iSM,iRpc,iChannel,fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][iChannel].size(),iPntInd,iTrackID)
			      <<FairLogger::endl;
            } // if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
            else {
               CbmTofDigi * tofDigi = new CbmTofDigi( iSM, iRpc, iChannel, dPadTime,
                     dChargeCentral*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][iChannel],
                     0, iSmType );
	       //			   tofDigi->GetMatch()->AddLink(1., iPntInd);
               fStorDigi[iSmType][iSM*iNbRpc + iRpc][iChannel].push_back( tofDigi );
	       fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][iChannel].push_back( iPntInd );

            } // else of if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
         } // else of if( 0 == iChType)

      // Loop over neighboring channel and check if they are above threshold
      if( 0 == iChType)
      {
         // Strips (readout on both side)
         // Loop in decreasing channel index until a channel with charge under threshold is found
         if( 0 < iChannel)
         {
            Int_t iSideChInd = iChannel - 1;

            // Channel index in this UID is in [1,nbCh] instead of [0, nbCh[
            // ... don't ask me why ...
            CbmTofDetectorInfo xDetInfo(kTOF, iSmType, iSM, iRpc, 0, iSideChInd + 1);

            Int_t iSideChId = fTofId->SetDetectorInfo( xDetInfo );

            fChannelInfo = fDigiPar->GetCell( iSideChId );

            // Calculate the fraction of the charge in this channel
            Double_t dChargeSideCh = f2ChargeDist->Integral(
                  fChannelInfo->GetX() - fChannelInfo->GetSizex()/2.0,
                  fChannelInfo->GetX() + fChannelInfo->GetSizex()/2.0,
                  fChannelInfo->GetY() - fChannelInfo->GetSizey()/2.0,
                  fChannelInfo->GetY() + fChannelInfo->GetSizey()/2.0 );

            while( fDigiBdfPar->GetFeeThreshold() <= dChargeSideCh &&
                     0 <= iSideChInd )
            {
               // Strips = readout on both side
               Double_t dTimeA = dCentralTime;
               Double_t dTimeB = dCentralTime;
               if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
               {
                  // Horizontal channel: A = Right and B = Left of the channel
                  dTimeA +=  fRandRes->Gaus( 0.0, fdTimeResElec)
#ifdef FULL_PROPAGATION_TIME
                           + TMath::Abs( vPntPos.X() - ( fChannelInfo->GetX() + fChannelInfo->GetSizex()/2.0 ) )
#else
                           + ( vPntPos.X() - fChannelInfo->GetX() )
#endif
                             /fdSignalPropSpeed;
                  dTimeB +=  fRandRes->Gaus( 0.0, fdTimeResElec)
#ifdef FULL_PROPAGATION_TIME
                           + TMath::Abs( vPntPos.X() - ( fChannelInfo->GetX() - fChannelInfo->GetSizex()/2.0 ) )
#else
                           - ( vPntPos.X() - fChannelInfo->GetX() )
#endif
                             /fdSignalPropSpeed;
               } // if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
                  else
                  {
                     // Vertical channel: A = Top and B = Bottom of the channel
                     dTimeA +=  fRandRes->Gaus( 0.0, fdTimeResElec)
#ifdef FULL_PROPAGATION_TIME
                              + TMath::Abs( vPntPos.Y() - ( fChannelInfo->GetY() + fChannelInfo->GetSizey()/2.0 ) )
#else
                              + ( vPntPos.Y() - fChannelInfo->GetY() )
#endif
                                /fdSignalPropSpeed;
                     dTimeB +=  fRandRes->Gaus( 0.0, fdTimeResElec)
#ifdef FULL_PROPAGATION_TIME
                              + TMath::Abs( vPntPos.Y() - ( fChannelInfo->GetY() - fChannelInfo->GetSizey()/2.0 ) )
#else
                              - ( vPntPos.Y() - fChannelInfo->GetY() )
#endif
                                /fdSignalPropSpeed;
                  } // else of if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )

               // Switch between Digi and DigiExp
               if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
               {
                  CbmTofDigiExp * tofDigi = new CbmTofDigiExp( iSM, iRpc, iSideChInd, dTimeA,
                        dChargeSideCh*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][2*iSideChInd+1]/2.0,
                        1, iSmType );
		  //				   tofDigi->GetMatch()->AddLink(1., iPntInd);
                  fStorDigiExp[iSmType][iSM*iNbRpc + iRpc][2*iSideChInd+1].push_back( tofDigi );
		  fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iSideChInd+1].push_back( iPntInd );
                  tofDigi = new CbmTofDigiExp( iSM, iRpc, iSideChInd, dTimeB,
                        dChargeSideCh*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][2*iSideChInd]/2.0,
                        0, iSmType );
		  //				  tofDigi->GetMatch()->AddLink(1., iPntInd);
                  fStorDigiExp[iSmType][iSM*iNbRpc + iRpc][2*iSideChInd].push_back( tofDigi );
		  fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iSideChInd].push_back( iPntInd );
		     LOG(DEBUG)<<Form("Digimatch (%d,%d,%d,%d): size %zu, val %d, MCt %d",
				     iSmType,iSM,iRpc,iChannel,fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iSideChInd].size(),iPntInd,iTrackID)
			      <<FairLogger::endl;

               } // if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
               else {
                  CbmTofDigi * tofDigi = new CbmTofDigi( iSM, iRpc, iSideChInd, dTimeA,
                        dChargeSideCh*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][2*iSideChInd+1]/2.0,
                        1, iSmType );
		  //				  tofDigi->GetMatch()->AddLink(1., iPntInd);
                  fStorDigi[iSmType][iSM*iNbRpc + iRpc][2*iSideChInd+1].push_back( tofDigi );
		  fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iSideChInd+1].push_back( iPntInd );
                  tofDigi = new CbmTofDigi( iSM, iRpc, iSideChInd, dTimeB,
                        dChargeSideCh*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][2*iSideChInd]/2.0,
                        0, iSmType );
		  //				  tofDigi->GetMatch()->AddLink(1., iPntInd);
                  fStorDigi[iSmType][iSM*iNbRpc + iRpc][2*iSideChInd].push_back( tofDigi );
		  fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iSideChInd].push_back( iPntInd );
               } // else of if( kTRUE == fDigiBdfPar->UseExpandedDigi() )

               // Check next
               iSideChInd --;

               // Channel index in this UID is in [1,nbCh] instead of [0, nbCh[
               // ... don't ask me why ...
               // FIXME: probleam around here
               xDetInfo.fCell = iSideChInd + 1;

               iSideChId = fTofId->SetDetectorInfo( xDetInfo );

               fChannelInfo = fDigiPar->GetCell( iSideChId );

               // Calculate the fraction of the charge in this channel
               dChargeSideCh = f2ChargeDist->Integral(
                     fChannelInfo->GetX() - fChannelInfo->GetSizex()/2.0,
                     fChannelInfo->GetX() + fChannelInfo->GetSizex()/2.0,
                     fChannelInfo->GetY() - fChannelInfo->GetSizey()/2.0,
                     fChannelInfo->GetY() + fChannelInfo->GetSizey()/2.0 );
            } // while signal and channel index ok
         } // if( 0 < iChannel)
         // Loop in increasing channel index until a channel with charge under threshold is found
         if( iChannel < iNbCh - 1)
         {
            Int_t iSideChInd = iChannel + 1;

            // Channel index in this UID is in [1,nbCh] instead of [0, nbCh[
            // ... don't ask me why ...
            CbmTofDetectorInfo xDetInfo(kTOF, iSmType, iSM, iRpc, 0, iSideChInd + 1);

            Int_t iSideChId = fTofId->SetDetectorInfo( xDetInfo );

            fChannelInfo = fDigiPar->GetCell( iSideChId );

            // Calculate the fraction of the charge in this channel
            Double_t dChargeSideCh = f2ChargeDist->Integral(
                  fChannelInfo->GetX() - fChannelInfo->GetSizex()/2.0,
                  fChannelInfo->GetX() + fChannelInfo->GetSizex()/2.0,
                  fChannelInfo->GetY() - fChannelInfo->GetSizey()/2.0,
                  fChannelInfo->GetY() + fChannelInfo->GetSizey()/2.0 );

            while( fDigiBdfPar->GetFeeThreshold() < dChargeSideCh &&
                    iSideChInd < iNbCh  )
            {
               // Strips = readout on both side
               Double_t dTimeA = dCentralTime;
               Double_t dTimeB = dCentralTime;
               if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
               {
                  // Horizontal channel: A = Right and B = Left of the channel
                  dTimeA +=  fRandRes->Gaus( 0.0, fdTimeResElec)
#ifdef FULL_PROPAGATION_TIME
                           + TMath::Abs( vPntPos.X() - ( fChannelInfo->GetX() + fChannelInfo->GetSizex()/2.0 ) )
#else
                           + ( vPntPos.X() - fChannelInfo->GetX() )
#endif
                             /fdSignalPropSpeed;
                  dTimeB +=  fRandRes->Gaus( 0.0, fdTimeResElec)
#ifdef FULL_PROPAGATION_TIME
                           + TMath::Abs( vPntPos.X() - ( fChannelInfo->GetX() - fChannelInfo->GetSizex()/2.0 ) )
#else
                           - ( vPntPos.X() - fChannelInfo->GetX() )
#endif
                             /fdSignalPropSpeed;
               } // if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
                  else
                  {
                     // Vertical channel: A = Top and B = Bottom of the channel
                     dTimeA +=  fRandRes->Gaus( 0.0, fdTimeResElec)
#ifdef FULL_PROPAGATION_TIME
                              + TMath::Abs( vPntPos.Y() - ( fChannelInfo->GetY() + fChannelInfo->GetSizey()/2.0 ) )
#else
                              + ( vPntPos.Y() - fChannelInfo->GetY() )
#endif
                                /fdSignalPropSpeed;
                     dTimeB +=  fRandRes->Gaus( 0.0, fdTimeResElec)
#ifdef FULL_PROPAGATION_TIME
                              + TMath::Abs( vPntPos.Y() - ( fChannelInfo->GetY() - fChannelInfo->GetSizey()/2.0 ) )
#else
                              - ( vPntPos.Y() - fChannelInfo->GetY() )
#endif
                                /fdSignalPropSpeed;
                  } // else of if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )

               // Switch between Digi and DigiExp
               if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
               {
                  CbmTofDigiExp * tofDigi = new CbmTofDigiExp( iSM, iRpc, iSideChInd, dTimeA,
                        dChargeSideCh*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][2*iSideChInd+1]/2.0,
                        1, iSmType );
		  //				   tofDigi->GetMatch()->AddLink(1., iPntInd);
                  fStorDigiExp[iSmType][iSM*iNbRpc + iRpc][2*iSideChInd+1].push_back( tofDigi );
		  fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iSideChInd+1].push_back( iPntInd );
		     LOG(DEBUG)<<Form("Digimatch (%d,%d,%d,%d): size %zu, val %d, MCt %d",
				     iSmType,iSM,iRpc,iChannel,fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iSideChInd+1].size(),iPntInd,iTrackID)
			      <<FairLogger::endl;

                  tofDigi = new CbmTofDigiExp( iSM, iRpc, iSideChInd, dTimeB,
                        dChargeSideCh*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][2*iSideChInd]/2.0,
                        0, iSmType );
		  //				   tofDigi->GetMatch()->AddLink(1., iPntInd);
                  fStorDigiExp[iSmType][iSM*iNbRpc + iRpc][2*iSideChInd].push_back( tofDigi );
		  fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iSideChInd].push_back( iPntInd );
		     LOG(DEBUG)<<Form("Digimatch (%d,%d,%d,%d): size %zu, val %d, MCt %d",
				     iSmType,iSM,iRpc,iChannel,fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iSideChInd].size(),iPntInd,iTrackID)
			      <<FairLogger::endl;
               } // if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
               else {
                  CbmTofDigi * tofDigi = new CbmTofDigi( iSM, iRpc, iSideChInd, dTimeA,
                        dChargeSideCh*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][2*iSideChInd+1]/2.0,
                        1, iSmType );
		  //				   tofDigi->GetMatch()->AddLink(1., iPntInd);
                  fStorDigi[iSmType][iSM*iNbRpc + iRpc][2*iSideChInd+1].push_back( tofDigi );
		  fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iSideChInd+1].push_back( iPntInd );
                  tofDigi = new CbmTofDigi( iSM, iRpc, iSideChInd, dTimeB,
                        dChargeSideCh*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][2*iSideChInd]/2.0,
                        0, iSmType );
		  //				   tofDigi->GetMatch()->AddLink(1., iPntInd);
                  fStorDigi[iSmType][iSM*iNbRpc + iRpc][2*iSideChInd].push_back( tofDigi );
		  fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iSideChInd].push_back( iPntInd );
               } // else of if( kTRUE == fDigiBdfPar->UseExpandedDigi() )

               // Check next
               iSideChInd ++;

               // Channel index in this UID is in [1,nbCh] instead of [0, nbCh[
               // ... don't ask me why ...
               xDetInfo.fCell = iSideChInd + 1;

               iSideChId = fTofId->SetDetectorInfo( xDetInfo );

               fChannelInfo = fDigiPar->GetCell( iSideChId );

               // Calculate the fraction of the charge in this channel
               dChargeSideCh = f2ChargeDist->Integral(
                     fChannelInfo->GetX() - fChannelInfo->GetSizex()/2.0,
                     fChannelInfo->GetX() + fChannelInfo->GetSizex()/2.0,
                     fChannelInfo->GetY() - fChannelInfo->GetSizey()/2.0,
                     fChannelInfo->GetY() + fChannelInfo->GetSizey()/2.0 );
            } // while signal and channel index ok
         } // if( 0 < iChannel)
      } // if( 0 == iChType)
         else
         {
            // Pad (Single side readout)
            // Assume that the bottom/left pads have lower channel index
            // E.g: for a 2*4 pads counter, pads 0-3 are the bottom/left ones and 4-7 the
            // top/right one with reversed numbering
            //               -----------------
            //  row 1        | 7 | 6 | 5 | 4 |          y
            //               -----------------          ^
            //  row 0        | 0 | 1 | 2 | 3 |          |
            //               -----------------           --> x
            // or            ---------
            //               | 4 | 3 |
            //               ---------
            //               | 5 | 2 |
            //               ---------
            //               | 6 | 1 |
            //               ---------
            //               | 7 | 0 |
            //               ---------
            //        row      1   0
            // Loop in decreasing channel index in same row, find min = 1st 0 channel
            // Loop in increasing channel index in same row, find max = 1st 0 channel
            Int_t iMinChanInd = iChannel;
            Int_t iMaxChanInd = iChannel;

            Double_t dClusterDist = 0;
            Int_t    iRow;
            Bool_t   bCheckOtherRow = kFALSE;
            if( iChannel < iNbCh/2.0 )
               iRow = 0;
               else iRow = 1;

            // Loop in decreasing channel index until a channel with charge under threshold is found
            if( iRow*iNbCh/2.0 < iChannel )
            {
               Int_t iSideChInd = iChannel - 1;

               // Channel index in this UID is in [1,nbCh] instead of [0, nbCh[
               // ... don't ask me why ...
               CbmTofDetectorInfo xDetInfo(kTOF, iSmType, iSM, iRpc, 0, iSideChInd + 1);

               Int_t iSideChId = fTofId->SetDetectorInfo( xDetInfo );

               fChannelInfo = fDigiPar->GetCell( iSideChId );

               // Calculate the fraction of the charge in this channel
               Double_t dChargeSideCh = f2ChargeDist->Integral(
                     fChannelInfo->GetX() - fChannelInfo->GetSizex()/2.0,
                     fChannelInfo->GetX() + fChannelInfo->GetSizex()/2.0,
                     fChannelInfo->GetY() - fChannelInfo->GetSizey()/2.0,
                     fChannelInfo->GetY() + fChannelInfo->GetSizey()/2.0 );

               while( fDigiBdfPar->GetFeeThreshold() < dChargeSideCh &&
                       iRow*iNbCh/2.0 <= iSideChInd )
               {
                  iMinChanInd = iSideChInd;

                  Double_t dClustToReadout = 0.0;
                  Double_t dPadTime = dCentralTime;
                  // First calculate the propagation time to the center of the pad base
                  if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
                     // Vertical => base = right/left edge
                     dClustToReadout = TMath::Sqrt(  TMath::Power( vPntPos.Y() - fChannelInfo->GetY() , 2)
                            + TMath::Power( vPntPos.X() - ( fChannelInfo->GetX() + (1-2*iRow)*fChannelInfo->GetSizex()/2.0 ), 2) );
                     else  // Horizontal => base = bottom/upper edge
                        dClustToReadout = TMath::Sqrt(  TMath::Power( vPntPos.X() - fChannelInfo->GetX() , 2)
                            + TMath::Power( vPntPos.Y() - ( fChannelInfo->GetY() - (1-2*iRow)*fChannelInfo->GetSizey()/2.0 ), 2) );

                  dPadTime += fRandRes->Gaus( 0.0, fdTimeResElec) + dClustToReadout/fdSignalPropSpeed;

                  // Switch between Digi and DigiExp
                  if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
                  {
                     CbmTofDigiExp * tofDigi = new CbmTofDigiExp( iSM, iRpc, iSideChInd, dPadTime,
                           dChargeSideCh*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][iSideChInd],
                           0, iSmType );
		     //					  tofDigi->GetMatch()->AddLink(1., iPntInd);
                     fStorDigiExp[iSmType][iSM*iNbRpc + iRpc][iSideChInd].push_back( tofDigi );
		     fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][iSideChInd].push_back( iPntInd );
		     LOG(DEBUG)<<Form("Digimatch (%d,%d,%d,%d): size %zu, val %d, MCt %d",
				     iSmType,iSM,iRpc,iChannel,fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][2*iSideChInd].size(),iPntInd,iTrackID)
			      <<FairLogger::endl;

                  } // if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
                  else {
                     CbmTofDigi * tofDigi = new CbmTofDigi( iSM, iRpc, iSideChInd, dPadTime,
                           dChargeSideCh*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][iSideChInd],
                           0, iSmType );
		     //					  tofDigi->GetMatch()->AddLink(1., iPntInd);
                     fStorDigi[iSmType][iSM*iNbRpc + iRpc][iSideChInd].push_back( tofDigi );
		     fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][iSideChInd].push_back( iPntInd );
                  } // else of if( kTRUE == fDigiBdfPar->UseExpandedDigi() )

                  // Check next
                  iSideChInd --;

                  // Channel index in this UID is in [1,nbCh] instead of [0, nbCh[
                  // ... don't ask me why ...
                  xDetInfo.fCell = iSideChInd + 1;

                  iSideChId = fTofId->SetDetectorInfo( xDetInfo );

                  fChannelInfo = fDigiPar->GetCell( iSideChId );

                  // Calculate the fraction of the charge in this channel
                  dChargeSideCh = f2ChargeDist->Integral(
                        fChannelInfo->GetX() - fChannelInfo->GetSizex()/2.0,
                        fChannelInfo->GetX() + fChannelInfo->GetSizex()/2.0,
                        fChannelInfo->GetY() - fChannelInfo->GetSizey()/2.0,
                        fChannelInfo->GetY() + fChannelInfo->GetSizey()/2.0 );
               } // while channel has good charge and is not out
            } // if( iRow*iNbCh/2.0 < iChannel )
            // Loop in increasing channel index until a channel with charge under threshold is found
            if( iChannel < (1+iRow)*iNbCh/2.0 - 1)
            {
               Int_t iSideChInd = iChannel + 1;

               // Channel index in this UID is in [1,nbCh] instead of [0, nbCh[
               // ... don't ask me why ...
               CbmTofDetectorInfo xDetInfo(kTOF, iSmType, iSM, iRpc, 0, iSideChInd + 1);

               Int_t iSideChId = fTofId->SetDetectorInfo( xDetInfo );

               fChannelInfo = fDigiPar->GetCell( iSideChId );

               // Calculate the fraction of the charge in this channel
               Double_t dChargeSideCh = f2ChargeDist->Integral(
                     fChannelInfo->GetX() - fChannelInfo->GetSizex()/2.0,
                     fChannelInfo->GetX() + fChannelInfo->GetSizex()/2.0,
                     fChannelInfo->GetY() - fChannelInfo->GetSizey()/2.0,
                     fChannelInfo->GetY() + fChannelInfo->GetSizey()/2.0 );

               while( fDigiBdfPar->GetFeeThreshold() < dChargeSideCh &&
                        iSideChInd < (1+iRow)*iNbCh/2.0 )
               {
                  iMaxChanInd = iSideChInd;

                  Double_t dClustToReadout = 0.0;
                  Double_t dPadTime = dCentralTime;
                  // First calculate the propagation time to the center of the pad base
                  if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
                     // Vertical => base = right/left edge
                     dClustToReadout = TMath::Sqrt(  TMath::Power( vPntPos.Y() - fChannelInfo->GetY() , 2)
                            + TMath::Power( vPntPos.X() - ( fChannelInfo->GetX() + (1-2*iRow)*fChannelInfo->GetSizex()/2.0 ), 2) );
                     else  // Horizontal => base = bottom/upper edge
                        dClustToReadout = TMath::Sqrt(  TMath::Power( vPntPos.X() - fChannelInfo->GetX() , 2)
                            + TMath::Power( vPntPos.Y() - ( fChannelInfo->GetY() - (1-2*iRow)*fChannelInfo->GetSizey()/2.0 ), 2) );

                  dPadTime += fRandRes->Gaus( 0.0, fdTimeResElec) + dClustToReadout/fdSignalPropSpeed;

                  // Switch between Digi and DigiExp
                  if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
                  {
                     CbmTofDigiExp * tofDigi = new CbmTofDigiExp( iSM, iRpc, iSideChInd, dPadTime,
                           dChargeSideCh*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][iSideChInd],
                           0, iSmType );
		     //					  tofDigi->GetMatch()->AddLink(1., iPntInd);
                     fStorDigiExp[iSmType][iSM*iNbRpc + iRpc][iSideChInd].push_back( tofDigi );
		     fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][iSideChInd].push_back( iPntInd );
		     LOG(DEBUG)<<Form("Digimatch (%d,%d,%d,%d): size %zu, val %d, MCt %d",
				     iSmType,iSM,iRpc,iChannel,fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][iSideChInd].size(),iPntInd,iTrackID)
			      <<FairLogger::endl;
                  } // if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
                  else {
                     CbmTofDigi * tofDigi = new CbmTofDigi( iSM, iRpc, iSideChInd, dPadTime,
                           dChargeSideCh*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][iSideChInd],
                           0, iSmType );
		     //					  tofDigi->GetMatch()->AddLink(1., iPntInd);
                     fStorDigi[iSmType][iSM*iNbRpc + iRpc][iSideChInd].push_back( tofDigi );
		     fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][iSideChInd].push_back( iPntInd );
                  } // else of if( kTRUE == fDigiBdfPar->UseExpandedDigi() )

                  // Check next
                  iSideChInd ++;

                  // Channel index in this UID is in [1,nbCh] instead of [0, nbCh[
                  // ... don't ask me why ...
                  xDetInfo.fCell = iSideChInd + 1;

                  iSideChId = fTofId->SetDetectorInfo( xDetInfo );

                  fChannelInfo = fDigiPar->GetCell( iSideChId );

                  // Calculate the fraction of the charge in this channel
                  dChargeSideCh = f2ChargeDist->Integral(
                        fChannelInfo->GetX() - fChannelInfo->GetSizex()/2.0,
                        fChannelInfo->GetX() + fChannelInfo->GetSizex()/2.0,
                        fChannelInfo->GetY() - fChannelInfo->GetSizey()/2.0,
                        fChannelInfo->GetY() + fChannelInfo->GetSizey()/2.0 );
               } // while channel has good charge and is not out
            } // if( iChannel < (1+iRow)*iNbCh/2.0 - 1)

            // Test if Pad in diff row but same column as central pad has enough charge
            // if Yes => Loop from min to max equivalents in the opposite row

            // Channel index in this UID is in [1,nbCh] instead of [0, nbCh[
            // ... don't ask me why ...
            CbmTofDetectorInfo xDetInfo(kTOF, iSmType, iSM, iRpc, 0, iChannel + ( 1 - 2*iRow )*iNbCh/2.0 + 1);
            Int_t iSideChId = fTofId->SetDetectorInfo( xDetInfo );
            fChannelInfo = fDigiPar->GetCell( iSideChId );

            // Calculate the fraction of the charge in this channel
            Double_t dChargeSideCh = f2ChargeDist->Integral(
                  fChannelInfo->GetX() - fChannelInfo->GetSizex()/2.0,
                  fChannelInfo->GetX() + fChannelInfo->GetSizex()/2.0,
                  fChannelInfo->GetY() - fChannelInfo->GetSizey()/2.0,
                  fChannelInfo->GetY() + fChannelInfo->GetSizey()/2.0 );

            if( fDigiBdfPar->GetFeeThreshold() < dChargeSideCh )
            {
               bCheckOtherRow = kTRUE;

               for( Int_t iChanInd  = iMinChanInd + (1-2*iRow)*iNbCh/2.0;
                          iChanInd <= iMaxChanInd + (1-2*iRow)*iNbCh/2.0;
                          iChanInd++ )
               {
                  // Channel index in this UID is in [1,nbCh] instead of [0, nbCh[
                  // ... don't ask me why ...
                  xDetInfo.fCell = iChanInd + 1;
                  iSideChId = fTofId->SetDetectorInfo( xDetInfo );
                  fChannelInfo = fDigiPar->GetCell( iSideChId );

                  // Calculate the fraction of the charge in this channel
                  dChargeSideCh = f2ChargeDist->Integral(
                        fChannelInfo->GetX() - fChannelInfo->GetSizex()/2.0,
                        fChannelInfo->GetX() + fChannelInfo->GetSizex()/2.0,
                        fChannelInfo->GetY() - fChannelInfo->GetSizey()/2.0,
                        fChannelInfo->GetY() + fChannelInfo->GetSizey()/2.0 );

                  if( fDigiBdfPar->GetFeeThreshold() < dChargeSideCh  )
                  {
                     Double_t dClustToReadout = 0.0;
                     Double_t dPadTime = dCentralTime;
                     // First calculate the propagation time to the center of the pad base
                     if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
                        // Vertical => base = right/left edge
                        dClustToReadout = TMath::Sqrt(  TMath::Power( vPntPos.Y() - fChannelInfo->GetY() , 2)
                               + TMath::Power( vPntPos.X() - ( fChannelInfo->GetX() + (1-2*iRow)*fChannelInfo->GetSizex()/2.0 ), 2) );
                        else  // Horizontal => base = bottom/upper edge
                           dClustToReadout = TMath::Sqrt(  TMath::Power( vPntPos.X() - fChannelInfo->GetX() , 2)
                               + TMath::Power( vPntPos.Y() - ( fChannelInfo->GetY() - (1-2*iRow)*fChannelInfo->GetSizey()/2.0 ), 2) );

                     dPadTime += fRandRes->Gaus( 0.0, fdTimeResElec) + dClustToReadout/fdSignalPropSpeed;

                     // Switch between Digi and DigiExp
                     if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
                     {
                        CbmTofDigiExp * tofDigi = new CbmTofDigiExp( iSM, iRpc, iChanInd, dPadTime,
                              dChargeSideCh*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][iChanInd],
                              0, iSmType );
			//						 tofDigi->GetMatch()->AddLink(1., iPntInd);
                        fStorDigiExp[iSmType][iSM*iNbRpc + iRpc][iChanInd].push_back( tofDigi );
			fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][iChanInd].push_back( iPntInd );
			LOG(DEBUG)<<Form("Digimatch (%d,%d,%d,%d): size %zu, val %d, MCt %d",
				     iSmType,iSM,iRpc,iChannel,fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][iChanInd].size(),iPntInd,iTrackID)
			      <<FairLogger::endl;
                     } // if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
                     else {
                        CbmTofDigi * tofDigi = new CbmTofDigi( iSM, iRpc, iChanInd, dPadTime,
                              dChargeSideCh*fdChannelGain[iSmType][iSM*iNbRpc + iRpc][iChanInd],
                              0, iSmType );
			//						 tofDigi->GetMatch()->AddLink(1., iPntInd);
                        fStorDigi[iSmType][iSM*iNbRpc + iRpc][iChanInd].push_back( tofDigi );
			fStorDigiMatch[iSmType][iSM*iNbRpc + iRpc][iChanInd].push_back( iPntInd );
                     } // else of if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
                  } // if( fDigiBdfPar->GetFeeThreshold() < dChargeSideCh  )
               } // for channels on other row where same row had signal
            } // if( fDigiBdfPar->GetFeeThreshold() < dChargeSideCh )
         } // else of if( 0 == iChType)

      delete f2ChargeDist;
   } // for (Int_t iPntInd = 0; iPntInd < nTofPoint; iPntInd++ )

   if( kTRUE == fDigiBdfPar->UseOneGapPerTrk() )
   {
      // Clear the Track to channel temporary storage
      for(Int_t iTrkInd = 0; iTrkInd < nMcTracks; iTrkInd++)
         fvlTrckChAddr[iTrkInd].clear();
      fvlTrckChAddr.clear();
   } // if( kTRUE == fDigiBdfPar->UseOneGapPerTrk() )

   return kTRUE;
}
/************************************************************************************/
// Auxiliary functions
Double_t CbmTofDigitizerBDF::ComputeClusterAreaOnChannel(
      Int_t iChanId, Double_t dClustRadius,
      Double_t dClustCentX, Double_t dClustCentY)
{
   Double_t dCornersX[4]; // Corners X position
   Double_t dCornersY[4]; // Corners X position
   Double_t dCornersR[4]; // Distance from cluster center to the corners

   fChannelInfo            = fDigiPar->GetCell(iChanId);
   Double_t dChanCentPosX  = fChannelInfo->GetX();
   Double_t dChanCentPosY  = fChannelInfo->GetY();
   Double_t dEdgeCentDistX = fChannelInfo->GetSizex()/2.0;
   Double_t dEdgeCentDistY = fChannelInfo->GetSizey()/2.0;

   // Upper Left
   dCornersX[0] = dChanCentPosX - dEdgeCentDistX;
   dCornersY[0] = dChanCentPosY + dEdgeCentDistY;
   dCornersR[0] = TMath::Sqrt(  TMath::Power( dCornersX[0] - dClustCentX, 2 )
                              + TMath::Power( dCornersY[0] - dClustCentY, 2 ) );
   // Upper Right
   dCornersX[1] = dChanCentPosX + dEdgeCentDistX;
   dCornersY[1] = dChanCentPosY + dEdgeCentDistY;
   dCornersR[1] = TMath::Sqrt(  TMath::Power( dCornersX[1] - dClustCentX, 2 )
                              + TMath::Power( dCornersY[1] - dClustCentY, 2 ) );
   // Bottom Right
   dCornersX[2] = dChanCentPosX + dEdgeCentDistX;
   dCornersY[2] = dChanCentPosY - dEdgeCentDistY;
   dCornersR[2] = TMath::Sqrt(  TMath::Power( dCornersX[2] - dClustCentX, 2 )
                              + TMath::Power( dCornersY[2] - dClustCentY, 2 ) );
   // Bottom Left
   dCornersX[3] = dChanCentPosX - dEdgeCentDistX;
   dCornersY[3] = dChanCentPosY - dEdgeCentDistY;
   dCornersR[3] = TMath::Sqrt(  TMath::Power( dCornersX[3] - dClustCentX, 2 )
                              + TMath::Power( dCornersY[3] - dClustCentY, 2 ) );

   Int_t iNbCornersIn = ( dCornersR[0] < dClustRadius ? 1 : 0 ) +
                        ( dCornersR[1] < dClustRadius ? 1 : 0 ) +
                        ( dCornersR[2] < dClustRadius ? 1 : 0 ) +
                        ( dCornersR[3] < dClustRadius ? 1 : 0 );

   LOG(DEBUG3)<<"CbmTofDigitizerBDF::ComputeClusterAreaOnChannel => Check "<<iNbCornersIn<<" Radius "<<dClustRadius
            <<" "<<dCornersR[0]<<" "<<dCornersR[1]<<" "<<dCornersR[2]<<" "<<dCornersR[3]<<FairLogger::endl;

   switch( iNbCornersIn )
   {
      case 0:
      {
         // No corner inside the circle
         // => if cluster center inside channel:  0-4 disc sections sticking out
         // => if cluster center outside channel: 0-1 disc section sticking in
         Double_t dEdgeR[4];
         dEdgeR[0] = dChanCentPosX - dEdgeCentDistX - dClustCentX; // Cluster -> Left edge
         dEdgeR[1] = dChanCentPosX + dEdgeCentDistX - dClustCentX; // Cluster -> Right edge
         dEdgeR[2] = dChanCentPosY - dEdgeCentDistY - dClustCentY; // Cluster -> Lower edge
         dEdgeR[3] = dChanCentPosY + dEdgeCentDistY - dClustCentY; // Cluster -> Upper edge
         if( ( 0.0 >= dEdgeR[0] ) && ( 0.0 <= dEdgeR[1] ) &&
              ( 0.0 >= dEdgeR[2] ) && ( 0.0 <= dEdgeR[3] ) )
         {
            // Cluster center inside channel

            // First disc area
            Double_t dArea = dClustRadius * dClustRadius * TMath::Pi();

	    LOG(DEBUG3)<<"CbmTofDigitizerBDF::ComputeClusterAreaOnChannel => CC in Ch "<<dArea
		       <<FairLogger::endl;

            // Now check for each edge if it cuts the cluster circle
            // and remove the corresponding disc section if it does
            // (no overlap as no corner inside cluster)
            if( TMath::Abs( dEdgeR[0] ) < dClustRadius )
               dArea -= DiscSectionArea( dClustRadius, TMath::Abs( dEdgeR[0] ) );
            if( TMath::Abs( dEdgeR[1] ) < dClustRadius )
               dArea -= DiscSectionArea( dClustRadius, TMath::Abs( dEdgeR[1] ) );
            if( TMath::Abs( dEdgeR[2] ) < dClustRadius )
               dArea -= DiscSectionArea( dClustRadius, TMath::Abs( dEdgeR[2] ) );
            if( TMath::Abs( dEdgeR[3] ) < dClustRadius )
               dArea -= DiscSectionArea( dClustRadius, TMath::Abs( dEdgeR[3] ) );
            if(dArea < 0.0)
               LOG(ERROR)<<"CbmTofDigitizerBDF::ComputeClusterAreaOnChannel => Neg. area! "<<dArea<<" Radius "<<dClustRadius
                           <<" "<<dEdgeR[0]<<" "<<dEdgeR[1]<<" "<<dEdgeR[2]<<" "<<dEdgeR[3]<<FairLogger::endl;
            return dArea;
         } // Cluster center inside channel
            else
            {
               // Cluster center outside channel
               // At max. one of the edges can cur the cluster circle
               // If it is the case, the area on the channel is the disc section area
               // If no crossing => no common area of cluster and channel

	      LOG(DEBUG3)<<"CbmTofDigitizerBDF::ComputeClusterAreaOnChannel => CC out Ch "<<dClustRadius
                         <<" "<<dEdgeR[0]<<" "<<dEdgeR[1]<<" "<<dEdgeR[2]<<" "<<dEdgeR[3]
			 <<FairLogger::endl;

               if( TMath::Abs( dEdgeR[0] ) < dClustRadius )
                  return DiscSectionArea( dClustRadius, TMath::Abs( dEdgeR[0] ) );
               else if( TMath::Abs( dEdgeR[1] ) < dClustRadius )
                  return DiscSectionArea( dClustRadius, TMath::Abs( dEdgeR[1] ) );
               else if( TMath::Abs( dEdgeR[2] ) < dClustRadius )
                  return DiscSectionArea( dClustRadius, TMath::Abs( dEdgeR[2] ) );
               else if( TMath::Abs( dEdgeR[3] ) < dClustRadius )
                  return DiscSectionArea( dClustRadius, TMath::Abs( dEdgeR[3] ) );
               else return 0.0;
            } // Cluster center outside channel
         break;
      } // case 0
      case 1:
      {
         // 1 corner inside the circle
         // => we get a "slice" of the cluster disc
         // There are then 2 intersection points with the channel, one on each edge
         // starting at the included corner. Those 2 points and the corner form a
         // triangle. The cluster/channel intersection area is this triangle area +
         // the area of the disc section having the 2 intersections for base
         Double_t dIntPtX[2];
         Double_t dIntPtY[2];
         Double_t dArea;

         if( dCornersR[0] < dClustRadius )
         {
            // Upper Left corner inside
               // Intersection point on left edge
            dIntPtX[0] = dCornersX[0];
            dIntPtY[0] = CircleIntersectPosY( iChanId, dClustRadius,
                                              dClustCentX, dClustCentY,
                                              kFALSE );
               // Intersection point on upper edge
            dIntPtX[1] = CircleIntersectPosX( iChanId, dClustRadius,
                                              dClustCentX, dClustCentY,
                                              kTRUE );
            dIntPtY[1] = dCornersY[0];

            // First triangle area
            dArea = TriangleArea( dCornersX[0], dCornersY[0],
                                  dIntPtX[0], dIntPtY[0],
                                  dIntPtX[1], dIntPtY[1]     );
         }
         else if( dCornersR[1] < dClustRadius )
         {
            // Upper Right corner inside
               // Intersection point on Right edge
            dIntPtX[0] = dCornersX[1];
            dIntPtY[0] = CircleIntersectPosY( iChanId, dClustRadius,
                                              dClustCentX, dClustCentY,
                                              kTRUE );
               // Intersection point on upper edge
            dIntPtX[1] = CircleIntersectPosX( iChanId, dClustRadius,
                                              dClustCentX, dClustCentY,
                                              kTRUE );
            dIntPtY[1] = dCornersY[1];

            // First triangle area
            dArea = TriangleArea( dCornersX[1], dCornersY[1],
                                  dIntPtX[0], dIntPtY[0],
                                  dIntPtX[1], dIntPtY[1]     );
         }
         else if( dCornersR[2] < dClustRadius )
         {
            // Bottom Right corner inside
               // Intersection point on Right edge
            dIntPtX[0] = dCornersX[2];
            dIntPtY[0] = CircleIntersectPosY( iChanId, dClustRadius,
                                              dClustCentX, dClustCentY,
                                              kTRUE );
               // Intersection point on lower edge
            dIntPtX[1] = CircleIntersectPosX( iChanId, dClustRadius,
                                              dClustCentX, dClustCentY,
                                              kFALSE );
            dIntPtY[1] = dCornersY[2];

            // First triangle area
            dArea = TriangleArea( dCornersX[2], dCornersY[2],
                                  dIntPtX[0], dIntPtY[0],
                                  dIntPtX[1], dIntPtY[1]     );
         }
         else if( dCornersR[3] < dClustRadius )
         {
            // Bottom Left corner inside
               // Intersection point on left edge
            dIntPtX[0] = dCornersX[3];
            dIntPtY[0] = CircleIntersectPosY( iChanId, dClustRadius,
                                              dClustCentX, dClustCentY,
                                              kFALSE );
               // Intersection point on lower edge
            dIntPtX[1] = CircleIntersectPosX( iChanId, dClustRadius,
                                              dClustCentX, dClustCentY,
                                              kFALSE );
            dIntPtY[1] = dCornersY[3];

            // First triangle area
            dArea = TriangleArea( dCornersX[3], dCornersY[3],
                                  dIntPtX[0], dIntPtY[0],
                                  dIntPtX[1], dIntPtY[1]     );
         }

         // Then add disc section area
            // Compute the distance between base and cluster center
         Double_t dSecBaseD = DistanceCircleToBase( dClustRadius,
                                                    dIntPtX[0], dIntPtY[0],
                                                    dIntPtX[1], dIntPtY[1] );
            // Add computed area
         dArea += DiscSectionArea( dClustRadius, dSecBaseD );

         return dArea;
         break;
      } // case 1
      case 2:
      {
         // 2 corners inside the circle
         // => 1 edge is fully included
         // Each of the edges in the other direction has 1 intersection point
         // with the circle. The area between the included edge and the line
         // they form can be otained by summing the area of 2 triangles, using
         // 1 of the corners inside the circle.
         // For the disc section having these points for base, there are 2
         // cases. Either it is fully contained inside the channel, or it sticks
         // out, making 2 intersection points on the edge opposite to the
         // included one. In the second case the area of this second disc section
         // has to be subtracted.
         Bool_t bFirstCorner = kTRUE;
         Int_t  iCorners[2];

         iCorners[0] = -1;
         iCorners[1] = -1;
         if( dCornersR[0] < dClustRadius )
         {
            // Upper Left corner inside
            iCorners[0] = 0;
            bFirstCorner = kFALSE;
         } // if( dCornersR[0] < dClustRadius )
         if( dCornersR[1] < dClustRadius )
         {
            // Upper Right corner inside
            if( kTRUE == bFirstCorner )
            {
               iCorners[0] = 1;
               bFirstCorner = kFALSE;
            } // if( kTRUE == bFirstCorner )
               else iCorners[1] = 1;
         } // else if( dCornersR[1] < dClustRadius )
         if( dCornersR[2] < dClustRadius )
         {
            // Bottom Right corner inside
            if( kTRUE == bFirstCorner )
            {
               iCorners[0] = 2;
               bFirstCorner = kFALSE;
            } // if( kTRUE == bFirstCorner )
               else iCorners[1] = 2;
         } // else if( dCornersR[2] < dClustRadius )
         if( dCornersR[3] < dClustRadius )
         {
            // Bottom Left corner inside
            // Has to be the 2nd one if there
            iCorners[1] = 3;
         } // else if( dCornersR[3] < dClustRadius )

         LOG(DEBUG3)<<"Corners In check "<<(iCorners[0])<<" "<<(iCorners[1])<<FairLogger::endl;
         // Get the interesting points info
         Double_t dIntPtX[2];
         Double_t dIntPtY[2];
         Double_t dEdgeR;
         Int_t    iOppCorner;
         if( 0 == iCorners[0] && 1 == iCorners[1] )
         {
            LOG(DEBUG3)<<"Test upper edge"<<FairLogger::endl;
            // Upper edge
               // 1st Intersection point on left edge
            dIntPtX[0] = dCornersX[0];
            dIntPtY[0] = CircleIntersectPosY( iChanId, dClustRadius,
                                              dClustCentX, dClustCentY,
                                              kFALSE );
               // 2nd Intersection point on right edge
            dIntPtX[1] = dCornersX[1];
            dIntPtY[1] = CircleIntersectPosY( iChanId, dClustRadius,
                                              dClustCentX, dClustCentY,
                                              kTRUE );
               // Corner opposed to first intersection
            iOppCorner = 1; // <- Upper Right corner
               // Distance between out edge and cluster center
            dEdgeR = dChanCentPosY - dEdgeCentDistY - dClustCentY; // Cluster -> Lower edge
         }
         else if( 1 == iCorners[0] && 2 == iCorners[1] )
         {
            LOG(DEBUG3)<<"Test right edge"<<FairLogger::endl;
            // Right edge
               // Intersection point on upper edge
            dIntPtX[0] = CircleIntersectPosX( iChanId, dClustRadius,
                                              dClustCentX, dClustCentY,
                                              kTRUE );
            dIntPtY[0] = dCornersY[1];
               // Intersection point on lower edge
            dIntPtX[1] = CircleIntersectPosX( iChanId, dClustRadius,
                                              dClustCentX, dClustCentY,
                                              kFALSE );
            dIntPtY[1] = dCornersY[2];
               // Corner opposed to first intersection
            iOppCorner = 2; // <- Bottom Right corner
               // Distance between out edge and cluster center
            dEdgeR = dChanCentPosX - dEdgeCentDistX - dClustCentX; // Cluster -> Left edge
         }
         else if( 2 == iCorners[0] && 3 == iCorners[1] )
         {
            LOG(DEBUG3)<<"Test lower edge"<<FairLogger::endl;
            // Lower edge
               // 1st Intersection point on right edge
            dIntPtX[0] = dCornersX[2];
            dIntPtY[0] = CircleIntersectPosY( iChanId, dClustRadius,
                                              dClustCentX, dClustCentY,
                                              kTRUE );
               // 2nd Intersection point on left edge
            dIntPtX[1] = dCornersX[3];
            dIntPtY[1] = CircleIntersectPosY( iChanId, dClustRadius,
                                              dClustCentX, dClustCentY,
                                              kFALSE );
               // Corner opposed to first intersection
            iOppCorner = 3; // <- Bottom left corner
               // Distance between out edge and cluster center
            dEdgeR = dChanCentPosY + dEdgeCentDistY - dClustCentY; // Cluster -> Upper edge
         }
         else if( 0 == iCorners[0] && 3 == iCorners[1] )
         {
            LOG(DEBUG3)<<"Test left edge"<<FairLogger::endl;
            // Left edge
               // Intersection point on lower edge
            dIntPtX[0] = CircleIntersectPosX( iChanId, dClustRadius,
                                              dClustCentX, dClustCentY,
                                              kFALSE );
            dIntPtY[0] = dCornersY[3];
               // Intersection point on upper edge
            dIntPtX[1] = CircleIntersectPosX( iChanId, dClustRadius,
                                              dClustCentX, dClustCentY,
                                              kTRUE );
            dIntPtY[1] = dCornersY[0];
               // Corner opposed to first intersection
            iOppCorner = 0; // <- Upper left corner
               // Distance between out edge and cluster center
            dEdgeR = dChanCentPosX + dEdgeCentDistX - dClustCentX; // Cluster -> Right edge
         }

         // First triangle: The 2 corners and the 1st intersection
         Double_t dArea = TriangleArea( dCornersX[ iCorners[0] ], dCornersY[ iCorners[0] ],
                                        dCornersX[ iCorners[1] ], dCornersY[ iCorners[1] ],
                                        dIntPtX[0], dIntPtY[0]     );
         // Second triangle: The corners opposed to first intersection and the 2 intersections
         dArea += TriangleArea( dCornersX[iOppCorner], dCornersY[iOppCorner],
                                dIntPtX[0], dIntPtY[0],
                                dIntPtX[1], dIntPtY[1]     );

         // Now add the disc section
            // Compute the distance between base and cluster center
         Double_t dSecBaseD = DistanceCircleToBase( dClustRadius,
                                                    dIntPtX[0], dIntPtY[0],
                                                    dIntPtX[1], dIntPtY[1] );
            // Add computed area
         dArea += DiscSectionArea( dClustRadius, dSecBaseD );

         // Use the distance between the cluster center and the opposite edge to
         // check if we need to remove a part sticking out

         // Now check for this edge if it cuts the cluster circle
         // and remove the corresponding disc section if it does
         if( TMath::Abs( dEdgeR ) < dClustRadius )
            dArea -= DiscSectionArea( dClustRadius, TMath::Abs( dEdgeR ) );

         return dArea;
         break;
      } // case 2
      case 3:
      {
         // 3 corners inside the circle
         // => 2 edges fully included, 1 intersection on each of the 2 others
         // In this case the ovelapped area is equal to the full channel area
         // minus the area of the triangle formed by the out corner and the 2
         // intersection, plus the area of the disc section having the 2
         // intersection points for base.
         Int_t  iOutCorner;
         Double_t dIntPtX[2];
         Double_t dIntPtY[2];

         if( dCornersR[0] > dClustRadius )
         {
            // Upper Left corner out
            iOutCorner = 0;
            // Intersection on upper edge
            dIntPtX[0] = CircleIntersectPosX( iChanId, dClustRadius,
                                              dClustCentX, dClustCentY,
                                              kTRUE );
            dIntPtY[0] = dCornersY[0];
            // Intersection on left edge
            dIntPtX[1] = dCornersX[0];
            dIntPtY[1] = CircleIntersectPosY( iChanId, dClustRadius,
                                              dClustCentX, dClustCentY,
                                              kFALSE );
         } // if( dCornersR[0] > dClustRadius )
         else if( dCornersR[1] > dClustRadius )
         {
            // Upper Right corner out
            iOutCorner = 1;
            // Intersection on upper edge
            dIntPtX[0] = CircleIntersectPosX( iChanId, dClustRadius,
                                              dClustCentX, dClustCentY,
                                              kTRUE );
            dIntPtY[0] = dCornersY[1];
            // Intersection on right edge
            dIntPtX[1] = dCornersX[1];
            dIntPtY[1] = CircleIntersectPosY( iChanId, dClustRadius,
                                              dClustCentX, dClustCentY,
                                              kTRUE );
         } // else if( dCornersR[1] > dClustRadius )
         else if( dCornersR[2] > dClustRadius )
         {
            // Bottom Right corner out
            iOutCorner = 2;
            // Intersection on bottom edge
            dIntPtX[0] = CircleIntersectPosX( iChanId, dClustRadius,
                                              dClustCentX, dClustCentY,
                                              kFALSE );
            dIntPtY[0] = dCornersY[2];
            // Intersection on right edge
            dIntPtX[1] = dCornersX[2];
            dIntPtY[1] = CircleIntersectPosY( iChanId, dClustRadius,
                                              dClustCentX, dClustCentY,
                                              kTRUE );
         } // else if( dCornersR[2] > dClustRadius )
         else if( dCornersR[3] > dClustRadius )
         {
            // Bottom Left corner out
            iOutCorner = 3;
            // Intersection on bottom edge
            dIntPtX[0] = CircleIntersectPosX( iChanId, dClustRadius,
                                              dClustCentX, dClustCentY,
                                              kFALSE );
            dIntPtY[0] = dCornersY[3];
            // Intersection on left edge
            dIntPtX[1] = dCornersX[3];
            dIntPtY[1] = CircleIntersectPosY( iChanId, dClustRadius,
                                              dClustCentX, dClustCentY,
                                              kFALSE );
         } // else if( dCornersR[3] > dClustRadius )

         // First get full channel area
         Double_t dArea = ( fChannelInfo->GetSizex() ) * ( fChannelInfo->GetSizey() );

         // Then subtract the triangle area
         dArea -= TriangleArea( dCornersX[iOutCorner], dCornersY[iOutCorner],
                                dIntPtX[0], dIntPtY[0],
                                dIntPtX[1], dIntPtY[1]     );

         // Finally add the disc section area
            // Compute the distance between base and cluster center
         Double_t dSecBaseD = DistanceCircleToBase( dClustRadius,
                                                    dIntPtX[0], dIntPtY[0],
                                                    dIntPtX[1], dIntPtY[1] );
            // Add computed area
         dArea += DiscSectionArea( dClustRadius, dSecBaseD );

         return dArea;
         break;
      } // case 3
      case 4:
      {
         // All in circle => channel fully contained!
         // => Area of Cluster/channel intersection = channel area
         return ( fChannelInfo->GetSizex() ) * ( fChannelInfo->GetSizey() );
         break;
      } // case 4
      default:
         // Should never happen !!!
         return 0;
         break;
   } // switch( iNbCornersIn )
}
/************************************************************************************/
// Auxiliary functions
   // Area
Double_t CbmTofDigitizerBDF::TriangleArea( Double_t dXa, Double_t dYa, Double_t dXb, Double_t dYb,
                                             Double_t dXc, Double_t dYc )
{
   Double_t dArea = 0.5*( (dXa - dXc)*(dYb - dYa) -(dXa - dXb)*(dYc - dYa) );
   return TMath::Abs( dArea );
}
Double_t CbmTofDigitizerBDF::DiscSectionArea( Double_t dDiscRadius, Double_t dDistBaseToCenter)
{
   if( dDiscRadius < dDistBaseToCenter ||
         dDiscRadius <= 0 ||
         dDistBaseToCenter < 0 )
   {
      LOG(ERROR)<<"CbmTofDigitizerBDF::DiscSectionArea => Invalid Input values!! Disc radius "
                <<dDiscRadius<<" and Base distance "<<dDistBaseToCenter<<FairLogger::endl;
      return 0.0;
   }
   // First Half disc area
   Double_t dArea = dDiscRadius * dDiscRadius * TMath::Pi() / 2.0;

   // Then remove the "rectangle" middle part of the area between base and center
   dArea -= dDistBaseToCenter * TMath::Sqrt(  dDiscRadius * dDiscRadius
                                            - dDistBaseToCenter * dDistBaseToCenter );
   // Finally remove the "arc like" side parts of the area between base and center
   dArea -= dDiscRadius * dDiscRadius * TMath::ASin( dDistBaseToCenter / dDiscRadius );

   return dArea;
}
   //----------------------------------------------------------------------------//
   // Points
Double_t CbmTofDigitizerBDF::CircleIntersectPosX( Int_t iChanId, Double_t dClustRadius,
                                                     Double_t dClustCentX, Double_t dClustCentY,
                                                     Bool_t bUpperSide )
{
   fChannelInfo            = fDigiPar->GetCell(iChanId);
   Double_t dChanCentPosX  = fChannelInfo->GetX();
   Double_t dChanCentPosY  = fChannelInfo->GetY();
   Double_t dEdgeCentDistX = fChannelInfo->GetSizex()/2.0;
   Double_t dEdgeCentDistY = fChannelInfo->GetSizey()/2.0;

   Double_t dEdgePosY =  dChanCentPosY
                       + ( kTRUE == bUpperSide? 1: -1)*dEdgeCentDistY;

   if( dChanCentPosX == dClustCentX )
   {
      // Clustered centered on channel center
      // => a single intersection means that the distance between cluster and edge is exactly
      //    the radius
      if( TMath::Abs(dEdgePosY - dClustCentY) == dClustRadius )
         // => Intersection at same X position as channel center
         return dChanCentPosX;
         // Other values mean 0 or 2 intersections in edge range
         // => return 0.0, faulty call to this function
         else
         {
            LOG(ERROR)<<"CbmTofDigitizerBDF::CircleIntersectPosX => Invalid values: "
                      <<" 0 or 2 intersections with cluster aligned on channel "<<FairLogger::endl;
            return 0.0;
         } // else of if( dEdgeCentDistY == dClustRadius )
   } // if( dChanCentPosX == dClustCentX )
   else
   {
      Double_t dRoot =  dClustRadius * dClustRadius
                      - TMath::Power( dClustCentY - dEdgePosY, 2 );
      if( 0.0 <= dRoot )
      {
         // Null root means 1 single solution
         // Positive root means 2 solutions
         /*
         // If the circle center is more to the left than the channel center and
         // there are 2 solutions, assuming only 1 intersection exists inside the
         // edge boundaries, it will be the rightmost one.
         // If the circle center is more to the left than the channel center, it
         // will be the leftmost one.
         */
         Double_t dDistX = TMath::Sqrt( dRoot );
         Double_t dSign = ( (dClustCentX - dDistX > dChanCentPosX - dEdgeCentDistX) &&
                            (dClustCentX - dDistX < dChanCentPosX + dEdgeCentDistX)?
                                  -1.0 : 1.0);
         return dClustCentX + dSign * dDistX;
      } // if( 0.0 <= dRoot )
      // Negative root means no intersection at all between
      // the circle and the line generated by this edge!!
      // => return 0.0, faulty call to this function
         else
         {
            LOG(ERROR)<<"CbmTofDigitizerBDF::CircleIntersectPosX => Invalid values: "
                      <<" 0 intersection at all (negative root = "<<dRoot
                      <<") "<<FairLogger::endl;
            TString sTemp = Form( " Radius = %5.4f ClustX = %6.3f ClustY = %6.3f Side = %1d EdgeY = %6.3f ChanX = %6.3f ChanY = %6.3f Channel = %6d",
                  dClustRadius, dClustCentX, dClustCentY, bUpperSide, dEdgePosY, dChanCentPosX, dChanCentPosY, iChanId );
            LOG(ERROR)<<"CbmTofDigitizerBDF::CircleIntersectPosX => Input values: "
                      <<sTemp
                      <<FairLogger::endl;
            return 0.0;
         } // else of if( 0.0 <= dRoot )
   } // else of if( dChanCentPosX == dClustCentX )
}
Double_t CbmTofDigitizerBDF::CircleIntersectPosY( Int_t iChanId, Double_t dClustRadius,
                                                     Double_t dClustCentX, Double_t dClustCentY,
                                                     Bool_t bRightSide )
{
   fChannelInfo            = fDigiPar->GetCell(iChanId);
   Double_t dChanCentPosX  = fChannelInfo->GetX();
   Double_t dChanCentPosY  = fChannelInfo->GetY();
   Double_t dEdgeCentDistX = fChannelInfo->GetSizex()/2.0;
   Double_t dEdgeCentDistY = fChannelInfo->GetSizey()/2.0;

   Double_t dEdgePosX =  dChanCentPosX
                       + ( kTRUE == bRightSide? 1: -1)*dEdgeCentDistX;

   if( dChanCentPosY == dClustCentY )
   {
      // Clustered centered on channel center
      // => a single intersection means that the distance between cluster and edge is exactly
      //    the radius
      if( TMath::Abs(dEdgePosX - dClustCentX) == dClustRadius )
         // => Intersection at same X position as channel center
         return dChanCentPosY;
         // Other values mean 0 or 2 intersections in edge range
         // => return 0.0, faulty call to this function
         else
         {
            LOG(ERROR)<<"CbmTofDigitizerBDF::CircleIntersectPosY => Invalid values: "
                      <<" 0 or 2 intersections with cluster aligned on channel "<<FairLogger::endl;
            return 0.0;
         } // else of if( dEdgeCentDistX == dClustRadius )
   } // if( dChanCentPosX == dClustCentX )
   else
   {
      Double_t dRoot =  dClustRadius * dClustRadius
                      - TMath::Power( dClustCentX - dEdgePosX, 2 );
      if( 0.0 <= dRoot )
      {
         // Null root means 1 single solution
         // Positive root means 2 solutions
         /*
         // If the circle center is higher than the channel center and
         // there are 2 solutions, assuming only 1 intersection exists inside the
         // edge boundaries, it will be lower one.
         // If the circle center is more to the left than the channel center, it
         // will be the higher one.
         Double_t dSign = ( dClustCentY > dChanCentPosY ? -1.0 : 1.0);

         return dChanCentPosY + dSign * TMath::Sqrt( dRoot );
         */
         Double_t dDistY = TMath::Sqrt( dRoot );
         Double_t dSign = ( (dClustCentY - dDistY > dChanCentPosY - dEdgeCentDistY) &&
                            (dClustCentY - dDistY < dChanCentPosY + dEdgeCentDistY)?
                                  -1.0 : 1.0);
         return dClustCentY + dSign * dDistY;
      } // if( 0.0 <= dRoot )
      // Negative root means no intersection at all between
      // the circle and the line generated by this edge!!
      // => return 0.0, faulty call to this function
         else
         {
            LOG(ERROR)<<"CbmTofDigitizerBDF::CircleIntersectPosY => Invalid values: "
                      <<" 0 intersection at all (negative root = "<<dRoot
                      <<") "<<FairLogger::endl;
            TString sTemp = Form( " Radius = %5.4f ClustX = %6.3f ClustY = %6.3f Side = %1d EdgeX = %6.3f ChanX = %6.3f ChanY = %6.3f Channel = %6d",
                  dClustRadius, dClustCentX, dClustCentY, bRightSide, dEdgePosX, dChanCentPosX, dChanCentPosY, iChanId );
            LOG(ERROR)<<"CbmTofDigitizerBDF::CircleIntersectPosY => Input values: "
                      <<sTemp
                      <<FairLogger::endl;
            return 0.0;
         } // else of if( 0.0 <= dRoot )
   } // else of if( dChanCentPosX == dClustCentX )
}
Double_t  CbmTofDigitizerBDF::DistanceCircleToBase(
      Double_t dClustRadius,
      Double_t dBaseXa, Double_t dBaseYa, Double_t dBaseXb, Double_t dBaseYb )
{
   // Both base endpoints should be on the circle, forming an isoscele triangle
   // with the circle center.
   // The distance to the base is then the height of the isoscele triangle.
   Double_t dBaseLength = TMath::Sqrt( TMath::Power( dBaseXb - dBaseXa, 2 ) +
                                       TMath::Power( dBaseYb - dBaseYa, 2 ) );
   Double_t dRoot = dClustRadius * dClustRadius - dBaseLength * dBaseLength / 4;
   if( 0.0 <= dRoot )
      return TMath::Sqrt( dRoot  );
      else
      {
         LOG(ERROR)<<"CbmTofDigitizerBDF::DistanceCircleToBase => Invalid values: "
                   <<" base end-points not on circle (negative root"<<dRoot
                      <<") "<<FairLogger::endl;
         TString sTemp = Form( " Radius = %5.4f Xa = %6.3f Ya = %6.3f Xb = %6.3f Yb = %6.3f ",
               dClustRadius, dBaseXa, dBaseYa, dBaseXb, dBaseYb );
         LOG(ERROR)<<"CbmTofDigitizerBDF::DistanceCircleToBase => Input values: "
                   <<sTemp
                   <<FairLogger::endl;
         return 0.0;
      } // else of if( 0.0 <= dRoot )
}
/************************************************************************************/
