/** @file CbmTofDigiExp.cxx
 ** @author Pierre-Alain Loizeau <loizeau@physi.uni-heidelberg.de>
 ** @date 07.06.2013
 **
 ** Code for Data class for expanded digital TOF information
 **/
#include "CbmTofDigiExp.h"

// ROOT Headers
#include "TString.h"

CbmTofDigiExp::CbmTofDigiExp() 
  : CbmDigi(),
    fuAddress(0),
    fdTime(0.),
    fdTot(-1.)
{
}

CbmTofDigiExp::CbmTofDigiExp(
      UInt_t address, 
      Double_t time, 
      Double_t tot)
  : CbmDigi(),
    fuAddress(address),
    fdTime(time),
    fdTot(tot)
{
}

CbmTofDigiExp::CbmTofDigiExp( 
      UInt_t Sm, UInt_t Rpc, UInt_t Channel, 
      Double_t time, Double_t tot, 
      UInt_t Side, UInt_t SmType )
  : CbmDigi(),
    fuAddress(0),
    fdTime(time),
    fdTot(tot)
{
   fuAddress = CbmTofAddress::GetUniqueAddress( Sm, Rpc, Channel, Side, SmType );
}
CbmTofDigiExp::CbmTofDigiExp(const CbmTofDigiExp& digiIn)
  : CbmDigi(digiIn),
    fuAddress(digiIn.fuAddress),
    fdTime(digiIn.fdTime),
    fdTot(digiIn.fdTot)  
{
  /*
   fuAddress = digiIn.GetAddress(); ///< Unique channel address
   fdTime    = digiIn.GetTime();    ///< Absolute time [ps]
   fdTot     = digiIn.GetTot();     ///< Tot [ps]
  */
   SetMatch( digiIn.GetMatch() );
}

CbmTofDigiExp::~CbmTofDigiExp()
{
}

TString CbmTofDigiExp::ToString() const 
{
   TString string = Form( "CbmTrdDigi: address = 0x%08X time = %7f tot = %7f", 
                          fuAddress, fdTime, fdTot);
   return string;
}

Bool_t  CbmTofDigiExp::operator <( const CbmTofDigiExp& rhs) const
{
   return (this->GetTime() < rhs.GetTime()) ? kTRUE : kFALSE;
} 
Int_t	  CbmTofDigiExp::Compare(      const TObject* obj) const
{
   return Compare( (CbmTofDigiExp*)  obj);
}
Int_t	  CbmTofDigiExp::Compare(      const CbmTofDigiExp*  obj) const
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

ClassImp(CbmTofDigiExp) 
