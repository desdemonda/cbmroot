// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           CbmTbEventBuilder                       -----
// -----                    Created 14.12.2014 by F. Uhlig                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef CBMTBEVENTBUILDER_H
#define CBMTBEVENTBUILDER_H

#include "TObject.h"

class CbmDigi;
class CbmTbEvent;

class CbmTbEventBuilder : public TObject
{
 public:
  
  CbmTbEventBuilder();
  virtual ~CbmTbEventBuilder();
    
  virtual Bool_t Init() = 0;
  virtual Bool_t IsInEvent(CbmDigi* digi) = 0;
  virtual void Reset() = 0;
  virtual void PrintCurrentEvent() = 0;
 protected:
   // --- Event status
   CbmTbEvent* fCurrentEvent;    ///< Pointer to current event
        
   ClassDef(CbmTbEventBuilder, 0)
};

#endif
