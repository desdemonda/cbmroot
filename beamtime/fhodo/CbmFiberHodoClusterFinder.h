/** CbmFiberHodoClusterFinder.h
 **
 ** Task to find neighboring fibres which are above
 ** the base line.
 ** Create as an output an array of
 ** the digis belonging to the cluster.
 **/

#ifndef CBMFIBERHODOCLUSTERFINDER_H
#define CBMFIBERHODOCLUSTERFINDER_H 

#include "FairTask.h"
#include "CbmFiberHodoAddress.h"
#include "CbmFiberHodoDigi.h"

//#include "TH1F.h"
//#include "TH2F.h"

#include <map>
#include <set>

//don't know why forward declaration doesn't work here
//class TClonesArray;
#include "TClonesArray.h"
#include "TString.h"

class TH1F;
class TH2F;

struct classcomp {
  bool operator() (const CbmFiberHodoDigi* lhs, const CbmFiberHodoDigi* rhs) const
    {return CbmFiberHodoAddress::GetStripId(lhs->GetAddress()) < CbmFiberHodoAddress::GetStripId(rhs->GetAddress());}
};
    
class CbmFiberHodoClusterFinder : public FairTask
{
  
 public:
  
  /**
   * Default constructor.
   */
  CbmFiberHodoClusterFinder();
  
  /**
   * Default destructor.
   */
  ~CbmFiberHodoClusterFinder();
 
  /** Initialisation **/
  virtual InitStatus ReInit();
  virtual InitStatus Init();
  virtual void SetParContainers();
  
  /** Executed task **/
  virtual void Exec(Option_t * option);
  
  /** Finish task **/
  virtual void Finish();

  void SetInputDataLevelName(TString name) { fInputLevelName = name; }

 private:

  TClonesArray*     fDigis;       /** Input array of CbmFiberHodoDigi **/
  TClonesArray*     fClusters;    /** candidates array of CbmFiberHodoCluster **/
  TClonesArray*     finalClusters;    /** Output array of CbmFiberHodoCluster **/

  std::map<Int_t, std::set<CbmFiberHodoDigi*, classcomp> > fDigiMap;   /** digis per hodo layer **/ 

  TString fInputLevelName;

  TH2F* hodo1_pos;
  TH2F* hodo2_pos;

  TH2F* hodo1_xcor;
  TH2F* hodo2_xcor;

  TH2F* hodo1_ycor;
  TH2F* hodo2_ycor;
  TH1F* hodo1x;
  TH1F* hodo1y;
  TH1F* hodo2x;
  TH1F* hodo2y;
  TH2F* hodo_xx;
  TH2F* hodo_yy;
  TH2F* hodo1_pos_nocut;
  TH1F* hodo1x_nocut;
  TH1F* hodo1y_nocut;

  CbmFiberHodoClusterFinder(const CbmFiberHodoClusterFinder&);
  CbmFiberHodoClusterFinder& operator=(const CbmFiberHodoClusterFinder&);
  
  ClassDef(CbmFiberHodoClusterFinder,2);
  
};
#endif
