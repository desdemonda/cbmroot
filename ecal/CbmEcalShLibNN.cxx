#include "CbmEcalShLibNN.h"

#include "CbmEcalInf.h"

#include "FairRootManager.h"

#include "TSystem.h"
#include "TString.h"
#include "TTree.h"
#include "TMath.h"
#include "TMultiLayerPerceptron.h"

#include <iostream>

using namespace std;

CbmEcalShLibNN::CbmEcalShLibNN(const char* name, Int_t verb)
  : CbmEcalShLib(name, verb),
    fRec(), fTr()
{
  Int_t i;

  for(i=0;i<10;i++)
  {
    fRec[i]=NULL;
    fTr[i]=0;
  }
}

void CbmEcalShLibNN::AddRecord(const char* name, const char* fname, Int_t celltype)
{
  if (celltype>9||celltype<1)
  {
    Fatal("CbmEcalShLibNN", "Bad celltype: %d", celltype);
    return;
  }

  TString str=fname;
  TTree* tr=new TTree(name, "Temporary tree. Should no be saved");
  Float_t x;
  Float_t y;
  Float_t theta;
  Float_t e;
  Float_t phi;
  Float_t resp;
  TMultiLayerPerceptron* net;

  tr->Branch("x", &x, "x/F");
  tr->Branch("y", &y, "y/F");
  tr->Branch("e", &x, "e/F");
  tr->Branch("theta", &theta, "theta/F");
  tr->Branch("phi", &phi, "phi/F");
  tr->Branch("resp", &resp, "resp/F");

  gSystem->ExpandPathName(str);
  net=new TMultiLayerPerceptron("x,y,psi,theta,e:10:25:resp", tr, "1", "", TNeuron::kGauss);
  net->LoadWeights(str);

  fRec[celltype]=net;
  fTr[celltype]=tr;
}

InitStatus CbmEcalShLibNN::Init()
{
  FairRootManager* fManager=FairRootManager::Instance();
  if (fManager==NULL)
  {
    Fatal("Init", "Can't find IOManager in the system.");
    return kFATAL;
  }
  fManager->Register(LibraryName(), "ECAL", this, kFALSE);

  return kSUCCESS;
}

Float_t CbmEcalShLibNN::GetResponse(Float_t x, Float_t y, Float_t phi, Float_t theta, Float_t e, Int_t celltype)
{
//  Info("Getresponse", "Here");
  if (fRec[celltype]==NULL)
    return -1111;
  Double_t tx=x;
  Double_t ty=y;
  Double_t tphi=phi;
  Double_t t;
  Double_t par[5];
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

  par[0]=tx; par[1]=ty; par[2]=tphi*TMath::DegToRad(); par[3]=theta*TMath::DegToRad(); par[5]=e;
  return fRec[celltype]->Evaluate(0, par)*e;
}

CbmEcalShLibNN::~CbmEcalShLibNN()
{
  Int_t i;
  for(i=0;i<10;i++)
    delete fRec[i];
}
ClassImp(CbmEcalShLibNN)
