
/** The CbmV0Analysis class
 *
 * @author  Y.Vassiliev open charm including V0 => K0s and Lambda...
 * @version 3.0 trunk
 * @since   07.12.10
 * 
 */

#include "CbmV0Analysis.h"

#include "FairRunAna.h"
#include "FairRootManager.h"
#include "CbmGeoStsPar.h"
//#include "CbmGeoTrdPar.h"
//#include "CbmGeoPassivePar.h"
#include "CbmStsPoint.h"
#include "CbmMCTrack.h"
#include "CbmStsTrack.h"
#include "CbmVertex.h"
#include "CbmStsKFTrackFitter.h"
#include "CbmStsTrack.h"
#include "CbmTrackMatch.h"
#include "CbmStsHit.h"

#include "TCanvas.h"
#include "TTree.h"
#include "TProfile2D.h"
#include "TCanvas.h"
#include "TEllipse.h"
#include "TText.h"
#include "TMarker.h"
#include "TStyle.h"
#include "TH3.h"
#include "TH2F.h"
#include "TClonesArray.h"
#include "TDatabasePDG.h"

#include "CbmKF.h"
#include "CbmKFFieldMath.h"

#include "CbmKFMath.h"
#include "CbmKFHit.h"
#include "CbmKFTrack.h"
#include "CbmKFVertex.h"
#include "CbmKFTrackInterface.h"
#include "CbmStsKFSecondaryVertexFinder.h"
#include "CbmKFParticle.h"
#include "CbmKFTrack.h"
#include <list>

#include <iostream>

ClassImp(CbmV0TrackInfo);
ClassImp(CbmV0Analysis)



CbmV0Analysis::CbmV0Analysis(const char *name, const char *title, Int_t iVerbose):FairTask(name,iVerbose){
}

CbmV0Analysis::~CbmV0Analysis(){
}

void CbmV0Analysis::SetParContainers(){
}

InitStatus CbmV0Analysis::ReInit(){
 FairRootManager *fManger = FairRootManager::Instance();

  listStsPts = (TClonesArray *)  fManger->GetObject("StsPoint");
  listMCTracks = (TClonesArray*) fManger->GetObject("MCTrack");
  listStsTracks = (TClonesArray *)  fManger->GetObject("StsTrack");
  fSTSTrackMatch = (TClonesArray*) fManger->GetObject("StsTrackMatch");
  fPrimVtx = (CbmVertex*) fManger->GetObject("PrimaryVertex");
  fHitArray = (TClonesArray *)  fManger->GetObject("StsHit");
  fMvdHitArray = (TClonesArray *)  fManger->GetObject("MvdHit");
  return kSUCCESS;
}

InitStatus CbmV0Analysis::Init(){
  return ReInit();
}

void CbmV0Analysis::Exec(Option_t * option){
  // generate primary vertex
  { 
      const Double_t  SigmaX = 1.e-4,  SigmaY = 1.e-4, SigmaZ = 5.e-4; 
      CbmKFVertex v;
      v.GetRefX() = 0;//gRandom->Gaus(0,SigmaX); 
      v.GetRefY() = 0;//gRandom->Gaus(0,SigmaY); 
      v.GetRefZ() = 0;//gRandom->Gaus(0,SigmaZ); 
      Double_t *C = v.GetCovMatrix();
      for( int i=0; i<6; i++ ) C[i] = 0;
      C[0] = SigmaX*SigmaX;
      C[2] = SigmaY*SigmaY;
      C[5] = SigmaZ*SigmaZ;
      v.GetVertex( GenVtx );
//            fPrimVtx = &GenVtx; //uncomment if need primary vertex for the detached signal
  }
EventAnalysis();
}

void CbmV0Analysis::Finish(){
}


void CbmV0Analysis::Set( CbmV0TrackInfo &T, int it, int iEvt ){

  T.is_ghost = 1;
  T.is_long = 0;
  T.iEvent = iEvt;
  T.NHits = 0;
  T.pdg = T.mc_pdg = T.mc_MotherID = T.mc_MotherPDG = T.mc_GrandMotherPDG =0;
  T.ChiPrimary = T.imx = T.imy = T.imr = 9999;
  T.p  = T.px = T.py = T.pz = T.pt = T.tx = T.ty = T.m = T.q = T.E = T.mc_x = T.mc_y = T.mc_z = T.mc_MotherZv = 0.;
  T.p_mc  = T.px_mc = T.py_mc = T.pz_mc = T.q_mc = 0.;

  if(!listStsTracks) return;
  CbmStsTrack *t = (CbmStsTrack*) listStsTracks->At( it );
  if( !t ) return;
  T.track = *t;
  T.is_long = IsLong(T.track);
  Fill(T);

  if(!fSTSTrackMatch) return;
  CbmTrackMatch *m = (CbmTrackMatch*) fSTSTrackMatch->At(it);
  if( !m ) return;
  T.is_ghost = m->GetNofTrueHits()<
    0.7*(m->GetNofTrueHits()+m->GetNofWrongHits()+m->GetNofFakeHits());
  Int_t mcTrackID = m->GetMCTrackId();
  if(mcTrackID < 0) return;
  if(!listMCTracks) return;
  CbmMCTrack* tm = (CbmMCTrack*)  listMCTracks->At(mcTrackID);
  if( !tm ) return;
  T.mc_track= *tm;

  
  TVector3 vr, mom_mc;
  T.mc_track.GetStartVertex(vr);
  T.mc_track.GetMomentum(mom_mc);
  T.mc_x = vr.X();
  T.mc_y = vr.Y();
  T.mc_z = vr.Z();
  T.mc_pdg  = T.mc_track.GetPdgCode();	  
  T.mc_MotherID = T.mc_track.GetMotherId();
  T.p_mc  = mom_mc.Mag();
  T.px_mc = mom_mc.X();
  T.py_mc = mom_mc.Y();
  T.pz_mc = mom_mc.Z();
  T.q_mc = 1;
  

   if( T.mc_MotherID>=0 ){
     CbmMCTrack *mother = (CbmMCTrack*) listMCTracks->At( T.mc_MotherID);
     if( !mother) return;
     T.mc_MotherPDG = mother->GetPdgCode();
     TVector3 vrM;
     mother->GetStartVertex(vrM);
     T.mc_MotherZv =  vrM.Z();
     int GrandMotherId = mother->GetMotherId();
    T.mc_GrandMotherID = GrandMotherId;
	if(GrandMotherId > 0 ){
     		CbmMCTrack *GrandMother = (CbmMCTrack*) listMCTracks->At(GrandMotherId);
     		 if( !GrandMother) return;
		T.mc_GrandMotherPDG = GrandMother->GetPdgCode();
	}
  }
}  

void CbmV0Analysis::ReFit( CbmV0TrackInfo &T, int pdg ){
  //if( !T.track ) return;
  Fitter.DoFit( &T.track, pdg );
  Fill(T);
}

void CbmV0Analysis::Fill(CbmV0TrackInfo &T){

  T.ChiPrimary = Fitter.GetChiToVertex(&T.track,fPrimVtx);
  FairTrackParam e_track;
  double zv = ( fPrimVtx )? fPrimVtx->GetZ() :0;
  if(zv >1.0 || zv < -1.0) zv=0.0; //safety by Yuri
  Fitter.Extrapolate( &T.track, zv, &e_track );

  TVector3 pos, mom;
  e_track.Position(pos);
  e_track.Momentum(mom);
  
  T.p  = mom.Mag();
  T.px = mom.X();
  T.py = mom.Y();
  T.pz = mom.Z();
  T.pt = sqrt(T.px*T.px + T.py*T.py);
  T.imx = pos.X();
  T.imy = pos.Y();
  T.imr = sqrt(T.imx*T.imx + T.imy*T.imy);
  T.pdg = T.track.GetPidHypo();
  T.m = TDatabasePDG::Instance()->GetParticle(T.pdg)->Mass();
  T.q = e_track.GetQp()>0 ?1 :-1;
  T.E = sqrt(T.p*T.p + T.m*T.m);
  if( fabs(T.pz)>1.e-8 ){
    T.tx = T.px/T.pz;
    T.ty = T.py/T.pz;
  }else T.tx = T.ty = 0.;
}

void CbmV0Analysis::SetFromMC( CbmV0TrackInfo &T, int it, int iEvt ){

  T.is_ghost = 1;
  T.is_long = 0;
  T.iEvent = iEvt;
  T.pdg = T.mc_pdg  = T.mc_MotherID = 0;
  T.ChiPrimary = T.imx = T.imy = T.imr = 9999;
  T.p  = T.px = T.py = T.pz = T.pt = T.tx = T.ty = T.m = T.q = T.E = T.mc_x = T.mc_y = T.mc_z = 0.;
  T.mc_MotherPDG = 0;
  if(!listMCTracks) return;
  CbmMCTrack *tm = (CbmMCTrack*) listMCTracks->At( it );
  if( !tm ) return;
  T.mc_track = *tm;

  TVector3 vr;
  T.mc_track.GetStartVertex(vr);

  TLorentzVector vp;
  T.mc_track.Get4Momentum(vp);

  T.mc_x = vr.X();
  T.mc_y = vr.Y();
  T.mc_z = vr.Z();
  T.mc_pdg  = T.mc_track.GetPdgCode();
  T.mc_MotherID = T.mc_track.GetMotherId();
   if( T.mc_MotherID>=0 ){
     CbmMCTrack *mother = (CbmMCTrack*) listMCTracks->At( T.mc_MotherID);
     T.mc_MotherPDG = mother->GetPdgCode();
  }

  T.p = vp.P();
  T.px = vp.Px();
  T.py = vp.Py();
  T.pz = vp.Pz();
  T.pt = sqrt(T.px*T.px + T.py*T.py);
  if( fabs(T.pz)>1.e-8 ){
    T.tx = T.px/T.pz;
    T.ty = T.py/T.pz;
  }else T.tx = T.ty = 0.;

  T.imx = T.mc_x - T.tx*(T.mc_z - 0.0); // 0.0 is Z-plane of track projection
  T.imy = T.mc_y - T.ty*(T.mc_z - 0.0);
  T.imr = sqrt(T.imx*T.imx + T.imy*T.imy);
  T.pdg  = T.mc_pdg;
  if ( T.pdg < 9999999 ){
    T.q = TDatabasePDG::Instance()->GetParticle(T.pdg)->Charge()/3.0;
    T.m = TDatabasePDG::Instance()->GetParticle(T.pdg)->Mass();      
  }else{
    T.q = T.m = 0;
  }
  T.E = sqrt(T.m*T.m + T.p*T.p);
}  


void CbmV0Analysis::EventAnalysis(){



  ///********************* Create the Tree of the tracks-candidates ********************
static TH1F *LambdaMotherPDG;
static TH1F *zv_primary;
static TH1F *NRecoTracks;
static TH1F *Ks_geo_mass, *mc_Ks_geo_mass,*Ks_geo_zv_mass, *L_geo_zv_mass, *Ks_chi2_geo, *Ks_z_signal;

static TH1F *HT_geo_mass, *HT_geo_zv_mass; 
static TH1F *DiE_geo_mass, *DiE_geo_zv_mass; 
static TH1F *lambda_mass, *lambda_mass_im, *lambda_geo_mass, *mc_lambda_mass;
static TH1F *lambda_chi2_topo_prim_mc;
static TH1F *hlambda_pt, *hlambda_y, *hlambda_p;
static TH1F *lambda_theta, *lambda_phi;
static TH1F *hKs_pt, *hKs_y, *hKs_p;
static TH1F *Ks_theta, *Ks_phi;
static TH1F *lambda_chi2_geo_BG, *lambda_chi2_geo_signal, *lambda_chi2_topo, *lambda_chi2_topo_signal, *lambda_Ksi_chi2_topo_signal,*Ksi_chi2_geo, *Ksi_chi2_topo;
static TH1F *lambda_z, *lambda_z_signal, *lambda_sec_z_signal;
static TH2F *YPt_Ks_reco;
static TH2F *YPt_L_reco;
static bool first_call_StsAnalysis = true;
static TCanvas *c1, *c2;
static TObjArray Hlist(0);
 static int EventCounter = 0;
bool DRAW = 1;
//bool DRAW = 0;
  if (first_call_StsAnalysis){    
	if(DRAW){
    		c1 = new TCanvas("c1","canvas",0,0,800,600);
    		c1->Divide(2,2); 
		gStyle->SetPalette(1);
	}
    
    zv_primary = new TH1F("zv_primary", "zv_primary", 2000,-1,1);
   zv_primary->SetLineColor(4);
   zv_primary->SetFillColor(7);
  (zv_primary->GetXaxis())->SetTitle("z_{pv} (mm)");
  (zv_primary->GetYaxis())->SetTitle("Entries (a.u.)");
     YPt_Ks_reco = new TH2F("YPt_Ks_reco", "YPt_Ks_reco", 60,0.,6., 60, 0., 3.);
     YPt_L_reco = new TH2F("YPt_L_reco", "YPt_L_reco", 60,0.,6., 60, 0., 3.);
   
     NRecoTracks= new TH1F("NRecoTracks", "NRecoTracks", 1200,0,1200);
	lambda_mass = new TH1F("lambda_mass", "lambda_mass", 1000,1.,2.);
	lambda_mass_im = new TH1F("lambda_mass_im", "lambda_mass_im", 1000,1.,2.);
	lambda_geo_mass = new TH1F("lambda_geo_mass", "lambda_geo_mass", 1000,1.,2.);
	Ks_geo_mass = new TH1F("Ks_geo_mass", "Ks_geo_mass", 1000,0.3,1.3);
	mc_Ks_geo_mass = new TH1F("mc_Ks_geo_mass", "mc_Ks_geo_mass", 1000,0.3,1.3);
	Ks_geo_zv_mass = new TH1F("Ks_geo_zv_mass", "Ks_geo_zv_mass", 1000,0.3,1.3);
	L_geo_zv_mass = new TH1F("L_geo_zv_mass", "L_geo_zv_mass", 1000,0.3,1.3);
	mc_lambda_mass = new TH1F("mc_lambda_mass", "mc_lambda_mass", 1000,1.,2.);

	lambda_chi2_topo_prim_mc = new TH1F("lambda_chi2_topo_prim_mc","lambda_chi2_topo_prim_mc", 200,0.,500.);
	lambda_chi2_geo_BG = new TH1F("lambda_chi2_geo_BG", "lambda_chi2_geo_BG", 200,0.,100.);
	lambda_chi2_geo_signal = new TH1F("lambda_chi2_geo_signal", "lambda_chi2_geo_signal", 200,0.,100.);
	lambda_chi2_topo = new TH1F("lambda_chi2_topo", "lambda_chi2_topo", 200,0.,500.);
	lambda_chi2_topo_signal = new TH1F("lambda_chi2_topo_signal", "lambda_chi2_topo_signal", 200,0.,500.);
	lambda_Ksi_chi2_topo_signal = new TH1F("lambda_Ksi_chi2_topo_signal", "lambda_Ksi_chi2_topo_signal", 200,0.,500.);

	hlambda_p = new TH1F("hlambda_p", "hlambda_p", 100,0.,10.);
	hlambda_pt = new TH1F("hlambda_pt", "hlambda_pt", 60,0.,3.);
	hlambda_y = new TH1F("hlambda_y", "hlambda_y", 60,0.,6.);
	lambda_theta = new TH1F("lambda_theta", "lambda_theta", 80,-2.,2.);
	lambda_phi = new TH1F("lambda_phi", "lambda_phi", 80,-2.,2.);

	hKs_p = new TH1F("hKs_p", "hKs_p", 100,0.,10.);
	hKs_pt = new TH1F("hKs_pt", "hKs_pt", 60,0.,3.);
	hKs_y = new TH1F("hKs_y", "hKs_y", 60,0.,6.);
	Ks_theta = new TH1F("Ks_theta", "Ks_theta", 80,-2.,2.);
	Ks_phi = new TH1F("Ks_phi", "Ks_phi", 80,-2.,2.);
	Ks_chi2_geo = new TH1F("Ks_chi2_geo", "Ks_chi2_geo", 200,0.,100.);
	Ks_z_signal = new TH1F("Ks_z_signal", "Ks_z_signal", 300,-5.,55.);

	lambda_z = new TH1F("lambda_z", "lambda_z", 300,-5.,55.);
	lambda_z_signal = new TH1F("lambda_z_signal", "lambda_z_signal", 300,-5.,55.);
	lambda_sec_z_signal = new TH1F("lambda_sec_z_signal", "lambda_sec_z_signal", 300,-5.,55.);
	LambdaMotherPDG = new TH1F("LambdaMotherPDG", "LambdaMotherPDG", 7000, -3500., 3500.);

	Ks_geo_mass->SetLineColor(4);
	Ks_geo_zv_mass->SetLineColor(6);
        TAxis *x2 = Ks_geo_mass->GetXaxis();
	x2->SetRangeUser(0.3,1.3);
	x2->SetTitle("m_{inv} (GeV/c^{2})");	
        TAxis *x1 = Ks_geo_zv_mass->GetXaxis();
	x1->SetRangeUser(0.3,1.3);
	x1->SetTitle("m_{inv} (GeV/c^{2})");	

    Hlist.Add(zv_primary);
    Hlist.Add(NRecoTracks);

    Hlist.Add(lambda_mass);
    Hlist.Add(lambda_mass_im);
    Hlist.Add(lambda_geo_mass);
    Hlist.Add(mc_lambda_mass);
    Hlist.Add(lambda_chi2_topo_prim_mc);
    Hlist.Add(lambda_chi2_geo_BG);
    Hlist.Add(lambda_chi2_geo_signal);
    Hlist.Add(lambda_chi2_topo);
    Hlist.Add(lambda_chi2_topo_signal);
    Hlist.Add(lambda_Ksi_chi2_topo_signal);

    Hlist.Add(hlambda_p);
    Hlist.Add(hlambda_pt);
    Hlist.Add(hlambda_y);
    Hlist.Add(lambda_theta);
    Hlist.Add(lambda_phi);

    Hlist.Add(hKs_p);
    Hlist.Add(hKs_pt);
    Hlist.Add(hKs_y);
    Hlist.Add(Ks_theta);
    Hlist.Add(Ks_phi);
   Hlist.Add(lambda_z);
    Hlist.Add(lambda_z_signal);
    Hlist.Add(lambda_sec_z_signal);
    Hlist.Add(Ks_geo_mass);
    Hlist.Add(mc_Ks_geo_mass);
    Hlist.Add(Ks_geo_zv_mass);
    Hlist.Add(Ks_chi2_geo);
    Hlist.Add(Ks_z_signal);
    Hlist.Add(YPt_Ks_reco);
    Hlist.Add(YPt_L_reco);
    Hlist.Add(L_geo_zv_mass);
    Hlist.Add(LambdaMotherPDG);

     first_call_StsAnalysis = 0;
  }
   
   CbmKFVertex PV( *fPrimVtx );
  if(PV.GetRefNTracks()>=2 && PV.GetRefChi2()/PV.GetRefNDF() <10. && PV.GetRefChi2()/PV.GetRefNDF() >0.)zv_primary->Fill(10*PV.GetRefZ());
// single track cuts:
  double cutChiPrimaryK = 3.0;	//Sigma(s)
  double cutChiPrimaryPion = 4.0;	//Sigma(s)
  double cutChiPrimaryProton = 5.0;	//Sigma(s)
  double cutz_geo_L = 3.0; 
  double cutChi2 = 3.0;	//Sigma(s)
   double cutChi2tL = 6.0; //7.5
   double cutChi2t = 3.0;
 double cutz_geo = 3.0; //cm 
  double cutz_topo = 3.0; 
  double cutIP_V0 = 10.;	//10 cm
  double cutChiPrimary = 3.0;	//Sigma(s)

  double cutP = 0.0;		//GeV
  double cutPtK = 0.0;		//GeV
  double cutPtpi = 0.0;		//GeV
  double cutIP_u = 0.0;	
 double cutIP_d = 10.;	//ip < 1000 mkm	new 2008 geometry!

  double massKS = 0.497671;
  double massL    = 1.115683;
  double massPi   = 0.1395679;
  double massP    = 0.93827231;

  CbmV0TrackInfo* InfoStatic = new CbmV0TrackInfo();
  cout<<"Event #   "<<++EventCounter<<endl;
  
  int NTracks = listStsTracks->GetEntries();
 NRecoTracks->Fill(NTracks);

  CbmV0TrackInfo TArray[NTracks];
  for( int i=0; i<NTracks; i++ ){
    CbmV0TrackInfo &T = TArray[i];
   Set(T, i, EventCounter); //reconstruction
   if ( !T.is_long || T.ChiPrimary < cutChiPrimary || 
	 T.p <= cutP || T.pt <= cutPtK || T.imr >= cutIP_d || T.imr <= cutIP_u) continue;
  }
  //Kshort and Lambda search

Bool_t KsLambda = 1;
//Bool_t KsLambda = 0;
if(KsLambda) {
 for( int it1=0; it1<NTracks; it1++ ) { //pi- 
  CbmV0TrackInfo &T1 = TArray[it1];
    if( T1.q > 0. || T1.ChiPrimary <= cutChiPrimary || T1.p < cutP) continue; //negative tracks
		for( int it2= 0; it2<NTracks; it2++ ){	
        	CbmV0TrackInfo &T2 = TArray[it2];
		if(it1==it2)continue; //if reco q is wrong to avoid pp particle 
	 	if(T2.q < 0. || T2.ChiPrimary <= cutChiPrimary || T2.p < cutP) continue; //positive tracks
		CbmKFTrack tt1(T1.track);
		CbmKFTrack tt2(T2.track);
		vector<CbmKFTrackInterface*> t1;
		t1.push_back(&tt1);
		t1.push_back(&tt2);
		CbmKFParticle Ks;
		Ks.Construct(t1, 0); //Ks has been created
		t1.clear();


		ReFit( T2, 2212 );   //proton hypothesis  -->
		CbmKFTrack proton(T2.track);
		ReFit( T2, 211 );   //back to pion hypothesis  -->
		t1.push_back(&tt1);
		t1.push_back(&proton);
		CbmKFParticle Lambda;//Lambda has been created
		Lambda.Construct(t1, 0);
		t1.clear();

		Ks.TransportToDecayVertex();
		Lambda.TransportToDecayVertex();
		double m1, merr1, ct1, cterr1, l1, lerr1, p1, perr1;
		double m2, merr2, ct2, cterr2, l2, lerr2, p2, perr2;
		Ks.GetMass(m1, merr1);
		Ks.GetMomentum(p1, perr1);
		Lambda.GetMass(m2, merr2);
		Lambda.GetMomentum(p2, perr2);

		double Ks_chi2 = Ks.Chi2/Ks.NDF;
		double L_chi2 = Lambda.Chi2/Lambda.NDF;

		double Ks_e = Ks.r[6];
		double Ks_pt = sqrt(Ks.r[3]*Ks.r[3] + Ks.r[4]*Ks.r[4]);
		double Ks_y = 0.5*log((Ks_e + Ks.r[5])/(Ks_e - Ks.r[5]));
		double Theta_Ks = atan2(Ks_pt,Ks.r[5]);
		double Phi_Ks = atan2(Ks.r[3],Ks.r[5]);

		double Lambda_e = Lambda.r[6];
		double Lambda_pt = sqrt(Lambda.r[3]*Lambda.r[3] + Lambda.r[4]*Lambda.r[4]);
		double Lambda_y = 0.5*log((Lambda_e + Lambda.r[5])/(Lambda_e - Lambda.r[5]));
		double Theta_L = atan2(Lambda_pt,Lambda.r[5]);
		double Phi_L = atan2(Lambda.r[3],Lambda.r[5]);

		if(T1.mc_MotherPDG == 3122 && T2.mc_MotherPDG == 3122&&T1.mc_MotherID==T2.mc_MotherID) { //reco mc lambda
		hlambda_p->Fill(p2);
		hlambda_pt->Fill(Lambda_pt);
		hlambda_y->Fill(Lambda_y);
		if(Lambda.r[4] >= 0.) lambda_theta->Fill(Theta_L);
		if(Lambda.r[4] <  0.) lambda_theta->Fill(-Theta_L);
		lambda_phi->Fill(Phi_L);
		YPt_L_reco->Fill(Lambda_y,Lambda_pt,1); 
		lambda_chi2_geo_signal->Fill(L_chi2);
		}
		
//		if(Ks_chi2 < cutChi2 && fabs(m1-massKS) < 3.0*0.0032) { //reco  K0s
		if(T1.mc_MotherPDG == 310 && T2.mc_MotherPDG == 310&&T1.mc_MotherID==T2.mc_MotherID) { //reco mc K0s
		hKs_p->Fill(p1);
		hKs_pt->Fill(Ks_pt);
		hKs_y->Fill(Ks_y);
		if(Ks.r[4] >= 0.) Ks_theta->Fill(Theta_Ks);
		if(Ks.r[4] <  0.) Ks_theta->Fill(-Theta_Ks);
		Ks_phi->Fill(Phi_Ks);
		YPt_Ks_reco->Fill(Ks_y,Ks_pt,1); 
		mc_Ks_geo_mass->Fill(m1);
		Ks_z_signal->Fill(Ks.r[2] );
		Ks_chi2_geo->Fill(Ks_chi2);
		}

		if(Ks_chi2 < cutChi2) Ks_geo_mass->Fill(m1);
		if(Ks_chi2 < cutChi2 && Ks.r[2] > cutz_geo)Ks_geo_zv_mass->Fill(m1);

		if(L_chi2 < cutChi2) lambda_geo_mass->Fill(m2);
		if(L_chi2 < cutChi2 && Lambda.r[2] > cutz_geo) L_geo_zv_mass->Fill(m2);
		}
	}
}
	if(EventCounter %100==0 && KsLambda == 1){
	c1->cd(1);
	Ks_geo_mass->Draw();
	lambda_geo_mass->Draw("same");
	c1->cd(2);
	Ks_geo_zv_mass->Draw();
	c1->cd(4);
	L_geo_zv_mass->Draw();
	c1->cd(3);
	Ks_z_signal->Draw();
	lambda_z_signal->Draw("same");
	c1->Update();
	}


  TFile fh("pv_histo.root","recreate");
  Hlist.Write();
  fh.Close();
}


Bool_t CbmV0Analysis::IsLong(CbmStsTrack &track){
  return kTRUE;
}
