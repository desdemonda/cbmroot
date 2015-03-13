// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                          CbmROCUnpackMuch                         -----
// -----                    Created 14.12.2014 by F. Uhlig                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef CBMROCUNPACKMUCH_H
#define CBMROCUNPACKMUCH_H

#include "roc/Message.h"

#include "CbmROCUnpack.h"

class CbmTbDaqBuffer;
class CbmDaqMap;
class CbmSourceLmdNew;
class TClonesArray;

class CbmROCUnpackMuch : public CbmROCUnpack
{
 public:
  
  CbmROCUnpackMuch();
  virtual ~CbmROCUnpackMuch();
    
  virtual Bool_t Init();
  virtual Bool_t DoUnpack(roc::Message* Message, ULong_t hitTime);
  virtual void FillOutput(CbmDigi* digi);
  virtual void Reset();
  virtual void Finish() {;}

 private:

  CbmTbDaqBuffer* fBuffer;        ///< Digi buffer instance
  CbmDaqMap* fDaqMap;             ///< Mapping from electronics to detectors
  CbmSourceLmdNew* fSource;
  TClonesArray* fMuchDigis;         ///< Output array of CbmMuchDigi
  TClonesArray* fMuchBaselineDigis; ///< Output array for baseline calib.

  ClassDef(CbmROCUnpackMuch, 1)
};

#endif
