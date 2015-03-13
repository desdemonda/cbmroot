void ana_histo_om(){
  gROOT->LoadMacro("$VMCWORKDIR/analysis/hyperon/hyperon_style.C");
  hyperon_style();
  TString resultFileName = "om.histo.res.root";
  TString title;

  Double_t mMin = 1.65;
  Double_t mMax = 1.69;
  Double_t b1Max   = 0.5;
  Double_t b2Max   = 2.5;
  Double_t chi1Max = 20.0;
  Double_t chi2Max = 20.0;
  Double_t chiMax  = 5.0;
  Double_t dcaMax  = 0.2;
  Double_t pcaMax  = 20.;
  Double_t bmoMax  = 0.20;
  Int_t nBins = 100;
  title = ";b_{#Lambda} [cm];#chi^{2}_{#pi^{ -}}";
  TH2D* hSbB1Chi2    = new TH2D("hSbB1Chi2"   ,title,nBins,0.,b1Max,nBins,0.,chi2Max);
  TH2D* hSnB1Chi2    = new TH2D("hSnB1Chi2"   ,title,nBins,0.,b1Max,nBins,0.,chi2Max);
  TH2D* hEfB1Chi2    = new TH2D("hEfB1Chi2"   ,title,nBins,0.,b1Max,nBins,0.,chi2Max);

  title = ";b_{p} [cm];b_{#pi^{ -}} [cm]";
  TH2D* hSbB1B2    = new TH2D("hSbB1B2"   ,title,nBins,0.,b1Max,nBins,0.,b2Max);
  TH2D* hSnB1B2    = new TH2D("hSnB1B2"   ,title,nBins,0.,b1Max,nBins,0.,b2Max);
  TH2D* hEfB1B2    = new TH2D("hEfB1B2"   ,title,nBins,0.,b1Max,nBins,0.,b2Max);

  title = ";#chi^{2}_{p};#chi^{2}_{#pi^{ -}}";
  TH2D* hSbChi1Chi2    = new TH2D("hSbChi1Chi2"   ,title,nBins,0.,chi1Max,nBins,0.,chi2Max);
  TH2D* hSnChi1Chi2    = new TH2D("hSnChi1Chi2"   ,title,nBins,0.,chi1Max,nBins,0.,chi2Max);
  TH2D* hEfChi1Chi2    = new TH2D("hEfChi1Chi2"   ,title,nBins,0.,chi1Max,nBins,0.,chi2Max);

  title = ";#chi^{2};Distance of closest approach [cm]";
  TH2D*   hSbChiDca  = new TH2D("hSbChiDca" ,title,nBins,0.,chiMax,nBins,0.,dcaMax);
  TH2D*   hSnChiDca  = new TH2D("hSnChiDca" ,title,nBins,0.,chiMax,nBins,0.,dcaMax);
  TH2D*   hEfChiDca  = new TH2D("hEfChiDca" ,title,nBins,0.,chiMax,nBins,0.,dcaMax);

  title = ";z-vertex [cm];b_{#Xi^{ -}} [cm]";
  TH2D*   hSbPcaBmo  = new TH2D("hSbPcaBmo" ,title,nBins,0.,pcaMax,nBins,0.,bmoMax);
  TH2D*   hSnPcaBmo  = new TH2D("hSnPcaBmo" ,title,nBins,0.,pcaMax,nBins,0.,bmoMax);
  TH2D*   hEfPcaBmo  = new TH2D("hEfPcaBmo" ,title,nBins,0.,pcaMax,nBins,0.,bmoMax);

  title = ";M_{inv} [GeV/c^{2}];pairs / 0.6 MeV";
  TH1D*   hAllM         = new TH1D("hAllM"        ,title,50,mMin,mMax);
  TH1D*   hAllB1Chi2M   = new TH1D("hAllB1Chi2M"  ,title,50,mMin,mMax);
  TH1D*   hAllB1B2M     = new TH1D("hAllB1B2M"    ,title,50,mMin,mMax);
  TH1D*   hAllChi1Chi2M = new TH1D("hAllChi1Chi2M",title,50,mMin,mMax);
  TH1D*   hAllChiDcaM   = new TH1D("hAllChiDcaM"  ,title,50,mMin,mMax);
  TH1D*   hAllPcaBmoM   = new TH1D("hAllPcaBmoM"  ,title,50,mMin,mMax);

  TFile* f = TFile::Open("om.histo.root");
  hBgdB1Chi2    = (TH2D*) f->Get("hBgdB1Chi2");
  hSigB1Chi2    = (TH2D*) f->Get("hSigB1Chi2");
  hBgdB1B2      = (TH2D*) f->Get("hBgdB1B2");
  hSigB1B2      = (TH2D*) f->Get("hSigB1B2");
  hBgdChi1Chi2  = (TH2D*) f->Get("hBgdChi1Chi2");
  hSigChi1Chi2  = (TH2D*) f->Get("hSigChi1Chi2");
  hBgdChiDca    = (TH2D*) f->Get("hBgdChiDca");
  hSigChiDca    = (TH2D*) f->Get("hSigChiDca");
  hBgdPcaBmo    = (TH2D*) f->Get("hBgdPcaBmo");
  hSigPcaBmo    = (TH2D*) f->Get("hSigPcaBmo");
  hBgdM         = (TH1D*) f->Get("hBgdM");
  hSigM         = (TH1D*) f->Get("hSigM");
  hBgdB1Chi2M   = (TH1D*) f->Get("hBgdB1Chi2M");
  hSigB1Chi2M   = (TH1D*) f->Get("hSigB1Chi2M");
  hBgdChi1Chi2M = (TH1D*) f->Get("hBgdChi1Chi2M");
  hSigChi1Chi2M = (TH1D*) f->Get("hSigChi1Chi2M");
  hBgdB1B2M     = (TH1D*) f->Get("hBgdB1B2M");
  hSigB1B2M     = (TH1D*) f->Get("hSigB1B2M");
  hBgdChiDcaM   = (TH1D*) f->Get("hBgdChiDcaM");
  hSigChiDcaM   = (TH1D*) f->Get("hSigChiDcaM");
  hBgdPcaBmoM   = (TH1D*) f->Get("hBgdPcaBmoM");
  hSigPcaBmoM   = (TH1D*) f->Get("hSigPcaBmoM");

  hAllM->Add(hBgdM,hSigM);
  hAllChiDcaM->Add(hBgdChiDcaM,hSigChiDcaM);
  hAllB1B2M->Add(hBgdB1B2M,hSigB1B2M);
  hAllPcaBmoM->Add(hBgdPcaBmoM,hSigPcaBmoM);
  hAllChi1Chi2M->Add(hBgdChi1Chi2M,hSigChi1Chi2M);
  hAllB1Chi2M->Add(hBgdB1Chi2M,hSigB1Chi2M);

  for (Int_t i=1;i<=nBins;i++){
    for (Int_t j=1;j<=nBins;j++){
      Double_t b = hBgdB1Chi2->Integral(i,nBins+1,j,nBins+1);
      Double_t s = hSigB1Chi2->Integral(i,nBins+1,j,nBins+1);
      Double_t ef = s/hSigB1Chi2->Integral(0,nBins+1,0,nBins+1);
      Double_t sb = (b) ? s/b : 0;
      Double_t sn = (s+b) ? s/sqrt(s+b): 0;
      hSbB1Chi2->SetBinContent(i,j,sb);
      hSnB1Chi2->SetBinContent(i,j,sn);
      hEfB1Chi2->SetBinContent(i,j,ef);
    }
  }
/*
  for (Int_t i=1;i<=nBins;i++){
    for (Int_t j=1;j<=nBins;j++){
      Double_t b = hBgdChi1Chi2->Integral(i,nBins+1,j,nBins+1);
      Double_t s = hSigChi1Chi2->Integral(i,nBins+1,j,nBins+1);
      Double_t ef = s/hSigChi1Chi2->Integral(0,nBins+1,0,nBins+1);
      Double_t sb = (b) ? s/b : 0;
      Double_t sn = (s+b) ? s/sqrt(s+b): 0;
      hSbChi1Chi2->SetBinContent(i,j,sb);
      hSnChi1Chi2->SetBinContent(i,j,sn);
      hEfChi1Chi2->SetBinContent(i,j,ef);
    }
  }

  for (Int_t i=1;i<=nBins;i++){
    for (Int_t j=1;j<=nBins;j++){
      Double_t b = hBgdB1B2->Integral(i,nBins+1,j,nBins+1);
      Double_t s = hSigB1B2->Integral(i,nBins+1,j,nBins+1);
      Double_t ef = s/hSigB1B2->Integral(0,nBins+1,0,nBins+1);
      Double_t sb = (b) ? s/b : 0;
      Double_t sn = (s+b) ? s/sqrt(s+b): 0;
      hSbB1B2->SetBinContent(i,j,sb);
      hSnB1B2->SetBinContent(i,j,sn);
      hEfB1B2->SetBinContent(i,j,ef);
    }
  }
*/
  for (Int_t i=1;i<=nBins;i++){
    for (Int_t j=1;j<=nBins;j++){
      Double_t b = hBgdChiDca->Integral(0,i,0,j);
      Double_t s = hSigChiDca->Integral(0,i,0,j);
      Double_t ef = s/hSigChiDca->Integral(0,nBins+1,0,nBins+1);
      Double_t sb = (b) ? s/b : 0;
      Double_t sn = (s+b) ? s/sqrt(s+b): 0;
      hSbChiDca->SetBinContent(i,j,sb);
      hSnChiDca->SetBinContent(i,j,sn);
      hEfChiDca->SetBinContent(i,j,ef);
    }
  }

  for (Int_t i=1;i<=nBins;i++){
    for (Int_t j=1;j<=nBins;j++){
      Double_t b = hBgdPcaBmo->Integral(i,nBins+1,0,j);
      Double_t s = hSigPcaBmo->Integral(i,nBins+1,0,j);
      Double_t sb = (b) ? s/b : 0;
      Double_t sn = (s+b) ? s/sqrt(s+b) : 0;
      Double_t ef = s/hSigPcaBmo->Integral(0,nBins+1,0,nBins+1);
      hSbPcaBmo->SetBinContent(i,j,sb);
      hSnPcaBmo->SetBinContent(i,j,sn);
      hEfPcaBmo->SetBinContent(i,j,ef);
    }
  }

  Double_t b = hBgdB1Chi2->Integral(0,nBins+1,0,nBins+1);
  Double_t s = hSigB1Chi2->Integral(0,nBins+1,0,nBins+1);
  printf("Statistics before cuts:\n");
  printf("Total signal:%i\n",hSigM->Integral());
  printf("s/b: %8.4f\n",s/b);
  printf("sgn: %8.4f\n",s/sqrt(s+b));
  printf("\n");
  Int_t z;
  Int_t b1_bin,b2_bin;
  Int_t chi1_bin,chi2_bin;
  Int_t chi_bin,dca_bin;
  Int_t pca_bin,bmo_bin;

  hSnB1Chi2->GetMaximumBin(b1_bin,chi2_bin,z);
//  hSnChi1Chi2->GetMaximumBin(chi1_bin,chi2_bin,z);
//  hSnB1B2->GetMaximumBin(b1_bin,b2_bin,z);
  hSnChiDca->GetMaximumBin(chi_bin,dca_bin,z);
  hSnPcaBmo->GetMaximumBin(pca_bin,bmo_bin,z);

  printf("Maximum significance at:\n");
  printf(" b1  =%7.4f"  ,b1_bin*b1Max/nBins);
  printf(" chi2=%7.4f",chi2_bin*chi2Max/nBins);
  printf(" ef=%8.4f",hEfB1Chi2->GetBinContent(b1_bin,chi2_bin));
  printf(" sb=%8.4f",hSbB1Chi2->GetBinContent(b1_bin,chi2_bin));
  printf(" sn=%8.4f",hSnB1Chi2->GetBinContent(b1_bin,chi2_bin));
  printf("\n");
/*
  printf(" chi1=%7.4f",chi1_bin*chi1Max/nBins);
  printf(" chi2=%7.4f",chi2_bin*chi2Max/nBins);
  printf(" ef=%8.4f",hEfChi1Chi2->GetBinContent(chi1_bin,chi2_bin));
  printf(" sb=%8.4f",hSbChi1Chi2->GetBinContent(chi1_bin,chi2_bin));
  printf(" sn=%8.4f",hSnChi1Chi2->GetBinContent(chi1_bin,chi2_bin));
  printf("\n");

  printf(" b1 =%8.4f",b1_bin*b1Max/nBins);
  printf(" b2 =%8.4f",b2_bin*b2Max/nBins);
  printf(" ef=%8.4f",hEfB1B2->GetBinContent(b1_bin,b2_bin));
  printf(" sb=%8.4f",hSbB1B2->GetBinContent(b1_bin,b2_bin));
  printf(" sn=%8.4f",hSnB1B2->GetBinContent(b1_bin,b2_bin));
  printf("\n");
*/
  printf(" chi=%8.4f",chi_bin*chiMax/nBins);
  printf(" dca=%8.4f",dca_bin*dcaMax/nBins);
  printf(" ef=%8.4f",hEfChiDca->GetBinContent(chi_bin,dca_bin));
  printf(" sb=%8.4f",hSbChiDca->GetBinContent(chi_bin,dca_bin));
  printf(" sn=%8.4f",hSnChiDca->GetBinContent(chi_bin,dca_bin));
  printf("\n");

  printf(" pca=%8.4f",pca_bin*pcaMax/nBins);
  printf(" bmo=%8.4f",bmo_bin*bmoMax/nBins);
  printf(" ef=%8.4f",hEfPcaBmo->GetBinContent(pca_bin,bmo_bin));
  printf(" sb=%8.4f",hSbPcaBmo->GetBinContent(pca_bin,bmo_bin));
  printf(" sn=%8.4f",hSnPcaBmo->GetBinContent(pca_bin,bmo_bin));
  printf("\n");


  TFile* fOut = new TFile(resultFileName,"recreate");
  hBgdB1Chi2   ->Write();
  hSigB1Chi2   ->Write();
  hSbB1Chi2    ->Write();
  hSnB1Chi2    ->Write();
  hEfB1Chi2    ->Write();
  hBgdB1B2   ->Write();
  hSigB1B2   ->Write();
  hSbB1B2    ->Write();
  hSnB1B2    ->Write();
  hEfB1B2    ->Write();
  hBgdChi1Chi2   ->Write();
  hSigChi1Chi2   ->Write();
  hSbChi1Chi2    ->Write();
  hSnChi1Chi2    ->Write();
  hEfChi1Chi2    ->Write();
  hBgdChiDca ->Write();
  hSigChiDca ->Write();
  hSbChiDca  ->Write();
  hSnChiDca  ->Write();
  hEfChiDca  ->Write();
  hBgdPcaBmo ->Write();
  hSigPcaBmo ->Write();
  hSbPcaBmo  ->Write();
  hSnPcaBmo  ->Write();
  hEfPcaBmo  ->Write();
  hAllM      ->Write();
  hBgdM      ->Write();
  hSigM      ->Write();
  hAllB1B2M  ->Write();
  hBgdB1B2M  ->Write();
  hSigB1B2M  ->Write();
  hAllChiDcaM->Write();
  hBgdChiDcaM->Write();
  hSigChiDcaM->Write();
  hAllPcaBmoM->Write();
  hBgdPcaBmoM->Write();
  hSigPcaBmoM->Write();
  hBgdChi1Chi2M   ->Write();
  hSigChi1Chi2M   ->Write();
  hBgdB1Chi2M   ->Write();
  hSigB1Chi2M   ->Write();
  fOut->Close();

  TCanvas* c1 = new TCanvas("c1","",600,600);
  c1->Divide(2,2);
  c1->cd(1);
  hAllM->SetMinimum(0.5);
  hAllM->Draw("e");
  hSigM->Draw("same e");
  c1->cd(2);
  hAllB1Chi2M->Draw("e");
  hSigB1Chi2M->Draw("same e");
  c1->cd(3);
  hAllChiDcaM->Draw("e");
  hSigChiDcaM->Draw("same e");
  c1->cd(4);
  hAllPcaBmoM->Draw("e");
  hSigPcaBmoM->Draw("same e");

  TCanvas* cBC = new TCanvas("cBC","",900,600);
  cBC->Divide(3,2);
  cBC->cd(1);
  gPad->SetLogz();
  hBgdB1Chi2->Draw("colz");

  cBC->cd(2);
  gPad->SetLogz();
  hSigB1Chi2->SetMinimum(0.9);
  hSigB1Chi2->Draw("colz");

  cBC->cd(3);
  gPad->SetLogy();
  hAllB1Chi2M->SetMinimum(0.5);
  hSigB1Chi2M->SetLineColor(kBlue);
  hAllB1Chi2M->Draw();
  hSigB1Chi2M->Draw("same");

  cBC->cd(4);
  gPad->SetLogz();
  hEfB1Chi2->Draw("colz");
  cBC->cd(5);
  gPad->SetLogz();
  hSbB1Chi2->Draw("colz");
  cBC->cd(6);
  gPad->SetLogz();
  hSnB1Chi2->Draw("colz");
/*
  TCanvas* c2 = new TCanvas("c2","",900,600);
  c2->Divide(3,2);
  c2->cd(1);
  gPad->SetLogz();
  hBgdB1B2->Draw("colz");

  c2->cd(2);
  gPad->SetLogz();
  hSigB1B2->SetMinimum(0.9);
  hSigB1B2->Draw("colz");

  c2->cd(3);
  gPad->SetLogy();
  hAllB1B2M->SetMinimum(0.5);
  hSigB1B2M->SetLineColor(kBlue);
  hAllB1B2M->Draw();
  hSigB1B2M->Draw("same");

  c2->cd(4);
  gPad->SetLogz();
  hEfB1B2->Draw("colz");
  c2->cd(5);
  gPad->SetLogz();
  hSbB1B2->Draw("colz");
  c2->cd(6);
  gPad->SetLogz();
  hSnB1B2->Draw("colz");

  TCanvas* cC = new TCanvas("cC","",900,600);
  cC->Divide(3,2);
  cC->cd(1);
  gPad->SetLogz();
  hBgdChi1Chi2->Draw("colz");

  cC->cd(2);
  gPad->SetLogz();
  hSigChi1Chi2->SetMinimum(0.9);
  hSigChi1Chi2->Draw("colz");

  cC->cd(3);
  gPad->SetLogy();
  hAllChi1Chi2M->SetMinimum(0.5);
  hSigChi1Chi2M->SetLineColor(kBlue);
  hAllChi1Chi2M->Draw();
  hSigChi1Chi2M->Draw("same");

  cC->cd(4);
  gPad->SetLogz();
  hEfChi1Chi2->Draw("colz");
  cC->cd(5);
  gPad->SetLogz();
  hSbChi1Chi2->Draw("colz");
  cC->cd(6);
  gPad->SetLogz();
  hSnChi1Chi2->Draw("colz");
*/

  TCanvas* c3 = new TCanvas("c3","",900,600);
  c3->Divide(3,2);
  c3->cd(1);
  gPad->SetLogz();
  hBgdChiDca->Draw("colz");

  c3->cd(2);
  gPad->SetLogz();
  hSigChiDca->SetMinimum(0.9);
  hSigChiDca->Draw("colz");

  c3->cd(3);
  gPad->SetLogy();
  hAllChiDcaM->SetMinimum(0.5);
  hSigChiDcaM->SetLineColor(kBlue);
  hAllChiDcaM->Draw();
  hSigChiDcaM->Draw("same");

  c3->cd(4);
  gPad->SetLogz();
  hEfChiDca->Draw("colz");
  c3->cd(5);
  gPad->SetLogz();
  hSbChiDca->Draw("colz");
  c3->cd(6);
  gPad->SetLogz();
  hSnChiDca->Draw("colz");

  TCanvas* c4 = new TCanvas("c4","",900,600);
  c4->Divide(3,2);
  c4->cd(1);
  gPad->SetLogz();
  hBgdPcaBmo->Draw("colz");

  c4->cd(2);
  gPad->SetLogz();
  hSigPcaBmo->SetMinimum(0.9);
  hSigPcaBmo->Draw("colz");

  c4->cd(3);
  gPad->SetLogy();
  hAllPcaBmoM->SetMinimum(0.5);
  hSigPcaBmoM->SetLineColor(kBlue);
  hAllPcaBmoM->Draw();
  hSigPcaBmoM->Draw("same");

  c4->cd(4);
  gPad->SetLogz();
  hEfPcaBmo->Draw("colz");
  c4->cd(5);
  gPad->SetLogz();
  hSbPcaBmo->Draw("colz");
  c4->cd(6);
  gPad->SetLogz();
  hSnPcaBmo->Draw("colz");


}
