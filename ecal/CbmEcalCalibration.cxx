#include "CbmEcalCalibration.h"

#include "CbmEcalCell.h"
#include "CbmEcalInf.h"
#include "CbmEcalStructure.h"
#include "CbmEcalParam.h"

#include "FairRootManager.h"

#include <list>
#include <iostream>

using namespace std;

/** Standard constructor **/
CbmEcalCalibration::CbmEcalCalibration(const char *name, const Int_t iVerbose, const  char* parname)
  : FairTask(name, iVerbose),
    fParName(parname),
    fP0a(),
    fP0b(),
    fP1a(),
    fP1b(),
    fInf(NULL)
{
  /**TODO: Need to read parameters from a parname file! **/
  fP0a[1]=0.003145;
  fP0a[2]=0.003143;
  fP0a[4]=0.003457;
  fP0b[1]=0.08834;
  fP0b[2]=0.08558;
  fP0b[4]=0.07944;

  fP1a[1]=0.002642;
  fP1a[2]=0.003136;
  fP1a[4]=0.004906;
  fP1b[1]=-0.004985;
  fP1b[2]=-0.005067;
  fP1b[4]=-0.008054;
}

/** Default constructor **/
CbmEcalCalibration::CbmEcalCalibration()
  : FairTask(),
    fParName(),
    fP0a(),
    fP0b(),
    fP1a(),
    fP1b(),
    fInf(NULL)
{
}

InitStatus CbmEcalCalibration::Init()
{
  Int_t i;
  TString tmp;

  CbmEcalParam* par=new CbmEcalParam("calibration", fParName);
  for(i=1;i<5;i++)
  {
    tmp="p0a["; tmp+=i; tmp+="]"; fP0a[i]=par->GetDouble(tmp);
    tmp="p0b["; tmp+=i; tmp+="]"; fP0b[i]=par->GetDouble(tmp);
    tmp="p1a["; tmp+=i; tmp+="]"; fP1a[i]=par->GetDouble(tmp);
    tmp="p1b["; tmp+=i; tmp+="]"; fP1b[i]=par->GetDouble(tmp);
  }
  delete par;

  FairRootManager* io=FairRootManager::Instance();

  if (!io)
  {
    Fatal("Init()", "Can't find FairRootManager.");
    return kFATAL;
  }
  CbmEcalStructure* fStr=(CbmEcalStructure*)io->GetObject("EcalStructure");
  if (!fStr) 
  {
    Fatal("Init()", "Can't find calorimeter structure in the system.");
    return kFATAL;
  }
  fInf=fStr->GetEcalInf();

  io->Register("EcalCalibration", "ECAL", this, kFALSE);
  return kSUCCESS;
}

/** A framework requirement **/
void CbmEcalCalibration::Finish()
{
  ;
}

void CbmEcalCalibration::Exec(const Option_t* option)
{
  ;
}

/** Get energy with standard 2x2 procedure **/
Double_t CbmEcalCalibration::GetEnergy(const CbmEcalCell* cell)
{
  Double_t e2=0;
  Int_t i;
  list<CbmEcalCell*> tmp;
  list<CbmEcalCell*>::const_iterator p;
  Double_t e;

  for(i=1;i<5;i++)
  {
    cell->GetNeighborsList(i, tmp);
    e=cell->GetTotalEnergy();
    for(p=tmp.begin();p!=tmp.end();++p)
      e+=(*p)->GetTotalEnergy();
    if (e>e2) e2=e;
  }
  return GetEnergy(e2, cell);
}

Double_t CbmEcalCalibration::GetEnergy(Double_t e2, const CbmEcalCell* cell) 
{
  Double_t x=cell->GetCenterX();
  Double_t y=cell->GetCenterY();
  Double_t tantheta=TMath::Sqrt(x*x+y*y)/fInf->GetZPos();

  return GetEnergy(e2, tantheta, cell->GetType());  
}

CbmEcalCalibration::~CbmEcalCalibration()
{
  ;
}

ClassImp(CbmEcalCalibration)
