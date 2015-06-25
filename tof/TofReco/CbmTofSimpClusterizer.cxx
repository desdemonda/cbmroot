/** @file CbmTofSimpClusterizer.cxx
 ** @author Pierre-Alain Loizeau <loizeau@physi.uni-heidelberg.de>
 ** @date 23.08.2013
 **/

#include "CbmTofSimpClusterizer.h"

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

// CBMroot classes and includes
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
#include "TProfile.h"
#include "TDirectory.h"
#include "TROOT.h"
#include "TGeoManager.h"


const Int_t DetMask = 4194303;
const Int_t nbClWalkBinX=20;
const Int_t nbClWalkBinY=41;  // choose odd number to have central bin symmetric around 0
const Double_t WalkNHmin=100; // minimal number of hits in bin for walk correction 
Double_t TOTMax=5.E4; 
Double_t TOTMin=2.E4; 
const Double_t TTotMean=2.E4; 

const Int_t nbClDelTofBinX=50;
const Int_t nbClDelTofBinY=49;
const Double_t DelTofMax=5000.;

const Int_t nbCldXdYBinX=49;
const Int_t nbCldXdYBinY=49;
const Double_t dXdYMax=10.;

const Int_t iNTrg=1;

const Double_t Zref = 200.;   // distance of projection plane to target
// C++ Classes and includes

/************************************************************************************/
CbmTofSimpClusterizer::CbmTofSimpClusterizer():
   FairTask("CbmTofSimpClusterizer"),
   fGeoHandler(new CbmTofGeoHandler()),
   fTofId(NULL),
   fDigiPar(NULL),
   fChannelInfo(NULL),
   fDigiBdfPar(NULL),
   fTofPointsColl(NULL),
   fMcTracksColl(NULL),
   fTofDigisColl(NULL),
   fbWriteHitsInOut(kTRUE),
   fTofHitsColl(NULL),
   fTofDigiMatchColl(NULL),
   fiNbHits(0),
   fVerbose(1),
   fStorDigi(),
   fStorDigiExp(),
   fStorDigiInd(),
   fviClusterMul(),
   fviClusterSize(),
   fviTrkMul(),
   fvdX(),
   fvdY(),
   fvdDifX(),
   fvdDifY(),
   fvdDifCh(),
   fhClustBuildTime(NULL),
   fhHitsPerTracks(NULL),
   fhPtsPerHit(NULL),
   fhTimeResSingHits(NULL),
   fhTimeResSingHitsB(NULL),
   fhTimePtVsHits(NULL),
   fhClusterSize(NULL),
   fhClusterSizeType(NULL),
   fhTrackMul(NULL),
   fhClusterSizeMulti(NULL),
   fhTrk1MulPos(NULL),
   fhHiTrkMulPos(NULL),
   fhAllTrkMulPos(NULL),
   fhMultiTrkProbPos(NULL),
   fhDigSpacDifClust(NULL),
   fhDigTimeDifClust(NULL),
   fhDigDistClust(NULL),
   fhClustSizeDifX(NULL),
   fhClustSizeDifY(NULL),
   fhChDifDifX(NULL),
   fhChDifDifY(NULL),
   fhRpcDigiCor(),     
   fhRpcCluMul(),      
   fhRpcSigPropSpeed(),
   fhRpcCluPosition(), 
   fhRpcCluTOff(),     
   fhRpcCluTrms(),     
   fhRpcCluTot(),      
   fhRpcCluSize(),     
   fhRpcCluAvWalk(),   
   fhRpcCluWalk(),
   fhTRpcCluMul(),     
   fhTRpcCluPosition(),
   fhTRpcCluTOff(),    
   fhTRpcCluTot(),     
   fhTRpcCluSize(),    
   fhTRpcCluAvWalk(),  
   fhTRpcCluDelTof(),  
   fhTRpcCludXdY(),    
   fhTRpcCluWalk(),
   fhTrgdT(),
   fvCPSigPropSpeed(),
   fvCPDelTof(),
   fvCPTOff(),  
   fvCPTotGain(),
   fvCPWalk(),
   fiNbSameSide(0),
   fhNbSameSide(NULL),
   fhNbDigiPerChan(NULL),
   fStart(),
   fStop(),
   dTRef(0.),
   fdTRefMax(0.),
   fCalMode(0),
   fCalTrg(0),
   fCalSmType(0),
   fdCaldXdYMax(0.),
   fTRefMode(0),
   fTRefHits(0),
   fPosYMaxScal(0.),
   fTRefDifMax(0.),
   fTotMax(0.),
   fTotMin(0.),
   fCalParFileName(""),
   fCalParFile(NULL)
{
}

CbmTofSimpClusterizer::CbmTofSimpClusterizer(const char *name, Int_t verbose, Bool_t writeDataInOut):
   FairTask(TString(name),verbose),
   fGeoHandler(new CbmTofGeoHandler()),
   fTofId(NULL),
   fDigiPar(NULL),
   fChannelInfo(NULL),
   fDigiBdfPar(NULL),
   fTofPointsColl(NULL),
   fMcTracksColl(NULL),
   fTofDigisColl(NULL),
   fbWriteHitsInOut(writeDataInOut),
   fTofHitsColl(NULL),
   fTofDigiMatchColl(NULL),
   fiNbHits(0),
   fVerbose(verbose),
   fStorDigi(),
   fStorDigiExp(),
   fStorDigiInd(),
   fviClusterMul(),
   fviClusterSize(),
   fviTrkMul(),
   fvdX(),
   fvdY(),
   fvdDifX(),
   fvdDifY(),
   fvdDifCh(),
   fhClustBuildTime(NULL),
   fhHitsPerTracks(NULL),
   fhPtsPerHit(NULL),
   fhTimeResSingHits(NULL),
   fhTimeResSingHitsB(NULL),
   fhTimePtVsHits(NULL),
   fhClusterSize(NULL),
   fhClusterSizeType(NULL),
   fhTrackMul(NULL),
   fhClusterSizeMulti(NULL),
   fhTrk1MulPos(NULL),
   fhHiTrkMulPos(NULL),
   fhAllTrkMulPos(NULL),
   fhMultiTrkProbPos(NULL),
   fhDigSpacDifClust(NULL),
   fhDigTimeDifClust(NULL),
   fhDigDistClust(NULL),
   fhClustSizeDifX(NULL),
   fhClustSizeDifY(NULL),
   fhChDifDifX(NULL),
   fhChDifDifY(NULL),
   fhRpcDigiCor(),     
   fhRpcCluMul(),      
   fhRpcSigPropSpeed(),
   fhRpcCluPosition(), 
   fhRpcCluTOff(),     
   fhRpcCluTrms(),     
   fhRpcCluTot(),      
   fhRpcCluSize(),     
   fhRpcCluAvWalk(),   
   fhRpcCluWalk(),
   fhTRpcCluMul(),     
   fhTRpcCluPosition(),
   fhTRpcCluTOff(),    
   fhTRpcCluTot(),     
   fhTRpcCluSize(),    
   fhTRpcCluAvWalk(),  
   fhTRpcCluDelTof(),  
   fhTRpcCludXdY(),    
   fhTRpcCluWalk(),
   fhTrgdT(),
   fvCPSigPropSpeed(),
   fvCPDelTof(),
   fvCPTOff(),  
   fvCPTotGain(),
   fvCPWalk(),
   fiNbSameSide(0),
   fhNbSameSide(NULL),
   fhNbDigiPerChan(NULL),
   fStart(),
   fStop(),
   dTRef(0.),
   fdTRefMax(0.),
   fCalMode(0),
   fCalTrg(0),
   fCalSmType(0),
   fdCaldXdYMax(0.),
   fTRefMode(0),
   fTRefHits(0),
   fPosYMaxScal(0.),
   fTRefDifMax(0.),
   fTotMax(0.),
   fTotMin(0.),
   fCalParFileName(""),
   fCalParFile(NULL)

{
}

CbmTofSimpClusterizer::~CbmTofSimpClusterizer()
{
   if( fGeoHandler )
      delete fGeoHandler;
//   DeleteHistos(); // <-- if needed  ?
}

/************************************************************************************/
// FairTasks inherited functions
InitStatus CbmTofSimpClusterizer::Init()
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

void CbmTofSimpClusterizer::SetParContainers()
{
   LOG(INFO)<<" CbmTofSimpClusterizer => Get the digi parameters for tof"<<FairLogger::endl;

   // Get Base Container
   FairRunAna* ana = FairRunAna::Instance();
   FairRuntimeDb* rtdb=ana->GetRuntimeDb();

   fDigiPar = (CbmTofDigiPar*)
              (rtdb->getContainer("CbmTofDigiPar"));

   LOG(INFO)<<"  CbmTofSimpClusterizer::SetParContainers found " 
            << fDigiPar->GetNrOfModules() << " cells " <<FairLogger::endl;
   fDigiBdfPar = (CbmTofDigiBdfPar*)
              (rtdb->getContainer("CbmTofDigiBdfPar"));
}

void CbmTofSimpClusterizer::Exec(Option_t * option)
{
   fTofHitsColl->Clear("C");

   fiNbHits = 0;

   LOG(DEBUG)<<" CbmTofSimpClusterizer => New event with "
	     <<fTofDigisColl->GetEntries()<<" digis "<<FairLogger::endl;
   fStart.Set();

   BuildClusters();

   fStop.Set();

   FillHistos();
}

void CbmTofSimpClusterizer::Finish()
{
   WriteHistos();
   // Prevent them from being sucked in by the CbmHadronAnalysis WriteHistograms method
   DeleteHistos();
}

/************************************************************************************/
// Functions common for all clusters approximations
Bool_t   CbmTofSimpClusterizer::RegisterInputs()
{
   FairRootManager *fManager = FairRootManager::Instance();

   fTofPointsColl  = (TClonesArray *) fManager->GetObject("TofPoint");
   if( NULL == fTofPointsColl)
   {
      LOG(ERROR)<<"CbmTofSimpClusterizer::RegisterInputs => Could not get the TofPoint TClonesArray!!!"<<FairLogger::endl;
      return kFALSE;
   } // if( NULL == fTofPointsColl)

   fMcTracksColl   = (TClonesArray *) fManager->GetObject("MCTrack");
   if( NULL == fMcTracksColl)
   {
      LOG(ERROR)<<"CbmTofSimpClusterizer::RegisterInputs => Could not get the MCTrack TClonesArray!!!"<<FairLogger::endl;
      return kFALSE;
   } // if( NULL == fMcTracksColl)

   fTofDigisColl   = (TClonesArray *) fManager->GetObject("TofDigi");
   if( NULL == fTofDigisColl)
   {
      LOG(ERROR)<<"CbmTofSimpClusterizer::RegisterInputs => Could not get the TofDigi TClonesArray!!!"<<FairLogger::endl;
      return kFALSE;
   } // if( NULL == fTofDigisColl)

   return kTRUE;
}
Bool_t   CbmTofSimpClusterizer::RegisterOutputs()
{
   FairRootManager* rootMgr = FairRootManager::Instance();

   fTofHitsColl = new TClonesArray("CbmTofHit");

   // Flag check to control whether digis are written in output root file
   rootMgr->Register( "TofHit","Tof", fTofHitsColl, fbWriteHitsInOut);

   fTofDigiMatchColl = new TClonesArray("CbmMatch",100);
   rootMgr->Register( "TofDigiMatch","Tof", fTofDigiMatchColl, fbWriteHitsInOut);

   return kTRUE;

}
Bool_t   CbmTofSimpClusterizer::InitParameters()
{

   // Initialize the TOF GeoHandler
   Bool_t isSimulation=kFALSE;
   Int_t iGeoVersion = fGeoHandler->Init(isSimulation);
   LOG(INFO)<<"CbmTofSimpClusterizer::InitParameters with GeoVersion "<<iGeoVersion<<FairLogger::endl;

   if( k12b > iGeoVersion )
   {
      LOG(ERROR)<<"CbmTofSimpClusterizer::InitParameters => Only compatible with geometries after v12b !!!"
                <<FairLogger::endl;
      return kFALSE;
   }
   
   if(NULL != fTofId) 
     LOG(INFO)<<"CbmTofSimpClusterizer::InitParameters with GeoVersion "<<fGeoHandler->GetGeoVersion()<<FairLogger::endl;
   else{
     switch(iGeoVersion){
     case k12b: 
       fTofId = new CbmTofDetectorId_v12b();
       break;
     case k14a:
       fTofId = new CbmTofDetectorId_v14a();
       break;
     default:
       LOG(ERROR)<<"CbmTofSimpClusterizer::InitParameters => Invalid geometry!!!"<<iGeoVersion
		 <<FairLogger::endl;
      return kFALSE;
     }
   }
   return kTRUE;
}
/************************************************************************************/
Bool_t   CbmTofSimpClusterizer::InitCalibParameter()
{
  // dimension and initialize calib parameter
  // 
  Int_t iNbSmTypes = fDigiBdfPar->GetNbSmTypes();

  fvCPSigPropSpeed.resize( iNbSmTypes );
  for (Int_t iT=0; iT<iNbSmTypes; iT++) fvCPSigPropSpeed[iT]=fDigiBdfPar->GetSignalSpeed();

  fvCPTOff.resize( iNbSmTypes );
  fvCPTotGain.resize( iNbSmTypes );
  fvCPWalk.resize( iNbSmTypes );
  fvCPDelTof.resize( iNbSmTypes );
  for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
  {
      Int_t iNbSm  = fDigiBdfPar->GetNbSm(  iSmType);
      Int_t iNbRpc = fDigiBdfPar->GetNbRpc( iSmType);
      fvCPTOff[iSmType].resize( iNbSm*iNbRpc );
      fvCPTotGain[iSmType].resize( iNbSm*iNbRpc );
      fvCPWalk[iSmType].resize( iNbSm*iNbRpc );
      fvCPDelTof[iSmType].resize( iNbSm*iNbRpc );
      for( Int_t iSm = 0; iSm < iNbSm; iSm++ )
      {
        for( Int_t iRpc = 0; iRpc < iNbRpc; iRpc++ )
        {
	  //	  LOG(INFO)<<Form(" fvCPDelTof resize for SmT %d, R %d, B %d ",iSmType,iNbSm*iNbRpc,nbClDelTofBinX)
	  //	   <<FairLogger::endl;
	  fvCPDelTof[iSmType][iSm*iNbRpc+iRpc].resize( nbClDelTofBinX );
	  for(Int_t iBx=0; iBx<nbClDelTofBinX; iBx++){ 
	    // LOG(INFO)<<Form(" fvCPDelTof for SmT %d, R %d, B %d",iSmType,iSm*iNbRpc+iRpc,iBx)<<FairLogger::endl;
  	    fvCPDelTof[iSmType][iSm*iNbRpc+iRpc][iBx].resize( iNTrg );
            for(Int_t iTrg=0; iTrg<iNTrg; iTrg++)
      	    fvCPDelTof[iSmType][iSm*iNbRpc+iRpc][iBx][iTrg]=0.;  // initialize
	  }

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
	      fvCPTOff[iSmType][iSm*iNbRpc+iRpc][iCh][iSide]=0.;      //initialize
	      fvCPTotGain[iSmType][iSm*iNbRpc+iRpc][iCh][iSide]=1.;   //initialize
              fvCPWalk[iSmType][iSm*iNbRpc+iRpc][iCh][iSide].resize( nbClWalkBinX );
	      for(Int_t iWx=0; iWx<nbClWalkBinX; iWx++){
		fvCPWalk[iSmType][iSm*iNbRpc+iRpc][iCh][iSide][iWx]=0.;
	      }
	    }    
	  }
	}
      }
  }

  LOG(INFO)<<"CbmTofSimpClusterizer::InitCalibParameter: defaults set"
	   <<FairLogger::endl;

  TDirectory * oldir = gDirectory; // <= To prevent histos from being sucked in by the param file of the TRootManager!
  /*
  gROOT->cd(); // <= To prevent histos from being sucked in by the param file of the TRootManager !
  */

  if(0<fCalMode){
    LOG(INFO) << "CbmTofSimpClusterizer::InitCalibParameter: read histos from "
		 << "file " << fCalParFileName << FairLogger::endl;

  // read parameter from histos
    if(fCalParFileName.IsNull()) return kTRUE;

    fCalParFile = new TFile(fCalParFileName,"");
    if(NULL == fCalParFile) {
      LOG(ERROR) << "CbmTofSimpClusterizer::InitCalibParameter: "
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
	  TH2F *htempPos_pfx =(TH2F*) gDirectory->FindObjectAny( Form("cl_CorSmT%01d_sm%03d_rpc%03d_Pos_pfx",iSmType,iSm,iRpc));
	  TH2F *htempTOff_pfx=(TH2F*) gDirectory->FindObjectAny( Form("cl_CorSmT%01d_sm%03d_rpc%03d_TOff_pfx",iSmType,iSm,iRpc));
	  TH2F *htempTot_pfx =(TH2F*) gDirectory->FindObjectAny( Form("cl_CorSmT%01d_sm%03d_rpc%03d_Tot_pfx",iSmType,iSm,iRpc));
          if(NULL != htempPos_pfx && NULL != htempTOff_pfx && NULL != htempTot_pfx)  
	  {
	    Int_t iNbCh = fDigiBdfPar->GetNbChan( iSmType, iRpc );
            Int_t iNbinTot = htempTot_pfx->GetNbinsX();
	    for( Int_t iCh = 0; iCh < iNbCh; iCh++ )
	      {
		Double_t YMean=((TProfile *)htempPos_pfx)->GetBinContent(iCh+1);  //nh +1 empirical(?)
		Double_t TMean=((TProfile *)htempTOff_pfx)->GetBinContent(iCh+1);
                Double_t dTYOff=YMean/fvCPSigPropSpeed[iSmType] ;
		fvCPTOff[iSmType][iSm*iNbRpc+iRpc][iCh][0] += -dTYOff + TMean ;
		fvCPTOff[iSmType][iSm*iNbRpc+iRpc][iCh][1] += +dTYOff + TMean ;
 
		Double_t TotMean=((TProfile *)htempTot_pfx)->GetBinContent(iCh+1);  //nh +1 empirical(?)
		if(1<TotMean){
		  fvCPTotGain[iSmType][iSm*iNbRpc+iRpc][iCh][0] *= TTotMean / TotMean;
		  fvCPTotGain[iSmType][iSm*iNbRpc+iRpc][iCh][1] *= TTotMean / TotMean;
		}
		LOG(DEBUG1)<<"CbmTofSimpClusterizer::InitCalibParameter:" 
			   <<" SmT "<< iSmType<<" Sm "<<iSm<<" Rpc "<<iRpc<<" Ch "<<iCh
			   <<": YMean "<<YMean<<", TMean "<< TMean
			   <<" -> " << fvCPTOff[iSmType][iSm*iNbRpc+iRpc][iCh][0]
			   <<", "   << fvCPTOff[iSmType][iSm*iNbRpc+iRpc][iCh][1]
			   <<", "   << fvCPTotGain[iSmType][iSm*iNbRpc+iRpc][iCh][0]
			   <<", NbinTot "<< iNbinTot
			   <<FairLogger::endl;

		TH1D *htempWalk0=(TH1D*)gDirectory->FindObjectAny( 
			         Form("Cor_SmT%01d_sm%03d_rpc%03d_Ch%03d_S0_Walk_px", iSmType, iSm, iRpc, iCh ));
		TH1D *htempWalk1=(TH1D*)gDirectory->FindObjectAny( 
			         Form("Cor_SmT%01d_sm%03d_rpc%03d_Ch%03d_S1_Walk_px", iSmType, iSm, iRpc, iCh ));
		if(NULL != htempWalk0 && NULL != htempWalk1 ) { // reinitialize Walk array 
		  LOG(INFO)<<"Initialize Walk correction for "
			    <<Form(" SmT%01d_sm%03d_rpc%03d_Ch%03d",iSmType, iSm, iRpc, iCh)
			    <<FairLogger::endl;
                  if(htempWalk0->GetNbinsX() != nbClWalkBinX) 
		    LOG(ERROR)<<"CbmTofSimpClusterizer::InitCalibParameter: Inconsistent Walk histograms"
			      <<FairLogger::endl;
                   for( Int_t iBin = 0; iBin < nbClWalkBinX; iBin++ )
		   {
		     fvCPWalk[iSmType][iSm*iNbRpc+iRpc][iCh][0][iBin]=htempWalk0->GetBinContent(iBin+1);
		     fvCPWalk[iSmType][iSm*iNbRpc+iRpc][iCh][1][iBin]=htempWalk1->GetBinContent(iBin+1);
		     LOG(DEBUG1)<<Form(" SmT%01d_sm%03d_rpc%03d_Ch%03d bin %d walk %f ",iSmType, iSm, iRpc, iCh, iBin,
				       fvCPWalk[iSmType][iSm*iNbRpc+iRpc][iCh][0][iBin])
			      <<FairLogger::endl;

		   }
		}
	    }
	  }
	  else {
	    LOG(ERROR)<<" Histos " << Form("cl_SmT%01d_sm%03d_rpc%03d_XXX", iSmType, iSm, iRpc) << " not found. "
		      <<FairLogger::endl;
	  }
	  for(Int_t iTrg=0; iTrg<iNTrg; iTrg++){
	   TH1D *htmpDelTof =(TH1D*) gDirectory->FindObjectAny( Form("cl_CorSmT%01d_sm%03d_rpc%03d_Trg%02d_DelTof",iSmType,iSm,iRpc,iTrg));
	   if (NULL==htmpDelTof) {
	    LOG(INFO)<<" Histos " << Form("cl_CorSmT%01d_sm%03d_rpc%03d_Trg%02d_DelTof", iSmType, iSm, iRpc, iTrg) << " not found. "
		      <<FairLogger::endl;
	    continue;
	   }
	   LOG(INFO)<<" Load DelTof from histos "<< Form("cl_CorSmT%01d_sm%03d_rpc%03d_Trg%02d_DelTof",iSmType,iSm,iRpc,iTrg)<<"."
	            <<FairLogger::endl;
           for(Int_t iBx=0; iBx<nbClDelTofBinX; iBx++){
	    fvCPDelTof[iSmType][iSm*iNbRpc+iRpc][iBx][iTrg] += htmpDelTof->GetBinContent(iBx+1);
	   }

	   // copy Histo to memory
	   TDirectory * curdir = gDirectory;
	   gDirectory->cd( oldir->GetPath() );
	   TH1D *h1DelTof=(TH1D *)htmpDelTof->Clone(Form("cl_CorSmT%01d_sm%03d_rpc%03d_Trg%02d_DelTof",iSmType,iSm,iRpc,iTrg));

	   LOG(INFO)<<" copy histo "
	 	    <<h1DelTof->GetName()
	 	    <<" to directory "
		    <<oldir->GetName()
		    <<FairLogger::endl;

	   gDirectory->cd( curdir->GetPath() );
	  }
	}
    }
  }
  //   fCalParFile->Delete();
  gDirectory->cd( oldir->GetPath() ); // <= To prevent histos from being sucked in by the param file of the TRootManager!
  LOG(INFO)<<"CbmTofSimpClusterizer::InitCalibParameter: initialization done"
	   <<FairLogger::endl; 
  return kTRUE;
}
/************************************************************************************/
Bool_t   CbmTofSimpClusterizer::LoadGeometry()
{
   LOG(INFO)<<"CbmTofSimpClusterizer::LoadGeometry starting for  "
            <<fDigiPar->GetNrOfModules() << " geometrically known modules "
            <<FairLogger::endl;

   Int_t iNrOfCells = fDigiPar->GetNrOfModules();
   LOG(INFO)<<"Digi Parameter container contains "<<iNrOfCells<<" cells"
	    <<", interpret with GeoVersion "<<fGeoHandler->GetGeoVersion()<<FairLogger::endl;

   fGeoHandler->CheckGeometryVersion();

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
               LOG(DEBUG)<<"CbmTofSimpClusterizer::LoadGeometry: StoreDigi with "
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
Bool_t   CbmTofSimpClusterizer::DeleteGeometry()
{
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
Bool_t   CbmTofSimpClusterizer::CreateHistos()
{
   TDirectory * oldir = gDirectory; // <= To prevent histos from being sucked in by the param file of the TRootManager!
   gROOT->cd(); // <= To prevent histos from being sucked in by the param file of the TRootManager !
   fhClustBuildTime = new TH1I( "TofSimpClus_ClustBuildTime", "Time needed to build clusters in each event; Time [s]",
                                  4000, 0.0, 4.0 );
   fhHitsPerTracks = new TH1I( "TofSimpClus_TofHitPerTrk", "Mean Number of TofHit per Mc Track; Nb TofHits/Nb MC Tracks []",
         2000, 0.0, 20.0 );
   if( kFALSE == fDigiBdfPar->ClustUseTrackId() )
      fhPtsPerHit = new TH1I( "TofSimpClus_TofPtsPerHit", "Distribution of the Number of MCPoints associated to each TofHit; Nb MCPoint []",
         20, 0.0, 20.0 );
   if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
   {
      fhTimeResSingHits = new TH1I( "TofSimpClus_TofTimeResClust", "Time resolution for TofHits containing Digis from a single MC Track; t(1st Mc Point) -tTofHit [ns]",
            10000, -25.0, 25.0 );
      fhTimeResSingHitsB = new TH2I( "TofSimpClus_TofTimeResClustB", "Time resolution for TofHits containing Digis from a single MC Track; (1st Mc Point) -tTofHit [ns]",
            5000, -25.0, 25.0,
            6, 0, 6);
      fhTimePtVsHits = new TH2I( "TofSimpClus_TofTimePtVsHit", "Time resolution for TofHits containing Digis from a single MC Track; t(1st Mc Point) -tTofHit [ps]",
            2000, 0.0, 50000.0,
            2000, 0.0, 50000.0 );
   }
      else
      {
         fhTimeResSingHits = new TH1I( "TofSimpClus_TofTimeResClust", "Time resolution for TofHits containing Digis from a single TofPoint; tMcPoint -tTofHit [ns]",
               10000, -25.0, 25.0 );
         fhTimeResSingHitsB = new TH2I( "TofSimpClus_TofTimeResClustB", "Time resolution for TofHits containing Digis from a single TofPoint; tMcPoint -tTofHit [ns]",
               5000, -25.0, 25.0,
               6, 0, 6);
         fhTimePtVsHits = new TH2I( "TofSimpClus_TofTimePtVsHit", "Time resolution for TofHits containing Digis from a single TofPoint; tMcPoint -tTofHit [ps]",
               2000, 0.0, 50000.0,
               2000, 0.0, 50000.0 );
      } // else of if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
   fhClusterSize = new TH1I( "TofSimpClus_ClusterSize", "Cluster Size distribution; Cluster Size [Strips]",
         100, 0.5, 100.5 );
   fhClusterSizeType = new TH2I( "TofSimpClus_ClusterSizeType", "Cluster Size distribution in each (SM type, Rpc) pair; Cluster Size [Strips]; 10*SM Type + Rpc Index []",
         100, 0.5, 100.5,
         40*fDigiBdfPar->GetNbSmTypes(), 0.0, 40*fDigiBdfPar->GetNbSmTypes() );
   if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
   {
      fhTrackMul = new TH1I( "TofSimpClus_TrackMul", "Number of MC tracks generating the cluster; MC Tracks multiplicity []",
            100, 0.5, 100.5 );
      fhClusterSizeMulti = new TH2I( "TofSimpClus_ClusterSizeMulti",
            "Cluster Size distribution as function of Number of MC tracks generating the cluster; Cluster Size [Strips]; MC tracks mul. []",
            100, 0.5, 100.5,
            100, 0.5, 100.5 );
      fhTrk1MulPos = new TH2D( "TofSimpClus_Trk1MulPos",
            "Position of Clusters with only 1 MC tracks generating the cluster; X [cm]; Y [cm]",
            1500, -750, 750,
            1000, -500, 500 );
      fhHiTrkMulPos = new TH2D( "TofSimpClus_HiTrkMulPos",
            "Position of Clusters with >1 MC tracks generating the cluster; X [cm]; Y [cm]",
            1500, -750, 750,
            1000, -500, 500 );
      fhAllTrkMulPos = new TH2D( "TofSimpClus_AllTrkMulPos",
            "Position of all clusters generating the cluster; X [cm]; Y [cm]",
            1500, -750, 750,
            1000, -500, 500 );
      fhMultiTrkProbPos = new TH2D( "TofSimpClus_MultiTrkProbPos",
            "Probability of having a cluster with multiple tracks as function of position; X [cm]; Y [cm]; Prob. [%]",
            1500, -750, 750,
            1000, -500, 500 );
   } // if( kTRUE == fDigiBdfPar->ClustUseTrackId() )

   fhDigSpacDifClust = new TH1I( "TofSimpClus_DigSpacDifClust",
         "Space difference along channel direction between Digi pairs on adjacent channels; PosCh i - Pos Ch i+1 [cm]",
         5000, -10.0, 10.0 );
   fhDigTimeDifClust = new TH1I( "TofSimpClus_DigTimeDifClust",
         "Time difference between Digi pairs on adjacent channels; 0.5*(tDigiA + tDigiA)chi - 0.5*(tDigiA + tDigiA)chi+1 [ns]",
         5000, -5.0, 5.0 );
   fhDigDistClust = new TH2I( "TofSimpClus_DigDistClust",
         "Distance between Digi pairs on adjacent channels; PosCh i - Pos Ch i+1 [cm along ch]; 0.5*(tDigiA + tDigiA)chi - 0.5*(tDigiA + tDigiA)chi+1 [ns]",
         5000, -10.0, 10.0,
         2000, -5.0, 5.0 );

   fhClustSizeDifX = new TH2I( "TofSimpClus_ClustSizeDifX", "Position difference between Point and Hit as function of Cluster Size; Cluster Size [Strips]; dX [cm]",
         100, 0.5, 100.5,
         500,-50,50);
   fhClustSizeDifY = new TH2I( "TofSimpClus_ClustSizeDifY", "Position difference between Point and Hit as function of Cluster Size; Cluster Size [Strips]; dY [cm]",
         100, 0.5, 100.5,
         500,-50,50);
   fhChDifDifX = new TH2I( "TofSimpClus_ChDifDifX", "Position difference between Point and Hit as function of Channel dif; Ch Dif [Strips]; dX [cm]",
         101, -50.5, 50.5,
         500,-50,50);
   fhChDifDifY = new TH2I( "TofSimpClus_ChDifDifY", "Position difference between Point and Hit as function of Channel Dif; Ch Dif [Strips]; dY [cm]",
         101, -50.5, 50.5,
         500,-50,50);

   fhNbSameSide = new TH1I( "TofSimpClus_NbSameSide",
         "How many time per event the 2 digis on a channel were of the same side ; Counts/Event []",
         500, 0.0, 500.0 );
   fhNbDigiPerChan = new TH1I( "TofSimpClus_NbDigiPerChan",
         "Nb of Digis per channel; Nb Digis []",
         100, 0.0, 100.0 );

   gDirectory->cd( oldir->GetPath() ); // <= To prevent histos from being sucked in by the param file of the TRootManager!

   return kTRUE;
}
Bool_t   CbmTofSimpClusterizer::FillHistos()
{
   fhClustBuildTime->Fill( fStop.GetSec() - fStart.GetSec()
                           + (fStop.GetNanoSec() - fStart.GetNanoSec())/1e9 );
   Int_t iNbTofHits  = fTofHitsColl->GetEntries();
   Int_t iNbTracks   = fMcTracksColl->GetEntries();

   // Trakcs Info
   Int_t iNbTofTracks     = 0;
   Int_t iNbTofTracksPrim = 0;
   CbmMCTrack  *pMcTrk;
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

   if( 0 < iNbTofTracks )
      fhHitsPerTracks->Fill( (Double_t)iNbTofHits/(Double_t)iNbTofTracks );

   CbmTofHit  *pHit;
   for( Int_t iHitInd = 0; iHitInd < iNbTofHits; iHitInd++)
   {
      pHit = (CbmTofHit*) fTofHitsColl->At( iHitInd );
      if( kFALSE == fDigiBdfPar->ClustUseTrackId() )
         fhPtsPerHit->Fill(pHit->GetFlag());
      if( 1 == pHit->GetFlag() )
      {
//         CbmTofPoint* pPt = (CbmTofPoint*)pHit->GetRefId();
         // Using the SetLinks/GetLinks of the TofHit class seems to conflict
         // with something in littrack QA
//         CbmTofPoint* pPt = (CbmTofPoint*)(pHit->GetLinks());
         // Use instead the index
         CbmTofPoint* pPt = (CbmTofPoint*)fTofPointsColl->At( pHit->GetRefId() );
         fhTimePtVsHits->Fill( pPt->GetTime(), pHit->GetTime() );
         fhTimeResSingHits->Fill( pHit->GetTime() - pPt->GetTime() );
         fhTimeResSingHitsB->Fill( pHit->GetTime() - pPt->GetTime(),
               fGeoHandler->GetSMType(pPt->GetDetectorID()) );
      } // if( 1 == pHit->GetFlag() )
   } // for( Int_t iHitInd = 0; iHitInd < iNbTofHits; iHitInd++)

   for( Int_t iSmType = 0; iSmType < fDigiBdfPar->GetNbSmTypes(); iSmType++ )
      for( Int_t iRpc = 0; iRpc < fDigiBdfPar->GetNbRpc( iSmType); iRpc++ )
      {
         for( Int_t iCluster = 0; iCluster < fviClusterSize[iSmType][iRpc].size(); iCluster++ )
         {
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
            if( 1 == fviTrkMul[iSmType][iRpc][iCluster] )
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

   fhNbSameSide->Fill(fiNbSameSide);

   return kTRUE;
}
Bool_t   CbmTofSimpClusterizer::WriteHistos()
{
   TDirectory * oldir = gDirectory;
   TFile *fHist = new TFile("./tofSimpClust.hst.root","RECREATE");

   fHist->cd();
   fhClustBuildTime->Write();
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
   fhDigSpacDifClust->Write();
   fhDigTimeDifClust->Write();
   fhDigDistClust->Write();

   fhClustSizeDifX->Write();
   fhClustSizeDifY->Write();

   fhChDifDifX->Write();
   fhChDifDifY->Write();

   fhNbSameSide->Write();
   fhNbDigiPerChan->Write();

   gDirectory->cd( oldir->GetPath() );

   fHist->Close();

   return kTRUE;
}
Bool_t   CbmTofSimpClusterizer::DeleteHistos()
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
Bool_t   CbmTofSimpClusterizer::BuildClusters()
{
   /*
    * FIXME: maybe use the 2D distance (X/Y) as criteria instead of the distance long channel
    * direction
    */
   Double_t dMaxTimeDist  = fDigiBdfPar->GetMaxTimeDist();
   Double_t dMaxSpaceDist = fDigiBdfPar->GetMaxDistAlongCh();

   LOG(DEBUG)<<" BuildCluster with MaxTimeDist "<<dMaxTimeDist<<", MaxSpaceDist "
	     <<dMaxSpaceDist
	     <<FairLogger::endl;

   // First Time order the Digis array
//   fTofDigisColl->Sort();

   // Then loop over the digis array and store the Digis in separate vectors for
   // each RPC modules
   Int_t iNbTofDigi  = fTofDigisColl->GetEntries();
   if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
   {
      CbmTofDigiExp *pDigi;
      for( Int_t iDigInd = 0; iDigInd < iNbTofDigi; iDigInd++ )
      {
         pDigi = (CbmTofDigiExp*) fTofDigisColl->At( iDigInd );
         LOG(DEBUG1)<<"CbmTofSimpClusterizer::BuildClusters: "
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
         Double_t dTotBinSize = (TOTMax-TOTMin)/ 2. / nbClWalkBinX;
         Int_t iWx = (Int_t)((pDigi->GetTot()-TOTMin/2.)/dTotBinSize);
         if (0>iWx) iWx=0;
         if (iWx>nbClWalkBinX) iWx=nbClWalkBinX-1;	
	 Double_t dDTot = (pDigi->GetTot()-TOTMin/2.)/dTotBinSize-(Double_t)iWx-0.5;
	 Double_t dWT  = fvCPWalk[pDigi->GetType()]
		         [pDigi->GetSm()*fDigiBdfPar->GetNbRpc( pDigi->GetType()) + pDigi->GetRpc()]
		         [pDigi->GetChannel()]
		         [pDigi->GetSide()]
                         [iWx];
         if(dDTot > 0) {    // linear interpolation to next bin
	   dWT += dDTot * (fvCPWalk[pDigi->GetType()]
		          [pDigi->GetSm()*fDigiBdfPar->GetNbRpc( pDigi->GetType()) + pDigi->GetRpc()]
		          [pDigi->GetChannel()]
		          [pDigi->GetSide()]
                          [iWx+1]
	                 -fvCPWalk[pDigi->GetType()]
		          [pDigi->GetSm()*fDigiBdfPar->GetNbRpc( pDigi->GetType()) + pDigi->GetRpc()]
		          [pDigi->GetChannel()]
		          [pDigi->GetSide()]
                          [iWx]);
	 }else  // dDTot < 0,  linear interpolation to next bin
	 {
	   dWT -= dDTot * (fvCPWalk[pDigi->GetType()]
		          [pDigi->GetSm()*fDigiBdfPar->GetNbRpc( pDigi->GetType()) + pDigi->GetRpc()]
		          [pDigi->GetChannel()]
		          [pDigi->GetSide()]
                          [iWx-1]
	                 -fvCPWalk[pDigi->GetType()]
		          [pDigi->GetSm()*fDigiBdfPar->GetNbRpc( pDigi->GetType()) + pDigi->GetRpc()]
		          [pDigi->GetChannel()]
		          [pDigi->GetSide()]
                          [iWx]);
	 }
	     
         pDigi->SetTime(pDigi->GetTime() - dWT); // calibrate Digi Time 

	 if(0) {//pDigi->GetType()==2 && pDigi->GetSm()==0){
	  LOG(INFO)<<"CbmTofSimpClusterizer::BuildClusters: CalDigi "
	            <<iDigInd<<",  T "
		    <<pDigi->GetType()<<", Sm "
		    <<pDigi->GetSm()<<", R "
		    <<pDigi->GetRpc()<<", Ch "
		    <<pDigi->GetChannel()<<", S "
		    <<pDigi->GetSide()<<", T "
		    <<pDigi->GetTime()<<", Tot "
		    <<pDigi->GetTot()
		    <<", TotGain "<<
	               fvCPTotGain[pDigi->GetType()]
		       [pDigi->GetSm()*fDigiBdfPar->GetNbRpc( pDigi->GetType()) + pDigi->GetRpc()]
		       [pDigi->GetChannel()]
		       [pDigi->GetSide()]
	            <<", Walk "<<iWx<<": "<<
		        fvCPWalk[pDigi->GetType()]
		        [pDigi->GetSm()*fDigiBdfPar->GetNbRpc( pDigi->GetType()) + pDigi->GetRpc()]
		        [pDigi->GetChannel()]
		        [pDigi->GetSide()]
                        [iWx]
		   <<FairLogger::endl;

	  LOG(INFO)<<" dDTot "<<dDTot
		   <<" BinSize: "<<dTotBinSize
		   <<", CPWalk "<<fvCPWalk[pDigi->GetType()]
		        [pDigi->GetSm()*fDigiBdfPar->GetNbRpc( pDigi->GetType()) + pDigi->GetRpc()]
		        [pDigi->GetChannel()]
		        [pDigi->GetSide()]
	                [iWx-1]
		  <<", "<<fvCPWalk[pDigi->GetType()]
		        [pDigi->GetSm()*fDigiBdfPar->GetNbRpc( pDigi->GetType()) + pDigi->GetRpc()]
		        [pDigi->GetChannel()]
		        [pDigi->GetSide()]
                        [iWx]
		  <<", "<<fvCPWalk[pDigi->GetType()]
		        [pDigi->GetSm()*fDigiBdfPar->GetNbRpc( pDigi->GetType()) + pDigi->GetRpc()]
		        [pDigi->GetChannel()]
		        [pDigi->GetSide()]
                        [iWx+1]
		  <<" -> dWT = "<< dWT
		  <<FairLogger::endl;
	  }
 	 } else 
	   {
           LOG(DEBUG)<<"CbmTofSimpBeamClusterizer::BuildClusters: Skip Digi "
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
               Int_t iNbCh = fDigiBdfPar->GetNbChan( iSmType, iRpc );
               Int_t iChType = fDigiBdfPar->GetChanType( iSmType, iRpc );
	       LOG(DEBUG2)<<"CbmTofSimpClusterizer::BuildClusters: RPC - Loop  "
			 << Form(" %3d %3d %3d %3d ",iSmType,iSm,iRpc,iChType)
                         <<FairLogger::endl;
               fviClusterMul[iSmType][iSm][iRpc]=0; 
               Int_t  iChId = 0;
               if( 0 == iChType )
               {
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
                  LOG(DEBUG2)<<"CbmTofSimpClusterizer::BuildClusters: ChanOrient "
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
			  LOG(DEBUG2)<<"CbmTofSimpClusterizer::BuildClusters: VDigisize "
			     << Form(" T %3d Sm %3d R %3d Ch %3d Size %3zu ",iSmType,iSm,iRpc,iCh,fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].size())
                             <<FairLogger::endl;

                           if( 0 < fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].size() )
                              fhNbDigiPerChan->Fill( fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].size() );
                           while( 1 < fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].size() )
                           {
			       LOG(DEBUG2) << "CbmTofSimpClusterizer::BuildClusters: digis processing for " 
					  << Form(" SmT %3d Sm %3d Rpc %3d Ch %3d # %3zu ",iSmType,iSm,iRpc,iCh,
						  fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].size())
                               <<FairLogger::endl;
			       /*
			      if (3 < fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].size()) // needs more work!
			       LOG(DEBUG) << "CbmTofSimpClusterizer::BuildClusters: digis ignored for " 
			       << Form(" SmT %3d Sm %3d Rpc %3d Ch %3d # %3d ",iSmType,iSm,iRpc,iCh,fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].size())
                               <<FairLogger::endl;
			       */

                               while( (fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh][0])->GetSide() ==
                                      (fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh][1])->GetSide() )
                               {
                                 // Not one Digi of each end!
                                 fiNbSameSide++;
                                 LOG(DEBUG) << "CbmTofSimpClusterizer::BuildClusters: SameSide Hits! Times: "
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

			      LOG(DEBUG2) << "CbmTofSimpClusterizer::BuildClusters: digis processing for " 
					  << Form(" SmT %3d Sm %3d Rpc %3d Ch %3d # %3zu ",iSmType,iSm,iRpc,iCh,
						  fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].size())
		  	                  <<FairLogger::endl;
                              if(2 > fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].size()) break;
                              Int_t iLastChId = iChId; // Save Last hit channel

                              // 2 Digis = both sides present
			      Int_t iCh1=iCh;
			      if(fGeoHandler->GetGeoVersion() < k14a) iCh1= iCh1+1; //FIXME
			      CbmTofDetectorInfo xDetInfo(kTOF, iSmType, iSm, iRpc, 0, iCh1); 

                              iChId = fTofId->SetDetectorInfo( xDetInfo );
			      Int_t iUCellId=CbmTofAddress::GetUniqueAddress(iSm,iRpc,iCh,0,iSmType);
			      LOG(DEBUG1)<<"CbmTofSimpClusterizer::BuildClusters:" 
					 << Form(" T %3d Sm %3d R %3d Ch %3d size %3zu ",
						 iSmType,iSm,iRpc,iCh,fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].size())
					 << Form(" ChId: 0x%08x 0x%08x ",iChId,iUCellId)
					 <<FairLogger::endl;
                              fChannelInfo = fDigiPar->GetCell( iChId );
                              if(NULL == fChannelInfo){
			      LOG(ERROR)<<"CbmTofSimpClusterizer::BuildClusters: no geometry info! "
					<< Form(" %3d %3d %3d %3d 0x%08x 0x%08x ",iSmType, iSm, iRpc, iCh, iChId,iUCellId)
					<<FairLogger::endl;
			      break;
			      }

                              CbmTofDigiExp * xDigiA = fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh][0];
                              CbmTofDigiExp * xDigiB = fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh][1];


                              // The "Strip" time is the mean time between each end
                              dTime =0.5 * ( xDigiA->GetTime()	+ xDigiB->GetTime() ) ;                             

                              // Weight is the total charge => sum of both ends ToT
                              dTotS = xDigiA->GetTot() + xDigiB->GetTot();

                              // X position is just the center of the channel
			      //dPosX = fChannelInfo->GetX();

                              // Y position from strip ends time difference
                              //dPosY = fChannelInfo->GetY();

                              // For Z always just take the one of the channel itself( in fact its gap one)
                              //dPosZ = fChannelInfo->GetZ();

			      TGeoNode *fNode=        // prepare local->global trafo
				gGeoManager->FindNode(fChannelInfo->GetX(),fChannelInfo->GetY(),fChannelInfo->GetZ());
			      LOG(DEBUG1)<<Form(" Node at (%6.1f,%6.1f,%6.1f) : 0x%p",
					       fChannelInfo->GetX(),fChannelInfo->GetY(),fChannelInfo->GetZ(),fNode)
					<<FairLogger::endl;
			      //        fNode->Print();
			      

			      // switch to local coordinates, (0,0,0) is in the center of counter  ?
			      dPosX=((Double_t)(-iNbCh/2 + iCh)+0.5)*fChannelInfo->GetSizex();
			      dPosY=0.;
			      dPosZ=0.;


                              if( 1 == xDigiA->GetSide() )
			     
                                 // 0 is the top side, 1 is the bottom side
                                 dPosY += fvCPSigPropSpeed[iSmType] *
				        ( xDigiA->GetTime() - xDigiB->GetTime() )/2.0;

                              else
			      
                                 // 0 is the bottom side, 1 is the top side
                                 dPosY += fvCPSigPropSpeed[iSmType] *
				       ( xDigiB->GetTime() - xDigiA->GetTime() )/2.0;



			      LOG(DEBUG1)
				   <<"CbmTofSimpClusterizer::BuildClusters: NbChanInHit  "
				   << Form(" %3d %3d %3d 0x%p %f Time %f PosY %f Svel %f ",
					   iNbChanInHit,iCh,iLastChan,xDigiA,xDigiA->GetSide(),
					   dTime,dPosY,fvCPSigPropSpeed[iSmType])

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

                                    LOG(DEBUG1)<<" Add Digi and erase fStor entries(a): NbChanInHit "<< iNbChanInHit<<", "
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
				    //  TVector3 hitPosLocal(dWeightedPosX, dWeightedPosY, dWeightedPosZ);
				    //TVector3 hitPos;
                                    Double_t hitpos_local[3];
				    hitpos_local[0] = dWeightedPosX;
				    hitpos_local[1] = dWeightedPosY;
				    hitpos_local[2] = dWeightedPosZ;

				    Double_t hitpos[3];
				    TGeoNode*	cNode= gGeoManager->GetCurrentNode();
				    TGeoHMatrix* cMatrix = gGeoManager->GetCurrentMatrix();
				    //cMatrix->Print();

				    gGeoManager->LocalToMaster(hitpos_local, hitpos);
				    LOG(DEBUG1)<<
				    Form(" LocalToMaster for node 0x%p: (%6.1f,%6.1f,%6.1f) ->(%6.1f,%6.1f,%6.1f)", 
					 cNode, hitpos_local[0], hitpos_local[1], hitpos_local[2], 
					 hitpos[0], hitpos[1], hitpos[2])
					     <<FairLogger::endl;

                                    TVector3 hitPos(hitpos[0],hitpos[1],hitpos[2]);

                                    // Simple errors, not properly done at all for now
                                    // Right way of doing it should take into account the weight distribution
                                    // and real system time resolution
                                    TVector3 hitPosErr( fChannelInfo->GetSizex()/TMath::Sqrt(12.0),   // Single strips approximation
                                                        fDigiBdfPar->GetFeeTimeRes() * fvCPSigPropSpeed[iSmType], // Use the electronics resolution
                                                        fDigiBdfPar->GetNbGaps( iSmType, iRpc)*
                                                        fDigiBdfPar->GetGapSize( iSmType, iRpc)/10.0 / // Change gap size in cm
                                                        TMath::Sqrt(12.0) ); // Use full RPC thickness as "Channel" Z size

				    // Int_t iDetId = vPtsRef[0]->GetDetectorID();// detID = pt->GetDetectorID() <= from TofPoint
			            // calc mean ch from dPosX=((Double_t)(-iNbCh/2 + iCh)+0.5)*fChannelInfo->GetSizex();

			            Int_t iChm=floor(dWeightedPosX/fChannelInfo->GetSizex())+iNbCh/2;
                                    Int_t iDetId = CbmTofAddress::GetUniqueAddress(iSm,iRpc,iChm,0,iSmType);
                                    Int_t iRefId = 0; // Index of the correspondng TofPoint
				    // if(NULL != fTofPointsColl) {     iRefId = fTofPointsColl->IndexOf( vPtsRef[0] ); }
				    LOG(DEBUG)<<"CbmTofSimpClusterizer::BuildClusters: Save Hit  "
					       << Form(" %3d %3d 0x%08x %3d %3d %3d %f %f",
						       fiNbHits,iNbChanInHit,iDetId,iCh,iLastChan,iRefId,
						       dWeightedTime,dWeightedPosY)
				               <<", DigiSize: "<<vDigiIndRef.size()
					       <<", DigiInds: ";
                                    fviClusterMul[iSmType][iSm][iRpc]++; 
				    for (Int_t i=0; i<vDigiIndRef.size();i++){
				      LOG(DEBUG)<<" "<<vDigiIndRef.at(i);
				    }
				    LOG(DEBUG)  <<FairLogger::endl;
				    
                                    new((*fTofHitsColl)[fiNbHits]) CbmTofHit( iDetId,
							hitPos, hitPosErr,  //local detector coordinates
                                                        fiNbHits,
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
 		                    LOG(DEBUG1)<<"CbmTofSimpClusterizer::BuildClusters: 1.Hit on channel " 
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
			LOG(DEBUG2)<<"CbmTofSimpClusterizer::BuildClusters: finished V-RPC"
				   << Form(" %3d %3d %3d %d",iSmType,iSm,iRpc,fTofHitsColl->GetEntries())
				   <<FairLogger::endl;
                     } // else of if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
               } // if( 0 == iChType)
                  else
                  {
                     LOG(ERROR)<<"CbmTofSimpClusterizer::BuildClusters => Cluster building "
                           <<"from digis to hits not implemented for pads, Sm type "
                           <<iSmType<<" Rpc "<<iRpc<<FairLogger::endl;
                     return kFALSE;
                  } // else of if( 0 == iChType)

               // Now check if another hit/cluster is started
               // and save it if it's the case
               if( 0 < iNbChanInHit)
               {
		   LOG(DEBUG1)<<"CbmTofSimpClusterizer::BuildClusters: Process cluster " 
			      <<iNbChanInHit<<FairLogger::endl;

                  // Check orientation to properly assign errors
                  if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
                  {
		     LOG(DEBUG1)<<"CbmTofSimpClusterizer::BuildClusters: H-Hit " <<FairLogger::endl;
                  } // if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
                  else
                  {
		     LOG(DEBUG2)<<"CbmTofSimpClusterizer::BuildClusters: V-Hit " <<FairLogger::endl;
                     // Save Hit
                     dWeightedTime /= dWeightsSum;
                     dWeightedPosX /= dWeightsSum;
                     dWeightedPosY /= dWeightsSum;
                     dWeightedPosZ /= dWeightsSum;
		     //TVector3 hitPos(dWeightedPosX, dWeightedPosY, dWeightedPosZ);

                     Double_t hitpos_local[3];
		     hitpos_local[0] = dWeightedPosX;
		     hitpos_local[1] = dWeightedPosY;
		     hitpos_local[2] = dWeightedPosZ;

		     Double_t hitpos[3];
		     TGeoNode*	cNode= gGeoManager->GetCurrentNode();
		     TGeoHMatrix* cMatrix = gGeoManager->GetCurrentMatrix();
		     //cMatrix->Print();

		     gGeoManager->LocalToMaster(hitpos_local, hitpos);
		     LOG(DEBUG2)<<
		     Form(" LocalToMaster for V-node 0x%p: (%6.1f,%6.1f,%6.1f) ->(%6.1f,%6.1f,%6.1f)", 
			 cNode, hitpos_local[0], hitpos_local[1], hitpos_local[2], 
			 hitpos[0], hitpos[1], hitpos[2])
			     <<FairLogger::endl;

                     TVector3 hitPos(hitpos[0],hitpos[1],hitpos[2]);
                     // TestBeam errors, not properly done at all for now
                     // Right way of doing it should take into account the weight distribution
                     // and real system time resolution
                     TVector3 hitPosErr( fChannelInfo->GetSizex()/TMath::Sqrt(12.0),   // Single strips approximation
                                         fDigiBdfPar->GetFeeTimeRes() * fvCPSigPropSpeed[iSmType], // Use the electronics resolution
                                         fDigiBdfPar->GetNbGaps( iSmType, iRpc)*
                                         fDigiBdfPar->GetGapSize( iSmType, iRpc)/10.0 / // Change gap size in cm
                                         TMath::Sqrt(12.0) ); // Use full RPC thickness as "Channel" Z size
//                     cout<<"a "<<vPtsRef.size()<<endl;
//                     cout<<"b "<<vPtsRef[0]<<endl;
//                     cout<<"c "<<vPtsRef[0]->GetDetectorID()<<endl;
//                     Int_t iDetId = vPtsRef[0]->GetDetectorID();// detID = pt->GetDetectorID() <= from TofPoint
//                     Int_t iDetId = iChId;
 	             Int_t iChm=floor(dWeightedPosX/fChannelInfo->GetSizex())+iNbCh/2;
                     Int_t iDetId = CbmTofAddress::GetUniqueAddress(iSm,iRpc,iChm,0,iSmType);
                     Int_t iRefId = 0; // Index of the correspondng TofPoint
		     //                     if(NULL != fTofPointsColl) iRefId = fTofPointsColl->IndexOf( vPtsRef[0] );
		     LOG(DEBUG)<<"CbmTofSimpClusterizer::BuildClusters: Save V-Hit  "
//		     << Form(" %3d %3d 0x%08x %3d %3d %3d 0x%08x",
		     << Form(" %3d %3d %3d %3d %3d ",
			     fiNbHits,iNbChanInHit,iDetId,iLastChan,iRefId) //vPtsRef.size(),vPtsRef[0])
		       //   dWeightedTime,dWeightedPosY)
				<<", DigiSize: "<<vDigiIndRef.size();
		     LOG(DEBUG)<<", DigiInds: ";
                     for (Int_t i=0; i<vDigiIndRef.size();i++){
		       LOG(DEBUG)<<" "<<vDigiIndRef.at(i);
		     }
		     LOG(DEBUG)  <<FairLogger::endl;
                     
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
     LOG(ERROR)<<" Compressed Digis not implemented ... "<<FairLogger::endl;
   }
   return kTRUE;
}
