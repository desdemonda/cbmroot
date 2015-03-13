// Returns X for given assymetry/energy.
// Data taken from root file

/* $Id: CbmEcalSCurveLibRecord.h,v 1.1 2006/09/18 07:58:04 prokudin Exp $ */
/*
 * $Log: CbmEcalSCurveLibRecord.h,v $
 * Revision 1.1  2006/09/18 07:58:04  prokudin
 * First implementation of SCurve library
 *
 */

#ifndef CBMECALSCURVELIBRECORD_H
#define CBMECALSCURVELIBRECORD_H

#include "TObject.h"

class CbmEcalSCurveLibRecord
{
public:
  CbmEcalSCurveLibRecord(const char* filename);
  //returns x coordinate for given assymetry and theta
  //-1111 if retrival not possible
  //e --- an incoming particle energy
  Float_t GetX(Float_t a, Float_t e, Float_t theta) const;
  Float_t GetCellSize() const;
  virtual ~CbmEcalSCurveLibRecord();
private:
  Int_t GetNum(Float_t energy, Float_t theta) const;
  void InitA();
  /** CellSize **/
  Float_t fCellSize;
  /** Number of records **/
  Int_t fPoints;
  /** Number of energies **/
  Int_t fEnergies;
  /** List of energies **/
  Float_t* fE;		//!
  /** Number of thetas **/
  Int_t fThetas;
  /** List of thetas **/
  Float_t* fTheta;	//!
  /** Assymetry **/
  Float_t* fA;		//!
  /** Correspondig X **/
  Float_t** fX;		//!
  /** Number of [A,X] pairs for each record **/
  Int_t fSize;

  Int_t fTail;

  Float_t* fXL;		//!
  Float_t* fDXL;	//!
  Float_t* fXR;		//!
  Float_t* fDXR;	//!

  CbmEcalSCurveLibRecord(const CbmEcalSCurveLibRecord&);
  CbmEcalSCurveLibRecord& operator=(const CbmEcalSCurveLibRecord&);

  ClassDef(CbmEcalSCurveLibRecord, 1)
};

inline Float_t CbmEcalSCurveLibRecord::GetCellSize() const
{
  return fCellSize;
}

#endif
