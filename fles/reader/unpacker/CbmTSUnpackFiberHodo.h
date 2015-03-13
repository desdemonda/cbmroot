// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                     CbmTSUnpackFiberHodo                          -----
// -----                    Created 07.11.2014 by                          -----
// -----                   E. Ovcharenko, F. Uhlig                         -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef CBMTSUNPACKFIBERHODO_H
#define CBMTSUNPACKFIBERHODO_H

#ifndef __CINT__
  #include "Timeslice.hpp"
#endif

#include "CbmTSUnpack.h"

#include "CbmNxyterRawMessage.h"
#include "CbmFiberHodoDigi.h"

#include "TClonesArray.h"

class CbmFiberHodoMapping;
class CbmDaqBuffer;

class CbmTSUnpackFiberHodo : public CbmTSUnpack
{
 public:

  CbmTSUnpackFiberHodo();
  virtual ~CbmTSUnpackFiberHodo();

  virtual Bool_t Init();
#ifndef __CINT__
  virtual Bool_t DoUnpack(const fles::Timeslice& ts, size_t component);
#endif
  virtual void Reset();

  virtual void Finish() {;}

  // protected:
  //  virtual void Register();

 private:
  Int_t fCurrEpoch; // Current epoch (first epoch in the stream initialises the 

  TClonesArray* fFiberHodoRaw;
  TClonesArray* fFiberHodoDigi;
  CbmNxyterRawMessage* fRawMessage;  
  CbmFiberHodoDigi* fDigi;  
  CbmFiberHodoMapping* fHodoMap;
  CbmDaqBuffer* fBuffer;

  void FillHodoDigiToBuffer();

  CbmTSUnpackFiberHodo(const CbmTSUnpackFiberHodo&);
  CbmTSUnpackFiberHodo operator=(const CbmTSUnpackFiberHodo&);

  ClassDef(CbmTSUnpackFiberHodo, 1)
};

#endif
