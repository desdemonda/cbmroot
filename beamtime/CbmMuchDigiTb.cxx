// TODO comment to be changed
/** CbmMuchDigiTb.cxx
 **@author M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 **@since 19.03.07
 **@version 1.0
 **
 ** Data class for digital MUCH information
 ** Data level: RAW
 **
 **
 **/
#include "CbmMuchDigiTb.h"
#include "CbmMuchAddress.h"
#include "CbmMuchDigiMatch.h"

#include <memory>

// -------------------------------------------------------------------------
const Int_t CbmMuchDigiTb::fgkCharBits = 12;
const Int_t CbmMuchDigiTb::fgkTimeBits = 14;
const Int_t CbmMuchDigiTb::fgkAddrBits = 32;
// -------------------------------------------------------------------------

// -----   Bit shift for charge and time stamp   ---------------------------
const Int_t CbmMuchDigiTb::fgkAddrShift = 0;
const Int_t CbmMuchDigiTb::fgkCharShift = CbmMuchDigiTb::fgkAddrShift + CbmMuchDigiTb::fgkAddrBits;
const Int_t CbmMuchDigiTb::fgkTimeShift = CbmMuchDigiTb::fgkCharShift + CbmMuchDigiTb::fgkCharBits;
// -------------------------------------------------------------------------

// -----   Bit masks for charge and time stamp   ---------------------------
const Long64_t CbmMuchDigiTb::fgkAddrMask = (1LL << CbmMuchDigiTb::fgkAddrBits) - 1;
const Long64_t CbmMuchDigiTb::fgkCharMask = (1LL << CbmMuchDigiTb::fgkCharBits) - 1;
const Long64_t CbmMuchDigiTb::fgkTimeMask = (1LL << CbmMuchDigiTb::fgkTimeBits) - 1;
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
CbmMuchDigiTb::CbmMuchDigiTb()
  : CbmDigi(),
    fData(0),
    fTime(0),
    fMatch(0)
{
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
CbmMuchDigiTb::CbmMuchDigiTb(Int_t address, Int_t charge, Int_t time)
  : CbmDigi(),
    fData(0),
    fTime(0),
    fMatch(0)
{
  fData |= (address & fgkAddrMask) << fgkAddrShift;
  fData |= (charge  & fgkCharMask) << fgkCharShift;
  fData |= (time    & fgkTimeMask) << fgkTimeShift;
}
// -------------------------------------------------------------------------

CbmMuchDigiTb::CbmMuchDigiTb(CbmMuchDigiTb* digi)
  : CbmDigi(*digi),
    fData(digi->fData),
    fTime(digi->fTime),
    fMatch(0)
{ 
}

CbmMuchDigiTb::CbmMuchDigiTb(CbmMuchDigiTb* digi,CbmMuchDigiMatch* match)
  : CbmDigi(*digi),
    fData(digi->fData),
    fTime(digi->fTime),
    fMatch(new CbmMuchDigiMatch(match))
{  
}


CbmMuchDigiTb::CbmMuchDigiTb(const CbmMuchDigiTb& rhs)
 : CbmDigi(rhs),
   fData(rhs.fData),
   fTime(rhs.fTime),
   fMatch(NULL)
{
  if (NULL != rhs.fMatch) {
     fMatch = new CbmMuchDigiMatch(*(rhs.fMatch));
   }
}

CbmMuchDigiTb& CbmMuchDigiTb::operator=(const CbmMuchDigiTb& rhs)
{

  if (this != &rhs) {
    CbmDigi::operator=(rhs);
    fData = rhs.fData;
    fTime = rhs.fTime;
    if (NULL != rhs.fMatch) {
      std::auto_ptr<CbmMuchDigiMatch> tmp(new CbmMuchDigiMatch(*rhs.fMatch));
      delete fMatch;
      fMatch = tmp.release();
    } else {
      fMatch = NULL;
    }

  }
  return *this;
}


// -----   Add charge   ----------------------------------------------------
void CbmMuchDigiTb::AddAdc(Int_t adc) {
  Int_t newAdc = GetAdc() + adc;
  if (newAdc > fgkCharMask) newAdc = fgkCharMask;
  SetAdc(newAdc);
}
// -------------------------------------------------------------------------


// -----   Set new charge   ------------------------------------------------
void CbmMuchDigiTb::SetAdc(Int_t adc) {
  // reset to 0
  fData &= ~(fgkCharMask << fgkCharShift);
  // set new value
  fData |=  (adc & fgkCharMask) << fgkCharShift;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchDigiTb::SetTime(Int_t time) {
  // reset to 0
  fData &= ~(fgkTimeMask << fgkTimeShift);
  // set new value
  fData |= (time    & fgkTimeMask) << fgkTimeShift; 
  fTime = time;
}
// -------------------------------------------------------------------------



ClassImp(CbmMuchDigiTb)
