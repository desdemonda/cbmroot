// ------------------------------------------------------------------
// -----                      CbmHadron.h                       -----
// -----            Created 2008-01-18 by D.Kresan              -----
// ------------------------------------------------------------------


#ifndef CBM_HADRON
#define CBM_HADRON


#include "TObject.h"


class CbmHadron : public TObject {

public:
    CbmHadron();
    CbmHadron(Int_t charge, Double_t p, Double_t pt, Double_t pz,
              Int_t nMvdHits, Int_t nStsHits, Int_t nTrdHits,
	      Double_t b, Double_t length, Double_t mass2,
	      Int_t pdg, Bool_t ghost, Bool_t tdh);
    virtual ~CbmHadron();

private:
    Int_t      fCharge;
    Double32_t fP;
    Double32_t fPt;
    Double32_t fPz;
    Int_t      fNMvdHits;
    Int_t      fNStsHits;
    Int_t      fNTrdHits;
    Double32_t fB;
    Double32_t fLength;
    Double32_t fMass2;
    Int_t      fPdg;
    Bool_t     fGhost;
    Bool_t     fTDH;

public:
    inline Int_t    GetCharge()   const {  return fCharge;    }
    inline Double_t GetP()        const {  return fP;         }
    inline Double_t GetPt()       const {  return fPt;        }
    inline Double_t GetPz()       const {  return fPz;        }
    inline Int_t    GetNMvdHits() const {  return fNMvdHits;  }
    inline Int_t    GetNStsHits() const {  return fNStsHits;  }
    inline Int_t    GetNTrdHits() const {  return fNTrdHits;  }
    inline Double_t GetB()        const {  return fB;         }
    inline Double_t GetLength()   const {  return fLength;    }
    inline Double_t GetMass2()    const {  return fMass2;     }
    inline Int_t    GetPdg()      const {  return fPdg;       }
    inline Bool_t   IsGhost()     const {  return fGhost;     }
    inline Bool_t   IsTDH()       const {  return fTDH;       }

    inline void SetCharge  (Int_t charge)        {  fCharge = charge;      }
    inline void SetP       (Double_t p)          {  fP = p;                }
    inline void SetPt      (Double_t pt)         {  fPt = pt;              }
    inline void SetPz      (Double_t pz)         {  fPz = pz;              }
    inline void SetNMvdHits(Int_t nMvdHits)      {  fNMvdHits = nMvdHits;  }
    inline void SetNStsHits(Int_t nStsHits)      {  fNStsHits = nStsHits;  }
    inline void SetNTrdHits(Int_t nTrdHits)      {  fNTrdHits = nTrdHits;  }
    inline void SetB       (Double_t b)          {  fB = b;                }
    inline void SetLength  (Double_t length)     {  fLength = length;      }
    inline void SetMass2   (Double_t mass2)      {  fMass2 = mass2;        }
    inline void SetPdg     (Int_t pdg)           {  fPdg = pdg;            }
    inline void SetGhost   (Bool_t ghost)        {  fGhost = ghost;        }
    inline void SetTDH     (Bool_t tdh)          {  fTDH = tdh;            }

    ClassDef(CbmHadron, 1);

};


#endif
