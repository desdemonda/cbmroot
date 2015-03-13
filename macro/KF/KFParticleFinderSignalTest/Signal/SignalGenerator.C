//---------------------------------------------------------------------------------
// @author  M. Zyzak
// @version 1.0
// @since   15.08.14
// 
// macro to generate signal events for KFParticleFinder
//_________________________________________________________________________________

void SignalGenerator(Int_t iParticle = 0 )
{
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libCbmBase");
  gSystem->Load("libCbmData");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libMvd");
  gSystem->Load("libSts");
  gSystem->Load("libTrd");
  gSystem->Load("libTof");
  gSystem->Load("libEcal");
  gSystem->Load("libGlobal");
  gSystem->Load("libKF");
  gSystem->Load("libL1");
  gSystem->Load("libMinuit2"); // Needs for rich ellipse fitter
  
  Style();
  TCanvas *c1 = new TCanvas("c1","canvas",0,0,800,600);
  const double kProtonMass = 0.938272321;    // Proton mass in GeV

  KFPartEfficiencies eff;

  const double kSignalMass = eff.partMass[iParticle];    //  mass in GeV
  const int    kSignalID =  eff.partPDG[iParticle];

  const double kKLifeTime  = 3.0*1.639;        // Mean Ksi- life time in cm;

  const double kTwoPi      = 2.*TMath::Pi(); // 2*pi
  double fSlope = 0.2;
  double fRapSigma     = 0.448;
  double eBeam = 25.;
  double pBeam = TMath::Sqrt(eBeam*eBeam - kProtonMass*kProtonMass);
  double fYcm = 0.25*TMath::Log( (eBeam+pBeam) / (eBeam-pBeam) );
  
  fThermal = new TF1("thermal", "x*exp(-1.*sqrt(x*x+[1]*[1]) / [0])", 0., 10.);
  fThermal->SetParameter(0, fSlope);
  fThermal->SetParameter(1, kSignalMass);
  fRandGen = new TRandom(0);
  
  double zmax = 100.*kKLifeTime;
  if (zmax > 70.) zmax =70.;
  TH1F *Pi_pt = new TH1F("Signal_pt", "Signal pt", 100., 0., 5.);
  Pi_pt->SetLineColor(4);
  Pi_pt->SetFillColor(7);
  (Pi_pt->GetXaxis())->SetTitle("pt (GeV/c)");
  (Pi_pt->GetYaxis())->SetTitle("Entries (a.u.)");
  
  TH1F *Pi_Y = new TH1F("Signal", "Y", 100., -10., 10.);
   Pi_Y->SetLineColor(4);
  (Pi_Y->GetXaxis())->SetTitle("Y");
  (Pi_Y->GetYaxis())->SetTitle("Entries (a.u.)");
  
  // Open output file
  ofstream outputfile;
  outputfile.open("Signal.txt");
  if ( ! outputfile.is_open() ) {
    cout << "-E Signal (generator): Cannot open output file " << endl;
    return;
  }
  
  int NEvent = 100000;
  int PionsPerEvent = 1;
  for (int i=0; i<NEvent; i++){
  // Generate rapidity, pt and azimuth
    outputfile<<PionsPerEvent<<"   "<<i + 1<<"  "<<0.<<"  "<<0.<<"  "<<0.<<endl;
    for(int j=0;j<PionsPerEvent;++j) {      
      double yD   = fRandGen->Gaus(fYcm, fRapSigma);
      double ptD  = fThermal->GetRandom();
      double phiD = fRandGen->Uniform(0., kTwoPi);
        
      // Calculate momentum, energy, beta and gamma
      double pxD    = ptD * TMath::Cos(phiD);
      double pyD    = ptD * TMath::Sin(phiD);
      double mtD    = TMath::Sqrt(kSignalMass*kSignalMass + ptD*ptD);
      double pzD    = mtD * TMath::SinH(yD);
      double pD     = TMath::Sqrt(pzD*pzD + ptD*ptD);
      double eD     = mtD * TMath::CosH(yD);
      double betaD  = pD / eD;
      double gammaD = 1. / TMath::Sqrt(1.-betaD*betaD);
      double thetaD = TMath::ATan2(ptD, pzD);

      outputfile<<kSignalID<<"  "<<pxD<<"  "<<pyD<<"  "<<pzD<<endl;
        
      Pi_pt->Fill(ptD);
      Pi_Y->Fill(yD);
    }
  }
  
  c1->Divide(2,1);
  c1->cd(1);
  Pi_pt->Draw();
  c1->cd(2);
  Pi_Y->Draw();

  outputfile.close();
  
  TFile fh("Signal.root","recreate");
  Pi_pt->Write();
  Pi_Y->Write();
  fh.Close();
}

void Style()
{
  gSystem->Load("libPostscript.so");
  gStyle->SetCanvasBorderMode(-1);
  gStyle->SetCanvasBorderSize(1);
  gStyle->SetCanvasColor(10);

  gStyle->SetFrameFillColor(10);
  gStyle->SetFrameBorderSize(1);
  gStyle->SetFrameBorderMode(-1);
  gStyle->SetFrameLineWidth(1.2);
  gStyle->SetFrameLineColor(1);

  gStyle->SetHistFillColor(0);
  gStyle->SetHistLineWidth(2.);
  gStyle->SetHistLineColor(1);

  gStyle->SetPadColor(10);
  gStyle->SetPadBorderSize(1);
  gStyle->SetPadBorderMode(-1);

  gStyle->SetStatColor(10);
  gStyle->SetTitleColor(1);

  gStyle->SetTitleFont(22,"xyz");
  gStyle->SetTitleSize(0.08,"xyz"); // axis titles
  gStyle->SetLabelFont(22,"xyz");
  gStyle->SetLabelFont(22,"xyz");
  gStyle->SetLabelSize(0.07,"xyz"); 
  gStyle->SetStatFont(22);
  gStyle->SetLabelOffset(0.015,"xyz");

  gStyle->SetTitleOffset(1.2,"xyz");

  gStyle->SetOptDate(0);
  gStyle->SetOptTitle(1);
  gStyle->SetPadTopMargin(0.08);
  gStyle->SetPadBottomMargin(0.2);
  gStyle->SetPadLeftMargin(0.2);
  gStyle->SetPadRightMargin(0.1);

  gStyle->SetNdivisions(505,"xy");
}
 