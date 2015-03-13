#include "CbmL1MuchFinderQa.h"

#include "CbmL1MuchHit.h"
#include "CbmL1MuchTrack.h"

#include "CbmStsTrackMatch.h"
#include "CbmMuchHit.h"
#include "CbmKF.h"
#include "CbmKFMath.h"
#include "FairRootManager.h"
#include "CbmKFHit.h"
#include "CbmKFPixelMeasurement.h"
#include "CbmKFMaterial.h"
#include "CbmKFTrackInterface.h"
#include "CbmMuchHit.h"
#include "CbmMuchTrack.h"
#include "CbmMuchPoint.h"
#include "CbmStsTrack.h"
#include "CbmMCTrack.h"
#include "CbmStsKFTrackFitter.h"
#include "CbmVertex.h"

#include "TFile.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TProfile.h"
#include "TClonesArray.h"

#include <iostream>
#include <vector>
#include <cmath>

using std::cout;
using std::endl;
using std::vector;
using std::fabs;

ClassImp(CbmL1MuchFinderQa);

CbmL1MuchFinderQa::CbmL1MuchFinderQa(const char *name, Int_t iVerbose ):FairTask(name, iVerbose)
{
  histodir = 0;
  fhPerfAll = 0;
  fhPerfSignal = 0;
}


CbmL1MuchFinderQa::~CbmL1MuchFinderQa()
{
}

InitStatus CbmL1MuchFinderQa::Init()
{
  return ReInit();
}

InitStatus CbmL1MuchFinderQa::ReInit()
{
  fMuchPoints=(TClonesArray *) FairRootManager::Instance()->GetObject("MuchPoint");
  fMuchHits =(TClonesArray *) FairRootManager::Instance()->GetObject("MuchHit");
  fStsTracks =(TClonesArray *) FairRootManager::Instance()->GetObject("StsTrack");
  fMuchTracks =(TClonesArray *) FairRootManager::Instance()->GetObject("MuchTrack");
  fMCTracks =(TClonesArray *) FairRootManager::Instance()->GetObject("MCTrack");
  fPrimVtx =  (CbmVertex *) FairRootManager::Instance() ->GetObject("PrimaryVertex");
  fSTSTrackMatch = (TClonesArray*) FairRootManager::Instance()->GetObject("StsTrackMatch");
  fStsFitter.Init();

  return kSUCCESS;
}

void CbmL1MuchFinderQa::SetParContainers()
{
}

void CbmL1MuchFinderQa::Finish()
{
  Write();
}

void CbmL1MuchFinderQa::Exec(Option_t * option)
{
  
  static int EventCounter = 0;  
  EventCounter++;
  cout<<" MuRecQa event "<<EventCounter<<endl;

  static bool first_call = 1; 

  int MuNStations = CbmKF::Instance()->MuchStation2MCIDMap.size();
  
  if ( first_call ){
    first_call = 0;
    TDirectory *curdir = gDirectory;
    histodir = gDirectory->mkdir("MuRecQa");
    histodir->cd();

    fhPerfSignal = new TProfile("eff_signal","Signal Mu eff vs mom",60,0,30);
    fhPerfAll = new TProfile("eff_all","All Mu eff vs mom",60,0,30);
    fhGhost = new TProfile("ghost","ghost vs mom",60,0,30);

    char S1[255], S2[255];
    for( int ist=0; ist<MuNStations; ist++ ){
      sprintf(S1,"station %i",ist);
      //TDirectory *d = histodir->mkdir(S1);
      //d->cd();
      sprintf(S1,"Pull_x_%i",ist);
      sprintf(S2,"Pull_x of sts track at Mu detector %i ",ist);
      histPull_dx[ist] = new TH1F(S1,S2,100,-10.,10.);     
      sprintf(S1,"Pull_y_%i",ist);
      sprintf(S2,"Pull_y of sts track at Mu detector %i ",ist);
      histPull_dy[ist] = new TH1F(S1,S2,100,-10.,10.);
      sprintf(S1,"Pull_tx_%i",ist);
      sprintf(S2,"Pull_tx of sts track at Mu detector %i ",ist);
      histPull_tx[ist] = new TH1F(S1,S2,100,-10.,10.);     
      sprintf(S1,"Pull_ty_%i",ist);
      sprintf(S2,"Pull_ty of sts track at Mu detector %i ",ist);
      histPull_ty[ist] = new TH1F(S1,S2,100,-10.,10.);
      sprintf(S1,"Pull_qp_%i",ist);
      sprintf(S2,"Pull_qp of sts track at Mu detector %i ",ist);
      histPull_qp[ist] = new TH1F(S1,S2,100,-10.,10.);      
 
      sprintf(S1,"chi2hit_%i",ist);
      sprintf(S2,"chi2 between track and hit at Mu detector %i ",ist);
      histChi2[ist] = new TH1F(S1,S2,1000,0,100);


      sprintf(S1,"b_Pull_x_%i",ist);
      sprintf(S2,"b_Pull_x of sts track at Mu detector %i ",ist);
      histPull_dx[20+ist] = new TH1F(S1,S2,100,-10.,10.);     
      sprintf(S1,"b_Pull_y_%i",ist);
      sprintf(S2,"b_Pull_y of sts track at Mu detector %i ",ist);
      histPull_dy[20+ist] = new TH1F(S1,S2,100,-10.,10.);
      sprintf(S1,"b_Pull_tx_%i",ist);
      sprintf(S2,"b_Pull_tx of sts track at Mu detector %i ",ist);
      histPull_tx[20+ist] = new TH1F(S1,S2,100,-10.,10.);     
      sprintf(S1,"b_Pull_ty_%i",ist);
      sprintf(S2,"b_Pull_ty of sts track at Mu detector %i ",ist);
      histPull_ty[20+ist] = new TH1F(S1,S2,100,-10.,10.);
      sprintf(S1,"b_Pull_qp_%i",ist);
      sprintf(S2,"b_Pull_qp of sts track at Mu detector %i ",ist);
      histPull_qp[20+ist] = new TH1F(S1,S2,100,-10.,10.);      


    }
    curdir->cd();
  }

  if( !fSTSTrackMatch ) return;
  if( !fMuchHits ) return;
  if( !fMuchPoints ) return;
  if( !fStsTracks ) return;
  if( !fMuchTracks ) return;
  if( !fSTSTrackMatch ) return;
  if( !fMCTracks ) return;

  int NHits = fMuchHits->GetEntries();
  int NStsTracks = fStsTracks->GetEntries();
  int NMuchTracks = fMuchTracks->GetEntries();
  //int NMCTracks = fMCTracks->GetEntries();

  bool was_problem = 0;

  static int NMu5=0, NMuRec5=0, NMu10=0, NMuRec10=0;
  static int NMuS5=0, NMuSRec5=0, NMuS10=0, NMuSRec10=0;
  static int NGhost5=0, NGhostRec5=0, NGhost10=0, NGhostRec10=0;
  
  int IndMuTrack[NStsTracks];

  for( int itr=0; itr<NStsTracks; itr++ ) IndMuTrack[itr] = -1;    
  
  for( int itr=0; itr<NMuchTracks; itr++ ){
    CbmMuchTrack *Tmu = (CbmMuchTrack*) fMuchTracks->At(itr);
    if( !Tmu ) continue;
    int ists = Tmu->GetStsTrackID();
    if( ists <0 ) continue;
    IndMuTrack[ists] = itr;
  }
  
  for( int ists=0; ists<NStsTracks; ists++ ){
    CbmStsTrack *Tsts = (CbmStsTrack*) fStsTracks->At( ists );
    CbmStsTrackMatch *m = (CbmStsTrackMatch*) fSTSTrackMatch->At(ists);
    if( !Tsts ) continue;
    if( !m ) continue;
    double ratio = 1.*m->GetNofTrueHits()
      /(m->GetNofTrueHits()+m->GetNofWrongHits()+m->GetNofFakeHits());      
    if( ratio<.7 ) continue;
    Int_t mcTrackID = m->GetMCTrackId();
    if (mcTrackID<0)  continue;  
    CbmMCTrack*  mcTrack= (CbmMCTrack*)  fMCTracks->At(mcTrackID);
    if( !mcTrack ) continue;
    //if (fabs((mcTrack->GetStartVertex()).z())>1.) continue;
    //if( mcTrack->GetMotherID() !=-1 ) continue;

    Double_t P = mcTrack->GetP();

    int NMuHits = 0;
    int NMissedStations = 0;
    int NH = fMuchHits->GetEntriesFast();
    int ista_last = -1;
    for( int ih = 0; ih < NH; ih++ ){
      CbmMuchHit *h = (CbmMuchHit*) fMuchHits->At(ih);
      int ist = h->GetStationNr()-1;
      if( ist!= ista_last+1 ){
	NMissedStations+=ist-1-ista_last;
      }
      ista_last = ist;      
      int ip =  h->GetRefIndex();
      if( ip<0 ) continue;
      CbmMuchPoint *p = (CbmMuchPoint *) fMuchPoints->At(ip);
      if( mcTrackID == p->GetTrackID() ) NMuHits++;
    }

    if (Tsts->GetNStsHits()+Tsts->GetNMvdHits()<4 ) continue;
    
    bool Muon = ( abs(mcTrack->GetPdgCode())==13 );
    bool Signal = ( mcTrack->GetMotherId()<0 && 
		    mcTrackID<3 && Muon );

    if( Muon && (NMuHits <3 || NMissedStations>0) ) continue;

    bool MuonFlag = 0;
    if( IndMuTrack[ists] >=0 ){
      CbmMuchTrack *Tmu = (CbmMuchTrack*) fMuchTracks->At(IndMuTrack[ists]);
      if( Tmu && Tmu->GetNHits() + Tmu->GetNMissedHits()>=3 )    
        MuonFlag = Tmu->GetNMissedHits()<=2 && Tmu->GetNMissedStations()==0;
    }

    //cout<<ok<<" "<<Tmu->GetNHits()<<" "<< Tmu->GetNMissedHits()<<endl;
    if( Muon ){ // muon
      if( Signal ){
	fhPerfSignal->Fill( P, MuonFlag );
	if( P<5 ){ NMuS5++; NMuSRec5+=MuonFlag; }
	else { NMuS10++; NMuSRec10+=MuonFlag; }
      }
      fhPerfAll->Fill( P, MuonFlag );
      if( P<5 ){ NMu5++; NMuRec5+=MuonFlag; }
      else { NMu10++; NMuRec10+=MuonFlag; }
      if( !MuonFlag ) was_problem = 1;
    }else{
      fhGhost->Fill(P, MuonFlag);
      if( P<5 ){ NGhost5++; NGhostRec5+=MuonFlag; }
      else { NGhost10++; NGhostRec10+=MuonFlag; }
    }
  } 
  
  cout<<" N Signal Mu Total/Reconstructed <5Gev = "<<NMuS5<<"/"<<NMuSRec5<<"; >=5Gev = "
      <<NMuS10<<"/"<<NMuSRec10<<endl;
  cout<<" N Mu Total/Reconstructed <5Gev = "<<NMu5<<"/"<<NMuRec5<<"; >=5Gev = "
      <<NMu10<<"/"<<NMuRec10<<endl;
  cout<<" N Ghost Total/Reconstructed <5Gev = "<<NGhost5<<"/"<<NGhostRec5<<"; >=5Gev = "
      <<NGhost10<<"/"<<NGhostRec10<<endl;
  
  //if( was_problem ) cout<<"\n problem\n"<<endl;//return;
  
  vector<CbmL1MuchHit> vMuchHits;
  
  for( int ih = 0; ih < NHits; ih++ ){
    CbmMuchHit *h = (CbmMuchHit*) fMuchHits->At(ih);
    CbmL1MuchHit m( h, ih );
    vMuchHits.push_back(m);
  }

  /*
  for( int mcTrackID=0; mcTrackID<NMCTracks; mcTrackID++ ){
    CbmMCTrack*  mcTrack= (CbmMCTrack*)  fMCTracks->At(mcTrackID);
    if( !mcTrack ) continue;     
    //cout<<"mc track "<<mcTrack->GetMotherID()<<"->"<<mcTrackID
    //<<" ( pdg "<<mcTrack->GetPdgCode()<<", p "<<mcTrack->GetMomentum().Mag()<<" ) : ";
    for( int ist=0; ist<MuNStations; ist++ ){
      //if( ist%3 ==0 ) cout<<" | ";
      CbmMuchPoint *mp = 0;
      for( int ih=0; ih<NHits; ih++ ){
	CbmL1MuchHit &h = vMuchHits[ih];
	if( h.iStation != ist ) continue;
	CbmMuchHit *mh = (CbmMuchHit*) fMuchHits->At(h.index);
	int ip = mh->GetRefIndex();
	if( ip<0 ) continue;
	CbmMuchPoint *pp = (CbmMuchPoint *) fMuchPoints->At(ip);
	if( mcTrackID == pp->GetTrackID() ){
	  mp = pp;
	  break;
	}
      }
      //if(mp ) cout<<"+ ";
      //else cout<<"- ";
    }
    //cout<<endl;
  }
  */

  for( int itr=0; itr<NStsTracks; itr++ ){
    CbmStsTrack *Tsts = (CbmStsTrack*) fStsTracks->At(itr);
    if( !Tsts ) continue;
    if ( Tsts->GetNStsHits()+Tsts->GetNMvdHits()<4 ) continue;

    CbmStsTrackMatch *m = (CbmStsTrackMatch*) fSTSTrackMatch->At(itr);
    if( !m ) continue;
    Int_t mcTrackID = m->GetMCTrackId();
    if (mcTrackID<0)  continue;  
    CbmMCTrack*  mcTrack= (CbmMCTrack*)  fMCTracks->At(mcTrackID);
    if( !mcTrack ) continue;
    if( abs(mcTrack->GetPdgCode())!=13 ) continue;
    double ratio = 1.*m->GetNofTrueHits()
      /(m->GetNofTrueHits()+m->GetNofWrongHits()+m->GetNofFakeHits());      
    if( ratio<.7 ) continue;

    fStsFitter.DoFit( Tsts, 13 ); // refit with muon mass
    CbmL1MuchTrack tr;
    tr.SetStsTrack(Tsts);
    tr.StsID = itr;
    tr.NHits = 0;
    tr.NMissed = 0;
    tr.ok = 1;
    
    for( int ist=0; ist<MuNStations; ist++ ){
      double Zdet = CbmKF::Instance()->vMuchDetectors[ist].ZReference;
      tr.Extrapolate(Zdet);
      if( fabs(tr.T[4])>100.) break;
      for( int ih=0; ih<NHits; ih++ ){
	CbmL1MuchHit &h = vMuchHits[ih];
	if( h.iStation != ist ) continue;
	CbmMuchHit *mh = (CbmMuchHit*) fMuchHits->At(h.index);
	int ip = mh->GetRefIndex();
	if( ip<0 ) continue;
	CbmMuchPoint *p = (CbmMuchPoint *) fMuchPoints->At(ip);
	if( p->GetTrackID()!=mcTrackID ) continue;

	double dx = tr.T[0] - h.FitPoint.x;
	double dy = tr.T[1] - h.FitPoint.y;
	double c0 = tr.C[0] + h.FitPoint.V[0];
	double c1 = tr.C[1] + h.FitPoint.V[1];
	double c2 = tr.C[2] + h.FitPoint.V[2];
	double chi2 = 0.5*(dx*dx*c0-2*dx*dy*c1+dy*dy*c2)/(c0*c2-c1*c1);

	histChi2[ist]->Fill(chi2);
	{
	  TVector3 pos, mom;
	  CbmL1MuchTrack tt = tr;
	  p->Position(pos);
	  p->Momentum(mom);
	  tt.Propagate(pos.Z());
	  Int_t  j = ist+20;
	  double sx =sqrt(tt.C[0]);
	  double sy =sqrt(tt.C[2]);
	  double stx =sqrt(tt.C[5]);
	  double sty =sqrt(tt.C[9]);
	  double sqp =sqrt(tt.C[14]);
	  double dx = (tt.T[0] - pos.X())/sx;
	  double dy = (tt.T[1] - pos.Y())/sy;
	  double dtx= (tt.T[2] - mom.X()/mom.Z())/stx;
	  double dty= (tt.T[3] - mom.Y()/mom.Z())/sty;
	  double dqp= (fabs(tt.T[4])-1./mom.Mag())/sqp;
	  if(sx >0.0) histPull_dx[j]->Fill( dx );
	  if(sy >0.0) histPull_dy[j]->Fill( dy );
	  if(stx >0.0) histPull_tx[j]->Fill( dtx );
	  if(sty >0.0) histPull_ty[j]->Fill( dty );
	  if(sqp>0.0) histPull_qp[j]->Fill( dqp );
	}

	double qp0 = tr.T[4];
	h.Filter(tr, 1, qp0);
	{
	  TVector3 pos, mom;
	  CbmL1MuchTrack tt = tr;
	  p->Position(pos);
	  p->Momentum(mom);
	  tt.Propagate(pos.Z());
	  if(tt.C[0] >0.0) histPull_dx[ist]->Fill( (tt.T[0] - pos.X())/sqrt(tt.C[0]) );
	  if(tt.C[2] >0.0) histPull_dy[ist]->Fill( (tt.T[1] - pos.Y())/sqrt(tt.C[2]) );
	  if(tt.C[5] >0.0) histPull_tx[ist]->Fill( (tt.T[2] - mom.X()/mom.Z())/sqrt(tt.C[5]) );
	  if(tt.C[9] >0.0) histPull_ty[ist]->Fill( (tt.T[3] - mom.Y()/mom.Z())/sqrt(tt.C[9]) );
	  if(tt.C[14]>0.0) histPull_qp[ist]->Fill( (fabs(tt.T[4])-1./mom.Mag())/sqrt(tt.C[14]) );
	}
      }
    }
  } // tracks


  
  if ( EventCounter <  100 && EventCounter % 10  == 0 || 
       EventCounter >= 100 && EventCounter % 100 == 0
       ) Write();
}


void CbmL1MuchFinderQa::Write()
{
  TFile HistoFile("MuRecQa.root","RECREATE");
  writedir2current(histodir);
  HistoFile.Close();
}

void CbmL1MuchFinderQa::writedir2current( TObject *obj ){
  if( !obj->IsFolder() ) obj->Write();
  else{
    TDirectory *cur = gDirectory;
    TDirectory *sub = cur->mkdir(obj->GetName());
    sub->cd();
    TList *listSub = ((TDirectory*)obj)->GetList();
    TIter it(listSub);
    while( TObject *obj1=it() ) writedir2current(obj1);
    cur->cd();
  }
}
