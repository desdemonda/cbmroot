// -------------------------------------------------------------------------
// -----                eventPlane.h source file             -----
// -----                Created 22/10/14  by Selim         -----
// -------------------------------------------------------------------------

// check in the code:
// - TO DO
// - WARNING (to know for users)

#include <iostream>
#include <fstream>

#include "TMath.h"
#include "TH1F.h"
#include "TH2F.h"
#include "FairRootManager.h"
#include "CbmMCEventHeader.h"
#include "TClonesArray.h"
#include "TCanvas.h"
#include "TF1.h"

#include "eventPlane.h"
#include "CbmPsdHit.h"
#include "CbmPsdDigi.h"
#include "CbmMCTrack.h"
#include "CbmStsTrack.h"
#include "CbmTrackMatch.h"
#include "TRandom.h"
#include "FairTrackParam.h"
#include "TVector3.h"
#include "CbmKFTrack.h"
#include "CbmKFVertex.h"
#include "CbmL1PFFitter.h"
#include "L1Field.h"

#include <vector>

#include "FairLogger.h"

using std::vector;
using std::cout;
using std::endl;
using std::ifstream;

// -----   Default constructor   -------------------------------------------
eventPlane::eventPlane() : FairTask("EventPlane",1), fHeader(NULL), fMCEventData(NULL), flistPSDdigit(NULL), flistPSDhit(NULL), fCbmPsdEvent(NULL), flistSTSMCtrack(NULL), flistSTSRECOtrack(NULL), flistSTStrackMATCH(NULL), flistPV(NULL), fCbmStsEvent(NULL)
{
    fPi = TMath::Pi();

    hEsec = new TH1F ("hEsec"," Edep vs sec ", 15, 0., 15.);

    hEmod = new TH2F ("hEmod"," Edep vs module ", 250, -100., 150., 200, -100., 100.);
    hCoormod_sub1 = new TH2F ("hCoormod_sub1"," Edep vs module (sub1) ", 250, -100., 150., 200, -100., 100.);
    hCoormod_sub2 = new TH2F ("hCoormod_sub2"," Edep vs module (sub2) ", 250, -100., 150., 200, -100., 100.);
    hCoormod_sub3 = new TH2F ("hCoormod_sub3"," Edep vs module (sub3) ", 250, -100., 150., 200, -100., 100.);

    hY = new TH1F("hY","mc/reco. track rapidity (cm)", 400, -2., 2.);
    hP = new TH1F("hP","reco. track momentum", 1000, 0., 10.);
    hIP = new TH1F ("hIP"," reco. track impact parameter ", 1000, 0., 10.);
    hChi2toNDF = new TH1F("hChi2toNDF"," reco. track chi2 to NDF ", 1000, 0., 30.);

    doFixStat = kFALSE;

    fdoFlat_PSD_Qcorr = kFALSE;
    fdoFlat_PSD_Barr = kFALSE;
    
    fdoFlat_STS_Qcorr = kFALSE;
    fdoFlat_STS_Barr = kFALSE;

    fileName_Qcorr = "";
    fileName_Barr = "";
    sPreSel = "StsEvent.fmult > 40.";
    
    fy_proj_cm = 0.;
    fy_cm = 0.;
    fEn = 25.;
    fdoSTSreco = kTRUE;
    fcut_STS_PSDsub = 10.;
}

eventPlane::eventPlane(const char* name, Int_t verbose, Double_t En) : FairTask(name, verbose), fHeader(NULL), fMCEventData(NULL), flistPSDdigit(NULL), flistPSDhit(NULL), fCbmPsdEvent(NULL), flistSTSMCtrack(NULL), flistSTSRECOtrack(NULL), flistSTStrackMATCH(NULL), flistPV(NULL), fCbmStsEvent(NULL)
{
    fPi = TMath::Pi();

    hEsec = new TH1F ("hEsec"," Edep vs sec ", 15, 0., 15.);

    hEmod = new TH2F ("hEmod"," Edep vs module ", 250, -100., 150., 200, -100., 100.);
    hCoormod_sub1 = new TH2F ("hCoormod_sub1"," Edep vs module (sub1) ", 250, -100., 150., 200, -100., 100.);
    hCoormod_sub2 = new TH2F ("hCoormod_sub2"," Edep vs module (sub2) ", 250, -100., 150., 200, -100., 100.);
    hCoormod_sub3 = new TH2F ("hCoormod_sub3"," Edep vs module (sub3) ", 250, -100., 150., 200, -100., 100.);

    hY = new TH1F("hY","mc/reco. track rapidity (cm)", 400, -2., 2.);
    hP = new TH1F("hP","reco. track momentum", 1000, 0., 10.);
    hIP = new TH1F ("hIP"," reco. track impact parameter ", 1000, 0., 10.);
    hChi2toNDF = new TH1F("hChi2toNDF"," reco. track chi2 to NDF ", 1000, 0, 30);

    doFixStat = kFALSE;

    fdoFlat_PSD_Qcorr = kFALSE;
    fdoFlat_PSD_Barr = kFALSE;
    
    fdoFlat_STS_Qcorr = kFALSE;
    fdoFlat_STS_Barr = kFALSE;

    fileName_Qcorr = "";
    fileName_Barr = "";
    sPreSel = "StsEvent.fmult > 40.";

    fy_proj_cm = 0.;
    fy_cm = 0.;
    fEn = En;
    fdoSTSreco = kTRUE;
    fcut_STS_PSDsub = 10.;
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
eventPlane::~eventPlane()
{
    if ( fHeader ) {
	fHeader->Delete();
	delete fHeader;
    }
    
    if ( fMCEventData ) {
	fMCEventData->Delete();
	delete fMCEventData;
    }

    if ( flistPSDdigit ) {
	flistPSDdigit->Delete();
	delete flistPSDdigit;
    }

    if ( flistPSDhit ) {
	flistPSDhit->Delete();
	delete flistPSDhit;
    }

    if ( fCbmPsdEvent ) {
	fCbmPsdEvent->Delete();
	delete fCbmPsdEvent;
    }

    if ( flistSTSMCtrack ) {
	flistSTSMCtrack->Delete();
	delete flistSTSMCtrack;
    }
    
    if ( flistSTSRECOtrack ) {
	flistSTSRECOtrack->Delete();
	delete flistSTSRECOtrack;
    }
    
    if ( flistSTStrackMATCH ) {
	flistSTStrackMATCH->Delete();
	delete flistSTStrackMATCH;
    }
    
    if ( flistPV ) {
	flistPV->Delete();
	delete flistPV;
    }
    
    if ( fCbmStsEvent ) {
	fCbmStsEvent->Delete();
	delete fCbmStsEvent;
    }
}
// -------------------------------------------------------------------------


// -----   Public method Init   --------------------------------------------
InitStatus eventPlane::Init()
{  
  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- eventPlane::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  fHeader = (CbmMCEventHeader*) ioman->GetObject("MCEventHeader.");
  if ( ! fHeader ) {
      LOG(FATAL) << "-E- eventPlane::Init: No event header!" << FairLogger::endl;
    return kERROR;
  }

  // =========== Get input array

  flistPSDdigit = (TClonesArray*) ioman->GetObject("PsdDigi");
  if ( ! flistPSDdigit ) {
      LOG(FATAL) << "-E- eventPlane::Init: No PSD digits array!" << FairLogger::endl;
    return kERROR;
  }

  flistPSDhit = (TClonesArray*) ioman->GetObject("PsdHit");
  if ( ! flistPSDhit ) {
      LOG(FATAL) << "-E- eventPlane::Init: No PSD hits array!" << FairLogger::endl;
    return kERROR;
  }

  flistSTSMCtrack = (TClonesArray*) ioman->GetObject("MCTrack");
  if ( ! flistSTSMCtrack ) {
      LOG(FATAL) << "-E- eventPlane::Init: No MC Tracks array!" << FairLogger::endl;
    return kERROR;
  }

  flistSTSRECOtrack = (TClonesArray*) ioman->GetObject("StsTrack");
  if ( ! flistSTSRECOtrack ) {
      LOG(FATAL) << "-E- eventPlane::Init: No reco. tracks array!" << FairLogger::endl;
    return kERROR;
  }

  flistSTStrackMATCH = (TClonesArray*) ioman->GetObject("StsTrackMatch");
  if ( ! flistSTStrackMATCH ) {
      LOG(FATAL) << "-E- eventPlane::Init: No track match array!" << FairLogger::endl;
    return kERROR;
  }

  flistPV = (CbmVertex*) ioman->GetObject("PrimaryVertex");
  if ( ! flistPV ) {
      LOG(FATAL) << "-E- eventPlane::Init: No reco. primary vertex array!" << FairLogger::endl;
    return kERROR;
  }    
  
  // =========== Create and register output array
  
  fMCEventData = new CbmMCEventData("CbmMcEvent");
  ioman->Register("McEvent", "MC", fMCEventData, kTRUE);  
  
  fCbmPsdEvent = new CbmPsdEventData("CbmPsdEvent");
  ioman->Register("PsdEvent", "PSD", fCbmPsdEvent, kTRUE);
  
  fCbmStsEvent = new CbmStsEventData("CbmStsEvent");
  ioman->Register("StsEvent", "STS", fCbmStsEvent, kTRUE);

  for (Int_t i=0; i<100; i++) // to change if different number of PSD modules
  {
      fX_mod[i] = 0.;
      fY_mod[i] = 0.;
      fphi_mod[i] = 0.;
      fR_mod[i] = 0.;
      fedep_mod[i] = 0.;

      fedep_mod_av[i] = 0.;
      fedep_sec_av[i] = 0.;
  }
  finc_mod = 0;   

  PSDcoordinates();

  STSprojRapidityCM();  

  //------- Event plane flattening (2 methods)

  // ==== Binning
  if (fdoFlat_PSD_Qcorr == kTRUE || fdoFlat_STS_Qcorr == kTRUE || fdoFlat_PSD_Barr == kTRUE || fdoFlat_STS_Barr == kTRUE) BinningFlattening();
  // TEST (flattening with fit function - no binning): for PSD & Qcorr only
  //if (fdoFlat_PSD_Qcorr == kTRUE || fdoFlat_STS_Qcorr == kTRUE || fdoFlat_PSD_Barr == kTRUE || fdoFlat_STS_Barr == kTRUE) FitFunctionFlattening();

  // ==== Q-recentering method

  for (Int_t k = 0; k < fNbinFlat; k++)
  {
      PSD_shift_Qx[k] = 0.;
      PSD_shift_Qy[k] = 0.;
      PSD_RMS_Qx[k] = 1.;
      PSD_RMS_Qy[k] = 1.;

      STS_harmo1_shift_Qx_sub1[k] = 0.;
      STS_harmo1_shift_Qy_sub1[k] = 0.;
      STS_harmo1_RMS_Qx_sub1[k] = 1.;
      STS_harmo1_RMS_Qy_sub1[k] = 1.;

      STS_harmo1_shift_Qx_sub2[k] = 0.;
      STS_harmo1_shift_Qy_sub2[k] = 0.;
      STS_harmo1_RMS_Qx_sub2[k] = 1.;
      STS_harmo1_RMS_Qy_sub2[k] = 1.;

      STS_harmo1_shift_Qx_full[k] = 0.;
      STS_harmo1_shift_Qy_full[k] = 0.;
      STS_harmo1_RMS_Qx_full[k] = 1.;
      STS_harmo1_RMS_Qy_full[k] = 1.;
      
      STS_harmo2_shift_Qx_sub1[k] = 0.;
      STS_harmo2_shift_Qy_sub1[k] = 0.;
      STS_harmo2_RMS_Qx_sub1[k] = 1.;
      STS_harmo2_RMS_Qy_sub1[k] = 1.;

      STS_harmo2_shift_Qx_sub2[k] = 0.;
      STS_harmo2_shift_Qy_sub2[k] = 0.;
      STS_harmo2_RMS_Qx_sub2[k] = 1.;
      STS_harmo2_RMS_Qy_sub2[k] = 1.;

      STS_harmo2_shift_Qx_full[k] = 0.;
      STS_harmo2_shift_Qy_full[k] = 0.;
      STS_harmo2_RMS_Qx_full[k] = 1.;
      STS_harmo2_RMS_Qy_full[k] = 1.;
  }

  //init for PSD
  if (fdoFlat_PSD_Qcorr == kTRUE && fileName_Qcorr == "")
  {
      LOG(FATAL) << "-E- eventPlane::Init: No input file for event plane flattening (Q-recentering)" << FairLogger::endl;
      return kERROR;
  }
  if (fdoFlat_PSD_Qcorr == kTRUE) PSDinit_flat_Qcorr();

  //init for STS
  if (fdoFlat_STS_Qcorr == kTRUE && fileName_Qcorr == "")
  {
      LOG(FATAL) << "-E- eventPlane::Init: No input file for event plane flattening (Q-recentering)" << FairLogger::endl;
      return kERROR;
  }
  if (fdoFlat_STS_Qcorr == kTRUE) STSinit_flat_Qcorr();

  cout << "==============" << endl;

  // ==== Barrette method

  for (Int_t k = 0; k < fNbinFlat; k++)
  {
      for (Int_t l = 0; l < 8; l++)
      {
	  PSD_mean_cosphi[k][l] = 0.;
	  PSD_mean_sinphi[k][l] = 0.;

          STS_harmo1_mean_cosphi_sub1[k][l] = 0.;
	  STS_harmo1_mean_sinphi_sub1[k][l] = 0.;

          STS_harmo1_mean_cosphi_sub2[k][l] = 0.;
	  STS_harmo1_mean_sinphi_sub2[k][l] = 0.;

          STS_harmo1_mean_cosphi_full[k][l] = 0.;
	  STS_harmo1_mean_sinphi_full[k][l] = 0.;
	  
	  STS_harmo2_mean_cosphi_sub1[k][l] = 0.;
	  STS_harmo2_mean_sinphi_sub1[k][l] = 0.;

          STS_harmo2_mean_cosphi_sub2[k][l] = 0.;
	  STS_harmo2_mean_sinphi_sub2[k][l] = 0.;

          STS_harmo2_mean_cosphi_full[k][l] = 0.;
	  STS_harmo2_mean_sinphi_full[k][l] = 0.;
      }
  }

  // init for PSD
  if (fdoFlat_PSD_Barr == kTRUE && fileName_Barr == "")
  {
      LOG(FATAL) << "-E- eventPlane::Init: No input file for PSD event plane flattening (Barrette method)" << FairLogger::endl;
      return kERROR;
  }
  if (fdoFlat_PSD_Barr == kTRUE) PSDinit_flat_Barr();

  // init for STS
  if (fdoFlat_STS_Barr == kTRUE && fileName_Barr == "")
  {
      LOG(FATAL) << "-E- eventPlane::Init: No input file for STS event plane flattening (Barrette method)" << FairLogger::endl;
      return kERROR;
  }
  if (fdoFlat_STS_Barr == kTRUE) STSinit_flat_Barr();

  cout << "-I- eventPlane: Intialisation successfull " << kSUCCESS<< endl;
  return kSUCCESS;

}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void eventPlane::Exec(Option_t* opt)
{
    if ( ! fHeader ) Fatal("Exec", "No event header");
    if ( ! flistPSDhit ) Fatal("Exec", "No PsdHit array");
    if ( ! flistSTSMCtrack ) Fatal("Exec", "No MCtrack array");

    PSDsecEdep();
    PSDmodEdep();
    PSDfullEdep();
    PSDtransverseMomMeth();

    if (fdoSTSreco == kTRUE)   
    {
      if ( ! flistSTSRECOtrack ) Fatal("Exec", "No reco. track array");
      if ( ! flistSTStrackMATCH ) Fatal("Exec", "No track match array");
      if ( ! flistPV ) Fatal("Exec", "No reco. PV array");
      
      STSRECOtransverseMomMeth();
    }
    else STSMCtransverseMomMeth();
}

// -----   Public method   --------------------------------------------

void eventPlane::WriteHistogramms()
{
    for (Int_t i=1; i<15; i++) hEsec->Fill(i, fedep_sec_av[i]);
    hEsec->Write();

    for (Int_t i=1; i<=finc_mod; i++) hEmod->Fill(fX_mod[i], fY_mod[i], fedep_mod_av[i]);
    hEmod->Write();

    hCoormod_sub1->Write();
    hCoormod_sub2->Write();
    hCoormod_sub3->Write();

    hY->Write();
    hP->Write();
    hIP->Write();
    hChi2toNDF->Write();
    
    hEmod->Delete();
    hCoormod_sub1->Delete();
    hCoormod_sub2->Delete();
    hCoormod_sub3->Delete();

    hY->Delete();
    hP->Delete();
    hIP->Delete();
    hChi2toNDF->Delete();
}

void eventPlane::Finish()
{
    // Finish of the task execution
    WriteHistogramms();
}

void eventPlane::BinningFlattening()
{
    // variable declaration
    for (int i = 0; i<fNbinFlat; i++)
    {
	b_STS[i] = 0.;
	b_PSD[i] = 0.;
        b_PSD1[i] = 0.;
        b_PSD2[i] = 0.;
        b_PSD3[i] = 0.;
    }
    double b_tmp[fNbinFlat];
    for (int i = 0; i<fNbinFlat; i++) b_tmp[i] = 0.;
    int bin[fNbinFlat];
    for (int i = 0; i<fNbinFlat; i++) bin[i] = 0;

    TString detinfo = "";
    TString sPSDsub1 = "centralmod_hole";
    TString sPSDsub2 = "1stCorona";
    TString sPSDsub3 = "2ndCorona";
    TString noSel = "";

    // ====== Input file and tree
    TFile *f = new TFile(fileName_Qcorr,"R");
    if (!f) cout << "-E- eventPlane::PSDinit_flat_Qcorr: pb with the correction file -----" << endl;
    TTree* tree = (TTree* ) f->Get("cbmsim");
    if (!tree) cout << "-E- eventPlane::PSDinit_flat_Qcorr: pb with the correction TTree -----" << endl;

    cout << "== Binning for event plane flattening " << endl;

    cout << "== STS " << endl;
    detinfo = "StsEvent.fmult";
    BinCalc(tree, detinfo, noSel, b_tmp);
    for (int i = 0; i<fNbinFlat; i++) b_STS[i] = b_tmp[i];
    for (int i = 0; i<fNbinFlat-1; i++) cout << "mult (cut " << i+1 << ") - stat: " << b_STS[i] << endl;

    cout << "== PSD " << endl;
    for (int i = 0; i<fNbinFlat; i++) b_tmp[i] = 0.;
    detinfo = "PsdEvent.fedep_" + sPSDsub1 + "+PsdEvent.fedep_" + sPSDsub2 + "+PsdEvent.fedep_" + sPSDsub3;
    BinCalc(tree, detinfo, sPreSel, b_tmp);
    for (int i = 0; i<fNbinFlat; i++) b_PSD[i] = b_tmp[i];
    for (int i = 0; i<fNbinFlat-1; i++) cout << "mult (cut " << i+1 << ") - stat: " << b_PSD[i] << endl;

    cout << "== PSD sub-event 1" << endl;
    for (int i = 0; i<fNbinFlat; i++) b_tmp[i] = 0.;
    detinfo = "PsdEvent.fedep_" + sPSDsub1;
    BinCalc(tree, detinfo, sPreSel, b_tmp);
    for (int i = 0; i<fNbinFlat; i++) b_PSD1[i] = b_tmp[i];
    for (int i = 0; i<fNbinFlat-1; i++) cout << "mult (cut " << i+1 << ") - stat: " << b_PSD1[i] << endl;

    cout << "== PSD sub-event 2" << endl;
    for (int i = 0; i<fNbinFlat; i++) b_tmp[i] = 0.;
    detinfo = "PsdEvent.fedep_" + sPSDsub2;
    BinCalc(tree, detinfo, sPreSel, b_tmp);
    for (int i = 0; i<fNbinFlat; i++) b_PSD2[i] = b_tmp[i];
    for (int i = 0; i<fNbinFlat-1; i++) cout << "mult (cut " << i+1 << ") - stat: " << b_PSD2[i] << endl;

    cout << "== PSD sub-event 3" << endl;
    for (int i = 0; i<fNbinFlat; i++) b_tmp[i] = 0.;
    detinfo = "PsdEvent.fedep_" + sPSDsub3;
    BinCalc(tree, detinfo, sPreSel, b_tmp);
    for (int i = 0; i<fNbinFlat; i++) b_PSD3[i] = b_tmp[i];
    for (int i = 0; i<fNbinFlat-1; i++) cout << "mult (cut " << i+1 << ") - stat: " << b_PSD3[i] << endl;
}

void eventPlane::BinCalc(TTree *&tree, TString &detinfo, TString &sPreSel_tmp, double *b_tmp)
{
    TH1F* h;
    double Nentries = 0.;
    TAxis* Xaxis;
    int NXbin = 0;
    double maxMult = 0.;
    double step = 0.;
    double entry = 0.;
    int j=0;
    int bin[fNbinFlat];
    for (int i = 0; i<fNbinFlat; i++) bin[i] = 0;

    //TCanvas* can = new TCanvas(detinfo,detinfo,200,10,500,500);
    tree->Draw(detinfo +">>"+detinfo, sPreSel_tmp, "goff");
    h = (TH1F*) tree->GetHistogram();
    Xaxis = h->GetXaxis();
    NXbin = Xaxis->GetLast();

    Nentries = h->Integral();
    cout << "% event selected: " <<  Nentries << endl;

    for (int i=0; i<NXbin; i++) if (h->GetBinContent(i) != 0. && maxMult < h->GetBinCenter(i)) maxMult = h->GetBinCenter(i);
    cout << "max (mult/E) = " << maxMult << endl;

    // bins with fixed multiplicity step
    if (doFixStat == kFALSE)
    {
	cout << "== Method with fixed multiplicity" << endl;

	step = maxMult/fNbinFlat;
	cout << "step = " << step << endl;

	for (int i=0;i<fNbinFlat-1;i++) b_tmp[i] = (i+1)*step;

	//sPreSel_tmp = "StsEvent.fmult > ";
	//sPreSel_tmp += b_tmp[fNbinFlat-2];
	//tree->Draw("McEvent.fB>>hb(170, 0.,17)", sPreSel_tmp);
    }
    // bins with fixed collision statistics
    else
    {
	cout << "== Method with fixed collision statistics" << endl;

	step = 1./fNbinFlat;
	cout << "step = " << step << endl;

	for( Int_t i=0; i<NXbin; i++ )
	{
	    entry += h->GetBinContent(i);
	    if (entry >= ((j+1)*step*Nentries) && b_tmp[j] == 0.)
	    {
		b_tmp[j] = h->GetBinCenter(i);
		j++;
	    }
	}
	//cout << "j = " << j << endl;
    }

    /*for (int i=0;i<fNbinFlat;i++)
    {
	if (i==0)
	{
	    bin[i] = h->FindBin(b_tmp[i]);
	    cout << "mult (cut " << i+1 << "): " << b_tmp[i] << ", % sample: " << 100*h->Integral(0, bin[i])/Nentries << endl;
	}
	else
	{
	    bin[i-1] = h->FindBin(b_tmp[i-1]);
	    if (i==fNbinFlat-1) cout << "mult (cut " << i+1 << "): " << b_tmp[i-1] << ", % sample: " << 100*h->Integral(bin[i-1],NXbin)/Nentries << endl;
	    else
	    {
		bin[i] = h->FindBin(b_tmp[i]);
		cout << "mult (cut " << i+1 << "): " << b_tmp[i] << ", % sample: " << 100*h->Integral(bin[i-1],bin[i])/Nentries << endl;
	    }
	}
    }*/
}

// TEST (flattening with fit function - no binning): for PSD & Qcorr only
void eventPlane::FitFunctionFlattening() 
{
    TString detinfo = "";
    TString sPSDsub1 = "centralmod_hole";
    TString sPSDsub2 = "1stCorona";
    TString sPSDsub3 = "2ndCorona";
    TString noSel = "";

    TString Qx1 = "PsdEvent.fQx_sub1";
    TString Qx2 = "PsdEvent.fQx_sub2";
    TString Qx3 = "PsdEvent.fQx_sub3";
    TString Qx =  "PsdEvent.fQx";

    TString Qy1 = "PsdEvent.fQy_sub1";
    TString Qy2 = "PsdEvent.fQy_sub2";
    TString Qy3 = "PsdEvent.fQy_sub3";
    TString Qy =  "PsdEvent.fQy";

    // ====== Input file and tree
    TFile *f = new TFile(fileName_Qcorr,"R");
    if (!f) cout << "-E- eventPlane::PSDinit_flat_Qcorr: pb with the correction file -----" << endl;
    TTree* tree = (TTree* ) f->Get("cbmsim");
    if (!tree) cout << "-E- eventPlane::PSDinit_flat_Qcorr: pb with the correction TTree -----" << endl;

    cout << "== TEST (flattening with fit function - no binning): for PSD only " << endl;

    cout << "== PSD " << endl;
    detinfo = "PsdEvent.fedep_" + sPSDsub1 + "+PsdEvent.fedep_" + sPSDsub2 + "+PsdEvent.fedep_" + sPSDsub3;
    FitFunctionCalc(tree, detinfo, Qx, sPreSel, 0, 0);
    FitFunctionCalc(tree, detinfo, Qy, sPreSel, 0, 2);

    cout << "== PSD - sub-event 1 " << endl;
    detinfo = "PsdEvent.fedep_" + sPSDsub1;
    FitFunctionCalc(tree, detinfo, Qx1, sPreSel, 1, 0);
    FitFunctionCalc(tree, detinfo, Qy1, sPreSel, 1, 2);

    cout << "== PSD - sub-event 2 " << endl;
    detinfo = "PsdEvent.fedep_" + sPSDsub2;
    FitFunctionCalc(tree, detinfo, Qx2, sPreSel, 2, 0);
    FitFunctionCalc(tree, detinfo, Qy2, sPreSel, 2, 2);

    cout << "== PSD - sub-event 3 " << endl;
    detinfo = "PsdEvent.fedep_" + sPSDsub3;
    FitFunctionCalc(tree, detinfo, Qx3, sPreSel, 3, 0);
    FitFunctionCalc(tree, detinfo, Qy3, sPreSel, 3, 2);
}

// TEST (flattening with fit function - no binning): for PSD & Qcorr only
void eventPlane::FitFunctionCalc(TTree *&tree, TString &detinfo_tmp, TString &Q_tmp, TString &sPreSel_tmp, int meth, int XY) 
{
    //cout <<"variables = " << sPreSel_tmp << ", " << detinfo_tmp << ", " << Q_tmp <<  endl;

    TH1F* h;
    int NbinX = 0;
    TH2F* h2;
    TH1D *h2_projX;
    TH1D *h2_projY;
    TH1D *mean;
    TH1D *rms;
    TString spoln = "pol5";
    double minX, maxX, minY, maxY;
    TString sminX="";
    TString smaxX="";
    TString sminY="";
    TString smaxY="";

    TString sname = "hfitfunct_";
    sname += meth;
    sname += XY;
    
    tree->Draw(Q_tmp + ":" + detinfo_tmp + ">>"+sname, sPreSel_tmp, "goff");
    h2 = (TH2F*) tree->GetHistogram();
    h2_projX = h2->ProjectionX(Q_tmp + ":" + detinfo_tmp + "_tmpX",0,-1);
    h2_projY = h2->ProjectionY(Q_tmp + ":" + detinfo_tmp + "_tmpY",0,-1);

    minX = h2_projX->GetBinCenter(h2_projX->FindFirstBinAbove())-h2_projX->GetBinWidth(0)/2.;
    maxX = h2_projX->GetBinCenter(h2_projX->FindLastBinAbove())+h2_projX->GetBinWidth(0)/2.;
    minY = h2_projY->GetBinCenter(h2_projY->FindFirstBinAbove())-h2_projY->GetBinWidth(0)/2.;
    maxY = h2_projY->GetBinCenter(h2_projY->FindLastBinAbove())+h2_projY->GetBinWidth(0)/2.;

    minX = TMath::Floor(minX);
    maxX = TMath::Ceil(maxX);
    minY = TMath::Floor(minY);
    maxY = TMath::Ceil(maxY);
    //cout << "(min X, maxX, min Y, max Y)" << minX <<", "<< maxX <<", "<< minY <<", "<< maxY << endl;;
    sminX += minX;
    smaxX += maxX;
    sminY += minY;
    smaxY += maxY;

    //TCanvas* can = new TCanvas(Q_tmp + ":" + detinfo_tmp, Q_tmp + ":" + detinfo_tmp,200,10,1500,500);
    //can->Divide(3, 1);
    //can->cd(1);
    tree->Draw(Q_tmp + ":" + detinfo_tmp + ">>"+sname+"_rescaled(40, "+sminX+", "+smaxX+", 1000, "+sminY+", "+smaxY+")", sPreSel_tmp, "goff");
    h2 = (TH2F*) tree->GetHistogram();
    NbinX = h2->GetNbinsX();

    h2_projX = h2->ProjectionX(Q_tmp + ":" + detinfo_tmp + "_tmpXX",0,-1);
    mean = (TH1D*) h2_projX->Clone("mean");
    rms = (TH1D*) h2_projX->Clone("rms");
    mean->Reset();
    rms->Reset();

    TH1D *sl[NbinX];
    //h2->ProjectionY("_tmp2",0,-1)->Draw();
    //int inc = 0;
    for (int i=0;i<NbinX;i++)
    {
	sl[i] = (TH1D*)h2->ProjectionY(Form("%i",i),i,i+1);
        //inc = 0;
	//for (int j=0;j<sl[i]->GetNbinsX();j++) if (sl[i]->GetBinContent(j)!=0.) inc++;
	//if (inc < 5) cout << "WARNING: too few non-empty bins: " << inc << " in x-slice nb: " << i << "("<< h2_projX->GetBinCenter(i) << ")" << endl;

	sl[i]->Draw("same, goff");
	sl[i]->SetLineColor(i+1);
	if(sl[i]->GetEntries())
	{
	    mean->SetBinContent(i,sl[i]->GetMean());
            mean->SetBinError(i,sl[i]->GetMeanError());
	    rms->SetBinContent(i,sl[i]->GetRMS());
            rms->SetBinError(i,sl[i]->GetRMSError());
	}
    }

    //can->cd(2);
    //mean->Draw();
    mean->Fit(spoln,"0, Q", "", 0, h2_projX->GetMaximum());
    funct[meth][XY] = mean->GetFunction(spoln);
    //funct[meth][XY]->Draw("same");

    //can->cd(3);
    //rms->Draw();
    rms->Fit(spoln,"0, Q", "", 0, h2_projX->GetMaximum());
    funct[meth][XY+1] = rms->GetFunction(spoln);
    //funct[meth][XY+1]->Draw("same");
}
// ======================= PSD member functions

void eventPlane::PSDcoordinates()
{
    // calculate the phi angle and distance to origin of center of modules (X and Y lab-coordinate taken from txt file)
        
    ifstream fxypos(fileName_PsdModCoord);      // to change: file in input -> later: mod coord. in PsdHit?
    for (Int_t i=1; i<100; i++) // to change if different number of PSD modules: use fxypos.end() -> continue? then inc for the other loops on module?
    {
	if (fxypos.eof()) break;
	
	fxypos >> fX_mod[i] >> fY_mod[i];
	cout << "module " << i << ": (X, Y) = (" << fX_mod[i] << ", " << fY_mod[i] << ")" << endl;
	
	finc_mod++;
    }
    fxypos.close();
    
    finc_mod -= 1;    
    cout << "evenPlane::PSDcoordinates : #modules = "<< finc_mod << endl;

    for (Int_t i=1; i<=finc_mod; i++) // to change if different number of PSD modules
    {
	fphi_mod[i] = TMath::ATan2(fY_mod[i], fX_mod[i]);
	fR_mod[i] = TMath::Sqrt(fX_mod[i] * fX_mod[i] + fY_mod[i] * fY_mod[i]);
    }
}

void eventPlane::PSDmodEdep()
{
    // calculate energy deposit / module
  
    for (Int_t i=0; i<100; i++) { fedep_mod[i] = 0.; }  // to change if different number of PSD modules

    Int_t nPSDhits = flistPSDhit->GetEntriesFast();
    cout << "evenPlane::PSDmodEdep : # PSD hits = " << nPSDhits << endl;

    CbmPsdHit* hit = NULL;
    Int_t mod;

    for (Int_t ihit=0; ihit<nPSDhits; ihit++) 
    {
	hit = (CbmPsdHit*) flistPSDhit->At(ihit);
	if ( ! hit ) continue;
	mod = hit->GetModuleID();
	fedep_mod[mod] += hit->GetEdep();
    }

    for (Int_t i=1; i<=finc_mod; i++) fedep_mod_av[i] += fedep_mod[i];
}

void eventPlane::PSDsecEdep()
{
    // calculate energy deposit / section

    Int_t nPSDdigits = flistPSDdigit->GetEntriesFast();
    cout << "evenPlane::PSDsecEdep : # PSD digits = " << nPSDdigits << endl;

    CbmPsdDigi* digit = NULL;
    Int_t sec;

    for (Int_t idigit=0; idigit<nPSDdigits; idigit++)
    {
	digit = (CbmPsdDigi*) flistPSDdigit->At(idigit);
	if ( ! digit ) continue;
	sec = digit->GetSectionID();
	
	fedep_sec_av[sec] += digit->GetEdep();
    }
}

void eventPlane::PSDfullEdep()
{
    // calculate full energy deposit
  
    Double_t edep_centralmod_nohole = 0.;
    Double_t edep_centralmod_hole = 0.;
    Double_t edep_1stCorona = 0.;
    Double_t edep_2ndCorona = 0.;

    int ishift = 0;

    for (Int_t i=1; i<=finc_mod; i++)  // to change if different number of PSD modules
    {
        // == to shift sub-event definition left (+1) or right (-1)
        //ishift = i+1;
        //ishift = i-1;
        ishift = i;

        // PSD geometry with 60 modules (detector center = 4 central modules' center)
        /*if (ishift==26 || ishift==27 || ishift==34 || ishift==35) edep_centralmod_nohole += fedep_mod[ishift];  // for now, excluding hole not possible at hit level (after reco.)
	if (ishift==26 || ishift==27 || ishift==34 || ishift==35) edep_centralmod_hole += fedep_mod[ishift];

	if (ishift==17 || ishift==18 || ishift==19 || ishift==20 || ishift==25 || ishift==28 ||
	    ishift==33 || ishift==36 || ishift==41 || ishift==42 || ishift==43 || ishift==44 ) edep_1stCorona += fedep_mod[ishift];
	else
	{
            if (ishift!=26 && ishift!=27 && ishift!=34 && ishift!=35) edep_2ndCorona += fedep_mod[ishift];
	}*/

	// == Convert: 60 mods -> 44 mods (not affected by ishift, pure detector geometry modification)
	/*if ( i == 1 || i == 2 ||i == 3 ||i == 4 ||i == 5 || i == 6 ||
	     i == 55 || i == 56 ||i == 57 ||i == 58 ||i == 59 || i == 60 ||
	     i == 7 || i == 14 ||i == 47 ||i == 54 ) continue;*/   

	// == Convert: 44 mods -> 32 mods (not affected by ishift, pure detector geometry modification)
        /*if ( i == 15 || i == 23 || i == 31 || i == 39 ||
             i == 22 || i == 30 || i == 38 || i == 46 ||
             i == 8 || i == 13 || i == 48 || i == 53 ) continue;*/ 

        // PSD geometry with 44 modules (detector center = 4 central modules' center)
        //if (ishift==18 || ishift==19 || ishift==26 || ishift==27) edep_centralmod_nohole += fedep_mod[ishift];  // for now, excluding hole not possible at hit level (after reco.)
	if (ishift==18 || ishift==19 || ishift==26 || ishift==27) edep_centralmod_hole += fedep_mod[ishift];

	if (ishift==9 || ishift==10 || ishift==11 || ishift==12 || ishift==17 || ishift==20 ||
	    ishift==33 || ishift==34 || ishift==35 || ishift==36 || ishift==25 || ishift==28 ) edep_1stCorona += fedep_mod[ishift];
	else
	{
            if (ishift!=18 && ishift!=19 && ishift!=26 && ishift!=27) edep_2ndCorona += fedep_mod[ishift];
	}

	// PSD geometry with 48 modules (detector center = 4 central modules' center)
        /*
        //if (ishift==18 || ishift==19 || ishift==26 || ishift==27) edep_centralmod_nohole += fedep_mod[ishift];  // for now, excluding hole not possible at hit level (after reco.)
	if (ishift==19 || ishift==20 || ishift==29 || ishift==30) edep_centralmod_hole += fedep_mod[ishift];

	if (ishift==9 || ishift==10 || ishift==11 || ishift==12 || ishift==18 || ishift==21 ||
	    ishift==37 || ishift==38 || ishift==39 || ishift==40 || ishift==28 || ishift==31 ) edep_1stCorona += fedep_mod[ishift];
	else
	{
            if (ishift!=19 && ishift!=20 && ishift!=29 && ishift!=30) edep_2ndCorona += fedep_mod[ishift];
	}*/
    }

    fCbmPsdEvent->SetEdep_full(edep_centralmod_nohole, edep_centralmod_hole, edep_1stCorona, edep_2ndCorona);
}

void eventPlane::PSDtransverseMomMeth()
{
    Int_t evtID = fHeader->GetEventID();
    Double_t B = fHeader->GetB();
    Double_t RP = fHeader->GetPhi(); // RP = Reaction plane: MC info (in radian)
    if ( RP > fPi ) RP -= 2 * fPi;   // !! from SHIELD: [0, 2pi]

    cout << "----- event number: " << evtID << " ------" << endl;
    cout << "----- imp: " << B << " ------" << endl;
    cout << "----- RP angle: " << RP << " ------" << endl;

    // MC stored in seperate branch "McEvent" in output
    fMCEventData->SetB(B);
    fMCEventData->SetRP(RP);
    
    fCbmPsdEvent->SetEvtID(evtID);               

    // Binning with detector info. -- REQUIRED: PSDfullEdep()
    double E1 = (double)fCbmPsdEvent->GetE1();
    double E2 = (double)fCbmPsdEvent->GetE2();
    double E3 = (double)fCbmPsdEvent->GetE3();
    double E = E1+E2+E3;

    Double_t Qx = 0.;
    Double_t Qy = 0.;
    Double_t EP = 0.;
    Double_t deltaEP = 0.;

    Double_t Qx_sub1 = 0.;
    Double_t Qy_sub1 = 0.;
    Double_t EP_sub1 = 0.;
    Double_t deltaEP_sub1 = 0.;

    Double_t Qx_sub2 = 0.;
    Double_t Qy_sub2 = 0.;
    Double_t EP_sub2 = 0.;
    Double_t deltaEP_sub2 = 0.;

    Double_t Qx_sub3 = 0.;
    Double_t Qy_sub3 = 0.;
    Double_t EP_sub3 = 0.;
    Double_t deltaEP_sub3 = 0.;

    int ishift = 0;

    for (Int_t i=1; i<=finc_mod; i++) // to change if different number of PSD modules
    {
	fR_mod[i] = 1.;       // right method: no weight apart from module energy !!
	  
	Qx += TMath::Cos(fphi_mod[i]) * fR_mod[i] * fedep_mod[i];
	Qy += TMath::Sin(fphi_mod[i]) * fR_mod[i] * fedep_mod[i];

        // == to shift sub-event definition left (+1) or right (-1)
        //ishift = i+1;
        //ishift = i-1;
        ishift = i;

	// !!!!!!!!! REQUIRED !!!!!!!!!: PSD geometry with 60 modules (detector center = 4 central modules' center)
	/*if (ishift==26 || ishift==27 || ishift==34 || ishift==35)
	{
	    Qx_sub1 += TMath::Cos(fphi_mod[ishift]) * fR_mod[ishift] * fedep_mod[ishift];
	    Qy_sub1 += TMath::Sin(fphi_mod[ishift]) * fR_mod[ishift] * fedep_mod[ishift];

            hCoormod_sub1->Fill(fX_mod[ishift], fY_mod[ishift]);
	}
	if (ishift==17 || ishift==18 || ishift==19 || ishift==20 || ishift==25 || ishift==28 ||
	    ishift==33 || ishift==36 || ishift==41 || ishift==42 || ishift==43 || ishift==44 )
	{
	    Qx_sub2 += TMath::Cos(fphi_mod[ishift]) * fR_mod[ishift] * fedep_mod[ishift];
	    Qy_sub2 += TMath::Sin(fphi_mod[ishift]) * fR_mod[ishift] * fedep_mod[ishift];

            hCoormod_sub2->Fill(fX_mod[ishift], fY_mod[ishift]);
	}
	else
	{
	    if (ishift!=26 && ishift!=27 && ishift!=34 && ishift!=35)
	    {
		Qx_sub3 += TMath::Cos(fphi_mod[ishift]) * fR_mod[ishift] * fedep_mod[ishift];
		Qy_sub3 += TMath::Sin(fphi_mod[ishift]) * fR_mod[ishift] * fedep_mod[ishift];

                hCoormod_sub3->Fill(fX_mod[ishift], fY_mod[ishift]);
	    }
	}*/

        // Convert: 60 mods -> 44 mods (not affected by ishift, pure detector geometry modification)
        /*if ( i == 1 || i == 2 ||i == 3 ||i == 4 ||i == 5 || i == 6 ||
	     i == 55 || i == 56 ||i == 57 ||i == 58 ||i == 59 || i == 60 ||
	     i == 7 || i == 14 ||i == 47 ||i == 54 ) continue; */  

	// Convert: 44 mods -> 32 mods (not affected by ishift, pure detector geometry modification)
	/*if ( i == 15 || i == 23 || i == 31 || i == 39 ||
             i == 22 || i == 30 || i == 38 || i == 46 ||
             i == 8 || i == 13 || i == 48 || i == 53 ) continue;*/

        // !!!!!!!!! REQUIRED !!!!!!!!!: PSD geometry with 44 modules (detector center = 4 central modules' center)
	if (ishift==18 || ishift==19 || ishift==26 || ishift==27)
	{
	    Qx_sub1 += TMath::Cos(fphi_mod[ishift]) * fR_mod[ishift] * fedep_mod[ishift];
	    Qy_sub1 += TMath::Sin(fphi_mod[ishift]) * fR_mod[ishift] * fedep_mod[ishift];

            hCoormod_sub1->Fill(fX_mod[ishift], fY_mod[ishift]);
	}
	if (ishift==9 || ishift==10 || ishift==11 || ishift==12 || ishift==17 || ishift==20 ||
	    ishift==33 || ishift==34 || ishift==35 || ishift==36 || ishift==25 || ishift==28 )
	{
	    Qx_sub2 += TMath::Cos(fphi_mod[ishift]) * fR_mod[ishift] * fedep_mod[ishift];
	    Qy_sub2 += TMath::Sin(fphi_mod[ishift]) * fR_mod[ishift] * fedep_mod[ishift];

            hCoormod_sub2->Fill(fX_mod[ishift], fY_mod[ishift]);
	}
	else
	{
	    if (ishift!=18 && ishift!=19 && ishift!=26 && ishift!=27)
	    {
		Qx_sub3 += TMath::Cos(fphi_mod[ishift]) * fR_mod[ishift] * fedep_mod[ishift];
		Qy_sub3 += TMath::Sin(fphi_mod[ishift]) * fR_mod[ishift] * fedep_mod[ishift];

                hCoormod_sub3->Fill(fX_mod[ishift], fY_mod[ishift]);
	    }
	}

        // !!!!!!!!! REQUIRED !!!!!!!!!: PSD geometry with 48 modules (detector center = 4 central modules' center)
        /*
	if (ishift==19 || ishift==20 || ishift==29 || ishift==30)
	{
	    Qx_sub1 += TMath::Cos(fphi_mod[ishift]) * fR_mod[ishift] * fedep_mod[ishift];
	    Qy_sub1 += TMath::Sin(fphi_mod[ishift]) * fR_mod[ishift] * fedep_mod[ishift];

            hCoormod_sub1->Fill(fX_mod[ishift], fY_mod[ishift]);
	}

	if (ishift==9 || ishift==10 || ishift==11 || ishift==12 || ishift==18 || ishift==21 ||
	    ishift==37 || ishift==38 || ishift==39 || ishift==40 || ishift==28 || ishift==31 )
	{
	    Qx_sub2 += TMath::Cos(fphi_mod[ishift]) * fR_mod[ishift] * fedep_mod[ishift];
	    Qy_sub2 += TMath::Sin(fphi_mod[ishift]) * fR_mod[ishift] * fedep_mod[ishift];

            hCoormod_sub2->Fill(fX_mod[ishift], fY_mod[ishift]);
	}
	else
	{
	    if (ishift!=19 && ishift!=20 && ishift!=29 && ishift!=30)
	    {
		Qx_sub3 += TMath::Cos(fphi_mod[ishift]) * fR_mod[ishift] * fedep_mod[ishift];
		Qy_sub3 += TMath::Sin(fphi_mod[ishift]) * fR_mod[ishift] * fedep_mod[ishift];

                hCoormod_sub3->Fill(fX_mod[ishift], fY_mod[ishift]);
	    }
	}*/

    }

    //---- event plane flattening: Q-recentering
    
    if (fdoFlat_PSD_Qcorr == kTRUE)
    {
        Int_t k;

        //  Binning with fixed MC b-step
	//if (B < 2) k = 0;
	//if (B >= 2 && B < 4) k = 1;
	//if (B >= 4 && B < 6) k = 2;
	//if (B >= 6 && B < 8) k = 3;
	//if (B >= 8 && B < 10) k = 4;
	//if (B >= 10 && B < 12) k = 5;
	//if (B >= 12) k = 6;
        
        k = FlatBinWhichK(E, b_PSD);

	Qx -= PSD_shift_Qx[k];
	Qx /= PSD_RMS_Qx[k];
        
	Qy -= PSD_shift_Qy[k];
	Qy /= PSD_RMS_Qy[k];

        // Sub PSD
        k = FlatBinWhichK(E1, b_PSD1);

        Qx_sub1 -= PSD_shift_Qx_sub1[k];
	Qx_sub1 /= PSD_RMS_Qx_sub1[k];
	Qy_sub1 -= PSD_shift_Qy_sub1[k];
	Qy_sub1 /= PSD_RMS_Qy_sub1[k];

        k = FlatBinWhichK(E2, b_PSD2);

        Qx_sub2 -= PSD_shift_Qx_sub2[k];
	Qx_sub2 /= PSD_RMS_Qx_sub2[k];
	Qy_sub2 -= PSD_shift_Qy_sub2[k];
	Qy_sub2 /= PSD_RMS_Qy_sub2[k];

        k = FlatBinWhichK(E3, b_PSD3);

        Qx_sub3 -= PSD_shift_Qx_sub3[k];
	Qx_sub3 /= PSD_RMS_Qx_sub3[k];
	Qy_sub3 -= PSD_shift_Qy_sub3[k];
	Qy_sub3 /= PSD_RMS_Qy_sub3[k];
    }

    // TEST (flattening with fit function - no binning): for PSD & Qcorr only
    /*if (fdoFlat_PSD_Qcorr == kTRUE)
    {
	Qx -= funct[0][0]->Eval(E);
	Qx /= funct[0][1]->Eval(E);

        Qy -= funct[0][2]->Eval(E);
	Qy /= funct[0][3]->Eval(E);

	//cout << "E1, funct[1][0]->Eval(E1), funct[1][1]->Eval(E1): " << E1 << ", " << funct[1][0]->Eval(E1) << ", " << funct[1][1]->Eval(E1) << endl;

        // sub-event 1
        Qx_sub1 -= funct[1][0]->Eval(E1);
	Qx_sub1 /= funct[1][1]->Eval(E1);

        Qy_sub1 -= funct[1][2]->Eval(E1);
	Qy_sub1 /= funct[1][3]->Eval(E1);

        // sub-event 2
	Qx_sub2 -= funct[2][0]->Eval(E2);
	Qx_sub2 /= funct[2][1]->Eval(E2);

        Qy_sub2 -= funct[2][2]->Eval(E2);
        Qy_sub2 /= funct[2][3]->Eval(E2);

        // sub-event 3
	Qx_sub3 -= funct[3][0]->Eval(E3);
	Qx_sub3 /= funct[3][1]->Eval(E3);

        Qy_sub3 -= funct[3][2]->Eval(E3);
        Qy_sub3 /= funct[3][3]->Eval(E3);
    }*/
    //

    EP = TMath::ATan2( Qy, Qx );

    EP_sub1 = TMath::ATan2( Qy_sub1, Qx_sub1 );
    EP_sub2 = TMath::ATan2( Qy_sub2, Qx_sub2 );
    EP_sub3 = TMath::ATan2( Qy_sub3, Qx_sub3 );

    // ---- event plane flattening: Barrette method
    
    if (fdoFlat_PSD_Barr == kTRUE)
    {
	Int_t k, m;

        //  Binning with fixed MC b-step
	//if (B < 2) k = 0;
	//if (B >= 2 && B < 4) k = 1;
	//if (B >= 4 && B < 6) k = 2;
	//if (B >= 6 && B < 8) k = 3;
	//if (B >= 8 && B < 10) k = 4;
	//if (B >= 10 && B < 12) k = 5;
	//if (B >= 12) k = 6;

	for (m = 0; m < 8; m++)
	{
            k = FlatBinWhichK(E, b_PSD);
	    EP += (2/(m+1)) * ( - PSD_mean_sinphi[k][m] * TMath::Cos( (m+1) * EP ) + PSD_mean_cosphi[k][m] * TMath::Sin( (m+1) * EP ) );

            // Sub PSD
            k = FlatBinWhichK(E1, b_PSD1);
	    EP_sub1 += (2/(m+1)) * ( - PSD_mean_sinphi_sub1[k][m] * TMath::Cos( (m+1) * EP_sub1 ) + PSD_mean_cosphi_sub1[k][m] * TMath::Sin( (m+1) * EP_sub1 ) );
            k = FlatBinWhichK(E2, b_PSD2);
	    EP_sub2 += (2/(m+1)) * ( - PSD_mean_sinphi_sub2[k][m] * TMath::Cos( (m+1) * EP_sub2 ) + PSD_mean_cosphi_sub2[k][m] * TMath::Sin( (m+1) * EP_sub2 ) );
            k = FlatBinWhichK(E3, b_PSD3);
	    EP_sub3 += (2/(m+1)) * ( - PSD_mean_sinphi_sub3[k][m] * TMath::Cos( (m+1) * EP_sub3 ) + PSD_mean_cosphi_sub3[k][m] * TMath::Sin( (m+1) * EP_sub3 ) );
	}
    }

    // diff RECO phi - MC phi in proper range [-pi, pi] -> for drawing purposes      
    deltaEP = EP - RP;
    deltaEP = Range(deltaEP);

    // Sub PSD

    deltaEP_sub1 = EP_sub1 - RP;
    deltaEP_sub1 = Range(deltaEP_sub1);

    deltaEP_sub2 = EP_sub2 - RP;
    deltaEP_sub2 = Range(deltaEP_sub2);

    deltaEP_sub3 = EP_sub3 - RP;
    deltaEP_sub3 = Range(deltaEP_sub3);

    // Set PSD event

    fCbmPsdEvent->SetQvect(Qx, Qy, Qx_sub1, Qy_sub1, Qx_sub2, Qy_sub2, Qx_sub3, Qy_sub3);
    fCbmPsdEvent->SetEP(EP, EP_sub1, EP_sub2, EP_sub3);
    fCbmPsdEvent->SetdeltaEP(deltaEP, deltaEP_sub1, deltaEP_sub2, deltaEP_sub3);
}

//---- event plane flattening: Q-recentering, init for PSD

void eventPlane::PSDinit_flat_Qcorr()
{
    cout << "-- correction file for PSD EP (Q-recentering): " << fileName_Qcorr << endl;

    TFile *f = new TFile(fileName_Qcorr,"R");
    if (!f) cout << "-E- eventPlane::PSDinit_flat_Qcorr: pb with the correction file -----" << endl;
    TTree* tree = (TTree* ) f->Get("cbmsim");
    if (!tree) cout << "-E- eventPlane::PSDinit_flat_Qcorr: pb with the correction TTree -----" << endl;

    TString cut;
    Int_t k, l;
    TString cut_PSD;
    TString cut_PSD1;
    TString cut_PSD2;
    TString cut_PSD3;

    TString detinfo = "";
    TString sPSDsub1 = "centralmod_hole";
    TString sPSDsub2 = "1stCorona";
    TString sPSDsub3 = "2ndCorona";

    TH1F* h1;
    TH1F* h2;
    TH1F* h11;
    TH1F* h12;
    TH1F* h21;
    TH1F* h22;
    TH1F* h31;
    TH1F* h32;

    for (k = 0; k < fNbinFlat; k++)
    {
        // Binning with fixed MC b-step
	//l = 2 * k;
	//if (k < 6) cut = Form("McEvent.fB >= %.1d && McEvent.fB < %.1d", l, l + 2);
	//else cut = Form("McEvent.fB >= %.1d", l);

        if (k==0)
	{
            detinfo = "PsdEvent.fedep_" + sPSDsub1 + "+PsdEvent.fedep_" + sPSDsub2 + "+PsdEvent.fedep_" + sPSDsub3;
	    cut_PSD = Form(detinfo + " <= %.1f", b_PSD[k]);

            detinfo = "PsdEvent.fedep_" + sPSDsub1;
	    cut_PSD1 = Form(detinfo + " <= %.1f", b_PSD1[k]);

            detinfo = "PsdEvent.fedep_" + sPSDsub2;
	    cut_PSD2 = Form(detinfo + " <= %.1f", b_PSD2[k]);

            detinfo = "PsdEvent.fedep_" + sPSDsub3;
	    cut_PSD3 = Form(detinfo + " <= %.1f", b_PSD3[k]);
	}
	else
	{
	    
	    if (k==fNbinFlat-1)
	    {
		detinfo = "PsdEvent.fedep_" + sPSDsub1 + "+PsdEvent.fedep_" + sPSDsub2 + "+PsdEvent.fedep_" + sPSDsub3;
		cut_PSD = Form(detinfo + " > %.1f", b_PSD[k-1]);

		detinfo = "PsdEvent.fedep_" + sPSDsub1;
		cut_PSD1 = Form(detinfo + " > %.1f", b_PSD1[k-1]);

		detinfo = "PsdEvent.fedep_" + sPSDsub2;
		cut_PSD2 = Form(detinfo + " > %.1f", b_PSD2[k-1]);

		detinfo = "PsdEvent.fedep_" + sPSDsub3;
		cut_PSD3 = Form(detinfo + " > %.1f", b_PSD3[k-1]);
	    }
	    else
	    {
                detinfo = "PsdEvent.fedep_" + sPSDsub1 + "+PsdEvent.fedep_" + sPSDsub2 + "+PsdEvent.fedep_" + sPSDsub3;
		cut_PSD = Form(detinfo + " > %.1f && " + detinfo + " <= %.1f", b_PSD[k-1], b_PSD[k]);

		detinfo = "PsdEvent.fedep_" + sPSDsub1;
		cut_PSD1 = Form(detinfo + " > %.1f && " + detinfo + " <= %.1f", b_PSD1[k-1], b_PSD1[k]);

		detinfo = "PsdEvent.fedep_" + sPSDsub2;
		cut_PSD2 = Form(detinfo + " > %.1f && " + detinfo + " <= %.1f", b_PSD2[k-1], b_PSD2[k]);

		detinfo = "PsdEvent.fedep_" + sPSDsub3;
		cut_PSD3 = Form(detinfo + " > %.1f && " + detinfo + " <= %.1f", b_PSD3[k-1], b_PSD3[k]);
	    }
	}

	cut =  cut_PSD;
        cut += " && " +sPreSel;
	//cout << "-- correction for: " << cut << endl;

	tree->Draw("PsdEvent.fQx", cut, "goff");
	h1  = (TH1F*) tree->GetHistogram();
	
        PSD_shift_Qx[k] = h1->GetMean();
	PSD_RMS_Qx[k] = h1->GetRMS();

        tree->Draw("PsdEvent.fQy", cut, "goff");
	h2  = (TH1F*) tree->GetHistogram();
	
        PSD_shift_Qy[k] = h2->GetMean();
	PSD_RMS_Qy[k] = h2->GetRMS();

	// Sub PSD
	cut =  cut_PSD1;
        cut += " && " +sPreSel;
	//cout << "-- correction for: " << cut << endl;

        tree->Draw("PsdEvent.fQx_sub1", cut, "goff");
	h11  = (TH1F*) tree->GetHistogram();
	
        PSD_shift_Qx_sub1[k] = h11->GetMean();
	PSD_RMS_Qx_sub1[k] = h11->GetRMS();

        tree->Draw("PsdEvent.fQy_sub1", cut, "goff");
	h12  = (TH1F*) tree->GetHistogram();
	
        PSD_shift_Qy_sub1[k] = h12->GetMean();
	PSD_RMS_Qy_sub1[k] = h12->GetRMS();

	//
	cut =  cut_PSD2;
        cut += " && " +sPreSel;
	//cout << "-- correction for: " << cut << endl;

	tree->Draw("PsdEvent.fQx_sub2", cut, "goff");
	h21  = (TH1F*) tree->GetHistogram();
	
        PSD_shift_Qx_sub2[k] = h21->GetMean();
	PSD_RMS_Qx_sub2[k] = h21->GetRMS();

        tree->Draw("PsdEvent.fQy_sub2", cut, "goff");
	h22  = (TH1F*) tree->GetHistogram();
	
        PSD_shift_Qy_sub2[k] = h22->GetMean();
	PSD_RMS_Qy_sub2[k] = h22->GetRMS();

	//
	cut =  cut_PSD3;
        cut += " && " +sPreSel;
	//cout << "-- correction for: " << cut << endl;

        tree->Draw("PsdEvent.fQx_sub3", cut, "goff");
	h31  = (TH1F*) tree->GetHistogram();
	
        PSD_shift_Qx_sub3[k] = h31->GetMean();
	PSD_RMS_Qx_sub3[k] = h31->GetRMS();

        tree->Draw("PsdEvent.fQy_sub3", cut, "goff");
	h32  = (TH1F*) tree->GetHistogram();
	
        PSD_shift_Qy_sub3[k] = h32->GetMean();
	PSD_RMS_Qy_sub3[k] = h32->GetRMS();
    }
    
    delete f;
}

//---- event plane flattening: Barrette method, init for PSD

void eventPlane::PSDinit_flat_Barr()
{
    cout << "================================================" << endl;
    cout << "-- correction file for PSD EP (Barrette method): " << fileName_Barr << endl;

    TFile *f = new TFile(fileName_Barr,"R");
    if (!f) cout << "-E- eventPlane::PSDinit_flat_Barr: pb with the correction file -----" << endl;
    TTree* tree = (TTree* ) f->Get("cbmsim");
    if (!tree) cout << "-E- eventPlane::PSDinit_flat_Barr: pb with the correction TTree -----" << endl;

    TString cut, coeff;
    Int_t k, l, m;
    TString cut_PSD;
    TString cut_PSD1;
    TString cut_PSD2;
    TString cut_PSD3;

    TString detinfo = "";
    TString sPSDsub1 = "centralmod_hole";
    TString sPSDsub2 = "1stCorona";
    TString sPSDsub3 = "2ndCorona";

    TH1F* h1;
    TH1F* h2;
    TH1F* h11;
    TH1F* h12;
    TH1F* h21;
    TH1F* h22;
    TH1F* h31;
    TH1F* h32;

    for (k = 0; k < fNbinFlat; k++)
    {
        // Binning with fixed MC b-step
	//l = 2 * k;
	//if (k < 6) cut = Form("McEvent.fB >= %.1d && McEvent.fB < %.1d", l, l + 2);
	//else cut = Form("McEvent.fB >= %.1d", l);

	if (k==0)
	{
            detinfo = "PsdEvent.fedep_" + sPSDsub1 + "+PsdEvent.fedep_" + sPSDsub2 + "+PsdEvent.fedep_" + sPSDsub3;
	    cut_PSD = Form(detinfo + " <= %.1f", b_PSD[k]);

            detinfo = "PsdEvent.fedep_" + sPSDsub1;
	    cut_PSD1 = Form(detinfo + " <= %.1f", b_PSD1[k]);

            detinfo = "PsdEvent.fedep_" + sPSDsub2;
	    cut_PSD2 = Form(detinfo + " <= %.1f", b_PSD2[k]);

            detinfo = "PsdEvent.fedep_" + sPSDsub3;
	    cut_PSD3 = Form(detinfo + " <= %.1f", b_PSD3[k]);
	}
	else
	{
	    
	    if (k==fNbinFlat-1)
	    {
		detinfo = "PsdEvent.fedep_" + sPSDsub1 + "+PsdEvent.fedep_" + sPSDsub2 + "+PsdEvent.fedep_" + sPSDsub3;
		cut_PSD = Form(detinfo + " > %.1f", b_PSD[k-1]);

		detinfo = "PsdEvent.fedep_" + sPSDsub1;
		cut_PSD1 = Form(detinfo + " > %.1f", b_PSD1[k-1]);

		detinfo = "PsdEvent.fedep_" + sPSDsub2;
		cut_PSD2 = Form(detinfo + " > %.1f", b_PSD2[k-1]);

		detinfo = "PsdEvent.fedep_" + sPSDsub3;
		cut_PSD3 = Form(detinfo + " > %.1f", b_PSD3[k-1]);
	    }
	    else
	    {
                detinfo = "PsdEvent.fedep_" + sPSDsub1 + "+PsdEvent.fedep_" + sPSDsub2 + "+PsdEvent.fedep_" + sPSDsub3;
		cut_PSD = Form(detinfo + " > %.1f && " + detinfo + " <= %.1f", b_PSD[k-1], b_PSD[k]);

		detinfo = "PsdEvent.fedep_" + sPSDsub1;
		cut_PSD1 = Form(detinfo + " > %.1f && " + detinfo + " <= %.1f", b_PSD1[k-1], b_PSD1[k]);

		detinfo = "PsdEvent.fedep_" + sPSDsub2;
		cut_PSD2 = Form(detinfo + " > %.1f && " + detinfo + " <= %.1f", b_PSD2[k-1], b_PSD2[k]);

		detinfo = "PsdEvent.fedep_" + sPSDsub3;
		cut_PSD3 = Form(detinfo + " > %.1f && " + detinfo + " <= %.1f", b_PSD3[k-1], b_PSD3[k]);
	    }
	}

	for (m = 0; m < 8; m++)
	{
	    cut = cut_PSD;
            cut += " && " +sPreSel;
            //if (m==0) cout << "-- correction for: " << cut << endl;

	    coeff = Form("TMath::Cos( %.1d * PsdEvent.fEP_RECO )", m+1);
	    tree->Draw(coeff, cut, "goff");
	    h1 = (TH1F*) tree->GetHistogram();
	    PSD_mean_cosphi[k][m] = h1->GetMean();

	    coeff = Form("TMath::Sin( %.1d * PsdEvent.fEP_RECO )", m+1);
	    tree->Draw(coeff, cut, "goff");
	    h2 = (TH1F*) tree->GetHistogram();
	    PSD_mean_sinphi[k][m] = h2->GetMean();

	    // Sub PSD

	    cut = cut_PSD1;
            cut += " && " +sPreSel;
            //if (m==0) cout << "-- correction for: " << cut << endl;

            coeff = Form("TMath::Cos( %.1d * PsdEvent.fEP_sub1 )", m+1);
	    tree->Draw(coeff, cut, "goff");
	    h11 = (TH1F*) tree->GetHistogram();
	    PSD_mean_cosphi_sub1[k][m] = h11->GetMean();

	    coeff = Form("TMath::Sin( %.1d * PsdEvent.fEP_sub1 )", m+1);
	    tree->Draw(coeff, cut, "goff");
	    h12 = (TH1F*) tree->GetHistogram();
	    PSD_mean_sinphi_sub1[k][m] = h12->GetMean();

	    //
	    cut = cut_PSD2;
            cut += " && " +sPreSel;
            //if (m==0) cout << "-- correction for: " << cut << endl;

            coeff = Form("TMath::Cos( %.1d * PsdEvent.fEP_sub2 )", m+1);
	    tree->Draw(coeff, cut, "goff");
	    h21 = (TH1F*) tree->GetHistogram();
	    PSD_mean_cosphi_sub2[k][m] = h21->GetMean();

	    coeff = Form("TMath::Sin( %.1d * PsdEvent.fEP_sub2 )", m+1);
	    tree->Draw(coeff, cut, "goff");
	    h22 = (TH1F*) tree->GetHistogram();
	    PSD_mean_sinphi_sub2[k][m] = h22->GetMean();

	    //
	    cut = cut_PSD3;
            cut += " && " +sPreSel;
            //if (m==0) cout << "-- correction for: " << cut << endl;

            coeff = Form("TMath::Cos( %.1d * PsdEvent.fEP_sub3 )", m+1);
	    tree->Draw(coeff, cut, "goff");
	    h31 = (TH1F*) tree->GetHistogram();
	    PSD_mean_cosphi_sub3[k][m] = h31->GetMean();

	    coeff = Form("TMath::Sin( %.1d * PsdEvent.fEP_sub3 )", m+1);
	    tree->Draw(coeff, cut, "goff");
	    h32 = (TH1F*) tree->GetHistogram();
	    PSD_mean_sinphi_sub3[k][m] = h32->GetMean();
	}
    }
    
    delete f;
}

// ======================= STS member functions

void eventPlane::STSMCtransverseMomMeth()
{
    // Declaration variables
    TRandom* rand = new TRandom();
    Int_t x;                      // random procedure
    Double_t w;
    Int_t pdgCode, trackID, type, motherID;
    Double_t p, px, py, pz, pt, phi, charge, mass, energy, y;
    CbmMCTrack* track;

    Double_t weight_harmo1;
    Double_t weight_harmo2;

    Int_t mult_v1 = 0;
    Int_t mult_v2 = 0;
    Double_t Qx_harmo1_sub1 = 0.;
    Double_t Qx_harmo1_sub2 = 0.;
    Double_t Qx_harmo1_full = 0.;
    Double_t Qy_harmo1_sub1 = 0.;
    Double_t Qy_harmo1_sub2 = 0.;
    Double_t Qy_harmo1_full = 0.;
    Double_t EP_harmo1_sub1 = 0.;
    Double_t EP_harmo1_sub2 = 0.;
    Double_t EP_harmo1_full = 0.;
    Double_t deltaEP_harmo1_sub1 = 0.;
    Double_t deltaEP_harmo1_sub2 = 0.;
    Double_t deltaEP_harmo1_full = 0.;

    Double_t Qx_harmo2_sub1 = 0.;
    Double_t Qx_harmo2_sub2 = 0.;
    Double_t Qx_harmo2_full = 0.;
    Double_t Qy_harmo2_sub1 = 0.;
    Double_t Qy_harmo2_sub2 = 0.;
    Double_t Qy_harmo2_full = 0.;
    Double_t EP_harmo2_sub1 = 0.;
    Double_t EP_harmo2_sub2 = 0.;
    Double_t EP_harmo2_full = 0.;
    Double_t deltaEP_harmo2_sub1 = 0.;
    Double_t deltaEP_harmo2_sub2 = 0.;
    Double_t deltaEP_harmo2_full = 0.;
    
    Double_t v1 = 0.;  
    Double_t v2 = 0.;
    
    Int_t evtID = fHeader->GetEventID();
    Double_t B = fHeader->GetB();
    Double_t RP = fHeader->GetPhi();     // RP = Reaction plane: MC info (in radian)
    if ( RP > fPi ) RP -= 2 * fPi;       // !!from SHIELD: [0, 2pi]

    //cout << "----- event number: " << evtID << " ------" << endl;
    //cout << "----- imp: " << B << " ------" << endl;
    //cout << "----- RP angle: " << RP << " ------" << endl;

    fMCEventData->SetB(B);
    fMCEventData->SetRP(RP);
    
    fCbmStsEvent->SetEvtID(evtID);       

    Int_t nSTStracks = flistSTSMCtrack->GetEntriesFast();
    cout << "evenPlane::STSMCtransverseMomMeth: # STS MC tracks = " << nSTStracks << endl;

    for (Int_t itrack=0; itrack<nSTStracks; itrack++)
    {
	track = (CbmMCTrack*) flistSTSMCtrack->At(itrack);
	if ( ! track ) 
        {
            cout << "evenPlane::STSMCtransverseMomMeth: no track pointer!" << endl;
	    continue;
	}

	// acceptance
	if ( track->AccSTS() < 4 ) continue;

	type = track->GetPdgCode();

	if (type < 1000000000)
	{
	    charge = track->GetCharge();
	    y = track->GetRapidity();
	}
	else
	{
	    //pdg = 1000000000 + 10*1000*z + 10*a + i;
	    charge = TMath::Floor( ( type - 1000000000 ) / 10000 );
	    mass = TMath::Floor( ( type - 1000000000 -  10000 * charge ) / 10 );
	    p = track->GetP();
	    energy = TMath::Sqrt(mass*0.93827203*mass*0.93827203 + p*p);
	    pz = track->GetPz();
	    y = 0.5 * TMath::Log( ( energy + pz ) / ( energy - pz ) );      // "pz" convention
	}

	if (charge == 0.) continue; // safety condition

	// ======= secondaries?
	motherID = track->GetMotherId();
	//if ( motherID > -1 ) continue;  // no secondary tracks

	y -= fy_cm;
	y /= fy_proj_cm;

	pt = track->GetPt();
	px = track->GetPx();
	py = track->GetPy();
	phi = TMath::ATan2(py, px);	

        // === weight
	weight_harmo1 = y;      // account for v1 Y-antisymmetry i.e. negative if y < 0.
        weight_harmo2 = pt;

	// opposite pion v1 and proton v1 (WA98 observation!) - WARNING: must hold at investigated energies
        if ( TMath::Abs(type) == 211 ) weight_harmo1 = - weight_harmo1;

        // cut to reduce STS/PSD overlap
	if ( y > fcut_STS_PSDsub )
	//if (y<0.5)                   // TEST: "ideal" projectile acceptance
	//if (track->AccPSD() == 0)    // WARNING: must use Forward Wall geometry
	{
	    weight_harmo1 = 0.;
            weight_harmo2 = 0.;
	}
	else
	{
            v1 += TMath::Cos(phi - RP);    // for checking resolution with theoretical expecation i.e. <cos(n(EP - RP))> vs vn x sqrt(N)
            v2 += TMath::Cos(2*(phi - RP));	

	    mult_v1++;
	    mult_v2++;

	    hY->Fill(y);
        }

	x = rand->Integer(2); // create sub-event 1 & sub-event 2
	if ( x == 0)
	{
	    Qx_harmo1_sub1 += weight_harmo1 * TMath::Cos(phi);
	    Qy_harmo1_sub1 += weight_harmo1 * TMath::Sin(phi);
	    
	    Qx_harmo2_sub1 += weight_harmo2 * TMath::Cos(2*phi);
	    Qy_harmo2_sub1 += weight_harmo2 * TMath::Sin(2*phi);
	}
	else
	{
	    Qx_harmo1_sub2 += weight_harmo1 * TMath::Cos(phi);
	    Qy_harmo1_sub2 += weight_harmo1 * TMath::Sin(phi);
	    
	    Qx_harmo2_sub2 += weight_harmo2 * TMath::Cos(2*phi);
	    Qy_harmo2_sub2 += weight_harmo2 * TMath::Sin(2*phi);
	}

	Qx_harmo1_full += weight_harmo1 * TMath::Cos(phi);
	Qy_harmo1_full += weight_harmo1 * TMath::Sin(phi);

	Qx_harmo2_full += weight_harmo2 * TMath::Cos(2*phi);
	Qy_harmo2_full += weight_harmo2 * TMath::Sin(2*phi);
    }

    v1 /= mult_v1;
    v2 /= mult_v2;
    fCbmStsEvent->Setv1v2(v1, v2);

    //---- event plane flattening: Q-recentering

    double mult = (double)mult_v2;

    if (fdoFlat_STS_Qcorr == kTRUE)
    {
        Int_t k;

        //  Binning with fixed MC b-step
	//if (B < 2) k = 0;
	//if (B >= 2 && B < 4) k = 1;
	//if (B >= 4 && B < 6) k = 2;
	//if (B >= 6 && B < 8) k = 3;
	//if (B >= 8 && B < 10) k = 4;
	//if (B >= 10 && B < 12) k = 5;
	//if (B >= 12) k = 6;

        k = FlatBinWhichK(mult, b_STS);

	// first order event plane
	Qx_harmo1_sub1 -= STS_harmo1_shift_Qx_sub1[k];
	Qx_harmo1_sub1 /= STS_harmo1_RMS_Qx_sub1[k];
        
	Qy_harmo1_sub1 -= STS_harmo1_shift_Qy_sub1[k];
	Qy_harmo1_sub1 /= STS_harmo1_RMS_Qy_sub1[k];

        Qx_harmo1_sub2 -= STS_harmo1_shift_Qx_sub2[k];
	Qx_harmo1_sub2 /= STS_harmo1_RMS_Qx_sub2[k];
        
	Qy_harmo1_sub2 -= STS_harmo1_shift_Qy_sub2[k];
	Qy_harmo1_sub2 /= STS_harmo1_RMS_Qy_sub2[k];

        Qx_harmo1_full -= STS_harmo1_shift_Qx_full[k];
	Qx_harmo1_full /= STS_harmo1_RMS_Qx_full[k];
        
	Qy_harmo1_full -= STS_harmo1_shift_Qy_full[k];
	Qy_harmo1_full /= STS_harmo1_RMS_Qy_full[k];
	
	// second order event plane
	Qx_harmo2_sub1 -= STS_harmo2_shift_Qx_sub1[k];
	Qx_harmo2_sub1 /= STS_harmo2_RMS_Qx_sub1[k];
        
	Qy_harmo2_sub1 -= STS_harmo2_shift_Qy_sub1[k];
	Qy_harmo2_sub1 /= STS_harmo2_RMS_Qy_sub1[k];

        Qx_harmo2_sub2 -= STS_harmo2_shift_Qx_sub2[k];
	Qx_harmo2_sub2 /= STS_harmo2_RMS_Qx_sub2[k];
        
	Qy_harmo2_sub2 -= STS_harmo2_shift_Qy_sub2[k];
	Qy_harmo2_sub2 /= STS_harmo2_RMS_Qy_sub2[k];

        Qx_harmo2_full -= STS_harmo2_shift_Qx_full[k];
	Qx_harmo2_full /= STS_harmo2_RMS_Qx_full[k];
        
	Qy_harmo2_full -= STS_harmo2_shift_Qy_full[k];
	Qy_harmo2_full /= STS_harmo2_RMS_Qy_full[k];
    }

    EP_harmo1_sub1 = TMath::ATan2( Qy_harmo1_sub1, Qx_harmo1_sub1 );
    EP_harmo1_sub2 = TMath::ATan2( Qy_harmo1_sub2, Qx_harmo1_sub2 );
    EP_harmo1_full = TMath::ATan2( Qy_harmo1_full, Qx_harmo1_full );

    EP_harmo2_sub1 = TMath::ATan2( Qy_harmo2_sub1, Qx_harmo2_sub1 )/2.;
    EP_harmo2_sub2 = TMath::ATan2( Qy_harmo2_sub2, Qx_harmo2_sub2 )/2.;
    EP_harmo2_full = TMath::ATan2( Qy_harmo2_full, Qx_harmo2_full )/2.;
    
    //---- event plane flattening: Barrette method
    
    if (fdoFlat_STS_Barr == kTRUE)
    {
	Int_t k, m;

        //  Binning with fixed MC b-step
	//if (B < 2) k = 0;
	//if (B >= 2 && B < 4) k = 1;
	//if (B >= 4 && B < 6) k = 2;
	//if (B >= 6 && B < 8) k = 3;
	//if (B >= 8 && B < 10) k = 4;
	//if (B >= 10 && B < 12) k = 5;
	//if (B >= 12) k = 6;

        k = FlatBinWhichK(mult, b_STS);

	for (m = 0; m < 8; m++)
	{
	    // first order event plane
	    EP_harmo1_sub1 += (2/(m+1)) * ( - STS_harmo1_mean_sinphi_sub1[k][m] * TMath::Cos( (m+1) * EP_harmo1_sub1 ) + STS_harmo1_mean_cosphi_sub1[k][m] * TMath::Sin( (m+1) * EP_harmo1_sub1 ) );
	    EP_harmo1_sub2 += (2/(m+1)) * ( - STS_harmo1_mean_sinphi_sub2[k][m] * TMath::Cos( (m+1) * EP_harmo1_sub2 ) + STS_harmo1_mean_cosphi_sub2[k][m] * TMath::Sin( (m+1) * EP_harmo1_sub2 ) );
            EP_harmo1_full += (2/(m+1)) * ( - STS_harmo1_mean_sinphi_full[k][m] * TMath::Cos( (m+1) * EP_harmo1_full ) + STS_harmo1_mean_cosphi_full[k][m] * TMath::Sin( (m+1) * EP_harmo1_full ) );
	    
	    // second order event plane
	    EP_harmo2_sub1 += (2/(m+1)) * ( - STS_harmo2_mean_sinphi_sub1[k][m] * TMath::Cos( (m+1) * EP_harmo2_sub1 ) + STS_harmo2_mean_cosphi_sub1[k][m] * TMath::Sin( (m+1) * EP_harmo2_sub1 ) );
	    EP_harmo2_sub2 += (2/(m+1)) * ( - STS_harmo2_mean_sinphi_sub2[k][m] * TMath::Cos( (m+1) * EP_harmo2_sub2 ) + STS_harmo2_mean_cosphi_sub2[k][m] * TMath::Sin( (m+1) * EP_harmo2_sub2 ) );
            EP_harmo2_full += (2/(m+1)) * ( - STS_harmo2_mean_sinphi_full[k][m] * TMath::Cos( (m+1) * EP_harmo2_full ) + STS_harmo2_mean_cosphi_full[k][m] * TMath::Sin( (m+1) * EP_harmo2_full ) );
	}
    }

    // diff RECO phi - MC phi in proper range -> for drawing purposes  
    
    // first order event plane (sub-event 1, sub-event 2, full event): proper range [-pi, pi]
    deltaEP_harmo1_sub1 = EP_harmo1_sub1 - RP;  
    deltaEP_harmo1_sub2 = EP_harmo1_sub2 - RP;
    deltaEP_harmo1_full = EP_harmo1_full - RP;

    deltaEP_harmo1_sub1 = Range(deltaEP_harmo1_sub1);  
    deltaEP_harmo1_sub2 = Range(deltaEP_harmo1_sub2);
    deltaEP_harmo1_full = Range(deltaEP_harmo1_full);

    // second order event plane (sub-event 1, sub-event 2, full event): proper range [-pi/2, pi/2]    
    deltaEP_harmo2_sub1 = EP_harmo2_sub1 - RP;  
    deltaEP_harmo2_sub2 = EP_harmo2_sub2 - RP;
    deltaEP_harmo2_full = EP_harmo2_full - RP;

    deltaEP_harmo2_sub1 = Range2nd(deltaEP_harmo2_sub1);
    deltaEP_harmo2_sub2 = Range2nd(deltaEP_harmo2_sub2);
    deltaEP_harmo2_full = Range2nd(deltaEP_harmo2_full);

    // fill sts event object

    fCbmStsEvent->SetMult(mult_v2);
    fCbmStsEvent->SetQvect(Qx_harmo1_sub1, Qy_harmo1_sub1, Qx_harmo1_sub2, Qy_harmo1_sub2, Qx_harmo1_full, Qy_harmo1_full, Qx_harmo2_sub1, Qy_harmo2_sub1, Qx_harmo2_sub2, Qy_harmo2_sub2, Qx_harmo2_full, Qy_harmo2_full);
    fCbmStsEvent->SetEP(EP_harmo1_sub1, EP_harmo1_sub2, EP_harmo1_full, EP_harmo2_sub1, EP_harmo2_sub2, EP_harmo2_full);
    fCbmStsEvent->SetdeltaEP(deltaEP_harmo1_sub1, deltaEP_harmo1_sub2, deltaEP_harmo1_full, deltaEP_harmo2_sub1, deltaEP_harmo2_sub2, deltaEP_harmo2_full);
    
    delete rand;
}

void eventPlane::STSRECOtransverseMomMeth()
{
    // Declaration variables
    TRandom* rand = new TRandom();
    Int_t x;                      // random procedure
    Double_t w;
    Int_t pdgCode, trackID, type, motherID;
    Double_t p, px, py, pz, pt, phi, charge, mass, energy, y;
    Double_t IPx, IPy, IP;
    Double_t chi2;
    Int_t NDF;
    CbmStsTrack* track;
    CbmTrackMatch* match;
    CbmMCTrack* mctrack;
    CbmKFTrack kftrack;

    Double_t weight_harmo1;
    Double_t weight_harmo2;

    Int_t mult_v1 = 0;    
    Int_t mult_v2 = 0;
    Double_t Qx_harmo1_sub1 = 0.;
    Double_t Qx_harmo1_sub2 = 0.;
    Double_t Qx_harmo1_full = 0.;
    Double_t Qy_harmo1_sub1 = 0.;
    Double_t Qy_harmo1_sub2 = 0.;
    Double_t Qy_harmo1_full = 0.;
    Double_t EP_harmo1_sub1 = 0.;
    Double_t EP_harmo1_sub2 = 0.;
    Double_t EP_harmo1_full = 0.;
    Double_t deltaEP_harmo1_sub1 = 0.;
    Double_t deltaEP_harmo1_sub2 = 0.;
    Double_t deltaEP_harmo1_full = 0.;

    Double_t Qx_harmo2_sub1 = 0.;
    Double_t Qx_harmo2_sub2 = 0.;
    Double_t Qx_harmo2_full = 0.;
    Double_t Qy_harmo2_sub1 = 0.;
    Double_t Qy_harmo2_sub2 = 0.;
    Double_t Qy_harmo2_full = 0.;
    Double_t EP_harmo2_sub1 = 0.;
    Double_t EP_harmo2_sub2 = 0.;
    Double_t EP_harmo2_full = 0.;
    Double_t deltaEP_harmo2_sub1 = 0.;
    Double_t deltaEP_harmo2_sub2 = 0.;
    Double_t deltaEP_harmo2_full = 0.;
    
    Double_t v1 = 0.;  
    Double_t v2 = 0.;
    
    Int_t evtID = fHeader->GetEventID();
    Double_t B = fHeader->GetB();
    Double_t RP = fHeader->GetPhi(); // RP = Reaction plane: MC info (in radian)
    if ( RP > fPi ) RP -= 2 * fPi;   // !! from SHIELD: [0, 2pi]

    //cout << "----- event number: " << evtID << " ------" << endl;
    //cout << "----- imp: " << B << " ------" << endl;
    //cout << "----- RP angle: " << RP << " ------" << endl;

    fMCEventData->SetB(B);
    fMCEventData->SetRP(RP);
    
    fCbmStsEvent->SetEvtID(evtID);        

    const FairTrackParam *trackParam;
    TVector3 momRec;
    TVector3 posRec;
    
    // MC tracks
    Int_t nMCtracks = flistSTSMCtrack->GetEntriesFast();
    cout << "evenPlane::STSRECOtransverseMomMeth: # STS MC tracks = " << nMCtracks << endl;
    
    // RECO tracks
    Int_t nSTStracks = flistSTSRECOtrack->GetEntriesFast();
    cout << "evenPlane::STSRECOtransverseMomMeth: # STS reco tracks = " << nSTStracks << endl; 
    
    // Extrapolation track parameters back to primary vertex
    vector<CbmStsTrack> vRTracks;
    vRTracks.resize(nSTStracks);    
    for(int iTr=0; iTr<nSTStracks; iTr++) vRTracks[iTr] = *( (CbmStsTrack*) flistSTSRECOtrack->At(iTr));    
        
    vector<L1FieldRegion> vField;
    vector<float> ChiToPrimVtx;
    CbmL1PFFitter fitter;
    CbmKFVertex kfPV(*flistPV);
    fitter.GetChiToVertex(vRTracks, vField, ChiToPrimVtx, kfPV, 1000000000);

    for (Int_t itrack=0; itrack<nSTStracks; itrack++)
    {
        track = &vRTracks[itrack];
	
	match = (CbmTrackMatch*) flistSTStrackMATCH->At(itrack);
	trackID = match->GetMCTrackId();

	if (trackID < 0) continue; // ghost track from combinatorix (no corresponding MC tracks)
	
	//need particle type/mass for rapidity calculation & rotation of pions (anti-v1 flow w.r.t. protons)
	//for now: use MC info for particle ID, TO DO: real PID
	//
	mctrack = (CbmMCTrack*) flistSTSMCtrack->At(trackID); 
	type = mctrack->GetPdgCode();
	mass = mctrack->GetMass();	

        //if ( TMath::Abs(type) != 2212 ) continue; 

	chi2 = track->GetChiSq();
	NDF = track->GetNDF();
	chi2 /= NDF;
	//std::cout << "chi2/NDF = " << chi2 << endl;
	hChi2toNDF->Fill(chi2);
	
	trackParam = track->GetParamFirst();	
	trackParam->Momentum(momRec);
	trackParam->Position(posRec);
	//std::cout << " track Z-StartVertex = " << track->GetParamFirst()->GetZ() << endl;
	
	IPx = posRec.X();
	IPy = posRec.Y();
	IP = TMath::Sqrt(IPx*IPx + IPy*IPy);
	//std::cout << "IPx, IPy, IP = " << IPx << ", " << IPy << ", " << IP << endl;
	hIP->Fill(IP);
	
	px = momRec.X();
	py = momRec.Y();
	pz = momRec.Z();	
	//std::cout << "px, py, pz = " << px << ", " << py << ", " << pz << endl;	
	
	pt = TMath::Sqrt(px*px + py*py);
	p = TMath::Sqrt(px*px + py*py + pz*pz);	
	//std::cout << "reco. track p = " << p << endl;
	hP->Fill(p);
	
	energy = TMath::Sqrt(mass*0.93827203*mass*0.93827203 + p*p);
	y = 0.5 * TMath::Log( ( energy + pz ) / ( energy - pz ) );    // "pz" convention

	y -= fy_cm;
	y /= fy_proj_cm;

        phi = TMath::ATan2(py, px);

        // === weight
	weight_harmo1 = y;      // account for v1 Y-antisymmetry i.e. negative if y < 0.
        weight_harmo2 = pt;

        // opposite pion v1 and proton v1 (WA98 observation!) - WARNING: must hold at investigated energies
        if ( TMath::Abs(type) == 211 ) weight_harmo1 = - weight_harmo1;

        // cut to reduce STS/PSD overlap
	if ( y > fcut_STS_PSDsub )
	{
	    weight_harmo1 = 0.;
            weight_harmo2 = 0.;
	}
	else
	{
            v1 += TMath::Cos(phi - RP);    // for checking resolution with theoretical expecation i.e. <cos(n(EP - RP))> vs vn x sqrt(N)
            v2 += TMath::Cos(2*(phi - RP));	

	    mult_v1++;
	    mult_v2++;

            hY->Fill(y);
        }

	x = rand->Integer(2);  // create sub-event 1 & sub-event 2
	if ( x == 0)
	{
	    Qx_harmo1_sub1 += weight_harmo1 * TMath::Cos(phi);
	    Qy_harmo1_sub1 += weight_harmo1 * TMath::Sin(phi);
	    
	    Qx_harmo2_sub1 += weight_harmo2 * TMath::Cos(2*phi);
	    Qy_harmo2_sub1 += weight_harmo2 * TMath::Sin(2*phi);
	}
	else
	{
	    Qx_harmo1_sub2 += weight_harmo1 * TMath::Cos(phi);
	    Qy_harmo1_sub2 += weight_harmo1 * TMath::Sin(phi);
	    
	    Qx_harmo2_sub2 += weight_harmo2 * TMath::Cos(2*phi);
	    Qy_harmo2_sub2 += weight_harmo2 * TMath::Sin(2*phi);
	}

	Qx_harmo1_full += weight_harmo1 * TMath::Cos(phi);
	Qy_harmo1_full += weight_harmo1 * TMath::Sin(phi);

	Qx_harmo2_full += weight_harmo2 * TMath::Cos(2*phi);
	Qy_harmo2_full += weight_harmo2 * TMath::Sin(2*phi);
    }
    
    v1 /= mult_v1;
    v2 /= mult_v2;
    fCbmStsEvent->Setv1v2(v1, v2);    
    
    cout << "evenPlane::STSRECOtransverseMomMeth: # STS reco tracks (no ghost) = " << mult_v2 << endl;

    //---- event plane flattening: Q-recentering

    double mult = (double)mult_v2;

    if (fdoFlat_STS_Qcorr == kTRUE)
    {
        Int_t k;

        //  Binning with fixed MC b-step
	//if (B < 2) k = 0;
	//if (B >= 2 && B < 4) k = 1;
	//if (B >= 4 && B < 6) k = 2;
	//if (B >= 6 && B < 8) k = 3;
	//if (B >= 8 && B < 10) k = 4;
	//if (B >= 10 && B < 12) k = 5;
	//if (B >= 12) k = 6;

        k = FlatBinWhichK(mult, b_STS);
	//cout << "===== STS: k = " << k << endl;

	// first order event plane
	Qx_harmo1_sub1 -= STS_harmo1_shift_Qx_sub1[k];
	Qx_harmo1_sub1 /= STS_harmo1_RMS_Qx_sub1[k];
        
	Qy_harmo1_sub1 -= STS_harmo1_shift_Qy_sub1[k];
	Qy_harmo1_sub1 /= STS_harmo1_RMS_Qy_sub1[k];

        Qx_harmo1_sub2 -= STS_harmo1_shift_Qx_sub2[k];
	Qx_harmo1_sub2 /= STS_harmo1_RMS_Qx_sub2[k];
        
	Qy_harmo1_sub2 -= STS_harmo1_shift_Qy_sub2[k];
	Qy_harmo1_sub2 /= STS_harmo1_RMS_Qy_sub2[k];

        Qx_harmo1_full -= STS_harmo1_shift_Qx_full[k];
	Qx_harmo1_full /= STS_harmo1_RMS_Qx_full[k];
        
	Qy_harmo1_full -= STS_harmo1_shift_Qy_full[k];
	Qy_harmo1_full /= STS_harmo1_RMS_Qy_full[k];
	
	// second order event plane
	Qx_harmo2_sub1 -= STS_harmo2_shift_Qx_sub1[k];
	Qx_harmo2_sub1 /= STS_harmo2_RMS_Qx_sub1[k];
        
	Qy_harmo2_sub1 -= STS_harmo2_shift_Qy_sub1[k];
	Qy_harmo2_sub1 /= STS_harmo2_RMS_Qy_sub1[k];

        Qx_harmo2_sub2 -= STS_harmo2_shift_Qx_sub2[k];
	Qx_harmo2_sub2 /= STS_harmo2_RMS_Qx_sub2[k];
        
	Qy_harmo2_sub2 -= STS_harmo2_shift_Qy_sub2[k];
	Qy_harmo2_sub2 /= STS_harmo2_RMS_Qy_sub2[k];

        Qx_harmo2_full -= STS_harmo2_shift_Qx_full[k];
	Qx_harmo2_full /= STS_harmo2_RMS_Qx_full[k];
        
	Qy_harmo2_full -= STS_harmo2_shift_Qy_full[k];
	Qy_harmo2_full /= STS_harmo2_RMS_Qy_full[k];
    }

    EP_harmo1_sub1 = TMath::ATan2( Qy_harmo1_sub1, Qx_harmo1_sub1 );
    EP_harmo1_sub2 = TMath::ATan2( Qy_harmo1_sub2, Qx_harmo1_sub2 );
    EP_harmo1_full = TMath::ATan2( Qy_harmo1_full, Qx_harmo1_full );

    EP_harmo2_sub1 = TMath::ATan2( Qy_harmo2_sub1, Qx_harmo2_sub1 )/2.;
    EP_harmo2_sub2 = TMath::ATan2( Qy_harmo2_sub2, Qx_harmo2_sub2 )/2.;
    EP_harmo2_full = TMath::ATan2( Qy_harmo2_full, Qx_harmo2_full )/2.;
    
    //---- event plane flattening: Barrette method
    
    if (fdoFlat_STS_Barr == kTRUE)
    {
	Int_t k, m;

        //  Binning with fixed MC b-step
	//if (B < 2) k = 0;
	//if (B >= 2 && B < 4) k = 1;
	//if (B >= 4 && B < 6) k = 2;
	//if (B >= 6 && B < 8) k = 3;
	//if (B >= 8 && B < 10) k = 4;
	//if (B >= 10 && B < 12) k = 5;
	//if (B >= 12) k = 6;

        k = FlatBinWhichK(mult, b_STS);

	for (m = 0; m < 8; m++)
	{
	    // first order event plane
	    EP_harmo1_sub1 += (2/(m+1)) * ( - STS_harmo1_mean_sinphi_sub1[k][m] * TMath::Cos( (m+1) * EP_harmo1_sub1 ) + STS_harmo1_mean_cosphi_sub1[k][m] * TMath::Sin( (m+1) * EP_harmo1_sub1 ) );
	    EP_harmo1_sub2 += (2/(m+1)) * ( - STS_harmo1_mean_sinphi_sub2[k][m] * TMath::Cos( (m+1) * EP_harmo1_sub2 ) + STS_harmo1_mean_cosphi_sub2[k][m] * TMath::Sin( (m+1) * EP_harmo1_sub2 ) );
            EP_harmo1_full += (2/(m+1)) * ( - STS_harmo1_mean_sinphi_full[k][m] * TMath::Cos( (m+1) * EP_harmo1_full ) + STS_harmo1_mean_cosphi_full[k][m] * TMath::Sin( (m+1) * EP_harmo1_full ) );
	    
	    // second order event plane
	    EP_harmo2_sub1 += (2/(m+1)) * ( - STS_harmo2_mean_sinphi_sub1[k][m] * TMath::Cos( (m+1) * EP_harmo2_sub1 ) + STS_harmo2_mean_cosphi_sub1[k][m] * TMath::Sin( (m+1) * EP_harmo2_sub1 ) );
	    EP_harmo2_sub2 += (2/(m+1)) * ( - STS_harmo2_mean_sinphi_sub2[k][m] * TMath::Cos( (m+1) * EP_harmo2_sub2 ) + STS_harmo2_mean_cosphi_sub2[k][m] * TMath::Sin( (m+1) * EP_harmo2_sub2 ) );
            EP_harmo2_full += (2/(m+1)) * ( - STS_harmo2_mean_sinphi_full[k][m] * TMath::Cos( (m+1) * EP_harmo2_full ) + STS_harmo2_mean_cosphi_full[k][m] * TMath::Sin( (m+1) * EP_harmo2_full ) );
	}
    }

    // diff RECO phi - MC phi in proper range -> for drawing purposes  
    
    // first order event plane (sub-event 1, sub-event 2, full event): proper range [-pi, pi] 
    deltaEP_harmo1_sub1 = EP_harmo1_sub1 - RP;        
    deltaEP_harmo1_sub2 = EP_harmo1_sub2 - RP;
    deltaEP_harmo1_full = EP_harmo1_full - RP;

    deltaEP_harmo1_sub1 = Range(deltaEP_harmo1_sub1);  
    deltaEP_harmo1_sub2 = Range(deltaEP_harmo1_sub2);
    deltaEP_harmo1_full = Range(deltaEP_harmo1_full);

    // second order event plane (sub-event 1, sub-event 2, full event): proper range [-pi/2, pi/2]
    deltaEP_harmo2_sub1 = EP_harmo2_sub1 - RP;        
    deltaEP_harmo2_sub2 = EP_harmo2_sub2 - RP;
    deltaEP_harmo2_full = EP_harmo2_full - RP;

    deltaEP_harmo2_sub1 = Range2nd(deltaEP_harmo2_sub1);
    deltaEP_harmo2_sub2 = Range2nd(deltaEP_harmo2_sub2);
    deltaEP_harmo2_full = Range2nd(deltaEP_harmo2_full);

    // fill sts event object
    fCbmStsEvent->SetMult(mult_v2);
    fCbmStsEvent->SetQvect(Qx_harmo1_sub1, Qy_harmo1_sub1, Qx_harmo1_sub2, Qy_harmo1_sub2, Qx_harmo1_full, Qy_harmo1_full, Qx_harmo2_sub1, Qy_harmo2_sub1, Qx_harmo2_sub2, Qy_harmo2_sub2, Qx_harmo2_full, Qy_harmo2_full);
    fCbmStsEvent->SetEP(EP_harmo1_sub1, EP_harmo1_sub2, EP_harmo1_full, EP_harmo2_sub1, EP_harmo2_sub2, EP_harmo2_full);
    fCbmStsEvent->SetdeltaEP(deltaEP_harmo1_sub1, deltaEP_harmo1_sub2, deltaEP_harmo1_full, deltaEP_harmo2_sub1, deltaEP_harmo2_sub2, deltaEP_harmo2_full);
    
    delete rand;
}

//---- event plane flattening: Q-recentering, init for STS

void eventPlane::STSinit_flat_Qcorr()
{
    cout << "================================================" << endl;
    cout << "-- correction file for STS EP (Q-recentering): " << fileName_Qcorr << endl;

    TFile *f = new TFile(fileName_Qcorr,"R");
    if (!f) cout << "-E- eventPlane::STSinit_flat_Qcorr: pb with the correction file -----" << endl;
    TTree* tree = (TTree* ) f->Get("cbmsim");
    if (!tree) cout << "-E- eventPlane::STSinit_flat_Qcorr: pb with the correction TTree -----" << endl;

    TString cut;
    Int_t k, l;

    TString detinfo = "StsEvent.fmult";

    TH1F* h1;
    TH1F* h2;
    TH1F* h3;
    TH1F* h4;
    TH1F* h5;
    TH1F* h6;
    TH1F* h7;
    TH1F* h8;
    TH1F* h9;
    TH1F* h10;
    TH1F* h11;
    TH1F* h12;

    for (k = 0; k < fNbinFlat; k++)
    {
        // Binning fixed MC b steps
	//l = 2 * k;
	//if (k < 6) cut = Form("McEvent.fB >= %.1d && McEvent.fB < %.1d", l, l + 2);
	//else cut = Form("McEvent.fB >= %.1d", l);

	if (k==0) cut = Form(detinfo + " <= %.1f", b_STS[k]);
	else
	{
	    if (k==fNbinFlat-1) cut = Form(detinfo + " > %.1f", b_STS[k-1]);
	    else cut = Form(detinfo + " > %.1f && " + detinfo + " <= %.1f", b_STS[k-1], b_STS[k]);
	}

	//cout << "-- correction for: " << cut << endl;

	// first order event plane
	tree->Draw("StsEvent.fQx_harmo1_sub1", cut, "goff");
	h1  = (TH1F*) tree->GetHistogram();
        STS_harmo1_shift_Qx_sub1[k] = h1->GetMean();
	STS_harmo1_RMS_Qx_sub1[k] = h1->GetRMS();

        tree->Draw("StsEvent.fQy_harmo1_sub1", cut, "goff");
	h2  = (TH1F*) tree->GetHistogram();
        STS_harmo1_shift_Qy_sub1[k] = h2->GetMean();
	STS_harmo1_RMS_Qy_sub1[k] = h2->GetRMS();

        tree->Draw("StsEvent.fQx_harmo1_sub2", cut, "goff");
	h3  = (TH1F*) tree->GetHistogram();
        STS_harmo1_shift_Qx_sub2[k] = h3->GetMean();
	STS_harmo1_RMS_Qx_sub2[k] = h3->GetRMS();

        tree->Draw("StsEvent.fQy_harmo1_sub2", cut, "goff");
	h4  = (TH1F*) tree->GetHistogram();
        STS_harmo1_shift_Qy_sub2[k] = h4->GetMean();
	STS_harmo1_RMS_Qy_sub2[k] = h4->GetRMS();

        tree->Draw("StsEvent.fQx_harmo1_full", cut, "goff");
	h5  = (TH1F*) tree->GetHistogram();
        STS_harmo1_shift_Qx_full[k] = h5->GetMean();
	STS_harmo1_RMS_Qx_full[k] = h5->GetRMS();

        tree->Draw("StsEvent.fQy_harmo1_full", cut, "goff");
	h6  = (TH1F*) tree->GetHistogram();
        STS_harmo1_shift_Qy_full[k] = h6->GetMean();
	STS_harmo1_RMS_Qy_full[k] = h6->GetRMS();
		
	// second order event plane
	tree->Draw("StsEvent.fQx_harmo2_sub1", cut, "goff");
	h7  = (TH1F*) tree->GetHistogram();
        STS_harmo2_shift_Qx_sub1[k] = h7->GetMean();
	STS_harmo2_RMS_Qx_sub1[k] = h7->GetRMS();

        tree->Draw("StsEvent.fQy_harmo2_sub1", cut, "goff");
	h8  = (TH1F*) tree->GetHistogram();
        STS_harmo2_shift_Qy_sub1[k] = h8->GetMean();
	STS_harmo2_RMS_Qy_sub1[k] = h8->GetRMS();

        tree->Draw("StsEvent.fQx_harmo2_sub2", cut, "goff");
	h9  = (TH1F*) tree->GetHistogram();
        STS_harmo2_shift_Qx_sub2[k] = h9->GetMean();
	STS_harmo2_RMS_Qx_sub2[k] = h9->GetRMS();

        tree->Draw("StsEvent.fQy_harmo2_sub2", cut, "goff");
	h10  = (TH1F*) tree->GetHistogram();
        STS_harmo2_shift_Qy_sub2[k] = h10->GetMean();
	STS_harmo2_RMS_Qy_sub2[k] = h10->GetRMS();

        tree->Draw("StsEvent.fQx_harmo2_full", cut, "goff");
	h11  = (TH1F*) tree->GetHistogram();
        STS_harmo2_shift_Qx_full[k] = h11->GetMean();
	STS_harmo2_RMS_Qx_full[k] = h11->GetRMS();

        tree->Draw("StsEvent.fQy_harmo2_full", cut, "goff");
	h12  = (TH1F*) tree->GetHistogram();
        STS_harmo2_shift_Qy_full[k] = h12->GetMean();
	STS_harmo2_RMS_Qy_full[k] = h12->GetRMS();
    }
    
    delete f;
}

//---- event plane flattening: Barrette method, init for STS

void eventPlane::STSinit_flat_Barr()
{
    cout << "================================================" << endl;
    cout << "-- correction file for STS EP (Barrette method): " << fileName_Barr << endl;

    TFile *f = new TFile(fileName_Barr,"R");
    if (!f) cout << "-E- eventPlane::STSinit_flat_Barr: pb with the correction file -----" << endl;
    TTree* tree = (TTree* ) f->Get("cbmsim");
    if (!tree) cout << "-E- eventPlane::STSinit_flat_Barr: pb with the correction TTree -----" << endl;

    TString cut, coeff;
    Int_t k, l, m;

    TString detinfo = "StsEvent.fmult";

    TH1F* h1;
    TH1F* h2;
    TH1F* h3;
    TH1F* h4;
    TH1F* h5;
    TH1F* h6;
    TH1F* h7;
    TH1F* h8;
    TH1F* h9;
    TH1F* h10;
    TH1F* h11;
    TH1F* h12;

    for (k = 0; k < fNbinFlat; k++)
    {
        // Binning fixed MC b steps
	//l = 2 * k;
	//if (k < 6) cut = Form("McEvent.fB >= %.1d && McEvent.fB < %.1d", l, l + 2);
	//else cut = Form("McEvent.fB >= %.1d", l);

	if (k==0) cut = Form(detinfo + " <= %.1f", b_STS[k]);
	else
	{
	    if (k==fNbinFlat-1) cut = Form(detinfo + " > %.1f", b_STS[k-1]);
	    else cut = Form(detinfo + " > %.1f && " + detinfo + " <= %.1f", b_STS[k-1], b_STS[k]);
	}

	//cout << "-- correction for: " << cut << endl;

	for (m = 0; m < 8; m++)
	{
	    // first order event plane
	    coeff = Form("TMath::Cos( %.1d * StsEvent.fEP_RECO_harmo1_sub1 )", m+1);
	    tree->Draw(coeff, cut, "goff");
	    h1 = (TH1F*) tree->GetHistogram();
	    STS_harmo1_mean_cosphi_sub1[k][m] = h1->GetMean();

	    coeff = Form("TMath::Sin( %.1d * StsEvent.fEP_RECO_harmo1_sub1 )", m+1);
	    tree->Draw(coeff, cut, "goff");
	    h2 = (TH1F*) tree->GetHistogram();
	    STS_harmo1_mean_sinphi_sub1[k][m] = h2->GetMean();

            coeff = Form("TMath::Cos( %.1d * StsEvent.fEP_RECO_harmo1_sub2 )", m+1);
	    tree->Draw(coeff, cut, "goff");
	    h3 = (TH1F*) tree->GetHistogram();
	    STS_harmo1_mean_cosphi_sub2[k][m] = h3->GetMean();

	    coeff = Form("TMath::Sin( %.1d * StsEvent.fEP_RECO_harmo1_sub2 )", m+1);
	    tree->Draw(coeff, cut, "goff");
	    h4 = (TH1F*) tree->GetHistogram();
	    STS_harmo1_mean_sinphi_sub2[k][m] = h4->GetMean();

            coeff = Form("TMath::Cos( %.1d * StsEvent.fEP_RECO_harmo1_full )", m+1);
	    tree->Draw(coeff, cut, "goff");
	    h5 = (TH1F*) tree->GetHistogram();
	    STS_harmo1_mean_cosphi_full[k][m] = h5->GetMean();

	    coeff = Form("TMath::Sin( %.1d * StsEvent.fEP_RECO_harmo1_full )", m+1);
	    tree->Draw(coeff, cut, "goff");
	    h6 = (TH1F*) tree->GetHistogram();
	    STS_harmo1_mean_sinphi_full[k][m] = h6->GetMean();
	    	    
	    // second order event plane
	    coeff = Form("TMath::Cos( %.1d * StsEvent.fEP_RECO_harmo2_sub1 )", m+1);
	    tree->Draw(coeff, cut, "goff");
	    h7 = (TH1F*) tree->GetHistogram();
	    STS_harmo2_mean_cosphi_sub1[k][m] = h7->GetMean();

	    coeff = Form("TMath::Sin( %.1d * StsEvent.fEP_RECO_harmo2_sub1 )", m+1);
	    tree->Draw(coeff, cut, "goff");
	    h8 = (TH1F*) tree->GetHistogram();
	    STS_harmo2_mean_sinphi_sub1[k][m] = h8->GetMean();

            coeff = Form("TMath::Cos( %.1d * StsEvent.fEP_RECO_harmo2_sub2 )", m+1);
	    tree->Draw(coeff, cut, "goff");
	    h9 = (TH1F*) tree->GetHistogram();
	    STS_harmo2_mean_cosphi_sub2[k][m] = h9->GetMean();

	    coeff = Form("TMath::Sin( %.1d * StsEvent.fEP_RECO_harmo2_sub2 )", m+1);
	    tree->Draw(coeff, cut, "goff");
	    h10 = (TH1F*) tree->GetHistogram();
	    STS_harmo2_mean_sinphi_sub2[k][m] = h10->GetMean();

            coeff = Form("TMath::Cos( %.1d * StsEvent.fEP_RECO_harmo2_full )", m+1);
	    tree->Draw(coeff, cut, "goff");
	    h11 = (TH1F*) tree->GetHistogram();
	    STS_harmo2_mean_cosphi_full[k][m] = h11->GetMean();

	    coeff = Form("TMath::Sin( %.1d * StsEvent.fEP_RECO_harmo2_full )", m+1);
	    tree->Draw(coeff, cut, "goff");
	    h12 = (TH1F*) tree->GetHistogram();
	    STS_harmo2_mean_sinphi_full[k][m] = h12->GetMean();
	}
    }
    
    delete f;
}

// ====================================================================== Small functions

void eventPlane::STSprojRapidityCM()
{
    // Calcul rapidity of c.m. & projectile (in c.m.) to compute particle normalized rapidity in c.m.
    
    if (fEn == 0.)
    {
      LOG(FATAL) << "-E- eventPlane::STSprojRapidityCM: Please specify beam kinetic energy when instantiating eventPlane object (needed for rapidity selection using STS, done in c.m. frame)" << FairLogger::endl;
      //return kERROR;
    }
    
    const Double_t kProtonMass = 0.938271998;
    Double_t energy_proj = Double_t(fEn) + kProtonMass;
    Double_t mom_proj = sqrt(energy_proj*energy_proj - kProtonMass*kProtonMass);
    Double_t y_proj = 0.5*log((energy_proj+mom_proj)/(energy_proj-mom_proj));           // "pz" convention: here p = pz assuming emittance(X and Y) = 0 mm.mrad 
    fy_cm = 0.5*y_proj;
    fy_proj_cm = y_proj - fy_cm;
}

int eventPlane::FlatBinWhichK(double E_tmp, double *b_tmp)
{
    int k;
    for (int j = 0; j < fNbinFlat; j++)
    {
	if (E_tmp <= b_tmp[0]) k=0;
	else
	{
	    if (E_tmp > b_tmp[fNbinFlat-2]) k=fNbinFlat-1;
	    else
	    {
		if (E_tmp > b_tmp[j-1] && E_tmp <= b_tmp[j]) k=j;
	    }
	}
    }
    //cout << "(E, k) = (" << E_tmp << ", " << k << ")" << endl;

    return k;
}

Double_t eventPlane::Range(Double_t phi)
{
    if (phi < -fPi) phi += 2*fPi;
    if (phi >  fPi) phi -= 2*fPi;   
    return phi;
}

Double_t eventPlane::Range2nd(Double_t phi)
{
    if (phi < -fPi/2.) phi += fPi;
    if (phi >  fPi/2.) phi -= fPi;
    return phi;
}

ClassImp(eventPlane)
