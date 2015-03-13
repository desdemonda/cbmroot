#include "CbmEcalClusterFinderV2.h"

#include "CbmEcalStructure.h"
#include "CbmEcalCell.h"
#include "CbmEcalInf.h"
#include "CbmEcalParam.h"
#include "CbmEcalPreCluster.h"
#include "CbmEcalCluster.h"
#include "CbmEcalCalibration.h"
#include "CbmEcalMaximum.h"

#include "FairRootManager.h"
#include "FairTrackParam.h"

#include "TClonesArray.h"
#include "TVector2.h"

#include <iostream>

using namespace std;

/** Exec a task **/
void CbmEcalClusterFinderV2::Exec(Option_t* option)
{
  fEv++;

  if (fCalibration)
    if (!fTree)
      CreateTree();
  FormPreClusters();

  if (fCalibration==0)
    FormClusters();
}

/** Form a preclusters.
 ** A precluster --- a group of cells neighbor to maximum cell.
 ** A cluster is a group of preclusters with common cells. **/
void CbmEcalClusterFinderV2::FormPreClusters()
{
  Int_t i;
  Int_t n;
  Int_t j;
  CbmEcalCell* cell;
  Double_t x;
  Double_t y;
  Double_t r;
  Double_t a;
  static Double_t z=fInf->GetZPos();
  CbmEcalMaximum* max;

  ClearPreClusters();
  n=fMaximums->GetEntriesFast();
  for(i=0;i<n;i++)
  {
    max=(CbmEcalMaximum*)fMaximums->At(i);
    /** Don't need empty maximums **/
    if (max==NULL) continue;
    /** and maximums due to charged tracks **/
    if (max->Mark()!=0) continue;
    cell=max->Cell();
    x=cell->GetCenterX();
    y=cell->GetCenterY();
    x*=x; y*=y;
    x+=y; r=TMath::Sqrt(x);
    a=TMath::ATan2(r, z)*TMath::RadToDeg();
    for(j=0;j<fRegions;j++)
      if (fTheta[j]>a)
	break;
//    cout << j << endl;
    max->SetRegion(j);
    if (fCalibration==0)
    {
      if (fPreClusterAlgo[j]==0)
        FormPreCluster(max);
      if (fPreClusterAlgo[j]==1)
        FormPreClusterNew(max);
    }
    else
    {
      fAlgo=j;
      fAlgoType=fPreClusterAlgo[j];
      fX=cell->GetCenterX();
      fY=cell->GetCenterY();
      fThetaOut=a;
      for(j=0;j<fRegions;j++)
      {
	fE[j]=-1111;
	fCurAlgo=j;
        if (fPreClusterAlgo[j]==0)
          FormPreCluster(max);
        if (fPreClusterAlgo[j]==1)
          FormPreClusterNew(max);
      }
      fTree->Fill();
    }
  }
}

/** A default algorithm of precluster formation **/
void CbmEcalClusterFinderV2::FormPreCluster(CbmEcalMaximum* max)
{
  CbmEcalCell* cell=max->Cell();
  Int_t reg=GetRegion(cell);
  if (fCalibration==1) reg=fCurAlgo;
  Int_t i;
  Int_t j;
  Double_t mine=cell->GetTotalEnergy();
  Double_t maxe=-1111;
  Double_t e;
  list<CbmEcalCell*> cells;
  list<CbmEcalCell*> cs;
  list<CbmEcalCell*>::const_iterator p;
  CbmEcalCell* min;
  
  mine=cell->GetTotalEnergy(); min=cell;
  /** Find minimum cell **/
  cell->GetNeighborsList(0, cells);
  for(p=cells.begin();p!=cells.end();++p)
    if ((*p)->GetTotalEnergy()<mine)
    {
      mine=(*p)->GetTotalEnergy();
      min=(*p);
    }

  /** Build a precluster **/
  j=max->I(); maxe=0;
  cell->GetNeighborsList(j, cells);
  for(p=cells.begin();p!=cells.end();++p)
    maxe+=(*p)->GetTotalEnergy();
  maxe+=cell->GetTotalEnergy();
  /** Check for calibration **/
  if (fCal)
    if (fCal->GetEnergy(maxe+mine, cell)<fMinClusterE[reg]) return;
  else
    if (maxe<fMinClusterE[reg]) return;
  cells.push_back(cell);
  if (fUseMinimumCell==1)
    if (find(cells.begin(), cells.end(), min)==cells.end())
      cells.push_back(min);
  if (fCalibration==0)
  {
    fPreClusters.push_back(new CbmEcalPreCluster(cells, max, min, mine+maxe)); 
    return;
  }
  else
  {
    fE[fCurAlgo]=maxe;
    return;
  }
}

/** A new algorithm of precluster formation **/
void CbmEcalClusterFinderV2::FormPreClusterNew(CbmEcalMaximum* maximum)
{
  CbmEcalCell* cell=maximum->Cell();
  Int_t i;
  Double_t e2m;
  Double_t e2=0;
  Int_t imax;
  list<CbmEcalCell*>::const_iterator p;
  list<CbmEcalCell*> cells;
  Double_t x;
  Double_t y;
  Double_t max;
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

  e2=max;
  cell->GetNeighborsList(maximum->I(), cells);
  for(p=cells.begin();p!=cells.end();++p)
  {
    e2+=(*p)->GetTotalEnergy();
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
  if (fCal)
    if (fCal->GetEnergy(e, cell)<fMinClusterE[reg]) return;
  else
    if (e<fMinClusterE[reg]) return;

  if (fCalibration==0)
  {
    fPreClusters.push_back(new CbmEcalPreCluster(cls, cCl2Size, maximum, NULL, e)); 
    return;
  }
  else
  {
    fE[fCurAlgo]=e;
    return;
  }
}

Double_t CbmEcalClusterFinderV2::SolveEllipse(CbmEcalCell* cell, Double_t cx, Double_t cy, Int_t reg)
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

/** Form clusters from precluster **/
void CbmEcalClusterFinderV2::FormClusters()
{
  /** CbmEcalCluster needs a destructor call :-( **/
  fClusters->Delete();
  Int_t fN=0;
  list<CbmEcalPreCluster*>::const_iterator p1=fPreClusters.begin();
  list<CbmEcalPreCluster*>::const_iterator p2;
  list<CbmEcalCell*> cluster;
  list<CbmEcalCell*>::const_iterator pc;
  list<CbmEcalCell*>::const_iterator pc1;
  list<CbmEcalMaximum*> maximums;
  list<Double_t> energy;
  UInt_t oldsize;
  Int_t MaxSize=0;
  Int_t Maximums=0;
  Int_t max;
  
  for(;p1!=fPreClusters.end();++p1)
  if ((*p1)->fMark==0)
  {
    cluster.clear(); oldsize=0; maximums.clear(); energy.clear();
    cluster=(*p1)->fCells;
    maximums.push_back((*p1)->fMax);
    energy.push_back((*p1)->fEnergy);
    max=1;
    while(cluster.size()!=oldsize)
    {
      oldsize=cluster.size();
      p2=p1;
      for(++p2;p2!=fPreClusters.end();++p2)
      if ((*p2)->fMark==0)
      {
        pc=cluster.begin();
	for(;pc!=cluster.end();++pc)
	{
	  if ((*p2)->fMinimum!=NULL) continue;
	  pc1=find((*p2)->fCells.begin(), (*p2)->fCells.end(), (*pc));
	  if (pc1==(*p2)->fCells.end()) continue;
	  break;
	}
	if (pc!=cluster.end())
	{
	  (*p2)->fMark=1;
	  pc=(*p2)->fCells.begin();
	  for(;pc!=(*p2)->fCells.end();++pc)
	    if (find(cluster.begin(), cluster.end(), (*pc))==cluster.end())
	      cluster.push_back(*pc);
	  maximums.push_back((*p2)->fMax);
	  energy.push_back((*p2)->fEnergy);
	  max++;
	}
      }
    }
    (*p1)->fMark=1;
    if ((Int_t)cluster.size()>MaxSize)
      MaxSize=cluster.size();
    if (max>Maximums) Maximums=max;
    CbmEcalCluster* cls=new ((*fClusters)[fN]) CbmEcalCluster(fN, cluster, maximums, energy); fN++;
 //   cls->Init(cluster);
  }
  if (fVerbose>0)
  {
    Info("FormClusters", "Total %d clusters formed.", fN);
    Info("FormClusters", "Maximum size of cluster is %d cells.",  MaxSize);
    Info("FormClusters", "Maximum number of photons per cluster is %d.",  Maximums);
  }
}

/** Get region number of the cell **/
Int_t CbmEcalClusterFinderV2::GetRegion(CbmEcalCell* cls)
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

/** Initialization **/
InitStatus CbmEcalClusterFinderV2::Init()
{
  FairRootManager* io=FairRootManager::Instance();
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
  fInf=fStr->GetEcalInf();
  if (fUseCalibration==1)
  {
    fCal=(CbmEcalCalibration*)io->GetObject("EcalCalibration");
    if (!fCal)
    {
      Fatal("Init", "Can't find EcalCalibration");
      return kFATAL;
    }
  }
  else
    fCal=NULL;
  fMaximums=(TClonesArray*)io->GetObject("EcalMaximums");
  if (fMaximums==NULL)
  {
    Fatal("Init", "Cant' find array of calorimeter maximums in the system");
    return kFATAL;
  }

  fClusters=new TClonesArray("CbmEcalCluster", 2000);
  io->Register("EcalClusters", "ECAL", fClusters, kTRUE);
  fEv=0;
  return kSUCCESS;
}

/** Destructor **/
CbmEcalClusterFinderV2::~CbmEcalClusterFinderV2()
{
  if (fClusters)
  {
    fClusters->Delete();
    delete fClusters;
  }
  if (fTree)
    delete fTree;
}

/** Only to comply with frame work. **/
CbmEcalClusterFinderV2::CbmEcalClusterFinderV2()
  : FairTask(),
    fEv(0),
    fStr(NULL),
    fInf(NULL),
    fCal(NULL),
    fTracks(NULL),
    fMaximums(NULL),
    fClusters(NULL),
    fPreClusters(),
    fRemoveCharged(0),
    fUseCalibration(0),
    fRegions(0),
    fPreClusterAlgo(),
    fMinClusterE(),
    fMinMaxE(),
    fTheta(),
    fCalibration(0),
    fUseMinimumCell(0),
    fPar1(),
    fPar2(),
    fPar3(),
    fParI(),
    fIntSteps(0),
    fTree(NULL),
    fE(),
    fAlgo(0),
    fAlgoType(0),
    fCurAlgo(0),
    fX(0.),
    fY(0.),
    fThetaOut(0.)
{
}

/** Finish a task **/
void CbmEcalClusterFinderV2::Finish()
{
  if (fTree)
    fTree->Write();
}

CbmEcalClusterFinderV2::CbmEcalClusterFinderV2(const char* name, const Int_t verbose, const char* cfg)
  : FairTask(name, verbose),
    fEv(0),
    fStr(NULL),
    fInf(NULL),
    fCal(NULL),
    fTracks(NULL),
    fMaximums(NULL),
    fClusters(NULL),
    fPreClusters(),
    fRemoveCharged(0),
    fUseCalibration(0),
    fRegions(0),
    fPreClusterAlgo(),
    fMinClusterE(),
    fMinMaxE(),
    fTheta(),
    fCalibration(0),
    fUseMinimumCell(0),
    fPar1(),
    fPar2(),
    fPar3(),
    fParI(),
    fIntSteps(0),
    fTree(NULL),
    fE(),
    fAlgo(0),
    fAlgoType(0),
    fCurAlgo(0),
    fX(0.),
    fY(0.),
    fThetaOut(0.)
{
  Int_t i;
  TString st;
  CbmEcalParam* par=new CbmEcalParam("ClusterParam", cfg);

  fUseCalibration=par->GetInteger("usecalibration");
  fRegions=par->GetInteger("regions");
  if (fRegions<0||fRegions>10)
  {
    Fatal("CbmEcalClusterFinderV2","Number of calorimeter regions to high.");
    delete par;
    return;
  }
  fCalibration=par->GetInteger("calibrationmode");
  fUseMinimumCell=par->GetInteger("useminimumcell");

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
  fTree=NULL;
}


/** Clear a preclusters list **/
void CbmEcalClusterFinderV2::ClearPreClusters()
{
  list<CbmEcalPreCluster*>::const_iterator p=fPreClusters.begin();
  for(;p!=fPreClusters.end();++p)
    delete (*p);
  fPreClusters.clear();
}


void CbmEcalClusterFinderV2::CreateTree()
{
  TString st;

  fTree=new TTree("c2","Preclusters information");
  fTree->Branch("ev", &fEv, "ev/I");
  fTree->Branch("algo", &fAlgo, "algo/I");
  fTree->Branch("algot", &fAlgoType, "algot/I");
  fTree->Branch("x", &fX, "x/D");
  fTree->Branch("y", &fY, "y/D");
  fTree->Branch("theta", &fThetaOut, "theta/D");
  st="e["; st+=fRegions; st+="]/D";
  fTree->Branch("e", fE, st);
}

ClassImp(CbmEcalClusterFinderV2)
