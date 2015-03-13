#include "CbmEcalShLibTable.h"

#include "CbmEcalShLibRecord.h"
#include "CbmEcalInf.h"

#include "FairRootManager.h"

#include "TSystem.h"
#include "TString.h"

#include <iostream>

using namespace std;

CbmEcalShLibTable::CbmEcalShLibTable(const char* name, Int_t verb)
  : CbmEcalShLib(name, verb),
    fFiles(),
    fRec()
{
  Int_t i;

  for(i=0;i<10;i++)
    fRec[i]=NULL;
}

void CbmEcalShLibTable::Add(const char* name)
{
  TString str=name;
  CbmEcalShLibRecord* rec;
  static CbmEcalInf* inf=CbmEcalInf::GetInstance(NULL);
  if (inf==NULL)
  {
    Fatal("CbmEcalShLibTable", "Can't find CbmEcalInf in the system.");
    return;
  }
  Double_t msize;
  Int_t num;

  gSystem->ExpandPathName(str);
  rec=new CbmEcalShLibRecord(str, fVerbose);
  msize=inf->GetModuleSize();
  num=(Int_t)((msize+0.001)/rec->CellSize());
  if (num<0||num>9)
  {
    Fatal("CbmEcalShLibTable", "Wrong cell number. CellSize is %f.", rec->CellSize());
    return;
  }
  fRec[num]=rec;
}

InitStatus CbmEcalShLibTable::Init()
{
  FairRootManager* fManager=FairRootManager::Instance();
  if (fManager==NULL)
  {
    Fatal("Init", "Can't find IOManager in the system.");
    return kFATAL;
  }
  fManager->Register(LibraryName(), "ECAL", this, kFALSE);
  for(list<TString>::const_iterator p=fFiles.begin(); p!=fFiles.end();++p)
    Add((*p).Data());

  return kSUCCESS;
}

Float_t CbmEcalShLibTable::GetResponse(Float_t x, Float_t y, Float_t phi, Float_t theta, Float_t e, Int_t celltype)
{
//  Info("Getresponse", "Here");
  if (fRec[celltype]==NULL)
    return -1111;
  Float_t tx=x;
  Float_t ty=y;
  Float_t tphi=phi;
  Float_t t;
  if (tphi>180.0)
  {
    tphi=360.0-tphi;
    ty=-ty;
  }
  if (tphi>90.0)
  {
    tphi=180.0-tphi;
    tx=-tx;
  }
  if (tphi>45.0)
  {
    tphi=90.0-tphi;
    t=tx;
    tx=ty;
    ty=t;
  }

  return fRec[celltype]->GetVal(tx, ty, tphi, theta, e)*e;
}

CbmEcalShLibTable::~CbmEcalShLibTable()
{
  Int_t i;
  for(i=0;i<10;i++)
    delete fRec[i];
}
ClassImp(CbmEcalShLibTable)
