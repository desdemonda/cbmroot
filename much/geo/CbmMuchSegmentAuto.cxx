/** CbmMuchSegmentAuto.cxx
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 20.06.07
 *@version 1.0
 **
 ** class for making parameter file for MUCH digitizer
 **
 **/

#include "CbmGeoMuchPar.h"
#include "CbmMuchSegmentAuto.h"

#include "FairRuntimeDb.h"
#include "FairRootManager.h"
#include "CbmMuchPoint.h"
#include "CbmMuchLayer.h"
#include "CbmMuchModuleGem.h"
#include "CbmMuchSectorRectangular.h"
#include "CbmMuchStation.h"
#include "CbmMuchGeoScheme.h"
#include "CbmMuchAddress.h"

#include "TMath.h"
#include "TClonesArray.h"
#include "TCanvas.h"
#include "TArc.h"
#include "TColor.h"
#include "TStyle.h"
#include "TFile.h"
#include "TH1.h"
#include "TF1.h"
#include "TSystem.h"

#include <cassert>
#include <stdio.h>
#include <string>

using std::string;

// -----   Default constructor   -------------------------------------------
CbmMuchSegmentAuto::CbmMuchSegmentAuto()
  : FairTask(),
    fEvents(0),
    fPoints(NULL),
    fHistHitDensity(),
    fNStations(0),
    fStations(NULL),
    fDigiFileName(),
    fGeoPar(NULL),
    fExp0(),
    fExp1(),
    fSigmaXmin(),
    fSigmaYmin(),
    fSigmaXmax(),
    fSigmaYmax(),
    fOccupancyMax()
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchSegmentAuto::CbmMuchSegmentAuto(char* digiFileName)
  : FairTask(),
    fEvents(0),
    fPoints(NULL),
    fHistHitDensity(),
    fNStations(0),
    fStations(NULL),
    fDigiFileName(digiFileName),
    fGeoPar(NULL),
    fExp0(),
    fExp1(),
    fSigmaXmin(),
    fSigmaYmin(),
    fSigmaXmax(),
    fSigmaYmax(),
    fOccupancyMax()
{
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMuchSegmentAuto::~CbmMuchSegmentAuto() {
}
// -------------------------------------------------------------------------

void CbmMuchSegmentAuto::SetNStations(Int_t nStations){
  fNStations = nStations;
  fSigmaXmin.resize(fNStations);
  fSigmaYmin.resize(fNStations);
  fSigmaXmax.resize(fNStations);
  fSigmaYmax.resize(fNStations);
  fOccupancyMax.resize(fNStations);
}

void CbmMuchSegmentAuto::SetSigmaMin(Double_t* sigmaXmin, Double_t* sigmaYmin){
  for(Int_t iStation = 0; iStation < fNStations; ++iStation){
    fSigmaXmin.at(iStation) = sigmaXmin[iStation];
    fSigmaYmin.at(iStation) = sigmaYmin[iStation];
  }
}
void CbmMuchSegmentAuto::SetSigmaMax(Double_t* sigmaXmax, Double_t* sigmaYmax){
  for(Int_t iStation = 0; iStation < fNStations; ++iStation){
    fSigmaXmax.at(iStation) = sigmaXmax[iStation];
    fSigmaYmax.at(iStation) = sigmaYmax[iStation];
  }
}

void CbmMuchSegmentAuto::SetOccupancyMax(Double_t* occupancyMax){
  for(Int_t iStation = 0; iStation < fNStations; ++iStation){
    fOccupancyMax.at(iStation) = occupancyMax[iStation];
  }
}

// -----   Private method SetParContainers  --------------------------------
void CbmMuchSegmentAuto::SetParContainers() {
  // Get runtime database
  FairRuntimeDb* db = FairRuntimeDb::instance();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");
  // Get MUCH geometry param
  fGeoPar = (CbmGeoMuchPar*) db->getContainer("CbmGeoMuchPar");
}
// -------------------------------------------------------------------------


// -----   Private method Init  --------------------------------------------
InitStatus CbmMuchSegmentAuto::Init(){
  FairRootManager* fManager = FairRootManager::Instance();
  fPoints = (TClonesArray *) fManager->GetObject("MuchPoint");
  fEvents = 0;

  fStations = fGeoPar->GetStations();
  if(!fStations) Fatal("Init", "No input array of MUCH stations.");
  if(fNStations != fStations->GetEntries()) Fatal("Init", "Incorrect number of stations set.");
  //fNStations = fStations->GetEntries();
  fHistHitDensity = new TH1D*[fNStations];

  for (Int_t i=0;i<fNStations;i++) {
    CbmMuchStation* station = (CbmMuchStation*) fStations->At(i);
    fHistHitDensity[i] = new TH1D(Form("hStation%i",i+1),Form("Station %i",i+1), 110, 0, 220);
    fHistHitDensity[i]->GetXaxis()->SetTitle("r, cm");
    fHistHitDensity[i]->GetYaxis()->SetTitle("hits/(event#timescm^{2})");
    fHistHitDensity[i]->SetLineColor(4);
    fHistHitDensity[i]->SetLineWidth(2);
    fHistHitDensity[i]->GetYaxis()->SetTitleOffset(1.27);
  }
  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMuchSegmentAuto::Exec(Option_t * option){
  fEvents++;
  printf("Event: %i\n",fEvents);

  gStyle->SetOptStat(0);
  for(Int_t iPoint = 0; iPoint < fPoints->GetEntriesFast(); iPoint++){
    CbmMuchPoint* point = (CbmMuchPoint*)fPoints->At(iPoint);
    if(!point) continue;
    Int_t iStation = CbmMuchAddress::GetStationIndex(point->GetDetectorID());
    //        CbmMuchStation* station = (CbmMuchStation*) fStations->At(iStation);
    //        if (station->GetDetectorType()==2) continue;

    Int_t iLayer = CbmMuchAddress::GetLayerIndex(point->GetDetectorID());
    //    printf("iStation = %i\n", iStation);
    //    printf("detId = %qd\n", point->GetDetectorID());
    //    printf("iPoint = %i\n", iPoint);
    assert(iStation >= 0 && iStation < fNStations);
    if(iLayer) continue;
    TVector3 pos;
    point->Position(pos);
    ((TH1D*) fHistHitDensity[iStation])->Fill(pos.Pt());
  }
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMuchSegmentAuto::FinishTask(){
  // Create normalization histo
  TH1D* hNorm = new TH1D("hNorm","",110,0,220);
  Double_t binSize = 2.;
  for(Int_t l=0; l<100; l++){
    Double_t R1 = l*binSize;
    Double_t R2 = (l+1)*binSize;
    Double_t s = TMath::Pi()*(R2*R2-R1*R1);
    hNorm->SetBinContent(l+1,s*fEvents);
  }

  for (Int_t i=0;i<fNStations;i++) {
    CbmMuchStation* station = (CbmMuchStation*)fStations->At(i);
    TH1D* h =fHistHitDensity[i];
    h->Divide(hNorm);
    TCanvas* c1 = new TCanvas(Form("cStation%i",i+1),Form("Station %i",i+1),10,10,500,500);
    c1->SetLogy();
    c1->SetLeftMargin(0.12);
    c1->SetRightMargin(0.08);
    TF1* f1 = new TF1("f1", "exp([0] + [1]*x)");
    f1->SetParameter(0, 0.5);
    f1->SetParameter(1, -0.1);
    h->Fit("f1","QLL","",station->GetRmin(),station->GetRmax());
    Double_t exp0 = h->GetFunction("f1")->GetParameter(0);
    Double_t exp1 = h->GetFunction("f1")->GetParameter(1);
    fExp0.push_back(exp0);
    fExp1.push_back(exp1);

    h->Draw();
    c1->Print(Form("%s/hd_station%i.eps",gSystem->DirName(fDigiFileName), i+1));
    c1->Print(Form("%s/hd_station%i.png",gSystem->DirName(fDigiFileName), i+1));
  }

  for (Int_t i=0;i<fNStations;i++) {
    CbmMuchStation* station = (CbmMuchStation*)fStations->At(i);
    //if (station->GetDetectorType()==2) continue;

    Int_t nLayers = station->GetNLayers();
    for(Int_t iLayer = 0; iLayer < nLayers; iLayer++){
      CbmMuchLayer* layer = station->GetLayer(iLayer);
      if(!layer) Fatal("Init", "Incomplete layers array.");
      // Get layer sides
      InitLayerSide(layer->GetSideF());
      InitLayerSide(layer->GetSideB());
    }
    printf("Station%i segmented\n",i+1);
  }

  // Save parameters
  TFile* oldfile = gFile;
  TFile* f = new TFile(fDigiFileName, "RECREATE");
  fStations->Write("stations",1);

  f->Close();
  gFile=oldfile;

  DrawSegmentation();

  Print();
}
// -------------------------------------------------------------------------


// -----   Private method InitLayerSide  -----------------------------------
void CbmMuchSegmentAuto::InitLayerSide(CbmMuchLayerSide* layerSide){
  if(!layerSide) Fatal("Init", "Incomplete layer sides array.");
  Int_t nModules = layerSide->GetNModules();
  for(Int_t iModule = 0; iModule < nModules; iModule++){
    CbmMuchModule* mod = layerSide->GetModule(iModule);
    if(mod->GetDetectorType()!=1) continue;
    CbmMuchModuleGem* module = (CbmMuchModuleGem*)mod;
    SegmentModule(module);
  }
}
// -------------------------------------------------------------------------


// -----   Private method SegmentModule  -----------------------------------
void CbmMuchSegmentAuto::SegmentModule(CbmMuchModuleGem* module){
  TVector3 modSize = module->GetSize();
  Double_t modLx = modSize.X();
  Double_t modLy = modSize.Y();
  Double_t modLz = modSize.Z();
  TVector3 modPosition = module->GetPosition();
  Double_t modX = modPosition.X();
  Double_t modY = modPosition.Y();
  Double_t modZ = modPosition.Z();

  Bool_t result = modLx > modLy;
  Int_t iRatio = (result) ? (Int_t)((modLx+1e-3)/modLy)
      : (Int_t)((modLy+1e-3)/modLx);
  Int_t detectorId = module->GetDetectorId();
  Double_t secLx = (result) ? modLx/iRatio : modLx;
  Double_t secLy = (result) ? modLy : modLy/iRatio;
  for(Int_t i = 0; i< iRatio; i++){
    Double_t secX = (result) ? modX - modLx/2. + (i+0.5)*secLx : modX;
    Double_t secY = (result) ? modY : modY - modLy/2. + (i+0.5)*secLy;
    Int_t iSector = module->GetNSectors();

    TVector3 sectorPosition(secX, secY, modZ);
    TVector3 sectorSize(secLx, secLy, modLz);
    CbmMuchSectorRectangular* sector = new CbmMuchSectorRectangular(detectorId, iSector, sectorPosition, sectorSize, 8, 16);
    SegmentSector(module,sector);
  }
}
// -------------------------------------------------------------------------

// -----  Private method SegmentSector  ------------------------------------
void CbmMuchSegmentAuto::SegmentSector(CbmMuchModuleGem* module, CbmMuchSectorRectangular* sector){
  TVector3 secSize = sector->GetSize();
  TVector3 secPosition = sector->GetPosition();
  Int_t detectorId = module->GetDetectorId();
  Int_t iStation = CbmMuchAddress::GetStationIndex(detectorId);
  Int_t iLayer = CbmMuchAddress::GetLayerIndex(detectorId);
  Int_t iSide = CbmMuchAddress::GetLayerSideIndex(detectorId);
  Int_t iModule = CbmMuchAddress::GetModuleIndex(detectorId);
  Double_t secLx = secSize.X();
  Double_t secLy = secSize.Y();
  Double_t secLz = secSize.Z();
  Double_t secX  = secPosition.X();
  Double_t secY  = secPosition.Y();
  Double_t secZ  = secPosition.Z();

  assert(CbmMuchAddress::GetStationIndex(sector->GetAddress()) == iStation);
  assert(CbmMuchAddress::GetLayerIndex(sector->GetAddress()) == iLayer);
  assert(CbmMuchAddress::GetLayerSideIndex(sector->GetAddress()) == iSide);
  assert(CbmMuchAddress::GetModuleIndex(sector->GetAddress()) == iModule);

  Bool_t result1 = ShouldSegmentByX(sector);
  Bool_t result2 = ShouldSegmentByY(sector);

  if(result1 || result2){
    delete sector;
  }
  else{
    CbmMuchStation* station = (CbmMuchStation*)fStations->At(iStation);
    Double_t rMax = station->GetRmax();
    if((IntersectsRad(sector, module->GetCutRadius())==2) || !IntersectsRad(sector, rMax)){
      delete sector;
      return;
    }
    module->AddSector(sector);
    return;
  }

  Int_t iSector;
  Double_t newSecX, newSecY, newSecLx, newSecLy;
  Bool_t equal = TMath::Abs(secLx - secLy) < 1e-5;
  Bool_t res = secLx > secLy;
  TVector3 position, size;
  if(result1 && result2){
    if(equal) {
      res = kTRUE;
    }
    for(Int_t i = 0; i < 2; ++i){
      iSector = module->GetNSectors();//CbmMuchGeoScheme::GetDetIdFromModule(moduleId, module->GetNSectors());
      newSecLx = res ? secLx/2. : secLx;
      newSecLy = res ? secLy    : secLy/2.;
      newSecX  = res ? secX + (i - 0.5)*newSecLx : secX;
      newSecY  = res ? secY : secY + (i - 0.5)*newSecLy;
      position.SetXYZ(newSecX, newSecY, secZ);
      size.SetXYZ(newSecLx, newSecLy, secLz);
      SegmentSector(module, new CbmMuchSectorRectangular(detectorId, iSector, position, size, 8, 16));
    }
  }
  else if(result1 || result2){
    for(Int_t i = 0; i < 2; i++){
      iSector = module->GetNSectors();//CbmMuchGeoScheme::GetDetIdFromModule(moduleId, module->GetNSectors());
      newSecLx = result1 ? secLx/2. : secLx;
      newSecLy = result1 ? secLy    : secLy/2;
      newSecX  = result1 ? secX + (i - 0.5)*newSecLx : secX;
      newSecY  = result1 ? secY : secY + (i - 0.5)*newSecLy;
      position.SetXYZ(newSecX, newSecY, secZ);
      size.SetXYZ(newSecLx, newSecLy, secLz);
      SegmentSector(module, new CbmMuchSectorRectangular(detectorId, iSector, position, size, 8, 16));
    }
  }
}
// -------------------------------------------------------------------------

// -----  Private method ShouldSegmentByX  ---------------------------------
Bool_t CbmMuchSegmentAuto::ShouldSegmentByX(CbmMuchSectorRectangular* sector){
  Double_t secLx = sector->GetSize()[0];
  Double_t secLy = sector->GetSize()[1];
  Double_t secX  = sector->GetPosition()[0];
  Double_t secY  = sector->GetPosition()[1];

  Double_t ulR = TMath::Sqrt((secX - secLx/2.)*(secX - secLx/2.) + (secY + secLy/2.)*(secY + secLy/2.));
  Double_t urR = TMath::Sqrt((secX + secLx/2.)*(secX + secLx/2.) + (secY + secLy/2.)*(secY + secLy/2.));
  Double_t blR = TMath::Sqrt((secX - secLx/2.)*(secX - secLx/2.) + (secY - secLy/2.)*(secY - secLy/2.));
  Double_t brR = TMath::Sqrt((secX + secLx/2.)*(secX + secLx/2.) + (secY - secLy/2.)*(secY - secLy/2.));

  Double_t uR = (ulR < urR) ? ulR : urR;
  Double_t bR = (blR < brR) ? blR : brR;
  Double_t R  = (uR < bR) ? uR : bR;

  Int_t iStation = CbmMuchAddress::GetStationIndex(sector->GetAddress());
  //CbmMuchStationGem* station = (CbmMuchStationGem*)fStations->At(iStation);
  // Check minimum and maximum allowed resolution
  Double_t sigmaMax = fSigmaXmax.at(iStation);//station->GetSigmaXmax(); //[cm]
  Double_t sigmaMin = fSigmaXmin.at(iStation);//station->GetSigmaXmin(); //[cm]
  Double_t sigma    = sector->GetSigmaX();
  if(sigma > sigmaMin && sigma/2. < sigmaMin) return false;
  if(sigma > sigmaMax) return true;
  // Check for occupancy
  Double_t hitDensity   = exp(fExp0.at(iStation) + fExp1.at(iStation)*R);
  Double_t occupancyMax = fOccupancyMax.at(iStation);//station->GetOccupancyMax();
  Double_t sPad  = secLx*secLy/128.;
  Double_t nPads = (1.354-0.304*TMath::Log2(sPad)); // number of pads fired by one track in average
  //printf("nPads:%f\n",nPads);
  Double_t occupancy = hitDensity*sPad*nPads;
  if(occupancy > occupancyMax) return true;
  return false;
}
// -------------------------------------------------------------------------

// -----  Private method ShouldSegmentByY  ---------------------------------
Bool_t CbmMuchSegmentAuto::ShouldSegmentByY(CbmMuchSectorRectangular* sector){
  Double_t secLx = sector->GetSize()[0];
  Double_t secLy = sector->GetSize()[1];
  Double_t secX  = sector->GetPosition()[0];
  Double_t secY  = sector->GetPosition()[1];

  Double_t ulR = TMath::Sqrt((secX - secLx/2.)*(secX - secLx/2.) + (secY + secLy/2.)*(secY + secLy/2.));
  Double_t urR = TMath::Sqrt((secX + secLx/2.)*(secX + secLx/2.) + (secY + secLy/2.)*(secY + secLy/2.));
  Double_t blR = TMath::Sqrt((secX - secLx/2.)*(secX - secLx/2.) + (secY - secLy/2.)*(secY - secLy/2.));
  Double_t brR = TMath::Sqrt((secX + secLx/2.)*(secX + secLx/2.) + (secY - secLy/2.)*(secY - secLy/2.));

  Double_t uR = (ulR < urR) ? ulR : urR;
  Double_t bR = (blR < brR) ? blR : brR;
  Double_t R  = (uR < bR) ? uR : bR;

  Int_t iStation = CbmMuchAddress::GetStationIndex(sector->GetAddress());
  //CbmMuchStationGem* station = (CbmMuchStationGem*)fStations->At(iStation);
  // Check minimum and maximum allowed resolution
  Double_t sigmaMax = fSigmaYmax.at(iStation);//station->GetSigmaYmax(); //[cm]
  Double_t sigmaMin = fSigmaYmin.at(iStation);//station->GetSigmaYmin(); //[cm]
  Double_t sigma    = sector->GetSigmaY();
  if(sigma > sigmaMin && sigma/2. < sigmaMin) return false;
  if(sigma > sigmaMax) return true;
  // Check for occupancy
  Double_t hitDensity   = exp(fExp0.at(iStation) + fExp1.at(iStation)*R);
  Double_t occupancyMax = fOccupancyMax.at(iStation);//station->GetOccupancyMax();
  Double_t sPad  = secLx*secLy/128.;
  Double_t nPads = (1.354-0.304*TMath::Log2(sPad)); // number of pads fired by one track in average
  Double_t occupancy = hitDensity*sPad*nPads;
  if(occupancy > occupancyMax) return true;
  return false;
}
// -------------------------------------------------------------------------


// -----  Private method IntersectsHole  -----------------------------------
Int_t CbmMuchSegmentAuto::IntersectsRad(CbmMuchSectorRectangular* sector, Double_t radius){
  if(radius < 0) return 0;

  Int_t intersects = 0;

  Double_t secLx = sector->GetSize()[0];
  Double_t secLy = sector->GetSize()[1];
  Double_t secX  = sector->GetPosition()[0];
  Double_t secY  = sector->GetPosition()[1];

  Double_t ulR = TMath::Sqrt((secX - secLx/2.)*(secX - secLx/2.) + (secY + secLy/2.)*(secY + secLy/2.));
  Double_t urR = TMath::Sqrt((secX + secLx/2.)*(secX + secLx/2.) + (secY + secLy/2.)*(secY + secLy/2.));
  Double_t blR = TMath::Sqrt((secX - secLx/2.)*(secX - secLx/2.) + (secY - secLy/2.)*(secY - secLy/2.));
  Double_t brR = TMath::Sqrt((secX + secLx/2.)*(secX + secLx/2.) + (secY - secLy/2.)*(secY - secLy/2.));

  if(ulR < radius) intersects++;
  if(urR < radius) intersects++;
  if(blR < radius) intersects++;
  if(brR < radius) intersects++;

  if(intersects == 4) return 2;
  if(intersects) return 1;
  else return 0;
}
// -------------------------------------------------------------------------

void CbmMuchSegmentAuto::Print(){
  printf("Segmentation written to file %s\n", fDigiFileName);
  Int_t nTotSectors = 0;
  Int_t nTotChannels = 0;
  Int_t nTotGems = 0;
  Int_t nTotStraws = 0;
  printf("=================================================================================================\n");
  for(Int_t iStation=0; iStation < fStations->GetEntries(); ++iStation){
    CbmMuchStation* station = (CbmMuchStation*) fStations->At(iStation);
    Int_t nGems = 0;
    Int_t nStraws = 0;
    Int_t nSectors = 0;
    Int_t nChannels = 0;
    Double_t padMaxLx = std::numeric_limits<Double_t>::min();
    Double_t padMinLx = std::numeric_limits<Double_t>::max();
    Double_t padMaxLy = std::numeric_limits<Double_t>::min();
    Double_t padMinLy = std::numeric_limits<Double_t>::max();
    if(!station) continue;
    for(Int_t iLayer = 0; iLayer < station->GetNLayers(); ++iLayer){
      CbmMuchLayer* layer = station->GetLayer(iLayer);
      if(!layer) continue;
      for(Int_t iSide = 0; iSide < 2; ++iSide){
        CbmMuchLayerSide* side = layer->GetSide(iSide);
        if(!side) continue;
        for(Int_t iModule=0; iModule < side->GetNModules(); ++iModule){
          CbmMuchModule* mod = side->GetModule(iModule);
          if(!mod) continue;
          switch(mod->GetDetectorType()){
            case 1:{ // GEMs
              CbmMuchModuleGem* module = (CbmMuchModuleGem*)mod;
              if(!module) continue;
              nGems++;
              nSectors += module->GetNSectors();
              for(Int_t iSector=0; iSector<module->GetNSectors(); ++iSector){
                CbmMuchSectorRectangular* sector = (CbmMuchSectorRectangular*) module->GetSector(iSector);
                if(!sector) continue;
                Double_t padLx = sector->GetPadDx();
                Double_t padLy = sector->GetPadDy();
                if(padLx > padMaxLx) padMaxLx = padLx;
                if(padLx < padMinLx) padMinLx = padLx;
                if(padLy > padMaxLy) padMaxLy = padLy;
                if(padLy < padMinLy) padMinLy = padLy;
                nChannels += sector->GetNChannels();
              }
              break;
            }
            case 2: // Straw tubes
              nStraws++;
              break;
          }
        }
      }
    }
    printf("Station %i:\n", iStation+1);
    printf("   GEM modules: %i\n", nGems);
    if(nGems) printf("      Sectors: %i,  Pads: %i, Min.Pad size:%3.2fx%3.2f, Max.Pad size:%3.2fx%3.2f\n",nSectors, nChannels, padMinLx, padMinLy, padMaxLx, padMaxLy);
    printf("   Straw modules: %i\n", nStraws);
    nTotSectors += nSectors;
    nTotChannels += nChannels;
    nTotGems += nGems;
    nTotStraws += nStraws;
  }
  printf("-------------------------------------------------------------------------------------------------\n");
  printf(" Summary: \n   GEM modules: %i\n      Sectors: %i, Pads: %i\n   Straw modules: %i\n",
      nTotGems, nTotSectors, nTotChannels, nTotStraws);
  printf("=================================================================================================\n");
}

void CbmMuchSegmentAuto::DrawSegmentation(){
  string digifile(fDigiFileName);
  Int_t startIndex = digifile.size() - 4;
  string txtfile  = digifile.erase(startIndex, 4);
  txtfile.append("txt");
  FILE* outfile;
  outfile = fopen(txtfile.c_str(), "w");

  Int_t colors[] = {kGreen, kBlue, kViolet, kRed, kYellow, kOrange, kMagenta, kCyan,  kSpring, kPink, kAzure, kTeal,
      kGreen+10, kBlue+10, kViolet+10, kRed+10, kYellow+10, kOrange+10, kMagenta+10, kCyan+10,  kSpring+10, kPink+10, kAzure+10, kTeal+10};

  Double_t secMinLx = std::numeric_limits<Double_t>::max();
  Double_t secMinLy = std::numeric_limits<Double_t>::max();
  for(Int_t iStation=0; iStation < fNStations; ++iStation){
    CbmMuchStation* station = (CbmMuchStation*) fStations->At(iStation);
    CbmMuchLayer* layer = station->GetLayer(0);
    for (Int_t iSide=1;iSide>=0;iSide--){
      CbmMuchLayerSide* side = layer->GetSide(iSide);
      for (Int_t iModule=0;iModule<side->GetNModules();++iModule) {
        CbmMuchModule* mod = side->GetModule(iModule);
        if(mod->GetDetectorType()!= 1) continue;
        CbmMuchModuleGem* module = (CbmMuchModuleGem*) mod;
        for(Int_t iSector=0;iSector<module->GetNSectors();++iSector){
          CbmMuchSectorRectangular* sector = (CbmMuchSectorRectangular*) module->GetSector(iSector);
          if(sector->GetSize()[0] < secMinLx) secMinLx = sector->GetSize()[0];
          if(sector->GetSize()[1] < secMinLy) secMinLy = sector->GetSize()[1];
        }
      } // modules
    } // sides
  }

  for (Int_t iStation=0;iStation<fStations->GetEntriesFast();++iStation){
    fprintf(outfile, "============================================================================\n");
    fprintf(outfile, "Station %i\n", iStation+1);
    fprintf(outfile, "Sector size, cm   Sector position, cm   Number of pads   Side   Pad size, cm\n");
    fprintf(outfile, "----------------------------------------------------------------------------\n");
    TCanvas* c1 = new TCanvas(Form("station%i",iStation+1),Form("station%i",iStation+1),800,800);
    c1->SetFillColor(0);
    c1->Range(-250,-250,250,250);
    CbmMuchStation* station = (CbmMuchStation*) fStations->At(iStation);
    CbmMuchLayer* layer = station->GetLayer(0);
    for (Int_t iSide=1;iSide>=0;iSide--){
      CbmMuchLayerSide* layerSide = layer->GetSide(iSide);
      for (Int_t iModule=0;iModule<layerSide->GetNModules();++iModule) {
        CbmMuchModule* mod = layerSide->GetModule(iModule);
        mod->SetFillStyle(0);
        mod->Draw();
        if(mod->GetDetectorType() != 1) continue;
        CbmMuchModuleGem* module = (CbmMuchModuleGem*)mod;
        for (Int_t iSector=0;iSector<module->GetNSectors();++iSector){
          CbmMuchSectorRectangular* sector = (CbmMuchSectorRectangular*) module->GetSector(iSector);

          Int_t i = Int_t((sector->GetSize()[0]+1e-3)/secMinLx) - 1;
          Int_t j = Int_t((sector->GetSize()[1]+1e-3)/secMinLy) - 1;
// TODO
//          sector->SetFillColor(iSide ? TColor::GetColorDark(colors[i+j]) : colors[i+j]);
//          sector->Draw("f");
//          sector->Draw();
          const char* side = iSide ? "Back" : "Front";
          fprintf(outfile, "%-4.2fx%-10.2f   %-6.2fx%-12.2f   %-14i   %-5s   ", sector->GetSize()[0], sector->GetSize()[1],
              sector->GetPosition()[0], sector->GetPosition()[1], sector->GetNChannels(), side);
//TODO          fprintf(outfile, "%-4.2fx%-4.2f\n", sector->GetDx(), sector->GetDy());
        } // sectors
      } // modules
    } // sides

    // Draw a hole
    TArc* arc = new TArc(0.,0.,station->GetRmin());
    arc->Draw();

    c1->Print(Form("%s/station%i.eps",gSystem->DirName(fDigiFileName), iStation+1));
    c1->Print(Form("%s/station%i.png",gSystem->DirName(fDigiFileName), iStation+1));
  }//stations
  fclose(outfile);
}

ClassImp(CbmMuchSegmentAuto)
