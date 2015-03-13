
/** CbmPsdEventData.cxx
 **@author Alla Maevskaya <alla@inr.ru> & SELIM
 **@since 3.08.20212
 **
 ** Modified to implement event plane method (S. Seddiki)
  **/


#include "CbmPsdEventData.h"
#include <iostream>
using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmPsdEventData::CbmPsdEventData() 
  : TNamed(),
    fevtID(0),
    fEP_RECO(0.),
    fdeltaEP(0.),
    fQx(0.),
    fQy(0.),
    fedep_centralmod_nohole(0.),
    fedep_centralmod_hole(0.),
    fedep_1stCorona(0.),
    fedep_2ndCorona(0.),
    fEP_sub1(0.),
    fdeltaEP_sub1(0.),
    fQx_sub1(0.),
    fQy_sub1(0.),
    fEP_sub2(0.),
    fdeltaEP_sub2(0.),
    fQx_sub2(0.),
    fQy_sub2(0.),
    fEP_sub3(0.),
    fdeltaEP_sub3(0.),
    fQx_sub3(0.),
    fQy_sub3(0.)
{
}

// -----   constructor   -------------------------------------------
CbmPsdEventData::CbmPsdEventData(const char *name) 
  : TNamed(),
    fevtID(0),
    fEP_RECO(0.),
    fdeltaEP(0.),
    fQx(0.),
    fQy(0.),
    fedep_centralmod_nohole(0.),
    fedep_centralmod_hole(0.),
    fedep_1stCorona(0.),
    fedep_2ndCorona(0.),
    fEP_sub1(0.),
    fdeltaEP_sub1(0.),
    fQx_sub1(0.),
    fQy_sub1(0.),
    fEP_sub2(0.),
    fdeltaEP_sub2(0.),
    fQx_sub2(0.),
    fQy_sub2(0.),
    fEP_sub3(0.),
    fdeltaEP_sub3(0.),
    fQx_sub3(0.),
    fQy_sub3(0.)
{
  TString namst = "RP_";
  namst += name;
  SetName(namst.Data());
  SetTitle(namst.Data());
}

/*
CbmPsdEventData::CbmPsdEventData(Float_t rp, Float_t b) 
  : TNamed(),
    fRP_MC(rp),
    fB_MC(b) 
{ 
 
}
*/

// -----   Destructor   ----------------------------------------------------
CbmPsdEventData::~CbmPsdEventData(){}


void CbmPsdEventData::Print() {  
  cout << "(RECO) EVENT PLANE : " << fEP_RECO << endl;
}

ClassImp(CbmPsdEventData)
