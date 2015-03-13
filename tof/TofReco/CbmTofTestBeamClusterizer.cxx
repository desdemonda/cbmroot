/** @file CbmTofTestBeamClusterizer.cxx 
/** author nh  
/** adopted from
/** @file CbmTofSimpClusterizer.cxx 
 ** @author Pierre-Alain Loizeau <loizeau@physi.uni-heidelberg.de>
 ** @date 23.08.2013
 **/

#include "CbmTofTestBeamClusterizer.h"

// TOF Classes and includes
#include "CbmTofPoint.h"      // in cbmdata/tof
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

//#include "TMbsMappingTof.h"   // in unpack/tof/mapping
#include "TMbsMappingTofPar.h"


// CBMroot classes and includes
#include "CbmMCTrack.h"

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
#include "TProfile.h"
#include "TDirectory.h"
#include "TROOT.h"

const Int_t DetMask = 4194303;
const Int_t nbClWalkBinX=20;
const Int_t nbClWalkBinY=41;  // choose odd number to have central bin symmetric around 0
Double_t TOTMax=1.E5; 
const Double_t TTotMean=3.E4; 

const Int_t iNTrg=1;

// C++ Classes and includes

/************************************************************************************/
CbmTofTestBeamClusterizer::CbmTofTestBeamClusterizer():
   FairTask("CbmTofTestBeamClusterizer"),
   fVerbose(1),
   fGeoHandler(new CbmTofGeoHandler()),
   fDigiPar(NULL),
   fDigiBdfPar(NULL),
   fChannelInfo(NULL),
   fTofPointsColl(NULL),
   fMcTracksColl(NULL),
   fbWriteHitsInOut(kTRUE),
   fTofDigisColl(NULL),
   fTofDigiMatchColl(NULL)
{

}

CbmTofTestBeamClusterizer::CbmTofTestBeamClusterizer(const char *name, Int_t verbose, Bool_t writeDataInOut):
   FairTask(TString(name),verbose),
   fVerbose(verbose),
   fGeoHandler(new CbmTofGeoHandler()),
   fDigiPar(NULL),
   fDigiBdfPar(NULL),
   fChannelInfo(NULL),
   fTofPointsColl(NULL),
   fMcTracksColl(NULL),
   fbWriteHitsInOut(writeDataInOut),
   fTofDigisColl(NULL),
   fTofDigiMatchColl(NULL)
{
  //  LOG(INFO) << "CbmTofTestBeamClusterizer initializing..."<<FairLogger::endl;
}

CbmTofTestBeamClusterizer::~CbmTofTestBeamClusterizer()
{
   if( fGeoHandler )
      delete fGeoHandler;
//   DeleteHistos(); // <-- if needed  ?
}

/************************************************************************************/
// FairTasks inherited functions
InitStatus CbmTofTestBeamClusterizer::Init()
{
   if( kFALSE == RegisterInputs() )
      return kFATAL;

   if( kFALSE == RegisterOutputs() )
      return kFATAL;

   if( kFALSE == InitParameters() )
      return kFATAL;

   if( kFALSE == LoadGeometry() )
      return kFATAL;

   if( kFALSE == InitCalibParameter() )
      return kFATAL;

   if( kFALSE == CreateHistos() )
      return kFATAL;

   return kSUCCESS;
}


void CbmTofTestBeamClusterizer::SetParContainers()
{
   LOG(INFO)<<" CbmTofTestBeamClusterizer::SetParContainers => Get the digi parameters for tof"<<FairLogger::endl;
   LOG(WARNING)<<" CbmTofTestBeamClusterizer::SetParContainers: Return without action"<<FairLogger::endl;
   return;
   // Get Base Container
   FairRunAna* ana = FairRunAna::Instance();
   FairRuntimeDb* rtdb=ana->GetRuntimeDb();

   fDigiPar = (CbmTofDigiPar*) (rtdb->getContainer("CbmTofDigiPar"));

   LOG(INFO)<<"  CbmTofTestBeamClusterizer::SetParContainers found " 
            << fDigiPar->GetNrOfModules() << " cells " <<FairLogger::endl;
   fDigiBdfPar = (CbmTofDigiBdfPar*)
              (rtdb->getContainer("CbmTofDigiBdfPar"));
}


void CbmTofTestBeamClusterizer::Exec(Option_t * option)
{
   fTofHitsColl->Clear("C");

   fiNbHits = 0;

   LOG(DEBUG)<<"CbmTofTestBeamClusterizer::Exec => New event"<<FairLogger::endl;
   fStart.Set();

   BuildClusters();

   fStop.Set();

   FillHistos();
}

void CbmTofTestBeamClusterizer::Finish()
{
   WriteHistos();
   // Prevent them from being sucked in by the CbmHadronAnalysis WriteHistograms method
   DeleteHistos();
}

/************************************************************************************/
// Functions common for all clusters approximations
Bool_t   CbmTofTestBeamClusterizer::RegisterInputs()
{
   FairRootManager *fManager = FairRootManager::Instance();
   /*
   fTofPointsColl  = (TClonesArray *) fManager->GetObject("TofPoint");
   if( NULL == fTofPointsColl)
   {
      LOG(ERROR)<<"CbmTofTestBeamClusterizer::RegisterInputs => Could not get the TofPoint TClonesArray!!!"<<FairLogger::endl;
      return kFALSE;
   } // if( NULL == fTofPointsColl)

   fMcTracksColl   = (TClonesArray *) fManager->GetObject("MCTrack");
   if( NULL == fMcTracksColl)
   {
      LOG(ERROR)<<"CbmTofTestBeamClusterizer::RegisterInputs => Could not get the MCTrack TClonesArray!!!"<<FairLogger::endl;
      return kFALSE;
   } // if( NULL == fMcTracksColl)
   */

   fTofDigisColl   = (TClonesArray *) fManager->GetObject("CbmTofDigiExp");

   if( NULL == fTofDigisColl)
      fTofDigisColl = (TClonesArray *) fManager->GetObject("CbmTofDigi");
  
   if( NULL == fTofDigisColl)
   {
      LOG(ERROR)<<"CbmTofTestBeamClusterizer::RegisterInputs => Could not get the CbmTofDigi TClonesArray!!!"<<FairLogger::endl;
      return kFALSE;
   } // if( NULL == fTofDigisColl)

   return kTRUE;
}
Bool_t   CbmTofTestBeamClusterizer::RegisterOutputs()
{
   FairRootManager* rootMgr = FairRootManager::Instance();

   fTofHitsColl = new TClonesArray("CbmTofHit");

   // Flag check to control whether digis are written in output root file
   rootMgr->Register( "TofHit","Tof", fTofHitsColl, fbWriteHitsInOut);

   fTofDigiMatchColl = new TClonesArray("CbmMatch",100);
   rootMgr->Register( "TofDigiMatch","Tof", fTofDigiMatchColl, kFALSE);

   return kTRUE;

}
Bool_t   CbmTofTestBeamClusterizer::InitParameters()
{

   // Initialize the TOF GeoHandler
   Bool_t isSimulation=kFALSE;
   LOG(INFO)<<"CbmTofTestBeamClusterizer::InitParameters - Geometry, Mapping, ...  ??"
             <<FairLogger::endl;

   // Get Base Container
   FairRun* ana = FairRun::Instance();
   FairRuntimeDb* rtdb=ana->GetRuntimeDb();   

   Int_t iGeoVersion = fGeoHandler->Init(isSimulation);
   if( k14a > iGeoVersion )
   {
      LOG(ERROR)<<"CbmTofTestBeamClusterizer::InitParameters => Only compatible with geometries after v14a !!!"
                <<FairLogger::endl;
      return kFALSE;
   }
   fTofId = new CbmTofDetectorId_v14a();
   
   // Mapping parameter
   fMbsMappingPar = (TMbsMappingTofPar*) (rtdb->getContainer("TMbsMappingTofPar"));
   if( 0 == fMbsMappingPar )
   {
      LOG(ERROR)<<"CbmTofTestBeamClusterizer::InitParameters => Could not obtain the TMbsMappingTofPar "<<FairLogger::endl;
      return kFALSE; 
   }

   fDigiPar = (CbmTofDigiPar*) (rtdb->getContainer("CbmTofDigiPar"));
   if( 0 == fDigiPar )
   {
      LOG(ERROR)<<"CbmTofTestBeamClusterizer::InitParameters => Could not obtain the CbmTofDigiPar "<<FairLogger::endl;
      return kFALSE; 
   }

   fDigiBdfPar = (CbmTofDigiBdfPar*) (rtdb->getContainer("CbmTofDigiBdfPar"));
   if( 0 == fDigiBdfPar )
   {
      LOG(ERROR)<<"CbmTofTestBeamClusterizer::InitParameters => Could not obtain the CbmTofDigiBdfPar "<<FairLogger::endl;
      return kFALSE; 
   }

   rtdb->initContainers(  ana->GetRunId() );

   LOG(INFO)<<"CbmTofTestBeamClusterizer::InitParameter: currently " 
            << fDigiPar->GetNrOfModules() << " digi cells " <<FairLogger::endl;

   
   return kTRUE;
}
/************************************************************************************/
Bool_t   CbmTofTestBeamClusterizer::InitCalibParameter()
{
  // dimension and initialize calib parameter
  Int_t iNbDet=fMbsMappingPar->GetNbMappedDet();

  Int_t iNbSmTypes = fDigiBdfPar->GetNbSmTypes();

  fvCPTOff.resize( iNbSmTypes );
  fvCPTotGain.resize( iNbSmTypes );
  fvCPWalk.resize( iNbSmTypes );
  for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
  {
      Int_t iNbSm  = fDigiBdfPar->GetNbSm(  iSmType);
      Int_t iNbRpc = fDigiBdfPar->GetNbRpc( iSmType);
      fvCPTOff[iSmType].resize( iNbSm*iNbRpc );
      fvCPTotGain[iSmType].resize( iNbSm*iNbRpc );
      fvCPWalk[iSmType].resize( iNbSm*iNbRpc );
      for( Int_t iSm = 0; iSm < iNbSm; iSm++ )
      {
        for( Int_t iRpc = 0; iRpc < iNbRpc; iRpc++ )
        {
          Int_t iNbChan = fDigiBdfPar->GetNbChan( iSmType, iRpc );
	  fvCPTOff[iSmType][iSm*iNbRpc+iRpc].resize( iNbChan );
	  fvCPTotGain[iSmType][iSm*iNbRpc+iRpc].resize( iNbChan );
	  fvCPWalk[iSmType][iSm*iNbRpc+iRpc].resize( iNbChan );
	  Int_t nbSide  =2 - fDigiBdfPar->GetChanType( iSmType, iRpc );
	  for (Int_t iCh=0; iCh<iNbChan; iCh++)
	  {
	    fvCPTOff[iSmType][iSm*iNbRpc+iRpc][iCh].resize( nbSide );
	    fvCPTotGain[iSmType][iSm*iNbRpc+iRpc][iCh].resize( nbSide );
	    fvCPWalk[iSmType][iSm*iNbRpc+iRpc][iCh].resize( nbSide );	  
	    for(Int_t iSide=0; iSide<nbSide; iSide++){
	      fvCPTOff[iSmType][iSm*iNbRpc+iRpc][iCh][iSide]=0.;
	      fvCPTotGain[iSmType][iSm*iNbRpc+iRpc][iCh][iSide]=1.;
              fvCPWalk[iSmType][iSm*iNbRpc+iRpc][iCh][iSide].resize( nbClWalkBinX );
	      for(Int_t iWx=0; iWx<nbClWalkBinX; iWx++){
		fvCPWalk[iSmType][iSm*iNbRpc+iRpc][iCh][iSide][iWx]=0.;
	      }
	    }    
	  }
	}
      }
  }

  LOG(INFO)<<"CbmTofTestBeamClusterizer::InitCalibParameter: defaults set"
	   <<FairLogger::endl;

  TDirectory * oldir = gDirectory; // <= To prevent histos from being sucked in by the param file of the TRootManager!
  /*
  gROOT->cd(); // <= To prevent histos from being sucked in by the param file of the TRootManager !
  */

  if(0<fCalMode){
    LOG(INFO) << "CbmTofTestBeamClusterizer::InitCalibParameter: read histos from "
		 << "file " << fCalParFileName << FairLogger::endl;

  // read parameter from histos
    if(fCalParFileName.IsNull()) return kTRUE;

    fCalParFile = new TFile(fCalParFileName,"");
    if(NULL == fCalParFile) {
      LOG(ERROR) << "CbmTofTestBeamClusterizer::InitCalibParameter: "
		 << "file " << fCalParFileName << " does not exist!" << FairLogger::endl;
      return kTRUE;
    }
    /*
    gDirectory->Print();
    fCalParFile->cd();
    fCalParFile->ls();
    */

    for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
    {
      Int_t iNbSm  = fDigiBdfPar->GetNbSm(  iSmType );
      Int_t iNbRpc = fDigiBdfPar->GetNbRpc( iSmType );
      for( Int_t iSm = 0; iSm < iNbSm; iSm++ )
        for( Int_t iRpc = 0; iRpc < iNbRpc; iRpc++ )
        {
	  TH2F *htempPos_pfx =(TH2F*) gDirectory->FindObjectAny( Form("cl_SmT%01d_sm%03d_rpc%03d_Pos_pfx",iSmType,iSm,iRpc));
	  TH2F *htempTOff_pfx=(TH2F*) gDirectory->FindObjectAny( Form("cl_SmT%01d_sm%03d_rpc%03d_TOff_pfx",iSmType,iSm,iRpc));
	  TH2F *htempTot_pfx =(TH2F*) gDirectory->FindObjectAny( Form("cl_SmT%01d_sm%03d_rpc%03d_Tot_pfx",iSmType,iSm,iRpc));
          if(NULL != htempPos_pfx && NULL != htempTOff_pfx && NULL != htempTot_pfx)  
	  {
	    Int_t iNbCh = fDigiBdfPar->GetNbChan( iSmType, iRpc );
            Int_t iNbinTot = htempTot_pfx->GetNbinsX();
	    for( Int_t iCh = 0; iCh < iNbCh; iCh++ )
	      {
		Double_t YMean=((TProfile *)htempPos_pfx)->GetBinContent(iCh+1);  //nh +1 empirical(?)
		Double_t TMean=((TProfile *)htempTOff_pfx)->GetBinContent(iCh+1);
                Double_t dTYOff=YMean/fDigiBdfPar->GetSignalSpeed() ;
		fvCPTOff[iSmType][iSm*iNbRpc+iRpc][iCh][0] += -dTYOff + TMean ;
		fvCPTOff[iSmType][iSm*iNbRpc+iRpc][iCh][1] += +dTYOff + TMean ;
 
		Double_t TotMean=((TProfile *)htempTot_pfx)->GetBinContent(iCh+1);  //nh +1 empirical(?)
		if(1<TotMean){
		  fvCPTotGain[iSmType][iSm*iNbRpc+iRpc][iCh][0] *= TTotMean / TotMean;
		  fvCPTotGain[iSmType][iSm*iNbRpc+iRpc][iCh][1] *= TTotMean / TotMean;
		}
		LOG(DEBUG1)<<"CbmTofTestBeamClusterizer::InitCalibParameter:" 
			   <<" SmT "<< iSmType<<" Sm "<<iSm<<" Rpc "<<iRpc<<" Ch "<<iCh
			   <<": YMean "<<YMean<<", TMean "<< TMean
			   <<" -> " << fvCPTOff[iSmType][iSm*iNbRpc+iRpc][iCh][0]
			   <<", "   << fvCPTOff[iSmType][iSm*iNbRpc+iRpc][iCh][1]
			   <<", "   << fvCPTotGain[iSmType][iSm*iNbRpc+iRpc][iCh][0]
			   <<", NbinTot "<< iNbinTot
			   <<FairLogger::endl;

		TH1D *htempWalk_px=(TH1D*)gDirectory->FindObjectAny( 
			         Form("cl_SmT%01d_sm%03d_rpc%03d_Ch%03d_Walk_px", iSmType, iSm, iRpc, iCh ));
		if(NULL != htempWalk_px) { // reinitialize Walk array 
		  LOG(DEBUG)<<"Initialize Walk correction for "
			    <<Form(" SmT%01d_sm%03d_rpc%03d_Ch%03d",iSmType, iSm, iRpc, iCh)
			    <<FairLogger::endl;
                  if(htempWalk_px->GetNbinsX() != nbClWalkBinX) 
		    LOG(ERROR)<<"CbmTofTestBeamClusterizer::InitCalibParameter: Inconsistent Walk histograms"
			      <<FairLogger::endl;
                   for( Int_t iBin = 0; iBin < nbClWalkBinX; iBin++ )
		   {
		     fvCPWalk[iSmType][iSm*iNbRpc+iRpc][iCh][0][iBin]=htempWalk_px->GetBinContent(iBin+1)*0.5;
		     fvCPWalk[iSmType][iSm*iNbRpc+iRpc][iCh][1][iBin]=htempWalk_px->GetBinContent(iBin+1)*0.5;
		     LOG(DEBUG1)<<Form(" SmT%01d_sm%03d_rpc%03d_Ch%03d bin %d walk %f ",iSmType, iSm, iRpc, iCh, iBin,
				       fvCPWalk[iSmType][iSm*iNbRpc+iRpc][iCh][0][iBin])
				<<FairLogger::endl;

		   }
		}
	    }
	  }
	  else {
	    LOG(ERROR)<<" Histos " <<  Form("cl_SmT%01d_sm%03d_rpc%03d_XXX", iSmType, iSm, iRpc) << " not found "
		      <<FairLogger::endl;
	  }
	}
    }
  }
  //   fCalParFile->Delete();
  gDirectory->cd( oldir->GetPath() ); // <= To prevent histos from being sucked in by the param file of the TRootManager!
  LOG(INFO)<<"CbmTofTestBeamClusterizer::InitCalibParameter: initialization done"
	   <<FairLogger::endl; 
  return kTRUE;
}
/************************************************************************************/
Bool_t   CbmTofTestBeamClusterizer::LoadGeometry()
{
   LOG(INFO)<<"CbmTofTestBeamClusterizer::LoadGeometry starting for  "
	    <<fMbsMappingPar->GetNbMappedDet() << " mapped detectors, "
            <<fDigiPar->GetNrOfModules() << " geometrically known ones "
            <<FairLogger::endl;

   //fTRefMode=0x00002016;  // use plastic 1 as time reference 

   Int_t iNrOfCells = fDigiPar->GetNrOfModules();
   LOG(INFO)<<"Digi Parameter container contains "<<iNrOfCells<<" cells."<<FairLogger::endl;
   for (Int_t icell = 0; icell < iNrOfCells; ++icell) {

     Int_t cellId = fDigiPar->GetCellId(icell); // cellId is assigned in CbmTofCreateDigiPar
     fChannelInfo = fDigiPar->GetCell(cellId);

     Int_t smtype  = fGeoHandler->GetSMType(cellId);
     Int_t smodule = fGeoHandler->GetSModule(cellId);
     Int_t module  = fGeoHandler->GetCounter(cellId);
     Int_t cell    = fGeoHandler->GetCell(cellId);

     Double_t x = fChannelInfo->GetX();
     Double_t y = fChannelInfo->GetY();
     Double_t z = fChannelInfo->GetZ();
     Double_t dx = fChannelInfo->GetSizex();
     Double_t dy = fChannelInfo->GetSizey();
     LOG(DEBUG1)
       << "-I- InitPar "<<icell<<" Id: "<< Form("0x%08x",cellId)
	    << " "<< cell << " tmcs: "<< smtype <<" "<<smodule<<" "<<module<<" "<<cell   
            << " x="<<Form("%6.2f",x)<<" y="<<Form("%6.2f",y)<<" z="<<Form("%6.2f",z)
            <<" dx="<<dx<<" dy="<<dy<<FairLogger::endl;
   }

   Int_t iNbDet=fMbsMappingPar->GetNbMappedDet();
   for(Int_t iDetIndx=0; iDetIndx<iNbDet; iDetIndx++){
       Int_t iUniqueId = fMbsMappingPar->GetMappedDetUId( iDetIndx );
       Int_t iSmType   = CbmTofAddress::GetSmType( iUniqueId );
       Int_t iSmId     = CbmTofAddress::GetSmId( iUniqueId );
       Int_t iRpcId    = CbmTofAddress::GetRpcId( iUniqueId );
       LOG(INFO) << " DetIndx "<<iDetIndx<<", SmType "<<iSmType<<", SmId "<<iSmId
                  << ", RpcId "<<iRpcId<<" => UniqueId "<<Form("0x%08x ",iUniqueId)
                  <<FairLogger::endl;
       Int_t iCell=-1;
       while (kTRUE){
	 Int_t iUCellId= CbmTofAddress::GetUniqueAddress(iSmId,iRpcId,++iCell,0,iSmType);
         fChannelInfo = fDigiPar->GetCell(iUCellId);
         if (NULL == fChannelInfo) break;
	 LOG(DEBUG) << " Cell " << iCell << Form(" 0x%08x ",iUCellId)
                   << Form(", fCh 0x%08x ",fChannelInfo)
                   << ", x: " << fChannelInfo->GetX()
                   << ", y: " << fChannelInfo->GetY()
                   << ", z: " << fChannelInfo->GetZ()
                   << FairLogger::endl; 
	 }

   }

   //   return kTRUE;

   Int_t iNbSmTypes = fDigiBdfPar->GetNbSmTypes();

   if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
   {
      fStorDigiExp.resize( iNbSmTypes );
      fStorDigiInd.resize( iNbSmTypes );
      fviClusterSize.resize( iNbSmTypes );
      fviTrkMul.resize( iNbSmTypes );
      fvdX.resize( iNbSmTypes );
      fvdY.resize( iNbSmTypes );
      fvdDifX.resize( iNbSmTypes );
      fvdDifY.resize( iNbSmTypes );
      fvdDifCh.resize( iNbSmTypes );
      fviClusterMul.resize( iNbSmTypes );
      for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
      {
         Int_t iNbSm  = fDigiBdfPar->GetNbSm(  iSmType);
         Int_t iNbRpc = fDigiBdfPar->GetNbRpc( iSmType);
         fStorDigiExp[iSmType].resize( iNbSm*iNbRpc );
         fStorDigiInd[iSmType].resize( iNbSm*iNbRpc );
         fviClusterSize[iSmType].resize( iNbRpc );
         fviTrkMul[iSmType].resize( iNbRpc );
         fvdX[iSmType].resize( iNbRpc );
         fvdY[iSmType].resize( iNbRpc );
         fvdDifX[iSmType].resize( iNbRpc );
         fvdDifY[iSmType].resize( iNbRpc );
         fvdDifCh[iSmType].resize( iNbRpc );
         for( Int_t iSm = 0; iSm < iNbSm; iSm++ )
         {
           fviClusterMul[iSmType].resize( iNbSm );
           for( Int_t iRpc = 0; iRpc < iNbRpc; iRpc++ )
            {
 	       fviClusterMul[iSmType][iSm].resize( iNbRpc );
               Int_t iNbChan = fDigiBdfPar->GetNbChan( iSmType, iRpc );
               LOG(DEBUG)<<"CbmTofTestBeamClusterizer::LoadGeometry: StoreDigi with "
			 << Form(" %3d %3d %3d %3d %5d ",iSmType,iSm,iNbRpc,iRpc,iNbChan)
                         << FairLogger::endl;
               fStorDigiExp[iSmType][iSm*iNbRpc+iRpc].resize( iNbChan );
               fStorDigiInd[iSmType][iSm*iNbRpc+iRpc].resize( iNbChan );
            } // for( Int_t iRpc = 0; iRpc < iNbRpc; iRpc++ )
         } // for( Int_t iSm = 0; iSm < iNbSm; iSm++ )
      } // for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
   } // if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
      else
      {
         fStorDigi.resize( iNbSmTypes );
         fStorDigiInd.resize( iNbSmTypes );
         fviClusterSize.resize( iNbSmTypes );
         fviTrkMul.resize( iNbSmTypes );
         fvdX.resize( iNbSmTypes );
         fvdY.resize( iNbSmTypes );
         fvdDifX.resize( iNbSmTypes );
         fvdDifY.resize( iNbSmTypes );
         fvdDifCh.resize( iNbSmTypes );
         for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
         {
            Int_t iNbSm  = fDigiBdfPar->GetNbSm(  iSmType);
            Int_t iNbRpc = fDigiBdfPar->GetNbRpc( iSmType);
            fStorDigi[iSmType].resize( iNbSm*iNbRpc );
            fStorDigiInd[iSmType].resize( iNbSm*iNbRpc );
            fviClusterSize[iSmType].resize( iNbRpc );
            fviTrkMul[iSmType].resize( iNbRpc );
            fvdX[iSmType].resize( iNbRpc );
            fvdY[iSmType].resize( iNbRpc );
            fvdDifX[iSmType].resize( iNbRpc );
            fvdDifY[iSmType].resize( iNbRpc );
            fvdDifCh[iSmType].resize( iNbRpc );
            for( Int_t iSm = 0; iSm < iNbSm; iSm++ )
            {
               for( Int_t iRpc = 0; iRpc < iNbRpc; iRpc++ )
               {
                  Int_t iNbChan = fDigiBdfPar->GetNbChan( iSmType, iRpc ); 
                  fStorDigi[iSmType][iSm*iNbRpc+iRpc].resize( iNbChan );
                  fStorDigiInd[iSmType][iSm*iNbRpc+iRpc].resize( iNbChan );
              } // for( Int_t iRpc = 0; iRpc < iNbRpc; iRpc++ )
            } // for( Int_t iSm = 0; iSm < iNbSm; iSm++ )
         } // for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
      } // else of if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
   return kTRUE;
}
Bool_t   CbmTofTestBeamClusterizer::DeleteGeometry()
{
   LOG(INFO)<<"CbmTofTestBeamClusterizer::DeleteGeometry starting"
             <<FairLogger::endl;
   return kTRUE;
   Int_t iNbSmTypes = fDigiBdfPar->GetNbSmTypes();
   if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
   {
      for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
      {
         Int_t iNbSm  = fDigiBdfPar->GetNbSm(  iSmType);
         Int_t iNbRpc = fDigiBdfPar->GetNbRpc( iSmType);
         for( Int_t iSm = 0; iSm < iNbSm; iSm++ )
         {
	   for( Int_t iRpc = 0; iRpc < iNbRpc; iRpc++ ) {
               fStorDigiExp[iSmType][iSm*iNbRpc+iRpc].clear();
               fStorDigiInd[iSmType][iSm*iNbRpc+iRpc].clear();
	   }
         } // for( Int_t iSm = 0; iSm < iNbSm; iSm++ )
         fStorDigiExp[iSmType].clear();
         fStorDigiInd[iSmType].clear();
      } // for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
      fStorDigiExp.clear();
      fStorDigiInd.clear();
   } // if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
      else
      {
         for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
         {
            Int_t iNbSm  = fDigiBdfPar->GetNbSm(  iSmType);
            Int_t iNbRpc = fDigiBdfPar->GetNbRpc( iSmType);
            for( Int_t iSm = 0; iSm < iNbSm; iSm++ )
            {
	      for( Int_t iRpc = 0; iRpc < iNbRpc; iRpc++ ){
                 fStorDigi[iSmType][iSm*iNbRpc+iRpc].clear();
                 fStorDigiInd[iSmType][iSm*iNbRpc+iRpc].clear();
	      }
            } // for( Int_t iSm = 0; iSm < iNbSm; iSm++ )
            fStorDigi[iSmType].clear();
            fStorDigiInd[iSmType].clear();
         } // for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
         fStorDigi.clear();
         fStorDigiInd.clear();
      } // else of if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
   return kTRUE;
}
/************************************************************************************/
// Histogramming functions
Bool_t   CbmTofTestBeamClusterizer::CreateHistos()
{
   TDirectory * oldir = gDirectory; // <= To prevent histos from being sucked in by the param file of the TRootManager!
   gROOT->cd(); // <= To prevent histos from being sucked in by the param file of the TRootManager !
   fhClustBuildTime = new TH1I( "TofTestBeamClus_ClustBuildTime", "Time needed to build clusters in each event; Time [s]",
                                 4000, 0.0, 4.0 );

   TH2* hTemp = 0;

   // RPC related distributions
   Int_t iNbDet=fMbsMappingPar->GetNbMappedDet();
   fhRpcDigiCor.resize( iNbDet  );
   fhRpcCluMul.resize( iNbDet  );
   fhRpcCluPosition.resize( iNbDet  );
   fhRpcCluTOff.resize( iNbDet  );
   fhRpcCluTot.resize( iNbDet  );
   fhRpcCluSize.resize( iNbDet  );
   fhRpcCluAvWalk.resize( iNbDet );
   fhRpcCluWalk.resize( iNbDet );

   for(Int_t iDetIndx=0; iDetIndx<iNbDet; iDetIndx++){
       Int_t iUniqueId = fMbsMappingPar->GetMappedDetUId( iDetIndx );
       Int_t iSmType   = CbmTofAddress::GetSmType( iUniqueId );
       Int_t iSmId     = CbmTofAddress::GetSmId( iUniqueId );
       Int_t iRpcId    = CbmTofAddress::GetRpcId( iUniqueId );
       Int_t iUCellId  = CbmTofAddress::GetUniqueAddress(iSmId,iRpcId,0,0,iSmType);
       fChannelInfo = fDigiPar->GetCell(iUCellId);
       if (NULL==fChannelInfo) {
	 LOG(WARNING)<<"CbmTofTestBeamClusterizer::CreateHistos: No DigiPar for Det "
                     <<Form("0x%08x", iUCellId)
                 <<FairLogger::endl;

	 break;
       }      
       LOG(INFO) << "CbmTofTestBeamClusterizer::CreateHistos: DetIndx "<<iDetIndx<<", SmType "<<iSmType<<", SmId "<<iSmId
	 	 << ", RpcId "<<iRpcId<<" => UniqueId "<<Form("(0x%08x, 0x%08x)",iUniqueId,iUCellId)
                 <<", dx "<<fChannelInfo->GetSizex()
                 <<", dy "<<fChannelInfo->GetSizey()
                 <<Form(" poi: 0x%08x ",fChannelInfo)
	 	 <<", nbCh "<<fMbsMappingPar->GetSmTypeNbCh(iSmType)
                 <<FairLogger::endl;

       fhRpcDigiCor[iDetIndx] =  new TH2I(
          Form("cl_SmT%01d_sm%03d_rpc%03d_DigiCor", iSmType, iSmId, iRpcId ),
          Form("Digi Correlation of Rpc #%03d in Sm %03d of type %d; digi 0; digi 1", iRpcId, iSmId, iSmType ),
          fMbsMappingPar->GetSmTypeNbCh(iSmType),0,fMbsMappingPar->GetSmTypeNbCh(iSmType),
	  fMbsMappingPar->GetSmTypeNbCh(iSmType),0,fMbsMappingPar->GetSmTypeNbCh(iSmType));

       fhRpcCluMul[iDetIndx] =  new TH1I(
          Form("cl_SmT%01d_sm%03d_rpc%03d_Mul", iSmType, iSmId, iRpcId ),
          Form("Clu multiplicity of Rpc #%03d in Sm %03d of type %d; M []; cnts", iRpcId, iSmId, iSmType ),
          fMbsMappingPar->GetSmTypeNbCh(iSmType),1,fMbsMappingPar->GetSmTypeNbCh(iSmType)+1);

       Double_t YSCAL=50.;
       if (fPosYMaxScal !=0.) YSCAL=fPosYMaxScal;
       fhRpcCluPosition[iDetIndx] =  new TH2F( 
          Form("cl_SmT%01d_sm%03d_rpc%03d_Pos", iSmType, iSmId, iRpcId ),
          Form("Clu position of Rpc #%03d in Sm %03d of type %d; Strip []; ypos [cm]", iRpcId, iSmId, iSmType ),
          fMbsMappingPar->GetSmTypeNbCh(iSmType), 0, fMbsMappingPar->GetSmTypeNbCh(iSmType),
          100, -YSCAL*fChannelInfo->GetSizey(),YSCAL*fChannelInfo->GetSizey()  ); 

       Double_t TSumMax=1.E4;
       if (fTRefDifMax !=0.) TSumMax=fTRefDifMax;
       fhRpcCluTOff[iDetIndx] =  new TH2F( 
          Form("cl_SmT%01d_sm%03d_rpc%03d_TOff", iSmType, iSmId, iRpcId ),
          Form("Clu TimeZero of Rpc #%03d in Sm %03d of type %d; Strip []; TOff [ps]", iRpcId, iSmId, iSmType ),
          fMbsMappingPar->GetSmTypeNbCh(iSmType), 0, fMbsMappingPar->GetSmTypeNbCh(iSmType),
          99, -TSumMax,TSumMax ); 

       if (fTotMax !=0.) TOTMax=fTotMax;
       fhRpcCluTot[iDetIndx] =  new TH2F( 
          Form("cl_SmT%01d_sm%03d_rpc%03d_Tot", iSmType, iSmId, iRpcId ),
          Form("Clu Tot of Rpc #%03d in Sm %03d of type %d; Strip []; TOT [ps]", iRpcId, iSmId, iSmType ),
          fMbsMappingPar->GetSmTypeNbCh(iSmType), 0, fMbsMappingPar->GetSmTypeNbCh(iSmType),
          100, 0., TOTMax); 

       fhRpcCluSize[iDetIndx] =  new TH2F( 
          Form("cl_SmT%01d_sm%03d_rpc%03d_Size", iSmType, iSmId, iRpcId ),
          Form("Clu size of Rpc #%03d in Sm %03d of type %d; Strip []; size [strips]", iRpcId, iSmId, iSmType ),
          fMbsMappingPar->GetSmTypeNbCh(iSmType), 0, fMbsMappingPar->GetSmTypeNbCh(iSmType),
          10, 0., 10.); 

       // Walk histos 
       fhRpcCluAvWalk[iDetIndx] = new TH2F( 
			  Form("cl_SmT%01d_sm%03d_rpc%03d_AvWalk", iSmType, iSmId, iRpcId),
			  Form("Walk in SmT%01d_sm%03d_rpc%03d_AvWalk", iSmType, iSmId, iRpcId),
			  nbClWalkBinX,0.,TOTMax,nbClWalkBinY,-TSumMax,TSumMax);

       for( Int_t iCh=0; iCh<fMbsMappingPar->GetSmTypeNbCh(iSmType); iCh++){
         hTemp = new TH2F( 
			  Form("cl_SmT%01d_sm%03d_rpc%03d_Ch%03d_Walk", iSmType, iSmId, iRpcId, iCh ),
			  Form("Walk in SmT%01d_sm%03d_rpc%03d_Ch%03d_Walk", iSmType, iSmId, iRpcId, iCh ),
			  nbClWalkBinX,0.,TOTMax,nbClWalkBinY,-TSumMax,TSumMax );
         (fhRpcCluWalk[iDetIndx]).push_back( hTemp );
       }
   }

   // Clusterizing monitoring 

   LOG(INFO)<<" Define Clusterizer monitoring histos "<<FairLogger::endl;
 
   fhDigSpacDifClust = new TH1I( "Clus_DigSpacDifClust",
         "Space difference along channel direction between Digi pairs on adjacent channels; PosCh i - Pos Ch i+1 [cm]",
         5000, -10.0, 10.0 );
   fhDigTimeDifClust = new TH1I( "Clus_DigTimeDifClust",
         "Time difference between Digi pairs on adjacent channels; 0.5*(tDigiA + tDigiA)chi - 0.5*(tDigiA + tDigiA)chi+1 [ns]",
         5000, -5.0, 5.0 );
   fhDigDistClust = new TH2I( "Clus_DigDistClust",
         "Distance between Digi pairs on adjacent channels; PosCh i - Pos Ch i+1 [cm along ch]; 0.5*(tDigiA + tDigiA)chi - 0.5*(tDigiA + tDigiA)chi+1 [ns]",
         5000, -10.0, 10.0,    2000, -5.0, 5.0 );
   fhClustSizeDifX = new TH2I( "Clus_ClustSizeDifX", "Position difference between Point and Hit as function of Cluster Size; Cluster Size [Strips]; dX [cm]",
         100, 0.5, 100.5,       500,-50,50);
   fhClustSizeDifY = new TH2I( "Clus_ClustSizeDifY", "Position difference between Point and Hit as function of Cluster Size; Cluster Size [Strips]; dY [cm]",
         100, 0.5, 100.5,       500,-50,50);
   fhChDifDifX = new TH2I( "Clus_ChDifDifX", "Position difference between Point and Hit as function of Channel dif; Ch Dif [Strips]; dX [cm]",
         101, -50.5, 50.5,      500,-50,50);
   fhChDifDifY = new TH2I( "Clus_ChDifDifY", "Position difference between Point and Hit as function of Channel Dif; Ch Dif [Strips]; dY [cm]",
         101, -50.5, 50.5,      500,-50,50);
   fhNbSameSide = new TH1I( "Clus_NbSameSide",
         "How many time per event the 2 digis on a channel were of the same side ; Counts/Event []",
         500, 0.0, 500.0 );
   fhNbDigiPerChan = new TH1I( "Clus_NbDigiPerChan",
         "Nb of Digis per channel; Nb Digis []",
         100, 0.0, 100.0 );

   // Trigger selected histograms 
   if (0<iNTrg){
   fhTRpcCluMul.resize( iNbDet  );
   fhTRpcCluPosition.resize( iNbDet  );
   fhTRpcCluTOff.resize( iNbDet  );
   fhTRpcCluTot.resize( iNbDet  );
   fhTRpcCluSize.resize( iNbDet  );
   fhTRpcCluAvWalk.resize( iNbDet );
   fhTRpcCluWalk.resize( iNbDet );
   for(Int_t iDetIndx=0; iDetIndx<iNbDet; iDetIndx++){
       Int_t iUniqueId = fMbsMappingPar->GetMappedDetUId( iDetIndx );
       Int_t iSmType   = CbmTofAddress::GetSmType( iUniqueId );
       Int_t iSmId     = CbmTofAddress::GetSmId( iUniqueId );
       Int_t iRpcId    = CbmTofAddress::GetRpcId( iUniqueId );
       Int_t iUCellId  = CbmTofAddress::GetUniqueAddress(iSmId,iRpcId,0,0,iSmType);
       fChannelInfo = fDigiPar->GetCell(iUCellId);
       if (NULL==fChannelInfo) {
	 LOG(WARNING)<<"CbmTofTestBeamClusterizer::CreateHistos: No DigiPar for Det "
                     <<Form("0x%08x", iUCellId)
                 <<FairLogger::endl;
	 break;
       }      
       LOG(INFO) << "CbmTofTestBeamClusterizer::CreateHistos: DetIndx "<<iDetIndx<<", SmType "<<iSmType<<", SmId "<<iSmId
	 	 << ", RpcId "<<iRpcId<<" => UniqueId "<<Form("(0x%08x, 0x%08x)",iUniqueId,iUCellId)
                 <<", dx "<<fChannelInfo->GetSizex()
                 <<", dy "<<fChannelInfo->GetSizey()
                 <<Form(" poi: 0x%08x ",fChannelInfo)
	 	 <<", nbCh "<<fMbsMappingPar->GetSmTypeNbCh(iSmType)
                 <<FairLogger::endl;
       fhTRpcCluMul[iDetIndx].resize( iNTrg  );
       fhTRpcCluPosition[iDetIndx].resize( iNTrg  );
       fhTRpcCluTOff[iDetIndx].resize( iNTrg  );
       fhTRpcCluTot[iDetIndx].resize( iNTrg  );
       fhTRpcCluSize[iDetIndx].resize( iNTrg  );
       fhTRpcCluAvWalk[iDetIndx].resize( iNTrg  );
       fhTRpcCluWalk[iDetIndx].resize( iNTrg  );

       for (Int_t iTrg=0; iTrg<iNTrg; iTrg++){
       fhTRpcCluMul[iDetIndx][iTrg] =  new TH1I(
	  Form("cl_SmT%01d_sm%03d_rpc%03d_Trg%02d_Mul", iSmType, iSmId, iRpcId, iTrg ),
	  Form("Clu multiplicity of Rpc #%03d in Sm %03d of type %d under Trigger %02d; M []; cnts", iRpcId, iSmId, iSmType, iTrg ),
          fMbsMappingPar->GetSmTypeNbCh(iSmType),1.,fMbsMappingPar->GetSmTypeNbCh(iSmType)+1);

       Double_t YSCAL=50.;
       if (fPosYMaxScal !=0.) YSCAL=fPosYMaxScal;
       fhTRpcCluPosition[iDetIndx][iTrg] =  new TH2F( 
	  Form("cl_SmT%01d_sm%03d_rpc%03d_Trg%02d_Pos", iSmType, iSmId, iRpcId, iTrg ),
          Form("Clu position of Rpc #%03d in Sm %03d of type %d under Trigger %02d; Strip []; ypos [cm]", iRpcId, iSmId, iSmType, iTrg ),
          fMbsMappingPar->GetSmTypeNbCh(iSmType), 0, fMbsMappingPar->GetSmTypeNbCh(iSmType),
          100, -YSCAL*fChannelInfo->GetSizey(),YSCAL*fChannelInfo->GetSizey()  ); 

       Double_t TSumMax=1.E4;
       if (fTRefDifMax !=0.) TSumMax=fTRefDifMax;
       fhTRpcCluTOff[iDetIndx][iTrg] =  new TH2F( 
	  Form("cl_SmT%01d_sm%03d_rpc%03d_Trg%02d_TOff", iSmType, iSmId, iRpcId, iTrg ),
	  Form("Clu TimeZero of Rpc #%03d in Sm %03d of type %d under Trigger %02d; Strip []; TOff [ps]", iRpcId, iSmId, iSmType, iTrg ),
          fMbsMappingPar->GetSmTypeNbCh(iSmType), 0, fMbsMappingPar->GetSmTypeNbCh(iSmType),
          99, -TSumMax,TSumMax ); 

       if (fTotMax !=0.) TOTMax=fTotMax;
       fhTRpcCluTot[iDetIndx][iTrg]  =  new TH2F( 
          Form("cl_SmT%01d_sm%03d_rpc%03d_Trg%02d_Tot", iSmType, iSmId, iRpcId, iTrg ),
          Form("Clu Tot of Rpc #%03d in Sm %03d of type %d under Trigger %02d; Strip []; TOT [ps]", iRpcId, iSmId, iSmType, iTrg ),
          fMbsMappingPar->GetSmTypeNbCh(iSmType), 0, fMbsMappingPar->GetSmTypeNbCh(iSmType),
          100, 0., TOTMax); 

       fhTRpcCluSize[iDetIndx][iTrg]  =  new TH2F( 
          Form("cl_SmT%01d_sm%03d_rpc%03d_Trg%02d_Size", iSmType, iSmId, iRpcId, iTrg ),
          Form("Clu size of Rpc #%03d in Sm %03d of type %d under Trigger %02d; Strip []; size [strips]", iRpcId, iSmId, iSmType, iTrg ),
          fMbsMappingPar->GetSmTypeNbCh(iSmType), 0, fMbsMappingPar->GetSmTypeNbCh(iSmType),
          10, 0., 10.); 

       // Walk histos 
       fhTRpcCluAvWalk[iDetIndx][iTrg]  = new TH2F( 
			  Form("cl_SmT%01d_sm%03d_rpc%03d_Trg%02d_AvWalk", iSmType, iSmId, iRpcId, iTrg),
			  Form("Walk in SmT%01d_sm%03d_rpc%03d_AvWalk under Trigger %02d", iSmType, iSmId, iRpcId, iTrg),
			  nbClWalkBinX,0.,TOTMax,nbClWalkBinY,-TSumMax,TSumMax);

       for( Int_t iCh=0; iCh<fMbsMappingPar->GetSmTypeNbCh(iSmType); iCh++){
         hTemp = new TH2F( 
			  Form("cl_SmT%01d_sm%03d_rpc%03d_Ch%03d_Trg%02d_Walk", iSmType, iSmId, iRpcId, iCh, iTrg ),
			  Form("Walk in SmT%01d_sm%03d_rpc%03d_Ch%03d_Walk under Trigger %02d", iSmType, iSmId, iRpcId, iCh, iTrg ),
			  nbClWalkBinX,0.,TOTMax,nbClWalkBinY,-TSumMax,TSumMax );
         (fhTRpcCluWalk[iDetIndx][iTrg] ).push_back( hTemp );
       }
      }
     }
   }
   // MC reference 
   fhHitsPerTracks = new TH1I( "Clus_TofHitPerTrk", "Mean Number of TofHit per Mc Track; Nb TofHits/Nb MC Tracks []",
         2000, 0.0, 20.0 );
   if( kFALSE == fDigiBdfPar->ClustUseTrackId() )
      fhPtsPerHit = new TH1I( "Clus_TofPtsPerHit", "Distribution of the Number of MCPoints associated to each TofHit; Nb MCPoint []",
         20, 0.0, 20.0 );
   if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
   {
      fhTimeResSingHits = new TH1I( "Clus_TofTimeResClust", "Time resolution for TofHits containing Digis from a single MC Track; t(1st Mc Point) -tTofHit [ns]",
            10000, -25.0, 25.0 );
      fhTimeResSingHitsB = new TH2I( "Clus_TofTimeResClustB", "Time resolution for TofHits containing Digis from a single MC Track; (1st Mc Point) -tTofHit [ns]",
            5000, -25.0, 25.0,
            6, 0, 6);
      fhTimePtVsHits = new TH2I( "Clus_TofTimePtVsHit", "Time resolution for TofHits containing Digis from a single MC Track; t(1st Mc Point) -tTofHit [ps]",
            2000, 0.0, 50000.0,
            2000, 0.0, 50000.0 );
   }
      else
      {
         fhTimeResSingHits = new TH1I( "Clus_TofTimeResClust", "Time resolution for TofHits containing Digis from a single TofPoint; tMcPoint -tTofHit [ns]",
               10000, -25.0, 25.0 );
         fhTimeResSingHitsB = new TH2I( "Clus_TofTimeResClustB", "Time resolution for TofHits containing Digis from a single TofPoint; tMcPoint -tTofHit [ns]",
               5000, -25.0, 25.0,
               6, 0, 6);
         fhTimePtVsHits = new TH2I( "Clus_TofTimePtVsHit", "Time resolution for TofHits containing Digis from a single TofPoint; tMcPoint -tTofHit [ps]",
               2000, 0.0, 50000.0,
               2000, 0.0, 50000.0 );
      } // else of if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
   fhClusterSize = new TH1I( "Clus_ClusterSize", "Cluster Size distribution; Cluster Size [Strips]",
         100, 0.5, 100.5 );
   fhClusterSizeType = new TH2I( "Clus_ClusterSizeType", "Cluster Size distribution in each (SM type, Rpc) pair; Cluster Size [Strips]; 10*SM Type + Rpc Index []",
         100, 0.5, 100.5,
         40*fDigiBdfPar->GetNbSmTypes(), 0.0, 40*fDigiBdfPar->GetNbSmTypes() );
   if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
   {
      fhTrackMul = new TH1I( "Clus_TrackMul", "Number of MC tracks generating the cluster; MC Tracks multiplicity []",
            100, 0.5, 100.5 );
      fhClusterSizeMulti = new TH2I( "Clus_ClusterSizeMulti",
            "Cluster Size distribution as function of Number of MC tracks generating the cluster; Cluster Size [Strips]; MC tracks mul. []",
            100, 0.5, 100.5,
            100, 0.5, 100.5 );
      fhTrk1MulPos = new TH2D( "Clus_Trk1MulPos",
            "Position of Clusters with only 1 MC tracks generating the cluster; X [cm]; Y [cm]",
            1500, -750, 750,
            1000, -500, 500 );
      fhHiTrkMulPos = new TH2D( "Clus_HiTrkMulPos",
            "Position of Clusters with >1 MC tracks generating the cluster; X [cm]; Y [cm]",
            1500, -750, 750,
            1000, -500, 500 );
      fhAllTrkMulPos = new TH2D( "Clus_AllTrkMulPos",
            "Position of all clusters generating the cluster; X [cm]; Y [cm]",
            1500, -750, 750,
            1000, -500, 500 );
      fhMultiTrkProbPos = new TH2D( "Clus_MultiTrkProbPos",
            "Probability of having a cluster with multiple tracks as function of position; X [cm]; Y [cm]; Prob. [%]",
            1500, -750, 750,
            1000, -500, 500 );
   } // if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
   
   gDirectory->cd( oldir->GetPath() ); // <= To prevent histos from being sucked in by the param file of the TRootManager!

   return kTRUE;
}
Bool_t   CbmTofTestBeamClusterizer::FillHistos()
{
 fhClustBuildTime->Fill( fStop.GetSec() - fStart.GetSec()
                           + (fStop.GetNanoSec() - fStart.GetNanoSec())/1e9 );
 Int_t iNbTofHits  = fTofHitsColl->GetEntries();
 if(0<iNbTofHits){
  CbmTofHit  *pHit;
  Bool_t BTrg[iNTrg];
  if(0<iNTrg){                  // check software triggers
   LOG(DEBUG)<<"CbmTofTestBeamClusterizer::FillHistos() for "<<iNTrg<<" triggers "
             <<FairLogger::endl;
   for (Int_t iTrg=0; iTrg<iNTrg; iTrg++){
       BTrg[iTrg]=kFALSE;
       switch(iTrg) {
       case 0 :         // plastic coincidence 
	 if(fviClusterMul[2][0][0]>0. && fviClusterMul[2][1][0]>0.){
	     LOG(INFO)<<"CbmTofTestBeamClusterizer::FillHistos(): Found selector 0"<<FairLogger::endl;
	     BTrg[iTrg]=kTRUE;
	 } 
	 break; 
       case 1:          // to be defined, Reference counter - Diamond coincidence 
	 break;

       default :
       LOG(INFO)<<"CbmTofTestBeamClusterizer::FillHistos: selection not implemented "<<iTrg<<FairLogger::endl;
	 ;
       }
    }

   }
   for( Int_t iHitInd = 0; iHitInd < iNbTofHits; iHitInd++)
   {
     pHit = (CbmTofHit*) fTofHitsColl->At( iHitInd );
     if( kFALSE == fDigiBdfPar->ClustUseTrackId() ) fhPtsPerHit->Fill(pHit->GetFlag());
     Int_t iDetId = (pHit->GetAddress() & DetMask);

     for(Int_t iDetIndx=0; iDetIndx<fMbsMappingPar->GetNbMappedDet(); iDetIndx++){
       Int_t iUniqueId = fMbsMappingPar->GetMappedDetUId( iDetIndx );

       LOG(DEBUG1)<<"CbmTofTestBeamClusterizer::FillHistos: Inspect Hit  "
                  << Form(" %d %08x %08x %d %08x ", iHitInd, pHit->GetAddress(), DetMask, iDetIndx, iUniqueId)
		  <<FairLogger::endl;
       if(iDetId == iUniqueId){    // detector index found
	 Int_t iSmType = CbmTofAddress::GetSmType( iUniqueId );
	 Int_t iSm     = CbmTofAddress::GetSmId( iUniqueId );
	 Int_t iRpc    = CbmTofAddress::GetRpcId( iUniqueId );
         if(-1<fviClusterMul[iSmType][iSm][iRpc]){           // fill only once per detector 
	   fhRpcCluMul[iDetIndx]->Fill(fviClusterMul[iSmType][iSm][iRpc]);
	   for (Int_t iTrg=0; iTrg<iNTrg; iTrg++) if(BTrg[iTrg]){
	       fhTRpcCluMul[iDetIndx][iTrg]->Fill(fviClusterMul[iSmType][iSm][iRpc]);
	   }	 
	   fviClusterMul[iSmType][iSm][iRpc]=-1;  // invalidate
	 }
         Int_t iChId = pHit->GetAddress();
         fChannelInfo = fDigiPar->GetCell( iChId );
	 Int_t iCh = CbmTofAddress::GetChannelId(iChId);

         LOG(DEBUG1)<<"CbmTofTestBeamClusterizer::FillHistos: Hit  "
		    <<Form(" 0x%08x %d %f %f %f %f %f %d",iChId,iCh,
			   pHit->GetX(),pHit->GetY(),pHit->GetTime(),fChannelInfo->GetX(),fChannelInfo->GetY(), pHit->GetRefId() )
		    <<FairLogger::endl;

	 fhRpcCluPosition[iDetIndx]->Fill((Double_t)iCh,pHit->GetY()-fChannelInfo->GetY());
	 for (Int_t iTrg=0; iTrg<iNTrg; iTrg++) if(BTrg[iTrg]){
	   fhTRpcCluPosition[iDetIndx][iTrg]->Fill((Double_t)iCh,pHit->GetY()-fChannelInfo->GetY());
	 }
	 LOG(DEBUG1)<<"CbmTofTestBeamClusterizer::FillHistos: TofDigiMatchColl entries:"
		    <<fTofDigiMatchColl->GetEntries()
		    <<FairLogger::endl;

	 if(pHit->GetRefId()>fTofDigiMatchColl->GetEntries()){
	   LOG(ERROR)<<"CbmTofTestBeamClusterizer::FillHistos: Inconsistent DigiMatches "
		    <<FairLogger::endl;
	   }

         CbmMatch* digiMatch=(CbmMatch *)fTofDigiMatchColl->At(pHit->GetRefId());
         LOG(DEBUG)<<"CbmTofTestBeamClusterizer::FillHistos: got matches: "
		   <<digiMatch->GetNofLinks()<<":";
	 fhRpcCluSize[iDetIndx]->Fill((Double_t)iCh,digiMatch->GetNofLinks()/2.);
	 for (Int_t iTrg=0; iTrg<iNTrg; iTrg++) if(BTrg[iTrg]){
	   fhTRpcCluSize[iDetIndx][iTrg]->Fill((Double_t)iCh,digiMatch->GetNofLinks()/2.);
	 }
         for (Int_t iLink=0; iLink<digiMatch->GetNofLinks(); iLink+=2){
           CbmLink L0 = digiMatch->GetLink(iLink);   //vDigish.at(ivDigInd);
	   Int_t iDigInd0=L0.GetIndex(); 
           Int_t iDigInd1=(digiMatch->GetLink(iLink+1)).GetIndex(); //vDigish.at(ivDigInd+1);
           LOG(DEBUG1)<<" " << iDigInd0<<", "<<iDigInd1<<FairLogger::endl;
           if (iDigInd0 < fTofDigisColl->GetEntries() && iDigInd1 < fTofDigisColl->GetEntries()){
            CbmTofDigiExp *pDig0 = (CbmTofDigiExp*) (fTofDigisColl->At(iDigInd0));
            CbmTofDigiExp *pDig1 = (CbmTofDigiExp*) (fTofDigisColl->At(iDigInd1));
	    fhRpcCluTot[iDetIndx]->Fill((Double_t)iCh,pDig0->GetTot()+pDig1->GetTot());
	    if (dTRef!=0. && fTRefHits==1){
	     fhRpcCluAvWalk[iDetIndx]->Fill(pDig0->GetTot()+pDig1->GetTot(),pHit->GetTime()-dTRef);
	     fhRpcCluWalk[iDetIndx][iCh]->Fill(pDig0->GetTot()+pDig1->GetTot(),pHit->GetTime()-dTRef);
	     for (Int_t iTrg=0; iTrg<iNTrg; iTrg++) if(BTrg[iTrg]){
		 fhTRpcCluAvWalk[iDetIndx][iTrg]->Fill(pDig0->GetTot()+pDig1->GetTot(),pHit->GetTime()-dTRef);
		 fhTRpcCluWalk[iDetIndx][iTrg][iCh]->Fill(pDig0->GetTot()+pDig1->GetTot(),pHit->GetTime()-dTRef);
	     }
	    }
	   }
	   else {
	     LOG(ERROR)<<"CbmTofTestBeamClusterizer::FillHistos: invalid digi index "<<iDetIndx
		       <<" digi0,1"<<iDigInd0<<", "<<iDigInd1<<" - max:"
		       << fTofDigisColl->GetEntries() 
	       //                       << " in event " << XXX 
		       << FairLogger::endl;  
           }
	 }

	 if (dTRef !=0. && fTRefHits==1){
	   fhRpcCluTOff[iDetIndx]->Fill((Double_t)iCh,pHit->GetTime()-dTRef);
           if(iUniqueId == fTRefMode && (pHit->GetTime()-dTRef) !=0.){
	     LOG(WARNING)<<Form(" Inconsistent dTRef %f for detetor 0x%08x with %d hits ",dTRef,iUniqueId,fTRefHits)
			 <<FairLogger::endl;
	   }
	   for (Int_t iTrg=0; iTrg<iNTrg; iTrg++) if(BTrg[iTrg]){
	       LOG(DEBUG1)<<"CbmTofTestBeamClusterizer::FillHistos: TRpcCluTOff "<< iDetIndx <<", "<< iTrg
		        <<FairLogger::endl;

	     fhTRpcCluTOff[iDetIndx][iTrg]->Fill((Double_t)iCh,pHit->GetTime()-dTRef);
	   }
           
	 }
	 break;       
       }
     }
   }


   for( Int_t iSmType = 0; iSmType < fDigiBdfPar->GetNbSmTypes(); iSmType++ ){
      for( Int_t iRpc = 0; iRpc < fDigiBdfPar->GetNbRpc( iSmType); iRpc++ )
      {
         LOG(DEBUG1)<<"CbmTofTestBeamClusterizer::FillHistos:  "
                    <<Form(" %3d %3d %3d ",iSmType,iRpc,fviClusterSize[iSmType][iRpc].size())
                    <<FairLogger::endl;
    
	 for( Int_t iCluster = 0; iCluster < fviClusterSize[iSmType][iRpc].size(); iCluster++ )
         {
            LOG(DEBUG1)<<"CbmTofTestBeamClusterizer::FillHistos:  "
                       <<Form(" %3d %3d %3d ",iSmType,iRpc,iCluster)
                       <<FairLogger::endl;

            fhClusterSize->Fill( fviClusterSize[iSmType][iRpc][iCluster]);
            fhClusterSizeType->Fill( fviClusterSize[iSmType][iRpc][iCluster], 40*iSmType + iRpc );
            if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
            {
               fhTrackMul->Fill( fviTrkMul[iSmType][iRpc][iCluster] );
               fhClusterSizeMulti->Fill( fviClusterSize[iSmType][iRpc][iCluster],
                                         fviTrkMul[iSmType][iRpc][iCluster] );
               if( 1 == fviTrkMul[iSmType][iRpc][iCluster] )
                  fhTrk1MulPos->Fill( fvdX[iSmType][iRpc][iCluster], fvdY[iSmType][iRpc][iCluster] );
               if( 1 < fviTrkMul[iSmType][iRpc][iCluster] )
                  fhHiTrkMulPos->Fill(fvdX[iSmType][iRpc][iCluster], fvdY[iSmType][iRpc][iCluster] );
               fhAllTrkMulPos->Fill(fvdX[iSmType][iRpc][iCluster], fvdY[iSmType][iRpc][iCluster] );
            } // if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
            if(kFALSE) //  1 == fviTrkMul[iSmType][iRpc][iCluster] )
            {
               fhClustSizeDifX->Fill( fviClusterSize[iSmType][iRpc][iCluster], fvdDifX[iSmType][iRpc][iCluster]);
               fhClustSizeDifY->Fill( fviClusterSize[iSmType][iRpc][iCluster], fvdDifY[iSmType][iRpc][iCluster]);
               if( 1 == fviClusterSize[iSmType][iRpc][iCluster] )
               {
                  fhChDifDifX->Fill( fvdDifCh[iSmType][iRpc][iCluster], fvdDifX[iSmType][iRpc][iCluster]);
                  fhChDifDifY->Fill( fvdDifCh[iSmType][iRpc][iCluster], fvdDifY[iSmType][iRpc][iCluster]);
               }
            }
         } // for( Int_t iCluster = 0; iCluster < fviClusterSize[iSmType][iRpc].size(); iCluster++ )
         fviClusterSize[iSmType][iRpc].clear();
         fviTrkMul[iSmType][iRpc].clear();
         fvdX[iSmType][iRpc].clear();
         fvdY[iSmType][iRpc].clear();
         fvdDifX[iSmType][iRpc].clear();
         fvdDifY[iSmType][iRpc].clear();
         fvdDifCh[iSmType][iRpc].clear();
      } // for( Int_t iRpc = 0; iRpc < fDigiBdfPar->GetNbRpc( iSmType); iRpc++ )
   }
   fhNbSameSide->Fill(fiNbSameSide);
  }
  return kTRUE;
}

Bool_t   CbmTofTestBeamClusterizer::WriteHistos()
{
   TDirectory * oldir = gDirectory;
   TFile *fHist;
   fHist = new TFile("./tofTestBeamClust.hst.root","RECREATE");

   fHist->cd();
   fhClustBuildTime->Write();

   for(Int_t iDetIndx=0; iDetIndx< fMbsMappingPar->GetNbMappedDet(); iDetIndx++){
     fhRpcCluPosition[iDetIndx]->Write();
     fhRpcCluTOff[iDetIndx]->Write();
     fhRpcCluTot[iDetIndx]->Write();
     fhRpcCluAvWalk[iDetIndx]->Write();

     Int_t iUniqueId = fMbsMappingPar->GetMappedDetUId( iDetIndx );
     Int_t iSmType   = CbmTofAddress::GetSmType( iUniqueId );
     Int_t iSm       = CbmTofAddress::GetSmId( iUniqueId );
     Int_t iRpc      = CbmTofAddress::GetRpcId( iUniqueId );

     Int_t iNent = fhRpcCluAvWalk[iDetIndx]->GetEntries();
     if(0==iNent){
       LOG(INFO)<<"CbmTofTestBeamClusterizer::WriteHistos: No entries in Walk histos for " 
		 <<"Smtype"<<iSmType<<", Sm "<<iSm<<", Rpc "<<iRpc 
		 <<FairLogger::endl;
       continue;
     }

     TProfile *htempPos_pfx  = fhRpcCluPosition[iDetIndx]->ProfileX();
     TProfile *htempTOff_pfx = fhRpcCluTOff[iDetIndx]->ProfileX();
     TProfile *htempTot_pfx  = fhRpcCluTot[iDetIndx]->ProfileX();

     if(0==fCalMode){  // Initialize
       htempPos_pfx->Write();
       htempTOff_pfx->Write();
       htempTot_pfx->Write();
     }
     else if (1==fCalMode) //save offsets, update walks 
     {
        Int_t iNbRpc = fDigiBdfPar->GetNbRpc( iSmType);
	Int_t iNbCh = fDigiBdfPar->GetNbChan( iSmType, iRpc );
        LOG(INFO)<<"CbmTofTestBeamClusterizer::WriteHistos: restore Offsets and Gains and save Walk for "
		 <<"Smtype"<<iSmType<<", Sm "<<iSm<<", Rpc "<<iRpc 
		 <<FairLogger::endl;
	htempPos_pfx->Reset();    //reset to restore mean of original histos
	htempTOff_pfx->Reset();
	htempTot_pfx->Reset();
	for( Int_t iCh = 0; iCh < iNbCh; iCh++ ) 
	{
	 Double_t YMean=fDigiBdfPar->GetSignalSpeed()*0.5
	                   *(fvCPTOff[iSmType][iSm*iNbRpc+iRpc][iCh][1]-fvCPTOff[iSmType][iSm*iNbRpc+iRpc][iCh][0]);
	 Double_t TMean=0.5*(fvCPTOff[iSmType][iSm*iNbRpc+iRpc][iCh][1]+fvCPTOff[iSmType][iSm*iNbRpc+iRpc][iCh][0]);
	 htempPos_pfx->Fill(iCh,YMean);
         if( ((TProfile *)htempPos_pfx)->GetBinContent(iCh+1)!=YMean) {
	   LOG(ERROR)<<"CbmTofTestBeamClusterizer::WriteHistos: restore unsuccessful! ch "<<iCh
		     <<" got "<< htempPos_pfx->GetBinContent(iCh)<<","<<htempPos_pfx->GetBinContent(iCh+1)
		     <<","<<htempPos_pfx->GetBinContent(iCh+2)
	             <<", expected "<<YMean
		     <<FairLogger::endl;
	 }
	 htempTOff_pfx->Fill(iCh,TMean); 
	 htempTot_pfx->Fill(iCh,TTotMean/fvCPTotGain[iSmType][iSm*iNbRpc+iRpc][iCh][1]);
	}

        LOG(DEBUG)<<" Restoring done ... "<<FairLogger::endl;

        htempPos_pfx->Write();
        htempTOff_pfx->Write();
        htempTot_pfx->Write();
        for( Int_t iCh=0; iCh<fMbsMappingPar->GetSmTypeNbCh(iSmType); iCh++){
          if(NULL == fhRpcCluWalk[iDetIndx][iCh]){
	    LOG(INFO)<<Form("CbmTofTestBeamClusterizer::WriteHistos: Walk histo not available for SmT %d, Sm %d, Rpc %d, Ch %d",iSmType,iSm,iRpc,iCh)
 		    <<FairLogger::endl;
	    break;
	  }
          Int_t iNEntries=fhRpcCluWalk[iDetIndx][iCh]->GetEntries();
	  LOG(DEBUG)<<Form(" Update Walk correction for SmT %d, Sm %d, Rpc %d, Ch %d: Entires %d",
			   iSmType,iSm,iRpc,iCh,iNEntries)
 		    <<FairLogger::endl;

	 fhRpcCluWalk[iDetIndx][iCh]->Write();
         if(0<iNEntries){
          TProfile *htmp = fhRpcCluWalk[iDetIndx][iCh]->ProfileX();
          TH1D *h1tmp    = fhRpcCluWalk[iDetIndx][iCh]->ProjectionX();
          Int_t iSide=0;
	  for(Int_t iWx=0; iWx<nbClWalkBinX; iWx++){
           if(iSmType==2)
	     LOG(DEBUG)<<"Update Walk iSm = "<<iSm<<"," << iWx << ": "
		      <<fvCPWalk[iSmType][iSm*iNbRpc+iRpc][iCh][iSide][iWx]<<" "
		      <<((TProfile *)htmp)->GetBinContent(iWx+1)
		      <<FairLogger::endl;
	   fvCPWalk[iSmType][iSm*iNbRpc+iRpc][iCh][iSide][iWx]+=0.5*((TProfile *)htmp)->GetBinContent(iWx+1);
	  }
          h1tmp->Reset();
	  for(Int_t iWx=0; iWx<nbClWalkBinX; iWx++){
	   h1tmp->SetBinContent(iWx+1,(2.*fvCPWalk[iSmType][iSm*iNbRpc+iRpc][iCh][iSide][iWx]));
           if( h1tmp->GetBinContent(iWx+1)!=2.*fvCPWalk[iSmType][iSm*iNbRpc+iRpc][iCh][iSide][iWx]) {
	   LOG(ERROR)<<"CbmTofTestBeamClusterizer::WriteHistos: restore unsuccessful! iWx "<<iWx
		     <<" got "<< h1tmp->GetBinContent(iWx+1)
	             <<", expected "<<2.*fvCPWalk[iSmType][iSm*iNbRpc+iRpc][iCh][iSide][iWx]
		     <<FairLogger::endl;	 }
	  }
          htmp->Write();
          h1tmp->Write();
	 }
        }     
     }
     else if (2==fCalMode) //update offsets, save walks 
     {     //tbd
        Int_t iNbRpc = fDigiBdfPar->GetNbRpc( iSmType);
	Int_t iNbCh = fDigiBdfPar->GetNbChan( iSmType, iRpc );
        LOG(INFO)<<"CbmTofTestBeamClusterizer::WriteHistos: update Offsets and Gains and keep Walk for "
		 <<"Smtype"<<iSmType<<", Sm "<<iSm<<", Rpc "<<iRpc 
		 <<FairLogger::endl;

	for( Int_t iCh = 0; iCh < iNbCh; iCh++ ) // update Offset and Gain 
	{
	  Double_t YMean=((TProfile *)htempPos_pfx)->GetBinContent(iCh+1);  //nh +1 empirical(?)
	  Double_t TMean=((TProfile *)htempTOff_pfx)->GetBinContent(iCh+1);
          Double_t dTYOff=YMean/fDigiBdfPar->GetSignalSpeed() ;
	  fvCPTOff[iSmType][iSm*iNbRpc+iRpc][iCh][0] += -dTYOff + TMean ;
	  fvCPTOff[iSmType][iSm*iNbRpc+iRpc][iCh][1] += +dTYOff + TMean ;
 	  Double_t TotMean=((TProfile *)htempTot_pfx)->GetBinContent(iCh+1);  //nh +1 empirical(?)
	  if(1<TotMean){
	    fvCPTotGain[iSmType][iSm*iNbRpc+iRpc][iCh][0] *= TTotMean / TotMean;
	    fvCPTotGain[iSmType][iSm*iNbRpc+iRpc][iCh][1] *= TTotMean / TotMean;
	  }
	  LOG(DEBUG1)<<"CbmTofTestBeamClusterizer::FillCalHist:" 
		   <<" SmT "<< iSmType<<" Sm "<<iSm<<" Rpc "<<iRpc<<" Ch "<<iCh
		   <<": YMean "<<YMean<<", TMean "<< TMean
		   <<" -> " << fvCPTOff[iSmType][iSm*iNbRpc+iRpc][iCh][0]
		   <<", "   << fvCPTOff[iSmType][iSm*iNbRpc+iRpc][iCh][1]
		   <<", "   << fvCPTotGain[iSmType][iSm*iNbRpc+iRpc][iCh][0]
		   <<FairLogger::endl;
	} // for( Int_t iCh = 0; iCh < iNbCh; iCh++ ) 

	htempPos_pfx->Reset();    //reset to store new values 
	htempTOff_pfx->Reset();
	htempTot_pfx->Reset();
	for( Int_t iCh = 0; iCh < iNbCh; iCh++ ) // store new values 
	{
	 Double_t YMean=fDigiBdfPar->GetSignalSpeed()*0.5
	  *(fvCPTOff[iSmType][iSm*iNbRpc+iRpc][iCh][1]-fvCPTOff[iSmType][iSm*iNbRpc+iRpc][iCh][0]);
	 Double_t TMean=0.5*(fvCPTOff[iSmType][iSm*iNbRpc+iRpc][iCh][1]+fvCPTOff[iSmType][iSm*iNbRpc+iRpc][iCh][0]);
	 htempPos_pfx->Fill(iCh,YMean);
         if( ((TProfile *)htempPos_pfx)->GetBinContent(iCh+1)!=YMean) {
	   LOG(ERROR)<<"CbmTofTestBeamClusterizer::WriteHistos: restore unsuccessful! ch "<<iCh
		     <<" got "<< htempPos_pfx->GetBinContent(iCh)<<","<<htempPos_pfx->GetBinContent(iCh+1)
		     <<","<<htempPos_pfx->GetBinContent(iCh+2)
	             <<", expected "<<YMean
		     <<FairLogger::endl;
	 }
	 htempTOff_pfx->Fill(iCh,TMean); 
	 htempTot_pfx->Fill(iCh,TTotMean/fvCPTotGain[iSmType][iSm*iNbRpc+iRpc][iCh][1]);
	} // for( Int_t iCh = 0; iCh < iNbCh; iCh++ ) 

        LOG(DEBUG)<<" Updating done ... write to file "<<FairLogger::endl;
        htempPos_pfx->Write();
        htempTOff_pfx->Write();
        htempTot_pfx->Write();

	// store walk histos
	for( Int_t iCh = 0; iCh < iNbCh; iCh++ ) // store new values 
	{
          TProfile *htmp = fhRpcCluWalk[iDetIndx][iCh]->ProfileX();
          TH1D *h1tmp    = fhRpcCluWalk[iDetIndx][iCh]->ProjectionX();
          Int_t iSide=0;
	  for(Int_t iWx=0; iWx<nbClWalkBinX; iWx++){
	   h1tmp->SetBinContent(iWx+1,(2.*fvCPWalk[iSmType][iSm*iNbRpc+iRpc][iCh][iSide][iWx]));
           if( h1tmp->GetBinContent(iWx+1)!=2.*fvCPWalk[iSmType][iSm*iNbRpc+iRpc][iCh][iSide][iWx]) {
	   LOG(ERROR)<<"CbmTofTestBeamClusterizer::WriteHistos: restore unsuccessful! iWx "<<iWx
		     <<" got "<< h1tmp->GetBinContent(iWx+1)
	             <<", expected "<<2.*fvCPWalk[iSmType][iSm*iNbRpc+iRpc][iCh][iSide][iWx]
		     <<FairLogger::endl;	 }
	  }
          htmp->Write();
          h1tmp->Write();
	}
     }
     else{
         LOG(DEBUG)<<"CbmTofTestBeamClusterizer::WriteHistos: update mode "
		   <<fCalMode<<" not yet implemented"
		   <<FairLogger::endl;
     }
   }

   fhDigSpacDifClust->Write();
   fhDigTimeDifClust->Write();
   fhDigDistClust->Write();

   fhClustSizeDifX->Write();
   fhClustSizeDifY->Write();

   fhChDifDifX->Write();
   fhChDifDifY->Write();

   fhNbSameSide->Write();
   fhNbDigiPerChan->Write();


   fhHitsPerTracks->Write();
   if( kFALSE == fDigiBdfPar->ClustUseTrackId() )
      fhPtsPerHit->Write();
   fhTimeResSingHits->Write();
   fhTimeResSingHitsB->Write();
   fhTimePtVsHits->Write();
   fhClusterSize->Write();
   fhClusterSizeType->Write();
   if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
   {
      fhTrackMul->Write();
      fhClusterSizeMulti->Write();
      fhTrk1MulPos->Write();
      fhHiTrkMulPos->Write();
      fhAllTrkMulPos->Write();
      fhMultiTrkProbPos->Divide( fhHiTrkMulPos, fhAllTrkMulPos);
      fhMultiTrkProbPos->Scale( 100.0 );
      fhMultiTrkProbPos->Write();
   } // if( kTRUE == fDigiBdfPar->ClustUseTrackId() )


   gDirectory->cd( oldir->GetPath() );

   fHist->Close();

   return kTRUE;
}
Bool_t   CbmTofTestBeamClusterizer::DeleteHistos()
{
   delete fhClustBuildTime;
   delete fhHitsPerTracks;
   delete fhPtsPerHit;
   delete fhTimeResSingHits;
   delete fhTimeResSingHitsB;
   delete fhTimePtVsHits;
   delete fhClusterSize;
   delete fhClusterSizeType;

   if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
   {
      delete fhTrackMul;
      delete fhClusterSizeMulti;
      delete fhTrk1MulPos;
      delete fhHiTrkMulPos;
      delete fhAllTrkMulPos;
      delete fhMultiTrkProbPos;
   }
   delete fhDigSpacDifClust;
   delete fhDigTimeDifClust;
   delete fhDigDistClust;

   delete fhClustSizeDifX;
   delete fhClustSizeDifY;

   delete fhChDifDifX;
   delete fhChDifDifY;

   delete fhNbSameSide;
   delete fhNbDigiPerChan;

   return kTRUE;
}
/************************************************************************************/
Bool_t   CbmTofTestBeamClusterizer::BuildClusters()
{
   /*
    * FIXME: maybe use the 2D distance (X/Y) as criteria instead of the distance long channel
    * direction
    */
  if(NULL == fTofDigisColl) {
    LOG(INFO) <<" No Digis defined ! Check! " << FairLogger::endl;
    return kFALSE;
  }
 
  LOG(DEBUG)<<"CbmTofTestBeamClusterizer::BuildClusters from "
	    <<fTofDigisColl->GetEntries()<<" digis "<<FairLogger::endl;

   dTRef=0.;     // reset reference time offset 
   fTRefHits=0.;

   Int_t iNbTofDigi  = fTofDigisColl->GetEntries();
   if( kTRUE )
   {
     CbmTofDigiExp *pDigi;
      for( Int_t iDigInd = 0; iDigInd < iNbTofDigi; iDigInd++ )
      {
       pDigi = (CbmTofDigiExp*) fTofDigisColl->At( iDigInd );
       LOG(DEBUG1)<<iDigInd<<" "<<pDigi<<Form(" Address : 0x%08x ",pDigi->GetAddress())
		 <<" SmT " << pDigi->GetType()
		 <<" Sm " << pDigi->GetSm()
		 <<" Rpc "<< pDigi->GetRpc()
		 <<" Ch " << pDigi->GetChannel()
		 <<" S " << pDigi->GetSide()
		 <<" Time "<<pDigi->GetTime()
		 <<" Tot " <<pDigi->GetTot()
		 <<FairLogger::endl;

       Int_t iDetIndx= fMbsMappingPar->GetMappedDetInd( pDigi->GetAddress() );
       /*
       Int_t iUniqueId;
       for(iDetIndx=0; iDetIndx<fMbsMappingPar->GetNbMappedDet(); iDetIndx++){
           iUniqueId = fMbsMappingPar->GetMappedDetUId( iDetIndx );
           LOG(DEBUG)<<Form(" %d. Digi: search detector for 0x%08x : 0x%08x, Side: %f, Time %f",
			    iDigInd,pDigi->GetAddress(),iUniqueId,pDigi->GetSide(),pDigi->GetTime())
		     <<FairLogger::endl;
           if (iUniqueId == (pDigi->GetAddress() & DetMask)) break;
       }

       if ( iDetIndx == fMbsMappingPar->GetNbMappedDet()) return kTRUE; // not a valid detector 

          LOG(DEBUG1)<<Form(" %d. Digi:  found detector for 0x%08x : 0x%08x, Side: %f, Time %f",
			    iDigInd,pDigi->GetAddress(),iUniqueId,pDigi->GetSide(),pDigi->GetTime())
		     <<FairLogger::endl;

       Int_t iMappedDet = fMbsMappingPar->GetMappedDetInd( pDigi->GetAddress() );

       if(iMappedDet != iDetIndx) {
	 LOG(ERROR)<<Form(" Inconcistent detetor mapping %d <-> %d",iDetIndx,iMappedDet)
		   <<FairLogger::endl;
       }
       */
        
       for (Int_t iDigI2 =iDigInd+1; iDigI2<iNbTofDigi;iDigI2++){
	 CbmTofDigiExp *pDigi2 = (CbmTofDigiExp*) fTofDigisColl->At( iDigI2 );
         if(iDetIndx == fMbsMappingPar->GetMappedDetInd( pDigi2->GetAddress() )){
	   if(0.==pDigi->GetSide() && 1.==pDigi2->GetSide())
	     fhRpcDigiCor[iDetIndx]->Fill( pDigi->GetChannel(),  pDigi2->GetChannel() );
	   else 
	     {if (1.==pDigi->GetSide() && 0.==pDigi2->GetSide())
		 fhRpcDigiCor[iDetIndx]->Fill( pDigi2->GetChannel(),  pDigi->GetChannel() );
	     }
	   }
         }
       
      }
   }
  
   Double_t dMaxTimeDist  = fDigiBdfPar->GetMaxTimeDist();
   Double_t dMaxSpaceDist = fDigiBdfPar->GetMaxDistAlongCh();

   LOG(DEBUG)<<" BuildCluster with MaxTimeDist "<<dMaxTimeDist<<", MaxSpaceDist "
	     <<dMaxSpaceDist
	     <<FairLogger::endl;

   // First Time order the Digis array
//   fTofDigisColl->Sort();

   // Then loop over the digis array and store the Digis in separate vectors for
   // each RPC modules
   if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
   {
      CbmTofDigiExp *pDigi;
      for( Int_t iDigInd = 0; iDigInd < iNbTofDigi; iDigInd++ )
      {
         pDigi = (CbmTofDigiExp*) fTofDigisColl->At( iDigInd );
         LOG(DEBUG1)<<"CbmTofTestBeamClusterizer::BuildClusters: "
	           <<iDigInd<<" "<<pDigi<<" "
		   <<pDigi->GetType()<<" "
		   <<pDigi->GetSm()<<" "
		   <<pDigi->GetRpc()<<" "
		   <<pDigi->GetChannel()<<" "
		   <<pDigi->GetTime()<<" "
		   <<pDigi->GetTot()
		   <<FairLogger::endl;
         if(    fDigiBdfPar->GetNbSmTypes() > pDigi->GetType()  // prevent crash due to misconfiguration 
	     &&	fDigiBdfPar->GetNbSm(  pDigi->GetType()) > pDigi->GetSm()
             && fDigiBdfPar->GetNbRpc( pDigi->GetType()) > pDigi->GetRpc()
	     && fDigiBdfPar->GetNbChan(pDigi->GetType(),0) >pDigi->GetChannel() 
		){

         fStorDigiExp[pDigi->GetType()]
                     [pDigi->GetSm()*fDigiBdfPar->GetNbRpc( pDigi->GetType()) + pDigi->GetRpc()]
                     [pDigi->GetChannel()].push_back(pDigi);
         fStorDigiInd[pDigi->GetType()]
                     [pDigi->GetSm()*fDigiBdfPar->GetNbRpc( pDigi->GetType()) + pDigi->GetRpc()]
                     [pDigi->GetChannel()].push_back(iDigInd);

	 // apply calibration vectors 
	 pDigi->SetTime(pDigi->GetTime()- // calibrate Digi Time 
			fvCPTOff[pDigi->GetType()]
		       [pDigi->GetSm()*fDigiBdfPar->GetNbRpc( pDigi->GetType()) + pDigi->GetRpc()]
		       [pDigi->GetChannel()]
		       [pDigi->GetSide()]);

	 pDigi->SetTot(pDigi->GetTot() *  // calibrate Digi ToT 
		fvCPTotGain[pDigi->GetType()]
		       [pDigi->GetSm()*fDigiBdfPar->GetNbRpc( pDigi->GetType()) + pDigi->GetRpc()]
		       [pDigi->GetChannel()]
		       [pDigi->GetSide()]);

	 // walk correction 
         Int_t iWx = (Int_t)(pDigi->GetTot()*nbClWalkBinX/TOTMax-0.5);
         if (0>iWx) iWx=0;
         if (iWx>nbClWalkBinX) iWx=nbClWalkBinX-1;	
	     
         pDigi->SetTime(pDigi->GetTime() -  // calibrate Digi Time 
		fvCPWalk[pDigi->GetType()]
		        [pDigi->GetSm()*fDigiBdfPar->GetNbRpc( pDigi->GetType()) + pDigi->GetRpc()]
		        [pDigi->GetChannel()]
		        [pDigi->GetSide()]
                        [iWx]);

	 LOG(DEBUG1)<<"CbmTofTestBeamClusterizer::BuildClusters: CalDigi "
	            <<iDigInd<<" "
		    <<pDigi->GetType()<<" "
		    <<pDigi->GetSm()<<" "
		    <<pDigi->GetRpc()<<" "
		    <<pDigi->GetChannel()<<" "
		    <<pDigi->GetTime()<<" "
		    <<pDigi->GetTot()
		    <<", TotGain "<<
	               fvCPTotGain[pDigi->GetType()]
		       [pDigi->GetSm()*fDigiBdfPar->GetNbRpc( pDigi->GetType()) + pDigi->GetRpc()]
		       [pDigi->GetChannel()]
		       [pDigi->GetSide()]
	            <<", Walk "<<iWx<<" "<<
		        fvCPWalk[pDigi->GetType()]
		        [pDigi->GetSm()*fDigiBdfPar->GetNbRpc( pDigi->GetType()) + pDigi->GetRpc()]
		        [pDigi->GetChannel()]
		        [pDigi->GetSide()]
                        [iWx]
		   <<FairLogger::endl;

 	 } else 
	   {
           LOG(DEBUG)<<"CbmTofTestBeamClusterizer::BuildClusters: Skip Digi "
		     <<" Type "<<pDigi->GetType()<<" "<< fDigiBdfPar->GetNbSmTypes()
		     <<" Sm "  <<pDigi->GetSm()<<" " << fDigiBdfPar->GetNbSm(pDigi->GetType())
		     <<" Rpc " <<pDigi->GetRpc()<<" "<< fDigiBdfPar->GetNbRpc(pDigi->GetType())
		     <<" Ch " <<pDigi->GetChannel()<<" "<<fDigiBdfPar->GetNbChan(pDigi->GetType(),0)
		     <<FairLogger::endl;
	   }
      } // for( Int_t iDigInd = 0; iDigInd < nTofDigi; iDigInd++ )
   } // if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
      else
      {
         CbmTofDigi *pDigi;
         for( Int_t iDigInd = 0; iDigInd < iNbTofDigi; iDigInd++ )
         {
            pDigi = (CbmTofDigi*) fTofDigisColl->At( iDigInd );
            fStorDigi[pDigi->GetType()]
                     [pDigi->GetSm()*fDigiBdfPar->GetNbRpc( pDigi->GetType()) + pDigi->GetRpc()]
                     [pDigi->GetChannel()].push_back(pDigi);
         fStorDigiInd[pDigi->GetType()]
                     [pDigi->GetSm()*fDigiBdfPar->GetNbRpc( pDigi->GetType()) + pDigi->GetRpc()]
                     [pDigi->GetChannel()].push_back(iDigInd);
         } // for( Int_t iDigInd = 0; iDigInd < nTofDigi; iDigInd++ )
      } // else of if( kTRUE == fDigiBdfPar->UseExpandedDigi() )

   // Then build clusters inside each RPC module
   // Assume only 0 or 1 Digi per channel/side in each event
   // Use simplest method possible, scan direction independent:
   // a) Loop over channels in the RPC starting from 0
   //   * If strips
   //     i) Loop over Digis to check if both ends of the channel have a Digi
   //    ii) Reconstruct a mean channel time and a mean position
   //     + If a Hit is currently filled & the mean position (space, time) is less than XXX from last channel position
   //   iii) Add the mean channel time and the mean position to the ones of the hit
   //     + else
   //   iii) Use nb of strips in cluster to cal. the hit mean time and pos (charge/tot weighting)
   //    iv) Save the hit
   //     v) Start a new hit with current channel
   //   * else (pads)
   //     i) Loop over Digis to find if this channel fired
   //    ii) FIXME: either scan all other channels to check for matching Digis or have more than 1 hit open
   Int_t iNbSmTypes = fDigiBdfPar->GetNbSmTypes();
   // Hit variables
   Double_t dWeightedTime = 0.0;
   Double_t dWeightedPosX = 0.0;
   Double_t dWeightedPosY = 0.0;
   Double_t dWeightedPosZ = 0.0;
   Double_t dWeightsSum   = 0.0;
   std::vector< CbmTofPoint* > vPtsRef;
   std::vector< Int_t > vDigiIndRef;
   Int_t    iNbChanInHit  = 0;
   // Last Channel Temp variables
   Int_t    iLastChan = -1;
   Double_t dLastPosX = 0.0;
   Double_t dLastPosY = 0.0;
   Double_t dLastTime = 0.0;
   // Channel Temp variables
   Double_t dPosX = 0.0;
   Double_t dPosY = 0.0;
   Double_t dPosZ = 0.0;
   Double_t dTime = 0.0;
   Double_t dTotS = 0.0;
   fiNbSameSide = 0;
   if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
   {
      for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
      {
         Int_t iNbSm  = fDigiBdfPar->GetNbSm(  iSmType);
         Int_t iNbRpc = fDigiBdfPar->GetNbRpc( iSmType);
         for( Int_t iSm = 0; iSm < iNbSm; iSm++ )
            for( Int_t iRpc = 0; iRpc < iNbRpc; iRpc++ )
            {
               Int_t    iChType = fDigiBdfPar->GetChanType( iSmType, iRpc );
	       LOG(DEBUG2)<<"CbmTofTestBeamClusterizer::BuildClusters: RPC - Loop  "
			 << Form(" %3d %3d %3d %3d ",iSmType,iSm,iRpc,iChType)
                         <<FairLogger::endl;
               fviClusterMul[iSmType][iSm][iRpc]=0; 
               Int_t  iChId = 0;
               if( 0 == iChType )
               {
                  Int_t iNbCh = fDigiBdfPar->GetNbChan( iSmType, iRpc );
                  // Don't spread clusters over RPCs!!!
                  dWeightedTime = 0.0;
                  dWeightedPosX = 0.0;
                  dWeightedPosY = 0.0;
                  dWeightedPosZ = 0.0;
                  dWeightsSum   = 0.0;
                  iNbChanInHit  = 0;
                  vPtsRef.clear();
                  // For safety reinitialize everything
                  iLastChan = -1;
                  dLastPosX = 0.0;
                  dLastPosY = 0.0;
                  dLastTime = 0.0;
                  LOG(DEBUG2)<<"CbmTofTestBeamClusterizer::BuildClusters: ChanOrient "
                             << Form(" %3d %3d %3d %3d %3d ",iSmType,iSm,iRpc,fDigiBdfPar->GetChanOrient( iSmType, iRpc ),iNbCh)
                             <<FairLogger::endl;

                  if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
                  {
                     // Horizontal strips => X comes from left right time difference
                  } // if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
                     else
                     {
                        // Vertical strips => Y comes from bottom top time difference
                        for( Int_t iCh = 0; iCh < iNbCh; iCh++ )
                        {
			  LOG(DEBUG2)<<"CbmTofTestBeamClusterizer::BuildClusters: VDigisize "
			     << Form(" T %3d Sm %3d R %3d Ch %3d Size %3d ",iSmType,iSm,iRpc,iCh,fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].size())
                             <<FairLogger::endl;

                           if( 0 < fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].size() )
                              fhNbDigiPerChan->Fill( fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].size() );
                           while( 1 < fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].size() )
                           {
			       LOG(DEBUG2) << "CbmTofTestBeamClusterizer::BuildClusters: digis processing for " 
					  << Form(" SmT %3d Sm %3d Rpc %3d Ch %3d # %3d ",iSmType,iSm,iRpc,iCh,
						  fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].size())
                               <<FairLogger::endl;
			       /*
			      if (3 < fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].size()) // needs more work!
			       LOG(DEBUG) << "CbmTofTestBeamClusterizer::BuildClusters: digis ignored for " 
			       << Form(" SmT %3d Sm %3d Rpc %3d Ch %3d # %3d ",iSmType,iSm,iRpc,iCh,fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].size())
                               <<FairLogger::endl;
			       */

                               while( (fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh][0])->GetSide() ==
                                      (fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh][1])->GetSide() )
                               {
                                 // Not one Digi of each end!
                                 fiNbSameSide++;
                                 LOG(DEBUG) << "CbmTofTestBeamClusterizer::BuildClusters: SameSide Hits! Times: "
					      <<   (fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh][0])->GetTime()
					      << ", "<<(fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh][1])->GetTime()
					      <<", DeltaT " <<(fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh][1])->GetTime() - 
                                                              (fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh][0])->GetTime()
				              <<FairLogger::endl;
                                    LOG(DEBUG2)<<" SameSide Erase fStor entries(d) "<<iSmType<<", SR "<<iSm*iNbRpc+iRpc<<", Ch"<<iCh
					       <<FairLogger::endl;
				    fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].erase(fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].begin());
				    fStorDigiInd[iSmType][iSm*iNbRpc+iRpc][iCh].erase(fStorDigiInd[iSmType][iSm*iNbRpc+iRpc][iCh].begin());
				 if(2 > fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].size()) break;
				 continue;  
                               }

			      LOG(DEBUG2) << "CbmTofTestBeamClusterizer::BuildClusters: digss processing for " 
					  << Form(" SmT %3d Sm %3d Rpc %3d Ch %3d # %3d ",iSmType,iSm,iRpc,iCh,
						  fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].size())
		  	                  <<FairLogger::endl;
                              if(2 > fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].size()) break;
                              Int_t iLastChId = iChId; // Save Last hit channel

                              // 2 Digis = both sides present
                              CbmTofDetectorInfo xDetInfo(kTOF, iSmType, iSm, iRpc, 0, iCh);
                              iChId = fTofId->SetDetectorInfo( xDetInfo );
			      Int_t iUCellId=CbmTofAddress::GetUniqueAddress(iSm,iRpc,iCh,0,iSmType);
			      LOG(DEBUG1)<<"CbmTofTestBeamClusterizer::BuildClusters:" 
					 << Form(" T %3d Sm %3d R %3d Ch %3d size %3d ",
						 iSmType,iSm,iRpc,iCh,fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].size())
					 << Form(" ChId: 0x%08x 0x%08x ",iChId,iUCellId)
					 <<FairLogger::endl;
                              fChannelInfo = fDigiPar->GetCell( iChId );
                              if(NULL == fChannelInfo){
			      LOG(ERROR)<<"CbmTofTestBeamClusterizer::BuildClusters: no geometry info! "
					<< Form(" %3d %3d %3d %3d 0x%08x 0x%08x ",iSmType, iSm, iRpc, iCh, iChId,iUCellId)
					<<FairLogger::endl;
			      break;
			      }

                              CbmTofDigiExp * xDigiA = fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh][0];
                              CbmTofDigiExp * xDigiB = fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh][1];


                              // The "Strip" time is the mean time between each end
                              dTime = xDigiA->GetTime() -fvCPTOff[iSmType][iSm*iNbRpc+iRpc][iCh][0]
				    + xDigiB->GetTime() -fvCPTOff[iSmType][iSm*iNbRpc+iRpc][iCh][1];
                              dTime /= 2.0;

                              // Weight is the total charge => sum of both ends ToT
                              dTotS = xDigiA->GetTot() + xDigiB->GetTot();

                              // X position is just the center of the channel
                              dPosX = fChannelInfo->GetX();

                              // Y position form strip ends time difference
                              dPosY = fChannelInfo->GetY();
			      
                              if( 1 == xDigiA->GetSide() )
			     
                                 // 0 is the top side, 1 is the bottom side
                                 dPosY += fDigiBdfPar->GetSignalSpeed() *
				        ( xDigiA->GetTime() - xDigiB->GetTime() )/2.0;

                              else
			      
                                 // 0 is the bottom side, 1 is the top side
                                 dPosY += fDigiBdfPar->GetSignalSpeed() *
				       ( xDigiB->GetTime() - xDigiA->GetTime() )/2.0;

                              // For Z always just take the one of the channel itself( in fact its gap one)
                              dPosZ = fChannelInfo->GetZ();

			      LOG(DEBUG1)
				   <<"CbmTofTestBeamClusterizer::BuildClusters: NbChanInHit  "
				   << Form(" %3d %3d %3d 0x%08x %d Time %f PosY %f Svel %f ",
					   iNbChanInHit,iCh,iLastChan,xDigiA,xDigiA->GetSide(),
					   dTime,dPosY,fDigiBdfPar->GetSignalSpeed())

				   << Form( ", Offs %f, %f ",fvCPTOff[iSmType][iSm*iNbRpc+iRpc][iCh][0],
					                     fvCPTOff[iSmType][iSm*iNbRpc+iRpc][iCh][1])
				   <<FairLogger::endl;

                              // Now check if a hit/cluster is already started
                              if( 0 < iNbChanInHit) 
                              {


                                 if( iLastChan == iCh - 1 )
                                 {
                                    fhDigTimeDifClust->Fill( dTime - dLastTime );
                                    fhDigSpacDifClust->Fill( dPosY - dLastPosY );
                                    fhDigDistClust->Fill( dPosY - dLastPosY,
                                                          dTime - dLastTime );
                                 } // if( iLastChan == iCh - 1 )
                                 // a cluster is already started => check distance in space/time
                                 // For simplicity, just check along strip direction for now
                                 // and break cluster when a not fired strip is found
                                 if( TMath::Abs( dTime - dLastTime) < dMaxTimeDist &&
                                      iLastChan == iCh - 1 &&
                                      TMath::Abs( dPosY - dLastPosY) < dMaxSpaceDist  )
                                 {
                                    // Add to cluster/hit
                                    dWeightedTime += dTime*dTotS;
                                    dWeightedPosX += dPosX*dTotS;
                                    dWeightedPosY += dPosY*dTotS;
                                    dWeightedPosZ += dPosZ*dTotS;
                                    dWeightsSum   += dTotS;
                                    iNbChanInHit  += 1;
                                    // if one of the side digi comes from a CbmTofPoint not already found
                                    // in this cluster, save its pointer
                                    Bool_t bFoundA = kFALSE;
                                    Bool_t bFoundB = kFALSE;
                                    if(NULL != fTofPointsColl){ // MC 
                                    if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
                                       for( Int_t iPtRef = 0; iPtRef < vPtsRef.size(); iPtRef++)
                                       {
					     //if( ((CbmTofPoint*)(vPtsRef[iPtRef]))->GetTrackID() == ((CbmTofPoint*)(xDigiA->GetLinks()))->GetTrackID() )
                                             bFoundA = kTRUE;
					     //if( ((CbmTofPoint*)(vPtsRef[iPtRef]))->GetTrackID() == ((CbmTofPoint*)(xDigiB->GetLinks()))->GetTrackID() )
                                             bFoundB = kTRUE;
                                       } // for( Int
                                       else for( Int_t iPtRef = 0; iPtRef < vPtsRef.size(); iPtRef++)
                                       {
					 //                                          if( vPtsRef[iPtRef] == (CbmTofPoint*)xDigiA->GetLinks() )
                                             bFoundA = kTRUE;
					 //                                          if( vPtsRef[iPtRef] == (CbmTofPoint*)xDigiB->GetLinks() )
                                             bFoundB = kTRUE;
                                       } // for( Int_t iPtRef = 0; iPtRef < vPtsRef.size(); iPtRef++)

                                    // CbmTofPoint pointer for 1st digi not found => save it
                                    //if( kFALSE == bFoundA )
				      //                                       vPtsRef.push_back( (CbmTofPoint*)(xDigiA->GetLinks()) );
                                    // CbmTofPoint pointer for 2nd digi not found => save it
				      //                                    if( kFALSE == bFoundB )
				      //                                       vPtsRef.push_back( (CbmTofPoint*)(xDigiB->GetLinks()) );
				    } // MC end 
				    vDigiIndRef.push_back( (Int_t )(fStorDigiInd[iSmType][iSm*iNbRpc+iRpc][iCh][0]));
				    vDigiIndRef.push_back( (Int_t )(fStorDigiInd[iSmType][iSm*iNbRpc+iRpc][iCh][1]));

                                    LOG(DEBUG)<<" Add Digi and erase fStor entries(a): NbChanInHit "<< iNbChanInHit<<", "
					     <<iSmType<<", SR "<<iSm*iNbRpc+iRpc<<", Ch"<<iCh
					     <<FairLogger::endl;
		
				    fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].erase(fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].begin());
				    fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].erase(fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].begin());
				    fStorDigiInd[iSmType][iSm*iNbRpc+iRpc][iCh].erase(fStorDigiInd[iSmType][iSm*iNbRpc+iRpc][iCh].begin());
				    fStorDigiInd[iSmType][iSm*iNbRpc+iRpc][iCh].erase(fStorDigiInd[iSmType][iSm*iNbRpc+iRpc][iCh].begin());
 
                                } // if current Digis compatible with last fired chan
                                 else
                                 {
                                    // Save Hit
                                    dWeightedTime /= dWeightsSum;
                                    dWeightedPosX /= dWeightsSum;
                                    dWeightedPosY /= dWeightsSum;
                                    dWeightedPosZ /= dWeightsSum;
                                    TVector3 hitPos(dWeightedPosX, dWeightedPosY, dWeightedPosZ);
                                    // Simple errors, not properly done at all for now
                                    // Right way of doing it should take into account the weight distribution
                                    // and real system time resolution
                                    TVector3 hitPosErr( fChannelInfo->GetSizex()/TMath::Sqrt(12.0),   // Single strips approximation
                                                        fDigiBdfPar->GetFeeTimeRes() * fDigiBdfPar->GetSignalSpeed(), // Use the electronics resolution
                                                        fDigiBdfPar->GetNbGaps( iSmType, iRpc)*
                                                        fDigiBdfPar->GetGapSize( iSmType, iRpc)/10.0 / // Change gap size in cm
                                                        TMath::Sqrt(12.0) ); // Use full RPC thickness as "Channel" Z size

				    // Int_t iDetId = vPtsRef[0]->GetDetectorID();// detID = pt->GetDetectorID() <= from TofPoint
                                    Int_t iDetId = iLastChId;  //last channel identifier
                                    Int_t iRefId = 0; // Index of the correspondng TofPoint
                                    if(NULL != fTofPointsColl) {
				      iRefId = fTofPointsColl->IndexOf( vPtsRef[0] );
				    }
				    LOG(DEBUG1)<<"CbmTofTestBeamClusterizer::BuildClusters: Save Hit  "
					       << Form(" %3d %3d 0x%08x %3d %3d %3d %f %f",
						       fiNbHits,iNbChanInHit,iDetId,iCh,iLastChan,iRefId,
						       dWeightedTime,dWeightedPosY)
				               <<", DigiSize: "<<vDigiIndRef.size()
					       <<", DigiInds: ";
                                    fviClusterMul[iSmType][iSm][iRpc]++; 
				    for (Int_t i=0; i<vDigiIndRef.size();i++){
				      LOG(DEBUG1)<<" "<<vDigiIndRef.at(i);
				    }
				    LOG(DEBUG1)  <<FairLogger::endl;
				    
                                    new((*fTofHitsColl)[fiNbHits]) CbmTofHit( iDetId,
                                                        hitPos, hitPosErr,
                                                        iRefId,
                                                        dWeightedTime,
							vPtsRef.size(), // flag  = number of TofPoints generating the cluster
							0) ; //channel
				      //		vDigiIndRef);

				    CbmMatch* digiMatch = new CbmMatch();
				    for (Int_t i=0; i<vDigiIndRef.size();i++){
				      digiMatch->AddLink(CbmLink(0.,vDigiIndRef.at(i)));
				    }
				    new((*fTofDigiMatchColl)[fiNbHits]) CbmMatch(*digiMatch);
				    delete digiMatch;

                                    LOG(DEBUG)<<"CbmTofTestBeamClusterizer::BuildClusters: Inspect Hit for TRef:"
					      <<Form(" %08x  %08x ",(iDetId & DetMask),fTRefMode)
					      <<Form(" fTRef %d, dTRef %f, %f",fTRefHits,dTRef,dWeightedTime)
					      <<FairLogger::endl;

				    if((iDetId & DetMask) == fTRefMode){
				      fTRefHits++;
                                      dTRef += dWeightedTime;
				    }
				  				    
                                    // Using the SetLinks/GetLinks of the TofHit class seems to conflict
                                    // with something in littrack QA
//                                    ((CbmTofHit*)fTofHitsColl->At(fiNbHits))->SetLinks(vPtsRef[0]);
                                    fiNbHits++;
                                    // For Histogramming
                                    fviClusterSize[iSmType][iRpc].push_back(iNbChanInHit);
                                    fviTrkMul[iSmType][iRpc].push_back( vPtsRef.size() );
                                    fvdX[iSmType][iRpc].push_back(dWeightedPosX);
                                    fvdY[iSmType][iRpc].push_back(dWeightedPosY);
				    /*  no TofPoint available for data!  
                                    fvdDifX[iSmType][iRpc].push_back( vPtsRef[0]->GetX() - dWeightedPosX);
                                    fvdDifY[iSmType][iRpc].push_back( vPtsRef[0]->GetY() - dWeightedPosY);
                                    fvdDifCh[iSmType][iRpc].push_back( fGeoHandler->GetCell( vPtsRef[0]->GetDetectorID() ) -1 -iLastChan );
				    */
                                    vPtsRef.clear();
                                    vDigiIndRef.clear();

                                    // Start a new hit
                                    dWeightedTime = dTime*dTotS;
                                    dWeightedPosX = dPosX*dTotS;
                                    dWeightedPosY = dPosY*dTotS;
                                    dWeightedPosZ = dPosZ*dTotS;
                                    dWeightsSum   = dTotS;
                                    iNbChanInHit  = 1;
                                    // Save pointer on CbmTofPoint
				    //                                    vPtsRef.push_back( (CbmTofPoint*)(xDigiA->GetLinks()) );
                                    // Save next digi address
                                    LOG(DEBUG2)<<" Next fStor Digi "<<iSmType<<", SR "<<iSm*iNbRpc+iRpc<<", Ch"<<iCh
					       <<", Dig0 "<<(fStorDigiInd[iSmType][iSm*iNbRpc+iRpc][iCh][0])
					       <<", Dig1 "<<(fStorDigiInd[iSmType][iSm*iNbRpc+iRpc][iCh][1])
					       <<FairLogger::endl;

				    vDigiIndRef.push_back( (Int_t )(fStorDigiInd[iSmType][iSm*iNbRpc+iRpc][iCh][0]));
				    vDigiIndRef.push_back( (Int_t )(fStorDigiInd[iSmType][iSm*iNbRpc+iRpc][iCh][1]));
                                    LOG(DEBUG2)<<" Erase fStor entries(b) "<<iSmType<<", SR "<<iSm*iNbRpc+iRpc<<", Ch"<<iCh
					       <<FairLogger::endl;
				    fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].erase(fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].begin());
				    fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].erase(fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].begin());
				    fStorDigiInd[iSmType][iSm*iNbRpc+iRpc][iCh].erase(fStorDigiInd[iSmType][iSm*iNbRpc+iRpc][iCh].begin());
				    fStorDigiInd[iSmType][iSm*iNbRpc+iRpc][iCh].erase(fStorDigiInd[iSmType][iSm*iNbRpc+iRpc][iCh].begin());

                                    if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
                                    {
				      //                                       if( ((CbmTofPoint*)(xDigiA->GetLinks()))->GetTrackID() !=
				      //                       ((CbmTofPoint*)(xDigiB->GetLinks()))->GetTrackID() )
                                          // if other side come from a different Track,
                                          // also save the pointer on CbmTofPoint
				      //  vPtsRef.push_back( (CbmTofPoint*)(xDigiB->GetLinks()) );
                                    } // if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
				    //else if( xDigiA->GetLinks() != xDigiB->GetLinks() )
                                          // if other side come from a different TofPoint,
                                          // also save the pointer on CbmTofPoint
				    //    vPtsRef.push_back( (CbmTofPoint*)(xDigiB->GetLinks()) );
                                 } // else of if current Digis compatible with last fired chan
                              } // if( 0 < iNbChanInHit)
                                 else
                                 {
 		                    LOG(DEBUG1)<<"CbmTofTestBeamClusterizer::BuildClusters: 1.Hit on channel " 
					       <<iCh<<", time: "<<dTime<<FairLogger::endl;

                                    // first fired strip in this RPC
                                    dWeightedTime = dTime*dTotS;
                                    dWeightedPosX = dPosX*dTotS;
                                    dWeightedPosY = dPosY*dTotS;
                                    dWeightedPosZ = dPosZ*dTotS;
                                    dWeightsSum   = dTotS;
                                    iNbChanInHit  = 1;
                                    // Save pointer on CbmTofPoint
                                    //if(NULL != fTofPointsColl)
				    //                                    vPtsRef.push_back( (CbmTofPoint*)(xDigiA->GetLinks()) );
				    vDigiIndRef.push_back( (Int_t )(fStorDigiInd[iSmType][iSm*iNbRpc+iRpc][iCh][0]));
				    vDigiIndRef.push_back( (Int_t )(fStorDigiInd[iSmType][iSm*iNbRpc+iRpc][iCh][1]));

                                    LOG(DEBUG2)<<" Erase fStor entries(c) "<<iSmType<<", SR "<<iSm*iNbRpc+iRpc<<", Ch"<<iCh
					       <<FairLogger::endl;
				    fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].erase(fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].begin());
				    fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].erase(fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].begin());
				    fStorDigiInd[iSmType][iSm*iNbRpc+iRpc][iCh].erase(fStorDigiInd[iSmType][iSm*iNbRpc+iRpc][iCh].begin());
				    fStorDigiInd[iSmType][iSm*iNbRpc+iRpc][iCh].erase(fStorDigiInd[iSmType][iSm*iNbRpc+iRpc][iCh].begin());

                                    if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
                                    {
				      //                                      if( ((CbmTofPoint*)(xDigiA->GetLinks()))->GetTrackID() !=
				      //     ((CbmTofPoint*)(xDigiB->GetLinks()))->GetTrackID() )
                                          // if other side come from a different Track,
                                          // also save the pointer on CbmTofPoint
				      //                                          vPtsRef.push_back( (CbmTofPoint*)(xDigiB->GetLinks()) );
                                    } // if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
				    //  else if( xDigiA->GetLinks() != xDigiB->GetLinks() )
                                          // if other side come from a different TofPoint,
                                          // also save the pointer on CbmTofPoint
				    //    vPtsRef.push_back( (CbmTofPoint*)(xDigiB->GetLinks()) );
                                 } // else of if( 0 < iNbChanInHit)
                              iLastChan = iCh;
                              dLastPosX = dPosX;
                              dLastPosY = dPosY;
                              dLastTime = dTime;
                           } // if( 2 == fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].size() )
                           fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].clear();
                           fStorDigiInd[iSmType][iSm*iNbRpc+iRpc][iCh].clear();
                        } // for( Int_t iCh = 0; iCh < iNbCh; iCh++ )
			LOG(DEBUG2)<<"CbmTofTestBeamClusterizer::BuildClusters: finished V-RPC"
				   << Form(" %3d %3d %3d %d",iSmType,iSm,iRpc,fTofHitsColl->GetEntries())
				   <<FairLogger::endl;
                     } // else of if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
               } // if( 0 == iChType)
                  else
                  {
                     LOG(ERROR)<<"CbmTofTestBeamClusterizer::BuildClusters => Cluster building "
                           <<"from digis to hits not implemented for pads, Sm type "
                           <<iSmType<<" Rpc "<<iRpc<<FairLogger::endl;
                     return kFALSE;
                  } // else of if( 0 == iChType)

               // Now check if another hit/cluster is started
               // and save it if it's the case
               if( 0 < iNbChanInHit)
               {
		   LOG(DEBUG1)<<"CbmTofTestBeamClusterizer::BuildClusters: Process cluster " 
			      <<iNbChanInHit<<FairLogger::endl;

                  // Check orientation to properly assign errors
                  if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
                  {
		     LOG(DEBUG1)<<"CbmTofTestBeamClusterizer::BuildClusters: H-Hit " <<FairLogger::endl;
                  } // if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
                  else
                  {
		     LOG(DEBUG2)<<"CbmTofTestBeamClusterizer::BuildClusters: V-Hit " <<FairLogger::endl;
                     // Save Hit
                     dWeightedTime /= dWeightsSum;
                     dWeightedPosX /= dWeightsSum;
                     dWeightedPosY /= dWeightsSum;
                     dWeightedPosZ /= dWeightsSum;
                     TVector3 hitPos(dWeightedPosX, dWeightedPosY, dWeightedPosZ);
                     // TestBeam errors, not properly done at all for now
                     // Right way of doing it should take into account the weight distribution
                     // and real system time resolution
                     TVector3 hitPosErr( fChannelInfo->GetSizex()/TMath::Sqrt(12.0),   // Single strips approximation
                                         fDigiBdfPar->GetFeeTimeRes() * fDigiBdfPar->GetSignalSpeed(), // Use the electronics resolution
                                         fDigiBdfPar->GetNbGaps( iSmType, iRpc)*
                                         fDigiBdfPar->GetGapSize( iSmType, iRpc)/10.0 / // Change gap size in cm
                                         TMath::Sqrt(12.0) ); // Use full RPC thickness as "Channel" Z size
//                     cout<<"a "<<vPtsRef.size()<<endl;
//                     cout<<"b "<<vPtsRef[0]<<endl;
//                     cout<<"c "<<vPtsRef[0]->GetDetectorID()<<endl;
//                     Int_t iDetId = vPtsRef[0]->GetDetectorID();// detID = pt->GetDetectorID() <= from TofPoint
                     Int_t iDetId = iChId;
                     Int_t iRefId = 0; // Index of the correspondng TofPoint
                     if(NULL != fTofPointsColl) iRefId = fTofPointsColl->IndexOf( vPtsRef[0] );
		     LOG(DEBUG1)<<"CbmTofTestBeamClusterizer::BuildClusters: Save V-Hit  "
		     << Form(" %3d %3d 0x%08x %3d %3d %3d 0x%08x",
			     fiNbHits,iNbChanInHit,iDetId,iLastChan,iRefId) //vPtsRef.size(),vPtsRef[0])
		       //   dWeightedTime,dWeightedPosY)
				<<", DigiSize: "<<vDigiIndRef.size();
		     LOG(DEBUG1)<<", DigiInds: ";
                     for (Int_t i=0; i<vDigiIndRef.size();i++){
		       LOG(DEBUG1)<<" "<<vDigiIndRef.at(i);
		     }
		     LOG(DEBUG1)  <<FairLogger::endl;
                     
		     fviClusterMul[iSmType][iSm][iRpc]++; 

                     new((*fTofHitsColl)[fiNbHits]) CbmTofHit( iDetId,
                                         hitPos, hitPosErr,
					 fiNbHits, //iRefId
                                         dWeightedTime,
					 vPtsRef.size(),
					 0);
		     //		                         vDigiIndRef);
		     CbmMatch* digiMatch = new CbmMatch();
                     for (Int_t i=0; i<vDigiIndRef.size();i++){
		       digiMatch->AddLink(CbmLink(0.,vDigiIndRef.at(i)));
		     }
                     new((*fTofDigiMatchColl)[fiNbHits]) CbmMatch(*digiMatch);
		     delete digiMatch;

                     LOG(DEBUG)<<"CbmTofTestBeamClusterizer::BuildClusters: Inspect Hit for TRef:"
			      <<Form(" %08x  %08x ",(iDetId & DetMask),fTRefMode)
			      <<Form(" fTRef %d, dTRef %f, %f",fTRefHits,dTRef,dWeightedTime)
			      <<FairLogger::endl;

 		     if((iDetId & DetMask) == fTRefMode){
			fTRefHits++;
                        dTRef += dWeightedTime;
		     }
		     /*
		     fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iLastChan].erase(fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iLastChan].begin());
		     fStorDigiInd[iSmType][iSm*iNbRpc+iRpc][iLastChan].erase(fStorDigiInd[iSmType][iSm*iNbRpc+iRpc][iLastChan].begin());
		     */

 // flag  = number of TofPoints generating the cluster
                     // Using the SetLinks/GetLinks of the TofHit class seems to conflict
                     // with something in littrack QA
//                     ((CbmTofHit*)fTofHitsColl->At(fiNbHits))->SetLinks(vPtsRef[0]);
                     fiNbHits++;
                     // For Histogramming
                     fviClusterSize[iSmType][iRpc].push_back(iNbChanInHit);
                     fviTrkMul[iSmType][iRpc].push_back( vPtsRef.size() );
                     fvdX[iSmType][iRpc].push_back(dWeightedPosX);
                     fvdY[iSmType][iRpc].push_back(dWeightedPosY);
		     /*
                     fvdDifX[iSmType][iRpc].push_back( vPtsRef[0]->GetX() - dWeightedPosX);
                     fvdDifY[iSmType][iRpc].push_back( vPtsRef[0]->GetY() - dWeightedPosY);
                     fvdDifCh[iSmType][iRpc].push_back( fGeoHandler->GetCell( vPtsRef[0]->GetDetectorID() ) -1 -iLastChan );
		     */
                     vPtsRef.clear();
                     vDigiIndRef.clear();
                  } // else of if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
               } // if( 0 < iNbChanInHit)
	       LOG(DEBUG2)<<" Fini-A "<<Form(" %3d %3d %3d ",iSmType, iSm, iRpc)<<FairLogger::endl;
            } // for each sm/rpc pair
	       LOG(DEBUG2)<<" Fini-B "<<Form(" %3d ",iSmType)<<FairLogger::endl;
      } // for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
   } // if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
      else
      {
         for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
         {
            Int_t iNbSm  = fDigiBdfPar->GetNbSm(  iSmType);
            Int_t iNbRpc = fDigiBdfPar->GetNbRpc( iSmType);
            for( Int_t iSm = 0; iSm < iNbSm; iSm++ )
               for( Int_t iRpc = 0; iRpc < iNbRpc; iRpc++ )
               {
		  Int_t iChId = 0;
                  Int_t    iChType = fDigiBdfPar->GetChanType( iSmType, iRpc );
                  if( 0 == iChType )
                  {
                     Int_t iNbCh = fDigiBdfPar->GetNbChan( iSmType, iRpc );
                     // Don't spread clusters over RPCs!!!
                     dWeightedTime = 0.0;
                     dWeightedPosX = 0.0;
                     dWeightedPosY = 0.0;
                     dWeightedPosZ = 0.0;
                     dWeightsSum   = 0.0;
                     iNbChanInHit  = 0;
                     vPtsRef.clear();
                     // For safety reinitialize everything
                     iLastChan = -1;
                     dLastPosX = 0.0;
                     dLastPosY = 0.0;
                     dLastTime = 0.0;
                     if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
                     {
                        // Horizontal strips => X comes from left right time difference
                     } // if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
                        else
                        {
                           // Vertical strips => Y comes from bottom top time difference
                           for( Int_t iCh = 0; iCh < iNbCh; iCh++ )
                           {
                              if( 0 < fStorDigi[iSmType][iSm*iNbRpc+iRpc][iCh].size() )
                                 fhNbDigiPerChan->Fill( fStorDigi[iSmType][iSm*iNbRpc+iRpc][iCh].size() );
//                              if( 2 == fStorDigi[iSmType][iSm][iRpc][iCh].size() )
                              if( 2 == fStorDigi[iSmType][iSm*iNbRpc+iRpc][iCh].size() )
                              {
                                 if( (fStorDigi[iSmType][iSm*iNbRpc+iRpc][iCh][0])->GetSide() ==
                                       (fStorDigi[iSmType][iSm*iNbRpc+iRpc][iCh][1])->GetSide() )
                                 {
                                    // Not one Digi of each end!
                                    fiNbSameSide++;
                                    continue;
                                 }

                                 // 2 Digis = both sides present
                                 CbmTofDetectorInfo xDetInfo(kTOF, iSmType, iSm, iRpc, 0, iCh + 1);
                                 iChId = fTofId->SetDetectorInfo( xDetInfo );
                                 fChannelInfo = fDigiPar->GetCell( iChId );

                                 CbmTofDigi * xDigiA = fStorDigi[iSmType][iSm*iNbRpc+iRpc][iCh][0];
                                 CbmTofDigi * xDigiB = fStorDigi[iSmType][iSm*iNbRpc+iRpc][iCh][1];

				 vDigiIndRef.push_back( (Int_t )(fStorDigiInd[iSmType][iSm*iNbRpc+iRpc][iCh][0]));
				 vDigiIndRef.push_back( (Int_t )(fStorDigiInd[iSmType][iSm*iNbRpc+iRpc][iCh][1]));

                                 // The "Strip" time is the mean time between each end
                                 dTime = xDigiA->GetTime() + xDigiB->GetTime();
                                 dTime /= 2.0;

                                 // Weight is the total charge => sum of both ends ToT
                                 dTotS = xDigiA->GetTot() + xDigiB->GetTot();

                                 // X position is just the center of the channel
                                 dPosX = fChannelInfo->GetX();

                                 // Y position form strip ends time difference
                                 dPosY = fChannelInfo->GetY();
                                 if( 1 == xDigiA->GetSide() )
                                    // 0 is the top side, 1 is the bottom side
                                    dPosY += fDigiBdfPar->GetSignalSpeed() *
                                             ( xDigiA->GetTime() - xDigiB->GetTime() )/2.0;
                                 else
                                    // 0 is the bottom side, 1 is the top side
                                    dPosY += fDigiBdfPar->GetSignalSpeed() *
                                             ( xDigiB->GetTime() - xDigiA->GetTime() )/2.0;

                                 // For Z always just take the one of the channel itself( in fact its gap one)
                                 dPosZ = fChannelInfo->GetZ();

                                 // Now check if a hit/cluster is already started
                                 if( 0 < iNbChanInHit)
                                 {
                                    if( iLastChan == iCh - 1 )
                                    {
                                       fhDigTimeDifClust->Fill( dTime - dLastTime );
                                       fhDigSpacDifClust->Fill( dPosY - dLastPosY );
                                       fhDigDistClust->Fill( dPosY - dLastPosY,
                                                             dTime - dLastTime );
                                    } // if( iLastChan == iCh - 1 )
                                    // a cluster is already started => check distance in space/time
                                    // For simplicity, just check along strip direction for now
                                    if( TMath::Abs( dTime - dLastTime) < dMaxTimeDist &&
                                         iLastChan == iCh - 1 &&
                                         TMath::Abs( dPosY - dLastPosY) < dMaxSpaceDist  )
                                    {
                                       // Add to cluster/hit
                                       dWeightedTime += dTime*dTotS;
                                       dWeightedPosX += dPosX*dTotS;
                                       dWeightedPosY += dPosY*dTotS;
                                       dWeightedPosZ += dPosZ*dTotS;
                                       dWeightsSum   += dTotS;
                                       iNbChanInHit  += 1;
                                       // if one of the side digi comes from a CbmTofPoint not already found
                                       // in this cluster, save its pointer
                                       Bool_t bFoundA = kFALSE;
                                       Bool_t bFoundB = kFALSE;
                                       if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
                                          for( Int_t iPtRef = 0; iPtRef < vPtsRef.size(); iPtRef++)
                                          {
					    //                                             if( ((CbmTofPoint*)(vPtsRef[iPtRef]))->GetTrackID() == ((CbmTofPoint*)(xDigiA->GetLinks()))->GetTrackID() )
                                                bFoundA = kTRUE;
						// if( ((CbmTofPoint*)(vPtsRef[iPtRef]))->GetTrackID() == ((CbmTofPoint*)(xDigiB->GetLinks()))->GetTrackID() )
                                                bFoundB = kTRUE;
                                          } // for( Int
                                          else for( Int_t iPtRef = 0; iPtRef < vPtsRef.size(); iPtRef++)
                                          {
					    //                                             if( vPtsRef[iPtRef] == (CbmTofPoint*)xDigiA->GetLinks() )
                                                bFoundA = kTRUE;
						//  if( vPtsRef[iPtRef] == (CbmTofPoint*)xDigiB->GetLinks() )
                                                bFoundB = kTRUE;
                                          } // for( Int_t iPtRef = 0; iPtRef < vPtsRef.size(); iPtRef++)

                                       // CbmTofPoint pointer for 1st digi not found => save it
				       //                                       if( kFALSE == bFoundA )
					 //                                          vPtsRef.push_back( (CbmTofPoint*)(xDigiA->GetLinks()) );
                                       // CbmTofPoint pointer for 2nd digi not found => save it
                                       // if( kFALSE == bFoundB )
					 //                                          vPtsRef.push_back( (CbmTofPoint*)(xDigiB->GetLinks()) );
                                    } // if current Digis compatible with last fired chan
                                    else
                                    {
                                       // Save Hit
                                       dWeightedTime /= dWeightsSum;
                                       dWeightedPosX /= dWeightsSum;
                                       dWeightedPosY /= dWeightsSum;
                                       dWeightedPosZ /= dWeightsSum;
                                       TVector3 hitPos(dWeightedPosX, dWeightedPosY, dWeightedPosZ);
                                       // Simple errors, not properly done at all for now
                                       // Right way of doing it should take into account the weight distribution
                                       // and real system time resolution
                                       TVector3 hitPosErr( fChannelInfo->GetSizey()/TMath::Sqrt(12.0),   // Single strips approximation
                                                           fDigiBdfPar->GetFeeTimeRes() * fDigiBdfPar->GetSignalSpeed(), // Use the electronics resolution
                                                           fDigiBdfPar->GetNbGaps( iSmType, iRpc)*
                                                           fDigiBdfPar->GetGapSize( iSmType, iRpc)/10.0 / // Change gap size in cm
                                                           TMath::Sqrt(12.0) ); // Use full RPC thickness as "Channel" Z size
                                       Int_t iDetId = vPtsRef[0]->GetDetectorID();// detID = pt->GetDetectorID() <= from TofPoint
//                                       Int_t iDetId = 0;
				       Int_t iRefId = 0; // Index of the correspondng TofPoint
				       if(NULL != fTofPointsColl) iRefId = fTofPointsColl->IndexOf( vPtsRef[0] );

                                       new((*fTofHitsColl)[fiNbHits]) CbmTofHit( iDetId,
                                                           hitPos, hitPosErr,
                                                           iRefId,
                                                           dWeightedTime,
							   vPtsRef.size(),
										 0);
				       //	   vDigiIndRef); // flag  = number of TofPoints generating the cluster
                                       // Using the SetLinks/GetLinks of the TofHit class seems to conflict
                                       // with something in littrack QA
//                                       ((CbmTofHit*)fTofHitsColl->At(fiNbHits))->SetLinks(vPtsRef[0]);
                                       fiNbHits++;
                                       // For Histogramming
                                       fviClusterSize[iSmType][iRpc].push_back(iNbChanInHit);
                                       fviTrkMul[iSmType][iRpc].push_back( vPtsRef.size() );
                                       fvdX[iSmType][iRpc].push_back(dWeightedPosX);
                                       fvdY[iSmType][iRpc].push_back(dWeightedPosY);
                                       fvdDifX[iSmType][iRpc].push_back( vPtsRef[0]->GetX() - dWeightedPosX);
                                       fvdDifY[iSmType][iRpc].push_back( vPtsRef[0]->GetY() - dWeightedPosY);
                                       fvdDifCh[iSmType][iRpc].push_back( fGeoHandler->GetCell( vPtsRef[0]->GetDetectorID() ) -1 -iLastChan );
                                       vPtsRef.clear();

                                       // Start a new hit
                                       dWeightedTime = dTime*dTotS;
                                       dWeightedPosX = dPosX*dTotS;
                                       dWeightedPosY = dPosY*dTotS;
                                       dWeightedPosZ = dPosZ*dTotS;
                                       dWeightsSum   = dTotS;
                                       iNbChanInHit  = 1;
                                       // Save pointer on CbmTofPoint
				       //                                       vPtsRef.push_back( (CbmTofPoint*)(xDigiA->GetLinks()) );

                                       if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
                                       {
					 //                                          if( ((CbmTofPoint*)(xDigiA->GetLinks()))->GetTrackID() !=
                                         //       ((CbmTofPoint*)(xDigiB->GetLinks()))->GetTrackID() )
                                             // if other side come from a different Track,
                                             // also save the pointer on CbmTofPoint
                                         //    vPtsRef.push_back( (CbmTofPoint*)(xDigiB->GetLinks()) );
                                       } // if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
                                         // else if( xDigiA->GetLinks() != xDigiB->GetLinks() )
                                             // if other side come from a different TofPoint,
                                             // also save the pointer on CbmTofPoint
				       //    vPtsRef.push_back( (CbmTofPoint*)(xDigiB->GetLinks()) );
                                    } // else of if current Digis compatible with last fired chan
                                 } // if( 0 < iNbChanInHit)
                                    else
                                    {
                                       // first fired strip in this RPC
                                       dWeightedTime = dTime*dTotS;
                                       dWeightedPosX = dPosX*dTotS;
                                       dWeightedPosY = dPosY*dTotS;
                                       dWeightedPosZ = dPosZ*dTotS;
                                       dWeightsSum   = dTotS;
                                       iNbChanInHit  = 1;
                                       // Save pointer on CbmTofPoint
				       //                                       vPtsRef.push_back( (CbmTofPoint*)(xDigiA->GetLinks()) );

                                       if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
                                       {
					 //                                          if( ((CbmTofPoint*)(xDigiA->GetLinks()))->GetTrackID() !=
                                         //       ((CbmTofPoint*)(xDigiB->GetLinks()))->GetTrackID() )
                                             // if other side come from a different Track,
                                             // also save the pointer on CbmTofPoint
					 //                                             vPtsRef.push_back( (CbmTofPoint*)(xDigiB->GetLinks()) );
                                       } // if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
                                         // else if( xDigiA->GetLinks() != xDigiB->GetLinks() )
                                             // if other side come from a different TofPoint,
                                             // also save the pointer on CbmTofPoint
				       //    vPtsRef.push_back( (CbmTofPoint*)(xDigiB->GetLinks()) );
                                    } // else of if( 0 < iNbChanInHit)
                                 iLastChan = iCh;
                                 dLastPosX = dPosX;
                                 dLastPosY = dPosY;
                                 dLastTime = dTime;
                              } // if( 2 <= fStorDigi[iSmType][iSm][iRpc][iCh].size() )
                              fStorDigi[iSmType][iSm*iNbRpc+iRpc][iCh].clear();
                              fStorDigiInd[iSmType][iSm*iNbRpc+iRpc][iCh].clear();
                           } // for( Int_t iCh = 0; iCh < iNbCh; iCh++ )
                        } // else of if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
                  } // if( 0 == iChType)
                     else
                     {
                        LOG(ERROR)<<"CbmTofTestBeamClusterizer::BuildClusters => Cluster building "
                              <<"from digis to hits not implemented for pads, Sm type "
                              <<iSmType<<" Rpc "<<iRpc<<FairLogger::endl;
                        return kFALSE;
                     } // else of if( 0 == iChType)

                  // Now check if a hit/cluster is started
                  // and save it if it's the case
                  if( 0 < iNbChanInHit)
                  {
                     // Check orientation to properly assign errors
                     if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
                     {
                        // Save Hit
                        dWeightedTime /= dWeightsSum;
                        dWeightedPosX /= dWeightsSum;
                        dWeightedPosY /= dWeightsSum;
                        dWeightedPosZ /= dWeightsSum;
                        TVector3 hitPos(dWeightedPosX, dWeightedPosY, dWeightedPosZ);
                        // Simple errors, not properly done at all for now
                        // Right way of doing it should take into account the weight distribution
                        // and real system time resolution
                        TVector3 hitPosErr( fDigiBdfPar->GetFeeTimeRes() * fDigiBdfPar->GetSignalSpeed(), // Use the electronics resolution
                                            fChannelInfo->GetSizey()/TMath::Sqrt(12.0),   // Single strips approximation
                                            fDigiBdfPar->GetNbGaps( iSmType, iRpc)*
                                            fDigiBdfPar->GetGapSize( iSmType, iRpc)/10.0 / // Change gap size in cm
                                            TMath::Sqrt(12.0) ); // Use full RPC thickness as "Channel" Z size
                        Int_t iDetId = vPtsRef[0]->GetDetectorID();// detID = pt->GetDetectorID() <= from TofPoint
//                        Int_t iDetId = 0;
                        Int_t iRefId = 0; // Index of the correspondng TofPoint
                        if(NULL != fTofPointsColl) iRefId = fTofPointsColl->IndexOf( vPtsRef[0] );

                        new((*fTofHitsColl)[fiNbHits]) CbmTofHit( iDetId,
                                            hitPos, hitPosErr,
                                            iRefId,
                                            dWeightedTime,
					    vPtsRef.size(),
					  0);
			//		    vDigiIndRef); // flag  = number of TofPoints generating the cluster
                        // Using the SetLinks/GetLinks of the TofHit class seems to conflict
                        // with something in littrack QA
//                        ((CbmTofHit*)fTofHitsColl->At(fiNbHits))->SetLinks(vPtsRef[0]);
                        fiNbHits++;
                        // For Histogramming
                        fviClusterSize[iSmType][iRpc].push_back(iNbChanInHit);
                        fviTrkMul[iSmType][iRpc].push_back( vPtsRef.size() );
                        fvdX[iSmType][iRpc].push_back(dWeightedPosX);
                        fvdY[iSmType][iRpc].push_back(dWeightedPosY);
                        fvdDifX[iSmType][iRpc].push_back( vPtsRef[0]->GetX() - dWeightedPosX);
                        fvdDifY[iSmType][iRpc].push_back( vPtsRef[0]->GetY() - dWeightedPosY);
                        fvdDifCh[iSmType][iRpc].push_back( fGeoHandler->GetCell( vPtsRef[0]->GetDetectorID() ) -1 -iLastChan );
                        vPtsRef.clear();
                     } // if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
                     else
                     {
                        // Save Hit
                        dWeightedTime /= dWeightsSum;
                        dWeightedPosX /= dWeightsSum;
                        dWeightedPosY /= dWeightsSum;
                        dWeightedPosZ /= dWeightsSum;
                        TVector3 hitPos(dWeightedPosX, dWeightedPosY, dWeightedPosZ);
                        // Simple errors, not properly done at all for now
                        // Right way of doing it should take into account the weight distribution
                        // and real system time resolution
                        TVector3 hitPosErr( fChannelInfo->GetSizex()/TMath::Sqrt(12.0),   // Single strips approximation
                                            fDigiBdfPar->GetFeeTimeRes() * fDigiBdfPar->GetSignalSpeed(), // Use the electronics resolution
                                            fDigiBdfPar->GetNbGaps( iSmType, iRpc)*
                                            fDigiBdfPar->GetGapSize( iSmType, iRpc)/10.0 / // Change gap size in cm
                                            TMath::Sqrt(12.0) ); // Use full RPC thickness as "Channel" Z size
                        Int_t iDetId = vPtsRef[0]->GetDetectorID();// detID = pt->GetDetectorID() <= from TofPoint
//                        Int_t iDetId = 0;
                        Int_t iRefId = 0; // Index of the correspondng TofPoint
                        if(NULL != fTofPointsColl) iRefId = fTofPointsColl->IndexOf( vPtsRef[0] );

                        new((*fTofHitsColl)[fiNbHits]) CbmTofHit( iDetId,
                                            hitPos, hitPosErr,
                                            iRefId,
                                            dWeightedTime,
					    vPtsRef.size(),
								  0);
			//	            vDigiIndRef); // flag  = number of TofPoints generating the cluster
                        // Using the SetLinks/GetLinks of the TofHit class seems to conflict
                        // with something in littrack QA
//                        ((CbmTofHit*)fTofHitsColl->At(fiNbHits))->SetLinks(vPtsRef[0]);
                        fiNbHits++;
                        // For Histogramming
                        fviClusterSize[iSmType][iRpc].push_back(iNbChanInHit);
                        fviTrkMul[iSmType][iRpc].push_back( vPtsRef.size() );
                        fvdX[iSmType][iRpc].push_back(dWeightedPosX);
                        fvdY[iSmType][iRpc].push_back(dWeightedPosY);
                        fvdDifX[iSmType][iRpc].push_back( vPtsRef[0]->GetX() - dWeightedPosX);
                        fvdDifY[iSmType][iRpc].push_back( vPtsRef[0]->GetY() - dWeightedPosY);
                        fvdDifCh[iSmType][iRpc].push_back( fGeoHandler->GetCell( vPtsRef[0]->GetDetectorID() ) -1 -iLastChan );
                        vPtsRef.clear();
                     } // else of if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
                  } // if( 0 < iNbChanInHit)
               } // for each sm/rpc pair
         } // for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
      } // else of if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
   LOG(DEBUG1)<<"CbmTofTestBeamClusterizer::BuildClusters finished with "
              << fTofHitsColl->GetEntries() << " Hits  "
	      <<FairLogger::endl;
   if (dTRef !=0. ) {
     dTRef /= (Double_t)fTRefHits;
     LOG(DEBUG)<<"CbmTofTestBeamClusterizer::BuildClusters got TRef = " << dTRef 
	      <<" from " << fTRefHits << " Hits "
	      <<FairLogger::endl;
	     
   }
   return kTRUE;
}
