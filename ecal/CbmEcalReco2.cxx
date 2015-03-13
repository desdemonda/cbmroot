#include "CbmEcalReco2.h"

#include "TTree.h"
#include "TClonesArray.h"
#include "TFormula.h"
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
#include "CbmEcalShLib.h"
#include "CbmEcalCalibration.h"
#include "CbmEcalParam.h"
#include "CbmEcalPosLib.h"
#include "CbmEcalMaximum.h"

#include <iostream>

using namespace std;

void FCNEcalCluster2::SetCluster(CbmEcalCluster* cluster)
{
  CbmEcalMaximum* max;
  Int_t i;
  Double_t eraw=0;
  fCluster=cluster;
  SetN(fCluster->Maxs());
  fClusterResp=fCluster->Energy();

  fClusterEnergy=0;
  for(i=0;i<cluster->Maxs();i++)
  {
    max=cluster->Maximum(i);
    eraw+=cluster->PreEnergy(i);
    fClusterEnergy+=fCal->GetEnergy(cluster->PreEnergy(i), max->TanTheta(), max->Region());
  }
  fClusterEnergy*=fClusterResp/eraw;
}

void FCNEcalCluster2::SetN(Int_t n)
{
//  Int_t i;
  fN=n;
  fNDF=fCluster->Size()-(n*3-fFixClusterEnergy);
}

// TODO: more careful estimation of cluster parameters is needed 
Double_t FCNEcalCluster2::DoEval(const Double_t* par) const
{
//  if (par.size()!=fN*3)
//    Fatal("()", "Size of input vector is wrong (%d, should be %d)!", par.size(), fN*3);

  CbmEcalCell* cell;
  CbmEcalMaximum* max=fCluster->Maximum(0);
  static Double_t module=fInf->GetModuleSize();
  Int_t type;
  Double_t de;
  Double_t x;
  Double_t y;
  Double_t theta;
  Double_t phi;
  Double_t cellsize;
  Double_t cx;
  Double_t cy;
  Double_t r;
  Double_t celle;
  CbmEcalCell* cl;
 // Int_t p;
  Int_t i;
  Int_t k;
 // CbmEcalRecParticle* part;
 // Double_t clustere=fClusterResp;
  Double_t clenergy;
 // Double_t sigma2;
  Double_t cellerr;
  Double_t epred;
 // Double_t sin4theta;
  Double_t chi2=0;
  Double_t e[6];
  Double_t t;
  Double_t emeas;
  Int_t region=max->Region();


  clenergy=fClusterEnergy;

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
 
  for(k=0;k<fCluster->Size();k++)
  {
    cell=fStr->GetHitCell(fCluster->CellNum(k));
    max=fCluster->Maximum(k);
    type=cell->GetType();
    cellsize=module/type;
    cellerr=0; celle=0;
    for(i=0;i<fN;i++)
    {
      cl=fCells[i]; cx=cl->GetCenterX(); cy=cl->GetCenterY(); 
      x=cell->GetCenterX(); x-=par[3*i+1-fFixClusterEnergy]; //part->X(); // x-=cx; 
      y=cell->GetCenterY(); y-=par[3*i+2-fFixClusterEnergy]; //part->Y(); // y-=cy; 

      r=TMath::Sqrt(cx*cx+cy*cy);

      /** TODO: should be Z of the cell**/
      theta=TMath::ATan(r/fInf->GetZPos());
      theta*=TMath::RadToDeg();
      phi=TMath::ACos(cx/r)*TMath::RadToDeg();
      if (cy<0) phi=360.0-phi;

      celle+=fShLib->GetResponse(x, y, phi, theta, e[i], type);
//      celle+=fShLib->GetSumEThetaPhi(x, y, cellsize, part->E(), theta, phi);
    }
    cx=cell->GetCenterX(); cy=cell->GetCenterY();
    r=TMath::Sqrt(cx*cx+cy*cy);
    theta=TMath::ATan(r/fInf->GetZPos());

    epred=celle*fClusterEnergy/fClusterResp;
    emeas=cell->GetTotalEnergy()*fClusterEnergy/fClusterResp;
//      cout << fClusterEnergy << " --- " << epred << " : " << emeas << endl;
    {
      fSigma[region]->SetParameter(0, clenergy);	//Ecluster
      fSigma[region]->SetParameter(1, emeas);		//Emeas
      fSigma[region]->SetParameter(2, epred);		//Epred
      fSigma[region]->SetParameter(3, theta);		//Theta
      cellerr=fSigma[region]->Eval(0);
      de=epred-emeas;
      de*=de; chi2+=de/cellerr;
    }
  }
//  cout << endl;
//  chi2/=fCluster->Size();
  return chi2;
}

/***************************************************************************/

void CbmEcalReco2::Exec(Option_t* option)
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
    for(j=0;j<cluster->Maxs();j++)
    {
      cell=fStr->GetHitCell(cluster->CellNum(j));
      fPreE=cluster->PreEnergy(j);
      fCurrentMaximum=cluster->Maximum(j);
      if (fCal->GetEnergy(cell->GetTotalEnergy(), fCurrentMaximum->TanTheta(), fCurrentMaximum->Region())>fMinMaxE)
        Reco(fCurrentMaximum, cluster);
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
void CbmEcalReco2::WriteClusterInfo(CbmEcalCluster* clstr)
{
  CbmEcalMaximum* max=clstr->Maximum(0);
  list<CbmEcalCell*>::const_iterator p;  
  CbmEcalCell* cell;
  CbmEcalRecParticle* part;
  Int_t i=0;
  Int_t j;
  Int_t k;
  Double_t celle;
  Double_t cellsize;
  Double_t e[3];
  Double_t x;
  Double_t y;
  Double_t r;
  Double_t theta;
  Double_t phi;
  CbmEcalCell* cl;
  Double_t cx;
  Double_t cy;
  Double_t de;
  Double_t epred;
  Double_t emeas;
  Double_t clenergy;
  Double_t cellerr;
  static Double_t module=fInf->GetModuleSize();

  fECluster=fCal->GetEnergy(clstr->Energy(), max->TanTheta(), max->Region());
  clenergy=fECluster;
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
    max=clstr->Maximum(k);
    fTypes[i]=cell->GetType();
    cellsize=module/fTypes[i];
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
    de=fCal->GetEnergy(celle, max->TanTheta(), max->Region())-fCal->GetEnergy(cell->GetTotalEnergy(), max->TanTheta(), max->Region());
    cx=cell->GetCenterX(); cy=cell->GetCenterY();
    r=TMath::Sqrt(cx*cx+cy*cy);
    theta=TMath::ATan(r/fInf->GetZPos());

    epred=fCal->GetEnergy(celle, max->TanTheta(), max->Region());
    emeas=fCal->GetEnergy(cell->GetTotalEnergy(), max->TanTheta(), max->Region());
    fSigma[fTypes[i]]->SetParameter(0, fECluster);		//Ecluster
    fSigma[fTypes[i]]->SetParameter(1, emeas);		//Emeas
    fSigma[fTypes[i]]->SetParameter(2, epred);		//Epred
    fSigma[fTypes[i]]->SetParameter(3, theta);		//Theta
    cellerr=fSigma[fTypes[i]]->Eval(0);
    de*=de; fChi2Fit[i]=de/cellerr;

    cx=cell->GetCenterX(); cy=cell->GetCenterY();
    r=TMath::Sqrt(cx*cx+cy*cy);
    fTheta[i]=TMath::ATan(r/fInf->GetZPos());
    fEpred[i]=fCal->GetEnergy(celle, max->TanTheta(), max->Region());
    fEmeas[i]=fCal->GetEnergy(cell->GetTotalEnergy(), max->TanTheta(), max->Region());
    fCX[i]=cell->GetCenterX();
    fCY[i]=cell->GetCenterY();
    i++;
  }
  for(j=i;j<50;j++)
    fTypes[j]=-1111;
}

void CbmEcalReco2::FillTree()
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
void CbmEcalReco2::FitCluster(CbmEcalCluster* clstr)
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
  /// The cluster it self  doesn't care about amplitude of maximums 
  fFCN->SetN(n);

  Int_t params=0;

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
      fFitter->SetVariable(i*3+1 ,par.Data(), p->X(), 0.5);
      fFitter->SetVariableLimits(i*3+1, p->X()-6., p->X()+6.);
      params++;
      par="y"; par+=i; 
      fFitter->SetVariable(i*3+2 ,par.Data(), p->Y(), 0.5);
      fFitter->SetVariableLimits(i*3+2, p->Y()-6., p->Y()+6.);
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
      fFitter->SetVariable(i*3 ,par.Data(), p->X(), 2.5);
      fFitter->SetVariableLimits(i*3,p->X()-6., p->X()+6.);
      params++;
      par="y"; par+=i; 
      fFitter->SetVariable(i*3+1 ,par.Data(), p->Y(), 2.5);
      fFitter->SetVariableLimits(i*3+1, p->Y()-6., p->Y()+6.);
      params++;
      if (i!=n-1) {
        par="n";
	par+=i;
	fFitter->SetVariable(i*3+2 ,par.Data(), p->E()/e, 0.5);
	fFitter->SetVariableLimits(i*3+2, 0.0, 1.0);
        params++;
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
    TimeReco(p, clstr);
  }
  
}

void CbmEcalReco2::TimeReco(CbmEcalRecParticle* p, CbmEcalCluster* cluster)
{
  Double_t time=0;
  Double_t te=0;

  static Double_t module=fInf->GetModuleSize();
  Double_t theta;
  Int_t k;
  CbmEcalCell* cell;
  Int_t type;
  Double_t cellsize;
  Double_t celle;
  Double_t celltime;
  Double_t x;
  Double_t y;
  Double_t r;
  Double_t cx;
  Double_t cy;
  Double_t phi;
  CbmEcalCell* cl;
  
  cl=fStr->GetHitCell(p->CellNum());
  cx=cl->GetCenterX(); cy=cl->GetCenterY(); 
  r=TMath::Sqrt(cx*cx+cy*cy);
  /** TODO: should be Z of the cell**/
  theta=TMath::ATan(r/fInf->GetZPos());
  theta*=TMath::RadToDeg();
  phi=TMath::ACos(cx/r)*TMath::RadToDeg();
  if (cy<0) phi=360.0-phi;

  for(k=0;k<cluster->Size();k++)
  {
    cell=fStr->GetHitCell(cluster->CellNum(k));
    celltime=cell->GetTime();
    if (celltime==-1111) continue;
    /** No time information for the cell **/
    type=cell->GetType();
    cellsize=module/type;
    x=cell->GetCenterX(); x-=p->X(); // x-=cx; 
    y=cell->GetCenterY(); y-=p->Y(); // y-=cy; 

    celle=fShLib->GetResponse(x, y, phi, theta, p->E(), type);
//    cout << "CellTime: "<< celltime << "	" << celle << "	" << cell->GetTotalEnergy() << "	" << cell->GetEnergy() << "	" << cell->GetCenterX() << "	" << cell->GetCenterY() << endl;
    time+=celle*celltime;
    te+=celle;
  }
 
//  cout << "CellTime: " << time << endl;
  time/=te;
//  cout << "CellTime: " << time << endl;
  if (time>0)
    p->SetTOF(time);
}

/** Calculate a chi2 for just reconstructed photons **/
Double_t CbmEcalReco2::CalculateChi2(CbmEcalCluster* cluster)
{
  if (!fShLib) return -1111;
 
  CbmEcalMaximum* max=cluster->Maximum(0);
  CbmEcalCell* cell;
  static Double_t module=fInf->GetModuleSize();
  Int_t type;
  Double_t de;
  Double_t x;
  Double_t y;
  Double_t theta;
  Double_t phi;
  Double_t cellsize;
  Double_t cx;
  Double_t cy;
  Double_t r;
  Double_t celle;
  CbmEcalCell* cl;
  Int_t p;
  Int_t i;
  Int_t k;
  CbmEcalRecParticle* part;
  Double_t clustere=0;
  Double_t clenergy;
 // Double_t sigma2;
  Double_t cellerr;
  Double_t epred;
 // Double_t sin4theta;
  Double_t chi2=0;
  Double_t emeas;
  Int_t region=max->Region();

  clenergy=0;
  for(i=0;i<cluster->Maxs();i++)
  {
    max=cluster->Maximum(i);
    clustere+=cluster->PreEnergy(i);
    clenergy+=fCal->GetEnergy(cluster->PreEnergy(i), max->TanTheta(), max->Region());
  }
  clenergy*=cluster->Energy()/clustere;
  clustere=cluster->Energy();

  i=0;
  for(k=0;k<cluster->Size();k++)
  {
    cell=fStr->GetHitCell(cluster->CellNum(k));
    max=cluster->Maximum(k);
    type=cell->GetType();
    cellsize=module/type;
    cellerr=0; celle=0;
    for(p=fNOld;p<fN;p++)
    {
      part=(CbmEcalRecParticle*)fReco->At(p);
      cl=fStr->GetHitCell(part->CellNum()); cx=cl->GetCenterX(); cy=cl->GetCenterY(); 
      x=cell->GetCenterX(); x-=part->X(); // x-=cx; 
      y=cell->GetCenterY(); y-=part->Y(); // y-=cy; 

      r=TMath::Sqrt(cx*cx+cy*cy);

      /** TODO: should be Z of the cell**/
      theta=TMath::ATan(r/fInf->GetZPos());
      theta*=TMath::RadToDeg();
      phi=TMath::ACos(cx/r)*TMath::RadToDeg();
      if (cy<0) phi=360.0-phi;
//      cout << "Cell(" << cx << ", " << cy << ")" << endl; 
      celle+=fShLib->GetResponse(x, y, phi, theta, part->E(), type);
//      cout << "Shlib " << x << " " << y << " " << phi << " " << theta << " " << part->E() << " " << type << " : " << celle << endl;
    }
//    cout << "---> " << x << "	" << y << "	" << cellsize << "	" << part->E() << "	" << theta << "	" << phi << endl;
//    cout << "-> celle " << celle << endl;
    cx=cell->GetCenterX(); cy=cell->GetCenterY();
    r=TMath::Sqrt(cx*cx+cy*cy);
    theta=TMath::ATan(r/fInf->GetZPos());

    epred=celle*clenergy/clustere;
    emeas=cell->GetTotalEnergy()*clenergy/clustere;
    fEfirst[i]=epred;
//    if (epred>clenergy) epred=clenergy;
//    if (celle>0)
    {
      fSigma[region]->SetParameter(0, clenergy);	//Ecluster
      fSigma[region]->SetParameter(1, emeas);		//Emeas
      fSigma[region]->SetParameter(2, epred);		//Epred
      fSigma[region]->SetParameter(3, theta);		//Theta
      cellerr=fSigma[region]->Eval(0);
      de=epred-emeas;
//      cout << "-> celle=" << celle << "	, epred(" << celle << ")=" << epred << ", emeas(" <<cell->GetTotalEnergy() << ")=" << emeas << endl;
//      cellerr=1;
      de*=de; chi2+=de/cellerr;
      fChi2First[i]=de/cellerr;
    }
    i++;
  }
  if (cluster->Size()-((fN-fNOld)*3-fFixClusterEnergy)<=0)
  {
    Info("CalculateChi2","NDF is %d.", cluster->Size()-((fN-fNOld)*3-fFixClusterEnergy));
    chi2=-chi2;
  }
  else
    chi2/=cluster->Size()-((fN-fNOld)*3-fFixClusterEnergy);
  for(p=fNOld;p<fN;p++)
  {
    part=(CbmEcalRecParticle*)fReco->At(p);
    part->SetChi2(chi2);
  }
  cluster->fChi2=chi2;
//  cout << fNOld << "	" << fN << "	" << chi2 << endl;
  return chi2;
}

/** Get first approximation of photon parameters from maximum **/
void CbmEcalReco2::Reco(CbmEcalMaximum* max, CbmEcalCluster* clstr)
{
  if (fLib==NULL)
  {
    Reco2(max, clstr);
    return;
  }
  Double_t x;
  Double_t y;
  Double_t amp;
  Double_t en;
  Double_t maxe;
  Double_t px;
  Double_t py;
  Double_t pz;
  Double_t z=fInf->GetZPos();
  CbmEcalCell* cell=max->Cell();
  Int_t type;
  list<CbmEcalCell*> cells;
  list<CbmEcalCell*>::const_iterator p;
  Int_t cnum;

  cells.clear();

  fType=cell->GetType();
  fDiffType=0;

  cell->GetNeighborsList(0,cells);
  fE=cell->GetEnergy();
  en=cell->GetEnergy();
  type=max->Region();
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
  cell->GetNeighborsList(max->I(),cells);
  en=cell->GetEnergy();
  for(p=cells.begin();p!=cells.end();++p) 
    en+=(*p)->GetEnergy();
  cells.push_back(cell);
  fE2x2=en;

  fCellX=cell->GetCenterX();
  fCellY=cell->GetCenterY();

  if (fCal->GetEnergy(fE2x2, max->TanTheta(), max->Region())<0.1) return; 
  fEReco=fCal->GetEnergy(fE2x2, max->TanTheta(), max->Region());
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
  amp=TMath::Sqrt(x*x+y*y+z*z);
  px=fEReco*x/amp;
  py=fEReco*y/amp;
  pz=fEReco*z/amp;
  new((*fReco)[fN++]) CbmEcalRecParticle(px, py, pz, fEReco, x, y, z, 22, -1111, clstr, cell, fType);
}

/** Get first approximation of photon parameters from maximum
 ** This version uses PosLib instead of S-curves
 ** TODO Architecture: Need a maximum, instead of cell here **/
void CbmEcalReco2::Reco2(CbmEcalMaximum* max, CbmEcalCluster* clstr)
{
  if (fPosLib==NULL)
    Fatal("Reco2", "No position library for reconstruction.");

  CbmEcalCell* cell=max->Cell();
  list<CbmEcalCell*> cells;
  list<CbmEcalCell*>::const_iterator p;
  Double_t px;
  Double_t py;
  Double_t pz;
  Double_t amp;
  Double_t x=max->X();
  Double_t y=max->Y();
  Double_t z;
  Double_t t;
  
  z=fInf->GetZPos();
  t=TMath::Sqrt(x*x+y*y);
  fEReco=fCal->GetEnergy(fPreE, t/z, max->Region());
  fPosLib->GetXY(fEReco, x, y);
  //One more iteration for better precesion
  x=fPosLib->X(); y=fPosLib->Y();
  fPosLib->GetXY(fEReco, x, y);
  x=fPosLib->X(); y=fPosLib->Y();
  t=TMath::Sqrt(x*x+y*y);
  fEReco=fCal->GetEnergy(fPreE, t/z, max->Region());
  amp=TMath::Sqrt(t+z*z);
  px=fEReco*x/amp;
  py=fEReco*y/amp;
  pz=fEReco*z/amp;
  new((*fReco)[fN++]) CbmEcalRecParticle(px, py, pz, fEReco, x, y, z, 22, -1111, clstr, cell, fType);

}
void CbmEcalReco2::CreateTree()
{
  fOutTree=new TTree("slow_reco", "Reconstruction by fitting with shower library");
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
CbmEcalReco2::CbmEcalReco2() 
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
    fC(),
    fSigma(),
    fParNames(),
    fSigmaFormula(),
    fParOffset(0),
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
    fCurrentMaximum(NULL)
{
}

/** Standard constructor **/
CbmEcalReco2::CbmEcalReco2(const char *name, const Int_t iVerbose, const char* configname)
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
    fC(),
    fSigma(),
    fParNames(),
    fSigmaFormula(),
    fParOffset(0),
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
    fCurrentMaximum(NULL)
{
  Int_t i;
  Int_t j;
  Int_t types;
  Int_t consts;
  TString nm;

  fParNames[0]="Ecluster";
  fParNames[1]="Emeas";
  fParNames[2]="Epred";
  fParNames[3]="Theta";
  fParOffset=3;
  for(i=0;i<10;i++)
  {
    fParNames[i+fParOffset]="c";
    fParNames[i+fParOffset]+=i;
  }

  CbmEcalParam* par=new CbmEcalParam("RecoSlowParam", configname);

  for(i=0;i<10;i++)
    fC[i]=-1111;
  types=par->GetInteger("types");
  consts=par->GetInteger("consts");
  for(j=0;j<types;j++)
  {
    for(i=0;i<consts;i++)
    {
      nm=fParNames[i+fParOffset]; nm+="_"; nm+=j;
      fC[i]=par->GetDouble(nm);
    }
    nm="sigma_"; nm+=j;
    fSigmaFormula[j]=par->GetString(nm);
    for(i=0;i<consts;i++)
    {
      nm=fParNames[i+fParOffset]; nm+="_"; nm+=j;
      fSigmaFormula[j].ReplaceAll(fParNames[i+fParOffset], par->GetString(nm));
    }

    Info("Constructor", "Formula for sigma_%d (with constants): %s. ", j, fSigmaFormula[j].Data());
    for(i=0;i<fParOffset;i++)
    {
      nm="["; nm+=i; nm+="]";
      fSigmaFormula[j].ReplaceAll(fParNames[i], nm);
    }
    Info("Constructor", "Used formula: %s.", fSigmaFormula[j].Data());
    nm="RecoSlowFormula_"; nm+=j;
    fSigma[j]=new TFormula("RecoSimpleFormula", fSigmaFormula[j].Data());
  }
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

CbmEcalReco2::~CbmEcalReco2()
{
  fReco->Delete();
  delete fReco;
}

void CbmEcalReco2::Finish()
{
  if (fOutTree) fOutTree->Write();
}

/** Init **/
InitStatus CbmEcalReco2::Init()
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

  fShLib=(CbmEcalShLib*)io->GetObject("EcalShLib");
  if (!fShLib)
  {
    Fatal("Init", "No shower library found in system. Will continue without chi2 calculation.");
    return kFATAL;
  }
  fReco=new TClonesArray("CbmEcalRecParticle", 2000);
  io->Register(fRecoName, "ECAL", fReco, kTRUE);

  fFCN=new FCNEcalCluster2(fCal, fShLib, fSigma, fInf);
  fFCN->SetEStep(fEStep);
  fFCN->SetCStep(fCStep);
  fFCN->SetFixClusterEnergy(fFixClusterEnergy);
  fFCN->SetStructure(fStr);
  fFitter=new ROOT::Minuit2::Minuit2Minimizer();
  fFitter->SetPrintLevel(-1);
  fFitter->SetMaxIterations(fMaxIterations);

  return kSUCCESS;
}

ClassImp(CbmEcalReco2)
