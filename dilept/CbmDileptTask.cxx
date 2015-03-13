//*******************************************//
//***                                     ***//
//***        CbmDiLeptTask.cxx            ***//
//***           T.Galatyuk                ***//
//***                                     ***//
//*******************************************//

#include "CbmDileptTask.h"

#include "CbmMCTrack.h"
#include "CbmStsPoint.h"
#include "CbmRichPoint.h"
#include "FairRootManager.h"
#include "FairPrimaryGenerator.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TObjArray.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TH2F.h"
#include "TF1.h"
#include "TMath.h"
#include "TRandom.h"
#include "TLorentzVector.h"

#include <iostream>
#include <map>

#define M2E 2.6112004954086e-7



ClassImp(CbmDiLeptTask);



TString fDaughters[] = {"ep", "em"};
TString fNames[] = {"omega", "eta", "gamma", "pi0", "omega_bg", "bg"};
TString fNames_neighb[] = {"omega_neighb", "eta_neighb", "gamma_neighb", "pi0_neighb"};



CbmDiLeptTask::CbmDiLeptTask()
  : FairTask("DiLeptTask")
{
  // Set default values
  fArrayMCTrack   = NULL;
  fArrayStsPoint  = NULL;
  fArrayRichPoint = NULL;
  
  fEvents = 0;
  fCoutBunch = 1;
  
  fGcut   = 0.02;    // Invariant mass for gamma cut (GeV/c^2)
  fDcut   = 0.1;     // Distance to the closest neighbour (cm)
  f2Dtcut = 1.5;     // Opening angle between closest neighbour (GeV/c)
  f2Dpcut = 1.5;     // Sqrt(p*p) of closest neighbour (GeV/c^2)
  fPtcut  = 0.1;     // Transverse momentum (GeV/c)
  fTcut   = 2.;      // Opening angle (deg)
  fPi0cut = 0.15;    // Invariant mass of the pi0-Dalitz decay

  fWeight = 1.;
}



CbmDiLeptTask::CbmDiLeptTask(const char *name)
  : FairTask(name)
{

  fArrayMCTrack   = NULL;
  fArrayStsPoint  = NULL;
  fArrayRichPoint = NULL;

  fEvents = 0;
  fCoutBunch = 1;

  fGcut   = 0.02;
  fDcut   = 0.1;
  f2Dtcut = 1.5;
  f2Dpcut = 1.5;
  fPtcut  = 0.1;
  fTcut   = 2.;
  fPi0cut = 0.15;

  fWeight = 1.;
}




CbmDiLeptTask::~CbmDiLeptTask()
{

}



// ============================================================================
void CbmDiLeptTask::CreateHistogramms()
{
  // =================================================================
  for(Int_t iTypeN = 0; iTypeN < NTYPES_NEIGHB; iTypeN++) {

    TString acc_d = "h_acc_" + fNames_neighb[iTypeN] + "_d";
    fh_acc_d[iTypeN] = new TH1D(acc_d, "",
				1000, 0., 1.);

    TString dcut_theta_minv = "h_dcut_" + fNames_neighb[iTypeN] + "_theta_minv";
    fh_dcut_theta_minv[iTypeN] = new TH2D(dcut_theta_minv, "",
					  1000, 0., 2.,
					  900, 0., 180.);
    TString dcut_theta_sqrt_mom = "h_dcut_" + fNames_neighb[iTypeN] + "_theta_sqrt_mom";
    fh_dcut_theta_sqrt_mom[iTypeN] = new TH2D(dcut_theta_sqrt_mom, "",
					      1000, 0., 10.,
					      900, 0., 180.);
  }
  // =================================================================



  // =================================================================
  // Loop over types
  for(Int_t iType = 0; iType < NTYPES; iType++) {

    // ----------------------------------------------------------
    for(Int_t iD = 0; iD < 2; iD++) {
      TString all_zr = "h_all_" + fNames[iType] + "_" +
	fDaughters[iD] + "_zr";
      fh_all_zr_lept[iType][iD] = new TH2D(all_zr, "",
					   400, 0., 400.,
					   400, -200., 200.);

      TString all_mom = "h_all_" + fNames[iType] + "_" +
	fDaughters[iD] + "_mom";
      fh_all_mom_lept[iType][iD] = new TH1D(all_mom, "",
					    1000, 0., 10.);
      TString acc_mom = "h_acc_" + fNames[iType] + "_" +
	fDaughters[iD] + "_mom";
      fh_acc_mom_lept[iType][iD] = new TH1D(acc_mom, "",
					    1000, 0., 10.);

      TString all_pt = "h_all_" + fNames[iType] + "_" +
	fDaughters[iD] + "_pt";
      fh_all_pt_lept[iType][iD] = new TH1D(all_pt, "",
					   1000, 0., 10.);
      TString acc_pt = "h_acc_" + fNames[iType] + "_" +
	fDaughters[iD] + "_pt";
      fh_acc_pt_lept[iType][iD] = new TH1D(acc_pt, "",
					   1000, 0., 10.);
      TString tpcut_pt = "h_tpcut_" + fNames[iType] + "_" +
	fDaughters[iD] + "_pt";
      fh_tpcut_pt_lept[iType][iD] = new TH1D(tpcut_pt, "",
					     1000, 0., 10.);

      TString all_pty_lept = "h_all_pty_lept_" + fNames[iType] + "_" +
	fDaughters[iD];
      fh_all_pty_lept[iType][iD] = new TH2D(all_pty_lept, "",
					    40, 0., 4.,
					    20, 0., 2.);
      TString acc_pty_lept = "h_acc_pty_lept_" + fNames[iType] + "_" +
	fDaughters[iD];
      fh_acc_pty_lept[iType][iD] = new TH2D(acc_pty_lept, "",
					    40, 0., 4.,
					    20, 0., 2.);
    }
    // ----------------------------------------------------------


    // ----------------------------------------------------------
    for(Int_t iStation = 0; iStation < 7; iStation++) {

      TString str;
      str += iStation;
      TString sts_ep_mom = "h_sts" + str + "_" +
	fNames[iType] +	"_ep_mom";
      fh_sts_ep_mom[iStation][iType] = new TH1D(sts_ep_mom, "",
						1000, 0., 10.);
    }
    // ----------------------------------------------------------


    TString all_nsts = "h_all_nsts_" + fNames[iType];
    fh_all_nsts[iType] = new TH2D(all_nsts, "",
				  7, 0., 7.,
				  7, 0., 7.);

    TString all_pty = "h_all_pty_" + fNames[iType];
    fh_all_pty[iType] = new TH2D(all_pty, "",
				 40, 0., 4.,
				 20, 0., 2.);
    TString acc_pty = "h_acc_pty_" + fNames[iType];
    fh_acc_pty[iType] = new TH2D(acc_pty, "",
				 40, 0., 4.,
				 20, 0., 2.);
    TString pi0cut_pty = "h_pi0cut_pty_" + fNames[iType];
    fh_pi0cut_pty[iType] = new TH2D(pi0cut_pty, "",
				    40, 0., 4.,
				    20, 0., 2.);

    TString all_epem_mom = "h_all_" + fNames[iType] + "_epem_mom";
    fh_all_epem_mom[iType] = new TH2D(all_epem_mom, "",
				      100, 0., 10.,
				      100, 0., 10.);
    TString acc_epem_mom = "h_acc_" + fNames[iType] + "_epem_mom";
    fh_acc_epem_mom[iType] = new TH2D(acc_epem_mom, "",
				      100, 0., 10.,
				      100, 0., 10.);

    TString all_mom = "h_all_" + fNames[iType] + "_mom";
    fh_all_mom[iType] = new TH1D(all_mom, "",
				 1000, 0., 10.);
    TString acc_mom = "h_acc_" + fNames[iType] + "_mom";
    fh_acc_mom[iType] = new TH1D(acc_mom, "",
				 1000, 0., 10.);

    TString all_theta = "h_all_" + fNames[iType] + "_theta";
    fh_all_theta[iType] = new TH1D(all_theta, "",
				   1800, 0., 180.);
    TString acc_theta = "h_acc_" + fNames[iType] + "_theta";
    fh_acc_theta[iType] = new TH1D(acc_theta, "",
				   1800, 0., 180.);
    TString ptcut_theta = "h_ptcut_" + fNames[iType] + "_theta";
    fh_ptcut_theta[iType] = new TH1D(ptcut_theta, "",
				     1800, 0., 180.);

    // theta vs. sqrt mom for signal pairs
    TString dcut_theta_sqrt_mom_true = "h_dcut_" + fNames[iType] + "_theta_sqrt_mom_true";
    fh_dcut_theta_sqrt_mom_true[iType] = new TH2D(dcut_theta_sqrt_mom_true, "",
						  1000, 0., 10.,
						  900, 0., 180.);

    // lepton multiplicities
    TString all_mult = "h_all_" + fNames[iType] + "_mult";
    fh_all_mult[iType] = new TH2D(all_mult, "",
				  100, 0., 100.,
				  100, 0., 100.);
    TString acc_mult = "h_acc_" + fNames[iType] + "_mult";
    fh_acc_mult[iType] = new TH2D(acc_mult, "",
				  100, 0., 100.,
				  100, 0., 100.);
    TString gcut_mult = "h_gcut_" + fNames[iType] + "_mult";
    fh_gcut_mult[iType] = new TH2D(gcut_mult, "",
				   100, 0., 100.,
				   100, 0., 100.);
    TString dcut_mult = "h_dcut_" + fNames[iType] + "_mult";
    fh_dcut_mult[iType] = new TH2D(dcut_mult, "",
				   100, 0., 100.,
				   100, 0., 100.);
    TString tpcut_mult = "h_tpcut_" + fNames[iType] + "_mult";
    fh_tpcut_mult[iType] = new TH2D(tpcut_mult, "",
				    100, 0., 100.,
				    100, 0., 100.);
    TString ptcut_mult = "h_ptcut_" + fNames[iType] + "_mult";
    fh_ptcut_mult[iType] = new TH2D(ptcut_mult, "",
				    100, 0., 100.,
				    100, 0., 100.);
    TString tcut_mult = "h_tcut_" + fNames[iType] + "_mult";
    fh_tcut_mult[iType] = new TH2D(tcut_mult, "",
				   100, 0., 100.,
				   100, 0., 100.);
    TString pi0cut_mult = "h_pi0cut_" + fNames[iType] + "_mult";
    fh_pi0cut_mult[iType] = new TH2D(pi0cut_mult, "",
				     100, 0., 100.,
				     100, 0., 100.);

    // invariant mass
    TString all_minv = "h_all_" + fNames[iType] + "_minv";
    fh_all_minv[iType] = new TH1D(all_minv, "",
				  2000, 0., 2.);
    TString acc_minv = "h_acc_" + fNames[iType] + "_minv";
    fh_acc_minv[iType] = new TH1D(acc_minv, "",
				  2000, 0., 2.);
    TString gcut_minv = "h_gcut_" + fNames[iType] + "_minv";
    fh_gcut_minv[iType] = new TH1D(gcut_minv, "",
				   2000, 0., 2.);
    TString dcut_minv = "h_dcut_" + fNames[iType] + "_minv";
    fh_dcut_minv[iType] = new TH1D(dcut_minv, "",
				   2000, 0., 2.);
    TString tpcut_minv = "h_tpcut_" + fNames[iType] + "_minv";
    fh_tpcut_minv[iType] = new TH1D(tpcut_minv, "",
				    2000, 0., 2.);
    TString ptcut_minv = "h_ptcut_" + fNames[iType] + "_minv";
    fh_ptcut_minv[iType] = new TH1D(ptcut_minv, "",
				    2000, 0., 2.);
    TString tcut_minv = "h_tcut_" + fNames[iType] + "_minv";
    fh_tcut_minv[iType] = new TH1D(tcut_minv, "",
				   2000, 0., 2.);
    TString pi0cut_minv = "h_pi0cut_" + fNames[iType] + "_minv";
    fh_pi0cut_minv[iType] = new TH1D(pi0cut_minv, "",
				     2000, 0., 2.);

  }
  // =================================================================


  // d sts cocktail
  fh_acc_d_gamma_gamma = new TH1D("h_acc_d_gamma_gamma", "",
				  1000, 0., 1.);
  fh_acc_d_pi0_pi0 = new TH1D("h_acc_d_pi0_pi0", "",
			      1000, 0., 1.);

  // momentum resolution
  fh_dp = new TH1D("h_dp", "", 100, -0.05, 0.05);

  // like-sign paitrs
  fh_acc_epep_minv = new TH1D("h_acc_epep_minv", "",
			      2000, 0., 2.);
  fh_acc_emem_minv = new TH1D("h_acc_emem_minv", "",
			      2000, 0., 2.);
  fh_gcut_epep_minv = new TH1D("h_gcut_epep_minv", "",
			       2000, 0., 2.);
  fh_gcut_emem_minv = new TH1D("h_gcut_emem_minv", "",
			       2000, 0., 2.);
  fh_dcut_epep_minv = new TH1D("h_dcut_epep_minv", "",
			       2000, 0., 2.);
  fh_dcut_emem_minv = new TH1D("h_dcut_emem_minv", "",
			       2000, 0., 2.);
  fh_tpcut_epep_minv = new TH1D("h_tpcut_epep_minv", "",
				2000, 0., 2.);
  fh_tpcut_emem_minv = new TH1D("h_tpcut_emem_minv", "",
				2000, 0., 2.);
  fh_ptcut_epep_minv = new TH1D("h_ptcut_epep_minv", "",
				2000, 0., 2.);
  fh_ptcut_emem_minv = new TH1D("h_ptcut_emem_minv", "",
				2000, 0., 2.);
  fh_tcut_epep_minv = new TH1D("h_tcut_epep_minv", "",
			       2000, 0., 2.);
  fh_tcut_emem_minv = new TH1D("h_tcut_emem_minv", "",
			       2000, 0., 2.);
  fh_pi0cut_epep_minv = new TH1D("h_pi0cut_epep_minv", "",
				 2000, 0., 2.);
  fh_pi0cut_emem_minv = new TH1D("h_pi0cut_emem_minv", "",
				 2000, 0., 2.);

  // background cocktail
  fh_pi0cut_gamma_pi0_minv = new TH1D("h_pi0cut_gamma_pi0_minv", "",
				      2000, 0., 2.);
  fh_pi0cut_gamma_omega_minv = new TH1D("h_pi0cut_gamma_omega_minv", "",
					2000, 0., 2.);
  fh_pi0cut_gamma_eta_minv = new TH1D("h_pi0cut_gamma_eta_minv", "",
				      2000, 0., 2.);
  fh_pi0cut_gamma_gamma_minv = new TH1D("h_pi0cut_gamma_gamma_minv", "",
					2000, 0., 2.);
  fh_pi0cut_pi0_omega_minv = new TH1D("h_pi0cut_pi0_omega_minv", "",
				      2000, 0., 2.);
  fh_pi0cut_pi0_eta_minv = new TH1D("h_pi0cut_pi0_eta_minv", "",
				    2000, 0., 2.);
  fh_pi0cut_pi0_pi0_minv = new TH1D("h_pi0cut_pi0_pi0_minv", "",
				    2000, 0., 2.);
  fh_pi0cut_eta_omega_minv = new TH1D("h_pi0cut_eta_omega_minv", "",
				      2000, 0., 2.);
  fh_pi0cut_eta_eta_minv = new TH1D("h_pi0cut_eta_eta_minv", "",
				    2000, 0., 2.);

  // pair acceptance
  fh_all_pt_minv_pairs = new TH2D("h_all_pt_minv_pairs", "",
				  100, 0., 2.,
				  200, 0., 4.);
  fh_all_pt_minv_omega = new TH2D("h_all_pt_minv_omega", "",
				  100, 0., 2.,
				  200, 0., 4.);
  fh_acc_pt_minv_pairs = new TH2D("h_acc_pt_minv_pairs", "",
				  100, 0., 2.,
				  200, 0., 4.);
  fh_acc_pt_minv_omega = new TH2D("h_acc_pt_minv_omega", "",
				  100, 0., 2.,
				  200, 0., 4.);
  fh_pi0cut_pt_minv_pairs = new TH2D("h_pi0cut_pt_minv_pairs", "",
				     100, 0., 2.,
				     200, 0., 4.);
  fh_pi0cut_pt_minv_omega = new TH2D("h_pi0cut_pt_minv_omega", "",
				     100, 0., 2.,
				     200, 0., 4.);

  // pi0-Dalitz background
  fh_all_pi0_bg_theta = new TH1D("h_all_pi0_bg_theta", "",
				 1800, 0., 180.);
  fh_acc_pi0_bg_theta = new TH1D("h_acc_pi0_bg_theta", "",
				 1800, 0., 180.);
  fh_all_pi0_bg_minv = new TH1D("h_all_pi0_bg_minv", "",
				2000, 0., 2.);
  fh_acc_pi0_bg_minv = new TH1D("h_acc_pi0_bg_minv", "",
				2000, 0., 2.);

  // STS 1 multiplicity
  fh_all_omega_tot_mult = new TH1D("h_all_omega_tot_mult", "",
				   2000, 0., 1000.);
  fh_acc_omega_tot_mult = new TH1D("h_acc_omega_tot_mult", "",
				   2000, 0., 1000.);
  fh_dcut_omega_tot_mult = new TH1D("h_dcut_omega_tot_mult", "",
				    2000, 0., 1000.);
}
// ============================================================================




// ============================================================================
void CbmDiLeptTask::SetParContainers()
{
  // Get Base Container
  FairRunAna *ana = FairRunAna::Instance();
  FairRuntimeDb *rtdb = ana->GetRuntimeDb();
  fBasePar = (FairBaseParSet*)(rtdb->getContainer("FairBaseParSet"));
}
// ============================================================================




// ============================================================================
InitStatus CbmDiLeptTask::Init()
{
  // Get list of generators
  fPrimGen = fBasePar->GetPriGen();
  fPrimGen->GetListOfGenerators()->ls();

  // Get pointers to root manager
  fRootManager = FairRootManager::Instance();

  // Get MCTrack
  fArrayMCTrack = (TClonesArray*) fRootManager->GetObject("MCTrack");
  if (! fArrayMCTrack) {
    cout << "-E- CbmMCTrack::Init: No MCTrack array!" << endl;
    return kERROR;
  }

  // Get StsPoint
  fArrayStsPoint = (TClonesArray*) fRootManager->GetObject("StsPoint");
  if (! fArrayStsPoint) {
    cout << "-E- CbmMCTrack::Init: No StsPoint array!" << endl;
    return kERROR;
  }

  // Get RichPoint
  fArrayRichPoint = (TClonesArray*) fRootManager->GetObject("RichPoint");
  if (! fArrayRichPoint) {
    cout << "-E- CbmMCTrack::Init: No RichPoint array!" << endl;
    return kERROR;
  }


  CreateHistogramms();

  ZERO2(fn_all_lept, NTYPES, 2);
  ZERO(fn_all, NTYPES);

  ZERO2(fn_acc_lept, NTYPES, 2);
  ZERO(fn_acc, NTYPES);

  ZERO2(fn_gcut_lept, NTYPES, 2);
  ZERO(fn_gcut, NTYPES);

  ZERO2(fn_dcut_lept, NTYPES, 2);
  ZERO(fn_dcut, NTYPES);

  ZERO2(fn_tpcut_lept, NTYPES, 2);
  ZERO(fn_tpcut, NTYPES);

  ZERO2(fn_ptcut_lept, NTYPES, 2);
  ZERO(fn_ptcut, NTYPES);

  ZERO2(fn_tcut_lept, NTYPES, 2);
  ZERO(fn_tcut, NTYPES);

  ZERO2(fn_pi0cut_lept, NTYPES, 2);
  ZERO(fn_pi0cut, NTYPES);

  fn_tpcut_omega_cut = 0;
  fn_tpcut_pi0_cut = 0;

  cout << "-I- CbmDiLeptTask::Init() : " << "initialisation completed." << endl;

  return kSUCCESS;
}
// ============================================================================



Bool_t mapTarget[500000];
Bool_t mapAccepted[500000];
Bool_t mapGamma[500000];
Bool_t mapDsts[500000];
Bool_t mapTp[500000];
Bool_t mapTheta[500000];
Bool_t mapPi0[500000];


TVector3 mapMomentum[500000];


Int_t ArrayTrackIdClosest[500000];
Int_t ArrayMotherPdgClosest[500000];
Int_t ArrayMotherIdClosest[500000];
Double_t ArrayDstsClosest[500000];
Double_t ArrayMinvClosest[500000];
Double_t ArraySqrtMomClosest[500000];
Double_t ArrayThetaClosest[500000];


CbmMCTrack* mcTrack = NULL;
CbmMCTrack* trackNeg = NULL;
CbmMCTrack* trackPos = NULL;



// ==================================================================
void CbmDiLeptTask::Exec(Option_t *option)
{

  Int_t nMCTrack = fArrayMCTrack->GetEntries();
  Int_t nStsPoint = fArrayStsPoint->GetEntries();
  Int_t nRichPoint = fArrayRichPoint->GetEntries();
  if( 0 == (fEvents % fCoutBunch) ) {
    cout << "Event : " << fEvents << ",  "
	 << "MC tracks : " << nMCTrack
	 << ",  STS points : " << nStsPoint
	 << ",  RICH points : " << nRichPoint << endl;
  }


  Double_t n_all_lept[NTYPES][2];
  Double_t n_all[NTYPES];

  Double_t n_acc_lept[NTYPES][2];
  Double_t n_acc[NTYPES];

  Double_t n_gcut_lept[NTYPES][2];
  Double_t n_gcut[NTYPES];

  Double_t n_dcut_lept[NTYPES][2];
  Double_t n_dcut[NTYPES];

  Double_t n_tpcut_lept[NTYPES][2];
  Double_t n_tpcut[NTYPES];

  Double_t n_ptcut_lept[NTYPES][2];
  Double_t n_ptcut[NTYPES];

  Double_t n_tcut_lept[NTYPES][2];
  Double_t n_tcut[NTYPES];

  Double_t n_pi0cut_lept[NTYPES][2];
  Double_t n_pi0cut[NTYPES];

  ZERO2(n_all_lept, NTYPES, 2);
  ZERO(n_all, NTYPES);

  ZERO2(n_acc_lept, NTYPES, 2);
  ZERO(n_acc, NTYPES);

  ZERO2(n_gcut_lept, NTYPES, 2);
  ZERO(n_gcut, NTYPES);

  ZERO2(n_dcut_lept, NTYPES, 2);
  ZERO(n_dcut, NTYPES);

  ZERO2(n_tpcut_lept, NTYPES, 2);
  ZERO(n_tpcut, NTYPES);

  ZERO2(n_ptcut_lept, NTYPES, 2);
  ZERO(n_ptcut, NTYPES);

  ZERO2(n_tcut_lept, NTYPES, 2);
  ZERO(n_tcut, NTYPES);

  ZERO2(n_pi0cut_lept, NTYPES, 2);
  ZERO(n_pi0cut, NTYPES);

  Double_t n_tpcut_omega_cut = 0;
  Double_t n_tpcut_pi0_cut = 0;



  // ------------------------------------------------------------------
  // --------- All tracks ---------------------------------------------
  // ------------------------------------------------------------------
  for(Int_t i = 0; i < nMCTrack; i++) {

    // Reset the maps
    mapTarget[i] = kFALSE;
    mapAccepted[i] = kFALSE;

    mapGamma[i] = kTRUE;
    mapDsts[i] = kTRUE;
    mapTp[i] = kTRUE;
    mapTheta[i] = kTRUE;
    mapPi0[i] = kTRUE;

    ArrayTrackIdClosest[i] = 0;
    ArrayMotherPdgClosest[i] = 0;
    ArrayMotherIdClosest[i] = 0;
    ArrayDstsClosest[i] = 0;
    ArrayMinvClosest[i] = 0;
    ArraySqrtMomClosest[i] = 0;
    ArrayThetaClosest[i] = 0;

    // Get pointer to the track
    mcTrack = (CbmMCTrack*) fArrayMCTrack->At(i);

    // Get the PDG code
    Int_t pdgCode = mcTrack->GetPdgCode();

    // Get motherID
    Int_t motherID = mcTrack->GetMotherID();

    // If primary, motherPdg = 0
    Int_t motherPdg = 0;
    if(motherID > -1) {
      motherPdg =
	((CbmMCTrack*)fArrayMCTrack->At(motherID))->GetPdgCode();
    }

    // Get vertex, momentum
    TVector3 vertex = mcTrack->GetStartVertex();
    TVector3 momentum_true = mcTrack->GetMomentum();
    TVector3 momentum = momentum_true;
    Double_t mom = momentum_true.Mag();

    // Smearing of track parameters (3-momentum)
    momentum.SetXYZ(gRandom->Gaus(momentum.X(), (0.006 * mom)),
		    gRandom->Gaus(momentum.Y(), (0.006 * mom)),
		    gRandom->Gaus(momentum.Z(), (0.006 * mom)));
    mapMomentum[i] = momentum;

    // Fill error histogramms
    fh_dp->Fill( (momentum.Mag()-momentum_true.Mag())/momentum_true.Mag() );

    // Calculate R
    Double_t R = TMath::Sqrt(vertex.X()*vertex.X() +
			     vertex.Y()*vertex.Y());
    if(vertex.Y() > 0.) {
      R = R;
    } else {
      R = -1*R;
    }

    // Fill radial distance vs. z
    Int_t type;
    Double_t weight;
    GetType(motherID, motherPdg,
	    type, weight);
    if(-11 == pdgCode) {
      fh_all_zr_lept[type][0]->Fill(vertex.Z(), R, weight);
    }
    if(11 == pdgCode) {
      fh_all_zr_lept[type][1]->Fill(vertex.Z(), R, weight);
    }

    // Start vertex cut
    Double_t r = TMath::Sqrt(vertex.X()*vertex.X() +
			     vertex.Y()*vertex.Y() +
			     vertex.Z()*vertex.Z());
    if(r > 0.0025) {
      continue;
    }
    mapTarget[i] = kTRUE;


    // Select leptons
    if(11 != TMath::Abs(pdgCode)) {
      continue;
    }

    // Calculate kinematic parameters
    Double_t energy = TMath::Sqrt( momentum.Mag2() + M2E );
    Double_t p = momentum.Mag();
    Double_t pt = momentum.Perp();
    Double_t pz = momentum.Pz();
    Double_t y = 0.5*TMath::Log( (energy+pz)/(energy-pz) );

    if(-11 == pdgCode) {
      fh_all_mom_lept[type][0]->Fill(p, weight);
      fh_all_pt_lept[type][0]->Fill(pz, weight);
      fh_all_pty_lept[type][0]->Fill(y, pt, weight);
    }
    if(11 == pdgCode) {
      fh_all_mom_lept[type][1]->Fill(p, weight);
      fh_all_pt_lept[type][1]->Fill(pz, weight);
      fh_all_pty_lept[type][1]->Fill(y, pt, weight);
    }

    // Mark as accepted
    if(mcTrack->GetStsPoints() < 6) {
      continue;
    }
    if(mcTrack->GetRichPoints() < 15) {
      continue;
    }

    mapAccepted[i] = kTRUE;
  }
  // ------------------------------------------------------------------



  // ------------------------------------------------------------------
  // -------------- Loop over all e+e- track pairs --------------------
  // -------------- e+, e- only from the target region ----------------
  // ------------------------------------------------------------------
  // Loop over negatives
  for(Int_t iNeg = 0; iNeg < nMCTrack; iNeg++) {
    trackNeg = (CbmMCTrack*) fArrayMCTrack->At(iNeg);

    // Select e-
    Int_t pdgCode_neg = trackNeg->GetPdgCode();
    if(11 != TMath::Abs(pdgCode_neg)) {
      continue;
    }

    // Skip, if not from the target
    if(! mapTarget[iNeg]) {
      continue;
    }

    // Get motherID
    Int_t motherID_neg = trackNeg->GetMotherID();

    // If primary, motherPdg = 0
    Int_t motherPdg_neg = 0;
    if(motherID_neg > -1) {
      motherPdg_neg =
	((CbmMCTrack*)fArrayMCTrack->At(motherID_neg))->GetPdgCode();
    }

    // Get vertex, momentum
    TVector3 vertex_neg = trackNeg->GetStartVertex();
    TVector3 momentum_neg = mapMomentum[iNeg];
    Double_t energy_neg = TMath::Sqrt( momentum_neg.Mag2() + M2E );
    TLorentzVector p_neg(momentum_neg, energy_neg);

    // Leptons multiplicities
    Int_t type;
    Double_t weight;
    GetType(motherID_neg, pdgCode_neg,
	    type, weight);
    if(-11 == pdgCode_neg) {
      n_all_lept[type][0] += weight;
    }
    if(11 == pdgCode_neg) {
      n_all_lept[type][1] += weight;
    }

    // Select e-
    if(11 != pdgCode_neg) {
      continue;
    }

    // Loop over positives
    for(Int_t iPos = 0; iPos < nMCTrack; iPos++) {
      trackPos = (CbmMCTrack*) fArrayMCTrack->At(iPos);

      // Select e+
      Int_t pdgCode_pos = trackPos->GetPdgCode();
      if(-11 != pdgCode_pos) {
	continue;
      }

      // Skip, if not from the target
      if(! mapTarget[iPos]) {
	continue;
      }

      // Get motherID
      Int_t motherID_pos = trackPos->GetMotherID();

      // If primary, motherPdg = 0
      Int_t motherPdg_pos = 0;
      if(motherID_pos > -1) {
	motherPdg_pos =
	  ((CbmMCTrack*)fArrayMCTrack->At(motherID_pos))->GetPdgCode();
      }

      // Get vertex, momentum
      TVector3 vertex_pos = trackPos->GetStartVertex();
      TVector3 momentum_pos = mapMomentum[iPos];
      Double_t energy_pos = TMath::Sqrt( momentum_pos.Mag2() + M2E );
      TLorentzVector p_pos(momentum_pos, energy_pos);

      // Calculate kinematic parameters of the pair
      TVector3 momentum = momentum_neg + momentum_pos;
      Double_t energy = energy_neg + energy_pos;
      Double_t pt = momentum.Perp();
      Double_t pz = momentum.Pz();
      Double_t y = 0.5*TMath::Log((energy+pz)/ (energy-pz));
      Double_t angle = p_neg.Angle(p_pos.Vect());
      Double_t theta = 180*angle/ TMath::Pi();
      Double_t minv = 2*TMath::Sin(angle/ 2)*TMath::Sqrt(momentum_neg.Mag()*momentum_pos.Mag());

      // Fill histograms
      GetType(motherID_pos, motherPdg_pos,
	      type, weight,
	      motherID_neg, motherPdg_neg,
	      kTRUE);
      fh_all_pty[type]->Fill(y, pt, weight);
      fh_all_minv[type]->Fill(minv, weight);
      fh_all_theta[type]->Fill(theta, weight);
      fh_all_mom[type]->Fill(momentum.Mag(), weight);
      fh_all_nsts[type]->Fill(trackNeg->GetStsPoints(), trackPos->GetStsPoints(), weight);
      fh_all_epem_mom[type]->Fill(momentum_neg.Mag(), momentum_pos.Mag(), weight);
      n_all[type] += weight;

      // pi0-Dalitz background
      if((111==motherPdg_neg) && (111==motherPdg_pos)) {
	fh_all_pi0_bg_minv->Fill(minv);
	fh_all_pi0_bg_theta->Fill(theta);
      }

      // Pair acceptance
      if(motherID_neg == motherID_pos) {
	if(-1 == motherID_neg) {
	  fh_all_pt_minv_omega->Fill(minv, momentum.Perp(), fWeight);
	} else {
	  fh_all_pt_minv_pairs->Fill(minv, momentum.Perp());
	}
      }

      // STS 1 multiplicity
      Double_t mult = nStsPoint/ 6.;
      fh_all_omega_tot_mult->Fill(mult);

    }//loop over positives
  }//loop over negatives
  // ------------------------------------------------------------------



  // ------------------------------------------------------------------
  // --------- Accepted tracks ----------------------------------------
  // ------------------------------------------------------------------
  for(Int_t iMCTrack = 0; iMCTrack < nMCTrack; iMCTrack++) {

    // Get pointer to the track
    mcTrack = (CbmMCTrack*) fArrayMCTrack->At(iMCTrack);

    // Get pdgCode
    Int_t pdgCode = mcTrack->GetPdgCode();
    if(11 != TMath::Abs(pdgCode)) {
      continue;
    }

    // Skip, if not accepted
    if(! mapAccepted[iMCTrack]) {
      continue;
    }

    // Get motherID
    Int_t motherID = mcTrack->GetMotherID();

    // If primary, motherPdg = 0
    Int_t motherPdg = 0;
    if(motherID > -1) {
      motherPdg =
	((CbmMCTrack*)fArrayMCTrack->At(motherID))->GetPdgCode();
    }

    // Get momentum
    TVector3 momentum = mapMomentum[iMCTrack];
    Double_t energy = TMath::Sqrt( momentum.Mag2() + M2E );
    Double_t pt = momentum.Perp();
    Double_t pz = momentum.Pz();
    Double_t y = 0.5*TMath::Log( (energy+pz)/(energy-pz) );

    // Get type
    Int_t type;
    Double_t weight;
    GetType(motherID, motherPdg,
	    type, weight);
    if(-11 == pdgCode) {
      fh_acc_mom_lept[type][0]->Fill(momentum.Mag(), weight);
      fh_acc_pt_lept[type][0]->Fill(momentum.Perp(), weight);
      fh_acc_pty_lept[type][0]->Fill(y, pt, weight);
      n_acc_lept[type][0] += weight;
    }

    if(11 == pdgCode) {
      fh_acc_mom_lept[type][1]->Fill(momentum.Mag(), weight);
      fh_acc_pt_lept[type][1]->Fill(momentum.Perp(), weight);
      fh_acc_pty_lept[type][1]->Fill(y, pt, weight);
      n_acc_lept[type][1] += weight;
    }
  }
  // ------------------------------------------------------------------



  // ------------------------------------------------------------------
  // ------------- Accepted pairs ------------------------------------
  // ------------------------------------------------------------------
  // Loop over negatives
  for(Int_t iNeg = 0; iNeg < nMCTrack; iNeg++) {
    trackNeg = (CbmMCTrack*) fArrayMCTrack->At(iNeg);

    // Get pdg code
    Int_t pdgCode_neg = trackNeg->GetPdgCode();

    // Select e-
    if(11 != pdgCode_neg) {
      continue;
    }

    // Skip, if not accepted
    if(! mapAccepted[iNeg]) {
      continue;
    }

    // Get motherID
    Int_t motherID_neg = trackNeg->GetMotherID();

    // If primary, motherPdg = 0
    Int_t motherPdg_neg = 0;
    if(motherID_neg > -1) {
      motherPdg_neg =
	((CbmMCTrack*)fArrayMCTrack->At(motherID_neg))->GetPdgCode();
    }

    // Get vertex, momentum
    TVector3 vertex_neg = trackNeg->GetStartVertex();
    TVector3 momentum_neg = mapMomentum[iNeg];
    Double_t energy_neg = TMath::Sqrt( momentum_neg.Mag2() + M2E );
    TLorentzVector p_neg(momentum_neg, energy_neg);

    // Loop over positives
    for(Int_t iPos = 0; iPos < nMCTrack; iPos++) {
      trackPos = (CbmMCTrack*) fArrayMCTrack->At(iPos);

      // Select e+
      Int_t pdgCode_pos = trackPos->GetPdgCode();
      if(-11 != pdgCode_pos) {
	continue;
      }

      // Skip, if not from target
      if(! mapTarget[iPos]) {
	continue;
      }

      // Get motherID
      Int_t motherID_pos = trackPos->GetMotherID();

      // If primary, motherPdg = 0
      Int_t motherPdg_pos = 0;
      if(motherID_pos > -1) {
	motherPdg_pos =
	  ((CbmMCTrack*)fArrayMCTrack->At(motherID_pos))->GetPdgCode();
      }

      // Get vertex, momentum
      TVector3 vertex_pos = trackPos->GetStartVertex();
      TVector3 momentum_pos = mapMomentum[iPos];
      Double_t energy_pos = TMath::Sqrt( momentum_pos.Mag2() + M2E );
      TLorentzVector p_pos(momentum_pos, energy_pos);

      // Calculate kinematic parameters of the pair
      TVector3 momentum = momentum_neg + momentum_pos;
      Double_t energy = energy_neg + energy_pos;
      Double_t pt = momentum.Perp();
      Double_t pz = momentum.Pz();
      Double_t y = 0.5*TMath::Log( (energy+pz)/(energy-pz) );
      Double_t angle = p_neg.Angle(p_pos.Vect());
      Double_t theta = 180*angle/ TMath::Pi();
      Double_t minv = 2*TMath::Sin(angle/ 2)*TMath::Sqrt(momentum_neg.Mag()*momentum_pos.Mag());

      // Get type
      Int_t type;
      Double_t weight;
      GetType(motherID_pos, motherPdg_pos,
	      type, weight);
      fh_sts_ep_mom[trackPos->GetStsPoints()-1][type]->Fill(momentum_pos.Mag(), weight);

      // Skip, if not accepted
      if(! mapAccepted[iPos]) {
	continue;
      }

      // Get type
      GetType(motherID_pos, motherPdg_pos,
	      type, weight,
	      motherID_neg, motherPdg_neg,
	      kTRUE);

      fh_acc_pty[type]->Fill(y, pt, weight);
      fh_acc_minv[type]->Fill(minv, weight);
      fh_acc_theta[type]->Fill(theta, weight);
      fh_acc_mom[type]->Fill(momentum.Mag(), weight);
      fh_acc_epem_mom[type]->Fill(momentum_neg.Mag(), momentum_pos.Mag(), weight);
      n_acc[type] += weight;

      // pi0-Dalitz background
      if((111==motherPdg_neg) && (111==motherPdg_pos)) {
	fh_acc_pi0_bg_minv->Fill(minv);
	fh_acc_pi0_bg_theta->Fill(theta);
      }

      // Pair acceptance
      if(motherID_neg == motherID_pos) {
	if(-1 == motherID_neg) {
	  fh_acc_pt_minv_omega->Fill(minv, momentum.Perp(), fWeight);
	} else {
	  fh_acc_pt_minv_pairs->Fill(minv, momentum.Perp());
	}
      }

      // <Multiplicity> / STS
      Double_t mult = nStsPoint/ 6.;
      fh_acc_omega_tot_mult->Fill(mult);

      // M gamma cut
      if(minv < fGcut) {
	mapGamma[iNeg] = kFALSE;
	mapGamma[iPos] = kFALSE;
      }

      //------------------------------------------------------------------
    }//loop over positives
  }//loop over negatives
  //------------------------------------------------------------------



  // ------------------------------------------------------------------
  // ------------- Accepted e+e+ pairs --------------------------------
  // ------------------------------------------------------------------
  // Loop over positives
  for(Int_t iNeg = 0; iNeg < nMCTrack; iNeg++) {
    trackNeg = (CbmMCTrack*) fArrayMCTrack->At(iNeg);

    // Select e+
    Int_t pdgCode_neg = trackNeg->GetPdgCode();
    if(-11 != pdgCode_neg) {
      continue;
    }

    // Skip, if not accepted
    if(! mapAccepted[iNeg]) {
      continue;
    }

    // Get motherID
    Int_t motherID_neg = trackNeg->GetMotherID();

    // If primary, motherPdg = 0
    Int_t motherPdg_neg = 0;
    if(motherID_neg > -1) {
      motherPdg_neg =
	((CbmMCTrack*)fArrayMCTrack->At(motherID_neg))->GetPdgCode();
    }

    // Get vertex, momentum
    TVector3 vertex_neg = trackNeg->GetStartVertex();
    TVector3 momentum_neg = mapMomentum[iNeg];
    Double_t energy_neg = TMath::Sqrt( momentum_neg.Mag2() + M2E );
    TLorentzVector p_neg(momentum_neg, energy_neg);

    // Loop over positives
    for(Int_t iPos = iNeg+1; iPos < nMCTrack; iPos++) {
      trackPos = (CbmMCTrack*) fArrayMCTrack->At(iPos);

      // Select e+
      Int_t pdgCode_pos = trackPos->GetPdgCode();
      if(-11 != pdgCode_pos) {
	continue;
      }

      // Skip, if not accepted
      if(! mapAccepted[iPos]) {
	continue;
      }

      // Get motherID
      Int_t motherID_pos = trackPos->GetMotherID();

      // If primary, motherPdg = 0
      Int_t motherPdg_pos = 0;
      if(motherID_pos > -1) {
	motherPdg_pos =
	  ((CbmMCTrack*)fArrayMCTrack->At(motherID_pos))->GetPdgCode();
      }

      // Get vertex, momentum and energy
      TVector3 vertex_pos = trackPos->GetStartVertex();
      TVector3 momentum_pos = mapMomentum[iPos];
      Double_t energy_pos = TMath::Sqrt( momentum_pos.Mag2() + M2E );
      TLorentzVector p_pos(momentum_pos, energy_pos);

      // Calculate kinematic parameters of the pair
      Double_t angle = p_neg.Angle(p_pos.Vect());
      Double_t minv = 2*TMath::Sin(angle/ 2)*TMath::Sqrt(momentum_neg.Mag()*momentum_pos.Mag());

      // Fill histogramms
      if((-1==motherID_neg) || (-1==motherID_pos)) {
	// bg + omega
	fh_acc_epep_minv->Fill(minv, fWeight);
      } else {
	// bg
	fh_acc_epep_minv->Fill(minv);
      }
      //------------------------------------------------------------------
    }//loop over positives
  }//loop over positives
  //------------------------------------------------------------------



  // ------------------------------------------------------------------
  // ------------- Accepted e-e- pairs --------------------------------
  // ------------------------------------------------------------------
  // Loop over negatives
  for(Int_t iNeg = 0; iNeg < nMCTrack; iNeg++) {
    trackNeg = (CbmMCTrack*) fArrayMCTrack->At(iNeg);

    // Select e-
    Int_t pdgCode_neg = trackNeg->GetPdgCode();
    if(11 != pdgCode_neg) {
      continue;
    }

    // Skip, if not accepted
    if(! mapAccepted[iNeg]) {
      continue;
    }

    // Get motherID
    Int_t motherID_neg = trackNeg->GetMotherID();

    // If primary, motherPdg = 0
    Int_t motherPdg_neg = 0;
    if(motherID_neg > -1) {
      motherPdg_neg =
	((CbmMCTrack*)fArrayMCTrack->At(motherID_neg))->GetPdgCode();
    }

    // Get vertex, momentum
    TVector3 vertex_neg = trackNeg->GetStartVertex();
    TVector3 momentum_neg = mapMomentum[iNeg];
    Double_t energy_neg = TMath::Sqrt( momentum_neg.Mag2() + M2E );
    TLorentzVector p_neg(momentum_neg, energy_neg);

    // Loop over negatives
    for(Int_t iPos = iNeg+1; iPos < nMCTrack; iPos++) {
      trackPos = (CbmMCTrack*) fArrayMCTrack->At(iPos);

      // Select e-
      Int_t pdgCode_pos = trackPos->GetPdgCode();
      if(11 != pdgCode_pos) {
	continue;
      }

      // Skip, if not accepted
      if(! mapAccepted[iPos]) {
	continue;
      }

      // Get motherID
      Int_t motherID_pos = trackPos->GetMotherID();

      // If primary, motherPdg = 0
      Int_t motherPdg_pos = 0;
      if(motherID_pos > -1) {
	motherPdg_pos =
	  ((CbmMCTrack*)fArrayMCTrack->At(motherID_pos))->GetPdgCode();
      }

      // Get vertex, momentum
      TVector3 vertex_pos = trackPos->GetStartVertex();
      TVector3 momentum_pos = mapMomentum[iPos];
      Double_t energy_pos = TMath::Sqrt( momentum_pos.Mag2() + M2E );
      TLorentzVector p_pos(momentum_pos, energy_pos);

      // Calculate kinematic parameters of the pair
      Double_t angle = p_neg.Angle(p_pos.Vect());
      Double_t minv = 2.*TMath::Sin(angle/2.)*TMath::Sqrt(momentum_neg.Mag()*momentum_pos.Mag());

      // Fill histogramms
      if((-1==motherID_neg) || (-1==motherID_pos)) {
	// bg + omega
	fh_acc_emem_minv->Fill(minv, fWeight);
      } else {
	// bg
	fh_acc_emem_minv->Fill(minv);
      }
      //------------------------------------------------------------------
    }//loop over negatives
  }//loop over negatives
  //------------------------------------------------------------------



  // ------------------------------------------------------------------
  // ------------- Gamma cut ------------------------------------------
  // ------------------------------------------------------------------
  // Loop over negatives
  for(Int_t iNeg = 0; iNeg < nMCTrack; iNeg++) {
    trackNeg = (CbmMCTrack*) fArrayMCTrack->At(iNeg);

    // Select e-
    Int_t pdgCode_neg = trackNeg->GetPdgCode();
    if(11 != TMath::Abs(pdgCode_neg)) {
      continue;
    }
    
    // Skip, if not accepted
    if(! mapAccepted[iNeg]) {
      continue;
    }
    if(! mapGamma[iNeg]) {
      continue;
    }

    // Get motherID
    Int_t motherID_neg = trackNeg->GetMotherID();
    
    // If primary, motherPdg = 0
    Int_t motherPdg_neg = 0;
    if(motherID_neg > -1) {
      motherPdg_neg =
  	((CbmMCTrack*)fArrayMCTrack->At(motherID_neg))->GetPdgCode();
    }

    // Get vertex, momentum
    TVector3 vertex_neg = trackNeg->GetStartVertex();
    TVector3 momentum_neg = mapMomentum[iNeg];
    Double_t energy_neg = TMath::Sqrt( momentum_neg.Mag2() + M2E );
    TLorentzVector p_neg(momentum_neg, energy_neg);

    // Leptons multiplicities
    Int_t type;
    Double_t weight;
    GetType(motherID_neg, pdgCode_neg,
	    type, weight);
    if(-11 == pdgCode_neg) {
      n_gcut_lept[type][0] += weight;
    }
    if(11 == pdgCode_neg) {
      n_gcut_lept[type][1] += weight;
    }

    // Select e-
    if(11 != pdgCode_neg) {
      continue;
    }

    // Loop over positives
    for(Int_t iPos = 0; iPos < nMCTrack; iPos++) {
      trackPos = (CbmMCTrack*) fArrayMCTrack->At(iPos);
 
      // Select e+
      Int_t pdgCode_pos = trackPos->GetPdgCode();
      if(-11 != pdgCode_pos) {
  	continue;
      }

      // Skip, if not accepted
      if(! mapAccepted[iPos]) {
  	continue;
      }
      if(! mapGamma[iPos]) {
  	continue;
      }

      // Get motherID
      Int_t motherID_pos = trackPos->GetMotherID();
      
      // If primary, motherPdg = 0
      Int_t motherPdg_pos = 0;
      if(motherID_pos > -1) {
  	motherPdg_pos =
  	  ((CbmMCTrack*)fArrayMCTrack->At(motherID_pos))->GetPdgCode();
      }
      
      // Get vertex, momentum
      TVector3 vertex_pos = trackPos->GetStartVertex();
      TVector3 momentum_pos = mapMomentum[iPos];
      Double_t energy_pos = TMath::Sqrt( momentum_pos.Mag2() + M2E );
      TLorentzVector p_pos(momentum_pos, energy_pos);
            
      // Calculate kinematic parameters of the pair
      Double_t angle = p_neg.Angle(p_pos.Vect());
      Double_t minv = 2*TMath::Sin(angle/ 2)*TMath::Sqrt(momentum_neg.Mag()*momentum_pos.Mag());      

      // Fill histogramms
      GetType(motherID_pos, motherPdg_pos,
  	      type, weight,
  	      motherID_neg, motherPdg_neg,
  	      kTRUE);

      fh_gcut_minv[type]->Fill(minv, weight);
      n_gcut[type] += weight;

      //------------------------------------------------------------------
    }//loop over positives
  }//loop over negatives 
  //------------------------------------------------------------------



  // -----------------------------------------------------------------
  // ------------- Gamma cut e+e+ pairs ------------------------------
  // -----------------------------------------------------------------
  // Loop over positives
  for(Int_t iNeg = 0; iNeg < nMCTrack; iNeg++) {
    trackNeg = (CbmMCTrack*) fArrayMCTrack->At(iNeg);
    
    // Select e+
    Int_t pdgCode_neg = trackNeg->GetPdgCode();
    if(-11 != pdgCode_neg) {
      continue;
    }

    // Skip, if not accepted
    if(! mapAccepted[iNeg]) {
      continue;
    }
    if(! mapGamma[iNeg]) {
      continue;
    }

    // Get motherID
    Int_t motherID_neg = trackNeg->GetMotherID();
    
    // If primary, motherPdg = 0
    Int_t motherPdg_neg = 0;
    if(motherID_neg > -1) {
      motherPdg_neg =
  	((CbmMCTrack*)fArrayMCTrack->At(motherID_neg))->GetPdgCode();
    }

    // Get vertex, momentum
    TVector3 vertex_neg = trackNeg->GetStartVertex();
    TVector3 momentum_neg = mapMomentum[iNeg];
    Double_t energy_neg = TMath::Sqrt( momentum_neg.Mag2() + M2E );
    TLorentzVector p_neg(momentum_neg, energy_neg);

    // Loop over positives
    for(Int_t iPos = iNeg+1; iPos < nMCTrack; iPos++) {
      trackPos = (CbmMCTrack*) fArrayMCTrack->At(iPos);
 
      // Select e+
      Int_t pdgCode_pos = trackPos->GetPdgCode();
      if(-11 != pdgCode_pos) {
  	continue;
      }

      // Skip, if not accepted
      if(! mapAccepted[iPos]) {
  	continue;
      }
      if(! mapGamma[iPos]) {
	continue;
      }

      // Get motherID
      Int_t motherID_pos = trackPos->GetMotherID();
      
      // If primary, motherPdg = 0
      Int_t motherPdg_pos = 0;
      if(motherID_pos > -1) {
  	motherPdg_pos =
  	  ((CbmMCTrack*)fArrayMCTrack->At(motherID_pos))->GetPdgCode();
      }

      // Get vertex, momentum and energy
      TVector3 vertex_pos = trackPos->GetStartVertex();
      TVector3 momentum_pos = mapMomentum[iPos];
      Double_t energy_pos = TMath::Sqrt( momentum_pos.Mag2() + M2E );
      TLorentzVector p_pos(momentum_pos, energy_pos);

      // Calculate kinematic parameters of the pair
      Double_t angle = p_neg.Angle(p_pos.Vect());
      Double_t minv = 2*TMath::Sin(angle/ 2)*TMath::Sqrt(momentum_neg.Mag()*momentum_pos.Mag());
      
      // Fill histogramms
      if((-1==motherID_neg) || (-1==motherID_pos)) {
  	// bg + omega
  	fh_gcut_epep_minv->Fill(minv, fWeight);
      } else {
  	// bg
   	fh_gcut_epep_minv->Fill(minv);
      }
      //------------------------------------------------------------------
    }//loop over positives
  }//loop over positives
  //------------------------------------------------------------------



  // -----------------------------------------------------------------
  // ------------- Gamma cut e-e- pairs ------------------------------
  // -----------------------------------------------------------------
  // Loop over negatives
  for(Int_t iNeg = 0; iNeg < nMCTrack; iNeg++) {
    trackNeg = (CbmMCTrack*) fArrayMCTrack->At(iNeg);
    
    // Select e-
    Int_t pdgCode_neg = trackNeg->GetPdgCode();
    if(11 != pdgCode_neg) {
      continue;
    }

    // Skip, if not accepted
    if(! mapAccepted[iNeg]) {
      continue;
    }
    if(! mapGamma[iNeg]) {
      continue;
    }

    // Get motherID
    Int_t motherID_neg = trackNeg->GetMotherID();
    
    // If primary, motherPdg = 0
    Int_t motherPdg_neg = 0;
    if(motherID_neg > -1) {
      motherPdg_neg =
  	((CbmMCTrack*)fArrayMCTrack->At(motherID_neg))->GetPdgCode();
    }

    // Get vertex, momentum
    TVector3 vertex_neg = trackNeg->GetStartVertex();
    TVector3 momentum_neg = mapMomentum[iNeg];
    Double_t energy_neg = TMath::Sqrt( momentum_neg.Mag2() + M2E );
    TLorentzVector p_neg(momentum_neg, energy_neg);

    // Loop over negatives
    for(Int_t iPos = iNeg+1; iPos < nMCTrack; iPos++) {
      trackPos = (CbmMCTrack*) fArrayMCTrack->At(iPos);
 
      // Select e-
      Int_t pdgCode_pos = trackPos->GetPdgCode();
      if(11 != pdgCode_pos) {
  	continue;
      }

      // Skip, if not accepted
      if(! mapAccepted[iPos]) {
  	continue;
      }
      if(! mapGamma[iPos]) {
	continue;
      }
      
      // Get motherID
      Int_t motherID_pos = trackPos->GetMotherID();
      
      // If primary, motherPdg = 0
      Int_t motherPdg_pos = 0;
      if(motherID_pos > -1) {
  	motherPdg_pos =
  	  ((CbmMCTrack*)fArrayMCTrack->At(motherID_pos))->GetPdgCode();
      }

      // Get vertex, momentum
      TVector3 vertex_pos = trackPos->GetStartVertex();
      TVector3 momentum_pos = mapMomentum[iPos];
      Double_t energy_pos = TMath::Sqrt( momentum_pos.Mag2() + M2E );
      TLorentzVector p_pos(momentum_pos, energy_pos);

      // Calculate kinematic parameters of the pair
      Double_t angle = p_neg.Angle(p_pos.Vect());
      Double_t minv = 2.*TMath::Sin(angle/2.)*TMath::Sqrt(momentum_neg.Mag()*momentum_pos.Mag());
            
      // Fill histogramms
      if((-1==motherID_neg) || (-1==motherID_pos)) {
  	// bg + omega
  	fh_gcut_emem_minv->Fill(minv, fWeight);
      } else {
  	// bg
   	fh_gcut_emem_minv->Fill(minv);
      }
      //------------------------------------------------------------------
    }//loop over negatives
  }//loop over negatives 
  //------------------------------------------------------------------



  Double_t x_p = 0;
  Double_t y_p = 0;
  Double_t x_n = 0;
  Double_t y_n = 0;
  //------------------------------------------------------------------
  //----------- Loop over STS point pairs of accepted tracks ---------
  //------------------------------------------------------------------
  //Loop over accepted leptons
  for(Int_t iStsPointNeg = 0; iStsPointNeg < fArrayStsPoint->GetEntries(); iStsPointNeg++) {
    CbmStsPoint *stsPointNeg = (CbmStsPoint*) fArrayStsPoint->At(iStsPointNeg);

    Int_t trackID_neg = stsPointNeg->GetTrackID();
    if(trackID_neg < 0) {
      continue;
    }

    CbmMCTrack *trackNeg = (CbmMCTrack*) fArrayMCTrack->At(trackID_neg);

    ArrayDstsClosest[trackID_neg] = 100.;
    
    // Select e+ and e-
    Int_t pdgCode_neg = trackNeg->GetPdgCode();
    if(11 != TMath::Abs(pdgCode_neg)) {
      continue;
    }

    // Get detector ID
    Int_t detectorID_neg = stsPointNeg->GetDetectorID();
    
    // Select 1 STS (z = 10 cm)
    if(14 != detectorID_neg) {
      continue;
    }
   
    // Get mother ID
    Int_t motherID_neg = trackNeg->GetMotherID();
    
    // If primary, motherPdg = 0
    Int_t motherPdg_neg = 0;
    if(motherID_neg > -1) {
      motherPdg_neg =
  	((CbmMCTrack*)fArrayMCTrack->At(motherID_neg))->GetPdgCode();
    }

    // Get position
    Double_t x_neg = stsPointNeg->GetX();
    Double_t y_neg = stsPointNeg->GetY();

    // Get momentum
    TVector3 momentum_neg = mapMomentum[trackID_neg];
    Double_t energy_neg = TMath::Sqrt( momentum_neg.Mag2() + M2E );
    TLorentzVector p_neg(momentum_neg, energy_neg);

    Int_t type;
    Double_t weight;
    GetType(motherID_neg, motherPdg_neg,
	    type, weight);

    // Skip, if not accepted
    if(! mapAccepted[trackID_neg]) {
      continue;
    }
    if(! mapGamma[trackID_neg]) {
      continue;
    }

    // Loop over all charged particles from the target region
    for(Int_t iStsPointPos = 0; iStsPointPos < fArrayStsPoint->GetEntries(); iStsPointPos++) {
      CbmStsPoint *stsPointPos = (CbmStsPoint*) fArrayStsPoint->At(iStsPointPos);
      
      Int_t trackID_pos = stsPointPos->GetTrackID();
      if(trackID_pos < 0) {
	continue;
      }

      CbmMCTrack *trackPos = (CbmMCTrack*) fArrayMCTrack->At(trackID_pos);

      // Get number of STS
      Int_t nsts_pos = trackPos->GetStsPoints();
      
      if(trackID_pos == trackID_neg) {
   	continue;
      }

      // Select Track Fragment
      if(! mapTarget[trackID_pos]) {
  	continue;
      }
      if(! mapGamma[trackID_pos]) {
	continue;
      }
      if(mapAccepted[trackID_pos]) {
	continue;
      }
      if(nsts_pos > 3) {
	continue;
      }
      
      // Get detector ID
      Int_t detectorID_pos = stsPointPos->GetDetectorID();

      // Select 1 STS (z = 10 cm)
      if(14 != detectorID_pos) {
  	continue;
      }
      
      // Get pdg code
      Int_t pdgCode_pos = trackPos->GetPdgCode();
      
      // Select e+-, pi+-, K+-, protons
      if((11 != TMath::Abs(pdgCode_pos)) &&
  	 (211 != TMath::Abs(pdgCode_pos)) &&
	 (321 != TMath::Abs(pdgCode_pos)) &&
  	 (2212 != pdgCode_pos)) {
  	continue;
      }
      
      // Set mother ID for pi+-, K+-, protons
      if((211 == TMath::Abs(pdgCode_pos)) ||
	 (321 == TMath::Abs(pdgCode_pos)) ||
  	 (2212 == pdgCode_pos)) {
  	trackPos->SetMotherID(-5);
      }
    
      // Get mother ID
      Int_t motherID_pos = trackPos->GetMotherID();
	
      // If primary, motherPdg = 0
      Int_t motherPdg_pos = 0;
      if(motherID_pos > -1) {
  	motherPdg_pos =
  	  ((CbmMCTrack*)fArrayMCTrack->At(motherID_pos))->GetPdgCode();
      }
      
      // Get position
      Double_t x_pos = stsPointPos->GetX();
      Double_t y_pos = stsPointPos->GetY();
      
      // Get momentum
      TVector3 momentum_pos = mapMomentum[trackID_pos];
      Double_t energy_pos = TMath::Sqrt( momentum_pos.Mag2() + M2E );
      TLorentzVector p_pos(momentum_pos, energy_pos);
      
      // Distance between two points
      Double_t d = TMath::Sqrt(TMath::Power((x_neg - x_pos), 2) +
  			       TMath::Power((y_neg - y_pos), 2));
      
      // Define d to the closest neighbour
      if(d < ArrayDstsClosest[trackID_neg]) {
	
  	ArrayDstsClosest[trackID_neg] = d;
  	ArrayTrackIdClosest[trackID_neg] = trackID_pos;
	ArrayMotherIdClosest[trackID_neg] = motherID_pos;
	ArrayMotherPdgClosest[trackID_neg] = motherPdg_pos;
	
  	x_p = x_pos;
  	y_p = y_pos;
	
  	x_n = x_neg;
  	y_n = y_neg;
      }
    }//loop over positives


    // Fill distance to the closest neighbour
    fh_acc_d[type]->Fill(ArrayDstsClosest[trackID_neg], weight);

    if((22==ArrayMotherPdgClosest[trackID_neg]) &&
       (motherID_neg==ArrayMotherIdClosest[trackID_neg])) {
      fh_acc_d_gamma_gamma->Fill(ArrayDstsClosest[trackID_neg], weight);
    }
    if((111==ArrayMotherPdgClosest[trackID_neg]) &&
       (motherID_neg==ArrayMotherIdClosest[trackID_neg])) {
      fh_acc_d_pi0_pi0->Fill(ArrayDstsClosest[trackID_neg], weight);
    }

    // ---   d sts cut   ----------------------------------------
    if(ArrayDstsClosest[trackID_neg] < fDcut) {
      Int_t trackID_pos = ArrayTrackIdClosest[trackID_neg];
      mapDsts[trackID_pos] = kFALSE;
      mapDsts[trackID_neg] = kFALSE;
    }
    // ----------------------------------------------------------

  }//loop over negatives
  // ------------------------------------------------------------------



  // ------------------------------------------------------------------
  // ---------- D sts cut ---------------------------------------------
  // ------------------------------------------------------------------
  // Loop over negatives
  for(Int_t iNeg = 0; iNeg < nMCTrack; iNeg++) {
    trackNeg = (CbmMCTrack*) fArrayMCTrack->At(iNeg);

    // Get pdg code
    Int_t pdgCode_neg = trackNeg->GetPdgCode();
    if(11 != TMath::Abs(pdgCode_neg)) {
      continue;
    }

    // Skip, if not accepted
    if(! mapAccepted[iNeg]) {
      continue;
    }
    if(! mapGamma[iNeg]) {
      continue;
    }
    if(! mapDsts[iNeg]) {
      continue;
    }

    // Get motherID
    Int_t motherID_neg = trackNeg->GetMotherID();
    
    // If primary, motherPdg = 0
    Int_t motherPdg_neg = 0;
    if(motherID_neg > -1) {
      motherPdg_neg =
	((CbmMCTrack*)fArrayMCTrack->At(motherID_neg))->GetPdgCode();
    }

    // Get vertex, momentum
    TVector3 vertex_neg = trackNeg->GetStartVertex();
    TVector3 momentum_neg = mapMomentum[iNeg];
    Double_t energy_neg = TMath::Sqrt( momentum_neg.Mag2() + M2E );
    TLorentzVector p_neg(momentum_neg, energy_neg);

    // Leptons multiplicities
    Int_t type;
    Double_t weight;
    GetType(motherID_neg, pdgCode_neg,
	    type, weight);
    if(-11 == pdgCode_neg) {
      n_dcut_lept[type][0] += weight;
    }
    if(11 == pdgCode_neg) {
      n_dcut_lept[type][1] += weight;
    }

    // Select e-
    if(11 != pdgCode_neg) {
      continue;
    }

    // Loop over positives
    for(Int_t iPos = 0; iPos < nMCTrack; iPos++) {
      trackPos = (CbmMCTrack*) fArrayMCTrack->At(iPos);
 
      // Select e+
      Int_t pdgCode_pos = trackPos->GetPdgCode();
      if(-11 != pdgCode_pos) {
	continue;
      }
      
      // Skip, if accepted
      if(! mapAccepted[iPos]) {
	continue;
      }
      if(! mapGamma[iPos]) {
	continue;
      }
      if(! mapDsts[iPos]) {
	continue;
      }

      // Get motherID
      Int_t motherID_pos = trackPos->GetMotherID();
      
      // If primary, motherPdg = 0
      Int_t motherPdg_pos = 0;
      if(motherID_pos > -1) {
	motherPdg_pos =
	  ((CbmMCTrack*)fArrayMCTrack->At(motherID_pos))->GetPdgCode();
      }

      // Get vertex, momentum
      TVector3 vertex_pos = trackPos->GetStartVertex();
      TVector3 momentum_pos = mapMomentum[iPos];
      Double_t energy_pos = TMath::Sqrt( momentum_pos.Mag2() + M2E );
      TLorentzVector p_pos(momentum_pos, energy_pos);

      // Calculate kinematic parameters of the pair
      Double_t angle = p_neg.Angle(p_pos.Vect());
      Double_t minv = 2 * TMath::Sin(angle / 2) *
	TMath::Sqrt(momentum_neg.Mag() * momentum_pos.Mag());

      GetType(motherID_pos, motherPdg_pos,
	      type, weight,
	      motherID_neg, motherPdg_neg,
	      kTRUE);

      fh_dcut_minv[type]->Fill(minv, weight);
      n_dcut[type] += weight;

      // total multiplicities
      Double_t mult = nStsPoint/ 6.;
      fh_dcut_omega_tot_mult->Fill(mult);
            
    }//loop over positives
  }//loop over negatives
  // ------------------------------------------------------------------



  // ------------------------------------------------------------------
  // ------------- d sts cut e+e+ pairs -------------------------------
  // ------------------------------------------------------------------
  // Loop over positives
  for(Int_t iNeg = 0; iNeg < nMCTrack; iNeg++) {
    trackNeg = (CbmMCTrack*) fArrayMCTrack->At(iNeg);    
    // Select e+
    Int_t pdgCode_neg = trackNeg->GetPdgCode();
    if(-11 != pdgCode_neg) {
      continue;
    }

    // Skip, if not accepted
    if(! mapAccepted[iNeg]) {
      continue;
    }
    if(! mapGamma[iNeg]) {
      continue;
    }
    if(! mapDsts[iNeg]) {
      continue;
    }

    // Get motherID
    Int_t motherID_neg = trackNeg->GetMotherID();
    
    // If primary, motherPdg = 0
    Int_t motherPdg_neg = 0;
    if(motherID_neg > -1) {
      motherPdg_neg =
	((CbmMCTrack*)fArrayMCTrack->At(motherID_neg))->GetPdgCode();
    }

    // Get vertex, momentum
    TVector3 vertex_neg = trackNeg->GetStartVertex();
    TVector3 momentum_neg = mapMomentum[iNeg];
    Double_t energy_neg = TMath::Sqrt( momentum_neg.Mag2() + M2E );
    TLorentzVector p_neg(momentum_neg, energy_neg);

    // Loop over positives
    for(Int_t iPos = iNeg+1; iPos < nMCTrack; iPos++) {
      trackPos = (CbmMCTrack*) fArrayMCTrack->At(iPos);
 
      // Select e+
      Int_t pdgCode_pos = trackPos->GetPdgCode();
      if(-11 != pdgCode_pos) {
	continue;
      }

      // Skip, if not accepted
      if(! mapAccepted[iPos]) {
	continue;
      }
      if(! mapGamma[iPos]) {
	continue;
      }
      if(! mapDsts[iPos]) {
	continue;
      }

      // Get motherID
      Int_t motherID_pos = trackPos->GetMotherID();
      
      // If primary, motherPdg = 0
      Int_t motherPdg_pos = 0;
      if(motherID_pos > -1) {
	motherPdg_pos =
	  ((CbmMCTrack*)fArrayMCTrack->At(motherID_pos))->GetPdgCode();
      }

      // Get vertex, momentum and energy
      TVector3 vertex_pos = trackPos->GetStartVertex();
      TVector3 momentum_pos = mapMomentum[iPos];
      Double_t energy_pos = TMath::Sqrt( momentum_pos.Mag2() + M2E );
      TLorentzVector p_pos(momentum_pos, energy_pos);

      // Calculate kinematic parameters of the pair
      Double_t angle = p_neg.Angle(p_pos.Vect());
      Double_t minv = 2*TMath::Sin(angle/ 2)*TMath::Sqrt(momentum_neg.Mag()*momentum_pos.Mag());
      
      // Fill histogramms
      if((-1==motherID_neg) || (-1==motherID_pos)) {
	// bg + omega
	fh_dcut_epep_minv->Fill(minv, fWeight);
      } else {
	// bg
	fh_dcut_epep_minv->Fill(minv);
      }
      //------------------------------------------------------------------
    }//loop over positives
  }//loop over positives
  //------------------------------------------------------------------



  // -----------------------------------------------------------------
  // ------------- d sts cut e-e- pairs ------------------------------
  // -----------------------------------------------------------------
  // Loop over negatives
  for(Int_t iNeg = 0; iNeg < nMCTrack; iNeg++) {
    trackNeg = (CbmMCTrack*) fArrayMCTrack->At(iNeg);
    
    // Select e-
    Int_t pdgCode_neg = trackNeg->GetPdgCode();
    if(11 != pdgCode_neg) {
      continue;
    }

    // Skip, if not accepted
    if(! mapAccepted[iNeg]) {
      continue;
    }
    if(! mapGamma[iNeg]) {
      continue;
    }
    if(! mapDsts[iNeg]) {
      continue;
    }

    // Get motherID
    Int_t motherID_neg = trackNeg->GetMotherID();
    
    // If primary, motherPdg = 0
    Int_t motherPdg_neg = 0;
    if(motherID_neg > -1) {
      motherPdg_neg =
	((CbmMCTrack*)fArrayMCTrack->At(motherID_neg))->GetPdgCode();
    }

    // Get vertex, momentum
    TVector3 vertex_neg = trackNeg->GetStartVertex();
    TVector3 momentum_neg = mapMomentum[iNeg];
    Double_t energy_neg = TMath::Sqrt( momentum_neg.Mag2() + M2E );
    TLorentzVector p_neg(momentum_neg, energy_neg);

    // Loop over negatives
    for(Int_t iPos = iNeg+1; iPos < nMCTrack; iPos++) {
      trackPos = (CbmMCTrack*) fArrayMCTrack->At(iPos);
 
      // Select e-
      Int_t pdgCode_pos = trackPos->GetPdgCode();
      if(11 != pdgCode_pos) {
	continue;
      }

      // Skip, if not accepted
      if(! mapAccepted[iPos]) {
	continue;
      }
      if(! mapGamma[iPos]) {
	continue;
      }
      if(! mapDsts[iPos]) {
	continue;
      }
      
      // Get motherID
      Int_t motherID_pos = trackPos->GetMotherID();
      
      // If primary, motherPdg = 0
      Int_t motherPdg_pos = 0;
      if(motherID_pos > -1) {
	motherPdg_pos =
	  ((CbmMCTrack*)fArrayMCTrack->At(motherID_pos))->GetPdgCode();
      }

      // Get vertex, momentum
      TVector3 vertex_pos = trackPos->GetStartVertex();
      TVector3 momentum_pos = mapMomentum[iPos];
      Double_t energy_pos = TMath::Sqrt( momentum_pos.Mag2() + M2E );
      TLorentzVector p_pos(momentum_pos, energy_pos);

      // Calculate kinematic parameters of the pair
      Double_t angle = p_neg.Angle(p_pos.Vect());
      Double_t minv = 2.*TMath::Sin(angle/2.)*TMath::Sqrt(momentum_neg.Mag()*momentum_pos.Mag());
            
      // Fill histogramms
      if((-1==motherID_neg) || (-1==motherID_pos)) {
	// bg + omega
	fh_dcut_emem_minv->Fill(minv, fWeight);
      } else {
	// bg
	fh_dcut_emem_minv->Fill(minv);
      }
      //------------------------------------------------------------------
    }//loop over negatives
  }//loop over negatives 
  //------------------------------------------------------------------



  //------------------------------------------------------------------
  //----------- Prepare (theta + sqrt(p*p) ) cut ---------------------
  //------------------------------------------------------------------
  Double_t sqrt_pp_cut;
  Double_t b = -1. * (f2Dtcut/ f2Dpcut);

  // Loop over negatives
  for(Int_t iNeg = 0; iNeg < nMCTrack; iNeg++) {
    trackNeg = (CbmMCTrack*) fArrayMCTrack->At(iNeg);
    
    ArrayThetaClosest[iNeg] = 360.;
    
    // Select e-
    Int_t pdgCode_neg = trackNeg->GetPdgCode();
    if(11 != TMath::Abs(pdgCode_neg)) {
      continue;
    }

    // Skip, if not accepted
    if(! mapAccepted[iNeg]) {
      continue;
    }
    if(! mapGamma[iNeg]) {
      continue;
    }
    if(! mapDsts[iNeg]) {
      continue;
    }
    
    // Get motherID
    Int_t motherID_neg = trackNeg->GetMotherID();
    
    // If primary, motherPdg = 0
    Int_t motherPdg_neg = 0;
    if(motherID_neg > -1) {
      motherPdg_neg =
  	((CbmMCTrack*)fArrayMCTrack->At(motherID_neg))->GetPdgCode();
    }

    // Get momentum
    TVector3 momentum_neg = mapMomentum[iNeg];
    Double_t energy_neg = TMath::Sqrt( momentum_neg.Mag2() + M2E );
    TLorentzVector p_neg(momentum_neg, energy_neg);

    Int_t type;
    Double_t weight;
    GetType(motherID_neg, motherPdg_neg,
   	    type, weight);

    // Loop over all charged particles from the target region
    for(Int_t iPos = 0; iPos < nMCTrack; iPos++) {
      trackPos = (CbmMCTrack*) fArrayMCTrack->At(iPos);
      
      // Get number of STS
      Int_t nsts_pos = trackPos->GetStsPoints();
      
      // Select Track Segment
      if(! mapTarget[iPos]) {
  	continue;
      }
      if(! mapGamma[iPos]) {
	continue;
      }
      if(! mapDsts[iPos]) {
	continue;
      }
      if(mapAccepted[iPos]) {
	continue;
      }
      if(nsts_pos < 3) {
	continue;
      }
      
      // Get pdg code
      Int_t pdgCode_pos = trackPos->GetPdgCode();
      
      // Select e+-, pi+-, K+-, protons
      if((11 != TMath::Abs(pdgCode_pos)) &&
  	 (211 != TMath::Abs(pdgCode_pos)) &&
	 (321 != TMath::Abs(pdgCode_pos)) &&
  	 (2212 != pdgCode_pos)) {
  	continue;
      }
      
      // Skip like-sign combinations
      if(pdgCode_neg == pdgCode_pos) {
  	continue;
      }
      if((11==pdgCode_neg) && (-211==pdgCode_pos)) {
  	continue;
      }
      if((11==pdgCode_neg) && (-321==pdgCode_pos)) {
  	continue;
      }
      if((-11==pdgCode_neg) && (211==pdgCode_pos)) {
  	continue;
      }
      if((-11==pdgCode_neg) && (321==pdgCode_pos)) {
  	continue;
      }
      if((-11==pdgCode_neg) && (2212==pdgCode_pos)) {
  	continue;
      }
      
      // Set mother ID for pi+-, K+-, protons
      if((211 == TMath::Abs(pdgCode_pos)) ||
	 (321 == TMath::Abs(pdgCode_pos)) ||
  	 (2212 == pdgCode_pos)) {
  	trackPos->SetMotherID(-5);
      }
    
      // Get mother ID
      Int_t motherID_pos = trackPos->GetMotherID();
	
      // If primary, motherPdg = 0
      Int_t motherPdg_pos = 0;
      if(motherID_pos > -1) {
  	motherPdg_pos =
  	  ((CbmMCTrack*)fArrayMCTrack->At(motherID_pos))->GetPdgCode();
      }
      
      // Get momentum
      TVector3 momentum_pos = mapMomentum[iPos];
      Double_t energy_pos = TMath::Sqrt( momentum_pos.Mag2() + M2E );
      TLorentzVector p_pos(momentum_pos, energy_pos);
      
      // Calculate kinematic parameters of the pair
      Double_t angle = p_neg.Angle(p_pos.Vect());
      Double_t theta = 180*angle/ TMath::Pi();
      Double_t minv = 2*TMath::Sin(angle/ 2)*TMath::Sqrt(momentum_neg.Mag()*momentum_pos.Mag());
      Double_t sqrt_mom = TMath::Sqrt(momentum_neg.Mag()*momentum_pos.Mag());

      // Fill theta vs. sqrt mom
      fh_dcut_theta_sqrt_mom_true[type]->Fill(sqrt_mom, theta, weight);
      
      // Find closest track by opening angle
      if(theta < ArrayThetaClosest[iNeg]) {
	
	ArrayThetaClosest[iNeg] = theta;
	ArrayMinvClosest[iNeg] = minv;
	ArraySqrtMomClosest[iNeg] = sqrt_mom;
	ArrayTrackIdClosest[iNeg] = iPos;
	
      }
    }//loop over positives
    
    
    // Fill histogram
    fh_dcut_theta_minv[type]->Fill(ArrayMinvClosest[iNeg], ArrayThetaClosest[iNeg], weight);
    fh_dcut_theta_sqrt_mom[type]->Fill(ArraySqrtMomClosest[iNeg], ArrayThetaClosest[iNeg], weight);
    
    // --- ( theta + sqrt(p*p) ) cut ---
    sqrt_pp_cut = b * ArrayThetaClosest[iNeg] + f2Dtcut;

    if((ArrayThetaClosest[iNeg] <= f2Dtcut) &&
       (ArraySqrtMomClosest[iNeg] <= sqrt_pp_cut)) {
     
      if((-1 == motherID_neg) || (-1 == ArrayMotherIdClosest[iNeg])) {
	n_tpcut_omega_cut += 1;
      }
      if((111 == motherPdg_neg) || (111 == ArrayMotherPdgClosest[iNeg])) {
	n_tpcut_pi0_cut += 1;
      }
      Int_t iPos = ArrayTrackIdClosest[iNeg];
      mapTp[iNeg] = kFALSE;
      mapTp[iPos] = kFALSE;
    }
    
  }//loop over negatives
  // ------------------------------------------------------------------



  // ------------------------------------------------------------------
  // ------------- ( theta + sqrt(p*p) ) cut --------------------------
  // ------------------------------------------------------------------
  // Loop over negatives
  for(Int_t iNeg = 0; iNeg < nMCTrack; iNeg++) {
    
    trackNeg = (CbmMCTrack*) fArrayMCTrack->At(iNeg);

    // Select e-, e+
    Int_t pdgCode_neg = trackNeg->GetPdgCode();
    if(11 != TMath::Abs(pdgCode_neg)) {
      continue;
    }
    // Skip, if not accepted
    if(! mapAccepted[iNeg]) {
      continue;
    }
    if(! mapGamma[iNeg]) {
      continue;
    }
    if(! mapDsts[iNeg]) {
      continue;
    }
    if(! mapTp[iNeg]) {
      continue;
    }
    
    // Get motherID
    Int_t motherID_neg = trackNeg->GetMotherID();
    
    // If primary, motherPdg = 0
    Int_t motherPdg_neg = 0;
    if(motherID_neg > -1) {
      motherPdg_neg =
	((CbmMCTrack*)fArrayMCTrack->At(motherID_neg))->GetPdgCode();
    }

    // Get vertex, momentum
    TVector3 vertex_neg = trackNeg->GetStartVertex();
    TVector3 momentum_neg = mapMomentum[iNeg];
    Double_t energy_neg = TMath::Sqrt( momentum_neg.Mag2() + M2E );
    TLorentzVector p_neg(momentum_neg, energy_neg);

    // Leptons multiplicities
    Int_t type;
    Double_t weight;
    GetType(motherID_neg, pdgCode_neg,
	    type, weight);
    if(-11 == pdgCode_neg) {
      n_tpcut_lept[type][0] += weight;
    }
    if(11 == pdgCode_neg) {
      n_tpcut_lept[type][1] += weight;
    }

    // Select e-
    if(11 != pdgCode_neg) {
      continue;
    }

    // Loop over positives
    for(Int_t iPos = 0; iPos < nMCTrack; iPos++) {
      trackPos = (CbmMCTrack*) fArrayMCTrack->At(iPos);

      // Select e+
      Int_t pdgCode_pos = trackPos->GetPdgCode();
      if(-11 != pdgCode_pos) {
	continue;
      }
      // Skip, if not accepted
      if(! mapAccepted[iPos]) {
	continue;
      }
      if(! mapGamma[iPos]) {
	continue;
      }
      if(! mapDsts[iPos]) {
	continue;
      }
      if(! mapTp[iPos]) {
	continue;
      }  
      
      // Get motherID
      Int_t motherID_pos = trackPos->GetMotherID();
      
      // If primary, motherPdg = 0
      Int_t motherPdg_pos = 0;
      if(motherID_pos > -1) {
	motherPdg_pos =
	  ((CbmMCTrack*)fArrayMCTrack->At(motherID_pos))->GetPdgCode();
      }
      
      // Get vertex, momentum
      TVector3 vertex_pos = trackPos->GetStartVertex();
      TVector3 momentum_pos = mapMomentum[iPos];
      Double_t energy_pos = TMath::Sqrt( momentum_pos.Mag2() + M2E );
      TLorentzVector p_pos(momentum_pos, energy_pos);

      // Calculate kinematic parameters of the pair
      Double_t angle = p_neg.Angle(p_pos.Vect());
      Double_t minv = 2*TMath::Sin(angle/ 2)*TMath::Sqrt(momentum_neg.Mag()*momentum_pos.Mag());

      GetType(motherID_pos, motherPdg_pos,
	      type, weight,
	      motherID_neg, motherPdg_neg,
	      kTRUE);
      
      fh_tpcut_minv[type]->Fill(minv, weight);
      n_tpcut[type] += weight;
   
    }//loop over positives
  }//loop over negatives 
  // ------------------------------------------------------------------



  // ------------------------------------------------------------------
  // ------------- ( theta + sqrt(p*p) ) cut  e+e+ pairs --------------
  // ------------------------------------------------------------------
  // Loop over positives
  for(Int_t iNeg = 0; iNeg < nMCTrack; iNeg++) {
    trackNeg = (CbmMCTrack*) fArrayMCTrack->At(iNeg);
    
    // Select e+
    Int_t pdgCode_neg = trackNeg->GetPdgCode();
    if(-11 != pdgCode_neg) {
      continue;
    }

    // Skip, if not accepted
    if(! mapAccepted[iNeg]) {
      continue;
    }
    if(! mapGamma[iNeg]) {
      continue;
    }
    if(! mapDsts[iNeg]) {
      continue;
    }
    if(! mapTp[iNeg]) {
      continue;
    }

    // Get motherID
    Int_t motherID_neg = trackNeg->GetMotherID();
    
    // If primary, motherPdg = 0
    Int_t motherPdg_neg = 0;
    if(motherID_neg > -1) {
      motherPdg_neg =
	((CbmMCTrack*)fArrayMCTrack->At(motherID_neg))->GetPdgCode();
    }

    // Get vertex, momentum
    TVector3 vertex_neg = trackNeg->GetStartVertex();
    TVector3 momentum_neg = mapMomentum[iNeg];
    Double_t energy_neg = TMath::Sqrt( momentum_neg.Mag2() + M2E );
    TLorentzVector p_neg(momentum_neg, energy_neg);

    // Loop over positives
    for(Int_t iPos = iNeg+1; iPos < nMCTrack; iPos++) {
      trackPos = (CbmMCTrack*) fArrayMCTrack->At(iPos);
 
      // Select e+
      Int_t pdgCode_pos = trackPos->GetPdgCode();
      if(-11 != pdgCode_pos) {
	continue;
      }

      // Skip, if not accepted
      if(! mapAccepted[iPos]) {
	continue;
      }
      if(! mapGamma[iPos]) {
	continue;
      }
      if(! mapDsts[iPos]) {
	continue;
      }
      if(! mapTp[iPos]) {
	continue;
      }

      // Get motherID
      Int_t motherID_pos = trackPos->GetMotherID();
      
      // If primary, motherPdg = 0
      Int_t motherPdg_pos = 0;
      if(motherID_pos > -1) {
	motherPdg_pos =
	  ((CbmMCTrack*)fArrayMCTrack->At(motherID_pos))->GetPdgCode();
      }

      // Get vertex, momentum and energy
      TVector3 vertex_pos = trackPos->GetStartVertex();
      TVector3 momentum_pos = mapMomentum[iPos];
      Double_t energy_pos = TMath::Sqrt( momentum_pos.Mag2() + M2E );
      TLorentzVector p_pos(momentum_pos, energy_pos);

      // Calculate kinematic parameters of the pair
      Double_t angle = p_neg.Angle(p_pos.Vect());
      Double_t minv = 2*TMath::Sin(angle/ 2)*TMath::Sqrt(momentum_neg.Mag()*momentum_pos.Mag());
      
      // Fill histogramms
      if((-1==motherID_neg) || (-1==motherID_pos)) {
	// bg + omega
	fh_tpcut_epep_minv->Fill(minv, fWeight);
      } else {
	// bg
	fh_tpcut_epep_minv->Fill(minv);
      }

    }//loop over positives
  }//loop over positives
  //------------------------------------------------------------------



  // -----------------------------------------------------------------
  // ------------- ( theta + sqrt(p*p) ) cut e-e- pairs --------------
  // -----------------------------------------------------------------
  // Loop over negatives
  for(Int_t iNeg = 0; iNeg < nMCTrack; iNeg++) {
    trackNeg = (CbmMCTrack*) fArrayMCTrack->At(iNeg);
    
    // Select e-
    Int_t pdgCode_neg = trackNeg->GetPdgCode();
    if(11 != pdgCode_neg) {
      continue;
    }

    // Skip, if not accepted
    if(! mapAccepted[iNeg]) {
      continue;
    }
    if(! mapGamma[iNeg]) {
      continue;
    }
    if(! mapDsts[iNeg]) {
      continue;
    }
    if(! mapTp[iNeg]) {
      continue;
    }

    // Get motherID
    Int_t motherID_neg = trackNeg->GetMotherID();
    
    // If primary, motherPdg = 0
    Int_t motherPdg_neg = 0;
    if(motherID_neg > -1) {
      motherPdg_neg =
	((CbmMCTrack*)fArrayMCTrack->At(motherID_neg))->GetPdgCode();
    }

    // Get vertex, momentum
    TVector3 vertex_neg = trackNeg->GetStartVertex();
    TVector3 momentum_neg = mapMomentum[iNeg];
    Double_t energy_neg = TMath::Sqrt( momentum_neg.Mag2() + M2E );
    TLorentzVector p_neg(momentum_neg, energy_neg);

    // Loop over negatives
    for(Int_t iPos = iNeg+1; iPos < nMCTrack; iPos++) {
      trackPos = (CbmMCTrack*) fArrayMCTrack->At(iPos);
 
      // Select e-
      Int_t pdgCode_pos = trackPos->GetPdgCode();
      if(11 != pdgCode_pos) {
	continue;
      }

      // Skip, if not accepted
      if(! mapAccepted[iPos]) {
	continue;
      }
      if(! mapGamma[iPos]) {
	continue;
      }
      if(! mapDsts[iPos]) {
	continue;
      }
      if(! mapTp[iPos]) {
	continue;
      }
      
      // Get motherID
      Int_t motherID_pos = trackPos->GetMotherID();
      
      // If primary, motherPdg = 0
      Int_t motherPdg_pos = 0;
      if(motherID_pos > -1) {
	motherPdg_pos =
	  ((CbmMCTrack*)fArrayMCTrack->At(motherID_pos))->GetPdgCode();
      }

      // Get vertex, momentum
      TVector3 vertex_pos = trackPos->GetStartVertex();
      TVector3 momentum_pos = mapMomentum[iPos];
      Double_t energy_pos = TMath::Sqrt( momentum_pos.Mag2() + M2E );
      TLorentzVector p_pos(momentum_pos, energy_pos);

      // Calculate kinematic parameters of the pair
      Double_t angle = p_neg.Angle(p_pos.Vect());
      Double_t minv = 2.*TMath::Sin(angle/2.)*TMath::Sqrt(momentum_neg.Mag()*momentum_pos.Mag());
            
      // Fill histogramms
      if((-1==motherID_neg) || (-1==motherID_pos)) {
	// bg + omega
	fh_tpcut_emem_minv->Fill(minv, fWeight);
      } else {
	// bg
	fh_tpcut_emem_minv->Fill(minv);
      }
      //------------------------------------------------------------------
    }//loop over negatives
  }//loop over negatives 
  //------------------------------------------------------------------



  // ------------------------------------------------------------------
  // ------------- pt cut ---------------------------------------------
  // ------------------------------------------------------------------
  // Loop over negatives
  for(Int_t iNeg = 0; iNeg < nMCTrack; iNeg++) {
    
    trackNeg = (CbmMCTrack*) fArrayMCTrack->At(iNeg);

    // Select e-, e+
    Int_t pdgCode_neg = trackNeg->GetPdgCode();
    if(11 != TMath::Abs(pdgCode_neg)) {
      continue;
    }
    // Skip, if not accepted
    if(! mapAccepted[iNeg]) {
      continue;
    }
    if(! mapGamma[iNeg]) {
      continue;
    }
    if(! mapDsts[iNeg]) {
      continue;
    }
    if(! mapTp[iNeg]) {
      continue;
    }
    
    // Get motherID
    Int_t motherID_neg = trackNeg->GetMotherID();
    
    // If primary, motherPdg = 0
    Int_t motherPdg_neg = 0;
    if(motherID_neg > -1) {
      motherPdg_neg =
	((CbmMCTrack*)fArrayMCTrack->At(motherID_neg))->GetPdgCode();
    }

    // Get vertex, momentum
    TVector3 vertex_neg = trackNeg->GetStartVertex();
    TVector3 momentum_neg = mapMomentum[iNeg];
    Double_t energy_neg = TMath::Sqrt( momentum_neg.Mag2() + M2E );
    TLorentzVector p_neg(momentum_neg, energy_neg);

    Int_t type;
    Double_t weight;
    GetType(motherID_neg, motherPdg_neg,
  	    type, weight);

    // Fill pt distributions
    if(-11 == pdgCode_neg) {
      fh_tpcut_pt_lept[type][0]->Fill(momentum_neg.Perp(), weight);
    } else {
      fh_tpcut_pt_lept[type][1]->Fill(momentum_neg.Perp(), weight);
    }

    // pt cut
    if(momentum_neg.Perp() < fPtcut) {
      continue;
    }
    
    // Leptons multiplicities
    if(-11 == pdgCode_neg) {
      n_ptcut_lept[type][0] += weight;
    }
    if(11 == pdgCode_neg) {
      n_ptcut_lept[type][1] += weight;
    }
    
    // Select e-
    if(11 != pdgCode_neg) {
      continue;
    }

    // Loop over positives
    for(Int_t iPos = 0; iPos < nMCTrack; iPos++) {
      trackPos = (CbmMCTrack*) fArrayMCTrack->At(iPos);
 
      // Select e+
      Int_t pdgCode_pos = trackPos->GetPdgCode();
      if(-11 != pdgCode_pos) {
	continue;
      }
      // Skip, if not accepted
      if(! mapAccepted[iPos]) {
	continue;
      }
      if(! mapGamma[iPos]) {
	continue;
      }
      if(! mapDsts[iPos]) {
	continue;
      }
      if(! mapTp[iPos]) {
	continue;
      }  
      
      // Get motherID
      Int_t motherID_pos = trackPos->GetMotherID();
      
      // If primary, motherPdg = 0
      Int_t motherPdg_pos = 0;
      if(motherID_pos > -1) {
	motherPdg_pos =
	  ((CbmMCTrack*)fArrayMCTrack->At(motherID_pos))->GetPdgCode();
      }
      
      // Get vertex, momentum
      TVector3 vertex_pos = trackPos->GetStartVertex();
      TVector3 momentum_pos = mapMomentum[iPos];
      Double_t energy_pos = TMath::Sqrt( momentum_pos.Mag2() + M2E );
      TLorentzVector p_pos(momentum_pos, energy_pos);

      // pt cut
      if(momentum_pos.Perp() < fPtcut) {
	continue;
      }

      // Calculate kinematic parameters of the pair
      Double_t angle = p_neg.Angle(p_pos.Vect());
      Double_t theta = 180*angle/ TMath::Pi();
      Double_t minv = 2*TMath::Sin(angle/ 2)*TMath::Sqrt(momentum_neg.Mag()*momentum_pos.Mag());

      GetType(motherID_pos, motherPdg_pos,
	      type, weight,
	      motherID_neg, motherPdg_neg,
	      kTRUE);
      
      fh_ptcut_minv[type]->Fill(minv, weight);
      fh_ptcut_theta[type]->Fill(theta, weight);
      n_ptcut[type] += weight;
      
      // opening angle cut
      if(theta < fTcut) {
	mapTheta[iNeg] = kFALSE;
	mapTheta[iPos] = kFALSE;
      }
      
    }//loop over positives
  }//loop over negatives 
  // ------------------------------------------------------------------



  // ------------------------------------------------------------------
  // ------------- pt cut e+e+ pairs ----------------------------------
  // ------------------------------------------------------------------
  // Loop over positives
  for(Int_t iNeg = 0; iNeg < nMCTrack; iNeg++) {
    trackNeg = (CbmMCTrack*) fArrayMCTrack->At(iNeg);
    
    // Select e+
    Int_t pdgCode_neg = trackNeg->GetPdgCode();
    if(-11 != pdgCode_neg) {
      continue;
    }

    // Skip, if not accepted
    if(! mapAccepted[iNeg]) {
      continue;
    }
    if(! mapGamma[iNeg]) {
      continue;
    }
    if(! mapDsts[iNeg]) {
      continue;
    }
    if(! mapTp[iNeg]) {
      continue;
    }

    // Get motherID
    Int_t motherID_neg = trackNeg->GetMotherID();
    
    // If primary, motherPdg = 0
    Int_t motherPdg_neg = 0;
    if(motherID_neg > -1) {
      motherPdg_neg =
	((CbmMCTrack*)fArrayMCTrack->At(motherID_neg))->GetPdgCode();
    }

    // Get vertex, momentum
    TVector3 vertex_neg = trackNeg->GetStartVertex();
    TVector3 momentum_neg = mapMomentum[iNeg];
    Double_t energy_neg = TMath::Sqrt( momentum_neg.Mag2() + M2E );
    TLorentzVector p_neg(momentum_neg, energy_neg);

    // pt cut
    if(momentum_neg.Perp() < fPtcut) {
      continue;
    }

    // Loop over positives
    for(Int_t iPos = iNeg+1; iPos < nMCTrack; iPos++) {
      trackPos = (CbmMCTrack*) fArrayMCTrack->At(iPos);
 
      // Select e+
      Int_t pdgCode_pos = trackPos->GetPdgCode();
      if(-11 != pdgCode_pos) {
	continue;
      }

      // Skip, if not accepted
      if(! mapAccepted[iPos]) {
	continue;
      }
      if(! mapGamma[iPos]) {
	continue;
      }
      if(! mapDsts[iPos]) {
	continue;
      }
      if(! mapTp[iPos]) {
	continue;
      }

      // Get motherID
      Int_t motherID_pos = trackPos->GetMotherID();
      
      // If primary, motherPdg = 0
      Int_t motherPdg_pos = 0;
      if(motherID_pos > -1) {
	motherPdg_pos =
	  ((CbmMCTrack*)fArrayMCTrack->At(motherID_pos))->GetPdgCode();
      }

      // Get vertex, momentum and energy
      TVector3 vertex_pos = trackPos->GetStartVertex();
      TVector3 momentum_pos = mapMomentum[iPos];
      Double_t energy_pos = TMath::Sqrt( momentum_pos.Mag2() + M2E );
      TLorentzVector p_pos(momentum_pos, energy_pos);

      // pt cut
      if(momentum_pos.Perp() < fPtcut) {
	continue;
      }

      // Calculate kinematic parameters of the pair
      Double_t angle = p_neg.Angle(p_pos.Vect());
      Double_t minv = 2*TMath::Sin(angle/ 2)*TMath::Sqrt(momentum_neg.Mag()*momentum_pos.Mag());
      
      // Fill histogramms
      if((-1==motherID_neg) || (-1==motherID_pos)) {
	// bg + omega
	fh_ptcut_epep_minv->Fill(minv, fWeight);
      } else {
	// bg
	fh_ptcut_epep_minv->Fill(minv);
      }

    }//loop over positives
  }//loop over positives
  //------------------------------------------------------------------



  // -----------------------------------------------------------------
  // ------------- pt cut e-e- pairs ---------------------------------
  // -----------------------------------------------------------------
  // Loop over negatives
  for(Int_t iNeg = 0; iNeg < nMCTrack; iNeg++) {
    trackNeg = (CbmMCTrack*) fArrayMCTrack->At(iNeg);
    
    // Select e-
    Int_t pdgCode_neg = trackNeg->GetPdgCode();
    if(11 != pdgCode_neg) {
      continue;
    }

    // Skip, if not accepted
    if(! mapAccepted[iNeg]) {
      continue;
    }
    if(! mapGamma[iNeg]) {
      continue;
    }
    if(! mapDsts[iNeg]) {
      continue;
    }
    if(! mapTp[iNeg]) {
      continue;
    }

    // Get motherID
    Int_t motherID_neg = trackNeg->GetMotherID();
    
    // If primary, motherPdg = 0
    Int_t motherPdg_neg = 0;
    if(motherID_neg > -1) {
      motherPdg_neg =
	((CbmMCTrack*)fArrayMCTrack->At(motherID_neg))->GetPdgCode();
    }

    // Get vertex, momentum
    TVector3 vertex_neg = trackNeg->GetStartVertex();
    TVector3 momentum_neg = mapMomentum[iNeg];
    Double_t energy_neg = TMath::Sqrt( momentum_neg.Mag2() + M2E );
    TLorentzVector p_neg(momentum_neg, energy_neg);

    // pt cut
    if(momentum_neg.Perp() < fPtcut) {
      continue;
    }

    // Loop over negatives
    for(Int_t iPos = iNeg+1; iPos < nMCTrack; iPos++) {
      trackPos = (CbmMCTrack*) fArrayMCTrack->At(iPos);
 
      // Select e-
      Int_t pdgCode_pos = trackPos->GetPdgCode();
      if(11 != pdgCode_pos) {
	continue;
      }

      // Skip, if not accepted
      if(! mapAccepted[iPos]) {
	continue;
      }
      if(! mapGamma[iPos]) {
	continue;
      }
      if(! mapDsts[iPos]) {
	continue;
      }
      if(! mapTp[iPos]) {
	continue;
      }
      
      // Get motherID
      Int_t motherID_pos = trackPos->GetMotherID();
      
      // If primary, motherPdg = 0
      Int_t motherPdg_pos = 0;
      if(motherID_pos > -1) {
	motherPdg_pos =
	  ((CbmMCTrack*)fArrayMCTrack->At(motherID_pos))->GetPdgCode();
      }

      // Get vertex, momentum
      TVector3 vertex_pos = trackPos->GetStartVertex();
      TVector3 momentum_pos = mapMomentum[iPos];
      Double_t energy_pos = TMath::Sqrt( momentum_pos.Mag2() + M2E );
      TLorentzVector p_pos(momentum_pos, energy_pos);

      // pt cut
      if(momentum_pos.Perp() < fPtcut) {
	continue;
      }

      // Calculate kinematic parameters of the pair
      Double_t angle = p_neg.Angle(p_pos.Vect());
      Double_t minv = 2.*TMath::Sin(angle/2.)*TMath::Sqrt(momentum_neg.Mag()*momentum_pos.Mag());
            
      // Fill histogramms
      if((-1==motherID_neg) || (-1==motherID_pos)) {
	// bg + omega
	fh_ptcut_emem_minv->Fill(minv, fWeight);
      } else {
	// bg
	fh_ptcut_emem_minv->Fill(minv);
      }
      
    }//loop over negatives
  }//loop over negatives 
  //------------------------------------------------------------------



  // ------------------------------------------------------------------
  // ---------- Opening angle cut -------------------------------------
  // ------------------------------------------------------------------
  // Loop over negatives
  for(Int_t iNeg = 0; iNeg < nMCTrack; iNeg++) {
    trackNeg = (CbmMCTrack*) fArrayMCTrack->At(iNeg);

    // Get pdg code
    Int_t pdgCode_neg = trackNeg->GetPdgCode();
    if(11 != TMath::Abs(pdgCode_neg)) {
      continue;
    }
    
    // Skip, if not accepted
    if(! mapAccepted[iNeg]) {
      continue;
    }
    if(! mapGamma[iNeg]) {
      continue;
    }
    if(! mapDsts[iNeg]) {
      continue;
    }
    if(! mapTp[iNeg]) {
      continue;
    }  

    // Get motherID
    Int_t motherID_neg = trackNeg->GetMotherID();
    
    // If primary, motherPdg = 0
    Int_t motherPdg_neg = 0;
    if(motherID_neg > -1) {
      motherPdg_neg =
	((CbmMCTrack*)fArrayMCTrack->At(motherID_neg))->GetPdgCode();
    }

    // Get vertex, momentum
    TVector3 vertex_neg = trackNeg->GetStartVertex();
    TVector3 momentum_neg = mapMomentum[iNeg];
    Double_t energy_neg = TMath::Sqrt( momentum_neg.Mag2() + M2E );
    TLorentzVector p_neg(momentum_neg, energy_neg);
    
    // pt cut
    if(momentum_neg.Perp() < fPtcut) {
      continue;
    }

    // theta cut
    if(! mapTheta[iNeg]) {
      continue;
    }
    
    // Leptons multiplicities
    Int_t type;
    Double_t weight;
    GetType(motherID_neg, pdgCode_neg,
	    type, weight);
    if(-11 == pdgCode_neg) {
      n_tcut_lept[type][0] += weight;
    }
    if(11 == pdgCode_neg) {
      n_tcut_lept[type][1] += weight;
    }
    
    // Select e-
    if(11 != pdgCode_neg) {
      continue;
    }
    
    // Loop over positives
    for(Int_t iPos = 0; iPos < nMCTrack; iPos++) {
      trackPos = (CbmMCTrack*) fArrayMCTrack->At(iPos);
 
      // Select e+
      Int_t pdgCode_pos = trackPos->GetPdgCode();
      if(-11 != pdgCode_pos) {
	continue;
      }
      
      // Skip, if accepted
      if(! mapAccepted[iPos]) {
	continue;
      }
      if(! mapGamma[iPos]) {
	continue;
      }
      if(! mapDsts[iPos]) {
	continue;
      }
      if(! mapTp[iPos]) {
	continue;
      }  

      // Get motherID
      Int_t motherID_pos = trackPos->GetMotherID();
      
      // If primary, motherPdg = 0
      Int_t motherPdg_pos = 0;
      if(motherID_pos > -1) {
	motherPdg_pos =
	  ((CbmMCTrack*)fArrayMCTrack->At(motherID_pos))->GetPdgCode();
      }

      // Get vertex, momentum
      TVector3 vertex_pos = trackPos->GetStartVertex();
      TVector3 momentum_pos = mapMomentum[iPos];
      Double_t energy_pos = TMath::Sqrt( momentum_pos.Mag2() + M2E );
      TLorentzVector p_pos(momentum_pos, energy_pos);

      // pt cut
      if(momentum_pos.Perp() < fPtcut) {
	continue;
      }

      // theta cut
      if(! mapTheta[iPos]) {
	continue;
      }
      
      // Calculate kinematic parameters of the pair
      Double_t angle = p_neg.Angle(p_pos.Vect());
      Double_t minv = 2*TMath::Sin(angle/ 2)*TMath::Sqrt(momentum_neg.Mag()*momentum_pos.Mag());
  
      // Fill histogramms
      GetType(motherID_pos, motherPdg_pos,
	      type, weight,
	      motherID_neg, motherPdg_neg,
	      kTRUE);
      
      fh_tcut_minv[type]->Fill(minv, weight);
      n_tcut[type] += weight;

    }//loop over positives
  }//loop over negatives
  // ------------------------------------------------------------------



  // ------------------------------------------------------------------
  // ------------- Opening angle cut e+e+ pairs -----------------------
  // ------------------------------------------------------------------
  // Loop over positives
  for(Int_t iNeg = 0; iNeg < nMCTrack; iNeg++) {
    trackNeg = (CbmMCTrack*) fArrayMCTrack->At(iNeg);
    
    // Select e+
    Int_t pdgCode_neg = trackNeg->GetPdgCode();
    if(-11 != pdgCode_neg) {
      continue;
    }

    // Skip, if not accepted
    if(! mapAccepted[iNeg]) {
      continue;
    }
    if(! mapGamma[iNeg]) {
      continue;
    }
    if(! mapDsts[iNeg]) {
      continue;
    }
    if(! mapTp[iNeg]) {
      continue;
    }

    // Get motherID
    Int_t motherID_neg = trackNeg->GetMotherID();
    
    // If primary, motherPdg = 0
    Int_t motherPdg_neg = 0;
    if(motherID_neg > -1) {
      motherPdg_neg =
	((CbmMCTrack*)fArrayMCTrack->At(motherID_neg))->GetPdgCode();
    }

    // Get vertex, momentum
    TVector3 vertex_neg = trackNeg->GetStartVertex();
    TVector3 momentum_neg = mapMomentum[iNeg];
    Double_t energy_neg = TMath::Sqrt( momentum_neg.Mag2() + M2E );
    TLorentzVector p_neg(momentum_neg, energy_neg);

    // pt cut
    if(momentum_neg.Perp() < fPtcut) {
      continue;
    }

    // theta cut
    if(! mapTheta[iNeg]) {
      continue;
    }

    // Loop over positives
    for(Int_t iPos = iNeg+1; iPos < nMCTrack; iPos++) {
      trackPos = (CbmMCTrack*) fArrayMCTrack->At(iPos);
 
      // Select e+
      Int_t pdgCode_pos = trackPos->GetPdgCode();
      if(-11 != pdgCode_pos) {
	continue;
      }

      // Skip, if not accepted
      if(! mapAccepted[iPos]) {
	continue;
      }
      if(! mapGamma[iPos]) {
	continue;
      }
      if(! mapDsts[iPos]) {
	continue;
      }
      if(! mapTp[iPos]) {
	continue;
      }

      // Get motherID
      Int_t motherID_pos = trackPos->GetMotherID();
      
      // If primary, motherPdg = 0
      Int_t motherPdg_pos = 0;
      if(motherID_pos > -1) {
	motherPdg_pos =
	  ((CbmMCTrack*)fArrayMCTrack->At(motherID_pos))->GetPdgCode();
      }

      // Get vertex, momentum and energy
      TVector3 vertex_pos = trackPos->GetStartVertex();
      TVector3 momentum_pos = mapMomentum[iPos];
      Double_t energy_pos = TMath::Sqrt( momentum_pos.Mag2() + M2E );
      TLorentzVector p_pos(momentum_pos, energy_pos);

      // pt cut
      if(momentum_pos.Perp() < fPtcut) {
	continue;
      }

      // theta cut
      if(! mapTheta[iPos]) {
	continue;
      }

      // Calculate kinematic parameters of the pair
      Double_t angle = p_neg.Angle(p_pos.Vect());
      Double_t minv = 2*TMath::Sin(angle/ 2)*TMath::Sqrt(momentum_neg.Mag()*momentum_pos.Mag());
      
      // Fill histogramms
      if((-1==motherID_neg) || (-1==motherID_pos)) {
	// bg + omega
	fh_tcut_epep_minv->Fill(minv, fWeight);
      } else {
	// bg
	fh_tcut_epep_minv->Fill(minv);
      }
      //------------------------------------------------------------------
    }//loop over positives
  }//loop over positives
  //------------------------------------------------------------------



  // ------------------------------------------------------------------
  // ------------- Opening angle cut e-e- pairs -----------------------
  // ------------------------------------------------------------------
  // Loop over negatives
  for(Int_t iNeg = 0; iNeg < nMCTrack; iNeg++) {
    trackNeg = (CbmMCTrack*) fArrayMCTrack->At(iNeg);
    
    // Select e-
    Int_t pdgCode_neg = trackNeg->GetPdgCode();
    if(11 != pdgCode_neg) {
      continue;
    }

    // Skip, if not accepted
    if(! mapAccepted[iNeg]) {
      continue;
    }
    if(! mapGamma[iNeg]) {
      continue;
    }
    if(! mapDsts[iNeg]) {
      continue;
    }
    if(! mapTp[iNeg]) {
      continue;
    }

    // Get motherID
    Int_t motherID_neg = trackNeg->GetMotherID();
    
    // If primary, motherPdg = 0
    Int_t motherPdg_neg = 0;
    if(motherID_neg > -1) {
      motherPdg_neg =
	((CbmMCTrack*)fArrayMCTrack->At(motherID_neg))->GetPdgCode();
    }

    // Get vertex, momentum
    TVector3 vertex_neg = trackNeg->GetStartVertex();
    TVector3 momentum_neg = mapMomentum[iNeg];
    Double_t energy_neg = TMath::Sqrt( momentum_neg.Mag2() + M2E );
    TLorentzVector p_neg(momentum_neg, energy_neg);

    // pt cut
    if(momentum_neg.Perp() < fPtcut) {
      continue;
    }

    // theta cut
    if(! mapTheta[iNeg]) {
      continue;
    }

    // Loop over negatives
    for(Int_t iPos = iNeg+1; iPos < nMCTrack; iPos++) {
      trackPos = (CbmMCTrack*) fArrayMCTrack->At(iPos);
 
      // Select e-
      Int_t pdgCode_pos = trackPos->GetPdgCode();
      if(11 != pdgCode_pos) {
	continue;
      }

      // Skip, if not accepted
      if(! mapAccepted[iPos]) {
	continue;
      }
      if(! mapGamma[iPos]) {
	continue;
      }
      if(! mapDsts[iPos]) {
	continue;
      }
      if(! mapTp[iPos]) {
	continue;
      }
      
      // Get motherID
      Int_t motherID_pos = trackPos->GetMotherID();
      
      // If primary, motherPdg = 0
      Int_t motherPdg_pos = 0;
      if(motherID_pos > -1) {
	motherPdg_pos =
	  ((CbmMCTrack*)fArrayMCTrack->At(motherID_pos))->GetPdgCode();
      }

      // Get vertex, momentum
      TVector3 vertex_pos = trackPos->GetStartVertex();
      TVector3 momentum_pos = mapMomentum[iPos];
      Double_t energy_pos = TMath::Sqrt( momentum_pos.Mag2() + M2E );
      TLorentzVector p_pos(momentum_pos, energy_pos);

      // pt cut
      if(momentum_pos.Perp() < fPtcut) {
	continue;
      }

      // theta cut
      if(! mapTheta[iPos]) {
	continue;
      }

      // Calculate kinematic parameters of the pair
      Double_t angle = p_neg.Angle(p_pos.Vect());
      Double_t minv = 2.*TMath::Sin(angle/2.)*TMath::Sqrt(momentum_neg.Mag()*momentum_pos.Mag());
            
      // Fill histogramms
      if((-1==motherID_neg) || (-1==motherID_pos)) {
	// bg + omega
	fh_tcut_emem_minv->Fill(minv, fWeight);
      } else {
	// bg
	fh_tcut_emem_minv->Fill(minv);
      }
      //------------------------------------------------------------------
    }//loop over negatives
  }//loop over negatives 
  //------------------------------------------------------------------



  // ------------------------------------------------------------------
  // ---------- pi0-Dalitz reconstruction (CERES cut ) ----------------
  // ------------------------------------------------------------------
  // Loop over negatives
  for(Int_t iNeg = 0; iNeg < nMCTrack; iNeg++) {
    trackNeg = (CbmMCTrack*) fArrayMCTrack->At(iNeg);

    // Get pdg code
    Int_t pdgCode_neg = trackNeg->GetPdgCode();
    if(11 != TMath::Abs(pdgCode_neg)) {
      continue;
    }
    
    // Skip, if not accepted
    if(! mapAccepted[iNeg]) {
      continue;
    }
    if(! mapGamma[iNeg]) {
      continue;
    }
    if(! mapDsts[iNeg]) {
      continue;
    }
    if(! mapTp[iNeg]) {
      continue;
    }  

    // Get motherID
    Int_t motherID_neg = trackNeg->GetMotherID();
    
    // If primary, motherPdg = 0
    Int_t motherPdg_neg = 0;
    if(motherID_neg > -1) {
      motherPdg_neg =
	((CbmMCTrack*)fArrayMCTrack->At(motherID_neg))->GetPdgCode();
    }

    // Get vertex, momentum
    TVector3 vertex_neg = trackNeg->GetStartVertex();
    TVector3 momentum_neg = mapMomentum[iNeg];
    Double_t energy_neg = TMath::Sqrt( momentum_neg.Mag2() + M2E );
    TLorentzVector p_neg(momentum_neg, energy_neg);

    // pt cut
    if(momentum_neg.Perp() < fPtcut) {
      continue;
    }

    // theta cut
    if(! mapTheta[iNeg]) {
      continue;
    }
    
    // Leptons multiplicities
    Int_t type;
    Double_t weight;
    GetType(motherID_neg, pdgCode_neg,
	    type, weight);
    if(-11 == pdgCode_neg) {
      n_pi0cut_lept[type][0] += weight;
    }
    if(11 == pdgCode_neg) {
      n_pi0cut_lept[type][1] += weight;
    }
    
    // Select e-
    if(11 != pdgCode_neg) {
      continue;
    }
    
    // Loop over positives
    for(Int_t iPos = 0; iPos < nMCTrack; iPos++) {
      trackPos = (CbmMCTrack*) fArrayMCTrack->At(iPos);
 
      // Select e+
      Int_t pdgCode_pos = trackPos->GetPdgCode();
      if(-11 != pdgCode_pos) {
	continue;
      }

      // Skip, if accepted
      if(! mapAccepted[iPos]) {
	continue;
      }
      if(! mapGamma[iPos]) {
	continue;
      }
      if(! mapDsts[iPos]) {
	continue;
      }
      if(! mapTp[iPos]) {
	continue;
      }  

      // Get motherID
      Int_t motherID_pos = trackPos->GetMotherID();
      
      // If primary, motherPdg = 0
      Int_t motherPdg_pos = 0;
      if(motherID_pos > -1) {
	motherPdg_pos =
	  ((CbmMCTrack*)fArrayMCTrack->At(motherID_pos))->GetPdgCode();
      }

      // Get vertex, momentum
      TVector3 vertex_pos = trackPos->GetStartVertex();
      TVector3 momentum_pos = mapMomentum[iPos];
      Double_t energy_pos = TMath::Sqrt( momentum_pos.Mag2() + M2E );
      TLorentzVector p_pos(momentum_pos, energy_pos);
      TVector3 momentum = momentum_pos + momentum_neg;
      Double_t energy = energy_pos + energy_neg;
      
      // pt cut
      if(momentum_pos.Perp() < fPtcut) {
	continue;
      }

      // theta cut
      if(! mapTheta[iPos]) {
	continue;
      }
      
      // Skip, if small minv
      if(! mapPi0[iNeg]) {
	continue;
      }
      if(! mapPi0[iPos]) {
	continue;
      }
      
      // Calculate kinematic parameters of the pair
      Double_t angle = p_neg.Angle(p_pos.Vect());
      Double_t minv = 2*TMath::Sin(angle/ 2)*TMath::Sqrt(momentum_neg.Mag()*momentum_pos.Mag());
      Double_t pt = momentum.Perp();
      Double_t pz = momentum.Pz();
      Double_t y = 0.5*TMath::Log((energy+pz)/ (energy-pz));

      // Fill histogramms
      GetType(motherID_pos, motherPdg_pos,
	      type, weight,
	      motherID_neg, motherPdg_neg,
	      kTRUE);

      fh_pi0cut_minv[type]->Fill(minv, weight);
      fh_pi0cut_pty[type]->Fill(y, pt, weight);
      n_pi0cut[type] += weight;


      if(motherID_neg == motherID_pos) {
	if(-1 == motherID_neg) {
	  fh_pi0cut_pt_minv_omega->Fill(minv, momentum.Perp(), fWeight);
	} else {
	  fh_pi0cut_pt_minv_pairs->Fill(minv, momentum.Perp());
	}
      }

      // Fill bg cocktail
      if(motherID_neg != motherID_pos) {
	
	if(((22==motherPdg_neg) && (111==motherPdg_pos)) ||
	   ((22==motherPdg_pos) && (111==motherPdg_neg))) {			
	  // gamma + pi0
	  fh_pi0cut_gamma_pi0_minv->Fill(minv);
	} else if(((22==motherPdg_neg) && (-1==motherID_pos)) ||
		  ((22==motherPdg_pos) && (-1==motherID_neg))) {
	  // gamma + omega
	  fh_pi0cut_gamma_omega_minv->Fill(minv, fWeight);
	} else if(((22==motherPdg_neg) && (221==motherPdg_pos)) ||
		  ((22==motherPdg_pos) && (221==motherPdg_neg))) {
	  // gamma + eta
	  fh_pi0cut_gamma_eta_minv->Fill(minv);
	} else if((22==motherPdg_neg) || (22==motherPdg_pos)) {
	  // gamma + gamma
	  fh_pi0cut_gamma_gamma_minv->Fill(minv);
	} else if(((111==motherPdg_neg) && (-1==motherID_pos)) ||
		  ((111==motherPdg_pos) && (-1==motherID_neg))) {
	  // pi0 + omega
	  fh_pi0cut_pi0_omega_minv->Fill(minv, fWeight);
	} else if(((111==motherPdg_neg) && (221==motherPdg_pos)) ||
		  ((111==motherPdg_pos) && (221==motherPdg_neg))) {
	  // pi0 + eta
	  fh_pi0cut_pi0_eta_minv->Fill(minv);
	} else if((111==motherPdg_neg) || (111==motherPdg_pos)) {
	  // pi0 + pi0
	  fh_pi0cut_pi0_pi0_minv->Fill(minv);
	} else if(((221==motherPdg_neg) && (-1==motherID_pos)) ||
		  ((221==motherPdg_pos) && (-1==motherID_neg))) {
	  // eta + omega
	  fh_pi0cut_eta_omega_minv->Fill(minv, fWeight);
	} else if((221==motherPdg_neg) || (221==motherPdg_pos)) {
	  // eta + eta
	  fh_pi0cut_eta_eta_minv->Fill(minv);
	}

      }
      
      // ----- pi0-Dalitz reconstruction --------
      if(minv < fPi0cut) {
	mapPi0[iNeg] = kFALSE;
	mapPi0[iPos] = kFALSE;
      }
      
    }//loop over positives
  }//loop over negatives
  // ------------------------------------------------------------------



  // ------------------------------------------------------------------
  // ------------- pi0-Dalitz reconstruction e+e+ pairs ---------------
  // ------------------------------------------------------------------

  for(Int_t i = 0; i < nMCTrack; i++) {
    mapPi0[i] = kTRUE;
  }
  
  // Loop over positives
  for(Int_t iNeg = 0; iNeg < nMCTrack; iNeg++) {
    trackNeg = (CbmMCTrack*) fArrayMCTrack->At(iNeg);
    
    // Select e+
    Int_t pdgCode_neg = trackNeg->GetPdgCode();
    if(-11 != pdgCode_neg) {
      continue;
    }

    // Skip, if not accepted
    if(! mapAccepted[iNeg]) {
      continue;
    }
    if(! mapGamma[iNeg]) {
      continue;
    }
    if(! mapDsts[iNeg]) {
      continue;
    }
    if(! mapTp[iNeg]) {
      continue;
    }

    // Get motherID
    Int_t motherID_neg = trackNeg->GetMotherID();
    
    // If primary, motherPdg = 0
    Int_t motherPdg_neg = 0;
    if(motherID_neg > -1) {
      motherPdg_neg =
	((CbmMCTrack*)fArrayMCTrack->At(motherID_neg))->GetPdgCode();
    }

    // Get vertex, momentum
    TVector3 vertex_neg = trackNeg->GetStartVertex();
    TVector3 momentum_neg = mapMomentum[iNeg];
    Double_t energy_neg = TMath::Sqrt( momentum_neg.Mag2() + M2E );
    TLorentzVector p_neg(momentum_neg, energy_neg);

    // pt cut
    if(momentum_neg.Perp() < fPtcut) {
      continue;
    }

    // theta cut
    if(! mapTheta[iNeg]) {
      continue;
    }

    // Loop over positives
    for(Int_t iPos = iNeg+1; iPos < nMCTrack; iPos++) {
      trackPos = (CbmMCTrack*) fArrayMCTrack->At(iPos);
 
      // Select e+
      Int_t pdgCode_pos = trackPos->GetPdgCode();
      if(-11 != pdgCode_pos) {
	continue;
      }


      // Skip, if not accepted
      if(! mapAccepted[iPos]) {
	continue;
      }
      if(! mapGamma[iPos]) {
	continue;
      }
      if(! mapDsts[iPos]) {
	continue;
      }
      if(! mapTp[iPos]) {
	continue;
      }

      // Get motherID
      Int_t motherID_pos = trackPos->GetMotherID();
      
      // If primary, motherPdg = 0
      Int_t motherPdg_pos = 0;
      if(motherID_pos > -1) {
	motherPdg_pos =
	  ((CbmMCTrack*)fArrayMCTrack->At(motherID_pos))->GetPdgCode();
      }

      // Get vertex, momentum and energy
      TVector3 vertex_pos = trackPos->GetStartVertex();
      TVector3 momentum_pos = mapMomentum[iPos];
      Double_t energy_pos = TMath::Sqrt( momentum_pos.Mag2() + M2E );
      TLorentzVector p_pos(momentum_pos, energy_pos);

      // pt cut
      if(momentum_pos.Perp() < fPtcut) {
	continue;
      }

      // theta cut
      if(! mapTheta[iPos]) {
	continue;
      }
      
      // Skip, if small minv
      if(! mapPi0[iNeg]) {
	continue;
      }
      if(! mapPi0[iPos]) {
	continue;
      }

      // Calculate kinematic parameters of the pair
      Double_t angle = p_neg.Angle(p_pos.Vect());
      Double_t minv = 2*TMath::Sin(angle/ 2)*TMath::Sqrt(momentum_neg.Mag()*momentum_pos.Mag());
      
      // Fill histogramms
      if((-1==motherID_neg) || (-1==motherID_pos)) {
	// bg + omega
	fh_pi0cut_epep_minv->Fill(minv, fWeight);
      } else {
	// bg
	fh_pi0cut_epep_minv->Fill(minv);
      }
      //------------------------------------------------------------------

      // ----- pi0 dalitz cut --------
      if(minv < fPi0cut) {
	mapPi0[iNeg] = kFALSE;
	mapPi0[iPos] = kFALSE;
      }

    }//loop over positives
  }//loop over positives
  //------------------------------------------------------------------



  // ------------------------------------------------------------------
  // ------------- pi0-Dalitz reconstruction e-e- pairs ---------------
  // ------------------------------------------------------------------

  for(Int_t i = 0; i < nMCTrack; i++) {
    mapPi0[i] = kTRUE;
  }
  
  // Loop over negatives
  for(Int_t iNeg = 0; iNeg < nMCTrack; iNeg++) {
    trackNeg = (CbmMCTrack*) fArrayMCTrack->At(iNeg);
    
    // Select e-
    Int_t pdgCode_neg = trackNeg->GetPdgCode();
    if(11 != pdgCode_neg) {
      continue;
    }

    // Skip, if not accepted
    if(! mapAccepted[iNeg]) {
      continue;
    }
    if(! mapGamma[iNeg]) {
      continue;
    }
    if(! mapDsts[iNeg]) {
      continue;
    }
    if(! mapTp[iNeg]) {
      continue;
    }

    // Get motherID
    Int_t motherID_neg = trackNeg->GetMotherID();
    
    // If primary, motherPdg = 0
    Int_t motherPdg_neg = 0;
    if(motherID_neg > -1) {
      motherPdg_neg =
	((CbmMCTrack*)fArrayMCTrack->At(motherID_neg))->GetPdgCode();
    }

    // Get vertex, momentum
    TVector3 vertex_neg = trackNeg->GetStartVertex();
    TVector3 momentum_neg = mapMomentum[iNeg];
    Double_t energy_neg = TMath::Sqrt( momentum_neg.Mag2() + M2E );
    TLorentzVector p_neg(momentum_neg, energy_neg);

    // pt cut
    if(momentum_neg.Perp() < fPtcut) {
      continue;
    }

    // theta cut
    if(! mapTheta[iNeg]) {
      continue;
    }

    // Loop over negatives
    for(Int_t iPos = iNeg+1; iPos < nMCTrack; iPos++) {
      trackPos = (CbmMCTrack*) fArrayMCTrack->At(iPos);

      // Select e-
      Int_t pdgCode_pos = trackPos->GetPdgCode();
      if(11 != pdgCode_pos) {
	continue;
      }

      // Skip, if not accepted
      if(! mapAccepted[iPos]) {
	continue;
      }
      if(! mapGamma[iPos]) {
	continue;
      }
      if(! mapDsts[iPos]) {
	continue;
      }
      if(! mapTp[iPos]) {
	continue;
      }
      
      // Get motherID
      Int_t motherID_pos = trackPos->GetMotherID();
      
      // If primary, motherPdg = 0
      Int_t motherPdg_pos = 0;
      if(motherID_pos > -1) {
	motherPdg_pos =
	  ((CbmMCTrack*)fArrayMCTrack->At(motherID_pos))->GetPdgCode();
      }

      // Get vertex, momentum
      TVector3 vertex_pos = trackPos->GetStartVertex();
      TVector3 momentum_pos = mapMomentum[iPos];
      Double_t energy_pos = TMath::Sqrt( momentum_pos.Mag2() + M2E );
      TLorentzVector p_pos(momentum_pos, energy_pos);

      // pt cut
      if(momentum_pos.Perp() < fPtcut) {
	continue;
      }

      // theta cut
      if(! mapTheta[iPos]) {
	continue;
      }

      // Skip. if small minv
      if(! mapPi0[iNeg]) {
	continue;
      }
      if(! mapPi0[iPos]) {
	continue;
      }

      // Calculate kinematic parameters of the pair
      Double_t angle = p_neg.Angle(p_pos.Vect());
      Double_t minv = 2.*TMath::Sin(angle/2.)*TMath::Sqrt(momentum_neg.Mag()*momentum_pos.Mag());
            
      // Fill histogramms
      if((-1==motherID_neg) || (-1==motherID_pos)) {
	// bg + omega
	fh_pi0cut_emem_minv->Fill(minv, fWeight);
      } else {
	// bg
	fh_pi0cut_emem_minv->Fill(minv);
      }
      //------------------------------------------------------------------

      // ----- pi0 dalitz cut --------
      if(minv < fPi0cut) {
	mapPi0[iNeg] = kFALSE;
	mapPi0[iPos] = kFALSE;
      }

    }//loop over negatives
  }//loop over negatives
  //------------------------------------------------------------------



  ADD2(fn_all_lept, n_all_lept, NTYPES, 2);
  ADD(fn_all, n_all, NTYPES);

  ADD2(fn_acc_lept, n_acc_lept, NTYPES, 2);
  ADD(fn_acc, n_acc, NTYPES);

  ADD2(fn_gcut_lept, n_gcut_lept, NTYPES, 2);
  ADD(fn_gcut, n_gcut, NTYPES);

  ADD2(fn_dcut_lept, n_dcut_lept, NTYPES, 2);
  ADD(fn_dcut, n_dcut, NTYPES);

  ADD2(fn_tpcut_lept, n_tpcut_lept, NTYPES, 2);
  ADD(fn_tpcut, n_tpcut, NTYPES);

  ADD2(fn_ptcut_lept, n_ptcut_lept, NTYPES, 2);
  ADD(fn_ptcut, n_ptcut, NTYPES);

  ADD2(fn_tcut_lept, n_tcut_lept, NTYPES, 2);
  ADD(fn_tcut, n_tcut, NTYPES);

  ADD2(fn_pi0cut_lept, n_pi0cut_lept, NTYPES, 2);
  ADD(fn_pi0cut, n_pi0cut, NTYPES);

  fn_tpcut_omega_cut += n_tpcut_omega_cut;
  fn_tpcut_pi0_cut += n_tpcut_pi0_cut;
  
  
  // Fill multiplicities histograms
  for(Int_t iT = 0; iT < NTYPES; iT++) {

    fh_all_mult[iT]->Fill(n_all_lept[iT][0], n_all_lept[iT][1]);
    fh_acc_mult[iT]->Fill(n_acc_lept[iT][0], n_acc_lept[iT][1]);
    fh_gcut_mult[iT]->Fill(n_gcut_lept[iT][0], n_gcut_lept[iT][1]);
    fh_dcut_mult[iT]->Fill(n_dcut_lept[iT][0], n_dcut_lept[iT][1]);
    fh_tpcut_mult[iT]->Fill(n_tpcut_lept[iT][0], n_tpcut_lept[iT][1]);
    fh_ptcut_mult[iT]->Fill(n_ptcut_lept[iT][0], n_ptcut_lept[iT][1]);
    fh_tcut_mult[iT]->Fill(n_tcut_lept[iT][0], n_tcut_lept[iT][1]);
    fh_pi0cut_mult[iT]->Fill(n_pi0cut_lept[iT][0], n_pi0cut_lept[iT][1]);
  }

  fEvents = fEvents + 1;

}
//------------------------------------------------------------------



//------------------------------------------------------------------
void CbmDiLeptTask::Finish()
{

}
//------------------------------------------------------------------



//------------------------------------------------------------------
void CbmDiLeptTask::WriteOutput()
{
  // Loop over neighbour type
  for(Int_t iTypeN = 0; iTypeN < NTYPES_NEIGHB; iTypeN++) {
    fh_acc_d[iTypeN]->Write();
    fh_dcut_theta_minv[iTypeN]->Write();
    fh_dcut_theta_sqrt_mom[iTypeN]->Write();
  }

  // Loop over types
  for(Int_t iType = 0; iType < NTYPES; iType++) {

    for(Int_t iD = 0; iD < 2; iD++) {
      fh_all_zr_lept[iType][iD]->Write();
      fh_all_mom_lept[iType][iD]->Write();
      fh_acc_mom_lept[iType][iD]->Write();
      fh_all_pt_lept[iType][iD]->Write();
      fh_acc_pt_lept[iType][iD]->Write();
      fh_tpcut_pt_lept[iType][iD]->Write();
      fh_all_pty_lept[iType][iD]->Write();
      fh_acc_pty_lept[iType][iD]->Write();
    }

    for(Int_t iStation = 0; iStation < 7; iStation++) {
      fh_sts_ep_mom[iStation][iType]->Write();
    }

    fh_all_nsts[iType]->Write();
    fh_all_pty[iType]->Write();
    fh_acc_pty[iType]->Write();
    fh_pi0cut_pty[iType]->Write();

    fh_all_epem_mom[iType]->Write();
    fh_acc_epem_mom[iType]->Write();
    fh_all_mom[iType]->Write();
    fh_acc_mom[iType]->Write();

    fh_all_theta[iType]->Write();
    fh_acc_theta[iType]->Write();
    fh_ptcut_theta[iType]->Write();

    fh_dcut_theta_sqrt_mom_true[iType]->Write();

    fh_all_mult[iType]->Write();
    fh_acc_mult[iType]->Write();
    fh_gcut_mult[iType]->Write();
    fh_dcut_mult[iType]->Write();
    fh_tpcut_mult[iType]->Write();
    fh_ptcut_mult[iType]->Write();
    fh_tcut_mult[iType]->Write();
    fh_pi0cut_mult[iType]->Write();

    fh_all_minv[iType]->Write();
    fh_acc_minv[iType]->Write();
    fh_gcut_minv[iType]->Write();
    fh_dcut_minv[iType]->Write();
    fh_tpcut_minv[iType]->Write();
    fh_ptcut_minv[iType]->Write();
    fh_tcut_minv[iType]->Write();
    fh_pi0cut_minv[iType]->Write();
  }
  // ----------------------------------------------------------

  // momentum resolution
  fh_dp->Write();

  // d sts cocktail
  fh_acc_d_gamma_gamma->Write();
  fh_acc_d_pi0_pi0->Write();

  // like-sign pairs
  fh_acc_epep_minv->Write();
  fh_acc_emem_minv->Write();
  fh_gcut_epep_minv->Write();
  fh_gcut_emem_minv->Write();
  fh_dcut_epep_minv->Write();
  fh_dcut_emem_minv->Write();
  fh_tpcut_epep_minv->Write();
  fh_tpcut_emem_minv->Write();
  fh_ptcut_epep_minv->Write();
  fh_ptcut_emem_minv->Write();
  fh_tcut_epep_minv->Write();
  fh_tcut_emem_minv->Write();
  fh_pi0cut_epep_minv->Write();
  fh_pi0cut_emem_minv->Write();

  // background cocktail
  fh_pi0cut_gamma_pi0_minv->Write();
  fh_pi0cut_gamma_omega_minv->Write();
  fh_pi0cut_gamma_eta_minv->Write();
  fh_pi0cut_gamma_gamma_minv->Write();
  fh_pi0cut_pi0_omega_minv->Write();
  fh_pi0cut_pi0_eta_minv->Write();
  fh_pi0cut_pi0_pi0_minv->Write();
  fh_pi0cut_eta_omega_minv->Write();
  fh_pi0cut_eta_eta_minv->Write();

  // pair acceptance
  fh_all_pt_minv_pairs->Write();
  fh_all_pt_minv_omega->Write();
  fh_acc_pt_minv_pairs->Write();
  fh_acc_pt_minv_omega->Write();
  fh_pi0cut_pt_minv_pairs->Write();
  fh_pi0cut_pt_minv_omega->Write();

  // pi0-Dalitz background
  fh_all_pi0_bg_theta->Write();
  fh_acc_pi0_bg_theta->Write();
  fh_all_pi0_bg_minv->Write();
  fh_acc_pi0_bg_minv->Write();

  // STS 1 multiplicity
  fh_all_omega_tot_mult->Write();
  fh_acc_omega_tot_mult->Write();
  fh_dcut_omega_tot_mult->Write();
  // ----------------------------------------------------------


/*
  cout << endl << endl
       << "--------------------------------------------------" << endl
       << "-I- CbmDiLeptTask : ALL events statistics" << endl
       << endl
       << "                 Signal " << endl
       << "              all e+: " << fn_all_lept[0][0]    << endl
       << "              acc e+: " << fn_acc_lept[0][0]    << endl
       << "        gamma cut e+: " << fn_gcut_lept[0][0]   << endl
       << "        d sts cut e+: " << fn_dcut_lept[0][0]   << endl
       << "         minv cut e+: " << fn_tpcut_lept[0][0]  << endl
       << "           pt cut e+: " << fn_ptcut_lept[0][0]  << endl
       << "        theta cut e+: " << fn_tcut_lept[0][0]   << endl
       << "          pi0 cut e+: " << fn_pi0cut_lept[0][0] << endl
       << endl
       << "              all e-: " << fn_all_lept[0][1]    << endl
       << "              acc e-: " << fn_acc_lept[0][1]    << endl
       << "        gamma cut e-: " << fn_gcut_lept[0][1]   << endl
       << "        d sts cut e-: " << fn_dcut_lept[0][1]   << endl
       << "         minv cut e-: " << fn_tpcut_lept[0][1]  << endl
       << "           pt cut e-: " << fn_ptcut_lept[0][1]  << endl
       << "        theta cut e-: " << fn_tcut_lept[0][1]   << endl
       << "          pi0 cut e-: " << fn_pi0cut_lept[0][1] << endl
       << endl
       << "                all : " << fn_all[0]    << endl
       << "                acc : " << fn_acc[0]    << endl
       << "          gamma cut : " << fn_gcut[0]   << endl
       << "          d sts cut : " << fn_dcut[0]   << endl
       << "           minv cut : " << fn_tpcut[0]  << endl
       << "             pt cut : " << fn_ptcut[0]  << endl
       << "          theta cut : " << fn_tcut[0]   << endl
       << "            pi0 cut : " << fn_pi0cut[0] << endl
       << "--------------------------------------------------" << endl
       << endl
       << "             Background " << endl
       << "              all e+: " << fn_all_lept[5][0]    << endl
       << "              acc e+: " << fn_acc_lept[5][0]    << endl
       << "        gamma cut e+: " << fn_gcut_lept[5][0]   << endl
       << "        d sts cut e+: " << fn_dcut_lept[5][0]   << endl
       << "         minv cut e+: " << fn_tpcut_lept[5][0]  << endl
       << "           pt cut e+: " << fn_ptcut_lept[5][0]  << endl
       << "        theta cut e+: " << fn_tcut_lept[5][0]   << endl
       << "          pi0 cut e+: " << fn_pi0cut_lept[5][0] << endl
       << endl
       << "              all e-: " << fn_all_lept[5][1]    << endl
       << "              acc e-: " << fn_acc_lept[5][1]    << endl
       << "        gamma cut e-: " << fn_gcut_lept[5][1]   << endl
       << "        d sts cut e-: " << fn_dcut_lept[5][1]   << endl
       << "         minv cut e-: " << fn_tpcut_lept[5][1]  << endl
       << "           pt cut e-: " << fn_ptcut_lept[5][1]  << endl
       << "        theta cut e-: " << fn_tcut_lept[5][1]   << endl
       << "          pi0 cut e-: " << fn_pi0cut_lept[5][1] << endl
       << endl
       << "                all : " << fn_all[5]    << endl
       << "                acc : " << fn_acc[5]    << endl
       << "          gamma cut : " << fn_gcut[5]   << endl
       << "          d sts cut : " << fn_dcut[5]   << endl
       << "           minv cut : " << fn_tpcut[5]  << endl
       << "             pt cut : " << fn_ptcut[5]  << endl
       << "          theta cut : " << fn_tcut[5]   << endl
       << "            pi0 cut : " << fn_pi0cut[5] << endl
       << "--------------------------------------------------" << endl
       << endl
       << "                  Gamma " << endl
       << "              all e+: " << fn_all_lept[2][0]    << endl
       << "              acc e+: " << fn_acc_lept[2][0]    << endl
       << "        gamma cut e+: " << fn_gcut_lept[2][0]   << endl
       << "        d sts cut e+: " << fn_dcut_lept[2][0]   << endl
       << "         minv cut e+: " << fn_tpcut_lept[2][0]  << endl
       << "           pt cut e+: " << fn_ptcut_lept[2][0]  << endl
       << "        theta cut e+: " << fn_tcut_lept[2][0]   << endl
       << "          pi0 cut e+: " << fn_pi0cut_lept[2][0] << endl
       << endl
       << "              all e-: " << fn_all_lept[2][1]    << endl
       << "              acc e-: " << fn_acc_lept[2][1]    << endl
       << "        gamma cut e-: " << fn_gcut_lept[2][1]   << endl
       << "        d sts cut e-: " << fn_dcut_lept[2][1]   << endl
       << "         minv cut e-: " << fn_tpcut_lept[2][1]  << endl
       << "           pt cut e-: " << fn_ptcut_lept[2][1]  << endl
       << "        theta cut e-: " << fn_tcut_lept[2][1]   << endl
       << "          pi0 cut e-: " << fn_pi0cut_lept[2][1] << endl
       << endl
       << "                all : " << fn_all[2]    << endl
       << "                acc : " << fn_acc[2]    << endl
       << "          gamma cut : " << fn_gcut[2]   << endl
       << "          d sts cut : " << fn_dcut[2]   << endl
       << "           minv cut : " << fn_tpcut[2]  << endl
       << "             pt cut : " << fn_ptcut[2]  << endl
       << "          theta cut : " << fn_tcut[2]   << endl
       << "            pi0 cut : " << fn_pi0cut[2] << endl
       << "--------------------------------------------------" << endl
       << endl
       << "                    pi0 " << endl
       << "              all e+: " << fn_all_lept[3][0]    << endl
       << "              acc e+: " << fn_acc_lept[3][0]    << endl
       << "        gamma cut e+: " << fn_gcut_lept[3][0]   << endl
       << "        d sts cut e+: " << fn_dcut_lept[3][0]   << endl
       << "         minv cut e+: " << fn_tpcut_lept[3][0]  << endl
       << "           pt cut e+: " << fn_ptcut_lept[3][0]  << endl
       << "        theta cut e+: " << fn_tcut_lept[3][0]   << endl
       << "          pi0 cut e+: " << fn_pi0cut_lept[3][0] << endl
       << endl
       << "              all e-: " << fn_all_lept[3][1]    << endl
       << "              acc e-: " << fn_acc_lept[3][1]    << endl
       << "        gamma cut e-: " << fn_gcut_lept[3][1]   << endl
       << "        d sts cut e-: " << fn_dcut_lept[3][1]   << endl
       << "         minv cut e-: " << fn_tpcut_lept[3][1]  << endl
       << "           pt cut e-: " << fn_ptcut_lept[3][1]  << endl
       << "        theta cut e-: " << fn_tcut_lept[3][1]   << endl
       << "          pi0 cut e-: " << fn_pi0cut_lept[3][1] << endl
       << endl
       << "                all : " << fn_all[3]    << endl
       << "                acc : " << fn_acc[3]    << endl
       << "          gamma cut : " << fn_gcut[3]   << endl
       << "          d sts cut : " << fn_dcut[3]   << endl
       << "           minv cut : " << fn_tpcut[3]  << endl
       << "             pt cut : " << fn_ptcut[3]  << endl
       << "          theta cut : " << fn_tcut[3]   << endl
       << "            pi0 cut : " << fn_pi0cut[3] << endl
       << "--------------------------------------------------" << endl
       << endl
       << "                    eta " << endl
       << "              all e+: " << fn_all_lept[1][0]    << endl
       << "              acc e+: " << fn_acc_lept[1][0]    << endl
       << "        gamma cut e+: " << fn_gcut_lept[1][0]   << endl
       << "        d sts cut e+: " << fn_dcut_lept[1][0]   << endl
       << "         minv cut e+: " << fn_tpcut_lept[1][0]  << endl
       << "           pt cut e+: " << fn_ptcut_lept[1][0]  << endl
       << "        theta cut e+: " << fn_tcut_lept[1][0]   << endl
       << "          pi0 cut e+: " << fn_pi0cut_lept[1][0] << endl
       << endl
       << "              all e-: " << fn_all_lept[1][1]    << endl
       << "              acc e-: " << fn_acc_lept[1][1]    << endl
       << "        gamma cut e-: " << fn_gcut_lept[1][1]   << endl
       << "        d sts cut e-: " << fn_dcut_lept[1][1]   << endl
       << "         minv cut e-: " << fn_tpcut_lept[1][1]  << endl
       << "           pt cut e-: " << fn_ptcut_lept[1][1]  << endl
       << "        theta cut e-: " << fn_tcut_lept[1][1]   << endl
       << "          pi0 cut e-: " << fn_pi0cut_lept[1][1] << endl
       << endl
       << "                all : " << fn_all[1] << endl
       << "                acc : " << fn_acc[1] << endl
       << "          gamma cut : " << fn_gcut[1] << endl
       << "          d sts cut : " << fn_dcut[1] << endl
       << "           minv cut : " << fn_tpcut[1] << endl
       << "             pt cut : " << fn_ptcut[1] << endl
       << "          theta cut : " << fn_tcut[1] << endl
       << "            pi0 cut : " << fn_pi0cut[1] << endl
       << "--------------------------------------------------" << endl
       << endl << endl;
*/
}



//------------------------------------------------------------------
void CbmDiLeptTask::GetType(const Int_t& motherID_pos, const Int_t& motherPdg_pos,
			    Int_t& type, Double_t& weight,
			    const Int_t& motherID_neg, const Int_t& motherPdg_neg,
			    const Bool_t& isPair)
{
    if(isPair) {// track pair

	if((-1==motherID_neg) && (-1==motherID_pos)) {
	    // omega
	    type = 0;
	    weight = fWeight;
	} else if((221==motherPdg_neg) && (motherID_neg==motherID_pos)) {
	    // eta
	    type = 1;
	    weight = 1.;
	} else if((22==motherPdg_neg) && (motherID_neg==motherID_pos)) {
	    // gamma
	    type = 2;
	    weight = 1.;
	} else if((111==motherPdg_neg) && (motherID_neg==motherID_pos)) {
	    // pi0
	    type = 3;
	    weight = 1.;
	} else if((-1==motherID_neg) || (-1==motherID_pos)) {
	    if((-1==motherID_neg) && (-1==motherID_pos)) {
	    }
	    // bg + omega
	    type = 4;
	    weight = fWeight;
	} else {
	    // bg
	    type = 5;
	    weight = 1.;
	}

    } else { // single track

	if(-1 == motherID_pos) {
	    type = 0;
	    weight = fWeight;
	} else if(221 == motherPdg_pos) {
	    type = 1;
	    weight = 1.;
	} else if(22 == motherPdg_pos) {
	    type = 2;
	    weight = 1.;
	} else if(111 == motherPdg_pos) {
	    type = 3;
	    weight = 1.;
	} else {
	    type = 5;
	    weight = 1.;
	}
    }
}
