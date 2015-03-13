
// -------------------------------------------------------------------------
// -----          CbmTrdClusterFinderFast header file                  -----
// -----   Created 02/06/14  by Cruz Garcia                            -----
// -------------------------------------------------------------------------


#ifndef CBMTRDCLUSTERFINDERDPB_H
#define CBMTRDCLUSTERFINDERDPB_H 

#include "FairTask.h"

#include <vector>
#include <list>
#include <map>
#include <utility>

class CbmTrdDigiPar;
class CbmTrdModule;
class CbmTrdGeoHandler;
class TClonesArray;
class CbmTrdDigi;
class CbmTrdCluster;

typedef std::list< std::pair<Int_t, Int_t/*CbmTrdDigi**/> > DigiList;
//typedef std::list<CbmTrdDigi*> DigiList;
//typedef std::vector<DigiList*> ClusterVector;
//typedef std::map<Int_t, CbmTrdDigi*> DigiMap;

class CbmTrdClusterFinderDPB : public FairTask
{

 public:

  /**
   * Default constructor.
   */
  CbmTrdClusterFinderDPB();

  /**
   * \brief Destructor.
   */
  virtual ~CbmTrdClusterFinderDPB();
 
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

 private:
  //static const Bool_t fMultiHit = true;

 
  void doClustering(Int_t ModuleAddress, DigiList* digiList);
  void addCluster(DigiList* clusterList);

  TClonesArray*     fDigis;       /** Input array of CbmTrdDigi **/
  TClonesArray*     fClusters;    /** Output array of CbmTrdCluster **/

  CbmTrdDigiPar *fDigiPar;   //!
  CbmTrdModule  *fModuleInfo; //!
  
  CbmTrdGeoHandler* fGeoHandler; //!

  CbmTrdClusterFinderDPB(const CbmTrdClusterFinderDPB&);
  CbmTrdClusterFinderDPB& operator=(const CbmTrdClusterFinderDPB&);

  ClassDef(CbmTrdClusterFinderDPB,2);
};
#endif

