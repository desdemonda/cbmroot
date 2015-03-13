tof_ana100(Int_t nEvents = 1000, Int_t iout=1)
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

  TFile *f1 = new TFile("$VMCWORKDIR/macro/tof/data/auaumbias.mc.root");
  TFile *f2 = new TFile("$VMCWORKDIR/macro/tof/data/auaumbias.eds.root");

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

  //histograms 
  fRun->SetOutputFile("data/auaumbias.ana.root");
  TFile *fHist = fRun->GetOutputFile();

  //generator
  TH2F* ptm_rap_gen_kp = new TH2F("ptm_rap_gen_kp","MCTrack-gen k-plus; y; p_{T}/m",30,-1,5,30,0,2.5);
  TH2F* ptm_rap_gen_km = new TH2F("ptm_rap_gen_km","MCTrack-gen k-minus; y; p_{T}/m",30,-1,5,30,0,2.5);
  TH2F* ptm_rap_gen_p  = new TH2F("ptm_rap_gen_p","MCTrack-gen proton; y; p_{T}/m",30,-1,5,30,0,2.5);

  TH1F* drp_gen = new TH1F("drp_gen","#Delta#phi-reaction plane;",100,-180.,180.);
  TH2F* cdrp_b_gen = new TH2F("cdrp_b_gen","reaction plane resolution;impact parameter b; cos#Delta#phi_{rp};",15,0.,15.,20,0.,1.);
  TH2F* drp_b_gen = new TH2F("drp_b_gen","#Delta#phi-reaction plane;impact parameter b;",15,0.,15.,36,-180.,180.);
 
  //points
  TH2F* ptm_rap_poi_kp = new TH2F("ptm_rap_poi_kp","MCTrack-poi k-plus; y; p_{T}/m",30,-1,5,30,0,2.5);
  TH2F* ptm_rap_poi_km = new TH2F("ptm_rap_poi_km","MCTrack-poi k-minus; y; p_{T}/m",30,-1,5,30,0,2.5);
  TH2F* ptm_rap_poi_p  = new TH2F("ptm_rap_poi_p","MCTrack-poi proton; y; p_{T}/m",30,-1,5,30,0,2.5);

  TH1F* drp_poi = new TH1F("drp_poi","#Delta#phi-reaction plane (Poi);",100,-180.,180.);
  TH2F* cdrp_b_poi = new TH2F("cdrp_b_poi","Poi reaction plane resolution;impact parameter b; cos#Delta#phi_{rp};",15,0.,15.,20,0.,1.);
  TH2F* drp_b_poi = new TH2F("drp_b_poi","Poi-reaction plane;impact parameter b; #Delta#phi;",15,0.,15.,36,-180.,180.);

  //hits 
  TH2F* ptm_rap_hit_kp = new TH2F("ptm_rap_hit_kp","MCTrack-hit k-plus; y; p_{T}/m",30,-1,5,30,0,2.5);
  TH2F* ptm_rap_hit_km = new TH2F("ptm_rap_hit_km","MCTrack-hit k-minus; y; p_{T}/m",30,-1,5,30,0,2.5);
  TH2F* ptm_rap_hit_p  = new TH2F("ptm_rap_hit_p","MCTrack-hit proton; y; p_{T}/m",30,-1,5,30,0,2.5);

  TH1F* drp_hit = new TH1F("drp_hit","#Delta#phi-reaction plane (TofHit);",100,-180.,180.);
  TH2F* cdrp_b_hit = new TH2F("cdrp_b_hit","TofHit reaction plane resolution;impact parameter b; cos#Delta#phi_{rp};",15,0.,15.,20,0.,1.);
  TH2F* drp_b_hit = new TH2F("drp_b_hit","TofHit-reaction plane;impact parameter b; #Delta#phi;",15,0.,15.,36,-180.,180.);

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
 
  TH1F* drp_glo = new TH1F("drp_glo","#Delta#phi-reaction plane (GloHit);",100,-180.,180.);
  TH2F* cdrp_b_glo = new TH2F("cdrp_b_glo","GloHit reaction plane resolution;impact parameter b; cos #Delta#phi_{rp};",15,0.,15.,20,0.,1.);
  TH2F* drp_b_glo = new TH2F("drp_b_glo","Glo-reaction plane;impact parameter b; #Delta#phi; ",15,0.,15.,36,-180.,180.);
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
  Int_t   pid, Np1, Np2;
  Float_t beta;
  Int_t nevent = t1->GetEntries();	

  if (nevent > nEvents) nevent=nEvents;
  Float_t rxy2 = wxy2*1.E7/nevent; // to scale rates for 10MHz interaction rates

  cout << "total number of events to process: " << nevent 
       << ", wxy2=" << wxy2 <<", rxy2="<< rxy2 <<endl;

// Event loop
	
for (Int_t iev=0; iev< nevent; iev++) {
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
    
// MCTrack loop		
    Qx1=0.; Qy1=0.; Np1=0; Qx2=0.; Qy2=0.; Np2=0; 
    for (Int_t k=0; k< nMCTracks; k++) { // inspect tracks 
			
	MCTrack = (CbmMCTrack*) MCTracks->At(k);			
	pid = MCTrack->GetPdgCode();			
	if (MCTrack->GetMotherId()!=-1) continue;    // primary particles
//	if (MCTrack->GetNPoints(kTOF)>0) continue;
	if (pid==321) {
	  ptm_rap_gen_kp->Fill(MCTrack->GetRapidity(),MCTrack->GetPt()/MCTrack->GetMass());
	}		    
	if (pid==-321) {
	  ptm_rap_gen_km->Fill(MCTrack->GetRapidity(),MCTrack->GetPt()/MCTrack->GetMass());
	}
	//	{                       // all particles 
       	if (pid==2212) {                       // protons
	  ptm_rap_gen_p->Fill(MCTrack->GetRapidity(),MCTrack->GetPt()/MCTrack->GetMass());
          if (MCTrack->GetRapidity()>yrp_mid){ // set weights for reaction plane 
	    rp_weight=1.;}
          else {
	    rp_weight = -1.;
          }
	  if(gRandom->Uniform(1)>0.5)  { //subdivide events into 2 random subevents
	    Np1++;
            Qx1=Qx1+rp_weight*MCTrack->GetPx();
            Qy1=Qy1+rp_weight*MCTrack->GetPy();
	  }else{
	    Np2++;
            Qx2=Qx2+rp_weight*MCTrack->GetPx();
            Qy2=Qy2+rp_weight*MCTrack->GetPy();	   
	  }
	  
	}	
    }
    if (Np1>0 && Np2>0){
      phirp1=atan2(Qx1,Qy1);
      phirp2=atan2(Qx2,Qy2);
      Float_t RADDEG=57.29577951;
      delrp=(phirp1-phirp2);
      cdrp_b_gen->Fill(MCEventHeader->GetB(),TMath::Cos(TMath::Abs(delrp)));
      delrp=delrp*RADDEG;
      if(delrp<-180.) delrp+=360.;
      if(delrp>180.)  delrp-=360.;
      //cout << "RP: N1 "<< Np1 <<" N2 "<<Np2<<" delrp "<<delrp<< endl;
      drp_gen->Fill(delrp);
      drp_b_gen->Fill(MCEventHeader->GetB(),delrp);
    }

// TofPoint loop					
    for (Int_t k=0; k< nMCTracks; k++) { // reset track detected flag
      TrackP[k]=0;
    }
    Qx1=0.; Qy1=0.; Np1=0; Qx2=0.; Qy2=0.; Np2=0; 
    for (Int_t l =0; l<nTofPoints; l++) {
      TofPoint = (CbmTofPoint*) TofPoints->At(l);
      Int_t k = TofPoint->GetTrackID();
      MCTrack = (CbmMCTrack*) MCTracks->At(k);
      pid = MCTrack->GetPdgCode();	
      px_MC = MCTrack->GetPx();
      py_MC = MCTrack->GetPy();
      pz_MC = MCTrack->GetPz();
      p_MC  = sqrt(px_MC*px_MC+py_MC*py_MC+pz_MC*pz_MC);
	
      if (TrackP[k]==0) { // for efficiency
        TrackP[k]=1;
	if (pid==321) {
	  ptm_rap_poi_kp->Fill(MCTrack->GetRapidity(),MCTrack->GetPt()/MCTrack->GetMass());
	}		    
	if (pid==-321) {
	  ptm_rap_poi_km->Fill(MCTrack->GetRapidity(),MCTrack->GetPt()/MCTrack->GetMass());
	}
	//	{                       // all particles 
       	if (pid==2212) {                       // protons
	  ptm_rap_poi_p->Fill(MCTrack->GetRapidity(),MCTrack->GetPt()/MCTrack->GetMass());
          if (MCTrack->GetRapidity()>yrp_mid){ // set weights for reaction plane 
	    rp_weight=1.;}
          else {
	    rp_weight = -1.;
          }
	  if(gRandom->Uniform(1)>0.5)  { //subdivide events into 2 random subevents
	    Np1++;
            Qx1=Qx1+rp_weight*MCTrack->GetPx();
            Qy1=Qy1+rp_weight*MCTrack->GetPy();
	  }else{
	    Np2++;
            Qx2=Qx2+rp_weight*MCTrack->GetPx();
            Qy2=Qy2+rp_weight*MCTrack->GetPy();	   
	  }
	}
	x_poi	   = TofPoint -> GetX();
	y_poi	   = TofPoint -> GetY();
	z_poi	   = TofPoint -> GetZ();
	tof_poi	   = TofPoint -> GetTime();
	SMtype_poi = TofPoint -> GetSMtype();
	mod_poi	   = TofPoint -> GetModule();
	cel_poi	   = TofPoint -> GetCell();
	gap_poi	   = TofPoint -> GetGap();

        xy_poi1	->Fill(x_poi,y_poi);
        xy_poi2	->Fill(x_poi,y_poi,wxy2);
        xy_poi3	->Fill(x_poi,y_poi,rxy2);	
	
        Float_t vel = (TofPoint->GetLength())/tof_poi;
        Float_t bet = vel / clight;  
        if (bet > 0.99999) {bet=0.99999;} 
        Float_t tofmass =p_MC/bet*sqrt(1.-bet**2); 
        pv_poi->Fill(vel,p_MC);
        tm_poi->Fill(p_MC,tofmass);
        if (MCTrack->GetMotherId()==-1){
         tm_poiprim->Fill(p_MC,tofmass);
         len_poiprim->Fill(TofPoint->GetLength());
         if(tofmass > 0.98){
           TVector3 vmom;
           TofPoint -> Momentum(vmom);
	   Float_t p_poi  = sqrt(vmom.X()*vmom.X()+vmom.Y()*vmom.Y()+vmom.Z()*vmom.Z());
	   cout << "Bad primary TofPoint from MCtrack " << k << " pid " << pid 
                << ", M " << tofmass << ", DetID " << TofPoint->GetDetectorID() 
		<< ", SM "<< SMtype_poi <<","<<mod_poi <<","<< cel_poi <<","<< gap_poi
                << ",p: "<< p_MC <<"-"<< p_poi 
                <<endl;
           //TofPoint->Print();
	 }
	}
	//x_poi1->Fill(x_poi);	
	//y_poi1->Fill(y_poi);
	//z_poi1->Fill(z_poi);
	//tof_poi1->Fill(tof_poi);
	// nreg->Fill(SMtype_poi);
	//nmod->Fill(mod_poi);
	//ncel->Fill(cel_poi);
	//ngap->Fill(gap_poi);			
	//mod_reg->Fill(TofPoint->GetModule(),TofPoint->GetSMtype());
	//detID->Fill(TofPoint->GetDetectorID());
	//trkID->Fill(TofPoint->GetTrackID());
	//length->Fill(TofPoint->GetLength());      
      } // condition of first point of track end 
    } // Tof Point loop end 
    if (Np1>0 && Np2>0){
      phirp1=atan2(Qx1,Qy1);
      phirp2=atan2(Qx2,Qy2);
      Float_t RADDEG=57.29577951;
      delrp=(phirp1-phirp2);
      cdrp_b_poi->Fill(MCEventHeader->GetB(),TMath::Cos(TMath::Abs(delrp)));
      delrp=delrp*RADDEG;
      if(delrp<-180.) delrp+=360.;
      if(delrp>180.)  delrp-=360.;
      //cout << "RP: N1 "<< Np1 <<" N2 "<<Np2<<" delrp "<<delrp<< endl;
      drp_poi->Fill(delrp);
      drp_b_poi->Fill(MCEventHeader->GetB(),delrp);
    }


// TofHit loop					
    for (Int_t k=0; k< nMCTracks; k++) { // reset track detected flag
      TrackP[k]=0;
    }
    Qx1=0.; Qy1=0.; Np1=0; Qx2=0.; Qy2=0.; Np2=0; 
    for (Int_t j =0; j<nTofHits; j++) {
      TofHit   = (CbmTofHit*) TofHits->At(j);
      Int_t l = TofHit->GetRefId();
      TofPoint = (CbmTofPoint*) TofPoints->At(l);
      Int_t k = TofPoint->GetTrackID();
      MCTrack = (CbmMCTrack*) MCTracks->At(k);			
      pid = MCTrack->GetPdgCode();			

      px_MC = MCTrack->GetPx();
      py_MC = MCTrack->GetPy();
      pz_MC = MCTrack->GetPz();
      p_MC  = sqrt(px_MC*px_MC+py_MC*py_MC+pz_MC*pz_MC);
							
      //px->Fill(px_MC);
      //py->Fill(py_MC);
      //pz->Fill(pz_MC);
      //p1->Fill(p_MC);
      //pt->Fill(MCTrack->GetPt());
      //rap->Fill(MCTrack->GetRapidity());
      //mass->Fill(MCTrack->GetMass());
      //energy->Fill(MCTrack->GetEnergy());
				
      //pdgcode->Fill(MCTrack->GetPdgCode());
      //motherid->Fill(MCTrack->GetMotherId());  //  -1 for primary particles.
      //npoints->Fill(MCTrack->GetNPoints(kTOF));    // detector id 
					
      if (TrackP[k]==0) { // for efficiency
        TrackP[k]=1;
	if (pid==321) {
	  ptm_rap_hit_kp->Fill(MCTrack->GetRapidity(),MCTrack->GetPt()/MCTrack->GetMass());
	}		    
	if (pid==-321) {
	  ptm_rap_hit_km->Fill(MCTrack->GetRapidity(),MCTrack->GetPt()/MCTrack->GetMass());
	}	
	if (pid==211) {
	  // if (pid==211 || pid==-211) {   //pions 
	  //mass_pion->Fill(MCTrack->GetMass());
	  //pt_rap_pion->Fill(MCTrack->GetRapidity(),MCTrack->GetPt());
	  //ptm_rap_pion->Fill(MCTrack->GetRapidity(),MCTrack->GetPt()/MCTrack->GetMass());
	}
	if (pid==2212) {  //protons 
	  ptm_rap_hit_p->Fill(MCTrack->GetRapidity(),MCTrack->GetPt()/MCTrack->GetMass());
	  //p_rap->Fill(MCTrack->GetRapidity(),MCTrack->GetP());
	  //pt_rap->Fill(MCTrack->GetRapidity(),MCTrack->GetPt());
          if (MCTrack->GetRapidity()>yrp_mid){ // set weights for reaction plane 
	    rp_weight=1.;}
          else {
	    rp_weight = -1.;
          }
	  if(gRandom->Uniform(1)>0.5)  { //subdivide events into 2 random subevents
	    Np1++;
            Qx1=Qx1+rp_weight*MCTrack->GetPx();
            Qy1=Qy1+rp_weight*MCTrack->GetPy();
	  }else{
	    Np2++;
            Qx2=Qx2+rp_weight*MCTrack->GetPx();
            Qy2=Qy2+rp_weight*MCTrack->GetPy();	   
	  }
	}
	//mass_p->Fill(MCTrack->GetP(),MCTrack->GetMass());
	//p_bg->Fill(MCTrack->GetP()/MCTrack->GetMass(),MCTrack->GetP());
	if (pid==321 || pid==2212 || pid==211) {
	  //p_bg->Fill(TofPoint->GetLength()/TofPoint->GetTime(),MCTrack->GetP());
	}
      }

      x_poi	= TofPoint -> GetX();
      y_poi	= TofPoint -> GetY();
      z_poi	= TofPoint -> GetZ();
      tof_poi	= TofPoint -> GetTime();

// TofHit					
	
      t_hit	= TofHit->GetTime()+TofOff;
      x_hit	= TofHit->GetX();
      y_hit	= TofHit->GetY();
      z_hit	= TofHit->GetZ();
      dy_hit	= TofHit->GetDy();
      Int_t flg_hit = TofHit->GetFlag();

      delta_tof	= tof_poi - t_hit;
      Float_t delta_x	= x_poi - x_hit;
      Float_t delta_y	= y_poi - y_hit;
      Float_t delta_z	= z_poi - z_hit;
      beta = (TofPoint->GetLength())/t_hit;
      Float_t bet = beta / clight;  
      if (bet > 0.99999) {bet=0.99999;} 
      Float_t tofmass =p_MC/bet*sqrt(1.-bet**2); 
			
      dxx ->Fill(x_poi,delta_x);
      dyy ->Fill(y_poi,delta_y);
      dzx ->Fill(x_poi,delta_z);
      dzy ->Fill(y_poi,delta_z);
      dzz ->Fill(z_poi,delta_z);
		
      xy_hit1	->Fill(x_hit,y_hit);
      xy_hit2	->Fill(x_hit,y_hit,wxy2);
      xy_hit3	->Fill(x_hit,y_hit,rxy2);

      tof_hit->Fill(t_hit);
      dt_hit->Fill(delta_tof);
      dt_p_hit->Fill(p_MC,delta_tof);
      pv_hit->Fill(beta,p_MC);
      tm_hit->Fill(p_MC,tofmass);

      if (flg_hit==1) {
       tm_hit1->Fill(p_MC,tofmass);
      }elseif (flg_hit==2) {
       tm_hit2->Fill(p_MC,tofmass);
      }

      if (MCTrack->GetMotherId()==-1){
       tm_hitprim->Fill(p_MC,tofmass);
      }

      if (pid==2212) {  //protons 
       tm_hitp->Fill(p_MC,tofmass);
       if (MCTrack->GetMotherId()!=-1) continue;
       tm_hitp_prim->Fill(p_MC,tofmass);
      }
      //dy_hit1	->Fill(dy_hit);
      //delta_tof1->Fill(delta_tof);
      //delta_y1	->Fill(delta_y);					
      //beta1	->Fill(beta);
      //tof_hit1	->Fill(tof_hit);
      //length1	->Fill(TofPoint->GetLength());	
      if (pid==321 || pid==2212 || pid==211) {
	//        p_bg_hit->Fill(beta,p_MC);
      }
      
    } // Tof Hit loop end
    if (Np1>0 && Np2>0){
      phirp1=atan2(Qx1,Qy1);
      phirp2=atan2(Qx2,Qy2);
      Float_t RADDEG=57.29577951;
      delrp=(phirp1-phirp2);
      cdrp_b_hit->Fill(MCEventHeader->GetB(),TMath::Cos(TMath::Abs(delrp)));
      delrp=delrp*RADDEG;
      if(delrp<-180.) delrp+=360.;
      if(delrp>180.)  delrp-=360.;
      //cout << "RP: N1 "<< Np1 <<" N2 "<<Np2<<" delrp "<<delrp<< endl;
      drp_hit->Fill(delrp);
      drp_b_hit->Fill(MCEventHeader->GetB(),delrp);
    }

// Global Track loop
    Int_t NGTofTrack=0;
    Qx1=0.; Qy1=0.; Np1=0; Qx2=0.; Qy2=0.; Np2=0; 
    for (Int_t i=0; i<nGlobTracks; i++) {    // inspect global tracks 
      GlobTrack = (CbmGlobalTrack*) GlobTracks->At(i);
      Int_t j = GlobTrack->GetTofHitIndex();
      if (j>0) {
       NGTofTrack++;
       TofHit   = (CbmTofHit*) TofHits->At(j);
       Int_t l = TofHit->GetRefId();
       TofPoint = (CbmTofPoint*) TofPoints->At(l);
       Int_t k = TofPoint->GetTrackID();
       MCTrack = (CbmMCTrack*) MCTracks->At(k);			
       pid = MCTrack->GetPdgCode();
       px_MC = MCTrack->GetPx();
       py_MC = MCTrack->GetPy();
       pz_MC = MCTrack->GetPz();
       p_MC  = sqrt(px_MC*px_MC+py_MC*py_MC+pz_MC*pz_MC);

       Double_t len = GlobTrack->GetLength();
       FairTrackParam *tpar = GlobTrack->GetParamFirst();
       //       FairTrackParam *tpar = GlobTrack->GetParamLast();
       if (tpar->GetQp()==0.) {
	 cout << "Invalid momentum for global track "<< i << " Hit " << j <<endl;
         continue;
       }
       Double_t mom = 1./tpar->GetQp();
       if(mom<0.) mom=-mom;
       Float_t vel=len/(TofHit->GetTime()+TofOff);
       Float_t bet = vel / clight;  
       if (bet > 0.99999) {bet=0.99999;} 
       Float_t tofmass =mom/bet*sqrt(1.-bet**2);
       pv_glo->Fill(vel,mom);
       tm_glo->Fill(mom,tofmass);
       if (MCTrack->GetMotherId()==-1) { // select primaries 
        pv_gloprim->Fill(vel,mom);
        tm_gloprim->Fill(mom,tofmass);

	if (pid==321) {
	  ptm_rap_glo_kp->Fill(MCTrack->GetRapidity(),MCTrack->GetPt()/MCTrack->GetMass());
	}		    
	if (pid==-321) {
	  ptm_rap_glo_km->Fill(MCTrack->GetRapidity(),MCTrack->GetPt()/MCTrack->GetMass());
	}	
        if (pid==2212) {  //protons
         tm_glop_prim->Fill(mom,tofmass);
         ptm_rap_glo_p->Fill(MCTrack->GetRapidity(),MCTrack->GetPt()/MCTrack->GetMass());

          if (MCTrack->GetRapidity()>yrp_mid){ // set weights for reaction plane 
	    rp_weight=1.;}
          else {
	    rp_weight = -1.;
          }
	  if(gRandom->Uniform(1)>0.5)  { //subdivide events into 2 random subevents
	    Np1++;
            Qx1=Qx1+rp_weight*MCTrack->GetPx();
            Qy1=Qy1+rp_weight*MCTrack->GetPy();
	  }else{
	    Np2++;
            Qx2=Qx2+rp_weight*MCTrack->GetPx();
            Qy2=Qy2+rp_weight*MCTrack->GetPy();	   
	  }

	} 
       }
       //       cout << "GTofTrack "<< NGTofTrack << " Hit " << j << " len  " << len << " mom "<< mom <<","<<p_MC<<endl;
       //       GlobTrack->Print();
      } // primary condition end 
    } // global track loop end 
    if (Np1>0 && Np2>0){
      phirp1=atan2(Qx1,Qy1);
      phirp2=atan2(Qx2,Qy2);
      Float_t RADDEG=57.29577951;
      delrp=(phirp1-phirp2);
      cdrp_b_glo->Fill(MCEventHeader->GetB(),TMath::Cos(TMath::Abs(delrp)));
      delrp=delrp*RADDEG;
      if(delrp<-180.) delrp+=360.;
      if(delrp>180.)  delrp-=360.;
      //cout << "RP: N1 "<< Np1 <<" N2 "<<Np2<<" delrp "<<delrp<< endl;
      drp_glo->Fill(delrp);
      drp_b_glo->Fill(MCEventHeader->GetB(),delrp);
    }

 } // event loop end 
// save histos to file
// TFile *fHist = new TFile("data/auaumbias.hst.root","RECREATE");
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
