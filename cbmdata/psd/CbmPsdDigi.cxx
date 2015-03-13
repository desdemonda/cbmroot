/** CbmPsdDigi.cxx
 **@author Alla Maevskaya <alla@inr.ru>
 **@since 3.08.20212
 **
 ** Modified to simplify fEdep[10][44] -> fEdep (S. Seddiki)
  **/


#include "CbmPsdDigi.h"
#include <iostream>
using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------

CbmPsdDigi::CbmPsdDigi() 
  : TObject(),
    fModuleID(-1),
    fSectionID(-1),
    fEdep(-1)                      // SELIM: simplification matrix [10][44] -> simple double
{
  /*
  for (Int_t i=0; i<10; i++)       // SELIM: simplification matrix [10][44] -> simple double
    for (Int_t j=0; j<44; j++)
      fEdep[i][j]=-1;
  */
}


CbmPsdDigi::CbmPsdDigi(Int_t section, Int_t module, Double_t edep)
  : TObject(),
    fModuleID(module),
    fSectionID(section),
    fEdep(edep)                     // SELIM: simplification matrix [10][44] -> simple double
{
  /*
  for (Int_t i=0; i<10; i++)        // SELIM: simplification matrix [10][44] -> simple double
    for (Int_t j=0; j<44; j++)
      fEdep[i][j] = edep;
      */
}



// -----   Destructor   ----------------------------------------------------
CbmPsdDigi::~CbmPsdDigi() { }
// -------------------------------------------------------------------------

void CbmPsdDigi::Print() {
  cout<<"module : "<<fModuleID <<" section    : "<<fSectionID <<
    " ELoss "<<fEdep <<  endl;


}


ClassImp(CbmPsdDigi)
