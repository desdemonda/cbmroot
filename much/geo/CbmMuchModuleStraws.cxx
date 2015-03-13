/** CbmMuchModuleStraws.cxx
 **/

#include "CbmMuchModuleStraws.h"
#include "CbmMuchModule.h"

// -----   Default constructor   -------------------------------------------
CbmMuchModuleStraws::CbmMuchModuleStraws(): CbmMuchModule() {
  fDetectorType = 2;
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
CbmMuchModuleStraws::CbmMuchModuleStraws(Int_t iStation, Int_t iLayer, Bool_t iSide,
    Int_t iModule, TVector3 position, TVector3 size, Double_t cutRadius)
:  CbmMuchModule(iStation, iLayer, iSide, iModule, position, size, cutRadius) {
  fDetectorType = 2;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMuchModuleStraws::~CbmMuchModuleStraws() {
}
// -------------------------------------------------------------------------

ClassImp(CbmMuchModuleStraws)
