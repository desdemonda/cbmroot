/** CbmPsdHit.h
 **@author Alla Maevskaya <alla@inr.ru>
 **@since 23.10.2012
 **@version 1.0
 **
 ** Data class for PSD reconstruction
 ** Energy deposition per module
 **
 ** Modified to simplify fEdep[49] -> fEdep (S. Seddiki)
 **/


#ifndef CBMPSDHIT_H
#define CBMPSDHIT_H 1


#include "CbmDetectorList.h"
#include "TObject.h"

#include <vector>

class CbmPsdHit : public TObject
{

 public:

  /**   Default constructor   **/
  CbmPsdHit() ;
  
  CbmPsdHit(Int_t module, Double_t edep) ;
  
  
  /**   Destructor   **/
  virtual ~CbmPsdHit();
  
 
  
  /**   Setters - Getters   **/
  
  //Float_t GetEdep(Int_t module) const { return fEdep[module]; }      // SELIM: simplification vector [49] -> simple double
  //void SetEdep(Float_t edep, Int_t module) {fEdep[module]=edep;}

  Double_t GetEdep() const { return fEdep; }                           // SELIM: simplification vector [49] -> simple double
  void SetEdep(Double_t edep) { fEdep = edep; }
  
  Int_t GetModuleID() const { return fModuleID; }
  void SetModuleID(Int_t mod) {  fModuleID = mod; }
  
   void Print();
  
 private:


  /**   Data members  **/
 
  Int_t fModuleID;
  Double_t fEdep;//[49];    // SELIM: simplification vector [49] -> simple double

  
  ClassDef(CbmPsdHit,1);

};


#endif
