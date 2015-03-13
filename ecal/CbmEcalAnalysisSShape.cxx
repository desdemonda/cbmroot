#include "CbmEcalAnalysisSShape.h"

#include "FairRootManager.h"

#include "CbmEcalStructure.h"
#include "CbmEcalPoint.h"
#include "CbmEcalCell.h"
#include "CbmEcalInf.h"

#include "TMath.h"
#include "TClonesArray.h"
#include "TTree.h"
#include "TVector3.h"
#include "TVector2.h"

#include <iostream>
#include <list>

using namespace std;

#define PAR1 0.38
#define PAR2 30.8
#define PAR3 12.0
#define PARI 7
/** Loop procedure **/
void CbmEcalAnalysisSShape::Exec(Option_t* option)
{
  fEv++;
  cout << "Event " << fEv << endl;
  InitTree();
  InitVariables();

  CbmEcalPoint* pt=(CbmEcalPoint*)fMCPoints->At(0);
  TVector3 tp;
  if (pt)
  {
    fTX=pt->GetX();
    fTY=pt->GetY();
    pt->Momentum(tp);
    fTPhi=TMath::ATan2(tp.Y(), tp.X())*TMath::RadToDeg();
    fTTheta=TMath::ATan2(tp.Z(), tp.Pt())*TMath::RadToDeg();
  }

  CbmEcalCell* cell=fStr->GetCell(fTX, fTY);
  static CbmEcalInf* fInf=fStr->GetEcalInf();
  Double_t d=fInf->GetModuleSize()/cell->GetType();
  Int_t ix;
  Int_t iy;
  Int_t n;
  CbmEcalCell* cl;
  for(iy=0;iy<7;iy++)
  for(ix=0;ix<7;ix++)
  {
    cl=fStr->GetCell(fTX+d*(ix-2), fTY+d*(iy-2));
    if (!cl) continue;
    n=ix+iy*7;
    fX[n]=cl->GetCenterX();
    fY[n]=cl->GetCenterY();
    fE[n]=cl->GetTotalEnergy();
  }
  //TODO: cluster information


  list<CbmEcalCell*> cells;
  list<CbmEcalCell*>::const_iterator p;
  Double_t maxe=-1111;

  fStr->GetCells(cells);
  for(p=cells.begin();p!=cells.end();++p)
  {
    if (maxe<(*p)->GetTotalEnergy())
    {
      cell=(*p);
      maxe=cell->GetTotalEnergy();
    }
  }
  fMaxE=maxe;
  fMaxX=cell->GetCenterX();
  fMaxY=cell->GetCenterY();
  FillClusterInfo(cell);

  if (fTree)
    fTree->Fill();
}

void CbmEcalAnalysisSShape::FillClusterInfo(CbmEcalCell* cell)
{
  Int_t i;
  Int_t j;
  Int_t k;
  Double_t max=-1111;
  Double_t e2;
  Double_t imax;
  list<CbmEcalCell*> cells;
  list<CbmEcalCell*>::const_iterator p;
  Double_t x;
  Double_t y;
  CbmEcalCell* cls[PARI];
  Double_t rc[PARI];
  Int_t cn[PARI];
  Double_t r;
  CbmEcalCell* tc;
  TVector2 v;

  for(i=1;i<5;i++)
  {
    cell->GetNeighborsList(i, cells);
    e2=cell->GetTotalEnergy();
    for(p=cells.begin();p!=cells.end();++p)
      e2+=(*p)->GetTotalEnergy();
    if (e2>max)
    {
      max=e2;
      imax=i;
    }
  }
  e2=cell->GetTotalEnergy();
  x=e2*cell->GetCenterX();
  y=e2*cell->GetCenterY();
  cell->GetNeighborsList(imax, cells);
  for(p=cells.begin();p!=cells.end();++p)
  {
    x+=(*p)->GetCenterX()*(*p)->GetTotalEnergy();
    y+=(*p)->GetCenterY()*(*p)->GetTotalEnergy();    
  }
  x/=max;
  y/=max;

  fCX=x; fCY=y;
  for(i=0;i<49;i++)
  {
    v.Set(fX[i]-x, fY[i]-y);
    v=v.Rotate(-fTPhi*TMath::DegToRad());
    fP[i]=v.X(); fQ[i]=v.Y();
  }

  for(i=0;i<PARI;i++)
  {
    cls[i]=NULL;
    rc[i]=-1111;
    cn[i]=-1111;
  }

  for(k=0;k<49;k++)
  {
    tc=fStr->GetCell(fX[k], fY[k]);
    if (!tc) continue;
    r=SolveEllipse(tc, x, y);
    for(i=0;i<PARI;i++)
      if (rc[i]<r)
	break;
    if (i==PARI) continue;
    for(j=PARI-1;j>i;j--)
    {
      rc[j]=rc[j-1];
      cls[j]=cls[j-1];
      cn[j]=cn[j-1];
    }
    rc[i]=r;
    cls[i]=tc;
    cn[i]=k;
  }

  for(i=0;i<PARI;i++)
    fCls[cn[i]]=1;
}

Double_t CbmEcalAnalysisSShape::SolveEllipse(CbmEcalCell* cell, Double_t cx, Double_t cy)
{
  Double_t cRx=PAR2;		//48*1.6
  Double_t cRy=PAR3;
  Int_t cSt=100;
  TVector2 centr(cx, cy);
  TVector2 t(centr);
  Double_t gm=fTPhi*TMath::DegToRad();
  Double_t sgm=TMath::Sin(gm);
  Double_t cgm=TMath::Cos(gm);
  Double_t x;
  Double_t y;
  Double_t p=sgm*sgm/cRx+cgm*cgm/cRy;
  Double_t q=cgm*sgm*(1.0/cRx-1.0/cRy);
  Double_t r=cgm*cgm/cRx+sgm*sgm/cRy;
  Double_t d;
  Double_t ex;
  Double_t ey;
  Double_t y1;
  Double_t y2;
  Double_t inte=0;
  Double_t step=(cell->X2()-cell->X1())/cSt;
  Double_t fx=step/2.0+cell->X1();
  Int_t i;
  
// cout << fCurAlgo << " " << reg << " " << fPar1[reg] << " " << cRx << " " << cRy << endl;
  t/=t.Mod(); centr+=t*TMath::Sqrt(PAR1);
  for(i=0;i<cSt;i++)
  {
    x=step; x*=i; x+=fx;
    ex=x; ex-=centr.X();
    d=q*q*ex*ex-p*(r*ex*ex-1);
    if (d<0) continue;
    d=TMath::Sqrt(d);
    y1=-q*ex/p; y2=y1;
    y1-=d/p; y2+=d/p;
    y1+=centr.Y(); y2+=centr.Y();
    if (y1>cell->Y2()) continue;
    if (y2<cell->Y1()) continue;
    if (y1<cell->Y1()) y1=cell->Y1();
    if (y2>cell->Y2()) y2=cell->Y2();
    inte+=(y2-y1)*step;
  }

  return inte;
}


CbmEcalAnalysisSShape::CbmEcalAnalysisSShape()
  : FairTask(),
    fTree(NULL),
    fE(),
    fX(),
    fY(),
    fP(),
    fQ(),
    fCls(),
    fMaxE(0.),
    fMaxX(0.),
    fMaxY(0.),
    fTX(0.),
    fTY(0.),
    fCX(0.),
    fCY(0.),
    fTTheta(0.),
    fTPhi(0.),
    fEv(0),
    fMCPoints(NULL),
    fStr(NULL)
{
}


CbmEcalAnalysisSShape::CbmEcalAnalysisSShape(const char* name, const Int_t iVerbose)
  : FairTask(name, iVerbose),
    fTree(NULL),
    fE(),
    fX(),
    fY(),
    fP(),
    fQ(),
    fCls(),
    fMaxE(0.),
    fMaxX(0.),
    fMaxY(0.),
    fTX(0.),
    fTY(0.),
    fCX(0.),
    fCY(0.),
    fTTheta(0.),
    fTPhi(0.),
    fEv(0),
    fMCPoints(NULL),
    fStr(NULL)
{
}

/** Initing routine **/
InitStatus CbmEcalAnalysisSShape::Init()
{
  FairRootManager* fManager=FairRootManager::Instance();
  fMCPoints=(TClonesArray*)fManager->GetObject("EcalPoint");
  if (fMCPoints==NULL)
  {
    cerr << "There are no EcalPoint branch in the file!!!" << endl;
    return kFATAL; 
  }
  fStr=(CbmEcalStructure*)fManager->GetObject("EcalStructure");
  if (!fStr) 
  {
    Fatal("Init()", "Can't find calorimeter structure in the system.");
    return kFATAL;
  }
  

  fTree=NULL;

  return kSUCCESS;
}

void CbmEcalAnalysisSShape::InitTree()
{
  if (fTree) return;
  fTree=new TTree("ssh","shower shape analysis");
  fTree->Branch("ev", &fEv, "ev/I");
  fTree->Branch("e", fE, "e[49]/D");
  fTree->Branch("x", fX, "x[49]/D");
  fTree->Branch("y", fY, "y[49]/D");
  fTree->Branch("p", fP, "p[49]/D");
  fTree->Branch("q", fQ, "q[49]/D");
  fTree->Branch("cls", fCls, "cls[49]/I");
  fTree->Branch("maxe", &fMaxE, "maxe/D");
  fTree->Branch("maxx", &fMaxX, "maxx/D");
  fTree->Branch("maxy", &fMaxY, "maxy/D");
  fTree->Branch("tx", &fTX, "tx/D");
  fTree->Branch("ty", &fTY, "ty/D");
  fTree->Branch("cx", &fCX, "cx/D");
  fTree->Branch("cy", &fCY, "cy/D");
  fTree->Branch("ttheta", &fTTheta, "ttheta/D");
  fTree->Branch("tphi", &fTPhi, "tphi/D");
}

void CbmEcalAnalysisSShape::InitVariables()
{
  Int_t i;

  for(i=0;i<49;i++)
  {
    fE[i]=-1111;
    fX[i]=-1111;
    fY[i]=-1111;
    fP[i]=-1111;
    fQ[i]=-1111;
    fCls[i]=-1111;
  }
  fCX=-1111;
  fCY=-1111;
  fTX=-1111;
  fTY=-1111;
  fMaxX=-1111;
  fMaxY=-1111;
  fMaxE=-1111;
}
/** Finishing routine **/
void CbmEcalAnalysisSShape::Finish()
{
  if (fTree)
    fTree->Write();
}

ClassImp(CbmEcalAnalysisSShape)
