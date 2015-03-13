/** CbmMuchLayer.cxx
 *@author  M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@version 1.0
 *@since   11.02.08
 **
 ** This class holds the transport geometry parameters
 ** of one MuCh tracking layer.
 **/

#include "CbmMuchLayer.h"
#include "CbmMuchLayerSide.h"
#include "CbmMuchModule.h"
#include "CbmMuchAddress.h"

// -----   Default constructor   -------------------------------------------
CbmMuchLayer::CbmMuchLayer() 
  : TObject(),
    fDetectorId(0),
    fZ(0.),
    fSideF(),
    fSideB(),
    fSupportDx(0.),
    fSupportDy(0.),
    fSupportDz(0.),
    fZtoStationCenter()
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchLayer::CbmMuchLayer(Int_t detId, Double_t z, Double_t zRel)
  : TObject(),
    fDetectorId(detId),
    fZ(z),
    fSideF(),
    fSideB(),
    fSupportDx(0.),
    fSupportDy(0.),
    fSupportDz(0.),
    fZtoStationCenter(zRel)
{
  Int_t iStation = CbmMuchAddress::GetStationIndex(detId);
  Int_t iLayer = CbmMuchAddress::GetLayerIndex(detId);
  fSideF=CbmMuchLayerSide(iStation,iLayer,0,z);
  fSideB=CbmMuchLayerSide(iStation,iLayer,1,z);
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchLayer::CbmMuchLayer(Int_t iStation, Int_t iLayer, Double_t z, Double_t zRel)  
  : TObject(),
    fDetectorId(CbmMuchAddress::GetAddress(iStation, iLayer)),
    fZ(z),
    fSideF(),
    fSideB(),
    fSupportDx(0.),
    fSupportDy(0.),
    fSupportDz(0.),
    fZtoStationCenter(zRel)

{
  fSideF=CbmMuchLayerSide(iStation,iLayer,0,z);
  fSideB=CbmMuchLayerSide(iStation,iLayer,1,z);
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMuchLayer::~CbmMuchLayer() {
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
Double_t CbmMuchLayer::GetDz() {
  Double_t dzmax=fSupportDz;

  for (Int_t s=0; s<2; s++){
    CbmMuchLayerSide* side = GetSide(s);
    for (Int_t i=0; i<side->GetNModules(); i++) {
      CbmMuchModule* module = side->GetModule(i);
      Double_t lz = module->GetSize().Z();
      Double_t z  = module->GetPosition().Z();
      Double_t dz = TMath::Abs(z-fZ) + lz/2.;
      if (dz>dzmax) dzmax = dz;
    }
  }
  return dzmax;
}
// -------------------------------------------------------------------------

ClassImp(CbmMuchLayer)
