// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                       CbmROCUnpackSystemMessage                   -----
// -----                    Created 10.11.2014 by F. Uhlig                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef CBMROCUNPACKSYSTEMMESSAGE_H
#define CBMROCUNPACKSYSTEMMESSAGE_H

#include "roc/Message.h"

#include "CbmROCUnpack.h"

#include <set>

class CbmTbDaqBuffer;
class CbmSourceLmdNew;
class CbmROCUnpackSystemMessage : public CbmROCUnpack
{
 public:
  
  CbmROCUnpackSystemMessage();
  virtual ~CbmROCUnpackSystemMessage();
    
  virtual Bool_t Init();
  virtual Bool_t DoUnpack(roc::Message* Message, ULong_t hitTime);
  virtual void FillOutput(CbmDigi* digi) {;}
  virtual void Reset();
  virtual void Finish() {;}
  
 private:

  CbmTbDaqBuffer* fBuffer;
  CbmSourceLmdNew* fSource;

  ClassDef(CbmROCUnpackSystemMessage, 1)
};

#endif
