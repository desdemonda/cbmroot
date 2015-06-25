#include "CbmEcalRecoCorr.h"

#include "TTree.h"
#include "TClonesArray.h"
#include "TVector3.h"

#include "FairRootManager.h"
#include "FairTrackParam.h"
#include "FairLogger.h"

#include "CbmEcalStructure.h"
#include "CbmEcalInf.h"
#include "CbmEcalSCurveLib.h"
#include "CbmEcalCell.h"
#include "CbmEcalRecParticle.h"
#include "CbmEcalCluster.h"
#include "CbmEcalShLibCorr.h"
#include "CbmEcalCalibration.h"
#include "CbmEcalParam.h"
#include "CbmEcalPosLib.h"

#include "TMatrixD.h"
#include "TMath.h"

#include <iostream>

using namespace std;

void FCNEcalClusterCorr::SetCluster(CbmEcalCluster* cluster)
{
  Int_t i;

  fCluster=cluster;

  fClsCells.resize(fCluster->Size());
  for(i=0;i<fCluster->Size();i++)
    fClsCells[i]=fStr->GetHitCell(fCluster->CellNum(i));

  SetN(fCluster->Maxs());
  fClusterResp=fCluster->Energy();
  fClusterEnergy=fCal->GetEnergy(fClusterResp, fStr->GetHitCell(cluster->PeakNum(0)));
}

void FCNEcalClusterCorr::SetN(Int_t n)
{
//  Int_t i;
  fN=n;
  fNDF=fCluster->Size()-(n*3-fFixClusterEnergy);
}

Double_t FCNEcalClusterCorr::Correlation(CbmEcalCell* c1, CbmEcalCell* c2, Double_t x, Double_t y, Double_t phi, Double_t theta, Double_t e) const
{
  if (c1->GetType()==c2->GetType())
    return fShLib->Correlation(c1->X1()-x, c1->Y1()-y, c2->X1()-x, c2->Y1()-y, phi, theta, e, c1);
  if (c1->GetType()>c2->GetType())
    return fShLib->Correlation2(c1->X1()-x, c1->Y1()-y, c2->X1()-x, c2->Y1()-y, phi, theta, e, c1->GetType());
  else
    return fShLib->Correlation2(c2->X1()-x, c2->Y1()-y, c1->X1()-x, c1->Y1()-y, phi, theta, e, c2->GetType());
}

Double_t FCNEcalClusterCorr::DoEval(const Double_t* par) const
{
//  if (par.size()!=fN*3)
//    Fatal("()", "Size of input vector is wrong (%d, should be %d)!", par.size(), fN*3);
  Int_t s=fCluster->Size();
  TMatrixD m(s, s);
  Int_t i;
  Int_t j;
  Int_t k;
  Double_t t;
  Double_t det[10];
  Double_t chi2=0;
  Double_t de[30];
  /** Energy and coordinates of reconstructing particle **/
  Double_t e[6];
  Double_t x[6];
  Double_t y[6];
  Double_t theta[6];
  Double_t phi[6];

  // Extract photons energy from the parameters
  if (fFixClusterEnergy==0)
    for(i=0;i<fN;i++)
      e[i]=par[3*i];
  else
  {
    t=fClusterEnergy;
    for(i=0;i<fN-1;i++)
    {
      e[i]=par[3*i+2]*t;
      t-=e[i];
      if (t<0) t=0;
    }
    e[i]=t;
  }
  
  // Extract coordinates and angles 
  for(i=0;i<fN;i++)
  {
    x[i]=par[3*i+1-fFixClusterEnergy];
    y[i]=par[3*i+2-fFixClusterEnergy];
    t=TMath::Sqrt(x[i]*x[i]+y[i]*y[i]);

    theta[i]=TMath::ATan(t/fInf->GetZPos());
    theta[i]*=TMath::RadToDeg();
    phi[i]=TMath::ACos(x[i]/t)*TMath::RadToDeg();
    if (y[i]<0) phi[i]=360.0-phi[i];
  }

  // Calculate covariance matrix
  for(i=0;i<s;i++)
  for(j=0;j<s;j++)
  {
    m[i][j]=0.0;
    for(k=0;k<fN;k++)
    {
      m[i][j]+=Correlation(fClsCells[i], fClsCells[j], x[k], y[k], phi[k], theta[k], e[k]);
    }
  }
  //Some magic numbers: TODO?
  for(i=0;i<s;i++)
  {
    if (m[i][i]<1e-8*fClusterEnergy) m[i][i]=1e-8*fClusterEnergy;
//    if (fClsCells[i]->GetType()==2&&m[i][j]<1e-10) m[i][j]=1e-10;    
//    if (fClsCells[i]->GetType()==4&&m[i][j]<1e-15) m[i][j]=1e-15;    
  }

  // Inversion
  m.Invert(det);

  // Energy difference calculation
  for(i=0;i<s;i++)
  {
    de[i]=fClsCells[i]->GetTotalEnergy();
    for(k=0;k<fN;k++)
      de[i]-=fShLib->GetResponse(fClsCells[i]->X1()-x[k], fClsCells[i]->Y1()-y[k], phi[k], theta[k], e[k], fClsCells[i]);
  }

  // Chi2 calculation
  for(i=0;i<s;i++)
  for(j=0;j<s;j++)
    chi2+=de[i]*de[j]*m[i][j];

  return chi2;
}

/***************************************************************************/

void CbmEcalRecoCorr::Exec(Option_t* option)
{
  Int_t i;
  Int_t n=fClusters->GetEntriesFast();
  CbmEcalCluster* cluster;
  list<CbmEcalCell*>::const_iterator p;
  Double_t chi2;
  CbmEcalCell* cell;
  Int_t j;

  fN=0;
  fReco->Delete();
  if (fToTree&&fOutTree==NULL)
    CreateTree();
  fEventN++;
  if (fVerbose>0) 
    Info("Exec", "Event %d, %d clusters in event.", fEventN, n);
  for(i=0;i<n;i++)
  {
    fNOld=fN;
    cluster=(CbmEcalCluster*)fClusters->At(i);
    if (cluster->Maxs()>fMaxPhotonsPerCluster)
      continue;
//    cout << cluster->Maxs() << endl;
    for(j=0;j<cluster->Maxs();j++)
    {
      cell=fStr->GetHitCell(cluster->CellNum(j));
      fPreE=cluster->PreEnergy(j);
      if (fCal->GetEnergy(cell->GetTotalEnergy(), cell)>fMinMaxE)
        Reco(cell, cluster);
    }
    chi2=CalculateChi2(cluster);
    if (chi2>fChi2Th)
      FitCluster(cluster);
    if (fStoreClusterInfo)
      WriteClusterInfo(cluster);
    if (fOutTree)
      FillTree();
  }
  if (fVerbose>1)
    Info("Exec", "%d photons reconstructed in calorimeter.", fN);
}

/** Write a cluster info **/
void CbmEcalRecoCorr::WriteClusterInfo(CbmEcalCluster* clstr)
{
  list<CbmEcalCell*>::const_iterator p;  
  CbmEcalCell* cell;
  CbmEcalRecParticle* part;
  Int_t i=0;
  Int_t j;
  Int_t k;
  Double_t celle;
//  Double_t cellsize;
  Double_t e[3];
  Double_t x=-1111;
  Double_t y=-1111;
  Double_t r;
  Double_t theta;
  Double_t phi;
  CbmEcalCell* cl;
  Double_t cx;
  Double_t cy;
  Double_t de;
//  Double_t epred;
//  Double_t emeas;
//  Double_t clenergy;
  Double_t cellerr=-1111;
  static Double_t module=fInf->GetModuleSize();

  fECluster=fCal->GetEnergy(clstr->Energy(), fStr->GetHitCell(clstr->PeakNum(0)));
//  clenergy=fECluster;
  for(k=0;k<clstr->Size();k++)
  {
    cell=fStr->GetHitCell(clstr->CellNum(k));
    e[i++]=cell->GetTotalEnergy();
    if (i==3) break;
  }
  fCMaxs=clstr->Maxs();
  fECls=e[0]/clstr->Energy();
  fE2Cls=(e[0]+e[1])/clstr->Energy();
  fE3Cls=(e[0]+e[1]+e[2])/clstr->Energy();
  i=0;

  for(k=0;k<clstr->Size();k++)
  {
    cell=fStr->GetHitCell(clstr->CellNum(k));
    fTypes[i]=cell->GetType();
 //   cellsize=module/fTypes[i];
    celle=0;
    for(j=fNOld;j<fN;j++)
    {
      part=(CbmEcalRecParticle*)fReco->At(j);
      cl=fStr->GetHitCell(part->CellNum()); cx=cl->GetCenterX(); cy=cl->GetCenterY(); 
      x=cell->GetCenterX(); x-=part->X(); // x-=cx; 
      y=cell->GetCenterY(); y-=part->Y(); // y-=cy; 

      r=TMath::Sqrt(cx*cx+cy*cy);

      /** TODO: should be Z of the cell**/
      theta=TMath::ATan(r/fInf->GetZPos());
      theta*=TMath::RadToDeg();
      phi=TMath::ACos(cx/r)*TMath::RadToDeg();
      if (cy<0) phi=360.0-phi;

      celle+=fShLib->GetResponse(x, y, phi, theta, part->E(), fTypes[i]);
    }
    de=fCal->GetEnergy(celle, cell)-fCal->GetEnergy(cell->GetTotalEnergy(), cell);
    cx=cell->GetCenterX(); cy=cell->GetCenterY();
    r=TMath::Sqrt(cx*cx+cy*cy);
    theta=TMath::ATan(r/fInf->GetZPos());

//    epred=fCal->GetEnergy(celle, cell);
//    emeas=fCal->GetEnergy(cell->GetTotalEnergy(), cell);
    de*=de; fChi2Fit[i]=de/cellerr;

    cx=cell->GetCenterX(); cy=cell->GetCenterY();
    r=TMath::Sqrt(cx*cx+cy*cy);
    fTheta[i]=TMath::ATan(r/fInf->GetZPos());
    fEpred[i]=fCal->GetEnergy(celle, cell);
    fEmeas[i]=fCal->GetEnergy(cell->GetTotalEnergy(), cell);
    fCX[i]=cell->GetCenterX();
    fCY[i]=cell->GetCenterY();
    i++;
  }
  for(j=i;j<50;j++)
    fTypes[j]=-1111;
}

void CbmEcalRecoCorr::FillTree()
{
  Int_t n=fN-fNOld;
  CbmEcalRecParticle* p;
  Int_t i;

  for(i=0;i<n;i++)
  {
    p=(CbmEcalRecParticle*)fReco->At(i+fNOld);
    fChi2=p->Chi2();
    fEReco=p->E();
    fXReco=p->X();
    fYReco=p->Y();
    fOutTree->Fill();
  }

}

/** Fit a given cluster. A first approximation should be available **/
void CbmEcalRecoCorr::FitCluster(CbmEcalCluster* clstr)
{
  vector<CbmEcalCell*> cells(fN-fNOld);
  Int_t i;
  Int_t n=fN-fNOld;
  Int_t ret;
  Double_t oldchi2=clstr->Chi2();
  Double_t e;
  Double_t pe[6];
  CbmEcalRecParticle* p;
  TString par;
  TVector3 tv;

  if (n>6)
  {
    Info("FitCluster", "Give up fitting the cluster with %d maximums. ", n);
    return;
  }

  fFCN->SetCluster(clstr);
  // The cluster it self  doesn't care about amplitude of maximums 

  fFCN->SetN(n);

  Int_t params=0;

  e=0;
  for(i=0;i<n;i++)
  {
    p=(CbmEcalRecParticle*)fReco->At(i+fNOld);
    e+=p->E();
  }

  fFCN->SetClusterEnergy(e);
  fFitter->Clear();
  fFitter->SetPrintLevel(-1);
  fFitter->SetMaxIterations(fMaxIterations);

  if (fFixClusterEnergy==0) {
    for(i=0;i<n;i++) {
      p=(CbmEcalRecParticle*)fReco->At(i+fNOld);
      cells[i]=fStr->GetHitCell(p->CellNum());
      par="e"; par+=i; 
      fFitter->SetVariable(i*3   ,par.Data(), p->E(), 0.01*p->E());
      fFitter->SetVariableLimits(i*3, p->E()*0.2, p->E()*2.0);
      params++;
      par="x"; par+=i; 
      fFitter->SetVariable(i*3+1 ,par.Data(), p->X(), 0.1);
      fFitter->SetVariableLimits(i*3+1, p->X()-9., p->X()+9.);
      params++;
      par="y"; par+=i; 
      fFitter->SetVariable(i*3+2 ,par.Data(), p->Y(), 0.1);
      fFitter->SetVariableLimits(i*3+2, p->Y()-9., p->Y()+9.);
      params++;
    } 
  } else {
    e=0;
    for(i=0;i<n;i++) {
      p=(CbmEcalRecParticle*)fReco->At(i+fNOld);
      pe[i]=p->E(); e+=pe[i];
    }
    for(i=0;i<n;i++) {
      p=(CbmEcalRecParticle*)fReco->At(i+fNOld);
      cells[i]=fStr->GetHitCell(p->CellNum());
      par="x"; par+=i; 
      fFitter->SetVariable(i*3 ,par.Data(), p->X(), 0.1);
      fFitter->SetVariableLimits(i*3,p->X()-9., p->X()+9.);
      params++;
      par="y"; par+=i; 
      fFitter->SetVariable(i*3+1 ,par.Data(), p->Y(), 0.1);
      fFitter->SetVariableLimits(i*3+1, p->Y()-9., p->Y()+9.);
      if (i!=n-1) {
        par="n";
	par+=i;
	fFitter->SetVariable(i*3+2 ,par.Data(), p->E()/e, 0.005);
	fFitter->SetVariableLimits(i*3+2, 0.0, 1.0);
      }
      e-=p->E();
    }

  }

  fFCN->SetNumParam(params);
  fFCN->SetParticleSeeds(cells);

  fFitter->SetFunction(*fFCN);
  ret=fFitter->Minimize();

  Double_t chi2=fFitter->MinValue();

  if (ret!=0&&fVerbose>1)
  {
    Info("FitCluster", "Minimization failed! Last chi2 %f, old chi2 %f.", chi2, oldchi2);
  }

//  cout << "chi2=" << chi2 << ", NDF=" << fFCN->NDF() << endl;
  //TODO: A temporary comment
  if (fFCN->NDF()>0)
    chi2/=fFCN->NDF();
  else
    chi2=-chi2;

  clstr->fChi2=chi2;


  e=fFCN->ClusterEnergy();

  // get the fit parameters from Minuit
  const double *fitResult = fFitter->X();

  for(i=0;i<n;i++) {
    p=(CbmEcalRecParticle*)fReco->At(i+fNOld);
    p->fX=fitResult[i*3+1-fFixClusterEnergy];
    p->fY=fitResult[i*3+2-fFixClusterEnergy];
    LOG(DEBUG2) << "fitResult[" << i*3+1-fFixClusterEnergy << "]: " << fitResult[i*3+1-fFixClusterEnergy] << FairLogger::endl;
    LOG(DEBUG2) << "fitResult[" << i*3+2-fFixClusterEnergy << "]: " << fitResult[i*3+2-fFixClusterEnergy] << FairLogger::endl;
    if (fFixClusterEnergy) {
      if (i!=n-1) {
        p->fE=fitResult[i*3+2]*e;
	LOG(DEBUG2) << "fitResult[" << i*3+2 << "]: " << fitResult[i*3+2] << FairLogger::endl;
	e-=p->fE;
      } else {
 	p->fE=e;
      }
    } else {
      p->fE=fitResult[i*3];
	LOG(DEBUG2) << "fitResult[" << i*3 << "]: " << fitResult[i*3] << FairLogger::endl;
    }

    tv.SetXYZ(p->fX, p->fY, fInf->GetZPos());
    tv*=p->fE/tv.Mag();
    p->fPx=tv.X();
    p->fPy=tv.Y();
    p->fPz=tv.Z();
    p->fChi2=chi2;
  }
//  cout << " ----------------------------------------------------------- " << endl;
//  if (TMath::Abs(chi2)>10)
    CalculateChi2(clstr);
}

/** Calculate a chi2 for just reconstructed photons **/
Double_t CbmEcalRecoCorr::CalculateChi2(CbmEcalCluster* cluster)
{
  if (!fShLib) return -1111;

  Int_t s=cluster->Size();
  TMatrixD m(s, s);
  Int_t i;
  Int_t j;
  Int_t k;
  Double_t t;
  Double_t det[10];
  Double_t chi2=0;
  Double_t de[30];
  /** Energy and coordinates of reconstructing particle **/
  Double_t e[6];
  Double_t et=0;
  Double_t x[6];
  Double_t y[6];
  Double_t theta[6];
  Double_t phi[6];
  CbmEcalRecParticle* part;
  vector<CbmEcalCell*> clscells(s);
  Int_t ndf=s-((fN-fNOld)*3-fFixClusterEnergy);

//  cout << "Cell codes:";
  // Copy cluster to a vector
  for(i=0;i<s;i++)
  {
    clscells[i]=fStr->GetHitCell(cluster->CellNum(i));
//    cout << (Int_t)clscells[i]->GetType();
  }
//  cout << endl;

  // Extract photons energy from the parameters
  // Extract coordinates and angles 
  for(i=0;i<fN-fNOld;i++)
  {
    part=(CbmEcalRecParticle*)fReco->At(i);
    e[i]=part->E(); et+=e[i];
    x[i]=part->X();
    y[i]=part->Y();
    t=TMath::Sqrt(x[i]*x[i]+y[i]*y[i]);

    theta[i]=TMath::ATan(t/fInf->GetZPos());
    theta[i]*=TMath::RadToDeg();
    phi[i]=TMath::ACos(x[i]/t)*TMath::RadToDeg();
    if (y[i]<0) phi[i]=360.0-phi[i];
//    cout << "(" << x[i] << ", " << y[i] << "): " << phi[i] << " and " << theta[i] << endl;
  }

  // Calculate covariance matrix
  for(i=0;i<s;i++)
  for(j=0;j<s;j++)
  {
    m[i][j]=0.0;
    for(k=0;k<fN-fNOld;k++)
    {
      m[i][j]+=fFCN->Correlation(clscells[i], clscells[j], x[k], y[k], phi[k], theta[k], e[k]);
//      cout << "Corr(" << x[k] << "," << y[k] << ",phi:" << phi[k];
//      cout << ",theta:" << theta[k] << ",e:" << e[k];
//      cout << ")=" << fFCN->Correlation(clscells[i], clscells[j], x[k], y[k], phi[k], theta[k], e[k])*1e6 << ":" << i << ";" << j << endl;
    };
  }
  //Some magic numbers: TODO?
  for(i=0;i<s;i++)
  {
    if (m[i][i]<1e-8*et) m[i][i]=1e-8*et;
//    if (clscells[i]->GetType()==2&&m[i][i]<1e-10) m[i][i]=1e-10;    
//    if (clscells[i]->GetType()==4&&m[i][i]<1e-15) m[i][i]=1e-15;    
  }

  // Inversion
//  m.Print();
  m.Invert(det);
//  m.Print();
  // Energy difference calculation
  for(i=0;i<s;i++)
  {
    de[i]=clscells[i]->GetTotalEnergy();
    for(k=0;k<fN-fNOld;k++)
    {
      de[i]-=fShLib->GetResponse(clscells[i]->X1()-x[k], clscells[i]->Y1()-y[k], phi[k], theta[k], e[k], clscells[i]);
//      cout << "Energy(" << x[k] << "," << y[k] << ",phi:" << phi[k] << ",theta";
//      cout << theta[k] << ",e:" << e[k] << ")=";
//      cout << fShLib->GetResponse(clscells[i]->X1()-x[k], clscells[i]->Y1()-y[k], phi[k], theta[k], e[k], clscells[i]) << endl;
    }
  }

  // Chi2 calculation
  for(i=0;i<s;i++)
  for(j=0;j<s;j++)
    chi2+=de[i]*de[j]*m[i][j];

  return chi2/ndf;
}

/** Get first approximation of photon parameters from maximum **/
void CbmEcalRecoCorr::Reco(CbmEcalCell* cell, CbmEcalCluster* clstr)
{
  if (fLib==NULL)
  {
    Reco2(cell, clstr);
    return;
  }
  Double_t x;
  Double_t y;
  Double_t z;
  Double_t amp;
  Double_t en;
  Double_t maxe;
  Double_t px;
  Double_t py;
  Double_t pz;
  Int_t type;
  list<CbmEcalCell*> cells;
  list<CbmEcalCell*>::const_iterator p;
  Int_t cnum=0;

  cells.clear();

  fType=cell->GetType();
  fDiffType=0;

  cell->GetNeighborsList(0,cells);
  fE=cell->GetEnergy();
  en=cell->GetEnergy();
  type=cell->GetType();
  fAX=0; fAY=0;
  for(p=cells.begin();p!=cells.end();++p) 
  {
    if (type!=(*p)->GetType())
    {
      type=-1111;
      fDiffType=1;
    }
    if (cell->GetX2()+0.001<(*p)->GetCenterX())
      fAX+=(*p)->GetEnergy();
    else
    if (cell->GetX1()-0.001>(*p)->GetCenterX())
      fAX-=(*p)->GetEnergy();
    if (cell->GetY2()+0.001<(*p)->GetCenterY())
      fAY+=(*p)->GetEnergy();
    else
    if (cell->GetY1()-0.001>(*p)->GetCenterY())
      fAY-=(*p)->GetEnergy();
    en+=(*p)->GetEnergy();
  }
  fE3x3=en;

  if (fAX!=-1111)
  {
    fAX/=fE3x3;
    fAY/=fE3x3;
  }

  maxe=0;
  for(Int_t i=1;i<5;i++)
  {
    cell->GetNeighborsList(i,cells);
    en=cell->GetEnergy();
    for(p=cells.begin();p!=cells.end();++p) 
      en+=(*p)->GetEnergy();
    if (maxe<en)
    {
      maxe=en;
      cnum=i;
    }
  }
  cell->GetNeighborsList(cnum, cells);
  cells.push_back(cell);
  fE2x2=maxe;

  fCellX=cell->GetCenterX();
  fCellY=cell->GetCenterY();
  
  if (fCal->GetEnergy(fE2x2, cell)<0.1) return; 
  fEReco=fCal->GetEnergy(fE2x2, cell);
  if (fAX!=-1111)
    fXReco=fLib->GetX(fAX, fEReco, cell);
  else
    fXReco=-1111;
  if (fXReco!=-1111)
    fXReco+=cell->GetCenterX();
  
  if (fAY!=-1111)
    fYReco=fLib->GetY(fAY, fEReco, cell);
  else
    fYReco=-1111;
  
  if (fYReco!=-1111)
    fYReco+=cell->GetCenterY();
  if (fXReco!=-1111)
    x=fXReco;
  else
    x=cell->GetCenterX();
  if (fYReco!=-1111)
    y=fYReco;
  else
    y=cell->GetCenterY();
  z=fInf->GetZPos();
  amp=TMath::Sqrt(x*x+y*y+z*z);
  px=fEReco*x/amp;
  py=fEReco*y/amp;
  pz=fEReco*z/amp;
//  cout << fAX << " --- " << fAY << endl;
//  cout << fXReco << " and " << fYReco << endl;
  new((*fReco)[fN++]) CbmEcalRecParticle(px, py, pz, fEReco, x, y, z, 22, -1111, clstr, cell, fType);
}

/** Get first approximation of photon parameters from maximum
 ** This version uses PosLib instead of S-curves
 ** TODO Architecture: Need a maximum, instead of cell here **/
void CbmEcalRecoCorr::Reco2(CbmEcalCell* cell, CbmEcalCluster* clstr)
{
  if (fPosLib==NULL)
    Fatal("Reco2", "No position library for reconstruction.");

  list<CbmEcalCell*> cells;
  list<CbmEcalCell*>::const_iterator p;
  Double_t px;
  Double_t py;
  Double_t pz;
  Double_t amp;
  Double_t x=-1111;
  Double_t y=-1111;
  Double_t z=-1111;
  Double_t t;

  t=TMath::Sqrt(x*x+y*y);
  fEReco=fCal->GetEnergy(fPreE, t/z, fRegion);
  z=fInf->GetZPos();
  amp=TMath::Sqrt(x*x+y*y+z*z);
  px=fEReco*x/amp;
  py=fEReco*y/amp;
  pz=fEReco*z/amp;
  new((*fReco)[fN++]) CbmEcalRecParticle(px, py, pz, fEReco, x, y, z, 22, -1111, clstr, cell, fType);

}
void CbmEcalRecoCorr::CreateTree()
{
  fOutTree=new TTree("reco_corr", "Reconstruction by fitting with shower library (correlations are in)");
  fOutTree->Branch("ev", &fEventN, "ev/I");
  fOutTree->Branch("type", &fType, "type/S");
  fOutTree->Branch("difftype", &fDiffType, "difftype/S");
  fOutTree->Branch("x", &fXReco, "x/D");
  fOutTree->Branch("y", &fYReco, "y/D");
  fOutTree->Branch("e", &fEReco, "e/D");
  fOutTree->Branch("chi2", &fChi2, "chi2/D");
  fOutTree->Branch("ecluster", &fECluster, "ecluster/D");
  if (fStoreClusterInfo==kFALSE) return;
  fOutTree->Branch("cls_e", &fECls, "cls_e/D");
  fOutTree->Branch("cls_e2", &fE2Cls, "cls_e2/D");
  fOutTree->Branch("cls_e3", &fE3Cls, "cls_e3/D");
  fOutTree->Branch("theta", fTheta, "theta[50]/D");
  fOutTree->Branch("ctypes", fTypes, "ctypes[50]/S");
  fOutTree->Branch("cemeas", fEmeas, "cemeas[50]/D");
  fOutTree->Branch("cepred", fEpred, "cepred[50]/D");
  fOutTree->Branch("cchi2", fChi2Fit, "cchi2[50]/D");
  fOutTree->Branch("cefirst", fEfirst, "cefirst[50]/D");
  fOutTree->Branch("cchi2first", fChi2First, "cchi2first[50]/D");
  fOutTree->Branch("cx", fCX, "cx[50]/D");
  fOutTree->Branch("cy", fCY, "cy[50]/D");
  fOutTree->Branch("cmaxs", &fCMaxs, "cmaxs/I");
}

/** Default constructor. Requirement of ROOT system **/
CbmEcalRecoCorr::CbmEcalRecoCorr() 
  : FairTask(),
    fN(0),
    fNOld(0),
    fMaximums(NULL),
    fExcluded(NULL),
    fEventN(0),
    fStr(NULL),
    fInf(NULL),
    fLib(NULL),
    fPosLib(NULL),
    fShLib(NULL),
    fCal(NULL),
    fTracks(NULL),
    fReco(NULL),
    fClusters(NULL),
    fRecoName(""),
    fToTree(kFALSE),
    fOutTree(NULL),
    fStoreClusterInfo(kFALSE),
    fType(0),
    fDiffType(0),
    fE(0.),
    fE3x3(0.),
    fE2x2(0.),
    fEReco(0.),
    fXReco(0.),
    fYReco(0.),
    fECls(0.),
    fE2Cls(0.),
    fE3Cls(0.),
    fECluster(0.),
    fAX(0.),
    fAY(0.),
    fCellX(0.),
    fCellY(0.),
    fChi2(0.),
    fTheta(),
    fTypes(),
    fEmeas(),
    fEpred(),
    fChi2Fit(),
    fEfirst(),
    fChi2First(),
    fCX(),
    fCY(),
    fCMaxs(0),
    fChi2Th(0.),
    fEStep(0.),
    fCStep(0.),
    fMaxPhotonsPerCluster(0),
    fMaxIterations(0),
    fFixClusterEnergy(0),
    fFCN(NULL),
    fFitter(NULL),
    fMinMaxE(0.),
    fUsePosLib(0),
    fPreE(0.),
    fRegion(0)
{
}

/** Standard constructor **/
CbmEcalRecoCorr::CbmEcalRecoCorr(const char *name, const Int_t iVerbose, const char* configname)
  : FairTask(name, iVerbose),
    fN(0),
    fNOld(0),
    fMaximums(NULL),
    fExcluded(NULL),
    fEventN(0),
    fStr(NULL),
    fInf(NULL),
    fLib(NULL),
    fPosLib(NULL),
    fShLib(NULL),
    fCal(NULL),
    fTracks(NULL),
    fReco(NULL),
    fClusters(NULL),
    fRecoName("EcalReco"),
    fToTree(kFALSE),
    fOutTree(NULL),
    fStoreClusterInfo(kFALSE),
    fType(0),
    fDiffType(0),
    fE(0.),
    fE3x3(0.),
    fE2x2(0.),
    fEReco(0.),
    fXReco(0.),
    fYReco(0.),
    fECls(0.),
    fE2Cls(0.),
    fE3Cls(0.),
    fECluster(0.),
    fAX(0.),
    fAY(0.),
    fCellX(0.),
    fCellY(0.),
    fChi2(0.),
    fTheta(),
    fTypes(),
    fEmeas(),
    fEpred(),
    fChi2Fit(),
    fEfirst(),
    fChi2First(),
    fCX(),
    fCY(),
    fCMaxs(0),
    fChi2Th(0.),
    fEStep(0.),
    fCStep(0.),
    fMaxPhotonsPerCluster(0),
    fMaxIterations(0),
    fFixClusterEnergy(0),
    fFCN(NULL),
    fFitter(NULL),
    fMinMaxE(0.),
    fUsePosLib(0),
    fPreE(0.),
    fRegion(0)
{
  Int_t i;
  Int_t j;
  Int_t types;
  Int_t consts;
  TString nm;

  CbmEcalParam* par=new CbmEcalParam("RecoSlowParam", configname);

  fChi2Th=par->GetDouble("chi2th");
  fEStep=par->GetDouble("estep");
  fCStep=par->GetDouble("cstep");
  fMaxIterations=par->GetInteger("maxiterations");
  fFixClusterEnergy=par->GetInteger("fixclusterenergy");
  fMaxPhotonsPerCluster=par->GetInteger("maxphotonspercluster");
  fMinMaxE=par->GetDouble("minmaxe");
  fUsePosLib=par->GetInteger("useposlib");

  Info("Constructor", "chi2 threshold is %f, Estep is %f, and Cstep is %f.", fChi2Th, fEStep, fCStep);

  delete par;
}

CbmEcalRecoCorr::~CbmEcalRecoCorr()
{
  fReco->Delete();
  delete fReco;
}

void CbmEcalRecoCorr::Finish()
{
  if (fOutTree) fOutTree->Write();
}

/** Init **/
InitStatus CbmEcalRecoCorr::Init()
{
  fEventN=0;
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
  fTracks=(TClonesArray*)io->GetObject("EcalTrackParam");
  if (!fTracks)
  {
    Fatal("Init", "Can't find EcalTrackParam.");
    return kFATAL;
  }
  fClusters=(TClonesArray*)io->GetObject("EcalClusters");
  if (!fClusters)
  {
    Fatal("Init", "Can't find EcalClusters");
    return kFATAL;
  }
  fCal=(CbmEcalCalibration*)io->GetObject("EcalCalibration");
  if (!fCal)
  {
    Fatal("Init", "Can't find EcalCalibration");
    return kFATAL;
  }

  if (fUsePosLib==0)
  {
    fLib=(CbmEcalSCurveLib*)io->GetObject("EcalSCurveLib");
    if (!fLib)
    {
      Fatal("Init", "Can't find EcalSCurveLib");
      return kFATAL;
    }
    fPosLib=NULL;
  }
  else
  {
    fPosLib=(CbmEcalPosLib*)io->GetObject("EcalPosition");
    if (!fPosLib)
    {
      Fatal("Init", "Can't find EcalSCurveLib");
      return kFATAL;
    }
    fLib=NULL;
  }

  fShLib=(CbmEcalShLibCorr*)io->GetObject("EcalShLib");
  if (!fShLib)
  {
    Fatal("Init", "No shower library found in system. Will continue without chi2 calculation.");
    return kFATAL;
  }
  fReco=new TClonesArray("CbmEcalRecParticle", 2000);
  io->Register(fRecoName, "ECAL", fReco, kTRUE);

  fFCN=new FCNEcalClusterCorr(fCal, fShLib, fInf);
  fFCN->SetEStep(fEStep);
  fFCN->SetCStep(fCStep);
  fFCN->SetFixClusterEnergy(fFixClusterEnergy);
  fFCN->SetStructure(fStr);
  fFitter=new ROOT::Minuit2::Minuit2Minimizer();
  fFitter->SetPrintLevel(-1);
  fFitter->SetMaxIterations(fMaxIterations);

  return kSUCCESS;
}

ClassImp(CbmEcalRecoCorr)
