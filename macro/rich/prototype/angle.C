#include "/misc/kresan/tools/drawStyle.C"



const Double_t n = 1.0004951;//1.0004462;
const Double_t mass_el = 0.000510998902;
const Double_t mass_pi = 0.13957018;



void angle(Double_t p = 1.)
{
    Double_t e_el = TMath::Sqrt(p*p + mass_el*mass_el);
    Double_t e_pi = TMath::Sqrt(p*p + mass_pi*mass_pi);
    Double_t beta_el = p/e_el;
    Double_t beta_pi = p/e_pi;
    Double_t t_el = 1./(beta_el*n);
    Double_t t_pi = 1./(beta_pi*n);
    Double_t a_el = 0., a_pi = 0.;
    if(t_el <= 1.) {
	a_el = TMath::ACos(t_el)*TMath::RadToDeg();
    }
    if(t_pi <= 1.) {
	a_pi = TMath::ACos(t_pi)*TMath::RadToDeg();
    }
    cout << "e  : " << a_el << " deg" << endl;
    cout << "pi : " << a_pi << " deg" << endl;
}



void radius()
{
    TPstyle();

    TF1 *f1_el = new TF1("f1_el", f_radius, 0., 30., 1);
    f1_el->SetParameter(0, mass_el);

    TF1 *f1_pi = new TF1("f1_pi", f_radius, 0., 30., 1);
    f1_pi->SetParameter(0, mass_pi);
    f1_pi->SetLineStyle(2);
    f1_pi->SetLineColor(2);

    gStyle->SetPaperSize(16, 16);
    TCanvas *c1 = new TCanvas("c1", "", 10, 10, 510, 510);
    TPostScript *ps1 = new TPostScript("plots/rich.prot.radius.eps", -113);
    ps1->Range(16, 16);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    f1_el->Draw();
    Style(f1_el->GetHistogram(), "p (GeV/c)", "light cone radius (cm)");
    f1_el->GetHistogram()->GetYaxis()->SetRangeUser(0., 7.);
    TBox *box = new TBox(1., 0., 10., 7.);
    box->SetFillColor(42);
    box->Draw();
    f1_el->Draw("same");
    f1_pi->Draw("same");
    TPaveLabel *l1 = new TPaveLabel(0.42, 0.72, 0.57, 0.87, "e", "NDC");
    Style(l1);
    l1->Draw();
    TPaveLabel *l2 = new TPaveLabel(0.45, 0.53, 0.57, 0.65, "#pi", "NDC");
    Style(l2);
    l2->SetTextColor(2);
    l2->Draw();
    ps1->Close();
}



Double_t f_radius(Double_t *x, Double_t *par)
{
    if(x[0] <= 0.) {
        return 0.;
    }
    Double_t e = TMath::Sqrt(x[0]*x[0]+par[0]*par[0]);
    Double_t beta = x[0]/e;
    Double_t t = 1./(beta*n);
    Double_t a = 0.;
    if(t <= 1.) {
	a = TMath::ACos(t);
    }
    Double_t r = 170.*TMath::Tan(a);
    return r;
}



