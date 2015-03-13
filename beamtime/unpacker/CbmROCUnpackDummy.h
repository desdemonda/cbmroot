// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                          CbmROCUnpackDummy                        -----
// -----                    Created 14.12.2014 by F. Uhlig                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef CBMROCUNPACKDUMMY_H
#define CBMROCUNPACKDUMMY_H

#include "roc/Message.h"

#include "CbmROCUnpack.h"

class CbmROCUnpackDummy : public CbmROCUnpack
{
 public:
  
  CbmROCUnpackDummy();
  virtual ~CbmROCUnpackDummy();
    
  virtual Bool_t Init();
  virtual Bool_t DoUnpack(roc::Message* Message, ULong_t hitTime);
  virtual void FillOutput(CbmDigi* digi) {;}
  virtual void Reset();
  virtual void Finish() {;}

 private:

  ClassDef(CbmROCUnpackDummy, 1)
};

#endif
