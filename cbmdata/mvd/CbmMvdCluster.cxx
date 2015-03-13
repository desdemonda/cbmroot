// -------------------------------------------------------------------------
// -----                CbmMvdCluster source file                  -----

// -------------------------------------------------------------------------

#include "CbmMvdCluster.h"
#include "TVector3.h"

#include <iostream>

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmMvdCluster::CbmMvdCluster() 
 : CbmCluster(),
   fDigiArray(),
   fNeighbourDown(-1),
   fNeighbourUp(-1),
   fDigisInThisObject(0),
   fTotalDigisInCluster(-1),
   fPixelMap(),
   fStation(-1),
   fRefId(-1),
   fDetectorId(-1),
   fClusterCharge(0)
{
  for(Int_t i=0;i<fMaxDigisInObject; i++){fDigiArray[i]=-1;}
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMvdCluster::CbmMvdCluster(Int_t* digiList, Short_t digisInThisObject, Short_t totalDigisInCluster, Int_t neighbourDown) 
 : CbmCluster(),
   fDigiArray(),
   fNeighbourDown(-1),
   fNeighbourUp(-1),
   fDigisInThisObject(0),
   fTotalDigisInCluster(-1),
   fPixelMap(),
   fStation(-1),
   fRefId(-1),
   fDetectorId(-1),
   fClusterCharge(0)
{
    for(Int_t i=0;i<fMaxDigisInObject; i++){fDigiArray[i]=-1;}
    for(Int_t i=0;i<digisInThisObject; i++){fDigiArray[i]=digiList[i];}   
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
CbmMvdCluster::CbmMvdCluster(const CbmMvdCluster& rhs)
 : CbmCluster(),
   fDigiArray(),
   fNeighbourDown(-1),
   fNeighbourUp(-1),
   fDigisInThisObject(0),
   fTotalDigisInCluster(-1),
   fPixelMap(),
   fStation(-1),
   fRefId(-1),
   fDetectorId(-1),
   fClusterCharge(0) 
{ 
	fPixelMap = rhs.fPixelMap;
        fNeighbourDown = rhs.fNeighbourDown;
        fNeighbourUp = rhs.fNeighbourUp;
        fDigisInThisObject = rhs.fDigisInThisObject;
        fTotalDigisInCluster = rhs.fTotalDigisInCluster;
        fStation = rhs.fStation;
        fRefId = rhs.fRefId;
        fDetectorId = rhs.fDetectorId;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMvdCluster::~CbmMvdCluster() {}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMvdCluster::SetPixelMap(map <pair<Int_t, Int_t>, Int_t > PixelMap)
{
fPixelMap = PixelMap;
for(map<pair<Int_t, Int_t>, Int_t>::iterator iter = fPixelMap.begin(); iter != fPixelMap.end(); iter++)
	fClusterCharge += iter->second;
}

ClassImp(CbmMvdCluster)
