/*
 * CbmMuchFindHitsGem.cxx
 */
#include "CbmMuchFindHitsGem.h"
#include "CbmMuchGeoScheme.h"
#include "CbmMuchModule.h"
#include "CbmMuchModuleGem.h"
#include "CbmMuchPad.h"
#include "CbmMuchPixelHit.h"
#include "CbmMuchCluster.h"
#include "FairRootManager.h"
#include "TMath.h"
#include "TFile.h"
#include "TStopwatch.h"
#include "TClonesArray.h"
#include "CbmTimeSlice.h"
#include "CbmMuchDigi.h"
#include "CbmMuchAddress.h"
#include <algorithm>
#include <iostream>

// -------------------------------------------------------------------------
CbmMuchFindHitsGem::CbmMuchFindHitsGem(const char* digiFileName) 
  : FairTask("MuchFindHitsGem", 1) ,
    fDigiFile(digiFileName),
    fAlgorithm(3),
    fClusterSeparationTime(100.),
    fThresholdRatio(0.1),
    fEvent(0),
    fDigis(NULL),
    fClusterCharges(),
    fLocalMax(),
    fClusterPads(),
    fNeighbours(),
    fClusters(new TClonesArray("CbmMuchCluster", 1000)),
    fHits(new TClonesArray("CbmMuchPixelHit", 1000)),
    fGeoScheme(CbmMuchGeoScheme::Instance()),
    fDigiIndices(),
    fFiredPads(),
    fDaq(),
    fTimeSlice(NULL),
    fDigiData()
{
}

// -----   Private method Init   -------------------------------------------
InitStatus CbmMuchFindHitsGem::Init() {
  FairRootManager* ioman = FairRootManager::Instance();
  if (fDaq) fTimeSlice = (CbmTimeSlice*) ioman->GetObject("TimeSlice.");
  else      fDigis     = (TClonesArray*) ioman->GetObject("MuchDigi");
  
  ioman->Register("MuchCluster", "Cluster in MUCH", fClusters, kTRUE);
  ioman->Register("MuchPixelHit", "Hit in MUCH", fHits, kTRUE);
  
  // Initialize GeoScheme
  TFile* oldfile = gFile;
  TFile* file = new TFile(fDigiFile);
  TObjArray* stations = (TObjArray*) file->Get("stations");
  file->Close();
  file->Delete();
  gFile = oldfile;
  fGeoScheme->Init(stations);
  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void CbmMuchFindHitsGem::Exec(Option_t* opt) {
  TStopwatch timer;
  timer.Start();
  fEvent++;
  fDigiData.clear();
  
  if (fDaq) fDigiData = fTimeSlice->GetMuchData();
  else {
    for (Int_t iDigi = 0; iDigi < fDigis->GetEntriesFast(); iDigi++) {
      CbmMuchDigi* digi = (CbmMuchDigi*) fDigis->At(iDigi);
      CbmMuchModule* module = fGeoScheme->GetModuleByDetId(digi->GetAddress()); //AZ
      //std::cout << module->GetDetectorType() << std::endl; //AZ
      if (module->GetDetectorType() == 2) continue; //AZ - skip 2-D straws
      fDigiData.push_back(*digi);
    }
  }
  
  
  // Clear output array
  if (fHits) fHits->Clear();
  if (fClusters) fClusters->Delete();//Clear(); // Delete because of memory escape

  // Find clusters
  FindClusters();

  for (Int_t iCluster = 0; iCluster < fClusters->GetEntriesFast(); ++iCluster) {
    CbmMuchCluster* cluster = (CbmMuchCluster*) fClusters->At(iCluster);
    switch (fAlgorithm) {
    // Hit
    case 0: 
      // One hit per pad
    case 1: {
      // One hit per cluster
      CreateHits(cluster, iCluster);
      break;
    }
    case 2: {
      // Simple cluster deconvolution
      ExecClusteringSimple(cluster,iCluster);
      break;
    }
    case 3: {
      ExecClusteringPeaks(cluster,iCluster);
      break;
    }
    default: {
      Fatal("CbmMuchFindHitsGem::Exec:","The algorithm index does not exist.");
      break;
    }
    }
  }
  fDigiIndices.clear();
  fFiredPads.clear();

  timer.Stop();
  gLogger->Info(MESSAGE_ORIGIN,"MuchFindHitsGem: time %f s, digis %lu,  clusters %i, hits %i",
      timer.RealTime(),fDigiData.size(),fClusters->GetEntriesFast(),fHits->GetEntriesFast());
}
// -------------------------------------------------------------------------


// -----   Private method FindClusters  ------------------------------------
void CbmMuchFindHitsGem::FindClusters() {
  Int_t nClusters = 0;
  
  if (fAlgorithm==0){
    for (Int_t iDigi = 0; iDigi < fDigiData.size(); iDigi++) {
      fDigiIndices.clear();
      fDigiIndices.push_back(iDigi);
      const CbmDigi* digi = static_cast<const CbmDigi*>(fDigis->At(iDigi));
      CbmMuchCluster* cluster = new ((*fClusters)[nClusters++]) CbmMuchCluster();
      Int_t address = CbmMuchAddress::GetAddress(CbmMuchAddress::GetStationIndex(digi->GetAddress()),
            CbmMuchAddress::GetLayerIndex(digi->GetAddress()), CbmMuchAddress::GetLayerSideIndex(digi->GetAddress()),
            CbmMuchAddress::GetModuleIndex(digi->GetAddress()));
      cluster->SetAddress(address);
      cluster->AddDigis(fDigiIndices);
    }
    return;
  }
  
  vector<CbmMuchModuleGem*> modules = fGeoScheme->GetGemModules();

  // Clear array of digis in the modules
  for (UInt_t m=0;m<modules.size();m++) modules[m]->ClearDigis();

  // Fill array of digis in the modules. Digis are automatically sorted in time
  for (Int_t iDigi = 0; iDigi < fDigiData.size(); iDigi++) {
    CbmMuchDigi* digi =&(fDigiData[iDigi]);
    Double_t time = digi->GetTime();
    UInt_t address = digi->GetAddress();
    fGeoScheme->GetModuleByDetId(address)->AddDigi(time,iDigi);
  }
  
  // Find clusters module-by-module
  for (UInt_t m=0;m<modules.size();m++){
    CbmMuchModuleGem* module = modules[m];
    multimap<Double_t,Int_t> digis = modules[m]->GetDigis();
    multimap<Double_t,Int_t>::iterator it,itmin,itmax;

    // Split module digis into time slices according to fClusterSeparationTime
    vector<multimap<Double_t,Int_t>::iterator> slices;
    Double_t tlast = -10000;
//    slices.push_back(digis.begin());
    for (it=digis.begin();it!=digis.end();++it){
      Double_t t  = it->first;
      if (t> tlast + fClusterSeparationTime) slices.push_back(it);
      tlast = t;
    }
    slices.push_back(it);
    for (Int_t s=1;s<slices.size();s++){
      fFiredPads.clear();
      for (it=slices[s-1];it!=slices[s];it++){
        Int_t iDigi = it->second;
        CbmMuchDigi* digi = &(fDigiData[iDigi]);
        CbmMuchPad* pad = module->GetPad(digi->GetAddress());
        pad->SetDigiIndex(iDigi);
        fFiredPads.push_back(pad);
      }
      // Loop over fired pads in a time slice
      for (Int_t p=0;p<fFiredPads.size();p++){
        fDigiIndices.clear();
        CreateCluster(fFiredPads[p]);
        if (fDigiIndices.size()==0) continue;
        const CbmDigi* digi = static_cast<const CbmDigi*>(fDigis->At(fDigiIndices.front()));
        CbmMuchCluster* cluster = new ((*fClusters)[nClusters++]) CbmMuchCluster();
        Int_t address = CbmMuchAddress::GetAddress(CbmMuchAddress::GetStationIndex(digi->GetAddress()),
              CbmMuchAddress::GetLayerIndex(digi->GetAddress()), CbmMuchAddress::GetLayerSideIndex(digi->GetAddress()),
              CbmMuchAddress::GetModuleIndex(digi->GetAddress()));
        cluster->SetAddress(address);
        cluster->AddDigis(fDigiIndices);
      }
      //return;
    }
  }
}
// -------------------------------------------------------------------------


// -----   Private method CreateCluster  -----------------------------------
void CbmMuchFindHitsGem::CreateCluster(CbmMuchPad* pad) {
  Int_t digiIndex = pad->GetDigiIndex();
  if (digiIndex<0) return;
  fDigiIndices.push_back(digiIndex);
  pad->SetDigiIndex(-1);
  vector<CbmMuchPad*> neighbours = pad->GetNeighbours();
  for (Int_t i=0;i<neighbours.size();i++) CreateCluster(neighbours[i]);
}
// -------------------------------------------------------------------------


// -----   Private method ExecClusteringSimple  ----------------------------
void CbmMuchFindHitsGem::ExecClusteringSimple(CbmMuchCluster* cluster,Int_t iCluster) {
  CbmMuchDigi* digi = &(fDigiData[cluster->GetDigi(0)]);
  CbmMuchModule* m = fGeoScheme->GetModuleByDetId(digi->GetAddress());
  CbmMuchModuleGem* module = (CbmMuchModuleGem*) m;
  Int_t iStation = CbmMuchAddress::GetStationIndex(digi->GetAddress());

  Int_t maxCharge = 0;
  for (Int_t iDigi = 0; iDigi < cluster->GetNofDigis(); iDigi++) {
    Int_t digiIndex = cluster->GetDigi(iDigi);
    digi = &(fDigiData[digiIndex]);
    Int_t charge = digi->GetAdc();
    if (charge>maxCharge) maxCharge = charge;
  }
 
  UInt_t threshold = UInt_t(fThresholdRatio*maxCharge);
  
  // Fire pads which are higher than threshold in a cluster
  fFiredPads.clear();
  for (Int_t iDigi = 0; iDigi < cluster->GetNofDigis(); iDigi++) {
    Int_t digiIndex = cluster->GetDigi(iDigi);
    digi = &(fDigiData[digiIndex]);
    if (digi->GetAdc()<=threshold) continue;
    CbmMuchPad* pad = module->GetPad(digi->GetAddress());
    pad->SetDigiIndex(digiIndex);
    fFiredPads.push_back(pad);
  }
  for (Int_t p=0;p<fFiredPads.size();p++){
    fDigiIndices.clear();
    CreateCluster(fFiredPads[p]);
    if (fDigiIndices.size()==0) continue;
    CbmMuchCluster cl;
    cl.AddDigis(fDigiIndices);
    CreateHits(&cl,iCluster);
  }
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchFindHitsGem::ExecClusteringPeaks(CbmMuchCluster* cluster,Int_t iCluster) {
  Int_t nDigis = cluster->GetNofDigis();
  if (nDigis<=2)  { CreateHits(cluster,iCluster); return; }
  fClusterCharges.clear();
  fClusterPads.clear();
  fLocalMax.clear();
//  for (Int_t i=0;i<fNeighbours.size();i++) fNeighbours[i].clear();
  fNeighbours.clear();
  
  // Fill cluster map
  for (Int_t i=0;i<nDigis;i++){
    Int_t iDigi = cluster->GetDigi(i);
    CbmMuchDigi* digi = &(fDigiData[iDigi]);
    UInt_t address = digi->GetAddress();
    CbmMuchModuleGem* module = (CbmMuchModuleGem*) fGeoScheme->GetModuleByDetId(address);
    CbmMuchPad* pad = module->GetPad(address);
    Int_t adc = digi->GetAdc();
    fClusterPads.push_back(pad);
    fClusterCharges.push_back(adc);
    fLocalMax.push_back(1);
  }
  
  // Fill neighbours
  for (Int_t i=0;i<nDigis;i++){
    CbmMuchPad* pad = fClusterPads[i];
    vector<CbmMuchPad*> neighbours = pad->GetNeighbours();
    vector<Int_t> selected_neighbours;
    for (Int_t ip=0;ip<neighbours.size();ip++){
      CbmMuchPad* p = neighbours[ip];
      vector<CbmMuchPad*>:: iterator it = find(fClusterPads.begin(),fClusterPads.end(),p);
      if (it==fClusterPads.end()) continue;
      selected_neighbours.push_back(it-fClusterPads.begin());
    }
    fNeighbours.push_back(selected_neighbours);
  }

  // Flag local maxima
  for (Int_t i=0; i<nDigis;i++) {
    Int_t c = fClusterCharges[i];
    for (Int_t n=0;n<fNeighbours[i].size();n++) {
      Int_t in = fNeighbours[i][n];
      Int_t cn = fClusterCharges[in];
      if (cn < c) fLocalMax[in] = 0;
    }
  }

  // Fire pads corresponding to local maxima
  fFiredPads.clear();
  for (Int_t i=0; i<nDigis;i++) {
    if (fLocalMax[i]==0) continue;
    CbmMuchPad* pad = fClusterPads[i];
    pad->SetDigiIndex(cluster->GetDigi(i));
    fFiredPads.push_back(pad);
  }
  
  // Create clusters
  for (Int_t p=0;p<fFiredPads.size();p++){
    fDigiIndices.clear();
    CreateCluster(fFiredPads[p]);
    if (fDigiIndices.size()==0) continue;
    CbmMuchCluster cl;
    cl.AddDigis(fDigiIndices);
    CreateHits(&cl,iCluster);
  }
}
// -------------------------------------------------------------------------


// -----   Private method CreateHits  --------------------------------------
void CbmMuchFindHitsGem::CreateHits(CbmMuchCluster* cluster, Int_t iCluster) {
  Int_t nDigis = cluster->GetNofDigis();
  Double_t sumq=0, sumx=0, sumy=0, sumt=0, sumdx2=0, sumdy2=0, sumdxy2=0, sumdt2=0;
  Double_t q=0,x=0,y=0,t=0,z=0,dx=0,dy=0,dxy=0,dt=0;
  Int_t address = 0;
  Int_t planeId = 0;
  CbmMuchModuleGem* module = NULL;
  
  Double_t tmin = -1;
  for (Int_t i=0;i<nDigis;i++) {
    Int_t iDigi = cluster->GetDigi(i);
    CbmMuchDigi* digi = &(fDigiData[iDigi]);
    if (i==0) {
      address = CbmMuchAddress::GetElementAddress(digi->GetAddress(),kMuchModule);
      planeId = fGeoScheme->GetLayerSideNr(address);
      module  = (CbmMuchModuleGem*) fGeoScheme->GetModuleByDetId(address);
      z       = module->GetPosition()[2];
    }
    CbmMuchPad* pad = module->GetPad(digi->GetAddress());
    x   = pad->GetX();
    y   = pad->GetY();
    t   = digi->GetTime();
    if (tmin<0) tmin = t;
    if (tmin<t) tmin = t;
    q   = digi->GetAdc();
    dx  = pad->GetDx();
    dy  = pad->GetDy();
    dxy = pad->GetDxy();
    dt  = 4.; // digi->GetDTime(); //TODO introduce time uncertainty determination
    sumq    += q;
    sumx    += q*x;
    sumy    += q*y;
    sumt    += q*t;
    sumdx2  += q*q*dx*dx;
    sumdy2  += q*q*dy*dy;
    sumdxy2 += q*q*dxy*dxy;
    sumdt2  += q*q*dt*dt;
  }
  
  x   = sumx/sumq;
  y   = sumy/sumq;
//  t   = sumt/sumq;
  t   = tmin;
  dx  = sqrt(sumdx2/12)/sumq;
  dy  = sqrt(sumdy2/12)/sumq;
  dxy = sqrt(sumdxy2/12)/sumq;
  dt = sqrt(sumdt2)/sumq;
  Int_t iHit = fHits->GetEntriesFast();
  new ((*fHits)[iHit]) CbmMuchPixelHit(address,x,y,z,dx,dy,0,dxy,iCluster,planeId,t,dt);
}
// -------------------------------------------------------------------------

ClassImp(CbmMuchFindHitsGem)


