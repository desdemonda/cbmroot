// -------------------------------------------------------------------------
// -----                      CbmMvdCluster header file            -----
// -----
// -------------------------------------------------------------------------


/** CbmMvdCluster.h



**/


#ifndef CBMMVDCLUSTER_H
#define CBMMVDCLUSTER_H 1


#include "TVector3.h"
#include "TObject.h"
#include "CbmCluster.h"
#include "FairLogger.h"
#include <map>

using std::map;
using std::pair;

class CbmMvdCluster : public CbmCluster
{

 public:    

  /** Default constructor **/
  CbmMvdCluster();

  CbmMvdCluster(Int_t* digiList, Short_t digisInThisObject, Short_t totalDigisInCluster, Int_t neighbourDown); 


CbmMvdCluster(const CbmMvdCluster&);

CbmMvdCluster& operator=(const CbmMvdCluster&){return *this;};

  /** Destructor **/
  virtual ~CbmMvdCluster();
 
  /** Setters **/
  void SetNeighbourUp(Int_t index){fNeighbourUp=index;};
  void SetNeighbourDown(Int_t index){fNeighbourDown=index;};
  void SetPixelMap(map <pair<Int_t, Int_t>, Int_t > PixelMap); 
  void SetStationNr(Int_t stationNr){fStation = stationNr;};
  void SetRefId(Int_t RefId){fRefId = RefId;}; //* stores the index to the global TClonesArray	
  void SetDetectorId(Int_t detId)      { fDetectorId = detId;};
 

 /** Accessors **/
  Int_t   GetNeighbourDown(){return fNeighbourDown;};
  Int_t   GetNeighbourUp(){return fNeighbourUp;};
  Short_t GetDigisInThisObject(){return fDigisInThisObject;};
  Short_t GetTotalDigisInCluster(){return fPixelMap.size();};
  Short_t GetMaxDigisInThisObject(){return fMaxDigisInObject;};
  map <pair<Int_t, Int_t>, Int_t > GetPixelMap(){return fPixelMap;};
  Int_t    GetStationNr() {return fStation;};
  Int_t* GetDigiList(){return fDigiArray;};
  Int_t GetRefId(){return fRefId;};
  Int_t    GetDetectorId() {return fDetectorId;};

  Float_t GetClusterCharge(){return fClusterCharge;};
   
 protected:
     static const Short_t fMaxDigisInObject=8;
     Int_t fDigiArray[fMaxDigisInObject];
     map <pair<Int_t, Int_t>, Int_t > fPixelMap;
     Int_t fNeighbourDown;
     Int_t fNeighbourUp;
     Short_t fDigisInThisObject;
     Short_t fTotalDigisInCluster;
     Int_t fStation;
     Int_t fRefId;
     Int_t fDetectorId;
     Float_t fClusterCharge;

  ClassDef(CbmMvdCluster,2);

};



 /* CbmMvdCluster(Int_t dummyInt1, TVector3 dummyVect1, TVector3 dummyVect2, Int_t dummyInt2, Short_t* dummyShort1, Float_t dummyFloat1, Float_t dummyFloat2)
    :fDigiArray(),
   fPixelMap(),
   fNeighbourDown(-1),
   fNeighbourUp(-1),
   fDigisInThisObject(0),
   fTotalDigisInCluster(-1),
   fStation(-1),
   fRefId(-1),
   fDetectorId(-1),
   fClusterCharge(0)
  {LOG(FATAL) << "usage of invalid constructor, please check CbmMvdCluster.h" << FairLogger::endl;};
    Int_t GetDominatorX(){return 0;};
    Int_t GetDominatorY(){return 0;};
    Int_t GetTrackID(){return 0;};
    Int_t GetContributors(){return 0;};
     
    void SetDebuggingInfo(Short_t* foo1, Float_t foo2[5], Float_t foo3[5]){;};
    void SetContributors(Short_t short1){;};
    void PrintCluster(){;}; 
*/

#endif
