// -------------------------------------------------------------------------
// -----                   CbmMuchPointInfo header file                -----
// -----                  Created 21/11/07  by E. Kryshen              -----
// -------------------------------------------------------------------------

#ifndef CBMMUCHPOINTINFO_H
#define CBMMUCHPOINTINFO_H

#include "TObject.h"

class CbmMuchPointInfo : public TObject{
 public:
   CbmMuchPointInfo();
   CbmMuchPointInfo(Int_t pdgCode, Int_t motherPdg, Double_t kine, Double_t length, Int_t stationId);
  ~CbmMuchPointInfo(){};

  void SetKinE(Double_t kine)      { fKine = kine;         }
  void SetPdgCode(Int_t pdgCode)   { fPdgCode = pdgCode;   }
  void SetMotherPdg(Int_t pdgCode) { fMotherPdg = pdgCode; }
  void SetCharge(Int_t charge)     { fCharge = charge;     }
  void SetStationId(Int_t id)      { fStationId = id;      }
  void SetLength(Double_t length)  { fLength = length;     }
  void AddCharge(Int_t charge)     { fCharge+= charge;     }
  void AddArea(Double_t s)         { fS+=s; fNPads ++;     }
  void Print();

  Double_t GetKine()      { return fKine;      }
  Double_t GetLength()    { return fLength;    }
  Int_t    GetPdgCode()   { return fPdgCode;   }
  Int_t    GetMotherPdg() { return fMotherPdg; }
  Int_t    GetCharge()    { return fCharge;    }
  Int_t    GetStationId() { return fStationId; }
  Double_t GetArea()      { return fS;         }
  Int_t    GetNPads()     { return fNPads;     } 
private:
  Double_t fKine;
  Double_t fLength;
  Int_t    fPdgCode;
  Int_t    fMotherPdg;
  Int_t    fCharge;
  Int_t    fStationId;
  Double_t fS;
  Int_t    fNPads;

  ClassDef(CbmMuchPointInfo,1)
};
#endif
