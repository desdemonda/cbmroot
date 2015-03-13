/* $Id: CbmEcalSCurveLib.cxx,v 1.1 2006/09/18 07:58:04 prokudin Exp $ */
/*
 * $Log: CbmEcalSCurveLib.cxx,v $
 * Revision 1.1  2006/09/18 07:58:04  prokudin
 * First implementation of SCurve library
 *
 */

#include "CbmEcalSCurveLib.h"

#include "CbmEcalInf.h"
#include "CbmEcalCell.h"
#include "CbmEcalSCurveLibRecord.h"

#include "FairRootManager.h"

#include "TMath.h"
#include "TSystem.h"

#include <iostream>

using std::cerr;
using std::endl;
using std::list;

CbmEcalSCurveLib::CbmEcalSCurveLib(const char* name, Int_t verbose) 
  : FairTask(name, verbose), 
    fFiles(),
    fVerbose(verbose),
    fModuleSize(0.),
    fSize(10),
    fRec(new CbmEcalSCurveLibRecord*[fSize])
{
  Int_t i;

  for(i=0;i<fSize;i++)
    fRec[i]=NULL;
  fFiles.clear();
}

void CbmEcalSCurveLib::GetModuleSize()
{
  if (fModuleSize!=0)
    return;
  CbmEcalInf* inf=CbmEcalInf::GetInstance(NULL);
  if (inf==NULL)
  {
    Fatal("GetModuleSize","Can't find CbmEcalInf object!");
    return;
  }
  fModuleSize=inf->GetModuleSize();
}

void CbmEcalSCurveLib::AddFile(const char* file)
{
  fFiles.push_back(file);
}

void CbmEcalSCurveLib::Add(const char* file)
{
  Int_t num;
  TString fname=file;
  gSystem->ExpandPathName(fname);
  CbmEcalSCurveLibRecord* rec=new CbmEcalSCurveLibRecord(fname);
  if (rec->GetCellSize()==0)
  {
    delete rec;
    return;
  }
  GetModuleSize();
  if (fVerbose>9)
    Info("AddFile", "Initing for cellsize %f.", rec->GetCellSize());
  num=(Int_t)((fModuleSize+0.0001)/rec->GetCellSize());
  fRec[num]=rec;
  if (fVerbose>0)
    Info("AddFile", "Inited for cell type %d", num);
}

InitStatus CbmEcalSCurveLib::Init()
{
  FairRootManager* fManager=FairRootManager::Instance();
  if (fManager==NULL)
  {
    Fatal("Init", "Can't find IOManager.");
    return kFATAL;
  }
  fManager->Register("EcalSCurveLib", "ECAL", this, kFALSE);
  for(list<TString>::const_iterator p=fFiles.begin(); p!=fFiles.end();++p)
    Add((*p).Data());

  return kSUCCESS;
}

void CbmEcalSCurveLib::Exec(Option_t* exec)
{
  ;
}

void CbmEcalSCurveLib::Finish()
{
  ;
}

void CbmEcalSCurveLib::GetCoord(Float_t e, CbmEcalCell* cell, Float_t& x, Float_t& y) const
{
  list<CbmEcalCell*> cells;
  list<CbmEcalCell*>::const_iterator p;
  Float_t ax=0;
  Float_t ay=0;
  Float_t en=cell->GetEnergy();

  Int_t type=cell->GetType();
  cell->GetNeighborsList(0, cells);
  x=0; y=0;
  for(p=cells.begin();p!=cells.end();++p) 
  {
    if (type!=(*p)->GetType())
    {
      x=-1111;
      y=-1111;
      return;
    } else
    {
      if (cell->GetCenterX()+0.001<(*p)->GetCenterX())
        ax+=(*p)->GetEnergy();
      else
      if (cell->GetCenterX()-0.001>(*p)->GetCenterX())
        ax-=(*p)->GetEnergy();
      if (cell->GetCenterY()+0.001<(*p)->GetCenterY())
        ay+=(*p)->GetEnergy();
      else
      if (cell->GetCenterY()-0.001>(*p)->GetCenterY())
        ay-=(*p)->GetEnergy();
    }
    en+=(*p)->GetEnergy();
  }
  ax/=en;
  ay/=en;
  x=GetX(ax, e, cell);
  y=GetX(ay, e, cell);
//  x+=cell->GetCenterX();
//  y+=cell->GetCenterY();
}
Float_t CbmEcalSCurveLib::GetY(Float_t a, Float_t e, CbmEcalCell* cell) const
{
  if (!cell) return -1111;
  CbmEcalInf* inf=CbmEcalInf::GetInstance(NULL);
  if (inf==NULL) return -1111;
  Int_t type=cell->GetType();
  if (type>=fSize||type<=0) return -1111;
  if (!fRec[type]) return -1111;

  Float_t z=inf->GetZPos();
  Float_t y=cell->GetCenterY();
  Float_t theta=TMath::ATan(y/z)*TMath::RadToDeg();
  return GetX(a, e, theta, type);
}

Float_t CbmEcalSCurveLib::GetX(Float_t a, Float_t e, CbmEcalCell* cell) const
{
  if (!cell) return -1111;
  CbmEcalInf* inf=CbmEcalInf::GetInstance(NULL);
  if (inf==NULL) return -1111;
  Int_t type=cell->GetType();
  if (type>=fSize||type<=0) return -1111;
  if (!fRec[type]) return -1111;

  Float_t z=inf->GetZPos();
  Float_t x=cell->GetCenterX();
  Float_t theta=TMath::ATan(x/z)*TMath::RadToDeg();
  return GetX(a, e, theta, type);
}

Float_t CbmEcalSCurveLib::GetX(Float_t a, Float_t e, Float_t theta, Int_t type) const
{
  if (type>=fSize||type<=0) return -1111;
  if (!fRec[type]) return -1111;
  if (theta<0)
  {
    Float_t res=fRec[type]->GetX(-a,e,-theta);
    if (res==-1111)
      return res;
    return -res;
  }
  return fRec[type]->GetX(a,e,theta);
}

CbmEcalSCurveLib::~CbmEcalSCurveLib()
{
  for(Int_t i=0;i<fSize;i++)
    if (fRec[i]) delete fRec;
  delete fRec;
}

ClassImp(CbmEcalSCurveLib)
