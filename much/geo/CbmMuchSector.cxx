#include "CbmMuchSector.h"
#include "CbmMuchPad.h"
#include <vector>

using std::vector;

// -----   Default constructor   -------------------------------------------
CbmMuchSector::CbmMuchSector()
  : TObject(),
    fAddress(0),
    fNChannels(0),
    fPads()
{
}
// -------------------------------------------------------------------------

// -----  Standard constructor  --------------------------------------------
CbmMuchSector::CbmMuchSector(UInt_t modAddress, UInt_t index, Int_t nChannels)
  : TObject(),  
    fAddress(CbmMuchAddress::SetElementId(modAddress,kMuchSector,index)),
    fNChannels(nChannels),
    fPads()
{
  fPads.resize(nChannels);
}
// -------------------------------------------------------------------------

CbmMuchPad* CbmMuchSector::GetPadByChannelIndex(Int_t iChannel) const { 
//  gLogger->Debug(MESSAGE_ORIGIN,"iChannel=%i fPads.size()=%i fNChannels=%i",iChannel,fPads.size(),fNChannels);
  if (iChannel>=fPads.size() || iChannel<0) {
    gLogger->Error(MESSAGE_ORIGIN,"iChannel=%i fPads.size()=%i",iChannel,fPads.size());
    gLogger->Error(MESSAGE_ORIGIN,"  station index=%i",CbmMuchAddress::GetStationIndex(fAddress));
    gLogger->Error(MESSAGE_ORIGIN,"  layer index=%i",CbmMuchAddress::GetLayerIndex(fAddress));
    gLogger->Error(MESSAGE_ORIGIN,"  module index=%i",CbmMuchAddress::GetModuleIndex(fAddress));
    gLogger->Error(MESSAGE_ORIGIN,"  sector index=%i",CbmMuchAddress::GetSectorIndex(fAddress));
    return NULL;
  }
  return fPads[iChannel]; 
}



//// -------------------------------------------------------------------------
//void CbmMuchSector::DrawPads(){
//  for(Int_t iChannel = 0; iChannel < fNChannels; iChannel++){
//     CbmMuchPad* pad = fPads[iChannel];
////     pad->Draw("f");
////     pad->Draw();
//  }
//}
//// -------------------------------------------------------------------------
ClassImp(CbmMuchSector)
