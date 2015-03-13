void drawM(){
  gStyle->SetOptStat(0);
  gStyle->SetOptLogy(1);
  gStyle->SetPadRightMargin(0.03);
  gStyle->SetPadTopMargin(0.03);
  gStyle->SetPadLeftMargin(0.12);
  gStyle->SetPadBottomMargin(0.13);
  gStyle->SetOptFit(0);

  TFile* f1 = new TFile("la.histo.root","read");
  hSigM          ->SetLineColor(kBlue);
  hSigB1B2M      ->SetLineColor(kBlue);
  hSigChiDcaM    ->SetLineColor(kBlue);
  hSigPcaBmoM    ->SetLineColor(kBlue);
  hSigChi1Chi2M  ->SetLineColor(kBlue);

  hAllM          ->SetMinimum(0.5);
  hAllB1B2M      ->SetMinimum(0.5);
  hAllChiDcaM    ->SetMinimum(0.5);
  hAllPcaBmoM    ->SetMinimum(0.5);
  hSigChi1Chi2M  ->SetMinimum(0.5);

  SetStyle(hAllM);
  SetStyle(hAllChiDcaM);
  SetStyle(hAllPcaBmoM);

  SetStyle(hSigM);
  SetStyle(hSigChiDcaM);
  SetStyle(hSigPcaBmoM);

  gSystem->mkdir("eps");
  gSystem->mkdir("png");

  gStyle->SetPaperSize(20,20);
  TCanvas* c1 = new TCanvas("c1","",400,400);
  c1->cd(1);
  gPad->SetLogy(0);
  Fit(hAllPcaBmoM,hSigPcaBmoM);
  gPad->SetLeftMargin(0.16);
  hAllPcaBmoM->GetYaxis()->SetTitleOffset(1.7);
  hAllPcaBmoM->Draw("e");
  hSigPcaBmoM->SetLineWidth(2);
  hSigPcaBmoM->Draw("same");
  gPad->Print("png/fig-lambda-minv.png");
  gPad->Print("eps/fig-lambda-minv.eps");
  printf("%6.2f\n",hSigPcaBmoM->Integral());


  gStyle->SetPaperSize(20*2,20*2);
  TCanvas* c1 = new TCanvas("c1","",800,800);
  c1->Divide(2,2);
  c1->cd(1);
  hAllM->Draw();
  hSigM->SetLineWidth(2);
  hSigM->Draw("same");
  gPad->Print("png/fig-xi-minv.png");
  gPad->Print("eps/fig-xi-minv.eps");
  c1->cd(2);
//  hAllBnBpM->Draw();
//  hSigBnBpM->SetLineWidth(2);
//  hSigBnBpM->Draw("same");
//  gPad->Print("png/fig-xi-minv-bdabla.png");
//  gPad->Print("eps/fig-xi-minv-bdabla.eps");
  c1->cd(3);
  //hAllChiDcaM->SetMaximum(10000);
  hAllChiDcaM->Draw();
  hSigChiDcaM->SetLineWidth(2);
  hSigChiDcaM->Draw("same");
  gPad->Print("png/fig-xi-minv-chidca.png");
  gPad->Print("eps/fig-xi-minv-chidca.eps");
  c1->cd(4);
  gPad->SetLogy(0);
  hAllPcaBmoM->Draw();
  //hSigPcaBmoM->Draw("same");
  gPad->Print("png/fig-xi-minv-pcabmo.png");
  gPad->Print("eps/fig-xi-minv-pcabmo.eps");
  c1->cd();

}

void Fit(TH1D* hAll,TH1D* hSignal){
  Double_t par[3];
  hSignal->Fit("gaus","Q","");
  hSignal->GetFunction("gaus")->GetParameters(par);

  TF1* fitSB = new TF1("fitSB","gaus(0)+pol2(3)",1.10,1.13);
  fitSB->FixParameter(0,par[0]);
  fitSB->FixParameter(1,par[1]);
  fitSB->FixParameter(2,par[2]);
  hAll->Fit("fitSB","","",1.10,1.13);

  hSignal->GetFunction("gaus") ->SetLineWidth(2);
  hAll   ->GetFunction("fitSB")->SetLineWidth(2);
  hSignal->GetFunction("gaus") ->SetLineColor(kBlue);
  hAll   ->GetFunction("fitSB")->SetLineColor(kBlue);

}
void SetStyle(TH1* h1){
  h1->GetXaxis()->CenterTitle();
  h1->GetYaxis()->CenterTitle();
  h1->SetTitleSize(0.045,"X");
  h1->SetTitleSize(0.045,"Y");
  h1->SetLabelSize(0.04,"X");
  h1->SetLabelSize(0.04,"Y");
  h1->GetXaxis()->SetTitleFont(42);
  h1->GetYaxis()->SetTitleFont(42);
  h1->GetXaxis()->SetTitleOffset(1.3);
  h1->GetYaxis()->SetTitleOffset(1.2);
  h1->SetLineWidth(1);
  h1->SetMinimum(0.5);
}
