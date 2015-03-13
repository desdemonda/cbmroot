// -------------------------------------------------------------------------
// -----                CbmAnaFlow.h source file             -----
// -----                Created 22/10/14  by Selim         -----
// -------------------------------------------------------------------------     

// CHECK in the code:
// - (IMPORTANT) NOTE
// - TO DO (suggestions for next steps)
// - TO CHANGE (maybe different range/unit definitions for particle angle from different generators)

// NOTE: in all error propagation: assume independent errors -> may have to be improved
// TO DO: calculate v1 and v2 in one single simulation (same 2D histos for N, pt, y) to save computing time and save both in same output file

#include <iostream>
#include <fstream>

#include "CbmAnaFlow.h"

#include "TMath.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include "TClonesArray.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TAxis.h"
#include "TRandom.h"
#include "TVector3.h"

#include "FairRootManager.h"
#include "FairRunAna.h"

#include "CbmMCEventHeader.h"
#include "CbmMCTrack.h"
#include "CbmVertex.h"
#include "CbmTrackMatch.h"
#include "CbmStsTrack.h"

#include "CbmKFParticleFinder.h"
#include "CbmKF.h"
/*#include "CbmKFParticle.h"*/
#include "CbmKFVertex.h"
#include "KFParticle.h"
#include "KFParticleSIMD.h"
#include "KFParticleMatch.h"
#include "CbmL1PFFitter.h"
#include "L1Field.h"
#include "KFMCParticle.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>
#include <cmath>
#include <vector>

#include "FairTrackParam.h"
#include "CbmKFTrack.h"
#include "CbmKFVertex.h"

#include "TCanvas.h"
#include "TGraphErrors.h"

#include "URun.h"
#include "UEvent.h"
#include "UParticle.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"

#include "CbmMCEventData.h"
#include "CbmPsdEventData.h"
#include "CbmStsEventData.h"
//#include "CbmAnaParticleData.h"

#include "TSystemDirectory.h"
#include "FairLogger.h"

using namespace std;
using std::vector;
using std::cout;
using std::endl;

const double CbmAnaFlow::minBin_y = -2.; 
const double CbmAnaFlow::maxBin_y = 2.;

const double CbmAnaFlow::minBin_pt = 0.;
const double CbmAnaFlow::maxBin_pt = 3.;



ClassImp(CbmAnaFlow)


    // -----   Default constructor   -------------------------------------------
    CbmAnaFlow::CbmAnaFlow() : FairTask("AnaFlow",1), fHeader(NULL), flistRECOtrack(NULL), flisttrackMATCH(NULL), flistPV(NULL), fRecParticles(NULL), fMCParticles(NULL),
    fMatchParticles(NULL), flistMCtrack(NULL), fTree_gen(NULL), fEvent_gen(NULL), fMcEvent(NULL),fPsdEvent(NULL), fStsEvent(NULL)//, fAnaPart(NULL)//, ftree_pointer(NULL)
{      
    fPi = TMath::Pi();
    fmass_proton = 0.938271998;

    fEn = 0.;
    fBetaCM = 0.;
    fGammaCM = 1.;
    fy_proj_cm = 0.;
    fy_cm = 0.;

    fmode = 1;

    fpart_mc_reco = 1;
    fileName_tree = "";
    fileName_tree_gen = "";
    fileName_gen = "";

    fileName_ascii_out_y = "";
    fileName_ascii_out_pt = "";

    fevt_inc = 0;  // to loop over generated events

    fY = 0.;
    fpt = 0.;
    fmass = 0.;
    fpdg = 0;
    fmotherID = 0;
    fB_MC = 0.;
    fQ = 0;
    fsignal = kTRUE;
    fphi_to_RP = 0.;
    fphi_to_EP_PSD = 0.;
    fphi_to_EP_PSD1 = 0.;
    fphi_to_EP_PSD2 = 0.;
    fphi_to_EP_PSD3 = 0.;
    fphi_to_EP_STS_harmo1 = 0.;
    fphi_to_EP_STS_harmo2 = 0.;
    fmult = 0;

    fdoPionRotation = kFALSE;
    fdoBackYRotation = kFALSE;

    hRap_vs_y_pt = new TH2F("y_2D","<Y> vs y vs pt", NyBin, minBin_y, maxBin_y, NptBin, minBin_pt, maxBin_pt);
    hpt_vs_y_pt = new TH2F("pt_2D","<pt> vs y vs pt", NyBin, minBin_y, maxBin_y, NptBin, minBin_pt, maxBin_pt);
    hvn_vs_y_pt = new TH2F("vn_2D","vn vs y vs pt", NyBin, minBin_y, maxBin_y, NptBin, minBin_pt, maxBin_pt);
    hN_vs_y_pt = new TH2F("N_2D","N vs y vs pt", NyBin, minBin_y, maxBin_y, NptBin, minBin_pt, maxBin_pt);
    hEff_vs_y_pt = new TH2F("eff_2D","Efficiency vs y vs pt", NyBin, minBin_y, maxBin_y, NptBin, minBin_pt, maxBin_pt);

    hRapidity = new TH1F("hrapidity","hrapidity", 600, -3., 3.);
    hpt = new TH1F("hpt","hpt", 100, 0., 10.);
    hphi = new TH1F("hphi","hphi", 360, -fPi, fPi);
    hcosnphi = new TH1F("hcosnphi","hcosnphi", 360, -1., 1.);

    hmult = new TH1F("hmult","hmult", 1000, 0., 1000.);

    fuseTrackatGen = kFALSE;
    fcoef_meth = "cosnphi";
    fdoInt_diffvn = 0;

    fdoPrim = kFALSE;
    fdoEff = kFALSE;

    fileName_EPcorrFactor = "";
    fEP_meth = "RP";
    fdoEPCorr = kFALSE;
    fuseSubevent = kFALSE;

    fsel_factor = "1";

    fsel_perc_min = 0.;
    fsel_perc_max = 0.;
    fsel_Bmin = 0.;
    fsel_Bmax = 0.;
    ssel_Bmin = "";
    ssel_Bmax = "";
    Bcut = "";

    fsel_PTmin = 0.;
    fsel_PTmax = 3.;
    fsel_PTNbin = 3;
    ssel_PTmin = "";
    ssel_PTmax = "";
    PTcut = "";
    fsel_PTcut_min = 0.;
    fsel_PTcut_max = 0.;

    fsel_Ymin = -2.;
    fsel_Ymax = 2.;
    fsel_YNbin = 10;
    ssel_Ymin = "";
    ssel_Ymax = "";
    Ycut = "";
    fsel_Ycut_min = 0.;
    fsel_Ycut_max = 0.;

    fsel_pdg = 0;
    spdg = "";
    PDGcut = "";

    PRIMcut = "";

    fsel_Q = "";
    Qcut = "";

    fsel_signal = "";
    Scut = "";

    fdirname = "";
}

CbmAnaFlow::CbmAnaFlow(const char* name, Int_t verbose, Double_t En) : FairTask(name, verbose), fHeader(NULL), flistRECOtrack(NULL), flisttrackMATCH(NULL), flistPV(NULL), fRecParticles(NULL), fMCParticles(NULL),
fMatchParticles(NULL), flistMCtrack(NULL), fTree_gen(NULL), fEvent_gen(NULL), fMcEvent(NULL),fPsdEvent(NULL), fStsEvent(NULL)//, fAnaPart(NULL)//, ftree_pointer(NULL)
{     
    fPi = TMath::Pi();
    fmass_proton = 0.938271998;

    fEn = En;
    fy_proj_cm = 0.;
    fy_cm = 0.;
    fBetaCM = 0.;
    fGammaCM = 1.;

    fmode = 1;

    fpart_mc_reco = 1;
    fileName_tree = "";
    fileName_tree_gen = "";
    fileName_gen = "";

    fileName_ascii_out_y = "";
    fileName_ascii_out_pt = "";

    fevt_inc = 0;  // to loop over generated events

    fY = 0.;
    fpt = 0.;
    fmass = 0.;
    fpdg = 0;
    fmotherID = 0;
    fB_MC = 0.;
    fQ = 0;
    fsignal = kTRUE;
    fphi_to_RP = 0.;
    fphi_to_EP_PSD = 0.;
    fphi_to_EP_PSD1 = 0.;
    fphi_to_EP_PSD2 = 0.;
    fphi_to_EP_PSD3 = 0.;
    fphi_to_EP_STS_harmo1 = 0.;
    fphi_to_EP_STS_harmo2 = 0.;
    fmult = 0;

    fdoPionRotation = kFALSE;
    fdoBackYRotation = kFALSE;

    hRap_vs_y_pt = new TH2F("y_2D","<Y> vs y vs pt", NyBin, minBin_y, maxBin_y, NptBin, minBin_pt, maxBin_pt);
    hpt_vs_y_pt = new TH2F("pt_2D","<pt> vs y vs pt", NyBin, minBin_y, maxBin_y, NptBin, minBin_pt, maxBin_pt);
    hvn_vs_y_pt = new TH2F("vn_2D","vn vs y vs pt", NyBin, minBin_y, maxBin_y, NptBin, minBin_pt, maxBin_pt);
    hN_vs_y_pt = new TH2F("N_2D","N vs y vs pt", NyBin, minBin_y, maxBin_y, NptBin, minBin_pt, maxBin_pt);
    hEff_vs_y_pt = new TH2F("eff_2D","Efficiency vs y vs pt", NyBin, minBin_y, maxBin_y, NptBin, minBin_pt, maxBin_pt);

    hRapidity = new TH1F("hrapidity","hrapidity", 600, -3., 3.);
    hpt = new TH1F("hpt","hpt", 100, 0., 10.);
    hphi = new TH1F("hphi","hphi", 360, -fPi, fPi);
    hcosnphi = new TH1F("hcosnphi","hcosnphi", 360, -1., 1.);

    hmult = new TH1F("hmult","hmult", 1000, 0., 1000.);

    fuseTrackatGen = kFALSE;                
    fcoef_meth = "cosnphi";
    fdoInt_diffvn = 0;

    fdoPrim = kFALSE;
    fdoEff = kFALSE;

    fileName_EPcorrFactor = "";
    fEP_meth = "RP";
    fdoEPCorr = kFALSE;
    fuseSubevent = kFALSE;

    fsel_factor = "1";

    fsel_perc_min = 0.;
    fsel_perc_max = 0.;
    fsel_Bmin = 0.;
    fsel_Bmax = 0.;
    ssel_Bmin = "";
    ssel_Bmax = "";
    Bcut = "";

    fsel_PTmin = 0.;
    fsel_PTmax = 3.;
    fsel_PTNbin = 3;
    ssel_PTmin = "";
    ssel_PTmax = "";
    PTcut = "";
    fsel_PTcut_min = 0.;
    fsel_PTcut_max = 0.;

    fsel_Ymin = -2.;
    fsel_Ymax = 2.;
    fsel_YNbin = 10;
    ssel_Ymin = "";
    ssel_Ymax = "";
    Ycut = "";
    fsel_Ycut_min = 0.;
    fsel_Ycut_max = 0.;

    fsel_pdg = 0;
    spdg = "";
    PDGcut = "";

    PRIMcut = "";

    fsel_Q = "";
    Qcut = "";

    fsel_signal = "";
    Scut = "";

    fdirname = "";
}

CbmAnaFlow::~CbmAnaFlow()
{
    if ( fHeader ) 
    {
	fHeader->Delete();
	delete fHeader;
    }    
  
    if ( flistRECOtrack ) 
    {
	flistRECOtrack->Delete();
	delete flistRECOtrack;
    }
    
    if ( flisttrackMATCH ) 
    {
	flisttrackMATCH->Delete();
	delete flisttrackMATCH;
    }
    
    if ( flistPV ) 
    {
	flistPV->Delete();
	delete flistPV;
    }
  
    if ( fRecParticles ) 
    {
	fRecParticles->Delete();
	delete fRecParticles;
    }
    
    if ( fMCParticles ) 
    {
	fMCParticles->Delete();
	delete fMCParticles;
    }
    
    if ( fMatchParticles ) 
    {
	fMatchParticles->Delete();
	delete fMatchParticles;
    }
    
    if ( flistMCtrack ) 
    {
	flistMCtrack->Delete();
	delete flistMCtrack;
    }
    
    if ( fTree_gen )
    {
	fTree_gen->Delete();
	delete fTree_gen;
    }
    
    if ( fEvent_gen )
    {
	fEvent_gen->Delete();
	delete fEvent_gen;
    }  
    
    if ( fMcEvent )
    {
	fMcEvent->Delete();
	delete fMcEvent;
    }
    
    if ( fPsdEvent )
    {
	fPsdEvent->Delete();
	delete fPsdEvent;
    }
    
    if ( fStsEvent )
    {
	fStsEvent->Delete();
	delete fStsEvent;
    }

    //if ( fAnaPart )
    //{
    //    fAnaPart->Delete();
    //    delete fAnaPart;
    //}

    //if ( ftree_pointer )
    //{
    //    ftree_pointer->Delete();
    //    delete ftree_pointer;
    //}
}

// -----   Public method Init   --------------------------------------------
InitStatus CbmAnaFlow::Init()
{   
  projRapidityCM();

  //====================== 2D histo: vn vs y vs pt
  hRapidity->Reset();
  hpt->Reset();
  hphi->Reset();
  hcosnphi->Reset();

  hRap_vs_y_pt->Reset();
  hpt_vs_y_pt->Reset();
  hvn_vs_y_pt->Reset();
  hN_vs_y_pt->Reset();
  hEff_vs_y_pt->Reset();

  hRap_vs_y_pt->SetDirectory(0);
  hpt_vs_y_pt->SetDirectory(0);
  hvn_vs_y_pt->SetDirectory(0);
  hN_vs_y_pt->SetDirectory(0);
  hEff_vs_y_pt->SetDirectory(0);

  // =========== Get input array

  if (fmode == 0)
  {
    if (fileName_gen == "")
    {
	LOG(FATAL) << "-E- CbmAnaFlow::Init: No generation file!" << FairLogger::endl;
	return kERROR;
    }
    
    cout << "-I- CbmAnaFlow::Init: Opening input file " << fileName_gen << endl;
    TFile *fInputFile = new TFile(fileName_gen);
    if (NULL == fInputFile) 
    {
        LOG(FATAL) << "-E- CbmAnaFlow::Init: Cannot open input file!" << FairLogger::endl;
	return kERROR;	
    }

    // Get run description -> for elab, plab, etc ... // note: C.M. momenta & energy for particles from UrQMD
    URun *run = (URun*) fInputFile->Get("run");
    if(NULL == run) 
    {
	LOG(FATAL) << "-E- CbmAnaFlow::Init: No run description in input file!" << FairLogger::endl;
	return kERROR;	
    }
	
    // momenta & energy in C.M. from event generators
    Double_t elab = (TMath::Power(run->GetSqrtS()/run->GetAProj(),2)- 2*TMath::Power(fmass_proton,2))/(2*fmass_proton);
    Double_t plab = TMath::Sqrt(elab*elab - TMath::Power(fmass_proton,2));
    fBetaCM = plab / (elab + fmass_proton);                                
    fGammaCM = 1./TMath::Sqrt(1. - fBetaCM*fBetaCM);
    
    cout << "-I- CbmAnaFlow::Init: Plab = " << plab << " AGeV, CM boost parameters betaCM = " << fBetaCM << ",  gammaCM = " << fGammaCM << endl;    
    delete run;

    fTree_gen = (TTree*) fInputFile->Get("events");
    if(NULL == fTree_gen) 
    {
        LOG(FATAL) << "-E- CbmAnaFlow::Init: No event tree in generation input file!" << FairLogger::endl;
	return kERROR;	
    }
    fEvent_gen = new UEvent();
    fTree_gen->SetBranchAddress("event", &fEvent_gen);
  }

  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman )
  {
      cout << "-E- CbmAnaFlow::Init: "
	  << "RootManager not instantised!" << endl;
      return kFATAL;
  }

  if (fmode == 0) // init for STS based centrality (based on StsEvent.fmult - charged particle multiplicity)
                  // -> also for mode==0 (gen. events) to have the same centrality calculation as for reco. events
  {
      flistMCtrack = (TClonesArray*) ioman->GetObject("MCTrack");
      if ( ! flistMCtrack ) 
      {
	  LOG(FATAL) << "-E- CbmAnaFlow::Init: No MC Tracks array!" << FairLogger::endl;
	return kERROR;
      }

      flistRECOtrack = (TClonesArray*) ioman->GetObject("StsTrack");
      if ( ! flistRECOtrack ) 
      {
	  LOG(FATAL) << "-E- CbmAnaFlow::Init: No reco. tracks array!" << FairLogger::endl;
	return kERROR;
      }

      flisttrackMATCH = (TClonesArray*) ioman->GetObject("StsTrackMatch");
      if ( ! flisttrackMATCH ) 
      {
	  LOG(FATAL) << "-E- CbmAnaFlow::Init: No track match array!" << FairLogger::endl;
	return kERROR;
      }
  }

  if (fmode == 1) 
  {
      fHeader = (CbmMCEventHeader*) ioman->GetObject("MCEventHeader.");
      
      if ( !fHeader ) 
      {
	LOG(FATAL) << "-E- CbmAnaFlow::Init: No event header!" << FairLogger::endl;
	return kERROR;
      }
    
      flistMCtrack = (TClonesArray*) ioman->GetObject("MCTrack");
      if ( ! flistMCtrack ) 
      {
	  LOG(FATAL) << "-E- CbmAnaFlow::Init: No MC Tracks array!" << FairLogger::endl;
	return kERROR;
      }

      flistRECOtrack = (TClonesArray*) ioman->GetObject("StsTrack");
      if ( ! flistRECOtrack ) 
      {
	  LOG(FATAL) << "-E- CbmAnaFlow::Init: No reco. tracks array!" << FairLogger::endl;
	return kERROR;
      }

      flisttrackMATCH = (TClonesArray*) ioman->GetObject("StsTrackMatch");
      if ( ! flisttrackMATCH ) 
      {
	  LOG(FATAL) << "-E- CbmAnaFlow::Init: No track match array!" << FairLogger::endl;
	return kERROR;
      }

      flistPV = (CbmVertex*) ioman->GetObject("PrimaryVertex");
      if ( ! flistPV ) 
      {
	  LOG(FATAL) << "-E- CbmAnaFlow::Init: No reco. primary vertex array!" << FairLogger::endl;
	return kERROR;
      }

      // =========== Get input EP

      fMcEvent = (CbmMCEventData*) ioman->GetObject("McEvent");
      if ( ! fMcEvent )
      {
	  LOG(FATAL) << "-E- CbmAnaFlow::Init: no McEvent from EP analysis." << FairLogger::endl;
	  return kERROR;
      }

      fPsdEvent = (CbmPsdEventData*) ioman->GetObject("PsdEvent");
      if ( ! fPsdEvent )
      {
	  LOG(FATAL) << "-E- CbmAnaFlow::Init: no PsdEvent from EP analysis." << FairLogger::endl;
	  return kERROR;
      }

      fStsEvent = (CbmStsEventData*) ioman->GetObject("StsEvent");
      if ( ! fStsEvent )
      {
	  LOG(FATAL) << "-E- CbmAnaFlow::Init: no StsEvent from EP analysis." << FairLogger::endl;
	  return kERROR;
      }
  }

  if (fmode == 2)
  {
      fHeader = (CbmMCEventHeader*) ioman->GetObject("MCEventHeader.");
      
      if ( !fHeader ) 
      {
	LOG(FATAL) << "-E- CbmAnaFlow::Init: No event header!" << FairLogger::endl;
	return kERROR;
      } 
    
      fRecParticles = dynamic_cast<TClonesArray*>( ioman->GetObject("RecoParticles") );        // TO DO: add StsTrack & StsTrackMatch
      fMCParticles = dynamic_cast<TClonesArray*>( ioman->GetObject("KFMCParticles") );         //        for STS based centrality (centrality_STSmult())
      fMatchParticles = dynamic_cast<TClonesArray*>( ioman->GetObject("KFParticleMatch") );  
      flistMCtrack = dynamic_cast<TClonesArray*>( ioman->GetObject("MCTrack") );  

      if ( !fRecParticles || !fMCParticles || !fMatchParticles || !flistMCtrack ) 
      {
	LOG(FATAL) << "-E- CbmAnaFlow::Init: problem with arrays!" << FairLogger::endl;
	return kERROR;
      }

      // =========== Get input EP

      fMcEvent = (CbmMCEventData*) ioman->GetObject("McEvent");
      if ( ! fMcEvent )
      {
	  LOG(FATAL) << "-E- CbmAnaFlow::Init: no McEvent from EP analysis." << FairLogger::endl;
	  return kERROR;
      }

      fPsdEvent = (CbmPsdEventData*) ioman->GetObject("PsdEvent");
      if ( ! fPsdEvent )
      {
	  LOG(FATAL) << "-E- CbmAnaFlow::Init: no PsdEvent from EP analysis." << FairLogger::endl;
	  return kERROR;
      }

      fStsEvent = (CbmStsEventData*) ioman->GetObject("StsEvent");
      if ( ! fStsEvent )
      {
	  LOG(FATAL) << "-E- CbmAnaFlow::Init: no StsEvent from EP analysis." << FairLogger::endl;
	  return kERROR;
      }
  }

  // ============ output Tree

  if (fmode == 0 || fmode == 1 || fmode == 2) 
  {
      //fAnaPart = (CbmAnaParticleData*) new CbmAnaParticleData("CbmAnaPart");
      //ioman->Register("AnaParticle", "AnaParticleFolder", fAnaPart, kTRUE);

      outTree = new TTree("cbmana","particles");
      outTree->Branch("fphi_to_RP", &fphi_to_RP, "fphi_to_RP/D");
      outTree->Branch("fphi_to_EP_PSD", &fphi_to_EP_PSD, "fphi_to_EP_PSD/D");
      outTree->Branch("fphi_to_EP_PSD1", &fphi_to_EP_PSD1, "fphi_to_EP_PSD1/D");
      outTree->Branch("fphi_to_EP_PSD2", &fphi_to_EP_PSD2, "fphi_to_EP_PSD2/D");
      outTree->Branch("fphi_to_EP_PSD3", &fphi_to_EP_PSD3, "fphi_to_EP_PSD3/D");
      outTree->Branch("fphi_to_EP_STS_harmo1", &fphi_to_EP_STS_harmo1, "fphi_to_EP_STS_harmo1/D");
      outTree->Branch("fphi_to_EP_STS_harmo2", &fphi_to_EP_STS_harmo2, "fphi_to_EP_STS_harmo2/D");  
      outTree->Branch("fY", &fY, "fY/D");
      outTree->Branch("fpt", &fpt, "fpt/D");  
      outTree->Branch("fpdg", &fpdg, "fpdf/I");
      outTree->Branch("fmass", &fmass, "fmass/D");
      outTree->Branch("fB_MC", &fB_MC, "fB_MC/D");
      outTree->Branch("fmotherID", &fmotherID, "fmotherID/I");
      outTree->Branch("fQ", &fQ, "fQ/I");
      outTree->Branch("fsignal", &fsignal, "fsignal/O");
      outTree->Branch("fmult", &fmult, "fmult/I");

      outTree->SetDirectory(0);
  }

  if (fuseTrackatGen == kTRUE) fileName_tree = fileName_tree_gen;

  if ( (fmode == 3 || fmode == 4) && (fileName_tree == "" || fileName_tree_gen == "") )
  {
      LOG(FATAL) << "-E- CbmAnaFlow::Init: no TTREE input file for histogramming !" << FairLogger::endl;
      return kERROR;
  }

  if ( fmode == 5 && fileName_tree == "" )                                    // ordering + 2D histos creation
  {
      LOG(FATAL) << "-E- CbmAnaFlow::Init: no TTREE input file for 2D histograms creation !" << FairLogger::endl;
      return kERROR;
  }

  if ( fmode == 6 && fdirname == "" )                                         // merging 2D histos
  {
      LOG(FATAL) << "-E- CbmAnaFlow::Init: no directory for Merging 2D histos !" << FairLogger::endl;
      return kERROR;
  }

  if ( ( fmode == 7 ) && (fileName_tree == "" || fileName_tree_gen == "") )   // Final drawing
  {
      LOG(FATAL) << "-E- CbmAnaFlow::Init: no TTREE input file for histogramming !" << FairLogger::endl;
      return kERROR;
  }

  // ============ Input parameters for final drawing

  if ( fmode == 3 || fmode == 4 || fmode == 5 || fmode == 7 )    // mode 6 just for histogram merging (only inputs: dir & file names)
  {
      // Centrality selection
      centrality_cutDef_STSmult();
      ssel_Bmin += fsel_Bmin;
      ssel_Bmax += fsel_Bmax;
      //Bcut = "fB_MC >= " + ssel_Bmin + " && fB_MC < " + ssel_Bmax;
      Bcut = "fmult >= " + ssel_Bmin + " && fmult < " + ssel_Bmax;

      // == PT cut
      PTcut = "fpt >= ";
      PTcut += fsel_PTcut_min;
      PTcut += " && fpt < ";
      PTcut += fsel_PTcut_max;

      // == Rapidity cut
      Ycut = "fY >= ";
      Ycut += fsel_Ycut_min;
      Ycut += " && fY < ";
      Ycut += fsel_Ycut_max;

      if ( fsel_pdg != 0 )
      {
	  spdg += fsel_pdg;
	  PDGcut = " && fpdg == ";
	  PDGcut += spdg;
      }

      if (fdoPrim == kTRUE) PRIMcut = " && fmotherID == -1";

      if (fsel_Q == "positive") Qcut = " && fQ > 0";
      if (fsel_Q == "negative") Qcut = " && fQ < 0";
      if (fsel_Q == "neutral") Qcut = " && fQ == 0";
      if (fsel_Q == "charged") Qcut = " && fQ != 0";

      if (fsel_signal == "signal") Scut = " && fsignal == kTRUE";
      if (fsel_signal == "bg") Scut = " && fsignal == kFALSE";

      cout << "-I- CbmAnaFlow::Init: Bcut: " << Bcut << ", Ycut: " << Ycut << ", PTcut: " << PTcut << ", PDGcut: " << PDGcut << ", Primary cut: " << PRIMcut << ", Qcut: " << Qcut << endl;

      if (fuseTrackatGen == kTRUE) fEP_meth = "RP";
  }

  return kSUCCESS;
}

// -----   Public method Exec   --------------------------------------------

void CbmAnaFlow::Exec(Option_t* opt)
{
    //========== Sequence for simulations:   ( '+' means sequential processing)
    //========== 1) mode = 0 or 1 or 2 (from gen. or reco. or KF events to generic -TTree- data format)
    //========== +
    //========== 2) mode = 3 (simple drawing from TTree) or 4 (2D histo. creation for efficiency corr. + drawing) or 5+6+7 (see below)

    // === STS based centrality
    int mult;
    if (fmode == 0 || fmode == 1 || fmode == 2) mult = centrality_STSmult();
    hmult->Fill(mult);

    //generic TTree to store info. from different sources + conversion: phi_particle(LAB) -> phi_particle(w.r.t. reaction/event plane)
    if (fmode == 0) CreateTreeGen(mult);
    if (fmode == 1) CreateTreeReco(mult);
    if (fmode == 2) CreateTreeKFPart(mult);

    // Draw particle flow from generic TTree: simple i.e. no efficiency correction
    if (fmode == 3)
    {
	if (fdoInt_diffvn == 1 || fdoInt_diffvn == 2) Draw_diffvn_simple();
        if (fdoInt_diffvn == 0 || fdoInt_diffvn == 2) Draw_intvn_simple();
    }

    // Create vn, eff. & N_stat (y, pt) 2d map + Draw particle flow with efficiency correction
    // Slow -> restriction: Nevent<100K and Nbin_y*Nbin_pt < 100's
    // Need particle (y, pt) distribution at gen. level to create efficieny 2d map 
    if (fmode == 4)
    {
	Create2Dhisto_Y_vs_pt();
	if (fdoEff == kTRUE) CreateEff_Y_vs_pt();

	//hvn_vs_y_pt->Draw(); // test
        //hEff_vs_y_pt->Draw();

	if (fdoInt_diffvn == 1 || fdoInt_diffvn == 2) Draw_diffvn_Effcorr();
        if (fdoInt_diffvn == 0 || fdoInt_diffvn == 2) Draw_intvn_Effcorr();
    }

    // Create vn, eff. & N_stat (y, pt) 2d map + Draw particle flow with efficiency correction
    // Fast: use Tree sorting (by increasing pt and y) to loop over it only once when creating 2d maps
    // No restriction
    if (fmode == 5)
    {
	SortTree();
	cout << "SortTree finished" << endl;
	//CheckSortedTree();
        Create2Dhisto_Y_vs_pt_1TreeLoop();
    }

    // mode 5 performed in parallel (even-wise) 
    if (fmode == 6) MergeHistoInFiles();

    // Need particle (y, pt) distribution at gen. level to create efficieny 2d map 
    if (fmode == 7)
    {
	if (fdoEff == kTRUE) CreateEff_Y_vs_pt_1TreeLoop();

        if (fdoInt_diffvn == 1 || fdoInt_diffvn == 2) Draw_diffvn_Effcorr_1TreeLoop();
        if (fdoInt_diffvn == 0 || fdoInt_diffvn == 2) Draw_intvn_Effcorr_1TreeLoop();
    }

}

// ========================================= STS based centrality
int CbmAnaFlow::centrality_STSmult()
{
    CbmStsTrack* track;
    CbmTrackMatch* match;
    CbmMCTrack* mctrack;
    const FairTrackParam *trackParam;
    TVector3 momRec;
    Int_t trackID;
    Double_t p, px, py, pz, mass, energy, y;
    int mult = 0;

    Int_t nSTStracks = flistRECOtrack->GetEntriesFast();
    cout << "CbmAnaFlow::centrality: # STS reco tracks = " << nSTStracks << endl;
    vector<CbmStsTrack> vRTracks;
    vRTracks.resize(nSTStracks);
    for(int iTr=0; iTr<nSTStracks; iTr++) vRTracks[iTr] = *( (CbmStsTrack*) flistRECOtrack->At(iTr));

    for (Int_t itrack=0; itrack<nSTStracks; itrack++)
    {
	track = &vRTracks[itrack];

	match = (CbmTrackMatch*) flisttrackMATCH->At(itrack);
	trackID = match->GetMCTrackId();
	if (trackID < 0) continue;

	mctrack = (CbmMCTrack*) flistMCtrack->At(trackID);
        mass = mctrack->GetMass();

        trackParam = track->GetParamFirst();
	trackParam->Momentum(momRec);

	px = momRec.X();
	py = momRec.Y();
	pz = momRec.Z();
	p = TMath::Sqrt(px*px + py*py + pz*pz);

	energy = TMath::Sqrt(mass*0.93827203*mass*0.93827203 + p*p);
	y = 0.5 * TMath::Log( ( energy + pz ) / ( energy - pz ) );
        y -= fy_cm;        // LAB -> CM
	y /= fy_proj_cm;

        if (y < 0.8) mult++;
    }

    cout << "CbmAnaFlow::centrality: # STS reco tracks with Y<0.8*Y_proj = " << mult << endl;
    return mult;
}

void CbmAnaFlow::centrality_cutDef_STSmult()
{
      TFile *f = new TFile(fileName_EPcorrFactor,"R");
      if (!f) cout << "-E- CbmAnaFlow::centrality_cutDef_STSmult: pb with file containing Tree -----" << endl;

      TTree* t = (TTree* ) f->Get("cbmsim");
      if (!t) cout << "-E- CbmAnaFlow::centrality_cutDef_STSmult: pb with Tree in file -----" << endl;

      cout << "CbmAnaFlow::centrality_cutDef_STSmult: % centrality: [" << fsel_perc_min << ", " << fsel_perc_max << "]" << endl;

      TString maxmult="";
      TString num="_1";
      TH1F* hmult_tmp;
      TAxis *Xaxis;
      int NXbin=0;
      double Nentries=0.;
      double entry=0.;

      t->Draw("StsEvent.fmult>>mult_tmp"+num,"","");
      hmult_tmp = (TH1F*) t->GetHistogram();
      maxmult+=hmult_tmp->GetBinCenter(hmult_tmp->FindLastBinAbove())+hmult_tmp->GetBinWidth(0)/2.;

      num="_2";
      t->Draw("StsEvent.fmult>>mult_tmp"+num+"(1000, 0, "+maxmult+")","","");
      hmult_tmp = (TH1F*) t->GetHistogram();

      Xaxis = hmult_tmp->GetXaxis();
      NXbin = Xaxis->GetLast();
      Nentries = hmult_tmp->Integral();

      entry=0.;
      for(int i=NXbin;i>0;i--)
      {
	  entry += hmult_tmp->GetBinContent(i);
	  if (entry >= ((fsel_perc_min/100.)*Nentries) && fsel_Bmax == 0.) fsel_Bmax = hmult_tmp->GetBinCenter(i);
          if (entry >= ((fsel_perc_max/100.)*Nentries) && fsel_Bmin == 0.) fsel_Bmin = hmult_tmp->GetBinCenter(i);
      }
      cout << "CbmAnaFlow::centrality_cutDef_STSmult: multiplicity cut: [" << fsel_Bmin << ", " << fsel_Bmax << "]" << endl;
}

// ========================================== TTree creation (in option: sorting) ==================================================

bool CbmAnaFlow::CreateTreeGen(int mult)
{
    // === STS based centrality
    fmult = mult;

    cout << "Number of events in generation file: " << fTree_gen->GetEntries() << endl;
    fTree_gen->GetEntry(fevt_inc);
    if(fevt_inc > fTree_gen->GetEntries())
    {
	cout << "-I- CbmAnaFlow::CreateTreeGen: End of input file reached" << endl;
	return kFALSE;
    }

    cout << "-I- CbmAnaFlow::CreateTreeGen: Event " << fEvent_gen->GetEventNr() << ", multiplicity " << fEvent_gen->GetNpa() << endl;
    fB_MC = fEvent_gen->GetB();
    Double_t RP = fEvent_gen->GetPhi();

    UParticle *particle;
    Double_t px, py, pz, pz_tmp, energy, energy_tmp;
    Double_t phi;
    fmotherID = -1;
    TParticlePDG* partPDG;
    FairRootManager* ioman = FairRootManager::Instance();   

    cout << "MC B = " << fB_MC << endl;
    cout << "MC RP = " << RP << ", (must be in radian and in [-pi, pi])" << endl;
    //SELIM: should be =0 in the future (-> random RP in FairPrimaryGenerator)
    // TO CHANGE IF NEEDED
    // RP = RP * fPi/180.; // deg -> rad
    // RP = Range(RP);
    // cout << "MC RP = " << RP << ", (must be in radian and in [-pi, pi])" << endl;

    // === Loop over generated particles
    for (Int_t itrack = 0; itrack < fEvent_gen->GetNpa(); itrack++)
    {
	// Get particle
	particle = fEvent_gen->GetParticle(itrack);
	fpdg = particle->GetPdg();

	if ( TDatabasePDG::Instance() )
	{
	    partPDG = TDatabasePDG::Instance()->GetParticle(fpdg);
	    if ( partPDG ) fQ = partPDG->Charge();
	    else fQ = -100;
	}
	else
	{
	    cout << "CbmAnaFlow::CreateTreeGen: no TDatabasePDG for charge info!" << endl;
	    fQ = -100;
	}

	// momenta & Lorentz boost
	px = particle->Px();
	py = particle->Py();
	fpt = TMath::Sqrt(px*px + py*py);

	pz_tmp = particle->Pz();
	energy_tmp = particle->E();

	pz = fGammaCM*(pz_tmp + fBetaCM*energy_tmp);       // momenta & energy in C.M. from event generators
	energy = fGammaCM*(energy_tmp + fBetaCM*pz_tmp);

	fY = 0.5*TMath::Log( ( energy + pz ) / ( energy - pz ) );
	fY -= fy_cm;        // LAB -> CM
	fY /= fy_proj_cm;

	phi = TMath::ATan2(py, px);

	if ( fdoPionRotation == kTRUE && TMath::Abs(fpdg) == 211 ) phi += fPi;
	phi = Range(phi);

	if ( fdoBackYRotation == kTRUE && fY < 0. ) phi += fPi;
	phi = Range(phi);

	fphi_to_RP = phi - RP;
	fphi_to_RP = Range(fphi_to_RP);

	fsignal = kTRUE;

	outTree->Fill();
    }

    fevt_inc++;
    return kTRUE;
}

bool CbmAnaFlow::CreateTreeReco(int mult)
{
    // === STS based centrality
    fmult = mult;

    Int_t evtID = fHeader->GetEventID();
    Double_t B = fHeader->GetB();
    Double_t RP = fHeader->GetPhi();   // RP = Reaction plane: MC info
    //Double_t RP = fMcEvent->GetRP(); // alternatively from McEventData
    RP = Range(RP);                    // en radian!! from SHIELD: [0, 2pi]
    
    Double_t EP_PSD, EP_PSD1, EP_PSD2, EP_PSD3;
    Double_t EP_STS_harmo1, EP_STS_harmo2;
    EP_PSD = fPsdEvent->GetEP();
    EP_PSD1 = fPsdEvent->GetEP_sub1();
    EP_PSD2 = fPsdEvent->GetEP_sub2();
    EP_PSD3 = fPsdEvent->GetEP_sub3();
    fStsEvent->GetEP(EP_STS_harmo1, EP_STS_harmo2);   // TO DO: change STS EP getter with pointer/reference in cbmdata/sts/CbmStsEventData?
      
    fB_MC = B;

    cout << "----- event number: " << evtID << " ------" << endl;
    cout << "----- MC impact parameter: " << B << " ------" << endl;
    cout << "----- MC RP angle: " << RP << ", (must be in radian and in [-pi, pi])" << " ------" << endl;

    Int_t trackID, type, motherID;
    Double_t p, px, py, pz, pt, phi, mass, energy, y;
    CbmStsTrack* track;
    CbmTrackMatch* match;
    CbmMCTrack* mctrack;

    const FairTrackParam *trackParam;
    TVector3 momRec;
    TVector3 posRec;

    // MC tracks
    Int_t nMCtracks = flistMCtrack->GetEntriesFast();
    cout << "evenPlane::CreateTreeReco: # STS MC tracks = " << nMCtracks << endl;

    // RECO tracks
    Int_t nSTStracks = flistRECOtrack->GetEntriesFast();
    cout << "evenPlane::CreateTreeReco: # STS reco tracks = " << nSTStracks << endl;

    // Extrapolation track parameters back to primary vertex
    vector<CbmStsTrack> vRTracks;
    vRTracks.resize(nSTStracks);
    for(int iTr=0; iTr<nSTStracks; iTr++) vRTracks[iTr] = *( (CbmStsTrack*) flistRECOtrack->At(iTr));

    vector<L1FieldRegion> vField;
    vector<float> ChiToPrimVtx;
    CbmL1PFFitter fitter;
    CbmKFVertex kfPV(*flistPV);
    fitter.GetChiToVertex(vRTracks, vField, ChiToPrimVtx, kfPV, 1000000000);

    // === Loop over reconstructed particle tracks
    for (Int_t itrack=0; itrack<nSTStracks; itrack++)
    {
	track = &vRTracks[itrack];

	match = (CbmTrackMatch*) flisttrackMATCH->At(itrack);    // Need MC info for particle type & mass (-> rapidity caluclation)
	trackID = match->GetMCTrackId();                         // TO DO: use real PID
	                                                         // note: also need motherID to keep primary/secondary MC info.

	if (trackID < 0) continue;                               // ghost track (wo corresponding MC track) due to combinatorix
	                                                         // later (with real PID), analysis must include all reconstructed tracks

	mctrack = (CbmMCTrack*) flistMCtrack->At(trackID);
	motherID = mctrack->GetMotherId();
	type = mctrack->GetPdgCode();
	mass = mctrack->GetMass();

	trackParam = track->GetParamFirst();
	trackParam->Momentum(momRec);
	trackParam->Position(posRec);

	px = momRec.X();
	py = momRec.Y();
	pz = momRec.Z();
	//std::cout << "px, py, pz = " << px << ", " << py << ", " << pz << endl;

	pt = TMath::Sqrt(px*px + py*py);
	p = TMath::Sqrt(px*px + py*py + pz*pz);

	energy = TMath::Sqrt(mass*0.93827203*mass*0.93827203 + p*p);
	y = 0.5 * TMath::Log( ( energy + pz ) / ( energy - pz ) );

	fmotherID = motherID;
	fQ = -100;
	fY = y;
	fpt = pt;
	fmass = mass;
	fpdg = type;

	fY -= fy_cm;        // LAB -> CM
	fY /= fy_proj_cm;

	phi = TMath::ATan2(py, px);
	// cout << "reco phi = " << phi << ", (must be in radian and in [-pi, pi])" << endl;
	// TO CHANGE IF NEEDED
	// phi = phi * fPi/180.;
	// phi = Range(phi);
	// cout << "reco phi = " << phi << ", (must be in radian and in [-pi, pi])" << endl;

	if ( fdoPionRotation == kTRUE && TMath::Abs(type) == 211 ) phi += fPi;
	phi = Range(phi);

	if ( fdoBackYRotation == kTRUE && fY < 0. ) phi += fPi;
	phi = Range(phi);

	fphi_to_RP = phi - RP;

	fphi_to_EP_PSD = phi - EP_PSD;
	fphi_to_EP_PSD1 = phi - EP_PSD1;
	fphi_to_EP_PSD2 = phi - EP_PSD2;
	fphi_to_EP_PSD3 = phi - EP_PSD3;

	fphi_to_EP_STS_harmo1 = phi - EP_STS_harmo1;
	fphi_to_EP_STS_harmo2 = phi - EP_STS_harmo2;

	fphi_to_RP = Range(fphi_to_RP);
	fphi_to_EP_PSD = Range(fphi_to_EP_PSD);
	fphi_to_EP_PSD1 = Range(fphi_to_EP_PSD1);
	fphi_to_EP_PSD2 = Range(fphi_to_EP_PSD2);
	fphi_to_EP_PSD3 = Range(fphi_to_EP_PSD3);
	fphi_to_EP_STS_harmo1 = Range(fphi_to_EP_STS_harmo1);
	fphi_to_EP_STS_harmo2 = Range2nd(fphi_to_EP_STS_harmo2);   // fphi_to_EP_STS_harmo2 must be in [-pi/2,pi/2] (pi-periodic)

	if (motherID == -1) fsignal = kTRUE;   // IMPORTANT NOTE: now, fsignal = kFALSE means secondary tracks
	else fsignal = kFALSE;                 // TO DO: fsignal = kFALSE also if: - trackID < 0 (combinatorix)
	                                       //                                  - real PID /= MC info

	if (fpart_mc_reco == 1) outTree->Fill();

	// ===========  With fpart_mc_reco == 0, one saves corresponding MC tracks (to use ideal momenta)

	motherID = mctrack->GetMotherId();
	y = mctrack->GetRapidity();
	pt = mctrack->GetPt();
	px = mctrack->GetPx();
	py = mctrack->GetPy();

	fmotherID = motherID;
	fQ = mctrack->GetCharge();
	fY = y;
	fpt = pt;
	fmass = mass;
	fpdg = type;

	fY -= fy_cm;        // LAB -> CM
	fY /= fy_proj_cm;

	phi = TMath::ATan2(py, px);

	if ( fdoPionRotation == kTRUE && TMath::Abs(type) == 211 ) phi += fPi;
	phi = Range(phi);

	if ( fdoBackYRotation == kTRUE && fY < 0. ) phi += fPi;
	phi = Range(phi);

	fphi_to_RP = phi - RP;

	fphi_to_EP_PSD = phi - EP_PSD;
	fphi_to_EP_PSD1 = phi - EP_PSD1;
	fphi_to_EP_PSD2 = phi - EP_PSD2;
	fphi_to_EP_PSD3 = phi - EP_PSD3;

	fphi_to_EP_STS_harmo1 = phi - EP_STS_harmo1;
	fphi_to_EP_STS_harmo2 = phi - EP_STS_harmo2;

	fphi_to_RP = Range(fphi_to_RP);
	fphi_to_EP_PSD = Range(fphi_to_EP_PSD);
	fphi_to_EP_PSD1 = Range(fphi_to_EP_PSD1);
	fphi_to_EP_PSD2 = Range(fphi_to_EP_PSD2);
	fphi_to_EP_PSD3 = Range(fphi_to_EP_PSD3);
	fphi_to_EP_STS_harmo1 = Range(fphi_to_EP_STS_harmo1);
	fphi_to_EP_STS_harmo2 = Range2nd(fphi_to_EP_STS_harmo2);  // fphi_to_EP_STS_harmo2 must be in [-pi/2,pi/2] (pi-periodic)

	if (motherID == -1) fsignal = kTRUE;
	else fsignal = kFALSE;

	if (fpart_mc_reco == 0) outTree->Fill();
    }

    return kTRUE;
}

bool CbmAnaFlow::CreateTreeKFPart(int mult)
{
    // === STS based centrality
    fmult = mult;

    Int_t evtID = fHeader->GetEventID();
    Double_t B = fHeader->GetB();
    Double_t RP = fHeader->GetPhi(); // RP = Reaction plane: MC info
    RP = Range(RP);                  // en radian!! from SHIELD: [0, 2pi]

    fB_MC = B;

    Float_t M, ErrM;
    Double_t Pt;
    Double_t Rapidity;
    Double_t phi;
    Double_t px, py;
    Int_t pdg;

    cout << "----- event number: " << evtID << " ------" << endl;
    cout << "----- MC impact parameter: " << B << " ------" << endl;
    cout << "----- MC RP angle: " << RP << ", (must be in radian and in [-pi, pi])" << " ------" << endl;
    // SELIM: Should be in [-pi, pi] from FairPrimaryGenerator! now from UnigenGenerator
    // TO CHANGE IF NEEDED
    //     RP = RP * fPi/180.;
    //     RP = Range(RP);
    //     cout << "----- MC RP angle: " << RP << ", (must be in radian and in [-pi, pi])" << " ------" << endl;

    Double_t EP_PSD, EP_PSD1, EP_PSD2, EP_PSD3;
    Double_t EP_STS_harmo1, EP_STS_harmo2;
    EP_PSD = fPsdEvent->GetEP();
    EP_PSD1 = fPsdEvent->GetEP_sub1();
    EP_PSD2 = fPsdEvent->GetEP_sub2();
    EP_PSD3 = fPsdEvent->GetEP_sub3();
    fStsEvent->GetEP(EP_STS_harmo1, EP_STS_harmo2);

    Int_t nRecoParts = fRecParticles->GetEntriesFast();
    cout << "CbmAnaFlow::CreateTreeKFPart: # reco. part. = " << nRecoParts << endl;

    for (Int_t irec=0; irec<nRecoParts; irec++)
    {
	// === RECO particles

	KFParticle partRec = *( (KFParticle*) fRecParticles->At(irec) );
	KFParticleMatch partMatch = *( (KFParticleMatch*) fMatchParticles->At(irec) );

	int iMCPart = partMatch.GetMatch();
	KFMCParticle mcPart = *( (KFMCParticle*) fMCParticles->At(iMCPart) );

	int iMCTrack = mcPart.GetMCTrackID();
	CbmMCTrack &mcTrack = *(static_cast<CbmMCTrack*>(flistMCtrack->At(iMCTrack)));
	int mcDaughterId = mcPart.GetDaughterIds()[0];
	CbmMCTrack &mcDaughter = *(static_cast<CbmMCTrack*>(flistMCtrack->At(mcDaughterId)));

	if ( !&partRec || !&partMatch || !&mcTrack || !&mcDaughter )
	{
	    cout << "CbmAnaFlow::CreateTreeKFPart: no pointer!" << endl;
	    continue;
	}

	if (!mcPart.IsReconstructable()) continue;                   // reconstructable == all daughters in acceptance

	//cout << "Get PDG: " << partRec.GetPDG() << endl;
	//partRec.GetMass(M,ErrM);
	//cout << "Get mass: " << M << endl;

	int iParticle = fParteff.GetParticleIndex(partRec.GetPDG()); // pdg hypothesis => just check if pdg exists, but it SHOULD by construction (dixit Maksym) -> no effect
	if(iParticle < 0) continue;

	KFParticle TempPart = partRec;

	pdg = TempPart.GetPDG();
	TempPart.GetMass(M,ErrM);

	Pt = TempPart.GetPt();
	Rapidity = TempPart.GetRapidity();

	fmotherID = mcTrack.GetMotherId();

	fQ = TempPart.GetQ();
	fpt = Pt;
	fmass = M;
	fpdg = pdg;

	fY = Rapidity;
	fY -= fy_cm;        // LAB -> CM
	fY /= fy_proj_cm;

	phi = TempPart.GetPhi();

	if ( fdoPionRotation == kTRUE && TMath::Abs(pdg) == 211 ) phi += fPi;
	phi = Range(phi);

	if ( fdoBackYRotation == kTRUE && fY < 0. ) phi += fPi;
	phi = Range(phi);

	//cout << "reco phi = " << phi << ", (must be in radian and in [-pi, pi])" << endl;
	// TO CHANGE IF NEEDED
	// phi = phi * fPi/180.;
	// phi = Range(phi);
	//cout << "reco phi = " << phi << ", (must be in radian and in [-pi, pi])" << endl;

	fphi_to_RP = phi - RP;

	fphi_to_EP_PSD = phi - EP_PSD;
	fphi_to_EP_PSD1 = phi - EP_PSD1;
	fphi_to_EP_PSD2 = phi - EP_PSD2;
	fphi_to_EP_PSD3 = phi - EP_PSD3;

	fphi_to_EP_STS_harmo1 = phi - EP_STS_harmo1;
	fphi_to_EP_STS_harmo2 = phi - EP_STS_harmo2;

	fphi_to_RP = Range(fphi_to_RP);
	fphi_to_EP_PSD = Range(fphi_to_EP_PSD);
	fphi_to_EP_PSD1 = Range(fphi_to_EP_PSD1);
	fphi_to_EP_PSD2 = Range(fphi_to_EP_PSD2);
	fphi_to_EP_PSD3 = Range(fphi_to_EP_PSD3);
	fphi_to_EP_STS_harmo1 = Range(fphi_to_EP_STS_harmo1);
	fphi_to_EP_STS_harmo2 = Range2nd(fphi_to_EP_STS_harmo2);  // fphi_to_EP_STS_harmo2 must be in [-pi/2,pi/2] (pi-periodic)

	if (partMatch.IsRecoParticle()) fsignal = kTRUE;
	else fsignal = kFALSE;

	if (fpart_mc_reco == 1) outTree->Fill();

	// ===========  With fpart_mc_reco == 0, one saves corresponding MC particles

	if (partMatch.IsCombinatorialBG()) continue; // no MC particle for combinatorial bg 'n-tuplets'

	fmotherID = mcTrack.GetMotherId();
	fQ = mcTrack.GetCharge();
	fpt = mcTrack.GetPt();
	fmass = mcTrack.GetMass();
	fpdg = mcTrack.GetPdgCode();

	fY = mcTrack.GetRapidity();
	fY -= fy_cm;        // LAB -> CM
	fY /= fy_proj_cm;

	px = mcTrack.GetPx();
	py = mcTrack.GetPy();
	phi = TMath::ATan2(py, px);
	//cout << "mc phi = " << phi << endl;

	if ( fdoPionRotation == kTRUE && TMath::Abs(pdg) == 211 ) phi += fPi;
	phi = Range(phi);

	if ( fdoBackYRotation == kTRUE && fY < 0. ) phi += fPi;
	phi = Range(phi);

	fphi_to_RP = phi - RP;

	fphi_to_EP_PSD = phi - EP_PSD;
	fphi_to_EP_PSD1 = phi - EP_PSD1;
	fphi_to_EP_PSD2 = phi - EP_PSD2;
	fphi_to_EP_PSD3 = phi - EP_PSD3;

	fphi_to_EP_STS_harmo1 = phi - EP_STS_harmo1;
	fphi_to_EP_STS_harmo2 = phi - EP_STS_harmo2;

	fphi_to_RP = Range(fphi_to_RP);
	fphi_to_EP_PSD = Range(fphi_to_EP_PSD);
	fphi_to_EP_PSD1 = Range(fphi_to_EP_PSD1);
	fphi_to_EP_PSD2 = Range(fphi_to_EP_PSD2);
	fphi_to_EP_PSD3 = Range(fphi_to_EP_PSD3);
	fphi_to_EP_STS_harmo1 = Range(fphi_to_EP_STS_harmo1);
	fphi_to_EP_STS_harmo2 = Range2nd(fphi_to_EP_STS_harmo2);  // fphi_to_EP_STS_harmo2 must be in [-pi/2,pi/2] (pi-periodic)

	if (partMatch.IsRecoParticle()) fsignal = kTRUE;
	else fsignal = kFALSE;

	if (fpart_mc_reco == 0) outTree->Fill();
    }

    return kTRUE;
}

bool CbmAnaFlow::SortTree()
{
  cout << "CbmAnaFlow::: file containing Tree for flow analysis: " << fileName_tree << endl;

  TFile *f = new TFile(fileName_tree,"R");
  if (!f) cout << "-E- CbmAnaFlow::SortTree: pb with file containing Tree -----" << endl;
  
  TTree* tree = (TTree* ) f->Get("cbmana");
  if (!tree) cout << "-E- CbmAnaFlow::SortTree: pb with Tree in file -----" << endl;

  Int_t nentries = (Int_t) tree->GetEntries();
  cout << "-I- CbmAnaFlow::SortTree: #entries: " << nentries << endl;

  tree->Draw("fY","","goff");
  Int_t *index = new Int_t[nentries];
  TMath::Sort(nentries, tree->GetV1(), index, kFALSE);

  //Create an empty clone of the original tree
  TTree *tsortedY = (TTree*) tree->CloneTree(0);
  TTree *tsortedYpt_tmp = (TTree*) tree->CloneTree(0);
  outTree = (TTree*) tsortedYpt_tmp->CloneTree(0);       

  tsortedY->SetDirectory(0);
  tsortedYpt_tmp->SetDirectory(0);
  outTree->SetDirectory(0);

  for (Int_t i=0;i<nentries;i++)
  {
      //if ( i%1000 == 0 ) cout << "-I- CbmAnaFlow::SortTree: Entry (track) number: " <<  i << endl;

      tree->GetEntry(index[i]);
      tsortedY->Fill();
  }

  TAxis *hX = (TAxis*) hvn_vs_y_pt->GetXaxis();
  Int_t binX1, binX2;
  binX1 = hX->GetFirst();
  binX2 = hX->GetLast();

  cout << "-I- CbmAnaFlow::SortTree: Number of rapidity bins: " <<  binX2 - binX1 +1 << endl;

  Double_t binWidth_X = hX->GetBinWidth(binX1);
  Double_t binCenter_X;
  Int_t nentries_2;
  Int_t *index_2;
  TString smin_y_bin, smax_y_bin, Ybin;

  for (Int_t iy=binX1; iy<=binX2; iy++)
  {
      binCenter_X = hX->GetBinCenter(iy);

      smin_y_bin = "";
      smax_y_bin = "";

      smin_y_bin += binCenter_X - binWidth_X/2.;
      smax_y_bin += binCenter_X + binWidth_X/2.;

      Ybin = "fY >= " + smin_y_bin + " && fY < " + smax_y_bin;

      tsortedYpt_tmp = (TTree*) tsortedY->CopyTree(Ybin);
      nentries_2 = (Int_t) tsortedYpt_tmp->GetEntriesFast();

      index_2 = new Int_t[nentries_2];
      tsortedYpt_tmp->Draw("fpt","","goff");
      TMath::Sort(nentries_2, tsortedYpt_tmp->GetV1(), index_2, kFALSE);

      //if (iy%100 == 0) cout << "-I- CbmAnaFlow::SortTree: #entries in (" << Ybin << ") bin: " <<  nentries_2 << endl;

      for (Int_t j=0;j<nentries_2;j++)
      {
	  tsortedYpt_tmp->GetEntry(index_2[j]);
          outTree->Fill();
      }
  }

  delete [] index;
  delete [] index_2;

  return kTRUE;
}

bool CbmAnaFlow::CheckSortedTree()
{
    TAxis *hX = (TAxis*) hvn_vs_y_pt->GetXaxis();
    TAxis *hY = (TAxis*) hvn_vs_y_pt->GetYaxis();

    Double_t y;
    Double_t pt;
    Int_t y_bin = 0;
    Int_t pt_bin = 0;

    outTree->SetBranchAddress("fY",&y);
    outTree->SetBranchAddress("fpt",&pt);

    Int_t nentries = (Int_t) outTree->GetEntriesFast();

    cout << "-I- CbmAnaFlow::CheckSortedTree: #entries = " << nentries << endl;

    for (Int_t i=0;i<nentries;i++)
    {
        outTree->GetEntry(i);

	if ( hX->FindBin(y) != y_bin || hY->FindBin(pt) != pt_bin )
	{
            cout << "-I- CbmAnaFlow::CheckSortedTree: y = " << y << ", pt = " << pt << endl;

	    y_bin = hX->FindBin(y);
	    pt_bin = hY->FindBin(pt);

	    cout << "-I- CbmAnaFlow::CheckSortedTree: bin: [" << y_bin << ", " << pt_bin << "]" << endl;
	}
    }
    return kTRUE;
}

// ========================================== Simple draw (no reco. eff. correction) ==================================================

bool CbmAnaFlow::Draw_diffvn_simple()
{ 
  cout << "CbmAnaFlow::Draw_diffvn_simple: file containing Tree for flow analysis: " << fileName_tree << endl;

  TFile *f = new TFile(fileName_tree,"R");
  if (!f) cout << "-E- CbmAnaFlow::Draw_diffvn_simple: pb with file containing Tree -----" << endl;
  
  TTree* tree = (TTree* ) f->Get("cbmana");
  if (!tree) cout << "-E- CbmAnaFlow::Draw_diffvn_simple: pb with Tree in file -----" << endl;

  TString smin_pt_bin, smax_pt_bin, PTbin;
  TString smin_y_bin, smax_y_bin, Ybin;
  TString ALLcut;

  Double_t corrFactor[2];
  corrFactor[0] = 1.;
  corrFactor[1] = 0.;

  if (fEP_meth != "RP" && fdoEPCorr == kTRUE) CalcCorrectionFactors(corrFactor);  
  cout << "CbmAnaFlow::Draw_diffvn_simple: corrFactor = " << corrFactor[0] << " +/- " << corrFactor[1] << endl;

  cout << "+++++++++++++++++++++++++++++++" << endl;
  cout << "+++++++++++++++++++++++++++++++" << endl;
  cout << "======= pt dependence of v"  << fsel_factor << " of " << fsel_pdg << " particles" << endl;

  Int_t Nbin;
  Double_t min, max, step;

  Nbin = fsel_PTNbin;
  min = fsel_PTmin;
  max = fsel_PTmax;
  step = (fsel_PTmax - fsel_PTmin)/fsel_PTNbin;

  Float_t meanFlow0[Nbin], emeanFlow0[Nbin];
  Float_t meanX0[Nbin], emeanX0[Nbin];
  Double_t N = 0.;

  for (Int_t ipt=0; ipt<Nbin; ipt++)     
  {
      smin_pt_bin = "";
      smax_pt_bin = "";

      smin_pt_bin += min + ipt * step;
      smax_pt_bin += min + (ipt+1) * step;
      
      PTbin = "fpt >= " + smin_pt_bin + " && fpt < " + smax_pt_bin;
      ALLcut = Bcut + " && " + Ycut + " && " + PTbin + PDGcut + Qcut + PRIMcut;

      //cout << "ALLcut: " << ALLcut << endl;

      if (fcoef_meth == "cosnphi") CalcFlowCoef_cosnphi(tree, ALLcut, meanFlow0, emeanFlow0, ipt, N);
      if (fcoef_meth == "phifit") CalcFlowCoef_phifit(tree, ALLcut, meanFlow0, emeanFlow0, ipt, N);
      cout << "N = " << N << endl;

      // correction for EP fluctu around true RP
      CalcErrorProp_quot(meanFlow0[ipt], emeanFlow0[ipt], corrFactor[0], corrFactor[1]); 

      CalcMeanPt(tree, ALLcut, meanX0, emeanX0, ipt);

      if (meanX0[ipt] == 0.)
      {
	  meanX0[ipt] = min + (ipt+0.5) * step;
	  emeanX0[ipt] = step/TMath::Sqrt(12.);
      }

      cout << "<pt> = " << meanX0[ipt] << " +/- " << emeanX0[ipt] << ", v" + fsel_factor + " = " << meanFlow0[ipt] << " +/- " << emeanFlow0[ipt] << endl;
  }

  gr0 = new TGraphErrors(Nbin,meanX0,meanFlow0,emeanX0,emeanFlow0);
  gr0->SetName("gr0");
  gr0->SetMarkerColor(kBlack);
  gr0->SetMarkerStyle(25);
  gr0->SetLineColor(kBlack);

  cout << "+++++++++++++++++++++++++++++++" << endl;
  cout << "+++++++++++++++++++++++++++++++" << endl;
  cout << "======= rapidity dependence of v" << fsel_factor << " of '" << fsel_pdg << "' particles" << endl;

  Nbin = fsel_YNbin;
  min = fsel_Ymin;
  max = fsel_Ymax;
  step = (fsel_Ymax - fsel_Ymin)/fsel_YNbin;

  Float_t meanFlow1[Nbin], emeanFlow1[Nbin];
  Float_t meanX1[Nbin], emeanX1[Nbin];

  for (Int_t iy=0; iy<Nbin; iy++)
  {
      smin_y_bin = "";
      smax_y_bin = "";

      smin_y_bin += min + iy * step;
      smax_y_bin += min + (iy+1) * step;
      
      Ybin = "fY >= " + smin_y_bin + " && fY < " + smax_y_bin;
      ALLcut = Bcut + " && " + Ybin + " && " + PTcut + PDGcut + Qcut + PRIMcut;

      //cout << "ALLcut: " << ALLcut << endl;

      if (fcoef_meth == "cosnphi") CalcFlowCoef_cosnphi(tree, ALLcut, meanFlow1, emeanFlow1, iy, N);
      if (fcoef_meth == "phifit") CalcFlowCoef_phifit(tree, ALLcut, meanFlow1, emeanFlow1, iy, N);
      cout << "N = " << N << endl;

      // correction for EP fluctu around true RP
      CalcErrorProp_quot(meanFlow1[iy], emeanFlow1[iy], corrFactor[0], corrFactor[1]);

      CalcMeanY(tree, ALLcut, meanX1, emeanX1, iy);

      if (meanX1[iy] == 0.)
      {
	  meanX1[iy] = min + (iy+0.5) * step;
	  emeanX1[iy] = step/TMath::Sqrt(12.);
      }

      cout << "<y> = " << meanX1[iy] << " +/- " << emeanX1[iy] << ", v" + fsel_factor + " = " << meanFlow1[iy] << " +/- " << emeanFlow1[iy] << endl;
  }

  gr1 = new TGraphErrors(Nbin,meanX1,meanFlow1,emeanX1,emeanFlow1);
  gr1->SetName("gr1");
  gr1->SetMarkerColor(kBlack);
  gr1->SetMarkerStyle(25);
  gr1->SetLineColor(kBlack);

  //======= write output to ascii
  Nbin = fsel_PTNbin;
  ofstream outtxtfile1(fileName_ascii_out_pt);
  if (outtxtfile1.is_open())
  {
      for (int i=0;i<Nbin;i++) outtxtfile1 << meanX0[i] << endl;
      outtxtfile1 << "" << endl;
      for (int i=0;i<Nbin;i++) outtxtfile1 << emeanX0[i] << endl;
      outtxtfile1 << "" << endl;
      for (int i=0;i<Nbin;i++) outtxtfile1 << meanFlow0[i] << endl;
      outtxtfile1 << "" << endl;
      for (int i=0;i<Nbin;i++) outtxtfile1 << emeanFlow0[i] << endl;
      outtxtfile1.close();
  }

  //======= write output to ascii
  Nbin = fsel_YNbin;
  ofstream outtxtfile2(fileName_ascii_out_y);
  if (outtxtfile2.is_open())
  {
      for (int i=0;i<Nbin;i++) outtxtfile2 << meanX1[i] << endl;
      outtxtfile2 << "" << endl;
      for (int i=0;i<Nbin;i++) outtxtfile2 << emeanX1[i] << endl;
      outtxtfile2 << "" << endl;
      for (int i=0;i<Nbin;i++) outtxtfile2 << meanFlow1[i] << endl;
      outtxtfile2 << "" << endl;
      for (int i=0;i<Nbin;i++) outtxtfile2 << emeanFlow1[i] << endl;
      outtxtfile2.close();
  }

  ///// DRAW

  TCanvas *c1 = new TCanvas("c1"," vn vs pt ",200,10,700,500);
  c1->SetFillColor(0);
  c1->SetGrid();

  // draw a frame to define the range
  TH1F *hr = c1->DrawFrame(0.,-1.,3.,1.);
  hr->SetXTitle("Tranverse momentum p_{T} [Gev/c]");
  hr->GetXaxis()->CenterTitle();
  hr->SetYTitle("v_{" + fsel_factor + "}");
  hr->GetYaxis()->CenterTitle();
  gr0->Draw("P");

  TCanvas *c2 = new TCanvas("c2"," vn vs y ",200,10,700,500);
  c2->SetFillColor(0);
  c2->SetGrid();

  // draw a frame to define the range
  TH1F *hr2 = c2->DrawFrame(-2.,-1.,2.,1.);
  hr2->SetXTitle("Rapidity (c.m., normalized to Y_{proj})");
  hr2->GetXaxis()->CenterTitle();
  hr2->SetYTitle("v_{" + fsel_factor + "}");
  hr2->GetYaxis()->CenterTitle();
  gr1->Draw("P");

  return kTRUE;
}

bool CbmAnaFlow::Draw_intvn_simple()
{ 
  cout << "CbmAnaFlow::Draw_intvn_simple: file containing Tree for flow analysis: " << fileName_tree << endl;

  TFile *f = new TFile(fileName_tree,"R");
  if (!f) cout << "-E- CbmAnaFlow::Draw_intvn_simple: pb with file containing Tree -----" << endl;
  
  TTree* tree = (TTree* ) f->Get("cbmana");
  if (!tree) cout << "-E- CbmAnaFlow::Draw_intvn_simple: pb with Tree in file -----" << endl;

  TString ALLcut;
  ALLcut = Bcut + " && " + Ycut + " && " + PTcut + PDGcut + Qcut + PRIMcut;      
  cout << "ALLcut: " << ALLcut << endl;

  Double_t corrFactor[2];
  corrFactor[0] = 1.;
  corrFactor[1] = 0.;

  if (fEP_meth != "RP" && fdoEPCorr == kTRUE) CalcCorrectionFactors(corrFactor);  
  cout << "CbmAnaFlow::Draw_intvn_simple: corrFactor = " << corrFactor[0] << " +/- " << corrFactor[1] << endl;

  Int_t bin = 0;
  Int_t Nbin = 2;
  Double_t N = 0.;
  Float_t meanFlow2[Nbin], emeanFlow2[Nbin];

  if (fcoef_meth == "cosnphi") CalcFlowCoef_cosnphi(tree, ALLcut, meanFlow2, emeanFlow2, bin, N);
  if (fcoef_meth == "phifit") CalcFlowCoef_phifit(tree, ALLcut, meanFlow2, emeanFlow2, bin, N);
  cout << "N = " << N << endl;

  // correction for EP fluctu around true RP
  CalcErrorProp_quot(meanFlow2[bin], emeanFlow2[bin], corrFactor[0], corrFactor[1]);

  cout << "Integrated v" << fsel_factor << " = " << meanFlow2[bin] << " +/- " << emeanFlow2[bin] << " in collisions with (STS) multiplicity bwt " << ssel_Bmin << " and " << ssel_Bmax << "." << endl;

  return kTRUE;
}

// ========================================== Create vn, N, eff (y, pt) 2D histograms for efficiency correction ===================

bool CbmAnaFlow::Create2Dhisto_Y_vs_pt()
{
  cout << "CbmAnaFlow::Create2Dhisto_Y_vs_pt: file containing Tree for flow analysis: " << fileName_tree << endl;

  TFile *f = new TFile(fileName_tree,"R");
  if (!f) cout << "-E- CbmAnaFlow::Create2Dhisto_Y_vs_pt: pb with file containing Tree -----" << endl;
  
  TTree* tree = (TTree* ) f->Get("cbmana");
  if (!tree) cout << "-E- CbmAnaFlow::Create2Dhisto_Y_vs_pt: pb with Tree in file -----" << endl;

  Double_t binCenter_X, binWidth_X;
  Double_t binCenter_Y, binWidth_Y;

  TString smin_pt_bin, smax_pt_bin, PTbin;
  TString smin_y_bin, smax_y_bin, Ybin;
  TString ALLcut;

  Double_t vn, vn_err;
  Double_t N, N_err;

  Int_t bin = 0;
  Int_t Nbin = 2;
  Float_t meanFlow[Nbin], emeanFlow[Nbin];
  Float_t meanX[Nbin], emeanX[Nbin];

  Double_t corrFactor[2];
  corrFactor[0] = 1.;
  corrFactor[1] = 0.;

  if (fEP_meth != "RP" && fdoEPCorr == kTRUE) CalcCorrectionFactors(corrFactor);  
  cout << "CbmAnaFlow::Create2Dhisto_Y_vs_pt: corrFactor = " << corrFactor[0] << " +/- " << corrFactor[1] << endl;

  TAxis *hX = (TAxis*) hvn_vs_y_pt->GetXaxis();
  TAxis *hY = (TAxis*) hvn_vs_y_pt->GetYaxis();

  Int_t binX1, binX2, binY1, binY2;
  binX1 = hX->GetFirst();   
  binX2 = hX->GetLast();
  binY1 = hY->GetFirst();   
  binY2 = hY->GetLast();

  cout << "CbmAnaFlow::Create2Dhisto_Y_vs_pt: binning for 2Dhistos: X: [" << binX1 << ", " << binX2 << "], Y: [" << binY1 << ", " << binY2 << "]" << endl;

  for (Int_t iy=binX1; iy<=binX2; iy++)
  {
      binCenter_X = hX->GetBinCenter(iy);
      binWidth_X = hX->GetBinWidth(iy);

      smin_y_bin = "";
      smax_y_bin = "";

      smin_y_bin += binCenter_X - binWidth_X/2.;
      smax_y_bin += binCenter_X + binWidth_X/2.;

      Ybin = "fY >= " + smin_y_bin + " && fY < " + smax_y_bin;

      for (Int_t ipt=binY1; ipt<=binY2; ipt++)
      {
	 binCenter_Y = hY->GetBinCenter(ipt);
	 binWidth_Y = hY->GetBinWidth(ipt);

         smin_pt_bin = "";
	 smax_pt_bin = "";

	 smin_pt_bin += binCenter_Y - binWidth_Y/2.;
	 smax_pt_bin += binCenter_Y + binWidth_Y/2.;

         PTbin = "fpt >= " + smin_pt_bin + " && fpt < " + smax_pt_bin;

	 ALLcut = Bcut + " && " + Ybin + " && " + PTbin + PDGcut + Qcut + PRIMcut;
	 //cout << "ALLcut: " << ALLcut << endl;

         if (fcoef_meth == "cosnphi") CalcFlowCoef_cosnphi(tree, ALLcut, meanFlow, emeanFlow, bin, N);
         if (fcoef_meth == "phifit") CalcFlowCoef_phifit(tree, ALLcut, meanFlow, emeanFlow, bin, N);

         N_err = TMath::Sqrt(N);
         // cout << "N = " << N << " +/- " << N_err << endl;

         hN_vs_y_pt->SetBinContent(iy, ipt, N);
         hN_vs_y_pt->SetBinError(iy, ipt, N_err);

	 vn = meanFlow[bin];
	 vn_err = emeanFlow[bin];

         // correction for EP fluctu around true RP
         CalcErrorProp_quot(vn, vn_err, corrFactor[0], corrFactor[1]);

	 hvn_vs_y_pt->SetBinContent(iy, ipt, vn);
	 hvn_vs_y_pt->SetBinError(iy, ipt, vn_err);
      }
  }

  return kTRUE;
}

bool CbmAnaFlow::CreateEff_Y_vs_pt()
{
  cout << "CbmAnaFlow::CreateEff_Y_vs_pt:  files containing Tree for flow analysis: " << fileName_tree << " (reco - or mc of reco), " << fileName_tree_gen << " (gen)" << endl;

  TAxis *hX = (TAxis*) hEff_vs_y_pt->GetXaxis();
  TAxis *hY = (TAxis*) hEff_vs_y_pt->GetYaxis();

  Int_t binX1, binX2, binY1, binY2;
  binX1 = hX->GetFirst();
  binX2 = hX->GetLast();
  binY1 = hY->GetFirst();
  binY2 = hY->GetLast();

  cout << "CbmAnaFlow::CreateEff_Y_vs_pt: binning for 2Dhistos: X: [" << binX1 << ", " << binX2 << "], Y: [" << binY1 << ", " << binY2 << "]" << endl;

  if (fuseTrackatGen == kFALSE)
  {
      TFile *f1 = new TFile(fileName_tree,"R");
      TFile *f2 = new TFile(fileName_tree_gen,"R");
      if (!f1 || !f2) cout << "-E- CbmAnaFlow::CreateEff_Y_vs_pt: pb with files containing Tree -----" << endl;

      TTree* t1 = (TTree* ) f1->Get("cbmana");
      TTree* t2 = (TTree* ) f2->Get("cbmana");
      if (!t1 || !t2) cout << "-E- CbmAnaFlow::CreateEff_Y_vs_pt: pb with Trees in files -----" << endl;

      Double_t binCenter_X, binWidth_X;
      Double_t binCenter_Y, binWidth_Y;

      TString smin_pt_bin, smax_pt_bin, PTbin;
      TString smin_y_bin, smax_y_bin, Ybin;
      TString ALLcut;

      Double_t N1, N_err1;
      Double_t N2, N_err2;

      Int_t bin = 0;
      Int_t Nbin = 2;
      Float_t meanFlow[Nbin], emeanFlow[Nbin];

      for (Int_t iy=binX1; iy<=binX2; iy++)
      {
	  binCenter_X = hX->GetBinCenter(iy);
	  binWidth_X = hX->GetBinWidth(iy);

	  smin_y_bin = "";
	  smax_y_bin = "";

	  smin_y_bin += binCenter_X - binWidth_X/2.;
	  smax_y_bin += binCenter_X + binWidth_X/2.;

	  Ybin = "fY >= " + smin_y_bin + " && fY < " + smax_y_bin;

	  for (Int_t ipt=binY1; ipt<=binY2; ipt++)
	  {
	      binCenter_Y = hY->GetBinCenter(ipt);
	      binWidth_Y = hY->GetBinWidth(ipt);

	      smin_pt_bin = "";
	      smax_pt_bin = "";

	      smin_pt_bin += binCenter_Y - binWidth_Y/2.;
	      smax_pt_bin += binCenter_Y + binWidth_Y/2.;

	      PTbin = "fpt >= " + smin_pt_bin + " && fpt < " + smax_pt_bin;

	      ALLcut = Bcut + " && " + Ybin + " && " + PTbin + PDGcut + Qcut + PRIMcut;
	      //cout << "ALLcut: " << ALLcut << endl;

	      CalcFlowCoef_cosnphi(t1, ALLcut, meanFlow, emeanFlow, bin, N1);    // just retrieve selected entries
	      N_err1 = TMath::Sqrt(N1);

	      CalcFlowCoef_cosnphi(t2, ALLcut, meanFlow, emeanFlow, bin, N2);    // just retrieve selected entries
	      N_err2 = TMath::Sqrt(N2);

              CalcErrorProp_quot(N1, N_err1, N2, N_err2);

	      hEff_vs_y_pt->SetBinContent(iy, ipt, N1);
	      hEff_vs_y_pt->SetBinError(iy, ipt, N_err1);
	  }
      }
  }
  else
  {
      for (Int_t iy=binX1; iy<=binX2; iy++)
      {
	  for (Int_t ipt=binY1; ipt<=binY2; ipt++)
	  {
	      hEff_vs_y_pt->SetBinContent(iy, ipt, 1.);
	      hEff_vs_y_pt->SetBinError(iy, ipt, 0.);
	  }
      }
  }

  return kTRUE;
}

// ========================================== Draw flow with reco. eff. correction (required: (y, pt) 2D histogram creation) ====================

bool CbmAnaFlow::Draw_diffvn_Effcorr()
{
    TFile *f = new TFile(fileName_tree,"R");
    if (!f) cout << "-E- CbmAnaFlow::Draw_diffvn_Effcorr: pb with files containing Tree -----" << endl;
    TTree* t = (TTree* ) f->Get("cbmana");
    
    TString smin_pt_bin, smax_pt_bin, PTbin;
    TString smin_y_bin, smax_y_bin, Ybin; 
    TString ALLcut;

    Int_t Nbin;
    Double_t min, max, step;
    Double_t y_tmp, pt_tmp;

    Double_t vn_tmp, vn_err_tmp;
    Double_t vn_tot, vn_err_tot;

    Double_t N_tmp;
    Double_t N_err_tmp;
    Double_t N_tot;
    Double_t N_err_tot;

    TAxis *hX = (TAxis*) hvn_vs_y_pt->GetXaxis();
    TAxis *hY = (TAxis*) hvn_vs_y_pt->GetYaxis();
    Int_t binX1, binX2, binY1, binY2;

    cout << "+++++++++++++++++++++++++++++++" << endl;
    cout << "+++++++++++++++++++++++++++++++" << endl;
    cout << "CbmAnaFlow::Draw_diffvn_Effcorr: pt dependence of v" << fsel_factor << " of '" << fsel_pdg << "' particles" << endl;

    binX1 = hX->FindBin(fsel_Ycut_min);
    binX2 = hX->FindBin(fsel_Ycut_max);

    Nbin = fsel_PTNbin;
    min = fsel_PTmin;
    max = fsel_PTmax;
    step = (fsel_PTmax - fsel_PTmin)/fsel_PTNbin;

    Float_t meanX0[Nbin], emeanX0[Nbin];
    Float_t meanFlow0[Nbin], emeanFlow0[Nbin];

    for (Int_t ipt=0; ipt<Nbin; ipt++)
    {
        //meanX0[ipt] = min + (ipt+0.5) * step;
	//emeanX0[ipt] = step/TMath::Sqrt(12);

        // Init
        vn_tmp = 0.;
	vn_err_tmp = 0.;
	N_tmp = 0.;
	N_err_tmp = 0.;
	vn_tot = 0.;
	vn_err_tot = 0.;
	N_tot = 0.;
	N_err_tot = 0.;

        smin_pt_bin = "";
	smax_pt_bin = "";

	smin_pt_bin += min + ipt * step;
	smax_pt_bin += min + (ipt+1) * step;

	PTbin = "fpt >= " + smin_pt_bin + " && fpt < " + smax_pt_bin;
	ALLcut = Bcut + " && " + Ycut + " && " + PTbin + PDGcut + Qcut + PRIMcut;

	//cout << "ALLcut: " << ALLcut << endl;

        CalcMeanPt(t, ALLcut, meanX0, emeanX0, ipt);

	pt_tmp = min + ipt * step;
	binY1 = hY->FindBin(pt_tmp);

	pt_tmp = min + (ipt+1) * step;
	binY2 = hY->FindBin(pt_tmp);

	for (Int_t iptpt=binY1; iptpt<=binY2; iptpt++)
	{
	    
		for (Int_t iy=binX1; iy<=binX2; iy++)
		{
		    vn_tmp = hvn_vs_y_pt->GetBinContent(iy, iptpt);
		    vn_err_tmp = hvn_vs_y_pt->GetBinError(iy, iptpt);

		    N_tmp = hN_vs_y_pt->GetBinContent(iy, iptpt);
		    N_err_tmp = TMath::Sqrt(N_tmp);

                    //cout << "N_tmp (before eff. corr.) = " << N_tmp << " +/- " << N_err_tmp << endl;

		    if (fdoEff == kTRUE && fuseTrackatGen == kFALSE) EffCorrection(iy, iptpt, N_tmp, N_err_tmp);

                    //cout << "N_tmp (after eff. corr.) = " << N_tmp << " +/- " << N_err_tmp << endl;

                    CalcErrorProp_prod(vn_tmp, vn_err_tmp, N_tmp, N_err_tmp);

		    vn_tot += vn_tmp;
		    vn_err_tot = TMath::Sqrt( TMath::Power(vn_err_tot, 2) + TMath::Power(vn_err_tmp, 2) );

		    N_tot += N_tmp;
		    N_err_tot = TMath::Sqrt( TMath::Power(N_err_tot, 2) + TMath::Power(N_err_tmp, 2) );
		}
	}

        CalcErrorProp_quot(vn_tot, vn_err_tot, N_tot, N_err_tot);

	meanFlow0[ipt] = vn_tot;
	emeanFlow0[ipt] = vn_err_tot;

	cout << "<pt> = " << meanX0[ipt] << " +/- " << emeanX0[ipt] << ", v" + fsel_factor + " = " << meanFlow0[ipt] << " +/- " << emeanFlow0[ipt] << endl;
    }

    gr0 = new TGraphErrors(Nbin,meanX0,meanFlow0,emeanX0,emeanFlow0);
    gr0->SetName("gr0");
    gr0->SetMarkerColor(kBlack);
    gr0->SetMarkerStyle(25);
    gr0->SetLineColor(kBlack);

    cout << "+++++++++++++++++++++++++++++++" << endl;
    cout << "+++++++++++++++++++++++++++++++" << endl;
    cout << "CbmAnaFlow::Draw_diffvn_Effcorr: rapidity dependence of v" << fsel_factor << " of '" << fsel_pdg << "' particles" << endl;

    binY1 = hY->FindBin(fsel_PTcut_min);
    binY2 = hY->FindBin(fsel_PTcut_max);

    Nbin = fsel_YNbin;
    min = fsel_Ymin;
    max = fsel_Ymax;
    step = (fsel_Ymax - fsel_Ymin)/fsel_YNbin;

    Float_t meanX1[Nbin], emeanX1[Nbin];
    Float_t meanFlow1[Nbin], emeanFlow1[Nbin];

    for (Int_t iy=0; iy<Nbin; iy++)
    {
        //meanX1[iy] = min + (iy+0.5) * step;
	//emeanX1[iy] = step/TMath::Sqrt(12);

        // Init
        vn_tmp = 0.;
	vn_err_tmp = 0.;
	N_tmp = 0.;
	N_err_tmp = 0.;
	vn_tot = 0.;
	vn_err_tot = 0.;
	N_tot = 0.;
	N_err_tot = 0.;

        smin_y_bin = "";
	smax_y_bin = "";

	smin_y_bin += min + iy * step;
	smax_y_bin += min + (iy+1) * step;

	Ybin = "fY >= " + smin_y_bin + " && fY < " + smax_y_bin;
	ALLcut = Bcut + " && " + Ybin + " && " + PTcut + PDGcut + Qcut + PRIMcut;

        CalcMeanY(t, ALLcut, meanX1, emeanX1, iy);

	y_tmp = min + iy * step;
	binX1 = hX->FindBin(y_tmp);

	y_tmp = min + (iy+1) * step;
	binX2 = hX->FindBin(y_tmp);

	for (Int_t iyy=binX1; iyy<=binX2; iyy++)
	{
	    for (Int_t ipt=binY1; ipt<=binY2; ipt++)
	    {
		vn_tmp = hvn_vs_y_pt->GetBinContent(iyy, ipt);
                vn_err_tmp = hvn_vs_y_pt->GetBinError(iyy, ipt);

		N_tmp = hN_vs_y_pt->GetBinContent(iyy, ipt);
		N_err_tmp = TMath::Sqrt(N_tmp);

                if (fdoEff == kTRUE && fuseTrackatGen == kFALSE) EffCorrection(iyy, ipt, N_tmp, N_err_tmp);

		//cout << "vn_tmp = " << vn_tmp << ", N = " << N << endl;

                CalcErrorProp_prod(vn_tmp, vn_err_tmp, N_tmp, N_err_tmp);

		vn_tot += vn_tmp;
		vn_err_tot = TMath::Sqrt( TMath::Power(vn_err_tot, 2) + TMath::Power(vn_err_tmp, 2) );

		N_tot += N_tmp;
		N_err_tot = TMath::Sqrt( TMath::Power(N_err_tot, 2) + TMath::Power(N_err_tmp, 2) );
	    }
	}

        CalcErrorProp_quot(vn_tot, vn_err_tot, N_tot, N_err_tot);

	meanFlow1[iy] = vn_tot;
	emeanFlow1[iy] = vn_err_tot;

	cout << "<y> = " << meanX1[iy] << " +/- " << emeanX1[iy] << ", v" + fsel_factor + " = " << meanFlow1[iy] << " +/- " << emeanFlow1[iy] << endl;
    }

    gr1 = new TGraphErrors(Nbin,meanX1,meanFlow1,emeanX1,emeanFlow1);
    gr1->SetName("gr1");
    gr1->SetMarkerColor(kBlack);
    gr1->SetMarkerStyle(25);
    gr1->SetLineColor(kBlack);

    //======= write output to ascii
    Nbin = fsel_PTNbin;
    ofstream outtxtfile1(fileName_ascii_out_pt);
    if (outtxtfile1.is_open())
    {
	for (int i=0;i<Nbin;i++) outtxtfile1 << meanX0[i] << endl;
	outtxtfile1 << "" << endl;
	for (int i=0;i<Nbin;i++) outtxtfile1 << emeanX0[i] << endl;
	outtxtfile1 << "" << endl;
	for (int i=0;i<Nbin;i++) outtxtfile1 << meanFlow0[i] << endl;
	outtxtfile1 << "" << endl;
	for (int i=0;i<Nbin;i++) outtxtfile1 << emeanFlow0[i] << endl;
	outtxtfile1.close();
    }

    //======= write output to ascii
    Nbin = fsel_YNbin;
    ofstream outtxtfile2(fileName_ascii_out_y);
    if (outtxtfile2.is_open())
    {
	for (int i=0;i<Nbin;i++) outtxtfile2 << meanX1[i] << endl;
	outtxtfile2 << "" << endl;
	for (int i=0;i<Nbin;i++) outtxtfile2 << emeanX1[i] << endl;
	outtxtfile2 << "" << endl;
	for (int i=0;i<Nbin;i++) outtxtfile2 << meanFlow1[i] << endl;
	outtxtfile2 << "" << endl;
	for (int i=0;i<Nbin;i++) outtxtfile2 << emeanFlow1[i] << endl;
	outtxtfile2.close();
    }

    ///// DRAW

    TCanvas *c1 = new TCanvas("c1"," v_{" + fsel_factor + "} vs pt ",200,10,700,500);
    c1->SetFillColor(0);
    c1->SetGrid();

    // draw a frame to define the range
    TH1F *hr = c1->DrawFrame(0.,-1.,3.,1.);
    hr->SetXTitle("Tranverse momentum p_{T} [Gev/c]");
    hr->GetXaxis()->CenterTitle();
    hr->SetYTitle("v_{" + fsel_factor + "}");
    hr->GetYaxis()->CenterTitle();
    gr0->Draw("P");

    //
    TCanvas *c2 = new TCanvas("c2"," v_{" + fsel_factor + "} vs y ",200,10,700,500);
    c2->SetFillColor(0);
    c2->SetGrid();

    // draw a frame to define the range
    TH1F *hr2 = c2->DrawFrame(-2.,-1.,2.,1.);
    hr2->SetXTitle("Rapidity (c.m., normalized to Y_{proj})");
    hr2->GetXaxis()->CenterTitle();
    hr2->SetYTitle("v_{" + fsel_factor + "}");
    hr2->GetYaxis()->CenterTitle();
    gr1->Draw("P");

    //c1->SaveAs("plot/protonflow/ptDependance_v" + fsel_factor);
    //c2->SaveAs("plot/protonflow/yDependance_v" + fsel_factor);

    return kTRUE;
}

bool CbmAnaFlow::Draw_intvn_Effcorr()
{
    Double_t vn_tmp, vn_err_tmp;
    Double_t vn_tot, vn_err_tot;

    Double_t N_tmp;
    Double_t N_err_tmp;
    Double_t N_tot;
    Double_t N_err_tot;

    vn_tmp = 0.;
    vn_err_tmp = 0.;
    N_tmp = 0.;
    N_err_tmp = 0.;
    vn_tot = 0.;
    vn_err_tot = 0.;
    N_tot = 0.;
    N_err_tot = 0.;

    TAxis *hX = (TAxis*) hvn_vs_y_pt->GetXaxis();
    TAxis *hY = (TAxis*) hvn_vs_y_pt->GetYaxis();
    Int_t binX1, binX2, binY1, binY2;

    binY1 = hY->FindBin(fsel_PTcut_min);
    binY2 = hY->FindBin(fsel_PTcut_max);

    binX1 = hX->FindBin(fsel_Ycut_min);
    binX2 = hX->FindBin(fsel_Ycut_max);

    for (Int_t ipt=binY1; ipt<=binY2; ipt++)
    {
	
	    for (Int_t iy=binX1; iy<=binX2; iy++)
	    {
		vn_tmp = hvn_vs_y_pt->GetBinContent(iy, ipt);
		vn_err_tmp = hvn_vs_y_pt->GetBinError(iy, ipt);

		N_tmp = hN_vs_y_pt->GetBinContent(iy, ipt);
		N_err_tmp = TMath::Sqrt(N_tmp);

		//cout << "N_tmp (before eff. corr.) = " << N_tmp << " +/- " << N_err_tmp << endl;

		if (fdoEff == kTRUE && fuseTrackatGen == kFALSE) EffCorrection(iy, ipt, N_tmp, N_err_tmp);

		//cout << "N_tmp (after eff. corr.) = " << N_tmp << " +/- " << N_err_tmp << endl;

                CalcErrorProp_prod(vn_tmp, vn_err_tmp, N_tmp, N_err_tmp);

		vn_tot += vn_tmp;
		vn_err_tot = TMath::Sqrt( TMath::Power(vn_err_tot, 2) + TMath::Power(vn_err_tmp, 2) );

		N_tot += N_tmp;
		N_err_tot = TMath::Sqrt( TMath::Power(N_err_tot, 2) + TMath::Power(N_err_tmp, 2) );
	    }
    }

    CalcErrorProp_quot(vn_tot, vn_err_tot, N_tot, N_err_tot);
    
    cout << "Integrated v" << fsel_factor << " = " << vn_tot << " +/- " << vn_err_tot << " in collisions with (STS) multiplicity bwt " << ssel_Bmin << " and " << ssel_Bmax << "." << endl;

    return kTRUE;
}

// ========================================== Draw with reco. eff. correction (required: (y, pt) 2D histogram creation)     ====================
// ========================================== ! WARNING !: No restriction on number of (Y, pt) bins BUT requires TTree sorting  ====================

bool CbmAnaFlow::Create2Dhisto_Y_vs_pt_1TreeLoop()  
{
    TAxis *hX = (TAxis*) hvn_vs_y_pt->GetXaxis();
    TAxis *hY = (TAxis*) hvn_vs_y_pt->GetYaxis();

    Double_t phi;
    Double_t y;
    Double_t pt;
    Double_t B;
    Int_t pdg;
    Int_t motherID;
    int mult;

    Int_t y_bin = 1;
    Int_t pt_bin = 1;

    Int_t y_bin_prevPart = 1;
    Int_t pt_bin_prevPart = 1;

    bool init_bin = kTRUE;

    TString sel_EP_meth = "fphi_to_" + fEP_meth;
    cout << "CbmAnaFlow::Create2Dhisto_Y_vs_pt_1TreeLoop: RP method: " << fEP_meth << endl;

    Double_t cosnphi = 0.;
    Double_t factor = 1.;
    if ( fsel_factor == "1" ) factor = 1.;
    if ( fsel_factor == "2" ) factor = 2.;

    outTree->SetBranchAddress(sel_EP_meth,&phi);
    outTree->SetBranchAddress("fY",&y);
    outTree->SetBranchAddress("fpt",&pt);
    outTree->SetBranchAddress("fpdg", &pdg);
    outTree->SetBranchAddress("fB_MC", &B);
    outTree->SetBranchAddress("fmotherID", &motherID);
    outTree->SetBranchAddress("fmult", &mult);

    Int_t nentries = (Int_t) outTree->GetEntriesFast();
    cout << "CbmAnaFlow::Create2Dhisto_Y_vs_pt_1TreeLoop: nentries = " << nentries << endl;

    for (Int_t i=0;i<nentries;i++)
    {
        outTree->GetEntry(i);

        //if (B < fsel_Bmin || B >= fsel_Bmax) continue;
        if (mult < fsel_Bmin || mult >= fsel_Bmax) continue;
	if (fsel_pdg != 0. && pdg != fsel_pdg) continue;
        if (fdoPrim == kTRUE && motherID>-1) continue;

	if (init_bin == kTRUE)
	{
	    y_bin_prevPart = hX->FindBin(y);
	    pt_bin_prevPart = hY->FindBin(pt);

            init_bin = kFALSE;
	}

        y_bin = hX->FindBin(y);
	pt_bin = hY->FindBin(pt);

        //cout << "===========================================" << endl;
        //cout << "CbmAnaFlow::Create2Dhisto_Y_vs_pt_1TreeLoop: selected entry " << i << ", (y, pt) = (" << y << ", " << pt << "), (y bin, pt bin) = (" << y_bin << ", " << pt_bin << ")" << endl;

        if ( y_bin != y_bin_prevPart || pt_bin != pt_bin_prevPart )
	{
            //cout << "----- new bin" << endl;
	    //cout << "y = " << y << ", pt = " << pt << endl;
	    //cout << "bin: [" << y_bin << ", " << pt_bin << "]" << endl;

            CalcMeanY_1TreeLoop(y_bin_prevPart, pt_bin_prevPart);
            CalcMeanPt_1TreeLoop(y_bin_prevPart, pt_bin_prevPart);

            if (fcoef_meth == "cosnphi") CalcFlowCoef_cosnphi_1TreeLoop(y_bin_prevPart, pt_bin_prevPart);//, corrFactor);
	    if (fcoef_meth == "phifit") CalcFlowCoef_phifit_1TreeLoop(y_bin_prevPart, pt_bin_prevPart);//, corrFactor);

	    hpt->Reset("M");
	    hRapidity->Reset("M");
	    hphi->Reset("M");
	    hcosnphi->Reset("M");
	}

	hpt->Fill(pt);
        hRapidity->Fill(y);

	hphi->Fill(phi);

	cosnphi = TMath::Cos(factor*phi);
        hcosnphi->Fill(cosnphi);

	y_bin_prevPart = y_bin;
        pt_bin_prevPart = pt_bin;
    }
    return kTRUE;
}

void CbmAnaFlow::MergeHistoInFiles()
{
    TFile* old = gFile;

    TSystemDirectory dir(fdirname, fdirname);
    TList *files = dir.GetListOfFiles();

    cout << "String conditions for files' name: start by " << fin << ", contains " << finside << ", ends with " << fex << endl;

    TH2F* hRap_vs_y_pt_tmp;
    TH2F* hpt_vs_y_pt_tmp;
    TH2F* hvn_vs_y_pt_tmp;
    TH2F* hN_vs_y_pt_tmp;
    TFile* file_tmp = new TFile();

    int file_inc = 0;

    double yvec[NyBin][NptBin];
    double ptvec[NyBin][NptBin];
    double vnvec[NyBin][NptBin];
    double Nvec[NyBin][NptBin];

    double yvec_err[NyBin][NptBin];
    double ptvec_err[NyBin][NptBin];
    double vnvec_err[NyBin][NptBin];

    for (int i=0;i<NyBin; i++)
    {
	for (int j=0;j<NptBin; j++)
	{
	    yvec[i][j] = 0.;
	    ptvec[i][j] = 0.;
	    vnvec[i][j] = 0.;
            Nvec[i][j] = 0.;

            yvec_err[i][j] = 0.;
	    ptvec_err[i][j] = 0.;
	    vnvec_err[i][j] = 0.;
	}
    }
    Double_t vn_tmp, vn_err_tmp;
    Double_t pt_tmp, pt_err_tmp;
    Double_t y_tmp, y_err_tmp;
    Double_t N_tmp, N_err_tmp;

    if (files)
    {
	TSystemFile *file;
	TString fname;
	TIter next(files);
        TString fileName_abs;

	while ((file=(TSystemFile*)next()))
	{
	    fname = file->GetName();
	    if (!file->IsDirectory() && fname.BeginsWith(fin) && fname.EndsWith(fex) && fname.Contains(finside))
	    {
		fileName_abs = dir.GetName();
                fileName_abs += "/";
		fileName_abs += fname.Data();
		//cout << "file name:" << fileName_abs << endl;

		file_tmp = (TFile*) file_tmp->Open(fileName_abs);

		if (file_tmp && !file_tmp->IsZombie())
		{
                    file_tmp->cd();

		    hpt_vs_y_pt_tmp = (TH2F*) file_tmp->Get("pt_2D");
		    hRap_vs_y_pt_tmp = (TH2F*) file_tmp->Get("y_2D");
		    hvn_vs_y_pt_tmp = (TH2F*) file_tmp->Get("vn_2D");
		    hN_vs_y_pt_tmp = (TH2F*) file_tmp->Get("N_2D");

		    /*if (fname.EndsWith("0986_2Dhistos.root"))
		    {
			hpt_vs_y_pt_tmp->Draw();

			TCanvas* c2 = new TCanvas;
			hRap_vs_y_pt_tmp->Draw("c2");

			TCanvas* c3 = new TCanvas;
			hvn_vs_y_pt_tmp->Draw("c3");

			TCanvas* c4 = new TCanvas;
			hN_vs_y_pt_tmp->Draw("c4");
		    }*/

                    // Init - safety assignment
		    vn_tmp = 0.;
		    vn_err_tmp = 0.;
		    N_tmp = 0.;
		    N_err_tmp = 0.;
		    pt_tmp = 0.;
		    pt_err_tmp = 0.;
		    y_tmp = 0.;
		    y_err_tmp = 0.;

		    for (int i=0;i<NyBin; i++)
		    {
			for (int j=0;j<NptBin; j++)
			{
                            vn_tmp = hvn_vs_y_pt_tmp->GetBinContent(i+1, j+1);
			    vn_err_tmp = hvn_vs_y_pt_tmp->GetBinError(i+1, j+1);

			    N_tmp = hN_vs_y_pt_tmp->GetBinContent(i+1, j+1);
			    N_err_tmp = TMath::Sqrt(N_tmp);

			    pt_tmp = hpt_vs_y_pt_tmp->GetBinContent(i+1, j+1);
			    pt_err_tmp = hpt_vs_y_pt_tmp->GetBinError(i+1, j+1);

			    y_tmp = hRap_vs_y_pt_tmp->GetBinContent(i+1, j+1);
			    y_err_tmp = hRap_vs_y_pt_tmp->GetBinError(i+1, j+1);

			    CalcErrorProp_prod(vn_tmp, vn_err_tmp, N_tmp, N_err_tmp);
			    CalcErrorProp_prod(pt_tmp, pt_err_tmp, N_tmp, N_err_tmp);
			    CalcErrorProp_prod(y_tmp, y_err_tmp, N_tmp, N_err_tmp);

                            // vn integration
			    vnvec[i][j] += vn_tmp;
			    vnvec_err[i][j] = TMath::Sqrt( TMath::Power(vnvec_err[i][j], 2) + TMath::Power(vn_err_tmp, 2) );

			    // yield integration
			    Nvec[i][j] += N_tmp;

			    // pt integration
			    ptvec[i][j] += pt_tmp;
			    ptvec_err[i][j] = TMath::Sqrt( TMath::Power(ptvec_err[i][j], 2) + TMath::Power(pt_err_tmp, 2) );

			    // y integration
			    yvec[i][j] += y_tmp;
			    yvec_err[i][j] = TMath::Sqrt( TMath::Power(yvec_err[i][j], 2) + TMath::Power(y_err_tmp, 2) );
			}
		    }

                    file_inc++;
		}
		
		file_tmp->Close("R");
	    }
	}

        cout << "Number of merged files = " << file_inc << endl;
    }

    gFile = old;
    //delete old;

    int inc_emptybin = 0;
    Double_t vn_tot, vn_err_tot;
    Double_t pt_tot, pt_err_tot;
    Double_t y_tot, y_err_tot;
    Double_t N_tot, N_err_tot;

    for (int i=0;i<NyBin; i++)
    {
	for (int j=0;j<NptBin; j++)
	{
	    if (Nvec[i][j] != 0.)
	    {
                vn_tot = vnvec[i][j];
                y_tot = yvec[i][j];
                pt_tot = ptvec[i][j];
                N_tot = Nvec[i][j];

                vn_err_tot = vnvec_err[i][j];
                y_err_tot = yvec_err[i][j];
                pt_err_tot = ptvec_err[i][j];
		N_err_tot = TMath::Sqrt(Nvec[i][j]);

                CalcErrorProp_quot(vn_tot, vn_err_tot, N_tot, N_err_tot);
		CalcErrorProp_quot(pt_tot, pt_err_tot, N_tot, N_err_tot);
		CalcErrorProp_quot(y_tot, y_err_tot, N_tot, N_err_tot);

		// rapidity
		hRap_vs_y_pt->SetBinContent(i+1, j+1, y_tot);
		hRap_vs_y_pt->SetBinError(i+1, j+1, y_err_tot);

		// pt
		hpt_vs_y_pt->SetBinContent(i+1, j+1, pt_tot);
		hpt_vs_y_pt->SetBinError(i+1, j+1, pt_err_tot);

		// vn
		hvn_vs_y_pt->SetBinContent(i+1, j+1, vn_tot);
		hvn_vs_y_pt->SetBinError(i+1, j+1, vn_err_tot);

		// N
		hN_vs_y_pt->SetBinContent(i+1, j+1, N_tot);
		hN_vs_y_pt->SetBinError(i+1, j+1, N_err_tot);
	    }
	    else
	    {
		hRap_vs_y_pt->SetBinContent(i+1, j+1, 0.);
		hRap_vs_y_pt->SetBinError(i+1, j+1, 0.);

		hpt_vs_y_pt->SetBinContent(i+1, j+1, 0.);
		hpt_vs_y_pt->SetBinError(i+1, j+1, 0.);

		hvn_vs_y_pt->SetBinContent(i+1, j+1, 0.);
		hvn_vs_y_pt->SetBinError(i+1, j+1, 0.);

		hN_vs_y_pt->SetBinContent(i+1, j+1, 0.);
		hN_vs_y_pt->SetBinError(i+1, j+1, 0.);

                inc_emptybin++;

		//cout << "CbmAnaFlow::MergeHistoInFiles: empty bin: (" << i+1 << ", " << j+1 << "), Nvec[i][j] = " << Nvec[i][j] << endl;
            }
	}
    }

    cout << "CbmAnaFlow::MergeHistoInFiles: number of empty bins = " << inc_emptybin << endl;

}

bool CbmAnaFlow::CreateEff_Y_vs_pt_1TreeLoop()
{
    cout << "CbmAnaFlow::CreateEff_Y_vs_pt_1TreeLoop:  files containing Tree for flow analysis (reco): " << fileName_tree << endl;
    cout << "CbmAnaFlow::CreateEff_Y_vs_pt_1TreeLoop:  files containing Tree for flow analysis (mc of reco), " << fileName_tree_gen << endl;

  TAxis *hX = (TAxis*) hEff_vs_y_pt->GetXaxis();
  TAxis *hY = (TAxis*) hEff_vs_y_pt->GetYaxis();

  Int_t binX1, binX2, binY1, binY2;
  binX1 = hX->GetFirst();
  binX2 = hX->GetLast();
  binY1 = hY->GetFirst();
  binY2 = hY->GetLast();

  cout << "CbmAnaFlow::CreateEff_Y_vs_pt_1TreeLoop: binning for 2Dhistos: X: [" << binX1 << ", " << binX2 << "], Y: [" << binY1 << ", " << binY2 << "]" << endl;

  if (fuseTrackatGen == kFALSE)
  {
      TFile *f1 = new TFile(fileName_tree,"R");
      TFile *f2 = new TFile(fileName_tree_gen,"R");
      if (!f1 || !f2) cout << "-E- CbmAnaFlow::CreateEff_Y_vs_pt_1TreeLoop: pb with files containing Tree -----" << endl;

      TH2F* h1 = (TH2F*) f1->Get("N_2D");
      TH2F* h2 = (TH2F*) f2->Get("N_2D");
      if (!h1 || !h2) cout << "-E- CbmAnaFlow::CreateEff_Y_vs_pt_1TreeLoop: pb with 2D histos in files -----" << endl;

      Double_t N1, N_err1;
      Double_t N2, N_err2;

      for (Int_t iy=binX1; iy<=binX2; iy++)
      {
	  for (Int_t ipt=binY1; ipt<=binY2; ipt++)
	  {
	      N1 = h1->GetBinContent(iy, ipt);
	      N_err1 = TMath::Sqrt(N1);

	      N2 = h2->GetBinContent(iy, ipt);
	      N_err2 = TMath::Sqrt(N2);

	      CalcErrorProp_quot(N1, N_err1, N2, N_err2);

	      hEff_vs_y_pt->SetBinContent(iy, ipt, N1);
	      hEff_vs_y_pt->SetBinError(iy, ipt, N_err1);
	  }
      }
  }
  else
  {
      for (Int_t iy=binX1; iy<=binX2; iy++)
      {
	  for (Int_t ipt=binY1; ipt<=binY2; ipt++)
	  {
	      hEff_vs_y_pt->SetBinContent(iy, ipt, 1.);
	      hEff_vs_y_pt->SetBinError(iy, ipt, 0.);
	  }
      }
  }

  return kTRUE;
}

// ================================== Draw flow with reco. eff. correction (required: (y, pt) 2D histogram creation, '1 Tree Loop' procedure) ====================

bool CbmAnaFlow::Draw_diffvn_Effcorr_1TreeLoop()
{
    cout << "CbmAnaFlow::Draw_diffvn_Effcorr_1TreeLoop:  files containing Tree for flow analysis: " << fileName_tree << endl;

    TFile *f = new TFile(fileName_tree,"R");
    if (!f) cout << "-E- CbmAnaFlow::Draw_diffvn_Effcorr_1TreeLoop: pb with files containing Tree -----" << endl;

    Double_t corrFactor[2];
    corrFactor[0] = 1.;
    corrFactor[1] = 0.;
    if (fEP_meth != "RP" && fdoEPCorr == kTRUE) CalcCorrectionFactors(corrFactor);
    cout << "CbmAnaFlow::Create2Dhisto_Y_vs_pt_1TreeLoop: corrFactor = " << corrFactor[0] << " +/- " << corrFactor[1] << endl;

    hN_vs_y_pt = (TH2F*) f->Get("N_2D");
    hRap_vs_y_pt = (TH2F*) f->Get("y_2D");
    hpt_vs_y_pt = (TH2F*) f->Get("pt_2D");
    hvn_vs_y_pt = (TH2F*) f->Get("vn_2D");

    Int_t Nbin;
    Double_t min, max, step;
    Double_t y_tmp_bin, pt_tmp_bin;

    Double_t vn_tmp, vn_err_tmp;
    Double_t vn_tot, vn_err_tot;

    Double_t pt_tmp, pt_err_tmp;
    Double_t pt_tot, pt_err_tot;
    Double_t y_tmp, y_err_tmp;
    Double_t y_tot, y_err_tot;

    Double_t N_tmp, N_err_tmp;
    Double_t N_tot, N_err_tot;

    TAxis *hX = (TAxis*) hvn_vs_y_pt->GetXaxis();
    TAxis *hY = (TAxis*) hvn_vs_y_pt->GetYaxis();
    Int_t binX1, binX2, binY1, binY2;

    cout << "+++++++++++++++++++++++++++++++" << endl;
    cout << "+++++++++++++++++++++++++++++++" << endl;
    cout << "CbmAnaFlow::Draw_diffvn_Effcorr_1TreeLoop: pt dependence of v" << fsel_factor << " of '" << fsel_pdg << "' particles" << endl;

    binX1 = hX->FindBin(fsel_Ycut_min);
    binX2 = hX->FindBin(fsel_Ycut_max);

    Nbin = fsel_PTNbin;
    min = fsel_PTmin;
    max = fsel_PTmax;
    step = (fsel_PTmax - fsel_PTmin)/fsel_PTNbin;

    Float_t meanX0[Nbin], emeanX0[Nbin];
    Float_t meanFlow0[Nbin], emeanFlow0[Nbin];

    for (Int_t ipt=0; ipt<Nbin; ipt++)
    {
        //meanX0[ipt] = min + (ipt+0.5) * step;
	//emeanX0[ipt] = step/TMath::Sqrt(12);

        // Init
        vn_tmp = 0.;
	vn_err_tmp = 0.;

	vn_tot = 0.;
	vn_err_tot = 0.;

        N_tmp = 0.;
	N_err_tmp = 0.;

	N_tot = 0.;
	N_err_tot = 0.;
	
        pt_tmp = 0.;
	pt_err_tmp = 0.;

	pt_tot = 0.;
	pt_err_tot = 0.;

        y_tmp = 0.;
	y_err_tmp = 0.;

	y_tot = 0.;
	y_err_tot = 0.;

	pt_tmp_bin = min + ipt * step;
	binY1 = hY->FindBin(pt_tmp_bin);

	pt_tmp_bin = min + (ipt+1) * step;
	binY2 = hY->FindBin(pt_tmp_bin);

	for (Int_t iptpt=binY1; iptpt<=binY2; iptpt++)
	{

	    for (Int_t iy=binX1; iy<=binX2; iy++)
	    {
		vn_tmp = hvn_vs_y_pt->GetBinContent(iy, iptpt);
		vn_err_tmp = hvn_vs_y_pt->GetBinError(iy, iptpt);

		N_tmp = hN_vs_y_pt->GetBinContent(iy, iptpt);
		N_err_tmp = TMath::Sqrt(N_tmp);

		pt_tmp = hpt_vs_y_pt->GetBinContent(iy, iptpt);
		pt_err_tmp = hpt_vs_y_pt->GetBinError(iy, iptpt);

		y_tmp = hRap_vs_y_pt->GetBinContent(iy, iptpt);
		y_err_tmp = hRap_vs_y_pt->GetBinError(iy, iptpt);

		// vn integration

		//cout << "N_tmp (before eff. corr.) = " << N_tmp << " +/- " << N_err_tmp << endl;
		if (fdoEff == kTRUE && fuseTrackatGen == kFALSE) EffCorrection(iy, iptpt, N_tmp, N_err_tmp);
		//cout << "N_tmp (after eff. corr.) = " << N_tmp << " +/- " << N_err_tmp << endl;

		CalcErrorProp_prod(vn_tmp, vn_err_tmp, N_tmp, N_err_tmp);
		CalcErrorProp_prod(pt_tmp, pt_err_tmp, N_tmp, N_err_tmp);
		CalcErrorProp_prod(y_tmp, y_err_tmp, N_tmp, N_err_tmp);

		vn_tot += vn_tmp;
		vn_err_tot = TMath::Sqrt( TMath::Power(vn_err_tot, 2) + TMath::Power(vn_err_tmp, 2) );

		// yield integration
		N_tot += N_tmp;
		N_err_tot = TMath::Sqrt( TMath::Power(N_err_tot, 2) + TMath::Power(N_err_tmp, 2) );

		// pt integration
		pt_tot += pt_tmp;
		pt_err_tot = TMath::Sqrt( TMath::Power(pt_err_tot, 2) + TMath::Power(pt_err_tmp, 2) );

		// y integration
		y_tot += y_tmp;
		y_err_tot = TMath::Sqrt( TMath::Power(y_err_tot, 2) + TMath::Power(y_err_tmp, 2) );

	    }
	}

        CalcErrorProp_quot(vn_tot, vn_err_tot, N_tot, N_err_tot);
        CalcErrorProp_quot(pt_tot, pt_err_tot, N_tot, N_err_tot);
	CalcErrorProp_quot(y_tot, y_err_tot, N_tot, N_err_tot);

        // Correction factor for reaction plane resolution
        CalcErrorProp_quot(vn_tot, vn_err_tot, corrFactor[0], corrFactor[1]);

	meanFlow0[ipt] = vn_tot;
	emeanFlow0[ipt] = vn_err_tot;

        meanX0[ipt] = pt_tot;
	emeanX0[ipt] = pt_err_tot;

	cout << "<pt> = " << meanX0[ipt] << " +/- " << emeanX0[ipt] << ", v" + fsel_factor + " = " << meanFlow0[ipt] << " +/- " << emeanFlow0[ipt] << endl;
    }

    gr0 = new TGraphErrors(Nbin,meanX0,meanFlow0,emeanX0,emeanFlow0);
    gr0->SetName("gr0");
    gr0->SetMarkerColor(kBlack);
    gr0->SetMarkerStyle(25);
    gr0->SetLineColor(kBlack);

    cout << "+++++++++++++++++++++++++++++++" << endl;
    cout << "+++++++++++++++++++++++++++++++" << endl;
    cout << "CbmAnaFlow::Draw_diffvn_Effcorr_1TreeLoop: rapidity dependence of v" << fsel_factor << " of '" << fsel_pdg << "' particles" << endl;

    binY1 = hY->FindBin(fsel_PTcut_min);
    binY2 = hY->FindBin(fsel_PTcut_max);

    Nbin = fsel_YNbin;
    min = fsel_Ymin;
    max = fsel_Ymax;
    step = (fsel_Ymax - fsel_Ymin)/fsel_YNbin;

    Float_t meanX1[Nbin], emeanX1[Nbin];
    Float_t meanFlow1[Nbin], emeanFlow1[Nbin];

    for (Int_t iy=0; iy<Nbin; iy++)
    {
        //meanX1[iy] = min + (iy+0.5) * step;
	//emeanX1[iy] = step/TMath::Sqrt(12);

        // Init
        vn_tmp = 0.;
	vn_err_tmp = 0.;

	vn_tot = 0.;
	vn_err_tot = 0.;

        N_tmp = 0.;
	N_err_tmp = 0.;

	N_tot = 0.;
	N_err_tot = 0.;
	
        pt_tmp = 0.;
	pt_err_tmp = 0.;

	pt_tot = 0.;
	pt_err_tot = 0.;

        y_tmp = 0.;
	y_err_tmp = 0.;

	y_tot = 0.;
	y_err_tot = 0.;

	y_tmp_bin = min + iy * step;
	binX1 = hX->FindBin(y_tmp_bin);

	y_tmp_bin = min + (iy+1) * step;
	binX2 = hX->FindBin(y_tmp_bin);

	for (Int_t iyy=binX1; iyy<=binX2; iyy++)
	{
	    for (Int_t ipt=binY1; ipt<=binY2; ipt++)
	    {
		vn_tmp = hvn_vs_y_pt->GetBinContent(iyy, ipt);
		vn_err_tmp = hvn_vs_y_pt->GetBinError(iyy, ipt);

		N_tmp = hN_vs_y_pt->GetBinContent(iyy, ipt);
		N_err_tmp = TMath::Sqrt(N_tmp);

		pt_tmp = hpt_vs_y_pt->GetBinContent(iyy, ipt);
		pt_err_tmp = hpt_vs_y_pt->GetBinError(iyy, ipt);

		y_tmp = hRap_vs_y_pt->GetBinContent(iyy, ipt);     
		y_err_tmp = hRap_vs_y_pt->GetBinError(iyy, ipt);

		// vn integration

		//cout << "N_tmp (before eff. corr.) = " << N_tmp << " +/- " << N_err_tmp << endl;
		if (fdoEff == kTRUE && fuseTrackatGen == kFALSE) EffCorrection(iyy, ipt, N_tmp, N_err_tmp);
		//cout << "N_tmp (after eff. corr.) = " << N_tmp << " +/- " << N_err_tmp << endl;

		CalcErrorProp_prod(vn_tmp, vn_err_tmp, N_tmp, N_err_tmp);
                CalcErrorProp_prod(pt_tmp, pt_err_tmp, N_tmp, N_err_tmp);
		CalcErrorProp_prod(y_tmp, y_err_tmp, N_tmp, N_err_tmp);  

		vn_tot += vn_tmp;
		vn_err_tot = TMath::Sqrt( TMath::Power(vn_err_tot, 2) + TMath::Power(vn_err_tmp, 2) );

		// yield integration
		N_tot += N_tmp;
		N_err_tot = TMath::Sqrt( TMath::Power(N_err_tot, 2) + TMath::Power(N_err_tmp, 2) );

		// pt integration
		pt_tot += pt_tmp;
		pt_err_tot = TMath::Sqrt( TMath::Power(pt_err_tot, 2) + TMath::Power(pt_err_tmp, 2) );

		// y integration
		y_tot += y_tmp;
		y_err_tot = TMath::Sqrt( TMath::Power(y_err_tot, 2) + TMath::Power(y_err_tmp, 2) );
	    }
	}

        CalcErrorProp_quot(vn_tot, vn_err_tot, N_tot, N_err_tot);
        CalcErrorProp_quot(pt_tot, pt_err_tot, N_tot, N_err_tot);
	CalcErrorProp_quot(y_tot, y_err_tot, N_tot, N_err_tot);

        // Correction factor for reaction plane resolution
        CalcErrorProp_quot(vn_tot, vn_err_tot, corrFactor[0], corrFactor[1]);

	meanFlow1[iy] = vn_tot;
	emeanFlow1[iy] = vn_err_tot;

        meanX1[iy] = y_tot;                
	emeanX1[iy] = y_err_tot;

	cout << "<y> = " << meanX1[iy] << " +/- " << emeanX1[iy] << ", v" + fsel_factor + " = " << meanFlow1[iy] << " +/- " << emeanFlow1[iy] << endl;
    }

    gr1 = new TGraphErrors(Nbin,meanX1,meanFlow1,emeanX1,emeanFlow1);
    gr1->SetName("gr1");
    gr1->SetMarkerColor(kBlack);
    gr1->SetMarkerStyle(25);
    gr1->SetLineColor(kBlack);

    //======= write output to ascii
    Nbin = fsel_PTNbin;
    ofstream outtxtfile1(fileName_ascii_out_pt);
    if (outtxtfile1.is_open())
    {
	for (int i=0;i<Nbin;i++) outtxtfile1 << meanX0[i] << endl;
	outtxtfile1 << "" << endl;
	for (int i=0;i<Nbin;i++) outtxtfile1 << emeanX0[i] << endl;
	outtxtfile1 << "" << endl;
	for (int i=0;i<Nbin;i++) outtxtfile1 << meanFlow0[i] << endl;
	outtxtfile1 << "" << endl;
	for (int i=0;i<Nbin;i++) outtxtfile1 << emeanFlow0[i] << endl;
	outtxtfile1.close();
    }

    //======= write output to ascii
    Nbin = fsel_YNbin;
    ofstream outtxtfile2(fileName_ascii_out_y);
    if (outtxtfile2.is_open())
    {
	for (int i=0;i<Nbin;i++) outtxtfile2 << meanX1[i] << endl;
	outtxtfile2 << "" << endl;
	for (int i=0;i<Nbin;i++) outtxtfile2 << emeanX1[i] << endl;
	outtxtfile2 << "" << endl;
	for (int i=0;i<Nbin;i++) outtxtfile2 << meanFlow1[i] << endl;
	outtxtfile2 << "" << endl;
	for (int i=0;i<Nbin;i++) outtxtfile2 << emeanFlow1[i] << endl;
	outtxtfile2.close();
    }

    ///// DRAW

    TCanvas *c1 = new TCanvas("c1"," vn vs pt ",200,10,700,500);
    c1->SetFillColor(0);
    c1->SetGrid();
    //c1->Divide(2,1);
    //c1->cd(1);

    // draw a frame to define the range
    TH1F *hr = c1->DrawFrame(0.,-1.,3.,1.);
    hr->SetXTitle("Tranverse momentum p_{T} [Gev/c]");
    hr->GetXaxis()->CenterTitle();
    hr->SetYTitle("v_{" + fsel_factor + "}");
    hr->GetYaxis()->CenterTitle();
    gr0->Draw("P");

    //
    TCanvas *c2 = new TCanvas("c2"," vn vs y ",200,10,700,500);
    c2->SetFillColor(0);
    c2->SetGrid();
    //c1->cd(2);

    // draw a frame to define the range
    TH1F *hr2 = c2->DrawFrame(-2.,-1.,2.,1.);
    hr2->SetXTitle("Rapidity (c.m., normalized to Y_{proj})");
    hr2->GetXaxis()->CenterTitle();
    hr2->SetYTitle("v_{" + fsel_factor + "}");
    hr2->GetYaxis()->CenterTitle();
    gr1->Draw("P");

    return kTRUE;
}

bool CbmAnaFlow::Draw_intvn_Effcorr_1TreeLoop()
{
    cout << "CbmAnaFlow::Draw_intvn_Effcorr_1TreeLoop:  files containing Tree for flow analysis: " << fileName_tree << endl;

    TFile *f = new TFile(fileName_tree,"R");
    if (!f) cout << "-E- CbmAnaFlow::Draw_diffvn_Effcorr_1TreeLoop: pb with files containing Tree -----" << endl;

    Double_t corrFactor[2];
    corrFactor[0] = 1.;
    corrFactor[1] = 0.;
    if (fEP_meth != "RP" && fdoEPCorr == kTRUE) CalcCorrectionFactors(corrFactor);
    cout << "CbmAnaFlow::Create2Dhisto_Y_vs_pt_1TreeLoop: corrFactor = " << corrFactor[0] << " +/- " << corrFactor[1] << endl;

    hN_vs_y_pt = (TH2F*) f->Get("N_2D");
    hRap_vs_y_pt = (TH2F*) f->Get("y_2D");
    hpt_vs_y_pt = (TH2F*) f->Get("pt_2D");
    hvn_vs_y_pt = (TH2F*) f->Get("vn_2D");

    Double_t vn_tmp, vn_err_tmp;
    Double_t vn_tot, vn_err_tot;

    Double_t pt_tmp, pt_err_tmp;
    Double_t pt_tot, pt_err_tot;
    Double_t y_tmp, y_err_tmp;
    Double_t y_tot, y_err_tot;

    Double_t N_tmp, N_err_tmp;
    Double_t N_tot, N_err_tot;

    // Init
    vn_tmp = 0.;
    vn_err_tmp = 0.;

    vn_tot = 0.;
    vn_err_tot = 0.;

    N_tmp = 0.;
    N_err_tmp = 0.;

    N_tot = 0.;
    N_err_tot = 0.;

    pt_tmp = 0.;
    pt_err_tmp = 0.;

    pt_tot = 0.;
    pt_err_tot = 0.;

    y_tmp = 0.;
    y_err_tmp = 0.;

    y_tot = 0.;
    y_err_tot = 0.;

    TAxis *hX = (TAxis*) hvn_vs_y_pt->GetXaxis();
    TAxis *hY = (TAxis*) hvn_vs_y_pt->GetYaxis();
    Int_t binX1, binX2, binY1, binY2;

    binY1 = hY->FindBin(fsel_PTcut_min);
    binY2 = hY->FindBin(fsel_PTcut_max);

    binX1 = hX->FindBin(fsel_Ycut_min);
    binX2 = hX->FindBin(fsel_Ycut_max);

    for (Int_t ipt=binY1; ipt<=binY2; ipt++)
    {
	for (Int_t iy=binX1; iy<=binX2; iy++)
	{
	    vn_tmp = hvn_vs_y_pt->GetBinContent(iy, ipt);
	    vn_err_tmp = hvn_vs_y_pt->GetBinError(iy, ipt);

	    N_tmp = hN_vs_y_pt->GetBinContent(iy, ipt);
	    N_err_tmp = TMath::Sqrt(N_tmp);

	    pt_tmp = hpt_vs_y_pt->GetBinContent(iy, ipt);
	    pt_err_tmp = hpt_vs_y_pt->GetBinError(iy, ipt);

	    y_tmp = hRap_vs_y_pt->GetBinContent(iy, ipt);
	    y_err_tmp = hRap_vs_y_pt->GetBinError(iy, ipt);

	    // vn integration

	    //cout << "N_tmp (before eff. corr.) = " << N_tmp << " +/- " << N_err_tmp << endl;
	    if (fdoEff == kTRUE && fuseTrackatGen == kFALSE) EffCorrection(iy, ipt, N_tmp, N_err_tmp);
	    //cout << "N_tmp (after eff. corr.) = " << N_tmp << " +/- " << N_err_tmp << endl;

	    CalcErrorProp_prod(vn_tmp, vn_err_tmp, N_tmp, N_err_tmp);
	    CalcErrorProp_prod(pt_tmp, pt_err_tmp, N_tmp, N_err_tmp);
	    CalcErrorProp_prod(y_tmp, y_err_tmp, N_tmp, N_err_tmp);

	    vn_tot += vn_tmp;
	    vn_err_tot = TMath::Sqrt( TMath::Power(vn_err_tot, 2) + TMath::Power(vn_err_tmp, 2) );

	    // yield integration
	    N_tot += N_tmp;
	    N_err_tot = TMath::Sqrt( TMath::Power(N_err_tot, 2) + TMath::Power(N_err_tmp, 2) );

	    // pt integration
	    pt_tot += pt_tmp;
	    pt_err_tot = TMath::Sqrt( TMath::Power(pt_err_tot, 2) + TMath::Power(pt_err_tmp, 2) );

	    // y integration
	    y_tot += y_tmp;
	    y_err_tot = TMath::Sqrt( TMath::Power(y_err_tot, 2) + TMath::Power(y_err_tmp, 2) );
	}
    }

    CalcErrorProp_quot(vn_tot, vn_err_tot, N_tot, N_err_tot);
    CalcErrorProp_quot(pt_tot, pt_err_tot, N_tot, N_err_tot);
    CalcErrorProp_quot(y_tot, y_err_tot, N_tot, N_err_tot);

    // Correction factor for reaction plane resolution
    CalcErrorProp_quot(vn_tot, vn_err_tot, corrFactor[0], corrFactor[1]);

    cout << "Integrated v" << fsel_factor << " = " << vn_tot << " +/- " << vn_err_tot << " in collisions with (STS) multiplicity bwt " << ssel_Bmin << " and " << ssel_Bmax << "." << endl;

    return kTRUE;
}

//////////////////////////////////////////////////
////////////////////////// ======= Small functions
//////////////////////////////////////////////////

void CbmAnaFlow::projRapidityCM()
{
    // Calcul rapidity of c.m. & projectile (in c.m.) to calculate normalized particle rapidity in c.m.
    
    if (fEn == 0.)
    {
      LOG(FATAL) << "-E- CbmAnaFlow::projRapidityCM: Please specify beam kinetic energy (per nucleon)" << FairLogger::endl;
      //return kERROR;
    }
    
    const Double_t kProtonMass = 0.938271998;
    Double_t energy_proj = Double_t(fEn) + kProtonMass;
    Double_t mom_proj = sqrt(energy_proj*energy_proj - kProtonMass*kProtonMass);
    Double_t y_proj = 0.5*log((energy_proj+mom_proj)/(energy_proj-mom_proj));
    fy_cm = 0.5*y_proj;
    fy_proj_cm = y_proj - fy_cm;
}

Double_t CbmAnaFlow::Range(Double_t phi)
{
    if (phi < -fPi) phi += 2*fPi;
    if (phi >  fPi) phi -= 2*fPi;   
    return phi;
}

Double_t CbmAnaFlow::Range2nd(Double_t phi)
{
    if (phi < -fPi/2.) phi += fPi;
    if (phi >  fPi/2.) phi -= fPi;
    return phi;
}

// TO DO: make special function without cast into float into double into float again ... ?

void CbmAnaFlow::CalcFlowCoef_cosnphi(TTree *&t, TString &ALLcut, Float_t* meanFlow, Float_t* emeanFlow, Int_t &bin, Double_t &N)
{
    TH1F* h;
    
    N = (double) t->Draw("TMath::Cos(" + fsel_factor + "*fphi_to_" + fEP_meth + ")", ALLcut, "goff");
    h = (TH1F*) t->GetHistogram();
    meanFlow[bin] = h->GetMean();
    emeanFlow[bin] = h->GetMeanError();

    //h->Delete();
}

void CbmAnaFlow::CalcFlowCoef_phifit(TTree *&t, TString &ALLcut, Float_t* meanFlow, Float_t* emeanFlow, Int_t &bin, Double_t &N)
{
    Int_t param;
    if ( fsel_factor == "1" ) param = 1;
    if ( fsel_factor == "2" ) param = 2;

    TH1F* h;
    TF1* fFourier = new TF1("Fourier","[0] * ( 1 + 2*[1]*cos(x) + 2*[2]*cos(2*x) + 2*[3]*cos(3*x) + 2*[4]*cos(4*x) + 2*[5]*cos(5*x) )",-fPi, fPi);
    TF1* funct;

    N = (double) t->Draw("(fphi_to_" + fEP_meth + ")", ALLcut, "goff");
    h = (TH1F*) t->GetHistogram();

    if (N != 0)
    {
	h->Fit("Fourier","","",-fPi, fPi);
	funct = (TF1*) h->GetFunction("Fourier");

	meanFlow[bin] = funct->GetParameter(param);
	emeanFlow[bin] = funct->GetParError(param);
    }
    else
    {
        meanFlow[bin] = 0.;
	emeanFlow[bin] = 0.;
    }

    //h->Delete();
}

void CbmAnaFlow::CalcMeanPt(TTree *&t, TString &ALLcut, Float_t* meanX, Float_t* emeanX, Int_t &bin)
{
    TH1F* h;

    t->Draw("fpt", ALLcut, "goff");
    h = (TH1F*) t->GetHistogram();

    meanX[bin] = h->GetMean();
    emeanX[bin] = h->GetMeanError();

    //h->Delete();
}

void CbmAnaFlow::CalcMeanY(TTree *&t, TString &ALLcut, Float_t* meanX, Float_t* emeanX, Int_t &bin)
{
    TH1F* h;

    t->Draw("fY", ALLcut, "goff");
    h = (TH1F*) t->GetHistogram();

    meanX[bin] = h->GetMean();
    emeanX[bin] = h->GetMeanError();

    //h->Delete();
}

void CbmAnaFlow::EffCorrection(Int_t &binX, Int_t &binY, Double_t &N, Double_t &N_err)
{
    double eff, eff_err;

    eff = hEff_vs_y_pt->GetBinContent(binX, binY);
    eff_err = hEff_vs_y_pt->GetBinError(binX, binY);

    //cout << "eff in bin [" << binX << ", " << binY << "] = " << eff << " +/- " << eff_err << endl;

    if (eff != 0. && N!= 0.)
    {
	N_err = TMath::Abs(N/eff) * TMath::Sqrt( (N_err/N)*(N_err/N) + (eff_err/eff)*(eff_err/eff) );
	N /= eff;
    }
    else
    {
       //cout << "reco. eff. null, N should be 0. N = " << N << " +/- " << N_err << endl;
        N = 0.;
        N_err = 0.;
    }
}

// ========================== small function for high stat studies (1 Loop TTree)

void CbmAnaFlow::CalcMeanY_1TreeLoop(Int_t &binX, Int_t &binY) 
{
    Double_t y;
    Double_t y_err;

    y = hRapidity->GetMean();
    y_err = hRapidity->GetMeanError();

    //cout << "===============================" << endl;
    //cout << "CbmAnaFlow::CalcMeanY_1TreeLoop: <y> = " << y << " +/- " << y_err << endl;
    //cout << "CbmAnaFlow::CalcMeanY_1TreeLoop: in bin (X-axis) = " << binX << endl;

    hRap_vs_y_pt->SetBinContent(binX, binY, y);
    hRap_vs_y_pt->SetBinError(binX, binY, y_err);
}

void CbmAnaFlow::CalcMeanPt_1TreeLoop(Int_t &binX, Int_t &binY)
{
    Double_t pt;
    Double_t pt_err;

    pt = hpt->GetMean();
    pt_err = hpt->GetMeanError();

    //cout << "===============================" << endl;
    //cout << "CbmAnaFlow::CalcMeanPt_1TreeLoop: <pt> = " << pt << " +/- " << pt_err << endl;
    //cout << "CbmAnaFlow::CalcMeanPt_1TreeLoop: in bin (Y-axis) = " << binY << endl;

    hpt_vs_y_pt->SetBinContent(binX, binY, pt);
    hpt_vs_y_pt->SetBinError(binX, binY, pt_err);
}

void CbmAnaFlow::CalcFlowCoef_cosnphi_1TreeLoop(Int_t &binX, Int_t &binY)
{
    Double_t vn;
    Double_t vn_err;
    Double_t N;
    Double_t N_err;

    vn = hcosnphi->GetMean();
    vn_err = hcosnphi->GetMeanError();

    hvn_vs_y_pt->SetBinContent(binX, binY, vn);
    hvn_vs_y_pt->SetBinError(binX, binY, vn_err);

    N = hcosnphi->GetEntries();
    N_err = TMath::Sqrt(N);

    hN_vs_y_pt->SetBinContent(binX, binY, N);
    hN_vs_y_pt->SetBinError(binX, binY, N_err);

    //cout << "===============================" << endl;
    //cout << "CbmAnaFlow::CalcFlowCoef_cosnphi_1TreeLoop: vn = " << vn << " +/- " << vn_err << ", with stat = " << N << " +/- " << N_err << endl;
    //cout << "CbmAnaFlow::CalcFlowCoef_cosnphi_1TreeLoop: in bin (X, Y) = (" << binX << ", " << binY << ")" << endl;
}

void CbmAnaFlow::CalcFlowCoef_phifit_1TreeLoop(Int_t &binX, Int_t &binY)
{
    Int_t param;
    if ( fsel_factor == "1" ) param = 1;
    if ( fsel_factor == "2" ) param = 2;

    Double_t vn;
    Double_t vn_err;
    Double_t N;
    Double_t N_err;

    N = hphi->GetEntries();
    N_err = TMath::Sqrt(N);

    hN_vs_y_pt->SetBinContent(binX, binY, N);
    hN_vs_y_pt->SetBinError(binX, binY, N_err);

    TF1* fFourier = new TF1("Fourier","[0] * ( 1 + 2*[1]*cos(x) + 2*[2]*cos(2*x) + 2*[3]*cos(3*x) + 2*[4]*cos(4*x) + 2*[5]*cos(5*x) )",-fPi, fPi);
    TF1* funct;

    if (N != 0)
    {
	hphi->Fit("Fourier","","",-fPi, fPi);
	funct = (TF1*) hphi->GetFunction("Fourier");

	vn = funct->GetParameter(param);
	vn_err = funct->GetParError(param);
    }
    else
    {
	vn = 0.;
	vn_err = 0.;
    }

    hvn_vs_y_pt->SetBinContent(binX, binY, vn);
    hvn_vs_y_pt->SetBinError(binX, binY, vn_err);

    //cout << "===============================" << endl;
    //cout << "CbmAnaFlow::CalcFlowCoef_phifit_1TreeLoop: vn = " << vn << " +/- " << vn_err << ", with stat = " << N << " +/- " << N_err << endl;
    //cout << "CbmAnaFlow::CalcFlowCoef_phifit_1TreeLoop: in bin (X, Y) = (" << binX << ", " << binY << ")" << endl;
}

void CbmAnaFlow::CalcErrorProp_prod(Double_t &N1, Double_t &N_err1, Double_t &N2, Double_t &N_err2)
{
    if (N1 != 0. && N2 != 0.)
    {
	N_err1 = TMath::Abs(N1*N2) * TMath::Sqrt( (N_err1/N1)*(N_err1/N1) + (N_err2/N2)*(N_err2/N2) );
	N1 *= N2;
    }
    else
    {
	N_err1 = 0.;
	N1 = 0.;
    }
}

void CbmAnaFlow::CalcErrorProp_quot(Double_t &N1, Double_t &N_err1, Double_t &N2, Double_t &N_err2)
{
    if (N1 != 0. && N2 != 0.)
    {
	N_err1 = TMath::Abs(N1/N2) * TMath::Sqrt( (N_err1/N1)*(N_err1/N1) + (N_err2/N2)*(N_err2/N2) );
	N1 /= N2;
    }
    else
    {
	N_err1 = 0.;
	N1 = 0.;
    }
}

void CbmAnaFlow::CalcErrorProp_quot(Float_t &N1, Float_t &N_err1, Double_t &N2, Double_t &N_err2)
{
    if (N1 != 0. && N2 != 0.)
    {
	N_err1 = TMath::Abs(N1/N2) * TMath::Sqrt( (N_err1/N1)*(N_err1/N1) + (N_err2/N2)*(N_err2/N2) );
	N1 /= N2;
    }
    else
    {
	N_err1 = 0.;
	N1 = 0.;
    }
}

void CbmAnaFlow::CalcCorrectionFactors(Double_t* corrFactor)
{  
  TFile *f = new TFile(fileName_EPcorrFactor,"R");
  if (!f) cout << "-E- CbmAnaFlow::CalcCorrectionFactors: pb with file containing Tree -----" << endl;
  
  TTree* t = (TTree* ) f->Get("cbmsim");  
  if (!t) cout << "-E- CbmAnaFlow::CalcCorrectionFactors: pb with Tree in file -----" << endl;

  cout << "CbmAnaFlow::CalcCorrectionFactors: correction factor for (STS) multiplicity bwt: [" << ssel_Bmin << ", " << ssel_Bmax << "]" << endl;

  //TString sCentrality = "McEvent.fB >= " + ssel_Bmin + " && McEvent.fB < " + ssel_Bmax;
  TString sCentrality = "StsEvent.fmult >= " + ssel_Bmin + " && StsEvent.fmult < " + ssel_Bmax;

  if (fuseSubevent == kFALSE)
  {
      TString EP_meth;
    
      if (fEP_meth == "EP_PSD") EP_meth = "PsdEvent.fEP_RECO";
      if (fEP_meth == "EP_PSD1") EP_meth = "PsdEvent.fEP_sub1";
      if (fEP_meth == "EP_PSD2") EP_meth = "PsdEvent.fEP_sub2";
      if (fEP_meth == "EP_PSD3") EP_meth = "PsdEvent.fEP_sub3";

      if (fEP_meth == "EP_STS_harmo1") EP_meth = "StsEvent.fEP_RECO_harmo1_full";
      if (fEP_meth == "EP_STS_harmo2") EP_meth = "StsEvent.fEP_RECO_harmo2_full";  
      
      t->Draw("TMath::Cos(" + fsel_factor + " * (" + EP_meth + " - McEvent.fRP) )>>h2",sCentrality, "goff");  
      
      TH1F* h = (TH1F*) t->GetHistogram();
      corrFactor[0] = h->GetMean();
      corrFactor[1] = h->GetMeanError();
  }
  else
  {
      if (fEP_meth == "EP_PSD" || fEP_meth == "EP_PSD1" || fEP_meth == "EP_PSD2" || fEP_meth == "EP_PSD3")
      {
	  TString term_1, term_2, term_3;

	  if (fEP_meth == "EP_PSD")
	  {
	      cout << "WARNING: you use Phi_part - EP_PSD ... better to use one of PSD sub-event EP's for sub-event method" << endl;

	      term_1 = "PsdEvent.fEP_RECO - StsEvent.fEP_RECO_harmo1_sub1";
	      term_2 = "PsdEvent.fEP_RECO - StsEvent.fEP_RECO_harmo1_sub2";
	      term_3 = "StsEvent.fEP_RECO_harmo1_sub1 - StsEvent.fEP_RECO_harmo1_sub2";
	  }

	  if (fEP_meth == "EP_PSD1")
	  {
	      term_1 = "PsdEvent.fEP_sub1 - PsdEvent.fEP_sub2";
	      term_2 = "PsdEvent.fEP_sub1 - StsEvent.fEP_RECO_harmo1_full";
	      term_3 = "PsdEvent.fEP_sub2 - StsEvent.fEP_RECO_harmo1_full";
	  }

	  if (fEP_meth == "EP_PSD2")
	  {
	      term_1 = "PsdEvent.fEP_sub2 - PsdEvent.fEP_sub3";
	      term_2 = "PsdEvent.fEP_sub2 - StsEvent.fEP_RECO_harmo1_full";
	      term_3 = "PsdEvent.fEP_sub3 - StsEvent.fEP_RECO_harmo1_full";
	  }

	  if (fEP_meth == "EP_PSD3")
	  {
	      term_1 = "PsdEvent.fEP_sub3 - PsdEvent.fEP_sub2";
	      term_2 = "PsdEvent.fEP_sub3 - StsEvent.fEP_RECO_harmo1_full";
	      term_3 = "PsdEvent.fEP_sub2 - StsEvent.fEP_RECO_harmo1_full";
	  }

	  t->Draw("TMath::Cos(" + fsel_factor + " * (" + term_1 + ") )>>h1",sCentrality, "goff");
	  TH1F* h1 = (TH1F*) t->GetHistogram();
	  
	  t->Draw("TMath::Cos(" + fsel_factor + " * (" + term_2 + ") )>>h2",sCentrality, "goff");
	  TH1F* h2 = (TH1F*) t->GetHistogram();
	  
	  t->Draw("TMath::Cos(" + fsel_factor + " * (" + term_3 + ") )>>h3",sCentrality, "goff");
	  TH1F* h3 = (TH1F*) t->GetHistogram();
	  
	  corrFactor[0] = TMath::Sqrt( ( h1->GetMean() * h2->GetMean()  ) / h3->GetMean() );
	  
	  // error
	  Double_t A, sA, B, sB, F, sF;
	  Double_t C, sC, F2, sF2, F3, sF3;

	  A = h1->GetMean();
	  sA = h1->GetMeanError();
	  B = h2->GetMean();
	  sB = h2->GetMeanError();
	  C = h3->GetMean();
	  sC = h3->GetMeanError();

	  //A = TMath::Abs(A);   // TO DO?: NOT sure it is mathematically correct ... only for very poor resolutions
	  //B = TMath::Abs(B);
	  //C = TMath::Abs(C);

	  F = A * B;
	  sF = F * TMath::Sqrt(TMath::Power(sA/A, 2) + TMath::Power(sB/B, 2));
	  F2 = F / C;
	  sF2 = F2 * TMath::Sqrt(TMath::Power(sF/F, 2) + TMath::Power(sC/C, 2));
	  sF3 = sF2 / (2 * TMath::Sqrt(F2));
	  
	  corrFactor[1] = sF3;
      }
      
      if (fEP_meth == "EP_STS_harmo1") 
      {
	TString term_1 = "StsEvent.fEP_RECO_harmo1_sub1 - StsEvent.fEP_RECO_harmo1_sub2";
	
	t->Draw("TMath::Cos(" + fsel_factor + " * (" + term_1 + ") )>>h",sCentrality, "goff");
	TH1F* h = (TH1F*) t->GetHistogram();
	
	corrFactor[0] = sqrt(2) * sqrt(TMath::Abs(h->GetMean()));
	corrFactor[1] = sqrt(TMath::Abs(1/(2 * h->GetMean()))) * h->GetMeanError();
      
      }
      
      if (fEP_meth == "EP_STS_harmo2") 
      {
	TString term_1 = "StsEvent.fEP_RECO_harmo2_sub1 - StsEvent.fEP_RECO_harmo2_sub2";
	
	t->Draw("TMath::Cos(" + fsel_factor + " * (" + term_1 + ") )>>h",sCentrality, "goff");
	TH1F* h = (TH1F*) t->GetHistogram();
	
	corrFactor[0] = sqrt(2) * sqrt(TMath::Abs(h->GetMean()));
	corrFactor[1] = sqrt(TMath::Abs(1/(2 * h->GetMean()))) * h->GetMeanError();      
      }
  }

  //t->Delete();
  //f->Close();
}


void CbmAnaFlow::Finish()
{
    // Finish of the task execution
    Write(); 
}

void CbmAnaFlow::Write()
{
    if (fmode == 0 || fmode == 1 || fmode == 2)
    {
	outTree->Write("",TObject::kOverwrite);
	hmult->Write();
    }

    if (fmode == 4 || fmode == 5 || fmode == 6)
    {
	cout << "output file name : " << fileName_out << endl;
	TFile *outfile = new TFile(fileName_out,"RECREATE");

	hpt_vs_y_pt->Write();
	hRap_vs_y_pt->Write();
	hvn_vs_y_pt->Write();
	hN_vs_y_pt->Write();
	if (fmode == 4) hEff_vs_y_pt->Write();
    }

    if (fmode == 7)
    {
	cout << "output file name : " << fileName_out << endl;
	TFile *outfile = new TFile(fileName_out,"RECREATE");

	hEff_vs_y_pt->Write();
    }
}
