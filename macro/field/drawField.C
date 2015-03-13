void drawField()
{


  TString fieldName = "field_v12b";
  Double_t fzmin =  -300.;    // along z axis
  Double_t fzmax =   300.;


  
  // -----   Load libraries   ---------------------------------------------
  // Load libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libField");
  // ----------------------------------------------------------------------

  CbmFieldMap* field = new CbmFieldMapSym3(fieldName.Data());
  field->SetPosition(0., 0., 50.);

  field->SetFieldRegionNew(-1000.,1000.,-1000., 1000, 30.,1000.);

  field->Init();
  field->Print();

  // -------  Create graphs and histograms  -------------------------------
  Int_t fnz = TMath::Nint( (fzmax-fzmin) * 3. );
  Double_t* z = new Double_t [fnz+1];
  Double_t* By =new Double_t [fnz+1];

  Double_t targZ = 0.;

  Double_t x, y;
  Double_t by;//, bz, bt, b;
  //  Double_t fdx = (fxmax-fxmin) / Double_t(fnx);
  //  Double_t fdy = (fymax-fymin) / Double_t(fny);
  Double_t fdz = (fzmax-fzmin) / Double_t(fnz);


  // ---> Loop over z axis (x=y=0)
  x = y = 0.;
  Double_t fieldval[3];
  Double_t point[3];
  for (Int_t iz=0; iz<=fnz; iz++) {
    z[iz] = fzmin + Double_t(iz) * fdz;
//    By[iz] = field->GetBy(x,y,z[iz]) / 10.;
    point[0]=x;
    point[1]=y;
    point[2]=z[iz];
    field->GetFieldValue(point, fieldval);
    By[iz] = fieldval[1];
  }

  TCanvas* c1 = new TCanvas("c1", "canvas", 768, 1152);
  TGraph* hBy1 = new TGraph(fnz+1,z,By);

  hBy1->SetLineColor(4);
  hBy1->SetLineWidth(2);
  hBy1->SetTitle("x = y = 0");
  hBy1->GetXaxis()->SetTitle("z [cm]");
  hBy1->GetYaxis()->SetTitle("B_{y} [T]");
  hBy1->Draw("ACP");
  max = hBy1->GetHistogram()->GetMaximum();
  min = hBy1->GetHistogram()->GetMinimum();
  TLine* l1 = new TLine(targZ, min, targZ, max);
  l1->Draw();

  c1->Update();
  
}
