// TODO comment to be changed
/** CbmMuchDigiTb.h
 **@author M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 **@since 19.03.07
 **@version 1.0
 **
 ** Data class for digital MUCH information
 ** Data level: RAW
 **
 **
 **/


#ifndef CBMMUCHDIGITB_H
#define CBMMUCHDIGITB_H 1

#include "CbmDigi.h"
#include "CbmDetectorList.h"
#include "CbmMuchDigiMatch.h"
#include "CbmMuchAddress.h"

class CbmMuchDigiTb : public CbmDigi{
 public:
  CbmMuchDigiTb();
  CbmMuchDigiTb(Int_t address, Int_t charge=0, Int_t time=0);
  CbmMuchDigiTb(CbmMuchDigiTb* digi);
  CbmMuchDigiTb(CbmMuchDigiTb* digi,CbmMuchDigiMatch* match);
  CbmMuchDigiTb(const CbmMuchDigiTb&);
  CbmMuchDigiTb& operator=(const CbmMuchDigiTb&);

  
  virtual ~CbmMuchDigiTb(){}
  Int_t GetSystemId() const { return kMUCH; }
  Int_t GetAddress()  const { return (fData >> fgkAddrShift) & fgkAddrMask; }
  Int_t GetAdc()      const { return (fData >> fgkCharShift) & fgkCharMask; }
  //Double_t GetTime()  const { return (fData >> fgkTimeShift) & fgkTimeMask; }
  virtual Double_t GetTime() const { return fTime; }

  
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
  ULong64_t fTime;

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

  ClassDef(CbmMuchDigiTb,1);
};
#endif
