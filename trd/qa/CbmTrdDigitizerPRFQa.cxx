// -----------------------------------------------------------------------
// -----                     CbmTrdDigitizerPRFQa                     -----
// -----               Created 19.03.13  by C. Bergmann               -----
// -----------------------------------------------------------------------


#include "CbmTrdDigitizerPRFQa.h"

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

CbmTrdDigitizerPRFQa::CbmTrdDigitizerPRFQa()
: FairTask()
{
}
CbmTrdDigitizerPRFQa::CbmTrdDigitizerPRFQa(const char* name,const char* title)
: FairTask(name)
{
}
CbmTrdDigitizerPRFQa::~CbmTrdDigitizerPRFQa()
{
}
InitStatus CbmTrdDigitizerPRFQa::Init()
{
  FairRootManager* rootMgr = FairRootManager::Instance();
  if(NULL == rootMgr) {
    cout << "-E- CbmTrdDigitizerPRFQa::Init : "
	 << "ROOT manager is not instantiated !" << endl;
    return kFATAL;
  }
  PrepareHistograms();
  return kSUCCESS;
}
void CbmTrdDigitizerPRFQa::Exec(Option_t* option)
{
}
void CbmTrdDigitizerPRFQa::Finish()
{
  WriteHistograms();
}
void CbmTrdDigitizerPRFQa::PrepareHistograms()
{
}
void CbmTrdDigitizerPRFQa::WriteHistograms()
{
  gDirectory->mkdir("CbmTrdDigitizerPRFQa");
  gDirectory->cd("CbmTrdDigitizerPRFQa");
  gDirectory->cd("..");
}
ClassImp(CbmTrdDigitizerPRFQa);
