// ------------------------------------------------------------------
// -----                      CbmEbefluctPYR                    -----
// -----             Created 2004-Sep-23 by D.Kresan            -----
// ------------------------------------------------------------------
#ifndef CBM_EBEFLUCT_PYR_H
#define CBM_EBEFLUCT_PYR_H

#include "FairTask.h"

class TFile;
class TClonesArray;
class TF1;
class TH1F;
class TH2F;


class CbmEbefluctPYR : public FairTask {

private:
    Int_t         fLevel;             // Level: 1 - all,acc; 2 - reco+PID
    Int_t         fEvents;            // Number of events processed
    Int_t         fCoutBunch;         // Size of output bunch
    Int_t         fNbinsRatio;        // Number of bins in ratio distrib.
    TClonesArray *fArrayMCTrack;      // MC tracks
    TClonesArray *fArrayTofPoint;     // TOF points
    TClonesArray *fArrayHadron;       // Hadrons

    TH1F         *fh_all_mom_kap;
    TH1F         *fh_acc_mom_kap;
    TH1F         *fh_pid_mom_kap;
    TH1F         *fh_rpid_mom_kap;

    TH1F         *fh_all_nka;
    TH1F         *fh_acc_nka;
    TH1F         *fh_pid_nka;
    TH1F         *fh_rpid_nka;
    TH1F         *fh_rpid_nka_true;


    Double_t fn_ka_all;
    Double_t fn_ka_true;


    TH1F         *fh_all_ka2pi;       // K/pi ratio (all particles)
    TH1F         *fh_acc_ka2pi;       // K/pi ratio (accepted particles)
    TH1F         *fh_pid_ka2pi;       // K/pi ratio (accepted particles + pcut)
    TH1F         *fh_rpid_ka2pi;      // K/pi ratio (identified particles)
    TH1F         *fh_rpid_ka2pi_true; // K/pi ratio (MC truth)
    TH1F         *fh_rpid_ka2pi_true_10;
    TH1F         *fh_rpid_ka2pi_true_9;
    TH1F         *fh_rpid_ka2pi_true_8;
    TH1F         *fh_rpid_ka2pi_true_7;
    TH1F         *fh_rpid_ka2pi_true_6;
    TH1F         *fh_rpid_ka2pi_true_5;
    TH1F         *fh_rpid_ka2pi_true_4;
    TH1F         *fh_rpid_ka2pi_true_3;
    TH1F         *fh_rpid_ka2pi_true_2;
    TH1F         *fh_rpid_ka2pi_true_475;
    TH1F         *fh_rpid_ka2pi_true_45;
    TH1F         *fh_rpid_ka2pi_true_35;
    TH1F         *fh_rpid_ka2pi_true_34;
    TH1F         *fh_rpid_ka2pi_true_325;
    TH1F         *fh_rpid_ka2pi_true_22;

    TH1F         *fh_all_p2pi;
    TH1F         *fh_acc_p2pi;
    TH1F         *fh_rpid_p2pi;
    TH1F         *fh_rpid_p2pi_true;

    TH1F         *fh_all_kap2pip;
    TH1F         *fh_acc_kap2pip;
    TH1F         *fh_rpid_kap2pip;
    TH1F         *fh_rpid_kap2pip_true;

    TH1F         *fh_all_kam2pip;
    TH1F         *fh_acc_kam2pip;
    TH1F         *fh_rpid_kam2pip;
    TH1F         *fh_rpid_kam2pip_true;

    TH1F         *fh_all_kap2pim;
    TH1F         *fh_acc_kap2pim;
    TH1F         *fh_rpid_kap2pim;
    TH1F         *fh_rpid_kap2pim_true;

    TH1F         *fh_all_kam2pim;
    TH1F         *fh_acc_kam2pim;
    TH1F         *fh_rpid_kam2pim;
    TH1F         *fh_rpid_kam2pim_true;

    TH1F         *fh_all_ka2p;
    TH1F         *fh_acc_ka2p;
    TH1F         *fh_rpid_ka2p;
    TH1F         *fh_rpid_ka2p_true;


    TString       fPdfFileName;       // Name of the output file of PDF task
    TFile        *fPdfFile;           // Pointer to the PDF file
    TH1F         *fh_m2min_pi;        // Min m2 value for Pion ID
    TH1F         *fh_m2max_pi;        // Max m2 value for Pion ID
    TH1F         *fh_m2min_ka;        // Min m2 value for Kaon ID
    TH1F         *fh_m2max_ka;        // Max m2 value for Kaon ID
    TH1F         *fh_m2min_prot;      // Min m2 value for Proton ID
    TH1F         *fh_m2max_prot;      // Max m2 value for Proton ID
    TH1F         *fh_pur_pi;          // Purity of pion sample
    TH1F         *fh_pur_ka;          // Purity of kaon sample
    TH1F         *fh_pur_prot;        // Purity of proton sample
    TH2F         *fh_pdf_pi;          // PDF for pi
    TH2F         *fh_pdf_ka;          // PDF for ka
    TH2F         *fh_pdf_protaprot;   // PDF for p/ap
    TH2F         *fh_pdf_pip;         // PDF for pi+
    TH2F         *fh_pdf_kap;         // PDF for K+
    TH2F         *fh_pdf_prot;        // PDF for p
    Int_t         fPdfNbinsM2;        // Number of bins in m2
    Double_t      fPdfMinM2;          // Min m2
    Double_t      fPdfMaxM2;          // Max m2
    Double_t      fPdfBinSizeM2;      // Binsize in m2
    Int_t         fPdfNbinsMom;       // Number of bins in momentum
    Double_t      fPdfMinMom;         // Min momentum
    Double_t      fPdfMaxMom;         // Max momentum
    Double_t      fPdfBinSizeMom;     // Binsize in momentum
    Int_t         fPdfNbinsMomCharge; // Number of bins in mom*charge

    void OpenPdfFile();
    void CreateHistogramms();

    CbmEbefluctPYR(const CbmEbefluctPYR&);
    CbmEbefluctPYR operator=(const CbmEbefluctPYR&);

public:
    CbmEbefluctPYR();
    CbmEbefluctPYR(const char *name,
		   Int_t verbose = 1,
		   Int_t nBinsRatio=100);
    virtual ~CbmEbefluctPYR();

    virtual InitStatus Init();// *MENU*
    virtual void Exec(Option_t *option);
    virtual void Finish();// *MENU*

    void WriteHistogramms();

    inline void SetLevel         (Int_t level)                {  fLevel = level;                   }
    inline void SetCoutBunch     (Int_t coutBunch)            {  fCoutBunch = coutBunch;           }
    inline void SetPdfFile       (const TString &pdfFileName) {  fPdfFileName = pdfFileName;       }

    inline const TString& GetPdfFile() const {  return fPdfFileName;    }

    static void ExperimentalResolution(Double_t &s_true, Double_t &error_s_true,
				       Double_t &s_exp,  Double_t &error_s_exp,
				       Double_t &resol,  Double_t &error_resol);
    static void DynamicalFluctuations(TH1F *h_data, TH1F *h_mix,
				      Double_t &s_data, Double_t &error_s_data,
				      Double_t &s_mix,  Double_t &error_s_mix,
				      Double_t &s_dyn,  Double_t &error_s_dyn);


    ClassDef(CbmEbefluctPYR,1);
};


#endif

