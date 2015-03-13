#include "CbmEcalCalibrationV2.h"

#include "CbmEcalCell.h"
#include "CbmEcalInf.h"
#include "CbmEcalStructure.h"
#include "CbmEcalParam.h"

#include "FairRootManager.h"

#include <list>
#include <iostream>

using namespace std;

/** Standard constructor **/
CbmEcalCalibrationV2::CbmEcalCalibrationV2(const char *name, const Int_t iVerbose, const  char* parname)
  : CbmEcalCalibration(name, iVerbose, parname),
    fParName(""),
    fContName(""),
    fP(NULL),
    fMethods(0),
    fMethodB(NULL),
    fInf(NULL)   
{
}

/** Default constructor **/
CbmEcalCalibrationV2::CbmEcalCalibrationV2()
  : CbmEcalCalibration(),
    fParName(""),
    fContName(""),
    fP(NULL),
    fMethods(0),
    fMethodB(NULL),
    fInf(NULL)   
{
}

InitStatus CbmEcalCalibrationV2::Init()
{
  Int_t p;
  Int_t m;
  Int_t n;
  TString tmp;

  CbmEcalParam* par=new CbmEcalParam("calibration", ParName());
  fMethods=par->GetInteger("methods");
  fMethodB=new Double_t[fMethods];
  fP=new Double_t[fMethods*9];
  for(m=0;m<fMethods;m++)
  {
    tmp="methodb["; tmp+=m; tmp+="]";
    fMethodB[m]=par->GetDouble(tmp);
  }

  for(m=0;m<fMethods;m++)
  for(p=0;p<3;p++)
  for(n=0;n<3;n++)
  {
    tmp="p"; tmp+=p; tmp+="m"; tmp+=m; tmp+="["; tmp+=n; tmp+="]";
    fP[N(p,m,n)]=par->GetDouble(tmp);
//    cout << tmp+"=" << par->GetDouble(tmp) << endl;
  }
  fContName=par->GetString("containername");

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

  io->Register(fContName, "ECAL", this, kFALSE);
  return kSUCCESS;
}

/** A framework requirement **/
void CbmEcalCalibrationV2::Finish()
{
  ;
}

void CbmEcalCalibrationV2::Exec(const Option_t* option)
{
  ;
}

/** Get energy with standard 2x2 procedure **/
Double_t CbmEcalCalibrationV2::GetEnergy(const CbmEcalCell* cell)
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

Double_t CbmEcalCalibrationV2::GetEnergy(Double_t Ecls, const CbmEcalCell* cell) 
{
  Double_t x=cell->GetCenterX();
  Double_t y=cell->GetCenterY();
  Double_t tantheta=TMath::Sqrt(x*x+y*y)/fInf->GetZPos();

//  cout << "--> "<< Ecls << " " << x << " " << y << " " << tantheta << " " << GetEnergy(Ecls, tantheta, cell->GetType()) << endl;
  return GetEnergy(Ecls, tantheta, cell->GetType());  
}

CbmEcalCalibrationV2::~CbmEcalCalibrationV2()
{
  delete fP;
  delete fMethodB;
}

ClassImp(CbmEcalCalibrationV2)
