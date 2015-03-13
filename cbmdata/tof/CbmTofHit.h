/**
 * \file CbmTofHit.h
 * \author E. Cordier
 * Modified by D. Gonzalez-Diaz 06/09/06
 * Modified by A.Lebedev 26/05/09
 * Modified by nh 16/12/12
 * Modified by A.Lebedev 15/05/13
**/

#ifndef CBMTOFHIT_H_
#define CBMTOFHIT_H_

#include "CbmPixelHit.h"
#include "TMath.h"

class CbmTofHit : public CbmPixelHit
{
public:
  /**
   * \brief Default constructor.
   **/
  CbmTofHit();

  /**
   * \brief Constructor with hit parameters (1a).
   **/
  CbmTofHit(Int_t address, TVector3 pos, TVector3 dpos, Int_t refIndex, Double_t tof, Int_t flag, Int_t channel);

  /**
   * \brief Constructor with hit parameters (1).
   **/
  CbmTofHit(Int_t address, TVector3 pos, TVector3 dpos, Int_t refIndex, Double_t tof, Int_t flag);

  /**
   * \brief Constructor with hit parameters (2) [not the flag]
   **/
  CbmTofHit(Int_t address, TVector3 pos, TVector3 dpos, Int_t refIndex, Double_t tof);

  /**
   * \brief Destructor.
   **/
  virtual ~CbmTofHit();

  /**
   * \brief Inherited from CbmBaseHit.
   */
  virtual string ToString() const;

  /**
   * \brief Inherited from CbmBaseHit.
   */
  Int_t GetPlaneId() const { return 0; }

  /** Accessors **/
  Double_t GetTime() const { return fTime; }
  Int_t GetFlag() const { return fFlag; }
  Int_t GetCh() const { return fChannel; }

  Double_t GetR()      const {return TMath::Sqrt(GetX()*GetX()+GetY()*GetY()+GetZ()*GetZ());}
  Double_t GetRt()     const {return TMath::Sqrt(GetX()*GetX()+GetY()*GetY());}
  Double_t GetCosThe() const {return GetZ()/GetR();}
  Double_t GetSinThe() const {return TMath::Sqrt(GetX()*GetX()+GetY()*GetY())/GetR();}
  Double_t GetCosPhi() const {return GetX()/GetRt();}
  Double_t GetSinPhi() const {return GetY()/GetRt();}

  /** Modifiers **/
  void SetTime(Double_t time) { fTime = time; };
  void SetFlag(Int_t flag) { fFlag = flag; };

private:

  Double32_t fTime; ///< Time since event start [ns]
  Int_t fFlag; ///< Flag for general purposes [TDC, event tagging...]
  Int_t fChannel; ///< Channel identifier

  ClassDef(CbmTofHit, 2)
};

#endif
