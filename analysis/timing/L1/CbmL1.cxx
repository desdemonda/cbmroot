/*
 *====================================================================
 *
 *  CBM Level 1 Reconstruction 
 *  
 *  Authors: I.Kisel,  S.Gorbunov
 *
 *  e-mail : ikisel@kip.uni-heidelberg.de 
 *
 *====================================================================
 *
 *  L1 main program
 *
 *====================================================================
 */
#include "CbmL1.h"

#include "L1Algo/L1Algo.h"
#include "L1Algo/L1StsHit.h"
#include "L1Algo/L1Branch.h"
#include "L1Algo/L1Field.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "CbmGeoStsPar.h"
#include "CbmStsStation.h"
#include "CbmStsSector.h"
#include "CbmStsSensor.h" // for field approx.
#include "CbmStsDigiPar.h" // for dynamic_cast
#include "CbmStsDigiScheme.h"
#include "CbmStsFindTracks.h"
#include "CbmKF.h"

#include "CbmL1ParticlesFinder.h"

#include "TVector3.h"
#include "TVectorD.h"
#include "TMatrixD.h"
#include "TROOT.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::ios;
using std::vector;

ClassImp(CbmL1)


static L1Algo algo_static _fvecalignment;

CbmL1 *CbmL1::fInstance = 0;



CbmL1::CbmL1():
    fEpoch(0),
    fSliceSeperationTime(1),
    algo(0), // for access to L1 Algorithm from L1::Instance
vRTracks(), // reconstructed tracks
vHitStore(),
NStation(0), NMvdStations(0), NStsStations(0), // number of detector stations (all\sts\mvd)
fPerformance(0),
fSTAPDataMode(0),
fSTAPDataDir(""),

fTrackingLevel(2),  // really doesn't used
fMomentumCutOff(0.1),  // really doesn't used
fGhostSuppression(1),  // really doesn't used
fUseMVD(0),  // really doesn't used

StsDigi(),
PrimVtx(),

listMCTracks (0),
listStsPts(0),
listStsDigi(0),
listStsClusters(0),
listStsHits(0),

listMvdPts(0),
listMvdHits(0),
listMvdHitMatches(0),
vStsHits(),
vMCPoints(),
vMCTracks(),
vHitMCRef(),
histodir(0),
    fFindParticlesMode()
{
  if( !fInstance ) fInstance = this;
  PF = new CbmL1ParticlesFinder();
  sliceArray = new TClonesArray("CbmStsHit");
}

CbmL1::CbmL1(const char *name, Int_t iVerbose, Int_t _fPerformance, int fSTAPDataMode_, TString fSTAPDataDir_, int findParticleMode_):FairTask(name,iVerbose),
fEpoch(0),
algo(0), // for access to L1 Algorithm from L1::Instance

vRTracks(), // reconstructed tracks
vHitStore(),
NStation(0), NMvdStations(0), NStsStations(0), // number of detector stations (all\sts\mvd)
fPerformance(_fPerformance),
fSTAPDataMode(fSTAPDataMode_),
fSTAPDataDir(fSTAPDataDir_),

fTrackingLevel(2),  // really doesn't used
fMomentumCutOff(0.1),  // really doesn't used
fGhostSuppression(1),  // really doesn't used
fUseMVD(0),  // really doesn't used

StsDigi(),
PrimVtx(),

listMCTracks (0),
listStsPts(0),
listStsDigi(0),
listStsClusters(0),
listStsHits(0),

listMvdPts(0),
listMvdHits(0),
listMvdHitMatches(0),
vStsHits(),
vMCPoints(),
vMCTracks(),
vHitMCRef(),
histodir(0),
    fFindParticlesMode(findParticleMode_)
{
  if( !fInstance ) fInstance = this;
  PF = new CbmL1ParticlesFinder();
}

CbmL1::~CbmL1()
{
  if( fInstance==this ) fInstance = 0;
}



void CbmL1::SetParContainers()
{
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  rtdb->getContainer("CbmGeoPassivePar");
  rtdb->getContainer("CbmGeoStsPar");
  rtdb->getContainer("CbmStsDigiPar");
}

InitStatus CbmL1::ReInit()
{
  StsDigi.Clear();
  SetParContainers();
  return Init();
}

InitStatus CbmL1::Init()
{
  if( fVerbose>1 ){
    char y[20] = " [0;33;44m"; // yellow
    char Y[20] = " [1;33;44m"; // yellow bold
    char W[20] = " [1;37;44m"; // white bold
    char o[20] = " [0m\n";       // color off
    Y[0] = y[0] = W[0] = o[0] = 0x1B;    // escape character
    
    cout<<endl<<endl;
    cout<<"  "<<W<<"                                                                 "<<o;
    cout<<"  "<<W<<"  ===////======================================================  "<<o;
    cout<<"  "<<W<<"  =                                                           =  "<<o;
    cout<<"  "<<W<<"  =                   "
        <<Y                      <<"L1 on-line finder"
        <<W                                       <<"                       =  "<<o;
    cout<<"  "<<W<<"  =                                                           =  "<<o;
    cout<<"  "<<W<<"  =  "
        <<W     <<"Cellular Automaton 3.1 Vector"
        <<y                                  <<" with "
        <<W                                        <<"KF Quadro"
        <<y                                                 <<" technology"
        <<W                                                            <<"  =  "<<o;
    cout<<"  "<<W<<"  =                                                           =  "<<o;
    cout<<"  "<<W<<"  =  "
        <<y     <<"Designed for CBM collaboration"
        <<W                                   <<"                           =  "<<o;
    cout<<"  "<<W<<"  =  "
        <<y     <<"All rights reserved"
        <<W                        <<"                                      =  "<<o;
    cout<<"  "<<W<<"  =                                                           =  "<<o;
    cout<<"  "<<W<<"  ========================================================////=  "<<o;
    cout<<"  "<<W<<"                                                                 "<<o;
    cout<<endl<<endl;
  }

  FairRootManager *fManger = FairRootManager::Instance();

  FairRunAna *Run = FairRunAna::Instance();
  FairRuntimeDb *RunDB = Run->GetRuntimeDb();
  {
    CbmGeoStsPar* StsPar = L1_DYNAMIC_CAST<CbmGeoStsPar*>( RunDB->findContainer("CbmGeoStsPar") );
    CbmStsDigiPar *digiPar = L1_DYNAMIC_CAST<CbmStsDigiPar*>( RunDB->findContainer("CbmStsDigiPar") );
    StsDigi.Init(StsPar, digiPar);
  }
  {
    fUseMVD = 1;
    CbmStsFindTracks * FindTask = L1_DYNAMIC_CAST<CbmStsFindTracks*>( Run->GetTask("STSFindTracks") );
    if( FindTask ) fUseMVD = FindTask->MvdUsage();
  }

  histodir = gROOT->mkdir("L1");

  if (fPerformance){
    listMCTracks = L1_DYNAMIC_CAST<TClonesArray*>( fManger->GetObject("MCTrack") );
    listStsPts = L1_DYNAMIC_CAST<TClonesArray*>(  fManger->GetObject("StsPoint") );
    listStsClusters = L1_DYNAMIC_CAST<TClonesArray*>( fManger->GetObject("StsCluster") );
    listStsDigi = L1_DYNAMIC_CAST<TClonesArray*>( fManger->GetObject("StsDigi") );
  }
  else{
    listMCTracks = 0;
    listStsPts = 0;
    listStsClusters = 0;
    listStsDigi = 0;
  }
  listStsHits = L1_DYNAMIC_CAST<TClonesArray*>(  fManger->GetObject("StsHit") );
  listStsHitsAll = L1_DYNAMIC_CAST<TClonesArray*>(  fManger->GetObject("StsHit") );

  if (fPerformance){
    if( !fUseMVD ){
      listMvdPts = 0;
      listMvdHitMatches = 0;
    } else {
      listMvdPts = L1_DYNAMIC_CAST<TClonesArray*>(  fManger->GetObject("MvdPoint") );
      listMvdHitMatches = L1_DYNAMIC_CAST<TClonesArray*>(  fManger->GetObject("MvdHitMatch") );
    }
  }
  else{
    listMvdPts = 0;
    listMvdHitMatches = 0;
  }
  if( !fUseMVD ){
    listMvdHits = 0;
  } else {
    listMvdHits = L1_DYNAMIC_CAST<TClonesArray*>(  fManger->GetObject("MvdHit") );
  }

  NMvdStations = 0;
  NStsStations = 0;
  NStation = 0;
  algo = & algo_static;

  fscal geo[10000];

  int ind = 0;
  for( int i=0; i<3; i++ ){
    Double_t point[3] = { 0,0,2.5*i};
    Double_t B[3] = {0,0,0};
    if( CbmKF::Instance()->GetMagneticField() ) CbmKF::Instance()->GetMagneticField()->GetFieldValue( point, B );
    geo[ind++] = 2.5*i;
    geo[ind++] = B[0];
    geo[ind++] = B[1];
    geo[ind++] = B[2];
  }
      
  NMvdStations = ( fUseMVD ) ? CbmKF::Instance()->vMvdMaterial.size() : 0;
  NStsStations = StsDigi.GetNStations();
  NStation = NMvdStations + NStsStations;
  geo[ind++] = NStation;
  geo[ind++] = NMvdStations;

  // field
  const int M=5; // polinom order
  const int N=(M+1)*(M+2)/2;

//   { // field at the z=0 plane
//     const double Xmax = 10, Ymax = 10;
//     const double z = 0.;
//     double dx = 1.; // step for the field approximation
//     double dy = 1.;
//     if( dx > Xmax/N/2 ) dx = Xmax/N/4.;
//     if( dy > Ymax/N/2 ) dy = Ymax/N/4.;
// 
//     TMatrixD A(N,N);
//     TVectorD b0(N), b1(N), b2(N);
//     for( int i=0; i<N; i++){
//       for( int j=0; j<N; j++) A(i,j)==0.;
//       b0(i)=b1(i)=b2(i) = 0.;
//     }
//     for( double x=-Xmax; x<=Xmax; x+=dx )
//       for( double y=-Ymax; y<=Ymax; y+=dy )
//     {
//       double r = sqrt(fabs(x*x/Xmax/Xmax+y/Ymax*y/Ymax));
//       if( r>1. ) continue;
//       Double_t w = 1./(r*r+1);
//       Double_t p[3] = { x, y, z};
//       Double_t B[3] = {0.,0.,0.};
//       CbmKF::Instance()->GetMagneticField()->GetFieldValue(p, B);
//       TVectorD m(N);
//       m(0)=1;
//       for( int i=1; i<=M; i++){
//         int k = (i-1)*(i)/2;
//         int l = i*(i+1)/2;
//         for( int j=0; j<i; j++ ) m(l+j) = x*m(k+j);
//         m(l+i) = y*m(k+i-1);
//       }
//         
//       TVectorD mt = m;
//       for( int i=0; i<N; i++){
//         for( int j=0; j<N;j++) A(i,j)+=w*m(i)*m(j);
//         b0(i)+=w*B[0]*m(i);
//         b1(i)+=w*B[1]*m(i);
//         b2(i)+=w*B[2]*m(i);
//       }
//     }
//     double det;
//     A.Invert(&det);
//     TVectorD c0 = A*b0, c1 = A*b1, c2 = A*b2;
//     
//     targetFieldSlice = new L1FieldSlice;
//     for(int i=0; i<N; i++){
//       targetFieldSlice->cx[i] = c0(i);
//       targetFieldSlice->cy[i] = c1(i);
//       targetFieldSlice->cz[i] = c2(i);
//     }
// 
//   } // target field

  
  
  for ( Int_t ist = 0; ist<NStation; ist++ )
  {
    double C[3][N];
    double z = 0;
    double Xmax, Ymax;
    if( ist<NMvdStations ){
      CbmKFTube &t = CbmKF::Instance()->vMvdMaterial[ist];
      geo[ind++] = t.z;
      geo[ind++] = t.dz;
      geo[ind++] = t.r;
      geo[ind++] = t.R;
      geo[ind++] = t.RadLength;
      fscal f_phi=0, f_sigma=5.e-4, b_phi=3.14159265358/2., b_sigma=5.e-4;
      f_phi=0; f_sigma=5.e-4; b_phi=3.14159265358/2.; b_sigma=5.e-4;
      geo[ind++] = f_phi;
      geo[ind++] = f_sigma;
      geo[ind++] = b_phi;
      geo[ind++] = b_sigma;
      z = t.z;
      Xmax = Ymax = t.R;
    }else{
      CbmStsStation *st = StsDigi.GetStation(ist - NMvdStations);
  
      geo[ind++] = st->GetZ();
      geo[ind++] = st->GetD();
      geo[ind++] = st->GetRmin();
      geo[ind++] = st->GetRmax();
      geo[ind++] = st->GetRadLength();
  
      CbmStsSector* sector = st->GetSector(0);
      fscal f_phi, f_sigma, b_phi, b_sigma; // angle and sigma front/back  side
      f_phi = sector->GetRotation();
      b_phi = sector->GetRotation();
      if( sector->GetType()==1 ){
        b_phi += 3.14159265358/2.;
        b_sigma = sector->GetDy()/sqrt(12.);
        f_sigma = b_sigma; // CHECKME
      }
      else{
        f_phi +=sector->GetStereoF();
        b_phi +=sector->GetStereoB();
        f_sigma = sector->GetDx() / TMath::Sqrt(12);
        b_sigma  = f_sigma;
      }
      f_sigma *= cos(f_phi);  // TODO: think about this
      b_sigma *= cos(b_phi);
      geo[ind++] = f_phi;
      geo[ind++] = f_sigma;
      geo[ind++] = b_phi;
      geo[ind++] = b_sigma;
      z = st->GetZ();

      Xmax=-100; Ymax=-100;

      double x,y;
      for(int isec = 0; isec<st->GetNSectors(); isec++)
      {
        CbmStsSector *sect = L1_DYNAMIC_CAST<CbmStsSector*>( st->GetSector(isec) );
        for(int isen = 0; isen < sect->GetNSensors(); isen++)
        {
          x = sect->GetSensor(isen)->GetX0() + sect->GetSensor(isen)->GetLx()/2.;
          y = sect->GetSensor(isen)->GetY0() + sect->GetSensor(isen)->GetLy()/2.;
          if(x>Xmax) Xmax = x;
          if(y>Ymax) Ymax = y;
        }
      }
//      cout << "Station  "<<  ist << ",  Xmax  " << Xmax<<",  Ymax" << Ymax<<endl;
    }

    double dx = 1.; // step for the field approximation
    double dy = 1.;

    if( dx > Xmax/N/2 ) dx = Xmax/N/4.;
    if( dy > Ymax/N/2 ) dy = Ymax/N/4.;

    for( int i=0; i<3; i++)
      for( int k=0; k<N; k++) C[i][k] = 0;
    TMatrixD A(N,N);
    TVectorD b0(N), b1(N), b2(N);
    for( int i=0; i<N; i++){
      for( int j=0; j<N; j++) A(i,j) = 0.;
      b0(i)=b1(i)=b2(i) = 0.;
    }
    for( double x=-Xmax; x<=Xmax; x+=dx )
      for( double y=-Ymax; y<=Ymax; y+=dy )
    {
      double r = sqrt(fabs(x*x/Xmax/Xmax+y/Ymax*y/Ymax));
      if( r>1. ) continue;
      Double_t w = 1./(r*r+1);
      Double_t p[3] = { x, y, z};
      Double_t B[3] = {0.,0.,0.};
      CbmKF::Instance()->GetMagneticField()->GetFieldValue(p, B);
      TVectorD m(N);
      m(0)=1;
      for( int i=1; i<=M; i++){
        int k = (i-1)*(i)/2;
        int l = i*(i+1)/2;
        for( int j=0; j<i; j++ ) m(l+j) = x*m(k+j);
        m(l+i) = y*m(k+i-1);
      }
      
      TVectorD mt = m;
      for( int i=0; i<N; i++){
        for( int j=0; j<N;j++) A(i,j)+=w*m(i)*m(j);
        b0(i)+=w*B[0]*m(i);
        b1(i)+=w*B[1]*m(i);
        b2(i)+=w*B[2]*m(i);
      }
    }
    double det;
    A.Invert(&det);
    TVectorD c0 = A*b0, c1 = A*b1, c2 = A*b2;
    for(int i=0; i<N; i++){
      C[0][i] = c0(i);
      C[1][i] = c1(i);
      C[2][i] = c2(i);
    }

    geo[ind++] = N;
    for( int k=0; k<3; k++ ){
      for( int j=0; j<N; j++) geo[ind++] = C[k][j];
    }
  }

  geo[ind++] = fTrackingLevel;
  geo[ind++] = fMomentumCutOff;
  geo[ind++] = fGhostSuppression;

  {
    if(fSTAPDataMode%2 == 1){ // 1,3
      WriteSTAPGeoData( static_cast<void*>(geo), ind);
    };
    if(fSTAPDataMode >= 2){ // 2,3
      int ind2;
      ReadSTAPGeoData( static_cast<void*>(geo), ind2);
      if (ind2 != ind)  cout << "-E- CbmL1: Read geometry from file " << fSTAPDataDir + "geo_algo.txt was NOT successful." << endl;
    };
  }
  
  algo->Init(geo);
  return kSUCCESS;
}


void CbmL1::Exec(Option_t * option)
{
  static int nevent=0;
  if( fVerbose>1 ) cout << endl << "CbmL1::Exec event " << ++nevent << " ..." << endl << endl;

  { // save data for standalone
    if(fSTAPDataMode%2 == 1){ // 1,3
      WriteSTAPAlgoData();
      WriteSTAPPerfData();
    };
    if(fSTAPDataMode >= 2){  // 2,3
      ReadSTAPAlgoData();
      ReadSTAPPerfData();
    };
  }
  vRTracks.clear();
  multimap<Int_t,Int_t> mapHits;
  TVector3 pos;
  for(Int_t ihit=0;ihit<listStsHitsAll->GetEntriesFast();ihit++){
    CbmStsHit* hit = (CbmStsHit*) listStsHitsAll->At(ihit);
    Double_t t = hit->GetTimeStamp();
    hit->Position(pos);
    mapHits.insert(pair<Int_t,Int_t>(t-pos.Mag()/29.97,ihit));
  }

  // create slices
  Int_t nHits=0;
  vector<Int_t> sliceUpperEdge;
  Int_t last_time=-1000;
  Int_t nHits_slice = 0;
  for (multimap<Int_t,Int_t>::iterator it=mapHits.begin();it!=mapHits.end();it++){
    if (nHits!=0 && it->first>=last_time+fSliceSeperationTime) { 
      if (nHits_slice>7) sliceUpperEdge.push_back(nHits);
      else if (sliceUpperEdge.size()>0) sliceUpperEdge[sliceUpperEdge.size()-1]=nHits;
      nHits_slice=0;
    }
    last_time = it->first;
    nHits++;
    nHits_slice++;
  }
  sliceUpperEdge.push_back(nHits-1);
  printf("nHits=%i",nHits);
  printf("Slices: ");
  for (Int_t i=0;i<sliceUpperEdge.size();i++) printf(" %i",sliceUpperEdge[i]);
  printf("\n");
  
  nHits=0;
  listStsHits = sliceArray;
  listStsHits->Clear();
  sliceHitIndices.clear();
  Int_t sliceId=0;
  for (multimap<Int_t,Int_t>::iterator it=mapHits.begin();it!=mapHits.end();it++){
    CbmStsHit* hit = (CbmStsHit*) listStsHitsAll->At(it->second);
    sliceHitIndices.push_back(it->second);
    new ((*listStsHits)[listStsHits->GetEntriesFast()]) CbmStsHit(*hit);
    nHits++;
    if (nHits-1!=sliceUpperEdge[sliceId]) continue;
    // Start from standard L1
    // repack data
    printf("NewSlice hits: %i\n",listStsHits->GetEntriesFast());
    ReadEvent();
    
        // input performance
    if (fPerformance){
      InputPerformance();
    }
    // FieldApproxCheck();

    if( fVerbose>1 ) cout<<"L1 Track finder..."<<endl;
    algo->CATrackFinder();
    //   IdealTrackFinder();
    if( fVerbose>1 ) cout<<"L1 Track finder ok"<<endl;
    algo->L1KFTrackFitter();
    //  algo->KFTrackFitter_simple();
    if( fVerbose>1 ) cout<<"L1 Track fitter  ok"<<endl;

    // output performance
    if (fPerformance){
      if( fVerbose>1 ) cout<<"Performance..."<<endl;
      TrackMatch();
    }

    // save recontstructed tracks
    int start_hit = 0;
    for(vector<L1Track>::iterator it = algo->vTracks.begin(); it!=algo->vTracks.end(); it++){
      CbmL1Track t;
      for( int i=0; i<6; i++) t.T[i] = it->TFirst[i];
      for( int i=0; i<15; i++) t.C[i] = it->CFirst[i];
      for( int i=0; i<6; i++) t.TLast[i] = it->TLast[i];
      for( int i=0; i<15; i++) t.CLast[i] = it->CLast[i];
      t.chi2 = it->chi2;
      t.NDF = it->NDF;
      //t.T[4] = it->Momentum;
      for( int i=0; i<it->NHits; i++ ){
        t.StsHits.push_back(vHitStore[algo->vRecoHits[start_hit++]].ExtIndex);
      }
      t.mass = 0.1395679; // pion mass
      t.is_electron = 0;

      t.SetId(vRTracks.size());
      vRTracks.push_back(t);
    }

    
    // End from standard L1
    sliceId++;
    listStsHits->Clear();
    sliceHitIndices.clear();
//    break;
  }
  

  
  //Find Primary vertex, Ks, Lambdas,...
//  static CbmL1ParticlesFinder PF;

  if ( fFindParticlesMode ) {
      //Find Primary vertex, Ks, Lambdas,...
    PF->FindParticles(vRTracks);
    vRParticles = PF->GetParticles();
  }
  
  if (fPerformance){
    EfficienciesPerformance();
    HistoPerformance();
    TrackFitPerformance();

    if ( fFindParticlesMode ) {
      GetMCParticles();
      FindReconstructableMCParticles();
      MatchParticles();
      PartEffPerformance();
      PartHistoPerformance();
    }
  }
  if( fVerbose>1 ) cout<<"End of L1"<<endl;


  static bool ask = 0;
  char symbol;
  if (ask){
    std::cout << std::endl << "L1 run (any/r/q) > ";
    do{
      std::cin.get(symbol);
      if (symbol == 'r')
        ask = false;
      if ((symbol == 'e')||(symbol == 'q'))
        exit(0);
    } while (symbol != '\n');
  }
}

// -----   Finish CbmStsFitPerformanceTask task   -----------------------------
void CbmL1::Finish(){
  TDirectory *curr = gDirectory;
  TFile *currentFile = gFile;
  // Open output file and write histograms
  TFile* outfile = new TFile("L1_histo.root","RECREATE");
  outfile->cd();
  writedir2current(histodir);
  outfile->Close();
  outfile->Delete();
  gFile = currentFile;
  gDirectory = curr;
}


void CbmL1::writedir2current( TObject *obj ){
  if( !obj->IsFolder() ) obj->Write();
  else{
    TDirectory *cur = gDirectory;
    TDirectory *sub = cur->mkdir(obj->GetName());
    sub->cd();
    TList *listSub = (L1_DYNAMIC_CAST<TDirectory*>(obj))->GetList();
    TIter it(listSub);
    while( TObject *obj1=it() ) writedir2current(obj1);
    cur->cd();
  }
}

/// -----   Ideal Tracking   -----------------------------

void CbmL1::IdealTrackFinder()
{
  algo->vTracks.clear();
  algo->vRecoHits.clear();

  for( vector<CbmL1MCTrack>::iterator i = vMCTracks.begin(); i != vMCTracks.end(); ++i){
    CbmL1MCTrack &MC = *i;

    if (!MC.IsReconstructable()) continue;
    if (!(MC.ID >= 0)) continue;

    L1Track algoTr;
    algoTr.NHits = MC.StsHits.size();
    for (int iH = 0; iH < algoTr.NHits; iH++){
      algo->vRecoHits.push_back(MC.StsHits[iH]);
    }
    algoTr.Momentum = MC.p;
          
    algo->vTracks.push_back(algoTr);

  }
  
}; // void CbmL1::IdealTrackFinder()


/// -----   STandAlone Package service-functions  -----------------------------

void CbmL1::WriteSTAPGeoData(void* geo_, int size)
{
  fscal *geo = static_cast<fscal*>( geo_ );
    // write geo in file
  TString fgeo_name = fSTAPDataDir + "geo_algo.txt";
  ofstream fgeo(fgeo_name);
  fgeo.setf(ios::scientific,ios::floatfield);
  fgeo.precision(20);
  for (int i = 0; i < size; i++){
    fgeo << geo[i] << endl;
  };
  fgeo.close();
  cout << "-I- CbmL1: Geometry data has been written in " << fgeo_name << endl;
} // void CbmL1::WriteSTAPGeoData(void* geo_, int size)

void CbmL1::WriteSTAPAlgoData()  // must be called after ReadEvent
{
    // write algo data in file
  static int vNEvent = 1;
  fstream fadata;
  
  TString fadata_name = fSTAPDataDir + "data_algo.txt";
//    if ( vNEvent <= maxNEvent ) {
  if ( 1 ) {
 
    if (vNEvent == 1)
      fadata.open(fadata_name,fstream::out);  // begin new file
    else
      fadata.open(fadata_name,fstream::out | fstream::app);
        
    fadata << "Event:" << " ";
    fadata << vNEvent << endl;
      // write vStsStrips
    int n = algo->vStsStrips.size();  // number of elements
    fadata << n << endl;
    for (int i = 0; i < n; i++){
      fadata << algo->vStsStrips[i] << endl;
    };
    if (fVerbose >= 4) cout << "vStsStrips[" << n << "]" << " have been written." << endl;
      // write vStsStripsB
    n = algo->vStsStripsB.size();
    fadata << n << endl;
    for (int i = 0; i < n; i++){
      fadata << algo->vStsStripsB[i] << endl;
    };
    if (fVerbose >= 4) cout << "vStsStripsB[" << n << "]" << " have been written." << endl;
      // write vStsZPos
    n = algo->vStsZPos.size();
    fadata << n << endl;
    for (int i = 0; i < n; i++){
      fadata << algo->vStsZPos[i] << endl;
    };
    if (fVerbose >= 4) cout << "vStsZPos[" << n << "]" << " have been written." << endl;
      // write vSFlag
    n = algo->vSFlag.size();
    fadata << n << endl;
    unsigned char element;
    for (int i = 0; i < n; i++){
      element = algo->vSFlag[i];
      fadata << static_cast<int>(element) << endl;
    };
    if (fVerbose >= 4) cout << "vSFlag[" << n << "]" << " have been written." << endl;
      // write vSFlagB
    n = algo->vSFlagB.size();
    fadata << n << endl;
    for (int i = 0; i < n; i++){
      element = algo->vSFlagB[i];
      fadata << static_cast<int>(element) << endl;
    };
    if (fVerbose >= 4) cout << "vSFlagB[" << n << "]" << " have been written." << endl;
      // write vStsHits
    n = algo->vStsHits.size();
    fadata << n << endl;
    unsigned short int element2;
    char element3;
    for (int i = 0; i < n; i++){
      element2 = algo->vStsHits[i].f;
      fadata  << static_cast<int>(element2) << " ";
      element2 = algo->vStsHits[i].b;
      fadata  << static_cast<int>(element2) << " ";
      element3 = algo->vStsHits[i].iz;
      fadata  << static_cast<int>(element3) << endl;
    };
    if (fVerbose >= 4) cout << "vStsHits[" << n << "]" << " have been written." << endl;
      // write StsHitsStartIndex and StsHitsStopIndex
    n = 20;
    for (int i = 0; i < n; i++){
      if (algo->MaxNStations+1 > i) fadata  << algo->StsHitsStartIndex[i] << endl;
      else fadata  << 0 << endl;
    };
    for (int i = 0; i < n; i++){
      if (algo->MaxNStations+1 > i) fadata  << algo->StsHitsStopIndex[i] << endl;
      else fadata  << 0 << endl;
    };

    
    fadata.close();
  }
  cout << "-I- CbmL1: CATrackFinder data for event number " << vNEvent << " have been written in file " << fadata_name << endl;
  vNEvent++;
} // void CbmL1::WriteSTAPAlgoData() 

void CbmL1::WriteSTAPPerfData()  // must be called after ReadEvent
{
  fstream fpdata;

  static int vNEvent = 1;

  TString fpdata_name = fSTAPDataDir + "data_perfo.txt";
        // write data for performance in file
  //   if ( vNEvent <= maxNEvent )  {
  if ( 1 ) {

    if (vNEvent == 1)
      fpdata.open(fpdata_name,fstream::out);  // begin new file
    else
      fpdata.open(fpdata_name,fstream::out | fstream::app);
  
    fpdata << "Event: " ;
    fpdata << vNEvent << endl;
      // write vMCPoints
    Int_t n = vMCPoints.size();  // number of elements
    fpdata << n << endl;
    for (int i = 0; i < n; i++){
      fpdata << vMCPoints[i].x << " ";
      fpdata << vMCPoints[i].y << " ";
      fpdata << vMCPoints[i].z << "  ";
      fpdata << vMCPoints[i].px << " ";
      fpdata << vMCPoints[i].py << " ";
      fpdata << vMCPoints[i].pz << " ";
      fpdata << vMCPoints[i].p << "  ";
      fpdata << vMCPoints[i].q << " ";
      fpdata << vMCPoints[i].mass << "   ";
      
      fpdata << vMCPoints[i].pdg << " ";
      fpdata << vMCPoints[i].ID << " ";
      fpdata << vMCPoints[i].mother_ID << " ";
      fpdata << vMCPoints[i].iStation << endl;

      const int nhits = vMCPoints[i].hitIds.size();
      fpdata << nhits << endl << "   ";
      for (int k = 0; k < nhits; k++){
        fpdata << vMCPoints[i].hitIds[k] << " ";
      };
      fpdata << endl;
    };
    if (fVerbose >= 4) cout << "vMCPoints[" << n << "]" << " have been written." << endl;
    
          // write vMCTracks  . without Points
    n = vMCTracks.size();  // number of elements
    fpdata << n << endl;
    for (int i = 0; i < n; i++){
      fpdata << vMCTracks[i].x << " ";
      fpdata << vMCTracks[i].y << " ";
      fpdata << vMCTracks[i].z << "  ";
      fpdata << vMCTracks[i].px << " ";
      fpdata << vMCTracks[i].py << " ";
      fpdata << vMCTracks[i].pz << " ";
      fpdata << vMCTracks[i].p << "  ";
      fpdata << vMCTracks[i].q << " ";
      fpdata << vMCTracks[i].mass << "   ";
      
      fpdata << vMCTracks[i].pdg << " ";
      fpdata << vMCTracks[i].ID << " ";
      fpdata << vMCTracks[i].mother_ID << endl;
      
      int nhits = vMCTracks[i].StsHits.size();
      fpdata  << "   " << nhits << endl << "   ";
      for (int k = 0; k < nhits; k++){
        fpdata << vMCTracks[i].StsHits[k] << " ";
      };
      fpdata << endl;

      const int nPoints = vMCTracks[i].Points.size();
      fpdata << nPoints << endl << "   ";
      for (int k = 0; k < nPoints; k++){
        fpdata << vMCTracks[i].Points[k] << " ";
      };
      fpdata << endl;
      
      fpdata << vMCTracks[i].nMCContStations << " ";
      fpdata << vMCTracks[i].nHitContStations << " ";
      fpdata << vMCTracks[i].maxNStaMC << " ";
      fpdata << vMCTracks[i].maxNSensorMC << " ";
      fpdata << vMCTracks[i].maxNStaHits << " ";
      fpdata << vMCTracks[i].nStations << endl;
      
    };
    if (fVerbose >= 4) cout << "vMCTracks[" << n << "]" << " have been written." << endl;
    
        // write vHitMCRef
    n = vHitMCRef.size();  // number of elements
    fpdata << n << endl;
    for (int i = 0; i < n; i++){
      fpdata << vHitMCRef[i] << endl;
    };
    if (fVerbose >= 4) cout << "vHitMCRef[" << n << "]" << " have been written." << endl;
    
      // write vHitStore
    n = vHitStore.size();  // number of elements
    fpdata << n << endl;
    for (int i = 0; i < n; i++){
      fpdata << vHitStore[i].ExtIndex << "  ";
      fpdata << vHitStore[i].iStation << "  ";
      
      fpdata << vHitStore[i].x << " ";
      fpdata << vHitStore[i].y << endl;
    };
    if (fVerbose >= 4) cout << "vHitStore[" << n << "]" << " have been written." << endl;

          // write vStsHits
    n = vStsHits.size();  // number of elements
    fpdata << n << endl;
    for (int i = 0; i < n; i++){
      fpdata << vStsHits[i].hitId << " ";
      fpdata << vStsHits[i].extIndex << endl;

      const int nPoints = vStsHits[i].mcPointIds.size();
      fpdata << nPoints << endl << "   ";
      for (int k = 0; k < nPoints; k++){
        fpdata << vStsHits[i].mcPointIds[k] << " ";
      };
      fpdata << endl;
    };
    if (fVerbose >= 4) cout << "vStsHits[" << n << "]" << " have been written." << endl;
    
    fpdata.close();
  }
  cout << "-I- CbmL1: Data for performance of event number " << vNEvent << " have been written in file " << fpdata_name << endl;
  vNEvent++;
} // void CbmL1::WriteSTAPPerfData()

istream& CbmL1::eatwhite(istream& is) // skip spaces
{
  char c;
  while (is.get(c)) {
    if (isspace(c)==0) {
      is.putback(c);
      break;
    }
  }
  return is;
}

void CbmL1::ReadSTAPGeoData(void* geo_, int &size)
{
  fscal *geo = static_cast<fscal*>( geo_ );
  TString fgeo_name = fSTAPDataDir + "geo_algo.txt";
  ifstream fgeo(fgeo_name);

  cout << "-I- CbmL1: Read geometry from file " << fgeo_name << endl;
  int i;
  for (i = 0; !fgeo.eof(); i++){
    fscal tmp;
    fgeo >> tmp >> eatwhite;
    geo[i] = tmp;
  };
  size = i;
  fgeo.close();
} // void CbmL1::ReadSTAPGeoData(void* geo_, int &size)

void CbmL1::ReadSTAPAlgoData()
{
  static int nEvent = 1;
  static fstream fadata;
  TString fadata_name = fSTAPDataDir + "data_algo.txt";
//  if (nEvent <= maxNEvent){
  if (1){
    if ( nEvent == 1 ){
      fadata.open(fadata_name,fstream::in);
    };
    
    algo->vStsHits.clear();
    algo->vStsStrips.clear();
    algo->vStsStripsB.clear();
    algo->vStsZPos.clear();
    algo->vSFlag.clear();
    algo->vSFlagB.clear();
    
      // check correct position in file
    char s[] = "Event:  ";
    int nEv;
    fadata >> s;
    fadata >> nEv;
    if (nEv != nEvent)  cout << "-E- CbmL1: Can't read event number " << nEvent << " from file " << fadata_name << endl;
    
    int n;  // number of elements
      // read algo->vStsStrips
    fadata >> n;
    for (int i = 0; i < n; i++){
      fscal element;
      fadata >> element;
      algo->vStsStrips.push_back(element);
    };
    if (fVerbose >= 4) cout << "vStsStrips[" << n << "]" << " have been read." << endl;
      // read algo->vStsStripsB
    fadata >> n;
    for (int i = 0; i < n; i++){
      fscal element;
      fadata >> element;
      algo->vStsStripsB.push_back(element);
    };
    if (fVerbose >= 4) cout << "vStsStripsB[" << n << "]" << " have been read." << endl;
      // read algo->vStsZPos
    fadata >> n;
    for (int i = 0; i < n; i++){
      fscal element;
      fadata >> element;
      algo->vStsZPos.push_back(element);
    };
    if (fVerbose >= 4) cout << "vStsZPos[" << n << "]" << " have been read." << endl;
      // read algo->vSFlag
    fadata >> n;
    for (int i = 0; i < n; i++){
      int element;
      fadata >> element;
      algo->vSFlag.push_back(static_cast<unsigned char>(element));
    };
    if (fVerbose >= 4) cout << "vSFlag[" << n << "]" << " have been read." << endl;
      // read algo->vSFlagB
    fadata >> n;
    for (int i = 0; i < n; i++){
      int element;
      fadata >> element;
      algo->vSFlagB.push_back(static_cast<unsigned char>(element));
    };
    if (fVerbose >= 4) cout << "vSFlagB[" << n << "]" << " have been read." << endl;
      // read algo->vStsHits
    fadata >> n;
    int element_f;  // for convert
    int element_b;
    int element_iz;
    for (int i = 0; i < n; i++){
      L1StsHit element;
      fadata >> element_f >> element_b >> element_iz;
      element.f = static_cast<THitI>(element_f);
      element.b = static_cast<THitI>(element_b);
      element.iz = static_cast<TZPosI>(element_iz);
      algo->vStsHits.push_back(element);
    };
    if (fVerbose >= 4) cout << "vStsHits[" << n << "]" << " have been read." << endl;
      // read StsHitsStartIndex and StsHitsStopIndex
    n = 20;
    for (int i = 0; i < n; i++){
      int tmp;
      fadata >> tmp;
      if (algo->MaxNStations+1 > i) algo->StsHitsStartIndex[i] = tmp;
    };
    for (int i = 0; i < n; i++){
      int tmp;
      fadata >> tmp;
      if (algo->MaxNStations+1 > i) algo->StsHitsStopIndex[i] = tmp;
    };

    cout << "-I- CbmL1: CATrackFinder data for event " << nEvent << " has been read from file " << fadata_name << " successfully." << endl;
//    if (nEvent == maxNEvent) fadata.close();  
  }
  nEvent++;
}; // void CbmL1::ReadSTAPAlgoData()

void CbmL1::ReadSTAPPerfData()
{
  static int nEvent = 1;
  static fstream fpdata;
  TString fpdata_name = fSTAPDataDir + "data_perfo.txt";
//  if (nEvent <= maxNEvent){
  if (1){
    if ( nEvent == 1 ){
      fpdata.open(fpdata_name,fstream::in);
    };

    vMCPoints.clear();
    vMCTracks.clear();
    vHitMCRef.clear();
    vHitStore.clear();
    vStsHits.clear();

      // check if it is right position in file
    char s[] = "EVENT:     ";  // buffer
    int nEv=0;                // event number
    fpdata >> s;
    fpdata >> nEv;

    if (nEv != nEvent)  cout << "-E- CbmL1: Performance: can't read event number " << nEvent << " from file " << "data_perfo.txt" << endl;
      // vMCPoints
    int n;  // number of elements
    fpdata >> n;
    for (int i = 0; i < n; i++){
      CbmL1MCPoint element;
      
      fpdata >> element.x;
      fpdata >> element.y;
      fpdata >> element.z;
      fpdata >> element.px;
      fpdata >> element.py;
      fpdata >> element.pz;
      fpdata >> element.p;
      fpdata >> element.q;
      fpdata >> element.mass;
      
      fpdata >> element.pdg;
      fpdata >> element.ID;
      fpdata >> element.mother_ID;
      fpdata >> element.iStation;

      int nhits;
      fpdata >> nhits;
      for (int k = 0; k < nhits; k++){
        int helement;
        fpdata >> helement;
        element.hitIds.push_back(helement);
      };
      
      vMCPoints.push_back(element);
    };
    if (fVerbose >= 4) cout << "vMCPoints[" << n << "]" << " have been read." << endl;

      // vMCTracks . without Points
    fpdata >> n;
    for (int i = 0; i < n; i++){
      CbmL1MCTrack element;

      fpdata >> element.x;
      fpdata >> element.y;
      fpdata >> element.z;
      fpdata >> element.px;
      fpdata >> element.py;
      fpdata >> element.pz;
      fpdata >> element.p;
      fpdata >> element.q;
      fpdata >> element.mass;

      fpdata >> element.pdg;
      fpdata >> element.ID;
      fpdata >> element.mother_ID;

      int nhits;
      fpdata >> nhits;
      for (int k = 0; k < nhits; k++){
        int helement;
        fpdata >> helement;
        element.StsHits.push_back(helement);
      };
      fpdata >> nhits;
      for (int k = 0; k < nhits; k++){
        int helement;
        fpdata >> helement;
        element.Points.push_back(helement);
      };

      fpdata >> element.nMCContStations;
      fpdata >> element.nHitContStations;
      fpdata >> element.maxNStaMC;
      fpdata >> element.maxNSensorMC;
      fpdata >> element.maxNStaHits;
      fpdata >> element.nStations;

      element.CalculateIsReconstructable();
      vMCTracks.push_back(element);
    };
    if (fVerbose >= 4) cout << "vMCTracks[" << n << "]" << " have been read." << endl;
    
        // vHitMCRef
    fpdata >> n;
    for (int i = 0; i < n; i++){
      int element;
      fpdata >> element;
      vHitMCRef.push_back(element);
    };
    if (fVerbose >= 4) cout << "vHitMCRef[" << n << "]" << " have been read." << endl;

      // vHitStore
    fpdata >> n;
    for (int i = 0; i < n; i++){
      CbmL1HitStore element;
      fpdata >> element.ExtIndex;
      fpdata >> element.iStation;

      fpdata >> element.x;
      fpdata >> element.y;
      vHitStore.push_back(element);
    };
    if (fVerbose >= 4) cout << "vHitStore[" << n << "]" << " have been read." << endl;
    
          // vStsHits
    fpdata >> n;
    for (int i = 0; i < n; i++){
      CbmL1StsHit element;
      fpdata >> element.hitId;
      fpdata >> element.extIndex;

      int nPoints;
      fpdata >> nPoints;
      for (int k = 0; k < nPoints; k++){
        int id;
        fpdata >> id;
        element.mcPointIds.push_back(id);
      };
      vStsHits.push_back(element);
    };
    if (fVerbose >= 4) cout << "vStsHits[" << n << "]" << " have been read." << endl;
    

    
//    if (nEvent == maxNEvent) { // file open on begin of all work class and close at end
//       fpdata.close();
//       cout << " -I- Performance: data read from file " << "data_perfo.txt" << " successfully"<< endl;
//     }
    cout << "-I- CbmL1: L1Performance data for event " << nEvent << " has been read from file " << fpdata_name << " successfully." << endl;

  } // if (nEvent <= maxNEvent)
  nEvent++;
}; // void CbmL1::ReadSTAPPerfData()

