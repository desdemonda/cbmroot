#include "CbmEcalCluster.h"

#include "CbmEcalCell.h"
#include "CbmEcalMaximum.h"

#include "TMath.h"

#include <algorithm>
#include <iostream>

using namespace std;

/** An empty constructor **/
CbmEcalCluster::CbmEcalCluster()
  : TObject(),
    fNum(0),
    fSize(0),
    fMaxs(0),
    fEnergy(0.),
    fType(0.),
    fMoment(0.),
    fMomentX(0.),
    fMomentY(0.),
    fX(0.),
    fY(0.),
    fChi2(0.),
    fCellNums(),
    fPeakNums(),
    fPreEnergy(),
    fMaximums(NULL)
{
}

/** A standart constructor **/
CbmEcalCluster::CbmEcalCluster(Int_t num, const std::list<CbmEcalCell*>& cluster)
  : TObject(),
    fNum(num),
    fSize(0),
    fMaxs(0),
    fEnergy(0.),
    fType(0.),
    fMoment(0.),
    fMomentX(0.),
    fMomentY(0.),
    fX(0.),
    fY(0.),
    fChi2(0.),
    fCellNums(),
    fPeakNums(),
    fPreEnergy(),
    fMaximums(NULL)
{
  std::list<CbmEcalCell*> cls;

  fMaximums=NULL;
  cls.clear();
  list<CbmEcalCell*>::const_iterator p=cluster.begin();
  for(;p!=cluster.end();++p)
    cls.push_back(*p);
  Init(cls);
}

/** A more advanced constructor. Should use this. **/
CbmEcalCluster::CbmEcalCluster(Int_t num, const std::list<CbmEcalCell*>& cluster, const std::list<CbmEcalMaximum*>& maximums, const std::list<Double_t>& energy)
  : TObject(),
    fNum(num),
    fSize(0),
    fMaxs(0),
    fEnergy(0.),
    fType(0.),
    fMoment(0.),
    fMomentX(0.),
    fMomentY(0.),
    fX(0.),
    fY(0.),
    fChi2(0.),
    fCellNums(),
    fPeakNums(),
    fPreEnergy(),
    fMaximums(NULL)
{
  if (maximums.size()!=energy.size())
  {
    Fatal("CbmEcalCluster","Sizes of maximums and energy lists are different.");
    return;
  }
  
  std::list<CbmEcalCell*> cls;
  list<CbmEcalCell*>::const_iterator p=cluster.begin();
  list<CbmEcalMaximum*>::const_iterator pm;
  list<Double_t>::const_iterator pd;
  Double_t e;
  Double_t x;
  Double_t y;
  Double_t r;
  Int_t i;

  fMaxs=maximums.size();
  fEnergy=0;
  fType=0;
  fX=0;
  fY=0;
  for(;p!=cluster.end();++p)
  {
    cls.push_back(*p);
    e=(*p)->GetTotalEnergy();
    x=(*p)->GetCenterX();
    y=(*p)->GetCenterY();
    fX+=x*e; fY*=y*e;
    fEnergy+=e;
    fType+=(*p)->GetType();
  }
  fSize=cls.size();

  cls.sort(CbmEcalClusterSortProcess());
  cls.reverse();
  fType/=fSize;
  fX/=fEnergy;
  fY/=fEnergy;
  fChi2=-1111.0;

  fCellNums.Set(fSize);
  fPeakNums.Set(fMaxs);
  fPreEnergy.Set(fMaxs);

  fMomentX=0; fMomentY=0; fMoment=0;
  for(p=cls.begin();p!=cls.end();++p)
  {
    /** Still not clear about next 3 variables **/
    e=(*p)->GetTotalEnergy();
    x=fX-(*p)->GetCenterX(); x*=x;
    y=fY-(*p)->GetCenterY(); y*=y;
    fMomentX+=x*e; fMomentY+=y*e; fMoment+=(x+y)*e;
  }
  fMomentX/=fEnergy; fMomentY/=fEnergy; fMoment/=fEnergy;

  i=0;
  for(p=cls.begin();p!=cls.end();++p)
    fCellNums[i++]=(*p)->GetCellNumber();
  i=0; pd=energy.begin();
  fMaximums=new CbmEcalMaximum*[fMaxs];
  for(pm=maximums.begin();pm!=maximums.end();++pm)
  {
    fPreEnergy[i]=(*pd); ++pd;
    fMaximums[i]=(*pm);
    fPeakNums[i++]=(*pm)->Cell()->GetCellNumber();
  }
}
/** An virtual destructor **/
CbmEcalCluster::~CbmEcalCluster()
{
  delete [] fMaximums;
}

/** An initialization **/
void CbmEcalCluster::Init(std::list<CbmEcalCell*>& fCells)
{
  list<CbmEcalCell*>::const_iterator p1;
  list<CbmEcalCell*> neib;
  list<CbmEcalCell*> fPeaks;
  Double_t e;
  Double_t x;
  Double_t y;
  Double_t r;
  Double_t ar=0;
  Double_t tx=0;
  Double_t ty=0;
  Double_t tr=0;
  Int_t i;

  fCells.sort(CbmEcalClusterSortProcess());
  fCells.reverse();
  list<CbmEcalCell*>::const_iterator p=fCells.begin();
  fEnergy=0;
  fX=0;
  fY=0;
  fType=0;

  fSize=fCells.size();
  fPeaks.clear();
  for(;p!=fCells.end();++p)
  {
    e=(*p)->GetTotalEnergy();
    fEnergy+=e;
    x=(*p)->GetCenterX(); y=(*p)->GetCenterY();
    fX+=e*x; x*=x; tx+=e*x;
    fY+=e*y; y*=y; ty+=e*y;
    r=TMath::Sqrt(x+y);
    ar+=e*r; tr+=e*(x+y);
    fType+=(*p)->GetType();
    neib.clear();
    (*p)->GetNeighborsList(0, neib);
    for(p1=neib.begin(); p1!=neib.end();++p1)
      if ((*p1)->GetTotalEnergy()>e) break;
    if (p1!=neib.end()) continue;
    for(p1=neib.begin(); p1!=neib.end();++p1)
      if (find(fPeaks.begin(), fPeaks.end(), *p1)!=fPeaks.end()) break;
    if (p1!=neib.end()) continue;
    fPeaks.push_back(*p);
  }
  fX/=fEnergy;
  fY/=fEnergy;
  fType/=fSize;
  fMaxs=fPeaks.size();
  fChi2=-1111;
  fMomentX=0; fMomentY=0; fMoment=0;
  for(p=fCells.begin();p!=fCells.end();++p)
  {
    /** Still not clear about next 3 variables **/
    e=(*p)->GetTotalEnergy();
    x=fX-(*p)->GetCenterX(); x*=x;
    y=fY-(*p)->GetCenterY(); y*=y;
    fMomentX+=x*e; fMomentY+=y*e; fMoment+=(x+y)*e;
  }
  fMomentX/=fEnergy; fMomentY/=fEnergy; fMoment/=fEnergy;
  fCellNums.Set(fSize);
  fPeakNums.Set(fMaxs);
  fPreEnergy.Set(fMaxs);
  i=0;
  for(p=fCells.begin();p!=fCells.end();++p)
    fCellNums[i++]=(*p)->GetCellNumber();
  i=0;
  for(p=fPeaks.begin();p!=fPeaks.end();++p)
  {
    fPreEnergy[i]=-1111.0;
    fPeakNums[i++]=(*p)->GetCellNumber();
  }
  fMaximums=NULL;
}

ClassImp(CbmEcalCluster)
