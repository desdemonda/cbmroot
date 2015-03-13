// --------------------------------------------------------------------------
//
// Macro for check of the symmetries of a magnetic field map
//
// V. Friese   06/03/2006
//
// --------------------------------------------------------------------------

{

  // Choose field map
  //  TString fieldName = "FieldActive";
  TString fieldName = "FieldMuonMagnet";
  TString psFile = "checksym." + fieldName + ".ps";
  Double_t fzref1 =   0.;        // Target plane
  Double_t fzref2 = 170.;        // RICH entrance and PM
  // ---> Regions of interest for the field
  Double_t fxmin =  -200.;    // along x axis
  Double_t fxmax =   200.;
  Double_t fymin =  -200.;    // along y axis
  Double_t fymax =   200.;
  Double_t fzmin =  -300.;    // along z axis
  Double_t fzmax =   300.;
  // ---> Target location
  Double_t targX = 0.;
  Double_t targY = 0.;
  Double_t targZ = 0.;

  
  // -----   Load libraries   ---------------------------------------------
  // Load libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libPostscript.so");
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libField");
  // ----------------------------------------------------------------------

  


  // -------  Get magnetic field  -----------------------------------------
  CbmFieldMap* field = NULL;
  if ( fieldName == "FieldActive" || fieldName == "FieldIron" || fieldName == "FieldMuonMagnet") 
    field = new CbmFieldMapSym3(fieldName.Data());
  else if (fieldName == "FieldSC_16x13" || fieldName == "FieldSCmuon_16x13" || fieldName == "FieldSC" 
	   //	   || fieldName == "FieldAlligator" ||  fieldName == "FieldDipole" || fieldName == "FieldHera" || 
	   //	   fieldName == "FieldHeraP" || fieldName == "FieldHeraS" || fieldName == "FieldHermes" 
	   )
    field = new CbmFieldMapSym2(fieldName.Data());
  else {
    cout << "=====> ERROR: Field map " << fieldName << " unknown!" << endl;
    exit;
  }
  field->Init();
  field->Print();
  Int_t type = field->GetType();
  if ( type >=1 && type <= 3) 
    const char* mapFile = field->GetFileName();
  // ----------------------------------------------------------------------



  // -------  Create graphs and histograms  -------------------------------
  Int_t fnx = TMath::Nint( (fxmax-fxmin) * 3. );
  Int_t fny = TMath::Nint( (fymax-fymin) * 3. );
  Int_t fnz = TMath::Nint( (fzmax-fzmin) * 3. );
  Int_t fmx = TMath::Nint( (fxmax-fxmin) * 3. / 10. );
  Int_t fmy = TMath::Nint( (fymax-fymin) * 3. / 10. );
  TGraph hBxx(fnx+1);
  TGraph hByx(fnx+1);
  TGraph hBzx(fnx+1);
  TGraph hBxy(fny+1);
  TGraph hByy(fny+1);
  TGraph hBzy(fny+1);
  TGraph hBxz(fnz+1);
  TGraph hByz(fnz+1);
  TGraph hBzz(fnz+1);
  TH2F hB1("hB1", "B at RICH entrance", 
	   fmx, fxmin, fxmax, fmy, fymin, fymax);
  TH2F hB2("hB2", "B at RICH PM",       
	   fmx, fxmin, fxmax, fmy, fymin, fymax);
  // ----------------------------------------------------------------------



  // -------  Get field and fill graphs and histograms  -------------------
  cout << endl;
  cout << "=====>  Filling histograms..." << endl;
  Double_t x, y, z;
  Double_t bx, by, bz, b;
  Double_t fdx = (fxmax-fxmin) / Double_t(fnx);
  Double_t fdy = (fymax-fymin) / Double_t(fny);
  Double_t fdz = (fzmax-fzmin) / Double_t(fnz);

  // ---> Loop over x axis
  cout << "                           ... x axis" << endl;
  y = 20.;
  z = 50.;
  for (Int_t ix=0; ix<=fnx; ix++) {
    x = fxmin + Double_t(ix) * fdx;
    bx = field->GetBx(x,y,z) / 10.;
    by = field->GetBy(x,y,z) / 10.;
    bz = field->GetBz(x,y,z) / 10.;
    hBxx.SetPoint(ix, x, bx);
    hByx.SetPoint(ix, x, by);
    hBzx.SetPoint(ix, x, bz);
  }
  
  // ---> Loop over y axis
  cout << "                           ... y axis" << endl;
  x = 20.;
  z = 50.;
  for (Int_t iy=0; iy<=fny; iy++) {
    y = fymin + Double_t(iy) * fdy;
    bx = field->GetBx(x,y,z) / 10.;
    by = field->GetBy(x,y,z) / 10.;
    bz = field->GetBz(x,y,z) / 10.;
    hBxy.SetPoint(iy, y, bx);
    hByy.SetPoint(iy, y, by);
    hBzy.SetPoint(iy, y, bz);
  }

  // ---> Loop over z axis
  cout << "                           ... z axis" << endl;
  x = y = 0.;
  for (Int_t iz=0; iz<=fnz; iz++) {
    z = fzmin + Double_t(iz) * fdz;
    bx = field->GetBx(x,y,z) / 10.;
    by = field->GetBy(x,y,z) / 10.;
    bz = field->GetBz(x,y,z) / 10.;
    hBxz.SetPoint(iz, z, bx);
    hByz.SetPoint(iz, z, by);
    hBzz.SetPoint(iz, z, bz);
  }

  // ---> Double loop over z (target) plane
  cout << "                           ... RICH entrance plane" << endl;
  fdx = (fxmax-fxmin) / Double_t(fmx);
  fdy = (fymax-fymin) / Double_t(fmy);
  z = fzref1;
  for (Int_t ix=0; ix<fmx; ix++) {
    x = fxmin + (Double_t(ix)+0.5) * fdx;
    for (Int_t iy=0; iy<fmy; iy++) {
      y = fymin + (Double_t(iy)+0.5) * fdy;
      bx = field->GetBx(x,y,z) / 10.;
      by = field->GetBy(x,y,z) / 10.;
      bz = field->GetBz(x,y,z) / 10.;
      b = TMath::Sqrt(bx*bx + by*by + bz*bz);
      hB1.SetBinContent(ix, iy, b);
    }
  }

  // ---> Double loop over z (RICH PM) plane
  cout << "                           ... RICH PM plane" << endl;
  fdx = (fxmax-fxmin) / Double_t(fmx);
  fdy = (fymax-fymin) / Double_t(fmy);
  z = fzref2;
  for (Int_t ix=0; ix<fmx; ix++) {
    x = fxmin + (Double_t(ix)+0.5) * fdx;
    for (Int_t iy=0; iy<fmy; iy++) {
      y = fymin + (Double_t(iy)+0.5) * fdy;
      bx = field->GetBx(x,y,z) / 10.;
      by = field->GetBy(x,y,z) / 10.;
      bz = field->GetBz(x,y,z) / 10.;
      b = TMath::Sqrt(bx*bx + by*by + bz*bz);
      hB2.SetBinContent(ix, iy, b);
    }
  }

  // ----------------------------------------------------------------------



  // -------  Draw graphs and histogram   ---------------------------------
  cout << endl << "=====>  Drawing..." << endl;
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0000);
  gStyle->SetPalette(1,0);
  gStyle->SetTitleW(0.5);
  gStyle->SetTitleAlign(13);
  gStyle->SetTitleBorderSize(0.);
  gStyle->SetTitleOffset(2.,"y");
  gStyle->SetOptDate(2);
  gStyle->GetAttDate()->SetTextSize(0.02);

  TPostScript* ps = new TPostScript(psFile, -111);
  ps->Range(20,30);

  TCanvas* c1 = new TCanvas("c1", "canvas", 768, 1152);
  TPad* master = new TPad("master","", 0.10, 0.05, 0.95, 0.95);
  master->Draw();
  master->cd();

  TPad* padinf = new TPad("padinf", "", 0.12, 0.80, 0.88, 0.90);
  TPad* pad1   = new TPad("pad1",   "", 0.03, 0.60, 0.36, 0.78);
  TPad* pad2   = new TPad("pad2",   "", 0.37, 0.60, 0.68, 0.78);
  TPad* pad3   = new TPad("pad3",   "", 0.69, 0.60, 0.98, 0.78);
  TPad* pad4   = new TPad("pad4",   "", 0.03, 0.41, 0.36, 0.59);
  TPad* pad5   = new TPad("pad5",   "", 0.37, 0.41, 0.68, 0.59);
  TPad* pad6   = new TPad("pad6",   "", 0.69, 0.41, 0.98, 0.59);
  TPad* pad7   = new TPad("pad7",   "", 0.03, 0.22, 0.36, 0.40);
  TPad* pad8   = new TPad("pad8",   "", 0.37, 0.22, 0.68, 0.40);
  TPad* pad9   = new TPad("pad9",   "", 0.69, 0.22, 0.98, 0.40);
  TPad* pad10  = new TPad("pad10",  "", 0.10, 0.02, 0.47, 0.20);
  TPad* pad11  = new TPad("pad11",  "", 0.53, 0.02, 0.90, 0.20);

  padinf->Draw();
  pad1->Draw();
  pad2->Draw();
  pad3->Draw();
  pad4->Draw();
  pad5->Draw();
  pad6->Draw();
  pad7->Draw();
  pad8->Draw();
  pad9->Draw();
  pad10->Draw();
  pad11->Draw();

  Double_t max, min;

  pad1->cd();
  gPad->SetFillColor(10);
  gPad->SetLeftMargin(0.15);
  hBxx.SetLineColor(4);
  hBxx.SetLineWidth(2);
  hBxx.SetTitle("y = 20 cm, z = 50 cm");
  hBxx.GetXaxis()->SetTitle("x [cm]");
  hBxx.GetYaxis()->SetTitle("B_{x} [T]");
  hBxx.Draw("AC");
  max = hBxx.GetHistogram()->GetMaximum();
  min = hBxx.GetHistogram()->GetMinimum();
  TLine l1(targX, min, targX, max);
  l1.Draw();

  pad2->cd();
  gPad->SetFillColor(10);
  gPad->SetLeftMargin(0.15);
  hByx.SetLineColor(2);
  hByx.SetLineWidth(2);
  hByx.SetTitle("y = 20 cm, z = 50 cm");
  hByx.GetXaxis()->SetTitle("x [cm]");
  hByx.GetYaxis()->SetTitle("B_{y} [T]");
  hByx.Draw("AC");
  max = hByx.GetHistogram()->GetMaximum();
  min = hByx.GetHistogram()->GetMinimum();
  TLine l2(targX, min, targX, max);
  l2.Draw();

  pad3->cd();
  gPad->SetFillColor(10);
  gPad->SetLeftMargin(0.15);
  hBzx.SetLineColor(3);
  hBzx.SetLineWidth(2);
  hBzx.SetTitle("y = 20 cm, z = 50 cm");
  hBzx.GetXaxis()->SetTitle("x [cm]");
  hBzx.GetYaxis()->SetTitle("B_{z} [T]");
  hBzx.Draw("AC");
  max = hBzx.GetHistogram()->GetMaximum();
  min = hBzx.GetHistogram()->GetMinimum();
  TLine l3(targX, min, targX, max);
  l3.Draw();

  pad4->cd();
  gPad->SetFillColor(10);
  gPad->SetLeftMargin(0.15);
  hBxy.SetLineColor(4);
  hBxy.SetLineWidth(2);
  hBxy.SetTitle("x = 20 cm, z = 50 cm");
  hBxy.GetXaxis()->SetTitle("y [cm]");
  hBxy.GetYaxis()->SetTitle("B_{x} [T]");
  hBxy.Draw("AC");
  max = hBxy.GetHistogram()->GetMaximum();
  min = hBxy.GetHistogram()->GetMinimum();
  TLine l4(targY, min, targY, max);
  l4.Draw();

  pad5->cd();
  gPad->SetLeftMargin(0.15);
  gPad->SetFillColor(10);
  hByy.SetLineColor(2);
  hByy.SetLineWidth(2);
  hByy.SetTitle("x = 20 xm, z = 50 cm");
  hByy.GetXaxis()->SetTitle("y [cm]");
  hByy.GetYaxis()->SetTitle("B_{y} [T]");
  hByy.Draw("AC");
  max = hByy.GetHistogram()->GetMaximum();
  min = hByy.GetHistogram()->GetMinimum();
  TLine l5(targY, min, targY, max);
  l5.Draw();

  pad6->cd();
  gPad->SetFillColor(10);
  gPad->SetLeftMargin(0.15);
  hBzy.SetLineColor(3);
  hBzy.SetLineWidth(2);
  hBzy.SetTitle("x = 20 cm, z = 50 cm");
  hBzy.GetXaxis()->SetTitle("y [cm]");
  hBzy.GetYaxis()->SetTitle("B_{z} [T]");
  hBzy.Draw("AC");
  max = hBzy.GetHistogram()->GetMaximum();
  min = hBzy.GetHistogram()->GetMinimum();
  TLine l6(targY, min, targY, max);
  l6.Draw();

  pad7->cd();
  gPad->SetFillColor(10);
  gPad->SetLeftMargin(0.15);
  hBxz.SetLineColor(4);
  hBxz.SetLineWidth(2);
  hBxz.SetTitle("x = y = 0");
  hBxz.GetXaxis()->SetTitle("z [cm]");
  hBxz.GetYaxis()->SetTitle("B_{x} [T]");
  hBxz.Draw("AC");
  max = hBxz.GetHistogram()->GetMaximum();
  min = hBxz.GetHistogram()->GetMinimum();
  TLine l7(targZ, min, targZ, max);
  l7.Draw();

  pad8->cd();
  gPad->SetFillColor(10);
  gPad->SetLeftMargin(0.15);
  hByz.SetLineColor(2);
  hByz.SetLineWidth(2);
  hByz.SetTitle("x = y = 0");
  hByz.GetXaxis()->SetTitle("z [cm]");
  hByz.GetYaxis()->SetTitle("B_{y} [T]");
  hByz.Draw("AC");
  max = hByz.GetHistogram()->GetMaximum();
  min = hByz.GetHistogram()->GetMinimum();
  TLine l8(targZ, min, targZ, max);
  l8.Draw();

  pad9->cd();
  gPad->SetFillColor(10);
  gPad->SetLeftMargin(0.15);
  hBzz.SetLineColor(3);
  hBzz.SetLineWidth(2);
  hBzz.SetTitle("x = y = 0");
  hBzz.GetXaxis()->SetTitle("z [cm]");
  hBzz.GetYaxis()->SetTitle("B_{z} [T]");
  hBzz.Draw("AC");
  max = hBzz.GetHistogram()->GetMaximum();
  min = hBzz.GetHistogram()->GetMinimum();
  TLine l9(targZ, min, targZ, max);
  l9.Draw();

  pad10->cd();
  gPad->SetFillColor(10);
  gPad->SetLeftMargin(0.15);
  hB1.UseCurrentStyle();
  hB1.GetXaxis()->SetTitle("x [cm]");
  hB1.GetYaxis()->SetTitle("y [cm]");
  char t[100];
  sprintf(t,"B [T] at z = %8.2f cm",fzref1);
  hB1->SetTitle(t);
  hB1.Draw("COLZ");

  pad11->cd();
  gPad->SetFillColor(10);
  gPad->SetLeftMargin(0.15);
  hB2.UseCurrentStyle();
  hB2.GetXaxis()->SetTitle("x [cm]");
  hB2.GetYaxis()->SetTitle("y [cm]");
  sprintf(t,"B [T] at z = %8.2f cm",fzref2);
  hB2.SetTitle(t);
  hB2.Draw("COLZ");


  TString fieldType = "";
  if      ( type == 0 ) fieldType = "Constant field";
  else if ( type == 1 ) fieldType = "Field map";
  else if ( type == 2 ) fieldType = "Field Map Sym2";
  else if ( type == 3 ) fieldType = "Field Map Sym3";


  char t1[300];
  padinf->cd();
  TPaveText info(0.1,0.1,0.9,0.9);
  info.SetFillColor(10);
  info.SetBorderSize(0);
  info.SetTextAlign(2);
  sprintf(t1,"Field name: %s, type: %s",field->GetName(),fieldType.Data());
  info.AddText(0.05, 0.9, t1);
  Double_t bx = field->GetBx(0.,0.,0.) / 10.;
  Double_t by = field->GetBy(0.,0.,0.) / 10.;
  Double_t bz = field->GetBz(0.,0.,0.) / 10.;
  sprintf(t1,"Field at target is (%7.4f, %7.4f, %7.4f) T",bx,by,bz);
  cout << t1 << endl;
  info.AddText(0.05, 0.7, t1);
  if ( type >= 1 && type <=3 ) {
    sprintf(t1, "Map file: %s",mapFile);
    cout << t1 << endl;
    info.AddText(0.05, 0.5, t1);
    Double_t xp = field->GetPositionX();
    Double_t yp = field->GetPositionY();
    Double_t zp = field->GetPositionZ();
    sprintf(t1,"Centre position (%.2f, %.2f, %.2f) cm",xp,yp,zp);
    info->AddText(0.05, 0.3, t1);
    Double_t scale = ((CbmFieldMap*)field)->GetScale();
    sprintf(t1,"Scaling factor %.2f",scale);
    info->AddText(0.05, 0.1, t1);
  }
  info.Draw();

  master->cd();
  sprintf(t1,"Symmetry check for %s", field->GetName());
  TPaveLabel label(0.20, 0.92, 0.88, 0.97, t1);
  label->Draw();
  
//   c1->cd();
//   const char* wrkdir = getenv("VMCWORKDIR");
//   TString logo = TString(wrkdir) + "/input/cbmlogo.gif";
//   TImage* cbm = TImage::Open(logo);
//   TPad* padimg = new TPad("padimg", "", 0.05, 0.80, 0.20, 0.95);
//   padimg->Draw();
//   padimg->cd();
//   cbm->Draw();
  


  ps->Close();
  
}


