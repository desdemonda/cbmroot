/***********************************************************************************
 *  $Id: CbmDileptonAssignMCid.h 3856 2012-02-15 17:14:28Z f.uhlig $
 *
 *  Class : CbmDileptonAssignMCid
 *  Descripton: This is a task, derived from FairTask. This works for the simulated
 *              data stream to assign MC information to the identified electron tracks.
 *              This also generates histograms for identification QA (efficiency,
 *              purity, pion supression etc.
 *              Input: Arrays of dileptontrackreal, global, sts, trd track, rich ring,
 *              tof hit and mc arrays.
 *              Output: Array of CbmDileptonTrackSim
 *
 *  Author: Supriya Das
 *  E-mail: S.Das@gsi.de
 ***********************************************************************************/

#ifndef CBM_DILEPTON_ASSIGN_MCID_H
#define CBM_DILEPTON_ASSIGN_MCID_H

#include "FairTask.h"
#include "CbmStsKFTrackFitter.h"

#include "TStopwatch.h"

class TClonesArray;
class FairRootManager;
class CbmVertex;
class CbmDileptonTrackSimCollection;
class TH1D;
class TH2D;
class TFile;

class CbmDileptonAssignMCid : public FairTask{

private:

   FairRootManager* fRootManager;

   CbmDileptonTrackSimCollection* fTrackSimColl;
   TClonesArray*   fArrayDileptonTrackReal;
   TClonesArray*   fArrayGlobalTrack;
   TClonesArray*   fArrayStsTrack;
   TClonesArray*   fArrayStsTrackMatch;
   TClonesArray*   fArrayRichRing;
   TClonesArray*   fArrayRichRingMatch;
   TClonesArray*   fArrayTrdTrack;
   TClonesArray*   fArrayTofHit;
   TClonesArray*   fArrayTofPoint;
   TClonesArray*   fArrayRichProjection;
   TClonesArray*   fArrayMCTrack;

   CbmStsKFTrackFitter fFitter;
   CbmVertex* fPrimVertex;

   Double_t fCutChiPrimary;
   Double_t fCutRadialDistance;
   Int_t    fCutStsHit;
  
   Int_t fVerbose;

   TFile* fOutFile;
   TString fOutFileName;

   TH1D* fh_momentum_id_rich;
   TH1D* fh_momentum_id_rich_trd;
   TH1D* fh_momentum_id_rich_tof;
   TH1D* fh_momentum_id_rich_trd_tof;

   TH1D* fh_momentum_id_rich_prim;
   TH1D* fh_momentum_id_rich_trd_prim;
   TH1D* fh_momentum_id_rich_tof_prim;
   TH1D* fh_momentum_id_rich_trd_tof_prim;

   TH1D* fh_momentum_projected_e_prim;
   TH1D* fh_momentum_projected_e;
   TH1D* fh_momentum_projected_pi;

   TH1D* fh_momentum_rich_true_e;
   TH1D* fh_momentum_rich_trd_true_e;
   TH1D* fh_momentum_rich_tof_true_e;
   TH1D* fh_momentum_rich_trd_tof_true_e;

   TH1D* fh_momentum_rich_true_e_prim;
   TH1D* fh_momentum_rich_trd_true_e_prim;
   TH1D* fh_momentum_rich_tof_true_e_prim;
   TH1D* fh_momentum_rich_trd_tof_true_e_prim;

   TH1D* fh_momentum_rich_true_pi;
   TH1D* fh_momentum_rich_trd_true_pi;
   TH1D* fh_momentum_rich_tof_true_pi;
   TH1D* fh_momentum_rich_trd_tof_true_pi;

   TH1D* fh_momentum_rich_true_p;
   TH1D* fh_momentum_rich_trd_true_p;
   TH1D* fh_momentum_rich_tof_true_p;
   TH1D* fh_momentum_rich_trd_tof_true_p;

   TH1D* fh_momentum_rich_false_e;
   TH1D* fh_momentum_rich_false_pion;
   TH1D* fh_momentum_rich_false_proton;
   TH1D* fh_momentum_rich_false_others;

   TH1D* fh_momentum_rich_trd_false_e;
   TH1D* fh_momentum_rich_trd_false_pion;
   TH1D* fh_momentum_rich_trd_false_proton;
   TH1D* fh_momentum_rich_trd_false_others;

   TH1D* fh_momentum_rich_tof_false_e;
   TH1D* fh_momentum_rich_tof_false_pion;
   TH1D* fh_momentum_rich_tof_false_proton;
   TH1D* fh_momentum_rich_tof_false_others;

   TH1D* fh_momentum_rich_trd_tof_false_e;
   TH1D* fh_momentum_rich_trd_tof_false_pion;
   TH1D* fh_momentum_rich_trd_tof_false_proton;
   TH1D* fh_momentum_rich_trd_tof_false_others;

   TH1D* fh_momentum_fake_rich;
   TH1D* fh_momentum_fake_rich_trd;
   TH1D* fh_momentum_fake_rich_tof;
   TH1D* fh_momentum_fake_rich_trd_tof;

   TStopwatch fTimer;                     //! Timer
   Int_t      fNEvents;                   //! Number of events
   Double_t   fTime;                      //! Total real time

   CbmDileptonAssignMCid(const CbmDileptonAssignMCid&);
   CbmDileptonAssignMCid& operator=(const CbmDileptonAssignMCid&);

public:

    CbmDileptonAssignMCid();

    CbmDileptonAssignMCid(Int_t iVerbose, TString fname, const char* name = "DileptonAssignMCid");

    virtual ~CbmDileptonAssignMCid();

    virtual InitStatus Init();

    virtual void Exec(Option_t* option);

    virtual void Finish();

    virtual void FinishTask();

    void SetCuts(Double_t, Double_t, Int_t);  //! set cut values for chi2primary, radial and sts acceptance
    void BookHistograms();
    void SaveHistograms();

    ClassDef(CbmDileptonAssignMCid,1);
};

#endif
