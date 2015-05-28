/*
 *====================================================================
 *
 *  CBM Hadron Resonance Gas
 *  
 *  Authors: V.Vovchenko
 *
 *  e-mail : new
 *
 *====================================================================
 *
 *  Statistical hadron-resonance gas model calculations
 *
 *====================================================================
 */

#include "CbmHRGModel.h"
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

#include "ThermalModel.h"
#include "ThermalModelEVMF.h"


using std::vector;
using std::ios;

namespace HRGModelNamespace {

	#include "NumericalIntegration.h"

	const Double_t kProtonMass = 0.938271998;
	const Double_t GeVtoifm = 5.06423;
	
	Double_t AcceptanceFunction::getAcceptance(const Double_t & y, const Double_t & pt) const {
		double ret = sfunc.Eval(y, pt);
		if (ret<0.) ret = 0.;
		if (ret>1.) ret = 1.;
		return ret;
	}
}


using namespace HRGModelNamespace;
using namespace std;

ClassImp(CbmHRGModel)

CbmHRGModel::CbmHRGModel(Int_t recoLevel, Int_t iVerbose, TString Mode, Int_t EventStats, KFParticleTopoReconstructor* tr,
			   Bool_t useWidth,
			   Bool_t useStatistics,
			   Double_t rad):
  CbmModelBase(tr),
  ekin(25.),
  ycm(1.),
  fUpdate(true),
  fusePID(true), //fusePID(usePID),
  fRecoLevel(recoLevel),
  fTrackNumber(1),//fTrackNumber(trackNumber),
  fEventStats(EventStats),
  events(0),
  //flistStsTracks(0),
  //flistStsTracksMatch(0),
  //fPrimVtx(0),
  fUseWidth(useWidth),
  fUseStatistics(useStatistics),
  fRadius(rad),
  fModeName(Mode),
  //flsitGlobalTracks(0),
  //flistTofHits(0),
  outfileName(""),
  histodir(0),
  flistMCTracks(0),
  IndexTemperature(0), IndexErrorTemperature(0), IndexMuB(0), IndexErrorMuB(0), IndexMuS(0), IndexErrorMuS(0), IndexMuQ(0), IndexErrorMuQ(0),
	    IndexnB(0), IndexEnergy(0), IndexEntropy(0), IndexPressure(0), IndexEoverN(0), IndexEtaoverS(0), IndexChi2Fit(0),
		IndexTmuB(0), IndexTnB(0), IndexTE(0),
		grTmuB(NULL), pullT(NULL), pullmuB(NULL),
		Ts(), muB(), errT(), errmuB(),
		PDGtoIndex(), ParticlePDGsTrack(),
		ParticleNames(), RatiosToFit(),
		SystErrors(), MultGlobal(), MultLocal(),
		AcceptanceSTS(), AcceptanceSTSTOF(),
		TPS(), model(NULL)
		
//  flistRichRings(0),
//  flistTrdTracks(0),
  
{
  // fModeName = Mode;
  // fEventStats = EventStats;
  
  // events = 0;
  Ts.resize(0);
  muB.resize(0);
  errT.resize(0);
  errmuB.resize(0);
  
  PDGtoIndex.clear();
  
  TDirectory *currentDir = gDirectory;
  
  gDirectory->cd("Models");
  
  histodir = gDirectory;
  
  gDirectory->mkdir("HadronResonanceGasModel");
  gDirectory->cd("HadronResonanceGasModel");
  gDirectory->mkdir(fModeName);
  gDirectory->cd(fModeName);
  TString tname = "PerEvent";
  if (fEventStats!=1) tname = TString("Each ") + TString::Itoa(fEventStats, 10) + TString(" events");
  gDirectory->mkdir(tname);
  gDirectory->cd(tname);
	int CurrentIndex = 0;
	
	IndexTemperature = CurrentIndex;
    histo1D[CurrentIndex] = new TH1F("Temperature",
			    "Event-by-event temperature", 
			    300, 0.,0.2*1000.);
    histo1D[CurrentIndex]->SetXTitle("T (MeV)");
    histo1D[CurrentIndex]->SetYTitle("Entries");
	CurrentIndex++;
    
	IndexErrorTemperature = CurrentIndex;
    histo1D[CurrentIndex] = new TH1F("Temperature error",
			    "Event-by-event temperature error", 
			    100, 0.,0.1*1000.);
    histo1D[CurrentIndex]->SetXTitle("Delta T (MeV)");
    histo1D[CurrentIndex]->SetYTitle("Entries");
	CurrentIndex++;
    
	IndexMuB = CurrentIndex;
    histo1D[CurrentIndex] = new TH1F("Baryon chemical potential",
			    "Event-by-event baryon chemical potential", 
			    300, 0.,0.8*1000.);
    histo1D[CurrentIndex]->SetXTitle("#mu_{B} (MeV)");
    histo1D[CurrentIndex]->SetYTitle("Entries");
	CurrentIndex++;
    
	IndexErrorMuB = CurrentIndex;
    histo1D[CurrentIndex] = new TH1F("Baryon chemical potential error",
			    "Event-by-event baryon chemical potential error", 
			    100, 0.,0.1*1000.);
    histo1D[CurrentIndex]->SetXTitle("Delta #mu_{B} (MeV)");
    histo1D[CurrentIndex]->SetYTitle("Entries");
	CurrentIndex++;
	
	IndexMuS = CurrentIndex;
	histo1D[CurrentIndex] = new TH1F("Strangeness chemical potential",
			    "Event-by-event strangeness chemical potential", 
			    300, -0.2*1000.,0.2*1000.);
    histo1D[CurrentIndex]->SetXTitle("#mu_{S} (MeV)");
    histo1D[CurrentIndex]->SetYTitle("Entries");
    CurrentIndex++;
	
	IndexErrorMuS = CurrentIndex;
    histo1D[CurrentIndex] = new TH1F("Strangeness chemical potential error",
			    "Event-by-event baryon strangeness potential error", 
			    100, 0.,0.2*1000.);
    histo1D[CurrentIndex]->SetXTitle("Delta #mu_{S} (MeV)");
    histo1D[CurrentIndex]->SetYTitle("Entries");
	CurrentIndex++;
	
	IndexMuQ = CurrentIndex;
	histo1D[CurrentIndex] = new TH1F("Charge chemical potential",
			    "Event-by-event baryon chemical potential", 
			    300, -0.2*1000.,0.2*1000.);
    histo1D[CurrentIndex]->SetXTitle("#mu_{Q} (MeV)");
    histo1D[CurrentIndex]->SetYTitle("Entries");
    CurrentIndex++;
	
	IndexErrorMuQ = CurrentIndex;
    histo1D[CurrentIndex] = new TH1F("Charge chemical potential error",
			    "Event-by-event charge chemical potential error", 
			    100, 0.,0.1*1000.);
    histo1D[CurrentIndex]->SetXTitle("Delta muQ (MeV)");
    histo1D[CurrentIndex]->SetYTitle("Entries");
	CurrentIndex++;
	
	IndexnB = CurrentIndex;
	histo1D[CurrentIndex] = new TH1F("Net baryon density",
			    "Event-by-event net baryon density", 
			    100, 0., 0.2);
    histo1D[CurrentIndex]->SetXTitle("n_{B} (fm^{-3})");
    histo1D[CurrentIndex]->SetYTitle("Entries");
	CurrentIndex++;
	
	IndexEnergy = CurrentIndex;
	histo1D[CurrentIndex] = new TH1F("Energy density",
			    "Event-by-event energy density", 
			    100, 0., 1.*1000.);
    histo1D[CurrentIndex]->SetXTitle("#varepsilon (MeV/fm^{3})");
    histo1D[CurrentIndex]->SetYTitle("Entries");
	CurrentIndex++;
	
	IndexEntropy = CurrentIndex;
	histo1D[CurrentIndex] = new TH1F("Entropy density",
			    "Event-by-event entropy density", 
			    100, 0., 10.);
    histo1D[CurrentIndex]->SetXTitle("s (fm^{-3})");
    histo1D[CurrentIndex]->SetYTitle("Entries");
	CurrentIndex++;
	
	IndexPressure = CurrentIndex;
	histo1D[CurrentIndex] = new TH1F("Pressure",
			    "Event-by-event pressure", 
			    100, 0., 0.2);
    histo1D[CurrentIndex]->SetXTitle("P (GeV/fm^{3})");
    histo1D[CurrentIndex]->SetYTitle("Entries");
	CurrentIndex++;
	
	IndexEoverN = CurrentIndex;
	histo1D[CurrentIndex] = new TH1F("Energy per hadron",
			    "Event-by-event energy per hadron", 
			    100, 0., 3.);
    histo1D[CurrentIndex]->SetXTitle("E/N (GeV)");
    histo1D[CurrentIndex]->SetYTitle("Entries");
	CurrentIndex++;
	
	IndexEtaoverS = CurrentIndex;
	histo1D[CurrentIndex] = new TH1F("Shear viscosity to entropy density ratio",
			    "Event-by-event viscosity to entropy", 
			    100, 0., 1.);
    histo1D[CurrentIndex]->SetXTitle("#eta/s");
    histo1D[CurrentIndex]->SetYTitle("Entries");
	CurrentIndex++;
	
	IndexChi2Fit = CurrentIndex;
	histo1D[CurrentIndex] = new TH1F("chi2/ndf",
			    "Event-by-event chi2/ndf", 
			    100, 0., 20.);
    histo1D[CurrentIndex]->SetXTitle("#chi^{2}/ndf");
    histo1D[CurrentIndex]->SetYTitle("Entries");
	CurrentIndex++;
	
	CurrentIndex = 0;
	IndexTmuB = CurrentIndex;
	histo2D[CurrentIndex] = new TH2F("T-muB", "Event-by-event T-muB",
			    100, 0., 0.8*1000., 
			    100, 0., 0.2*1000.);
    histo2D[CurrentIndex]->SetXTitle("#mu_{B} (MeV)");
    histo2D[CurrentIndex]->SetYTitle("T (MeV)");
	CurrentIndex++;
	
	IndexTnB = CurrentIndex;
	histo2D[CurrentIndex] = new TH2F("T-nB", "Event-by-event T-nB",
			    100, 0., 0.3, 
			    100, 0., 0.2*1000.);
    histo2D[CurrentIndex]->SetXTitle("n_{B} (fm^{-3})");
    histo2D[CurrentIndex]->SetYTitle("T (MeV)");
	CurrentIndex++;
	
	IndexTE = CurrentIndex;
	histo2D[CurrentIndex] = new TH2F("T-en", "Event-by-event T-en",
			    100, 0., 0.3*1000., 
			    100, 0., 0.2*1000.);
    histo2D[CurrentIndex]->SetXTitle("#varepsilon (MeV/fm^{3})");
    histo2D[CurrentIndex]->SetYTitle("T (MeV)");
	CurrentIndex++;
	
	pullT = new TH1F("Pull T",
			    "Event-by-event pull T", 
			    300, -5.,5.);
    pullT->SetXTitle("Pull T");
    pullT->SetYTitle("Entries");
	
	pullmuB = new TH1F("Pull muB",
			    "Event-by-event pull muB", 
			    300, -5.,5.);
    pullmuB->SetXTitle("Pull muB");
    pullmuB->SetYTitle("Entries");
	
	grTmuB = new TGraphErrors();
    grTmuB->SetTitle("Event-by-event T-muB");
	grTmuB->GetXaxis()->SetTitle("#mu_{B} (MeV)");
    grTmuB->GetYaxis()->SetTitle("T (MeV)");
	grTmuB->SetName(TString("T-muB-") + fModeName);
	gDirectory->Add(grTmuB);
	
  gDirectory->cd("..");
  gDirectory->cd("..");
  gDirectory->cd("..");
  
  gDirectory = currentDir;
  
  double pbeam = sqrt((kProtonMass+ekin)*(kProtonMass+ekin)-kProtonMass*kProtonMass);
  double betacm = pbeam / (2.*kProtonMass+ekin);
  ycm = 0.5*log((1.+betacm)/(1.-betacm));
  
  events = 0;
  
  TString work      = getenv("VMCWORKDIR");
  TString dir  = work + "/KF/KFModelParameters/HRGModel/";

  TPS = ThermalParticleSystem(std::string((dir + "Database.dat").Data()));
  
  model = NULL;
}

CbmHRGModel::~CbmHRGModel()
{
	if (model!=NULL) delete model;
}

void CbmHRGModel::ReadAcceptanceFunction(AcceptanceFunction & func, TString filename)
{
  double ymin = 0., ymax = 6.;
  double ptmin = 0., ptmax = 2.5;
  func.ys.resize(0);
  func.pts.resize(0);
  func.probs.resize(0);
  ifstream fin(filename.Data());
  fin >> func.dy >> func.dpt;
  double ty, tpt, prob;//, tmp;
  func.ys.resize(0);
  func.pts.resize(0);
  func.probs.resize(0);
  while (fin >> ty >> tpt >> prob) {
    if (tpt<ptmin || tpt>ptmax || ty<ymin || ty>ymax) continue;
    func.ys.push_back(ty);
    func.pts.push_back(tpt);
    func.probs.push_back(prob);
  }
  func.setSpline();
  fin.close();
}

void CbmHRGModel::ReInit(FairRootManager *fManger)
{
  flistMCTracks = dynamic_cast<TClonesArray*>( fManger->GetObject("MCTrack") );
}

void CbmHRGModel::Init()
{
  //ReInit();
}

void CbmHRGModel::Exec()
{
  if (fRecoLevel==-1 && !flistMCTracks) return;
  if (fRecoLevel!=-1 && !fTopoReconstructor) return;

  CalculateMultiplicitiesInEvent(MultLocal, fRecoLevel, 1);
  
  events++;
  
  if (events%fEventStats==0) {
  
	  ThermalModelFitParameters params;
	  /*std::vector< std::pair<int, int> > ratios(0);
	  ratios.push_back(make_pair(0, 1)); // pi-/pi+
	  ratios.push_back(make_pair(2, 3)); // K-/K+
	  ratios.push_back(make_pair(3, 1)); // K+/pi+
	  //ratios.push_back(make_pair(2, 0)); // K-/pi-
	  ratios.push_back(make_pair(4, 0)); // p/pi-
	  ratios.push_back(make_pair(5, 4)); // p+/p-
	  ratios.push_back(make_pair(7, 6)); // Labar/La*/
	  // ratios.push_back(make_pair(-211, 211)); // pi-/pi+
	  // ratios.push_back(make_pair(-321, 321)); // K-/K+
	  // ratios.push_back(make_pair(321, 211)); // K+/pi+
	  // ratios.push_back(make_pair(-321, -211)); // K-/pi-
	  // ratios.push_back(make_pair(2212, -211)); // p/pi-
	  
	  ThermalModelEVMF modell(&TPS);
	  //model.RHad = 0.;
	  modell.SetUseWidth(fUseWidth);
	  modell.SetStatistics(fUseStatistics);
	  modell.Parameters.R = fRadius;
	  double tmpdens = 0., tmpenerd = 0.;
	  double tmpeta = 0., tmpsdens = 0.;
	   
	  params = GetThermalParameters(MultLocal);
	  
	  if (fabs(params.T.value-params.T.xmin)/params.T.value<1e-2 || fabs(params.T.value-params.T.xmax)/params.T.value<1e-2 || params.T.error*1.e3>20. || params.T.value>0.160) 
		std::cout << "Fit failed!" << endl;
	  else {
	  
		  histo1D[IndexTemperature]->Fill(params.T.value*1.e3);
		  histo1D[IndexErrorTemperature]->Fill(params.T.error*1.e3);
		  histo1D[IndexMuB]->Fill(params.muB.value*1.e3);
		  histo1D[IndexErrorMuB]->Fill(params.muB.error*1.e3);
		  histo1D[IndexMuS]->Fill(params.muS.value*1.e3);
		  histo1D[IndexErrorMuS]->Fill(params.muS.error*1.e3);
		  histo1D[IndexMuQ]->Fill(params.muQ.value*1.e3);
		  histo1D[IndexErrorMuQ]->Fill(params.muQ.error*1.e3);
		  histo1D[IndexChi2Fit]->Fill(params.chi2ndf);
		  histo2D[IndexTmuB]->Fill(params.muB.value*1.e3, params.T.value*1.e3);
		  Ts.push_back(params.T.value*1.e3);
		  muB.push_back(params.muB.value*1.e3);
		  errT.push_back(params.T.error*1.e3);
		  errmuB.push_back(params.muB.error*1.e3);
		  
		  /*hTempFullMC->Fill(params.T.value*1.e3);
		  hTempErrFullMC->Fill(params.T.error*1.e3);
		  hMuBFullMC->Fill(params.muB.value*1.e3);
		  hMuBErrFullMC->Fill(params.muB.error*1.e3);
		  hMuSFullMC->Fill(params.muS.value*1.e3);
		  hMuSErrFullMC->Fill(params.muS.error*1.e3);
		  hMuQFullMC->Fill(params.muQ.value*1.e3);
		  hMuQErrFullMC->Fill(params.muQ.error*1.e3);
		  hChi2NDFMC->Fill(params.chi2ndf);
		  hTMuFullMC->Fill(params.muB.value*1.e3, params.T.value*1.e3);*/
		  
		  modell.SetParameters(params.GetThermalModelParameters());//, 0.5);
		  tmpdens = modell.CalculateBaryonDensity();
		  tmpenerd = modell.CalculateEnergyDensity();
		  tmpeta = modell.CalculateShearViscosity();
		  tmpsdens = modell.CalculateEntropyDensity(); 
		  histo1D[IndexnB]->Fill(tmpdens);
		  histo1D[IndexEnergy]->Fill(tmpenerd*1.e3);
		  histo1D[IndexEntropy]->Fill(modell.CalculateEntropyDensity());
		  histo1D[IndexPressure]->Fill(modell.CalculatePressure());
		  histo1D[IndexEoverN]->Fill(tmpenerd / modell.CalculateHadronDensity());
		  histo2D[IndexTnB]->Fill(tmpdens, params.T.value*1.e3);
		  histo2D[IndexTE]->Fill(tmpenerd*1.e3, params.T.value*1.e3);
		  histo1D[IndexEtaoverS]->Fill(tmpeta / tmpsdens);
		  /*hnBFullMC->Fill(tmpdens);
		  hendFullMC->Fill(tmpenerd*1.e3);
		  hsdFullMC->Fill(model.CalculateEntropyDensity());
		  hpFullMC->Fill(model.CalculatePressure());
		  hENFullMC->Fill(tmpenerd / model.CalculateHadronDensity());
		  hTnBFullMC->Fill(tmpdens, params.T.value*1.e3);
		  hTenFullMC->Fill(tmpenerd*1.e3, params.T.value*1.e3);
		  hetasFullMC->Fill(tmpeta / tmpsdens);*/
		  
		  pullT->Fill((params.T.value-0.100)/params.T.error);
		  pullmuB->Fill((params.muB.value-0.550)/params.muB.error);
		  
		  
		  std::cout << "nB = " << tmpdens << endl;
		  std::cout << "eta/s = " << modell.CalculateShearViscosity() / modell.CalculateEntropyDensity() << endl;
	  }
	  
	  events = 0;
	  for(unsigned int i=0;i<MultLocal.size();++i) {
		MultLocal[i] = 0;
	  }
  }
    
  //delete[] MCTrackSortedArray;
  //delete[] TrRecons;
}

 ThermalModelFitParameters CbmHRGModel::GetThermalParameters(const std::vector<int> & Mults) {
	//ThermalModelFit TFit(&TPS);
	ThermalModelBase *modelnew;
	if (fabs(fRadius<1e-6)) modelnew = new ThermalModel(&TPS);
	else modelnew = new ThermalModelEVMF(&TPS);
	if (model!=NULL) delete model;
    model = modelnew;
	ThermalModelFit TFit(model);
	modelnew->SetUseWidth(fUseWidth);
	modelnew->SetStatistics(fUseStatistics);
	modelnew->Parameters.R = fRadius;
	double Z = 79., B = 197., QBmod = Z/B;
	TFit.SetQBConstraint(QBmod);
	for(unsigned int i=0;i<RatiosToFit.size();++i) {
		double n1 = Mults[PDGtoIndex[RatiosToFit[i].first]];
		double n2 = Mults[PDGtoIndex[RatiosToFit[i].second]];
		double err1 = sqrt(n1);
		double err2 = sqrt(n2);
		double terr = sqrt(err1*err1/n2/n2 + n1*n1/n2/n2/n2/n2*err2*err2);
		double rat = n1/n2;
		//terr = sqrt(terr*terr + 0.05*rat*0.05*rat);
		terr = sqrt(terr*terr + SystErrors[i]*SystErrors[i]*rat*rat);
		std::cout << "Ratio " << ParticleNames[PDGtoIndex[RatiosToFit[i].first]] << "/" << ParticleNames[PDGtoIndex[RatiosToFit[i].second]] << ": " << n1/n2 << " " << terr << " " << n1 << " " << n2 << "\n";
		
		
		//if (n1/n2>2.*terr) 
		if (n1!=0 && n2!=0) TFit.AddRatio(ExperimentRatio(ParticlePDGsTrack[PDGtoIndex[RatiosToFit[i].first]], ParticlePDGsTrack[PDGtoIndex[RatiosToFit[i].second]], n1/n2, terr));
		else {
			std::cout << "Bad ratio " << ParticleNames[PDGtoIndex[RatiosToFit[i].first]] << "/" << ParticleNames[PDGtoIndex[RatiosToFit[i].second]] << ": " << n1/n2 << " " << terr << " " << n1 << " " << n2 << "\n";
			std::cout << "Aborting fit\n";
			ThermalModelFitParameters params;
			params.T.error = 100.;
			return params;
		}
		//std::cout << pdgIds[ratios[i].first] << " " << pdgIds[ratios[i].second] << " " << n1/n2 << " " << terr << endl;
	}
	return TFit.PerformFit();
 }

Double_t CbmHRGModel::RecEfficiency(Double_t p) {
  if (fTrackNumber==0) return 0.99 - 0.98 * exp(-p*p/2./0.135/0.135);
  else return 0.98 - 0.88 * exp(-p*p/2./0.2/0.2);
}

bool CbmHRGModel::checkIfReconstructable(CbmKFTrErrMCPoints *inTrack)
{
  //bool ret = 0;
  int stsHits = inTrack->GetNStsPoints();
  int tofHits = inTrack->GetNTofPoints();
  if (fusePID==2 && tofHits==0) return 0;
  vector<int> hitz(0);
  for(int hit=0;hit<stsHits;++hit) {
    hitz.push_back(static_cast<int>((inTrack->GetStsPoint(hit)->GetZ()+5.)/10.));
  }
  sort(hitz.begin(), hitz.end());
  for(int hit1=0;hit1<stsHits-3;++hit1)
  {
    int station1 = hitz[hit1];
    int hitsConsecutive = 1;
    for(int hit2=hit1+1;hit2<stsHits && hitsConsecutive<4;++hit2)
    {
      int station2 = hitz[hit2];
      if (station2==station1) continue;
      if (station2-station1>1) break;
      if (station2-station1==1) {
	hitsConsecutive++;
	station1 = station2;
      }
    }
    if (hitsConsecutive==4) return 1;
  }
  return 0;
}

void CbmHRGModel::Finish(){
  ThermalModelFitParameters params;
  params = GetThermalParameters(MultGlobal);
  
  for(unsigned int i=0;i<Ts.size();++i) {
	grTmuB->SetPoint(i, muB[i], Ts[i]);
	grTmuB->SetPointError(i, errmuB[i], errT[i]);
  }
  grTmuB->GetXaxis()->SetLimits(0., 600.);
  grTmuB->SetMinimum(0.);
  grTmuB->SetMinimum(300.);
  grTmuB->GetYaxis()->SetLimits(0., 200.);
}

void CbmHRGModel::AddRatio(int pdgid1, int pdgid2, /*TString name1, TString name2,*/ double SystError) {
  RatiosToFit.push_back(make_pair(pdgid1, pdgid2));
  SystErrors.push_back(SystError);
  int pIndex1 = -1, pIndex2 = -1;
  //std::cout << pdgid1 << " " << pdgid2 << " " << name1 << " " << name2 << " " << SystError << "\n";
  for(unsigned int i = 0; i < ParticlePDGsTrack.size(); ++i) {
	if (ParticlePDGsTrack[i]==pdgid1) pIndex1 = i;
	if (ParticlePDGsTrack[i]==pdgid2) pIndex2 = i;
  }
  
  if (pIndex1==-1) {
	ParticlePDGsTrack.push_back(pdgid1);
	ParticleNames.push_back(TString(TDatabasePDG::Instance()->GetParticle(pdgid1)->GetName()));
	MultGlobal.push_back(0);
	MultLocal.push_back(0);
	PDGtoIndex[pdgid1] = ParticlePDGsTrack.size() - 1;
  }
  /*else if (ParticleNames[pIndex1]=="") {
	ParticleNames[pIndex1] = name1;
  }*/
  
  if (pIndex2==-1) {
	ParticlePDGsTrack.push_back(pdgid2);
	//ParticleNames.push_back(name2);
	ParticleNames.push_back(TString(TDatabasePDG::Instance()->GetParticle(pdgid2)->GetName()));
	MultGlobal.push_back(0);
	MultLocal.push_back(0);
	PDGtoIndex[pdgid2] = ParticlePDGsTrack.size() - 1;
  }
  
}

void CbmHRGModel::CalculateMultiplicitiesInEvent(std::vector<int> & Mult, int RecoLevel, bool UpdateGlobal) {
	
  if (RecoLevel==-1) {
	vector<CbmMCTrack> vRTracksMC;
    int nTracksMC = flistMCTracks->GetEntries();
	std::cout << "MC tracks: " << nTracksMC << "\n";
    vRTracksMC.resize(nTracksMC);
    for(int iTr=0; iTr<nTracksMC; iTr++)
      //vRTracksMC[iTr] = *( (CbmMCTrack*) flistMCTracks->At(iTr));
	  vRTracksMC[iTr] = *( dynamic_cast<CbmMCTrack*>(flistMCTracks->At(iTr)));
	  
	for(int iTr=0; iTr<nTracksMC; iTr++) {
	  //iTr = ((CbmTrackMatch*)flistStsTracksMatch->At(iTrs))->GetMCTrackId();
      for(unsigned int part=0;part<ParticlePDGsTrack.size();++part) {
	    if (vRTracksMC[iTr].GetPdgCode()==ParticlePDGsTrack[part] && vRTracksMC[iTr].GetMotherId()==-1) {
		  Mult[part]++;
		  if (UpdateGlobal) MultGlobal[part]++;
	    }
      }
    }
  }
  else {
	//int ind = 2;
	for(int itype = 2; itype <= 3; ++itype) {
		const KFPTrackVector &tr = fTopoReconstructor->GetTracks() [itype];
		const kfvector_int &pdgs = tr.PDG();
		for(unsigned int ind=0; ind < pdgs.size(); ++ind) {
			int iPDG = pdgs[ind];
			for(unsigned int part=0;part<ParticlePDGsTrack.size();++part) {
				if (iPDG==ParticlePDGsTrack[part]) {
					Mult[part]++;
					if (UpdateGlobal) MultGlobal[part]++;
				}
			}
		}
	}
  }
}
