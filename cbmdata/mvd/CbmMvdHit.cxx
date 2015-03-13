// -------------------------------------------------------------------------
// -----                       CbmMvdHit source file                   -----
// -----                  Created 07/11/06  by V. Friese               -----
// -----               Based on CbmStsMapsHit by M. Deveaux            -----
// -------------------------------------------------------------------------

#include "CbmMvdHit.h"
#include "CbmMvdCluster.h"

#include <iostream>

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmMvdHit::CbmMvdHit() 
 : CbmPixelHit(),
   fFlag(-1),
   fClusterIndex(-1),
   fIndexCentralX(-1),
   fIndexCentralY(-1),
   fTimeStamp(0),
   fTimeStampError(0),
   fDetectorID(-1)
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmMvdHit::CbmMvdHit(Int_t statNr, TVector3& pos, TVector3& dpos, Int_t indexCentralX, Int_t indexCentralY,
		     Int_t clusterIndex, Int_t flag) 
  : CbmPixelHit(0, pos, dpos, 0., -1),
    fFlag(flag),
    fClusterIndex(clusterIndex),
    fIndexCentralX(indexCentralX),
    fIndexCentralY(indexCentralY),
    fTimeStamp(0),
    fTimeStampError(0),
    fDetectorID(-1)
{
  fDetectorID = DetectorId(statNr);
}
// -------------------------------------------------------------------------

void CbmMvdHit::GetDigiIndexVector(TClonesArray* cbmMvdClusterArray, std::vector<Int_t>* digiIndexVector)
{
  CbmMvdCluster* cluster;
  
  if(!digiIndexVector){digiIndexVector=new std::vector<Int_t>;}
  if(digiIndexVector->size()!=0){digiIndexVector->clear();}
  
  Int_t indexLow=fClusterIndex;
  
  while (indexLow!=-1) {
    cluster=(CbmMvdCluster*) cbmMvdClusterArray->At(indexLow);
    indexLow=cluster->GetNeighbourDown();
  }
  
  Int_t* digiArray;
  Int_t digisInCluster;
  Int_t indexUp=0; 
  
  while (indexUp!=-1) {
    digiArray = cluster->GetDigiList();
    digisInCluster=cluster->GetTotalDigisInCluster();
    
    for (Int_t i=0;i<digisInCluster; i++){
      digiIndexVector->push_back(digiArray[i]);
    };
    
    indexUp=cluster->GetNeighbourUp();
    
  }
  
  if(cluster->GetTotalDigisInCluster()!=digiIndexVector->size()) {
    cout << "-W- " << GetName() << " - GetDigiIndexVector: Inconsistent number of digis in cluster. Ignored. " << endl;
  }
    
  
  
  
  
};

// -----   Destructor   ----------------------------------------------------
CbmMvdHit::~CbmMvdHit() {} 
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void CbmMvdHit::Print(const Option_t* opt) const {
  //cout.precision(10);
  cout << "MvdHit in station " << GetStationNr() << " at (" 
       << GetX() << ", "  << GetY() << ", " 
       << GetZ() << ") cm" << endl;
}
// -------------------------------------------------------------------------


ClassImp(CbmMvdHit)
