/******************************************************************************
 *  $Id: CbmDileptonAssignPid.h 3856 2012-02-15 17:14:28Z f.uhlig $
 *
 *  Class : CbmDileptonAssignPid
 *  Descripton: This is a task, derived from FairTask. This works for the real
 *              data stream to perform electron identification.
 *              Input: Arrays of global, sts, trd track, rich ring and tof hit
 *              arrays.
 *              Output: Array of CbmDileptonTrackReal
 *
 *  Author: Supriya Das
 *  E-mail: S.Das@gsi.de
 ****************************************************************************/

#ifndef CBM_DILEPTON_ASSIGN_PID_H
#define CBM_DILEPTON_ASSIGN_PID_H

#include "FairTask.h"
#include "CbmStsKFTrackFitter.h"

#include "TStopwatch.h"

class TClonesArray;
class FairRootManager;
class CbmVertex;
class CbmDileptonTrackReal;
class CbmDileptonTrackRealCollection;
class TH1D;
class TH2D;
class TFile;
class CbmDileptonAssignPid : public FairTask{

private:

    FairRootManager* fRootManager;

    CbmDileptonTrackRealCollection* fTrackRealColl;

    TClonesArray*   fArrayGlobalTrack;
    TClonesArray*   fArrayStsTrack;
    TClonesArray*   fArrayRichRing;
    TClonesArray*   fArrayTrdTrack;
    TClonesArray*   fArrayTofHit;

    CbmStsKFTrackFitter fFitter;
    CbmVertex* fPrimVertex;
 
    /** switch to select different criteria for rich id **/

    Bool_t fswitchRichMom; //! default = true (momentum dependent distant cut), =false (momentum independent)
    Bool_t fswitchRichSelection; //! default = true (Selection NN), = false (Selection 2D)

    /** cuts for RICH identification **/

    Float_t fRichDist;
    Float_t fRichNN;
    Float_t fRich2D;
    Float_t fRichRadial;
    Float_t fRichNHitMean;
    Float_t fRichNHitSigma;
    Float_t fRichRmean;
    Float_t fRichRsigma;

    /** switches to select different criteria for TRD id **/

    Bool_t fswitchMom;  //! default = true,  = false (turns off selction below mom = 1.5 GeV)
    Bool_t fswitchAcceptTrd; //! decision when there is no trd track
    Bool_t fswitchLike; //! default = false, = true (turns on selection based on Likelyhood method)
    Bool_t fswitchWkn;  //! default = false, = true (turns on selection based on Wkn method)
    Bool_t fswitchAnn;  //! default = false, = true (turns on selection based on Ann method)

    /**  cuts for TRD identification **/

    Float_t fTrdMom;
    Float_t fTrdPidLikeLow;
    Float_t fTrdPidLikeHigh;
    Float_t fTrdPidWkn;
    Float_t fTrdPidAnn;

    /** switch to select TOF pid criteria **/

    Bool_t fswitchTof;  //! default = true (momentum dependent mass cut), = false (momentum independent)
    Bool_t fswitchAcceptTof; //! decision when no Tof hit assocciated to global track

    /** TOF mass2 cut **/
    Float_t fTofMass2;

    Int_t fVerbose; //! verbosity level, =0 gives no message from exec

    TString fOutFileName;

    Int_t nElec;

    // Histograms

    TH2D* fh_rich_ring_dist_mom;          //! distance vs momentum for all rings
    TH2D* fh_rich_ring_dist_mom_after;    //! distance vs momentum for rings after cut

    TH1D* fh_rich_selection_NN;           //! distribution of neural network parameter
    TH1D* fh_rich_selection_NN_after;     //! distribution of neural network parameter after cut

    TH1D* fh_rich_selection_2D;           //! distribution of 2D selection parameter
    TH1D* fh_rich_selection_2D_after;     //! distribution of 2D selection cut after cut
   
    TH1D* fh_rich_ring_radial;           //! distribution of radial distance
    TH1D* fh_rich_ring_radial_after;     //! distribution of radial distance after cut
    TH2D* fh_rich_ring_xy;               //! XY distribution of ring centres
    TH2D* fh_rich_ring_xy_after;         //! XY distributions after radial cut

    TH2D* fh_rich_ring_hits_radius;      //! distribution of hits vs. radius
    TH2D* fh_rich_ring_hits_radius_after;//! distribution of hits vs. radius after cut
    
    TH2D* fh_rich_ring_radius_mom;       //! radius vs momentum distribution
    TH2D* fh_rich_ring_radius_mom_after; //! radius vs momentum distribution after

    TH1D* fh_trd_like_id;                //! distribution of TRD Likelyhood parameter
    TH1D* fh_trd_like_id_after;          //! distribution of TRD Likelyhood parameter
  
    TH1D* fh_trd_wkn_id;                 //! distribution of TRD Wkn parameter
    TH1D* fh_trd_wkn_id_after;           //! distribution of TRD Wkn parameter
   
    TH1D* fh_trd_ann_id;                 //! distribution of TRD Ann parameter
    TH1D* fh_trd_ann_id_after;           //! distribution of TRD Ann parameter

    TH2D* fh_tof_m2_mom;                 //! mass2 vs momentum distribution from TOF
    TH2D* fh_tof_m2_mom_after;           //! mass2 vs momentum distribution from TOF

    TFile* foutFile;

    TStopwatch fTimer;                     //! Timer
    Int_t      fNEvents;                   //! Number of events
    Double_t   fTime;                      //! Total real time

public:

    /** Default constructor **/
    CbmDileptonAssignPid();

    CbmDileptonAssignPid(Int_t iVerbose, TString fname = "PidHistos.root", const char* name = "DileptonAssignPid");

    /** Destructor **/
    virtual ~CbmDileptonAssignPid();

    /** Initialize task **/
    virtual InitStatus Init();

    /**
     * Process the input
     */
    virtual void Exec(Option_t* option);

    /**
     * Finalise the processing of the input
     */
    virtual void Finish(); //*MENU
    virtual void FinishTask();

    /** Accessors **/

    Bool_t GetRichPid(Bool_t, Bool_t, TVector3, Int_t);
    Bool_t GetTrdPid(Bool_t, Bool_t, Bool_t, Bool_t, Bool_t, Bool_t, TVector3, Int_t);
    Bool_t GetTofPid(Bool_t, Bool_t, Bool_t, TVector3, Int_t, Double_t);

    /** Modifiers **/

    void SetRichCuts(Bool_t, Bool_t, Float_t []); // switchMomentum, switchSelection, richCuts
    void SetTrdCuts(Bool_t, Bool_t, Bool_t, Bool_t, Bool_t, Float_t []); // switchMom, switchAcceptTrd, switchLike, switchWkn, switchAnn, trdCuts
    void SetTofCuts(Bool_t, Bool_t, Float_t); //switchMomentum, switchAcceptTof, tofCut

    void BookHistograms();
    void SaveHistograms();

 private:

    CbmDileptonAssignPid(const CbmDileptonAssignPid&);
    CbmDileptonAssignPid& operator=(const CbmDileptonAssignPid&);

    ClassDef(CbmDileptonAssignPid,1);
};

#endif
