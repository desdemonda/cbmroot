#include "CbmEcalPosLib.h"

#include "CbmEcalStructure.h"
#include "CbmEcalInf.h"
#include "CbmEcalParam.h"

#include "FairRootManager.h"

#include <iostream>

using namespace std;

CbmEcalPosLib::CbmEcalPosLib()
  : FairTask(),
    fX(0.),
    fY(0.),
    fParName(""),
    fContName(""),
    fF0P0(0.),
    fF1P0(0.),
    fF2P0(0.),
    fF0P1(0.),
    fF1P1(0.),
    fF2P1(0.),
    fF0P2(0.),
    fF1P2(0.),
    fF2P2(0.),
    fInf(NULL)
{
}


CbmEcalPosLib::CbmEcalPosLib(const char* nm, Int_t verb, const char* fname)
  : FairTask(nm, verb),
    fX(0.),
    fY(0.),
    fParName(fname),
    fContName(""),
    fF0P0(0.),
    fF1P0(0.),
    fF2P0(0.),
    fF0P1(0.),
    fF1P1(0.),
    fF2P1(0.),
    fF0P2(0.),
    fF1P2(0.),
    fF2P2(0.),
    fInf(NULL)
{
}

void CbmEcalPosLib::Exec(const Option_t* option)
{
  ;
}

InitStatus CbmEcalPosLib::Init()
{
  FairRootManager* io=FairRootManager::Instance();
  CbmEcalParam* par=new CbmEcalParam("calibration", fParName);

  fF0P0=par->GetDouble("f0p0");
  fF1P0=par->GetDouble("f1p0");
  fF2P0=par->GetDouble("f2p0");
  fF0P1=par->GetDouble("f0p1");
  fF1P1=par->GetDouble("f1p1");
  fF2P1=par->GetDouble("f2p1");
  fF0P2=par->GetDouble("f0p2");
  fF1P2=par->GetDouble("f1p2");
  fF2P2=par->GetDouble("f2p2");

  fContName=par->GetString("containername");

  delete par;

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

void CbmEcalPosLib::GetXY(Double_t e, Double_t x, Double_t y)
{
  static Double_t z=fInf->GetZPos();
  Double_t r=TMath::Sqrt(x*x+y*y);
  Double_t theta=TMath::ATan2(r, z)*TMath::RadToDeg();
  Double_t dr=GetDR(e, theta);
  theta=TMath::ATan2(r+dr, z)*TMath::RadToDeg();
  dr=GetDR(e, theta);
  Double_t phi=TMath::ATan2(y, x);
  fX=x; fX+=TMath::Cos(phi)*dr;
  fY=y; fY+=TMath::Sin(phi)*dr;
}

void CbmEcalPosLib::Finish()
{
  ;
}

CbmEcalPosLib::~CbmEcalPosLib()
{
  ;
}

ClassImp(CbmEcalPosLib)
