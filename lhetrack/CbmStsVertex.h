/////////////////////////////////////////////////////////////////
//                                                             //
// CbmStsVertex class - representation of the  vertex          //
//                                                             //
/////////////////////////////////////////////////////////////////

#ifndef CBM_STS_VERTEX_H
#define CBM_STS_VERTEX_H

#include "TObject.h"
#include "TVector3.h"

class CbmStsVertex : public TObject {

private:

  TVector3 mCoord;  // coordinates of vertex
  TVector3 mError;  // errors of vertex coordinates

public:

  CbmStsVertex();                                      // default constructor
  CbmStsVertex(Double_t pos[3]);                       // constructor from array of doubles
  CbmStsVertex(Double_t x, Double_t y, Double_t z);    // constructor from doubles
  virtual  ~CbmStsVertex();                            // destructor

  // getters
  Double_t GetX()     const { return mCoord.X(); }
  Double_t GetY()     const { return mCoord.Y(); }
  Double_t GetZ()     const { return mCoord.Z(); }
  Double_t GetXerr()  const { return mError.X(); }
  Double_t GetYerr()  const { return mError.Y(); }
  Double_t GetZerr()  const { return mError.Z(); }
  TVector3 GetCoord() const { return mCoord;}
  TVector3 GetError() const { return mError;}

  // setters
  void SetX(Double_t f)    { mCoord.SetX(f); }
  void SetY(Double_t f)    { mCoord.SetY(f); }
  void SetZ(Double_t f)    { mCoord.SetZ(f); }
  void SetXerr(Double_t f) { mError.SetX(f); }
  void SetYerr(Double_t f) { mError.SetY(f); }
  void SetZerr(Double_t f) { mError.SetZ(f); }
  
  ClassDef(CbmStsVertex, 1)   // Cbm STS vertex class
};

#endif
  
