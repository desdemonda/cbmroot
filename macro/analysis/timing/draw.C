void draw(){
  TString histoFileName = "data0/histo.root";
  TFile* histoFile = TFile::Open(histoFileName);
  TH3D* hDetEventTime = (TH3D*) histoFile->Get("hDetEventTime");
  printf("Nentries:%i\n",hDetEventTime->GetEntries());

  
  for (Int_t det=1;det<=6;det++){
    TCanvas* c1 = new TCanvas(Form("c1_det%i",det),Form("c1_det%i",det),800,500);
    gPad->SetRightMargin(0.02);
    gPad->SetLeftMargin(0.12);
    gPad->SetLogy();
    TH1D* hTime = hDetEventTime->ProjectionZ(Form("StsEvent%i",det),det,det,1,1000);
    hTime->SetStats(0);
    hTime->SetLineColor(kBlue);
    hTime->SetLineWidth(2);
    if (det==1) hTime->SetTitle("STS @  30 cm; time, ns;Entries");
    if (det==2) hTime->SetTitle("STS @ 100 cm; time, ns;Entries");
    if (det==3) hTime->SetTitle("MuCh @ 130 cm; time, ns;Entries");
    if (det==4) hTime->SetTitle("MuCh @ 315 cm; time, ns;Entries");
    if (det==5) hTime->SetTitle("TOF @ 1000 cm; time, ns;Entries");
    if (det==6) hTime->SetTitle("TRD @ 948 cm; time, ns;Entries");
    hTime->Draw();
    c1->Print(".png");
  }
  
  
  
  TCanvas* c2 = new TCanvas("c2","Primary tracks distribution",800,500);
  gPad->SetRightMargin(0.04);
//  c2->Divide(2,1);
//  TH1D* hNPrimaryTracks = (TH1D*) histoFile->Get("hNPrimaryTracks");
  TH1D* hNPrimaryTracksInAcceptance = (TH1D*) histoFile->Get("hNPrimaryTracksInAcceptance");
  hNPrimaryTracksInAcceptance->SetLineColor(kBlue);
  //  c2->cd(1);
//  hNPrimaryTracks->Draw();
//  c2->cd(2);
  hNPrimaryTracksInAcceptance->SetTitle(";Number of primary tracks in CBM acceptance;Entries");
  hNPrimaryTracksInAcceptance->Draw();
//  gPad->Print(".png");
  
  TCanvas* c3 = new TCanvas("c3","tau",800,500);
  gPad->SetRightMargin(0.04);
  gPad->SetRightMargin(0.12);
  TF1* f1 = new TF1("time","(1-exp(-x/[0]))",0,600);
  f1->SetParameter(0,100);
  f1->SetLineColor(kBlue);
  f1->Draw();
  f1->SetTitle(";time, ns;Probability");
  
  TH1D* hStsPointsZ = (TH1D*) histoFile->Get("hStsPointsZ");
  TH1D* hTrdPointsZ = (TH1D*) histoFile->Get("hTrdPointsZ");
  TH1D* hTofPointsZ = (TH1D*) histoFile->Get("hTofPointsZ");
  TH1D* hMuchPointsZ = (TH1D*) histoFile->Get("hMuchPointsZ");
  TCanvas* c4 = new TCanvas("c4","z position",1000,800);
  c4->Divide(2,2);
  c4->cd(1);
  hStsPointsZ->Draw();
  c4->cd(2);
  hTrdPointsZ->Draw();
  c4->cd(3);
  hTofPointsZ->Draw();
  c4->cd(4);
  hMuchPointsZ->Draw();
}
