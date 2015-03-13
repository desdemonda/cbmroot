// TODO comment to be changed
/** CbmMuchDigi.h
 **@author M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 **@since 19.03.07
 **@version 1.0
 **
 ** Data class for digital MUCH information
 ** Data level: RA
 **
 **
 **/


#ifndef CBMMUCHBEAMTIMEDIGI_H
#define CBMMUCHBEAMTIMEDIGI_H 1

#include "CbmDigi.h"
#include "CbmDetectorList.h"
#include "CbmMuchDigiMatch.h"
#include "CbmMuchAddress.h"

class CbmMuchBeamTimeDigi : public CbmDigi{
 public:
  CbmMuchBeamTimeDigi();
  CbmMuchBeamTimeDigi(Int_t address, Int_t charge=0, ULong_t time=0);
  CbmMuchBeamTimeDigi(CbmMuchBeamTimeDigi* digi);
  CbmMuchBeamTimeDigi(CbmMuchBeamTimeDigi* digi,CbmMuchDigiMatch* match);
  CbmMuchBeamTimeDigi(const CbmMuchBeamTimeDigi&);
  CbmMuchBeamTimeDigi& operator=(const CbmMuchBeamTimeDigi&);

  
  virtual ~CbmMuchBeamTimeDigi(){}
  Int_t GetSystemId() const { return kMUCH; }
  Int_t GetAddress()  const { return (fData >> fgkAddrShift) & fgkAddrMask; }
  Int_t GetAdc()      const { return (fData >> fgkCharShift) & fgkCharMask; }
  //Double_t GetTime()  const { return (fData >> fgkTimeShift) & fgkTimeMask; }
  virtual Double_t GetTime() const { return Double_t(fTime); }
  
  void AddAdc(Int_t adc);
  void SetAdc(Int_t adc);
  void SetTime(ULong_t time);
  
  void SetMatch(CbmMuchDigiMatch* match) { fMatch = match; }
  CbmMuchDigiMatch* GetMatch() const { return fMatch; }
 
  // Specially for littrack
  // TODO remove after littrack fix
  Int_t GetDetectorId() const { return CbmMuchAddress::GetElementAddress(GetAddress(),kMuchModule); }
  Int_t GetChannelId()  const { return GetAddress(); }
  Int_t GetADCCharge()  const { return GetAdc(); }
  Int_t GetDTime() const { return 0; }

  void SetPadX(Int_t padX) { fPadX = padX; }
  void SetPadY(Int_t padY) { fPadY = padY; }
  void SetRocId(Int_t rocId) { fRocId = rocId; }
  void SetNxId(Int_t nxId) { fNxId = nxId; }
  void SetNxCh(Int_t nxCh) { fNxCh = nxCh; }

  Int_t GetPadX() const {return fPadX; }
  Int_t GetPadY() const {return fPadY; }
  Int_t GetRocId() const {return fRocId; }
  Int_t GetNxId() const {return fNxId; }
  Int_t GetNxCh() const {return fNxCh; }



 
 private:
  Long64_t fData;
  ULong_t fTime;
  CbmMuchDigiMatch* fMatch;  ///< matches to MC points (to be replaced with Fair links)
  Int_t fPadX;
  Int_t fPadY;
  Int_t fRocId;
  Int_t fNxId;
  Int_t fNxCh;
  
  static const Int_t fgkAddrBits;
  static const Int_t fgkCharBits;
  static const Int_t fgkTimeBits;
  static const Int_t fgkAddrShift;
  static const Int_t fgkCharShift;
  static const Int_t fgkTimeShift;
  static const Long64_t fgkAddrMask;
  static const Long64_t fgkCharMask;
  static const Long64_t fgkTimeMask;

  ClassDef(CbmMuchBeamTimeDigi,1);
};
#endif
