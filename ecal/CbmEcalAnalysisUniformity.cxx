#include "CbmEcalAnalysisUniformity.h"

#include "FairRootManager.h"
#include "CbmMCTrack.h"

#include "CbmEcalCell.h"
#include "CbmEcalPoint.h"
#include "CbmEcalStructure.h"
#include "CbmEcalCell.h"

#include "TTree.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"

#include <iostream>

using std::cout;
using std::cerr;
using std::endl;
using std::list;

/** Loop procedure **/
void CbmEcalAnalysisUniformity::Exec(Option_t* option)
{
  CbmEcalPoint* pt=(CbmEcalPoint*)fPoints->At(0);
  CbmEcalCell* c;
  list<CbmEcalCell*>::const_iterator p;
  list<CbmEcalCell*> cl;
  if (pt==NULL) return;

  InitTree();

  fX=pt->GetX();
  fY=pt->GetY();

//  c=fStr->GetCell(fX, fY);
//  fE=c->GetTotalEnergy();
//  c->GetNeighborsList(0, cl);
//  for(p=cl.begin();p!=cl.end();++p)
//    fE+=(*p)->GetTotalEnergy();

  fStr->GetCells(cl);
  fE=0;
  for(p=cl.begin();p!=cl.end();++p)
    fE+=(*p)->GetTotalEnergy();
    
  fTree->Fill();
}


/** Initializes tree **/
void CbmEcalAnalysisUniformity::InitTree()
{
  if (fTree) return;
  fTree=new TTree("uniformity","");
  fTree->Branch("x",&fX,"x/D");
  fTree->Branch("y",&fY,"y/D");
  fTree->Branch("e",&fE,"e/D");
}

CbmEcalAnalysisUniformity::CbmEcalAnalysisUniformity()
  : FairTask(),
    fTree(NULL),
    fEntries(0),
    fN(0),
    fE(0.),
    fX(0.),
    fY(0.),
    fStr(NULL),
    fPoints(NULL)
{
}

CbmEcalAnalysisUniformity::CbmEcalAnalysisUniformity(const char* name, const Int_t iVerbose)
  : FairTask(name, iVerbose),
    fTree(NULL),
    fEntries(0),
    fN(0),
    fE(0.),
    fX(0.),
    fY(0.),
    fStr(NULL),
    fPoints(NULL)
{
}

/** Initing routine **/
InitStatus CbmEcalAnalysisUniformity::Init()
{
  FairRootManager* fManager=FairRootManager::Instance();
  if (!fManager)
  {
    Fatal("Init", "Can't find a Root Manager.");
    return kFATAL;
  }
  fStr=(CbmEcalStructure*)fManager->GetObject("EcalStructure");
  if (!fStr)
  {
    Fatal("Init", "Can't find calorimeter structure in the system");
    return kFATAL;
  }
  fPoints=(TClonesArray*)fManager->GetObject("ECALPoint");
  if (!fPoints)
  {
    Fatal("Init","Can't find an array of calorimeter points.");
    return kFATAL;
  }
  fTree=NULL;

  return kSUCCESS;
}

/** Finishing routine **/
void CbmEcalAnalysisUniformity::Finish()
{
  if (fTree)
    fTree->Write();
}

ClassImp(CbmEcalAnalysisUniformity)
