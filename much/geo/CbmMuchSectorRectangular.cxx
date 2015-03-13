#include "CbmMuchSectorRectangular.h"
#include "CbmMuchModuleGem.h"
#include "CbmMuchPad.h"
#include "TVector3.h"
#include "CbmMuchPadRectangular.h"

// -----   Default constructor   -------------------------------------------
CbmMuchSectorRectangular::CbmMuchSectorRectangular()
  : CbmMuchSector(),TPave(),
    fPosition(),
    fSize(),
    fPadNx(0),
    fPadNy(0),
    fPadDx(0),
    fPadDy(0),
    fNeighbours()
{
}
// -------------------------------------------------------------------------


// -----  Standard constructor  --------------------------------------------
CbmMuchSectorRectangular::CbmMuchSectorRectangular(UInt_t modAddress, UInt_t index, 
    TVector3 pos, TVector3 size, Int_t padNx, Int_t padNy)
  : CbmMuchSector(modAddress,index,padNx*padNy),
    TPave(pos[0]-size[0]/2,pos[1]-size[1]/2,pos[0]+size[0]/2,pos[1]+size[1]/2,1),
    fPosition(pos),
    fSize(size),
    fPadNx(padNx),
    fPadNy(padNy),
    fPadDx(fSize[0]/padNx),
    fPadDy(fSize[1]/padNy),
    fNeighbours()
{
}
// -------------------------------------------------------------------------

CbmMuchPadRectangular* CbmMuchSectorRectangular::GetPad(Double_t x, Double_t y){
  Int_t ix = Int_t((x-GetXmin())/fPadDx);
  Int_t iy = Int_t((y-GetYmin())/fPadDy);
  if (ix<0 || ix >=fPadNx) return NULL;
  if (iy<0 || iy >=fPadNy) return NULL;
  Int_t channelIndex = fPadNy*ix+iy;
  return (CbmMuchPadRectangular*) fPads[channelIndex];
}


// -------------------------------------------------------------------------
void CbmMuchSectorRectangular::AddPads(){
  for(Int_t ix = 0; ix < fPadNx; ix++){
    for(Int_t iy = 0; iy < fPadNy; iy++){
      Int_t channelIndex = fPadNy*ix+iy;
      UInt_t address = CbmMuchAddress::SetElementId(fAddress,kMuchChannel,channelIndex);
      Double_t x0 = GetXmin()+(ix+0.5)*fPadDx;
      Double_t y0 = GetYmin()+(iy+0.5)*fPadDy;
      CbmMuchPadRectangular* pad = new CbmMuchPadRectangular(address,x0,y0,fPadDx,fPadDy);
      fPads.push_back(pad);
    }
  }
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMuchSectorRectangular::DrawPads(){
  for(Int_t iChannel = 0; iChannel < fNChannels; iChannel++){
     CbmMuchPadRectangular* pad = (CbmMuchPadRectangular*) fPads[iChannel];
     if (!pad) return;
     pad->DrawPad();
  }
}
// -------------------------------------------------------------------------



ClassImp(CbmMuchSectorRectangular)
