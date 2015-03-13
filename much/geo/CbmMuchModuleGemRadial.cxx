/** CbmMuchModuleGemRadial.cxx
 *@author  E.Kryshen <e.kryshen@gsi.de>
 *@version 1.0
 *@since   07.12.11
 *
 * This class holds geometry parameters of much radial-type modules 
 **/

#include "CbmMuchModuleGemRadial.h"
#include "CbmMuchSectorRadial.h"
#include <algorithm>
#include <vector>
#include "TArrayL64.h"
#include "CbmMuchPad.h"
#include "CbmMuchPadRadial.h"
#include "TMath.h"
using std::vector;

// -------------------------------------------------------------------------
CbmMuchModuleGemRadial::CbmMuchModuleGemRadial(): 
  CbmMuchModuleGem(),
  fSectorRadii(),
  fDx1(0.),
  fDx2(0.),
  fDy(0.),
  fDz(0.)
{
  fDetectorType = 3;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
CbmMuchModuleGemRadial::CbmMuchModuleGemRadial(Int_t iStation, Int_t iLayer, Bool_t iSide, Int_t iModule, 
  TVector3 pos, Double_t dx1, Double_t dx2, Double_t dy, Double_t dz, Double_t cutRadius):  
    CbmMuchModuleGem(iStation, iLayer, iSide, iModule, pos, TVector3(dx1+dx2,2*dy,2*dz), cutRadius),
    fSectorRadii(),
    fDx1(dx1),
    fDx2(dx2),
    fDy(dy),
    fDz(dz)
{
  fDetectorType = 3;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
CbmMuchSectorRadial* CbmMuchModuleGemRadial::GetSectorByRadius(Double_t r){
  vector<Double_t>::iterator i0,ie,i; 
  i0 = fSectorRadii.begin();
  ie = fSectorRadii.end();
  i = upper_bound(i0,ie,r);
  return i-i0-1>=0 ? (CbmMuchSectorRadial*) fSectors[i-i0-1] : NULL;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
CbmMuchPadRadial* CbmMuchModuleGemRadial::GetPad(Double_t x, Double_t y){
  TVector3 v = TVector3(x,y,0);
  CbmMuchSectorRadial* sector = GetSectorByRadius(v.Mag());
  return sector->GetPadByPhi(v.Phi());
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
Bool_t CbmMuchModuleGemRadial::InitModule(){
  for (Int_t s=0;s<GetNSectors();s++){
    CbmMuchSectorRadial* sector = (CbmMuchSectorRadial*) GetSectorByIndex(s);
    if (!sector) continue;
    fSectorRadii.push_back(sector->GetR1());
    sector->AddPads();
  }
  vector<CbmMuchPad*> neighbours;
  for (Int_t s=0;s<GetNSectors();s++){
    CbmMuchSectorRadial* sector = (CbmMuchSectorRadial*) GetSectorByIndex(s);
    if (!sector) continue;
    for (Int_t p=0;p<sector->GetNChannels();p++){
      CbmMuchPadRadial* pad = (CbmMuchPadRadial*) sector->GetPadByChannelIndex(p);
      if (!pad) continue;
      if (p>0)                        neighbours.push_back(sector->GetPadByChannelIndex(p-1));
      if (p<sector->GetNChannels()-1) neighbours.push_back(sector->GetPadByChannelIndex(p+1));
      CbmMuchSectorRadial* sec1 = s>0 ?               (CbmMuchSectorRadial*) GetSectorByIndex(s-1) : 0;
      CbmMuchSectorRadial* sec2 = s<GetNSectors()-1 ? (CbmMuchSectorRadial*) GetSectorByIndex(s+1) : 0;
      Double_t phi1 = pad->GetPhi1()-0.001;
      Double_t phi2 = pad->GetPhi2()+0.001;
      if (sec1) {
        CbmMuchPad* pad11 = sec1->GetPadByPhi(phi1);
        CbmMuchPad* pad12 = sec1->GetPadByPhi(phi2);
        Int_t iMin = (pad11) ? pad11->GetChannelIndex() : 0;
        Int_t iMax = (pad12) ? pad12->GetChannelIndex() : sec1->GetNChannels()-1;
        for (Int_t i=iMin;i<=iMax;i++) neighbours.push_back(sec1->GetPadByChannelIndex(i));
      }
      if (sec2) {
        CbmMuchPad* pad21 = sec2->GetPadByPhi(phi1);
        CbmMuchPad* pad22 = sec2->GetPadByPhi(phi2);
        Int_t iMin = (pad21) ? pad21->GetChannelIndex() : 0;
        Int_t iMax = (pad22) ? pad22->GetChannelIndex() : sec2->GetNChannels()-1;
        for (Int_t i=iMin;i<=iMax;i++) neighbours.push_back(sec2->GetPadByChannelIndex(i));
      }
      pad->SetNeighbours(neighbours);
      neighbours.clear();
    }
  }
  gLogger->Debug4(MESSAGE_ORIGIN,"Init module successful");
  return kTRUE;
}
// -------------------------------------------------------------------------

ClassImp(CbmMuchModuleGemRadial)
