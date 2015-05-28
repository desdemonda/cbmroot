/*
 *====================================================================
 *
 *  Extraction of temperature from Boltzmann distribution
 *  
 *  Authors: V.Vovchenko
 *
 *  e-mail : new
 *
 *====================================================================
 *
 *  Extraction of temperature from Boltzmann distribution
 *
 *====================================================================
 */

#include "CbmBoltzmannDistribution.h"
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

#include "BoltzmannDistribution.h"


using std::vector;
using std::ios;

using namespace std;

ClassImp(CbmBoltzmannDistribution)

CbmBoltzmannDistribution::CbmBoltzmannDistribution(Int_t recoLevel, Int_t iVerbose, TString Mode, Int_t PDG, TString pname, Int_t EventStats, KFParticleTopoReconstructor* tr, Float_t ekin_):
  CbmModelBase(tr),
  name(pname),
  //ekin(ekin_),
  //fusePID(usePID),
  //fTrackNumber(trackNumber),
  //flistStsTracks(0),
  //flistStsTracksMatch(0),
  //fPrimVtx(0),
  ekin(ekin_),
  p0cm(5.),
  ycm(2.),
  fUpdate(true),
  fusePID(true),
  fRecoLevel(recoLevel),
  fTrackNumber(1),
  fEventStats(EventStats),
  events(0),
  fModeName(Mode),
  outfileName(""),
  //flsitGlobalTracks(0),
  //flistTofHits(0),
  histodir(0),
  flistMCTracks(0),
  IndexT(0), IndexMt(0), IndexModelMt(0), 
  IndexMt2(0), IndexModelMt2(0), IndexModelMt4Pi(0),
  histodndy(0), histodndymodel(0),
  histo1DIntervals(0),
  grTy(0),
  grdndyReco(0),
  pullT(0),
  Ts(),
  kProtonMass(0.938271998),
  fPDGID(PDG),
  fMass(TDatabasePDG::Instance()->GetParticle(fPDGID)->Mass()),
  fYminv(), fYmaxv(),
  paramGlobal(0), paramGlobalInterval(0), param2GlobalInterval(0),
  paramLocal(0), paramLocalInterval(0),
  totalLocal(0), totalGlobal(0),
  totalGlobalInterval(0), totalLocalInterval(0),
  totalEvents(0),
  model(0), modelmc(0), modelsY(0)
  
//  flistRichRings(0),
//  flistTrdTracks(0),
  
{
  // fModeName = Mode;
  // fEventStats = EventStats;
  // fMass = TDatabasePDG::Instance()->GetParticle(fPDGID)->Mass();
  
  // events = 0;
  Ts.resize(0);
  
  //PPDG = 2212;
  // kProtonMass = 0.938271998;
  
  //PDGtoIndex.clear();
  
  TDirectory *currentDir = gDirectory;
  
  //std::cout << 0.128 << "\t" << mtTall->Eval(0.128) << endl;
  //gDirectory->mkdir("KFModelParameters");
  gDirectory->cd("Models");
  
  histodir = gDirectory;
  
  char ccc[200];
  sprintf(ccc, "BoltzmannDistribution %s", name.Data());
  gDirectory->mkdir(ccc);
  gDirectory->cd(ccc);
  gDirectory->mkdir(fModeName);
  gDirectory->cd(fModeName);
  TString tname = "PerEvent";
  if (fEventStats!=1) tname = TString("Each ") + TString::Itoa(fEventStats, 10) + TString(" events");
  gDirectory->mkdir(tname);
  gDirectory->cd(tname);
  //for(int part=0;part<p_sz;++part) {
    //gDirectory->mkdir(p_names[part]);
    //gDirectory->cd(p_names[part]);
	int CurrentIndex = 0;
	
	IndexT = CurrentIndex;
    histo1D[CurrentIndex] = new TH1F("T",
			    "Event-by-event T", 
			    100, 0., 0.4);
    histo1D[CurrentIndex]->SetXTitle("T (GeV)");
    histo1D[CurrentIndex]->SetYTitle("Entries");
	CurrentIndex++;
	
	IndexMt = CurrentIndex;
	histo1D[CurrentIndex] = new TH1F("f(m_{T})",
			    "mt distribution", 
			    200, 0., 2.5);
    histo1D[CurrentIndex]->SetXTitle("m_{T} - m_{0} (GeV)");
    histo1D[CurrentIndex]->SetYTitle("Entries");
    CurrentIndex++;
	
	IndexModelMt = CurrentIndex;
	histo1D[CurrentIndex] = new TH1F("Model f(m_{T})",
			    "Model mt distribution", 
			    200, 0., 2.5);
    histo1D[CurrentIndex]->SetXTitle("m_{T} - m_{0} (GeV)");
    histo1D[CurrentIndex]->SetYTitle("Entries");
    CurrentIndex++;
	
	IndexMt2 = CurrentIndex;
	histo1D[CurrentIndex] = new TH1F("f2(m_{T})",
			    "mt2 distribution", 
			    40, 0., 2.5);
    histo1D[CurrentIndex]->SetXTitle("m_{T} - m_{0} (GeV)");
    histo1D[CurrentIndex]->SetYTitle("#frac{1}{N} #frac{dN}{m_{T} dm_{T}} [(GeV)^{-2}]");
    CurrentIndex++;
	
	IndexModelMt2 = CurrentIndex;
	histo1D[CurrentIndex] = new TH1F("Model f2(m_{T})",
			    "Model mt2 distribution", 
			    200, 0., 2.5);
    histo1D[CurrentIndex]->SetXTitle("m_{T} - m_{0} (GeV)");
    histo1D[CurrentIndex]->SetYTitle("#frac{1}{N} #frac{dN}{m_{T} dm_{T}} [(GeV)^{-2}]");
    CurrentIndex++;
	
	IndexModelMt4Pi = CurrentIndex;
	histo1D[CurrentIndex] = new TH1F("Model f2(m_{T}) 4Pi",
			    "Model mt2 distribution", 
			    200, 0., 2.5);
    histo1D[CurrentIndex]->SetXTitle("m_{T} - m_{0} (GeV)");
    histo1D[CurrentIndex]->SetYTitle("#frac{1}{N} #frac{dN}{m_{T} dm_{T}} [(GeV)^{-2}]");
    CurrentIndex++;
	
	histodndy = new TH1F("dN/dy",
			    "Rapidity distribution", 
			    40, -3., 3.);
    histodndy->SetXTitle("y");
    histodndy->SetYTitle("dN/dy");
	
	histodndymodel = new TH1F("dN/dy model",
			    "Rapidity distribution from model", 
			    40, -3., 3.);
    histodndymodel->SetXTitle("y");
    histodndymodel->SetYTitle("dN/dy");
	
	grdndyReco = new TGraphErrors();
    grdndyReco->SetTitle("Rapidity distribution");
	grdndyReco->GetXaxis()->SetTitle("y");
    grdndyReco->GetYaxis()->SetTitle("dN/dy");
	grdndyReco->SetName(TString("dNdy-") + fModeName);
	gDirectory->Add(grdndyReco);
	
	
  if (fYminv.size()>0) {
	histo1DIntervals = new TH1F**[fYminv.size()];
	for(unsigned int ind = 0;ind < fYminv.size(); ++ind) {
	    histo1DIntervals[ind] = new TH1F*[nHisto1D];
		char cc[200], cc2[200];
		CurrentIndex = 0;
		
		sprintf(cc, "Event-by-event T, %.2lf<y<%.2lf", fYminv[ind], fYmaxv[ind]);
		sprintf(cc2, "T, %.2lf<y<%.2lf", fYminv[ind], fYmaxv[ind]);
		histo1DIntervals[ind][CurrentIndex] = new TH1F(cc2,
			    cc, 
			    100, 0., 1.);
		histo1DIntervals[ind][CurrentIndex]->SetXTitle("T (GeV)");
		histo1DIntervals[ind][CurrentIndex]->SetYTitle("Entries");
		CurrentIndex++;
		
		sprintf(cc, "mt distribution, %.2lf<y<%.2lf", fYminv[ind], fYmaxv[ind]);
		sprintf(cc2, "f(m_{T}, %.2lf<y<%.2lf", fYminv[ind], fYmaxv[ind]);
		histo1DIntervals[ind][CurrentIndex] = new TH1F(cc2,
			    cc, 
			    200, 0., 2.5);
		histo1DIntervals[ind][CurrentIndex]->SetXTitle("m_{T} - m_{0} (GeV)");
		histo1DIntervals[ind][CurrentIndex]->SetYTitle("Entries");
		CurrentIndex++;
		
		sprintf(cc, "Model mt distribution, %.2lf<y<%.2lf", fYminv[ind], fYmaxv[ind]);
		sprintf(cc2, "Model f(m_{T}), %.2lf<y<%.2lf", fYminv[ind], fYmaxv[ind]);
		histo1DIntervals[ind][CurrentIndex] = new TH1F(cc2,
			    cc, 
			    200, 0., 2.5);
		histo1DIntervals[ind][CurrentIndex]->SetXTitle("m_{T} - m_{0} (GeV)");
		histo1DIntervals[ind][CurrentIndex]->SetYTitle("Entries");
		CurrentIndex++;
		
		sprintf(cc, "mt2 distribution, %.2lf<y<%.2lf", fYminv[ind], fYmaxv[ind]);
		sprintf(cc2, "f2(m_{T}, %.2lf<y<%.2lf", fYminv[ind], fYmaxv[ind]);
		histo1DIntervals[ind][CurrentIndex] = new TH1F(cc2,
			    cc, 
			    40, 0., 2.5);
		histo1DIntervals[ind][CurrentIndex]->SetXTitle("m_{T} - m_{0} (GeV)");
		histo1DIntervals[ind][CurrentIndex]->SetYTitle("#frac{1}{N} #frac{dN}{m_{T} dm_{T}} [(GeV)^{-2}]");
		CurrentIndex++;
		
		sprintf(cc, "Model mt2 distribution, %.2lf<y<%.2lf", fYminv[ind], fYmaxv[ind]);
		sprintf(cc2, "Model f2(m_{T}), %.2lf<y<%.2lf", fYminv[ind], fYmaxv[ind]);
		histo1DIntervals[ind][CurrentIndex] = new TH1F(cc2,
			    cc, 
			    200, 0., 2.5);
		histo1DIntervals[ind][CurrentIndex]->SetXTitle("m_{T} - m_{0} (GeV)");
		histo1DIntervals[ind][CurrentIndex]->SetYTitle("#frac{1}{N} #frac{dN}{m_{T} dm_{T}} [(GeV)^{-2}]");
		CurrentIndex++;
		
		sprintf(cc, "Model mt2 distribution 4Pi, %.2lf<y<%.2lf", fYminv[ind], fYmaxv[ind]);
		sprintf(cc2, "Model f2(m_{T}) 4Pi, %.2lf<y<%.2lf", fYminv[ind], fYmaxv[ind]);
		histo1DIntervals[ind][CurrentIndex] = new TH1F(cc2,
			    cc, 
			    200, 0., 2.5);
		histo1DIntervals[ind][CurrentIndex]->SetXTitle("m_{T} - m_{0} (GeV)");
		histo1DIntervals[ind][CurrentIndex]->SetYTitle("#frac{1}{N} #frac{dN}{m_{T} dm_{T}} [(GeV)^{-2}]");
		CurrentIndex++;
	}
  }
	
	
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

  paramGlobal = paramLocal = 0.;
  totalGlobal = totalLocal = 0;
  paramGlobalInterval.resize(0);
  param2GlobalInterval.resize(0);
  paramLocalInterval.resize(0);
  totalGlobalInterval.resize(0);
  totalLocalInterval.resize(0);
  modelsY.resize(0);
  
  if (fRecoLevel==-1 || fRecoLevel>10) model = new BoltzmannDistribution(fMass);
  else model = new BoltzmannDistribution(fMass, fPDGID, true, -ycm, ycm, ycm, 1.);
  modelmc = new BoltzmannDistribution(fMass);
}

CbmBoltzmannDistribution::~CbmBoltzmannDistribution()
{
	if (model!=NULL) delete model;
	for(unsigned int ind=0;ind<fYminv.size();++ind) if (modelsY[ind]!=NULL) delete modelsY[ind];
}

void CbmBoltzmannDistribution::AddRapidityInterval(double ymin, double ymax)
{
	fYminv.push_back(ymin);
	fYmaxv.push_back(ymax);
	paramGlobalInterval.push_back(0.);
	param2GlobalInterval.push_back(0.);
	paramLocalInterval.push_back(0.);
	totalGlobalInterval.push_back(0);
	totalLocalInterval.push_back(0);
	if (fRecoLevel==-1 || fRecoLevel>10) modelsY.push_back(new BoltzmannDistribution(fMass));
	else modelsY.push_back(new BoltzmannDistribution(fMass, fPDGID, true, ymin, ymax, ycm, 1.));
}

void CbmBoltzmannDistribution::AddHistos()
{
  TDirectory *currentDir = gDirectory;
  
  gDirectory->cd("Models");
  
  histodir = gDirectory;
  
  char ccc[200];
  sprintf(ccc, "BoltzmannDistribution %s", name.Data());
  //gDirectory->mkdir(ccc);
  gDirectory->cd(ccc);
  //gDirectory->mkdir(fModeName);
  gDirectory->cd(fModeName);
  TString tname = "PerEvent";
  if (fEventStats!=1) tname = TString("Each ") + TString::Itoa(fEventStats, 10) + TString(" events");
  gDirectory->cd(tname);
	int CurrentIndex = 0;
	
  if (fYminv.size()>0) {
	histo1DIntervals = new TH1F**[fYminv.size()];
	for(unsigned int ind = 0;ind < fYminv.size(); ++ind) {
		char cc3[200];
		sprintf(cc3, "%.2lf<y<%.2lf", fYminv[ind], fYmaxv[ind]);
		gDirectory->mkdir(cc3);
		gDirectory->cd(cc3);
		
	    histo1DIntervals[ind] = new TH1F*[nHisto1D];
		char cc[200], cc2[200];
		CurrentIndex = 0;
		
		sprintf(cc, "Event-by-event T, %.2lf<y<%.2lf", fYminv[ind], fYmaxv[ind]);
		sprintf(cc2, "T, %.2lf<y<%.2lf", fYminv[ind], fYmaxv[ind]);
		histo1DIntervals[ind][CurrentIndex] = new TH1F(cc2,
			    cc, 
			    100, 0., 1.);
		histo1DIntervals[ind][CurrentIndex]->SetXTitle("T (GeV)");
		histo1DIntervals[ind][CurrentIndex]->SetYTitle("Entries");
		CurrentIndex++;
		
		sprintf(cc, "mt distribution, %.2lf<y<%.2lf", fYminv[ind], fYmaxv[ind]);
		sprintf(cc2, "f(m_{T}, %.2lf<y<%.2lf", fYminv[ind], fYmaxv[ind]);
		histo1DIntervals[ind][CurrentIndex] = new TH1F(cc2,
			    cc, 
			    200, 0., 2.5);
		histo1DIntervals[ind][CurrentIndex]->SetXTitle("m_{T} - m_{0} (GeV)");
		histo1DIntervals[ind][CurrentIndex]->SetYTitle("Entries");
		CurrentIndex++;
		
		sprintf(cc, "Model mt distribution, %.2lf<y<%.2lf", fYminv[ind], fYmaxv[ind]);
		sprintf(cc2, "Model f(m_{T}), %.2lf<y<%.2lf", fYminv[ind], fYmaxv[ind]);
		histo1DIntervals[ind][CurrentIndex] = new TH1F(cc2,
			    cc, 
			    200, 0., 2.5);
		histo1DIntervals[ind][CurrentIndex]->SetXTitle("m_{T} - m_{0} (GeV)");
		histo1DIntervals[ind][CurrentIndex]->SetYTitle("Entries");
		CurrentIndex++;
		
		sprintf(cc, "mt2 distribution, %.2lf<y<%.2lf", fYminv[ind], fYmaxv[ind]);
		sprintf(cc2, "f2(m_{T}, %.2lf<y<%.2lf", fYminv[ind], fYmaxv[ind]);
		histo1DIntervals[ind][CurrentIndex] = new TH1F(cc2,
			    cc, 
			    40, 0., 2.5);
		histo1DIntervals[ind][CurrentIndex]->SetXTitle("m_{T} - m_{0} (GeV)");
		histo1DIntervals[ind][CurrentIndex]->SetYTitle("#frac{1}{N} #frac{dN}{m_{T} dm_{T}} [(GeV)^{-2}]");
		CurrentIndex++;
		
		sprintf(cc, "Model mt2 distribution, %.2lf<y<%.2lf", fYminv[ind], fYmaxv[ind]);
		sprintf(cc2, "Model f2(m_{T}), %.2lf<y<%.2lf", fYminv[ind], fYmaxv[ind]);
		histo1DIntervals[ind][CurrentIndex] = new TH1F(cc2,
			    cc, 
			    200, 0., 2.5);
		histo1DIntervals[ind][CurrentIndex]->SetXTitle("m_{T} - m_{0} (GeV)");
		histo1DIntervals[ind][CurrentIndex]->SetYTitle("#frac{1}{N} #frac{dN}{m_{T} dm_{T}} [(GeV)^{-2}]");
		CurrentIndex++;
		
		sprintf(cc, "Model mt2 distribution 4Pi, %.2lf<y<%.2lf", fYminv[ind], fYmaxv[ind]);
		sprintf(cc2, "Model f2(m_{T}) 4Pi, %.2lf<y<%.2lf", fYminv[ind], fYmaxv[ind]);
		histo1DIntervals[ind][CurrentIndex] = new TH1F(cc2,
			    cc, 
			    200, 0., 2.5);
		histo1DIntervals[ind][CurrentIndex]->SetXTitle("m_{T} - m_{0} (GeV)");
		histo1DIntervals[ind][CurrentIndex]->SetYTitle("#frac{1}{N} #frac{dN}{m_{T} dm_{T}} [(GeV)^{-2}]");
		CurrentIndex++;
		
		gDirectory->cd("..");
	}
	grTy = new TGraphErrors();
    grTy->SetTitle("T(y)");
	grTy->GetXaxis()->SetTitle("y");
    grTy->GetYaxis()->SetTitle("T(y) (MeV)");
	grTy->SetName(TString("T(y)-") + fModeName);
	gDirectory->Add(grTy);
  }
	
	
  gDirectory->cd("..");
  gDirectory->cd("..");
  gDirectory->cd("..");
  
  gDirectory = currentDir;
}

void CbmBoltzmannDistribution::ReInit(FairRootManager *fManger)
{
  flistMCTracks = dynamic_cast<TClonesArray*>( fManger->GetObject("MCTrack") );
}

void CbmBoltzmannDistribution::Init()
{
  //ReInit();
}

void CbmBoltzmannDistribution::Exec()
{
  if (fRecoLevel==-1 && !flistMCTracks) return;
  if (fRecoLevel!=-1 && !fTopoReconstructor) return;

  CalculateAveragesInEvent(fRecoLevel, 1);

  events++;
  totalEvents++;
  
  //if (events%10==0) std::cout << "Event # " << events << "\n";
  
  if (events%fEventStats==0) {
  
	  double T = model->GetT(paramLocal / totalLocal);
	  
	  histo1D[IndexT]->Fill(T);
	  
	  events = 0;
	  paramLocal = 0.;
	  totalLocal = 0;
	  
	  for(unsigned int i=0;i<fYminv.size();++i) {
		if (totalLocalInterval[i]>0) histo1DIntervals[i][IndexT]->Fill(modelsY[i]->GetT(paramLocalInterval[i] / totalLocalInterval[i]));
		//std::cout << events << " " << paramLocalInterval[i] / totalLocalInterval[i] << "\n";
		paramLocalInterval[i] = 0.;
		totalLocalInterval[i] = 0;
	  }
  }
}

void CbmBoltzmannDistribution::Finish(){

  double T = model->GetT(paramGlobal / totalGlobal);
  std::cout << "T = " << T << "\t<m_T> = " << paramGlobal / totalGlobal << "\n";
  
  histo1D[IndexMt]->Sumw2();
  histo1D[IndexMt2]->Sumw2();
  histo1D[IndexMt]->Scale(1./ totalGlobal / histo1D[IndexMt]->GetXaxis()->GetBinWidth(1));
  histo1D[IndexMt2]->Scale(1./ totalGlobal / histo1D[IndexMt2]->GetXaxis()->GetBinWidth(1));
  for(int n = 1; n < histo1D[IndexMt2]->GetNbinsX(); n++) {
	histo1D[IndexMt2]->SetBinContent(n, histo1D[IndexMt2]->GetBinContent(n)/ (histo1D[IndexMt2]->GetXaxis()->GetBinCenter(n) + fMass));
	histo1D[IndexMt2]->SetBinError(n, histo1D[IndexMt2]->GetBinError(n)/ (histo1D[IndexMt2]->GetXaxis()->GetBinCenter(n) + fMass));
  }
  histodndy->Scale(1./ totalEvents / histodndy->GetXaxis()->GetBinWidth(1));
  
  for(int n = 1; n < histo1D[IndexModelMt]->GetNbinsX(); n++) {
	  histo1D[IndexModelMt]->SetBinContent(n, model->fmt(histo1D[IndexModelMt]->GetXaxis()->GetBinCenter(n) + fMass, T));
	  histo1D[IndexModelMt2]->SetBinContent(n, model->fmt(histo1D[IndexModelMt2]->GetXaxis()->GetBinCenter(n) + fMass, T) / (histo1D[IndexModelMt2]->GetXaxis()->GetBinCenter(n) + fMass));
	  histo1D[IndexModelMt4Pi]->SetBinContent(n, modelmc->fmt(histo1D[IndexModelMt4Pi]->GetXaxis()->GetBinCenter(n) + fMass, T) / (histo1D[IndexModelMt4Pi]->GetXaxis()->GetBinCenter(n) + fMass));
  }
  
  std::vector<double> ys(0), dndys(0), dndyerrs(0);
  int grindex = 0;
  for(unsigned int ind = 0; ind < fYminv.size(); ++ind) {
	if (totalGlobalInterval[ind]>0) T = modelsY[ind]->GetT(paramGlobalInterval[ind] / totalGlobalInterval[ind]);
	else continue;
	histo1DIntervals[ind][IndexMt]->Sumw2();
    histo1DIntervals[ind][IndexMt2]->Sumw2();
	histo1DIntervals[ind][IndexMt]->Scale(1./ totalGlobalInterval[ind] / histo1DIntervals[ind][IndexMt]->GetXaxis()->GetBinWidth(1));
	histo1DIntervals[ind][IndexMt2]->Scale(1./ totalGlobalInterval[ind] / histo1DIntervals[ind][IndexMt2]->GetXaxis()->GetBinWidth(1));
	for(int n = 1; n < histo1DIntervals[ind][IndexMt2]->GetNbinsX(); n++) {
		histo1DIntervals[ind][IndexMt2]->SetBinContent(n, histo1DIntervals[ind][IndexMt2]->GetBinContent(n)/ (histo1DIntervals[ind][IndexMt2]->GetXaxis()->GetBinCenter(n) + fMass));
		histo1DIntervals[ind][IndexMt2]->SetBinError(n, histo1DIntervals[ind][IndexMt2]->GetBinError(n)/ (histo1DIntervals[ind][IndexMt2]->GetXaxis()->GetBinCenter(n) + fMass));
	}
	double avmt = 0.;
	if (T==T && totalGlobalInterval[ind]>0) {
		for(int n = 1; n < histo1DIntervals[ind][IndexModelMt]->GetNbinsX(); n++) {
		  histo1DIntervals[ind][IndexModelMt]->SetBinContent(n, modelsY[ind]->fmt(histo1DIntervals[ind][IndexModelMt]->GetXaxis()->GetBinCenter(n) + fMass, T));
		  histo1DIntervals[ind][IndexModelMt2]->SetBinContent(n, modelsY[ind]->fmt(histo1DIntervals[ind][IndexModelMt2]->GetXaxis()->GetBinCenter(n) + fMass, T) / (histo1DIntervals[ind][IndexModelMt2]->GetXaxis()->GetBinCenter(n) + fMass));
		  histo1DIntervals[ind][IndexModelMt4Pi]->SetBinContent(n, modelmc->fmt(histo1DIntervals[ind][IndexModelMt4Pi]->GetXaxis()->GetBinCenter(n) + fMass, T) / (histo1DIntervals[ind][IndexModelMt4Pi]->GetXaxis()->GetBinCenter(n) + fMass));
		  avmt += (histo1DIntervals[ind][IndexModelMt]->GetXaxis()->GetBinCenter(n) + fMass) * modelsY[ind]->fmt(histo1DIntervals[ind][IndexModelMt]->GetXaxis()->GetBinCenter(n) + fMass, T);
		}
	}
	avmt *= histo1DIntervals[ind][IndexModelMt]->GetXaxis()->GetBinWidth(1);
	std::cout << fYminv[ind] << "<y<" << fYmaxv[ind] << "\tT = " << T << "\t<m_T> = " << paramGlobalInterval[ind] / totalGlobalInterval[ind] 
	<< "\t<m_T>_2 = " << avmt << "\n";
	double s1 = paramGlobalInterval[ind];
	double s2 = param2GlobalInterval[ind];
	int  Numb = totalGlobalInterval[ind];
	double errT = modelsY[ind]->dTdmt(paramGlobalInterval[ind] / totalGlobalInterval[ind]) * TMath::Sqrt((s2 - s1*s1 / Numb) / (Numb-1.) / Numb);
	//std::cout << T << "  " << errT << " " << (s2 - s1*s1 / Numb)  << "\n";
	if (T!=T || errT!=errT || errT>T/3.) continue;
	grTy->SetPoint(grindex, 0.5*(fYminv[ind]+fYmaxv[ind]), T*1.e3);
	grTy->SetPointError(grindex, 0.*0.5*(fYmaxv[ind]-fYminv[ind]), errT*1.e3);
	//std::cout << T << " " << errT << "\n";
	
	double A = modelsY[ind]->GetA(totalGlobalInterval[ind] / static_cast<double>(totalEvents), T);
	double errA = modelsY[ind]->GetAerror(totalGlobalInterval[ind] / static_cast<double>(totalEvents), T, sqrt(totalGlobalInterval[ind]) / static_cast<double>(totalEvents), errT);
	std::cout << "A = " << A << " error = " << errA << "\n";
	grdndyReco->SetPoint(grindex, 0.5*(fYminv[ind]+fYmaxv[ind]), A / (fYmaxv[ind]-fYminv[ind]));
	grdndyReco->SetPointError(grindex, 0.*0.5*(fYmaxv[ind]-fYminv[ind]), errA / (fYmaxv[ind]-fYminv[ind]));
	
	ys.push_back(0.5*(fYminv[ind]+fYmaxv[ind]));
	dndys.push_back(A / (fYmaxv[ind]-fYminv[ind]));
	dndyerrs.push_back(errA / (fYmaxv[ind]-fYminv[ind]));
	
	grindex++;
  }
  
  T = model->GetT(paramGlobal / totalGlobal);
  
  for(int n = 1; n < histodndymodel->GetNbinsX(); n++) {
	  histodndymodel->SetBinContent(n, model->dndy(histodndymodel->GetXaxis()->GetBinCenter(n), model->GetA(totalGlobal / static_cast<double>(totalEvents),T), T));
  }
}

void CbmBoltzmannDistribution::CalculateAveragesInEvent(int RecoLevel, bool UpdateGlobal) {
  if (RecoLevel==-1) {
	vector<CbmMCTrack> vRTracksMC;
    int nTracksMC = flistMCTracks->GetEntries();
	std::cout << "MC tracks: " << nTracksMC << "\n";
    vRTracksMC.resize(nTracksMC);
    for(int iTr=0; iTr<nTracksMC; iTr++)
      // vRTracksMC[iTr] = *( (CbmMCTrack*) flistMCTracks->At(iTr));
	  vRTracksMC[iTr] = *( static_cast<CbmMCTrack*> (flistMCTracks->At(iTr) ) );
	  
	for(int iTr=0; iTr<nTracksMC; iTr++) {
	    if (vRTracksMC[iTr].GetPdgCode()==fPDGID && vRTracksMC[iTr].GetMotherId()==-1) {
		  totalLocal++;
		  double pt = vRTracksMC[iTr].GetPt();
		  double ty = vRTracksMC[iTr].GetRapidity() - ycm;
		  double mt = TMath::Sqrt(vRTracksMC[iTr].GetMass()*vRTracksMC[iTr].GetMass() + pt*pt);
		  paramLocal += mt;
		  if (UpdateGlobal) {
			totalGlobal++;
			paramGlobal += mt;
		  }
		  histo1D[IndexMt]->Fill(mt - fMass);
		  histo1D[IndexMt2]->Fill(mt - fMass);
		  histodndy->Fill(ty);
		  for(unsigned int ind=0;ind<fYminv.size();++ind) {
			if (ty>=fYminv[ind] && ty<=fYmaxv[ind]) {
				if (UpdateGlobal) totalGlobalInterval[ind]++;
				totalLocalInterval[ind]++;
				if (UpdateGlobal) {
					paramGlobalInterval[ind] += mt;
					param2GlobalInterval[ind] += mt * mt;
				}
				paramLocalInterval[ind] += mt;
				histo1DIntervals[ind][IndexMt]->Fill(mt - fMass);
				histo1DIntervals[ind][IndexMt2]->Fill(mt - fMass);
			}
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
			//for(unsigned int part=0;part<ParticlePDGsTrack.size();++part) {
				if (iPDG==fPDGID) {
					  totalLocal++;
					  double pt = tr.Pt(ind);
					  double p = tr.P(ind);
					  double m = TDatabasePDG::Instance()->GetParticle(iPDG)->Mass();
					  double p0 = TMath::Sqrt(m*m+p*p);
					  double pz = TMath::Sqrt(p*p - pt*pt);
					  double ty = 0.5*log((p0+pz)/(p0-pz)) - ycm;
					  pz = TMath::Sqrt(m*m + pt*pt) * TMath::SinH(ty);
					  double mt = TMath::Sqrt(m*m + pt*pt);
					  
					  paramLocal += mt;
					  if (UpdateGlobal) {
						totalGlobal++;
						paramGlobal += mt;
					  }
					  histo1D[IndexMt]->Fill(mt - fMass);
					  histo1D[IndexMt2]->Fill(mt - fMass);
					  histodndy->Fill(ty);
					  for(unsigned int ind2=0;ind2<fYminv.size();++ind2) {
						if (ty>=fYminv[ind2] && ty<=fYmaxv[ind2]) {
							if (UpdateGlobal) totalGlobalInterval[ind2]++;
							totalLocalInterval[ind2]++;
							if (UpdateGlobal) {
								paramGlobalInterval[ind2] += mt;
								param2GlobalInterval[ind2] += mt * mt;
							}
							paramLocalInterval[ind2] += mt;
							histo1DIntervals[ind2][IndexMt]->Fill(mt - fMass);
							histo1DIntervals[ind2][IndexMt2]->Fill(mt - fMass);
						}
					  }
				}
			//}
		}
	}
  }
}
