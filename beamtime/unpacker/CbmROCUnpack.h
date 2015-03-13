// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                             CbmROCUnpack                          -----
// -----                    Created 10.11.2014 by F. Uhlig                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef CBMROCUNPACK_H
#define CBMROCUNPACK_H

#include "roc/Message.h"

#include "TObject.h"

class CbmSourceLmdNew;
class CbmDigi;

class CbmROCUnpack : public TObject
{
 public:
  
  CbmROCUnpack();
  virtual ~CbmROCUnpack();
    
  virtual Bool_t Init() = 0;
  virtual Bool_t DoUnpack(roc::Message* Message, ULong_t hitTime) = 0;
  virtual void FillOutput(CbmDigi* digi) = 0;
  virtual void Reset() = 0;
  virtual void Finish() = 0;

  void SetPersistence(Bool_t persistence = kTRUE) 
  { fPersistence = persistence; }

 protected:

  Bool_t  fPersistence;

  ClassDef(CbmROCUnpack, 0)
};

#endif
