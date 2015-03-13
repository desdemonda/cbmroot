/** CbmMuchSegmentManual.cxx
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 02.04.09
 *@version 1.0
 *
 * class for making parameter file for MUCH digitizer
 *
 */

#include "CbmMuchSegmentManual.h"
#include "CbmGeoMuchPar.h"
#include "CbmMuchStation.h"
#include "CbmMuchLayer.h"
#include "CbmMuchLayerSide.h"
#include "CbmMuchModuleGemRectangular.h"
#include "CbmMuchSectorRectangular.h"
#include "CbmMuchAddress.h"

#include "FairRuntimeDb.h"

#include "TFile.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TArc.h"
#include "TColor.h"
#include "TSystem.h"

using std::ifstream;

// -----   Default constructor   -------------------------------------------
CbmMuchSegmentManual::CbmMuchSegmentManual()
  : FairTask(),
    fGeoPar(NULL),
    fNStations(0),
    fStations(NULL),
    fInputFileName((char*)"much.digi.root"),
    fDigiFileName(),
    fNRegions(),
    fRadii(),
    fSecLx(),
    fSecLy(),
    fNChannels(),
    fNCols(),
    fNRows(),
    fDebug(0)
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchSegmentManual::CbmMuchSegmentManual(char* inputFileName, char* digiFileName)
  : FairTask(),
    fGeoPar(NULL),
    fNStations(0),
    fStations(NULL),
    fInputFileName(inputFileName),
    fDigiFileName(digiFileName),
    fNRegions(),
    fRadii(),
    fSecLx(),
    fSecLy(),
    fNChannels(),
    fNCols(),
    fNRows(),
    fDebug(0)
{
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMuchSegmentManual::~CbmMuchSegmentManual() {
}
// -------------------------------------------------------------------------

// -----   Public method SetNRegions  --------------------------------------
void CbmMuchSegmentManual::SetNRegions(Int_t iStation, Int_t nRegions){
  if(iStation < 0 || iStation >= fNStations) Fatal("SetNRegions", "iStation is out of range.");
  fNRegions[iStation] = nRegions;
  fRadii[iStation].resize(nRegions);
  fSecLx[iStation].resize(nRegions);
  fSecLy[iStation].resize(nRegions);
  fNCols[iStation].resize(nRegions);
  fNRows[iStation].resize(nRegions);

  if(fDebug){
    printf("Station %i has %i regions\n", iStation + 1, nRegions);
  }

  // Deal with channels more universally
  Int_t n = Int_t(TMath::Log2(fNChannels[iStation]) + 1e-2);
  Int_t nChans = Int_t(TMath::Power(2, n) + 1e-2);
  if(nChans != fNChannels[iStation]) Fatal("Init", "Number of channels should be equal to two with integer power.");
  Int_t nPower = n/2;
  for(Int_t iRegion = 0; iRegion < fNRegions[iStation]; ++iRegion){
    fNCols[iStation].at(iRegion) = (Int_t)TMath::Power(2, nPower);
    fNRows[iStation].at(iRegion) = n%2 != 0 ? (Int_t)TMath::Power(2, nPower+1) : fNCols[iStation].at(iRegion);

    if(fDebug){
      printf("Region %i has %i columns and %i rows per sector\n", iRegion+1, fNCols[iStation].at(iRegion),
          fNRows[iStation].at(iRegion));
    }
  }
}
// -------------------------------------------------------------------------

// -----   Public method SetNChannels --------------------------------------
void CbmMuchSegmentManual::SetNChannels(Int_t iStation, Int_t nChannels){
  if(iStation < 0 || iStation >= fNStations) Fatal("SetNChannels", "iStation is out of range.");

  fNChannels[iStation] = nChannels;

  if(fDebug){
    printf("Station %i has %i channels per sector\n", iStation+1, nChannels);
  }
}
// -------------------------------------------------------------------------

// -----   Public method SetRegionRadius  ----------------------------------
void CbmMuchSegmentManual::SetRegionRadius(Int_t iStation, Int_t iRegion, Double_t radius){
  if(iStation < 0 || iStation >= fNStations) Fatal("SetRegionRadius", "iStation is out of range.");
  if(iRegion < 0 || iRegion >= fNRegions[iStation]) Fatal("SetRegionRadius", "iRegion is out of range.");
  fRadii[iStation].at(iRegion) = radius;
  if(fDebug){
    printf("Radius of the Region %i of station %i is %4.2f cm\n", iRegion+1, iStation+1,
        fRadii[iStation].at(iRegion));
  }
}
// -------------------------------------------------------------------------

// -----   Public method SetSigma  -----------------------------------------
void CbmMuchSegmentManual::SetSigma(Int_t iStation, Int_t iRegion, Double_t sigmaX, Double_t sigmaY){
  if(iStation < 0 || iStation >= fNStations) Fatal("SetSigma", "iStation is out of range.");
  if(iRegion < 0 || iRegion >= fNRegions[iStation]) Fatal("SetSigma", "iRegion is out of range.");

  Double_t secLx = fSecLx[iStation].at(iRegion)
                 = fNCols[iStation].at(iRegion)*TMath::Sqrt(12.)*sigmaX;
  Double_t secLy = fSecLy[iStation].at(iRegion)
                 = fNRows[iStation].at(iRegion)*TMath::Sqrt(12.)*sigmaY;
  if(TMath::Abs(secLy-secLx) < 1e-5) return;
  if(TMath::Abs(secLy/secLx - 2) > 1e-5){
    if(secLy > 2*secLx){
      fNCols[iStation].at(iRegion) *= 2;
      fNRows[iStation].at(iRegion) /= 2;
    }
    else{
      fNCols[iStation].at(iRegion) /= 2;
      fNRows[iStation].at(iRegion) *= 2;
    }
    SetSigma(iStation, iRegion, sigmaX, sigmaY);
  }
}
// -------------------------------------------------------------------------

// -----   Public method SetPadSize ----------------------------------------
void CbmMuchSegmentManual::SetPadSize(Int_t iStation, Int_t iRegion, Double_t padLx, Double_t padLy){
  if(iStation < 0 || iStation >= fNStations) Fatal("SetPadSize", "iStation is out of range.");
  if(iRegion < 0 || iRegion >= fNRegions[iStation]) Fatal("SetPadSize", "iRegion is out of range.");

  Double_t secLx = fSecLx[iStation].at(iRegion)
                 = fNCols[iStation].at(iRegion)*padLx;
  Double_t secLy = fSecLy[iStation].at(iRegion)
                 = fNRows[iStation].at(iRegion)*padLy;
  if(TMath::Abs(secLy-secLx) < 1e-5) return;
  if(TMath::Abs(secLy/secLx - 2) > 1e-5){
    if(secLy > 2*secLx){
      fNCols[iStation].at(iRegion) *= 2;
      fNRows[iStation].at(iRegion) /= 2;
    }
    else{
      fNCols[iStation].at(iRegion) /= 2;
      fNRows[iStation].at(iRegion) *= 2;
    }
    SetPadSize(iStation, iRegion, padLx, padLy);
  }
}
// -------------------------------------------------------------------------

// -----   Private method SetParContainers  --------------------------------
void CbmMuchSegmentManual::SetParContainers() {
  // Get runtime database
  FairRuntimeDb* db = FairRuntimeDb::instance();
  if ( ! db ) Fatal("Init", "No runtime database");
  fGeoPar = (CbmGeoMuchPar*) db->getContainer("CbmGeoMuchPar");
}
// -------------------------------------------------------------------------

// -----   Private method Init ---------------------------------------------
InitStatus CbmMuchSegmentManual::Init(){
  printf("\n=============================  Inputs segmentation parameters  ================================\n");
  ReadInputFile();
  printf("\n===============================================================================================\n");


  // Get MUCH geometry parameter container
  fStations = fGeoPar->GetStations();
  if(!fStations) Fatal("Init", "No input array of MUCH stations.");
  if(fStations->GetEntries() != fNStations) Fatal("Init", "Incorrect number of stations.");

  if(fDebug){
    for(Int_t iStation = 0; iStation < fNStations; ++iStation){
      printf("Station %i\n", iStation+1);
      for(Int_t iRegion = 0; iRegion < fNRegions[iStation]; ++iRegion){
        printf("Region %i: fSecLx = %f fSecLy = %f\n", iRegion+1, fSecLx[iStation].at(iRegion),
            fSecLy[iStation].at(iRegion));
        printf("fNCols = %i fNRows = %i\n", fNCols[iStation].at(iRegion),
            fNRows[iStation].at(iRegion));

      }
    }
  }
  // Segment MuCh
  SegmentMuch();
  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method SegmentMuch  --------------------------------------
void CbmMuchSegmentManual::SegmentMuch(){
  for(Int_t iStation=0; iStation<fStations->GetEntries(); ++iStation){
    CbmMuchStation* station = (CbmMuchStation*)fStations->At(iStation);

    Int_t nLayers = station->GetNLayers();
    for(Int_t iLayer = 0; iLayer < nLayers; ++iLayer){
      CbmMuchLayer* layer = station->GetLayer(iLayer);
      if(!layer) Fatal("SegmentMuch", "Incomplete layers array.");
      // Segment layer sides
      SegmentLayerSide(layer->GetSideF());
      SegmentLayerSide(layer->GetSideB());
    }
    printf("Station %i segmented\n",iStation+1);
  }

  // Save parameters
  TFile* oldfile = gFile;
  TFile* f = new TFile(fDigiFileName, "RECREATE");
  fStations->Write("stations",1);

  f->Close();
  gFile=oldfile;

  // Draw colored stations
  DrawSegmentation();

  // Print some output info
  Print();
}
// -------------------------------------------------------------------------

// -----   Private method SegmentLayerSide  --------------------------------
void CbmMuchSegmentManual::SegmentLayerSide(CbmMuchLayerSide* layerSide){
  if(!layerSide) Fatal("SegmentLayerSide", "Incomplete layer sides array.");
  Int_t nModules = layerSide->GetNModules();
  for(Int_t iModule = 0; iModule < nModules; iModule++){
    CbmMuchModule* module = layerSide->GetModule(iModule);
    if(module->GetDetectorType()!=1) continue;
    CbmMuchModuleGemRectangular* mod = (CbmMuchModuleGemRectangular*)module;
    if(nModules > 1) SegmentModule(mod, true); // Module design
    else SegmentModule(mod, false);            // Monolithic design
  }
}
// -------------------------------------------------------------------------

// -----   Private method SegmentSector  -----------------------------------
void CbmMuchSegmentManual::SegmentModule(CbmMuchModuleGemRectangular* module, Bool_t useModuleDesign){
  Int_t detectorId = module->GetDetectorId();
  Int_t iStation = CbmMuchAddress::GetStationIndex(detectorId);
  CbmMuchStation* station = (CbmMuchStation*)fStations->At(iStation);
  Int_t iRegion = -1;
  Double_t secMaxLx = GetSectorMaxSize(module, "Width", iRegion);
  Double_t secMaxLy = GetSectorMaxSize(module, "Length", iRegion);
  Double_t padMaxLx = GetPadMaxSize(module, "Width");
  Double_t padMaxLy = GetPadMaxSize(module, "Length");
  TVector3 size = module->GetSize();
  Double_t modLx = size.X();
  Double_t modLy = size.Y();
  Double_t modLz = size.Z();
  TVector3 position = module->GetPosition();
  Double_t modX = position.X();
  Double_t modY = position.Y();
  Double_t modZ = position.Z();

  Int_t nCols = Int_t(modLx/secMaxLx);
  Int_t nRows = Int_t(modLy/secMaxLy);
  Int_t nX = modX < 0 ? -1 : 1;
  Int_t nY = modY < 0 ? -1 : 1;

  Double_t secX, secY, secLx, secLy;
  TVector3 secSize, secPosition;
  Int_t iSector;
  for(Int_t iCol=0; iCol<nCols; ++iCol){
      secX = useModuleDesign ? modX + nX*((iCol + 0.5)*secMaxLx - modLx/2.)
          : secMaxLx*nCols/2. - (0.5 + iCol)*secMaxLx;
    for(Int_t iRow=0; iRow<nRows; ++iRow){
      secY = useModuleDesign ? modY + nY*((iRow + 0.5)*secMaxLy - modLy/2.)
          : secMaxLy*nRows/2. - (0.5 + iRow)*secMaxLy;
      iSector = module->GetNSectors();
      secSize.SetXYZ(secMaxLx, secMaxLy, modLz);
      secPosition.SetXYZ(secX, secY, modZ);
      SegmentSector(module, new CbmMuchSectorRectangular(detectorId, iSector, secPosition, secSize,
                    fNCols[iStation].at(iRegion), fNRows[iStation].at(iRegion)));
    }
  }

  // Process incomplete sectors
  Int_t nPadRows, nPadCols;
  Int_t n = useModuleDesign ? 1 : 2;
  Double_t ly = useModuleDesign ? modLy - secMaxLy*nRows : (modLy - secMaxLy*nRows)/2.;
  if(ly > padMaxLy){
    nPadRows = Int_t(ly/padMaxLy);
    for(Int_t i=0;i<n; ++i){
      secY = useModuleDesign ? modY + nY*(modLy/2. - ly/2.)
          : TMath::Power(-1, i)*(modLy/2. - ly/2.);
      for(Int_t iCol=0; iCol<nCols; ++iCol){
        secX = useModuleDesign ? modX + nX*((iCol + 0.5)*secMaxLx - modLx/2.)
            : secMaxLx*nCols/2. - (0.5 + iCol)*secMaxLx;
        iSector = module->GetNSectors();
        secSize.SetXYZ(secMaxLx, ly, modLz);
        secPosition.SetXYZ(secX, secY, modZ);
        SegmentSector(module, new CbmMuchSectorRectangular(detectorId, iSector, secPosition, secSize, fNCols[iStation].at(iRegion), nPadRows));
      }
    }
  }

  Double_t lx = useModuleDesign ? modLx - secMaxLx*nCols : (modLx - secMaxLx*nCols)/2.;
  if(lx > padMaxLx){
    nPadCols = (Int_t)(lx/padMaxLx);
    for(Int_t i=0; i<n; ++i){
      secX = useModuleDesign ? modX + nX*(modLx/2. - lx/2.)
          : TMath::Power(-1, i)*(modLx/2. - lx/2.);
      for(Int_t iRow=0; iRow<nRows; ++iRow){
        secY = useModuleDesign ? modY + nY*((iRow + 0.5)*secMaxLy - modLy/2.)
            : secMaxLy*nRows/2. - (0.5 + iRow)*secMaxLy;
        iSector = module->GetNSectors();
        secSize.SetXYZ(lx, secMaxLy, modLz);
        secPosition.SetXYZ(secX, secY, modZ);
        SegmentSector(module, new CbmMuchSectorRectangular(detectorId, iSector, secPosition, secSize, nPadCols, fNRows[iStation].at(iRegion)));
      }
    }
  }

  if(lx > padMaxLx && ly > padMaxLy){
    nPadCols = (Int_t)(lx/padMaxLx);
    nPadRows = (Int_t)(ly/padMaxLy);
    secX = modX + nX*(modLx/2. - lx/2.);
    secY = modY + nY*(modLy/2. - ly/2.);
    iSector = module->GetNSectors();
    secSize.SetXYZ(lx, ly, modLz);
    secPosition.SetXYZ(secX, secY, modZ);
    SegmentSector(module, new CbmMuchSectorRectangular(detectorId, iSector, secPosition, secSize, nPadCols, nPadRows));
  }
}
// -------------------------------------------------------------------------

// -----   Private method SegmentSector  -----------------------------------
void CbmMuchSegmentManual::SegmentSector(CbmMuchModuleGemRectangular* module, CbmMuchSectorRectangular* sector){
  TVector3 secSize = sector->GetSize();
  TVector3 secPosition = sector->GetPosition();
  Int_t detectorId = module->GetDetectorId();
  Int_t iStation = CbmMuchAddress::GetStationIndex(detectorId);
  Int_t iSector = module->GetNSectors();
  Double_t secLx = secSize.X();
  Double_t secLy = secSize.Y();
  Double_t secLz = secSize.Z();
  Double_t padLx = sector->GetPadDx();
  Double_t padLy = sector->GetPadDy();
  Double_t secX  = secPosition.X();
  Double_t secY  = secPosition.Y();
  Double_t secZ  = secPosition.Z();
  Bool_t isIncomplete = IsIncompleteSector(sector);
  Int_t nCols = sector->GetPadNx();
  Int_t nRows = sector->GetPadNy();
  Int_t nX = secX < 0 ? -1 : 1;
  Int_t nY = secY < 0 ? -1 : 1;

  Int_t iRegion = -1;
  Bool_t resultX = ShouldSegment(sector, "X", iRegion);
  Bool_t resultY = ShouldSegment(sector, "Y", iRegion);

  if(!resultX && !resultY){
    delete sector;

    CbmMuchStation* station = (CbmMuchStation*)fStations->At(iStation);
    Double_t rMax = station->GetRmax();
    if(IntersectsRad(sector, module->GetCutRadius())==2 ||
       !IntersectsRad(sector, rMax)) return;
    nCols = isIncomplete ? nCols : fNCols[iStation].at(iRegion);
    nRows = isIncomplete ? nRows : fNRows[iStation].at(iRegion);
    module->AddSector(new CbmMuchSectorRectangular(detectorId, iSector, secPosition, secSize, nCols, nRows));
    return;
  }

  // Needed for the case of incomplete sectors
  Int_t iReg = -1;
  Int_t nC = Int_t(padLx/GetPadMaxSize(module, "Width"));
  Int_t nR = Int_t(padLy/GetPadMaxSize(module, "Length"));
  Double_t pLx = nC == 0 ? padLx : nC*GetPadMaxSize(module, "Width");
  Double_t pLy = nR == 0 ? padLy : nR*GetPadMaxSize(module, "Length");
  Double_t sLx = nC == 0 ? secLx : nC*GetSectorMaxSize(module, "Width", iReg);
  Double_t sLy = nR == 0 ? secLy : nR*GetSectorMaxSize(module, "Length", iReg);
  nCols = Int_t(sLx/pLx);
  nRows = Int_t(sLy/pLy);
  delete sector;

  TVector3 position, size;
  Double_t newSecLx, newSecLy, newSecX, newSecY;
  for(Int_t i=0; i<2; ++i){
    newSecLx = resultX ? i*(secLx - newSecLx) + (1-i)*pLx/2.*nCols : secLx;
    newSecLy = resultY ? i*(secLy - newSecLy) + (1-i)*pLy/2.*nRows : secLy;
    newSecX  = resultX ? secX - TMath::Power(-1, i)*nX*(secLx/2. - newSecLx/2.) : secX;
    newSecY  = resultY ? secY - TMath::Power(-1, i)*nY*(secLy/2. - newSecLy/2.) : secY;

    Double_t newPadLx = resultX ? pLx/2. : padLx;
    Double_t newPadLy = resultY ? pLy/2. : padLy;
    nCols = Int_t(newSecLx/newPadLx);
    nRows = Int_t(newSecLy/newPadLy);

    position.SetXYZ(newSecX, newSecY, secZ);
    size.SetXYZ(newSecLx, newSecLy, secLz);
    iSector = module->GetNSectors();
    SegmentSector(module, new CbmMuchSectorRectangular(detectorId, iSector, position, size, nCols, nRows));
  }
}
// -------------------------------------------------------------------------

// -----   Private method IntersectsRad  -----------------------------------
Int_t CbmMuchSegmentManual::IntersectsRad(CbmMuchModuleGemRectangular* module, Double_t radius){
  if(radius < 0) return 0;

  Int_t intersects = 0;

  Double_t modLx = module->GetSize()[0];
  Double_t modLy = module->GetSize()[1];
  Double_t modX  = module->GetPosition()[0];
  Double_t modY  = module->GetPosition()[1];

  Double_t ulR = TMath::Sqrt((modX - modLx/2.)*(modX - modLx/2.) + (modY + modLy/2.)*(modY + modLy/2.));
  Double_t urR = TMath::Sqrt((modX + modLx/2.)*(modX + modLx/2.) + (modY + modLy/2.)*(modY + modLy/2.));
  Double_t blR = TMath::Sqrt((modX - modLx/2.)*(modX - modLx/2.) + (modY - modLy/2.)*(modY - modLy/2.));
  Double_t brR = TMath::Sqrt((modX + modLx/2.)*(modX + modLx/2.) + (modY - modLy/2.)*(modY - modLy/2.));

  if(ulR < radius) intersects++;
  if(urR < radius) intersects++;
  if(blR < radius) intersects++;
  if(brR < radius) intersects++;

  if(intersects == 4) return 2;
  if(intersects) return 1;
  else return 0;
}
// -------------------------------------------------------------------------

// -----   Private method IntersectsRad  -----------------------------------
Int_t CbmMuchSegmentManual::IntersectsRad(CbmMuchSectorRectangular* sector, Double_t radius){
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

// -----   Private method ShouldSegment  -----------------------------------
Bool_t CbmMuchSegmentManual::ShouldSegment(CbmMuchSectorRectangular* sector, const TString direction, Int_t &iRegion){
  Double_t secLx = sector->GetSize()[0];
  Double_t secLy = sector->GetSize()[1];
  Double_t secArea = secLx*secLy;
  Double_t secL = direction == "X" ? secLx : secLy;
  Bool_t isIncomplete = IsIncompleteSector(sector);

  Int_t iStation = CbmMuchAddress::GetStationIndex(sector->GetAddress());

  // Get region index for the sector
  iRegion = GetRegionIndex(sector);
  Double_t secRegL = direction == "X" ? fSecLx[iStation].at(iRegion) : fSecLy[iStation].at(iRegion);
  Double_t secRegArea = fSecLx[iStation].at(iRegion)*fSecLy[iStation].at(iRegion);

  if(secArea > secRegArea){
    // If sector length is larger than it's width
    if(secLy > secLx && direction == "X" && ShouldSegment(sector, "Y", iRegion)) return false;
    // If sector width is larger than or equal to it's length
    if(secLy <= secLx && direction == "Y") return false;

    // If sector size is larger than that corresponding to the region
    if(secL > secRegL) return true;
  }
  return false;
}
// -------------------------------------------------------------------------

// -----   Private method GetRegionIndex  ----------------------------------
Int_t CbmMuchSegmentManual::GetRegionIndex(CbmMuchSectorRectangular* sector){
  Int_t iStation = CbmMuchAddress::GetStationIndex(sector->GetAddress());
  Double_t secLx = sector->GetSize()[0];
  Double_t secLy = sector->GetSize()[1];
  Double_t secArea = secLx*secLy;
  Double_t sX = TMath::Abs(sector->GetPosition()[0]) - secLx/2.;
  Double_t sY = TMath::Abs(sector->GetPosition()[1]) - secLy/2.;
  Double_t secRad = TMath::Sqrt(sX*sX + sY*sY);

  Int_t iRegion = fNRegions[iStation] - 1;

  for(Int_t iReg = 0; iReg < fNRegions[iStation]; ++iReg){
    Double_t secRegArea = fSecLx[iStation].at(iReg)*fSecLy[iStation].at(iReg);
    Double_t regionRad = fRadii[iStation].at(iReg);
    if(secRad > regionRad) continue;

    iRegion = iReg;
    if(iReg > 0 && !IsIncompleteSector(sector)){
      Double_t secPrevRegArea = fSecLx[iStation].at(iReg-1)*fSecLy[iStation].at(iReg-1);
      if(secArea < secRegArea && secArea >= secPrevRegArea) iRegion--;
    }
    break;
  }
  return iRegion;
}
// -------------------------------------------------------------------------

// -----   Private method GetSectorMaxSize  --------------------------------
Double_t CbmMuchSegmentManual::GetSectorMaxSize(CbmMuchModuleGemRectangular* module,
    const TString side, Int_t &iRegion){
  Int_t iStation = CbmMuchAddress::GetStationIndex(module->GetDetectorId());
  Int_t nRegions = fNRegions[iStation];
  for(iRegion = 0; iRegion < nRegions; ++iRegion){
    Double_t rad = fRadii[iStation].at(iRegion);
    Int_t result = IntersectsRad(module, rad);
    if(result == 2) return side == "Width" ? fSecLx[iStation].at(iRegion) :fSecLy[iStation].at(iRegion);
  }
  iRegion = nRegions - 1;
  return side == "Width" ? fSecLx[iStation].at(iRegion) :fSecLy[iStation].at(iRegion);
}
// -------------------------------------------------------------------------

// -----   Private method GetPadMaxSize  -----------------------------------
Double_t CbmMuchSegmentManual::GetPadMaxSize(CbmMuchModuleGemRectangular* module, const TString side){
  Int_t iStation = CbmMuchAddress::GetStationIndex(module->GetDetectorId());
  Int_t iRegion = -1;
  Double_t sectorSize = GetSectorMaxSize(module, side, iRegion);
  return side == "Width" ? sectorSize/fNCols[iStation].at(iRegion)
                          : sectorSize/fNRows[iStation].at(iRegion);
}
// -------------------------------------------------------------------------

// -----   Private method IsIncompleteSector  ------------------------------
Bool_t CbmMuchSegmentManual::IsIncompleteSector(CbmMuchSectorRectangular* sector){
  Bool_t result = false;
  Int_t iStation = CbmMuchAddress::GetStationIndex(sector->GetAddress());
  Double_t secLx = sector->GetSize()[0];
  Double_t secLy = sector->GetSize()[1];
  Double_t minL = TMath::Min(secLx, secLy);
  Double_t maxL = TMath::Max(secLx, secLy);
  Int_t nFrac = Int_t((maxL+1e-5)/minL);
  Int_t nPower = Int_t(TMath::Log2(nFrac) + 1e-2);
  Double_t maxL1 = minL*TMath::Power(2,nPower);

  if(TMath::Abs(maxL-maxL1 ) > 1e-5 || sector->GetNChannels() < fNChannels[iStation])
    result = true;
  return result;
}
// -------------------------------------------------------------------------

// -----   Private method Print  -------------------------------------------
void CbmMuchSegmentManual::Print(){
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
    Double_t secMaxLx = std::numeric_limits<Double_t>::min();
    Double_t secMinLx = std::numeric_limits<Double_t>::max();
    Double_t secMaxLy = std::numeric_limits<Double_t>::min();
    Double_t secMinLy = std::numeric_limits<Double_t>::max();
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
              CbmMuchModuleGemRectangular* module = (CbmMuchModuleGemRectangular*)mod;
              if(!module) continue;
              nGems++;
              nSectors += module->GetNSectors();
              for(Int_t iSector=0; iSector<module->GetNSectors(); ++iSector){
                CbmMuchSectorRectangular* sector = (CbmMuchSectorRectangular*) module->GetSectorByIndex(iSector);
                if(!sector) continue;
                Double_t padLx = sector->GetPadDx();
                Double_t padLy = sector->GetPadDy();
                if(padLx > padMaxLx) { padMaxLx = padLx; secMaxLx = sector->GetPadNx()*padLx; }
                if(padLx < padMinLx) { padMinLx = padLx; secMinLx = sector->GetPadNx()*padLx; }
                if(padLy > padMaxLy) { padMaxLy = padLy; secMaxLy = sector->GetPadNy()*padLy; }
                if(padLy < padMinLy) { padMinLy = padLy; secMinLy = sector->GetPadNy()*padLy; }
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
    if(nGems) {
      printf("      Sectors: %i, Min.Sector size:%3.2fx%3.2f, Max.Sector size:%3.2fx%3.2f\n",nSectors, secMinLx, secMinLy, secMaxLx, secMaxLy);
      for(Int_t iReg=0; iReg<fNRegions[iStation]; ++iReg){
        printf("Region %i: size %fx%f\n", iReg, fSecLx[iStation].at(iReg), fSecLy[iStation].at(iReg));
      }
      printf("      Pads: %i, Min.Pad size:%3.2fx%3.2f, Max.Pad size:%3.2fx%3.2f\n", nChannels, padMinLx, padMinLy, padMaxLx, padMaxLy);
    }
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

void CbmMuchSegmentManual::DrawSegmentation(){
  // Change file extension
  char txtfile[100];
  Int_t length = strlen(fDigiFileName);
  Int_t iChar;
  for(iChar = length-1; iChar >= 0; --iChar){
    if(fDigiFileName[iChar] == '.') break;
  }
  strncpy(txtfile, fDigiFileName, iChar+1);
  txtfile[iChar+1] = '\0';
  strcat(txtfile, "txt");

  FILE* outfile;
  outfile = fopen(txtfile, "w");
  Int_t colors[] = {kGreen, kMagenta, kCyan, kRed, kBlue, kYellow, kTeal,
                    kPink, kAzure, kOrange, kViolet, kSpring,
                    kGreen+2, kMagenta+2, kCyan+2, kRed+2, kBlue+2, kYellow+2, kTeal+2,
                    kPink+2, kAzure+2, kOrange+2, kViolet+2, kSpring+2,
                    kGreen+4, kMagenta+4, kCyan+4, kRed+4, kBlue+4, kYellow+4, kTeal+4,
                    kPink+4, kAzure+4, kOrange+4, kViolet+4, kSpring+4};

  for (Int_t iStation=0;iStation<fStations->GetEntriesFast();++iStation){
    fprintf(outfile, "===========================================================================\n");
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
        if(mod->GetDetectorType() != 1) continue;
        CbmMuchModuleGemRectangular* module = (CbmMuchModuleGemRectangular*)mod;
        module->InitModule();
        for (Int_t iSector=0;iSector<module->GetNSectors();++iSector){
          CbmMuchSectorRectangular* sector = (CbmMuchSectorRectangular*) module->GetSectorByIndex(iSector);
          // Reject incomplete sectors by size
          Double_t secLx = sector->GetSize()[0];
          Double_t secLy = sector->GetSize()[1];
          Bool_t isIncomplete = IsIncompleteSector(sector);
// TODO
//          if(isIncomplete){
//            iSide ? sector->SetFillColor(TColor::GetColorDark(kGray+1)) : sector->SetFillColor(kGray + 1);
//          }
//          else{
//            Int_t i = Int_t((secLx+1e-5)/fSecLx[iStation].at(0)) - 1;
//            Int_t j = Int_t((secLy+1e-5)/fSecLy[iStation].at(0)) - 1;
//            sector->SetFillColor(iSide ? TColor::GetColorDark(colors[i+j]) : colors[i+j]);
//          }
          sector->DrawPads();
          const char* side = iSide ? "Back" : "Front";
          fprintf(outfile, "%-4.2fx%-10.2f   %-6.2fx%-12.2f   %-14i   %-5s   ", secLx, secLy,
              sector->GetPosition()[0], sector->GetPosition()[1], sector->GetNChannels(), side);
          fprintf(outfile, "%-4.2fx%-4.2f\n", sector->GetPadDx(), sector->GetPadDy());
        } // sectors
      } // modules
    } // sides

    // Draw a hole
    TArc* holeArc = new TArc(0.,0.,station->GetRmin());
    holeArc->Draw();

    for(Int_t iRegion=0; iRegion < fNRegions[iStation]; ++iRegion){
      TArc* arc = new TArc(0.,0.,fRadii[iStation].at(iRegion));
      arc->SetLineColor(kBlack);
      arc->SetLineWidth(2);
      arc->SetFillStyle(0);
      arc->Draw();
    }

    c1->Print(Form("%s/station%i.eps",gSystem->DirName(fDigiFileName), iStation+1));
    c1->Print(Form("%s/station%i.png",gSystem->DirName(fDigiFileName), iStation+1));
  }//stations
  fclose(outfile);
}

void CbmMuchSegmentManual::ReadInputFile(){
  ifstream infile;
  infile.open(fInputFileName);
  if(!infile){
    Fatal("ReadInputFile","Error: Cannot open the input file.");
  }

  Int_t index;
  string str;
  vector<string> strs;

  // Set number of stations
  OmitDummyLines(infile, str);
  strs = Split(str, ' ');
  index = strs.size()-1;
  Int_t nStations;
  StrToNum(strs.at(index), nStations);
  SetNStations(nStations);
  printf("Number of stations: \t%i", fNStations);

  // Set number of channels
  OmitDummyLines(infile, str);
  strs = Split(str, ' ');
  printf("\nNumber of channels: ");
  for(Int_t iStation = 0; iStation < fNStations; ++iStation){
    index = strs.size() - fNStations + iStation;
    Int_t nChannels;
    StrToNum(strs.at(index), nChannels);
    SetNChannels(iStation, nChannels);
    printf("\t%i", fNChannels[iStation]);
  }

  // Set number of regions
  OmitDummyLines(infile, str);
  strs = Split(str, ' ');
  printf("\nNumber of regions: ");
  for(Int_t iStation = 0; iStation < fNStations; ++iStation){
    index = strs.size() - fNStations + iStation;
    Int_t nRegions;
    StrToNum(strs.at(index), nRegions);
    SetNRegions(iStation, nRegions);
    printf("\t%i", fNRegions[iStation]);
  }

  for(Int_t iStation = 0; iStation < fNStations; ++iStation){
    vector<Double_t> padWidth(fNRegions[iStation], 0);
    vector<Double_t> padLength(fNRegions[iStation], 0);

    // Set region radii
    OmitDummyLines(infile, str);
    strs = Split(str, ' ');
    printf("\nStation %i", iStation + 1);
    printf("\n   Region radii [cm]: ");
    for(Int_t iRegion = 0; iRegion < fNRegions[iStation]; ++iRegion){
      index = strs.size() - fNRegions[iStation] + iRegion;
      Double_t radius;
      StrToNum(strs.at(index), radius);
      SetRegionRadius(iStation, iRegion, radius);
      printf("\t%4.2f", fRadii[iStation].at(iRegion));
    }

    // Set pad width
    OmitDummyLines(infile, str);
    strs = Split(str, ' ');
    printf("\n   Pad width [cm]: ");
    for(Int_t iRegion = 0; iRegion < fNRegions[iStation]; ++iRegion){
      index = strs.size() - fNRegions[iStation] + iRegion;
      StrToNum(strs.at(index), padWidth.at(iRegion));
      printf("\t%4.2f", padWidth.at(iRegion));
    }

    // Set pad length
    OmitDummyLines(infile, str);
    strs = Split(str, ' ');
    printf("\n   Pad length [cm]: ");
    for(Int_t iRegion = 0; iRegion < fNRegions[iStation]; ++iRegion){
      index = strs.size() - fNRegions[iStation] + iRegion;
      StrToNum(strs.at(index), padLength.at(iRegion));
      printf("\t%4.2f", padLength.at(iRegion));
    }

    for(Int_t iRegion = 0; iRegion < fNRegions[iStation]; ++iRegion)
      SetPadSize(iStation, iRegion, padWidth.at(iRegion), padLength.at(iRegion));
  }
  infile.close();
}

ClassImp(CbmMuchSegmentManual)




