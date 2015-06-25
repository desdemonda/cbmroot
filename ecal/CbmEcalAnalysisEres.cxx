#include "CbmEcalAnalysisEres.h"

#include "CbmEcalCell.h"
#include "CbmEcalMaximum.h"

#include "FairRootManager.h"

#include "TClonesArray.h"


#include <iostream>
#include <list>

using namespace std;

/** Loop procedure **/
void CbmEcalAnalysisEres::Exec(Option_t* option)
{
  fEv++;
  if (fTree==NULL) InitTree();
  CbmEcalMaximum* max=NULL;
  CbmEcalCell* cell=NULL;
  CbmEcalCell* min;
  list<CbmEcalCell*> cells;
  list<CbmEcalCell*> cells2;
  list<CbmEcalCell*>::const_iterator p;
  Int_t n=fMax->GetEntries();
  Int_t i;
  Double_t me=1e10;
  for(i=0;i<n;i++)
  {
    max=(CbmEcalMaximum*)fMax->At(i);
    if (max==NULL) continue;
    cell=max->Cell();
    if (cell->GetTotalEnergy()<0.005) continue;
    break;
  }
  if (i==n)
  {
    Info("Exec","No suitable maximum found in event %d.", fEv);
    return;
  }
  cells.clear();
  cell->GetNeighborsList(max->I(), cells);
  cells.push_back(cell);
  cell->GetNeighborsList(0, cells2);
  for(p=cells2.begin();p!=cells2.end();++p)
    if ((*p)->GetTotalEnergy()<me)
    {
      min=(*p);
      me=min->GetTotalEnergy();
    }
  if (find(cells.begin(), cells.end(), min)==cells.end())
    cells.push_back(min);

  fCellType=cell->GetType();
  fE=0;
  for(p=cells.begin();p!=cells.end();++p)
    fE+=(*p)->GetTotalEnergy();
  
  fTree->Fill();
}

void CbmEcalAnalysisEres::InitTree()
{
  if (fTree!=NULL) return;

  fTree=new TTree("eres", "Energy resolution tree for calorimeter");
  fTree->Branch("ev", &fEv, "ev/I");
  fTree->Branch("celltype", &fCellType, "celltype/I");
  fTree->Branch("e", &fE, "e/D");
}

CbmEcalAnalysisEres::CbmEcalAnalysisEres()
  : FairTask(),
    fMax(NULL),
    fTree(NULL),
    fCellType(0),
    fE(0.),
    fEv(0)
{
}

CbmEcalAnalysisEres::CbmEcalAnalysisEres(const char* name, const Int_t iVerbose)
  : FairTask(name, iVerbose),
    fMax(NULL),
    fTree(NULL),
    fCellType(0),
    fE(0.),
    fEv(0)
{
}

/** Initing routine **/
InitStatus CbmEcalAnalysisEres::Init()
{
  fTree=NULL;
  FairRootManager* fManager=FairRootManager::Instance();
  if (!fManager)
  {
    Fatal("Init", "Can't find a Root Manager.");
    return kFATAL;
  }
  fMax=(TClonesArray*)fManager->GetObject("EcalMaximums");
  if (!fMax)
  {
    Fatal("Init", "Can't find an calorimeter maximums.");
    return kFATAL;
  }

  return kSUCCESS;
}

/** Finishing routine **/
void CbmEcalAnalysisEres::Finish()
{
  fTree->Write();
//  ofstream f(fName);
//  f << fEv << endl;
//  f.close();
}

ClassImp(CbmEcalAnalysisEres)
