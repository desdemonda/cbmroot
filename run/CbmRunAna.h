/** CbmRunAna.h
 *@author Volker Friese v.friese@gsi.de
 *@since 10.12.2009
 *@version 1.0
 **
 ** Run manager for reconstruction and analysis in CBM.
 ** Derives from FairRunAna, but overrides the Run() method in order to
 ** enable asynchroneous output (fill output tree on demand and not after
 ** processing one input event).
 **/


#ifndef CBMRUNANA_H
#define CBMRUNANA_H 1


#include "FairRootManager.h"
#include "FairRunAna.h"


class CbmRunAna : public FairRunAna
{

 public:

  /**   Constructor   **/
  CbmRunAna();


  /**   Destructor   **/
  virtual ~CbmRunAna();


  /**   Mark tree for filling after tasks' execution  **/
  void MarkFill(Bool_t mark = kTRUE) { fMarkFill = mark; }


  /**   Set asynchroneous output mode  **/
  void SetAsync(Bool_t async = kTRUE) { fAsync = async; }


  /**   Run all events in input file  **/
  void Run() { ExecRun(0, fRootManager->GetInChain()->GetEntries()); }


  /**   Run n events 
   **@param nEvents   Number of events to be processed  **/
  void Run(Int_t nEvents) { ExecRun(0, nEvents); }


  /**   Run from event iStart to event iStop
   **@param iStart   Number of first event
   **@param iStop    Number of last event
   **/
  void Run(Int_t iStart, Int_t iStop) { ExecRun(iStart-1, iStop); }



 private:

  /**   Run execution
   **@param iStart  Number of first event
   **@param iStop   Number of last event
   **/
  void ExecRun(Int_t iStart, Int_t iStop);


  Bool_t fAsync;       /** Flag for asynchroneous output mode **/
  Bool_t fMarkFill;    /** Flag for filling output tree at end of event
			** Only relevant in asynchroneous mode **/



  ClassDef(CbmRunAna,1);

};

#endif



  



