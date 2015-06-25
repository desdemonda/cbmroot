#include "CbmEcalStructureFiller.h"
#include "CbmEcalPointLite.h"
#include "CbmEcalHit.h"
#include "CbmEcalCellMC.h"

#include "FairRootManager.h"

#include "TRandom.h"
#include "TVector3.h"
#include "TRandom.h"

#include <vector>
#include <iostream>

using namespace std;

// -----   Default constructor   -------------------------------------------
CbmEcalStructureFiller::CbmEcalStructureFiller() 
  : FairTask(), 
    fStr(NULL),
    fInf(NULL),
    fListECALpts(NULL),
    fListUHits(NULL),
    fListHits(NULL),
    fEvent(0),
    fInited(kFALSE),
    fUseMCPoints(kFALSE),
    fUseSummableHits(kFALSE),
    fUseUnSummableHits(kFALSE),
    fStoreTrackInfo(kTRUE),
    fFileGeo("")
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   -------------------------------------------
CbmEcalStructureFiller::CbmEcalStructureFiller(const char *name, const Int_t iVerbose, const char* fileGeo) 
  : FairTask(name,iVerbose),
    fStr(NULL),
    fInf(CbmEcalInf::GetInstance(fileGeo)),
    fListECALpts(NULL),
    fListUHits(NULL),
    fListHits(NULL),
    fEvent(0),
    fInited(kFALSE),
    fUseMCPoints(kFALSE),
    fUseSummableHits(kFALSE),
    fUseUnSummableHits(kFALSE),
    fStoreTrackInfo(kTRUE),
    fFileGeo(fileGeo)
{
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmEcalStructureFiller::~CbmEcalStructureFiller()
{
  FairRootManager *fManager =FairRootManager::Instance();
  fManager->Write();
}
// -------------------------------------------------------------------------

void CbmEcalStructureFiller::SetParContainers()
{
  ;
}

// -----   Initialization   ------------------------------------------------
InitStatus CbmEcalStructureFiller::Init()
{
  fInited=kTRUE;
  FairRootManager* fManager = FairRootManager::Instance();

  if (fUseMCPoints==kFALSE&&fUseSummableHits==kFALSE&&fUseUnSummableHits==kFALSE)
  {
    Warning("Init()", "No data source for structure filler! Calorimeter structure will be empty.");
//    return kFATAL;
  }
  //ECAL MC points
  if (fUseMCPoints) 
    fListECALpts = (TClonesArray*)fManager->GetObject("EcalPointLite");

  //ECAL Summable hits
  if (fUseSummableHits) 
    fListHits = (TClonesArray*)fManager->GetObject("EcalSumHit");
  
  //ECAL UnSummable hits
  if (fUseUnSummableHits) 
    fListUHits = (TClonesArray*)fManager->GetObject("EcalHit");
  
  // all MC tracks
  //fListStack = (TClonesArray *)fManager->GetObject("MCTrack");
  fInf->CheckVariables();
  fStr=new CbmEcalStructure(fInf);
  if (fStoreTrackInfo) fStr->SetUseMC(1);
  fStr->Construct();
  
  fManager->Register("EcalStructure", "ECAL", fStr, kFALSE);
  return kSUCCESS;
}
// -------------------------------------------------------------------------

/** Loop over MCPoints hits and add them to cells **/
void CbmEcalStructureFiller::LoopForMCPoints()
{
  CbmEcalPointLite* pt=NULL;
  CbmEcalCell* cell;
  Int_t ten;
  UInt_t n;
  Bool_t isPS;

  n=fListECALpts->GetEntriesFast();
  if (fVerbose>0)
  {
    cout << ". Number of input MC points: ";
    cout << n << flush;
  }

  for(UInt_t j=0; j<n; j++)
  {
    pt=(CbmEcalPointLite*)fListECALpts->At(j);
    cell=fStr->GetCell(pt->GetDetectorID(), ten, isPS);
    if (ten==0)
    {
      if (isPS)
	; // cell->AddPSEnergy(pt->GetEnergyLoss()); preshower removed
      else
	cell->AddEnergy(pt->GetEnergyLoss());
    }
  }
  if (fStoreTrackInfo)
  for(UInt_t j=0; j<n; j++)
  {
    pt=(CbmEcalPointLite*)fListECALpts->At(j);
    CbmEcalCellMC* cellmc=(CbmEcalCellMC*)fStr->GetCell(pt->GetDetectorID(), ten, isPS);
    if (ten==0)
    {
      if (isPS)
	; // cell->AddTrackPSEnergy(pt->GetTrackID(),pt->GetEnergyLoss()); //preshower removed
      else
	cellmc->AddTrackEnergy(pt->GetTrackID(),pt->GetEnergyLoss(), pt->GetTime());
    }
  }
}

/** Loop over summable hits and add them to cells **/
void CbmEcalStructureFiller::LoopForSummableHits()
{
  CbmEcalHit* pt=NULL;
  CbmEcalCell* cell;
  Int_t n;

  n=fListHits->GetEntriesFast();
  if (fVerbose>0)
  {
    cout << ". Number of input summable hits: ";
    cout << n << flush;
  }
  for(Int_t j=0; j<n; j++)
  {
    pt=(CbmEcalHit*)fListHits->At(j);
    cell=fStr->GetHitCell(pt->GetDetectorId());
    cell->AddEnergy(pt->GetEnergy());
  }
  if (fStoreTrackInfo)
  for(Int_t j=0; j<n; j++)
  {
    pt=(CbmEcalHit*)fListHits->At(j);
    CbmEcalCellMC* cellmc=(CbmEcalCellMC*)fStr->GetHitCell(pt->GetDetectorId());
    cellmc->AddTrackEnergy(pt->GetTrackId(), pt->GetEnergy(), pt->GetTime());
  }
}

/** Loop over hits and add them to cells **/
void CbmEcalStructureFiller::LoopForUnSummableHits()
{
  CbmEcalHit* pt=NULL;
  CbmEcalCell* cell;
  Int_t n;

  n=fListUHits->GetEntriesFast();
  if (fVerbose>0)
  {
    cout << ". Number of input unsummable hits: ";
    cout << n << flush;
  }
  for(Int_t j=0; j<n; j++)
  {
    pt=(CbmEcalHit*)fListUHits->At(j);
    cell=fStr->GetHitCell(pt->GetDetectorId());
    cell->AddEnergy(pt->GetEnergy());
    cell->SetTime(pt->GetTime());
  }
}

// -----   Execution of Task   ---------------------------------------------
void CbmEcalStructureFiller::Exec(Option_t* option)
{
  fEvent++;
  if (fVerbose>0)
    cout << "--> Event no. " << fEvent;

  fStr->ResetModules();
  if (fUseMCPoints) LoopForMCPoints();
  if (fUseSummableHits) LoopForSummableHits();
  if (fUseUnSummableHits) LoopForUnSummableHits();

  if (fVerbose>0)
    cout << endl << flush;
}
// -------------------------------------------------------------------------


// -----   Finish Task   ---------------------------------------------------
void CbmEcalStructureFiller::Finish()
{
  ;
}
// -------------------------------------------------------------------------


ClassImp(CbmEcalStructureFiller)
