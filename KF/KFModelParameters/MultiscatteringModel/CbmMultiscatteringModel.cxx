/*
 *====================================================================
 *
 *  CBM Multiscattering Model
 *  
 *  Authors: V.Vovchenko
 *
 *  e-mail : 
 *
 *====================================================================
 *
 *  Multiscattering model parameter extraction, works well only for 4-pi MC data atm
 *
 *====================================================================
 */

#include "CbmMultiscatteringModel.h"
#include "CbmL1Def.h"


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

#include "MultiscatteringModel.h"


using std::vector;
using std::ios;

using namespace std;

ClassImp(CbmMultiscatteringModel)

CbmMultiscatteringModel::CbmMultiscatteringModel(Int_t recoLevel, Int_t iVerbose, TString Mode, Int_t EventStats, KFParticleTopoReconstructor* tr, Float_t ekin_):
  CbmModelBase(tr),
  //ekin(ekin_),
  //fusePID(usePID),
  ekin(ekin_),
  p0cm(5.),
  ycm(1.),
  fUpdate(true),
  fusePID(true),
  fRecoLevel(recoLevel),
  fTrackNumber(1),
  fEventStats(EventStats),
  events(0),
  fModeName(Mode),
  outfileName(""),
  histodir(0),
  flistMCTracks(0),
  IndexSigt(0), IndexSigz(0), IndexQz(0), IndexPt(0), IndexPz(0), 
  IndexY(0), IndexModelPt(0), IndexModelPz(0), IndexModelY(0),
  IndexYPt(0), IndexModelYPt(0),
  pullsigt(0), pullsigz(0), pullqz(0),
  sigts(), sigzs(), qzs(),
  //fTrackNumber(trackNumber),
  //flistStsTracks(0),
  //flistStsTracksMatch(0),
  //fPrimVtx(0),
  //flsitGlobalTracks(0),
  //flistTofHits(0),
  PPDG(2212),
  paramsGlobal(),
  paramsLocal(),
  totalGlobal(0),
  totalLocal(0),
  kProtonMass(0.938271998),
  model(0)
//  flistRichRings(0),
//  flistTrdTracks(0),
  
{
  // fModeName = Mode;
  // fEventStats = EventStats;
  
  //events = 0;
  sigts.resize(0);
  sigzs.resize(0);
  qzs.resize(0);
  
  // PPDG = 2212;
  // kProtonMass = 0.938271998;
  
  //PDGtoIndex.clear();
  
  TDirectory *currentDir = gDirectory;
  
  //std::cout << 0.128 << "\t" << mtTall->Eval(0.128) << endl;
  //gDirectory->mkdir("KFModelParameters");
  gDirectory->cd("Models");
  
  histodir = gDirectory;
  
  gDirectory->mkdir("MultiscatteringModel");
  gDirectory->cd("MultiscatteringModel");
  gDirectory->mkdir(fModeName);
  gDirectory->cd(fModeName);
  //gDirectory->mkdir("All tracks");
  //gDirectory->cd("All tracks");
  TString tname = "PerEvent";
  if (fEventStats!=1) tname = TString("Each ") + TString::Itoa(fEventStats, 10) + TString(" events");
  gDirectory->mkdir(tname);
  gDirectory->cd(tname);
  //for(int part=0;part<p_sz;++part) {
    //gDirectory->mkdir(p_names[part]);
    //gDirectory->cd(p_names[part]);
	int CurrentIndex = 0;
	
	IndexSigt = CurrentIndex;
    histo1D[CurrentIndex] = new TH1F("sigma_t",
			    "Event-by-event sigma_t", 
			    300, 0., 3.);
    histo1D[CurrentIndex]->SetXTitle("sigma_t (GeV)");
    histo1D[CurrentIndex]->SetYTitle("Entries");
	CurrentIndex++;
    
    
	IndexSigz = CurrentIndex;
    histo1D[CurrentIndex] = new TH1F("sigma_z",
			    "Event-by-event sigma_z", 
			    300, 0., 15.);
    histo1D[CurrentIndex]->SetXTitle("sigma_z (GeV)");
    histo1D[CurrentIndex]->SetYTitle("Entries");
	CurrentIndex++;
    
	
	IndexQz = CurrentIndex;
	histo1D[CurrentIndex] = new TH1F("Qz",
			    "Event-by-event Qz", 
			    300, 0., 10.);
    histo1D[CurrentIndex]->SetXTitle("Q_{z} (GeV)");
    histo1D[CurrentIndex]->SetYTitle("Entries");
    CurrentIndex++;
	
	IndexPt = CurrentIndex;
	histo1D[CurrentIndex] = new TH1F("f(p_{t})",
			    "pt distribution", 
			    300, 0., 5.);
    histo1D[CurrentIndex]->SetXTitle("p_{t} (GeV)");
    histo1D[CurrentIndex]->SetYTitle("Entries");
    CurrentIndex++;
	
	IndexPz = CurrentIndex;
	histo1D[CurrentIndex] = new TH1F("f(p_{z})",
			    "pz distribution", 
			    300, -20., 20.);
    histo1D[CurrentIndex]->SetXTitle("p_{z} (GeV)");
    histo1D[CurrentIndex]->SetYTitle("Entries");
    CurrentIndex++;
	
	IndexY = CurrentIndex;
	histo1D[CurrentIndex] = new TH1F("dN/dy",
			    "rapidity distribution", 
			    300, -10., 10.);
    histo1D[CurrentIndex]->SetXTitle("y");
    histo1D[CurrentIndex]->SetYTitle("Entries");
    CurrentIndex++;
	
	
	IndexModelPt = CurrentIndex;
	histo1D[CurrentIndex] = new TH1F("Model f(p_{t})",
			    "Model pt distribution", 
			    300, 0., 5.);
    histo1D[CurrentIndex]->SetXTitle("p_{t} (GeV)");
    histo1D[CurrentIndex]->SetYTitle("Entries");
    CurrentIndex++;
	
	IndexModelPz = CurrentIndex;
	histo1D[CurrentIndex] = new TH1F("Model f(p_{z})",
			    "Model pz distribution", 
			    300, -20., 20.);
    histo1D[CurrentIndex]->SetXTitle("p_{z} (GeV)");
    histo1D[CurrentIndex]->SetYTitle("Entries");
    CurrentIndex++;
	
	IndexModelY = CurrentIndex;
	histo1D[CurrentIndex] = new TH1F("Model dN/dy",
			    "Model rapidity distribution", 
			    300, -10., 10.);
    histo1D[CurrentIndex]->SetXTitle("y");
    histo1D[CurrentIndex]->SetYTitle("Entries");
    CurrentIndex++;
	
	CurrentIndex = 0;
	IndexYPt = CurrentIndex;
	histo2D[CurrentIndex] = new TH2F("y-pt", "y-pt distribution",
			    100, -3., 3., 
			    100, 0., 5.);
    histo2D[CurrentIndex]->SetXTitle("y");
    histo2D[CurrentIndex]->SetYTitle("p_t (GeV)");
	CurrentIndex++;
	
	IndexModelYPt = CurrentIndex;
	histo2D[CurrentIndex] = new TH2F("Model y-pt", "Model y-pt distribution",
			    100, -3., 3., 
			    100, 0., 5.);
    histo2D[CurrentIndex]->SetXTitle("y");
    histo2D[CurrentIndex]->SetYTitle("p_t (GeV)");
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
  
  paramsLocal.resize(3);
  paramsGlobal.resize(3);
  for(int i=0;i<3;++i) {
	paramsLocal[i] = paramsGlobal[i] = 0.;
  }
  totalGlobal = totalLocal = 0;
  
  
  model = new MultiscatteringModel();
}

CbmMultiscatteringModel::~CbmMultiscatteringModel()
{
	if (model!=NULL) delete model;
}

void CbmMultiscatteringModel::ReInit(FairRootManager *fManger)
{
  flistMCTracks = dynamic_cast<TClonesArray*>( fManger->GetObject("MCTrack") );
}

void CbmMultiscatteringModel::Init()
{
  //ReInit();
}

void CbmMultiscatteringModel::Exec()
{
  if (fRecoLevel==-1 && !flistMCTracks) return;
  if (fRecoLevel!=-1 && !fTopoReconstructor) return;

  CalculateAveragesInEvent(fRecoLevel, 1);

  events++;
  
  if (events%fEventStats==0) {
  
      std::cout << paramsLocal[0] / totalLocal << "\t" << paramsLocal[1] / totalLocal << "\t" << paramsLocal[2] / totalLocal << "\n";
	  double sigt = model->GetSigt(paramsLocal[0] / totalLocal);
	  double sigz = model->GetSigz(paramsLocal[1] / totalLocal, paramsLocal[2] / totalLocal);
	  double qz = model->GetQz(paramsLocal[1] / totalLocal, paramsLocal[2] / totalLocal);
	  
	  histo1D[IndexSigt]->Fill(sigt);
	  histo1D[IndexSigz]->Fill(sigz);
	  histo1D[IndexQz]->Fill(p0cm-qz);
	  
	  std::cout << "sigt = " << sigt << "\n";
	  std::cout << "sigz = " << sigz << "\n";
	  std::cout << "qz   = " << p0cm-qz   << "\n";
	  
	  events = 0;
	  for(int i=0;i<3;++i) {
		paramsLocal[i] = 0.;
	  }
	  totalLocal = 0;
  }
}

void CbmMultiscatteringModel::Finish(){

  double sigt = model->GetSigt(paramsGlobal[0] / totalGlobal);
  double sigz = model->GetSigz(paramsGlobal[1] / totalGlobal, paramsGlobal[2] / totalGlobal);
  double qz = model->GetQz(paramsGlobal[1] / totalGlobal, paramsGlobal[2] / totalGlobal);
  std::cout << "sigt = " << sigt << "\n";
  std::cout << "sigz = " << sigz << "\n";
  std::cout << "qz   = " << p0cm-qz   << "\n";
  
  histo1D[IndexPt]->Scale(1./ totalGlobal / histo1D[IndexPt]->GetXaxis()->GetBinWidth(1));
  histo1D[IndexPz]->Scale(1./ totalGlobal / histo1D[IndexPz]->GetXaxis()->GetBinWidth(1));
  histo1D[IndexY]->Scale(1./ totalGlobal / histo1D[IndexY]->GetXaxis()->GetBinWidth(1));
  histo2D[IndexYPt]->Scale(1./ totalGlobal / histo2D[IndexYPt]->GetXaxis()->GetBinWidth(1) / histo2D[IndexYPt]->GetYaxis()->GetBinWidth(1));
  
  for(int n = 0; n < histo1D[IndexModelPt]->GetNbinsX(); n++) {
	  histo1D[IndexModelPt]->SetBinContent(n, model->fpt(histo1D[IndexModelPt]->GetXaxis()->GetBinCenter(n), sigt));
  }
  
  for(int n = 0; n < histo1D[IndexModelPz]->GetNbinsX(); n++) {
	  histo1D[IndexModelPz]->SetBinContent(n, model->fpz(histo1D[IndexModelPz]->GetXaxis()->GetBinCenter(n), sigz, qz));
  }
  
  for(int n = 0; n < histo1D[IndexModelY]->GetNbinsX(); n++) {
	  histo1D[IndexModelY]->SetBinContent(n, model->dndy(histo1D[IndexModelY]->GetXaxis()->GetBinCenter(n), kProtonMass, sigt, sigz, qz));
  }
  
  for(int nx = 0; nx < histo2D[IndexModelYPt]->GetNbinsX(); nx++) {
	  for(int ny = 0; ny < histo2D[IndexModelYPt]->GetNbinsY(); ny++) {
		  histo2D[IndexModelYPt]->SetBinContent(nx, ny, model->fypt(histo2D[IndexModelYPt]->GetXaxis()->GetBinCenter(nx), 
			histo2D[IndexModelYPt]->GetYaxis()->GetBinCenter(ny), kProtonMass, sigt, sigz, qz));
	  }
  }
}

void CbmMultiscatteringModel::CalculateAveragesInEvent(int RecoLevel, bool UpdateGlobal) {
  if (RecoLevel==-1) {
	vector<CbmMCTrack> vRTracksMC;
    int nTracksMC = flistMCTracks->GetEntries();
	std::cout << "MC tracks: " << nTracksMC << "\n";
    vRTracksMC.resize(nTracksMC);
    for(int iTr=0; iTr<nTracksMC; iTr++)
      //vRTracksMC[iTr] = *( (CbmMCTrack*) flistMCTracks->At(iTr));
	  vRTracksMC[iTr] = *( static_cast<CbmMCTrack*>( flistMCTracks->At(iTr)) );
	  
	for(int iTr=0; iTr<nTracksMC; iTr++) {
      //for(unsigned int part=0;part<ParticlePDGsTrack.size();++part) {
	    if (vRTracksMC[iTr].GetPdgCode()==PPDG && vRTracksMC[iTr].GetMotherId()==-1) {
		  totalLocal++;
		  double pt = vRTracksMC[iTr].GetPt();
		  double ty = vRTracksMC[iTr].GetRapidity() - ycm;
		  double pz = TMath::Sqrt(vRTracksMC[iTr].GetMass()*vRTracksMC[iTr].GetMass() + pt*pt) * TMath::SinH(ty);
		  paramsLocal[0] += pt;
		  paramsLocal[1] += pz*pz;
		  paramsLocal[2] += pz*pz*pz*pz;
		  if (UpdateGlobal) {
			totalGlobal++;
			paramsGlobal[0] += pt;
			paramsGlobal[1] += pz*pz;
			paramsGlobal[2] += pz*pz*pz*pz;
		  }
		  histo1D[IndexPt]->Fill(pt);
		  histo1D[IndexPz]->Fill(pz);
		  histo1D[IndexY]->Fill(ty);
		  histo2D[IndexYPt]->Fill(ty,pt);
	    }
      //}
    }
  }
  else {
	for(int itype = 2; itype <= 3; ++itype) {
		const KFPTrackVector &tr = fTopoReconstructor->GetTracks() [itype];
		const kfvector_int &pdgs = tr.PDG();
		for(unsigned int ind=0; ind < pdgs.size(); ++ind) {
			int iPDG = pdgs[ind];
			//for(unsigned int part=0;part<ParticlePDGsTrack.size();++part) {
				if (iPDG==PPDG) {
					  totalLocal++;
					  double pt = tr.Pt(ind);
					  double p = tr.P(ind);
					  double m = TDatabasePDG::Instance()->GetParticle(iPDG)->Mass();
					  double p0 = TMath::Sqrt(m*m+p*p);
					  double ty = 0.5*log((p0+p)/(p0-p)) - ycm;
					  double pz = TMath::Sqrt(m*m + pt*pt) * TMath::SinH(ty);
					  paramsLocal[0] += pt;
					  paramsLocal[1] += pz*pz;
					  paramsLocal[2] += pz*pz*pz*pz;
					  if (UpdateGlobal) {
						totalGlobal++;
						paramsGlobal[0] += pt;
						paramsGlobal[1] += pz*pz;
						paramsGlobal[2] += pz*pz*pz*pz;
					  }
					  histo1D[IndexPt]->Fill(pt);
					  histo1D[IndexPz]->Fill(pz);
					  histo1D[IndexY]->Fill(ty);
					  histo2D[IndexYPt]->Fill(ty,pt);
				}
			//}
		}
	}
  }
}
