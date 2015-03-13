// -----------------------------------------------------------------------
// -----                     CbmTrdHitProducerClusterQa                     -----
// -----               Created 19.03.13  by C. Bergmann               -----
// -----------------------------------------------------------------------


#include "CbmTrdHitProducerClusterQa.h"

#include "CbmTrdHit.h"
#include "CbmTrdPoint.h"
#include "CbmTrdTrack.h"

#include "FairRootManager.h"
#include "CbmMCTrack.h"

#include "TClonesArray.h"
#include "TVector3.h"
#include "TMath.h"
#include "TH1F.h"
#include "TH2F.h"

#include <iostream>
using std::cout;
using std::endl;

CbmTrdHitProducerClusterQa::CbmTrdHitProducerClusterQa()
: FairTask()
{
}
CbmTrdHitProducerClusterQa::CbmTrdHitProducerClusterQa(const char* name,const char* title)
: FairTask(name)
{
}
CbmTrdHitProducerClusterQa::~CbmTrdHitProducerClusterQa()
{
}
InitStatus CbmTrdHitProducerClusterQa::Init()
{
  FairRootManager* rootMgr = FairRootManager::Instance();
  if(NULL == rootMgr) {
    cout << "-E- CbmTrdHitProducerClusterQa::Init : "
	 << "ROOT manager is not instantiated !" << endl;
    return kFATAL;
  }
  PrepareHistograms();
  return kSUCCESS;
}
void CbmTrdHitProducerClusterQa::Exec(Option_t* option)
{
}
void CbmTrdHitProducerClusterQa::Finish()
{
  WriteHistograms();
}
void CbmTrdHitProducerClusterQa::PrepareHistograms()
{
}
void CbmTrdHitProducerClusterQa::WriteHistograms()
{
  gDirectory->mkdir("CbmTrdHitProducerClusterQa");
  gDirectory->cd("CbmTrdHitProducerClusterQa");
  gDirectory->cd("..");
}
ClassImp(CbmTrdHitProducerClusterQa);
