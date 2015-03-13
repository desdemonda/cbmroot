// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                              CbmTSUnpack                          -----
// -----                    Created 07.11.2014 by F. Uhlig                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef CBMTSUNPACK_H
#define CBMTSUNPACK_H

#ifndef __CINT__
  #include "Timeslice.hpp"
#endif


#include "TObject.h"

class CbmTSUnpack : public TObject
{
 public:
  
  CbmTSUnpack();
  virtual ~CbmTSUnpack();
    
  virtual Bool_t Init() = 0;
#ifndef __CINT__
  virtual Bool_t DoUnpack(const fles::Timeslice& ts, size_t component) = 0;
#endif
  virtual void Reset() = 0;

  virtual void Finish() = 0;
  // protected:
  //  virtual void Register() = 0;

  ClassDef(CbmTSUnpack, 0)
};

#endif
