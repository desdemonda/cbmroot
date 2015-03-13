// -------------------------------------------------------------------------
// -----                 eventPlane.h header file             -----
// -----                 Created 22/10/14  by Selim                   -----
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
// ----- This task calculates the event plane for each collision event from STS and PSD information (input: reco. files)
// ----- It also provides comparision with real reaction plane (input: MC -transport output- files)
// ----- It outputs a TTree containing event planes and sub-event planes
// ----- Sub-event plane definition: 
// ----- STS: random sharing of tracks between 2 sub-events
// ----- PSD: sub-event 1 = central modules, sub-event 2 = 1st corona of modules around it, sub-event 3 = peripheral modules
// ----- It also provides event plane flattening techniques to correct for detection setup azimuthal asymmetry: Q-recentering (A. M. Poskanzer and S. A. Voloshin, Phys. Rev. C58, 1671 (1998) [arXiv:nucl-ex/9805001]) and Barrette method
// ----- Flattening techniques applied for each event and sub-event planes and in bins of detector multiplicity (detector multiplicity := number of reco. particles in STS or deposited energy in PSD - full or in sub-events)
// ----- Flattening techniques can be applied without binning if wanted (then use of fit functions - see TEST in the code)
// -------------------------------------------------------------------------

#ifndef eventPlane_H
#define eventPlane_H 1


#include "FairTask.h"
#include "CbmMCEventData.h"
#include "CbmPsdEventData.h"
#include "CbmStsEventData.h"
#include <vector>
#include "CbmVertex.h"
#include "CbmStsKFTrackFitter.h"

class TClonesArray;
class CbmMCEventHeader;

class eventPlane : public FairTask
{

    public:

	/** Default constructor **/
	eventPlane();
        eventPlane(const char* name, Int_t verbose = 1, Double_t En = 0.);

	/** Destructor **/
	~eventPlane();

	/** Virtual method Init **/
	virtual InitStatus Init();

	/** Virtual method Exec **/
	virtual void Exec(Option_t* opt);

	void setFileName_PsdModCoord(TString fileName) { fileName_PsdModCoord = fileName; } // File containing PSD module (x, y) coordinates in LAB
	
        void setFileName_flat_Qcorr(TString fileName) { fileName_Qcorr = fileName; }        // file with (uncorrected) event planes -> Q-recentering flattening method
	void setFileName_flat_Barr(TString fileName) { fileName_Barr = fileName; }          // file with (uncorrected) event planes -> flattening method of Barrette

        void WriteHistogramms();

	void Finish();

        void BinningFlattening();                                                           // Calculate detector multiplicity intervals for event plane flattening method
        void BinCalc(TTree *&tree, TString &detinfo, TString &sPreSel, double *b_tmp);      //
	void doFlatBinFixStat() { doFixStat = kTRUE; }                                      // doFixStat=kFALSE: fixed multiplicity intervals (default); doFixStat=kTRUE: interval with fixed collision statistics
        int FlatBinWhichK(double E_tmp, double *b_tmp);                                     // Find in which interval a given collision belongs to
	// NOTE: number of bins for flattening correction flexible (see "fNbinFlat" below)

	// TEST (flattening with fit function - no binning): for PSD & Qcorr only
        void FitFunctionFlattening();
        void FitFunctionCalc(TTree *&tree, TString &detinfo_tmp, TString &Q_tmp, TString &sPreSel_tmp, int meth, int XY);

        //====== PSD

	void PSDcoordinates();           // retrive PSD module (x, y) coordinates in LAB from ascii file

        void PSDsecEdep();               // compute energy deposition / PSD section
        void PSDmodEdep();               // compute energy deposition / PSD module

        void PSDfullEdep();              // compute full energy deposition in PSD

	void PSDtransverseMomMeth();     // Event plane calculation: tranverse momentum (approximate) method adapted to PSD

	//-- EP flattening
	void PSDsetFlat(Bool_t Qcorr, Bool_t Barr) 
	{
	    fdoFlat_PSD_Qcorr = Qcorr;   // event plane flattening correction wished or not? if yes, requires file with (uncorrected) event planes
	    fdoFlat_PSD_Barr = Barr;
	}

	void PSDinit_flat_Qcorr();       // event plane flattening correction: init (correction factor calculations)
        void PSDinit_flat_Barr();

	//====== STS

	void STSsetYcut_PSDsub(Double_t cut) { fcut_STS_PSDsub = cut; }  // apply rapidity selection to decrease PSD/STS overlap: suggested cut y<0.8*y_projectile (in c.m.)

        void STSprojRapidityCM();                                        // compute c.m. rapidity and projectile rapidity (in c.m.)

	void STSuseReco(Bool_t reco) { fdoSTSreco = reco; }              // fdoSTSreco=kTRUE: use STS reco. tracks (default); fdoSTSreco=kFALSE: use MC tracks with >= 4 hits in STS
	
        void STSMCtransverseMomMeth();    

        void STSRECOtransverseMomMeth();  // Event plane calculation: tranverse momentum method

	//-- EP flattening
	void STSsetFlat(Bool_t Qcorr, Bool_t Barr) 
	{
	    fdoFlat_STS_Qcorr = Qcorr;    // event plane flattening correction wished or not? if yes, requires file with (uncorrected) event planes
	    fdoFlat_STS_Barr = Barr;
	}

        void STSinit_flat_Qcorr();        // event plane flattening correction: init (correction factor calculations)
        void STSinit_flat_Barr();

	Double_t Range(Double_t phi);     // shift angle modulo 2pi in proper range (here: [-pi, pi])
        Double_t Range2nd(Double_t phi);  // shift angle modulo pi in proper range (here: [-pi/2, pi/2])

    private:

        Double_t fPi;
        CbmMCEventHeader *fHeader;
	CbmMCEventData *fMCEventData;            // data container for MC info
	TString fileName_PsdModCoord;            // ascii file with PSD module (x, y) coordinates in LAB
        TString fileName_Qcorr, fileName_Barr;   // file with (uncorrected) event planes for Q-recentering and Barrette methods

	// == Binning for event plane flattening

	static const int fNbinFlat = 20;
	bool doFixStat;            // doFixStat=kFALSE: fixed multiplicity intervals (default); doFixStat=kTRUE: interval with fixed collision statistics
	double b_STS[fNbinFlat];   // Interval definition for STS and (full and sub-event) PSD event plane flattening
	double b_PSD[fNbinFlat];   //
	// PSD sub-event 1         //
	double b_PSD1[fNbinFlat];  //
	// PSD sub-event 2         //
	double b_PSD2[fNbinFlat];  //
	// PSD sub-event 3         //
	double b_PSD3[fNbinFlat];  //

        // TEST (flattening with fit function - no binning): for PSD  & Qcorr only
        TF1* funct[4][4];          // Fit functions (PSD and Qcorr only): <Qx>, rms(Qx), <Qy>, rms(Qy) versus E, E1, E2, E3, resp.;

        TString sPreSel;           // pre-selection for calibrating Q-recentering correction coefficients

        // == PSD

        TClonesArray *flistPSDdigit;             // data container for PSD digits
	TClonesArray *flistPSDhit;               // data container for PSD hits
	CbmPsdEventData* fCbmPsdEvent;           // data container for PSD event planes

        Int_t finc_mod;
	Double_t fX_mod[100];                    // PSD module (x, y) coordinates in LAB
	Double_t fY_mod[100];
	Double_t fphi_mod[100];                  // PSD module azimuth in LAB
	Double_t fR_mod[100];                    // Distance between origin and center of modules (not used presently)
	Double_t fedep_mod[100];                 // Energy deposited in PSD modules for each event (integrated over 60 scintillator layers)

        Double_t fedep_mod_av[100];              // Energy deposited in PSD modules integrated over many events (NOT normalized to N_event)
        Double_t fedep_sec_av[100];              // Energy deposited in PSD sections integrated over many events (NOT normalized to N_event)

        TH1F *hEsec;                             // Energy deposited in PSD modules integrated over many events (NOT normalized to N_event)
	TH2F *hEmod;                             // Energy deposited in PSD sections integrated over many events (NOT normalized to N_event)


        TH2F *hCoormod_sub1;                     // module (x, y) coordinates in LAB for PSD sub-events
        TH2F *hCoormod_sub2;                     //
        TH2F *hCoormod_sub3;                     //

	Bool_t fdoFlat_PSD_Qcorr, fdoFlat_PSD_Barr; // boolen for event plane flattening

        // Q-recentering correction coefficients
        Double_t PSD_shift_Qx[fNbinFlat], PSD_shift_Qy[fNbinFlat], PSD_RMS_Qx[fNbinFlat], PSD_RMS_Qy[fNbinFlat]; 
        Double_t PSD_shift_Qx_sub1[fNbinFlat], PSD_shift_Qy_sub1[fNbinFlat], PSD_RMS_Qx_sub1[fNbinFlat], PSD_RMS_Qy_sub1[fNbinFlat];
        Double_t PSD_shift_Qx_sub2[fNbinFlat], PSD_shift_Qy_sub2[fNbinFlat], PSD_RMS_Qx_sub2[fNbinFlat], PSD_RMS_Qy_sub2[fNbinFlat];
        Double_t PSD_shift_Qx_sub3[fNbinFlat], PSD_shift_Qy_sub3[fNbinFlat], PSD_RMS_Qx_sub3[fNbinFlat], PSD_RMS_Qy_sub3[fNbinFlat];

        // Barrette method correction coefficients
	// 8 order correction
        Double_t PSD_mean_cosphi[fNbinFlat][8], PSD_mean_sinphi[fNbinFlat][8];
        Double_t PSD_mean_cosphi_sub1[fNbinFlat][8], PSD_mean_sinphi_sub1[fNbinFlat][8]; 
        Double_t PSD_mean_cosphi_sub2[fNbinFlat][8], PSD_mean_sinphi_sub2[fNbinFlat][8]; 
        Double_t PSD_mean_cosphi_sub3[fNbinFlat][8], PSD_mean_sinphi_sub3[fNbinFlat][8]; 

	// == STS

	TClonesArray *flistSTSMCtrack;          // data container for MC tracks
	TClonesArray *flistSTSRECOtrack;        // data container for STS reco. tracks
	TClonesArray *flistSTStrackMATCH;       // data container for STS track match
	CbmVertex    *flistPV;                  // data container for reco. primary vertex
	CbmStsEventData* fCbmStsEvent;          // data container for STS event planes

	TH1F *hY;                               // keep trace of cuts on reco. STS tracks
	TH1F *hP;                               //
	TH1F *hIP;                              //
	TH1F *hChi2toNDF;                       //

        Bool_t fdoSTSreco;                      // fdoSTSreco=kTRUE: use STS reco. tracks (default); fdoSTSreco=kFALSE: use MC tracks with >= 4 hits in STS
        Double_t fy_proj_cm, fy_cm;             // projectile rapidity in c.m., c.m. rapidity
	Double_t fEn;                           // Beam kinetic energy per nucleon (AGeV)
        Double_t fcut_STS_PSDsub;               // rapidity cut to decrease PSD/STS overlap: suggested cut y<0.8*y_projectile (in c.m.)

        Bool_t fdoFlat_STS_Qcorr, fdoFlat_STS_Barr;  // boolen for event plane flattening

        // Q-recentering correction coefficients
	Double_t STS_harmo1_shift_Qx_sub1[fNbinFlat], STS_harmo1_shift_Qy_sub1[fNbinFlat], STS_harmo1_RMS_Qx_sub1[fNbinFlat], STS_harmo1_RMS_Qy_sub1[fNbinFlat]; 
	Double_t STS_harmo1_shift_Qx_sub2[fNbinFlat], STS_harmo1_shift_Qy_sub2[fNbinFlat], STS_harmo1_RMS_Qx_sub2[fNbinFlat], STS_harmo1_RMS_Qy_sub2[fNbinFlat];
        Double_t STS_harmo1_shift_Qx_full[fNbinFlat], STS_harmo1_shift_Qy_full[fNbinFlat], STS_harmo1_RMS_Qx_full[fNbinFlat], STS_harmo1_RMS_Qy_full[fNbinFlat];

	Double_t STS_harmo2_shift_Qx_sub1[fNbinFlat], STS_harmo2_shift_Qy_sub1[fNbinFlat], STS_harmo2_RMS_Qx_sub1[fNbinFlat], STS_harmo2_RMS_Qy_sub1[fNbinFlat]; 
	Double_t STS_harmo2_shift_Qx_sub2[fNbinFlat], STS_harmo2_shift_Qy_sub2[fNbinFlat], STS_harmo2_RMS_Qx_sub2[fNbinFlat], STS_harmo2_RMS_Qy_sub2[fNbinFlat];
        Double_t STS_harmo2_shift_Qx_full[fNbinFlat], STS_harmo2_shift_Qy_full[fNbinFlat], STS_harmo2_RMS_Qx_full[fNbinFlat], STS_harmo2_RMS_Qy_full[fNbinFlat];
	
        // Barrette method correction coefficients
	// 8 order correction
	Double_t STS_harmo1_mean_cosphi_sub1[fNbinFlat][8], STS_harmo1_mean_sinphi_sub1[fNbinFlat][8];
	Double_t STS_harmo1_mean_cosphi_sub2[fNbinFlat][8], STS_harmo1_mean_sinphi_sub2[fNbinFlat][8];
        Double_t STS_harmo1_mean_cosphi_full[fNbinFlat][8], STS_harmo1_mean_sinphi_full[fNbinFlat][8]; 
	
        Double_t STS_harmo2_mean_cosphi_sub1[fNbinFlat][8], STS_harmo2_mean_sinphi_sub1[fNbinFlat][8];
	Double_t STS_harmo2_mean_cosphi_sub2[fNbinFlat][8], STS_harmo2_mean_sinphi_sub2[fNbinFlat][8];
        Double_t STS_harmo2_mean_cosphi_full[fNbinFlat][8], STS_harmo2_mean_sinphi_full[fNbinFlat][8];

	ClassDef(eventPlane,2);
};

#endif
