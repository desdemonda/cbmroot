/** @file CbmTofDigi.cxx
 ** @author Pierre-Alain Loizeau <loizeau@physi.uni-heidelberg.de>
 ** @date 07.06.2013
 **
 ** Code for Data class for compressed digital TOF information
 **/
#include "CbmTofDigi.h"

// ROOT Headers
#include "TString.h"

// It seems C++ standard force the initialization to be in cxx/cpp file (outside of class definition)
// When not trivial constant values => To check if it should apply also to simple values maybe?     

const Double_t CbmTofDigi::fgkdTimeBinSize =  10.0; // [ps] => 41 943.40 ns ( > GET4 e
const Double_t CbmTofDigi::fgkdTotBinSize  =  50.0; // [ps] => 0-51.200 range

// -----   Bit masks -----------------------------------------------------------
const Long64_t CbmTofDigi::fgklAddrMask = ((Long64_t)1 << CbmTofDigi::fgkiAddressSize) - 1;
const Long64_t CbmTofDigi::fgklTimeMask = ((Long64_t)1 << CbmTofDigi::fgkiTimeSize)    - 1;
const Long64_t CbmTofDigi::fgklTotMask  = ((Long64_t)1 << CbmTofDigi::fgkiTotSize)     - 1;
// -----------------------------------------------------------------------------

// -----   Bit shifts   --------------------------------------------------------
const Int_t    CbmTofDigi::fgkiAddressOffs =   0;
const Int_t    CbmTofDigi::fgkiTimeOffs    =   CbmTofDigi::fgkiAddressOffs + CbmTofDigi::fgkiAddressSize;
const Int_t    CbmTofDigi::fgkiTotOffs     =   CbmTofDigi::fgkiTimeOffs    + CbmTofDigi::fgkiTotSize;
// -----------------------------------------------------------------------------

CbmTofDigi::CbmTofDigi() 
  : CbmDigi(),
    flData(0)
{
}

CbmTofDigi::CbmTofDigi(
      UInt_t address, 
      Double_t time, 
      Double_t tot)
  : CbmDigi(),
    flData(0)
{
   SetAddress( address );
   SetTime( time);
   SetTot(  tot);
}

CbmTofDigi::CbmTofDigi( 
      UInt_t Sm, UInt_t Rpc, UInt_t Channel, 
      Double_t time, Double_t tot, 
      UInt_t Side, UInt_t SmType )
  : CbmDigi(),
    flData(0)
{
   SetAddress( Sm, Rpc, Channel, Side, SmType );
   SetTime( time);
   SetTot(  tot);
}

CbmTofDigi::CbmTofDigi(const CbmTofDigi& digiIn)
  : CbmDigi(digiIn),
    flData(digiIn.flData)
{
  //   flData = digiIn.GetData(); ///< Fully compressed data
}

CbmTofDigi::~CbmTofDigi()
{
}

void CbmTofDigi::SetAddress( UInt_t address) 
{ 
   // First set old address to zero
   flData = flData & ~(fgklAddrMask << fgkiAddressOffs);

   // Now set new address
   flData = flData | ( (address & fgklAddrMask) << fgkiAddressOffs );
};
void CbmTofDigi::SetAddress( UInt_t Sm, UInt_t Rpc, UInt_t Channel,
                  UInt_t Side, UInt_t SmType )
{ 
   Int_t iAddress = CbmTofAddress::GetUniqueAddress( Sm, Rpc, Channel, Side, SmType );
   
   // First set old address to zero
   flData = flData & ~(fgklAddrMask << fgkiAddressOffs);

   // Now set new address
   flData = flData | ( (iAddress & fgklAddrMask) << fgkiAddressOffs );
}
void CbmTofDigi::SetTime(Double_t time) 
{ 
   Int_t iBinnedTime = (Int_t)( time / fgkdTimeBinSize );
   // First set old time to zero
   flData = flData & ~(fgklTimeMask << fgkiTimeOffs);

   // Now set new time
   flData = flData | ( (iBinnedTime & fgklTimeMask) << fgkiTimeOffs );
};
void CbmTofDigi::SetTot(Double_t tot) 
{ 
   Int_t iBinnedTot = (Int_t)( tot / fgkdTotBinSize );
   
   // First set old tot to zero
   flData = flData & ~(fgklTotMask << fgkiTotOffs);

   // Now set new tot
   flData = flData | ( (iBinnedTot & fgklTotMask) << fgkiTotOffs );
};
                     
TString CbmTofDigi::ToString() const 
{
   TString string = Form( "CbmTrdDigi: address = 0x%08X time = %7f tot = %7f", 
                          GetAddress(), GetCharge(), GetTot() );
   return string;
}

Bool_t  CbmTofDigi::operator <( const CbmTofDigi& rhs) const
{
   return (this->GetTime() < rhs.GetTime()) ? kTRUE : kFALSE;
} 
Int_t	  CbmTofDigi::Compare(      const TObject* obj) const
{
   return Compare( (CbmTofDigi*)  obj);
}
Int_t	  CbmTofDigi::Compare(      const CbmTofDigi*  obj) const
{
   if( this->GetTime() < obj->GetTime() )
      // hit ... obj
      return -1;
   else if( this->GetTime() > obj->GetTime() )
      // obj ... hit
      return 1;
   // obj = hit
   else return 0;
}

ClassImp(CbmTofDigi)  
