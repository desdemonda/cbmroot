#include "CbmEcalAnalysisCluster.h"

#include "FairRootManager.h"
#include "CbmMCTrack.h"

#include "CbmEcalStructure.h"
#include "CbmEcalInf.h"
#include "CbmEcalCell.h"
#include "CbmEcalPoint.h"
#include "CbmEcalParam.h"

#include "TTree.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TVector2.h"

#include <iostream>

using std::cout;
using std::cerr;
using std::endl;
using std::list;
void CbmEcalAnalysisCluster::BuildCluster3(CbmEcalCell* cell, Int_t clusters, Double_t x, Double_t y)
{
  const Double_t cTpSigma=TMath::Sqrt(48.0*1.6/2.0);
  const Double_t cTqSigma=TMath::Sqrt(12.0/2.0);
  const Int_t cDiv=10;
  const Int_t cCl2Size=8;
  Int_t ix;
  Int_t iy;
  CbmEcalCell* cll;
  Double_t tx;
  Double_t ty;
  TVector2 p;
  Double_t r;
  Double_t r1;
  CbmEcalCell* cls[cCl2Size];
  Double_t rc[cCl2Size];
  Int_t i;
  Int_t j;
  TVector2 ox(x, y);
  Double_t phi=-(ox.Phi());
  Double_t tp;
  Double_t tq;
  Double_t dx;
  Double_t dy;
  Double_t xstep;
  Double_t ystep;
  Int_t iix;
  Int_t iiy;

  fCE3=-1111;
  fCSize3=-1111;

  for(i=0;i<cCl2Size;i++)
  {
    cls[i]=NULL;
    rc[i]=-1111;
  }
  fCSize3=0; fCE3=0;
  xstep=(cell->X2()-cell->X1())/cDiv;
  ystep=(cell->Y2()-cell->Y1())/cDiv;
  for(ix=-4;ix<5;ix++)
  for(iy=-4;iy<5;iy++)
  {
    tx=cell->GetCenterX(); tx+=ix*(cell->X2()-cell->X1());
    ty=cell->GetCenterY(); ty+=iy*(cell->Y2()-cell->Y1());
    cll=fStr->GetCell(tx, ty);
    if (cll==NULL) continue;
    p.Set(tx-x, ty-y); p=p.Rotate(phi);
    tp=p.X()-TMath::Sqrt(3.0);
    tq=p.Y();
    r=0;
    dx=cll->GetX1()+xstep/2.0;
    for(iix=0;iix<cDiv;iix++)
    {
      dy=cll->GetY1()+ystep/2.0;
      for(iiy=0;iiy<cDiv;iiy++)
      {
        p.Set(dx-x, dy-y); p=p.Rotate(phi);
        tp=p.X()-TMath::Sqrt(3.0);
        tq=p.Y();
//	r1=TMath::Sqrt(tp*tp/cTpSigma+tq*tq/cTqSigma);
        r1=TMath::Gaus(tp, 0.0, cTpSigma)*TMath::Gaus(tq, 0.0, cTqSigma);
        r+=r1;
	dy+=ystep;
      }
      dx+=xstep;
    }
    for(i=0;i<cCl2Size;i++)
      if (rc[i]<r)
	break;
    if (i==cCl2Size) continue;
    for(j=cCl2Size-1;j>i;j--)
    {
      rc[j]=rc[j-1];
      cls[j]=cls[j-1];
    }
    rc[i]=r;
    cls[i]=cll;
  }
  fCSize3=cCl2Size;
  for(i=0;i<cCl2Size;i++)
    fCE3+=cls[i]->GetTotalEnergy();
}


void CbmEcalAnalysisCluster::BuildCluster2(CbmEcalCell* cell, Int_t clusters, Double_t x, Double_t y)
{
  Int_t cCl2Size=fC2ParI;
  Int_t ix;
  Int_t iy;
  CbmEcalCell* cll;
  Double_t tx;
  Double_t ty;
  TVector2 p;
  Double_t r;
  CbmEcalCell* cls[cCl2Size];
  Double_t rc[cCl2Size];
  Int_t i;
  Int_t j;
  Double_t phi;
  Double_t tp;
  Double_t tq;

  fCE2=-1111;
  fCSize2=-1111;

//  cout << x << "," << y << ": " << phi << endl;
  for(i=0;i<cCl2Size;i++)
  {
    cls[i]=NULL;
    rc[i]=-1111;
  }
  fCSize2=0; fCE2=0;
  for(ix=-4;ix<5;ix++)
  for(iy=-4;iy<5;iy++)
  {
    tx=cell->GetCenterX(); tx+=ix*(cell->X2()-cell->X1());
    ty=cell->GetCenterY(); ty+=iy*(cell->Y2()-cell->Y1());
    cll=fStr->GetCell(tx, ty);
    if (cll==NULL) continue;
    p.Set(tx-x, ty-y); p=p.Rotate(phi);
    // fC2Par1=3.0;
    tp=p.X()-TMath::Sqrt(fC2Par1);
    tq=p.Y();
    // fC2Par2=72
    // fC2Par3=12
    r=TMath::Sqrt(tp*tp/fC2Par2+tq*tq/fC2Par3);
    r=SolveEllipse(cll, x, y);
    for(i=0;i<cCl2Size;i++)
      if (rc[i]<r)
	break;
    if (i==cCl2Size) continue;
    for(j=cCl2Size-1;j>i;j--)
    {
      rc[j]=rc[j-1];
      cls[j]=cls[j-1];
    }
    rc[i]=r;
    cls[i]=cll;
  }
  fCSize2=cCl2Size;
  for(i=0;i<cCl2Size;i++)
    fCE2+=cls[i]->GetTotalEnergy();
}

Double_t CbmEcalAnalysisCluster::SolveEllipse(CbmEcalCell* cell, Double_t cx, Double_t cy)
{
  Double_t cRx=fC2Par2;		//48*1.6
  Double_t cRy=fC2Par3;
  const Int_t cSt=100;
  TVector2 centr(cx, cy);
  TVector2 t(centr);
  Double_t gm=centr.Phi();
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
  
  t/=t.Mod(); centr+=t*TMath::Sqrt(fC2Par1);
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

/** Build a cluster information **/
void CbmEcalAnalysisCluster::BuildCluster(CbmEcalCell* cell, Int_t cls, Double_t x, Double_t y)
{
  static CbmEcalInf* inf=fStr->GetEcalInf();
  CbmEcalCell* cll;
  Int_t ix;
  Int_t iy;
  Double_t tx;
  Double_t ty;
  Double_t t;
  Int_t i;
  Int_t j;
  TVector2 ox(x, y);
  TVector2 oy;
  TVector2 p;
  Double_t tp;
  Double_t tq;
  Double_t phi;
  Double_t r;

  ox/=ox.Mod(); oy=ox; phi=-(ox.Phi());
  for(i=0;i<fClsSize;i++)
  {
    fClsP[i]=-1111;
    fClsQ[i]=-1111;
    fClsE[i]=-1111;
    fClsX[i]=-1111;
    fClsY[i]=-1111;
    fClsSE[i]=-1111;
    fClsN[i]=-1111;
  }

  for(ix=-5;ix<6;ix++)
  for(iy=-5;iy<6;iy++)
  {
    tx=cell->GetCenterX(); tx+=ix*(cell->X2()-cell->X1());
    ty=cell->GetCenterY(); ty+=iy*(cell->Y2()-cell->Y1());
    cll=fStr->GetCell(tx, ty);
    if (cll!=NULL)
    {
      for(i=0;i<fClsSize;i++)
	if (fClsE[i]<cll->GetTotalEnergy())
	  break;
      if (i==fClsSize) continue;
      for(j=fClsSize-1;j>i;j--)
      {
	fClsP[j]=fClsP[j-1];
	fClsQ[j]=fClsQ[j-1];
	fClsX[j]=fClsX[j-1];
	fClsY[j]=fClsY[j-1];
	fClsE[j]=fClsE[j-1];
	fClsS[j]=fClsS[j-1];
      }
      tx=cll->GetCenterX();
      ty=cll->GetCenterY();
      fClsX[i]=tx;
      fClsY[i]=ty;
      fClsE[i]=cll->GetTotalEnergy();
/*      p.Set(tx-x, ty-y); q=p;
      t=p*ox;
      p=ox; p*=t;
      q-=p;
*/
//      fClsP[i]=t;

      p.Set(tx-x, ty-y); p=p.Rotate(phi);
      fClsP[i]=p.X();
      fClsQ[i]=p.Y();
      fClsS[i]=SolveEllipse(cll, x, y);
//      cout << phi << ". " << p.X() << ", " << p.Y() << " : " <<  t << ", " << q.Mod() << " : " << ox.X() << ", " << ox.Y() <<  endl; 
//      if (q*oy>0) fClsQ[i]=q.Mod(); else fClsQ[i]=-q.Mod();
/*
      cout << ox.X() << ", " << ox.Y() << endl;
      cout << p.X() << ", " << p.Y() << endl;
      cout << q.X() << ", " << q.Y() << endl;
      cout << endl;
*/
    }
  }

  fClsSE[0]=fClsE[0]; fClsN[0]=0;
  for(i=1;i<fClsSize;i++)
  {
    fClsSE[i]=fClsSE[i-1]+fClsE[i];
    fClsN[i]=i;
  }

  fCSize=0; fCE=0;
  for(ix=-4;ix<5;ix++)
  for(iy=-4;iy<5;iy++)
  {
    tx=cell->GetCenterX(); tx+=ix*(cell->X2()-cell->X1());
    ty=cell->GetCenterY(); ty+=iy*(cell->Y2()-cell->Y1());
    cll=fStr->GetCell(tx, ty);
    if (cll==NULL) continue;
    p.Set(tx-x, ty-y); p=p.Rotate(phi);
    tp=p.X()-TMath::Sqrt(3.0);
    tq=p.Y();
    r=TMath::Sqrt(tp*tp/48+tq*tq/12);
    if (r>1.0) continue;
    if (SolveEllipse(cll, x, y)/6.25<0.75) continue;
    fCSize++;
    fCE+=cll->GetTotalEnergy();
  }
}

/** Loop procedure **/
void CbmEcalAnalysisCluster::Exec(Option_t* option)
{
  fEvent++;
  if (fVerbose>0)
    Info("Exec", "Event %d.", fEvent);
  InitTree();

  static CbmEcalInf* inf=fStr->GetEcalInf();
  Int_t i;
  Int_t n;
  Int_t imax;
  Int_t dx;
  Int_t dy;
  Int_t ix;
  Int_t iy;
  list<CbmEcalCell*> cells;
  list<CbmEcalCell*>::const_iterator p;
  Double_t e;
  Double_t x;
  Double_t y;
  Double_t r;
  Double_t max=-1111;
  Double_t efull;
  Double_t e2m;
  Double_t e2;
  Double_t e3;
  CbmEcalPoint* pt;
  CbmEcalCell* cl;
  CbmEcalCell* cll;
  
  fStr->GetCells(cells);
  efull=0;
  for(p=cells.begin();p!=cells.end();++p)
  {
    if ((*p)==NULL) continue;
    e=(*p)->GetTotalEnergy();
    efull+=e;
    if (e>max)
    {
      max=e;
      cl=(*p);
    }
  }
  fEMax=max;
  fEFull=efull;
//cells.sort(CbmEcalClusterSortProcess());
  if (max<=0) return;
  cl->GetNeighborsList(0, cells);
  e3=max;
  for(p=cells.begin();p!=cells.end();++p)
    e3+=(*p)->GetTotalEnergy();
  fE3=e3;
  e2=0;
  for(i=1;i<5;i++)
  {
    cl->GetNeighborsList(i, cells);
    e2m=max;
    for(p=cells.begin();p!=cells.end();++p)
      e2m+=(*p)->GetTotalEnergy();
    if (e2m>e2)
    { 
      e2=e2m;
      imax=i;
    }
  }
  fE2=e2;
  
  cl->GetNeighborsList(imax, cells);
  fMX=cl->GetCenterX();
  fMY=cl->GetCenterY();
  fGX=cl->GetCenterX()*cl->GetTotalEnergy(); 
  fGY=cl->GetCenterY()*cl->GetTotalEnergy(); 
  for(p=cells.begin();p!=cells.end();++p)
  {
    fGX+=(*p)->GetCenterX()*(*p)->GetTotalEnergy();
    fGY+=(*p)->GetCenterY()*(*p)->GetTotalEnergy();
  }
  fGX/=e2; fGY/=e2;
  BuildCluster(cl, imax, fGX, fGY);
  BuildCluster2(cl, imax, fGX, fGY);
  BuildCluster3(cl, imax, fGX, fGY);
  cl->GetNeighborsList(0, cells);
  fGX2=cl->GetCenterX()*cl->GetTotalEnergy(); 
  fGY2=cl->GetCenterY()*cl->GetTotalEnergy(); 
  for(p=cells.begin();p!=cells.end();++p)
  {
    fGX2+=(*p)->GetCenterX()*(*p)->GetTotalEnergy();
    fGY2+=(*p)->GetCenterY()*(*p)->GetTotalEnergy();
  }
  fGX2/=e3; fGY2/=e3;

  pt=(CbmEcalPoint*)fMC->At(0);
  fX=-1111; fY=-1111; fTheta=-1111;
  if (pt!=NULL)
  {
    fX=pt->GetX();
    fY=pt->GetY();
    fTheta=TMath::ATan(TMath::Sqrt(fX*fX+fY*fY)/inf->GetZPos());
    fPhi=TMath::ATan2(fY, fX);
  }
  fE4=0; fE5=0; fE5_2=0;
  if (fX!=-1111)
  {
    if (fX>0) dx=1; else dx=0;
    if (fY>0) dy=1; else dy=0;
    for(ix=-2;ix<2;ix++)
    for(iy=-2;iy<2;iy++)
    {
      x=cl->GetCenterX(); x+=(ix+dx)*(cl->X2()-cl->X1());
      y=cl->GetCenterY(); y+=(iy+dy)*(cl->Y2()-cl->Y1());
      cll=fStr->GetCell(x, y);
      if (cll!=NULL)
	fE4+=cll->GetTotalEnergy();
    }
//    if (fX>0) dx=2; else dx=0;
//    if (fY>0) dy=2; else dy=0;
    for(ix=-2;ix<3;ix++)
    for(iy=-2;iy<3;iy++)
    {
      x=cl->GetCenterX(); x+=ix*(cl->X2()-cl->X1());
      y=cl->GetCenterY(); y+=iy*(cl->Y2()-cl->Y1());
      cll=fStr->GetCell(x, y);
      if (cll!=NULL)
	fE5+=cll->GetTotalEnergy();
    }
    if (fX>0) dx=2; else dx=0;
    if (fY>0) dy=2; else dy=0;
    for(ix=-3;ix<2;ix++)
    for(iy=-3;iy<2;iy++)
    {
      x=cl->GetCenterX(); x+=(ix+dx)*(cl->X2()-cl->X1());
      y=cl->GetCenterY(); y+=(iy+dy)*(cl->Y2()-cl->Y1());
      cll=fStr->GetCell(x, y);
      if (cll!=NULL)
	fE5_2+=cll->GetTotalEnergy();
    }
  }
  else
  {
    fE4=-1111;
    fE5=-1111;
    fE5_2=-1111;
  }
  fTree->Fill();
}


/** Initializes tree **/
void CbmEcalAnalysisCluster::InitTree()
{
  if (fTree) return;
  TString n;
  fClsSize=20;
  fClsP=new Double_t[fClsSize];
  fClsQ=new Double_t[fClsSize];
  fClsE=new Double_t[fClsSize];
  fClsSE=new Double_t[fClsSize];
  fClsX=new Double_t[fClsSize];
  fClsY=new Double_t[fClsSize];
  fClsS=new Double_t[fClsSize];
  fClsN=new Int_t[fClsSize];
  fTree=new TTree("cls","Various information about cluster");
  fTree->Branch("ev", &fEvent, "ev/I");
  fTree->Branch("x", &fX, "x/D");
  fTree->Branch("y", &fY, "y/D");
  fTree->Branch("gx", &fGX, "gx/D");
  fTree->Branch("gy", &fGY, "gy/D");
  fTree->Branch("gx2", &fGX2, "gx2/D");
  fTree->Branch("gy2", &fGY2, "gy2/D");
  fTree->Branch("mx", &fMX, "mx/D");
  fTree->Branch("my", &fMY, "my/D");
  fTree->Branch("z", &fZ, "z/D");
  fTree->Branch("efull", &fEFull, "efull/D");
  fTree->Branch("emax", &fEMax, "emax/D");
  fTree->Branch("e2", &fE2, "e2/D");
  fTree->Branch("e3", &fE3, "e3/D");
  fTree->Branch("e4", &fE4, "e4/D");
  fTree->Branch("e5", &fE5, "e5/D");
  fTree->Branch("e5_2", &fE5_2, "e5_2/D");
  fTree->Branch("theta", &fTheta, "theta/D");
  fTree->Branch("phi", &fPhi, "phi/D");
  fTree->Branch("ce", &fCE, "ce/D");
  fTree->Branch("csize", &fCSize, "csize/I");
  fTree->Branch("ce2", &fCE2, "ce2/D");
  fTree->Branch("csize2", &fCSize2, "csize2/I");
  fTree->Branch("ce3", &fCE3, "ce3/D");
  fTree->Branch("csize3", &fCSize3, "csize3/I");
  n="clsp["; n+=fClsSize; n+="]/D"; fTree->Branch("clsp", fClsP, n);
  n="clsq["; n+=fClsSize; n+="]/D"; fTree->Branch("clsq", fClsQ, n);
  n="clsx["; n+=fClsSize; n+="]/D"; fTree->Branch("clsx", fClsX, n);
  n="clsy["; n+=fClsSize; n+="]/D"; fTree->Branch("clsy", fClsY, n);
  n="clse["; n+=fClsSize; n+="]/D"; fTree->Branch("clse", fClsE, n);
  n="clsse["; n+=fClsSize; n+="]/D"; fTree->Branch("clsse", fClsSE, n);
  n="clss["; n+=fClsSize; n+="]/D"; fTree->Branch("clss", fClsS, n);
  n="clsn["; n+=fClsSize; n+="]/I"; fTree->Branch("clsn", fClsN, n);
}

CbmEcalAnalysisCluster::CbmEcalAnalysisCluster()
  : FairTask(),
    fTree(NULL),
    fEvent(0),
    fX(0.),
    fY(0.),
    fZ(0.),
    fEFull(0.),
    fEMax(0.),
    fE2(0.),
    fE3(0.),
    fE4(0.),
    fE5(0.),
    fE5_2(0.),
    fGX(0.),
    fGY(0.),
    fGX2(0.),
    fGY2(0.),
    fMX(0.),
    fMY(0.),
    fTheta(0.),
    fPhi(0.),
    fClsSize(0),
    fClsX(NULL),
    fClsY(NULL),
    fClsP(NULL),
    fClsQ(NULL),
    fClsE(NULL),
    fClsSE(NULL),
    fClsN(NULL),
    fClsS(NULL),
    fCSize(0),
    fCE(0.),
    fCE2(0.),
    fCSize2(0),
    fCE3(0.),
    fCSize3(0),
    fC2ParI(0),
    fC2Par1(0.),
    fC2Par2(0.),
    fC2Par3(0.),
    fMC(NULL),
    fStr(NULL)
{
}

CbmEcalAnalysisCluster::CbmEcalAnalysisCluster(const char* name, const Int_t iVerbose, const char* cfgname)
  : FairTask(name, iVerbose),
    fTree(NULL),
    fEvent(0),
    fX(0.),
    fY(0.),
    fZ(0.),
    fEFull(0.),
    fEMax(0.),
    fE2(0.),
    fE3(0.),
    fE4(0.),
    fE5(0.),
    fE5_2(0.),
    fGX(0.),
    fGY(0.),
    fGX2(0.),
    fGY2(0.),
    fMX(0.),
    fMY(0.),
    fTheta(0.),
    fPhi(0.),
    fClsSize(0),
    fClsX(NULL),
    fClsY(NULL),
    fClsP(NULL),
    fClsQ(NULL),
    fClsE(NULL),
    fClsSE(NULL),
    fClsN(NULL),
    fClsS(NULL),
    fCSize(0),
    fCE(0.),
    fCE2(0.),
    fCSize2(0),
    fCE3(0.),
    fCSize3(0),
    fC2ParI(0),
    fC2Par1(TMath::Sqrt(3)),
    fC2Par2(48.0*1.6),
    fC2Par3(12.),
    fMC(NULL),
    fStr(NULL)
{
  CbmEcalParam* par=new CbmEcalParam("ClusterAnalysisParam", cfgname);
  fC2ParI=par->GetInteger("cpari");
  fC2Par1=par->GetDouble("cpar1");
  fC2Par2=par->GetDouble("cpar2");
  fC2Par3=par->GetDouble("cpar3");
  delete par;
}

/** Initing routine **/
InitStatus CbmEcalAnalysisCluster::Init()
{
  FairRootManager* fManager=FairRootManager::Instance();
  if (!fManager)
  {
    Fatal("Init", "Can't find a Root Manager.");
    return kFATAL;
  }
  fMC=(TClonesArray*)fManager->GetObject("ECALPoint");
  if (!fMC)
  {
    Fatal("Init", "Can't find an array of ECAL points.");
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

/** Finishing routine **/
void CbmEcalAnalysisCluster::Finish()
{
  if (fTree)
    fTree->Write();
}

ClassImp(CbmEcalAnalysisCluster)
