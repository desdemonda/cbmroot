#ifndef CBML1TRDTRACKLET4_H
#define CBML1TRDTRACKLET4_H 1

#include "TObject.h"
#include <vector>

class CbmL1TrdTracklet4 : public TObject
{

 public:

  /** Default constructor **/
  CbmL1TrdTracklet4();

  //CbmL1TrdTracklet(Int_t indLeft, Int_t indRight) {fIndLeft=indLeft; fIndRight=indRight;};

  /** Destructor **/
  virtual ~CbmL1TrdTracklet4();

  //get a given hit hit index
  inline  Int_t GetInd(Int_t i)    { return fInd[i]; };

  //gets the appropriate coordinate values
  inline Double_t GetCoord(Int_t ind) { return fCoord[ind]; };

  //value of the tracklet used in tagging
  inline Int_t GetVal()     { return fVal;     }; 
  
  //says if this tracklet has any left-side neighbours
  inline Int_t GetIsAlone() { return fIsAlone; }; 

  inline Int_t GetIndex()   { return fIndex;   };


  //sets the index of a given hit
  inline  void SetInd(Int_t ind, Int_t val)      { fInd[ind] = val; };

  //sets the appropriate coordinate values
  inline  void SetCoord(Int_t ind, Double_t val) { fCoord[ind] = val;};

  //set value of the tracklet used in tagging
  inline  void SetVal(Int_t segVal)        { fVal = segVal;      }; 

  //sets flag if this tracklet has any left-side neighbours
  inline  void SetIsAlone(Bool_t isAlone)  { fIsAlone = isAlone; }; 

  inline  void SetIndex(Int_t index)       { fIndex = index;     }; 
  
  /** Get extrapolated value of the precise coordinate of the 1st and 2nd layer **/
  inline Double_t GetExt(Int_t ind) { return fExt[ind]; };

  /** Set extrapolated value of the precise coordinate of the 1st and 2nd layer **/
  //ind == 0 1st layer
  //ind == 1 2nd layer
  inline void SetExt(Int_t ind, Double_t val) { fExt[ind] = val; };

  std::vector<Int_t> vAccostTracklet; //position of next tracklets in vector 
  std::vector<Int_t> vAccostLeft;     //position of next tracklets in vector 
  std::vector<Int_t> vAccostRight;    //position of next tracklets in vector 

  //compares two first (or two second) coordinates of two tracklets
  inline static Bool_t compare1(CbmL1TrdTracklet4 *tr1, 
				CbmL1TrdTracklet4 *tr2) {
    return (tr1->GetCoord(0) < tr2->GetCoord(0)); 
  };
  
  inline static Bool_t compare2(CbmL1TrdTracklet4 *tr1, 
				CbmL1TrdTracklet4 *tr2) {
    return (tr1->GetCoord(1) < tr2->GetCoord(1)); 
  };
  
  inline static Bool_t Compare3S(CbmL1TrdTracklet4 *tr1, 
				 CbmL1TrdTracklet4 *tr2) {
    return (tr1->GetVal() > tr2->GetVal()); 
  };

  //to store 4 Z value 
  Double_t M[4];

 private:
  


  //indexes of the four consistuent hits
  Int_t fInd[4];

  //tracklet tag value
  Int_t fVal;

  
  Int_t fY;

  //says if a tracklet has any left-sede neighbours
  Bool_t fIsAlone;
  Int_t fIndex;

  Double_t fExt1; //coordinates extrapolated to the next station (X or Y)
  Double_t fExt2;
  Double_t fExt[2];

  Double_t fCoord[4];

  ClassDef(CbmL1TrdTracklet4,1);

};

#endif
