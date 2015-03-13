#include "CbmEcalIdentification.h"

#include "FairRootManager.h"
#include "FairTrackParam.h"

#include "CbmEcalStructure.h"
#include "CbmEcalCalibration.h"
#include "CbmEcalIdParticle.h"
#include "CbmEcalTrackExtrapolation.h"
#include "CbmEcalCell.h"
#include "CbmEcalCellMC.h"
#include "CbmEcalParam.h"
#include "CbmMCTrack.h"
#include "CbmEcalInf.h"
#include "CbmEcalPoint.h"
#include "CbmEcalMaximum.h"
#include "CbmEcalShLibCorr.h"

#include "CbmGlobalTrack.h"
#include "CbmStsTrack.h"
#include "CbmTrackMatch.h"

#include "TTree.h"
#include "TClonesArray.h"
#include "TMath.h"
#include "TFormula.h"
#include "TVector2.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TMatrixD.h"

#include <iostream>
#include <map>

using namespace std;

/** Loop procedure **/
void CbmEcalIdentification::Exec(Option_t* option)
{
  fEvent++;
  if (fTreeOut) InitTree();
  fId->Clear();

  Int_t n=fTracks->GetEntriesFast();
  Int_t i;
  Int_t oldn;
  fN=0;
  FairTrackParam* tr;
  CbmGlobalTrack* gtr;
  if (fVerbose>9)
    Info("Exec", "Total %d tracks before the calorimeter", n);
  for(i=0;i<n;i++)
  {
    if (fTreeOut) InitVar();
    tr=(FairTrackParam*)fTracks->At(i);
    if (tr==NULL) continue;
    fExtraNum=i;
    fTrackNum=i;
    if (fExtra)
    {
      fTrackNum=fExtra->Map()[fExtraNum];
      gtr=(CbmGlobalTrack*)fGlobal->At(fTrackNum);
      fStsTrackNum=gtr->GetStsTrackIndex();
    }
    else
      fStsTrackNum=i;
    oldn=fN;
    Identify(tr);
    if (oldn!=fN&&fTreeOut)
      WriteTree();
  }
//  gDirectory->Cd("../");
//  gDirectory->ls();
//  cout << gDirectory->GetPath() << endl;
//  if (fTreeOut) fTree->Write();
}

/** Identify a track **/
void CbmEcalIdentification::Identify(FairTrackParam* tr)
{
  CbmEcalCell* cell=NULL;
  CbmEcalCell* cell2;
  list<CbmEcalCell*> cells;
  list<CbmEcalCell*>::const_iterator p;
  list<CbmEcalCell*> ocells;
  list<CbmEcalCell*>::const_iterator op;
  Double_t e;
  Double_t tracke;
  Float_t x;
  Float_t y;
  Float_t dst;
  Float_t t;
  CbmEcalInf* fInf=fStr->GetEcalInf();
  TVector3 trackmom;
  TVector3 tv3;
  Double_t te;
  Double_t me;
  Double_t me1;
  Double_t sigma;
  Int_t i;
  Int_t mc;
  Int_t nm=fMaximums->GetEntriesFast();
  CbmEcalMaximum* mx;
  CbmEcalMaximum* mxx=NULL;


  tr->Momentum(trackmom);
  fTrackP=trackmom.Mag();
  fPhi=TMath::ATan2(trackmom.Y(), trackmom.X())*TMath::RadToDeg();
  fTheta=TMath::ATan2(trackmom.Pt(), trackmom.Z())*TMath::RadToDeg();
  fX=tr->GetX();
  fY=tr->GetY();
  fZ=tr->GetZ();
//  Info("Identify", "Track %d (%f, %f).", fExtraNum, fX, fY);

  cell=fStr->GetCell(fX, fY);
  if (cell)
    fCellE=GetEnergy(cell);
  tv3=trackmom.Unit();
  x=fX+tv3.X()*2.0;
  y=fY+tv3.Y()*2.0;
  cell=fStr->GetCell(x, y);
  if (cell)
    fCellE2=GetEnergy(cell);
  cell=NULL;

  if (fSimpleMaximumLocationAlgo==1)
    mxx=FindMaximum(tr);
  else
  {
    for(i=0;i<nm;i++)
    {
//    cout << fZ << " -----------------------------------------------" << endl;
      mx=(CbmEcalMaximum*)fMaximums->At(i);
      if (!mx) continue;
      if (CheckMaximum(mx))
      {
        if (cell&&fVerbose>10)
	  Info("Identify", "For track %d (%f, %f) more than one maximum located", fTrackNum, fX, fY);
        cell=mx->Cell();
        mx->SetMark(mx->Mark()+1);
        mxx=mx;
      }
    }
  }
  if (mxx==NULL)
  {
    if (fTreeOut) WriteTreeLight();
    return;
  }
  cell=mxx->Cell();
//  cout << "here" << endl;
  ConstructCluster(mxx);
  if (fCluster.size()==0)
  {
    if (fTreeOut) WriteTreeLight();
    return;
  }
  if (fTreeOut) ShapeAnalysis(mxx);
  CalculateShape(mxx);
  if (fShLib) CalculateChi2(mxx);
  e=0;
  for(p=fCluster.begin();p!=fCluster.end();++p)
    e+=GetEnergy(*p);
//  fShape=-1111
  fE=fCal->GetEnergy(e, TMath::Tan(TMath::DegToRad()*fTheta), fRegion);
//  cout << fE << " " << fRegion << endl;
//  cout << e << "->" << fE << ", theta=" << fTheta << ", region=" << fRegion << endl;
  fMaxX=cell->GetCenterX();
  fMaxY=cell->GetCenterY();
  //TODO: Calculate probability
  fEProb=1;
  tracke=trackmom.Mag();
  fTrackP=tracke;
//  if (fE<tracke)
  {
    sigma=fEResolution->Eval(tracke)*tracke;
    fEProb*=TMath::Erfc((tracke-fE)/sigma/TMath::Sqrt(2.0));
  }

  fMCTrackNum=-1111;
  if (fUseMC==1)
  {
    CbmTrackMatch* ststr=(CbmTrackMatch*)fStsTracksMatch->At(fStsTrackNum);
    if (ststr)
      fMCTrackNum=ststr->GetMCTrackId();
  }
  // Use PSE for \chi^2 storage
  new((*fId)[fN++]) CbmEcalIdParticle(cell, fX, fY, fZ, fE/tracke, fChi2, fShape, fTrackNum, fEProb, fMCTrackNum);
}

Double_t CbmEcalIdentification::Correlation(CbmEcalCell* c1, CbmEcalCell* c2, Double_t x, Double_t y, Double_t phi, Double_t theta, Double_t e) const
{
  if (c1->GetType()==c2->GetType())
    return fShLib->Correlation(c1->X1()-x, c1->Y1()-y, c2->X1()-x, c2->Y1()-y, phi, theta, e, c1);
  if (c1->GetType()>c2->GetType())
    return fShLib->Correlation2(c1->X1()-x, c1->Y1()-y, c2->X1()-x, c2->Y1()-y, phi, theta, e, c1->GetType());
  else
    return fShLib->Correlation2(c2->X1()-x, c2->Y1()-y, c1->X1()-x, c1->Y1()-y, phi, theta, e, c2->GetType());
}

/** Calculate chi2 of the cluster**/
void CbmEcalIdentification::CalculateChi2(CbmEcalMaximum* mx)
{
  // fX, fY --- corrdinates
  // fTheta, fPhi --- angles
  // fTrackP --- momenta of the track
  // fCluster --- list of cluster cell
  Int_t s=fCluster.size();
  TMatrixD m(s, s);
  Int_t i=0;
  Int_t j;
  list<CbmEcalCell*>::const_iterator p;
  CbmEcalCell* cls[10];
  Double_t de[10];
  Double_t det[10];

  for(p=fCluster.begin();p!=fCluster.end();++p)
    cls[i++]=(*p);

  if (fPhi<0) fPhi+=360.0;
//  cout << fPhi << " " << fTheta << " " << fTrackP << endl;
  // Calculate covariance matrix
  for(i=0;i<s;i++)
  for(j=0;j<s;j++)
    m[i][j]=Correlation(cls[i], cls[j], fX, fY, fPhi, fTheta, fTrackP);

  //Some magic numbers: TODO?
  for(i=0;i<s;i++)
    if (m[i][i]<1e-8*fTrackP) m[i][i]=1e-8*fTrackP;

  // Inversion
  m.Invert(det);

  // Energy difference calculation
  for(i=0;i<s;i++)
    de[i]=cls[i]->GetTotalEnergy()-fShLib->GetResponse(cls[i]->X1()-fX, cls[i]->Y1()-fY, fPhi, fTheta, fTrackP, cls[i]);

  // Chi2 calculation
  fChi2=0.0;
  for(i=0;i<s;i++)
  for(j=0;j<s;j++)
    fChi2+=de[i]*de[j]*m[i][j];
}

/** Simple maximum location algorithm. Copy/paste from CbmEcalIdentification **/
CbmEcalMaximum* CbmEcalIdentification::FindMaximum(FairTrackParam* tr)
{
  CbmEcalCell* cell=fStr->GetCell(fX, fY);
  if (cell==NULL) return NULL;
  Double_t e=cell->GetTotalEnergy();
  list<CbmEcalCell*> cells;
  list<CbmEcalCell*> ocells;
  list<CbmEcalCell*>::const_iterator p;
  list<CbmEcalCell*>::const_iterator op;
  CbmEcalCell* max=NULL;
  Double_t x;
  Double_t y;
  Double_t dst;
  Double_t t;
 
 
//  Info("FindMaximum", "E=%f", e);
  cell->GetNeighborsList(0, cells);  
  for(p=cells.begin();p!=cells.end();++p)
    if ((*p)->GetTotalEnergy()>e)
      break;
  if (p==cells.end())
  {
    max=cell;
//    Info("FindMaximum", "Found cell is maximum");
  }
//  cell->GetNeighborsList(0, cells);
//  e=cell->GetTotalEnergy();
  if (max==NULL)
  /** Check cells near given for maximums **/
    for(p=cells.begin();p!=cells.end();++p)
    {
      x=(*p)->GetCenterX()-tr->GetX();
      y=(*p)->GetCenterY()-tr->GetY();
      dst=TMath::Sqrt(x*x+y*y);
//        if (dst>fInf->GetModuleSize()*TMath::Sqrt(2.0)+0.001) continue;
      x=tr->GetX(); x*=x; y=tr->GetY(); y*=y;
      t=TMath::Sqrt(x+y);
      x=(*p)->GetCenterX()-cell->GetCenterX();
      y=(*p)->GetCenterY()-cell->GetCenterY();
//      if (x*tr->GetTx()/t+y*tr->GetTy()/t<TMath::Sqrt(2.0)*fInf->GetModuleSize()/cell->GetType()+0.001)
//      if (x*tr->GetTx()/t+y*tr->GetTy()/t<-0.01&&dst>fInf->GetModuleSize()/cell->GetType()+0.001) continue;
      (*p)->GetNeighborsList(0, ocells);
      e=(*p)->GetTotalEnergy();
      for(op=ocells.begin(); op!=ocells.end();++op)
        if ((*op)->GetTotalEnergy()>e) break;
      if (op==ocells.end())
        if (max==NULL||max->GetTotalEnergy()<(*p)->GetTotalEnergy())
          max=*p;
    }
  if (max==NULL) return NULL;

  Int_t i;
  Int_t nm=fMaximums->GetEntriesFast();
  CbmEcalMaximum* mx;
  for(i=0;i<nm;i++)
  {
    mx=(CbmEcalMaximum*)fMaximums->At(i);
    if (!mx) continue;
    if (mx->Cell()==max) break;
  }
  if (i==nm)
  {
//    Info("FindMaximum", "Maximum (%f,%f,%f) not found in the table", fX, fY, max->GetTotalEnergy());
//    for(i=0;i<nm;i++)
//    {
//      mx=(CbmEcalMaximum*)fMaximums->At(i);
//      if (!mx) continue;
//      Info("FindMaximum", "Table maximum %d (%f, %f, %f) ", i, mx->X(), mx->Y(), mx->Cell()->GetTotalEnergy());
//    }
    return NULL;
  }
  mx->SetMark(mx->Mark()+1);

  return mx;
}
/** If identification failed :-( **/
void CbmEcalIdentification::WriteTreeLight()
{
  fPDG=-1111;
  fMCPDG=-1111;
  fMotherMCPDG=-1111;
  fMCM=-1111;
  if (fUseMC==1)
  {
    CbmTrackMatch* ststr=(CbmTrackMatch*)fStsTracksMatch->At(fStsTrackNum);
    if (ststr)
      fMCTrackNum=ststr->GetMCTrackId();
  }
  if (fUseMC&&fMCTrackNum>=0)
  {
    CbmMCTrack* mctr=(CbmMCTrack*)fMC->At(fMCTrackNum);
    CbmMCTrack* mother;
    if (mctr->GetMotherId()>=0)
    {
      mother=(CbmMCTrack*)fMC->At(mctr->GetMotherId());
      fMotherMCPDG=mother->GetPdgCode();
    }
    fMCPDG=mctr->GetPdgCode();
    fMCP=mctr->GetP();

    CbmEcalPoint* pt=NULL;
    CbmMCTrack* tr;
    CbmMCTrack* mtr;
    Int_t n=fMCPoints->GetEntriesFast();
    Int_t i=0;
    Int_t j;
    Double_t x1;
    Double_t y1;
    Double_t dx;
    Double_t dy;
    Double_t r;
    TVector3 mom;

    for(i=0;i<n;i++)
    {
      pt=(CbmEcalPoint*)fMCPoints->At(i);
      if (pt->GetTrackID()==fMCTrackNum)
	break;
    }
    if (i!=n)
    {
      fMCM=0;
      fMCX=pt->GetX();
      fMCY=pt->GetY();
      dx=pt->GetX()-fX;
      dy=pt->GetY()-fY;
      x1=pt->GetX();
      y1=pt->GetY();
      fR=TMath::Sqrt(dx*dx+dy*dy);
      pt->Momentum(mom);
      fMCSurfE=mom.Mag();
      fMCCircE=fMCSurfE;
      for(j=0;j<n;j++)
      if (j!=i)
      {
	pt=(CbmEcalPoint*)fMCPoints->At(j);
	if (pt->GetTrackID()>=0)
	  tr=(CbmMCTrack*)fMC->At(pt->GetTrackID());
	else
	  tr=NULL;
	while(tr!=NULL)
	{
	  if (tr->GetMotherId()<0)
	  {
	    tr=NULL; break;
	  }
	  if (tr->GetMotherId()==fMCTrackNum)
	    break;
	  tr=(CbmMCTrack*)fMC->At(tr->GetMotherId());
	}
	if (tr)
        {
          pt->Momentum(mom);
	  dx=x1-pt->GetX();
	  dy=y1-pt->GetY();
	  r=TMath::Sqrt(dx*dx+dy*dy);
	  fMCM+=r*mom.Mag();
          if (r<fCircRad)
  	    fMCCircE+=mom.Mag();
	}
      }
    }
    else
    {
      fR=-1111;
      fMCSurfE=-1111;
    }
  }
  fTree->Fill();
}
/** Write information to the tree **/
void CbmEcalIdentification::WriteTree()
{
  fPDG=-1111;
  fMCPDG=-1111;
  fMotherMCPDG=-1111;
  fMCM=-1111;
  if (fStsTrackNum>=0)
  {
    CbmStsTrack* gtr=(CbmStsTrack*)fStsTracks->At(fStsTrackNum);
    fPDG=gtr->GetPidHypo();
    fTrChi2=gtr->GetChiSq();
    fTrNDF=gtr->GetNDF();
  }
  else
  {
    fPDG=-1111;
    fTrChi2=-1111;
  }
  if (fMCTrackNum>=0&&fUseMC==1)
  {
    CbmMCTrack* mctr=(CbmMCTrack*)fMC->At(fMCTrackNum);
    CbmMCTrack* mother;
    if (mctr->GetMotherId()>=0)
    {
      mother=(CbmMCTrack*)fMC->At(mctr->GetMotherId());
      fMotherMCPDG=mother->GetPdgCode();
    }
    fMCPDG=mctr->GetPdgCode();
    fMCP=mctr->GetP();

    CbmEcalPoint* pt=NULL;
    CbmMCTrack* tr;
    CbmMCTrack* mtr;
    Int_t n=fMCPoints->GetEntriesFast();
    Int_t i=0;
    Int_t j;
    Double_t x1;
    Double_t y1;
    Double_t dx;
    Double_t dy;
    Double_t r;
    TVector3 mom;

    for(i=0;i<n;i++)
    {
      pt=(CbmEcalPoint*)fMCPoints->At(i);
      if (pt->GetTrackID()==fMCTrackNum)
	break;
    }
    if (i!=n)
    {
      fMCM=0;
      fMCX=pt->GetX();
      fMCY=pt->GetY();
      dx=pt->GetX()-fX;
      dy=pt->GetY()-fY;
      x1=pt->GetX();
      y1=pt->GetY();
      fR=TMath::Sqrt(dx*dx+dy*dy);
      pt->Momentum(mom);
      fMCSurfE=mom.Mag();
      fMCCircE=fMCSurfE;
      for(j=0;j<n;j++)
      if (j!=i)
      {
	pt=(CbmEcalPoint*)fMCPoints->At(j);
	if (pt->GetTrackID()>=0)
	  tr=(CbmMCTrack*)fMC->At(pt->GetTrackID());
	else
	  tr=NULL;
	while(tr!=NULL)
	{
	  if (tr->GetMotherId()<0)
	  {
	    tr=NULL; break;
	  }
	  if (tr->GetMotherId()==fMCTrackNum)
	    break;
	  tr=(CbmMCTrack*)fMC->At(tr->GetMotherId());
	}
	if (tr)
        {
          pt->Momentum(mom);
	  dx=x1-pt->GetX();
	  dy=y1-pt->GetY();
	  r=TMath::Sqrt(dx*dx+dy*dy);
	  fMCM+=r*mom.Mag();
          if (r<fCircRad)
  	    fMCCircE+=mom.Mag();
	}
      }
    }
    else
    {
      fR=-1111;
      fMCSurfE=-1111;
    }
  }
  fTree->Fill();
}

/** Get energy of the cell **/
Double_t CbmEcalIdentification::GetEnergy(CbmEcalCell* cell)
{
  if (fUseHack==0)
    return cell->GetEnergy();

  CbmEcalCellMC* cellmc=(CbmEcalCellMC*)cell; //This will fail if no MC info is available
  map<Int_t, Float_t>::const_iterator q;
  Double_t e=0;
  CbmMCTrack* tr;
  Int_t pdg;

  for(q=cellmc->GetTrackEnergyBegin();q!=cellmc->GetTrackEnergyEnd();++q)
  {
    tr=(CbmMCTrack*)fMC->At((*q).first);
    if (tr==NULL)
    {
      Info("GetEnergy","Can't find MC track with number %d.",(*q).first);
      continue;
    }
    pdg=tr->GetPdgCode();
    if (pdg==22||pdg==-11||pdg==11||pdg==13||pdg==-13)
      e+=(*q).second;
    else
      e+=(*q).second*fHack;
  }
  return e;
}

/** Initializes tree **/
void CbmEcalIdentification::InitTree()
{
  if (fTree) return;
  fTree=new TTree("ecalid","Identification in calorimeter");
  fTree->Branch("ev", &fEvent, "ev/I");
  fTree->Branch("cellx", &fCellX, "cellx/D");
  fTree->Branch("celly", &fCellY, "celly/D");
  fTree->Branch("maxx", &fMaxX, "maxx/D");
  fTree->Branch("maxy", &fMaxY, "maxy/D");
  fTree->Branch("mcx", &fMCX, "mcx/D");
  fTree->Branch("mcy", &fMCY, "mcy/D");
  fTree->Branch("x", &fX, "x/D");
  fTree->Branch("y", &fY, "y/D");
  fTree->Branch("z", &fZ, "z/D");
  fTree->Branch("e", &fE, "e/D");
  fTree->Branch("shape", &fShape, "shape/D");
  fTree->Branch("eprob", &fEProb, "eprob/D");
  fTree->Branch("n", &fN, "n/I");
  fTree->Branch("tr", &fTrackNum, "tr/I");
  fTree->Branch("mctr", &fMCTrackNum, "mctr/I");
  fTree->Branch("pdg", &fPDG, "pdg/I");
  fTree->Branch("tcell", &fCellType, "pdg/I");
  fTree->Branch("mcpdg", &fMCPDG, "mcpdg/I");
  fTree->Branch("mmcpdg", &fMotherMCPDG, "mmcpdg/I");
  fTree->Branch("mcp", &fMCP, "mcp/D");
  fTree->Branch("mcm", &fMCM, "mcm/D");
  fTree->Branch("trp", &fTrackP, "trp/D");
  fTree->Branch("r", &fR, "r/D");
  fTree->Branch("mcsurfe", &fMCSurfE, "mcsurfe/D");
  fTree->Branch("mccirce", &fMCCircE, "mccirce/D");
  fTree->Branch("trchi2", &fTrChi2, "trchi2/D");
  fTree->Branch("trcndf", &fTrNDF, "trndf/I");
  fTree->Branch("emax", &fEMax, "emax/D");
  fTree->Branch("e2", &fE2, "e2/D");
  fTree->Branch("e3", &fE3, "e3/D");
  fTree->Branch("ecluster", &fECluster, "ecluster/D");
  fTree->Branch("se", fSE, "se[5]/D");
  fTree->Branch("p2", &fP2, "p2/D");
  fTree->Branch("q2", &fQ2, "q2/D");
  fTree->Branch("theta", &fTheta, "theta/D");
  fTree->Branch("celle", &fCellE, "celle/D");
  fTree->Branch("celle2", &fCellE2, "celle2/D");
  fTree->Branch("p", &fP, "p/D");
  fTree->Branch("q", &fQ, "q/D");
  fTree->Branch("angle", &fAngle, "angle/D");
  fTree->Branch("ph", &fPH, "ph/D");
  fTree->Branch("pl", &fPL, "pl/D");
  fTree->Branch("pc", &fPC, "pc/D");
  fTree->Branch("pr", &fPR, "pr/D");
  fTree->Branch("chi2", &fChi2, "chi2/D");
}

/** Initializes tree **/
void CbmEcalIdentification::InitVar()
{
  fCellX=-1111;
  fCellY=-1111;
  fMaxX=-1111;
  fMaxY=-1111;
  fX=-1111;
  fY=-1111;
  fZ=-1111;
  fE=-1111;
  fShape=-1111;
  fEProb=-1111;
  fTrackNum=-1111;
  fStsTrackNum=-1111;
  fMCTrackNum=-1111;
  fPDG=-1111;
  fCellType=-1111;
  fMCPDG=-1111;
  fMotherMCPDG=-1111;
  fMCP=-1111;
  fMCM=-1111;
  fTrackP=-1111;
  fR=-1111;
  fMCSurfE=-1111;
  fMCCircE=-1111;
  fTrChi2=-1111;
  fTrNDF=-1111;
  fMCX=-1111;
  fMCY=-1111;
  fEMax=-1111;
  fE2=-1111;
  fECluster=-1111;
  for(Int_t i=0;i<5;i++)
    fSE[i]=-1111;
  fP2=-1111;
  fQ2=-1111;
  fCellE=-1111;
  fCellE2=-1111;
  fE3=-1111;
  fP=-1111;
  fQ=-1111;
  fAngle=-1111;
  fPH=-1111;
  fPL=-1111;
  fPC=-1111;
  fPR=-1111;
  fChi2=-1111;
}


CbmEcalIdentification::CbmEcalIdentification()
  : FairTask(),
    fCfgName(""),
    fTree(NULL),
    fX(0.),
    fY(0.),
    fZ(0.),
    fTheta(0.),
    fPhi(0.),
    fCellX(0.),
    fCellY(0.),
    fMaxX(0.),
    fMaxY(0.),
    fMCX(0.),
    fMCY(0.),
    fE(0.),
    fShape(0.),
    fEProb(0.),
    fExtraNum(-1111),
    fTrackNum(-1111),
    fStsTrackNum(-1111),
    fTrackP(0.),
    fMCTrackNum(0),
    fTreeOut(0),
    fUseMC(0),
    fUseHack(0),
    fHack(0.),
    fEvent(0),
    fCellType(0),
    fN(0),
    fTrChi2(0.),
    fTrNDF(0),
    fPDG(0),
    fMCPDG(0),
    fMotherMCPDG(0),
    fMCP(0.),
    fMCM(0.),
    fR(0.),
    fMCSurfE(0.),
    fMCCircE(0.),
    fCircRad(0.),
    fChi2(0.),
    fUseShLib(0),
    fSimpleMaximumLocationAlgo(0),
    fF(NULL),
    fForm(),
    fQL(0.),
    fQH(0.),
    fCPL(0.),
    fCPH(0.),
    fPH(0.),
    fPL(0.),
    fOldShapeAlgo(0),
    fCluster(),
    fRegions(0),
    fRegion(0),
    fThetaB(NULL),
    fAlgo(NULL),
    fPar1(NULL),
    fPar2(NULL),
    fPar3(NULL),
    fParI(NULL),
    fIntSteps(0),
    fUseCellType(0),
    fEMax(0.),
    fE2(0.),
    fE3(0.),
    fECluster(0.),
    fSE(),
    fP2(0.),
    fQ2(0.),
    fCellE(0.),
    fCellE2(0.),
    fAngle(0.),
    fP(0.),
    fQ(0.),
    fPC(0.),
    fPR(0.),
    fQCH(0.),
    fQCL(0.),
    fEResolution(NULL),
    fId(NULL),
    fTracks(NULL),
    fMC(NULL),
    fGlobal(NULL),
    fStsTracks(NULL),
    fStsTracksMatch(NULL),
    fMCPoints(NULL),
    fMaximums(NULL),
    fCal(NULL),
    fStr(NULL),
    fShLib(NULL),
    fExtra(NULL)
{
}


CbmEcalIdentification::CbmEcalIdentification(const char* name, const Int_t iVerbose, const char* config)
  : FairTask(name, iVerbose),
    fCfgName(config),
    fTree(NULL),
    fX(0.),
    fY(0.),
    fZ(0.),
    fTheta(0.),
    fPhi(0.),
    fCellX(0.),
    fCellY(0.),
    fMaxX(0.),
    fMaxY(0.),
    fMCX(0.),
    fMCY(0.),
    fE(0.),
    fShape(0.),
    fEProb(0.),
    fExtraNum(-1111),
    fTrackNum(-1111),
    fStsTrackNum(-1111),
    fTrackP(0.),
    fMCTrackNum(0),
    fTreeOut(0),
    fUseMC(0),
    fUseHack(0),
    fHack(0.),
    fEvent(0),
    fCellType(0),
    fN(0),
    fTrChi2(0.),
    fTrNDF(0),
    fPDG(0),
    fMCPDG(0),
    fMotherMCPDG(0),
    fMCP(0.),
    fMCM(0.),
    fR(0.),
    fMCSurfE(0.),
    fMCCircE(0.),
    fCircRad(0.),
    fChi2(0.),
    fUseShLib(0),
    fSimpleMaximumLocationAlgo(0),
    fF(NULL),
    fForm(),
    fQL(0.),
    fQH(0.),
    fCPL(0.),
    fCPH(0.),
    fPH(0.),
    fPL(0.),
    fOldShapeAlgo(0),
    fCluster(),
    fRegions(0),
    fRegion(0),
    fThetaB(NULL),
    fAlgo(NULL),
    fPar1(NULL),
    fPar2(NULL),
    fPar3(NULL),
    fParI(NULL),
    fIntSteps(0),
    fUseCellType(0),
    fEMax(0.),
    fE2(0.),
    fE3(0.),
    fECluster(0.),
    fSE(),
    fP2(0.),
    fQ2(0.),
    fCellE(0.),
    fCellE2(0.),
    fAngle(0.),
    fP(0.),
    fQ(0.),
    fPC(0.),
    fPR(0.),
    fQCH(0.),
    fQCL(0.),
    fEResolution(NULL),
    fId(NULL),
    fTracks(NULL),
    fMC(NULL),
    fGlobal(NULL),
    fStsTracks(NULL),
    fStsTracksMatch(NULL),
    fMCPoints(NULL),
    fMaximums(NULL),
    fCal(NULL),
    fStr(NULL),
    fShLib(NULL),
    fExtra(NULL)
{
}

/** Initing routine **/
InitStatus CbmEcalIdentification::Init()
{
  Int_t f;
  Int_t r;
  TString nm;
  TString res;

  FairRootManager* fManager=FairRootManager::Instance();
  if (!fManager)
  {
    Fatal("Init", "Can't find a Root Manager.");
    return kFATAL;
  }
  fTracks=(TClonesArray*)fManager->GetObject("EcalTrackParam");
  if (!fTracks)
  {
    Fatal("Init", "Can't find an array of reconstructed tracks.");
    return kFATAL;
  }
  fStr=(CbmEcalStructure*)fManager->GetObject("EcalStructure");
  if (!fStr)
  {
    Fatal("Init()", "Can't find calorimeter structure in the system.");
    return kFATAL;
  }
  fCal=(CbmEcalCalibration*)fManager->GetObject("EcalECalibration");
  if (!fCal)
  {
    Fatal("Init", "Can't find EcalECalibration");
    return kFATAL;
  }
  fExtra=(CbmEcalTrackExtrapolation*)fManager->GetObject("EcalTrackExtrapolation");
  if (!fExtra)
    Info("Init", "Can't find EcalTrackParam. Will use STS tracks.");
  else // Need an array of global tracks
  {
    fGlobal=(TClonesArray*)fManager->GetObject("GlobalTrack");
    if (!fGlobal)
    {
      Fatal("Init()","Can't find global tracks array");
      return kFATAL;
    }
  }
  fStsTracks=(TClonesArray*)fManager->GetObject("StsTrack");
  if (!fStsTracks)
  {
    Fatal("Init()","Can't find STS tracks array");
    return kFATAL;
  }
  fMaximums=(TClonesArray*)fManager->GetObject("EcalMaximums");
  if (!fMaximums)
  {
    Fatal("Init()","Can't find array of calorimeter maximums");
    return kFATAL;
  }
  fId=new TClonesArray("CbmEcalIdParticle", 2000);
  fManager->Register("EcalId", "ECAL", fId, kTRUE);
  fEvent=0;
  CbmEcalParam* p=new CbmEcalParam("IdentificationParam", fCfgName);
  fTreeOut=p->GetInteger("treeout");
  fUseHack=p->GetInteger("usehacks");
  fUseMC=p->GetInteger("usemcinfo");
  nm=p->GetString("eresolution");
  fOldShapeAlgo=p->GetInteger("oldshapealgo");
  fSimpleMaximumLocationAlgo=p->GetInteger("simplemaximumlocationalgo");
  fEResolution=new TFormula("calorimeter_energy_resolution", nm);
  if (fUseHack>0)
  {
    Info("Init", "Hack option given in configuration file.");
    fHack=p->GetDouble("hack");
    fMC=(TClonesArray*)fManager->GetObject("MCTrack");
    if (!fMC)
    {
      Fatal("Init", "Can't find array of MC tracks in system.");
      return kFATAL;
    }
    fMCPoints=(TClonesArray*)fManager->GetObject("EcalPoint");
    if (!fMCPoints)
    {
      Fatal("Init","Can't find array of EcalPoint in the system");
      return kFATAL;
    }
  }
  if (fUseMC)
  {
    fCircRad=p->GetDouble("circrad");
/*
    fStsTracks=(TClonesArray*)fManager->GetObject("StsTrack");
    if (!fStsTracks)
    {
      Fatal("Init()","Can't find STS tracking information");
      return kFATAL;
    }
*/
    fStsTracksMatch=(TClonesArray*)fManager->GetObject("StsTrackMatch");
    if (!fStsTracksMatch)
    {
      Fatal("Init()","Can't find STS track matching information");
      return kFATAL;
    }
  }
  fF=new Double_t[18];
  for(f=0;f<6;f++)
  {
    nm="f"; nm+=f;
    res=p->GetString(nm);
    nm="dist_formula_f"; nm+=f;
    fForm[f]=new TFormula(nm, res);
    for(r=0;r<3;r++)
    {
      nm="f"; nm+=f; nm+="p"; nm+=r;
      fF[f*3+r]=p->GetDouble(nm);
      fForm[f]->SetParameter(r, fF[f*3+r]);
    }
  }
  fUseShLib=p->GetInteger("useshlib");
  if (fUseShLib)
  {
    fShLib=(CbmEcalShLibCorr*)fManager->GetObject("EcalEShLib");
    if (!fShLib)
    {
      Fatal("Init()", "Can't find EcalEShLib in the system");
      return kFATAL;
    }
  }
  else
    fShLib=NULL;
  fQL=p->GetDouble("ql");
  fQH=p->GetDouble("qh");
  if (fOldShapeAlgo==0)
  {
    fQCL=p->GetDouble("qcl");
    fQCH=p->GetDouble("qch");
  }
  fCPL=p->GetDouble("cpl");
  fCPH=p->GetDouble("cph");

  fUseCellType=p->GetInteger("usecelltype");
  // Clustering stuff
  fRegions=p->GetInteger("regions");
  fIntSteps=p->GetInteger("intsteps");
  fThetaB=new Double_t[fRegions];
  fAlgo=new Int_t[fRegions];
  fPar1=new Double_t[fRegions];
  fPar2=new Double_t[fRegions];
  fPar3=new Double_t[fRegions];
  fParI=new Int_t[fRegions];
  for(r=0;r<fRegions;r++)
  {
    nm="theta["; nm+=r; nm+="]";
    fThetaB[r]=p->GetDouble(nm);

    nm="preclusteralgo["; nm+=r; nm+="]";
    fAlgo[r]=p->GetDouble(nm);
    
    if (fAlgo[r]==0) continue;

    nm="par1["; nm+=r; nm+="]";
    fPar1[r]=p->GetDouble(nm);

    nm="par2["; nm+=r; nm+="]";
    fPar2[r]=p->GetDouble(nm);

    nm="par3["; nm+=r; nm+="]";
    fPar3[r]=p->GetDouble(nm);

    nm="pari["; nm+=r; nm+="]";
    fParI[r]=p->GetInteger(nm);
  }
  delete p;

  fTree=NULL;

  return kSUCCESS;
}

/** Finishing routine **/
void CbmEcalIdentification::Finish()
{
//  TFile* f=new TFile("cal.root", "RECREATE");
//  cout << gFile->GetName() << endl;
  if (fTree)
    fTree->Write();
//  f->Close();
}

Double_t CbmEcalIdentification::GetPL(Double_t e, Double_t theta) const
{
  Double_t p0=fForm[0]->Eval(e);
  Double_t p1=fForm[1]->Eval(e);
  Double_t p2=fForm[2]->Eval(e);
  return p0+theta*(p1+theta*p2);
}

Double_t CbmEcalIdentification::GetPH(Double_t e, Double_t theta) const
{
  Double_t p0=fForm[3]->Eval(e);
  Double_t p1=fForm[4]->Eval(e);
  Double_t p2=fForm[5]->Eval(e);
  return p0+theta*(p1+theta*p2);
}

Int_t CbmEcalIdentification::CheckMaximum(CbmEcalMaximum* max)
{
  Double_t trpl;
  Double_t trph;
  Double_t trdp;
  Double_t trp;
  Double_t trq; 
  Double_t dq=fQH-fQL;
  Double_t trpm;
  TVector2 v;

  trpl=GetPL(fTrackP, fTheta); fPL=trpl;
  trph=GetPH(fTrackP, fTheta); fPH=trph;
  fPC=(fPL+fPH)/2.0; fPR=(fPH-fPL)/2.0;
//  cout << "(pl.ph)=(" << trpl << ","<< trph ;
  trdp=trph-trpl;
  if (trdp>dq)
  {
    trpl-=fCPL*trdp;
    trph+=fCPH*trdp;
  }
  else
  {
    trpm=(trph+trpl)/2.0;
    trpl=trpm-dq/2.0;
    trph=trpm+dq/2.0;
  }

  v.Set(fX-max->X(), fY-max->Y());
  v=v.Rotate(-fPhi*TMath::DegToRad());
  trp=v.X(); trq=v.Y();
//  cout << ").(" << fX << "," << fY << "):(" << max->X() << "," << max->Y() << ")"; 
//  cout << ".(p,q)=(" << trp << "," << trq << ").(pl.ph)=(" << trpl << ","<< trph << "):Phi=" << fPhi;
  if (trp>trpl&&trp<trph&&trq>fQL&&trq<fQH)
  {
//    cout << " <---------" << endl;
    return 1;
  }

//  cout << endl;
  return 0;  
}

void CbmEcalIdentification::ConstructCluster(CbmEcalMaximum* max)
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

  fCluster.clear();
  cell=max->Cell();
  if (fUseCellType==1)
    fRegion=cell->GetType();
  else
  {
    for(i=0;i<fRegions;i++)
      if (fThetaB[i]>fTheta)
        break;
    fRegion=i;
  }
  if (fRegion>=fRegions||fRegion<0)
  {
    Info("ConstructCluster", "Cluster can't be constructed. Theta=%f.", fTheta);
    return;
  }
  if (fAlgo[fRegion]==0)
  {
//    cell->GetNeighborsList(0, cells);
    cell->GetNeighborsList(max->I(), fCluster);
    fCluster.push_back(cell);
//    for(p=cells.begin();p!=cells.end();++p)
//      if ((*p)->GetTotalEnergy()<e)
//      {
//	cell=(*p);
//	e=(*p)->GetTotalEnergy();
//     }
//    if (find(fCluster.begin(), fCluster.end(), cell)==fCluster.end())
//      fCluster.push_back(cell);
    return;
  }

  for(i=0;i<fParI[fRegion];i++)
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
    r=SolveEllipse(cll, max->X(), max->Y(), fRegion);
    for(i=0;i<fParI[fRegion];i++)
      if (rc[i]<r)
	break;
    if (i==fParI[fRegion]) continue;
    for(j=fParI[fRegion]-1;j>i;j--)
    {
      rc[j]=rc[j-1];
      cls[j]=cls[j-1];
    }
    rc[i]=r;
    cls[i]=cll;
  }
  for(j=0;j<fParI[fRegion];j++)
    fCluster.push_back(cls[j]);
}

Double_t CbmEcalIdentification::SolveEllipse(CbmEcalCell* cell, Double_t cx, Double_t cy, Int_t reg)
{
  Double_t cRx=fPar2[reg];		//48*1.6
  Double_t cRy=fPar3[reg];
  Int_t cSt=fIntSteps;
  TVector2 centr(cx, cy);
  TVector2 t(centr);
  Double_t gm=fPhi;
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

void CbmEcalIdentification::ShapeAnalysis(CbmEcalMaximum* max)
{
  if (!fTreeOut) return;

  CbmEcalCell* cell;
  list<CbmEcalCell*> cells;
  list<CbmEcalCell*>::const_iterator p;
  Int_t i;
  Int_t j;
  Double_t cp=0;
  Double_t cq=0;
  Double_t dp;
  Double_t dq;
  Double_t e;
  TVector2 v;
  TVector2 v1;
  TVector2 v2;

  cell=max->Cell();
  fEMax=GetEnergy(cell);

  fE2=fEMax; fE3=fEMax;
  cell->GetNeighborsList(max->I(), cells);
  for(p=cells.begin();p!=cells.end();++p)
    fE2+=GetEnergy(*p);

  cell->GetNeighborsList(0, cells);
  for(p=cells.begin();p!=cells.end();++p)
    fE3+=GetEnergy(*p);

  fECluster=0;
  for(p=fCluster.begin();p!=fCluster.end();++p)
  {
    //TODO: Should use GetEnergy only once
    e=GetEnergy(*p);
    fECluster+=e;
    for(i=0;i<5;i++)
      if (fSE[i]<e)
	break;
    if (i!=5)
    {
      for(j=4;j>i;j--)
       fSE[j]=fSE[j-1];
      fSE[i]=e;
    }
    v.Set((*p)->GetCenterX(), (*p)->GetCenterY());
    v=v.Rotate(-fPhi*TMath::DegToRad());
    cp+=v.X()*e; cq+=v.Y()*e;
  }
  for(i=1;i<5;i++)
    fSE[i]+=fSE[i-1];

  fP2=0; fQ2=0;
  cp/=fECluster; cq/=fECluster;
  for(p=fCluster.begin();p!=fCluster.end();++p)
  {
    e=GetEnergy(*p);
    v.Set((*p)->GetCenterX(), (*p)->GetCenterY());
    v=v.Rotate(-fPhi*TMath::DegToRad());
    dp=v.X()-cp; dq=v.Y()-cq;
    fP2=dp*dp*e; fQ2=dq*dq*e;
  }
  fP2/=fECluster; fQ2/=fECluster;

  v1.Set(fX, fY);
  v1=v1.Rotate(-fPhi*TMath::DegToRad());
  v2.Set(max->X(), max->Y());
  v2=v2.Rotate(-fPhi*TMath::DegToRad());
  v=v2; v-=v1;
  fP=v.X(); fQ=v.Y();
  fAngle=v.Phi()*TMath::RadToDeg();
  if (fAngle>180) fAngle-=360;
}

void CbmEcalIdentification::CalculateShape(CbmEcalMaximum* max)
{
  if (fOldShapeAlgo==1)
  {
    CbmEcalCell* cell=max->Cell();
    Double_t e=cell->GetTotalEnergy();
    list<CbmEcalCell*> cells;
    cell->GetNeighborsList(max->I(), cells);
    list<CbmEcalCell*>::const_iterator p=cells.begin();
    Double_t e2=e;
    for(;p!=cells.end();++p)
      e2+=(*p)->GetTotalEnergy();

    cell->GetNeighborsList(0, cells);
    Double_t e3=e;
    for(p=cells.begin();p!=cells.end();++p)
      e3+=(*p)->GetTotalEnergy();
    fShape=e2/e3;

    return;
  }
  TVector2 v1(fX, fY); v1=v1.Rotate(-fPhi*TMath::DegToRad());
  TVector2 v2(max->X(), max->Y()); v2=v2.Rotate(-fPhi*TMath::DegToRad());
  v1-=v2;
  fShape=0;
//  cerr << v1.X() << ", " << v1.Y() << " : ([" << fPL << ", " << fPH << "], [" << fQCL << ", " << fQCH << "])" << endl;
  if (v1.Y()<fQCL) return;
  if (v1.Y()>fQCH) return;
  if (v1.X()<fPL)  return;
  if (v1.X()>fPH)  return;
  fShape=1;
}

ClassImp(CbmEcalIdentification)

