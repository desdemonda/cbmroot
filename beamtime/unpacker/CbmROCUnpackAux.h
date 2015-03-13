// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                            CbmROCUnpackAux                        -----
// -----                    Created 14.12.2014 by F. Uhlig                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef CBMROCUNPACKAUX_H
#define CBMROCUNPACKAUX_H

#include "roc/Message.h"

#include "CbmROCUnpack.h"

class CbmSourceLmdNew;
class CbmTbDaqBuffer;
class TClonesArray;

class CbmROCUnpackAux : public CbmROCUnpack
{
 public:
  
  CbmROCUnpackAux();
  virtual ~CbmROCUnpackAux();
    
  virtual Bool_t Init();
  virtual Bool_t DoUnpack(roc::Message* Message, ULong_t hitTime);
  virtual void FillOutput(CbmDigi* digi);
  virtual void Reset();
  virtual void Finish() {;}

 private:

  CbmSourceLmdNew* fSource;
  CbmTbDaqBuffer* fBuffer;
  TClonesArray* fAuxDigis;          ///< Output array of CbmAuxDigi

  ClassDef(CbmROCUnpackAux, 1)
};

#endif
