tof_deb100(Int_t nEvents = 1000, Int_t iout=1)
{

  TString  InputFile     ="data/auaumbias.mc.root";
  TString  ParFile       ="data/auaumbias.params.root";

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();  
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libCbmBase");
  gSystem->Load("libCbmData");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libEcal");
  gSystem->Load("libKF");
  gSystem->Load("libMvd");
  gSystem->Load("libSts");
  gSystem->Load("libRich");
  gSystem->Load("libTrd");
  gSystem->Load("libTof");
  gSystem->Load("libEcal");
  gSystem->Load("libEve");
  gSystem->Load("libEventDisplay");
  gSystem->Load("libCbmDisplay");
  gSystem->Load("libHadron");

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();

//  fRun->SetInputFile(InputFile.Data());
  fRun->SetInputFile("data/auaumbias.mc.root");
  fRun->AddFriend("data/auaumbias.eds.root");

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(ParFile.Data());
  rtdb->setFirstInput(parInput1);

  TFile *f1 = new TFile("$VMCWORKDIR/macro/run/data/auaumbias.mc.root");
  TFile *f2 = new TFile("$VMCWORKDIR/macro/run/data/auaumbias.eds.root");

  TTree *t1 = f1->Get("cbmsim");
  TTree *t2 = f2->Get("cbmsim");
	

  FairMCEventHeader *MCEventHeader  = new FairMCEventHeader();
  TClonesArray *MCTracks   = new TClonesArray("CbmMCTrack");
  TClonesArray *TofPoints  = new TClonesArray("CbmTofPoint");
  TClonesArray *TofHits    = new TClonesArray("CbmTofHit");
  TClonesArray *GlobTracks = new TClonesArray("CbmGlobalTrack");

  t1->SetBranchAddress("MCEventHeader.",&MCEventHeader);
  t1->SetBranchAddress("MCTrack",       &MCTracks);
  t1->SetBranchAddress("TofPoint",      &TofPoints);
  t2->SetBranchAddress("TofHit",        &TofHits);
  t2->SetBranchAddress("GlobalTrack",   &GlobTracks);

  CbmMCTrack     *MCTrack;
  CbmTofPoint    *TofPoint;
  CbmTofHit      *TofHit;
  CbmGlobalTrack *GlobTrack;
  // constants 
  Float_t clight=29.9792;

  // histograms
  fRun->SetOutputFile("data/auaumbias.ana.root");
  TFile *fHist = fRun->GetOutputFile();

  //generator
  TH2F* ptm_rap_gen_kp = new TH2F("ptm_rap_gen_kp","MCTrack-gen k-plus; y; p_{T}/m",30,-1,5,30,0,2.5);
  TH2F* ptm_rap_gen_km = new TH2F("ptm_rap_gen_km","MCTrack-gen k-minus; y; p_{T}/m",30,-1,5,30,0,2.5);
  TH2F* ptm_rap_gen_p  = new TH2F("ptm_rap_gen_p","MCTrack-gen proton; y; p_{T}/m",30,-1,5,30,0,2.5);

  TH1F* drp_gen = new TH1F("drp_gen","[Df]-reaction plane;",100,-180.,180.);
  TH2F* cdrp_b_gen = new TH2F("cdrp_b_gen","reaction plane resolution;impact parameter b; cos[D]_{rp};",15,0.,15.,20,0.,1.);
  TH2F* drp_b_gen = new TH2F("drp_b_gen","[Df]-reaction plane;impact parameter b;",15,0.,15.,36,-180.,180.);
 
  //points
  TH2F* ptm_rap_poi_kp = new TH2F("ptm_rap_poi_kp","MCTrack-poi k-plus; y; p_{T}/m",30,-1,5,30,0,2.5);
  TH2F* ptm_rap_poi_km = new TH2F("ptm_rap_poi_km","MCTrack-poi k-minus; y; p_{T}/m",30,-1,5,30,0,2.5);
  TH2F* ptm_rap_poi_p  = new TH2F("ptm_rap_poi_p","MCTrack-poi proton; y; p_{T}/m",30,-1,5,30,0,2.5);

  TH1F* drp_poi = new TH1F("drp_poi","[Df]-reaction plane (Poi);",100,-180.,180.);
  TH2F* cdrp_b_poi = new TH2F("cdrp_b_poi","Poi reaction plane resolution;impact parameter b; cos[D]_{rp};",15,0.,15.,20,0.,1.);
  TH2F* drp_b_poi = new TH2F("drp_b_poi","Poi-reaction plane;impact parameter b; [Df]",15,0.,15.,36,-180.,180.);

  //hits 
  TH2F* ptm_rap_hit_kp = new TH2F("ptm_rap_hit_kp","MCTrack-hit k-plus; y; p_{T}/m",30,-1,5,30,0,2.5);
  TH2F* ptm_rap_hit_km = new TH2F("ptm_rap_hit_km","MCTrack-hit k-minus; y; p_{T}/m",30,-1,5,30,0,2.5);
  TH2F* ptm_rap_hit_p  = new TH2F("ptm_rap_hit_p","MCTrack-hit proton; y; p_{T}/m",30,-1,5,30,0,2.5);

  TH1F* drp_hit = new TH1F("drp_hit","[Df]-reaction plane (TofHit);",100,-180.,180.);
  TH2F* cdrp_b_hit = new TH2F("cdrp_b_hit","TofHit reaction plane resolution;impact parameter b; cos[D]_{rp};",15,0.,15.,20,0.,1.);
  TH2F* drp_b_hit = new TH2F("drp_b_hit","TofHit-reaction plane;impact parameter b; [Df]",15,0.,15.,36,-180.,180.);

  Int_t nbinx=100;
  Int_t nbiny=100;
  Float_t xrange=800.;
  Float_t yrange=600.;
  Float_t wxy2 = nbinx*nbiny/4./xrange/yrange;

  TH2F* xy_poi1 = new TH2F("xy_poi1","TofPoint; ;",nbinx,-xrange,xrange,nbiny,-yrange,yrange);
  TH2F* xy_poi2 = new TH2F("xy_poi2","TofPoint /cm^{2}; ;",nbinx,-xrange,xrange,nbiny,-yrange,yrange);
  TH2F* xy_poi3 = new TH2F("xy_poi3","TofPoint /cm^{2}/s; ;",nbinx,-xrange,xrange,nbiny,-yrange,yrange);

  TH2F* pv_poi = new TH2F("pv_poi","momentum; velocity;",100,0.,32.,100.,0.,4.);
  TH2F* tm_poi = new TH2F("tm_poi","Tofpoi(all); momentum; Tofmass;",100,0.,10.,100.,0.,2.5);
  TH2F* tm_poiprim = new TH2F("tm_poiprim","Tofpoi(primary points); momentum; Tofmass",100,0.,10.,100.,0.,2.5);
  TH1F* len_poiprim  = new TH1F("len_poiprim","TofPoi; tof (ns); Counts",100,0.,800.);

  //hits
  TH2F* xy_hit1 = new TH2F("xy_hit1","TofHit; ;",nbinx,-xrange,xrange,nbiny,-yrange,yrange);
  TH2F* xy_hit2 = new TH2F("xy_hit2","TofHit /cm^{2}; ;",nbinx,-xrange,xrange,nbiny,-yrange,yrange);
  TH2F* xy_hit3 = new TH2F("xy_hit3","TofHit /cm^{2}/s; ;",nbinx,-xrange,xrange,nbiny,-yrange,yrange);

  TH2F* dxx    = new TH2F("dxx","TofHit; x; Delta x;",100,-xrange,xrange,50.,-10.,10.);
  TH2F* dyy    = new TH2F("dyy","TofHit; y; Delta y;",100,-yrange,yrange,50.,-10.,10.);
  TH2F* dzx    = new TH2F("dzx","TofHit; z; Delta x;",100,-xrange,xrange,50,-20.,20.);
  TH2F* dzy    = new TH2F("dzy","TofHit; z; Delta y;",100,-yrange,yrange,50,-20.,20.);
  TH2F* dzz    = new TH2F("dzz","TofHit; z; Delta z;",100,400.,650.,50,-20.,20.);

  TH2F* pv_hit   = new TH2F("pv_hit","TofHit; velocity; momentum; ",100,0.,32.,100.,0.,4.);
  TH1F* tof_hit  = new TH1F("tof_hit","TofHit; tof (ns)",100,0.,200.);
  TH1F* dt_hit   = new TH1F("dt_hit","TofHit; delta-tof (ns)",100,-2.,2.);
  TH2F* dt_p_hit = new TH2F("dt_p_hit","TofHit; momentum; delta-tof (ns)",100,0.,10.,50,-0.5,1.5);
  TH2F* tm_hit  = new TH2F("tm_hit","TofHit; momentum; mass;",100,0.,10.,100.,0.,2.5);
  TH2F* tm_hit1 = new TH2F("tm_hit1","TofHit(flg=1); momentum; mass;",100,0.,10.,100.,0.,2.5);
  TH2F* tm_hit2 = new TH2F("tm_hit2","TofHit(flg=2); momentum; mass;",100,0.,10.,100.,0.,2.5);
  TH2F* tm_hitp = new TH2F("tm_hitp","TofHit(protons); momentum; mass;",100,0.,10.,100.,0.,2.5);
  TH2F* tm_hitprim   = new TH2F("tm_hitprim","TofHit(primaries); momentum; mass;",100,0.,10.,100.,0.,2.5);
  TH2F* tm_hitp_prim = new TH2F("tm_hitp_prim","TofHit(prim. protons); momentum; mass;",100,0.,10.,100.,0.,2.5);

//global tracks
  TH2F* pv_glo = new TH2F("pv_glo","Global tracks; velocity; momentum;",100,0.,32.,100.,0.,4.);
  TH2F* tm_glo = new TH2F("tm_glo","Global tracks; momentum; mass;",100,0.,10.,100.,0.,2.5);

  TH2F* pv_gloprim = new TH2F("pv_gloprim","Global primary tracks; velocity(cm/ns); momentum(GeV/c);",100,0.,32.,100.,0.,4.);
  TH2F* tm_gloprim = new TH2F("tm_gloprim","Global primary tracks; momentum (GeV/c); mass;",100,0.,10.,100.,0.,2.5);
  TH2F* tm_glop_prim = new TH2F("tm_glop_prim","Global primary proton tracks; momentum (GeV/c); mass;",100,0.,10.,100.,0.,2.5);

  TH2F* ptm_rap_glo_kp = new TH2F("ptm_rap_glo_kp","MCTrack-glo k-plus; y; p_{T}/m",30,-1,5,30,0,2.5);
  TH2F* ptm_rap_glo_km = new TH2F("ptm_rap_glo_km","MCTrack-glo k-minus; y; p_{T}/m",30,-1,5,30,0,2.5);
  TH2F* ptm_rap_glo_p  = new TH2F("ptm_rap_glo_p","MCTrack-glo proton; y; p_{T}/m",30,-1,5,30,0,2.5);
 
  TH1F* drp_glo = new TH1F("drp_glo","[Df]-reaction plane (GloHit);",100,-180.,180.);
  TH2F* cdrp_b_glo = new TH2F("cdrp_b_glo","GloHit reaction plane resolution;impact parameter b; cos[D]_{rp};",15,0.,15.,20,0.,1.);
  TH2F* drp_b_glo = new TH2F("drp_b_glo","Glo-reaction plane;impact parameter b; [Df]",15,0.,15.,36,-180.,180.);
// var
	
  Int_t nMCTracks, nTofPoints, nTofHits, nGlobTracks;
  Float_t x_point, y_point, z_point, tof_point, SMtype_point, mod_point, cel_point, gap_point;  
  Float_t x_poi, y_poi, z_poi, tof_poi; 
  Float_t SMtype_poi, mod_poi, cel_poi, gap_poi;
  Float_t p_MC, px_MC, py_MC, pz_MC;
  Float_t x_hit, y_hit, z_hit, t_hit, dy_hit;
  Float_t delta_tof;

  TVector3 p3_MC;
  Int_t TrackP[10000];
  Float_t TofOff=0.12;

  Float_t Qx1, Qy1, Qx2, Qy2, phirp1, phirp2, delrp, rp_weight;
  Float_t yrp_mid=1.3;  // midrapidity -> update from simulation!	
  Int_t count, pid, Np1, Np2;
  Float_t beta;
  Int_t nevent = t1->GetEntries();	
  Int_t nnevent = 5;
  Float_t rxy2 = wxy2*1.E7/nevent; // to scale rates for 10MHz interaction rates

  if (nevent > nEvents) nevent=nEvents;

  cout << "total number of events to process: " << nevent <<endl;

// Event loop
	
for (Int_t iev=0; iev< nevent; iev++) {
// for (Int_t iev=0; iev< nnevent; iev++) {
// get entry	
    t1->GetEntry(iev);
    t2->GetEntry(iev);
		
    nMCTracks = MCTracks->GetEntriesFast();
    nTofPoints = TofPoints->GetEntriesFast();
    nTofHits = TofHits->GetEntriesFast();
    nGlobTracks = GlobTracks->GetEntriesFast();
    		
    cout << " Event"  << iev << ":";
    cout << nMCTracks << " MC tracks ";
    cout << nTofPoints << " Tof points ";
    cout << nTofHits << " Tof Hits ";
    cout << nGlobTracks << " Global tracks " <<endl;

 }
 cout << "Processing done, outflag =" <<iout << endl;
 if (iout==1){
 fHist->Write();
 if(0){ // explicit writing 
  TIter next(gDirectory->GetList());
  TH1 *h;
  TObject* obj;
  while(obj= (TObject*)next()){
      if(obj->InheritsFrom(TH1::Class())){
         h = (TH1*)obj;
         cout << "Write histo " << h->GetTitle() << endl;
         h->Write();
      }
  }
 }
 fHist->ls();
 fHist->Close();
 }
}
