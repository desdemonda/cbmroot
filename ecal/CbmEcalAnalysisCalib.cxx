#include "CbmEcalAnalysisCalib.h"

#include "FairRootManager.h"
#include "FairTrackParam.h"

#include "CbmEcalStructure.h"
#include "CbmEcalCell.h"
#include "CbmEcalPoint.h"

#include "TTree.h"
#include "TClonesArray.h"

#include <iostream>
#include <fstream>
#include <list>

using namespace std;

/** Loop procedure **/
void CbmEcalAnalysisCalib::Exec(Option_t* option)
{
  Int_t n=fTracks->GetEntries();
  Int_t i;
  FairTrackParam* t;
  CbmEcalCell* cell;
  CbmEcalCell* mcell;
  TVector3 m;
  list<CbmEcalCell*> cells;
  list<CbmEcalCell*>::const_iterator p;
//  TVector3 m1;

  fEv++;
  InitTree();

  for(i=0;i<n;i++)
  {
    t=(FairTrackParam*)fTracks->At(i);
    fX=t->GetX();
    fY=t->GetY();
    t->Momentum(m);
    fP=m.Mag();
    fPX=m.Px();
    fPY=m.Py();
    fPZ=m.Pz();

//    m1=m.Unit();
    cell=fStr->GetCell(fX, fY);
    if (!cell) continue;
    mcell=cell;
    cell->GetNeighborsList(0, cells);
    for(p=cells.begin();p!=cells.end();++p)
      if ((*p)->GetTotalEnergy()>mcell->GetTotalEnergy())
	mcell=(*p);

    mcell->GetNeighborsList(0, cells);
    for(p=cells.begin();p!=cells.end();++p)
      if ((*p)->GetTotalEnergy()>mcell->GetTotalEnergy())
	break;

    if (p!=cells.end()) continue;

    fCX=mcell->GetCenterX();
    fCY=mcell->GetCenterY();
    fCE=mcell->GetTotalEnergy();
    fCellNum=mcell->GetCellNumber();
    fADC=mcell->ADC();
    fOE=0.0;
    for(p=cells.begin();p!=cells.end();++p)
      fOE+=(*p)->GetTotalEnergy();

    fTree->Fill();
  }

}

void CbmEcalAnalysisCalib::InitTree()
{
  if (fTree) return;
  fTree=new TTree("calib", "calib");
  fTree->Branch("px", &fPX, "px/D");
  fTree->Branch("py", &fPY, "py/D");
  fTree->Branch("pz", &fPZ, "pz/D");
  fTree->Branch("p" , &fP , "p/D");
  fTree->Branch("x" , &fX , "x/D");
  fTree->Branch("y" , &fY , "y/D");
  fTree->Branch("cx", &fCX, "cx/D");
  fTree->Branch("cy", &fCY, "cy/D");
  fTree->Branch("ce", &fCE, "ce/D");
  fTree->Branch("oe", &fOE, "oe/D");
  fTree->Branch("ev", &fEv, "ev/I");
  fTree->Branch("cn", &fCellNum, "cn/I");
  fTree->Branch("adc", &fADC, "adc/I");
}

CbmEcalAnalysisCalib::CbmEcalAnalysisCalib(const char* name, const Int_t iVerbose)
  : FairTask(name, iVerbose),
    fTree(NULL),
    fX(0.),
    fY(0.),
    fCX(0.),
    fCY(0.),
    fP(0.),
    fCE(0.),
    fOE(0.),
    fPX(0.),
    fPY(0.),
    fPZ(0.),
    fEv(0),
    fCellNum(0),
    fADC(0),
    fStr(NULL),
    fTracks(NULL)
{
}

CbmEcalAnalysisCalib::CbmEcalAnalysisCalib()
  : FairTask(),
    fTree(NULL),
    fX(0.),
    fY(0.),
    fCX(0.),
    fCY(0.),
    fP(0.),
    fCE(0.),
    fOE(0.),
    fPX(0.),
    fPY(0.),
    fPZ(0.),
    fEv(0),
    fCellNum(0),
    fADC(0),
    fStr(NULL),
    fTracks(NULL)
{
}

/** Initing routine **/
InitStatus CbmEcalAnalysisCalib::Init()
{
  FairRootManager* fManager=FairRootManager::Instance();
  fStr=(CbmEcalStructure*)fManager->GetObject("EcalStructure");
  if (!fStr)
  {
    Fatal("Init()", "Can't find calorimeter structure. ");
    return kFATAL;
  }
  fTracks=(TClonesArray*)fManager->GetObject("EcalTrackParam");
  if (!fTracks)
  {
    Fatal("Init()", "Can't find array of reconstructed tracks. ");
    return kFATAL;
  }
  return kSUCCESS;
}

/** Finishing routine **/
void CbmEcalAnalysisCalib::Finish()
{
  if (fTree)
    fTree->Write();
}

ClassImp(CbmEcalAnalysisCalib)
