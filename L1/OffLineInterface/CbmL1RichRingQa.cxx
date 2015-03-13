#include "CbmL1RichRingQa.h"

#include "CbmL1Def.h"

#include "FairTask.h"
#include "FairRootManager.h"
#include "CbmMCTrack.h"
#include "CbmRichPoint.h"
#include "CbmRichHit.h"
#include "CbmRichRing.h"
#include "CbmMCTrack.h"
#include "CbmRichRingLight.h"
#include "CbmRichRingFitterEllipseTau.h"

#include "TH1.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TH2.h"
#include "TProfile.h"
#include "TFile.h"
#include "TClonesArray.h"
#include "TROOT.h"
#include "TArc.h"
#include "TLatex.h"
#include "TText.h"
#include "TEllipse.h"
#include "TStyle.h"


#include <iostream>
#include <list>
#include <vector>
#include <map>
#include <cmath>

#include <sstream>
#include <ctype.h>
#include <stdio.h>

using namespace std;

using std::cout;
using std::endl;
using std::ios;
using std::list;
using std::pair;
using std::vector;
using std::map;
using std::fabs;

ClassImp(CbmL1RichRingQa)

#define DRAW

//------------  standard constructor (with verbosity level)  ---------------------------------
CbmL1RichRingQa::CbmL1RichRingQa(const char *name, const char *title, Int_t verbose)
  :FairTask(name),
  fRingArray(0),        // Array of CbmRichRings
  fMCPointArray(0),       // Array of FairMCPoints
  fMCTrackArray(0),       // Array of CbmMCTracks
  fHitArray(0),         // Array of CbmRichHits

  Chi2Ghost(0),
  Chi2Ref(0),
  Chi2All(0),
  Chi2Clone(0),
  Chi2NhitsGhost(0),
  Chi2NhitsAll(0),
  RGhost(0),
  REl(0),
  RPi(0),
  NHitsMC(0),
  NSameHits(0),

    //Chi2NhitsGhost(0),
  Chi2NhitsPi(0),
  Chi2NhitsEll(0),
  RNhitsGhost(0),
  RNhitsPi(0),
  RNhitsEll(0),
  RChi2Ghost(0),
  RChi2Pi(0),
  RChi2Ell(0),
  NSameHitsVsP(0),
  NHitsVsMCP(0),
  RadiusVsPForClone(0),
  DistanceVsPClone(0),
  Chi2VsPClone(0),
  RadiusVsDistanceClone(0),
  NHitsRecoVsNHitsMC(0)

  
{
  Chi2Ghost = new TH1F("Chi2 for Ghost", "Chi2 for Ghost", 100, 0.f , 1.f);
  Chi2Ghost->SetXTitle("Chi2 Ghost");
  Chi2Ghost->SetYTitle("Enteric");

  Chi2Ref = new TH1F("Ref Chi2", "Ref Chi2", 100, 0.f , 1.f);
  Chi2Ref->SetXTitle("Chi2 Ref");
  Chi2Ref->SetYTitle("Enteric");

  Chi2All = new TH1F("Chi2 for reconstructed rings", "Chi2 for reconstructed rings", 100, 0.f , 1.f);
  Chi2All->SetXTitle("Chi2 reco rings");
  Chi2All->SetYTitle("Enteric");

  Chi2Clone = new TH1F("Clone Chi2", "Clone Chi2", 100, 0.f , 1.f);
  Chi2Clone->SetXTitle("Chi2 Clone");
  Chi2Clone->SetYTitle("Enteric");

  Chi2NhitsGhost = new TH2F("Chi2 Ghost vs Nhits", "Chi2 vs Nhits", 30, 4.5f, 34.5f,100, 0.f, 1.f);
  Chi2NhitsGhost->SetXTitle("Ghost Nhits");
  Chi2NhitsGhost->SetYTitle("Ghost Chi2");

  Chi2NhitsAll = new TH2F("Chi2 All vs Nhits", "Chi2 vs Nhits", 30, 4.5f, 34.5f,100, 0.f, 1.f);
  Chi2NhitsAll->SetXTitle("Nhits All");
  Chi2NhitsAll->SetYTitle("Chi2 All");

  REl = new TH1F("REl", "REl", 100, 0.f , 7.f);
  RPi = new TH1F("RPi", "RPi", 100, 0.f , 7.f);
  RGhost = new TH1F("RGhost", "RGhost", 100, 0.f , 7.f);
//  Chi2NhitsGhost = new TH2F("Chi2 Ghost vs Nhits", "Chi2 Ghost vs Nhits", 30, 4.5f, 34.5f,100, 0.f, 1.f);
  Chi2NhitsPi = new TH2F("Chi2 Pi vs Nhits", "Chi2 Pi vs Nhits", 30, 4.5f, 34.5f,100, 0.f, 1.f);
  Chi2NhitsPi->SetXTitle("Nhits Pi");
  Chi2NhitsPi->SetYTitle("Chi2 Pi");

  Chi2NhitsEll = new TH2F("Chi2 Ell vs Nhits", "Chi2 Ell vs Nhits", 30, 4.5f, 34.5f,100, 0.f, 1.f);
  Chi2NhitsEll->SetXTitle("Nhits Ell");
  Chi2NhitsEll->SetYTitle("Chi2 Ell");

  RNhitsGhost = new TH2F("R Ghost vs Nhits", "R Ghost vs Nhits", 30, 4.5f, 34.5f,100, 2.f, 8.f);
  RNhitsGhost->SetXTitle("Nhits Ghost");
  RNhitsGhost->SetYTitle("R Ghost");

  RNhitsPi = new TH2F("R Pi vs Nhits", "R Pi vs Nhits", 30, 4.5f, 34.5f,100, 2.f, 8.f);
  RNhitsPi->SetXTitle("Nhits Pi");
  RNhitsPi->SetYTitle("R Pi");

  RNhitsEll = new TH2F("R Ell vs Nhits", "R Ell vs Nhits", 30, 4.5f, 34.5f,100, 2.f, 8.f);
  RNhitsEll->SetXTitle("Nhits electrons");
  RNhitsEll->SetYTitle("R electrons");

  RChi2Ghost = new TH2F("R Ghost vs Chi2", "R Ghost vs Chi2",  100, 0.f, 1.f,100, 2.f, 8.f);
  RChi2Ghost->SetXTitle("Chi2 Ghost");
  RChi2Ghost->SetYTitle("R Ghost");

  RChi2Pi = new TH2F("R Pi vs Chi2", "R Pi vs Chi2",  100, 0.f, 1.f,100, 2.f, 8.f);
  RChi2Pi->SetXTitle("Chi2 Pi");
  RChi2Pi->SetYTitle("R Pi");

  RChi2Ell = new TH2F("R Ell vs Chi2", "R Ell vs Chi2", 100, 0.f, 1.f,100, 2.f, 8.f);
  RChi2Ell->SetXTitle("Chi2 Electrons");
  RChi2Ell->SetYTitle("R Electrons");

  NHitsMC = new TH1F("MC Hits %","MC Hits %", 100, 0.f, 1.f);
  NHitsMC->SetXTitle("MC hits found, %");
  NHitsMC->SetYTitle("Entries");

  NSameHits = new TH1F ("Same Hits","Same Hits", 100, 0.f, 30.f);
  NSameHits->SetXTitle("N Same Hits");
  NSameHits->SetYTitle("Enteric");

  NSameHitsVsP = new TH2F ("MC Hits % vs P(MC)", "MC Hits % vs P(MC)",  100, 0.f, 10.f,100, 0.f, 1.f);
  NSameHitsVsP->SetXTitle("P MC");
  NSameHitsVsP->SetYTitle("MC Hits, %");

  NHitsVsMCP = new TH2F ("Same Hits vs P(MC)", "Same Hits vs P(MC)",  100, 0.f, 20.f,100, 0.f, 30.f);
  NHitsVsMCP->SetXTitle("P MC");
  NHitsVsMCP->SetYTitle("Same Hits");

  RadiusVsPForClone = new TH2F (" Radius Vs P ", " Radius Vs P ",  100, 0.f, 12.f, 100 , 0.f, 3.f);
  RadiusVsPForClone->SetXTitle("P Clone");
  RadiusVsPForClone->SetYTitle("R1-R2");

  DistanceVsPClone = new TH2F (" Distance Vs P ", " Distance Vs P ",  100, 0.f, 12.f, 100 , 0.f, 5.f);
  DistanceVsPClone->SetXTitle("P Clone");
  DistanceVsPClone->SetYTitle("Distance");

  Chi2VsPClone = new TH2F (" Chi2 Vs P ", " Chi2 Vs P ",  100, 0.f, 12.f, 100 , 0.f, 1.f);
  Chi2VsPClone->SetXTitle("P Clone");
  Chi2VsPClone->SetYTitle("Chi2");

  RadiusVsDistanceClone = new TH2F (" Radius Vs Distance ", " Radius Vs Distance ",  100, 0.f, 5.f, 100 , 0.f, 3.f);
  RadiusVsDistanceClone->SetXTitle("Distance");
  RadiusVsDistanceClone->SetYTitle("R1-R2");

  NHitsRecoVsNHitsMC = new TH2F (" N Hits Reco Vs N Hits MC ", " N Hits Reco Vs N Hits MC ",  100, 0.f, 35.f, 100 , 0.f, 35.f);
  NHitsRecoVsNHitsMC->SetXTitle("N Hits MC");
  NHitsRecoVsNHitsMC->SetYTitle("N Hits Reco");

  fVerbose = verbose;

}

  // -----   Destructor   ----------------------------------------------------
CbmL1RichRingQa::~CbmL1RichRingQa()
{ 

}

InitStatus CbmL1RichRingQa::Init()
{

 // Get and check FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if (! ioman) {
    cout << "-E- CbmL1RichRingQa::Init: "
	 << "RootManager not instantised!" << endl;
    return kERROR;
  }

  // Get hit Array
  fHitArray = L1_DYNAMIC_CAST<TClonesArray*>( ioman->GetObject("RichHit") );
  if ( ! fHitArray) {
    cout << "-W- CbmL1RichRingQa::Init: No RichHit array!"
	 << endl;
  }

  // Get RichRing Array
  fRingArray = L1_DYNAMIC_CAST<TClonesArray*>( ioman->GetObject("RichRing") );
  if ( ! fRingArray ) {
    cout << "-E- CbmL1RichRingQa::Init: No RichRing array!" << endl;
    return kERROR;
  }

  // Get MC Point array
  fMCPointArray = L1_DYNAMIC_CAST<TClonesArray*>( ioman->GetObject("RichPoint") );
  if ( ! fMCPointArray ) {
    cout << "-E- CbmL1RichRingQa::Init: No RichPoint array!" << endl;
    return kERROR;
  }

  // Get MC Point array
  fMCTrackArray = L1_DYNAMIC_CAST<TClonesArray*>( ioman->GetObject("MCTrack") );
  if ( ! fMCTrackArray ) {
    cout << "-E- CbmL1RichRingQa::Init: No MCTrack array!" << endl;
    return kERROR;
  }

  return kSUCCESS;
}

void CbmL1RichRingQa::CirFit( list<pair<Double_t,Double_t> > &P, Double_t *X, Double_t *Y, Double_t *R )
{
  Double_t S[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  Int_t n=0;
  for( list<pair<Double_t,Double_t> >::iterator i=P.begin(); i!=P.end(); ++i )
    {
      Double_t &x = i->first;
      Double_t &y = i->second;
      Double_t r2 = x*x+y*y;
      S[0] +=  x * x;
      S[1] +=  y * y;
      S[2] +=  x * y;
      S[3] +=  x * r2 ;
      S[4] += y * r2 ;
      S[5] += x;
      S[6] += y;
      S[7] += r2;
      n++;
    }
  Double_t s0 = S[6]*S[0]-S[2]*S[5];
  Double_t s1 = S[0]*S[1]-S[2]*S[2];
  Double_t s2 = S[0]*S[4]-S[2]*S[3];
  *X = *Y = *R = 0.;
  if( fabs(s0)<1.E-6 || fabs(s1)<1.E-6 ) return;
  Double_t tmp = s1*(S[5]*S[5]-n*S[0])+s0*s0;
  Double_t A = ( ( S[0]*S[7]-S[3]*S[5] )*s1-s2*s0 ) / tmp;
  *Y = (s2 + s0*A )/s1/2;
  *X = ( S[3] + S[5]*A - S[2]*(*Y)*2 )/S[0]/2;
  Double_t R2 = (*X)*(*X)+(*Y)*(*Y)-A;
  if( R2<0 ) return;
  *R = sqrt( R2 );
}

void CbmL1RichRingQa::Exec(Option_t* option)
{
  // histogramms
//anagai
    map < void*, int > pHitIndex;
    map < Int_t, MCRing> MCRingMap;
  
    Int_t NRecoEx = 0, NRecoG = 0, NGostmy = 0, NMC=0, NMC37=0;

    static Int_t niv=0;
    Double_t ArrRingX[fRingArray->GetEntriesFast()];
    Double_t ArrRingY[fRingArray->GetEntriesFast()];
    Double_t ArrRingR[fRingArray->GetEntriesFast()];

    Int_t my_NHits[fRingArray->GetEntriesFast()];


    Double_t HitArrX[fHitArray->GetEntriesFast()];
    Double_t HitArrY[fHitArray->GetEntriesFast()];
    Double_t NoiseHitArrX[fHitArray->GetEntriesFast()];
    Double_t NoiseHitArrY[fHitArray->GetEntriesFast()];

    Int_t NpiMC=0, NeMC=0;
    niv++;
    std::string s;
    std::stringstream out;
    out << niv;
    s = out.str();

#ifdef DRAW
    TCanvas* Up = new TCanvas("Up","Up",0,0,240,180);
    Up->SetFillColor(kWhite);

    Up->Divide(1,2);
    Up->cd(1);
    Up->Range(-110,110,110,180);
    gPad->DrawFrame(-110,110,110,180);
    Up->cd(2);
    Up->Range(-110,-180,110,-110);
    gPad->DrawFrame(-110,-180,110,-110);
#endif //DRAW
    Int_t NfakeHits = 0;
  for( Int_t hit = 0 ; hit<fHitArray->GetEntriesFast(); hit++ ){

    CbmRichHit * phit = L1_DYNAMIC_CAST<CbmRichHit*>( fHitArray->At( hit ) );
    HitArrX[hit] = (phit->GetX());
    HitArrY[hit] = -1*(phit->GetY());
    if ( phit->GetRefId() == -1 )
	{
	NoiseHitArrX[NfakeHits] = (phit->GetX());
	NoiseHitArrY[NfakeHits] = -1*(phit->GetY());
	NfakeHits++;
	}
  }
  for( Int_t i = 0; i<fRingArray->GetEntriesFast(); i++ ){
    CbmRichRing *ring = L1_DYNAMIC_CAST<CbmRichRing*>( fRingArray->At( i ) );
    //CbmRichRingLight *my_ring = (CbmRichRingLight*) fRingArray->At( i );    
    ArrRingX[i] = ring->GetCenterX();
    ArrRingY[i] = -1*(ring->GetCenterY());
    ArrRingR[i] = ring->GetRadius();
    my_NHits[i] = 0;
    my_NHits[i] = ring->GetNofHits();
    TString st = "";
    st += my_NHits[i];
    NGostmy++;
#ifdef DRAW
    //Draw All Rings
    Up->cd(1);
    TArc* AllRingsUp = new TArc ( ArrRingX[i] , ArrRingY[i] , ArrRingR[i] , 0 , 360);
    AllRingsUp->SetLineWidth(1);
    AllRingsUp->SetLineColor(1);
    AllRingsUp->SetFillStyle(0);
    AllRingsUp->Draw("*");
    // Draw N Hits for rings 
    /*
    TText* textn = new TText( ArrRingX[i] - ArrRingR[i]/5 , ArrRingY[i] + ArrRingR[i]/5 , st.Data() );
    textn->SetTextAlign(12);
    textn->SetTextSize(0.01);
    textn->Draw();
    */
    Up->cd(2);
    TArc* AllRingsDown = new TArc ( ArrRingX[i] , ArrRingY[i] , ArrRingR[i] , 0 , 360);
    AllRingsDown->SetLineWidth(1);
    AllRingsDown->SetLineColor(1);
    AllRingsDown->SetFillStyle(0);
    AllRingsDown->Draw("*");
    // Draw N Hits for rings 
    /*
    TText* text1 = new TText( ArrRingX[i] - ArrRingR[i]/5 , ArrRingY[i] + ArrRingR[i]/5  , st.Data() ); 
    text1->SetTextAlign(12);
    text1->SetTextSize(0.01);
    text1->Draw();
*/
#endif // DRAW
  }

//anagai

  static TH1F
    *h_MC_radius,
    *h_MC_nhits,
    *h_MC_primary_nhits,
    *h_MC_momentum,
    *h_MC_primary_momentum,
    *h_MC_resolution,
    *h_MC_ref_resolution,
    *h_MC_extra_resolution,
    *h_ghost_nhits;

  static TH2F
    *h_MC_primary_res_vs_momentum;

  static TProfile
    *p_ref_eff_vs_nhits,
    *p_extra_eff_vs_nhits;

  static TList *listHisto;
  static bool first_call_performance = 1;

  if ( first_call_performance )
    {
      first_call_performance = 0;
      TDirectory *curdir = gDirectory;
      TDirectory *histodir = gROOT->mkdir("CbmL1RichRingQaHisto");
      histodir->cd();

      h_MC_radius = new TH1F("h_MC_radius", "MC ring radius (cm)", 100, 0.0, 10.);
      h_MC_nhits  = new TH1F("h_MC_nhits", "Hits per MC ring", 50, 0.0, 50.);
      h_MC_primary_nhits  = new TH1F("h_MC_primary_nhits", "Hits per primary MC ring", 50, 0.0, 50.);
      h_MC_momentum = new TH1F("h_MC_momentum", "MC track momentum (GeV)", 100, 0.0, 15.);
      h_MC_primary_momentum = new TH1F("h_MC_primary_momentum", "MC primary track momentum (GeV)", 100, 0.0, 15.);
      h_MC_resolution =
	new TH1F("h_MC_resolution", "Hit deviation from MC ring (cm)", 500, -5.0, 5.0);
      h_MC_ref_resolution =
	new TH1F("h_MC_ref_resolution", "Hit deviation from REF MC ring (cm)", 500, -5.0, 5.0);
      h_MC_extra_resolution =
	new TH1F("h_MC_extra_resolution", "Hit deviation from EXTRA MC ring (cm)", 500, -5.0, 5.0);

      h_ghost_nhits = new TH1F("h_ghost_nhits", "Hits per ghost ring", 50, 0.0, 50.);

      h_MC_primary_res_vs_momentum =
	new TH2F("h_MC_primary_res_vs_momentum", "Hit deviation from ptimary MC ring (cm) vs P",
		  100, 0., 15., 500, -5.0, 5.0 );

      p_ref_eff_vs_nhits =
	new TProfile("p_ref_eff_vs_nhits", "Refset efficiency vs N Hits", 100, 0.0, 50.0, 0.0, 1.0 );

      p_extra_eff_vs_nhits =
	new TProfile("p_extra_eff_vs_nhits", "Extraset efficiency vs N Hits", 100, 0.0, 50.0, 0.0, 1.0 );

      // ----- Create list of all histogram pointers
      listHisto = gDirectory->GetList();

      curdir->cd();
    }

  // Create hit vector

  if( !fHitArray || !fMCTrackArray || !fMCPointArray || !fRingArray ) return;

  int NHits = fHitArray->GetEntriesFast();
  PerfHit Hits[NHits];

//  map < void*, int > pHitIndex;

  for( Int_t i = 0; i<NHits; i++ ){

    PerfHit &hit = Hits[i];
    hit.index = i;
    hit.x = 0;
    hit.y = 0;
    hit.MCTrackID = - 1;

    CbmRichHit * phit = L1_DYNAMIC_CAST<CbmRichHit*>( fHitArray->At( i ) );
    if ( !phit ) continue;
    pHitIndex.insert(pair<void*, int>(phit,i));
    hit.x = phit->GetX();
    hit.y = phit->GetY();
    Int_t pointID = phit->GetRefId();
    if (pointID < 0) continue;
    CbmRichPoint* point = L1_DYNAMIC_CAST<CbmRichPoint*>( fMCPointArray->At(pointID) );
    if ( !point ) continue;
    Int_t trackID = point->GetTrackID();
    if( trackID< 0 ) continue;
    CbmMCTrack *track= L1_DYNAMIC_CAST<CbmMCTrack*>( fMCTrackArray->At(trackID) );
    if( !track || track->GetPdgCode() != 50000050 ) continue; // select only Cherenkov photons
    hit.MCTrackID = track->GetMotherId();
  }

  // Create map of MC rings

  //map < Int_t, MCRing> MCRingMap;

  // match hits

  for( int ih = 0; ih<NHits; ih++ ){
    int ID =  Hits[ih].MCTrackID;
    if ( ID>=0 && MCRingMap.find( ID )==MCRingMap.end() )
      {
	MCRing tmp;
	tmp.NHits = 0;
	MCRingMap.insert(pair<Int_t, MCRing>( ID, tmp));
      }
    MCRingMap[ID].NHits++;
    MCRingMap[ID].Hits.push_back(ih);
    MCRingMap[ID].NHitsBestReco = 0;
    MCRingMap[ID].BestReco = 0;
    MCRingMap[ID].NHitsBestvsNHitsMC = 0.;
  }

  // fit MC rings & set parameters
	Int_t NMCRings = 0; 
 for( map<Int_t,MCRing>::iterator i=MCRingMap.begin(); i!=MCRingMap.end(); ++i )
	{
		NMCRings++;
	}

 TH2F *h_e = new TH2F("MC ring radius (cm) vs MC track momentum (GeV)", "", 100, 0.f, 12.f, 100, 0.f, 6.f );
 h_e->SetXTitle("MC track momentum (GeV)");
 h_e->SetYTitle("MC ring R1/R2");

  for( map<Int_t,MCRing>::iterator i=MCRingMap.begin(); i!=MCRingMap.end(); ++i )
    {
     vector<Double_t> hitX;
     vector<Double_t> hitY;
//     CbmRichRing *Ellipse = new CbmRichRing();

      MCRing &ring = i->second;
      ring.MCTrackID = i->first;
      ring.primary = 0;
      ring.P = 0;
      ring.PDG = 0;
      ring.Reconstructed = 0;
      ring.kind = 0;
      ring.k = 0;

      // find momentum 
      if( !fMCTrackArray || ring.MCTrackID < 0 ) continue;

      CbmMCTrack *pm= L1_DYNAMIC_CAST<CbmMCTrack*>( fMCTrackArray->At( ring.MCTrackID ) );
      if ( pm ){
	ring.PDG = pm->GetPdgCode();  // get PDG code of mother

	Double_t vx = pm->GetStartX();
	Double_t vy = pm->GetStartY();
	Double_t vz = pm->GetStartZ();

	if (fabs(vx)<0.1 && fabs(vy)<0.1 && fabs(vz)<0.1) ring.primary = 1;

	ring.P = pm->GetP();
      }

      // fit the MC ring

      list<pair<Double_t,Double_t> > L;
      Int_t in=0;
      for( vector<int>::iterator j=ring.Hits.begin(); j!=ring.Hits.end(); ++j ){
	L.push_back(pair<Double_t,Double_t>( Hits[(*j)].x, Hits[(*j)].y) );
	hitX.push_back(Hits[(*j)].x);
	hitY.push_back(Hits[(*j)].y);
        in++;
      }
      CirFit( L, &ring.x, &ring.y, &ring.r );
#ifdef DRAW
       // Use Fitter EllipseTau to fit MC Rings
/*       CbmRichRingFitterEllipseTau *FitterEllipse = new CbmRichRingFitterEllipseTau();
       FitterEllipse -> DoFit1(Ellipse, hitX , hitY );
       Double_t x0, y0, d, delta ,A=Ellipse->GetAPar()
				,B=Ellipse->GetBPar()
				,C=Ellipse->GetCPar()
				,D=Ellipse->GetDPar()
				,E=Ellipse->GetEPar()
				,F=Ellipse->GetFPar()
				,l1,l2, r1, r2, theta;
        d = A*C-0.25*B*B;
        x0 = -(0.5*D*C-0.25*B*E)/d;
        y0 = -(0.5*A*E-0.25*B*D)/d;
        delta = A*(C*F-0.25*E*E) - 0.5*B*(0.5*B*F-0.25*E*D) + 0.5*D*(0.25*B*E-0.5*D*C);
        l1 = 0.5*(A+C-sqrt((A+C)*(A+C)-4*A*C+B*B));
        l2 = 0.5*(A+C+sqrt((A+C)*(A+C)-4*A*C+B*B));
        r1 = sqrt(-delta/(l2*d));
        r2 = sqrt(-delta/(l1*d));
        theta = -(90*atan(B/(A-C)))/3.1415;
        Up->cd(1);
        TEllipse *el1 = new TEllipse( x0 , -y0 , r1 , r2, 0, 360, theta );
        el1->SetFillStyle(0);
        el1->SetLineColor(2);
        el1->Draw();
        Up->cd(2);
        TEllipse *el2 = new TEllipse( x0 , -y0 , r1 , r2, 0, 360, theta );
        el2->SetFillStyle(0);
        el2->SetLineColor(2);
        el2->Draw();
        delete FitterEllipse;*/
//End of Use Fitter EllipseTau to fit MC Rings
#endif //DRAW
      if ( ring.r>3. && ring.r<7. && ring.NHits >=7/* && ring.P > 0.2 */)
	{
	  if ( ring.NHits >=15 && ring.primary )
	    {
	      ring.kind = 2;
	    }
	  else ring.kind = 1;
	}
      else ring.kind = 0;

#ifdef DRAW
       Up->cd(1);
       TArc* MCUp = new TArc ( ring.x , -ring.y , ring.r , 0 , 360);
       MCUp->SetLineWidth(2);
       if ( ring.PDG == 11 || ring.PDG == -11 ) 
	{
		MCUp->SetLineColor(2); // electron MC Ring 
		if ( ring.kind != 0 ) NeMC++;
	}
	else if ( ring.PDG == 211 || ring.PDG == -211 || ring.PDG == 111 ) 
	{
		MCUp->SetLineColor(28); // pion MC Ring 
		if ( ring.kind != 0 ) NpiMC++;
	}
        else  MCUp->SetLineColor(5); // other MC Ring
        MCUp->SetFillStyle(0);

       if (ring.kind == 0) {MCUp->SetLineStyle (3); MCUp->SetLineWidth(1); }
       else MCUp->SetLineStyle (1);

       // Draw NHits and P for MC rings
       /*
       TString st1 = "";
       st1 += ring.NHits;
       st1 += "  P = ";
       st1 += ring.P;
       TText* textMc1 = new TText( ring.x + ring.r/5 , -ring.y - ring.r/5 , st1.Data() ); 
       textMc1->SetTextAlign(12);
       textMc1->SetTextSize(0.01);
       textMc1->SetTextColor(1);
       textMc1->Draw();
       */
       MCUp->Draw("*");

       Up->cd(2);
       // Draw NHits and P for MC rings
       /*
       TText* textMc2 = new TText( ring.x + ring.r/5 , -ring.y - ring.r/5 , st1.Data() );
       textMc2->SetTextAlign(12);
       textMc2->SetTextSize(0.01);
       textMc2->SetTextColor(1);
       textMc2->Draw();
       */

       TArc* MCDown = new TArc ( ring.x , -ring.y , ring.r , 0 , 360);
       MCDown->SetLineWidth(2);
       if ( ring.PDG == 11 || ring.PDG == -11) MCDown->SetLineColor(2); //electron MC Ring 
	else if ( ring.PDG == 211 || ring.PDG == -211 ) MCDown->SetLineColor(28); //pion MC Ring 
		else MCDown->SetLineColor(5); // other MC Ring
       MCDown->SetFillStyle(0);
       if (ring.kind == 0) { MCDown->SetLineStyle (3); MCDown->SetLineWidth(1); }
       else MCDown->SetLineStyle (1);
       MCDown->Draw("*");

       for(Int_t i1 = 0; i1 < fRingArray->GetEntriesFast(); i1++ )
	{
        // Reco Rings
        if (
            (
              ( sqrt((ArrRingX[i1]-ring.x)*(ArrRingX[i1]-ring.x)+(ArrRingY[i1]+ring.y)*(ArrRingY[i1]+ring.y)) < 0.15 * ring.r ) &&
              ( sqrt( (ArrRingR[i1]-ring.r)*(ArrRingR[i1]-ring.r) ) < 0.15 * ring.r ) &&
              ( ring.k == 0 )
            ) ||
            ( ( sqrt((ArrRingX[i1]-ring.x)*(ArrRingX[i1]-ring.x)+(ArrRingY[i1]+ring.y)*(ArrRingY[i1]+ring.y)) < 0.2 * ring.r ) &&
              ( sqrt( (ArrRingR[i1]-ring.r)*(ArrRingR[i1]-ring.r) ) < 0.05 * ring.r ) &&
              (ring.k == 0)
            )
          ){
           Up->cd(1);
           TArc* RecoRingUp = new TArc ( ArrRingX[i1] , ArrRingY[i1] , ArrRingR[i1] , 0 , 360 );
           NGostmy--;
	   ring.k = 1;
           if ( ring.r > 3 && ring.r < 7 && ring.NHits >=5 ) NRecoG++;
           RecoRingUp->SetLineWidth(1);
           RecoRingUp->SetLineColor(4);
           if ( ring.r < 3 || ring.r > 7 || ring.NHits < 5 ) RecoRingUp->SetLineColor(5);
           RecoRingUp->SetFillStyle(0);
           RecoRingUp->Draw("*");
           Up->cd(2);
           TArc* RecoRingDown = new TArc ( ArrRingX[i1] , ArrRingY[i1] , ArrRingR[i1] , 0 , 360 );
           RecoRingDown->SetLineWidth(1);
           RecoRingDown->SetLineColor(4);
           if ( ring.r < 3 || ring.r > 7 || ring.NHits < 5 ) RecoRingDown->SetLineColor(5);
           RecoRingDown->SetFillStyle(0);
           RecoRingDown->Draw("*"); 
        }
         // Good Reco Rings
           if (sqrt((ArrRingX[i1]-ring.x)*(ArrRingX[i1]-ring.x)+(ArrRingY[i1]+ring.y)*(ArrRingY[i1]+ring.y)) < 0.05 * ring.r && sqrt( (ArrRingR[i1]-ring.r)*(ArrRingR[i1]-ring.r) ) < 0.05 * ring.r && ring.k != 2 ) {
           Up->cd(1);
           if (ring.k == 1) NRecoG--;
           if ( ring.r > 3 && ring.r < 7 && ring.NHits >=5 ) NRecoEx++;
           TArc* GoodRecoRingUp = new TArc ( ArrRingX[i1] , ArrRingY[i1] , ArrRingR[i1] , 0 , 360);
           GoodRecoRingUp->SetLineWidth(1);
           GoodRecoRingUp->SetLineColor(3);
	   if ( ring.r < 3 || ring.r > 7 || ring.NHits < 5 ) GoodRecoRingUp->SetLineColor(5);
           GoodRecoRingUp->SetFillStyle(0);
	   ring.k = 2;
           GoodRecoRingUp->Draw("*");
           Up->cd(2);
           TArc* GoodRecoRingDown = new TArc ( ArrRingX[i1] , ArrRingY[i1] , ArrRingR[i1] , 0 , 360);
           GoodRecoRingDown->SetLineWidth(1);
           GoodRecoRingDown->SetLineColor(3);
	   if ( ring.r < 3 || ring.r > 7 || ring.NHits < 5 ) GoodRecoRingDown->SetLineColor(5);
           GoodRecoRingDown->SetFillStyle(0);
           GoodRecoRingDown->Draw("*");
        }
      }
#endif //DRAW
  }
  // match reconstructed->simulated rings, calc. ghost

  Int_t NGhost = 0;
  Int_t NReco = fRingArray->GetEntriesFast();
//   Double_t local_x[fRingArray->GetEntriesFast()];
//   Double_t local_y[fRingArray->GetEntriesFast()];
//   Double_t local_r[fRingArray->GetEntriesFast()];

  Int_t MCRecoCor[NReco];

  for( Int_t ir = 0; ir<NReco; ir++ ){
    MCRecoCor[ir] = -1;
    map< Int_t, Int_t > hitmap;
    CbmRichRing *r = L1_DYNAMIC_CAST<CbmRichRing*>( fRingArray->At(ir) );
//     local_x[ir] = r->GetCenterX();
//     local_y[ir] = r->GetCenterY();
//     local_r[ir] = r->GetRadius();
    Int_t nh = r->GetNofHits();
    for( int ih=0; ih<nh; ih++ )
      {
	CbmRichHit *h = L1_DYNAMIC_CAST<CbmRichHit*>( fHitArray->At(r->GetHit(ih)) );
	Int_t jh = pHitIndex[h];
	int ID = Hits[jh].MCTrackID;
	if( hitmap.find( ID )==hitmap.end() ) hitmap.insert( pair<Int_t,Int_t>( ID, 0 ) );
	hitmap[ ID ]++;
      }
    Bool_t ghost = 1;
    Int_t CurrentMCTrack;
    for( map<Int_t,Int_t>::iterator j=hitmap.begin(); j!=hitmap.end(); ++j )
      {
	if ( ( static_cast<Double_t>( j->second ) ) < 0.7 * nh ) continue;
        CurrentMCTrack = j->first;
        MCRecoCor[ir] = MCRingMap[ j->first ].MCTrackID;
	MCRingMap[ j->first ].Reconstructed++;
        if ( ( static_cast<Int_t>( j->second ) ) > MCRingMap[ j->first ].NHitsBestReco) 
          {
           MCRingMap[ j->first ].NHitsBestReco = ( static_cast<Int_t>( j->second ) ) ;
           MCRingMap[ j->first ].NHitsBestvsNHitsMC = ( static_cast<Double_t>( j->second ) )/MCRingMap[ j->first ].NHits; 
          }
	ghost = 0;
	break;
      }
    if ( ghost )
      {
       h_ghost_nhits->Fill( nh );
       NGhost ++;
       Chi2Ghost->Fill(r->GetChi2());
       Chi2NhitsGhost->Fill(r->GetNofHits(), r->GetChi2());
       RGhost->Fill(r->GetRadius());
       RNhitsGhost->Fill(r->GetNofHits(), r->GetRadius());
       RChi2Ghost->Fill( r->GetChi2(), r->GetRadius());
      }
    else 
    { 
      Chi2All->Fill(r->GetChi2());
      Chi2NhitsAll->Fill(r->GetNofHits(), r->GetChi2());
      MCRing &ring_mc = MCRingMap[CurrentMCTrack];
      NHitsRecoVsNHitsMC->Fill( ring_mc.NHits , r->GetNofHits() );
      if ( ring_mc.PDG == 11 || ring_mc.PDG == -11 ){
          REl->Fill(ring_mc.r);
          Chi2NhitsEll->Fill(r->GetNofHits(), r->GetChi2());
          RNhitsEll->Fill(r->GetNofHits(), r->GetRadius());
          RChi2Ell->Fill( r->GetChi2(), r->GetRadius());
      }
      else if ( ring_mc.PDG == 211 || ring_mc.PDG == -211 || ring_mc.PDG == 111 ){
          RPi->Fill(ring_mc.r);
          Chi2NhitsPi->Fill(r->GetNofHits(), r->GetChi2());
          RNhitsPi->Fill(r->GetNofHits(), r->GetRadius());
          RChi2Pi->Fill( r->GetChi2(), r->GetRadius());
	}
    }
  }
//   Int_t IfClone[NReco];

  for( Int_t i1 = 0; i1<NReco; i1++ ){
//     IfClone[i1] = 0;
    CbmRichRing *r1 = L1_DYNAMIC_CAST<CbmRichRing*>( fRingArray->At(i1) );
    Int_t nh1 = r1->GetNofHits();
    for( Int_t i2 = 0; i2<NReco; i2++ ){
      if(MCRecoCor[i1]!=MCRecoCor[i2] ) continue;
      CbmRichRing *r2 = L1_DYNAMIC_CAST<CbmRichRing*>( fRingArray->At(i2) );
      Int_t nh2 = r2->GetNofHits();
      Int_t NSame = 0;
      for ( Int_t j1 = 0; j1<nh1; j1++ ){
         for ( Int_t j2 = 0; j2<nh2; j2++ ){
           if ( r1->GetHit(j1) == r2->GetHit(j2) ) NSame++;
           }
      }
      if (NSame != 0) 
      {
        NSameHits->Fill(NSame);
        NHitsVsMCP -> Fill( MCRingMap[MCRecoCor[i1]].P , NSame  );
      }
    }
  }
  Int_t myNClone=0;
  Int_t CloneFlag[NReco];
  for( Int_t i = 0; i<NReco; i++ )
    {
    CloneFlag[i]=0;
    }
  for( Int_t i = 0; i<NReco; i++ )
    {
    CbmRichRing *r1 = L1_DYNAMIC_CAST<CbmRichRing*>( fRingArray->At(i) );
    if (MCRecoCor[i] == -1) continue;
    for( Int_t j = i+1; j<NReco; j++ )
      {
       CbmRichRing *r2 = L1_DYNAMIC_CAST<CbmRichRing*>( fRingArray->At(j) );
       if (MCRecoCor[j] == -1) continue;
       if (  MCRecoCor[i] == MCRecoCor[j] ) {
        CloneFlag[i]=MCRecoCor[i];
        CloneFlag[j]=MCRecoCor[j];
        myNClone++;
        double dist = 0;
        dist =  sqrt((r1->GetCenterX()-r2->GetCenterX())*(r1->GetCenterX()-r2->GetCenterX())+(r1->GetCenterY()-r2->GetCenterY())*(r1->GetCenterY()-r2->GetCenterY()));
        double dr;
        if ( r1->GetRadius() >= r2->GetRadius() ) dr = r1->GetRadius() - r2->GetRadius();
        else dr = r2->GetRadius() - r1->GetRadius();
        RadiusVsDistanceClone->Fill( dist , dr );
        for( map<Int_t,MCRing>::iterator MC=MCRingMap.begin(); MC!=MCRingMap.end(); ++MC )
          {
           MCRing &ring = MC->second;
           if ( ring.MCTrackID == -1 ) continue;
           if ( ring.MCTrackID == MCRecoCor[j] )
             {
             RadiusVsPForClone->Fill( ring.P , dr );
             DistanceVsPClone->Fill( ring.P , dist );
             Chi2VsPClone->Fill( ring.P , r1->GetChi2() );
             continue;
             }
          }
        MCRecoCor[j] = -1;
        }
      }
      MCRecoCor[i] = -1;
  }
 //Draw Clones
  for( Int_t i = 0; i<NReco; i++ )
    {
    if ( CloneFlag[i] == 0 ) continue;
    CbmRichRing *r = L1_DYNAMIC_CAST<CbmRichRing*>( fRingArray->At(i) );
    for( map<Int_t,MCRing>::iterator MC=MCRingMap.begin(); MC!=MCRingMap.end(); ++MC )
      {
      MCRing &ring = MC->second;
      if ( CloneFlag[i] != 0 )
      {
        if ( CloneFlag[i] == ring.MCTrackID )
          {
          Up->cd(1);
          TArc* MCringUp = new TArc ( ring.x , -ring.y , ring.r , 0 , 360);
          MCringUp->SetLineWidth(1);
          MCringUp->SetLineColor(2);
          MCringUp->SetFillStyle(0);
          ///MCringUp->Draw("*"); //Draw MC ring for clone 
          Up->cd(2);
          TArc* MCringDown = new TArc ( ring.x , -ring.y , ring.r , 0 , 360);
          MCringDown->SetLineWidth(1);
          MCringDown->SetLineColor(2);
          MCringDown->SetFillStyle(0);
          ///MCringDown->Draw("*"); //Draw MC ring for clone 
          }
        Up->cd(1);
        TArc* CloneUp = new TArc ( r->GetCenterX() , -r->GetCenterY() , r->GetRadius() , 0 , 360);
        CloneUp->SetLineWidth(1);
        CloneUp->SetLineColor(7);
        CloneUp->SetFillStyle(0);
        CloneUp->Draw("*");
        Up->cd(2);
        TArc* CloneDown = new TArc ( r->GetCenterX() , -r->GetCenterY() , r->GetRadius() , 0 , 360);
        CloneDown->SetLineWidth(1);
        CloneDown->SetLineColor(7);
        CloneDown->SetFillStyle(0);
        CloneDown->Draw("*");
       
      }
    }
  }
 // End of Draw Clones
  // get statistics from MC rings
  Int_t
    NAll = 0, NRef = 0, NExtra = 0,
    NAllReco = 0, NRefReco = 0, NExtraReco = 0,
    NClone = 0;
  for( map<Int_t,MCRing>::iterator i=MCRingMap.begin(); i!=MCRingMap.end(); ++i )
    {
      MCRing &ring = i->second;
      if (ring.NHitsBestvsNHitsMC != 0.0) 
       {
       NHitsMC->Fill(ring.NHitsBestvsNHitsMC);
       NSameHitsVsP->Fill( ring.P , ring.NHitsBestvsNHitsMC );
       }
      if ( ring.kind==0 ) { NMC++; continue;}
       NAll++;
       NMC37++;
//       Int_t fl=0;
      if ( ring.kind==1 ) NExtra++;
      if ( ring.kind==2 ) NRef++;
      if ( ring.Reconstructed )
      {
//       fl = NClone;
       NAllReco++;
       NClone += i->second.Reconstructed - 1 ;
       if ( ring.kind==2 )  NRefReco++;
       else NExtraReco++;
       }
    }

#ifdef DRAW
        // Draw Hits
        Up->cd(1);
        TGraph* U1 = new TGraph(fHitArray->GetEntriesFast(), HitArrX , HitArrY);
        U1->SetMarkerColor(kBlue);
        U1->SetMarkerStyle(20);
        U1->SetMarkerSize(0.3);
        U1->Draw("P");
        TGraph* U2 = new TGraph( NfakeHits , NoiseHitArrX , NoiseHitArrY);
        U2->SetMarkerColor(7);
        U2->SetMarkerStyle(20);
        U2->SetMarkerSize(0.3);
        U2->Draw("P,Same");
        Up->cd(2);
        TGraph* U = new TGraph(fHitArray->GetEntriesFast(), HitArrX , HitArrY);
        U->SetMarkerColor(kBlue);
        U->SetMarkerStyle(20);
        U->SetMarkerSize(0.3);
        U->Draw("P");
        TGraph* U3 = new TGraph( NfakeHits , NoiseHitArrX , NoiseHitArrY);
        U3->SetMarkerColor(7);
        U3->SetMarkerStyle(20);
        U3->SetMarkerSize(0.3);
        U3->Draw("P,Same");
#endif //DRAW

  // accumulated statistics

  static Int_t
    S_NAll = 0, S_NRef = 0, S_NExtra = 0, S_NReco = 0,
    S_NAllReco = 0, S_NRefReco = 0, S_NExtraReco = 0,
    S_NClone = 0, S_NGhost = 0,
    S_NEvents = 0;

  S_NAll       += NAll;
  S_NRef       += NRef;
  S_NExtra     += NExtra;
  S_NReco      += NReco;
  S_NAllReco   += NAllReco;
  S_NRefReco   += NRefReco;
  S_NExtraReco += NExtraReco;
  S_NClone     += NClone;
  S_NGhost     += NGhost;
  

  S_NEvents++ ;

  // write statistics

  cout.setf(ios::fixed);
  cout.setf(ios::showpoint);
  cout.precision(4);
 

  Double_t p_all   = (NAll   >0)  ? static_cast<Double_t>( NAllReco ) / NAll : 0.;
  Double_t p_ref   = (NRef   >0)  ? static_cast<Double_t>( NRefReco ) / NRef : 0.;
  Double_t p_extra = (NExtra >0)  ? static_cast<Double_t>( NExtraReco ) / NExtra : 0.;
  Double_t p_clone = (NReco  >0)  ? static_cast<Double_t>( NClone ) / NReco : 0.;
  Double_t p_ghost = (NReco  >0)  ? static_cast<Double_t>( NGhost ) / NReco : 0.;
//  Double_t N_all = double(NRecoG+NRecoEx)/double(NMC37);

  cout                                               << endl;
  cout << "L1ENN PER EVENT STAT   : "                << endl;
  cout << "MC Refset              : " << NRef        << endl;
  cout << "MC Extras              : " << NExtra      << endl;
  cout << "ALL SIMULATED          : " << NAll        << endl;
  cout                                               << endl;
  cout << "RC Refset              : " << NRefReco    << endl;
  cout << "RC Extras              : " << NExtraReco  << endl;
  cout << "clones                 : " << NClone      << endl;
  cout << "ghosts                 : " << NGhost      << endl;
  cout << "ALL RECONSTRUCTED      : " << NAllReco    << endl;
  cout                                               << endl;
  cout << "Refset efficiency      : " << p_ref       << endl;
  cout << "Allset efficiency      : " << p_all       << endl;
  cout << "Extra  efficiency      : " << p_extra     << endl;
  cout << "clone  probability     : " << p_clone     << endl;
  cout << "ghost  probability     : " << p_ghost         << endl;
  cout << endl;
//  cout << "Reco time (ms)         : " << fRecoTime/1000. << endl;
 
  
  
  std::string MC_Refset;
  std::stringstream out1;
  out1 << NRef;
  MC_Refset = out.str();


    TString name="2_" + s + ".pdf";
#ifdef DRAW
    Up->SaveAs(name);
#endif // DRAW

  cout << endl;

  Double_t S_p_all   = (S_NAll   >0)  ? static_cast<Double_t>( S_NAllReco ) / S_NAll : 0.;
  Double_t S_p_ref   = (S_NRef   >0)  ? static_cast<Double_t>( S_NRefReco ) / S_NRef : 0.;
  Double_t S_p_extra = (S_NExtra >0)  ? static_cast<Double_t>( S_NExtraReco ) / S_NExtra : 0.;
  Double_t S_p_clone = (S_NReco  >0)  ? static_cast<Double_t>( S_NClone ) / S_NReco : 0.;
  Double_t S_p_ghost = (S_NReco  >0)  ? static_cast<Double_t>( S_NGhost ) / S_NReco : 0.;

  cout << "ACCUMULATED STAT : " << S_NEvents << " EVENTS "               << endl << endl;

  cout << "                         " << "  %   "      <<" | "<< "Reco" << "	| " << "MC" << endl;
  cout << "Refset efficiency      : " << S_p_ref       <<" | "<< S_NRefReco<< "	| " << S_NRef << endl;
  cout << "Allset efficiency      : " << S_p_all       <<" | "<< S_NAllReco <<"	| " << S_NAll << endl;
  cout << "Extra  efficiency      : " << S_p_extra     <<" | "<< S_NExtraReco << "	| " << S_NExtra << endl;
  cout << "clone  probability     : " << S_p_clone     <<" | "<< S_NClone << endl;
  cout << "ghost  probability     : " << S_p_ghost     <<" | "<< S_NGhost << endl;
  cout << "MC rings/event found   : " << Int_t(static_cast<Double_t>(S_NAllReco)/static_cast<Double_t>(S_NEvents)) << endl;
  cout << endl;
  cout << "Reco time (ms)         : " << 0.*1000./S_NEvents
       << endl;

  cout << endl;


  // fill histogramms

  for( map<Int_t,MCRing>::iterator i=MCRingMap.begin(); i!=MCRingMap.end(); ++i )
    {
      MCRing &ring = i->second;

      h_MC_radius->Fill( ring.r );
      h_MC_nhits->Fill( ring.NHits );
      h_MC_momentum->Fill( ring.P );
      if ( ring.primary )
	{
	  h_MC_primary_nhits->Fill( ring.NHits );
	  h_MC_primary_momentum->Fill( ring.P );
	}

      for( vector<int>::iterator j=ring.Hits.begin(); j!=ring.Hits.end(); ++j )
	{
	  Double_t dx = Hits[(*j)].x - ring.x;
	  Double_t dy = Hits[(*j)].y - ring.y;
	  Double_t res = sqrt( dx*dx + dy*dy) - ring.r;
	  h_MC_resolution->Fill( res );
	  if ( ring.kind==1 ) h_MC_extra_resolution->Fill( res );
	  if ( ring.kind==2 ) h_MC_ref_resolution->Fill( res );
	  if ( ring.primary ) h_MC_primary_res_vs_momentum->Fill( ring.P, res );
	}
      Double_t entry = ( ring.Reconstructed ) ? 1.0  : 0.0 ;
      if ( ring.kind==1 ) p_extra_eff_vs_nhits->Fill( ring.NHits, entry );
      if ( ring.kind==2 ) p_ref_eff_vs_nhits->Fill  ( ring.NHits, entry );
    }

  // Open output file and write histograms
  TDirectory *curr = gDirectory;
  TFile* outfile = new TFile("L1RingQaHisto.root","RECREATE");
  outfile->cd();
  TIter hiter(listHisto);
  while (TObject* obj = hiter()) obj->Write();
  outfile->Close();
  curr->cd();
}

void CbmL1RichRingQa::Finish()
{
  TStyle plain("Plain","Plain Style(no colors/fill areas)");
  plain.SetPadColor(0);
  plain.SetCanvasColor(0);
  plain.SetTitleColor(0);
  plain.SetStatColor(0);
  plain.SetOptFit(1111);
  plain.SetStatW(0.225);
  plain.SetOptStat(0);
  plain.SetPalette(1);
  plain.cd();

  TCanvas* Chi2Histos = new TCanvas("Chi2Histos","Chi2Histos",0,0,240,270);
  Chi2Histos->SetFillColor(kWhite);
  Chi2Histos->Divide(2,2);
  Chi2Histos->cd(1);
  Chi2Ghost->Draw();
  Chi2Histos->cd(2);
  //Chi2Ref->Draw();
  Chi2NhitsGhost->Draw("colz");
  Chi2Histos->cd(3);
  Chi2All->Draw();
  Chi2Histos->cd(4);
  //Chi2Clone->Draw();
  Chi2NhitsAll->Draw("colz");
  Chi2Histos->SaveAs("Chi2.pdf");
  TCanvas* All = new TCanvas("All","All",0,0,240,270);
  All->SetFillColor(kWhite);
  All->Divide(3,3);
  All->cd(1);
  Chi2NhitsGhost->Draw("colz");
  All->cd(2);
  Chi2NhitsPi->Draw("colz");
  All->cd(3);
  Chi2NhitsEll->Draw("colz");
  All->cd(4);
  RNhitsGhost->Draw("colz");
  All->cd(5);
  RNhitsPi->Draw("colz");
  All->cd(6);
  RNhitsEll->Draw("colz");
  All->cd(7);
  RChi2Ghost->Draw("colz");
  All->cd(8);
  RChi2Pi->Draw("colz");
  All->cd(9);
  RChi2Ell->Draw("colz");
  All->SaveAs("All.pdf");

  TCanvas* MCNHits = new TCanvas("MCNHits","MCNHits",0,0,240,270);
  MCNHits->SetFillColor(kWhite);
  MCNHits->Divide(2,2);
  MCNHits->cd(1);
  NHitsMC->Draw();
  MCNHits->cd(2);
  NSameHits->Draw();
  MCNHits->cd(3);
  NSameHitsVsP->Draw("colz");
  MCNHits->cd(4);
  NHitsVsMCP->Draw("colz"); 

  MCNHits->SaveAs("MCHits.pdf");

  TCanvas* CloneToEll = new TCanvas("CloneToEll","CloneToEll",0,0,240,270);
  CloneToEll->SetFillColor(kWhite);
  CloneToEll->Divide(2,2);
  CloneToEll->cd(1);
  RadiusVsPForClone->Draw("colz");
  CloneToEll->cd(2);
  DistanceVsPClone->Draw("colz");
  CloneToEll->cd(3);
  RadiusVsDistanceClone->Draw("colz");
  CloneToEll->cd(4);
  Chi2VsPClone->Draw("colz");
  
  CloneToEll->SaveAs("CloneToEll.pdf");

  TCanvas* RecoVsMC = new TCanvas("RecoVsMC","RecoVsMC",0,0,240,270);
  RecoVsMC->SetFillColor(kWhite);
  NHitsRecoVsNHitsMC->Draw("colz");

  RecoVsMC->SaveAs("RecoVsMC.pdf");

  delete Chi2Histos;
  delete All;
  delete MCNHits;
  delete CloneToEll;
  delete RecoVsMC;
  if (Chi2Ghost) delete Chi2Ghost;
  if (Chi2Ref) delete Chi2Ref;
  if (Chi2All) delete Chi2All;
  if (Chi2Clone) delete Chi2Clone;
  if (Chi2NhitsGhost) delete Chi2NhitsGhost;
  if (Chi2NhitsAll) delete Chi2NhitsAll;
  if (REl) delete REl;
  if (RPi) delete RPi;
  if (RGhost) delete RGhost;
  if (Chi2NhitsEll) delete Chi2NhitsEll;
  if (Chi2NhitsPi) delete Chi2NhitsPi;
  if (RNhitsGhost) delete RNhitsGhost;
  if (RNhitsEll) delete RNhitsEll;
  if (RNhitsPi) delete RNhitsPi;
  if (RChi2Ghost) delete RChi2Ghost;
  if (RChi2Ell) delete RChi2Ell;
  if (RChi2Pi) delete RChi2Pi;
  if (NHitsMC) delete NHitsMC;
  if (NSameHits) delete NSameHits;
  if (NHitsVsMCP) delete NHitsVsMCP;
  if (NSameHitsVsP) delete NSameHitsVsP;
  if (RadiusVsPForClone) delete RadiusVsPForClone;
  if (DistanceVsPClone) delete DistanceVsPClone;
  if (Chi2VsPClone) delete Chi2VsPClone;
  if (RadiusVsDistanceClone) delete RadiusVsDistanceClone;
  if (NHitsRecoVsNHitsMC) delete NHitsRecoVsNHitsMC;
}
