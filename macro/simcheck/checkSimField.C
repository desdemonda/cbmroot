// --------------------------------------------------------------------------
//
// Macro for check of the magnetic field used in simulation
//
// V. Friese   06/03/2006
//
// --------------------------------------------------------------------------

{


  // -----   Adjust this part to your requirements   ----------------------
  // ---> File names
  TString inFile  = "test.mc.root";
  TString outFile = "/tmp/dummy.root";
  TString parFile = "params.root";
  TString psFile  = "fieldcheck.ps";
  // ---> Regions of interest for the field
  Double_t fxmin =  -200.;    // along x axis
  Double_t fxmax =   200.;
  Double_t fymin =  -200.;    // along y axis
  Double_t fymax =   200.;
  Double_t fzmin =  -300.;    // along z axis
  Double_t fzmax =   300.;
  // ----------------------------------------------------------------------


  
  // -----   Load libraries   ---------------------------------------------
  // Load libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libPostscript.so");
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libField");
  gSystem->Load("libBase");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libSts");
  gSystem->Load("libRich");
  gSystem->Load("libTrd");
  gSystem->Load("libTof");
  gSystem->Load("libEcal");
  // ----------------------------------------------------------------------

  
  // -----   Analysis run, runtime database, parameter container  ---------
  FairRunAna* run = new FairRunAna();
  run->SetInputFile(inFile);
  run->SetOutputFile(outFile);

  // Runtime database and parameter IO
  FairRuntimeDb* rtdb = FairRuntimeDb::instance();
  FairParRootFileIo* parIo = new FairParRootFileIo();
  parIo->open(parFile.Data());
  rtdb->setFirstInput(parIo);

  // Get geometry parameter container for PASSIVE and RICH
  CbmGeoPassivePar* passSet
    = (CbmGeoPassivePar*) rtdb->getContainer("CbmGeoPassivePar");
  CbmGeoRichPar* richSet 
    = (CbmGeoRichPar*) rtdb->getContainer("CbmGeoRichPar");

  // Init run and geometry
  cout << endl << "=====>  Initialising run..." << endl;
  run->Init();
  run->LoadGeometry();
  // ----------------------------------------------------------------------



  // -------  Get target and rich geometry  -------------------------------
  cout << endl << "=====>  Reading geometry..." << endl;
  Double_t fzref1, fzref2, fzref3;
  Double_t targX, targY, targZ;
  FairGeoVector v1, v2;

  TObjArray* passNodes = passSet->GetGeoPassiveNodes();
  FairGeoNode* target = passNodes->FindObject("targ");
  if ( ! target ) {
    cout << "Warning: target node not found!" << endl;
    targX = targY = targZ = 0.;
  }
  else {
    v1 = target->getLabTransform()->getTranslation();
    v2 = target->getCenterPosition().getTranslation();
    targX = v1.X() + v2.X();
    targY = v1.Y() + v2.Y();
    targZ = v1.Z() + v2.Z();
    cout << "=====>  Target position ( " << targX << ", " << targY
	 << ", " << targZ << " ) cm" << endl;
  }
  fzref1 = targZ;

  TObjArray* richActNodes  = richSet->GetGeoSensitiveNodes();
  TObjArray* richPassNodes = richSet->GetGeoPassiveNodes();
  FairGeoNode* richEnt 
    = (FairGeoNode*) richPassNodes->FindObject("rich1entrance");
  if ( ! richEnt ) {
    cout << "Warning: RICH entrance node not found!" << endl;
    fzref2 = 200.;
  }
  else {
    v1 = richEnt->getLabTransform()->getTranslation();
    v2 = richEnt->getCenterPosition().getTranslation();
    fzref2 = v1.Z() + v2.Z();
    cout << "=====>  RICH entrance at z = " << fzref2 << " cm " << endl;
  }
  FairGeoNode* richPm = (FairGeoNode*) richActNodes->FindObject("rich1d#1");
  if ( ! richPm ) {
    cout << "Warning: RICH PM node not found!" << endl;
    fzref3 = 200.;
  }
  else {
    v1 = richPm->getLabTransform()->getTranslation();
    v2 = richPm->getCenterPosition().getTranslation();
    fzref3 = v1.Z() + v2.Z();
    cout << "=====>  RICH PM at z = " << fzref3 << " cm " << endl;
  }
  // ----------------------------------------------------------------------
  


  // -------  Get magnetic field  -----------------------------------------
  cout << endl << "=====>  Getting magnetic field..." << endl;
  CbmField* field = run->GetField();
  field->Print();
  Int_t type = field->GetType();
  if ( type >=1 && type <= 3) 
    const char* mapFile = ((CbmFieldMap*)field)->GetFileName();
  // ----------------------------------------------------------------------



  // -------  Create graphs and histograms  -------------------------------
  Int_t fnx = TMath::Nint( (fxmax-fxmin) * 3. );
  Int_t fny = TMath::Nint( (fymax-fymin) * 3. );
  Int_t fnz = TMath::Nint( (fzmax-fzmin) * 3. );
  Int_t fmx = TMath::Nint( (fxmax-fxmin) * 3. / 10. );
  Int_t fmy = TMath::Nint( (fymax-fymin) * 3. / 10. );
  TGraph hBy1(fnz+1);
  TGraph hBy2(fnz+1);
  TGraph hBt1(fnz+1);
  TGraph hBt2(fnz+1);
  TH2F hB1("hB1", "B at z_{target}", 
	   fmx, fxmin, fxmax, fmy, fymin, fymax);
  TH2F hB2("hB2", "B at RICH entrance", 
	   fmx, fxmin, fxmax, fmy, fymin, fymax);
  TH2F hB3("hB3", "B at RICH PM",       
	   fmx, fxmin, fxmax, fmy, fymin, fymax);
  // ----------------------------------------------------------------------



  // -------  Get field and fill graphs and histograms  -------------------
  cout << endl;
  cout << "=====>  Filling histograms..." << endl;
  Double_t x, y, z;
  Double_t bx, by, bz, bt, b;
  Double_t fdx = (fxmax-fxmin) / Double_t(fnx);
  Double_t fdy = (fymax-fymin) / Double_t(fny);
  Double_t fdz = (fzmax-fzmin) / Double_t(fnz);

  // ---> Loop over z axis (x=y=0)
  cout << "                           ... z axis 1" << endl;
  x = y = 0.;
  for (Int_t iz=0; iz<=fnz; iz++) {
    z = fzmin + Double_t(iz) * fdz;
    bx = field->GetBx(x,y,z) / 10.;
    by = field->GetBy(x,y,z) / 10.;
    bz = field->GetBz(x,y,z) / 10.;
    bt = TMath::Sqrt(bx*bx + bz*bz);
    hBy1.SetPoint(iz, z, by);
    hBt1.SetPoint(iz, z, bt);
  }

  // ---> Loop over z axis (x=y=20)
  cout << "                           ... z axis 2" << endl;
  x = y = 20.;
  for (Int_t iz=0; iz<=fnz; iz++) {
    z = fzmin + Double_t(iz) * fdz;
    bx = field->GetBx(x,y,z) / 10.;
    by = field->GetBy(x,y,z) / 10.;
    bz = field->GetBz(x,y,z) / 10.;
    bt = TMath::Sqrt(bx*bx + bz*bz);
    hBy2.SetPoint(iz, z, by);
    hBt2.SetPoint(iz, z, bt);
  }

  // ---> Double loop over z (target) plane
  cout << "                           ... target plane" << endl;
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

  // ---> Double loop over z (RICH entrance) plane
  cout << "                           ... RICH entrance plane" << endl;
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

  // ---> Double loop over z (RICH PM) plane
  cout << "                           ... RICH PM plane" << endl;
  fdx = (fxmax-fxmin) / Double_t(fmx);
  fdy = (fymax-fymin) / Double_t(fmy);
  z = fzref3;
  for (Int_t ix=0; ix<fmx; ix++) {
    x = fxmin + (Double_t(ix)+0.5) * fdx;
    for (Int_t iy=0; iy<fmy; iy++) {
      y = fymin + (Double_t(iy)+0.5) * fdy;
      bx = field->GetBx(x,y,z) / 10.;
      by = field->GetBy(x,y,z) / 10.;
      bz = field->GetBz(x,y,z) / 10.;
      b = TMath::Sqrt(bx*bx + by*by + bz*bz);
      hB3.SetBinContent(ix, iy, b);
    }
  }
  // ----------------------------------------------------------------------



  // --------   Calculate field integral from target along z axis   -------
  Double_t zint1 = targZ;
  Double_t zint2 = 400.;
  Int_t nz = Int_t(zint2-zint1);
  Double_t bint = 0;
  z = zint1 - 1.;
  for (Int_t iz=0; iz<=nz; iz++) {
    z += 1;
    by = field->GetBy(targX, targY, z) / 10.;
    bint += by * 0.01;
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
  TPad* pad1   = new TPad("pad1",   "", 0.05, 0.60, 0.48, 0.78);
  TPad* pad2   = new TPad("pad2",   "", 0.52, 0.60, 0.95, 0.78);
  TPad* pad3   = new TPad("pad3",   "", 0.05, 0.41, 0.48, 0.59);
  TPad* pad4   = new TPad("pad4",   "", 0.52, 0.41, 0.95, 0.59);
  TPad* pad5   = new TPad("pad5",   "", 0.30, 0.22, 0.70, 0.40);
  TPad* pad6   = new TPad("pad6",   "", 0.10, 0.02, 0.47, 0.20);
  TPad* pad7   = new TPad("pad7",   "", 0.53, 0.02, 0.90, 0.20);

  padinf->Draw();
  pad1->Draw();
  pad2->Draw();
  pad3->Draw();
  pad4->Draw();
  pad5->Draw();
  pad6->Draw();
  pad7->Draw();

  Double_t max, min;

  pad1->cd();
  gPad->SetFillColor(10);
  gPad->SetLeftMargin(0.15);
  hBy1.SetLineColor(4);
  hBy1.SetLineWidth(2);
  hBy1.SetTitle("x = y = 0");
  hBy1.GetXaxis()->SetTitle("z [cm]");
  hBy1.GetYaxis()->SetTitle("B_{y} [T]");
  hBy1.Draw("AC");
  max = hBy1.GetHistogram()->GetMaximum();
  min = hBy1.GetHistogram()->GetMinimum();
  TLine l1(targZ, min, targZ, max);
  l1.Draw();

  pad2->cd();
  gPad->SetFillColor(10);
  gPad->SetLeftMargin(0.15);
  hBy2.SetLineColor(4);
  hBy2.SetLineWidth(2);
  hBy2.SetTitle("x = y = 20 cm");
  hBy2.GetXaxis()->SetTitle("z [cm]");
  hBy2.GetYaxis()->SetTitle("B_{y} [T]");
  hBy2.Draw("AC");
  max = hBy2.GetHistogram()->GetMaximum();
  min = hBy2.GetHistogram()->GetMinimum();
  TLine l2(targZ, min, targZ, max);
  l2.Draw();

  pad3->cd();
  gPad->SetFillColor(10);
  gPad->SetLeftMargin(0.15);
  hBt1.SetLineColor(2);
  hBt1.SetLineWidth(2);
  hBt1.SetTitle("x = y = 0");
  hBt1.GetXaxis()->SetTitle("z [cm]");
  hBt1.GetYaxis()->SetTitle("B_{xz} [T]");
  hBt1.Draw("AC");
  max = hBt1.GetHistogram()->GetMaximum();
  min = hBt1.GetHistogram()->GetMinimum();
  TLine l3(targZ, min, targZ, max);
  l3.Draw();

  pad4->cd();
  gPad->SetFillColor(10);
  gPad->SetLeftMargin(0.15);
  hBt2.SetLineColor(2);
  hBt2.SetLineWidth(2);
  hBt2.SetTitle("x = y = 20 cm");
  hBt2.GetXaxis()->SetTitle("z [cm]");
  hBt2.GetYaxis()->SetTitle("B_{xz} [T]");
  hBt2.Draw("AC");
  max = hBt2.GetHistogram()->GetMaximum();
  min = hBt2.GetHistogram()->GetMinimum();
  TLine l4(targZ, min, targZ, max);
  l4.Draw();

  pad5->cd();
  gPad->SetLeftMargin(0.15);
  gPad->SetFillColor(10);
  hB1.UseCurrentStyle();
  hB1.GetXaxis()->SetTitle("x [cm]");
  hB1.GetYaxis()->SetTitle("y [cm]");
  char t[100];
  sprintf(t,"B [T] at target (z=%8.2f cm)",fzref1);
  hB1.SetTitle(t);
  hB1.Draw("COLZ");

  pad6->cd();
  gPad->SetFillColor(10);
  gPad->SetLeftMargin(0.15);
  hB2.UseCurrentStyle();
  hB2.GetXaxis()->SetTitle("x [cm]");
  hB2.GetYaxis()->SetTitle("y [cm]");
  char t[100];
  sprintf(t,"B [T] at RICH entrance (z=%8.2f cm)",fzref2);
  hB2->SetTitle(t);
  hB2.Draw("COLZ");

  pad7->cd();
  gPad->SetFillColor(10);
  gPad->SetLeftMargin(0.15);
  hB3.UseCurrentStyle();
  hB3.GetXaxis()->SetTitle("x [cm]");
  hB3.GetYaxis()->SetTitle("y [cm]");
  sprintf(t,"B [T] at RICH PM plane (z=%8.2f cm)",fzref3);
  hB3.SetTitle(t);
  hB3.Draw("COLZ");


  TString fieldType = "";
  if      ( type == 0 ) fieldType = "Constant field";
  else if ( type == 1 ) fieldType = "Field map";
  else if ( type == 2 ) fieldType = "Field Map Sym2";
  else if ( type == 3 ) fieldType = "Field Map Sym3";
  Double_t xp = 0.;
  Double_t yp = 0.;
  Double_t zp = 0.;
  if ( type >=1 && type <=3 ) {
    xp = ((CbmFieldMap*)field)->GetPositionX();
    yp = ((CbmFieldMap*)field)->GetPositionY();
    zp = ((CbmFieldMap*)field)->GetPositionZ();
  }


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
    sprintf(t1,"Centre position (%.2f, %.2f, %.2f) cm",xp,yp,zp);
    info->AddText(0.05, 0.3, t1);
    Double_t scale = ((CbmFieldMap*)field)->GetScale();
    sprintf(t1,"Scaling factor %.2f, Field integral along z = %.4f Tm",
	    scale,bint);
    info->AddText(0.05, 0.1, t1);
  }
  info.Draw();

  master->cd();
  sprintf(t1,"Field check for file %s", inFile.Data());
  TPaveLabel label(0.20, 0.92, 0.88, 0.97, t1);
  label->Draw();
  
  c1->cd();
  const char* wrkdir = getenv("VMCWORKDIR");
  TString logo = TString(wrkdir) + "/input/cbmlogo.gif";
  	cout << "Logo is " << logo << endl;
  TImage* cbm = TImage::Open(logo);
  TPad* padimg = new TPad("padimg", "", 0.05, 0.80, 0.20, 0.95);
  padimg->Draw();
  padimg->cd();
  cbm->Draw();
  


  ps->Close();
  
}


