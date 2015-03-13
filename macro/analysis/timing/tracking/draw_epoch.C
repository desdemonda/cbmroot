void SetFonts(TH1* h);

void draw_epoch(){
//  gROOT->LoadMacro("$VMCWORKDIR/analysis/hyperon/hyperon_style.C");
//  hyperon_style();

  gStyle->SetPadGridX(0);
  gStyle->SetPadGridY(0);
//  gStyle->SetTextFont(43);
//  gStyle->SetTextSizePixels(13);
  gStyle->SetLabelFont(42,"XY");
  gStyle->SetLabelSize(0.05,"XY");
//  gStyle->SetTitleFont(42,"Y");
//  gStyle->SetTitleSize(20,"Y");
  gStyle->SetOptLogy(1);
  gStyle->SetPadBottomMargin(0.14);
  gStyle->SetPadTopMargin(0.01);
  gStyle->SetPadLeftMargin(0.03);
  gStyle->SetPadRightMargin(0.02);
  

  
  TString histoFileName = "data/histo.digis.root";
  TFile* histoFile = TFile::Open(histoFileName);
  TH1D* fPointsTimeAll     = (TH1D*) histoFile->Get("fPointsTimeAll");
  TH1D* fPointsTimeSector  = (TH1D*) histoFile->Get("fPointsTimeSector");
  TH1D* fDigiTimeAll       = (TH1D*) histoFile->Get("fDigiTimeAll");
  TH1D* fDigiTimeSector    = (TH1D*) histoFile->Get("fDigiTimeSector");
  TString hitFileName = "data/histo.hits.root";
  TFile* hitsFile = TFile::Open(hitFileName);
  hitsFile->ls();
  TH1D* fHitTimeAll = (TH1D*) hitsFile->Get("fHitTimeAll");
  TH1D* fHitTimeCorrected = (TH1D*) hitsFile->Get("fHitTimeCorrected");

  TString tracksFileName = "data/histo.tracking.root";
  TFile* tracksFile = TFile::Open(tracksFileName);
  tracksFile->ls();
  TH1D* fTrackTime        = (TH1D*) tracksFile->Get("fTrackTime");
  TH1D* fTrackTimeFromHit = (TH1D*) tracksFile->Get("fTrackTimeFromHit");
  SetFonts(fPointsTimeAll);
  SetFonts(fPointsTimeSector);
  SetFonts(fDigiTimeAll);
  SetFonts(fDigiTimeSector);
  SetFonts(fHitTimeAll);
  SetFonts(fHitTimeCorrected);
  SetFonts(fTrackTime);
  SetFonts(fTrackTimeFromHit);
  TCanvas* c1 = new TCanvas("c1","Sts time",1400,400);
  fPointsTimeAll->Draw();
  gPad->Print("points_all.png");
  TCanvas* c2 = new TCanvas("c2","Sts time sector",1400,400);
  fPointsTimeSector->Draw();
  gPad->Print("points_sector.png");
  TCanvas* c3 = new TCanvas("c3","Sts digi all",1400,400);
  fDigiTimeAll->Draw();
  gPad->Print("digi_all.png");
  TCanvas* c4 = new TCanvas("c4","Sts digi sector",1400,400);
  fDigiTimeSector->Draw();
  gPad->Print("digi_sector.png");
  TCanvas* c5 = new TCanvas("c5","Hit time all",1400,400);
  fHitTimeAll->Draw();
  gPad->Print("hit_all.png");
  TCanvas* c6 = new TCanvas("c6","Hit time corrected",1400,400);
  fHitTimeCorrected->Draw();
  gPad->Print("hit_corrected.png");
  TCanvas* c7 = new TCanvas("c7","Track time",1400,400);
  fTrackTime->Draw();
  gPad->Print("track_time.png");
  TCanvas* c8 = new TCanvas("c8","Track time from hit",1400,400);
  fTrackTimeFromHit->Draw();
  gPad->Print("track_time_from_hit.png");
}

void SetFonts(TH1* h){
  h->GetXaxis()->SetLabelSize(0.055);
  h->GetYaxis()->SetLabelSize(0.055);
  h->GetXaxis()->SetTitleSize(0.055);
  h->GetXaxis()->SetRangeUser(0,600);
}
