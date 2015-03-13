/** StsCosyClusterFinder.h
 *@author Florian Uhlig <f.uhlig@gsi.de>
 **
 ** Task to find neighboring fibres which are above
 ** the base line.
 ** Create as an output an array of
 ** the digis belonging to the cluster.
 **/

#ifndef STSCOSYCLUSTERFINDER_H
#define STSCOSYCLUSTERFINDER_H 

#include "FairTask.h"
#include "CbmStsAddress.h"
#include "CbmStsDigi.h"

#include "TH1F.h"
#include "TH2F.h"

#include <map>
#include <set>

//don't know why forward declaration doesn't work here
//class TClonesArray;
#include "TClonesArray.h"



struct classcomp1 {
  bool operator() (const CbmStsDigi* lhs, const CbmStsDigi* rhs) const
    {return CbmStsAddress::GetElementId(lhs->GetAddress(),kStsChannel) < CbmStsAddress::GetElementId(rhs->GetAddress(),kStsChannel);}
};
    
class StsCosyClusterFinder : public FairTask
{
  
 public:
  
  /**
   * Default constructor.
   */
  StsCosyClusterFinder();
  
  /**
   * Default destructor.
   */
  ~StsCosyClusterFinder();
 
  /** Initialisation **/
  virtual InitStatus ReInit();
  virtual InitStatus Init();
  virtual void SetParContainers();
  
  /** Executed task **/
  virtual void Exec(Option_t * option);
  
  void SetTriggeredMode(Bool_t mode) { fTriggeredMode = mode; }
  void SetTriggeredStation(Int_t station) { fTriggeredStation = station ; }

/** Finish task **/
  virtual void Finish();

 private:

  TClonesArray*     fDigis;       /** Input array of CbmStsDigi **/
  TClonesArray*     fClusters;    /** candidates array of CbmStsCluster **/
  TClonesArray*     finalClusters;    /** Output array of CbmStsCluster **/

  TH1F *n_digi_cluster[6];
  TH1F *n_cluster_layer[6];
  TH1F *cluster_pos[6];
  TH1F *single_cluster_adc[6];
  TH1F *cluster_adc[6];
  TH2F *sts0_XY;
  TH2F *sts1_XY;
  TH2F *sts2_XY;
  TH1F *time_diff[6];
  TH1F *m_time_diff[6];
  TH1F *time_diff_0;
  TH1F *time_diff_1;
  TH1F *time_diff_2;

  Bool_t fTriggeredMode; ///< Flag if data is taken in triggered mode
  Int_t  fTriggeredStation;

  std::map<Int_t, std::set<CbmStsDigi*, classcomp1> > fDigiMap;   /** digis per hodo layer **/ 

  StsCosyClusterFinder(const StsCosyClusterFinder&);
  StsCosyClusterFinder& operator=(const StsCosyClusterFinder&);

  ClassDef(StsCosyClusterFinder,1);
  
};
#endif
