#include "CbmEcalAnalysisRe.h"

#include "FairRootManager.h"

#include "CbmEcalStructure.h"
#include "CbmEcalCell.h"
#include "CbmEcalPoint.h"

#include "TTree.h"
#include "TClonesArray.h"

#include <iostream>
#include <fstream>

using namespace std;

/** Loop procedure **/
void CbmEcalAnalysisRe::Exec(Option_t* option)
{
  Int_t n=fMCPoints->GetEntries();
  Int_t x;
  Int_t y;
  CbmEcalCell* cell;
  CbmEcalPoint* p;

  InitTree();

  if (n>0)
  {
    p=(CbmEcalPoint*)fMCPoints->At(0);
    fMCX=p->GetX();
    fMCY=p->GetY();
  }
  else
  {
    fMCX=-1111;
    fMCY=-1111;
  }
  for(x=0;x<9;x++)
  for(y=0;y<9;y++)
  {
    cell=fStr->GetCell(fSX+(x-2)*fD+0.001, fSY+(y-2)*fD+0.001);
    fOut[x+y*9]=cell->GetTotalEnergy();
    fX[x+y*9]=cell->GetX1();
    fY[x+y*9]=cell->GetY1();
//    cout << cell->GetX1() << " " << cell->GetX2() << endl;
  }

  fTree->Fill();
}

void CbmEcalAnalysisRe::InitTree()
{
  if (fTree) return;
  fTree=new TTree("re", "re");
  fTree->Branch("d", fOut, "d[81]/D");
  fTree->Branch("x", fX  , "x[81]/D");
  fTree->Branch("y", fY  , "y[81]/D");
  fTree->Branch("mcx",&fMCX, "mcx/D");
  fTree->Branch("mcy",&fMCY, "mcy/D");
}

CbmEcalAnalysisRe::CbmEcalAnalysisRe(const char* name, const Int_t iVerbose)
  : FairTask(name, iVerbose)
{
  fTree=NULL;
  fS=9;
  fD=6.0;
  fSX=0.0;
  fSY=120.0;
}

/** Initing routine **/
InitStatus CbmEcalAnalysisRe::Init()
{
  FairRootManager* fManager=FairRootManager::Instance();
  fStr=(CbmEcalStructure*)fManager->GetObject("EcalStructure");
  if (!fStr) 
  {
    Fatal("Init()", "Can't find calorimeter structure in the system.");
    return kFATAL;
  }
  fMCPoints=(TClonesArray*)fManager->GetObject("EcalPoint");
  if (!fMCPoints)
  {
    Fatal("Init()", "Can't find array of calorimeter MCPoints. ");
    return kFATAL;
  }
  return kSUCCESS;
}

/** Finishing routine **/
void CbmEcalAnalysisRe::Finish()
{
  if (fTree)
    fTree->Write();
}

ClassImp(CbmEcalAnalysisRe)
