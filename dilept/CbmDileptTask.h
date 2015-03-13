//*******************************************//
//***                                     ***//
//***        CbmDiLeptTask.h              ***//
//***           T.Galatyuk                ***//
//***                                     ***//
//*******************************************//

#ifndef CBM_DI_LEPT_TASK_H
#define CBM_DI_LEPT_TASK_H

#include "FairTask.h"
#include "FairBaseParSet.h"
#include "CbmMCTrack.h"

#include "TString.h"

class TClonesArray;
class TH2D;
class TH1D;
class TH2F;
class FairRootManager;


/**
 * @class CbmDiLeptTask
 * @author T.Galatyuk
 * @version 1
 * @since 2006-May-29
 */


class CbmDiLeptTask : public FairTask {

public:
    /**
     *Default constructor
     */
    CbmDiLeptTask();

    /**
     * Standart constructor
     */
    CbmDiLeptTask(const char *name);

    /**
     * Destructor
     */
    virtual ~CbmDiLeptTask();

    /**
     * Initialise
     */
    InitStatus Init(); //*MENU

    /**
     * Process the input
     */
    virtual void Exec(Option_t *option);

    /**
     * Finalise the processing of the input
     */
    virtual void Finish(); //*MENU

    /**
     * Writes the output to file
     */
    void WriteOutput();

    void SetParContainers();

    ClassDef(CbmDiLeptTask,1);


private:

    FairRootManager *fRootManager;

    TClonesArray *fArrayMCTrack;
    TClonesArray *fArrayStsPoint;
    TClonesArray *fArrayRichPoint;

    FairBaseParSet *fBasePar;
    FairPrimaryGenerator *fPrimGen;

    Int_t fEvents;


    void GetType(const Int_t& motherID_pos, const Int_t& motherPdg_pos,
		 Int_t& type, Double_t& weight,
		 const Int_t& motherID_neg = 0, const Int_t& motherPdg_neg = 0,
		 const Bool_t& isPair = kFALSE);


#define NTYPES 6
#define NTYPES_NEIGHB 4

#define ZERO(p,n) {for(Int_t i=0;i<n;i++){p[i]=0;}}
#define ZERO2(p,n,m) {for(Int_t i=0;i<n;i++)for(Int_t j=0;j<m;j++){p[i][j]=0;}}

#define ADD(p,q,n) {for(Int_t i=0;i<n;i++){p[i]+=q[i];}}
#define ADD2(p,q,n,m) {for(Int_t i=0;i<n;i++)for(Int_t j=0;j<m;j++){p[i][j]+=q[i][j];}}

    Double_t fn_all_lept[NTYPES][2];
    Double_t fn_all[NTYPES];

    Double_t fn_acc_lept[NTYPES][2];
    Double_t fn_acc[NTYPES];

    Double_t fn_gcut_lept[NTYPES][2];
    Double_t fn_gcut[NTYPES];

    Double_t fn_dcut_lept[NTYPES][2];
    Double_t fn_dcut[NTYPES];

    Double_t fn_tpcut_lept[NTYPES][2];
    Double_t fn_tpcut[NTYPES];

    Double_t fn_ptcut_lept[NTYPES][2];
    Double_t fn_ptcut[NTYPES];

    Double_t fn_tcut_lept[NTYPES][2];
    Double_t fn_tcut[NTYPES];

    Double_t fn_pi0cut_lept[NTYPES][2];
    Double_t fn_pi0cut[NTYPES];

    Double_t fn_tpcut_omega_cut;
    Double_t fn_tpcut_pi0_cut;

    // d sts
    TH1D* fh_acc_d[NTYPES_NEIGHB];
    TH1D* fh_acc_d_gamma_gamma;
    TH1D* fh_acc_d_pi0_pi0;

    // (theta + minv) cut
    TH2D* fh_dcut_theta_minv[NTYPES_NEIGHB];
    TH2D* fh_dcut_theta_sqrt_mom[NTYPES_NEIGHB];
    TH2D* fh_dcut_theta_sqrt_mom_true[NTYPES];

    // radial distace vs. z
    TH2D* fh_all_zr_lept[NTYPES][2];

    // momentum
    TH1D* fh_all_mom_lept[NTYPES][2];
    TH1D* fh_acc_mom_lept[NTYPES][2];
    TH1D* fh_all_mom[NTYPES];
    TH1D* fh_acc_mom[NTYPES];
    TH2D* fh_all_epem_mom[NTYPES];
    TH2D* fh_acc_epem_mom[NTYPES];
    TH1D* fh_sts_ep_mom[7][NTYPES];

    // pt
    TH1D* fh_all_pt_lept[NTYPES][2];
    TH1D* fh_acc_pt_lept[NTYPES][2];
    TH1D* fh_tpcut_pt_lept[NTYPES][2];

    // number of sts for e+, e-
    TH2D* fh_all_nsts[NTYPES];

    // pt vs. rapidity
    TH2D* fh_all_pty_lept[NTYPES][2];
    TH2D* fh_acc_pty_lept[NTYPES][2];
    TH2D* fh_all_pty[NTYPES];
    TH2D* fh_acc_pty[NTYPES];
    TH2D* fh_pi0cut_pty[NTYPES];

    // opening angle
    TH1D* fh_all_theta[NTYPES];
    TH1D* fh_acc_theta[NTYPES];
    TH1D* fh_ptcut_theta[NTYPES];

    // lepton multiplicities
    TH2D* fh_all_mult[NTYPES];
    TH2D* fh_acc_mult[NTYPES];
    TH2D* fh_gcut_mult[NTYPES];
    TH2D* fh_dcut_mult[NTYPES];
    TH2D* fh_tpcut_mult[NTYPES];
    TH2D* fh_ptcut_mult[NTYPES];
    TH2D* fh_tcut_mult[NTYPES];
    TH2D* fh_pi0cut_mult[NTYPES];

    // invariant mass
    TH1D* fh_all_minv[NTYPES];
    TH1D* fh_acc_minv[NTYPES];
    TH1D* fh_gcut_minv[NTYPES];
    TH1D* fh_dcut_minv[NTYPES];
    TH1D* fh_tpcut_minv[NTYPES];
    TH1D* fh_ptcut_minv[NTYPES];
    TH1D* fh_tcut_minv[NTYPES];
    TH1D* fh_pi0cut_minv[NTYPES];

    // momentum resolution
    TH1D* fh_dp;

    // like-sign pairs
    TH1D* fh_acc_epep_minv;
    TH1D* fh_acc_emem_minv;
    TH1D* fh_gcut_epep_minv;
    TH1D* fh_gcut_emem_minv;
    TH1D* fh_dcut_epep_minv;
    TH1D* fh_dcut_emem_minv;
    TH1D* fh_tpcut_epep_minv;
    TH1D* fh_tpcut_emem_minv;
    TH1D* fh_ptcut_epep_minv;
    TH1D* fh_ptcut_emem_minv;
    TH1D* fh_tcut_epep_minv;
    TH1D* fh_tcut_emem_minv;
    TH1D* fh_pi0cut_epep_minv;
    TH1D* fh_pi0cut_emem_minv;

    // background cocktail
    TH1D* fh_pi0cut_gamma_pi0_minv;
    TH1D* fh_pi0cut_gamma_omega_minv;
    TH1D* fh_pi0cut_gamma_eta_minv;
    TH1D* fh_pi0cut_gamma_gamma_minv;
    TH1D* fh_pi0cut_pi0_omega_minv;
    TH1D* fh_pi0cut_pi0_eta_minv;
    TH1D* fh_pi0cut_pi0_pi0_minv;
    TH1D* fh_pi0cut_eta_omega_minv;
    TH1D* fh_pi0cut_eta_eta_minv;

    // pair acceptance
    TH2D* fh_all_pt_minv_pairs;
    TH2D* fh_all_pt_minv_omega;
    TH2D* fh_acc_pt_minv_pairs;
    TH2D* fh_acc_pt_minv_omega;
    TH2D* fh_pi0cut_pt_minv_pairs;
    TH2D* fh_pi0cut_pt_minv_omega;

    // pi0-Dalitz background
    TH1D* fh_all_pi0_bg_theta;
    TH1D* fh_acc_pi0_bg_theta;
    TH1D* fh_all_pi0_bg_minv;
    TH1D* fh_acc_pi0_bg_minv;

    // <Multiplicities> / STS
    TH1D* fh_all_omega_tot_mult;
    TH1D* fh_acc_omega_tot_mult;
    TH1D* fh_dcut_omega_tot_mult;


    Int_t fCoutBunch;

    Double_t fGcut;
    Double_t fDcut;
    Double_t f2Dtcut;
    Double_t f2Dpcut;
    Double_t fPtcut;
    Double_t fTcut;
    Double_t fPi0cut;

    Double_t fWeight;

    /**
     * Create the functions
     */

    void CreateHistogramms();

public:

    inline void SetCoutBunch(Int_t coutBunch) {fCoutBunch = coutBunch;};

    // Cuts value
    inline void SetGcut (Double_t gcut) {fGcut = gcut;};
    inline void SetDcut (Double_t dcut) {fDcut = dcut;};
    inline void Set2Dtcut (Double_t dtcut) {f2Dtcut = dtcut;};
    inline void Set2Dpcut (Double_t dpcut) {f2Dpcut = dpcut;};
    inline void SetTcut (Double_t tcut) {fTcut = tcut;};
    inline void SetPtcut (Double_t ptcut) {fPtcut = ptcut;};
    inline void SetPi0cut (Double_t pi0cut) {fPi0cut = pi0cut;};

    inline void SetWeight (Double_t weight) {fWeight = weight;};

    void DrawPoints();
};

#endif
