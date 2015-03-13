#include "CbmEcalShLibRecord.h"

#include "TFile.h"
#include "TTree.h"
#include "TH2D.h"

#include <iostream>

using namespace std;

CbmEcalShLibRecord::CbmEcalShLibRecord(const char* filename, Int_t verb)
  : TObject(),
    fCellSize(0.),
    fNE(0),
    fE(NULL),
    fNTheta(0),
    fTheta(NULL),
    fNPhi(0),
    fPhi(NULL),
    fNX(0),
    fStepX(0.),
    fStartX(0.),
    fNY(0),
    fStepY(0.),
    fStartY(0.),
    fVerbose(verb),
    fN(0),
    fR(NULL)
{
  TFile* f=new TFile(filename);
  TTree* info;
  TTree* data;
  Float_t t;
  Int_t i;


  Info("CbmEcalShLibRecord", "Loading %s...", filename);
  if (f->IsZombie())
  {
    Fatal("CbmEcalShLibRecord", "File %s not found.", filename);
    return;
  }
  info=(TTree*)f->Get("info");
  info->SetBranchAddress("nx", &fNX);
  info->SetBranchAddress("startx", &fStartX);
  info->SetBranchAddress("stepx", &fStepX);
  info->SetBranchAddress("ny", &fNY);
  info->SetBranchAddress("starty", &fStartY);
  info->SetBranchAddress("stepy", &fStepY);
  info->SetBranchAddress("ne", &fNE);
  info->SetBranchAddress("nphi", &fNPhi);
  info->SetBranchAddress("ntheta", &fNTheta);
  info->SetBranchAddress("n", &fN);
  info->SetBranchAddress("cellsize", &fCellSize);
  info->GetEntry(0);
  if (fN!=fNX*fNY*fNE*fNPhi*fNTheta)
  {
    Fatal("CbmEcalShLibRecord", "Consistency check failed.");
    return;
  }
  fE=new Float_t[fNE];
  fTheta=new Float_t[fNTheta];
  fPhi=new Float_t[fNPhi];
  info->SetBranchAddress("e", fE);
  info->SetBranchAddress("phi", fPhi);
  info->SetBranchAddress("theta", fTheta);
  info->GetEntry(0);
  fR=new Float_t[fN];
  data=(TTree*)f->Get("shlib");
  data->SetBranchAddress("dta", &t);
  if (fN!=data->GetEntries())
  {
    Fatal("CbmEcalShLibRecord", "Info and data trees inconsitant.");
    return;
  }
  for(i=0;i<fN;i++)
  {
    data->GetEntry(i);
    fR[i]=t;
  }
  f->Close();

  if (fVerbose>99)
  {
    cout << "Information tree read." << endl;
    cout << "	n	= " << fN << endl;
    cout << "	cellsize= " << fCellSize << endl;
    cout << "	nx	= " << fNX << endl;
    cout << "	startx	= " << fStartX << endl;
    cout << "	stepx	= " << fStepX << endl;
    cout << "	ny	= " << fNY << endl;
    cout << "	starty	= " << fStartY << endl;
    cout << "	stepy	= " << fStepY << endl;
    cout << "	nphi	= " << fNPhi << " : (";
    for(i=0;i<fNPhi;i++)
      if (i!=0) cout << ", " << fPhi[i]; else cout << fPhi[i];
    cout << ")" << endl; 
    cout << "	ntheta	= " << fNTheta << " : (";
    for(i=0;i<fNTheta;i++)
      if (i!=0) cout << ", " << fTheta[i]; else cout << fTheta[i];
    cout << ")" << endl; 
    cout << "	ne	= " << fNE << " : (";
    for(i=0;i<fNE;i++)
      if (i!=0) cout << ", " << fE[i]; else cout << fE[i];
    cout << ")" << endl; 
  }
}

void CbmEcalShLibRecord::DrawTest(Float_t phi, Float_t theta, Float_t e)
{
  TH2D* h;
  Float_t x;
  Float_t y;
  Int_t ix;
  Int_t iy;
  h=new TH2D("ht","", fNX, fStartX, fStartX+fNX*fStepX, fNY, fStartY, fStartY+fStepY*fNY);
  h->SetStats(kFALSE);
  for(ix=0;ix<fNX;ix++)
  for(iy=0;iy<fNX;iy++)
  {
    x=(ix+0.5)*fStepX+fStartX;
    y=(iy+0.5)*fStepY+fStartY;
    h->SetBinContent(h->FindBin(x, y), GetVal(x, y, phi, theta, e));
  }
  h->DrawCopy("colz");
  delete h;
}
CbmEcalShLibRecord::~CbmEcalShLibRecord()
{
  delete fR;
  delete fE;
  delete fTheta;
  delete fPhi;
}

ClassImp(CbmEcalShLibRecord)
