/** CbmMCStreamerQa.cxx
 *@author E.Kryshen <e.kryshen@gsi.de>
 *@since 2010-10-06
 **/

#include "CbmMCStreamerQa.h"
#include "CbmMCTrack.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairMCPoint.h"

#include "CbmMCEvent.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TH3D.h"
#include "TH1D.h"
#include "TVector3.h"
#include "CbmMCEpoch.h"
#include "map"

using namespace std;

// -----   Default constructor   -------------------------------------------
CbmMCStreamerQa::CbmMCStreamerQa()
  : FairTask(),
    fNepoch(0),
    fMcEpoch(NULL),
    fMcChain(NULL),
    fPointArrays(),
    fMapPointsInEvents()
{
  for (Int_t i=kREF;i<kTutDet;i++) fPointArrays[i]=NULL;
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
CbmMCStreamerQa::CbmMCStreamerQa(const char* name, TChain* mcChain)
  : FairTask(name),
    fNepoch(0),
    fMcEpoch(NULL),
    fMcChain(mcChain),
    fPointArrays(),
    fMapPointsInEvents()
{
  for (Int_t i=kREF;i<kTutDet;i++) fPointArrays[i]=NULL;
}

// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmMCStreamerQa::~CbmMCStreamerQa(){
}
// -------------------------------------------------------------------------


// -----  SetParContainers -------------------------------------------------
void CbmMCStreamerQa::SetParContainers(){
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb = ana->GetRuntimeDb();

  rtdb->getContainer("FairBaseParSet");
  rtdb->getContainer("CbmGeoPassivePar");
  rtdb->getContainer("CbmGeoStsPar");
  rtdb->getContainer("CbmGeoMuchPar");
  rtdb->getContainer("CbmFieldPar");
}
// -------------------------------------------------------------------------


// -----   Public method Init (abstract in base class)  --------------------
InitStatus CbmMCStreamerQa::Init(){
  
  // Get and check FairRootManager
  FairRootManager* fManager = FairRootManager::Instance();
  fMcEpoch = (CbmMCEpoch*) fManager->GetObject("MCEpoch.");
  if (!fMcEpoch) {
    Fatal("Init","No MC epoch branch found in file");
    return kFATAL;
  }
  
  // Check for the chain of MC files
  if (!fMcChain) {
    Fatal("Init","MC chain pointer is NULL"); 
    return kFATAL;
  }
  
  if (!fMcChain->GetEntries()) {
    Fatal("Init","No entries in the MC chain");
    return kFATAL;
  }
  
  // Set branches
  fPointArrays[kSTS] = new TClonesArray("CbmStsPoint");
  fPointArrays[kMUCH] = new TClonesArray("CbmMuchPoint");

  fMcChain->SetBranchAddress("StsPoint",& fPointArrays[kSTS]);
  fMcChain->SetBranchAddress("MuchPoint", & fPointArrays[kMUCH]);
  
  fNepoch=0;

  
  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void CbmMCStreamerQa::Exec(Option_t* opt){
  fNepoch++;
  if (fVerbose>-1) printf("Epoch: %i",fNepoch);
  if (fVerbose>-1) printf("\n");
  
  for (Int_t detId=kREF;detId<kTutDet;detId++){
    TString detName="";
    CbmDetectorList::GetSystemName(DetectorId(detId),detName);
    for (Int_t i=0;i<fMcEpoch->GetNofPoints(DetectorId(detId));i++){
      FairMCPoint* point = fMcEpoch->GetPoint(DetectorId(detId),i);
      Int_t eventId = point->GetEventID();
      Double_t t = point->GetTime();
      Double_t x = point->GetX();
      Double_t y = point->GetY();
      Double_t z = point->GetZ();
      if (fVerbose>1) printf("  detector: %4s",detName.Data());
      if (fVerbose>1) printf("  point: %i",i);
      if (fVerbose>1) printf("  eventId=%i",eventId);
      if (fVerbose>1) printf("  position=(%8.2f,%8.2f,%8.2f)",x,y,z);
      if (fVerbose>1) printf("  time=%8.2f",t);
      if (fVerbose>1) printf("\n");
      // increment point counter for the event
      fMapPointsInEvents[detId][eventId]++;
    }
    
  }
}
// -------------------------------------------------------------------------


// -----   Public method Finish   ------------------------------------------
void CbmMCStreamerQa::Finish(){
  Int_t nEpochTotal = FairRootManager::Instance()->GetInChain()->GetEntries();
  if (fNepoch!=nEpochTotal) Warning("Finish","Not all MC epochs were analysed: analysed=%i total=%i\n",fNepoch,nEpochTotal);

  for (Int_t detId=kREF;detId<kTutDet;detId++){
    TString detName="";
    CbmDetectorList::GetSystemName(DetectorId(detId),detName);
    if (!fMapPointsInEvents[detId].size()) continue;
    if (!fPointArrays[detId]) Warning("Finish","Array of MC points for detector %s not initialized",detName.Data());

    Int_t eventIdFirst = fMapPointsInEvents[detId].begin()->first;
    Int_t eventIdLast  = (--fMapPointsInEvents[detId].end())->first;

    printf("Detector: %s size=%i ",detName.Data(),(Int_t)fMapPointsInEvents[detId].size());
    printf("  first event = %i",eventIdFirst);
    printf("  last event = %i",eventIdLast);
    printf("\n");
    
    if (eventIdLast-eventIdFirst<0) continue;
    
    Int_t nCorrect = 0;
    for (Int_t eventId=eventIdFirst;eventId<=eventIdLast;eventId++){
      fMcChain->GetEntry(eventId);
      Int_t nPointsInMcFile = fPointArrays[detId]->GetEntriesFast();
      Int_t nPointsInEpoch  = fMapPointsInEvents[detId][eventId];
      Bool_t isCorrect = (nPointsInMcFile==nPointsInEpoch);
      if (fVerbose>0 || !isCorrect) printf(" Event: %i",eventId);
      if (fVerbose>0 || !isCorrect) printf(" nPointsInMcFile=%i",nPointsInMcFile);
      if (fVerbose>0 || !isCorrect) printf(" nPointsInEpoch=%i",nPointsInEpoch);
      if (fVerbose>0 || !isCorrect) printf("\n");
      if (isCorrect) nCorrect++;
    }

    printf("Detector: %4s",detName.Data());
    if (eventIdLast-eventIdFirst+1==nCorrect) printf(" - number of points per event is correct");
    else                                    printf(" - number of points per event is wrong. See details above.");
    printf("\n");
  }
}
// -------------------------------------------------------------------------

ClassImp(CbmMCStreamerQa);
