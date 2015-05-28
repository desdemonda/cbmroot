/*
 *====================================================================
 *
 *  CBM impact parameter extraction
 *  
 *  Authors: V.Vovchenko
 *
 *  e-mail : 
 *
 *====================================================================
 *
 *  Extraction of event impact parameter from total participant electric charge, based on Glauber model predictions
 *
 *====================================================================
 */

#include "CbmImpactParameterModel.h"
#include "CbmL1Def.h"


//#include "KFParticleFinder.h"
//#include "KFParticleSIMD.h"
#include "CbmKFVertex.h"
#include "CbmKFTrack.h"
#include "CbmStsTrack.h"

#include "TClonesArray.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TMath.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TSpline.h"
#include "TGraphErrors.h"
#include "TCanvas.h"

#include "L1Field.h"

#include "CbmVertex.h"

#include "TStopwatch.h"
#include <iostream>
#include <cstdio>
#include <cmath>
#include <map>

//#include "CbmTrackMatch.h"
#include "CbmTrackMatchNew.h"
#include "CbmMCTrack.h"
//for particle ID from global track
#include "CbmTofHit.h"
#include "CbmGlobalTrack.h"
#include "CbmRichRing.h"
#include "CbmTrdTrack.h"
#include "TDatabasePDG.h"
//for RICH identification
#include "TSystem.h"
// #include "CbmRichElectronIdAnn.h"

#include "CbmL1PFFitter.h"

#include "KFParticleTopoReconstructor.h"
#include "KFPTrackVector.h"
#include "FairMCEventHeader.h"

#include "ImpactParameterModel.h"


using std::vector;
using std::ios;

using namespace std;

ClassImp(CbmImpactParameterModel)

CbmImpactParameterModel::CbmImpactParameterModel(Int_t recoLevel, Int_t iVerbose, TString Mode, KFParticleTopoReconstructor* tr, Float_t ekin_, TString InputTable):
  CbmModelBase(tr),
  //ekin(ekin_),
  //fusePID(usePID),
  ekin(ekin_),
  p0cm(5.),
  ycm(2.),
  fUpdate(true),
  fusePID(true),
  fRecoLevel(recoLevel),
  fTrackNumber(0),
  fEventStats(1),
  events(0),
  fModeName(Mode),
  outfileName(""),
  //fTrackNumber(trackNumber),
  //flistStsTracks(0),
  //flistStsTracksMatch(0),
  //fPrimVtx(0),
  // fUseWidth(useWidth),
  // fUseStatistics(useStatistics),
  // fRadius(rad),
  //flsitGlobalTracks(0),
  //flistTofHits(0),
  histodir(0),
  flistMCTracks(0),
  MCEvent(0),
  Indexb(0), IndexNwp(0), IndexNpe(0), IndexbMC(0), Indexbpe(0), 
  IndexbpeMC(0), Indexbdiff(0), Indexbres(0), Indexbpull(0),
  totalEvents(0),
  PPDG(2212),
  kProtonMass(0.938271998),
//  fCor(0.62)
  fCor(0.40),
  model(NULL)
//  flistRichRings(0),
//  flistTrdTracks(0),
  
{
  // fModeName = Mode;
  
  // events = 0;
  
  // PPDG = 2212;
  // kProtonMass = 0.938271998;
  
  //PDGtoIndex.clear();
  
  TDirectory *currentDir = gDirectory;

  gDirectory->cd("Models");
  
  histodir = gDirectory;
  
  gDirectory->mkdir("ImpactParameterModel");
  gDirectory->cd("ImpactParameterModel");
  gDirectory->mkdir(fModeName);
  gDirectory->cd(fModeName);
  //gDirectory->mkdir("All tracks");
  //gDirectory->cd("All tracks");
  TString tname = "PerEvent";
  //if (fEventStats!=1) tname = TString("Each ") + TString::Itoa(fEventStats, 10) + TString(" events");
  gDirectory->mkdir(tname);
  gDirectory->cd(tname);
	int CurrentIndex = 0;
	
	Indexb = CurrentIndex;
    histo1D[CurrentIndex] = new TH1F("Impact parameter",
			    "Event-by-event b", 
			    30, 0., 15.);
    histo1D[CurrentIndex]->SetXTitle("b (fm)");
    histo1D[CurrentIndex]->SetYTitle("Entries");
	CurrentIndex++;
	
	IndexbMC = CurrentIndex;
    histo1D[CurrentIndex] = new TH1F("Impact parameter MC",
			    "Event-by-event b MC", 
			    30, 0., 15.);
    histo1D[CurrentIndex]->SetXTitle("b (fm)");
    histo1D[CurrentIndex]->SetYTitle("Entries");
	CurrentIndex++;
	
	
	IndexNwp = CurrentIndex;
    histo1D[CurrentIndex] = new TH1F("Total electric charge",
			    "Event-by-event electric charge", 
			    201, -0.5, 200.5);
    histo1D[CurrentIndex]->SetXTitle("Q");
    histo1D[CurrentIndex]->SetYTitle("Entries");
	CurrentIndex++;
	
	IndexNpe = CurrentIndex;
    histo1D[CurrentIndex] = new TH1F("Electric charge",
			    "Event-by-event electric charge", 
			    1000, 0.5, 1000.5);
    histo1D[CurrentIndex]->SetXTitle("Event");
    histo1D[CurrentIndex]->SetYTitle("Np");
	CurrentIndex++;
	
	Indexbpe = CurrentIndex;
    histo1D[CurrentIndex] = new TH1F("Event-by-event impact parameter reco",
			    "Event-by-event impact parameter reco", 
			    1000, 0.5, 1000.5);
    histo1D[CurrentIndex]->SetXTitle("Event");
    histo1D[CurrentIndex]->SetYTitle("b (fm)");
	CurrentIndex++;

	IndexbpeMC = CurrentIndex;
    histo1D[CurrentIndex] = new TH1F("Event-by-event impact parameter MC",
			    "Event-by-event impact parameter MC", 
			    1000, 0.5, 1000.5);
    histo1D[CurrentIndex]->SetXTitle("Event");
    histo1D[CurrentIndex]->SetYTitle("b (fm)");
	CurrentIndex++;
	
	Indexbdiff = CurrentIndex;
    histo1D[CurrentIndex] = new TH1F("Impact parameter diff",
			    "Event-by-event b diff", 
			    150, -10., 10.);
    histo1D[CurrentIndex]->SetXTitle("b-b_{MC} (fm)");
    histo1D[CurrentIndex]->SetYTitle("Entries");
	CurrentIndex++;
	
	Indexbres = CurrentIndex;
    histo1D[CurrentIndex] = new TH1F("Impact parameter res",
			    "Event-by-event b res", 
			    30, -1., 1.);
    histo1D[CurrentIndex]->SetXTitle("b_{res}");
    histo1D[CurrentIndex]->SetYTitle("Entries");
	CurrentIndex++;
	
	Indexbpull = CurrentIndex;
    histo1D[CurrentIndex] = new TH1F("Impact parameter pull",
			    "Event-by-event b pull", 
			    75, -5., 5.);
    histo1D[CurrentIndex]->SetXTitle("b_{pull}");
    histo1D[CurrentIndex]->SetYTitle("Entries");
	CurrentIndex++;
  gDirectory->cd("..");
  gDirectory->cd("..");
  gDirectory->cd("..");
  
  gDirectory = currentDir;
  
  double pbeam = sqrt((kProtonMass+ekin)*(kProtonMass+ekin)-kProtonMass*kProtonMass);
  double betacm = pbeam / (2.*kProtonMass+ekin);
  ycm = 0.5*log((1.+betacm)/(1.-betacm));
  
  double ssqrt = sqrt(2. * kProtonMass * (ekin + 2. * kProtonMass));
  p0cm = sqrt(0.25 * ssqrt * ssqrt - kProtonMass * kProtonMass);
  
  std::cout << "ekin = " << ekin << "\n";
  std::cout << "ycm = " << ycm << "\n";
  
  events = 0;
  
   TString work      = getenv("VMCWORKDIR");
   TString dir  = work + "/KF/KFModelParameters/ImpactParameterModel/";
  
  model = new ImpactParameterModel(std::string((dir + InputTable).Data()));
  
  totalEvents = 0;
}

CbmImpactParameterModel::~CbmImpactParameterModel()
{
	if (model!=NULL) delete model;
}

void CbmImpactParameterModel::ReInit(FairRootManager *fManger)
{
  flistMCTracks = dynamic_cast<TClonesArray*>( fManger->GetObject("MCTrack") );
  MCEvent = dynamic_cast<FairMCEventHeader*>( fManger->GetObject("MCEventHeader.") );
}

void CbmImpactParameterModel::Init()
{
  //ReInit();
}

void CbmImpactParameterModel::Exec()
{
  if (fRecoLevel==-1 && !flistMCTracks) return;
  if (fRecoLevel!=-1 && !fTopoReconstructor) return;

  int charge = CalculateTotalChargeInEvent(fRecoLevel);
  

  totalEvents++;
  events++;
  if (fRecoLevel==-1) {
    std::cout << charge << "  " << model->getB(charge) << "\n";
	histo1D[Indexb]->Fill(model->getB(charge));
	if (MCEvent) histo1D[IndexbMC]->Fill(MCEvent->GetB());
	histo1D[IndexNwp]->Fill(charge);
	histo1D[IndexNpe]->Fill(totalEvents, charge);
	histo1D[Indexbpe]->Fill(totalEvents, model->getB(charge));
	histo1D[Indexbpe]->SetBinError(totalEvents, model->getBerror(charge));
	if (MCEvent) {
		histo1D[IndexbpeMC]->Fill(totalEvents, MCEvent->GetB());
		histo1D[IndexbpeMC]->SetBinError(totalEvents, 1.e-4);
	}
	if (MCEvent) {
		histo1D[Indexbdiff]->Fill(model->getB(charge)-MCEvent->GetB());
		histo1D[Indexbres]->Fill((model->getB(charge)-MCEvent->GetB())/MCEvent->GetB());
		histo1D[Indexbpull]->Fill((model->getB(charge)-MCEvent->GetB())/model->getBerror(charge));
	}
  }
  else {
    std::cout << charge/fCor << "  " << model->getB(charge/fCor) << "\n";
	histo1D[Indexb]->Fill(model->getB(charge/fCor));
	if (MCEvent) histo1D[IndexbMC]->Fill(MCEvent->GetB());
	histo1D[IndexNwp]->Fill(charge/fCor);
	histo1D[IndexNpe]->Fill(totalEvents, charge/fCor);
	histo1D[Indexbpe]->Fill(totalEvents, model->getB(charge/fCor));
	histo1D[Indexbpe]->SetBinError(totalEvents, model->getBerror(charge/fCor));
	if (MCEvent) {
		histo1D[IndexbpeMC]->Fill(totalEvents, MCEvent->GetB());
		histo1D[IndexbpeMC]->SetBinError(totalEvents, 1.e-4);
	}
	if (MCEvent) {
		histo1D[Indexbdiff]->Fill(model->getB(charge/fCor)-MCEvent->GetB());
		histo1D[Indexbres]->Fill((model->getB(charge/fCor)-MCEvent->GetB())/MCEvent->GetB());
		histo1D[Indexbpull]->Fill((model->getB(charge/fCor)-MCEvent->GetB())/model->getBerror(charge/fCor));
	}
  }
}

void CbmImpactParameterModel::Finish(){
}

int CbmImpactParameterModel::CalculateTotalChargeInEvent(int RecoLevel) {
  int ret = 0;
  if (RecoLevel==-1) {
	vector<CbmMCTrack> vRTracksMC;
    int nTracksMC = flistMCTracks->GetEntries();
	std::cout << "MC tracks: " << nTracksMC << "\n";
    vRTracksMC.resize(nTracksMC);
    for(int iTr=0; iTr<nTracksMC; iTr++)
      // vRTracksMC[iTr] = *( (CbmMCTrack*) flistMCTracks->At(iTr));
	  vRTracksMC[iTr] = *( static_cast<CbmMCTrack*> (flistMCTracks->At(iTr) ) );
	  
	for(int iTr=0; iTr<nTracksMC; iTr++) {
	  if (vRTracksMC[iTr].GetMotherId()==-1) 
	  {
		double pt = vRTracksMC[iTr].GetPt();
		if (pt>1.e-4) {
			ret += vRTracksMC[iTr].GetCharge() / 3;
		}
		else {
		}
	  }
    }
  }
  else {
	for(int itype = 2; itype <= 3; ++itype) {
		const KFPTrackVector &tr = fTopoReconstructor->GetTracks() [itype];
		const kfvector_int &pdgs = tr.PDG();
		for(unsigned int ind=0; ind < pdgs.size(); ++ind) {
			int iPDG = pdgs[ind];
			if ((iPDG>0 && iPDG<10000) || (iPDG<0 && iPDG>-10000)) {
				ret += TDatabasePDG::Instance()->GetParticle(iPDG)->Charge() / 3;
			}
		}
	}
  }
  return ret;
}
