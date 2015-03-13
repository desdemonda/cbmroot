/** CbmMuchStrawDigi.h
 **@author E.Kryshen <e.kryshen@gsi.de>
 **@since 23.07.2013
 **@version 1.0
 **/

#ifndef CBMMUCHSTRAWDIGI_H
#define CBMMUCHSTRAWDIGI_H 1

#include "CbmDigi.h"
#include "CbmDetectorList.h"
#include "CbmMuchDigiMatch.h"


class CbmMuchStrawDigi : public CbmDigi{
 public:
  CbmMuchStrawDigi();
  CbmMuchStrawDigi(UInt_t address, Float_t x, Float_t y, Float_t z, Double_t time);
  virtual ~CbmMuchStrawDigi(){}
  Int_t GetSystemId()  const { return kMUCH;    }
  Int_t GetAddress()  const { return fAddress; }
  Float_t GetX() const { return fX; }
  Float_t GetY() const { return fY; }
  Float_t GetZ() const { return fZ; }
  Double_t GetTime() const {return fTime; }
 private:
  Int_t fAddress;
  Float_t fX;
  Float_t fY;
  Float_t fZ;
  Double_t fTime;
  ClassDef(CbmMuchStrawDigi,1);
};

#endif
