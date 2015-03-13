/***********************************************************************************
 *  $Id: CbmJpsiTrigger.h 1536 2007-09-27 09:41:06Z uhlig $
 *
 ***********************************************************************************/

#ifndef CBM_JPSITRIGGER_H
#define CBM_JPSITRIGGER_H

#include "FairTask.h"
#include "FairRootManager.h"
#include "CbmJpsiTriggerElectron.h"
#include "CbmJpsiTrackCollection.h"
#include "TStopwatch.h"

class TClonesArray;
//class CbmRootManager;
class TFile;

class CbmJpsiTrigger : public FairTask{



public:

    CbmJpsiTrigger();

    CbmJpsiTrigger(Int_t iVerbose, TString fname, const char* name = "JpsiTrigger");

    virtual ~CbmJpsiTrigger();

    virtual InitStatus Init();

    virtual void Exec(Option_t* option);

    virtual void Finish() {};

    virtual void FinishTask() {};

    void SetCuts(Double_t cutPt=0 );  //! set cut values for chi2primary, radial and sts acceptance

private:

   FairRootManager* fRootManager;
   //   TClonesArray*   fMCStack;
   TClonesArray*   fStsTrack;
   TClonesArray*   fGlobalTrack;
   TClonesArray*   fTrdTrack;

   TClonesArray*   fJpsiEl; 
   CbmJpsiTrackCollection *fJpsiColl; 
   //  CbmJpsiTriggerElectron *fJpsiColl; 
   Double_t fCutPt;
   Int_t fVerbose;

   TFile* fOutFile;
   TString fOutFileName;

    TStopwatch fTimer;                     //! Timer
   Int_t      fNEvents;                   //! Number of events
   Int_t fElectrons;
 
    ClassDef(CbmJpsiTrigger,1);
};

#endif
