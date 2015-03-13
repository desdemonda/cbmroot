// TODO comment to be changed
/** CbmMuchDigi.h
 **@author M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 **@since 19.03.07
 **@version 1.0
 **
 ** Data class for digital MUCH information
 ** Data level: RAW
 **
 **
 **/


#ifndef CBMMUCHDIGI_H
#define CBMMUCHDIGI_H 1

#include "CbmDigi.h"
#include "CbmDetectorList.h"
#include "CbmMuchDigiMatch.h"
#include "CbmMuchAddress.h"

class CbmMuchDigi : public CbmDigi{
 public:
  CbmMuchDigi();
  CbmMuchDigi(Int_t address, Int_t charge=0, Int_t time=0);
  CbmMuchDigi(CbmMuchDigi* digi);
  CbmMuchDigi(CbmMuchDigi* digi,CbmMuchDigiMatch* match);
  CbmMuchDigi(const CbmMuchDigi&);
  CbmMuchDigi& operator=(const CbmMuchDigi&);

  
  virtual ~CbmMuchDigi(){}
  Int_t GetSystemId() const { return kMUCH; }
  Int_t GetAddress()  const { return (fData >> fgkAddrShift) & fgkAddrMask; }
  Int_t GetAdc()      const { return (fData >> fgkCharShift) & fgkCharMask; }
  Double_t GetTime()  const { return (fData >> fgkTimeShift) & fgkTimeMask; }
  
  void AddAdc(Int_t adc);
  void SetAdc(Int_t adc);
  void SetTime(Int_t time);
  
  void SetMatch(CbmMuchDigiMatch* match) { fMatch = match; }
  CbmMuchDigiMatch* GetMatch() const { return fMatch; }
 
  // Specially for littrack
  // TODO remove after littrack fix
  Int_t GetDetectorId() const { return CbmMuchAddress::GetElementAddress(GetAddress(),kMuchModule); }
  Int_t GetChannelId()  const { return GetAddress(); }
  Int_t GetADCCharge()  const { return GetAdc(); }
  Int_t GetDTime() const { return 0; }
 
 private:
  Long64_t fData;
  CbmMuchDigiMatch* fMatch;  ///< matches to MC points (to be replaced with Fair links)
  
  static const Int_t fgkAddrBits;
  static const Int_t fgkCharBits;
  static const Int_t fgkTimeBits;
  static const Int_t fgkAddrShift;
  static const Int_t fgkCharShift;
  static const Int_t fgkTimeShift;
  static const Long64_t fgkAddrMask;
  static const Long64_t fgkCharMask;
  static const Long64_t fgkTimeMask;

  ClassDef(CbmMuchDigi,1);
};
#endif
