// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                   CbmTbEventBuilderFixedTimeWindow                -----
// -----                    Created 14.12.2014 by F. Uhlig                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef CBMTBEVENTBUILDERFIXEDTIMEWINDOW_H
#define CBMTBEVENTBUILDERFIXEDTIMEWINDOW_H

#include "CbmTbEventBuilder.h"
#include "CbmTbEvent.h"

class CbmDigi;

class CbmTbEventBuilderFixedTimeWindow : public CbmTbEventBuilder
{
 public:
  
  CbmTbEventBuilderFixedTimeWindow();
  virtual ~CbmTbEventBuilderFixedTimeWindow();
    
  virtual Bool_t Init();
  virtual Bool_t IsInEvent(CbmDigi* digi);
  virtual void Reset();
  virtual void PrintCurrentEvent()
  { fCurrentEvent->Print(); }


  void SetEventTimeWindow(Double_t window) { fEventTimeWindow = window; }

 private:
        
  Double_t fEventTimeWindow;
  
  ClassDef(CbmTbEventBuilderFixedTimeWindow, 1)
};

#endif
