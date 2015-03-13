#include "CbmEcalShLibCorrRecord.h"

#include "TFile.h"
#include "TTree.h"
#include "TH2D.h"
#include "TMath.h"

#include <iostream>

using namespace std;

CbmEcalShLibCorrRecord::CbmEcalShLibCorrRecord(const char* filename, Int_t verb)
  : TObject(),
    fError(0),
    fCellSize(0.),
    fNE(0),
    fE(NULL),
    fNTheta(0),
    fTheta(NULL),
    fNPhi(0),
    fPhi(NULL),
    fStep(0.),
    iCellSize(0),
    fEndX(0.),
    fStartX(0.),
    fEndY(0.),
    fStartY(0.),
    fNCells(0),
    fNCells2(0),
    fSizeData(0),
    fSizeCorr(0),
    fSize(0),
    fN(0),
    fVerbose(verb),
    fData(NULL),
    fCorr(NULL),
    fX1(0.),
    fX2(0.),
    fY1(0.),
    fY2(0.),
    fCEnergy(0)
{
  TFile* f=new TFile(filename);
  TTree* info;
  TTree* data;
  Float_t t;
  Int_t i;
  Int_t j;
  Float_t* dta;
  Float_t* corr;

  Info("CbmEcalShLibCorrRecord", "Loading %s...", filename);
  if (f->IsZombie())
  {
    Fatal("CbmEcalShLibCorrRecord", "File %s not found.", filename);
    return;
  }
  info=(TTree*)f->Get("info");
  info->SetBranchAddress("startx", &fStartX);
  info->SetBranchAddress("endx", &fEndX);
  info->SetBranchAddress("starty", &fStartY);
  info->SetBranchAddress("endy", &fEndY);
  info->SetBranchAddress("ne", &fNE);
  info->SetBranchAddress("nphi", &fNPhi);
  info->SetBranchAddress("ntheta", &fNTheta);
  info->SetBranchAddress("cellsize", &fCellSize);
  info->SetBranchAddress("sizedata", &fSizeData);
  info->SetBranchAddress("sizecorr", &fSizeCorr);
  info->SetBranchAddress("icellsize", &iCellSize);
  info->SetBranchAddress("step", &fStep);
  info->SetBranchAddress("ncells", &fNCells);
  info->GetEntry(0);
  fNCells2=fNCells*fNCells;
  fE=new Float_t[fNE];
  fTheta=new Float_t[fNTheta];
  fPhi=new Float_t[fNPhi];
  fN=fNE*fNTheta*fNPhi;
  fData=new Float_t*[fN];
  fCorr=new Float_t*[fN];
  fSize=iCellSize*iCellSize;
  for(i=0;i<fN;i++)
  {
    fData[i]=new Float_t[fSizeData];
    fCorr[i]=new Float_t[fSizeCorr];
  }
  dta=new Float_t[fSizeData];
  corr=new Float_t[fSizeCorr];
  info->SetBranchAddress("e", fE);
  info->SetBranchAddress("phi", fPhi);
  info->SetBranchAddress("theta", fTheta);
  info->GetEntry(0);
  data=(TTree*)f->Get("shlib");
  data->SetBranchAddress("data", dta);
  data->SetBranchAddress("corr", corr);
  if (fN!=data->GetEntries())
  {
    Fatal("CbmEcalShLibCorrRecord", "Info and data trees inconsistent.");
    return;
  }
  for(i=0;i<fN;i++)
  {
    data->GetEntry(i);
    for(j=0;j<fSizeData;j++)
      fData[i][j]=dta[j];
    for(j=0;j<fSizeCorr;j++)
      fCorr[i][j]=corr[j];

  }

//  delete [] dta;
//  delete [] corr;
  f->Close();

  if (fVerbose>99)
  {
    cout << "Information tree read." << endl;
    cout << "	n        = " << fN << endl;
    cout << "	cellsize = " << fCellSize << endl;
    cout << "	icellsize= " << iCellSize << endl;
    cout << "	step     = " << fStep << endl;
    cout << "	startx   = " << fStartX << endl;
    cout << "	endx     = " << fEndX << endl;
    cout << "	starty   = " << fStartY << endl;
    cout << "	endy     = " << fEndY << endl;
    cout << "	ncells   = " << fNCells << endl;
    cout << "	sizedata = " << fSizeData << endl;
    cout << "	sizecorr = " << fSizeCorr << endl;
    cout << "	nphi     = " << fNPhi << " : (";
    for(i=0;i<fNPhi;i++)
      if (i!=0) cout << ", " << fPhi[i]; else cout << fPhi[i];
    cout << ")" << endl; 
    cout << "	ntheta   = " << fNTheta << " : (";
    for(i=0;i<fNTheta;i++)
      if (i!=0) cout << ", " << fTheta[i]; else cout << fTheta[i];
    cout << ")" << endl; 
    cout << "	ne       = " << fNE << " : (";
    for(i=0;i<fNE;i++)
      if (i!=0) cout << ", " << fE[i]; else cout << fE[i];
    cout << ")" << endl; 
  }
}

Float_t CbmEcalShLibCorrRecord::Energy(Float_t* data)
{
  Float_t x=fX1;
  Float_t y=fY1;
  if (x<fStartX-fStep||y<fStartY-fStep) return 0;
  if (x>fStartX+fNCells*fCellSize||y>fStartY+fNCells*fCellSize) return 0;
  Float_t v1=0;
  Float_t v2=0;
  Float_t v3=0;
  Float_t v4=0;
  Float_t sx=x-fStartX;
  Float_t sy=y-fStartY;
  Float_t dx=(sx-(((Int_t)(sx/fStep))*fStep))/fStep;
  Float_t dy=(sy-(((Int_t)(sy/fStep))*fStep))/fStep;
  Float_t sx2=sx+fStep;
  Float_t sy2=sy+fStep;
  Int_t cx=(Int_t)(sx/fCellSize);
  Int_t cy=(Int_t)(sy/fCellSize);
  Int_t cx2=(Int_t)(sx2/fCellSize);
  Int_t cy2=(Int_t)(sy2/fCellSize);

  sx-=cx*fCellSize;
  sy-=cy*fCellSize;
  sx2-=cx2*fCellSize;
  sy2-=cy2*fCellSize;

  Int_t ix=(Int_t)(sx/fStep);
  Int_t iy=(Int_t)(sy/fStep);
  Int_t ix2=(Int_t)(sx2/fStep);
  Int_t iy2=(Int_t)(sy2/fStep);

/*
  cout << "(" << x << ", " << y << ") : (" << dx << ", " << dy << ")" << endl;
  Int_t is;
  Int_t im;
  GetN(x, y, is, im);
  cout << (ix+iy*iCellSize)+(cx+cy*fNCells)*fSize << " : " << im+is*fSize << endl;
  GetN(x+fStep, y, is, im);
  cout << (ix2+iy*iCellSize)+(cx2+cy*fNCells)*fSize << " : " << im+is*fSize << endl;
  GetN(x, y+fStep, is, im);
  cout << (ix+iy2*iCellSize)+(cx+cy2*fNCells)*fSize << " : " << im+is*fSize << endl;
  GetN(x+fStep, y+fStep, is, im);
  cout << (ix2+iy2*iCellSize)+(cx2+cy2*fNCells)*fSize << " : " << im+is*fSize << endl;
*/
  if (x>=fStartX&&y>=fStartY)
    v1=data[(ix+iy*iCellSize)+(cx+cy*fNCells)*fSize];
  if (x<fStartX+fNCells*fCellSize-fStep)
  {
    v2=data[(ix2+iy*iCellSize)+(cx2+cy*fNCells)*fSize];
    if (y<fStartY+fNCells*fCellSize-fStep)
    {
      if (x>=fStartX) v3=data[(ix+iy2*iCellSize)+(cx+cy2*fNCells)*fSize];
      v4=data[(ix2+iy2*iCellSize)+(cx2+cy2*fNCells)*fSize];
    }
  }
  else
  if (y<fStartY+fNCells*fCellSize-fStep&&x>=fStartX)
    v3=data[(ix+iy2*iCellSize)+(cx+cy2*fNCells)*fSize];

//  cout << v1 << ", " << v2 << ", " << v3 << ", "  << v4 << ":" << v1+dx*(v2-v1)+dy*(v3-v1+dx*(v4-v3-v2+v1)) << endl;
//  cout << fError << endl;
  return v1+dx*(v2-v1)+dy*(v3-v1+dx*(v4-v3-v2+v1));
//  return (1.0-dy)*(v1+dx*(v2-v1))+dy*(v3+dx*(v4-v3));
}

Int_t CbmEcalShLibCorrRecord::CorrelationN(Float_t x1, Float_t y1, Float_t x2, Float_t y2)
{
  Float_t sX=fStartX+fNCells*fCellSize;
  Float_t sY=fStartY+fNCells*fCellSize;
  if (x1<fStartX||y1<fStartY) return -1111;
  if (x2<fStartX||y2<fStartY) return -1111;
  if (x1>sX||y1>sY) return -1111;
  if (x2>sX||y2>sY) return -1111;
  Float_t sx1=x1-fStartX;
  Float_t sy1=y1-fStartY;
  Int_t cx1=(Int_t)(sx1/fCellSize);
  Int_t cy1=(Int_t)(sy1/fCellSize);
  Int_t is1=cx1+cy1*fNCells;
  sx1-=cx1*fCellSize;
  sy1-=cy1*fCellSize;
  Int_t ix1=(Int_t)(sx1/fStep);
  Int_t iy1=(Int_t)(sy1/fStep);
/* 11.08.22 //Dr.Sys
  Float_t sx2=x2-fStartX;
  Float_t sy2=y2-fStartY;
  Int_t cx2=(Int_t)(sx2/fCellSize);
  Int_t cy2=(Int_t)(sy2/fCellSize);
  Int_t is2=cx2+cy2*fNCells;
  sx2-=cx2*fCellSize;
  sy2-=cy2*fCellSize;
  Int_t ix2=(Int_t)(sx2/fStep);
  Int_t iy2=(Int_t)(sy2/fStep);
*/
  Float_t dx=x2-x1;
  Int_t idx=(Int_t)((dx*1.00001)/fCellSize);
  Float_t dy=y2-y1;
  Int_t idy=(Int_t)((dy*1.00001)/fCellSize);
  Int_t nis2=(idx+cx1)+(idy+cy1)*fNCells;
/*
  if (nis2!=is2)
  {
    cout << "-1212:	"<< ix1 << ":" << ix2 << ", " << iy1 << ":" << iy2 << endl;
    cout << "	" << x1 << ":" << x2 << ", " << y1 << ":" << y2 << endl;
    cout << "	" << is1 << ":" << is2 << endl;
    cout << "id:	" << idx << ":" << idy << "->" << nis2 << "(" << dx << ";" << dy << ")" << endl; 
  }
*/
  //Wrong input data
  if (TMath::Abs(dx-idx*fCellSize)>1e-4||TMath::Abs(dy-idy*fCellSize)>1e-4)
  {
//    cout << "-1112:" << (dx-idx*fCellSize) << " : " << (dy-idy*fCellSize) << endl;
    fError=-1112;
    return -1112;
  }
/* 11.08.22 //Dr.Sys
  //Wrong input data
  if (ix1!=ix2||iy1!=iy2)
  {
    cout << "-1112:	"<< ix1 << ":" << ix2 << ", " << iy1 << ":" << iy2 << endl;
    cout << "	" << x1 << ":" << x2 << ", " << y1 << ":" << y2 << endl;
    cout << "	" << is1 << ":" << is2 << endl;
    fError=-1112;
    return -1112;
  }
*/
//  cout << sx1 << ", " << sy1 << endl;
//  cout << sx2 << ", " << sy2 << endl;
//  cout << ix1 << ", " << iy1 << ": " << is1 << ", " << is2 << endl;
//  cout << fSize << " : " << iCellSize << " : "  << fNCells2 << endl;
  return (ix1+iCellSize*iy1)+fSize*(is1+nis2*fNCells2);
}


Float_t CbmEcalShLibCorrRecord::Correlation(Float_t* corr)
{
  Float_t x1=fX1;
  Float_t y1=fY1;
  Float_t x2=fX2;
  Float_t y2=fY2;
  Float_t v1=0;
  Float_t v2=0;
  Float_t v3=0;
  Float_t v4=0;

  // TODO: Test!!!!!
  // 1
  Int_t n1=CorrelationN(x1, y1, x2, y2);
  if (-1112==n1) return -1112;
  if (-1111!=n1) v1=corr[n1];

  // 2
  Int_t n2=CorrelationN(x1+fStep+0.001, y1, x2+fStep+0.001, y2);
  if (-1112==n2)
  { 
    if (-1111!=n1)
    {
      cerr << "n2: (" << x1 << ", " << y1 << "), (" << x2 << ", " << y2 << ")" << endl;
      cerr << "-1112 returned. Check floating point errors." << endl;
    }
    return -1112;
  }
  if (-1111!=n2) v2=corr[n2];

  // 3
  Int_t n3=CorrelationN(x1, y1+fStep+0.001, x2, y2+fStep+0.001);
  if (-1112==n3)
  { 
    if (-1111!=n2)
    {
      cerr << "n3: (" << x1 << ", " << y1 << "), (" << x2 << ", " << y2 << ")" << endl;
      cerr << "-1112 returned. Check floating point errors." << endl;
    }
    return -1112;
  }
  if (-1111!=n3) v3=corr[n3];

  // 4
  Int_t n4=CorrelationN(x1+fStep+0.001, y1+fStep+0.001, x2+fStep+0.001, y2+fStep+0.001);
  if (-1112==n4)
  { 
    if (-1111!=n1)
    {
      cerr << "n4: (" << x1 << ", " << y1 << "), (" << x2 << ", " << y2 << ")" << endl;
      cerr << "-1112 returned. Check floating point errors." << endl;
    }
    return -1112;
  }
  if (-1111!=n4) v4=corr[n4];

  Float_t dx=(x1-(((Int_t)TMath::FloorNint(x1/fStep))*fStep))/fStep;
  Float_t dy=(y1-(((Int_t)TMath::FloorNint(y1/fStep))*fStep))/fStep;

//  cout << x1 << ", " << y1 << " : " << dx << ", " << dy << endl;
//  cout << v1 << ", " << v2 << ", " << v3 << ", "  << v4 << ":" << v1+dx*(v2-v1)+dy*(v3-v1+dx*(v4-v3-v2+v1)) << endl;
//  cout << "->" << n1 << " : " << v1 << endl;
//  cout << "->" << n2 << " : " << v2 << endl;
//  cout << "->" << n3 << " : " << v3 << endl;
//  cout << "->" << n4 << " : " << v4 << endl;
//  cout << fError << " : " << v1+dx*(v2-v1)+dy*(v3-v1+dx*(v4-v3-v2+v1)) << endl;
  return v1+dx*(v2-v1)+dy*(v3-v1+dx*(v4-v3-v2+v1));
}

void CbmEcalShLibCorrRecord::DrawTest(Float_t phi, Float_t theta, Float_t e)
{
  ;
}

Float_t CbmEcalShLibCorrRecord::Energy(Float_t x, Float_t y, Float_t phi, Float_t theta, Float_t e)
{
  fError=0;
  fCEnergy=1;
  fX1=x;  fY1=y;

  return Get(phi, theta, e);
}

Float_t CbmEcalShLibCorrRecord::Correlation(Float_t x1, Float_t y1, Float_t x2, Float_t y2, Float_t phi, Float_t theta, Float_t e)
{
  fError=0;
  fCEnergy=0;
  fX1=x1;  fY1=y1;  fX2=x2;  fY2=y2;
  return Get(phi, theta, e);
}
CbmEcalShLibCorrRecord::~CbmEcalShLibCorrRecord()
{
  delete [] fE;
  delete [] fTheta;
  delete [] fPhi;
}

ClassImp(CbmEcalShLibCorrRecord)
