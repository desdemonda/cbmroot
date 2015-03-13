#include "CbmEcalShLibCorrTable.h"

#include "CbmEcalShLibCorrRecord.h"
#include "CbmEcalInf.h"

#include "FairRootManager.h"

#include "TSystem.h"
#include "TString.h"

#include <iostream>

using namespace std;

CbmEcalShLibCorrTable::CbmEcalShLibCorrTable(const char* name, Int_t verb)
  : CbmEcalShLibCorr(name, verb),
    fFiles(),
    fRec()
{
  Int_t i;

  for(i=0;i<10;i++)
    fRec[i]=NULL;
}

void CbmEcalShLibCorrTable::Add(const char* name)
{
  TString str=name;
  CbmEcalShLibCorrRecord* rec;
  static CbmEcalInf* inf=CbmEcalInf::GetInstance(NULL);
  if (inf==NULL)
  {
    Fatal("CbmEcalShLibCorrTable", "Can't find CbmEcalInf in the system.");
    return;
  }
  Double_t msize;
  Int_t num;

  gSystem->ExpandPathName(str);
  rec=new CbmEcalShLibCorrRecord(str, fVerbose);
  msize=inf->GetModuleSize();
  num=(Int_t)((msize+0.001)/rec->CellSize());
  if (num<0||num>9)
  {
    Fatal("CbmEcalShLibCorrTable", "Wrong cell number. CellSize is %f.", rec->CellSize());
    return;
  }
  fRec[num]=rec;
}

InitStatus CbmEcalShLibCorrTable::Init()
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

Float_t CbmEcalShLibCorrTable::GetResponse(Float_t x, Float_t y, Float_t phi, Float_t theta, Float_t e, Int_t celltype)
{
//  Info("Getresponse", "Here");
  if (fRec[celltype]==NULL)
    return -1111;
  static CbmEcalInf* inf=CbmEcalInf::GetInstance(NULL);
  static Float_t msize=inf->GetModuleSize();
  Float_t csize=msize/celltype;
  Float_t tx=x;
  Float_t ty=y;
  Float_t tphi=phi;
  Float_t t;
  if (tphi>180.0)
  {
    tphi=360.0-tphi;
    ty=-ty; ty-=csize;
  }
  if (tphi>90.0)
  {
    tphi=180.0-tphi;
    tx=-tx; tx-=csize;
  }
  if (tphi>45.0)
  {
    tphi=90.0-tphi;
    t=tx;
    tx=ty;
    ty=t;
  }

  return fRec[celltype]->Energy(tx, ty, tphi, theta, e);
}

Float_t CbmEcalShLibCorrTable::Correlation(Float_t x1, Float_t y1, Float_t x2, Float_t y2, Float_t phi, Float_t theta, Float_t e, Int_t celltype)
{
  if (fRec[celltype]==NULL)
    return -1111;
  static CbmEcalInf* inf=CbmEcalInf::GetInstance(NULL);
  static Float_t msize=inf->GetModuleSize();
  Float_t csize=msize/celltype;
  Float_t tx1=x1;
  Float_t tx2=x2;
  Float_t ty1=y1;
  Float_t ty2=y2;
  Float_t tphi=phi;
  Float_t t;
  if (tphi>180.0)
  {
    tphi=360.0-tphi;
    ty1=-ty1; ty1-=csize;
    ty2=-ty2; ty2-=csize;
  }
  if (tphi>90.0)
  {
    tphi=180.0-tphi;
    tx1=-tx1; tx1-=csize;
    tx2=-tx2; tx2-=csize;
  }
  if (tphi>45.0)
  {
    tphi=90.0-tphi;
    t=tx1; tx1=ty1; ty1=t;
    t=tx2; tx2=ty2; ty2=t;
  }

  t=fRec[celltype]->Correlation(tx1, ty1, tx2, ty2, tphi, theta, e);

//TODO!!!
  return t;
}


Float_t CbmEcalShLibCorrTable::Correlation2(Float_t x1, Float_t y1, Float_t x2, Float_t y2, Float_t phi, Float_t theta, Float_t e, Int_t celltype)
{
  if (fRec[celltype]==NULL)
    return -1111;
  if (fRec[celltype/2]==NULL)
    return -1111;
  static CbmEcalInf* inf=CbmEcalInf::GetInstance(NULL);
  static Float_t msize=inf->GetModuleSize();
  Float_t csize=msize/celltype;
  Float_t tx1=x1;
  Float_t tx2=x2;
  Float_t ty1=y1;
  Float_t ty2=y2;
  Float_t tphi=phi;
  Float_t t;
  Float_t d=inf->GetModuleSize()/celltype;
  Float_t tx3=x2+d;
  Float_t ty3=y2+d;
  if (tphi>180.0)
  {
    tphi=360.0-tphi;
    ty1=-ty1; ty1-=csize;
    ty2=-ty2; ty2-=csize;
    ty3=-ty3; ty3-=csize;
  }
  if (tphi>90.0)
  {
    tphi=180.0-tphi;
    tx1=-tx1; tx1-=csize;
    tx2=-tx2; tx2-=csize;
    tx3=-tx3; tx3-=csize;
  }
  if (tphi>45.0)
  {
    tphi=90.0-tphi;
    t=tx1; tx1=ty1; ty1=t;
    t=tx2; tx2=ty2; ty2=t;
    t=tx3; tx3=ty3; ty3=t;
  }

//  cout << "(" << x1 << "," << y1 << ");(" << x2 << "," << y2 << ")";
//  cout << ":phi=" << phi << endl;
//  cout << "(" << tx1 << "," << ty1 << ");(" << tx2 << "," << ty2 << ")" << endl;
//  cout << ":" << tx3 << "," << ty3 << endl;
  t =fRec[celltype]->Correlation(tx1, ty1, tx2, ty2, tphi, theta, e);
  t+=fRec[celltype]->Correlation(tx1, ty1, tx3, ty2, tphi, theta, e);
  t+=fRec[celltype]->Correlation(tx1, ty1, tx2, ty3, tphi, theta, e);
  t+=fRec[celltype]->Correlation(tx1, ty1, tx3, ty3, tphi, theta, e);

//TODO!!!
  return t;
}

CbmEcalShLibCorrTable::~CbmEcalShLibCorrTable()
{
  Int_t i;
  for(i=0;i<10;i++)
    delete fRec[i];
}

ClassImp(CbmEcalShLibCorrTable)
