#include "CbmTrdAsic.h"

#include "CbmTrdPoint.h"
#include "CbmTrdAddress.h"
#include "FairLogger.h"

#include "TGeoManager.h"
#include "TMath.h"

#include <iomanip>

CbmTrdAsic::CbmTrdAsic() 
  : TNamed(),
    fAsicAddress(0),
    fX(0.),
    fY(0.),
    fZ(0.),
    fSizeX(0.),
    fSizeY(0.),
    fSizeZ(0.),
    fChannelAddresses(),
    fnofChannels(0),
    fFebGrouping(-1)
{
}

CbmTrdAsic::CbmTrdAsic(
		       Int_t address, Int_t nofChannels, Int_t FebGrouping, 
		       Double_t x, Double_t y, Double_t z,
		       Double_t sizex, Double_t sizey, Double_t sizez)
  : TNamed(),
    fAsicAddress(address),
    fX(x),
    fY(y),
    fZ(z),
    fSizeX(sizex),
    fSizeY(sizey),
    fSizeZ(sizez),
    fChannelAddresses(),
    fnofChannels(nofChannels),
    fFebGrouping(FebGrouping)
{

}

CbmTrdAsic::~CbmTrdAsic() 
{
}

void CbmTrdAsic::SetChannelAddresses(std::vector<Int_t> addresses){
  Int_t nofChannels = addresses.size();
  if (nofChannels != fnofChannels){
    LOG(WARNING) << "CbmTrdAsic::SetChannelAddresses previous nof channels:" << fnofChannels << " will be set to:" <<  nofChannels << FairLogger::endl;
    fnofChannels = nofChannels;
  }
  for (Int_t i = 0; i < nofChannels; i++)
    fChannelAddresses.push_back(addresses[i]);
  addresses.clear();
}

ClassImp(CbmTrdAsic)
