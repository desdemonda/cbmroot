// -------------------------------------------------------------------------
// -----                      CbmTrdDigiDraw header file                  -----
// -----          Created 10/12/07  by M. Al-Turany                    -----
// -------------------------------------------------------------------------


/** CbmTrdDigiDraw         
 * @author M. Al-Turany
 * @since 03.01.08
 *   Task to display Hits 
 **
 **/

#ifndef CBMTRDDIGIDRAW_H
#define CBMTRDDIGIDRAW_H

#include "FairTask.h"                   // for FairTask, InitStatus
#include "Rtypes.h"                     // for Bool_t, Int_t, etc

class CbmTrdDigiPar;
class CbmTrdModule;
class FairEventManager;

class TClonesArray;
class TEveBoxSet;

class CbmTrdDigiDraw : public FairTask {

 public:

  /** Default constructor **/
  CbmTrdDigiDraw();


  /** Standard constructor 
  *@param name        Name of task
  *@param iVerbose    Verbosity level
  **/
  CbmTrdDigiDraw(const char* name, Color_t color ,Style_t mstyle, Int_t iVerbose = 1);

  /** Destructor **/
  virtual ~CbmTrdDigiDraw();

  /** Set verbosity level. For this task and all of the subtasks. **/
  void SetVerbose(Int_t iVerbose);
   /** Executed task **/ 
  virtual void Exec(Option_t* option);
  void Reset();

void SetActiveLayer(Bool_t Layer1=kTRUE, Bool_t Layer2=kFALSE,
		    Bool_t Layer3=kFALSE, Bool_t Layer4=kFALSE,
		    Bool_t Layer5=kFALSE, Bool_t Layer6=kFALSE,
		    Bool_t Layer7=kFALSE, Bool_t Layer8=kFALSE,
		    Bool_t Layer9=kFALSE, Bool_t Layer10=kFALSE,
		    Bool_t Layer11=kFALSE, Bool_t Layer12=kFALSE,
		    Bool_t Layer13=kFALSE, Bool_t Layer14=kFALSE,
		    Bool_t Layer15=kFALSE);

protected:

  Int_t   fVerbose;       //  Verbosity level
  virtual void SetParContainers() ;
  virtual InitStatus Init();
  /** Action after each event**/
  virtual void Finish() ;
  TClonesArray *fPointList; //!
  CbmTrdDigiPar *fDigiPar; //!
  CbmTrdModule *fModuleInfo; //!
  FairEventManager *fEventManager;   //!
  TEveBoxSet* fq;    //!
  Color_t fColor; //!
  Style_t fStyle; //!

  Bool_t fActiveLayers[15];

  CbmTrdDigiDraw(const CbmTrdDigiDraw&);
  CbmTrdDigiDraw& operator=(const CbmTrdDigiDraw&);

  ClassDef(CbmTrdDigiDraw,2);
    
};

                            
#endif
