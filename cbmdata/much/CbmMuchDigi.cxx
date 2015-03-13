// TODO comment to be changed
/** CbmMuchDigi.cxx
 **@author M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 **@since 19.03.07
 **@version 1.0
 **
 ** Data class for digital MUCH information
 ** Data level: RAW
 **
 **
 **/
#include "CbmMuchDigi.h"
#include "CbmMuchAddress.h"
#include "CbmMuchDigiMatch.h"

#include <memory>

// -------------------------------------------------------------------------
const Int_t CbmMuchDigi::fgkCharBits = 12;
const Int_t CbmMuchDigi::fgkTimeBits = 14;
const Int_t CbmMuchDigi::fgkAddrBits = 32;
// -------------------------------------------------------------------------

// -----   Bit shift for charge and time stamp   ---------------------------
const Int_t CbmMuchDigi::fgkAddrShift = 0;
const Int_t CbmMuchDigi::fgkCharShift = CbmMuchDigi::fgkAddrShift + CbmMuchDigi::fgkAddrBits;
const Int_t CbmMuchDigi::fgkTimeShift = CbmMuchDigi::fgkCharShift + CbmMuchDigi::fgkCharBits;
// -------------------------------------------------------------------------

// -----   Bit masks for charge and time stamp   ---------------------------
const Long64_t CbmMuchDigi::fgkAddrMask = (1LL << CbmMuchDigi::fgkAddrBits) - 1;
const Long64_t CbmMuchDigi::fgkCharMask = (1LL << CbmMuchDigi::fgkCharBits) - 1;
const Long64_t CbmMuchDigi::fgkTimeMask = (1LL << CbmMuchDigi::fgkTimeBits) - 1;
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
CbmMuchDigi::CbmMuchDigi() 
  : CbmDigi(),
    fData(0),
    fMatch(0)
{
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
CbmMuchDigi::CbmMuchDigi(Int_t address, Int_t charge, Int_t time)
  : CbmDigi(),
    fData(0),
    fMatch(0)
{
  fData |= (address & fgkAddrMask) << fgkAddrShift;
  fData |= (charge  & fgkCharMask) << fgkCharShift;
  fData |= (time    & fgkTimeMask) << fgkTimeShift;
}
// -------------------------------------------------------------------------

CbmMuchDigi::CbmMuchDigi(CbmMuchDigi* digi)
  : CbmDigi(*digi),
    fData(digi->fData),
    fMatch(0)
{ 
}

CbmMuchDigi::CbmMuchDigi(CbmMuchDigi* digi,CbmMuchDigiMatch* match)
  : CbmDigi(*digi),
    fData(digi->fData),
    fMatch(new CbmMuchDigiMatch(match))
{  
}


CbmMuchDigi::CbmMuchDigi(const CbmMuchDigi& rhs)
 : CbmDigi(rhs),
   fData(rhs.fData),
   fMatch(NULL)
{
  if (NULL != rhs.fMatch) {
     fMatch = new CbmMuchDigiMatch(*(rhs.fMatch));
   }
}

CbmMuchDigi& CbmMuchDigi::operator=(const CbmMuchDigi& rhs)
{

  if (this != &rhs) {
    CbmDigi::operator=(rhs);
    fData = rhs.fData;
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
void CbmMuchDigi::AddAdc(Int_t adc) {
  Int_t newAdc = GetAdc() + adc;
  if (newAdc > fgkCharMask) newAdc = fgkCharMask;
  SetAdc(newAdc);
}
// -------------------------------------------------------------------------


// -----   Set new charge   ------------------------------------------------
void CbmMuchDigi::SetAdc(Int_t adc) {
  // reset to 0
  fData &= ~(fgkCharMask << fgkCharShift);
  // set new value
  fData |=  (adc & fgkCharMask) << fgkCharShift;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchDigi::SetTime(Int_t time) { 
  // reset to 0
  fData &= ~(fgkTimeMask << fgkTimeShift);
  // set new value
  fData |= (time    & fgkTimeMask) << fgkTimeShift; 
}
// -------------------------------------------------------------------------



ClassImp(CbmMuchDigi)
