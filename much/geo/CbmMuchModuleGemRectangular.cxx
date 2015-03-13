/** CbmMuchModuleGemRectangular.cxx
 *@author  M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@version 1.0
 *@since   11.02.08
 **
 ** This class holds the transport geometry parameters
 ** of one side of MuCh module.
 **/

#include "CbmMuchModuleGemRectangular.h"
#include "CbmMuchSectorRectangular.h"
#include "CbmMuchPadRectangular.h"
#include "CbmMuchGeoScheme.h"

#include <algorithm>
#include <vector>
using std::vector;

// -----   Default constructor   -------------------------------------------
CbmMuchModuleGemRectangular::CbmMuchModuleGemRectangular()
  : CbmMuchModuleGem(),
    fUseModuleDesign(kFALSE),
    fGridNx(0),
    fGridNy(0),
    fGridDx(0.),
    fGridDy(0.),
    fGrid()
{
    fDetectorType = 1;
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
CbmMuchModuleGemRectangular::CbmMuchModuleGemRectangular(Int_t iStation, Int_t iLayer, Bool_t iSide,
    Int_t iModule, TVector3 position, TVector3 size, Double_t cutRadius)
  : CbmMuchModuleGem(iStation, iLayer, iSide, iModule, position, size, cutRadius),
    fUseModuleDesign(kFALSE),
    fGridNx(0),
    fGridNy(0),
    fGridDx(0.),
    fGridDy(0.),
    fGrid()
{
    fDetectorType = 1;
}
// -------------------------------------------------------------------------


// -----   Public method GetSector   ---------------------------------------
CbmMuchSectorRectangular* CbmMuchModuleGemRectangular::GetSector(Int_t ix, Int_t iy) {
  if(ix < 0 || ix >= fGridNx) return NULL;
  if(iy < 0 || iy >= fGridNy) return NULL;
  Long64_t iSector = fGrid[ix][iy];
  if (iSector==-1) return NULL;
  return (CbmMuchSectorRectangular*) fSectors[iSector];
}
// -------------------------------------------------------------------------


// -----   Public method GetSector   ---------------------------------------
CbmMuchSectorRectangular* CbmMuchModuleGemRectangular::GetSector(Double_t x, Double_t y){
  return GetSector(GetGridIndexX(x),GetGridIndexY(y));
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Int_t CbmMuchModuleGemRectangular::GetGridIndexX(Double_t x){
  Double_t mx0 = fPosition[0];
  Double_t mlx = fSize[0];
  Double_t msx = mx0>0 ? +1 : -1;
  Double_t mx1 = mx0-msx*mlx/2;
  return Int_t((x-mx1)/msx/fGridDx);
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Int_t CbmMuchModuleGemRectangular::GetGridIndexY(Double_t y){
  Double_t my0 = fPosition[1];
  Double_t mly = fSize[1];
  Double_t msy = my0>0 ? +1 : -1;
  Double_t my1 = my0-msy*mly/2;
  return Int_t((y-my1)/msy/fGridDy);
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
CbmMuchPadRectangular* CbmMuchModuleGemRectangular::GetPad(Double_t x, Double_t y){
  CbmMuchSectorRectangular* sector = GetSector(x,y);
  if (!sector) return NULL;
  return sector->GetPad(x,y);
}
// -------------------------------------------------------------------------


// -----   Public method InitGrid  -----------------------------------------
Bool_t CbmMuchModuleGemRectangular::InitGrid(Bool_t useModuleDesign) {
  Int_t nSectors = GetNSectors();
  if (!nSectors) return kFALSE;
  fUseModuleDesign = useModuleDesign;

  Double_t mx0 = fPosition[0];
  Double_t my0 = fPosition[1];
  Double_t mlx = fSize[0];
  Double_t mly = fSize[1];
  Double_t msx = mx0>0 ? +1 : -1;
  Double_t msy = my0>0 ? +1 : -1;
  Double_t mx1 = mx0-msx*mlx/2;
  Double_t my1 = my0-msy*mly/2;
  
  // Determine grid dimensions
  fGridDx = std::numeric_limits<Double_t>::max();
  fGridDy = std::numeric_limits<Double_t>::max();
  for (Int_t iSector = 0; iSector < nSectors; iSector++) {
    CbmMuchSectorRectangular* s = (CbmMuchSectorRectangular*) fSectors[iSector];
    if (s->IsIncomplete()) continue;
    Double_t dx = s->GetSize()[0];
    Double_t dy = s->GetSize()[1];
    if (dx<fGridDx) fGridDx = dx;
    if (dy<fGridDy) fGridDy = dy;
  }

  Int_t nCell = fUseModuleDesign ? 1 : 2; // Number of additional columns/rows in the grid
  fGridNx = Int_t ((mlx + 1e-5) / fGridDx) + nCell;
  fGridNy = Int_t ((mly + 1e-5) / fGridDy) + nCell;

  // Fill grid
  fGrid.resize(fGridNx);
  for (Int_t ix=0;ix<fGridNx;ix++){
    fGrid[ix].resize(fGridNy);
    for (Int_t iy=0;iy<fGridNy;iy++){
      Double_t x = mx1+msx*fGridDx*(ix+1e-3);
      Double_t y = my1+msy*fGridDy*(iy+1e-3);
      fGrid[ix][iy]=-1;
      for (int iSector = 0; iSector < nSectors; iSector++) {
        CbmMuchSectorRectangular* sec = (CbmMuchSectorRectangular*) fSectors[iSector];
        if(sec->Inside(x, y)) {
          fGrid[ix][iy] = sec->GetSectorIndex();
          break;
        }
      }
    }
  }
  return kTRUE;
}
// -------------------------------------------------------------------------

// ------ Public method InitNeighbours  ------------------------------------
void CbmMuchModuleGemRectangular::InitNeighbourSectors() {
  vector<CbmMuchSectorRectangular*> neighbours;
  vector<CbmMuchSectorRectangular*>::iterator it;
  for (Int_t iSector = 0; iSector < GetNSectors(); iSector++) {
    CbmMuchSectorRectangular* sector = (CbmMuchSectorRectangular*) fSectors[iSector];
    Double_t x1 = sector->GetX1()-1e-3;
    Double_t x2 = sector->GetX2()+1e-3;
    Double_t y1 = sector->GetY1()-1e-3;
    Double_t y2 = sector->GetY2()+1e-3;
    Double_t ix1 = GetGridIndexX(x1);
    Double_t ix2 = GetGridIndexX(x2);
    Double_t iy1 = GetGridIndexY(y1);
    Double_t iy2 = GetGridIndexY(y2);
    Double_t ixmin = (ix1<ix2) ? ix1 : ix2;
    Double_t ixmax = (ix1<ix2) ? ix2 : ix1;
    Double_t iymin = (iy1<iy2) ? iy1 : iy2;
    Double_t iymax = (iy1<iy2) ? iy2 : iy1;
    if (ixmin<0       ) ixmin = 0;
    if (ixmax>=fGridNx) ixmax = fGridNx-1;
    if (iymin<0       ) iymin = 0;
    if (iymax>=fGridNy) iymax = fGridNy-1;
    
    for (Int_t ix=ixmin;ix<=ixmax;ix++){
      for (Int_t iy=iymin;iy<=iymax;iy++){
        Int_t iSec = fGrid[ix][iy];
        if (iSec<0) continue;
        if (iSec==iSector) continue;
        CbmMuchSectorRectangular* sec = (CbmMuchSectorRectangular*) fSectors[iSec];
        it = find(neighbours.begin(),neighbours.end(),sec);
        if (it == neighbours.end()) neighbours.push_back(sec);
      }
    }
    sector->SetNeighbours(neighbours);
    neighbours.clear();
  }
}
// -------------------------------------------------------------------------

// ------ Public method InitNeighbourPads ----------------------------------
void CbmMuchModuleGemRectangular::InitNeighbourPads() {
  vector<CbmMuchPad*>::iterator it;
  vector<CbmMuchPad*> neighbours;
  
  // Loop over all sectors within the module
  for (Int_t iSector = 0; iSector < GetNSectors(); iSector++) {
    CbmMuchSectorRectangular* sector =(CbmMuchSectorRectangular*) fSectors[iSector];
    if (!sector) continue;
    Double_t mindx = sector->GetPadDx();
    Double_t mindy = sector->GetPadDy();
    vector<CbmMuchSectorRectangular*> neighbourSectors = sector->GetNeighbours();
    for (Int_t iSec=0;iSec<neighbourSectors.size();iSec++) {
      CbmMuchSectorRectangular* sec = neighbourSectors[iSec];
      Double_t dx = sec->GetPadDx();
      Double_t dy = sec->GetPadDy();
      if (dx < mindx) mindx = dx;
      if (dy < mindy) mindy = dy;
    }

    for (Int_t iChannel = 0; iChannel < sector->GetNChannels(); iChannel++) {
      CbmMuchPadRectangular* pad = (CbmMuchPadRectangular*) sector->GetPadByChannelIndex(iChannel);
      Double_t x1 = pad->GetX1();
      Double_t x2 = pad->GetX2();
      Double_t y1 = pad->GetY1();
      Double_t y2 = pad->GetY2();
      for (Double_t x=x1-mindx/2;x<x2+mindx/2+1e-3;x+=mindx){
        for (Double_t y=y1-mindy/2;y<y2+mindy/2+1e-3;y+=mindy){
          CbmMuchPad* p = GetPad(x,y);
          if (!p) continue;
          if (p==pad) continue;
          it = find(neighbours.begin(),neighbours.end(),p);
          if (it == neighbours.end()) neighbours.push_back(p);
        }
      }
      pad->SetNeighbours(neighbours);
      neighbours.clear();
    }
  }
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Bool_t CbmMuchModuleGemRectangular::InitModule(){
  Bool_t useModuleDesign = TMath::Abs(fPosition[0]) > 1e-5 || TMath::Abs(fPosition[1]) > 1e-5;
  if (!InitGrid(useModuleDesign)) return kFALSE;
  InitNeighbourSectors();
  for (Int_t iSector = 0; iSector < GetNSectors(); iSector++) {
    CbmMuchSectorRectangular* sector = (CbmMuchSectorRectangular*) fSectors[iSector];
    if (!sector) continue;
    sector->AddPads();
  }
  InitNeighbourPads();
  return kTRUE;
}
// -------------------------------------------------------------------------

ClassImp(CbmMuchModuleGemRectangular)
