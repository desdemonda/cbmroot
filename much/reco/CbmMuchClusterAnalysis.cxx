/*
 * CbmMuchClusterAnalysis.cxx
 *
 *  Created on: July 2, 2009
 *      Author: M. Ryzhinskiy <m.ryzhinskiy@gsi.de>
 */

#include "CbmMuchClusterAnalysis.h"
#include "CbmMuchGeoScheme.h"
#include "CbmMuchCluster.h"
#include "CbmMuchDigi.h"
#include "CbmMuchDigiMatch.h"
#include "CbmMuchModuleGem.h"
#include "CbmMuchPad.h"
#include "CbmMuchPoint.h"
#include "CbmMCTrack.h"

#include "FairRootManager.h"

#include "TClonesArray.h"
#include "TH1.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TStyle.h"

#include <set>

using std::set;

CbmMuchClusterAnalysis::CbmMuchClusterAnalysis(const char* name, Int_t verbose)
  : FairTask(name,verbose),
    fEvent(0),
    fPoints(NULL),
    fDigis(NULL),
    fDigiMatches(NULL),
    fClusters(NULL),
    fHits(NULL),
    fMCTracks(NULL),
    fGeoFileName("much.digi.root"),
    fGeoScheme(CbmMuchGeoScheme::Instance()),
    fChargeTotal(NULL),
    fClusterSize(),
    fClusterPoints(),
    fClusterPadsCharge(),
    fClusterRadii(),
    fClusterMeanEuclide(),
    fSingleMuCluster()
{
}

CbmMuchClusterAnalysis::~CbmMuchClusterAnalysis() {
}

// -------------------------------------------------------------------------
InitStatus CbmMuchClusterAnalysis::Init()
{
  FairRootManager* manager = FairRootManager::Instance();
  fMCTracks    = (TClonesArray*) manager->GetObject("MCTrack");
  fPoints      = (TClonesArray*) manager->GetObject("MuchPoint");
  fHits        = (TClonesArray*) manager->GetObject("MuchPixelHit");
  fDigis       = (TClonesArray*) manager->GetObject("MuchDigi");
  fDigiMatches = (TClonesArray*) manager->GetObject("MuchDigiMatch");
  fClusters    = (TClonesArray*) manager->GetObject("MuchCluster");

  TFile* f = new TFile(fGeoFileName,"R");
  TObjArray* stations = (TObjArray*) f->Get("stations");
  fGeoScheme->Init(stations);


  // Create histograms
  fChargeTotal = new TH1F("Charge on pads","Charge on pads", 255, 0, 255);
  for(Int_t iStation = 0; iStation < fGeoScheme->GetNStations(); ++iStation){
    // Cluster size distribution
    TH1F* clusterSizeHist = new TH1F(Form("Cluster size, station %i",iStation+1), Form("Station %i",iStation+1), 9, 1,10);
    fClusterSize.push_back(clusterSizeHist);

    // Single muon clusters distribution
    TH1F* singleMuCluster = new TH1F(Form("Single mu clusters, station %i",iStation+1), Form("Station %i",iStation+1), 10, 0,1);
    fSingleMuCluster.push_back(singleMuCluster);

    vector<TH1F*> clusterPointsHistos;
    vector<vector<TH1F*> > clusterPadCharges;
    vector<vector<TH1F*> > clusterRadii;
    vector<vector<TH1F*> > clusterEuclide;
    for(Int_t iSize=0; iSize<6; ++iSize){
      // Number of points distribution in the given cluster
      TH1F* clusterPointsHist = new TH1F(Form("Number of points in %i-pad clusters, station %i",iSize+1, iStation+1),
          Form("%i-pad clusters",iSize+1), 4, 1,5);
      clusterPointsHistos.push_back(clusterPointsHist);

      vector<TH1F*> chargesVector;
      vector<TH1F*> radiiVector;
      vector<TH1F*> euclideVector;
      for(Int_t iPoint=0; iPoint<4; ++iPoint){
        // Pad-charge distribution in the given cluster
        TH1F* clusterPadCharge = new TH1F(Form("Pad-charge in %i-pad clusters with %i points, station %i",iSize+1, iPoint+1, iStation+1),
            Form("%i-pad clusters",iSize+1), 255, 0,255);
        chargesVector.push_back(clusterPadCharge);

        // Cluster radii distribution
        TH1F* clusterRadius = new TH1F(Form("Cluster radii for %i-pad clusters with %i points, station %i",iSize+1, iPoint+1, iStation+1),
            Form("%i-pad clusters",iSize+1), 80,0,250);
        radiiVector.push_back(clusterRadius);

        // Mean Euclidean distance distribution
        TH1F* meanEuclide = new TH1F(Form("Mean Euclidean distance for %i-pad clusters with %i points, station %i",iSize+1, iPoint+1, iStation+1),
            Form("%i-pad clusters",iSize+1), 25,0,150);
        euclideVector.push_back(meanEuclide);

      } // Number of points loop
      clusterPadCharges.push_back(chargesVector);
      clusterRadii.push_back(radiiVector);
      clusterEuclide.push_back(euclideVector);
    } // Cluster size loop
    fClusterPoints.push_back(clusterPointsHistos);
    fClusterPadsCharge.push_back(clusterPadCharges);
    fClusterRadii.push_back(clusterRadii);
    fClusterMeanEuclide.push_back(clusterEuclide);
  } // Stations loop

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMuchClusterAnalysis::SetParContainers() {
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMuchClusterAnalysis::Exec(Option_t * option){
  ++fEvent;
  if(fEvent%50==0)
    gLogger->Info(MESSAGE_ORIGIN,"Event: %i",fEvent);

  Int_t nStations = fGeoScheme->GetNStations();
  vector<Int_t> nSingleMuClusters(nStations, 0);

  // Loop over clusters
  Int_t nClusters = fClusters->GetEntriesFast();
  for(Int_t iCluster=0; iCluster<nClusters; ++iCluster){
    CbmMuchCluster* cluster = (CbmMuchCluster*) fClusters->At(iCluster);
    Int_t iDigi = cluster->GetDigiIndex(0);
    CbmMuchDigi* digi = (CbmMuchDigi*) fDigis->At(iDigi);
    Int_t iStation = CbmMuchAddress::GetStationIndex(digi->GetDetectorId());

    if(IsSingleMuCluster(cluster)) nSingleMuClusters.at(iStation)++;

    // Fill cluster size histogram
    Int_t nPads = cluster->GetNDigis();
    fClusterSize.at(iStation)->Fill(nPads);

    Double_t xNominator = 0;
    Double_t yNominator = 0;
    Int_t totalQ = 0;
    std::set<Int_t> points;
    for(Int_t i=0; i<nPads; ++i){
      Int_t digiIndex = cluster->GetDigiIndex(i);
      CbmMuchDigi* d = (CbmMuchDigi*) fDigis->At(digiIndex);
      CbmMuchDigiMatch* m = (CbmMuchDigiMatch*) fDigiMatches->At(digiIndex);

      for(Int_t j=0; j<m->GetNPoints(); ++j){
        Int_t iPoint = m->GetRefIndex(j);
        if(points.find(iPoint) == points.end()) points.insert(iPoint);
      }

      // Fill total charge histogram
      fChargeTotal->Fill(d->GetADCCharge());

      // Calculate parts for center of cluster's gravity
      CbmMuchModuleGem*  module = (CbmMuchModuleGem*)fGeoScheme->GetModuleByDetId(d->GetDetectorId());
      CbmMuchPad* pad = module->GetPad(d->GetChannelId());
      xNominator += pad->GetX0()*d->GetADCCharge();
      yNominator += pad->GetY0()*d->GetADCCharge();
      totalQ += d->GetADCCharge();
    }

    if(nPads > 6) continue; // Process only cluster sizes < 7 pads
    Int_t nPoints = points.size();
    // Fill number of points histogram
    fClusterPoints[iStation][nPads-1]->Fill(nPoints);

    // Fill cluster radii histogram
    if(nPoints>4) continue; // Process only pads with number of contributed points < 5
    Double_t xCluster = xNominator/totalQ;
    Double_t yCluster = yNominator/totalQ;
    Double_t radius = TMath::Sqrt(xCluster*xCluster + yCluster*yCluster);
    fClusterRadii[iStation][nPads-1][nPoints-1]->Fill(radius);

    // Fill mean Euclidean distance
    fClusterMeanEuclide[iStation][nPads-1][nPoints-1]->Fill(GetMeanDistance(cluster));

    // Fill pad-charge histograms
    for(Int_t iPad=0; iPad < nPads; iPad++){
      Int_t digiIndex = cluster->GetDigiIndex(iPad);
      CbmMuchDigi* d = (CbmMuchDigi*) fDigis->At(digiIndex);
      Double_t charge = d->GetADCCharge();
      fClusterPadsCharge[iStation][iPad][nPoints-1]->Fill(charge);
    }
  }

  vector<Int_t> nSignalMuons = GetNSignalMuons();
  for(Int_t iStation = 0; iStation < nStations; ++iStation){
    fSingleMuCluster.at(iStation)->Fill((Double_t)nSingleMuClusters.at(iStation)/nSignalMuons.at(iStation));
  }
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Bool_t CbmMuchClusterAnalysis::IsSingleMuCluster(CbmMuchCluster* cluster){
  Int_t iPoint = -1;
  for(Int_t i = 0; i < cluster->GetNDigis(); ++i){
    Int_t iDigi = cluster->GetDigiIndex(i);
    CbmMuchDigiMatch* match = (CbmMuchDigiMatch*) fDigiMatches->At(iDigi);
    // Only one point in the pad
    if(match->GetNPoints() > 1) return false;
    // The same point as in previous pads
    if(iPoint < 0) iPoint = match->GetRefIndex();
    if(iPoint != match->GetRefIndex()) return false;
  }
  Bool_t isParticle = iPoint >= 0; // Avoid noise

  CbmMuchPoint* point = (CbmMuchPoint*)fPoints->At(iPoint);
  Int_t trackId = point->GetTrackID();
  CbmMCTrack* track = (CbmMCTrack*) fMCTracks->At(trackId);
  Int_t pdgCode = track->GetPdgCode();

  Bool_t isMuon = TMath::Abs(pdgCode) == 13; // Only muons
  Bool_t isSignal = track->GetMotherId() == 0; // Only signal
  return  isParticle && isSignal && isMuon;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
vector<Int_t> CbmMuchClusterAnalysis::GetNSignalMuons(){
  Int_t nStations = fGeoScheme->GetNStations();
  vector<Int_t> signalMuons(nStations, 0);
  for(Int_t iPoint = 0; iPoint < fPoints->GetEntriesFast(); +iPoint){
    CbmMuchPoint* point = (CbmMuchPoint*) fPoints->At(iPoint);
    Int_t iStation = CbmMuchAddress::GetStationIndex(point->GetDetectorId());
    Int_t trackId = point->GetTrackID();
    CbmMCTrack* track = (CbmMCTrack*)fMCTracks->At(trackId);
    Int_t pdgCode = track->GetPdgCode();
    if(TMath::Abs(pdgCode) == 13 && track->GetMotherId() == 0)
      signalMuons.at(iStation)++;
  }
  return signalMuons;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMuchClusterAnalysis::FinishTask(){
  gStyle->SetOptStat(1111110);

  TCanvas *c = new TCanvas("Charge", "Charge", 10,10, 400,400);
  fChargeTotal->SetLineWidth(2);
  fChargeTotal->GetXaxis()->SetTitle("ADC channels");
  fChargeTotal->Scale(1./fChargeTotal->Integral());
  fChargeTotal->Draw();
  c->Print(".eps");
  c->Print(".png");

  TCanvas *c1 = new TCanvas(Form("Cluster size"), Form("Cluster size"), 20, 20, 1200, 800);
  c1->Divide(3,2);

  TCanvas *c6 = new TCanvas(Form("Single mu clusters fraction"),
      Form("Single mu clusters fraction"), 30, 30, 2000, 400);
  c6->Divide(3,2);
  for(Int_t iStation=0; iStation<fGeoScheme->GetNStations(); ++iStation){
    // Cluster size
    c1->cd(iStation+1);
    fClusterSize[iStation]->GetXaxis()->SetTitle("Cluster size");
    fClusterSize[iStation]->Scale(1./fClusterSize[iStation]->Integral());
    fClusterSize[iStation]->Draw();

    // Single muon clusters fraction
    c6->cd(iStation+1);
    fSingleMuCluster[iStation]->GetXaxis()->SetTitle("Single mu clusters fraction");
    fSingleMuCluster[iStation]->Scale(1./fSingleMuCluster[iStation]->Integral());
    fSingleMuCluster[iStation]->Draw();

    // Number of points in different size clusters
    TCanvas *c2 = new TCanvas(Form("Number of points, station %i", iStation+1), Form("Number of points, station %i", iStation+1), 30, 30, 1200, 800);
    c2->Divide(3,2);
    for(Int_t iPad=0; iPad<6; ++iPad){
      c2->cd(iPad+1);
      c2->cd(iPad+1)->SetLogy();
      TH1F* hist = fClusterPoints[iStation][iPad];
      hist->GetXaxis()->SetTitle("Number of points");
      hist->Scale(1./hist->Integral());
      hist->Draw();
    }
    c2->Print(".eps");
    c2->Print(".png");

    // Pad-charge in different size clusters with different number of contributed points
    TCanvas *c3 = new TCanvas(Form("Pad-charge in clusters, station %i", iStation+1), Form("Pad-charge in clusters, station %i", iStation+1), 30, 30, 1200, 800);
    c3->Divide(3,2);
    printf("======================= %s =======================\n",c3->GetName());
    printf("        1 points 2 points 3 points 4 points\n");
    for(Int_t iPad=0; iPad<6; ++iPad){
      printf("%i pad:  ", iPad+1);
      c3->cd(iPad+1);
      c3->cd(iPad+1)->SetLogy();
      Double_t norm = fClusterPoints[iStation][iPad]->Integral();
      for(Int_t iPoint=0; iPoint<4; ++iPoint){
        TH1F* hist = fClusterPadsCharge[iStation][iPad][iPoint];
        hist->SetLineColor(2+iPoint);
        hist->GetXaxis()->SetTitle("ADC channels");
        //hist->Scale(1./norm);
        hist->SetMaximum(1e5);
        hist->SetMinimum(1e-2);
        if(!iPoint) hist->Draw();
        else hist->Draw("same");
        printf("%6.2f   ", hist->GetMean());
      }
      printf("\n");
    }
    printf("\n\n");
    c3->Print(".eps");
    c3->Print(".png");

    // Cluster radii for different size clusters with different number of contributed points
    TCanvas *c4 = new TCanvas(Form("Cluster radii, station %i", iStation+1), Form("Cluster radii, station %i", iStation+1), 30, 30, 1200, 800);
    c4->Divide(3,2);
    for(Int_t iPad=0; iPad<6; ++iPad){
      c4->cd(iPad+1);
      c4->cd(iPad+1)->SetLogy();
      Double_t norm = fClusterPoints[iStation][iPad]->Integral();
      for(Int_t iPoint=0; iPoint<4; ++iPoint){
        TH1F* hist = fClusterRadii[iStation][iPad][iPoint];
        hist->SetLineColor(2+iPoint);
        hist->GetXaxis()->SetTitle("Radius, cm");
        hist->Scale(1./norm);
        if(!iPoint) hist->Draw();
        else hist->Draw("same");
      }
    }
    c4->Print(".eps");
    c4->Print(".png");

    // Mean Euclidean distance between each pair of pads in the cluster of given type (definite number of pads and points)
    TCanvas *c5 = new TCanvas(Form("Mean Euclidean distance, station %i", iStation+1),
        Form("Mean Euclidean distance, station %i", iStation+1), 30, 30, 2000, 400);
    c5->Divide(5,1);
    printf("======================= %s =======================\n",c5->GetName());
    printf("        1 points 2 points 3 points 4 points\n");
    for(Int_t iPad=1; iPad<6; ++iPad){
      printf("%i pad:  ", iPad+1);
      c5->cd(iPad);
      c5->cd(iPad)->SetLogy();
      Double_t norm = fClusterPoints[iStation][iPad]->Integral();
      for(Int_t iPoint=0; iPoint<4; ++iPoint){
        TH1F* hist = fClusterMeanEuclide[iStation][iPad][iPoint];
        hist->SetLineColor(2+iPoint);
        hist->GetXaxis()->SetTitle("Euclidean distance");
        hist->Scale(norm);
        hist->SetMaximum(1e5);
        hist->SetMinimum(1e-1);
        if(!iPoint) hist->Draw();
        else hist->Draw("same");
        printf("%6.2f   ", hist->GetMean());
      }
      printf("\n");
    }
    printf("\n\n");
    c5->Print(".eps");
    c5->Print(".png");
  } // Station loop
  c1->Print(".eps");
  c1->Print(".png");

  c6->Print(".eps");
  c6->Print(".png");
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Double_t CbmMuchClusterAnalysis::GetMeanDistance(CbmMuchCluster* cluster){
  Double_t distance = 0;
  for(Int_t i=0; i<cluster->GetNDigis(); ++i){
    Int_t iDigi = cluster->GetDigiIndex(i);
    for(Int_t j=0; j<cluster->GetNDigis(); ++j){
      Int_t jDigi = cluster->GetDigiIndex(j);
      distance += GetDistance(iDigi, jDigi);
    }
  }
  return distance/cluster->GetNDigis();
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Double_t CbmMuchClusterAnalysis::GetDistance(Int_t iDigi, Int_t jDigi){
  CbmMuchDigi* digi1 = (CbmMuchDigi*) fDigis->At(iDigi);
  CbmMuchDigi* digi2 = (CbmMuchDigi*) fDigis->At(jDigi);
  UInt_t charge1 = digi1->GetADCCharge();
  UInt_t charge2 = digi2->GetADCCharge();
  Int_t deltaCharge = charge1 - charge2;
  CbmMuchPad* pad1 = GetPadByDigi(iDigi);
  CbmMuchPad* pad2 = GetPadByDigi(jDigi);
  // Normalized space distances (because of different pad sizes)
  Double_t deltaX = 2*(pad1->GetX0() - pad2->GetX0())/(pad1->GetLx() + pad2->GetLx());
  Double_t deltaY = 2*(pad1->GetY0() - pad2->GetY0())/(pad1->GetLy() + pad2->GetLy());
  return TMath::Sqrt(deltaX*deltaX + deltaY*deltaY + deltaCharge*deltaCharge);
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
CbmMuchPad* CbmMuchClusterAnalysis::GetPadByDigi(Int_t iDigi){
  CbmMuchDigi* digi = (CbmMuchDigi*) fDigis->At(iDigi);
  CbmMuchModuleGem* module = (CbmMuchModuleGem*)fGeoScheme->GetModuleByDetId(digi->GetDetectorId());
  return module->GetPad(digi->GetChannelId());
}
// -------------------------------------------------------------------------

ClassImp(CbmMuchClusterAnalysis)
