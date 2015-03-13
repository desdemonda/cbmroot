// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                          CbmROCUnpackEpoch                        -----
// -----                    Created 13.12.2014 by F. Uhlig                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef CBMROCUNPACKEPOCH_H
#define CBMROCUNPACKEPOCH_H

#include "roc/Message.h"

#include "CbmROCUnpack.h"

class CbmSourceLmdNew;

class CbmROCUnpackEpoch : public CbmROCUnpack
{
 public:
  
  CbmROCUnpackEpoch();
  virtual ~CbmROCUnpackEpoch();
    
  virtual Bool_t Init();
  virtual Bool_t DoUnpack(roc::Message* Message, ULong_t hitTime);
  virtual void FillOutput(CbmDigi* digi) {;}
  virtual void Reset();
  virtual void Finish() {;}

 private:

  CbmSourceLmdNew* fSource;
  Int_t fNofEpochs;

  ClassDef(CbmROCUnpackEpoch, 1)
};

#endif
