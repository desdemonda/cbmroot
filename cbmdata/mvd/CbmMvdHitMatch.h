// -------------------------------------------------------------------------
// -----                    CbmMvdHitMatch source file                 -----
// -----                  Created 07/11/06  by V. Friese               -----
// -----            Based on CbmStsMapsHitInfo by M. Deveaux           -----
// -----           Update to new CbmMatch Class by P. Sitzmann         -----
// -------------------------------------------------------------------------


#ifndef CBMMVDHITMATCH_H
#define CBMMVDHITMATCH_H 1


#include "TObject.h"

#include "CbmMatch.h"

#include <iostream>


class CbmMvdHitMatch: public CbmMatch
{

 public:

  /** Default constructor **/
  CbmMvdHitMatch();


  /** Constructor with all parameters **/
  CbmMvdHitMatch(Double_t weight, Int_t index, Int_t entry = -1, Int_t file = -1);

  CbmMvdHitMatch(Int_t par1, Int_t par2, Int_t par3, Int_t par4, Int_t par5) 
   : CbmMatch(),
     fFileNumber(-1),
     fIndex(-1),
     fWeight(-1.),
     fEntry(-1)
   { ; } //quick solution for error in CbmMvdHitProducer
   


  /** Destructor **/
  virtual ~CbmMvdHitMatch();  


  /** Accessors **/
  Int_t  GetFileNumber()  const { return fFileNumber;  }
  Int_t  GetIndexNumber() const { return fIndex; }
  Int_t  GetEntryNumber() const { return fEntry; }
  Float_t GetWeight()     const { return fWeight; }

  Int_t GetPointId() const { return 0; }
  Int_t GetTrackId() const { return 0; }
  Int_t GetNMerged() const { return 0; }  
  void AddMerged() { ; }
 
  

  /** Reset data members **/
  void Clear() { ; }

 private:

  Int_t fFileNumber;
  Int_t fIndex; 
  Double_t fWeight;
  Int_t fEntry;

  ClassDef(CbmMvdHitMatch,1);

};


#endif
 
