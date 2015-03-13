///////////////////////////////////////////////////////////////////////////////////////
//                                                                                   //
//     CbmStsCMPoint class -  conformal mapping class for points                     //
//                                                                                   //
///////////////////////////////////////////////////////////////////////////////////////

#ifndef CBM_STS_CM_POINT_H
#define CBM_STS_CM_POINT_H

#include "CbmStsHit.h"
#include "CbmStsVertex.h"

class CbmStsCMPoint : public CbmStsHit {
  
private:
  
  // Interaction point
  Double_t   fXt;          // x-value of the interaction point
  Double_t   fYt;          // y-value of the interaction point
  Double_t   fZt;          // z-value of the interaction point
  
  Double_t   fXterr;       // error of fXt
  Double_t   fYterr;       // error of fYt
  Double_t   fZterr;       // error of fZt
  
  // conformal mapping coordinates
  Double_t   fXprime;      // transformed x
  Double_t   fZprime;      // transformed y  
  
  Double_t   fXprimeerr;   // error of fXprime
  Double_t   fZprimeerr;   // error of fYprime
  
  // coordinates with respect to the vertex
  
  // cartesian coordinates
  Double_t   fXv;          // x with respect to vertex
  Double_t   fYv;          // y with respect to vertex
  Double_t   fZv;          // z with respect to vertex
  
  Double_t   fXverr;       // error of fXv
  Double_t   fYverr;       // error of fYv
  Double_t   fZverr;       // error of fZv
  
  // spherical coordinates
  Double_t   fPhi;         // angle phi
  Double_t   fEta;         // angle eta
  
  // angles
  Double_t   fAlpha;         // deep angle
  Double_t   fBeta;          // angle beta
  
  // distances to the conformal mapping fits
  Double_t   fCircleDist;  // distance from the fitted circle
  Double_t   fDeepDist;    // distance from the line fit in YZ plane
  
  
public:
  
            CbmStsCMPoint();                   // default constructor
            CbmStsCMPoint(CbmStsHit *point);   // constructor

  virtual  ~CbmStsCMPoint();                   // destructor
  
  // getters
  Double_t   GetCircleDist() const   { return fCircleDist; }
  Double_t   GetDeepDist() const     { return fDeepDist; }
  
  Double_t   GetPhi() const          { return fPhi;        }
  Double_t   GetEta() const          { return fEta;        }
  
  Double_t   GetXprime() const       { return fXprime;     }
  Double_t   GetZprime() const       { return fZprime;     }
  Double_t   GetXprimeerr() const    { return fXprimeerr;  }
  Double_t   GetZprimeerr() const    { return fZprimeerr;  }
  
  Double_t   GetXt() const           { return fXt;         }
  Double_t   GetYt() const           { return fYt;         }
  Double_t   GetZt() const           { return fZt;         }
  Double_t   GetXterr() const        { return fXterr;      }
  Double_t   GetYterr() const        { return fYterr;      }
  Double_t   GetZterr() const        { return fZterr;      }
  
  Double_t   GetXv() const           { return fXv;         }
  Double_t   GetYv() const           { return fYv;         }
  Double_t   GetZv() const           { return fZv;         }
  Double_t   GetXverr() const        { return fXverr;      }
  Double_t   GetYverr() const        { return fYverr;      }
  Double_t   GetZverr() const        { return fZverr;      }

  Double_t   GetAlpha() const        { return fAlpha;      }
  Double_t   GetBeta() const         { return fBeta;       }
  
  // setters
  void    SetDist(Double_t c, Double_t l)  { fCircleDist = c; fDeepDist = l; }
  void    SetCircleDist(Double_t f)        { fCircleDist = f; }
  void    SetDeepDist(Double_t f)          { fDeepDist = f; }  
  
  void    SetPhi(Double_t f)         {           fPhi = f; }
  void    SetEta(Double_t f)         {           fEta = f; }
  
  void    SetXprime(Double_t f)      {        fXprime = f; }
  void    SetZprime(Double_t f)      {        fZprime = f; }
  void    SetXprimeerr(Double_t f)   {     fXprimeerr = f; }
  void    SetZprimeerr(Double_t f)   {     fZprimeerr = f; }
  
  void    SetXt(Double_t f)          {            fXt = f; }
  void    SetYt(Double_t f)          {            fYt = f; }
  void    SetZt(Double_t f)          {            fZt = f; }
  void    SetXterr(Double_t f)       {         fXterr = f; }
  void    SetYterr(Double_t f)       {         fYterr = f; }
  void    SetZterr(Double_t f)       {         fZterr = f; }
  
  void    SetXv(Double_t f)          {            fXv = f; }
  void    SetYv(Double_t f)          {            fYv = f; }
  void    SetZv(Double_t f)          {            fZv = f; }
  void    SetXverr(Double_t f)       {         fXverr = f; }
  void    SetYverr(Double_t f)       {         fYverr = f; }
  void    SetZverr(Double_t f)       {         fZverr = f; }
  void    SetAlpha(Double_t f)       {         fAlpha = f; }
  void    SetBeta(Double_t f)        {          fBeta = f; }
  
  void    Setup(CbmStsVertex *vertex);   // does the usual setup in the right order
  void    SetAngles();                   // calculate spherical angles and set values
  // set interaction point
  void    SetIntPoint(const Double_t in_x = 0., const Double_t in_y = 0.,
		      const Double_t in_z = 0., const Double_t in_x_err = 0.,
		      const Double_t in_y_err = 0., const Double_t in_z_err = 0.);
  void    SetShiftedCoord();             // set shifted coordinates  
  void    SetConfCoord();                // conformal mapping of coordinates
  void    SetAllCoord(const CbmStsCMPoint *hit);   // set conformal mapping coordinates in respect to given hit
  
  ClassDef(CbmStsCMPoint, 1)   //
};

#endif
