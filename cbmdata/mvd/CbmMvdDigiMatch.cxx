// -------------------------------------------------------------------------
// -----               CbmMvdDigiMatch source file                     -----
// -----              Created 12/05/11  by M. Deveaux                  -----
// -----           Update to new CbmMatch Class by P. Sitzmann         -----
// -------------------------------------------------------------------------

#include <iostream>
#include "CbmMvdDigiMatch.h"
#include "TMath.h"

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmMvdDigiMatch::CbmMvdDigiMatch():
 fFileNumber(-1),
 fIndex(0),
 fWeight(0.),
 fEntry(-1)
{

 AddLink(0., 0, -1, -1);	
}
// -------------------------------------------------------------------------


// -----   Constructor with parameters   -----------------------------------
CbmMvdDigiMatch::CbmMvdDigiMatch(Double_t weight, Int_t index, Int_t entry, Int_t file):
 fFileNumber(file),
 fIndex(index),
 fWeight(weight),
 fEntry(entry)
{    
AddLink(weight, index, entry, file);
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmMvdDigiMatch::~CbmMvdDigiMatch(){}
// -------------------------------------------------------------------------

ClassImp(CbmMvdDigiMatch)
