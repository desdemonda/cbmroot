// ------------------------------------------------------------------
// -----                     CbmHadronSpectra                   -----
// -----              Created 06/02/2006 by D.Kresan            -----
// ------------------------------------------------------------------
#ifndef _CBMHADRONSPECTRA_H_
#define _CBMHADRONSPECTRA_H_

#include "FairTask.h"

class TClonesArray;
class TH1F;
class TH2F;
class TString;


class CbmHadronSpectra : public FairTask {

private:
    Int_t         fEvents;            // Number of processed events

    Int_t         fBeamMomentum;      // Beam momentum

    TString       fPdfFileName;       // name of the file name with hadron PDF
    Double_t      fPdfMinMom;         // Min momentum
    Double_t      fPdfMaxMom;         // Max momentum
    Double_t      fPdfMomBinSize;     // Binsize in momentum
    TH1F         *fh_m2min_pi;        // Min m2 for pi id
    TH1F         *fh_m2max_pi;        // Max m2 for pi id
    TH1F         *fh_m2min_ka;        // Min m2 for K id
    TH1F         *fh_m2max_ka;        // Max m2 for K id
    TH1F         *fh_m2min_prot;      // Min m2 for prot id
    TH1F         *fh_m2max_prot;      // Max m2 for prot id
    TH1F         *fh_pur_pi;          // Purity of pi sample
    TH1F         *fh_pur_ka;          // Purity of K sample
    TH1F         *fh_pur_prot;        // Purity of p sample

    TClonesArray *fArrayMCTrack;      // MC tracks
    TClonesArray *fArrayTofPoint;     // TOF points
    TClonesArray *fArrayHadron;       // Hadrons

    TH1F         *fh_mom_all_pip;     // Momentum of all pi+
    TH1F         *fh_mom_all_kap;     // Momentum of all K+
    TH1F         *fh_mom_all_prot;    // Momentum of all p
    TH1F         *fh_mom_all_pim;     // Momentum of all pi-
    TH1F         *fh_mom_all_kam;     // Momentum of all K-
    TH1F         *fh_mom_acc_pip;     // Momentum of accepted pi+
    TH1F         *fh_mom_acc_kap;     // Momentum of accepted K+
    TH1F         *fh_mom_acc_prot;    // Momentum of accepted p
    TH1F         *fh_mom_acc_pim;     // Momentum of accepted pi-
    TH1F         *fh_mom_acc_kam;     // Momentum of accepted K-
    TH1F         *fh_mom_pid_pip;     // Momentum of pid pi+
    TH1F         *fh_mom_pid_kap;     // Momentum of pid K+
    TH1F         *fh_mom_pid_prot;    // Momentum of pid p
    TH1F         *fh_mom_pid_pim;     // Momentum of pid pi-
    TH1F         *fh_mom_pid_kam;     // Momentum of pid K-
    TH1F         *fh_mom_rpid_pip;    // Momentum of rpid pi+
    TH1F         *fh_mom_rpid_kap;    // Momentum of rpid K+
    TH1F         *fh_mom_rpid_prot;   // Momentum of rpid p
    TH1F         *fh_mom_rpid_pim;    // Momentum of rpid pi-
    TH1F         *fh_mom_rpid_kam;    // Momentum of rpid K-

    TH1F         *fh_pt_all_pip;      // pt of all pi+
    TH1F         *fh_pt_all_kap;      // pt of all K+
    TH1F         *fh_pt_all_prot;     // pt of all p
    TH1F         *fh_pt_all_pim;      // pt of all pi-
    TH1F         *fh_pt_all_kam;      // pt of all K-
    TH1F         *fh_pt_acc_pip;      // pt of accepted pi+
    TH1F         *fh_pt_acc_kap;      // pt of accepted K+
    TH1F         *fh_pt_acc_prot;     // pt of accepted p
    TH1F         *fh_pt_acc_pim;      // pt of accepted pi-
    TH1F         *fh_pt_acc_kam;      // pt of accepted K-
    TH1F         *fh_pt_rpid_pip;     // pt of identified pi+
    TH1F         *fh_pt_rpid_kap;     // pt of identified K+
    TH1F         *fh_pt_rpid_prot;    // pt of identified p
    TH1F         *fh_pt_rpid_pim;     // pt of identified pi-
    TH1F         *fh_pt_rpid_kam;     // pt of identified K-

    TH2F         *fh_pty_all_pip;     // pt-y of all pi+
    TH2F         *fh_pty_all_kap;     // pt-y of all K+
    TH2F         *fh_pty_all_prot;    // pt-y of all p
    TH2F         *fh_pty_all_pim;     // pt-y of all pi-
    TH2F         *fh_pty_all_kam;     // pt-y of all K-
    TH2F         *fh_pty_acc_pip;     // pt-y of accepted pi+
    TH2F         *fh_pty_acc_kap;     // pt-y of accepted K+
    TH2F         *fh_pty_acc_prot;    // pt-y of accepted p
    TH2F         *fh_pty_acc_pim;     // pt-y of accepted pi-
    TH2F         *fh_pty_acc_kam;     // pt-y of accepted K-
    TH2F         *fh_pty_pid_pip;     // pt-y of pid pi+
    TH2F         *fh_pty_pid_kap;     // pt-y of pid K+
    TH2F         *fh_pty_pid_prot;    // pt-y of pid p
    TH2F         *fh_pty_pid_pim;     // pt-y of pid pi-
    TH2F         *fh_pty_pid_kam;     // pt-y of pid K-
    TH2F         *fh_pty_rpid_pip;    // pt-y of rpid pi+
    TH2F         *fh_pty_rpid_kap;    // pt-y of rpid K+
    TH2F         *fh_pty_rpid_prot;   // pt-y of rpid p
    TH2F         *fh_pty_rpid_pim;    // pt-y of rpid pi-
    TH2F         *fh_pty_rpid_kam;    // pt-y of rpid K-

    TH2F         *fh_mty_all_pip;     // mt-y of all pi+
    TH2F         *fh_mty_all_kap;     // mt-y of all K+
    TH2F         *fh_mty_all_prot;    // mt-y of all p
    TH2F         *fh_mty_all_pim;     // mt-y of all pi-
    TH2F         *fh_mty_all_kam;     // mt-y of all K-
    TH2F         *fh_mty_acc_pip;     // mt-y of accepted pi+
    TH2F         *fh_mty_acc_kap;     // mt-y of accepted K+
    TH2F         *fh_mty_acc_prot;    // mt-y of accepted p
    TH2F         *fh_mty_acc_pim;     // mt-y of accepted pi-
    TH2F         *fh_mty_acc_kam;     // mt-y of accepted K-
    TH2F         *fh_mty_rpid_pip;    // mt-y of identified pi+
    TH2F         *fh_mty_rpid_kap;    // mt-y of identified K+
    TH2F         *fh_mty_rpid_prot;   // mt-y of identified p
    TH2F         *fh_mty_rpid_pim;    // mt-y of identified pi-
    TH2F         *fh_mty_rpid_kam;    // mt-y of identified K-

    void CreateHistogramms();
    InitStatus ReadPdfFile();

    CbmHadronSpectra(const CbmHadronSpectra&);
    CbmHadronSpectra operator=(const CbmHadronSpectra&);

public:
    CbmHadronSpectra();
    CbmHadronSpectra(const char* name, Int_t verbose = 1);
    virtual ~CbmHadronSpectra();

    virtual InitStatus Init();
    virtual void Exec(Option_t* option);
    virtual void Finish();

    void WriteHistogramms();

    inline void SetBeamMomentum(Int_t bm)    { fBeamMomentum = bm;          }
    void SetPdfFileName(TString pdfFileName) {  fPdfFileName = pdfFileName; }

    inline Int_t GetBeamMomentum() const { return fBeamMomentum; }

    ClassDef(CbmHadronSpectra, 1);
};


#endif

