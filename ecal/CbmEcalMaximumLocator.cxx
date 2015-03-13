#include "CbmEcalMaximumLocator.h"

#include "CbmEcalStructure.h"
#include "CbmEcalInf.h"
#include "CbmEcalMaximum.h"
#include "CbmEcalCell.h"

#include "FairRootManager.h"

#include "TClonesArray.h"

#include <list>
#include <iostream>

using namespace std;

CbmEcalMaximumLocator::CbmEcalMaximumLocator()
  : FairTask(),
    fMaximums(NULL),
    fStr(NULL),
    fECut(0.005),
    fEvent(-1)
{
}

CbmEcalMaximumLocator::CbmEcalMaximumLocator(const char* name, Int_t verbose)
  : FairTask(name, verbose),
    fMaximums(NULL),
    fStr(NULL),
    fECut(0.005),
    fEvent(-1)
{
}

CbmEcalMaximumLocator::~CbmEcalMaximumLocator()
{
  if (fMaximums)
    delete fMaximums;
}

InitStatus CbmEcalMaximumLocator::Init()
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
    Fatal("Init()", "Can't find calorimeter structure in the system.");
    return kFATAL;
  }
  fMaximums=new TClonesArray("CbmEcalMaximum", 3000);
  fManager->Register("EcalMaximums", "ECAL", fMaximums, kFALSE);
  fEvent=0;
  return kSUCCESS;
}

void CbmEcalMaximumLocator::Finish()
{
  fMaximums->Clear();
}

void CbmEcalMaximumLocator::Exec(const Option_t* opt)
{
  list<CbmEcalCell*> all;
  list<CbmEcalCell*> cells;
  list<CbmEcalCell*>::const_iterator p;
  list<CbmEcalCell*>::const_iterator r;
  Double_t e;
  Double_t z=fStr->GetEcalInf()->GetZPos();
  Double_t r1;
  Double_t r2;
  Double_t t;
  Int_t n=0;

  fEvent++;
  fMaximums->Clear();
  fStr->GetCells(all);
  for(p=all.begin();p!=all.end();++p)
  {
    e=(*p)->GetTotalEnergy();
    r1=(*p)->GetCenterX(); r1*=r1;
    t=(*p)->GetCenterY(); t*=t;
    r1=TMath::Sqrt(r1*r1+t*t);
    if (e<fECut)
      continue;
    (*p)->GetNeighborsList(0, cells);
    for(r=cells.begin();r!=cells.end();++r)
    {
      if ((*r)->GetTotalEnergy()<e) continue;
      if ((*r)->GetTotalEnergy()==e)
      {
        r2=(*r)->GetCenterX(); r2*=r2;
        t=(*r)->GetCenterY(); t*=t;
        r2=TMath::Sqrt(r2*r2+t*t);
	if (r1>=r2) continue;
      }
      break;
    }
    if (r!=cells.end())
      continue;
    new ((*fMaximums)[n++]) CbmEcalMaximum(*p, z);
  }
  if (fVerbose>9)
    Info("Exec", "%d maximums found", n);
}

ClassImp(CbmEcalMaximumLocator)
