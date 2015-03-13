void draw_efficiency(){
  gROOT->LoadMacro("$VMCWORKDIR/analysis/hyperon/hyperon_style.C");
  hyperon_style();
  gStyle->SetPadGridX(0);
  gStyle->SetPadGridY(0);

  TString histoFileName  = "data6/histo.root";
  TString histoFileName1 = "data5/histo1.root";
  TFile* histoFile  = TFile::Open(histoFileName);
  TH1D* fhStationTotalR[5];
  TH1D* fhStationFoundR[5];
  TH1D* fhStationEffR[5];
  TH1D* fhStationInEffR[5];
  TH1D* fhStationNotFoundR[5];
  
  for (Int_t i=0;i<5;i++){
    fhStationTotalR[i] = (TH1D*) histoFile->Get(Form("hStationTotalR%i",i));
    fhStationFoundR[i] = (TH1D*) histoFile->Get(Form("hStationFoundR%i",i));
    fhStationEffR[i] = (TH1D*) histoFile->Get(Form("hStationEffR%i",i));
    fhStationNotFoundR[i] = (TH1D*) fhStationTotalR[i]->Clone();
    fhStationNotFoundR[i]->Add(fhStationFoundR[i],-1);
//    fhStationNotFoundR[i]->Sumw2();
//    fhStationTotalR[i]->Sumw2();
    for (Int_t bin=1;bin<=fhStationNotFoundR[i]->GetNbinsX();bin++){
      fhStationNotFoundR[i]->SetBinError(bin,sqrt(fhStationNotFoundR[i]->GetBinContent(bin)));
    }
    fhStationInEffR[i] = (TH1D*) fhStationNotFoundR[i]->Clone();
    fhStationInEffR[i]->Divide(fhStationNotFoundR[i],fhStationTotalR[i]);
  }
  
  TFile* histoFile1 = TFile::Open(histoFileName1);


  TH1D* fhStationTotalR1[5];
  TH1D* fhStationFoundR1[5];
  TH1D* fhStationEffR1[5];
  TH1D* fhStationInEffR1[5];
  TH1D* fhStationNotFoundR1[5];
  for (Int_t i=0;i<5;i++){
    fhStationTotalR1[i] = (TH1D*) histoFile1->Get(Form("hStationTotalR%i",i));
    fhStationFoundR1[i] = (TH1D*) histoFile1->Get(Form("hStationFoundR%i",i));
    fhStationEffR1[i] = (TH1D*) histoFile1->Get(Form("hStationEffR%i",i));
    fhStationNotFoundR1[i] = (TH1D*) fhStationTotalR1[i]->Clone();
    fhStationNotFoundR1[i]->Add(fhStationFoundR1[i],-1);
//    fhStationNotFoundR[i]->Sumw2();
//    fhStationTotalR[i]->Sumw2();
    for (Int_t bin=1;bin<=fhStationNotFoundR1[i]->GetNbinsX();bin++){
      fhStationNotFoundR1[i]->SetBinError(bin,sqrt(fhStationNotFoundR1[i]->GetBinContent(bin)));
    }
    fhStationInEffR1[i] = (TH1D*) fhStationNotFoundR1[i]->Clone();
    fhStationInEffR1[i]->Divide(fhStationNotFoundR1[i],fhStationTotalR1[i]);
  }
  
  TH1D* fhChannelT = (TH1D*) histoFile->Get("hChannelT");
  TH1D* fhPointT = (TH1D*) histoFile->Get("hPointT");
  TH1D* fhModuleT = (TH1D*) histoFile->Get("hModuleT");
  
  TCanvas* c1 = new TCanvas("c1","",1600,1000);
  c1->Divide(5,3,0.001,0.001);
  for (Int_t i=0;i<5;i++){
    c1->cd(i+1);
    gPad->SetGridx(0);
    gPad->SetGridy(0);
    fhStationTotalR1[i]->Draw();   
    c1->cd(i+6);
    gPad->SetGridx(0);
    gPad->SetGridy(0);
    fhStationFoundR1[i]->Draw();   
    c1->cd(i+11);
    gPad->SetGridx(0);
    gPad->SetGridy(0);
    fhStationInEffR[i]->SetTitle(Form("Station %i;R [cm];Inefficiency",i+1));
    fhStationInEffR[i]->Draw();
    fhStationInEffR1[i]->SetLineColor(kBlue);
    fhStationInEffR1[i]->Draw("same");
  }
  
  TCanvas* c2 = new TCanvas("c2","",400,400);
  gPad->SetLeftMargin(0.15);
  fhStationInEffR[0]->SetMaximum(0.09);
  fhStationInEffR[0]->SetMinimum(0.);
  fhStationInEffR[0]->GetYaxis()->SetTitleOffset(1.6);
  fhStationInEffR[0]->Draw();
  fhStationInEffR1[0]->Draw("same");
  c2->Print("Station1.png");
  
  TCanvas* c3 = new TCanvas("c3","",400,400);
  gPad->SetLeftMargin(0.15);
  fhStationInEffR[1]->SetMaximum(0.09);
  fhStationInEffR[1]->SetMinimum(0.);
  fhStationInEffR[1]->GetYaxis()->SetTitleOffset(1.6);
  fhStationInEffR[1]->Draw();
  fhStationInEffR1[1]->Draw("same");
  c3->Print("Station2.png");

  TCanvas* c4 = new TCanvas("c4","",400,400);
  gPad->SetLeftMargin(0.15);
  fhStationInEffR[2]->SetMaximum(0.09);
  fhStationInEffR[2]->SetMinimum(0.);
  fhStationInEffR[2]->Draw();
  fhStationInEffR[2]->GetYaxis()->SetTitleOffset(1.6);
  fhStationInEffR1[2]->Draw("same");
  c4->Print("Station3.png");
  
  TCanvas* c5 = new TCanvas("c5","",1600,500);
  gPad->SetLeftMargin(0.03);
  gPad->SetRightMargin(0.03);
  fhModuleT->SetTitle("; time [ns];");
  fhModuleT->SetLineColor(kBlue);
  fhModuleT->Draw();
  c5->Print("Module4digis_no_smearing.png");
  fhModuleT->GetXaxis()->SetRangeUser(8000,9000);
  c5->Print("Module4digis_zoomed_no_smearing.png");
}
