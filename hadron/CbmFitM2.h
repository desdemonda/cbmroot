// ------------------------------------------------------------------
// -----                          CbmFitM2                      -----
// -----               Created 24-09-2004 by D.Kresan           -----
// ------------------------------------------------------------------
#ifndef CBM_FIT_M2_H
#define CBM_FIT_M2_H

#include "FairTask.h"

#include "TH1F.h"
#include "TH2F.h"

class TClonesArray;


class CbmFitM2 : public FairTask {

private:
    TH2F         *fh_m2mom;          // m2 vs. mom for hadron
    TH1F         *fh_sm2;            // squared mass resol.
    TH2F         *fh_pdf_pi;         // PDF for pi
    TH2F         *fh_pdf_ka;         // PDF for K
    TH2F         *fh_pdf_protaprot;  // PDF for p/ap
    TH2F         *fh_pdf_pip;        // PDF for pi+
    TH2F         *fh_pdf_kap;        // PDF for K+
    TH2F         *fh_pdf_prot;       // PDF for p
    TH2F         *fh_pdf_pim;        // PDF for pi-
    TH2F         *fh_pdf_kam;        // PDF for K-
    TH2F         *fh_pdf_aprot;      // PDF for ap
    TH1F         *fh_eff_ka;         // Efficiency for K id
    TH1F         *fh_m2min_pi;       // Min m2 for pi id
    TH1F         *fh_m2max_pi;       // Max m2 for pi id
    TH1F         *fh_m2min_ka;       // Min m2 for K id
    TH1F         *fh_m2max_ka;       // Max m2 for K id
    TH1F         *fh_m2min_prot;     // Min m2 for p id
    TH1F         *fh_m2max_prot;     // Max m2 for p id
    TH1F         *fh_pur_pi;         // Purity of pi sample
    TH1F         *fh_pur_ka;         // Purity of K sample
    TH1F         *fh_pur_prot;       // Purity of p sample

    Double_t      fPurity;           // purity of K identification
    Int_t         fLevel;            // level

    Int_t         fNbinsM2;          // Number of bins in m2
    Double_t      fMinM2;            // Min m2
    Double_t      fMaxM2;            // Max m2
    Double_t      fBinSizeM2;        // Binsize in m2
    Int_t         fNbinsMom;         // Number of bins in momentum
    Int_t         fNbinsMomCharge;   // Number of bins in mom*charge
    Double_t      fMinMom;           // Min momentum
    Double_t      fMaxMom;           // Max momentum
    Double_t      fBinSizeMom;       // Binsize in momentum

    void CreateHistogramms();

    CbmFitM2(const CbmFitM2&);
    CbmFitM2 operator=(const CbmFitM2&);

public:
    CbmFitM2();
    CbmFitM2(const char *name, Int_t verbose=1);
    virtual ~CbmFitM2();

    virtual InitStatus Init();
    virtual void Exec(Option_t *option);
    virtual void Finish();

    void CreatePDF();
    void WriteHistogramms();

    inline Double_t GetMinMom()         const {  return fMinMom;         }
    inline Double_t GetMaxMom()         const {  return fMaxMom;         }
    inline Int_t    GetNbinsMom()       const {  return fNbinsMom;       }
    inline Double_t GetMinM2()          const {  return fMinM2;          }
    inline Double_t GetMaxM2()          const {  return fMaxM2;          }
    inline Int_t    GetNbinsM2()        const {  return fNbinsM2;        }

    inline void SetPurity(Double_t purity)    {  fPurity = purity;       }
    inline void SetLevel(Int_t level)         {  fLevel = level;         }

    ClassDef(CbmFitM2,1);
};

#endif

