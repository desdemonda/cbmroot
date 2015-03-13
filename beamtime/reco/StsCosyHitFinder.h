/** StsCosyHitFinder.h
 *@author Florian Uhlig <f.uhlig@gsi.de>
 **
 ** Task to find neighboring fibres which are above
 ** the base line.
 ** Create as an output an array of
 ** the digis belonging to the cluster.
 **/

#ifndef STSCOSYHITFINDER_H
#define STSCOSYHITFINDER_H 

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



class StsCosyHitFinder : public FairTask
{
  
 public:
  
  /**
   * Default constructor.
   */
  StsCosyHitFinder();
  
  /**
   * Default destructor.
   */
  ~StsCosyHitFinder();
 
  /** Initialisation **/
  virtual InitStatus ReInit();
  virtual InitStatus Init();
  virtual void SetParContainers();
  
  /** Executed task **/
  virtual void Exec(Option_t * option);
  Int_t GetEntries () { return fChain->GetEntries();}
  
  void SetTriggeredMode(Bool_t mode) { fTriggeredMode = mode; }
  void SetTriggeredStation(Int_t station) { fTriggeredStation = station ; }
  
  /** Finish task **/
  virtual void Finish();
 
 private:

  TClonesArray*     hodoClusters;   /** Input array of CbmFiberHodoCluster **/
  TClonesArray*     stsClusters;    /** Input array of CbmStsCluster **/
  TClonesArray*     fDigis;    /** Input array of CbmStsCluster **/
  TClonesArray*     hDigis;    /** Input array of CbmStsCluster **/

  TClonesArray*     fHits;    /** Output array of CbmHits **/
  
  TChain *fChain;           
  TH1F *deltat0;
  TH1F *deltat1;
  TH1F *deltat2;
  TH1F *deltat3;
  TH1F *deltat4;
  TH1F *h_time_diff;

  Bool_t fTriggeredMode; ///< Flag if data is taken in triggered mode
  Int_t  fTriggeredStation;

  StsCosyHitFinder(const StsCosyHitFinder&);
  StsCosyHitFinder& operator=(const StsCosyHitFinder&);

  ClassDef(StsCosyHitFinder,1);
  
};
#endif
