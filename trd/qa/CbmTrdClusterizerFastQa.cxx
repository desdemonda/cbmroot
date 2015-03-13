// -----------------------------------------------------------------------
// -----                     CbmTrdClusterizerFastQa                  -----
// -----               Created 19.03.13  by C. Bergmann               -----
// -----------------------------------------------------------------------


#include "CbmTrdClusterizerFastQa.h"

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

CbmTrdClusterizerFastQa::CbmTrdClusterizerFastQa()
: FairTask()
{
}
CbmTrdClusterizerFastQa::CbmTrdClusterizerFastQa(const char* name,const char* title)
: FairTask(name)
{
}
CbmTrdClusterizerFastQa::~CbmTrdClusterizerFastQa()
{
}
InitStatus CbmTrdClusterizerFastQa::Init()
{
  FairRootManager* rootMgr = FairRootManager::Instance();
  if(NULL == rootMgr) {
    cout << "-E- CbmTrdClusterizerFastQa::Init : "
	 << "ROOT manager is not instantiated !" << endl;
    return kFATAL;
  }
  PrepareHistograms();
  return kSUCCESS;
}
void CbmTrdClusterizerFastQa::Exec(Option_t* option)
{
}
void CbmTrdClusterizerFastQa::Finish()
{
  WriteHistograms();
}
void CbmTrdClusterizerFastQa::PrepareHistograms()
{
}
void CbmTrdClusterizerFastQa::WriteHistograms()
{
  gDirectory->mkdir("CbmTrdClusterizerFastQa");
  gDirectory->cd("CbmTrdClusterizerFastQa");
  gDirectory->cd("..");
}
ClassImp(CbmTrdClusterizerFastQa);
