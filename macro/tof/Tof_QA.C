#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TLegend.h"

#include "CbmMCTrack.h"
#include "FairTrackParam.h"

#include "CbmTofPoint.h"
#include "CbmTofHit.h"
#include "CbmTrdTrack.h"
#include "CbmTrdTrackMatch.h"
#include "CbmStsTrack.h"
#include "CbmStsTrackMatch.h"
#include "CbmGlobalTrack.h"

#include "TVector3.h"
#include "TParticlePDG.h"
#include "TDatabasePDG.h"
#include "TCanvas.h"
#include "TSystem.h"
#include <iostream>

using namespace std;

int Tof_QA()
{
  // Open the files

  //  TFile *fileSIM  = new TFile("/d/cbm01/diego/CBM/FEB07/SIMUL/AUAU_CENT_25GEV/std_TOF_8gap_1000.root");
  //  TFile *fileRECO = new TFile("/d/cbm01/diego/CBM/FEB07/ANAL/AUAU_CENT_25GEV/std_TOF_8gap_1000_reco.root");
    TFile *fileSIM  = new TFile("/misc/diego/cbmroot/macro/run/bor.root");
    TFile *fileRECO = new TFile("/misc/diego/cbmroot/macro/run/bor_reco.root");


  // Get the trees 
  TTree *treeSIM  = (TTree*)fileSIM->Get("cbmsim");
  TTree *treeRECO = (TTree*)fileRECO->Get("cbmsim");

  // Get number of events
   Int_t nevents = treeSIM->GetEntries();

  // Activate branches

  TClonesArray *TofPoints = new TClonesArray("CbmTofPoint", 100);
  TClonesArray *McTracks  = new TClonesArray("CbmMCTrack",  100);
    
  treeSIM->SetBranchAddress("TOFPoint", &TofPoints);
  treeSIM->SetBranchAddress("MCTrack",  &McTracks);

  TClonesArray *TofHits    = new TClonesArray("CbmTofHit",        100);
  //  TClonesArray *TrdTracks  = new TClonesArray("CbmTrdTrack",      100);
  //TClonesArray *TrdMatches = new TClonesArray("CbmTrdTrackMatch", 100);
  //TClonesArray *StsTracks  = new TClonesArray("CbmStsTrack",      100);
  //TClonesArray *Sts_matches= new TClonesArray("CbmStsTrackMatch", 100);
  //TClonesArray *GlobTracks = new TClonesArray("CbmGlobalTrack",   100);
    
  treeRECO->SetBranchAddress("TofHit",        &TofHits);
  //treeRECO->SetBranchAddress("TRDTrack",      &TrdTracks);
  //treeRECO->SetBranchAddress("TRDTrackMatch", &TrdMatches);
  //treeRECO->SetBranchAddress("STSTrack",      &StsTracks);
  //treeRECO->SetBranchAddress("STSTrackMatch", &Sts_matches);
  //treeRECO->SetBranchAddress("GlobalTrack",   &GlobTracks);

  CbmMCTrack       *McTrack;
  CbmTofPoint      *TofPoint;
  CbmTofPoint      *TofPoint2;
  CbmTofHit        *TofHit;
  //CbmTrdTrack      *TrdTrack; 
  //CbmStsTrack      *StsTrack;
  //FairTrackParam    *FitPar;
  //CbmTrdTrackMatch *Trd_MCmatch;
  //CbmStsTrackMatch *Sts_MCmatch;
  //CbmGlobalTrack   *GlobTrack;


  TH2F *p_mass         = new TH2F("p_mass_single_target","p_mass_single_target", 200,-0.5, 1.2, 200,0,10);
  TH2F *p_mass_double  = new TH2F("p_mass_double_target","p_mass_double_target", 200,-0.5, 1.2, 200,0,10);
  TH2F *p_mass_interf  = new TH2F("p_mass_interf_target","p_mass_interf_target", 200,-0.5, 1.2, 200,0,10);
  TH2F *p_mass_all     = new TH2F("p_mass_all","p_mass_all", 200,-0.5, 1.2, 200,0,10);

  TH2F *xy      = new TH2F("xy_POINT_Flag1","xy_POINT_Flag1", 200,-1000, 1000, 200,-1000,1000);
  TH1F *z       = new TH1F("z_POINT_Flag1","z_POINT_Flag1", 40,  980, 1020); 
  TH1F *t       = new TH1F("tof_POINT_Flag1","tof_POINT_Flag1", 1000, 30, 60);
  TH1F *nreg    = new TH1F("region_POINT_Flag1","region_POINT_Flag1", 10, 0, 10);
  TH1F *nmod    = new TH1F("module_POINT_Flag1","module_POINT_Flag1", 200,0,200);
  TH1F *ncel    = new TH1F("cel_POINT_Flag1","cel_POINT_Flag1", 200,0,200);
  TH1F *ngap    = new TH1F("gap_POINT_Flag1","gap_POINT_Flag1", 20, 0,20);
  TH1F *detID   = new TH1F("detID_POINT_Flag1","detID_POINT_Flag1", 10000, 0, 5e+6);


  TH1F *dx_pi1   = new TH1F("dx_pi1","dx_pi1", 100, -10,10); 
  TH1F *dy_pi1   = new TH1F("dy_pi1","dy_pi1", 100, -10,10);
  TH1F *dz_pi1   = new TH1F("dz_pi1","dz_pi1", 100, -3,3); 
  TH1F *dt_pi1   = new TH1F("dt_pi1","dt_pi1", 200, -1, 1); 

  TH1F *dx_k1    = new TH1F("dx_k1","dx_k1", 100, -10,10); 
  TH1F *dy_k1    = new TH1F("dy_k1","dy_k1", 100, -10,10);
  TH1F *dz_k1    = new TH1F("dz_k1","dz_k1", 100, -3,3); 
  TH1F *dt_k1    = new TH1F("dt_k1","dt_k1", 200, -1, 1 ); 

  TH1F *dx_p1    = new TH1F("dx_p1","dx_p1", 100, -10,10); 
  TH1F *dy_p1    = new TH1F("dy_p1","dy_p1", 100, -10,10);
  TH1F *dz_p1    = new TH1F("dz_p1","dz_p1", 100, -3,3);
  TH1F *dt_p1    = new TH1F("dt_p1","dt_p1", 200, -1, 1 ); 

  TH1F *dx_all   = new TH1F("dx_all_full_range","dx_all_full_range", 1000, -10,10); 
  TH1F *dy_all   = new TH1F("dy_all_full_range","dy_all_full_range", 1000, -10,10);
  TH1F *dz_all   = new TH1F("dz_all_full_range","dz_all_full_range", 1000, -3,3);
  TH1F *dt_all   = new TH1F("dt_all_full_range","dt_all_full_range", 2000, -1, 1 );

  TH1F *eff_p_p1    = new TH1F("eff_p_p1","eff_p_p1", 50, 0, 30);
  TH1F *eff_p_theta1= new TH1F("eff_p_theta1","eff_p_theta1", 50, 0, 90);
  TH1F *eff_p_x1    = new TH1F("eff_p_x1","eff_p_x1", 50, -1000, 1000);
  TH1F *eff_p_y1    = new TH1F("eff_p_y1","eff_p_y1", 50, -1000, 1000);
  
  TH1F *eff_pi_p1    = new TH1F("eff_pi_p1","eff_pi_p1", 50, 0, 30);
  TH1F *eff_pi_theta1= new TH1F("eff_pi_theta1","eff_pi_theta1", 50, 0, 90);
  TH1F *eff_pi_x1    = new TH1F("eff_pi_x1","eff_pi_x1", 50, -1000, 1000);
  TH1F *eff_pi_y1    = new TH1F("eff_pi_y1","eff_pi_y1", 50, -1000, 1000);

  TH1F *eff_k_p1    = new TH1F("eff_k_p1","eff_k_p1", 50, 0, 30);
  TH1F *eff_k_theta1= new TH1F("eff_k_theta1","eff_k_theta1", 50, 0, 90);
  TH1F *eff_k_x1    = new TH1F("eff_k_x1","eff_k_x1", 50, -1000, 1000);
  TH1F *eff_k_y1    = new TH1F("eff_k_y1","eff_k_y1", 50, -1000, 1000);

  TH1F *norm_p_p1    = new TH1F("norm_p_p1","norm_p_p1", 50, 0, 30);
  TH1F *norm_p_theta1= new TH1F("norm_p_theta1","norm_p_theta1", 50, 0, 90);
  TH1F *norm_p_x1    = new TH1F("norm_p_x1","norm_p_x1", 50, -1000, 1000);
  TH1F *norm_p_y1    = new TH1F("norm_p_y1","norm_p_y1", 50, -1000, 1000);

  TH1F *norm_pi_p1    = new TH1F("norm_pi_p1","norm_pi_p1", 50, 0, 30);
  TH1F *norm_pi_theta1= new TH1F("norm_pi_theta1","norm_pi_theta1", 50, 0, 90);
  TH1F *norm_pi_x1    = new TH1F("norm_pi_x1","norm_pi_x1", 50, -1000, 1000);
  TH1F *norm_pi_y1    = new TH1F("norm_pi_y1","norm_pi_y1", 50, -1000, 1000);

  TH1F *norm_k_p1    = new TH1F("norm_k_p1","norm_k_p1", 50, 0, 30);
  TH1F *norm_k_theta1= new TH1F("norm_k_theta1","norm_k_theta1", 50, 0, 90);
  TH1F *norm_k_x1    = new TH1F("norm_k_x1","norm_k_x1", 50, -1000, 1000);
  TH1F *norm_k_y1    = new TH1F("norm_k_y1","norm_k_y1", 50, -1000, 1000);


  TH1F *dx_pi2   = new TH1F("dx_pi2","dx_pi2", 100, -10,10); 
  TH1F *dy_pi2   = new TH1F("dy_pi2","dy_pi2", 100, -10,10);
  TH1F *dz_pi2   = new TH1F("dz_pi2","dz_pi2", 100, -3,3); 
  TH1F *dt_pi2   = new TH1F("dt_pi2","dt_pi2", 200, -1, 1); 

  TH1F *dx_k2    = new TH1F("dx_k2","dx_k2", 100, -10,10); 
  TH1F *dy_k2    = new TH1F("dy_k2","dy_k2", 100, -10,10);
  TH1F *dz_k2    = new TH1F("dz_k2","dz_k2", 100, -3,3); 
  TH1F *dt_k2    = new TH1F("dt_k2","dt_k2", 200, -1, 1 ); 

  TH1F *dx_p2    = new TH1F("dx_p2","dx_p2", 100, -10,10); 
  TH1F *dy_p2    = new TH1F("dy_p2","dy_p2", 100, -10,10);
  TH1F *dz_p2    = new TH1F("dz_p2","dz_p2", 100, -3,3);
  TH1F *dt_p2    = new TH1F("dt_p2","dt_p2", 200, -1, 1 ); 

  TH1F *eff_p_p2    = new TH1F("eff_p_p2","eff_p_p2", 50, 0, 30);
  TH1F *eff_p_theta2= new TH1F("eff_p_theta2","eff_p_theta2", 50, 0, 90);
  TH1F *eff_p_x2    = new TH1F("eff_p_x2","eff_p_x2", 50, -1000, 1000);
  TH1F *eff_p_y2    = new TH1F("eff_p_y2","eff_p_y2", 50, -1000, 1000);
  
  TH1F *eff_pi_p2    = new TH1F("eff_pi_p2","eff_pi_p2", 50, 0, 30);
  TH1F *eff_pi_theta2= new TH1F("eff_pi_theta2","eff_pi_theta2", 50, 0, 90);
  TH1F *eff_pi_x2    = new TH1F("eff_pi_x2","eff_pi_x2", 50, -1000, 1000);
  TH1F *eff_pi_y2    = new TH1F("eff_pi_y2","eff_pi_y2", 50, -1000, 1000);

  TH1F *eff_k_p2    = new TH1F("eff_k_p2","eff_k_p2", 50, 0, 30);
  TH1F *eff_k_theta2= new TH1F("eff_k_theta2","eff_k_theta2", 50, 0, 90);
  TH1F *eff_k_x2    = new TH1F("eff_k_x2","eff_k_x2", 50, -1000, 1000);
  TH1F *eff_k_y2    = new TH1F("eff_k_y2","eff_k_y2", 50, -1000, 1000);

  TH1F *norm_p_p2    = new TH1F("norm_p_p2","norm_p_p2", 50, 0, 30);
  TH1F *norm_p_theta2= new TH1F("norm_p_theta2","norm_p_theta2", 50, 0, 90);
  TH1F *norm_p_x2    = new TH1F("norm_p_x2","norm_p_x2", 50, -1000, 1000);
  TH1F *norm_p_y2    = new TH1F("norm_p_y2","norm_p_y2", 50, -1000, 1000);

  TH1F *norm_pi_p2    = new TH1F("norm_pi_p2","norm_pi_p2", 50, 0, 30);
  TH1F *norm_pi_theta2= new TH1F("norm_pi_theta2","norm_pi_theta2", 50, 0, 90);
  TH1F *norm_pi_x2    = new TH1F("norm_pi_x2","norm_pi_x2", 50, -1000, 1000);
  TH1F *norm_pi_y2    = new TH1F("norm_pi_y2","norm_pi_y2", 50, -1000, 1000);

  TH1F *norm_k_p2    = new TH1F("norm_k_p2","norm_k_p2", 50, 0, 30);
  TH1F *norm_k_theta2= new TH1F("norm_k_theta2","norm_k_theta2", 50, 0, 90);
  TH1F *norm_k_x2    = new TH1F("norm_k_x2","norm_k_x2", 50, -1000, 1000);
  TH1F *norm_k_y2    = new TH1F("norm_k_y2","norm_k_y2", 50, -1000, 1000);
    

  Float_t x_point, y_point, z_point, tof_point, x_hit, y_hit, z_hit, tof_hit;
  Float_t mom_MC, momx_MC, momy_MC, momz_MC, theta;
  TVector3 mom3_MC;

  Int_t nMCTracks, nTofPoints, mother_ID, P_ID, nTofHits;
  Int_t counter=0;
  Int_t region1=0, region2=1;
  Float_t norm=0, eff=0;
  Float_t delta_x, delta_y, delta_z, delta_tof;
  Float_t delta_x_min=1e+10, delta_x_max=1e-10, delta_y_min=1e+10, delta_y_max=1e-10, delta_z_min=1e+10, 
delta_z_max=1e-10, delta_tof_min=1e+10, delta_tof_max=1e-10;


  Float_t C=3.14159/180; 

  Float_t mass, beta;

  // Loop over events
  nevents=10; //Something very strange went on with the last events...

  for(Int_t iev = 0; iev <= nevents; iev++) {

    // Get entry
    treeSIM->GetEntry(iev);
    treeRECO->GetEntry(iev);
    // Get number of MC tracks in the event
    nMCTracks = McTracks->GetEntriesFast();
    // Get number of TOF tracks in the event
    nTofPoints = TofPoints->GetEntriesFast();
    nTofHits   = TofHits->GetEntriesFast();

    // Control output
    cout << "Event " << iev << " has "
	 << nMCTracks << " MC tracks" << endl;

    //Loop over TofTracks in order to obtain the efficiency of creation
    //of TofHits.

    for(Int_t k = 0; k<nMCTracks; k++) {
	  
      McTrack   = (CbmMCTrack*) McTracks->At(k);
      mother_ID = McTrack->GetMotherID();
      P_ID      = McTrack->GetPdgCode();

      //Select tracks with at least one point in the TOF that come from the vertex

      if (McTrack->GetTofPoints()<1) continue; 
      if(mother_ID!=-1) continue;

      counter=0;

      for(Int_t j = 0; j<nTofHits; j++) {
	
	TofHit  = (CbmTofHit*) TofHits->At(j);
	TofPoint = (CbmTofPoint*) TofPoints->At(TofHit->GetRefIndex());

	
	//Double hits are skept here (but considered later)
	
	if(TofHit->GetFlag()!=1)	  continue; 
	
	//Ask if the Hit was created by the MCtrack. Efficiency.

	if (TofPoint->GetTrackID()==k) {

	  counter=counter+1;

	  //Coordinates of the point
	  
	  x_point      = TofPoint -> GetX();
	  y_point      = TofPoint -> GetY();
	  z_point      = TofPoint -> GetZ();
	  tof_point    = TofPoint -> GetTime();

	  //Coordinates of the hit
	  
	  x_hit        = TofHit   -> GetX();
	  y_hit        = TofHit   -> GetY();
	  z_hit        = TofHit   -> GetZ();
	  tof_hit      = TofHit   -> GetTime();

	  mom3_MC = McTrack->GetMomentum();
	  momx_MC = mom3_MC.Px(); 
	  momy_MC = mom3_MC.Py();
	  momz_MC = mom3_MC.Pz();
	  mom_MC  = sqrt(momx_MC*momx_MC + momy_MC*momy_MC + momz_MC*momz_MC);

	  theta = 1/C*acos(momz_MC/mom_MC);

	  delta_x   =  x_hit   - x_point;
	  delta_y   =  y_hit   - y_point;
	  delta_z   =  z_hit   - z_point;
	  delta_tof =  tof_hit - tof_point;
	  
	  if(delta_x<delta_x_min) delta_x_min = delta_x;
	  if(delta_x>delta_x_max) delta_x_max = delta_x;
	  if(delta_y<delta_y_min) delta_y_min = delta_y;
	  if(delta_y>delta_y_max) delta_y_max = delta_y;
	  if(delta_z<delta_z_min) delta_z_min = delta_z;
	  if(delta_z>delta_z_max) delta_z_max = delta_z;
	  if(delta_tof<delta_tof_min) delta_tof_min = delta_tof;
	  if(delta_tof>delta_tof_max) delta_tof_max = delta_tof;
		  
	  //Fill histos!

	   xy->Fill(x_point,y_point);
	   z->Fill(z_point);
	   t->Fill(tof_point);
	   nreg->Fill(TofPoint->GetRegion());
	   nmod->Fill(TofPoint->GetModule());
	   ncel->Fill(TofPoint->GetCell());
	   ngap->Fill(TofPoint->GetGap());
	   detID->Fill(TofPoint->GetDetectorID());

	   dx_all->Fill(delta_x);
	   dy_all->Fill(delta_y);
	   dz_all->Fill(delta_z);
	   dt_all->Fill(delta_tof);

	  if (TofHit -> GetRegion()==region1) {
	
	    if (P_ID==2212) { //proton
	      dx_p1->Fill(delta_x);
	      dy_p1->Fill(delta_y);
	      dz_p1->Fill(delta_z);
	      dt_p1->Fill(delta_tof);
	      norm_p_p1->Fill(mom_MC);
	      norm_p_theta1->Fill(theta);
	      norm_p_x1->Fill(x_point);
	      norm_p_y1->Fill(y_point);
	      eff_p_p1->Fill(mom_MC);
	      eff_p_theta1->Fill(theta);
	      eff_p_x1->Fill(x_point);
	      eff_p_y1->Fill(y_point);
	    }

	    if (P_ID==321 || P_ID==-321) { //K+ or K-
	      dx_k1->Fill(delta_x);
	      dy_k1->Fill(delta_y);
	      dz_k1->Fill(delta_z);
	      dt_k1->Fill(delta_tof);
	      norm_pi_p1->Fill(mom_MC);
	      norm_pi_theta1->Fill(theta);
	      norm_pi_x1->Fill(x_point);
	      norm_pi_y1->Fill(y_point);
	      eff_pi_p1->Fill(mom_MC);
	      eff_pi_theta1->Fill(theta);
	      eff_pi_x1->Fill(x_point);
	      eff_pi_y1->Fill(y_point);
	    }
	
	    if (P_ID==211 || P_ID==-211) { //pi+ and pi-
	      dx_pi1->Fill(delta_x);
	      dy_pi1->Fill(delta_y);
	      dz_pi1->Fill(delta_z);
	      dt_pi1->Fill(delta_tof);
	      norm_k_p1->Fill(mom_MC);
	      norm_k_theta1->Fill(theta);
	      norm_k_x1->Fill(x_point);
	      norm_k_y1->Fill(y_point);
	      eff_k_p1->Fill(mom_MC);
	      eff_k_theta1->Fill(theta);
	      eff_k_x1->Fill(x_point);
	      eff_k_y1->Fill(y_point);
	    }
	  }


	  if (TofHit -> GetRegion()==region2) {
	
	    if (P_ID==2212) { //proton
	      dx_p2->Fill(delta_x);
	      dy_p2->Fill(delta_y);
	      dz_p2->Fill(delta_z);
	      dt_p2->Fill(delta_tof);
	      norm_p_p2->Fill(mom_MC);
	      norm_p_theta2->Fill(theta);
	      norm_p_x2->Fill(x_point);
	      norm_p_y2->Fill(y_point);
	      eff_p_p2->Fill(mom_MC);
	      eff_p_theta2->Fill(theta);
	      eff_p_x2->Fill(x_point);
	      eff_p_y2->Fill(y_point);
	    }

	    if (P_ID==321 || P_ID==-321) { //K+ or K-
	      dx_k2->Fill(delta_x);
	      dy_k2->Fill(delta_y);
	      dz_k2->Fill(delta_z);
	      dt_k2->Fill(delta_tof);
	      norm_pi_p2->Fill(mom_MC);
	      norm_pi_theta2->Fill(theta);
	      norm_pi_x2->Fill(x_point);
	      norm_pi_y2->Fill(y_point);
	      eff_pi_p2->Fill(mom_MC);
	      eff_pi_theta2->Fill(theta);
	      eff_pi_x2->Fill(x_point);
	      eff_pi_y2->Fill(y_point);
	    }
	
	    if (P_ID==211 || P_ID==-211) { //pi+ and pi-
	      dx_pi2->Fill(delta_x);
	      dy_pi2->Fill(delta_y);
	      dz_pi2->Fill(delta_z);
	      dt_pi2->Fill(delta_tof);
	      norm_k_p2->Fill(mom_MC);
	      norm_k_theta2->Fill(theta);
	      norm_k_x2->Fill(x_point);
	      norm_k_y2->Fill(y_point);
	      eff_k_p2->Fill(mom_MC);
	      eff_k_theta2->Fill(theta);
	      eff_k_x2->Fill(x_point);
	      eff_k_y2->Fill(y_point);
	    }
	  }

	  beta = (TofPoint->GetLength())/tof_hit/30;
	  mass = mom_MC*mom_MC*(1/beta/beta-1);
	  p_mass->Fill(mass,mom_MC);
	  norm++;
	  eff++;

	  // NOTE: the algorithm takes the first Hit in the array. If there are more, the following
	  // are not considered
	  
	  break;
	}
      }
      
      if (counter==0) {//It was not detected. Search for the point corresponding to the MC track.

	for(Int_t j = 0; j<nTofPoints; j++) {
	  TofPoint  = (CbmTofPoint*) TofPoints->At(j);
	  
	  //Ask if it corresponds to the MC track.

	  if (TofPoint->GetTrackID()==k) {

	    //Coordinates of the point

	    x_point      = TofPoint -> GetX();
	    y_point      = TofPoint -> GetY();
	    z_point      = TofPoint -> GetZ();
	    tof_point    = TofPoint -> GetTime();

	    mom3_MC = McTrack->GetMomentum();
	    momx_MC = mom3_MC.Px(); 
	    momy_MC = mom3_MC.Py();
	    momz_MC = mom3_MC.Pz();
	    mom_MC  = sqrt(momx_MC*momx_MC + momy_MC*momy_MC + momz_MC*momz_MC);

	    theta = 1/C*acos(momz_MC/mom_MC);

	    // Histos for normalization

	    if (TofPoint -> GetRegion()==region1) {
	      if (P_ID==2212) { //proton
		norm_p_p1->Fill(mom_MC);
		norm_p_theta1->Fill(theta);
		norm_p_x1->Fill(x_point);
		norm_p_y1->Fill(y_point);
	      }

	      if (P_ID==321 || P_ID==-321) { //K+ or K-
		norm_pi_p1->Fill(mom_MC);
		norm_pi_theta1->Fill(theta);
		norm_pi_x1->Fill(x_point);
		norm_pi_y1->Fill(y_point);
	      }
	    
	      if (P_ID==211 || P_ID==-211) { //pi+ and pi-
		norm_k_p1->Fill(mom_MC);
		norm_k_theta1->Fill(theta);
		norm_k_x1->Fill(x_point);
		norm_k_y1->Fill(y_point);
	      }	      
	    }
	
	    if (TofPoint -> GetRegion()==region2) {
	      if (P_ID==2212) { //proton
		norm_p_p2->Fill(mom_MC);
		norm_p_theta2->Fill(theta);
		norm_p_x2->Fill(x_point);   //p
		norm_p_y2->Fill(y_point);
	      }

	      if (P_ID==321 || P_ID==-321) { //K+ or K-
		norm_pi_p2->Fill(mom_MC);
		norm_pi_theta2->Fill(theta);
		norm_pi_x2->Fill(x_point);
		norm_pi_y2->Fill(y_point);
	      }
	    
	      if (P_ID==211 || P_ID==-211) { //pi+ and pi-
		norm_k_p2->Fill(mom_MC);
		norm_k_theta2->Fill(theta);
		norm_k_x2->Fill(x_point);
		norm_k_y2->Fill(y_point);
	      }
	    }
	    norm++;
	    break;
	  }
	}
      }
    }

    //After closing the efficiency loop I deal with double hits

    for(Int_t j = 0; j<nTofHits; j++) {

      	TofHit  = (CbmTofHit*) TofHits->At(j);

      if(TofHit->GetFlag()==1) {	
	TofPoint  = (CbmTofPoint*) TofPoints->At(TofHit->GetRefIndex());
	McTrack   = (CbmMCTrack*)  McTracks->At(TofPoint->GetTrackID());
	beta    = (TofPoint->GetLength())/(TofHit->GetTime())/30;
	mom3_MC = McTrack->GetMomentum();
	momx_MC = mom3_MC.Px(); 
	momy_MC = mom3_MC.Py();
	momz_MC = mom3_MC.Pz();
	mom_MC  = sqrt(momx_MC*momx_MC + momy_MC*momy_MC + momz_MC*momz_MC);
	mass    = mom_MC*mom_MC*(1/beta/beta-1);
	p_mass_all->Fill(mass,mom_MC);
      }


      if(TofHit->GetFlag()!=1) {
	//Flag=2 means that the faster dominates the timing, anyway	
	if(TofHit->GetFlag()==2) {

	  TofPoint  = (CbmTofPoint*) TofPoints->At(TofHit->GetRefIndex());
	  McTrack   = (CbmMCTrack*)  McTracks->At(TofPoint->GetTrackID());
	  mother_ID = McTrack->GetMotherID();
	  if(mother_ID!=-1) continue;  //Note that I take only those from target

	  beta    = (TofPoint->GetLength())/(TofHit->GetTime())/30;
	  mom3_MC = McTrack->GetMomentum();
	  momx_MC = mom3_MC.Px(); 
	  momy_MC = mom3_MC.Py();
	  momz_MC = mom3_MC.Pz();
	  mom_MC  = sqrt(momx_MC*momx_MC + momy_MC*momy_MC + momz_MC*momz_MC);
	  mass    = mom_MC*mom_MC*(1/beta/beta-1);
	  p_mass_double->Fill(mass,mom_MC);

	}
      
	//Flag>2, then now gives the RefIndex plus a 10 offset
	else {

	  //The first one
	  TofPoint  = (CbmTofPoint*) TofPoints->At(TofHit->GetRefIndex());
	  McTrack   = (CbmMCTrack*) McTracks->At(TofPoint->GetTrackID());
	  mother_ID = McTrack->GetMotherID();
	  if(mother_ID!=-1) continue;   //Note that I take only those from target

	  beta    = (TofPoint2->GetLength())/(TofHit->GetTime())/30;
	  mom3_MC = McTrack->GetMomentum();
	  momx_MC = mom3_MC.Px(); 
	  momy_MC = mom3_MC.Py();
	  momz_MC = mom3_MC.Pz();
	  mom_MC  = sqrt(momx_MC*momx_MC + momy_MC*momy_MC + momz_MC*momz_MC);
	  mass    = mom_MC*mom_MC*(1/beta/beta-1);
	  p_mass_interf->Fill(mass,mom_MC);

	  //The second one
	  TofPoint2 = (CbmTofPoint*) TofPoints->At((TofHit->GetFlag()-10));
	  McTrack   = (CbmMCTrack*) McTracks->At(TofPoint2->GetTrackID());
	  mother_ID = McTrack->GetMotherID();
	  if(mother_ID!=-1) continue;   //Note that I take only those from target

	  beta    = (TofPoint2->GetLength())/(TofHit->GetTime())/30;
	  mom3_MC = McTrack->GetMomentum();
	  momx_MC = mom3_MC.Px(); 
	  momy_MC = mom3_MC.Py();
	  momz_MC = mom3_MC.Pz();
	  mom_MC  = sqrt(momx_MC*momx_MC + momy_MC*momy_MC + momz_MC*momz_MC);
	  mass    = mom_MC*mom_MC*(1/beta/beta-1);
	} 
      }
    }
  }

  //Point distributions

  TCanvas *c2 = new TCanvas("TOF point distributions","TOF point distributions");

  gPad->Divide(4,2);
	      
  c2->cd(1);
  xy->SetXTitle("x[cm]");
  xy->SetYTitle("y[cm]");
  xy->Draw("colz");
  
  c2->cd(2);
  z->SetXTitle("z[cm]");
  z->Draw("");
  c2->cd(3);
  t->SetXTitle("tof[ns]");
  t->Draw("");  
  c2->cd(4);
  nreg->SetXTitle("region");
  nreg->Draw("");  
  c2->cd(5);
  nmod->SetXTitle("module");
  nmod->Draw("");
  c2->cd(6);
  ncel->SetXTitle("cell");
  ncel->Draw("");
  c2->cd(7);
  ngap->SetXTitle("gap");
  ngap->Draw("");
  c2->cd(8);
  detID->SetXTitle("detector ID");
  detID->Draw("");


  //Residuals (region 1)

  TCanvas *c3 = new TCanvas("Distributions of residuals of region 1", "Distribution of residuals of region 1");

  gPad->Divide(2,2);

  c3->cd(1);
  dx_pi1->SetXTitle("x_{hit} - x_{point} [cm]");
  dx_pi1->Scale(1./(dx_pi1->GetEntries()));
  dx_pi1->Draw("");
  dx_pi1->SetLineColor(2); //red
  dx_pi1->SetLineStyle(2);
  dx_p1->Scale(1./(dx_p1->GetEntries()));
  dx_p1->Draw("SAME");
  dx_p1->SetLineColor(3);  //green
  dx_p1->SetLineStyle(2); 
  dx_k1->Scale(1./(dx_k1->GetEntries()));
  dx_k1->Draw("SAME");
  dx_k1->SetLineColor(4);  //blue
  dx_k1->SetLineStyle(2); 
 
  c3->cd(2);
  dy_pi1->SetXTitle("y_{hit} - y_{point} [cm]");
  dy_pi1->Scale(1./(dy_pi1->GetEntries()));
  dy_pi1->Draw("");
  dy_pi1->SetLineColor(2);
  dy_pi1->SetLineStyle(2);
  dy_p1->Scale(1./(dy_p1->GetEntries())); 
  dy_p1->Draw("SAME");
  dy_p1->SetLineColor(3);
  dy_p1->SetLineStyle(2);
  dy_k1->Scale(1./(dy_k1->GetEntries()));
  dy_k1->Draw("SAME");
  dy_k1->SetLineColor(4);
  dy_k1->SetLineStyle(2); 

  c3->cd(3);
  dz_pi1->SetXTitle("z_{hit} - z_{point} [cm]");
  dz_pi1->Scale(1./(dz_pi1->GetEntries()));
  dz_pi1->Draw("");
  dz_pi1->SetLineColor(2);
  dz_pi1->SetLineStyle(2);
  dz_p1->Scale(1./(dz_p1->GetEntries()));
  dz_p1->Draw("SAME");
  dz_p1->SetLineColor(3);
  dz_p1->SetLineStyle(2);
  dz_k1->Scale(1./(dz_k1->GetEntries()));
  dz_k1->Draw("SAME");
  dz_k1->SetLineColor(4);
  dz_k1->SetLineStyle(2); 

  c3->cd(4);
  dt_pi1->SetXTitle("tof_{hit} - tof_{point} [ns]");
  dt_pi1->Scale(1./(dt_pi1->GetEntries()));
  dt_pi1->Draw("");
  dt_pi1->SetLineColor(2);
  dt_pi1->SetLineStyle(2);
  dt_p1->Scale(1./(dt_p1->GetEntries()));
  dt_p1->Draw("SAME");
  dt_p1->SetLineColor(3);
  dt_p1->SetLineStyle(2); 
  dt_k1->Scale(1./(dt_k1->GetEntries()));
  dt_k1->Draw("SAME");
  dt_k1->SetLineColor(4);
  dt_k1->SetLineStyle(2);

  //Residuals (region 2)

  TCanvas *c4 = new TCanvas("Distributions of residuals of region 2", "Distribution of residuals of region 2");

  gPad->Divide(2,2);

  c4->cd(1);
  dx_pi2->SetXTitle("x_{hit} - x_{point} [cm]");
  dx_pi2->Scale(1./(dx_pi2->GetEntries()));
  dx_pi2->Draw("");
  dx_pi2->SetLineColor(2); //red
  dx_pi2->SetLineStyle(2);
  dx_p2->Scale(1./(dx_p2->GetEntries()));
  dx_p2->Draw("SAME");
  dx_p2->SetLineColor(3);  //green
  dx_p2->SetLineStyle(2); 
  dx_k2->Scale(1./(dx_k2->GetEntries()));
  dx_k2->Draw("SAME");
  dx_k2->SetLineColor(4);  //blue
  dx_k2->SetLineStyle(2); 
 
  c4->cd(2);
  dy_pi2->SetXTitle("y_{hit} - y_{point} [cm]");
  dy_pi2->Scale(1./(dy_pi2->GetEntries()));
  dy_pi2->Draw("");
  dy_pi2->SetLineColor(2);
  dy_pi2->SetLineStyle(2);
  dy_p2->Scale(1./(dy_p2->GetEntries())); 
  dy_p2->Draw("SAME");
  dy_p2->SetLineColor(3);
  dy_p2->SetLineStyle(2);
  dy_k2->Scale(1./(dy_k2->GetEntries()));
  dy_k2->Draw("SAME");
  dy_k2->SetLineColor(4);
  dy_k2->SetLineStyle(2); 

  c4->cd(3);
  dz_pi2->SetXTitle("z_{hit} - z_{point} [cm]");
  dz_pi2->Scale(1./(dz_pi2->GetEntries()));
  dz_pi2->Draw("");
  dz_pi2->SetLineColor(2);
  dz_pi2->SetLineStyle(2);
  dz_p2->Scale(1./(dz_p2->GetEntries()));
  dz_p2->Draw("SAME");
  dz_p2->SetLineColor(3);
  dz_p2->SetLineStyle(2);
  dz_k2->Scale(1./(dz_k2->GetEntries()));
  dz_k2->Draw("SAME");
  dz_k2->SetLineColor(4);
  dz_k2->SetLineStyle(2); 

  c4->cd(4);
  dt_pi2->SetXTitle("tof_{hit} - tof_{point} [ns]");
  dt_pi2->Scale(1./(dt_pi2->GetEntries()));
  dt_pi2->Draw("");
  dt_pi2->SetLineColor(2);
  dt_pi2->SetLineStyle(2);
  dt_p2->Scale(1./(dt_p2->GetEntries()));
  dt_p2->Draw("SAME");
  dt_p2->SetLineColor(3);
  dt_p2->SetLineStyle(2); 
  dt_k2->Scale(1./(dt_k2->GetEntries()));
  dt_k2->Draw("SAME");
  dt_k2->SetLineColor(4);
  dt_k2->SetLineStyle(2);

  //Residuals (all)

  TCanvas *c5 = new TCanvas("Distributions of all residuals", "Distribution of all residuals");

  gPad->Divide(2,2);

  c5->cd(1);
  dx_all->SetXTitle("x_{hit} - x_{point} [cm]");
  dx_all->SetAxisRange(delta_x_min,delta_x_max,"X");
  dx_all->Draw("");
 
  c5->cd(2);
  dy_all->SetXTitle("y_{hit} - y_{point} [cm]");
  dy_all->SetAxisRange(delta_y_min,delta_y_max,"X");
  dy_all->Draw("");

  c5->cd(3);
  dz_all->SetXTitle("z_{hit} - z_{point} [cm]");
  dz_all->SetAxisRange(delta_z_min,delta_z_max,"X");
  dz_all->Draw("");

  c5->cd(4);
  dt_all->SetXTitle("tof_{hit} - tof_{point} [ns]");
  dt_all->SetAxisRange(delta_tof_min,delta_tof_max,"X");
  dt_all->Draw("");

  //Efficiencies of Hit production (region 1)

  TCanvas *c6 = new TCanvas("Tof Hit creation efficiency in region 1","Tof Hit creation efficiency in region 1");

  gPad->Divide(2,2);

  c6->cd(1);
  eff_pi_p1->SetXTitle("p_{geant} [GeV/c]");
  eff_pi_p1->SetYTitle("eff");
  eff_pi_p1->Divide(norm_pi_p1);
  eff_pi_p1->Draw("");
  eff_pi_p1->SetLineColor(2); //red
  eff_pi_p1->SetLineStyle(2); 
  eff_p_p1->Divide(norm_p_p1);
  eff_p_p1->Draw("SAME");
  eff_p_p1->SetLineColor(3);  //green
  eff_p_p1->SetLineStyle(2);
  eff_k_p1->Divide(norm_k_p1);
  eff_k_p1->Draw("SAME");
  eff_k_p1->SetLineColor(4);  //blue
  eff_k_p1->SetLineStyle(2); 
 
  c6->cd(2);
  eff_pi_theta1->SetXTitle("theta_{geant} [deg]");
  eff_pi_theta1->SetYTitle("eff");
  eff_pi_theta1->Divide(norm_pi_theta1);
  eff_pi_theta1->Draw("");
  eff_pi_theta1->SetLineColor(2); //red
  eff_pi_theta1->SetLineStyle(2); 
  eff_p_theta1->Divide(norm_p_theta1);
  eff_p_theta1->Draw("SAME");
  eff_p_theta1->SetLineColor(3);  //green
  eff_p_theta1->SetLineStyle(2);
  eff_k_theta1->Divide(norm_k_theta1);
  eff_k_theta1->Draw("SAME");
  eff_k_theta1->SetLineColor(4);  //blue
  eff_k_theta1->SetLineStyle(2); 
 
  c6->cd(3);
  eff_pi_x1->SetXTitle("X_{geant} [cm]");
  eff_pi_x1->SetYTitle("eff");
  eff_pi_x1->Divide(norm_pi_x1);
  eff_pi_x1->Draw("");
  eff_pi_x1->SetLineColor(2); //red
  eff_pi_x1->SetLineStyle(2); 
  eff_p_x1->Divide(norm_p_x1);
  eff_p_x1->Draw("SAME");
  eff_p_x1->SetLineColor(3);  //green
  eff_p_x1->SetLineStyle(2);
  eff_k_x1->Divide(norm_k_x1);
  eff_k_x1->Draw("SAME");
  eff_k_x1->SetLineColor(4);  //blue
  eff_k_x1->SetLineStyle(2); 
 
  c6->cd(4);
  eff_pi_y1->SetXTitle("Y_{geant} [cm]");
  eff_pi_y1->SetYTitle("eff");
  eff_pi_y1->Divide(norm_pi_y1);
  eff_pi_y1->Draw("");
  eff_pi_y1->SetLineColor(2); //red
  eff_pi_y1->SetLineStyle(2); 
  eff_p_y1->Divide(norm_p_y1);
  eff_p_y1->Draw("SAME");
  eff_p_y1->SetLineColor(3);  //green
  eff_p_y1->SetLineStyle(2);
  eff_k_y1->Divide(norm_k_y1);
  eff_k_y1->Draw("SAME");
  eff_k_y1->SetLineColor(4);  //blue
  eff_k_y1->SetLineStyle(2); 

  //Efficiencies of Hit production (region 2)

  TCanvas *c7 = new TCanvas("Tof Hit creation efficiency in region 2","Tof Hit creation efficiency in region 2");

  gPad->Divide(2,2);

  c7->cd(1);
  eff_pi_p2->SetXTitle("p_{geant} [GeV/c]");
  eff_pi_p2->SetYTitle("eff");
  eff_pi_p2->Divide(norm_pi_p2);
  eff_pi_p2->Draw("");
  eff_pi_p2->SetLineColor(2); //red
  eff_pi_p2->SetLineStyle(2); 
  eff_p_p2->Divide(norm_p_p2);
  eff_p_p2->Draw("SAME");
  eff_p_p2->SetLineColor(3);  //green
  eff_p_p2->SetLineStyle(2);
  eff_k_p2->Divide(norm_k_p2);
  eff_k_p2->Draw("SAME");
  eff_k_p2->SetLineColor(4);  //blue
  eff_k_p2->SetLineStyle(2); 
 
  c7->cd(2);
  eff_pi_theta2->SetXTitle("theta_{geant} [deg]");
  eff_pi_theta2->SetYTitle("eff");
  eff_pi_theta2->Divide(norm_pi_theta2);
  eff_pi_theta2->Draw("");
  eff_pi_theta2->SetLineColor(2); //red
  eff_pi_theta2->SetLineStyle(2); 
  eff_p_theta2->Divide(norm_p_theta2);
  eff_p_theta2->Draw("SAME");
  eff_p_theta2->SetLineColor(3);  //green
  eff_p_theta2->SetLineStyle(2);
  eff_k_theta2->Divide(norm_k_theta2);
  eff_k_theta2->Draw("SAME");
  eff_k_theta2->SetLineColor(4);  //blue
  eff_k_theta2->SetLineStyle(2); 
 
  c7->cd(3);
  eff_pi_x2->SetXTitle("X_{geant} [cm]");
  eff_pi_x2->SetYTitle("eff");
  eff_pi_x2->Divide(norm_pi_x2);
  eff_pi_x2->Draw("");
  eff_pi_x2->SetLineColor(2); //red
  eff_pi_x2->SetLineStyle(2); 
  eff_p_x2->Divide(norm_p_x2);
  eff_p_x2->Draw("SAME");
  eff_p_x2->SetLineColor(3);  //green
  eff_p_x2->SetLineStyle(2);
  eff_k_x2->Divide(norm_k_x2);
  eff_k_x2->Draw("SAME");
  eff_k_x2->SetLineColor(4);  //blue
  eff_k_x2->SetLineStyle(2); 

  c7->cd(4);
  eff_pi_y2->SetXTitle("Y_{geant} [cm]");
  eff_pi_y2->SetYTitle("eff");
  eff_pi_y2->Divide(norm_pi_y2);
  eff_pi_y2->Draw("");
  eff_pi_y2->SetLineColor(2); //red
  eff_pi_y2->SetLineStyle(2); 
  eff_p_y2->Divide(norm_p_y2);
  eff_p_y2->Draw("SAME");
  eff_p_y2->SetLineColor(3);  //green
  eff_p_y2->SetLineStyle(2);
  eff_k_y2->Divide(norm_k_y2);
  eff_k_y2->Draw("SAME");
  eff_k_y2->SetLineColor(4);  //blue
  eff_k_y2->SetLineStyle(2); 

  TCanvas *c8 = new TCanvas("p vs mass2 for single hits","p vs mass2 for single hits");

  gPad->Divide(2,2);

  c8->cd(1);
  p_mass->SetYTitle("p_{geant} [GeV/c]");
  p_mass->SetXTitle("m^{2} (t_{hit}) [GeV/c]");
  p_mass->Draw("colz");

  c8->cd(2);
  p_mass_all->SetYTitle("p_{geant} [GeV/c]");
  p_mass_all->SetXTitle("m^{2} (t_{hit}) [GeV/c]");
  p_mass_all->Draw("colz");

  c8->cd(3);
  p_mass_double->SetYTitle("p_{geant} [GeV/c]");
  p_mass_double->SetXTitle("m^{2} (t_{hit}) [GeV/c]");
  p_mass_double->Draw("colz");

  c8->cd(4);
  p_mass_interf->SetYTitle("p_{geant} [GeV/c]");
  p_mass_interf->SetXTitle("m^{2} (t_{hit}) [GeV/c]");
  p_mass_interf->Draw("colz");

  cout<<"Total efficiency of creation of a single hit in a cell =  "<<eff/norm<<endl;

  return 1;
}


