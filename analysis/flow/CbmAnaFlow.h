// -------------------------------------------------------------------------
// -----                 CbmAnaFlow.h header file             -----
// -----                 Created 22/10/14  by Selim                   -----
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
// ----- This task reconstructs the particle flow (vn, n = 1 & 2) out of measured particle azimuthal distributions (input: reco. files or KFParticle Finder output)
// ----- in a certain collision centrality range; centrality range defined in % most central collisions using STS track multiplicity
// ----- Particle azimuthal distributions can be taken w.r.t. the true reaction plane (input: MC -transport output- file)
// ----- or w.r.t. the event plane (input: output files from eventPlane task) - WARNING, need files with CORRECTED event planes (flattening)
// ----- Several choices for the event plane (using STS or PSD - see eventPlane task)
// ----- It can calculate the correction factor which account for the reaction plane resolution of the detectors & apply it to the measures vn
// ----- (input: output file from eventPlane task, containg a large number of collisions, Nevent > 10 000)
// ----- Particle flow drawn and correction factor calculated for a given centrality interval. Centrality intervals now defined based on STS reco. track multiplicy
// ----- It can correct for inhomogeneous detector acceptance and reconstruction efficiency (input: from collision models, data format: UNIGEN)
// ----- Final drawing: pt- and y-dependences of vn & integrated vn.
// ----- Suggested use: the user can draw the reconstructed flow versus input flow from models
// -------------------------------------------------------------------------

#ifndef CbmAnaFlow_H
#define CbmAnaFlow_H 1

#include "CbmMCEventData.h"
#include "CbmPsdEventData.h"
#include "CbmStsEventData.h"
//#include "CbmAnaParticleData.h"

#include "KFMCParticle.h"
#include "KFParticleMatch.h"
#include "CbmKFPartEfficiencies.h"

#include "FairTask.h"
#include "CbmVertex.h"
#include <vector>
#include <map>
#include <cstring>

#include "UEvent.h"
#include "TGraphErrors.h"
#include <iostream>

class CbmMCEventHeader;
class CbmKFParticlesFinder;
class TClonesArray;
class CbmVertex;
class TDirectory;
class TH1F;
class TH2F;

class CbmAnaFlow : public FairTask
{
    public:

	/** Default constructor **/
	CbmAnaFlow();
        CbmAnaFlow(const char* name, Int_t verbose = 1, Double_t En = 0.);

	/** Destructor **/
	~CbmAnaFlow();

	/** Virtual method Init **/
	virtual InitStatus Init();

	/** Virtual method Exec **/
	virtual void Exec(Option_t* opt);
	
	void setMode(Int_t mode) { fmode = mode; }     
	//========== Sequence for simulations:   ( '+' means sequential processing)
	//========== 1) mode = 0 or 1 or 2:
	//==========    Creation of generic TTree (containing particle azimuth w.r.t. reaction plane or event plane),
	//==========    either from model-generated events (mode 0) or reconstructed events (mode 1) or KF events (mode 2)
	//==========            +
	//========== 2) mode = 3 or 4 or 5+6+7:
	//==========    Calculate the particle flow and draw rapidity-, pt-dependence & integrated value
	//==========    - Simple drawing from TTree (mode 3)
	//==========    - or Drawing including detection efficiency correction (mode 4) - Slow -> restriction: Nevent<100K and Nbin_y*Nbin_pt < 100's
	//==========    - or Drawing including detection efficiency correction (modes 5+6+7) - fast -> no restriction

	//========== NOTE: Detection efficiency correction requires flow computation in narrow (pt, y) bins
	//==========       in mode 5: TTree ordering by increasing pt & y to loop only once over TTree
	//==========       then mode 6: merging of several files with 2D maps (vn, etc) produced in mode 5 (needed if mode 5 run on parallel on several event sub-samples)
	//==========       then mode 7: Detection efficiency map calculation (requires to run mode 5 and 6 on reco. AND model-generated events) and final drawing


        // === I/O files
        void setFileName_gen(TString fileName) { fileName_gen = fileName; }                       // file model-generated events (UNIGEN data format)
        void setFileName_tree(TString fileName) { fileName_tree = fileName; }                     // input generic TTree with reco. events
        void setFileName_tree_gen(TString fileName) { fileName_tree_gen = fileName; }             // input generic TTree with model-generated events
        void setFileName_EPcorrFactor(TString fileName) { fileName_EPcorrFactor = fileName; }     // input file with CORRECTED event planes (need large event samples, typically Nevent>10K)
        void setDirName(const char *dirname, const char *in, const char *ex, const char *inside)  // directory name where files containing 2D map (vn, etc) are; used in mode 6 to merge them 
	{
	    fdirname = dirname;
	    fex = ex;
	    fin = in;
	    finside = inside;
	}

        void setFileName_out(TString fileName) { fileName_out = fileName; }                                                                  // output file name (ROOT file containing TTree)
        void SetFileName_ascii(TString fileName1, TString fileName2) { fileName_ascii_out_y = fileName1; fileName_ascii_out_pt = fileName2;} // final output file name (ascii) to store flow values vs pt vs y

	// === Input parameters
	void setBrange(Double_t sel_perc_min, Double_t sel_perc_max) { fsel_perc_min = sel_perc_min; fsel_perc_max = sel_perc_max; }         // Centrality range
	// IMPORTANT NOTE: Centrality range defined in % most central collisions using STS track multiplicity
	// ... need better naming than "setBrange"?

	void doPionRotation() { fdoPionRotation = kTRUE; }                           // Account for opposite pion v1 w.r.t. proton v1 (WA98 observation!), in case the user is interested in e.g. charge particle v1 - WARNING: must hold at investigated energies
        void doBackYRotation() { fdoBackYRotation = kTRUE; }                         // Account for anti-symmetry of v1 versus y, in case rapidity selection includes backward & forward hemisphere
        void setMCorRECOTrack(Int_t part_mc_reco) { fpart_mc_reco = part_mc_reco; }  // for modes 1, 2: stored in generic TTree either reco. tracks, or MC tracks associated with reco. tracks
	void useTrackatGen() { fuseTrackatGen = kTRUE; }                             // for modes 3, 4, 7: to draw the flow from model-generated events directly
        void setParticleToAnalyse(Int_t sel_pdg) { fsel_pdg = sel_pdg; }             // PDG code of particle to analyse (if not set, take all particles)
	void setQcut(TString sel_Q) { fsel_Q = sel_Q; }                              // cut on particle charge: "positive"/"negative"/"charged"(even "neutral"...), no cut if not set
	void setS_BG(TString sel_signal) { fsel_signal = sel_signal; }               // analyse signal (true) candidates or background n-tuplets (not used yet)
	void setflowHarmoToAnalyse(TString sel_factor) { fsel_factor = sel_factor; } // flow component to analyse: v1 or v2
	void setCalcFlowCoef_meth(TString coef_meth) { fcoef_meth = coef_meth; }     // method to calculate flow component: <cos(n*phi)> (default) or phi-fit
	void doPrimary() { fdoPrim = kTRUE; }                                        // restrict analysis to primary particles
	void doEffCorr() { fdoEff = kTRUE; }                                         // apply detection efficiency correction

	void setEPmeth(TString EP_meth) { fEP_meth = EP_meth; }                      // reaction/event plane used: RP (MC info), EP_STS_..., EP_PSD_... etc
	void setdoEPCorr(Bool_t doEPCorr) { fdoEPCorr = doEPCorr; }                  // apply correction factor for reaction plane resolution to measured vn
	void setEPsubevent(Bool_t useSubevent) { fuseSubevent = useSubevent; }       // use Sub-event method to calulate correction factor
        void CalcCorrectionFactors(Double_t* corrFactor);                            // Compute correction factor

	// STS based centrality
        int centrality_STSmult();
        void centrality_cutDef_STSmult();

        // === mode 0, 1, 2: Create generic TTree's for analysis
	bool CreateTreeGen(int mult);
	bool CreateTreeReco(int mult);                                      // IMPORTANT NOTE: CreateTreeReco() stores only reco. track
	                                                                    // which have a corresponding MC track!! 
	                                                                    // & particle mass (to compute rapidity) from MC tracks -> TO DO: real PID and bg track inclusion
	bool CreateTreeKFPart(int mult);

	// === mode 3: Simple draw wo detection efficiency correction
        bool Draw_diffvn_simple();      // Draw pt- & y-dependence of vn
        bool Draw_intvn_simple();       // calculate integrated vn

	// === mode 4: Draw with detection efficiency correction - slow -> small collision event sample
        bool Create2Dhisto_Y_vs_pt();   // Create 2D map vn vs pt vs y
        bool CreateEff_Y_vs_pt();       // Create detection efficiency 2D map
	bool Draw_diffvn_Effcorr();     // Draw pt- & y-dependence of vn
	bool Draw_intvn_Effcorr();      // calculate integrated vn

	// === mode 3 or 4:
	void CalcFlowCoef_cosnphi(TTree *&t, TString &ALLcut, Float_t* meanFlow, Float_t* emeanFlow, Int_t &bin, Double_t &N);  // calculate flow component, <cos(n*phi)> method
        void CalcFlowCoef_phifit(TTree *&t, TString &ALLcut, Float_t* meanFlow, Float_t* emeanFlow, Int_t &bin, Double_t &N);   // calculate flow component, phi-fit method
        void CalcMeanPt(TTree *&t, TString &ALLcut, Float_t* meanX, Float_t* emeanX, Int_t &bin);                               // calculate <pt> for pt-dependence of vn
        void CalcMeanY(TTree *&t, TString &ALLcut, Float_t* meanX, Float_t* emeanX, Int_t &bin);                                // calculate <y> for y-dependence of vn
	void EffCorrection(Int_t &binX, Int_t &binY, Double_t &N, Double_t &N_err);                                             // apply detection efficiency correction

	// === mode 5+6+7: Draw with detection efficiency correction - fast -> no restriction
        bool SortTree();                  // sorte generic TTree by increasing pt and y
        bool CheckSortedTree();           //
        void MergeHistoInFiles();         // merging of several files with 2D maps (vn, etc)
	
	void CalcFlowCoef_cosnphi_1TreeLoop(Int_t &binX, Int_t &binY); // calculate flow component, <cos(n*phi)> method
        void CalcFlowCoef_phifit_1TreeLoop(Int_t &binX, Int_t &binY);  // calculate flow component, phi-fit method
	void CalcMeanY_1TreeLoop(Int_t &binX, Int_t &binY);            // calculate <y> for y-dependence of vn
        void CalcMeanPt_1TreeLoop(Int_t &binX, Int_t &binY);           // calculate <pt> for pt-dependence of vn
        bool Create2Dhisto_Y_vs_pt_1TreeLoop();                        // Create 2D map vn vs pt vs y
	bool CreateEff_Y_vs_pt_1TreeLoop();                            // Create detection efficiency 2D map
        bool Draw_diffvn_Effcorr_1TreeLoop();                          // Draw pt- & y-dependence of vn
        bool Draw_intvn_Effcorr_1TreeLoop();                           // calculate integrated vn

        // === Final draw
        void setdoInt_diffvn(Int_t doInt_diffvn) { fdoInt_diffvn = doInt_diffvn; } // Draw option; 0: int. vn, 1: diff. vn (both f(y) & f(pt)), 2: both int. & diff. vn

        // pt and y range and number of bins for final drawing
        void setPTrange(Double_t sel_PTmin, Double_t sel_PTmax, Int_t sel_PTNbin) { fsel_PTmin = sel_PTmin; fsel_PTmax = sel_PTmax; fsel_PTNbin = sel_PTNbin; }
        void setYrange(Double_t sel_Ymin, Double_t sel_Ymax, Int_t sel_YNbin) { fsel_Ymin = sel_Ymin; fsel_Ymax = sel_Ymax; fsel_YNbin = sel_YNbin; }

        // pt and y cut for y-dependence and pt-dependence, resp.
        void setPTcut(Double_t sel_PTcut_min, Double_t sel_PTcut_max) { fsel_PTcut_min = sel_PTcut_min; fsel_PTcut_max = sel_PTcut_max; }
	void setYcut(Double_t sel_Ycut_min, Double_t sel_Ycut_max) { fsel_Ycut_min = sel_Ycut_min; fsel_Ycut_max = sel_Ycut_max; }

        // === Small common functions
	void CalcErrorProp_prod(Double_t &N1, Double_t &N_err1, Double_t &N2, Double_t &N_err2);  // product and quotient + error propagation
	void CalcErrorProp_quot(Double_t &N1, Double_t &N_err1, Double_t &N2, Double_t &N_err2);  //
        void CalcErrorProp_quot(Float_t &N1, Float_t &N_err1, Double_t &N2, Double_t &N_err2);    //

	void projRapidityCM();                // compute c.m. rapidity and projectile rapidity (in c.m.)
	Double_t Range(Double_t phi);         // shift angle modulo 2pi in proper range (here: [-pi, pi])
        Double_t Range2nd(Double_t phi);      // shift angle modulo pi in proper range (here: [-pi/2, pi/2])

	void Write();
	void Finish();

    private:
      
      Double_t fPi;
      Double_t fmass_proton;
      Double_t fEn;                     // Beam kinetic energy per nucleon (AGeV)
      Double_t fy_proj_cm, fy_cm;       // c.m. rapidity and projectile rapidity (in c.m.)

      Int_t fmode;

      //=== Generic TTree (either from model, reconstruction or kfparticle finder output)
      TTree* outTree;                   // particle-wise entries

      Double_t fB_MC;                   // MC collision impact parameter [fm]
      Double_t fphi_to_RP;              // MC collision  reaction plane [radian]
      Double_t fphi_to_EP_PSD;          // Event planes from STS and PSD (full detector and sub-events) [radian]
      Double_t fphi_to_EP_PSD1;         //
      Double_t fphi_to_EP_PSD2;         //
      Double_t fphi_to_EP_PSD3;         //
      Double_t fphi_to_EP_STS_harmo1;   //
      Double_t fphi_to_EP_STS_harmo2;   //
      Double_t fY;                      // Particle rapidity
      Double_t fpt;                     // Particle transverse momentum [GeV/c]
      Double_t fmass;                   // Particle mass (GeV)
      Int_t fpdg;                       // Particle type
      Int_t fmotherID;                  
      Int_t fQ;                         // Particle charge
      bool fsignal;                     // signal (true) candidates or background n-tuples (not used yet)
      int fmult;                        // STS track multiplicy in corresponding collision event

      TH1F* hmult;                      // check STS track multiplicy

      double fsel_perc_min;             // centrality range, min in % most central collsions
      double fsel_perc_max;             // centrality range, max in % most central collsions
      Double_t fsel_Bmin;               // centrality range, minimum STS multiplicity
      Double_t fsel_Bmax;               // centrality range, maximum STS multiplicity
      TString ssel_Bmin;                //
      TString ssel_Bmax;                //
      TString Bcut;                     // centrality cut

      Int_t fpart_mc_reco;              // for modes 1, 2: stored in generic TTree either reco. tracks (=1), or MC tracks associated with reco. tracks (=0)


      bool fdoPionRotation;             // Account for opposite pion v1 w.r.t. proton v1 (WA98 observation!), in case the user is interested in e.g. charge particle v1 - WARNING: must hold at investigated energies
      bool fdoBackYRotation;            // Account for anti-symmetry of v1 versus y, in case rapidity selection includes backward & forward hemisphere

      // I/O file name
      TString fileName_gen;             // file model-generated events (UNIGEN data format)
      TString fileName_tree;            // input generic TTree with reco. events
      TString fileName_tree_gen;        // input generic TTree with model-generated events
      TString fileName_out;             // output file name (ROOT file containing TTree)

      TString fileName_ascii_out_y;     // final output file name (ascii) to store flow values vs y
      TString fileName_ascii_out_pt;    // final output file name (ascii) to store flow values vs pt

      // model output
      CbmMCEventHeader *fHeader;        // data container for MC info
      TTree* fTree_gen;
      UEvent *fEvent_gen;
      Double_t fBetaCM;                 // CM velocity
      Double_t fGammaCM;                // CM Lorentz factor
      Int_t fevt_inc;
      
      // reconstruction output
      TClonesArray *flistRECOtrack;     // data container for STS reco. tracks
      TClonesArray *flisttrackMATCH;    // data container for STS track match
      CbmVertex    *flistPV;            // data container for reco. primary collision vertex
      
      CbmMCEventData *fMcEvent;         // data container for MC info
      CbmPsdEventData* fPsdEvent;       // data container for PSD event planes
      CbmStsEventData* fStsEvent;       // data container for STS event planes
      
      // KFParticle finder output
      TClonesArray* fRecParticles;
      TClonesArray* fMCParticles;
      TClonesArray* fMatchParticles;
      TClonesArray* flistMCtrack;
      
      CbmKFPartEfficiencies fParteff;

      //=== Draw

      TString fsel_factor;      // fsel_factor = "1": v1 analysis, "2": v2 analysis; by default, fsel_factor = "1"; IMPORTANT NOTE: fsel_factor >= harmo. of evenPlane!!
      Bool_t fuseTrackatGen;    // for modes 3, 4, 7: to draw the flow from model-generated events directly
      TString fcoef_meth;       // method to calculate flow component: <cos(n*phi)> (default) or phi-fit (fcoef_meth="phifit")
      Int_t fdoInt_diffvn;      // Draw option; 0: int. vn, 1: diff. vn (both f(y) & f(pt)), 2: both int. & diff. vn

      bool fdoPrim;             // restrict analysis to primary particles
      bool fdoEff;              // apply detection efficiency correction

      // Final draw: range (in GeV/c) & #bins
      Int_t fsel_PTNbin;
      Double_t fsel_PTmin;      
      Double_t fsel_PTmax;
      TString ssel_PTmin;
      TString ssel_PTmax;

      // Cut: pt selection for y-dependence & int. vn
      Double_t fsel_PTcut_min;  
      Double_t fsel_PTcut_max;
      TString PTcut;

      // Final draw: range (in CM & normalized to Y_proj) & #bins
      Int_t fsel_YNbin;
      Double_t fsel_Ymin;       
      Double_t fsel_Ymax;
      TString ssel_Ymin;
      TString ssel_Ymax;

      // Cut: rapidity selection for pt-dependence & int. vn
      Double_t fsel_Ycut_min;   
      Double_t fsel_Ycut_max;
      TString Ycut;

      Int_t fsel_pdg;           // selection of particle pdgCode, no cut if not set
      TString spdg;
      TString PDGcut;

      TString fsel_Q;           // selection of particle charge: "positive"/"negative"/"charged" (even "neutral"...), no cut if not set
      TString Qcut;

      TString PRIMcut;          // analyse primary particles only, no cut if not set - No effect if model-generated files used
                                

      TString fsel_signal;      // analyse signal (true) candidates or background n-tuplets (not used yet)
      TString Scut;

      TString fileName_EPcorrFactor;  // input file with CORRECTED event planes (need large event samples, typically Nevent>10K)
      TString fEP_meth;               // reaction/event plane used: RP (MC info), EP_STS_..., EP_PSD_... etc
      Bool_t fdoEPCorr;               // if =kTRUE, apply correction factor for reaction plane resolution to measured vn
      Bool_t fuseSubevent;            // if =kTRUE, use Sub-event method to calulate correction factor

      TGraphErrors *gr0, *gr1;        // final histo

      // Range and binning for detection efficiency correction 

      static const int NyBin = 400;
      static const double minBin_y;
      static const double maxBin_y;

      static const int NptBin = 300;
      static const double minBin_pt;
      static const double maxBin_pt;

      // 2d (pt, y) maps 
      TH2F* hRap_vs_y_pt; //Z-axis: <rapidity>
      TH2F* hpt_vs_y_pt;  //Z-axis: <pt>
      TH2F* hvn_vs_y_pt;  //Z-axis: vn
      TH2F* hN_vs_y_pt;   //Z-axis: Nparticle
      TH2F* hEff_vs_y_pt; //Z-axis: detection efficiency

      TH1F* hRapidity;
      TH1F* hpt;
      TH1F* hphi;
      TH1F* hcosnphi;

      // for merging several histograms in files contained in fdirname
      const char *fdirname; 
      const char *fex;
      const char *fin;
      const char *finside;

      CbmAnaFlow(const CbmAnaFlow&);
      CbmAnaFlow& operator=(const CbmAnaFlow&);

      ClassDef(CbmAnaFlow,1);
};

#endif
