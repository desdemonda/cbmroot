// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                          CbmROCUnpackHodo                         -----
// -----                    Created 14.12.2014 by F. Uhlig                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef CBMROCUNPACKHODO_H
#define CBMROCUNPACKHODO_H

#include "roc/Message.h"

#include "CbmROCUnpack.h"
#include "CbmHistManager.h"

class CbmTbDaqBuffer;
class CbmDaqMap;
class CbmSourceLmdNew;
class TClonesArray;

class CbmROCUnpackHodo : public CbmROCUnpack
{
 public:
  
  CbmROCUnpackHodo();
  virtual ~CbmROCUnpackHodo();
    
  virtual Bool_t Init();
  virtual Bool_t DoUnpack(roc::Message* Message, ULong_t hitTime);
  virtual void FillOutput(CbmDigi* digi);
  virtual void Reset();
  virtual void Finish();

 private:

  CbmTbDaqBuffer* fBuffer;        ///< Digi buffer instance
  CbmDaqMap* fDaqMap;             ///< Mapping from electronics to detectors
  CbmSourceLmdNew* fSource;
  TClonesArray* fHodoDigis;         ///< Output array of CbmHodoDigi
  TClonesArray* fHodoBaselineDigis; ///< Output array for baseline calib.
  CbmHistManager* fHM;  ///< Histogram manager

  void CreateHistograms();

  ClassDef(CbmROCUnpackHodo, 1)
};

#endif
