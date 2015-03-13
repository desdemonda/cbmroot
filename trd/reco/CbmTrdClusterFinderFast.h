// -------------------------------------------------------------------------
// -----          CbmTrdClusterFinderFast header file                  -----
// -----   Created 27/07/10  by Lorenz Quack and Cyrano Bergmann       -----
// -------------------------------------------------------------------------


#ifndef CBMTRDCLUSTERFINDERFAST_H
#define CBMTRDCLUSTERFINDERFAST_H 

#include "FairTask.h"

#include <vector>
#include <list>
#include <map>

class CbmTrdDigiPar;
class CbmTrdModule;
class CbmTrdGeoHandler;
class TClonesArray;

typedef struct MyDigi
{
  Int_t digiId;
  Int_t rowId;
  Int_t colId;
  Int_t combiId;
  Float_t charge;
} MyDigi;

typedef std::list<MyDigi*> MyDigiList;
typedef std::vector<MyDigiList*> ClusterList;

class RowCluster
{
 public:
  RowCluster()
    : hasBeenVisited(false),
    minCol(-1),
    maxCol(-1),
    row(-1),
    centerOfGravity(-1.),
    digis(new MyDigiList),
    parents(),
    children()
    {
    }
  ~RowCluster()
    {
      if (digis) {
	delete digis;
	digis = NULL;
      }
    }
  Bool_t hasBeenVisited;
  Int_t minCol;
  Int_t maxCol;
  Int_t row;
  Double_t centerOfGravity;
  MyDigiList *digis;
  std::list<RowCluster*> parents;
  std::list<RowCluster*> children;
 private:
  RowCluster(const RowCluster&);
  RowCluster& operator=(const RowCluster&);
};


class CbmTrdClusterFinderFast : public FairTask
{

 public:

  /**
   * Default constructor.
   */
  CbmTrdClusterFinderFast();
  CbmTrdClusterFinderFast(Bool_t MultiHit, Bool_t NeighbourReadout, Bool_t RowClusterMerger, Double_t MinimumChargeTH);

  /**
   * \brief Destructor.
   */
  virtual ~CbmTrdClusterFinderFast();
 
  /**
   * \brief Inherited from FairTask.
   */
  virtual InitStatus Init();

  /**
   * \breif Inherited from FairTask.
   */
  virtual void SetParContainers();
  
  /**
   * \breif Inherited from FairTask.
   */
  virtual void Exec(Option_t * option);

  void SetTriggerThreshold(Double_t triggerthreshold);
  void SetNeighbourTrigger(Bool_t trigger);
  void SetNeighbourRowTrigger(Bool_t trigger);
  void SetPrimaryClusterRowMerger(Bool_t rowMerger);
  //void SetMinimumChargeThreshold(Double_t minCharge);
  void SetTriangularPads(Bool_t triangles);
 private:
  //static const Bool_t fMultiHit = true;

  ClusterList *clusterModule(MyDigiList *digis/*, MyDigiList *neighbours*/);  
  void mergeRowCluster(RowCluster *currentCluster,
		       std::list<RowCluster*> *openList);
  ClusterList *findCluster(std::list<RowCluster*> *rowClusterList);
  void walkCluster(std::list<RowCluster*> *rowClusterList, 
		   RowCluster *currentRowCluster,
		   MyDigiList *cluster);
  void drawCluster(Int_t moduleId, ClusterList *clusterList);

  void addNeighbourDigis(Int_t nRow, ClusterList *clusters, MyDigiList *neighbours);

  void addNeighbourDigisTriangular(Int_t nRow, ClusterList *clusters, MyDigiList *neighbours);

  void addCluster(std::map<Int_t, ClusterList*> fModClusterMap);

  Double_t CenterOfGravity(RowCluster *rowCluster);

  TClonesArray*     fDigis;       /** Input array of CbmTrdDigi **/
  TClonesArray*     fClusters;    /** Output array of CbmTrdCluster **/

  CbmTrdDigiPar *fDigiPar;   //!
  CbmTrdModule  *fModuleInfo; //!
  
  CbmTrdGeoHandler* fGeoHandler; //!

  //static const Float_t minimumChargeTH = 5e-03;
  Int_t fRowMergerCounter;
  Int_t ClusterSum;
  //static 
  Double_t fMinimumChargeTH;
  Bool_t fNeighbourReadout; // trigger neighbouring channels (left, right in upper row, same row and lower row)
  Bool_t fNeighbourRowTrigger; // trigger channels in the neighbouring rows (if false and fNeighbourReadout == true: only left and right neighbour are triggered)
  Bool_t fRowClusterMerger;  // merge self triggered channels accross rows
  Bool_t fMultiHit;
  Bool_t fTrianglePads;
  CbmTrdClusterFinderFast(const CbmTrdClusterFinderFast&);
  CbmTrdClusterFinderFast& operator=(const CbmTrdClusterFinderFast&);

  ClassDef(CbmTrdClusterFinderFast,2);
};
#endif
