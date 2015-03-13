#ifndef CBML1TRDTRACKLET_H
#define CBML1TRDTRACKLET_H 1

#include "TObject.h"
#include <vector>

class CbmL1TrdTracklet : public TObject
{

 public:

  /** Default constructor **/
  CbmL1TrdTracklet();

  /** Destructor **/
  virtual ~CbmL1TrdTracklet();

  inline Int_t    GetIndLeft()        { return fIndLeft;   };
  inline Int_t    GetIndRight()       { return fIndRight;  };
  inline Int_t    GetVal()            { return fVal;       }; 
  inline Int_t    GetIsAlone()        { return fIsAlone;   }; 
  inline Int_t    GetIndex()          { return fIndex;     };
  inline Double_t GetCoord(Int_t ind) { return fCoord[ind]; };
  inline Int_t    GetPlanesID(Int_t num) { return fID[num]; };

  inline void SetIndLeft(Int_t indLeft)   { fIndLeft = indLeft;   };
  inline void SetIndRight(Int_t indRight) { fIndRight = indRight; };
  inline void SetVal(Int_t segVal)        { fVal = segVal;        }; 
  inline void SetIsAlone(Bool_t isAlone)  { fIsAlone = isAlone;   }; 
  inline void SetIndex(Int_t index)       { fIndex = index;       }; 
  inline void SetCoord(Int_t ind, Double_t val) { fCoord[ind] = val;};
  inline void SetPlanesID(Int_t A_ID, Int_t B_ID) {
    fID[0] = A_ID;
    fID[1] = B_ID;    
  };
  

  std::vector<Int_t> vAccostTracklet;

  //compares the first or second two coordinates of two tracklets
  inline static Bool_t Compare1(CbmL1TrdTracklet *tr1, 
				CbmL1TrdTracklet *tr2) {
    return (tr1->GetCoord(0) > tr2->GetCoord(0)); 
  };
  
  inline static Bool_t Compare2(CbmL1TrdTracklet *tr1, 
				CbmL1TrdTracklet *tr2) {
    return (tr1->GetCoord(1) > tr2->GetCoord(1)); 
  };

 private:

  Int_t fID[2];
  Int_t fIndLeft;
  Int_t fIndRight;
  Int_t fVal;
  Int_t fY;
  Bool_t fIsAlone;
  Int_t  fIndex;
  Double_t fCoord[4];

  ClassDef(CbmL1TrdTracklet,1);

};

#endif
