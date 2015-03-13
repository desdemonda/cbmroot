void eff_draw(char* fileName="la.ef.root"){
  gROOT->LoadMacro("$VMCWORKDIR/analysis/hyperon/hyperon_style.C");
  hyperon_style();
  gSystem->mkdir("eps");
  gSystem->mkdir("png");

  TFile* file  = new TFile(fileName,"update");
  printf("MC: %i\n",hMC->GetEntries());
  printf("AC: %i\n",hAC->GetEntries());
  printf("RC: %i\n",hRC->GetEntries());

  TH2D* hAccYPt   = new TH2D("hAccYPt"  ,";Rapidity;p_t [GeV/c]",20,0,4,20,0,2);
  TH2D* hRcEffYPt = new TH2D("hRcEffYPt",";Rapidity;p_t [GeV/c]",20,0,4,20,0,2);
  hAccYPt->Divide(hAC,hMC);
  hRcEffYPt->Divide(hRC,hAC);
  hAccYPt->Write();
  hRcEffYPt->Write();

  gStyle->SetOptStat(0);
  gStyle->SetPaperSize(16,16);

  TH2D* h1 = (TH2D*) hRcEffYPt->Clone();

  h1->SetTitle(";Rapidity;p_{t} [GeV/c]");
  h1->GetXaxis()->CenterTitle();
  h1->GetYaxis()->CenterTitle();
  h1->GetXaxis()->SetTitleOffset(1.4);
  h1->GetYaxis()->SetTitleOffset(1.6);
  h1->GetXaxis()->SetDecimals();
  h1->GetYaxis()->SetDecimals();
  h1->GetZaxis()->SetDecimals();

  c1 = new TCanvas("c1","",500,500);
  gPad->SetLeftMargin(0.13);
  gPad->SetBottomMargin(0.12);
  gPad->SetTopMargin(0.03);
  gPad->SetRightMargin(0.12);
  h1->Draw("colz");
  c1->Print("eps/fig-lambda-acc.eps");
  c1->Print("png/fig-lambda-acc.png");

  c2 = new TCanvas("c2","",500,500);
  gPad->SetLeftMargin(0.13);
  gPad->SetBottomMargin(0.12);
  gPad->SetTopMargin(0.03);
  gPad->SetRightMargin(0.04);
  h1->Draw("box");
  gStyle->SetPaperSize(16,16);
  c2->Print("eps/fig-lambda-acc-bw.eps");
}

