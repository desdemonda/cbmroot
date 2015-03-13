// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                          CbmTSUnpackDummy                         -----
// -----                    Created 21.11.2014 by F. Uhlig                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef CBMTSUNPACKDUMMY_H
#define CBMTSUNPACKDUMMY_H

#ifndef __CINT__
  #include "Timeslice.hpp"
#endif


#include "CbmTSUnpack.h"

class CbmTSUnpackDummy : public CbmTSUnpack
{
 public:
  
  CbmTSUnpackDummy();
  virtual ~CbmTSUnpackDummy();
    
  virtual Bool_t Init() { return kTRUE; }
#ifndef __CINT__
  virtual Bool_t DoUnpack(const fles::Timeslice& ts, size_t component) 
  { return kTRUE; }
#endif
  virtual void Reset() { return; }

  virtual void Finish() { return; }

  ClassDef(CbmTSUnpackDummy, 1)
};

#endif
