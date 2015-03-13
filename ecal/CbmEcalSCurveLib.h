// Returns X for given assymetry/energy.
// Data taken from root files

/* $Id: CbmEcalSCurveLib.h,v 1.1 2006/09/18 07:58:04 prokudin Exp $ */
/*
 * $Log: CbmEcalSCurveLib.h,v $
 * Revision 1.1  2006/09/18 07:58:04  prokudin
 * First implementation of SCurve library
 *
 */

#ifndef CBMECALSCURVELIB_H
#define CBMECALSCURVELIB_H

#include "FairTask.h"
#include "TString.h"

#include <list>

class CbmEcalSCurveLibRecord;
class CbmEcalCell;

class CbmEcalSCurveLib : public FairTask
{
public:
  CbmEcalSCurveLib(const char* name, Int_t verbose=0);
  void AddFile(const char* file);
  // a is assymetry, e --- energy of incoming particle
  void GetCoord(Float_t e, CbmEcalCell* cell, Float_t& x, Float_t& y) const;
  Float_t GetX(Float_t a, Float_t e, CbmEcalCell* cell) const;
  Float_t GetY(Float_t a, Float_t e, CbmEcalCell* cell) const;
  Float_t GetX(Float_t a, Float_t e, Float_t theta, Int_t type) const;
  InitStatus Init();
  void Exec(Option_t* exec);
  void Finish();
  virtual ~CbmEcalSCurveLib();

  /** Set module size, mostly for debuging
   ** Don't use it, ModuleSize will be inited automaticly
   ** via CbmEcalInf container **/
  void SetModuleSize(Float_t msize);
private:
  void Add(const char* file);

  std::list<TString> fFiles;
  /** if fModuleSize==0, load it from CbmEcalInf **/
  void GetModuleSize();
  /** A verbose flag **/
  Int_t fVerbose;
  Float_t fModuleSize;

  /** Size of fRec array **/
  Int_t fSize;
  CbmEcalSCurveLibRecord** fRec; //!

  CbmEcalSCurveLib(const CbmEcalSCurveLib&);
  CbmEcalSCurveLib& operator=(const CbmEcalSCurveLib&);

  ClassDef(CbmEcalSCurveLib, 1)
};

inline void CbmEcalSCurveLib::SetModuleSize(Float_t msize)
{
  fModuleSize=msize;
}
#endif
