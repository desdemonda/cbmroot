// -------------------------------------------------------------------------
// -----                   CbmEcalHitProducer source file              -----
// -----               Created 11/03/05  by Yu.Kharlov                 -----
// -------------------------------------------------------------------------

/* $Id: CbmEcalHitProducer.cxx,v 1.14 2006/07/29 00:33:53 prokudin Exp $ */

#include "CbmEcalHitProducer.h"

#include "CbmEcal.h"
#include "CbmEcalCellMC.h"
#include "CbmEcalPointLite.h"
#include "CbmEcalHit.h"

#include "FairRootManager.h"
#include "CbmMCTrack.h"

#include "TVector3.h"
#include "TRandom.h"

#include <iostream>
#include <iomanip>
#include <vector>

using std::cout;
using std::cerr;
using std::endl;
using std::flush;
using std::map;
using std::vector;

// -----   Default constructor   -------------------------------------------
CbmEcalHitProducer::CbmEcalHitProducer() 
  : FairTask(),
    fStr(NULL),
    fInf(NULL),
    fListECALpts(NULL),
    fListUHits(NULL),
    fListHits(NULL),
    fHitCollection(NULL),
    fListStack(NULL),
    fNHits(0),
    fEvent(0),
    fInited(kFALSE),
    fUseMCPoints(kFALSE),
    fUseSummableHits(kFALSE),
    fUseUnSummableHits(kFALSE),
    fThreshold(0.),
    fThresholdPS(0.),
    fNoise(0.),
    fNoisePS(0.),
    fFileGeo(""),
    fProduceSummableHits(kTRUE),
    fStandAlone(kTRUE),
    fEcalBinSize(-1.),
    fPSBinSize(-1.),
    fProduceHits(kTRUE)
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   -------------------------------------------
CbmEcalHitProducer::CbmEcalHitProducer(const char *name, const Int_t iVerbose, const char* fileGeo) 
  : FairTask(name,iVerbose),
    fStr(NULL),
    fInf(CbmEcalInf::GetInstance(fFileGeo)),
    fListECALpts(NULL),
    fListUHits(NULL),
    fListHits(NULL),
    fHitCollection(NULL),
    fListStack(NULL),
    fNHits(0),
    fEvent(0),
    fInited(kFALSE),
    fUseMCPoints(kFALSE),
    fUseSummableHits(kFALSE),
    fUseUnSummableHits(kFALSE),
    fThreshold(0.),
    fThresholdPS(0.),
    fNoise(0.),
    fNoisePS(0.),
    fFileGeo(fileGeo),
    fProduceSummableHits(kTRUE),
    fStandAlone(kTRUE),
    fEcalBinSize(-1.),
    fPSBinSize(-1.),
    fProduceHits(kTRUE)
{
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmEcalHitProducer::~CbmEcalHitProducer()
{
  FairRootManager *fManager =FairRootManager::Instance();
  fManager->Write();
}
// -------------------------------------------------------------------------

void CbmEcalHitProducer::SetParContainers()
{
  ;
}

// -----   Initialization   ------------------------------------------------
InitStatus CbmEcalHitProducer::Init()
{
  fInited=kTRUE;
  FairRootManager* fManager = FairRootManager::Instance();

  if (fUseMCPoints==kFALSE&&fUseSummableHits==kFALSE&&fUseUnSummableHits==kFALSE)
  {
    cerr << "Hit producer need some data source!!!" << endl;
    cerr << "Use CbmEcalHitProducer::SummableHitProducer() to produce summable hits" << endl;
    cerr << "Or CbmEcalHitProducer::UnSummableHitProducer() to produce unsummable hits." << endl;
    return kFATAL;
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
  
  // all tracks
  fListStack = (TClonesArray *)fManager->GetObject("MCTrack");
  fHitCollection = new TClonesArray("CbmEcalHit",100);
  if (fProduceHits)
    if (fProduceSummableHits)
      fManager->Register("EcalSumHit","ECAL",fHitCollection,kTRUE);
    else
      fManager->Register("EcalHit","ECAL",fHitCollection,kTRUE);
  else
    if (fProduceSummableHits)
      fManager->Register("EcalSumHit","ECAL",fHitCollection,kFALSE);
    else
      fManager->Register("EcalHit","ECAL",fHitCollection,kFALSE);

  if (fStandAlone)
  {
    fInf->CheckVariables();
    fStr=new CbmEcalStructure(fInf);
    fStr->Construct();
  }
  else
  {
    fStr=(CbmEcalStructure*)fManager->GetObject("EcalStructure");
    if (!fStr) 
    {
      Fatal("Init()", "Can't find calorimeter structure in the system.");
      return kFATAL;
    }
    fInf=fStr->GetEcalInf();
  }
  
  return kSUCCESS;
}
// -------------------------------------------------------------------------

/** Loop over MCPoints hits and add them to cells **/
void CbmEcalHitProducer::LoopForMCPoints()
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
  if (fProduceSummableHits==kFALSE)
    for(UInt_t j=0; j<n; j++)
    {
      pt = (CbmEcalPointLite*)fListECALpts->At(j);
      cell=fStr->GetCell(pt->GetDetectorID(), ten, isPS);
      if (ten==0)
        if (isPS) ; // cell->AddPSEnergy(pt->GetEnergyLoss());  Preshower removed
        else
          cell->AddEnergy(pt->GetEnergyLoss());
    }
  else
    for(UInt_t j=0; j<n; j++)
      {
        CbmEcalCellMC* cellmc;
        pt = (CbmEcalPointLite*)fListECALpts->At(j);
        cellmc=(CbmEcalCellMC*)fStr->GetCell(pt->GetDetectorID(), ten, isPS);
        if (ten==0)
          if (isPS) ; // cell->AddTrackPSEnergy(pt->GetTrackID(),pt->GetEnergyLoss()); Preshower removed
            else cellmc->AddTrackEnergy(pt->GetTrackID(),pt->GetEnergyLoss());
      } 
}

/** Loop over summable hits and add them to cells **/
void CbmEcalHitProducer::LoopForSummableHits()
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
  if (fProduceSummableHits==kFALSE)
  for(Int_t j=0; j<n; j++)
  {
    pt = (CbmEcalHit*)fListHits->At(j);
    cell=fStr->GetHitCell(pt->GetDetectorId());
    cell->AddEnergy(pt->GetEnergy());
  } else
  for(Int_t j=0; j<n; j++)
  {
    CbmEcalCellMC* cellmc;
    pt = (CbmEcalHit*)fListHits->At(j);
    cellmc=(CbmEcalCellMC*)fStr->GetHitCell(pt->GetDetectorId());
    cellmc->AddTrackEnergy(pt->GetTrackId(), pt->GetEnergy());
  }
}

/** Loop over hits and add them to cells **/
void CbmEcalHitProducer::LoopForUnSummableHits()
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
        	pt = (CbmEcalHit*)fListUHits->At(j);
		cell=fStr->GetHitCell(pt->GetDetectorId());
		cell->AddEnergy(pt->GetEnergy());
//		cell->AddPSEnergy(pt->GetPSEnergy());

	}
}

/** Adds hits to the hit collection from information
 ** from cell **/
void CbmEcalHitProducer::CellToHits(CbmEcalCell* cell)
{
  Float_t energy;
  Float_t psenergy=0;
  Int_t ebins;
  Int_t psbins;
  if (fProduceSummableHits==kFALSE)
  {
    if (fNoise>0)
      energy=cell->GetEnergy()+gRandom->Gaus(0,fNoise);
    else
      energy=cell->GetEnergy();

    if (fEcalBinSize>0)
    {
      ebins=(Int_t)(energy/fEcalBinSize);
      if (energy<0) ebins--;
      energy=ebins*fEcalBinSize;
    }

    if (energy<fThreshold) energy=0;
    cell->SetEnergy(energy);
    if (fProduceHits)
    if (energy!=0||psenergy!=0)
      AddHit(cell->GetCellNumber(),energy,psenergy, -1, cell->GetTime());
  }
  else
  {
    CbmEcalCellMC* cellmc=(CbmEcalCellMC*)cell;
    map<Int_t, Float_t>::const_iterator p=cellmc->GetTrackEnergyBegin();
    for(;p!=cellmc->GetTrackEnergyEnd();++p)
    {
      energy=p->second;
      AddHit(cellmc->GetCellNumber(),energy,psenergy, p->first, cellmc->GetTrackTime(p->first));
    }
  }
}
// -----   Execution of Task   ---------------------------------------------
void CbmEcalHitProducer::Exec(Option_t* option)
{

  vector<CbmEcalModule*>::const_iterator p;
  vector<CbmEcalModule*> modules;
  CbmEcalModule* module;
  vector<CbmEcalCell*> cells;

  fEvent++;
  if (fVerbose>0)
    cout << "--> Event no. " << fEvent;

  fHitCollection->Clear();
  fNHits = 0;
  UInt_t i;

  if (fStandAlone)
  {
    fStr->ResetModules();
    if (fUseMCPoints) LoopForMCPoints();
    if (fUseSummableHits) LoopForSummableHits();
    if (fUseUnSummableHits) LoopForUnSummableHits();
  }

  if (fVerbose>0)
    cout << endl << flush;
  fStr->GetStructure(modules);
  for(p=modules.begin();p!=modules.end();++p)
  {
    module=*p;
    if (module)
    {
      cells=module->GetCells();
      for(i=0;i<cells.size();i++)
          CellToHits(cells[i]);
    }
  }
}
// -------------------------------------------------------------------------


// -----   Add Hit to HitCollection   --------------------------------------
void CbmEcalHitProducer::AddHit(Int_t cellnumber, Float_t energy, Float_t psenergy, Int_t trackID, Double32_t time)
{
  new((*fHitCollection)[fNHits++]) CbmEcalHit(cellnumber,energy,psenergy,trackID,time); //No time information at the moment
}
// -------------------------------------------------------------------------


// -----   Finish Task   ---------------------------------------------------
void CbmEcalHitProducer::Finish()
{
  fHitCollection->Clear();
}
// -------------------------------------------------------------------------


ClassImp(CbmEcalHitProducer)
