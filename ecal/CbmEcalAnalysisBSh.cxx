#include "CbmEcalAnalysisBSh.h"

#include "FairRootManager.h"

#include "CbmMCTrack.h"

#include "CbmEcalStructure.h"
#include "CbmEcalPoint.h"
#include "CbmEcalParam.h"
#include "CbmEcalCell.h"
#include "CbmEcalCalibration.h"
#include "CbmEcalInf.h"

#include "TTree.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TVector2.h"

#include <iostream>

using namespace std;

Double_t CbmEcalAnalysisBSh::GetPL(Double_t e, Double_t theta)
{
// Formula 0: (([0]/sqrt(x))+[1])+([2]*x)
  Double_t F0P0=-0.511389352836172;
  Double_t F0P1=0.112078881354459;
  Double_t F0P2=0.0078928249438169;                                
  Double_t p0=F0P0/TMath::Sqrt(e)+F0P1+e*F0P2;
// Formula 1: (([0]*log(x))+[1])+([2]*x)      
  Double_t F1P0=-0.035601043502675;
  Double_t F1P1=-0.208980807090046;
  Double_t F1P2=0.000845322275601826;
  Double_t p1=F1P0*TMath::Log(e)+F1P1+F1P2*e;
// Formula 2: ([0]/sqrt(x))+[1]
  Double_t F2P0=-0.000885986793426617;
  Double_t F2P1=0.00161447557665425;
  Double_t F2P2=0; 
  Double_t p2=F2P0/TMath::Sqrt(e)+F2P1;
  Double_t res=p0+theta*p1+theta*theta*p2;

  return res;
  // For tungsten  calorimeter
/*
//# Formula 0: ([0]/sqrt(x))+[1]
  Double_t F0P0=-0.430846583116232;
  Double_t F0P1=0.121138651211173;
  Double_t F0P2=0;
  Double_t p0=F0P0/TMath::Sqrt(e)+F0P1;
//# Formula 1: (([0]*log(x))+[1])+([2]*x)
  Double_t F1P0=-0.0281906348040203;
  Double_t F1P1=-0.157848864778439;
  Double_t F1P2=0.00116139446600647;
  Double_t p1=F1P0*TMath::Log(e)+F1P1+F1P2*e;
//# Formula 2: ([0]/x)+[1]
  Double_t F2P0=-0.000357569484705054;
  Double_t F2P1=0.000840324598592234;
  Double_t F2P2=0;
  Double_t p2=F2P0/TMath::Sqrt(e)+F2P1;
  Double_t res=p0+theta*p1+theta*theta*p2;
  return res;
*/
}

Double_t CbmEcalAnalysisBSh::GetPH(Double_t e, Double_t theta)
{
// Formula 3: ([0]/sqrt(x))+[1]
  Double_t F3P0=0.369616901912454;
  Double_t F3P1=0.43088175058375;
  Double_t F3P2=0;
  Double_t p0=F3P0/sqrt(e)+F3P1;
// Formula 4: pol2
  Double_t F4P0=-0.120449496069499;
  Double_t F4P1=-0.00896745040885047;
  Double_t F4P2=0.000338080287821179;
  Double_t p1=F4P0+F4P1*e+F4P2*e*e;
// Formula 5: pol2
  Double_t F5P0=0.00125025561523754;
  Double_t F5P1=5.1075264956049e-05;
  Double_t F5P2=-1.81288313960512e-06;
  Double_t p2=F5P0+F5P1*e+F5P2*e*e;
  Double_t res=p0+theta*p1+theta*theta*p2;
  return res;
  // For tungsten  calorimeter
/*
//# Formula 3: ([0]/x)+[1]
  Double_t F3P0=0.169589556951151;
  Double_t F3P1=0.391751654446709;
  Double_t F3P2=0;
  Double_t p0=F3P0/e+F3P1;
//# Formula 4: pol2
  Double_t F4P0=-0.084397709284501;
  Double_t F4P1=-0.0069293349995729;
  Double_t F4P2=0.000237137391972728;
  Double_t p1=F4P0+F4P1*e+F4P2*e*e;
//# Formula 5: pol2
  Double_t F5P0=0.000757869675318593;
  Double_t F5P1=4.10658831181072e-05;
  Double_t F5P2=-1.11503261196945e-06;
  Double_t p2=F5P0+F5P1*e+F5P2*e*e;
  Double_t res=p0+theta*p1+theta*theta*p2;
  return res;
*/
}

/** A default algorithm of precluster formation **/
void  CbmEcalAnalysisBSh::FormPreCluster(CbmEcalCell* cell)
{
  Int_t reg=GetRegion(cell);
  if (fCalibration==1) reg=fCurAlgo;
  Int_t i;
  Int_t j;
  Int_t max=-1111;
  Double_t mine;
  Double_t maxe;
  Double_t e;
  list<CbmEcalCell*> cells;
  list<CbmEcalCell*> cs;
  list<CbmEcalCell*>::const_iterator p;
  CbmEcalCell* min;
  
  mine=cell->GetTotalEnergy(); min=cell;
  maxe=-1.0;
  /** Find minimum cell **/
  cell->GetNeighborsList(0, cells);
  for(p=cells.begin();p!=cells.end();++p)
    if ((*p)->GetTotalEnergy()<mine)
    {
      mine=(*p)->GetTotalEnergy();
      min=(*p);
    }
  for(j=1;j<5;j++)
  {
    cell->GetNeighborsList(j, cells);
    e=0;
    for(p=cells.begin();p!=cells.end();++p)
      e+=(*p)->GetTotalEnergy();
    if (e>maxe)
    {
      maxe=e;
      max=j;
    }
  }
  maxe+=cell->GetTotalEnergy();
  if (fCal)
  { if (GetEnergy(maxe, cell)<fMinClusterE[reg]) return; }
  else
  { if (maxe<fMinClusterE[reg]) return; }
  cell->GetNeighborsList(max, cells);
  cells.push_back(cell);
  if (find(cells.begin(), cells.end(), min)==cells.end())
    cells.push_back(min);
//fPreClusters.push_back(new CbmEcalPreCluster(cells, cell, min)); 
  fE=GetEnergy(maxe, cell);
}

/** A new algorithm of precluster formation **/
void  CbmEcalAnalysisBSh::FormPreClusterNew(CbmEcalCell* cell)
{
  Int_t i;
  Double_t e2m;
  Double_t e2=0;
  Int_t imax=-1111;
  list<CbmEcalCell*>::const_iterator p;
  list<CbmEcalCell*> cells;
  Double_t x;
  Double_t y;
  Double_t max=-1111;
  Int_t reg=GetRegion(cell);
  if (fCalibration==1) reg=fCurAlgo;
  Int_t cCl2Size=fParI[reg];
  Int_t ix;
  Int_t iy;
  CbmEcalCell* cll;
  Double_t tx;
  Double_t ty;
  TVector2 pv;
  Double_t r;
  CbmEcalCell* cls[cCl2Size];
  Double_t rc[cCl2Size];
  Int_t j;
  Double_t phi;
  Double_t tp;
  Double_t tq;
  Double_t e;

  max=cell->GetTotalEnergy();
  x=cell->GetCenterX()*max;
  y=cell->GetCenterY()*max;

  for(i=1;i<5;i++)
  {
    cell->GetNeighborsList(i, cells);
    e2m=max;
    for(p=cells.begin();p!=cells.end();++p)
      e2m+=(*p)->GetTotalEnergy();
    if (e2m>e2)
    { 
      e2=e2m;
      imax=i;
    }
  }
  cell->GetNeighborsList(imax, cells);
  for(p=cells.begin();p!=cells.end();++p)
  {
    x+=(*p)->GetCenterX()*(*p)->GetTotalEnergy();
    y+=(*p)->GetCenterY()*(*p)->GetTotalEnergy();
  }
  x/=e2; y/=e2;

  for(i=0;i<cCl2Size;i++)
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
    r=SolveEllipse(cll, x, y, reg);
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

  e=0;
  for(j=0;j<cCl2Size;j++)
    e+=cls[j]->GetTotalEnergy();
//  cout << e << "	" << GetEnergy(e, cell) << endl;
  if (fCal)
  { if (GetEnergy(e, cell)<fMinClusterE[reg]) return; }
  else
  { if (e<fMinClusterE[reg]) return; }
  fE=GetEnergy(e, cell);

//fPreClusters.push_back(new CbmEcalPreCluster(cls, cCl2Size, cell, NULL)); 
}

Double_t CbmEcalAnalysisBSh::GetEnergy(Double_t e, CbmEcalCell* cell)
{
  Double_t x=cell->GetCenterX();
  Double_t y=cell->GetCenterY();
  Double_t tantheta=TMath::Sqrt(x*x+y*y)/fInf->GetZPos();

  return fCal->GetEnergy(e, tantheta, GetRegion(cell));
}

Double_t  CbmEcalAnalysisBSh::SolveEllipse(CbmEcalCell* cell, Double_t cx, Double_t cy, Int_t reg)
{
  Double_t cRx=fPar2[reg];		//48*1.6
  Double_t cRy=fPar3[reg];
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
  t/=t.Mod(); centr+=t*TMath::Sqrt(fPar1[reg]);
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

/** Get region number of the cell **/
Int_t CbmEcalAnalysisBSh::GetRegion(CbmEcalCell* cls)
{
  Double_t x=cls->GetCenterX(); x*=x;
  Double_t y=cls->GetCenterY(); y*=y;
  x+=y;
  Double_t r=TMath::Sqrt(x);
  static Double_t z=fInf->GetZPos();
  Double_t a=TMath::ATan2(r, z)*TMath::RadToDeg();
  Int_t i=0;
  for(;i<fRegions;i++)
    if (fTheta[i]>a)
      break;

  return i;
}
/** Loop procedure **/
void CbmEcalAnalysisBSh::Exec(Option_t* option)
{
  InitTree();

  Int_t i;
  Int_t j;
  Int_t n;
  Int_t imax=-1111;
  Int_t dx;
  Int_t dy;
  Int_t ix;
  Int_t iy;
  Int_t pn;
  Int_t tri;
  Double_t xtr;
  Double_t ytr;
  Double_t tre;
  Double_t thetatr;
  Double_t phtr;
  Double_t pltr;
  Double_t dptr;
  Double_t qhtr;
  Double_t qltr;
  TVector3 trp;
  TVector2 v;
  list<CbmEcalCell*> cells;
  list<CbmEcalCell*> cells2;
  list<CbmEcalCell*>::const_iterator p;
  list<CbmEcalCell*>::const_iterator p2;
  Double_t e;
  Double_t max=-1111;
 // Double_t efull;
  Double_t e2m;
  Double_t e2;
  Double_t e3;
  Double_t x[10000];
  Double_t y[10000];
  Double_t eii[10000];
  Double_t php;
  Double_t phq;
  Double_t phi;
  Int_t phn;
  Int_t reg;
  CbmEcalCell* clls[10000];
  CbmEcalPoint* pt;
  CbmEcalCell* cl;
  CbmEcalCell* cll;
  CbmMCTrack* tr;
  
  fStr->GetCells(cells);
 // efull=0;
  n=0;
  cout << "." << flush;
  for(p=cells.begin();p!=cells.end();++p)
  {
    if ((*p)==NULL) continue;
    e=(*p)->GetTotalEnergy();
    if (e<0.001) continue;
    (*p)->GetNeighborsList(0, cells2);
    for(p2=cells2.begin();p2!=cells2.end();++p2)
      if ((*p2)->GetTotalEnergy()>e)
	break;
    if (p2!=cells2.end())
      continue;
    e2=0;
    for(i=1;i<5;i++)
    {
      (*p)->GetNeighborsList(i, cells2);
      e2m=e;
      for(p2=cells2.begin();p2!=cells2.end();++p2)
	e2m+=(*p2)->GetTotalEnergy();
      if (e2m>e2)
      {
	e2=e2m;
	imax=i;
      }
    }
    (*p)->GetNeighborsList(imax, cells2);
    clls[n]=(*p);
    x[n]=(*p)->GetCenterX()*e;
    y[n]=(*p)->GetCenterY()*e;
    for(p2=cells2.begin();p2!=cells2.end();++p2)
    {
      x[n]+=(*p2)->GetCenterX()*(*p2)->GetTotalEnergy();
      y[n]+=(*p2)->GetCenterY()*(*p2)->GetTotalEnergy();
    }
    x[n]/=e2;
    y[n]/=e2;
    eii[n]/=e2;
    n++;
  }

  pn=fMC->GetEntries();
  for(i=0;i<pn;i++)
  {
    pt=(CbmEcalPoint*)fMC->At(i);
    tri=pt->GetTrackID();
    if (tri<0) continue;
    tr=(CbmMCTrack*)fMCTr->At(tri);
    if (tr==NULL) continue;
    if (tr->GetPdgCode()!=22) continue;
   if (tr->GetStartZ()>20) continue;
//    cout << tri << " " << tr << " " << tr->GetPdgCode() << endl << flush;
    pt->Momentum(trp);
    tre=trp.Mag();
    if (tre<0.7) continue;
    xtr=pt->GetX();
    ytr=pt->GetY();
    thetatr=TMath::ATan2(TMath::Sqrt(xtr*xtr+ytr*ytr),pt->GetZ())*TMath::RadToDeg();
    phtr=GetPH(tre, thetatr);
    pltr=GetPL(tre, thetatr);
    dptr=phtr-pltr;
    phtr+=0.2*dptr;
    pltr-=0.3*dptr;
    qhtr=1.0;
    qltr=-1.0;
    phn=-1111;
    fX=xtr;
    fY=ytr;
    fEMC=tre;
    fERes=0.076*0.076/fEMC;
    fERes+=0.014*0.014;
    fERes=TMath::Sqrt(fERes)*fEMC;
    fTh=TMath::ATan2(TMath::Sqrt(fX*fX+fY*fY), pt->GetZ())*TMath::RadToDeg();
    fE=-1111;
    for(j=0;j<n;j++)
    {
      v.Set(x[j], y[j]);
      phi=-(v.Phi());
      v.Set(xtr-x[j], ytr-y[j]);
      v=v.Rotate(phi);
      php=v.X(); phq=v.Y();
      if (php>pltr&&php<phtr&&phq>qltr&&phq<qhtr)
      {
	if (phn<0)
	{
	  phn=j;
	  continue;
	}
	else
	{
	  phn=-1112;
	  break;
	}
      }
    }
    if (phn<0)
    {
      fTree->Fill();
      continue;
    }
//    cout << clls[phn] << endl;
    reg=GetRegion(clls[phn]);
    if (fPreClusterAlgo[reg]==0)
      FormPreCluster(clls[phn]);
    if (fPreClusterAlgo[reg]==1)
      FormPreClusterNew(clls[phn]);    
    fTree->Fill();
  }
}


/** Initializes tree **/
void CbmEcalAnalysisBSh::InitTree()
{
  if (fTree) return;
  fTree=new TTree("ps","ps ana");
  fTree->Branch("x", &fX, "x/D");
  fTree->Branch("y", &fY, "y/D");
  fTree->Branch("emc", &fEMC, "emc/D");
  fTree->Branch("e", &fE, "e/D");
  fTree->Branch("theta", &fTh, "theta/D");
  fTree->Branch("eres", &fERes, "eres/D");
}


CbmEcalAnalysisBSh::CbmEcalAnalysisBSh()
  : FairTask(),
    fTree(NULL),
    fX(0.),
    fY(0.),
    fEMC(0.),
    fE(0.),
    fTh(0.),
    fERes(0.),
    fCurAlgo(0),
    fRemoveCharged(0),
    fUseCalibration(0),
    fRegions(0),
    fCalibration(0),
    fIntSteps(0),
    fPreClusterAlgo(),
    fMinClusterE(),
    fMinMaxE(),
    fTheta(),
    fPar1(),
    fPar2(),
    fPar3(),
    fParI(),
    fMC(NULL),
    fMCTr(NULL),
    fStr(NULL),
    fCal(NULL),
    fInf(NULL)
{
}

CbmEcalAnalysisBSh::CbmEcalAnalysisBSh(const char* name, const Int_t iVerbose, const char* cfgname)
  : FairTask(name, iVerbose),
    fTree(NULL),
    fX(0.),
    fY(0.),
    fEMC(0.),
    fE(0.),
    fTh(0.),
    fERes(0.),
    fCurAlgo(0),
    fRemoveCharged(0),
    fUseCalibration(0),
    fRegions(0),
    fCalibration(0),
    fIntSteps(0),
    fPreClusterAlgo(),
    fMinClusterE(),
    fMinMaxE(),
    fTheta(),
    fPar1(),
    fPar2(),
    fPar3(),
    fParI(),
    fMC(NULL),
    fMCTr(NULL),
    fStr(NULL),
    fCal(NULL),
    fInf(NULL)
{
  Int_t i;
  TString st;
  CbmEcalParam* par=new CbmEcalParam("ClusterParam", cfgname);

  fRemoveCharged=par->GetInteger("removecharged");
  fUseCalibration=par->GetInteger("usecalibration");
  fRegions=par->GetInteger("regions");
  if (fRegions<0||fRegions>10)
  {
    Fatal("CbmEcalClusterFinderV2","Number of calorimeter regions to high.");
    delete par;
    return;
  }
  fCalibration=par->GetInteger("calibrationmode");

  fIntSteps=par->GetInteger("intsteps");
  for(i=0;i<fRegions;i++)
  {
    st="preclusteralgo["; st+=i; st+="]";
    fPreClusterAlgo[i]=par->GetInteger(st);
    st="minclustere["; st+=i; st+="]";
    fMinClusterE[i]=par->GetDouble(st);
    st="minmaxe["; st+=i; st+="]";
    fMinMaxE[i]=par->GetDouble(st);
    st="theta["; st+=i; st+="]";
    fTheta[i]=par->GetDouble(st);
    st="par1["; st+=i; st+="]";
    fPar1[i]=par->GetDouble(st);
    st="par2["; st+=i; st+="]";
    fPar2[i]=par->GetDouble(st);
    st="par3["; st+=i; st+="]";
    fPar3[i]=par->GetDouble(st);
    st="pari["; st+=i; st+="]";
    fParI[i]=par->GetInteger(st);
  }
  delete par;
}

/** Initing routine **/
InitStatus CbmEcalAnalysisBSh::Init()
{
  FairRootManager* fManager=FairRootManager::Instance();
  if (!fManager)
  {
    Fatal("Init", "Can't find a Root Manager.");
    return kFATAL;
  }
  fMCTr=(TClonesArray*)fManager->GetObject("MCTrack");
  if (!fMCTr)
  {
    Fatal("Init", "Can't find an array of MC tracks");
    return kFATAL;
  }
  fMC=(TClonesArray*)fManager->GetObject("EcalPoint");
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
  fCal=(CbmEcalCalibration*)fManager->GetObject("EcalCalibration");
  if (!fCal)
  {
    Fatal("Init", "Can't find EcalCalibration");
    return kFATAL;
  }  
  fTree=NULL;
  fInf=fStr->GetEcalInf();

  return kSUCCESS;
}

/** Finishing routine **/
void CbmEcalAnalysisBSh::Finish()
{
  if (fTree)
    fTree->Write();
}

ClassImp(CbmEcalAnalysisBSh)
