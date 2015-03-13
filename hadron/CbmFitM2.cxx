// ------------------------------------------------------------------
// -----                          CbmFitM2                      -----
// -----               Created 24-09-2004 by D.Kresan           -----
// ------------------------------------------------------------------
#include <iostream>
#include <map>
using namespace std;

#include "TF1.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TDirectory.h"

#include "FairRootManager.h"
#include "CbmFitM2.h"

#define OOSTP 0.3989422804014327

#define M2PI 0.019479835
#define M2KA 0.24371698
#define M2PROT 0.88035435


Double_t fun_m2(Double_t *x, Double_t *par);
Double_t fun_m2_all(Double_t *x, Double_t *par);


//___________________________________________________________________
//
// CbmFitM2
//
// Task for fitting the m2 vs momentum histogramm
//


// ------------------------------------------------------------------
CbmFitM2::CbmFitM2()
  : FairTask("FitM2"),
    fh_m2mom(NULL),
    fh_sm2(NULL),
    fh_pdf_pi(NULL),
    fh_pdf_ka(NULL),
    fh_pdf_protaprot(NULL),
    fh_pdf_pip(NULL),
    fh_pdf_kap(NULL),
    fh_pdf_prot(NULL),
    fh_pdf_pim(NULL),
    fh_pdf_kam(NULL),
    fh_pdf_aprot(NULL),
    fh_eff_ka(NULL),
    fh_m2min_pi(NULL),
    fh_m2max_pi(NULL),
    fh_m2min_ka(NULL),
    fh_m2max_ka(NULL),
    fh_m2min_prot(NULL),
    fh_m2max_prot(NULL),
    fh_pur_pi(NULL),
    fh_pur_ka(NULL),
    fh_pur_prot(NULL),
    fPurity(0.99),
    fLevel(1),
    fNbinsM2(900),
    fMinM2(-1.5),
    fMaxM2(3.0),
    fBinSizeM2( (fMaxM2 - fMinM2) / (Double_t)fNbinsM2 ),
    fNbinsMom(100),
    fMinMom(0.),
    fMaxMom(10.),
    fNbinsMomCharge( TMath::Nint( 2*(fNbinsMom+fMinMom/fBinSizeMom)) ),
    fBinSizeMom((fMaxMom - fMinMom) / (Double_t)fNbinsMom )
{
    CreateHistogramms();
    cout << "Nbins momentum*charge  :  " << fNbinsMomCharge << endl;
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
CbmFitM2::CbmFitM2(const char *name, Int_t verbose)
  : FairTask(name, verbose),
    fh_m2mom(NULL),
    fh_sm2(NULL),
    fh_pdf_pi(NULL),
    fh_pdf_ka(NULL),
    fh_pdf_protaprot(NULL),
    fh_pdf_pip(NULL),
    fh_pdf_kap(NULL),
    fh_pdf_prot(NULL),
    fh_pdf_pim(NULL),
    fh_pdf_kam(NULL),
    fh_pdf_aprot(NULL),
    fh_eff_ka(NULL),
    fh_m2min_pi(NULL),
    fh_m2max_pi(NULL),
    fh_m2min_ka(NULL),
    fh_m2max_ka(NULL),
    fh_m2min_prot(NULL),
    fh_m2max_prot(NULL),
    fh_pur_pi(NULL),
    fh_pur_ka(NULL),
    fh_pur_prot(NULL),
    fPurity(0.99),
    fLevel(1),
    fNbinsM2(900),
    fMinM2(-1.5),
    fMaxM2(3.0),
    fBinSizeM2( (fMaxM2 - fMinM2) / (Double_t)fNbinsM2 ),
    fNbinsMom(100),
    fMinMom(0.),
    fMaxMom(10.),
    fNbinsMomCharge( TMath::Nint( 2*(fNbinsMom+fMinMom/fBinSizeMom)) ),
    fBinSizeMom((fMaxMom - fMinMom) / (Double_t)fNbinsMom )
{
    CreateHistogramms();
    cout << "Nbins momentum*charge  :  " << fNbinsMomCharge << endl;
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
CbmFitM2::~CbmFitM2()
{
    // Destructor
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
InitStatus CbmFitM2::Init()
{
    // Task initialisation

    // Get pointers to root manager
    FairRootManager* rootMgr = FairRootManager::Instance();
    if(NULL == rootMgr) {
	cout << "-E- CbmFitM2::Init : "
	    << "root manager is not instantiated." << endl;
        return kERROR;
    }

    fh_m2mom = (TH2F*) rootMgr->GetInFile()->Get("h_m2mom_hadron");
    if(NULL == fh_m2mom) {
	cout << "-E- CbmFitM2::Init : "
	    << "No m2 vs. mom. histogram!" << endl;
        return kERROR;
    }

    return kSUCCESS;
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
void CbmFitM2::Exec(Option_t *option)
{
    // Task execution
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
void CbmFitM2::Finish()
{
    // Finish the task execution
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
void CbmFitM2::CreatePDF()
{
    // Fit the m2 vs. mom spectrum and create PDF

    const Double_t cM2Mom = 1./fBinSizeM2 * 1./fBinSizeMom;

    // Fitting functions
    TF1 *f_m2_all = new TF1("f_m2_all", fun_m2_all, fMinM2, fMaxM2, 7);
    f_m2_all->SetParameters(0.01, M2PI, M2KA, M2PROT, 1., 1., 1.);
    f_m2_all->FixParameter(1, M2PI);
    f_m2_all->FixParameter(2, M2KA);
    f_m2_all->FixParameter(3, M2PROT);

    // Extracted single particle distributions
    TF1 *f_m2_pi = new TF1("f_m2_pi", fun_m2, fMinM2, fMaxM2, 3);
    TF1 *f_m2_ka = new TF1("f_m2_ka", fun_m2, fMinM2, fMaxM2, 3);
    TF1 *f_m2_prot = new TF1("f_m2_prot", fun_m2, fMinM2, fMaxM2, 3);

    TCanvas *c1 = new TCanvas("c1", "", 100, 100, 800, 600);
    c1->SetFillColor(10);
    c1->SetLogy();


    // Loop over momentum_charge bins ----------------------------------
    for (Int_t j = 0; j < fNbinsMomCharge; j++) {
	// Momentum value
	Double_t momentum_charge = (j+0.5)*fBinSizeMom - fMaxMom;
	Double_t momentum = TMath::Abs(momentum_charge);
	Int_t charge = TMath::Nint(momentum_charge / momentum);
	if(momentum < fMinMom) continue;
        if(momentum > 5) continue;

	// Make projection
	TH1D *h_m2 = fh_m2mom->ProjectionY("h_m2", j+1, j+1);

	// Fit total distribution
	f_m2_all->SetParameters(0.01, M2PI, M2KA, M2PROT, 1., 1., 1.);
	h_m2->Fit(f_m2_all, "QN");

	// Initialise single particle distributions
	f_m2_pi->SetParameter(0, f_m2_all->GetParameter(0));
	f_m2_pi->SetParameter(1, f_m2_all->GetParameter(1));
	f_m2_pi->SetParameter(2, f_m2_all->GetParameter(4));
	f_m2_ka->SetParameter(0, f_m2_all->GetParameter(0));
	f_m2_ka->SetParameter(1, f_m2_all->GetParameter(2));
	f_m2_ka->SetParameter(2, f_m2_all->GetParameter(5));
	f_m2_prot->SetParameter(0, f_m2_all->GetParameter(0));
	f_m2_prot->SetParameter(1, f_m2_all->GetParameter(3));
	f_m2_prot->SetParameter(2, f_m2_all->GetParameter(6));
	Double_t sm2 = f_m2_all->GetParameter(0);

	Int_t i = TMath::Nint(momentum/fBinSizeMom);
        fh_sm2->SetBinContent(i+1, sm2);

	// Calculate the efficiency -----------------------
        Double_t a_pi = M2PI - 3*sm2;
        Double_t b_pi = M2PI + 3*sm2;
        Double_t a_prot = M2PROT - 3*sm2;
        Double_t b_prot = M2PROT + 3*sm2;
	Double_t a0 = TMath::Max(M2PI + sm2, M2KA - 2*sm2);
	Double_t b0 = TMath::Min(M2PROT - sm2, M2KA + 2*sm2);
	Double_t a = a0;
	Double_t b = b0;
	Double_t step = (fMaxM2-fMinM2)/(Double_t)fNbinsM2;
	Double_t n0_ka = f_m2_ka->Integral(a, b);
	Double_t eff_ka = 0;
	Double_t pur_pi = 0;
	Double_t pur_ka = 0;
	Double_t pur_prot = 0;
	Bool_t exit = kFALSE;
	while(b > a) {
	    Double_t f_all_b = f_m2_pi->Eval(b)+f_m2_ka->Eval(b)+f_m2_prot->Eval(b);
	    if(f_all_b < 1e-10) {
		b -= step;
		continue;
	    }
	    Double_t f_b = f_m2_ka->Eval(b) / f_all_b;
	    while(a < b) {
		Double_t f_all_a = f_m2_pi->Eval(a)+f_m2_ka->Eval(a)+f_m2_prot->Eval(a);
		if(f_all_a < 1e-10) {
		    a += step;
		    continue;
		}
		Double_t f_a = f_m2_ka->Eval(a) / f_all_a;
		if( (f_a-f_b) > (1e-4*f_b) ) {
		    a += step;
		    continue;
		}
		Double_t n_pi = f_m2_pi->Integral(a, b);
		Double_t n_ka = f_m2_ka->Integral(a, b);
		Double_t n_prot = f_m2_prot->Integral(a, b);
		if( (n_ka/(n_pi+n_ka+n_prot)) >= fPurity ) {
		    eff_ka = 100 * n_ka / n0_ka;
		    pur_ka = n_ka / (n_pi+n_ka+n_prot);
		    exit = kTRUE;
		    break;
		}
		a += step;
	    }
	    if(exit) {break;}
	    else {a = a0;}
	    b -= step;
	}
	fh_eff_ka->SetBinContent(j+1, eff_ka);
	fh_m2min_pi->SetBinContent(j+1, a_pi);
	fh_m2max_pi->SetBinContent(j+1, b_pi);
	fh_m2min_ka->SetBinContent(j+1, a);
	fh_m2max_ka->SetBinContent(j+1, b);
	fh_m2min_prot->SetBinContent(j+1, a_prot);
	fh_m2max_prot->SetBinContent(j+1, b_prot);
	fh_pur_ka->SetBinContent(j+1, pur_ka);

        Double_t n_pi = f_m2_pi->Integral(a_pi, b_pi);
        Double_t n_ka = f_m2_ka->Integral(a_pi, b_pi);
        Double_t n_prot = f_m2_prot->Integral(a_pi, b_pi);
	fh_pur_pi->SetBinContent(j+1, (n_pi/(n_pi+n_ka+n_prot)));

	n_pi = f_m2_pi->Integral(a_prot, b_prot);
        n_ka = f_m2_ka->Integral(a_prot, b_prot);
        n_prot = f_m2_prot->Integral(a_prot, b_prot);
	fh_pur_prot->SetBinContent(j+1, (n_prot/(n_pi+n_ka+n_prot)));
	// ------------------------------------------------


	h_m2->Draw();
	f_m2_pi->Draw("same");
	f_m2_ka->Draw("same");
	f_m2_prot->Draw("same");
	c1->Modified();
	c1->Update();
	cout << ((j+0.5)*fBinSizeMom-10) << endl;
        Int_t t = system("sleep 3");


	// Fill the look-up tables
	for (Int_t k = 0; k < fNbinsM2; k++) {
	    Double_t pdf_pi = f_m2_pi->Eval((k+0.5)*fBinSizeM2 + fMinM2);
	    Double_t pdf_ka = f_m2_ka->Eval((k+0.5)*fBinSizeM2 + fMinM2);
	    Double_t pdf_prot = f_m2_prot->Eval((k+0.5)*fBinSizeM2 + fMinM2);

	    Double_t pdf_hadron = pdf_pi + pdf_ka + pdf_prot;

	    fh_pdf_pi->SetBinContent(j+1, k+1, pdf_pi);
	    fh_pdf_ka->SetBinContent(j+1, k+1, pdf_ka);
	    fh_pdf_protaprot->SetBinContent(j+1, k+1, pdf_prot);
	    if(1 == charge) {
		fh_pdf_pip->SetBinContent(j+1, k+1, pdf_pi);
		fh_pdf_kap->SetBinContent(j+1, k+1, pdf_ka);
		fh_pdf_prot->SetBinContent(j+1, k+1, pdf_prot);
	    } else {
		fh_pdf_pim->SetBinContent(j+1, k+1, pdf_pi);
		fh_pdf_kam->SetBinContent(j+1, k+1, pdf_ka);
		fh_pdf_aprot->SetBinContent(j+1, k+1, pdf_prot);
	    }
	}

	cout << "momentum * charge  =  " << momentum_charge << endl;
    } // loop over momentum bins ---------------------------------


    // Normalize PDF to 1
    fh_pdf_pi->Scale( 1./fh_pdf_pi->Integral() );
    fh_pdf_ka->Scale( 1./fh_pdf_ka->Integral() );
    fh_pdf_protaprot->Scale( 1./fh_pdf_protaprot->Integral() );
    fh_pdf_pip->Scale( 1./fh_pdf_pip->Integral() );
    fh_pdf_kap->Scale( 1./fh_pdf_kap->Integral() );
    fh_pdf_prot->Scale( 1./fh_pdf_prot->Integral() );
    fh_pdf_pim->Scale( 1./fh_pdf_pim->Integral() );
    fh_pdf_kam->Scale( 1./fh_pdf_kam->Integral() );
    fh_pdf_aprot->Scale( 1./fh_pdf_aprot->Integral() );
    fh_pdf_pi->Scale(cM2Mom);
    fh_pdf_ka->Scale(cM2Mom);
    fh_pdf_protaprot->Scale(cM2Mom);
    fh_pdf_pip->Scale(cM2Mom);
    fh_pdf_kap->Scale(cM2Mom);
    fh_pdf_prot->Scale(cM2Mom);
    fh_pdf_pim->Scale(cM2Mom);
    fh_pdf_kam->Scale(cM2Mom);
    fh_pdf_aprot->Scale(cM2Mom);
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
void CbmFitM2::CreateHistogramms()
{
    // Create histogramms

    fh_sm2 = new TH1F("h_sm2", "Squared mass resolution",
		      fNbinsMom, fMinMom, fMaxMom);


    // PDF = PDF(m2, p)
    fh_pdf_pi = new TH2F("h_pdf_pi", "",
			 fNbinsMomCharge, -fMaxMom, fMaxMom,
			 fNbinsM2, fMinM2, fMaxM2);
    fh_pdf_ka = new TH2F("h_pdf_ka", "",
			 fNbinsMomCharge, -fMaxMom, fMaxMom,
			 fNbinsM2, fMinM2, fMaxM2);
    fh_pdf_protaprot = new TH2F("h_pdf_protaprot", "",
				fNbinsMomCharge, -fMaxMom, fMaxMom,
				fNbinsM2, fMinM2, fMaxM2);

    fh_pdf_pip = new TH2F("h_pdf_pip", "",
			  fNbinsMomCharge, -fMaxMom, fMaxMom,
			  fNbinsM2, fMinM2, fMaxM2);
    fh_pdf_kap = new TH2F("h_pdf_kap", "",
			  fNbinsMomCharge, -fMaxMom, fMaxMom,
			  fNbinsM2, fMinM2, fMaxM2);
    fh_pdf_prot = new TH2F("h_pdf_prot", "",
			   fNbinsMomCharge, -fMaxMom, fMaxMom,
			   fNbinsM2, fMinM2, fMaxM2);
    fh_pdf_pim = new TH2F("h_pdf_pim", "",
			  fNbinsMomCharge, -fMaxMom, fMaxMom,
			  fNbinsM2, fMinM2, fMaxM2);
    fh_pdf_kam = new TH2F("h_pdf_kam", "",
			  fNbinsMomCharge, -fMaxMom, fMaxMom,
			  fNbinsM2, fMinM2, fMaxM2);
    fh_pdf_aprot = new TH2F("h_pdf_aprot", "",
			    fNbinsMomCharge, -fMaxMom, fMaxMom,
			    fNbinsM2, fMinM2, fMaxM2);

    fh_eff_ka = new TH1F("h_eff_ka", "",
			 fNbinsMomCharge, -fMaxMom, fMaxMom);

    fh_m2min_pi = new TH1F("h_m2min_pi", "",
			   fNbinsMomCharge, -fMaxMom, fMaxMom);
    fh_m2max_pi = new TH1F("h_m2max_pi", "",
			   fNbinsMomCharge, -fMaxMom, fMaxMom);
    fh_m2min_ka = new TH1F("h_m2min_ka", "",
			   fNbinsMomCharge, -fMaxMom, fMaxMom);
    fh_m2max_ka = new TH1F("h_m2max_ka", "",
			   fNbinsMomCharge, -fMaxMom, fMaxMom);
    fh_m2min_prot = new TH1F("h_m2min_prot", "",
			     fNbinsMomCharge, -fMaxMom, fMaxMom);
    fh_m2max_prot = new TH1F("h_m2max_prot", "",
			     fNbinsMomCharge, -fMaxMom, fMaxMom);

    fh_pur_pi = new TH1F("h_pur_pi", "",
			 fNbinsMomCharge, -fMaxMom, fMaxMom);
    fh_pur_ka = new TH1F("h_pur_ka", "",
			 fNbinsMomCharge, -fMaxMom, fMaxMom);
    fh_pur_prot = new TH1F("h_pur_prot", "",
			   fNbinsMomCharge, -fMaxMom, fMaxMom);
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
void CbmFitM2::WriteHistogramms()
{
    // Write histogramms
    fh_m2mom->Write();

    fh_sm2->Write();


    fh_pdf_pi->Write();
    fh_pdf_ka->Write();
    fh_pdf_protaprot->Write();

    fh_pdf_pip->Write();
    fh_pdf_kap->Write();
    fh_pdf_prot->Write();
    fh_pdf_pim->Write();
    fh_pdf_kam->Write();
    fh_pdf_aprot->Write();


    fh_eff_ka->Write();
    fh_m2min_pi->Write();
    fh_m2max_pi->Write();
    fh_m2min_ka->Write();
    fh_m2max_ka->Write();
    fh_m2min_prot->Write();
    fh_m2max_prot->Write();

    fh_pur_pi->Write();
    fh_pur_ka->Write();
    fh_pur_prot->Write();
}
// ------------------------------------------------------------------



ClassImp(CbmFitM2);




Double_t fun_m2(Double_t *x, Double_t *par)
{
    return (par[2]*OOSTP/par[0]*exp(-0.5*pow((x[0]-par[1])/par[0],2)));
}


Double_t fun_m2_all(Double_t *x, Double_t *par)
{
    return (par[4]*OOSTP/par[0]*exp(-0.5*pow((x[0]-par[1])/par[0],2)) +
	    par[5]*OOSTP/par[0]*exp(-0.5*pow((x[0]-par[2])/par[0],2)) +
	    par[6]*OOSTP/par[0]*exp(-0.5*pow((x[0]-par[3])/par[0],2)));
}



