#ifndef CBMECALSHLIBRECORD_H
#define CBMECALSHLIBRECORD_H

#include "TObject.h"

class CbmEcalShLibRecord : public TObject
{
public:
  CbmEcalShLibRecord(const char* filename, Int_t verb=0);
  Float_t CellSize() const {return fCellSize;}
  Float_t GetVal(Float_t x, Float_t y, Float_t phi, Float_t theta, Float_t e) const;
  void DrawTest(Float_t phi, Float_t theta, Float_t e);
  virtual ~CbmEcalShLibRecord();
private:
  Float_t GetValX(Int_t x, Float_t y, Float_t phi, Float_t theta, Float_t e) const;
  Float_t GetValXY(Int_t x, Int_t y, Float_t phi, Float_t theta, Float_t e) const;
  Float_t GetValXYTheta(Int_t x, Int_t y, Float_t phi, Int_t theta, Float_t e) const;
  Float_t GetValXYThetaE(Int_t x, Int_t y, Float_t phi, Int_t theta, Int_t e) const;

  Float_t fCellSize;
  Int_t fNE;
  Float_t* fE;			//!

  Int_t fNTheta;
  Float_t* fTheta;		//!

  Int_t fNPhi;
  Float_t* fPhi;		//!
  
  Int_t fNX;
  Float_t fStepX;
  Float_t fStartX;

  Int_t fNY;
  Float_t fStepY;
  Float_t fStartY;

  Int_t fVerbose;

  Int_t fN;
  Float_t* fR;			//!

  CbmEcalShLibRecord(const CbmEcalShLibRecord&);
  CbmEcalShLibRecord& operator=(const CbmEcalShLibRecord&);

  ClassDef(CbmEcalShLibRecord, 1);
};

inline Float_t CbmEcalShLibRecord::GetValXYThetaE(Int_t x, Int_t y, Float_t phi, Int_t theta, Int_t e) const
{
  static Float_t dphi=fPhi[1]-fPhi[0];
//  static Float_t oldphi=-1111;
  Float_t t=phi/dphi;
  Int_t n=(Int_t)(t);
  Float_t d=t-n;
  Int_t n1=x+fNX*(y+fNY*(n+fNPhi*(theta+fNTheta*e)));
  Int_t n2=n1+fNX*fNY;
/*
  if (oldphi!=phi)
  {
    oldphi=phi;
    cout << phi << "	" << n << " --- " << n1 << endl;
  }
  cout << x << ", " << y << ", " << n << ", " << theta << ", " << e << endl;
*/
  return fR[n1]*(1.0-d)+fR[n2]*d;
}

inline Float_t CbmEcalShLibRecord::GetValXYTheta(Int_t x, Int_t y, Float_t phi, Int_t theta, Float_t e) const
{
  Int_t i=0;
  for(;i<fNE;i++)
    if (fE[i]>=e)
      break;
//  cout << i << endl;
  if (i==fNE) return GetValXYThetaE(x, y, phi, theta, fNE-1);
  if (i==0) return GetValXYThetaE(x, y, phi, theta, 0);
  Float_t t=fE[i]-fE[i-1];
  Float_t d=(fE[i]-e)/t;
  return GetValXYThetaE(x, y, phi, theta, i)*(1.0-d)+GetValXYThetaE(x, y, phi, theta, i-1)*d;
}

inline Float_t CbmEcalShLibRecord::GetValXY(Int_t x, Int_t y, Float_t phi, Float_t theta, Float_t e) const 
{
  Int_t i=0;
  for(;i<fNTheta;i++)
    if (fTheta[i]>=theta)
      break;
//  if (i==fNTheta||i==0) return 0;
  if (i==fNTheta) return GetValXYTheta(x, y, phi, fNTheta-1, e);
  if (i==0) return GetValXYTheta(x, y, phi, 0, e);
  Float_t t=fTheta[i]-fTheta[i-1];
  Float_t d=(fTheta[i]-theta)/t;
//  cout << i << " --- " << cTheta[i] <<" "<< d << endl;
  return GetValXYTheta(x, y, phi, i, e)*(1.0-d)+GetValXYTheta(x, y, phi, i-1, e)*d;
}

inline Float_t CbmEcalShLibRecord::GetValX(Int_t x, Float_t y, Float_t phi, Float_t theta, Float_t e) const
{
  Float_t t=(y-fStartY)/fStepY;
  Int_t n=(Int_t)(t);
  if (n<0||n>=fNY) return 0;
  Float_t d=t-n;
  if (n+1==fNY)
    return GetValXY(x, n, phi, theta, e)*(1.0-d);
  else
    return GetValXY(x, n, phi, theta, e)*(1.0-d)+d*GetValXY(x, n+1, phi, theta, e);
}

inline Float_t CbmEcalShLibRecord::GetVal(Float_t x, Float_t y, Float_t phi, Float_t theta, Float_t e) const 
{
  Float_t t=(x-fStartX)/fStepX;
  Int_t n=(Int_t)(t);
  if (n<0||n>=fNX) return 0;
  Float_t d=t-n;
  if (n+1==fNX)
    return GetValX(n, y, phi, theta, e)*(1.0-d);
  else
    return GetValX(n, y, phi, theta, e)*(1.0-d)+d*GetValX(n+1, y, phi, theta, e);
}




#endif
