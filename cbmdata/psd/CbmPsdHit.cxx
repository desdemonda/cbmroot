
/** CbmPsdHit.cxx
 **@author Alla Maevskaya <alla@inr.ru>
 **@since 3.08.20212
 **
 ** Modified to simplify fEdep[49] -> fEdep (S. Seddiki)
  **/


#include "CbmPsdHit.h"
#include <iostream>
using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmPsdHit::CbmPsdHit() 
  : TObject(),
    fModuleID(-1),
    fEdep(-1)                        // SELIM: simplification vector [49] -> simple double
{
  
    //for (Int_t j=0; j<49; j++)     // SELIM: simplification vector [49] -> simple double
    //  fEdep[j]=0;
  
}
CbmPsdHit::CbmPsdHit(Int_t module, Double_t edep)
  : TObject(),
    fModuleID(module),
    fEdep(edep)                      // SELIM: simplification vector [49] -> simple double
{ 
    //for (Int_t j=0; j<49; j++)     // SELIM: simplification vector [49] -> simple double
      //fEdep[j] = edep;
  
}



// -----   Destructor   ----------------------------------------------------
CbmPsdHit::~CbmPsdHit() { }
// -------------------------------------------------------------------------

void CbmPsdHit::Print() {
  cout<<"module : "<<fModuleID <<" ELoss "<<fEdep <<  endl;

}

ClassImp(CbmPsdHit)
