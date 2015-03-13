/** CbmMuchModule.cxx
 *@author  M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@version 1.0
 *@since   11.02.08
 **
 ** This class holds the transport geometry parameters
 ** of one side of MuCh module.
 **/

#include "CbmMuchModule.h"
#include "CbmMuchSector.h"
#include "CbmMuchPad.h"
#include "CbmMuchGeoScheme.h"
#include "CbmMuchAddress.h"

#include "TMath.h"
#include "TPolyLine.h"
#include "TPave.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <cassert>

using std::cout;
using std::endl;
using std::vector;

// -----   Default constructor   -------------------------------------------
CbmMuchModule::CbmMuchModule() 
  : TPave(), 
    fDetectorId(0),
    fDetectorType(-1),
    fCutRadius(0),
    fSize(TVector3()),
    fPosition(TVector3()),
    fPoints(NULL),
    fHits(NULL),
    fClusters(NULL),
    fDigis()
{
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
CbmMuchModule::CbmMuchModule(Int_t iStation, Int_t iLayer, Bool_t iSide,
        Int_t iModule, TVector3 position, TVector3 size, Double_t cutRadius)
  : TPave(position[0] - size[0] / 2, position[1] - size[1] / 2, position[0]
	  + size[0] / 2, position[1] + size[1] / 2, 1),
    fDetectorId(CbmMuchAddress::GetAddress(iStation,iLayer, iSide,iModule)),
    fDetectorType(-1),
    fCutRadius(cutRadius),
    fSize(size),
    fPosition(position),
    fPoints(NULL),
    fHits(NULL),
    fClusters(NULL),
    fDigis()
{
}
// -------------------------------------------------------------------------


ClassImp(CbmMuchModule)
