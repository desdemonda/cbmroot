#ifndef CBMECALSHLIBCORRRECORD_H
#define CBMECALSHLIBCORRRECORD_H

#include "TObject.h"

#include <iostream>

using namespace std;

class CbmEcalShLibCorrRecord : public TObject
{
public:
  CbmEcalShLibCorrRecord(const char* filename, Int_t verb=0);
  Float_t CellSize() const {return fCellSize;}
  Float_t Energy(Float_t x, Float_t y, Float_t phi, Float_t theta, Float_t e);
  Float_t Correlation(Float_t x1, Float_t y1, Float_t x2, Float_t y2, Float_t phi, Float_t theta, Float_t e);
  void DrawTest(Float_t phi, Float_t theta, Float_t e);
  virtual ~CbmEcalShLibCorrRecord();
  Int_t fError;
private:
  Float_t Get(Float_t phi, Float_t theta, Float_t e);
  Float_t GetE(Float_t phi, Float_t theta, Int_t e);
  Float_t GetETheta(Float_t phi, Int_t theta, Int_t e);
  Int_t GetN(Int_t phi, Int_t theta, Int_t e) { return phi+fNPhi*(theta+fNTheta*e); }
  Float_t Energy(Float_t* data);
  Float_t Correlation(Float_t* corr);
  Int_t CorrelationN(Float_t x1, Float_t y1, Float_t x2, Float_t y2);


  Float_t fCellSize;
  Int_t fNE;
  Float_t* fE;			//!

  Int_t fNTheta;
  Float_t* fTheta;		//!

  Int_t fNPhi;
  Float_t* fPhi;		//!
  
  Float_t fStep;
  Int_t iCellSize;
  Float_t fEndX;
  Float_t fStartX;

  Float_t fEndY;
  Float_t fStartY;

  Int_t fNCells;
  Int_t fNCells2;

  Int_t fSizeData;
  Int_t fSizeCorr;

  // iCellSize*iCellSize
  Int_t fSize;

  Int_t fN;

  Int_t fVerbose;

  // Response and correlation data
  Float_t** fData;		//!
  Float_t** fCorr;		//!

  // For caching 
  Float_t fX1;
  Float_t fX2;
  Float_t fY1;
  Float_t fY2;
  Int_t fCEnergy;

  CbmEcalShLibCorrRecord(const CbmEcalShLibCorrRecord&);
  CbmEcalShLibCorrRecord& operator=(const CbmEcalShLibCorrRecord&);

  ClassDef(CbmEcalShLibCorrRecord, 1);
};

inline Float_t CbmEcalShLibCorrRecord::Get(Float_t phi, Float_t theta, Float_t e)
{
  Int_t i=0;
  for(;i<fNE;i++)
    if (fE[i]>=e)
      break;
//  cout << i << endl;
  if (i==fNE) return GetE(phi, theta, fNE-1);
  if (i==0) return GetE(phi, theta, 0);
  Float_t t=fE[i]-fE[i-1];
  Float_t d=(fE[i]-e)/t;
//  cout << phi << " " << theta << " " << e << endl;
//  cout << t << " " << d << endl;
//  cout <<  GetE(phi, theta, i) << endl;
//  cout <<  GetE(phi, theta, i-1) << endl;
  return GetE(phi, theta, i)*(1.0-d)+GetE(phi, theta, i-1)*d;
}

inline Float_t CbmEcalShLibCorrRecord::GetE(Float_t phi, Float_t theta, Int_t e)
{
  Int_t i=0;
  for(;i<fNTheta;i++)
    if (fTheta[i]>=theta)
      break;
//  if (i==fNTheta||i==0) return 0;
  if (i==fNTheta) return GetETheta(phi, fNTheta-1, e);
  if (i==0) return GetETheta(phi, 0, e);
  Float_t t=fTheta[i]-fTheta[i-1];
  Float_t d=(fTheta[i]-theta)/t;
//  cout << i << " --- " << cTheta[i] <<" "<< d << endl;
  return GetETheta(phi, i, e)*(1.0-d)+GetETheta(phi, i-1, e)*d;
}

inline Float_t CbmEcalShLibCorrRecord::GetETheta(Float_t phi, Int_t theta, Int_t e)
{
  Float_t dphi=fPhi[1]-fPhi[0];
  Float_t t=phi/dphi;
  Int_t n=(Int_t)(t);
  Float_t d=t-n;
  Int_t n1=GetN(n, theta, e);
  if (n==fNPhi-1)
  {
    if (fCEnergy)
      return Energy(fData[n1]);
    else
      return Correlation(fCorr[n1]);
  }
  Int_t n2=GetN(n+1, theta, e);

//  cout << "Serial numbers " << n1 << " --- " << n2 << ":" << phi << " is " << n << endl;
  if (fCEnergy)
    return Energy(fData[n1])*(1.0-d)+d*Energy(fData[n2]);
  else
    return Correlation(fCorr[n1])*(1.0-d)+d*Correlation(fCorr[n2]);
}

#endif
