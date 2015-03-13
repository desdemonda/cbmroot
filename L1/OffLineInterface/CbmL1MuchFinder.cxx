#include "CbmL1MuchFinder.h"

#include "CbmL1MuchHit.h"
#include "CbmL1MuchTrack.h"

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
#include "CbmStsTrackMatch.h"

#include "TFile.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TClonesArray.h"

#include <iostream>
#include <vector>
#include <cmath>

using std::cout;
using std::endl;
using std::vector;
using std::fabs;

ClassImp(CbmL1MuchFinder);

CbmL1MuchFinder::CbmL1MuchFinder(const char *name, Int_t iVerbose ):FairTask(name, iVerbose)
{
  fTrackCollection = new TClonesArray("CbmMuchTrack", 100);
  histodir = 0;
}


CbmL1MuchFinder::~CbmL1MuchFinder()
{
}

InitStatus CbmL1MuchFinder::Init()
{
  return ReInit();
}

InitStatus CbmL1MuchFinder::ReInit()
{
  fMuchPoints=(TClonesArray *) FairRootManager::Instance()->GetObject("MuchPoint");
  fMuchHits =(TClonesArray *) FairRootManager::Instance()->GetObject("MuchHit");
  fStsTracks =(TClonesArray *) FairRootManager::Instance()->GetObject("StsTrack");
  fMCTracks =(TClonesArray *) FairRootManager::Instance()->GetObject("MCTrack");
  fSTSTrackMatch = (TClonesArray*) FairRootManager::Instance()->GetObject("StsTrackMatch");
  fPrimVtx =  (CbmVertex *) FairRootManager::Instance() ->GetObject("PrimaryVertex");
  fStsFitter.Init();

  FairRootManager::Instance()->Register("MuchTrack", "Much", fTrackCollection, kTRUE);

  return kSUCCESS;
}

void CbmL1MuchFinder::SetParContainers()
{
}

void CbmL1MuchFinder::Finish()
{
  Write();
}

void CbmL1MuchFinder::Exec(Option_t * option)
{
  const int MaxBranches = 50;

  static bool first_call_murec = 1; 
  fTrackCollection->Clear();
  static int EventCounter = 0;    
  EventCounter++;
  cout<<" MuRec event "<<EventCounter<<endl;

  int MuNStations = CbmKF::Instance()->MuchStation2MCIDMap.size();
  
  if ( first_call_murec ){
    first_call_murec = 0;
    TDirectory *curdir = gDirectory;
    histodir = gDirectory->mkdir("MuRec");
    histodir->cd();
    fhNBranches = new TH1F("NBranches","N Branches",MaxBranches,0,MaxBranches);
    curdir->cd();
  }
  
  int NHits = fMuchHits->GetEntriesFast();
  vector<CbmL1MuchHit> vMuchHits;

  for( int ih = 0; ih < NHits; ih++ ){
    CbmMuchHit *h = (CbmMuchHit*) fMuchHits->At(ih);
    CbmL1MuchHit m( h, ih );
    vMuchHits.push_back(m);
  }
  
  vector<CbmL1MuchTrack> vTracks;
  
  int NStsTracks = fStsTracks->GetEntries();
 
  CbmL1MuchTrack Branches[MaxBranches];

  for( int itr=0; itr<NStsTracks; itr++ ){
    
    CbmStsTrack *sts = (CbmStsTrack*) fStsTracks->At(itr);
    if ( sts->GetNStsHits()+sts->GetNMvdHits()<4 ) continue;

    // MC
    if( 0&&fSTSTrackMatch && fMCTracks ){
      CbmStsTrackMatch *m = (CbmStsTrackMatch*) fSTSTrackMatch->At(itr);
      if( !m ) continue;
      Int_t mcTrackID = m->GetMCTrackId();
      if (mcTrackID<0)  continue;  
      CbmMCTrack*  mcTrack= (CbmMCTrack*)  fMCTracks->At(mcTrackID);
      if( !mcTrack ) continue;
      if( abs(mcTrack->GetPdgCode())!=13 ) continue;
    }

    fStsFitter.DoFit( sts, 13 ); // refit with muon mass

    int NBranches = 1;

    Branches[0].SetStsTrack(sts);
    Branches[0].StsID = itr;
    Branches[0].NHits = 0;
    Branches[0].NMissed = 0;
    Branches[0].NMissedStations = 0;
    Branches[0].ok = 1;
    Branches[0].stopped = 0;
    Branches[0].vHits.clear();
    //cout<<"Sts track N "<<itr<<" with initial mom="<<1./Branches[0].T[4]<<endl;
    for( int ist=0; ist<MuNStations; ist++ ){
      
      int NBranchesOld = NBranches;

      for( int ibr=0; ibr<NBranchesOld; ibr++ ){
	CbmL1MuchTrack &tr = Branches[ibr];
	if( tr.stopped ) continue;
	//if( ist%3 ==0 ) cout<<" | ";
	double Zdet = CbmKF::Instance()->vMuchDetectors[ist].ZReference;
	tr.Extrapolate(Zdet);
	if( fabs(tr.T[4])>100.) tr.stopped = 1;
	if( 1.<0.5 *fabs(tr.T[4]) ) tr.stopped = 1; // 0.5 GeV, stop transport
	//if( tr.stopped ) cout<<"Sts track N "<<itr<<" stopped at Mu station "<<ist
	//<<" with mom="<<1./tr.T[4]<<endl;
	if( tr.stopped ) continue;
	if( CbmKF::Instance()->vMuchDetectors[ist].IsOutside( tr.T[0], tr.T[1] ) ){
	  //cout<<" out ";
	  tr.NMissedStations++;
	  continue;
	}

	vector<int> NewHits;
	for( int ih=0; ih<NHits; ih++ ){
	  CbmL1MuchHit &h = vMuchHits[ih];
	  if( h.iStation != ist ) continue;
	  if(0){ // !!! Cut for the time of flight (ns)
	    double hl = sqrt(h.FitPoint.x*h.FitPoint.x+h.FitPoint.y*h.FitPoint.y+h.FitPoint.z*h.FitPoint.z);
	    double hp = 1./fabs(tr.T[4]);
	    double texp = hl*sqrt(1. - 0.1057*0.1057/(hp*hp))/29.9792458;  //ns
	    if( h.time - texp > 1.0 ) continue;
	  }
	  double dx = tr.T[0] - h.FitPoint.x;
	  double dy = tr.T[1] - h.FitPoint.y;
	  double c0 = tr.C[0] + h.FitPoint.V[0];
	  double c1 = tr.C[1] + h.FitPoint.V[1];
	  double c2 = tr.C[2] + h.FitPoint.V[2];
	  double chi2 = 0.5*(dx*dx*c0-2*dx*dy*c1+dy*dy*c2)/(c0*c2-c1*c1);
	  if ( chi2 <= 20. ) NewHits.push_back( ih );
	}
	int nnew = NewHits.size();
	for( int ih=1; ih<nnew; ih++){
	  if( NBranches >= MaxBranches ) break;
	  CbmL1MuchTrack &t = Branches[NBranches++];
	  t = tr;
	  CbmL1MuchHit &h = vMuchHits[NewHits[ih]];
	  t.vHits.push_back(&h);
	  t.NHits++;
	  double qp0 = t.T[4];
	  h.Filter(t, 1, qp0);	
	}
	if( nnew >0 ){
	  CbmL1MuchHit &h = vMuchHits[NewHits[0]];
	  tr.vHits.push_back(&h);
	  tr.NHits++;
	  double qp0 = tr.T[4];
	  h.Filter(tr, 1, qp0);
	}else tr.NMissed++;
      }
    }
    int bestbr=0;
    for( int ibr=1; ibr<NBranches; ibr++ ){
      if( (Branches[ibr].NHits  > Branches[bestbr].NHits) ||
	  (Branches[ibr].NHits == Branches[bestbr].NHits)&&
	  (Branches[ibr].chi2<Branches[bestbr].chi2)
	  ) bestbr = ibr;
    }
    vTracks.push_back(Branches[bestbr]);
    // MC
    if( fSTSTrackMatch && fMCTracks ){
      CbmStsTrackMatch *m = (CbmStsTrackMatch*) fSTSTrackMatch->At(itr);
      if( !m ) continue;
      Int_t mcTrackID = m->GetMCTrackId();
      if (mcTrackID<0)  continue;
      CbmMCTrack*  mcTrack= (CbmMCTrack*)  fMCTracks->At(mcTrackID);
      if( !mcTrack ) continue;
      if( abs(mcTrack->GetPdgCode())==13 ) fhNBranches->Fill(NBranches);
    }
  } // tracks

  int NTracks = vTracks.size();
  cout<<"NTracks="<<NTracks<<endl;
  //sort tracks and check for common muon hits

  vector<CbmL1MuchTrack*> vpTracks;
  for( int i=0; i<NTracks; i++ ) vpTracks.push_back(&(vTracks[i]));
  sort( vpTracks.begin(), vpTracks.end(), CbmL1MuchTrack::Compare );
  
  int NOutTracks = fTrackCollection->GetEntries();

  for( int it=0; it<NTracks; it++ ){
    CbmL1MuchTrack &tr = *vpTracks[it];
    if( tr.NDF<=0 || tr.chi2 > 10.*tr.NDF ){ 
      tr.ok = 0; 
      continue; 
    }
    int nbusy = 0;
    for( int ih=0; ih<tr.NHits; ih++ ) if( tr.vHits[ih]->busy ) nbusy++;      
    if( 0 && nbusy>2 ){
      tr.ok = 0;
      continue;
    }
    { 
      new((*fTrackCollection)[NOutTracks]) CbmMuchTrack();
      CbmMuchTrack* MuchTrack = (CbmMuchTrack*) fTrackCollection->At(NOutTracks++);
      MuchTrack->SetChi2(tr.GetRefChi2());
      MuchTrack->SetNDF(tr.GetRefNDF());
      FairTrackParam tp;
      CbmKFMath::CopyTC2TrackParam( &tp, tr.T, tr.C );
      MuchTrack->SetMuchTrack( &tp );
      MuchTrack->SetStsTrackID(tr.StsID);
      int nh=0;
      for( vector<CbmL1MuchHit* >::iterator i= tr.vHits.begin(); i!=tr.vHits.end(); i++){
	if( ++nh>30 ) break;
	MuchTrack->AddHitIndex( (*i)->index );
      } 
      MuchTrack->SetNMissedHits( tr.NMissed );
      MuchTrack->SetNMissedStations( tr.NMissedStations );
    }
    for( int ih=0; ih<tr.NHits; ih++ ) tr.vHits[ih]->busy=1;
  }

  if ( EventCounter <  100 && EventCounter % 10  == 0 || 
       EventCounter >= 100 && EventCounter % 100 == 0
       ) Write();
  //cout<<"end of MuRec"<<endl;
}


void CbmL1MuchFinder::Write()
{
  TFile HistoFile("MuRec.root","RECREATE");
  writedir2current(histodir);
  HistoFile.Close();
}

void CbmL1MuchFinder::writedir2current( TObject *obj ){
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
