#if !defined(__CINT__) || defined(__MAKECINT__)
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "TH1.h"
#include "TVector3.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TProfile2D.h"

#include "../../base/event/FairRadLenPoint.h"

#include <iostream>
#include <vector>
using std::cout;
using std::endl;
using std::vector;
#endif

Int_t matbudget_ana(const char* stsGeo, Int_t nEvents=10000000)
{
  TString dir = gSystem->Getenv("VMCWORKDIR");
  TString file = dir + "/gconfig/basiclibs.C";
  gROOT->LoadMacro(file);
  gROOT->ProcessLine("basiclibs()");
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libCbmBase");
	
  // Input file (MC)
  TString stsVersion(stsGeo);
  TString inFile = "data/matbudget." + stsVersion + ".mc.root";
  TFile* input = new TFile(inFile);
	if ( ! input ) {
		cout << "*** matbudget_ana: Input file " << inFile << " not found!\n"
				 << "Bes sure to run matbudget_mc.C before for the respective "
				 << "STS geometry!" << endl;
		exit;
	}

	// Output file (material maps)
	TString outFile = "sts_matbudget_" + stsVersion + ".root";

	// Input tree and branch
  TTree* tree = (TTree*) input->Get("cbmsim") ;
  TClonesArray* points = new TClonesArray("FairRadLenPoint");
  tree->SetBranchAddress("RadLen", &points);

  // Create output histograms
  TH1D* hisRadLen = new TH1D("hisRadLen","Radiation Length", 1000,0,100);
  const int nStations = 8;       // number of STS stations
  const int nBins     = 1000;    // number of bins in histograms (in both x and y)
  const int rMax      = 55;      // maximal radius for histograms (for both x and y)
  TProfile2D* hStaRadLen[nStations];
  for ( int i = 0; i < nStations; ++i ) {
    TString name = "Radiation Thickness [%],";
    name += " Station";
    name += i+1;
    hStaRadLen[i] = new TProfile2D(name, name, nBins,-rMax, rMax, nBins,-rMax, rMax);
  }
  
  // Auxiliary variables
  TVector3 vecs,veco;
  std::map<int,int> trackHitMap;


  // Event loop
  int firstEvent = 0;
  for (Int_t event = firstEvent;
  		 event < (firstEvent + nEvents) && event<tree->GetEntriesFast();
  		 event++) {
    tree->GetEntry(event);
    if ( 0 == event%10000 )  cout << "*** Processing event "<< event << endl;

    const int nTracks = 1;
    std::vector<double> RadLengthOnTrack (nTracks,0.0); //trackID, vector with points on track
    std::vector<double> TrackLength (nTracks,0.0); //trackID, vector with points on track

    vector<double> RadThick(nStations,0);
    double x,y;

    // For this implementation to be correct, there should be only one MCTrack per event.

    // Point loop
    for (Int_t iPoint = 0; iPoint < points->GetEntriesFast(); iPoint++) {
      FairRadLenPoint *point=(FairRadLenPoint*) points->At(iPoint);

      // Get track position at entry and exit of material
      TVector3 posIn, posOut, posDif;
      posIn  = point->GetPosition();
      posOut = point->GetPositionOut();
      posDif = 0.5* (posOut - posIn);

      // Midpoint between in and out
      const TVector3 middle = (posOut + posIn);
      x = middle.X()/2;
      y = middle.Y()/2;
      const double z = middle.Z()/2;
      
      // Material budget per unit length
      const double radThick = posDif.Mag() / point->GetRadLength();
      RadLengthOnTrack[point->GetTrackID()] += radThick;
      TrackLength[point->GetTrackID()] += posDif.Mag();

      // Determine station number
      int iStation    =  posIn.Z()/10  - 3 + 0.5; // suppose equidistant stations at 30-100 cm
      int iStationOut =  posOut.Z()/10 - 3 + 0.5;
      if ( iStationOut != iStation) continue;
      if ( iStation >= nStations || iStation < 0 ) continue;
      RadThick[iStation] += radThick;

    }
    
    // Fill material budget map for each station
    for ( int i = 0; i < nStations; ++i )
      hStaRadLen[i]->Fill( x, y, RadThick[i]*100 );
    
    for (int k = 0; k < RadLengthOnTrack.size(); k++)
      if (RadLengthOnTrack[k] > 0)
      	hisRadLen->Fill(RadLengthOnTrack[k]);

  } // event loop



  // Plotting the results
  TCanvas* can1 = new TCanvas();
  can1->Divide(nStations/2,2);
  gStyle->SetPalette(1);
  gStyle->SetOptStat(0);

  // Open output file
  TFile* output = new TFile(outFile, "RECREATE");
  output->cd();
  
  for ( int iStation = 0; iStation < nStations; iStation++) {
    can1->cd(iStation+1);
    hStaRadLen[iStation]->GetXaxis()->SetTitle("x [cm]");
    hStaRadLen[iStation]->GetYaxis()->SetTitle("y [cm]");
    //hStaRadLen[i]->GetZaxis()->SetTitle("radiation thickness [%]");
    hStaRadLen[iStation]->SetAxisRange(0, 2, "Z");
    hStaRadLen[iStation]->Draw("colz");
    hStaRadLen[iStation]->Write();
  }

  // Plot file
  TString plotFile = "sts_matbudget_" + stsVersion + ".png";
  can1->SaveAs("plot");

  // Close files
  input->Close();
  output->Close();
  cout << "Material budget maps written to " << outFile << endl;

  return 0;
}

