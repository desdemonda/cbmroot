// ------------------------------------------------------------------
// -----                     CbmHadronAnalysis                  -----
// -----              Created 14/12/2012 by N.Herrmann          -----
// ------------------------------------------------------------------
#ifndef _CBMHADRONANALYSIS_H_
#define _CBMHADRONANALYSIS_H_

#include "FairTask.h"
#include "CbmMCEventHeader.h"
#include "CbmStsKFTrackFitter.h"

class CbmTofGeoHandler;
class CbmTofCell;
class CbmMatch;
class CbmVertex;
class CbmStsKFTrackFitter;

class TClonesArray;
class TH1F;
class TH2F;
class TString;


class CbmHadronAnalysis : public FairTask {

private:
    Int_t         fEvents;            // Number of processed events

    Float_t       fBeamMomentum;      // Beam momentum
    Float_t       fMidY;              // Midrapidity 
    Float_t       fDY;                // reaction plane exclusion interval  
    Float_t       fFlowMinPtm;        // min pt/m for RP calculation   
    Float_t       fBSelMin;           // impact parameter selection  
    Float_t       fBSelMax;           // impact parameter selection  
    Float_t       fwxy2;              // normalisation increment  
    Double_t      fWMax;              // track matching weight
    Double_t      fVtxBMax;           // impact parameter cutfor primaries 

    TString       fPdfFileName;       // name of the file name with hadron PDF
    TString       fFlowFileName;      // name of the file name with Fourier corrections
    TFile*        fflowFile;          // pointer to RP corrections 

    CbmMCEventHeader *fMCEventHeader;
    CbmTofGeoHandler *fGeoHandler;
    CbmTofCell       *fCellInfo;

    TClonesArray *fMCTracks;      // MC tracks
    TClonesArray *fStsPoints;     // STS points
    TClonesArray *fStsHits;       // STS hits
    TClonesArray *fStsTracks;     // STS tracks
    TClonesArray *fTofPoints;     // TOF points
    TClonesArray *fTofHits;       // TOF hits
    TClonesArray *fTofDigis;      // TOF Digis
    TClonesArray *fTofDigiMatchColl;  // TOF Matched Digis
    TClonesArray *fTofDigiMatchPointsColl;  // TOF Matched Digi Points
    TClonesArray *fGlobalTracks;  // Global tracks
    TClonesArray *fHadrons;       // Hadrons
    CbmVertex    *fPrimVertex;        // Pointer to the primary vertex
    CbmStsKFTrackFitter fTrackFitter;// Pointer to the Kalman Filter Fitter algorithm

    TH2F *fa_ptm_rap_gen_pip;
    TH2F *fa_ptm_rap_gen_pim;
    TH2F *fa_ptm_rap_gen_kp;
    TH2F *fa_ptm_rap_gen_km;
    TH2F *fa_ptm_rap_gen_p;
    TH2F *fa_ptm_rap_gen_pbar;
    TH2F *fa_ptm_rap_gen_d;
    TH2F *fa_ptm_rap_gen_t;
    TH2F *fa_ptm_rap_gen_h;
    TH2F *fa_ptm_rap_gen_a;
    TH2F *fa_ptm_rap_gen_imf;

    TH1F *fa_plab_sts_pip;
    TH1F *fa_plab_sts_pim;
    TH1F *fa_plab_sts_kp;
    TH1F *fa_plab_sts_km;
    TH1F *fa_plab_sts_p;
    TH1F *fa_plab_sts_pbar;

    TH2F *fa_ptm_rap_sts_pip;
    TH2F *fa_ptm_rap_sts_pim;
    TH2F *fa_ptm_rap_sts_kp;
    TH2F *fa_ptm_rap_sts_km;
    TH2F *fa_ptm_rap_sts_p;
    TH2F *fa_ptm_rap_sts_pbar;
    TH2F *fa_ptm_rap_sts_d;
    TH2F *fa_ptm_rap_sts_t;
    TH2F *fa_ptm_rap_sts_h;
    TH2F *fa_ptm_rap_sts_a;
    TH2F *fa_ptm_rap_sts_imf;

    TH2F *fa_ptm_rap_poi_pip;
    TH2F *fa_ptm_rap_poi_pim;
    TH2F *fa_ptm_rap_poi_kp;
    TH2F *fa_ptm_rap_poi_km;
    TH2F *fa_ptm_rap_poi_p;
    TH2F *fa_ptm_rap_poi_pbar;
    TH2F *fa_ptm_rap_poi_d;
    TH2F *fa_ptm_rap_poi_t;
    TH2F *fa_ptm_rap_poi_h;
    TH2F *fa_ptm_rap_poi_a;
    TH2F *fa_ptm_rap_poi_imf;

    TH2F *fa_ptm_rap_hit_pip;
    TH2F *fa_ptm_rap_hit_pim;
    TH2F *fa_ptm_rap_hit_kp;
    TH2F *fa_ptm_rap_hit_km;
    TH2F *fa_ptm_rap_hit_p;
    TH2F *fa_ptm_rap_hit_pbar;
    TH2F *fa_ptm_rap_hit_d;
    TH2F *fa_ptm_rap_hit_t;
    TH2F *fa_ptm_rap_hit_h;
    TH2F *fa_ptm_rap_hit_a;
    TH2F *fa_ptm_rap_hit_imf;

    TH2F *fa_ptm_rap_glo_pip;
    TH2F *fa_ptm_rap_glo_pim;
    TH2F *fa_ptm_rap_glo_kp;
    TH2F *fa_ptm_rap_glo_km;
    TH2F *fa_ptm_rap_glo_p;
    TH2F *fa_ptm_rap_glo_pbar;
    TH2F *fa_ptm_rap_glo_d;
    TH2F *fa_ptm_rap_glo_t;
    TH2F *fa_ptm_rap_glo_h;
    TH2F *fa_ptm_rap_glo_a;
    TH2F *fa_ptm_rap_glo_imf;

    TH2F *fa_mul_b_gen;
    TH2F *fa_mul_b_poi;
    TH2F *fa_mul_b_hit;
    TH2F *fa_mul_b_glo;
    TH2F *fa_mul_b_had;

    TH2F *fa_phirp_b_gen;
    TH2F *fa_phgrp_b_gen;
    TH2F *fa_phphrp_gen;
    TH2F *fa_delrp_b_gen;
    TH2F *fa_delrp_b_poi;
    TH2F *fa_delrp_b_hit;
    TH2F *fa_delrp_b_glo;

    TH2F *fa_drp_b_gen;
    TH2F *fa_cdrp_b_gen;
    TH2F *fa_drp_b_poi;
    TH2F *fa_cdrp_b_poi;
    TH2F *fa_drp_b_hit;
    TH2F *fa_cdrp_b_hit;
    TH2F *fa_drp_b_glo;
    TH2F *fa_cdrp_b_glo;
    TH2F *fa_drp_b_had;
    TH2F *fa_cdrp_b_had;

    TH2F *fa_cdelrp_b_gen;
    TH2F *fa_cdelrp_b_poi;
    TH2F *fa_cdelrp_b_hit;
    TH2F *fa_cdelrp_b_glo;
    TH2F *fa_cdelrp_b_had;

    TH1F *fa_phirp_gen;
    TH1F *fa_phirp_poi;
    TH1F *fa_phirp_hit;
    TH1F *fa_phirp_glo;
    TH1F *fa_phirp_had;

    TH1F *fa_phirps_gen;
    TH1F *fa_phirps_poi;
    TH1F *fa_phirps_hit;
    TH1F *fa_phirps_glo;
    TH1F *fa_phirps_had;

    TH2F *fa_v1_rap_gen_pip;
    TH2F *fa_v1_rap_gen_pim;
    TH2F *fa_v1_rap_gen_kp;
    TH2F *fa_v1_rap_gen_km;
    TH2F *fa_v1_rap_gen_p;
    TH2F *fa_v1_rap_gen_pbar;
    TH2F *fa_v1_rap_gen_d;
    TH2F *fa_v1_rap_gen_t;
    TH2F *fa_v1_rap_gen_h;
    TH2F *fa_v1_rap_gen_a;
    TH2F *fa_v1_rap_gen_imf;

    TH2F *fa_v2_rap_gen_pip;
    TH2F *fa_v2_rap_gen_pim;
    TH2F *fa_v2_rap_gen_kp;
    TH2F *fa_v2_rap_gen_km;
    TH2F *fa_v2_rap_gen_p;
    TH2F *fa_v2_rap_gen_pbar;
    TH2F *fa_v2_rap_gen_d;
    TH2F *fa_v2_rap_gen_t;
    TH2F *fa_v2_rap_gen_h;
    TH2F *fa_v2_rap_gen_a;
    TH2F *fa_v2_rap_gen_imf;

    TH2F *fa_v1_rap_poi_pip;
    TH2F *fa_v1_rap_poi_pim;
    TH2F *fa_v1_rap_poi_kp;
    TH2F *fa_v1_rap_poi_km;
    TH2F *fa_v1_rap_poi_p;
    TH2F *fa_v1_rap_poi_pbar;
    TH2F *fa_v1_rap_poi_d;
    TH2F *fa_v1_rap_poi_t;
    TH2F *fa_v1_rap_poi_h;
    TH2F *fa_v1_rap_poi_a;
    TH2F *fa_v1_rap_poi_imf;

    TH2F *fa_v2_rap_poi_pip;
    TH2F *fa_v2_rap_poi_pim;
    TH2F *fa_v2_rap_poi_kp;
    TH2F *fa_v2_rap_poi_km;
    TH2F *fa_v2_rap_poi_p;
    TH2F *fa_v2_rap_poi_pbar;
    TH2F *fa_v2_rap_poi_d;
    TH2F *fa_v2_rap_poi_t;
    TH2F *fa_v2_rap_poi_h;
    TH2F *fa_v2_rap_poi_a;
    TH2F *fa_v2_rap_poi_imf;

    TH2F *fa_v1_rap_hit_pip;
    TH2F *fa_v1_rap_hit_pim;
    TH2F *fa_v1_rap_hit_kp;
    TH2F *fa_v1_rap_hit_km;
    TH2F *fa_v1_rap_hit_p;
    TH2F *fa_v1_rap_hit_pbar;
    TH2F *fa_v1_rap_hit_d;
    TH2F *fa_v1_rap_hit_t;
    TH2F *fa_v1_rap_hit_h;
    TH2F *fa_v1_rap_hit_a;
    TH2F *fa_v1_rap_hit_imf;

    TH2F *fa_v2_rap_hit_pip;
    TH2F *fa_v2_rap_hit_pim;
    TH2F *fa_v2_rap_hit_kp;
    TH2F *fa_v2_rap_hit_km;
    TH2F *fa_v2_rap_hit_p;
    TH2F *fa_v2_rap_hit_pbar;
    TH2F *fa_v2_rap_hit_d;
    TH2F *fa_v2_rap_hit_t;
    TH2F *fa_v2_rap_hit_h;
    TH2F *fa_v2_rap_hit_a;
    TH2F *fa_v2_rap_hit_imf;

    TH2F *fa_v1_rap_glo_pip;
    TH2F *fa_v1_rap_glo_pim;
    TH2F *fa_v1_rap_glo_kp;
    TH2F *fa_v1_rap_glo_km;
    TH2F *fa_v1_rap_glo_p;
    TH2F *fa_v1_rap_glo_pbar;
    TH2F *fa_v1_rap_glo_d;
    TH2F *fa_v1_rap_glo_t;
    TH2F *fa_v1_rap_glo_h;
    TH2F *fa_v1_rap_glo_a;
    TH2F *fa_v1_rap_glo_imf;

    TH2F *fa_v2_rap_glo_pip;
    TH2F *fa_v2_rap_glo_pim;
    TH2F *fa_v2_rap_glo_kp;
    TH2F *fa_v2_rap_glo_km;
    TH2F *fa_v2_rap_glo_p;
    TH2F *fa_v2_rap_glo_pbar;
    TH2F *fa_v2_rap_glo_d;
    TH2F *fa_v2_rap_glo_t;
    TH2F *fa_v2_rap_glo_h;
    TH2F *fa_v2_rap_glo_a;
    TH2F *fa_v2_rap_glo_imf;

    // xy hit density
    TH2F* fa_xy_poi1;
    TH2F* fa_xy_poi2;
    TH2F* fa_xy_poi3;

    TH2F* fa_xy_hit1;
    TH2F* fa_xy_hit2;
    TH2F* fa_xy_hit3;

    TH2F* fa_xy_glo1;
    TH2F* fa_xy_glo_pip;
    TH2F* fa_xy_glo_pim;
    TH2F* fa_xy_glo_kp;
    TH2F* fa_xy_glo_km;
    TH2F* fa_xy_glo_p;
    TH2F* fa_xy_glo_pbar;
    TH2F* fa_xy_glo_d;
    TH2F* fa_xy_glo_t;
    TH2F* fa_xy_glo_h;
    TH2F* fa_xy_glo_a;

    // point debugging 
    TH2F* fa_pv_poi;
    TH2F* fa_tm_poi;
    TH2F* fa_tm_poiprim;

    // hit debugging 
    TH2F* fa_dxx;
    TH2F* fa_dxy;
    TH2F* fa_dxz;
    TH2F* fa_dyx;
    TH2F* fa_dyy;
    TH2F* fa_dyz;
    TH2F* fa_dzx;
    TH2F* fa_dzy;
    TH2F* fa_dzz;

    TH1F* fa_hit_ch;
    TH2F* fa_dhit_ch;

    TH1F* fa_tof_hit;
    TH1F* fa_dtof_hit;
    TH1F* fa_tof_hitprim;
    TH2F* fa_pv_hit;
    TH2F* fa_tm_hit;
    TH2F* fa_tm_hitprim;
    TH1F* fa_tn_hit;
    TH1F* fa_t0_hit;
    TH1F* fa_t0m_hit;
    TH1F* fa_t0mn_hit;
    TH2F* fa_t0m_b_hit;
    TH2F* fa_t0mn_b_hit;

    TH1F* fa_t0m_f_hit;
    TH1F* fa_t0mn_f_hit;
    TH2F* fa_t0m_f_b_hit;
    TH2F* fa_t0mn_f_b_hit;

    TH1F* fa_t0m_nf_hit;
    TH1F* fa_t0mn_nf_hit;
    TH2F* fa_t0m_nf_b_hit;
    TH2F* fa_t0mn_nf_b_hit;

    //GlobalTrack performance 
    TH1F* fa_TofTrackMul;
    TH1F* fa_VtxB;
    TH2F* fa_chi2_mom_glo;
    TH2F* fa_chi2_mom_gloprim;
    TH2F* fa_len_mom_glo;
    TH2F* fa_pv_glo;
    TH2F* fa_tm_glo;       
    TH2F* fa_tm_glo_pip;       
    TH2F* fa_tm_glo_pim;       
    TH2F* fa_tm_glo_kp;       
    TH2F* fa_tm_glo_km;       
    TH2F* fa_tm_glo_p;       
    TH2F* fa_tm_glo_pbar;       
    TH2F* fa_tm_glo_d;       
    TH2F* fa_tm_glo_t;       
    TH2F* fa_tm_glo_h;       
    TH2F* fa_tm_glo_a;  

    TH2F* fa_tm_gloprim;
    TH2F* fa_tm_glomis;
    TH2F* fa_tm_glovtxb;
    TH2F* fa_tm_gloprimvtxb;

    TH2F* fa_m2mom_glo;       
    TH2F* fa_m2mom_glovtxb; 
    TH2F* fa_m2mom_gloprim;       
    TH2F* fa_m2mom_gloprimvtxb;             
    TH2F* fa_m2mom_glo_pip;       
    TH2F* fa_m2mom_glo_pim;       
    TH2F* fa_m2mom_glo_kp;       
    TH2F* fa_m2mom_glo_km;       
    TH2F* fa_m2mom_glo_p;       
    TH2F* fa_m2mom_glo_pbar;       
    TH2F* fa_m2mom_glo_d;       
    TH2F* fa_m2mom_glo_t;       
    TH2F* fa_m2mom_glo_h;       
    TH2F* fa_m2mom_glo_a;  

    TH2F* fa_pMCmom_glo;       
    TH2F* fa_pMCmom_glo_pip;       
    TH2F* fa_pMCmom_glo_pim;       
    TH2F* fa_pMCmom_glo_kp;       
    TH2F* fa_pMCmom_glo_km;       
    TH2F* fa_pMCmom_glo_p;       
    TH2F* fa_pMCmom_glo_pbar;       
    TH2F* fa_pMCmom_glo_d;       
    TH2F* fa_pMCmom_glo_t;       
    TH2F* fa_pMCmom_glo_h;       
    TH2F* fa_pMCmom_glo_a;  
     
    TH2F* fa_w_mom_glo;       
    TH2F* fa_w_mom_glo_pip;       
    TH2F* fa_w_mom_glo_pim;       
    TH2F* fa_w_mom_glo_kp;       
    TH2F* fa_w_mom_glo_km;       
    TH2F* fa_w_mom_glo_p;       
    TH2F* fa_w_mom_glo_pbar;       
    TH2F* fa_w_mom_glo_d;       
    TH2F* fa_w_mom_glo_t;       
    TH2F* fa_w_mom_glo_h;       
    TH2F* fa_w_mom_glo_a;       
    TH2F* fa_w_mom_gloprim;
    TH2F* fa_w_mom_glomis;

    TH2F* fa_LenDismom_glo;       
    TH2F* fa_LenDismom_glo_pip;       
    TH2F* fa_LenDismom_glo_pim;       
    TH2F* fa_LenDismom_glo_kp;       
    TH2F* fa_LenDismom_glo_km;       
    TH2F* fa_LenDismom_glo_p;       
    TH2F* fa_LenDismom_glo_pbar;       
    TH2F* fa_LenDismom_glo_d;       
    TH2F* fa_LenDismom_glo_t;       
    TH2F* fa_LenDismom_glo_h;       
    TH2F* fa_LenDismom_glo_a; 

    TH2F* fhwdist;
    TH2F* fhwmindelmass;
    TH2F* fhwminlen;
    TH2F* fhwdelp;

    TH1F* fhTofTrkDx;
    TH1F* fhTofTrkDy;
    TH1F* fhTofTrkDxsel;
    TH1F* fhTofTrkDysel;

    void CreateHistogramms();
    InitStatus ReadPdfFile();
    InitStatus ReadFlowFile();

    CbmHadronAnalysis(const CbmHadronAnalysis&);
    CbmHadronAnalysis operator=(const CbmHadronAnalysis&);

public:
    CbmHadronAnalysis();
    CbmHadronAnalysis(const char* name, Int_t verbose = 1);
    virtual ~CbmHadronAnalysis();

    virtual InitStatus Init();
    virtual void Exec(Option_t* option);
    virtual void Finish();

    void WriteHistogramms();

    inline void SetBeamMomentum(Float_t bm)  { fBeamMomentum = bm;          }
    inline void SetMidY(Float_t midY)        { fMidY = midY;          }
    inline void SetDY(Float_t DY)            { fDY = DY;          }
    inline void SetFlowMinPtm(Float_t mptm)  { fFlowMinPtm = mptm;}
    inline void SetBSelMin(Float_t b)        { fBSelMin=b; }
    inline void SetBSelMax(Float_t b)        { fBSelMax=b; }
    inline void SetVtxBMax(Float_t b)        { fVtxBMax=b; }
    inline void SetWMax(Float_t w)           { fWMax=w; }

    void SetPdfFileName(TString pdfFileName) { fPdfFileName = pdfFileName; }
    void SetFlowFileName(TString flowFileName) { fFlowFileName = flowFileName; }

    inline Float_t GetBeamMomentum() const { return fBeamMomentum; }
    inline Float_t GetMidY() const      { return fMidY; } 
    inline Float_t GetDY() const        { return fDY; }
    inline Float_t GetFlowMinPtm() const{ return fFlowMinPtm; }
    inline Float_t GetBSelMin() const   { return fBSelMin; }
    inline Float_t GetBSelMax() const   { return fBSelMax; }

    ClassDef(CbmHadronAnalysis, 1);
};

#endif

