// -------------------------------------------------------------------------
// -----                   CbmVisMuchCluster source file               -----
// -----                  Created 15/11/07  by E. Kryshen              -----
// -------------------------------------------------------------------------
#include "CbmMuchCluster.h"
#include "CbmVisMuchCluster.h"
#include "CbmMuchPad.h"
#include "CbmMuchDigi.h"
#include "CbmMuchDigiMatch.h"
#include "TArrayI.h"
#include "FairRootManager.h"
#include "CbmMuchStation.h"
#include "TPolyLine.h"
#include "TMarker.h"
#include "TCanvas.h"
#include "TH2I.h"
#include "TMath.h"
#include "CbmVisPoint.h"
#include "TClonesArray.h"
#include "CbmMuchGeoScheme.h"
#include "CbmMuchModuleGem.h"

// -------------------------------------------------------------------------
CbmVisMuchCluster::CbmVisMuchCluster(Int_t id){
  FairRootManager* fRootManager = FairRootManager::Instance();
  fDigis       = (TClonesArray*) fRootManager->GetObject("MuchDigi");
  fDigiMatches = (TClonesArray*) fRootManager->GetObject("MuchDigiMatch");
  fClusters    = (TClonesArray*) fRootManager->GetObject("MuchCluster");
  fClusterId = id;
  fCluster = (CbmMuchCluster*) fClusters->At(id);
  fStationNr = 0;
  fPads.clear();
  CreateHisto();
  //PrintInfo();
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
CbmVisMuchCluster::~CbmVisMuchCluster(){
  fX.clear();
  fY.clear();
  fC.clear();
  if (fhChargeXY) delete fhChargeXY;
  if (fCanvas)    delete fCanvas;

}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmVisMuchCluster::PrintInfo(){
  printf("Cluster info: id=%i  ",fClusterId);
  printf("pads: %i",fPads.size());
  printf("pad cluster ids:");
  for (Int_t i=0;i<fPads.size();i++){
    Int_t digiId = fCluster->GetDigiIndex(i);
    CbmMuchDigiMatch* match = (CbmMuchDigiMatch*) fDigiMatches->At(digiId);
//    printf(" %i",match->GetClusterIndex());
  }
  printf("\n");
}
// -------------------------------------------------------------------------


void CbmVisMuchCluster::CreateHisto(){
//  printf("CbmVisMuchCluster::CreateHisto()\n");
  TArrayI indices = fCluster->GetDigiIndices();

  Int_t size = indices.GetSize();
  //Define minimum pad dimensions in the cluster
  Double_t bin_size_x = 0;
  Double_t bin_size_y = 0;

  for (Int_t i=0; i<size; i++){
    Int_t id = indices[i];
    CbmMuchDigi* digi  = (CbmMuchDigi*) fDigis->At(id);
    CbmMuchDigiMatch* match = (CbmMuchDigiMatch*) fDigiMatches->At(id);

    CbmMuchModule* module = (CbmMuchModule*) CbmMuchGeoScheme::Instance()->GetModuleByDetId(digi->GetDetectorId());
    if (module->GetDetectorType()!=1) Fatal("CreateHisto","Wrong digi type in a cluster array");
    CbmMuchModuleGem* gem_module = (CbmMuchModuleGem*) module;
    CbmMuchPad* pad = gem_module->GetPad(digi->GetChannelId());

    if (i==0){
      bin_size_x = pad->GetLx();
      bin_size_y = pad->GetLy();
      continue;
    }
    if (pad->GetLx()<bin_size_x) bin_size_x = pad->GetLx();
    if (pad->GetLy()<bin_size_y) bin_size_y = pad->GetLy();
  }

  fX.clear();
  fY.clear();
  fC.clear();

  for (Int_t iIndex=0; iIndex<size; iIndex++){
    Int_t id = indices[iIndex];
    CbmMuchDigi* digi  = (CbmMuchDigi*) fDigis->At(id);
    CbmMuchDigiMatch* match = (CbmMuchDigiMatch*) fDigiMatches->At(id);
    CbmMuchModule* module = (CbmMuchModule*) CbmMuchGeoScheme::Instance()->GetModuleByDetId(digi->GetDetectorId());
    if (module->GetDetectorType()!=1) Fatal("CreateHisto","Wrong digi type in a cluster array");
    CbmMuchModuleGem* gem_module = (CbmMuchModuleGem*) module;
    CbmMuchPad* pad = gem_module->GetPad(digi->GetChannelId());
    fPads.push_back(pad);
    Double_t x0  = pad->GetX0();
    Double_t y0  = pad->GetY0();
    Double_t c = 1;
    Double_t s = 0;
    Double_t x = x0*c-y0*s;
    Double_t y = x0*s+y0*c;
    Double_t dx = pad->GetLx();
    Double_t dy = pad->GetLy();
    Double_t xmin = x-dx/2+bin_size_x/2;
    Double_t ymin = y-dy/2+bin_size_y/2;
    Int_t nx = TMath::Nint(dx/bin_size_x);
    Int_t ny = TMath::Nint(dy/bin_size_y);
    // TODO Set ADC charge
    Double_t charge = 0; //((Double_t)digi->GetCharge())/((Double_t)nx*ny);
    for (Int_t i=1;i<=nx;i++){
      for (Int_t j=1;j<=ny;j++){
         fX.push_back(xmin+bin_size_x*i);
         fY.push_back(ymin+bin_size_y*j);
         fC.push_back(charge);
      }
    }
  }
  Double_t xmax = fX[0];
  Double_t xmin = fX[0];
  Double_t ymax = fY[0];
  Double_t ymin = fY[0];
  for(Int_t i=1;i<fX.size();i++){
    if (fX[i]>xmax) xmax = fX[i];
    if (fX[i]<xmin) xmin = fX[i];
    if (fY[i]>ymax) ymax = fY[i];
    if (fY[i]<ymin) ymin = fY[i];
  }
  xmax+=1.5*bin_size_x;
  xmin-=1.5*bin_size_x;
  ymax+=1.5*bin_size_y;
  ymin-=1.5*bin_size_y;

  Int_t nbins_x = TMath::Nint((xmax-xmin)/bin_size_x);
  Int_t nbins_y = TMath::Nint((ymax-ymin)/bin_size_y);

  Double_t dx = (xmax-xmin)/2;
  Double_t dy = (ymax-ymin)/2;
  Double_t xc = (xmax+xmin)/2;
  Double_t yc = (ymax+ymin)/2;

//  if (dx>dy) { ymin = yc - dx; ymax = yc + dx; }
//  else       { xmin = xc - dy; xmax = xc + dy; }

  fhChargeXY = new TH2I(Form("hChargeXY%i",fClusterId),"",nbins_x,xmin,xmax,nbins_y,ymin,ymax);
  for(Int_t i=0;i<fX.size();i++){
    Int_t binx = fhChargeXY->GetXaxis()->FindBin(fX[i]);
    Int_t biny = fhChargeXY->GetYaxis()->FindBin(fY[i]);
    fhChargeXY->SetBinContent(binx,biny,(Double_t) fC[i]);
  }
}

void CbmVisMuchCluster::DrawHisto(){
  fCanvas = new TCanvas(Form("chargeXY%i",fClusterId),Form("Cluster %i Station %i",fClusterId,fStationNr),600,600);
  fCanvas->Divide(2,2);
  fCanvas->cd(1);
  fhChargeXY->DrawCopy("colz");
/*
  Double_t xmin = fhChargeXY->GetXaxis()->GetXmin();
  Double_t xmax = fhChargeXY->GetXaxis()->GetXmax();
  Double_t ymin = fhChargeXY->GetYaxis()->GetXmin();
  Double_t ymax = fhChargeXY->GetYaxis()->GetXmax();
  printf("GetRangeAxis:%f,%f,%f,%f\n",xmin,ymin,xmax,ymax);

  gPad->SetLeftMargin(0);
  gPad->SetRightMargin(0);
  gPad->SetTopMargin(0);
  gPad->SetBottomMargin(0);
  gPad->Range(xmin,ymin,xmax,ymax);


  printf("Station:%i\n",fStationNr);
  printf("Npoints:%i\n",fPoints[fStationNr]->GetEntriesFast());

  for (Int_t i=0;i<fPoints[fStationNr]->GetEntriesFast();i++){
    CbmVisPoint* point = (CbmVisPoint*) fPoints[fStationNr]->At(i);
    Double_t x = point->GetMarkerIn()->GetX();
    Double_t y = point->GetMarkerIn()->GetY();
//    printf("Draw point: %f %f\n",x,y);
//    if (x<xmin || x>xmax || y<ymin || y>ymax) continue;
//    printf("Draw point: %f %f\n",x,y);
    point->DrawSpread();
  }
*/
  fCanvas->cd(2);
  TH1D* hProjectionY = fhChargeXY->ProjectionY();
  hProjectionY->SetFillColor(kBlue);
  hProjectionY->Draw("hbar");
  fCanvas->cd(3);
  TH1D* hProjectionX = fhChargeXY->ProjectionX();
  hProjectionX->SetFillColor(kBlue);
  hProjectionX->Draw("bar");
  fCanvas->cd(4);
  fhChargeXY->DrawCopy("lego2");
  fCanvas->cd();
}

ClassImp(CbmVisMuchCluster)
