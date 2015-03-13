/* $Id: CbmEcalDrawer2.cxx,v 1.3 2006/07/14 15:11:51 prokudin Exp $ */
#include "CbmEcalDrawer2.h"

#include "TH2D.h"
#include "TCanvas.h"
#include "TClonesArray.h"
#include "TMarker.h"
#include "TLine.h"
#include "TMath.h"
#include "TText.h"
#include "TVector2.h"
#include "TEllipse.h"

#include "FairRootManager.h"
#include "CbmEcalStructure.h"
#include "CbmEcalInf.h"
#include "CbmEcalPoint.h"
#include "CbmEcalCluster.h"

#include <iostream>

using namespace std;

CbmEcalDrawer2::CbmEcalDrawer2()
  : FairTask(),
    fStr(NULL),
    fC(NULL),
    fOut(NULL),
    fEvent(0),
    fDrawOpt(""),
    fNamePrefix(""),
    fMCPoints(NULL),
    fCluster(),
    fClusterE(0.),
    fParI(0),
    fIntSteps(0),
    fPar1(0.),
    fPar2(0.),
    fPar3(0.)
{
  SetDrawOpt();
  SetNamePrefix();
}
	
CbmEcalDrawer2::CbmEcalDrawer2(const char* name, const char* title)
  : FairTask(name, 0),
    fStr(NULL),
    fC(NULL),
    fOut(NULL),
    fEvent(0),
    fDrawOpt(""),
    fNamePrefix(""),
    fMCPoints(NULL),
    fCluster(),
    fClusterE(0.),
    fParI(7),
    fIntSteps(100),
    fPar1(0.38),
    fPar2(30.8),
    fPar3(12.0)


{
  SetDrawOpt();
  SetNamePrefix();
}

InitStatus CbmEcalDrawer2::Init()
{
  fEvent=0;

  list<CbmEcalCell*> cells;
  list<CbmEcalCell*>::const_iterator p;
  Int_t type;
  FairRootManager* io=FairRootManager::Instance();
  Double_t x1;
  Double_t x2;
  Double_t y1;
  Double_t y2;
  Int_t ix;
  Int_t iy;
  CbmEcalInf* inf;
  if (!io)
  {
    Fatal("Init", "Can't find IOManager.");
    return kFATAL;
  }
  fStr=(CbmEcalStructure*)io->GetObject("EcalStructure");
  if (!fStr) 
  {
    Fatal("Init()", "Can't find calorimeter structure in the system.");
    return kFATAL;
  }
  fMCPoints=(TClonesArray*)io->GetObject("EcalPoint");
  if (!fMCPoints) 
  {
    Fatal("Init()", "Can't find calorimeter points in the system.");
    return kFATAL;
  }

  fStr->GetCells(cells);
  type=-1111;
  for(p=cells.begin();p!=cells.end();++p)
  {
    if ((*p)==NULL) continue;
    if ((*p)->GetType()==0) continue;
    if (type==-1111)
      type=(*p)->GetType();
    else
      if (type!=(*p)->GetType())
	break;
  }
  if (p!=cells.end())
  {
    Fatal("Init()","Calorimeter consists of cells with different types.");
    return kFATAL;
  }
  inf=fStr->GetEcalInf();
  x1=fStr->GetX1();
  x2=fStr->GetX2();
  y1=fStr->GetY1();
  y2=fStr->GetY2();
  ix=inf->GetXSize(); ix*=type;
  iy=inf->GetYSize(); iy*=type;
  fOut=new TH2D("ecal","CBM calorimeter", ix, x1, x2, iy, y1, y2);
  fOut->SetStats(kFALSE);
//  fC=new TCanvas("c","", (x2-x1)/(y2-y1)*1000, 1000);
  fC=new TCanvas("c","", 1000, 1000);
  fC->SetLeftMargin(0.075);
  fC->SetRightMargin(0.125);

  return kSUCCESS;
}

void CbmEcalDrawer2::Exec(Option_t* opt)
{
  const Int_t cNCells=8;
  fEvent++;
  TString nm="";
  Int_t i;
  Int_t n;
  Int_t imax;
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
  TMarker* mk;
  TMarker* mk2;
  TString cp;
  TString st;
  TLine* ln;
  TLine* ln2;
  list<TLine*> lines;
  list<TLine*>::const_iterator pl;
  TLine* tl;
  CbmEcalCell* cls[cNCells];
  TText* txt[cNCells];
  TEllipse* el;
  TVector2 tv;
  TVector2 tv1;
  Double_t theta;

  fC->Clear();
  nm+=fEvent;
  i=nm.Length();
  for(;i<7;i++)
    nm="0"+nm;
  nm=fNamePrefix+nm;
  gPad=fC;

  fStr->GetCells(cells);
  efull=0;
  for(p=cells.begin();p!=cells.end();++p)
  {
    if ((*p)==NULL) continue;
    e=(*p)->GetTotalEnergy();
    efull+=e;
    x=(*p)->GetCenterX();
    y=(*p)->GetCenterY();
    fOut->SetBinContent(fOut->FindBin(x, y), e);
    if (e>max)
    {
      max=e;
      cl=(*p);
    }
  }
  if (max<=0.02) return;
  ConstructCluster(cl);
  if (fClusterE<0.12) return;
  cells.sort(CbmEcalClusterSortProcess());
  cells.reverse(); i=0;
  for(p=cells.begin();p!=cells.end();++p)
  {
    cls[i++]=(*p);
    if (i==cNCells) break;
  }
  cl->GetNeighborsList(0, cells);
//  if (max<0.1) return;
  for(p=cells.begin();p!=cells.end();++p)
    if ((*p)==cls[1]) break;
//  if (p!=cells.end())
//    return;

  e3=max;
  for(p=cells.begin();p!=cells.end();++p)
    e3+=(*p)->GetTotalEnergy();
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
  cl->GetNeighborsList(imax, cells);
  cp="E_{Full}=";
  st=""; st+=efull; st.ReplaceAll(" ",""); st.ReplaceAll("	",""); st.Remove(6);
  cp+=st; cp+=", E_{2x2}=";
  st=""; st+=e2; st.ReplaceAll(" ",""); st.ReplaceAll("	",""); st.Remove(6);
  cp+=st; cp+="(";
  st=""; st+=e2/efull*100; st.ReplaceAll(" ",""); st.ReplaceAll("	",""); if (e2!=efull) st.Remove(2); else st.Remove(3);
  cp+=st; cp+="%), E_{cls}=";
  st=""; st+=fClusterE; st.ReplaceAll(" ",""); st.ReplaceAll("	",""); st.Remove(6);
  cp+=st; cp+="(";
  st=""; st+=fClusterE/efull*100; st.ReplaceAll(" ",""); st.ReplaceAll("	",""); if (e3!=efull) st.Remove(2); else st.Remove(3);

  cp+=st; cp+="%)";
  fOut->SetTitle(cp);
  x=cl->GetCenterX();
  y=cl->GetCenterY();
  fOut->SetAxisRange(x-15, x+15, "X");
  fOut->SetAxisRange(y-15, y+15, "Y");
  fOut->Draw(fDrawOpt);
  n=fMCPoints->GetEntriesFast();
//  for(i=0;i<n;i++)
  { 
    pt=(CbmEcalPoint*)fMCPoints->At(0);
    if (pt!=NULL)
    {
      mk=new TMarker(pt->GetX(), pt->GetY(), kFullStar);
      mk->SetMarkerSize(2);
      mk->SetMarkerColor(kOrange);
      mk->Draw();
      x=pt->GetX();
      y=pt->GetY();
      r=TMath::Sqrt(x*x+y*y);
      x/=r; y/=r;
      ln=new TLine(pt->GetX()-x*2.0, pt->GetY()-y*2.0, pt->GetX(), pt->GetY());
      ln->Draw();
      ln2=new TLine(pt->GetX()+x*12.0, pt->GetY()+y*12.0, pt->GetX(), pt->GetY());
      ln2->SetLineColor(13);
      ln2->Draw();
    }
    else
    {
      mk=NULL;
      ln2=NULL;
      ln=NULL;
    }
  }
  for(i=0;i<cNCells;i++)
  {
//    cout << cls[i]->GetTotalEnergy() << endl;
    if (cls[i]->GetTotalEnergy()<=0.0)
    {
      txt[i]=NULL;
      continue;
    }
    st=""; st+=i+1;
    txt[i]=new TText(cls[i]->GetCenterX(), cls[i]->GetCenterY(), st.Data());
    txt[i]->SetTextAlign(22);
    txt[i]->Draw();
  }

  {
    x=cl->GetCenterX()*cl->GetTotalEnergy(); 
    y=cl->GetCenterY()*cl->GetTotalEnergy(); 

    for(p=cells.begin();p!=cells.end();++p)
    {
      x+=(*p)->GetCenterX()*(*p)->GetTotalEnergy();
      y+=(*p)->GetCenterY()*(*p)->GetTotalEnergy();
    }
    x/=e2;
    y/=e2;
    mk2=new TMarker(x, y, kOpenStar);
    mk2->SetMarkerSize(2);
    mk2->SetMarkerColor(kOrange);
    mk2->Draw();
    tv.Set(x, y); theta=tv.Phi(); tv1=tv;
    tv1/=tv.Mod();
    tv+=tv1*fPar1;
    el=new TEllipse(tv.X(), tv.Y(), TMath::Sqrt(fPar2), TMath::Sqrt(fPar3), 0.0, 360.0, theta*TMath::RadToDeg());
    el->SetFillColor(0);
    el->SetFillStyle(0);
    el->Draw();
  }
  
  lines.clear();
  for(p=fCluster.begin();p!=fCluster.end();++p)
  {
    tl=new TLine((*p)->GetX1(), (*p)->GetY1(), (*p)->GetX2(), (*p)->GetY1());
    lines.push_back(tl);
    tl=new TLine((*p)->GetX2(), (*p)->GetY1(), (*p)->GetX2(), (*p)->GetY2());
    lines.push_back(tl);
    tl=new TLine((*p)->GetX2(), (*p)->GetY2(), (*p)->GetX1(), (*p)->GetY2());
    lines.push_back(tl);
    tl=new TLine((*p)->GetX1(), (*p)->GetY2(), (*p)->GetX1(), (*p)->GetY1());
    lines.push_back(tl);
  }
  for(pl=lines.begin();pl!=lines.end();++pl)
  {
    tl=*pl;
    tl->SetLineColor(kOrange);
    tl->SetLineWidth(3);
    tl->Draw();
  }
  fC->SaveAs(nm+".png");
  fC->SaveAs(nm+".C");
  fC->Clear();
  delete mk;
  delete mk2;
  delete ln;
  delete ln2;
  for(i=0;i<cNCells;i++)
    delete txt[i];
  delete el;
  for(pl=lines.begin();pl!=lines.end();++pl)
    delete (*pl);
}

void CbmEcalDrawer2::Finish()
{
  ;
}

CbmEcalDrawer2::~CbmEcalDrawer2()
{
  fC->Clear();
  delete fC;
  fOut->Clear();
  delete fOut;
}

void CbmEcalDrawer2::ConstructCluster(CbmEcalCell* max)
{
  Int_t i;
  Int_t j;
  list<CbmEcalCell*> cells;
  list<CbmEcalCell*>::const_iterator p;
  CbmEcalCell* cell;
  CbmEcalCell* cll;
  Double_t e=1e9;
  Int_t ix;
  Int_t iy;
  Double_t tx;
  Double_t ty;
  CbmEcalCell* cls[20];
  Double_t rc[20];
  Double_t r;
  Double_t cx;
  Double_t cy;
  Double_t me;

  fCluster.clear();

  cell=max;
  for(i=1;i<5;i++)
  {
    cell->GetNeighborsList(i, cells);
    e=0;
    for(p=cells.begin();p!=cells.end();++p)
      e+=(*p)->GetTotalEnergy();
    if (e>me)
    {
      j=i;
      me=e;
    }
  }
  me+=cell->GetTotalEnergy();
  cx=cell->GetTotalEnergy()*cell->GetCenterX();
  cy=cell->GetTotalEnergy()*cell->GetCenterY();
  cell->GetNeighborsList(j, cells);
  for(p=cells.begin();p!=cells.end();++p)
  {
    cx+=(*p)->GetTotalEnergy()*(*p)->GetCenterX();
    cy+=(*p)->GetTotalEnergy()*(*p)->GetCenterY();
  }
  cx/=me; cy/=me;


  cell=max; e=1e9;
  for(i=0;i<fParI;i++)
  {
    cls[i]=NULL;
    rc[i]=-1111;
  }

  for(ix=-4;ix<5;ix++)
  for(iy=-4;iy<5;iy++)
  {
    tx=cell->GetCenterX(); tx+=ix*(cell->X2()-cell->X1());
    ty=cell->GetCenterY(); ty+=iy*(cell->Y2()-cell->Y1());
    cll=fStr->GetCell(tx, ty);
    if (cll==NULL) continue;
    r=SolveEllipse(cll, cx, cy);
    for(i=0;i<fParI;i++)
      if (rc[i]<r)
	break;
    if (i==fParI) continue;
    for(j=fParI-1;j>i;j--)
    {
      rc[j]=rc[j-1];
      cls[j]=cls[j-1];
    }
    rc[i]=r;
    cls[i]=cll;
  }
  fClusterE=0;
  for(j=0;j<fParI;j++)
  {
    fCluster.push_back(cls[j]);
    fClusterE+=cls[j]->GetTotalEnergy();
  }
}

Double_t CbmEcalDrawer2::SolveEllipse(CbmEcalCell* cell, Double_t cx, Double_t cy)
{
  Double_t cRx=fPar2;		//48*1.6
  Double_t cRy=fPar3;
  Int_t cSt=fIntSteps;
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
  
// cout << fCurAlgo << " " << reg << " " << fPar1[reg] << " " << cRx << " " << cRy << endl;
  t/=t.Mod(); centr+=t*TMath::Sqrt(fPar1);
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

ClassImp(CbmEcalDrawer2);
