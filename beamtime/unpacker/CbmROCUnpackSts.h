// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           CbmROCUnpackSts                         -----
// -----                    Created 10.11.2014 by F. Uhlig                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef CBMROCUNPACKSTS_H
#define CBMROCUNPACKSTS_H

#include "roc/Message.h"

#include "CbmROCUnpack.h"

class CbmTbDaqBuffer;
class CbmDaqMap;
class CbmSourceLmdNew;
class TClonesArray;

class CbmROCUnpackSts : public CbmROCUnpack
{
 public:
  
  CbmROCUnpackSts();
  virtual ~CbmROCUnpackSts();
    
  virtual Bool_t Init();
  virtual Bool_t DoUnpack(roc::Message* Message, ULong_t hitTime);
  virtual void FillOutput(CbmDigi* digi);
  virtual void Reset();
  virtual void Finish() {;}
 private:

  CbmTbDaqBuffer* fBuffer;        ///< Digi buffer instance
  CbmDaqMap* fDaqMap;             ///< Mapping from electronics to detectors
  CbmSourceLmdNew* fSource;
  TClonesArray* fStsDigis;          ///< Output array of CbmStsDigi
  TClonesArray* fStsBaselineDigis;  ///< Output array for baseline calib.

  ClassDef(CbmROCUnpackSts, 1)
};

#endif
