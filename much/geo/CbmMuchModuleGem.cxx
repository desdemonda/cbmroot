/** CbmMuchModuleGem.cxx
 *@author  M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@version 1.0
 *@since   11.02.08
 **
 ** This class holds the transport geometry parameters
 ** of one side of MuCh module.
 **/

#include "CbmMuchModuleGem.h"
#include "CbmMuchGeoScheme.h"
#include "CbmMuchModule.h"
#include "CbmMuchSector.h"
#include "CbmMuchPad.h"


#include <vector>
using std::vector;

// -------------------------------------------------------------------------
CbmMuchModuleGem::CbmMuchModuleGem()
  : CbmMuchModule(),
    fSectors()
{
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
CbmMuchModuleGem::CbmMuchModuleGem(Int_t iStation, Int_t iLayer, Bool_t iSide,
    Int_t iModule, TVector3 position, TVector3 size, Double_t cutRadius)
  : CbmMuchModule(iStation, iLayer, iSide, iModule, position, size, cutRadius),
    fSectors()
{
}
// -------------------------------------------------------------------------


// -----   Public method GetPads -------------------------------------------
vector<CbmMuchPad*> CbmMuchModuleGem::GetPads() {
  vector<CbmMuchPad*> pads;
  for(Int_t iSector = 0; iSector < GetNSectors(); ++iSector){
    CbmMuchSector* sector = GetSectorByIndex(iSector);
    if(!sector) continue;
    for(Int_t iPad=0; iPad<sector->GetNChannels(); ++iPad){
      CbmMuchPad* pad = sector->GetPadByChannelIndex(iPad);
      if(!pad) continue;
      pads.push_back(pad);
    }
  }
  return pads;
}
// -------------------------------------------------------------------------


// -----   Public method GetNPads ------------------------------------------
Int_t CbmMuchModuleGem::GetNPads() {
  Int_t nChannels = 0;
  for(Int_t iSector=0; iSector < GetNSectors(); ++iSector){
    CbmMuchSector* sector = GetSectorByIndex(iSector);
    if(!sector) continue;
    nChannels += sector->GetNChannels();
  }
  return nChannels;
}
// -------------------------------------------------------------------------


// -----   Public method GetSector   ---------------------------------------
CbmMuchSector* CbmMuchModuleGem::GetSector(Int_t address) {
  Int_t iSector = CbmMuchAddress::GetSectorIndex(address);
  return (CbmMuchSector*)fSectors[iSector];
}
// -------------------------------------------------------------------------


// -----   Public method GetPad  -------------------------------------------
CbmMuchPad* CbmMuchModuleGem::GetPad(Int_t address) {
  CbmMuchSector* sector = GetSector(address);
  Int_t iChannel = CbmMuchAddress::GetChannelIndex(address);
  return sector ? sector->GetPadByChannelIndex(iChannel) : NULL;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchModuleGem::DrawModule(Color_t color) {
  for (Int_t s=0;s<fSectors.size();s++){
    CbmMuchSector* sector = (CbmMuchSector*) fSectors[s];
// TODO
//    sector->SetFillColor(color);
//    sector->Draw("f");
//    sector->Draw();
  }
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchModuleGem::DrawPads() {
  for (Int_t s=0;s<fSectors.size();s++){
    CbmMuchSector* sector = (CbmMuchSector*) fSectors[s];
    sector->DrawPads();
  }
}
// -------------------------------------------------------------------------


void CbmMuchModuleGem::SetPadFired(Int_t address,Int_t digiIndex,Int_t adcCharge){
  CbmMuchPad* pad = GetPad(address);
  if (pad) pad->SetFired(digiIndex,adcCharge);
}
ClassImp(CbmMuchModuleGem)
