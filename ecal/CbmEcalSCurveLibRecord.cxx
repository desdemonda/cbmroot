/* $Id: CbmEcalSCurveLibRecord.cxx,v 1.1 2006/09/18 07:58:04 prokudin Exp $ */
/*
 * $Log: CbmEcalSCurveLibRecord.cxx,v $
 * Revision 1.1  2006/09/18 07:58:04  prokudin
 * First implementation of SCurve library
 *
 */


#include "CbmEcalSCurveLibRecord.h"

#include "TFile.h"
#include "TTree.h"

#include <list>
#include <iostream>
#include <algorithm>

using std::cerr;
using std::endl;
using std::list;

// - implementation

CbmEcalSCurveLibRecord::CbmEcalSCurveLibRecord(const char* filename)
  : fCellSize(0),
    fPoints(0),
    fEnergies(0),
    fE(NULL),
    fThetas(),
    fTheta(NULL),
    fA(NULL),
    fX(NULL),
    fSize(0),
    fTail(10),
    fXL(NULL),
    fDXL(NULL),
    fXR(NULL),
    fDXR(NULL)
{
  TFile* file;
  TTree* tree;
  Float_t e;
  Float_t theta;
  Int_t size;
  Float_t* x;
  Float_t  cellsize;
  list<Float_t>::const_iterator p;
  list<Float_t> es;
  list<Float_t> thetas;
  Int_t i;
  Int_t j;
  Int_t num;

  file=new TFile(filename);
  if (file->IsZombie())
  {
    cerr << "Something wrong with input file " << filename << endl;
    return;
  }
  
  tree=(TTree*)file->Get("sc");
  if (tree==NULL)
  {
    cerr << "Can't find SCurve tree in " << filename << endl;
    return;
  }
  if (tree->GetEntries()<1)
  {
    file->Close();
    return;
  }
  tree->SetBranchAddress("outcellsize",&cellsize);
  tree->SetBranchAddress("oute",&e);
  tree->SetBranchAddress("outtheta",&theta);
  tree->SetBranchAddress("outsize",&size);
  tree->GetEntry(0);

  fCellSize=cellsize;
  fPoints=tree->GetEntries();
  fSize=size;
  x=new Float_t[size];

  es.clear();
  thetas.clear();
  for(i=0;i<fPoints;i++)
  {
    tree->GetEntry(i);
    if (find(es.begin(), es.end(), e)==es.end())
      es.push_back(e);
    if (find(thetas.begin(), thetas.end(), theta)==thetas.end())
      thetas.push_back(theta);
  }

//Initializing energies and thetas lists

  fE=new Float_t[es.size()];
  fEnergies=0;
  for(p=es.begin();p!=es.end();p++)
   fE[fEnergies++]=(*p);

  fThetas=0;
  fTheta=new Float_t[thetas.size()];
  for(p=thetas.begin();p!=thetas.end();p++)
    fTheta[fThetas++]=(*p);

  tree->SetBranchAddress("outx",x);
  fX=new Float_t*[fPoints];
  for(i=0;i<fPoints;i++)
    fX[i]=NULL;
  
  for(i=0;i<fPoints;i++)
  {
    tree->GetEntry(i);
    num=GetNum(e, theta);
    if (fX[num]!=NULL)
    {
      cerr << "Duplicate entry for " << e << "GeV, theta ";
      cerr << theta << ". Skipping!" << endl;
      continue;
    }
    fX[num]=new Float_t[fSize];
    for(j=0;j<fSize;j++)
      fX[num][j]=x[j];
  }

  delete [] x;
  file->Close();
  delete file;
  InitA();
}

void CbmEcalSCurveLibRecord::InitA()
{
  Int_t i;
  Int_t j;
  Float_t step=1.0/(fSize-1);

  fA=new Float_t[fSize];
  fA[0]=-0.5;
  for(i=1;i<fSize;i++)
    fA[i]=fA[i-1]+step;

  fTail=fSize/6;
  fXL=new Float_t[fPoints];
  fXR=new Float_t[fPoints];
  fDXL=new Float_t[fPoints];
  fDXR=new Float_t[fPoints];
  for(j=0;j<fPoints;j++)
  {
    fXL[j]=0;
    fXR[j]=0;
    fDXL[j]=0;
    fDXR[j]=0;

    for(i=0;i<fTail;i++)
      fDXL[j]+=(fX[j][i+1]-fX[j][i])/step/fTail;
    for(i=0;i<fTail;i++)
      fXL[j]+=fX[j][i]-fA[i]*fDXL[j];
    fXL[j]/=fTail;


    for(i=fSize-fTail;i<fSize;i++)
      fDXR[j]+=(fX[j][i]-fX[j][i-1])/step/fTail;
    for(i=fSize-fTail;i<fSize;i++)
      fXR[j]+=fX[j][i]-fA[i]*fDXR[j];
    fXR[j]/=fTail;
  }
}


// Performance is UGLY!
Float_t CbmEcalSCurveLibRecord::GetX(Float_t a, Float_t e, Float_t theta) const
{
  Int_t i;
  Int_t tp;
  Int_t tn;
  Float_t tpf;
  Float_t tnf;
  Int_t ep;
  Int_t en;
  Float_t epf;
  Float_t enf;

  Int_t xn;
  Int_t xp;
  Float_t xnf;
  Float_t xpf;

  if (theta<fTheta[0]||e<fE[0])
    return -1111;
  for(i=1;i<fThetas;i++)
    if (fTheta[i]>theta) break;
  if (i==fThetas)
  {
    if ((fTheta[fThetas-1]-theta)/theta>0.02)
      return -1111;
    tp=i-1;
    tn=i;
  } else
  {
    tp=i-1;
    tn=i;
  }
  tpf=(fTheta[tn]-theta)/(fTheta[tn]-fTheta[tp]);
  tnf=(theta-fTheta[tp])/(fTheta[tn]-fTheta[tp]);

  for(i=1;i<fEnergies;i++)
    if (fE[i]>e) break;
  if (i==fEnergies)
  { ep=i-1; en=i-1; epf=1.0; enf=0.0;}
  else
  { ep=i-1; en=i;
    epf=(fE[en]-e)/(fE[en]-fE[ep]);
    enf=(e-fE[ep])/(fE[en]-fE[ep]);
  }
  if (TMath::Abs(a)<=0.5)
  {
    for(i=1;i<fSize;i++)
      if (fA[i]>a)
	break;
    xn=i;
    xp=i-1;
    xpf=(fX[ep*fThetas+tp][xp]*tpf+fX[ep*fThetas+tn][xp]*tnf)*epf+(fX[en*fThetas+tp][xp]*tpf+fX[en*fThetas+tn][xp]*tnf)*enf;
    xnf=(fX[ep*fThetas+tp][xn]*tpf+fX[ep*fThetas+tn][xn]*tnf)*epf+(fX[en*fThetas+tp][xn]*tpf+fX[en*fThetas+tn][xn]*tnf)*enf;
    return (xpf*(fA[xn]-a)+xnf*(a-fA[xp]))/(fA[xn]-fA[xp]);
  }
  if (a>0.5)
  {
    xpf=(fXR[ep*fThetas+tp]*tpf+fXR[ep*fThetas+tn]*tnf)*epf+(fXR[en*fThetas+tp]*tpf+fXR[en*fThetas+tn]*tnf)*enf;
    xnf=(fDXR[ep*fThetas+tp]*tpf+fDXR[ep*fThetas+tn]*tnf)*epf+(fDXR[en*fThetas+tp]*tpf+fDXR[en*fThetas+tn]*tnf)*enf;
    return xpf+xnf*a;
  }
  xpf=(fXL[ep*fThetas+tp]*tpf+fXL[ep*fThetas+tn]*tnf)*epf+(fXL[en*fThetas+tp]*tpf+fXL[en*fThetas+tn]*tnf)*enf;
  xnf=(fDXL[ep*fThetas+tp]*tpf+fDXL[ep*fThetas+tn]*tnf)*epf+(fDXL[en*fThetas+tp]*tpf+fDXL[en*fThetas+tn]*tnf)*enf;
  return xpf+xnf*a;
}

Int_t CbmEcalSCurveLibRecord::GetNum(Float_t energy, Float_t theta) const 
{
  Int_t i;
  Int_t en;
  Int_t the;

  for(i=0;i<fEnergies;i++)
    if (TMath::Abs(energy-fE[i])/fE[i]<0.001)
      break;
  if (i==fEnergies) return -1;
  en=i;
  
  for(i=0;i<fThetas;i++)
    if (TMath::Abs(theta-fTheta[i])<0.001)
      break;
  if (i==fThetas) return -1;
  the=i;

  return the+en*(fThetas);
}


CbmEcalSCurveLibRecord::~CbmEcalSCurveLibRecord()
{
  delete [] fE;
  delete [] fXL;
  delete [] fXR;
  delete [] fDXL;
  delete [] fDXR; 
  if (fX)
  {
    for(Int_t i=0;i<fPoints;i++)
      delete [] fX[i];
    delete [] fX;
  }
  delete [] fTheta;
  delete [] fA;
}

ClassImp(CbmEcalSCurveLibRecord)
