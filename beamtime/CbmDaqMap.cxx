/**@file CbmDaqMap.cxx
 **@date 11.12.2013
 **@author V. Friese <v.friese@gsi.de>
 **/

#include "CbmDaqMap.h"

// -----   Default Constructor  ---------------------------------------------- 
CbmDaqMap::CbmDaqMap() 
  : TObject(),
    fRun(-1)
{
}
// ---------------------------------------------------------------------------




// -----   Standard Constructor  --------------------------------------------- 
CbmDaqMap::CbmDaqMap(Int_t iRun) 
  : TObject(),
    fRun(iRun)
{
}
// ---------------------------------------------------------------------------



// -----   Destructor   ------------------------------------------------------
CbmDaqMap::~CbmDaqMap() 
{
}
// ---------------------------------------------------------------------------

ClassImp(CbmDaqMap)

