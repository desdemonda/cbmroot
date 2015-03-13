anaTof100()
{

  TString  InputFile     ="data/auaumbias.root";
  TString  ParFile       ="data/testparams.root";

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

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();

//  fRun->SetInputFile(InputFile.Data());
  fRun->SetInputFile("data/auaumbias.root");
  fRun->AddFriend("data/auaumbiasHits.root");
  fRun->SetOutputFile("data/test.root");

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(ParFile.Data());
  rtdb->setFirstInput(parInput1);

  TFile *f1 = new TFile("$VMCWORKDIR/macro/run/data/auaumbias.root");
  TFile *f2 = new TFile("$VMCWORKDIR/macro/run/data/auaumbiasHits.root");

  TTree *t1 = f1->Get("cbmsim");
  TTree *t2 = f2->Get("cbmsim");
	
  TClonesArray *MCTracks  = new TClonesArray("CbmMCTrack");
  TClonesArray *TofPoints = new TClonesArray("CbmTofPoint");
  TClonesArray *TofHits   = new TClonesArray("CbmTofHit");
	
  t1->SetBranchAddress("MCTrack",&MCTracks);
  t1->SetBranchAddress("TofPoint",&TofPoints);
  t2->SetBranchAddress("TofHit",&TofHits);
	
  CbmMCTrack  *MCTrack;
  CbmTofPoint *TofPoint;
  CbmTofPoint *TofPoint1;
  CbmTofHit   *TofHit;

  //points
  TH2F* ptm_rap_gen_kp = new TH2F("ptm_rap_gen_kp","MCTrack-gen k-plus; y; p_{T}/m",30,-1,5,30,0,2.5);
  TH2F* ptm_rap_gen_km = new TH2F("ptm_rap_gen_km","MCTrack-gen k-minus; y; p_{T}/m",30,-1,5,30,0,2.5);

  TH2F* ptm_rap_poi_kp = new TH2F("ptm_rap_poi_kp","MCTrack-poi k-plus; y; p_{T}/m",30,-1,5,30,0,2.5);
  TH2F* ptm_rap_poi_km = new TH2F("ptm_rap_poi_km","MCTrack-poi k-minus; y; p_{T}/m",30,-1,5,30,0,2.5);

  TH2F* ptm_rap_hit_kp = new TH2F("ptm_rap_hit_kp","MCTrack-hit k-plus; y; p_{T}/m",30,-1,5,30,0,2.5);
  TH2F* ptm_rap_hit_km = new TH2F("ptm_rap_hit_km","MCTrack-hit k-minus; y; p_{T}/m",30,-1,5,30,0,2.5);

  //hits
  Int_t nbinx=100;
  Int_t nbiny=100;
  Float_t xrange=800.;
  Float_t yrange=600.;
  Float_t wxy2 = nbinx*nbiny/4./xrange/yrange;

  TH2F* xy_poi1 = new TH2F("xy_poi1","TofPoint1; ;",nbinx,-xrange,xrange,nbiny,-yrange,yrange);
  TH2F* xy_poi2 = new TH2F("xy_poi2","TofPoint1 /cm^{2}; ;",nbinx,-xrange,xrange,nbiny,-yrange,yrange);
  TH2F* xy_poi3 = new TH2F("xy_poi3","TofPoint1 /cm^{2}/s; ;",nbinx,-xrange,xrange,nbiny,-yrange,yrange);
  
  TH2F* xy_hit1 = new TH2F("xy_hit1","TofHit; ;",nbinx,-xrange,xrange,nbiny,-yrange,yrange);
  TH2F* xy_hit2 = new TH2F("xy_hit2","TofHit /cm^{2}; ;",nbinx,-xrange,xrange,nbiny,-yrange,yrange);
  TH2F* xy_hit3 = new TH2F("xy_hit3","TofHit /cm^{2}/s; ;",nbinx,-xrange,xrange,nbiny,-yrange,yrange);

  TH2F* dxx    = new TH2F("dxx","Delta x; x;",100,-xrange,xrange,50.,-10.,10.);
  TH2F* dyy    = new TH2F("dyy","Delta y; y;",100,-yrange,yrange,50.,-10.,10.);
  TH2F* dzx    = new TH2F("dzx","Delta z; x;",100,-xrange,xrange,50,-20.,20.);
  TH2F* dzy    = new TH2F("dzy","Delta z; y;",100,-yrange,yrange,50,-20.,20.);
  TH2F* dzz    = new TH2F("dzz","Delta z; z;",100,400.,650.,50,-20.,20.);
  TH2F* pv_hit = new TH2F("pv_hit","momentum; velocity;",100,0.,32.,100.,0.,4.);

  TH1F* tof_hit = new TH1F("tof_hit","TofHit;",100,0.,200.);

  TH2F* tm_hit  = new TH2F("tm_hit","Tofmass; momentum;",100,0.,10.,100.,0.,2.5);
  TH2F* tm_hit1 = new TH2F("tm_hit1","Tofmass(flg=1); momentum;",100,0.,10.,100.,0.,2.5);
  TH2F* tm_hit2 = new TH2F("tm_hit2","Tofmass(flg=2); momentum;",100,0.,10.,100.,0.,2.5);

// var
	
  Int_t nMCTracks, nTofPoints, nTofHits;
  Float_t x_point, y_point, z_point, tof_point, SMtype_point, mod_point, cel_point, gap_point;  
  Float_t x_poi, y_poi, z_poi, tof_poi; 
  Float_t SMtype_poi, mod_poi, cel_poi, gap_poi;
  Float_t p_MC, px_MC, py_MC, pz_MC;
  Float_t x_hit, y_hit, z_hit, t_hit, dy_hit;
  Float_t delta_tof;

  TVector3 p3_MC;
  Int_t TrackP[10000];
	
  Int_t count, pid;
  Float_t beta;
  Int_t nevent = t1->GetEntries();	
  Int_t nnevent = 5;
  Float_t rxy2 = wxy2*1.E7/nevent; // to scale rates for 10MHz interaction rates

  cout << "total events number:" << nevent <<endl;

// Event loop
	
for (Int_t iev=0; iev< nevent; iev++) {
// for (Int_t iev=0; iev< nnevent; iev++) {
// get entry	
    t1->GetEntry(iev);
    t2->GetEntry(iev);
		
    nMCTracks = MCTracks->GetEntriesFast();
    nTofPoints = TofPoints->GetEntriesFast();
    nTofHits = TofHits->GetEntriesFast();
		
    cout << iev << " Event" << endl;
    cout << nMCTracks << " MC tracks" <<endl;
    cout << nTofPoints << " Tof points" <<endl;
    cout << nTofHits << " Tof Hits" <<endl;

// MCTrack loop		
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
    }

// TofPoint loop					
    for (Int_t k=0; k< nMCTracks; k++) { // reset track detected flag
      TrackP[k]=0;
    }
    for (Int_t l =0; l<nTofPoints; l++) {
      TofPoint = (CbmTofPoint*) TofPoints->At(l);
      Int_t k = TofPoint->GetTrackID();
      MCTrack = (CbmMCTrack*) MCTracks->At(k);
      pid = MCTrack->GetPdgCode();	
	
      if (TrackP[k]==0) { // for efficiency
        TrackP[k]=1;
	if (pid==321) {
	  ptm_rap_poi_kp->Fill(MCTrack->GetRapidity(),MCTrack->GetPt()/MCTrack->GetMass());
	}		    
	if (pid==-321) {
	  ptm_rap_poi_km->Fill(MCTrack->GetRapidity(),MCTrack->GetPt()/MCTrack->GetMass());
	}
	x_poi		= TofPoint -> GetX();
	y_poi		= TofPoint -> GetY();
	z_poi		= TofPoint -> GetZ();
	tof_poi	= TofPoint -> GetTime();
	SMtype_poi	= TofPoint -> GetSMtype();
	mod_poi	= TofPoint -> GetModule();
	cel_poi	= TofPoint -> GetCell();
	gap_poi	= TofPoint -> GetGap();

        xy_poi1	->Fill(x_poi,y_poi);
        xy_poi2	->Fill(x_poi,y_poi,wxy2);
        xy_poi3	->Fill(x_poi,y_poi,rxy2);	
	
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

// TofHit loop					
    for (Int_t k=0; k< nMCTracks; k++) { // reset track detected flag
      TrackP[k]=0;
    }
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
	  // if (pid==211 || pid==-211) {
	  //mass_pion->Fill(MCTrack->GetMass());
	  //pt_rap_pion->Fill(MCTrack->GetRapidity(),MCTrack->GetPt());
	  //ptm_rap_pion->Fill(MCTrack->GetRapidity(),MCTrack->GetPt()/MCTrack->GetMass());
	}
	if (pid==2212) {
	  //p_rap->Fill(MCTrack->GetRapidity(),MCTrack->GetP());
	  //pt_rap->Fill(MCTrack->GetRapidity(),MCTrack->GetPt());
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
	
      t_hit	= TofHit->GetTime();
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
      Float_t bet = beta / 30.;  
      if (bet > 0.999) {bet=0.999;} 
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
      pv_hit->Fill(beta,p_MC);
      tm_hit->Fill(p_MC,tofmass);

      if (flg_hit==1) {
       tm_hit1->Fill(p_MC,tofmass);
      }elseif (flg_hit==2) {
       tm_hit2->Fill(p_MC,tofmass);
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
//		cout << count << " count"<< endl;						
 } // event loop end 
}
