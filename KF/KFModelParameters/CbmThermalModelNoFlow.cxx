/*
 *====================================================================
 *
 *  CBM Thermal Model, No Flow. This module is obsolete, to be completely replaced by CbmBoltzmannDistribution
 *  
 *  Authors: V.Vovchenko
 *
 *  e-mail : new
 *
 *====================================================================
 *
 *  Boltzmann distribution calculations
 *
 *====================================================================
 */

#include "CbmThermalModelNoFlow.h"
#include "CbmL1Def.h"


#include "CbmKFVertex.h"
#include "CbmStsTrack.h"

#include "TClonesArray.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TMath.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TSpline.h"

#include "CbmVertex.h"

#include "L1Field.h"

#include "TStopwatch.h"
#include <iostream>
#include <cstdio>
#include <cmath>
#include <algorithm>
#include <map>

#include "CbmTrackMatch.h"
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



using std::vector;
using std::ios;

namespace ThermalModelNoFlowNamespace {

	#include "NumericalIntegration.h"

	const Double_t kProtonMass = 0.938271998;
	const int p_sz1 = 2;
	//const int p_sz = 4;
	const TString p_names[p_sz1] = {"pi-", "pi+"};//, "K-", "K+"};//, "p"};
	const TString p_names_gr[p_sz1] = {"#pi^{-}", "#pi^{+}"};//, "K^{-}", "K^{+}"};//, "p"};
	const int pdgIds[p_sz1] = {-211, 211};//, -321, 321};//, 2212};
	const Double_t GeVtoifm = 5.06423;

	const int recoLevels = 4;
	const TString LevelNames[recoLevels] = {"Level 0", "Level 1", "Level 2", "Level 3"};
	
	Double_t AcceptanceFunction::getAcceptance(const Double_t & y, const Double_t & pt) const {
		double ret = sfunc.Eval(y, pt);
		if (ret<0.) ret = 0.;
		if (ret>1.) ret = 1.;
		return ret;
	}
	
	class ThermalDistributionFunction {

	public:

	  ThermalDistributionFunction(int part, double T_, double R_, double ekin_, AcceptanceFunction *af_=NULL, ReconstructionEfficiencyFunction *rf_=NULL) : 
		  fT(T_), fV(4./3.*TMath::Pi()*R_*R_*R_), mass(0.), ekin(ekin_), ycm(0.), af(af_), rf(rf_)
	  {
			mass = TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass();
			double v = sqrt(2.*kProtonMass*ekin+ekin*ekin)/(2.*kProtonMass+ekin);
			ycm = 0.5*log((1 + v)/(1 - v));//TMath::ATanH(v);
	  }

	  ~ThermalDistributionFunction() {}
	  
	  double dndycm(double y) const {
		  double ret = 0.;
		  double dpt = 0.05;
		  double tmpmt = 0.;
		  double tmpr = 0.;
		  for(double pt=0.5*dpt;pt<6.;pt+=dpt) {
			tmpmt = sqrt(pt*pt+mass*mass);
			tmpr = pt * tmpmt * TMath::CosH(y) * TMath::Exp(-tmpmt * TMath::CosH(y) / fT);
			if (af!=NULL) tmpr *= af->getAcceptance(y + ycm, pt);
			if (rf!=NULL) {
			    double tp = tmpmt * TMath::CosH(y + ycm);
				tp = sqrt(tp*tp - mass*mass);
				tmpr *= rf->f(tp);
			}
			ret += tmpr;
		  }
		  ret *= dpt * GeVtoifm * GeVtoifm * GeVtoifm * fV / 4. / TMath::Pi() / TMath::Pi();
		  return ret;
	  }
	  
	  double dndylab(double y) const {
		  double dpt = 0.025;
		  double tmpmt = 0.;
		  double tmpr = 0.;
		  double ret = 0.;
		  double ss2 = 0.5*sqrt(2*kProtonMass*(ekin+2*kProtonMass));
		  double v = sqrt(1.-kProtonMass*kProtonMass/ss2/ss2);
		  y = y - TMath::ATanH(v);

		  for(double pt=0.5*dpt;pt<6.;pt+=dpt) {
			tmpmt = sqrt(pt*pt+mass*mass);
			tmpr = pt * tmpmt * TMath::CosH(y) * TMath::Exp(-tmpmt * TMath::CosH(y) / fT);
			if (af!=NULL) tmpr *= af->getAcceptance(y + ycm, pt);
			if (rf!=NULL) {
			    double tp = tmpmt * TMath::CosH(y + ycm);
				tp = sqrt(tp*tp - mass*mass);
				tmpr *= rf->f(tp);
			}
			ret += tmpr;
		  }
		  ret *= dpt * GeVtoifm * GeVtoifm * GeVtoifm * fV / 4. / TMath::Pi() / TMath::Pi();
		  return ret;
	  }
	  
	  double dndybinlab(double ymin, double ymax, int itery) const {
		double dy = (ymax - ymin) / itery;
		double ty = 0., ret = 0., ret2 = 0.;
			for(int iy=0;iy<itery;++iy) {
				ty = ymin + dy*iy + dy*0.5;
				ret2 += 1.;
				ret += dndylab(ty);
			}
		return ret / ret2;
	  }

	  double dndydptcm(double y, double pt) const {
		  double tmpmt = sqrt(pt*pt + mass*mass);
		  double ret = GeVtoifm * GeVtoifm * GeVtoifm * fV / 4. / TMath::Pi() / TMath::Pi() * pt * tmpmt * TMath::CosH(y) * TMath::Exp(-tmpmt * TMath::CosH(y) / fT);
		  if (af!=NULL) ret *= af->getAcceptance(y + ycm, pt);
		  if (rf!=NULL) {
			  double tp = tmpmt * TMath::CosH(y + ycm);
			  tp = sqrt(tp*tp - mass*mass);
			  ret *= rf->f(tp);
		  }
		  return ret;
	  }
	  
	  double dndydptlab(double y, double pt) const {
		  double tmpmt = sqrt(pt*pt + mass*mass);
		  double ss2 = 0.5*sqrt(2*kProtonMass*(ekin+2*kProtonMass));
		  double v = sqrt(1.-kProtonMass*kProtonMass/ss2/ss2);
		  double ret = 1.;
		  y = y - TMath::ATanH(v);
		  ret *= GeVtoifm * GeVtoifm * GeVtoifm * fV / 4. / TMath::Pi() / TMath::Pi() * pt * tmpmt * TMath::CosH(y) * TMath::Exp(-tmpmt * TMath::CosH(y) / fT);
		  if (af!=NULL) ret *= af->getAcceptance(y + ycm, pt);
		  if (rf!=NULL) {
			  double tp = tmpmt * TMath::CosH(y + ycm);
			  tp = sqrt(tp*tp - mass*mass);
			  ret *= rf->f(tp);
		  }
		  return ret;
	  }
	  
	  double dndydptbin(double ymin, double ymax, int itery, double ptmin, double ptmax, int iterpt) const {
		double dpt = (ptmax - ptmin) / iterpt;
		double dy = (ymax - ymin) / itery;
		double ty = 0., tpt = 0., ret = 0., ret2 = 0.;
		double tmpmt = 0.;
		double tmpr = 0.;
		double ss2 = 0.5*sqrt(2*kProtonMass*(ekin+2*kProtonMass));
		double v = sqrt(1.-kProtonMass*kProtonMass/ss2/ss2);
		ymin = ymin - TMath::ATanH(v);
		for(int ipt=0;ipt<iterpt;++ipt) {
			tpt = ptmin + dpt*ipt + dpt*0.5;
			tmpmt = sqrt(tpt*tpt + mass*mass);
			for(int iy=0;iy<itery;++iy) {
				ty = ymin + dy*iy + dy*0.5;
				tmpr = GeVtoifm * GeVtoifm * GeVtoifm * fV / 4. / TMath::Pi() / TMath::Pi() * tpt * tmpmt * TMath::CosH(ty) * TMath::Exp(-tmpmt * TMath::CosH(ty) / fT);
				ret2 += 1.;
				if (af!=NULL) tmpr *= af->getAcceptance(ty + ycm, tpt);
				if (rf!=NULL) {
					double tp = tmpmt * TMath::CosH(ty + ycm);
					tp = sqrt(tp*tp - mass*mass);
					tmpr *= rf->f(tp);
				}
				ret += tmpr;
			}
		}
		return ret / ret2;
	  }
	  
	  static double chi2func(double y, double pt, double m) {
		return sqrt(pt*pt+m*m);//y*y;//pt*pt + m*m;
	  }

	  // Function to determine temperature
	  static double tempCrit(double y, double pt, double m) {
			return sqrt(pt*pt+m*m);//*cosh(y);
	  }
	  
	  double tempCritAv() {
		  if (af==NULL) {
			  double ret1 = 0., ret2 = 0.;
			  double ymin = -3., ymax = 6.;
			  int itery = 500;
			  double dy = (ymax - ymin) / itery;
			  double ptmin = 0., ptmax = 3.;
			  int iterpt = 400;
			  double dpt = (ptmax - ptmin) / iterpt;
			  double ty = 0., tpt = 0., tmp = 0.;
			  double tp = 0.;
			  for(int iy=0;iy<itery;++iy) {
				ty = ymin + (iy+0.5)*dy;
				for(int ipt=0;ipt<iterpt;++ipt) {
					tpt = ptmin + (ipt+0.5)*dpt;
					tmp = tpt * sqrt(tpt*tpt + mass*mass) * TMath::CosH(ty-ycm) * TMath::Exp(-sqrt(tpt*tpt + mass*mass)*TMath::CosH(ty-ycm)/fT);
					ret1 += tmp;
					ret2 += tmp * tempCrit(ty, tpt, mass);
					if (rf!=NULL) {
						tp = sqrt(mass*mass*TMath::SinH(ty)*TMath::SinH(ty) + tpt*tpt*TMath::CosH(ty)*TMath::CosH(ty));
						ret1 *= rf->f(tp);
						ret2 *= rf->f(tp);
					}
				}
			  }
			  return ret2 / ret1;
		 }
		 else {
			  if (af->ys.size()==0) return -1.;
			  double ret1 = 0., ret2 = 0.;
			  double tmp = 0., tp = 0.;
			  for(unsigned int i=0;i<af->ys.size();++i) {
				tp = sqrt(af->pts[i]*af->pts[i] + mass*mass) * cosh(af->ys[i]);
				tp = sqrt(tp*tp - mass*mass);
				tmp = af->pts[i] * sqrt(af->pts[i]*af->pts[i]+ mass*mass) * cosh(af->ys[i]-ycm) * exp(-sqrt(af->pts[i]*af->pts[i] + mass*mass)*cosh(af->ys[i]-ycm)/fT) 
				  * af->probs[i];
				if (rf!=NULL) tmp *= rf->f(tp);
				ret1 += tmp;
				ret2 += tmp * tempCrit(af->ys[i], af->pts[i], mass);
			  }
			  return ret2 / ret1;
		 }
	  }
	  
	  double chi2FuncAv() {
		  if (af==NULL) {
			  double ret1 = 0., ret2 = 0.;
			  double ymin = -3., ymax = 6.;
			  int itery = 500;
			  double dy = (ymax - ymin) / itery;
			  double ptmin = 0., ptmax = 3.;
			  int iterpt = 400;
			  double dpt = (ptmax - ptmin) / iterpt;
			  double ty = 0., tpt = 0., tmp = 0.;
			  double tp = 0.;
			  for(int iy=0;iy<itery;++iy) {
				ty = ymin + (iy+0.5)*dy;
				for(int ipt=0;ipt<iterpt;++ipt) {
					tpt = ptmin + (ipt+0.5)*dpt;
					tmp = tpt * sqrt(tpt*tpt + mass*mass) * TMath::CosH(ty-ycm) * TMath::Exp(-sqrt(tpt*tpt + mass*mass)*TMath::CosH(ty-ycm)/fT);
					ret1 += tmp;
					ret2 += tmp * chi2func(ty, tpt, mass);
					if (rf!=NULL) {
						tp = sqrt(mass*mass*TMath::SinH(ty)*TMath::SinH(ty) + tpt*tpt*TMath::CosH(ty)*TMath::CosH(ty));
						ret1 *= rf->f(tp);
						ret2 *= rf->f(tp);
					}
				}
			  }
			  return ret2 / ret1;
		 }
		 else {
			  if (af->ys.size()==0) return -1.;
			  double ret1 = 0., ret2 = 0.;
			  double tmp = 0., tp = 0.;
			  for(unsigned int i=0;i<af->ys.size();++i) {
				tp = sqrt(af->pts[i]*af->pts[i] + mass*mass) * cosh(af->ys[i]);
				tp = sqrt(tp*tp - mass*mass);
				tmp = af->pts[i] * sqrt(af->pts[i]*af->pts[i]+ mass*mass) * cosh(af->ys[i]-ycm) * exp(-sqrt(af->pts[i]*af->pts[i] + mass*mass)*cosh(af->ys[i]-ycm)/fT) 
				  * af->probs[i];
				if (rf!=NULL) tmp *= rf->f(tp);
				ret1 += tmp;
				ret2 += tmp * chi2func(af->ys[i], af->pts[i], mass);
			  }
			  return ret2 / ret1;
		 }
	  }

	  double T() const {return fT;}
	  double V() const {return fV;}
	  
	  ThermalDistributionFunction(const ThermalDistributionFunction&);
	  ThermalDistributionFunction& operator=(const ThermalDistributionFunction&);

	private:
	  double fT, fV;
	  double mass;
	  double ekin;
	  double ycm;
	  AcceptanceFunction *af;
	  ReconstructionEfficiencyFunction *rf;
	};
	
	class ThermalChi2Func {

	public:
	  ThermalChi2Func(TH1F *dndyexp, TH2F *dndydptexp, double Norm_) : Norm(Norm_), dndyHist(0), dndydptHist(0)
	  { 
		  dndyHist = dndyexp; 
		  dndydptHist = dndydptexp; 
	  }

	  ~ThermalChi2Func() {}
	  
	  double chi2dndy(int part, double T_, double R_, double ekin_, AcceptanceFunction *af_=NULL, ReconstructionEfficiencyFunction *rf_=NULL, double systerr = 0.) const {
		ThermalDistributionFunction pl(part, T_, R_, ekin_, af_, rf_);
		double chi2 = 0.;
		double tmpval = 0., tmpval2 = 0., tmpvar = 0.;
		
		int iters = 0;
		

		for(int n = 0; n < dndyHist->GetNbinsX(); n++) {
		  tmpval = dndyHist->GetBinContent(n);
		  if (tmpval>100.) {
			iters++;
			tmpval /= Norm * dndyHist->GetXaxis()->GetBinWidth(n);
			tmpvar = dndyHist->GetBinError(n) / Norm / dndyHist->GetXaxis()->GetBinWidth(n);
			tmpvar = sqrt(tmpvar*tmpvar + systerr*systerr*tmpval*tmpval);
			tmpval2 = pl.dndybinlab(dndyHist->GetXaxis()->GetBinLowEdge(n), dndyHist->GetXaxis()->GetBinUpEdge(n), 10);
			chi2 += (tmpval - tmpval2) * (tmpval - tmpval2) / tmpvar / tmpvar;///fMVariances[n]);
		  }
		}
		
		chi2 /= iters;
		
		return chi2;
	  }
	  
	  double chi2ypt(int part, double T_, double R_, double ekin_, AcceptanceFunction *af_=NULL, ReconstructionEfficiencyFunction *rf_=NULL, double systerr = 0.) const {
		ThermalDistributionFunction pl(part, T_, R_, ekin_, af_, rf_);
		double chi2 = 0.;
		int iters = 0;
		double tmpval = 0., tmpval2 = 0., tmpvar = 0.;
		for(int nx = 0; nx < dndydptHist->GetNbinsX(); nx++) {
		  for(int ny = 0; ny < dndydptHist->GetNbinsY(); ny++) {
			  tmpval = dndydptHist->GetBinContent(nx, ny);
			  if (tmpval>5.) {
				iters++;
				tmpval /= Norm * dndydptHist->GetXaxis()->GetBinWidth(nx) * dndydptHist->GetYaxis()->GetBinWidth(ny);
				tmpvar = dndydptHist->GetBinError(nx,ny) / Norm / dndydptHist->GetXaxis()->GetBinWidth(nx) / dndydptHist->GetYaxis()->GetBinWidth(ny);
				tmpvar = sqrt(tmpvar*tmpvar + systerr*systerr*tmpval*tmpval);
				tmpval2 = pl.dndydptbin(dndydptHist->GetXaxis()->GetBinLowEdge(nx), dndydptHist->GetXaxis()->GetBinUpEdge(nx), 
							10, dndydptHist->GetYaxis()->GetBinLowEdge(ny), dndydptHist->GetYaxis()->GetBinUpEdge(ny), 10);//pl.dndyexp(dndyHist->GetBinCenter(n));
				chi2 += (tmpval - tmpval2) * (tmpval - tmpval2) / tmpvar / tmpvar;///fMVariances[n]);
			  }
		  }
		}
		return chi2 / iters;
	  }
	  
	  ThermalChi2Func(const ThermalChi2Func&);
	  ThermalChi2Func& operator=(const ThermalChi2Func&);
	  
	  private:
		double Norm;
		TH1F *dndyHist;
		TH2F *dndydptHist;
	};
}


using namespace ThermalModelNoFlowNamespace;
using namespace std;

ClassImp(CbmThermalModelNoFlow)

CbmThermalModelNoFlow::CbmThermalModelNoFlow(Float_t ekin_, Int_t recoLevel, Int_t usePID, Int_t trackNumber, Int_t iVerbose):
  ekin(ekin_),
  ycm(1.),
  fUpdate(true),
  fusePID(usePID),
  fRecoLevel(recoLevel),
  fTrackNumber(trackNumber),
  flistStsPts(0),
  flistTofPts(0),
  flistStsTracksMatch(0),
  flistStsTracks(0),
  fPrimVtx(0),
  outfileName(""),
  fChiToPrimVtx(),
  NPrimGlobalMC(0),
  NPrimGlobalReco(0),
  flistMCTracks(0),
  flsitGlobalTracks(0),
  flistTofHits(0),
  flistTofPoints(0),
  histodir(0),
  events(0),
  AcceptanceSTS(),
  AcceptanceSTSTOF()
{
  NPrimGlobalMC = 0;
  NPrimGlobalReco = 0;
  
  double pbeam = sqrt((kProtonMass+ekin)*(kProtonMass+ekin)-kProtonMass*kProtonMass);
  double betacm = pbeam / (2.*kProtonMass+ekin);
  ycm = 0.5*log((1.+betacm)/(1.-betacm));
  
  for(int part=0;part<p_sz;++part) ComputeThermalDependence(part);

  /*globalmtavMC = new Double_t[p_sz];
  globalmt2avMC = new Double_t[p_sz];
  globalfavMC = new Double_t[p_sz];
  globalf2avMC = new Double_t[p_sz];
  globalmtmomerrMC = new Double_t[p_sz];
  globalnTracksMC = new Int_t[p_sz];*/
  for(int part = 0;part<p_sz;++part) {
    globalmtavMC[part] = 0.;
    globalmt2avMC[part] = 0.;
	globalfavMC[part] = 0.;
    globalf2avMC[part] = 0.;
    globalmtmomerrMC[part] = 0.;
    globalnTracksMC[part] = 0;
  }
  
  //for(Int_t i=0;i<recoLevels;++i)  {
    /*globalmtavReco = new Double_t[p_sz];
    globalmt2avReco = new Double_t[p_sz];
	globalfavReco = new Double_t[p_sz];
    globalf2avReco = new Double_t[p_sz];
    globalnTracksReco = new Int_t[p_sz];
    globalmtmomerrReco = new Double_t[p_sz];*/
    for(int part = 0;part<p_sz;++part) {
      globalmtavReco[part] = 0.;
      globalmt2avReco[part] = 0.;
	  globalfavReco[part] = 0.;
      globalf2avReco[part] = 0.;
	  //globalmt4avReco[part] = 0.;
      globalmtmomerrReco[part] = 0.;
      globalnTracksReco[part] = 0;
    }
  //}
  
  
  TDirectory *currentDir = gDirectory;
  
  gDirectory->cd("Models");
  
  histodir = gDirectory;
  
  gDirectory->mkdir("BoltzmannDistribution");
  gDirectory->cd("BoltzmannDistribution");
  gDirectory->mkdir("All tracks");
  gDirectory->cd("All tracks");
  gDirectory->mkdir("PerEvent");
  gDirectory->cd("PerEvent");
  /*hTempFullMC = new TH1F*[1];
  hTempErrStatFullMC = new TH1F*[1];
  hTempErrMomFullMC = new TH1F*[1];
  hRadFullMC = new TH1F*[1];
  hRadErrStatFullMC = new TH1F*[1];
  hRadErrMomFullMC = new TH1F*[1];*/
  for(int part=0;part<p_sz;++part) {
    gDirectory->mkdir(p_names[part]);
    gDirectory->cd(p_names[part]);
    hTempFullMC[part] = new TH1F("Temperature " + p_names_gr[part],
			    "Event-by-event temperature", 
			    300, 0.,0.3);
    hTempFullMC[part]->SetXTitle("T (GeV)");
    hTempFullMC[part]->SetYTitle("Entries");
    
    hTempErrStatFullMC[part] = new TH1F("Temperature statistical error" + p_names_gr[part],
			    "Event-by-event temperature statistical error", 
			    100, 0.,0.1);
    hTempErrStatFullMC[part]->SetXTitle("T (GeV)");
    hTempErrStatFullMC[part]->SetYTitle("Entries");
    
    hTempErrMomFullMC[part] = new TH1F("Temperature momentum error" + p_names_gr[part],
			    "Event-by-event temperature momentum error", 
			    100, 0.,0.01);
    hTempErrMomFullMC[part]->SetXTitle("T (GeV)");
    hTempErrMomFullMC[part]->SetYTitle("Entries");
    
    hRadFullMC[part] = new TH1F("Fireball radius " + p_names_gr[part],
			    "Event-by-event radius", 
			    200, 0., 100.);
    hRadFullMC[part]->SetXTitle("R (fm)");
    hRadFullMC[part]->SetYTitle("Entries");
    
    hRadErrStatFullMC[part] = new TH1F("Fireball radius statistical error " + p_names_gr[part],
			    "Event-by-event radius statistical error", 
			    100, 0., 10.);
    hRadErrStatFullMC[part]->SetXTitle("R (fm)");
    hRadErrStatFullMC[part]->SetYTitle("Entries");
    
    hRadErrMomFullMC[part] = new TH1F("Fireball radius momentum error " + p_names_gr[part],
			    "Event-by-event radius momentum error", 
			    100, 0., 10.);
    hRadErrMomFullMC[part]->SetXTitle("R (fm)");
    hRadErrMomFullMC[part]->SetYTitle("Entries");

    gDirectory->cd("..");
  }
  gDirectory->cd("..");
  gDirectory->cd("..");
  gDirectory->cd("..");
  
  gDirectory->cd("BoltzmannDistribution");
  gDirectory->cd("All tracks");
  gDirectory->mkdir("Global");
  gDirectory->cd("Global");
  /*hfyMC = new TH1F*[1];
  hfyMCmodel = new TH1F*[1];
  hfdndydptMC = new TH2F*[1];
  hfdndydptMCmodel = new TH2F*[1];*/
  for(int part=0;part<p_sz;++part) {
    gDirectory->mkdir(p_names[part]);
    gDirectory->cd(p_names[part]);
	
    hfyMC[part] = new TH1F("dndy " + p_names_gr[part],
			    "Rapidity distribution",
			    100, -5. ,5.);
	hfyMCmodel[part] = new TH1F("dndy " + p_names_gr[part] + " model",
			    "Rapidity distribution",
			    100, -5. ,5.);
	hfdndydptMC[part] = new TH2F("dndydpt " + p_names_gr[part],
			    "YPt distribution",
			    50, -5., 5., 25, 0., 5.);
	hfdndydptMCmodel[part] = new TH2F("dndydpt " + p_names_gr[part] + " model",
			    "YPt distribution",
			    50, -5., 5., 25, 0., 5.);

    gDirectory->cd("..");
  }
  gDirectory->cd("..");
  gDirectory->cd("..");
  gDirectory->cd("..");
  
    gDirectory->cd("BoltzmannDistribution");
    gDirectory->mkdir("Reconstructible tracks");
    gDirectory->cd("Reconstructible tracks");
    gDirectory->mkdir("PerEvent");
    gDirectory->cd("PerEvent");
    
    for(int part=0;part<p_sz;++part) {
      gDirectory->mkdir(p_names[part]);
      gDirectory->cd(p_names[part]);
      hTempFullReco[part] = new TH1F("Temperature " + p_names_gr[part],
			      "Event-by-event temperature", 
			      300, 0.,0.3);
      hTempFullReco[part]->SetXTitle("T (GeV)");
      hTempFullReco[part]->SetYTitle("Entries");
      
      hTempErrStatFullReco[part] = new TH1F("Temperature statistical error " + p_names_gr[part],
			      "Event-by-event temperature statistical error ", 
			      100, 0.,0.1);
      hTempErrStatFullReco[part]->SetXTitle("T (GeV)");
      hTempErrStatFullReco[part]->SetYTitle("Entries");
      
      hTempErrMomFullReco[part] = new TH1F("Temperature momentum error " + p_names_gr[part],
			    "Event-by-event temperature momentum error", 
			    100, 0.,0.01);
      hTempErrMomFullReco[part]->SetXTitle("T (GeV)");
      hTempErrMomFullReco[part]->SetYTitle("Entries");
    
      
      hRadFullReco[part] = new TH1F("Fireball radius " + p_names_gr[part],
			      "Event-by-event radius", 
			      200, 0., 100.);
      hRadFullReco[part]->SetXTitle("R (fm)");
      hRadFullReco[part]->SetYTitle("Entries");
      
      hRadErrStatFullReco[part] = new TH1F("Fireball radius statistical error " + p_names_gr[part],
			      "Event-by-event radius statistical error ", 
			      10, 0., 10.);
      hRadErrStatFullReco[part]->SetXTitle("R (fm)");
      hRadErrStatFullReco[part]->SetYTitle("Entries");
      
      hRadErrMomFullReco[part] = new TH1F("Fireball radius momentum error " + p_names_gr[part],
			    "Event-by-event radius momentum error", 
			    100, 0., 10.);
      hRadErrMomFullReco[part]->SetXTitle("R (fm)");
      hRadErrMomFullReco[part]->SetYTitle("Entries");

      gDirectory->cd("..");
    }

    gDirectory->cd("..");
    gDirectory->cd("..");
    gDirectory->cd("..");
	
  gDirectory->cd("BoltzmannDistribution");
  gDirectory->cd("Reconstructible tracks");
  gDirectory->mkdir("Global");
  gDirectory->cd("Global");
  for(int part=0;part<p_sz;++part) {
    gDirectory->mkdir(p_names[part]);
    gDirectory->cd(p_names[part]);
	
    hfyReco[part] = new TH1F("dndy " + p_names_gr[part],
			    "Rapidity distribution",
			    100, -5. ,5.);
	hfyRecomodel[part] = new TH1F("dndy " + p_names_gr[part] + " model",
			    "Rapidity distribution",
			    100, -5. ,5.);
	hfdndydptReco[part] = new TH2F("dndydpt " + p_names_gr[part],
			    "YPt distribution",
			    50, -5., 5., 25, 0., 5.);
	hfdndydptRecomodel[part] = new TH2F("dndydpt " + p_names_gr[part] + " model",
			    "YPt distribution",
			    50, -5., 5., 25, 0., 5.);

    gDirectory->cd("..");
  }
  gDirectory->cd("..");
  gDirectory->cd("..");
  gDirectory->cd("..");
    
    gDirectory->cd("BoltzmannDistribution");
    gDirectory->mkdir("Reconstructible tracks corrected");
    gDirectory->cd("Reconstructible tracks corrected");
    gDirectory->mkdir("PerEvent");
    gDirectory->cd("PerEvent");
    for(int part=0;part<p_sz;++part) {
      gDirectory->mkdir(p_names[part]);
      gDirectory->cd(p_names[part]);
      hTempFullRecoCor[part] = new TH1F("Temperature " + p_names_gr[part],
			      "Event-by-event temperature", 
			      300, 0.,0.3);
      hTempFullRecoCor[part]->SetXTitle("T (GeV)");
      hTempFullRecoCor[part]->SetYTitle("Entries");
      
      hTempErrStatFullRecoCor[part] = new TH1F("Temperature statistical error " + p_names_gr[part],
			      "Event-by-event temperature statistical error ", 
			      100, 0.,0.1);
      hTempErrStatFullRecoCor[part]->SetXTitle("T (GeV)");
      hTempErrStatFullRecoCor[part]->SetYTitle("Entries");
      
      hTempErrMomFullRecoCor[part] = new TH1F("Temperature momentum error " + p_names_gr[part],
			    "Event-by-event temperature momentum error", 
			    100, 0.,0.01);
      hTempErrMomFullRecoCor[part]->SetXTitle("T (GeV)");
      hTempErrMomFullRecoCor[part]->SetYTitle("Entries");
      
      hRadFullRecoCor[part] = new TH1F("Fireball radius " + p_names_gr[part],
			      "Event-by-event radius", 
			      200, 0., 100.);
      hRadFullRecoCor[part]->SetXTitle("R (fm)");
      hRadFullRecoCor[part]->SetYTitle("Entries");
      
      hRadErrStatFullRecoCor[part] = new TH1F("Fireball radius statistical error " + p_names_gr[part],
			      "Event-by-event radius statistical error ", 
			      100, 0., 10.);
      hRadErrStatFullRecoCor[part]->SetXTitle("R (fm)");
      hRadErrStatFullRecoCor[part]->SetYTitle("Entries");
      
      hRadErrMomFullRecoCor[part] = new TH1F("Fireball radius momentum error " + p_names_gr[part],
			    "Event-by-event radius momentum error", 
			    100, 0., 10.);
      hRadErrMomFullRecoCor[part]->SetXTitle("R (fm)");
      hRadErrMomFullRecoCor[part]->SetYTitle("Entries");

      gDirectory->cd("..");
    }
    gDirectory->cd("..");
    gDirectory->cd("..");
    gDirectory->cd("..");
  
  gDirectory->cd("BoltzmannDistribution");
  gDirectory->cd("Reconstructible tracks corrected");
  gDirectory->mkdir("Global");
  gDirectory->cd("Global");
  for(int part=0;part<p_sz;++part) {
    gDirectory->mkdir(p_names[part]);
    gDirectory->cd(p_names[part]);
	
    hfyRecoCor[part] = new TH1F("dndy " + p_names_gr[part],
			    "Rapidity distribution",
			    100, -5. ,5.);
	hfyRecoCormodel[part] = new TH1F("dndy " + p_names_gr[part] + " model",
			    "Rapidity distribution",
			    100, -5. ,5.);
	hfdndydptRecoCor[part] = new TH2F("dndydpt " + p_names_gr[part],
			    "YPt distribution",
			    50, -5., 5., 25, 0., 5.);
	hfdndydptRecoCormodel[part] = new TH2F("dndydpt " + p_names_gr[part] + " model",
			    "YPt distribution",
			    50, -5., 5., 25, 0., 5.);

    gDirectory->cd("..");
  }
  gDirectory->cd("..");
  gDirectory->cd("..");
  gDirectory->cd("..");
  
  gDirectory = currentDir;
  //cout << "ycm = " << ycm;
  
  events = 0;
}

CbmThermalModelNoFlow::~CbmThermalModelNoFlow()
{
  
}

void CbmThermalModelNoFlow::ReadAcceptanceFunction(AcceptanceFunction & func, TString filename)
{
  double ymin = 0., ymax = 6.;
  double ptmin = 0., ptmax = 2.5;
  func.ys.resize(0);
  func.pts.resize(0);
  func.probs.resize(0);
  ifstream fin(filename.Data());
  fin >> func.dy >> func.dpt;
  double ty, tpt, prob;
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

void CbmThermalModelNoFlow::ReInit(FairRootManager *fManger)
{
  //FairRootManager *fManger = FairRootManager::Instance();

  // flistStsTracks = (TClonesArray *)  fManger->GetObject("StsTrack");
  // flistTofPts = (TClonesArray *)  fManger->GetObject("TofPoint");
  // fPrimVtx = (CbmVertex*) fManger->GetObject("PrimaryVertex");
  flistStsTracks = dynamic_cast<TClonesArray*>(  fManger->GetObject("StsTrack") );
  flistTofPts = dynamic_cast<TClonesArray*>(  fManger->GetObject("TofPoint") );
  fPrimVtx = dynamic_cast<CbmVertex*>(  fManger->GetObject("PrimaryVertex") );
  //fPrimVtx = new CbmVertex();
  //for the particle id
  flistStsTracksMatch = dynamic_cast<TClonesArray*>(  fManger->GetObject("StsTrackMatch") );
  flistMCTracks = dynamic_cast<TClonesArray*>( fManger->GetObject("MCTrack") );
  // flistStsPts = (TClonesArray *)  fManger->GetObject("StsPoint");
  // flistTofPts = (TClonesArray *)  fManger->GetObject("TofPoint");
  flistStsPts = dynamic_cast<TClonesArray*>(  fManger->GetObject("StsPoint") );
  flistTofPts = dynamic_cast<TClonesArray*>(  fManger->GetObject("TofPoint") );
  

  if (fusePID == 2){
    flsitGlobalTracks = dynamic_cast<TClonesArray*>( fManger->GetObject("GlobalTrack") );
    flistTofHits = dynamic_cast<TClonesArray*>( fManger->GetObject("TofHit") );
  }
}

void CbmThermalModelNoFlow::Init()
{
  //ReInit();
}

void CbmThermalModelNoFlow::Exec()
{
  if(!flistStsTracks) return;

  if(!flistStsTracksMatch) return;
  if(!flistStsPts) return;
  if(!flistMCTracks) return;

  vector<CbmStsTrack> vRTracks;
  vector<CbmMCTrack> vRTracksMC;
  int nTracks = flistStsTracks->GetEntries();
  int nTracksMC = flistMCTracks->GetEntries();
  vRTracks.resize(nTracks);
  for(int iTr=0; iTr<nTracks; iTr++)
    // vRTracks[iTr] = *( (CbmStsTrack*) flistStsTracks->At(iTr));
	vRTracks[iTr] = *( static_cast<CbmStsTrack*>( flistStsTracks->At(iTr)) );
  vRTracksMC.resize(nTracksMC);
  for(int iTr=0; iTr<nTracksMC; iTr++)
    // vRTracksMC[iTr] = *( (CbmMCTrack*) flistMCTracks->At(iTr));
	vRTracksMC[iTr] = *( static_cast<CbmMCTrack*>( flistMCTracks->At(iTr)) );

  CbmKFVertex kfVertex;
  if(fPrimVtx)
    kfVertex = CbmKFVertex(*fPrimVtx);
	
  vector<int> vTrackPDG(vRTracks.size(), -1);
  if(fusePID == 1)
  {
    for(int iTr=0; iTr<nTracks; iTr++)
    {
      // CbmTrackMatch* stsTrackMatch = (CbmTrackMatch*)flistStsTracksMatch->At(iTr);
	  CbmTrackMatch* stsTrackMatch = static_cast<CbmTrackMatch*> (flistStsTracksMatch->At(iTr) );
      if(stsTrackMatch -> GetNofMCTracks() == 0) continue;
      const int mcTrackId = stsTrackMatch->GetMCTrackId();
      // CbmMCTrack* mcTrack = (CbmMCTrack*)flistMCTracks->At(mcTrackId);
	  CbmMCTrack* mcTrack = static_cast<CbmMCTrack*> (flistMCTracks->At(mcTrackId) );
      vTrackPDG[iTr] = mcTrack->GetPdgCode();
    }
  }
  
  if(fusePID == 2)
  {
    if (NULL == flsitGlobalTracks) { Fatal("KF Particle Finder", "No GlobalTrack array!"); }
    if (NULL == flistTofHits) { Fatal("KF Particle Finder", "No TOF hits array!"); }

    const Double_t m2P  = 0.885;
    const Double_t m2K  = 0.245;
    const Double_t m2Pi = 0.019479835;
   
    Double_t sP[3][5] = { {0.0337428,-0.013939,0.00567602,-0.000202229,4.07531e-06},
                          {0.00717827,-0.00257353, 0.00389851,-9.83097e-05, 1.33011e-06},
                          {0.001348,0.00220126,0.0023619,7.35395e-05,-4.06706e-06} };//SIS-300
//    Double_t sP[3][5] = { {0.056908,-0.0470572,0.0216465,-0.0021016,8.50396e-05},
//                          {0.00943075,-0.00635429,0.00998695,-0.00111527,7.77811e-05},
//                          {0.00176298,0.00367263,0.00308013,0.000844013,-0.00010423} }; //SIS-100

//     sP[0][0] =  0.0618927;
//     sP[0][1] = -0.0719277;
//     sP[0][2] =  0.0396255;
//     sP[0][3] = -0.00583356;
//     sP[0][4] =  0.000317072;
// 
//     sP[1][0] =  0.0291881;
//     sP[1][1] = -0.0904978;
//     sP[1][2] =  0.086161;
//     sP[1][3] = -0.0229688;
//     sP[1][4] =  0.00199382;
// 
//     sP[2][0] =  0.162171;
//     sP[2][1] = -0.194007;
//     sP[2][2] =  0.0893264;
//     sP[2][3] = -0.014626;
//     sP[2][4] =  0.00088203;

    const Int_t PdgHypo[4] = {2212, 321, 211, -11};

    for (Int_t igt = 0; igt < flsitGlobalTracks->GetEntriesFast(); igt++) {
      const CbmGlobalTrack* globalTrack = static_cast<const CbmGlobalTrack*>(flsitGlobalTracks->At(igt));

      Int_t stsTrackIndex = globalTrack->GetStsTrackIndex();
      if( stsTrackIndex<0 ) continue;

//       Bool_t isElectronTRD = 0;
//       Bool_t isElectronRICH = 0;
//       Bool_t isElectron = 0;

      const FairTrackParam *stsPar = vRTracks[stsTrackIndex].GetParamFirst();
      TVector3 mom;
      stsPar->Momentum(mom);

      Double_t p = mom.Mag();
      Int_t q = stsPar->GetQp() > 0 ? 1 : -1;

      Double_t l = globalTrack->GetLength();
      if( !((l>800.) && (l<1400.)) ) continue;//SIS 300
//      if( !((l>580.) && (l<700.)) ) continue;//SIS 100

      Double_t time;
      Int_t tofHitIndex = globalTrack->GetTofHitIndex();
      if (tofHitIndex >= 0) {
	 vTrackPDG[stsTrackIndex] = -211;
	 continue;
         const CbmTofHit* tofHit = static_cast<const CbmTofHit*>(flistTofHits->At(tofHitIndex));
         if(!tofHit) continue;
//         time = tofHit->GetTime()-0.021;
         time = tofHit->GetTime();
      }
      else
        continue;

      if( !((time>29.) && (time<50.)) ) continue; //SIS 300
//      if( !((time>19.) && (time<42.)) ) continue; //SIS 100

      Double_t m2 = p*p*(1./((l/time/29.9792458)*(l/time/29.9792458))-1.);

      Double_t sigma[3];
      sigma[0] = sP[0][0] + sP[0][1]*p + sP[0][2]*p*p + sP[0][3]*p*p*p + sP[0][4]*p*p*p*p;
      sigma[1] = sP[1][0] + sP[1][1]*p + sP[1][2]*p*p + sP[1][3]*p*p*p + sP[1][4]*p*p*p*p;
      sigma[2] = sP[2][0] + sP[2][1]*p + sP[2][2]*p*p + sP[2][3]*p*p*p + sP[2][4]*p*p*p*p;

      Double_t dm2[3];
      dm2[0] = fabs(m2 - m2P)/sigma[0];
      dm2[1] = fabs(m2 - m2K)/sigma[1];
      dm2[2] = fabs(m2 - m2Pi)/sigma[2];

      int iPdg=2;
      Double_t dm2min = dm2[2];

//       if(isElectronRICH && isElectronTRD)
//       {
//         if (p >= 1.) {
//           if (m2 < (0.01 + (p - 1.) * 0.09))
//             isElectron = 1;
//         }
//         else {
//           if (m2 < 0.0)
//             isElectron = 1;
//         }
//       }
// 
//       if(!isElectron)
      {
        if(p>12.) continue;
        if(q>0)
        {
          if(dm2[1] < dm2min) { iPdg = 1; dm2min = dm2[1]; }
          if(dm2[0] < dm2min) { iPdg = 0; dm2min = dm2[0]; }

          if(dm2min > 2) iPdg=-1;
        }
        else
        {
          if(dm2[1] < dm2min) { iPdg = 1; dm2min = dm2[1]; }
          if((dm2min>3) && (dm2[0] < dm2min)) { iPdg = 0; dm2min = dm2[0]; }

          if(dm2min > 2) iPdg=-1;
        }

        if(iPdg > -1)
          vTrackPDG[stsTrackIndex] = q*PdgHypo[iPdg];
      }
//       else
//         vTrackPDG[stsTrackIndex] = q*PdgHypo[3];
    }
  }
  
  static int NEv=0;
  NEv++;
  events++;
  
  //cout << NEv << endl;
  
  
  CbmKFTrErrMCPoints* MCTrackSortedArray = new CbmKFTrErrMCPoints[flistMCTracks->GetEntriesFast()+1];
  for (Int_t iSts=0; iSts<flistStsPts->GetEntriesFast(); iSts++)
  {
     // CbmStsPoint* StsPoint = (CbmStsPoint*)flistStsPts->At(iSts);
	 CbmStsPoint* StsPoint = static_cast<CbmStsPoint*> (flistStsPts->At(iSts) );
     MCTrackSortedArray[StsPoint->GetTrackID()].StsArray.push_back(StsPoint);
  }
  
  for (Int_t iTof=0; iTof<flistTofPts->GetEntriesFast(); iTof++)
  {
     // CbmTofPoint* TofPoint = (CbmTofPoint*)flistTofPts->At(iTof);
	 CbmTofPoint* TofPoint = static_cast<CbmTofPoint*> (flistTofPts->At(iTof) );
     MCTrackSortedArray[TofPoint->GetTrackID()].TofArray.push_back(TofPoint);
  }
  
  //int allrec;
  //allrec = 0;
  int *TrRecons = new int[flistMCTracks->GetEntriesFast()+1];
  int iTr = 0;
 
  double mtavMC[p_sz];
  double mtavReco[p_sz];
  double mt2avMC[p_sz];
  double mt2avReco[p_sz];
  double mtmomerrMC[p_sz];
  double mtmomerrReco[p_sz];
  double tmpmt;
  int nTracksPaAllMC[p_sz];
  int nTracksReco[p_sz];
  for(int part=0;part<p_sz;++part) {
	mtavMC[part] = 0.;
	mt2avMC[part] = 0.;
	mtmomerrMC[part] = 0.;
	mtavReco[part] = 0.; mt2avReco[part] = 0.; mtmomerrReco[part] = 0.;
	nTracksPaAllMC[part] = 0;
	nTracksReco[part] = 0;
  }
  
  for(iTr=0; iTr<nTracksMC; iTr++) {
    //TrRecons[iTr] = (int)(checkIfReconstructable(&MCTrackSortedArray[iTr]));
    TrRecons[iTr] = MCTrackSortedArray[iTr].IsReconstructable(&vRTracksMC[iTr], 4, 3, 0.);
  }
  
  //fParticles.clear();
  fChiToPrimVtx.clear();

  CbmL1PFFitter fitter;
  
  //  fitter.Fit(vRTracks); //assumed, that the initial fit should be fixed and must return good results!!!
  vector<FairTrackParam> paramsinit;
  for(int iTrs=0; iTrs<nTracks; iTrs++) { 
    paramsinit.push_back(*(vRTracks[iTrs].GetParamFirst()));
  }

  vector<L1FieldRegion> vField;
  fitter.GetChiToVertex(vRTracks, vField, fChiToPrimVtx, kfVertex, 3000000);
  
  // Level 0
  {
    for(iTr=0; iTr<nTracksMC; iTr++) {
	  //iTr = ((CbmTrackMatch*)flistStsTracksMatch->At(iTrs))->GetMCTrackId();
      for(int part=0;part<p_sz;++part) {
	    if (vRTracksMC[iTr].GetPdgCode()==pdgIds[part] && vRTracksMC[iTr].GetMotherId()==-1) {
		  tmpmt = sqrt(vRTracksMC[iTr].GetMass() * vRTracksMC[iTr].GetMass() + vRTracksMC[iTr].GetPx() * vRTracksMC[iTr].GetPx() + vRTracksMC[iTr].GetPy() * vRTracksMC[iTr].GetPy());  
		  nTracksPaAllMC[part]++;
		  //mtavMC[part] += tmpmt;
		  //mt2avMC[part] += tmpmt * tmpmt;
		  mtavMC[part] += tempCrit(vRTracksMC[iTr].GetRapidity(), sqrt(vRTracksMC[iTr].GetPx() * vRTracksMC[iTr].GetPx() + vRTracksMC[iTr].GetPy() * vRTracksMC[iTr].GetPy()), vRTracksMC[iTr].GetMass());
		  mt2avMC[part] += tempCrit(vRTracksMC[iTr].GetRapidity(), sqrt(vRTracksMC[iTr].GetPx() * vRTracksMC[iTr].GetPx() + vRTracksMC[iTr].GetPy() * vRTracksMC[iTr].GetPy()), vRTracksMC[iTr].GetMass()) * tempCrit(vRTracksMC[iTr].GetRapidity(), sqrt(vRTracksMC[iTr].GetPx() * vRTracksMC[iTr].GetPx() + vRTracksMC[iTr].GetPy() * vRTracksMC[iTr].GetPy()), vRTracksMC[iTr].GetMass());
		  mtmomerrMC[part] += 0.;
		  
		  hfyMC[part]->Fill(vRTracksMC[iTr].GetRapidity());
		  hfdndydptMC[part]->Fill(vRTracksMC[iTr].GetRapidity(), sqrt(vRTracksMC[iTr].GetPx() * vRTracksMC[iTr].GetPx() + vRTracksMC[iTr].GetPy() * vRTracksMC[iTr].GetPy()));
		  
		  globalnTracksMC[part]++;
		  //globalmtavMC[part] += tmpmt;
		  //globalmt2avMC[part] += tmpmt * tmpmt;
		  globalmtavMC[part] += tempCrit(vRTracksMC[iTr].GetRapidity(), sqrt(vRTracksMC[iTr].GetPx() * vRTracksMC[iTr].GetPx() + vRTracksMC[iTr].GetPy() * vRTracksMC[iTr].GetPy()), vRTracksMC[iTr].GetMass());
		  globalmt2avMC[part] += tempCrit(vRTracksMC[iTr].GetRapidity(), sqrt(vRTracksMC[iTr].GetPx() * vRTracksMC[iTr].GetPx() + vRTracksMC[iTr].GetPy() * vRTracksMC[iTr].GetPy()), vRTracksMC[iTr].GetMass()) * tempCrit(vRTracksMC[iTr].GetRapidity(), sqrt(vRTracksMC[iTr].GetPx() * vRTracksMC[iTr].GetPx() + vRTracksMC[iTr].GetPy() * vRTracksMC[iTr].GetPy()), vRTracksMC[iTr].GetMass());
		  globalfavMC[part] += chi2func(vRTracksMC[iTr].GetRapidity(), sqrt(vRTracksMC[iTr].GetPx() * vRTracksMC[iTr].GetPx() + vRTracksMC[iTr].GetPy() * vRTracksMC[iTr].GetPy()), vRTracksMC[iTr].GetMass());
		  globalf2avMC[part] += chi2func(vRTracksMC[iTr].GetRapidity(), sqrt(vRTracksMC[iTr].GetPx() * vRTracksMC[iTr].GetPx() + vRTracksMC[iTr].GetPy() * vRTracksMC[iTr].GetPy()), vRTracksMC[iTr].GetMass()) * chi2func(vRTracksMC[iTr].GetRapidity(), sqrt(vRTracksMC[iTr].GetPx() * vRTracksMC[iTr].GetPx() + vRTracksMC[iTr].GetPy() * vRTracksMC[iTr].GetPy()), vRTracksMC[iTr].GetMass());
		  //globalmt4avMC[part] += tmpmt * tmpmt * tmpmt * tmpmt;
		  globalmtmomerrMC[part] += 0.;
		  if (fRecoLevel==0 && TrRecons[iTr]) {
		  //if (1) {
			  nTracksReco[part]++;
			  //mtavReco[part] += tmpmt;
			  //mt2avReco[part] += tmpmt * tmpmt;
			  mtavReco[part] += tempCrit(vRTracksMC[iTr].GetRapidity(), sqrt(vRTracksMC[iTr].GetPx() * vRTracksMC[iTr].GetPx() + vRTracksMC[iTr].GetPy() * vRTracksMC[iTr].GetPy()), vRTracksMC[iTr].GetMass());
			  mt2avReco[part] += tempCrit(vRTracksMC[iTr].GetRapidity(), sqrt(vRTracksMC[iTr].GetPx() * vRTracksMC[iTr].GetPx() + vRTracksMC[iTr].GetPy() * vRTracksMC[iTr].GetPy()), vRTracksMC[iTr].GetMass()) * tempCrit(vRTracksMC[iTr].GetRapidity(), sqrt(vRTracksMC[iTr].GetPx() * vRTracksMC[iTr].GetPx() + vRTracksMC[iTr].GetPy() * vRTracksMC[iTr].GetPy()), vRTracksMC[iTr].GetMass());
			  mtmomerrReco[part] += 0.;
			  globalnTracksReco[part]++;
			  //globalmtavReco[part] += tmpmt;
			  //globalmt2avReco[part] += tmpmt * tmpmt;
			  //globalmt4avReco[part] += tmpmt * tmpmt * tmpmt * tmpmt;
			  globalmtavReco[part] += tempCrit(vRTracksMC[iTr].GetRapidity(), sqrt(vRTracksMC[iTr].GetPx() * vRTracksMC[iTr].GetPx() + vRTracksMC[iTr].GetPy() * vRTracksMC[iTr].GetPy()), vRTracksMC[iTr].GetMass());
			  globalmt2avReco[part] += tempCrit(vRTracksMC[iTr].GetRapidity(), sqrt(vRTracksMC[iTr].GetPx() * vRTracksMC[iTr].GetPx() + vRTracksMC[iTr].GetPy() * vRTracksMC[iTr].GetPy()), vRTracksMC[iTr].GetMass()) * tempCrit(vRTracksMC[iTr].GetRapidity(), sqrt(vRTracksMC[iTr].GetPx() * vRTracksMC[iTr].GetPx() + vRTracksMC[iTr].GetPy() * vRTracksMC[iTr].GetPy()), vRTracksMC[iTr].GetMass());
			  globalfavReco[part] += chi2func(vRTracksMC[iTr].GetRapidity(), sqrt(vRTracksMC[iTr].GetPx() * vRTracksMC[iTr].GetPx() + vRTracksMC[iTr].GetPy() * vRTracksMC[iTr].GetPy()), vRTracksMC[iTr].GetMass());
			  globalf2avReco[part] += chi2func(vRTracksMC[iTr].GetRapidity(), sqrt(vRTracksMC[iTr].GetPx() * vRTracksMC[iTr].GetPx() + vRTracksMC[iTr].GetPy() * vRTracksMC[iTr].GetPy()), vRTracksMC[iTr].GetMass()) * chi2func(vRTracksMC[iTr].GetRapidity(), sqrt(vRTracksMC[iTr].GetPx() * vRTracksMC[iTr].GetPx() + vRTracksMC[iTr].GetPy() * vRTracksMC[iTr].GetPy()), vRTracksMC[iTr].GetMass());
		  
			  globalmtmomerrReco[part] += 0.;
			  
			  hfyReco[part]->Fill(vRTracksMC[iTr].GetRapidity());
		      hfdndydptReco[part]->Fill(vRTracksMC[iTr].GetRapidity(), sqrt(vRTracksMC[iTr].GetPx() * vRTracksMC[iTr].GetPx() + vRTracksMC[iTr].GetPy() * vRTracksMC[iTr].GetPy()));
		  
		      hfyRecoCor[part]->Fill(vRTracksMC[iTr].GetRapidity());
		      hfdndydptRecoCor[part]->Fill(vRTracksMC[iTr].GetRapidity(), sqrt(vRTracksMC[iTr].GetPx() * vRTracksMC[iTr].GetPx() + vRTracksMC[iTr].GetPy() * vRTracksMC[iTr].GetPy()));
		  
		  }
	    }
      }
    }
  }
  
  //std::map<int, int> trs;
  //int clcnt = 0, totsize = 0;
  
  // Level 1
  if (fRecoLevel==1) {
    for(int iTrs=0; iTrs<nTracks; iTrs++) {
      if (vTrackPDG[iTrs]!=-1) {
	// iTr = ((CbmTrackMatch*)flistStsTracksMatch->At(iTrs))->GetMCTrackId();
	iTr = (static_cast<CbmTrackMatch*> (flistStsTracksMatch->At(iTrs) ))->GetMCTrackId();
	
	for(int part=0;part<p_sz;++part) {
	  if (vTrackPDG[iTrs]==pdgIds[part] && vRTracksMC[iTr].GetMotherId()==-1) {
		//if (!TrRecons[iTr]) break;
	        //if (trs.count(iTr)) { trs[iTr]++; /*std::cout << trs[iTr] << " "; */ clcnt++; }
		//else trs[iTr] = 1;
		//totsize++;
		tmpmt = sqrt(vRTracksMC[iTr].GetMass()*vRTracksMC[iTr].GetMass() + vRTracksMC[iTr].GetPx() * vRTracksMC[iTr].GetPx() + vRTracksMC[iTr].GetPy() * vRTracksMC[iTr].GetPy());  
		nTracksReco[part]++;
		//mtavReco[part] += tmpmt;
		//mt2avReco[part] += tmpmt * tmpmt;
		mtavReco[part] += tempCrit(vRTracksMC[iTr].GetRapidity(), sqrt(vRTracksMC[iTr].GetPx() * vRTracksMC[iTr].GetPx() + vRTracksMC[iTr].GetPy() * vRTracksMC[iTr].GetPy()), vRTracksMC[iTr].GetMass());
		mt2avReco[part] += tempCrit(vRTracksMC[iTr].GetRapidity(), sqrt(vRTracksMC[iTr].GetPx() * vRTracksMC[iTr].GetPx() + vRTracksMC[iTr].GetPy() * vRTracksMC[iTr].GetPy()), vRTracksMC[iTr].GetMass()) * tempCrit(vRTracksMC[iTr].GetRapidity(), sqrt(vRTracksMC[iTr].GetPx() * vRTracksMC[iTr].GetPx() + vRTracksMC[iTr].GetPy() * vRTracksMC[iTr].GetPy()), vRTracksMC[iTr].GetMass());
		mtmomerrReco[part] += 0.;
		globalnTracksReco[part]++;
		//globalmtavReco[part] += tmpmt;
		//globalmt2avReco[part] += tmpmt * tmpmt;
		globalmtavReco[part] += tempCrit(vRTracksMC[iTr].GetRapidity(), sqrt(vRTracksMC[iTr].GetPx() * vRTracksMC[iTr].GetPx() + vRTracksMC[iTr].GetPy() * vRTracksMC[iTr].GetPy()), vRTracksMC[iTr].GetMass());
		globalmt2avReco[part] += tempCrit(vRTracksMC[iTr].GetRapidity(), sqrt(vRTracksMC[iTr].GetPx() * vRTracksMC[iTr].GetPx() + vRTracksMC[iTr].GetPy() * vRTracksMC[iTr].GetPy()), vRTracksMC[iTr].GetMass()) * tempCrit(vRTracksMC[iTr].GetRapidity(), sqrt(vRTracksMC[iTr].GetPx() * vRTracksMC[iTr].GetPx() + vRTracksMC[iTr].GetPy() * vRTracksMC[iTr].GetPy()), vRTracksMC[iTr].GetMass());
		//globalmt4avReco[part] += tmpmt * tmpmt * tmpmt * tmpmt;
		globalfavReco[part] += chi2func(vRTracksMC[iTr].GetRapidity(), sqrt(vRTracksMC[iTr].GetPx() * vRTracksMC[iTr].GetPx() + vRTracksMC[iTr].GetPy() * vRTracksMC[iTr].GetPy()), vRTracksMC[iTr].GetMass());
		globalf2avReco[part] += chi2func(vRTracksMC[iTr].GetRapidity(), sqrt(vRTracksMC[iTr].GetPx() * vRTracksMC[iTr].GetPx() + vRTracksMC[iTr].GetPy() * vRTracksMC[iTr].GetPy()), vRTracksMC[iTr].GetMass()) * chi2func(vRTracksMC[iTr].GetRapidity(), sqrt(vRTracksMC[iTr].GetPx() * vRTracksMC[iTr].GetPx() + vRTracksMC[iTr].GetPy() * vRTracksMC[iTr].GetPy()), vRTracksMC[iTr].GetMass());
		globalmtmomerrReco[part] += 0.;
		
		hfyReco[part]->Fill(vRTracksMC[iTr].GetRapidity());
		hfdndydptReco[part]->Fill(vRTracksMC[iTr].GetRapidity(), sqrt(vRTracksMC[iTr].GetPx() * vRTracksMC[iTr].GetPx() + vRTracksMC[iTr].GetPy() * vRTracksMC[iTr].GetPy()));
  
		hfyRecoCor[part]->Fill(vRTracksMC[iTr].GetRapidity());
		hfdndydptRecoCor[part]->Fill(vRTracksMC[iTr].GetRapidity(), sqrt(vRTracksMC[iTr].GetPx() * vRTracksMC[iTr].GetPx() + vRTracksMC[iTr].GetPy() * vRTracksMC[iTr].GetPy()));
  
	  }
	}
      }
    }
    //std::cout << (double)(clcnt) / totsize << endl;
  }
  
  double tmprapid;
  
  // Level 2
  if (fRecoLevel==2) {
    for(int iTrs=0; iTrs<nTracks; iTrs++) {
      if (vTrackPDG[iTrs]!=-1) {
	// iTr = ((CbmTrackMatch*)flistStsTracksMatch->At(iTrs))->GetMCTrackId();
	iTr = (static_cast<CbmTrackMatch*> (flistStsTracksMatch->At(iTrs) ))->GetMCTrackId();
	TVector3 tmpv;
	vRTracks[iTrs].GetParamFirst()->Momentum(tmpv);
	for(int part=0;part<p_sz;++part) {
	  tmpmt = sqrt(TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()*TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass() + tmpv.Pt()*tmpv.Pt());
	  tmprapid = 0.5*log((sqrt(TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()*TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()+tmpv.Mag()*tmpv.Mag())+tmpv.Pz())/(sqrt(TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()*TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()+tmpv.Mag()*tmpv.Mag())-tmpv.Pz()));
	  if (!(tmpmt>0. && tmpmt<10.)) break;
	  if (vTrackPDG[iTrs]==pdgIds[part] && vRTracksMC[iTr].GetMotherId()==-1) {
		//if (!TrRecons[iTr]) break;
		Double_t covMatrix[15];
		vRTracks[iTrs].GetParamFirst()->CovMatrix(covMatrix);
		double tmperr = getMtErrorSquare(vRTracks[iTrs].GetParamFirst()->GetQp(), vRTracks[iTrs].GetParamFirst()->GetTx(), vRTracks[iTrs].GetParamFirst()->GetTy(), covMatrix, TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass());
		//tmperr = sqrt(tmperr);
		if (!(tmperr>0. && tmperr<0.1)) break;
		//if (tmperr<0. || sqrt(tmperr)>0.001) break;
		//tmpmt = sqrt(vRTracksMC[iTr].GetMass()*vRTracksMC[iTr].GetMass() + vRTracksMC[iTr].GetPx() * vRTracksMC[iTr].GetPx() + vRTracksMC[iTr].GetPy() * vRTracksMC[iTr].GetPy());  
		nTracksReco[part]++;
		//mtavReco[part] += tmpmt;
		//mt2avReco[part] += tmpmt * tmpmt;
		mtavReco[part] += tempCrit(tmprapid, sqrt(tmpmt*tmpmt-TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()*TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()), TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass());
		mt2avReco[part] += tempCrit(tmprapid, sqrt(tmpmt*tmpmt-TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()*TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()), TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()) * tempCrit(tmprapid, sqrt(tmpmt*tmpmt-TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()*TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()), TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass());
		
		
		mtmomerrReco[part] += getMtErrorSquare(vRTracks[iTrs].GetParamFirst()->GetQp(), vRTracks[iTrs].GetParamFirst()->GetTx(), vRTracks[iTrs].GetParamFirst()->GetTy(), covMatrix, TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass());
		globalnTracksReco[part]++;
		//globalmtavReco[part] += tmpmt;
		//globalmt2avReco[part] += tmpmt * tmpmt;
		globalmtavReco[part] += tempCrit(tmprapid, sqrt(tmpmt*tmpmt-TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()*TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()), TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass());
		globalmt2avReco[part] += tempCrit(tmprapid, sqrt(tmpmt*tmpmt-TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()*TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()), TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()) * tempCrit(tmprapid, sqrt(tmpmt*tmpmt-TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()*TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()), TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass());
		//globalmt4avReco[part] += tmpmt * tmpmt * tmpmt * tmpmt;
		globalfavReco[part] += chi2func(tmprapid, sqrt(tmpmt*tmpmt-TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()*TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()), TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass());
		globalf2avReco[part] += chi2func(tmprapid, sqrt(tmpmt*tmpmt-TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()*TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()), TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()) * chi2func(tmprapid, sqrt(tmpmt*tmpmt-TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()*TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()), TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass());
		globalmtmomerrReco[part] += getMtErrorSquare(vRTracks[iTrs].GetParamFirst()->GetQp(), vRTracks[iTrs].GetParamFirst()->GetTx(), vRTracks[iTrs].GetParamFirst()->GetTy(), covMatrix, TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass());
	  
		hfyReco[part]->Fill(tmprapid);
		hfdndydptReco[part]->Fill(tmprapid, sqrt(tmpmt*tmpmt-TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()*TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()));
  
		hfyRecoCor[part]->Fill(tmprapid);
		hfdndydptRecoCor[part]->Fill(tmprapid, sqrt(tmpmt*tmpmt-TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()*TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()));
	  }
	}
      }
    }
  }
  
  // Level 3
  if (fRecoLevel==3) {
    for(int iTrs=0; iTrs<nTracks; iTrs++) {
      if (vTrackPDG[iTrs]!=-1) {
	// iTr = ((CbmTrackMatch*)flistStsTracksMatch->At(iTrs))->GetMCTrackId();
	iTr = (static_cast<CbmTrackMatch*> (flistStsTracksMatch->At(iTrs) ))->GetMCTrackId();
	TVector3 tmpv;
	vRTracks[iTrs].GetParamFirst()->Momentum(tmpv);
	for(int part=0;part<p_sz;++part) {
	  tmpmt = sqrt(TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()*TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass() + tmpv.Pt()*tmpv.Pt());
	  tmprapid = 0.5*log((sqrt(TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()*TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()+tmpv.Mag()*tmpv.Mag())+tmpv.Pz())/(sqrt(TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()*TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()+tmpv.Mag()*tmpv.Mag())-tmpv.Pz()));
	  if (!(tmpmt>0. && tmpmt<10.)) break;
	  //std::cout << tmprapid << " ";
	  //if (tmprapid<0.) break;
	  //if (tmprapid>6.) break;
	  //if (tmprapid<0. || tmprapid>6. || tmpv.Pt()<0. || tmpv.Pt()>2.5) continue;
	  if (vTrackPDG[iTrs]==pdgIds[part] && fChiToPrimVtx[iTrs]<3.) {//vRTracksMC[iTr].GetMotherId()==-1) {
		//tmpmt = sqrt(vRTracksMC[iTr].GetMass()*vRTracksMC[iTr].GetMass() + vRTracksMC[iTr].GetPx() * vRTracksMC[iTr].GetPx() + vRTracksMC[iTr].GetPy() * vRTracksMC[iTr].GetPy());  
		//if (!TrRecons[iTr]) break;
		Double_t covMatrix[15];
		vRTracks[iTrs].GetParamFirst()->CovMatrix(covMatrix);
		double tmperr = getMtErrorSquare(vRTracks[iTrs].GetParamFirst()->GetQp(), vRTracks[iTrs].GetParamFirst()->GetTx(), vRTracks[iTrs].GetParamFirst()->GetTy(), covMatrix, TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass());
		//tmperr = sqrt(tmperr);
		if (!(tmperr>0. && tmperr<0.1)) break;
		
		nTracksReco[part]++;
		//mtavReco[part] += tmpmt;
		//mt2avReco[part] += tmpmt * tmpmt;
		mtavReco[part] += tempCrit(tmprapid, sqrt(tmpmt*tmpmt-TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()*TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()), TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass());
		mt2avReco[part] += tempCrit(tmprapid, sqrt(tmpmt*tmpmt-TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()*TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()), TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()) * tempCrit(tmprapid, sqrt(tmpmt*tmpmt-TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()*TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()), TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass());
		
		
		
		mtmomerrReco[part] += getMtErrorSquare(vRTracks[iTrs].GetParamFirst()->GetQp(), vRTracks[iTrs].GetParamFirst()->GetTx(), vRTracks[iTrs].GetParamFirst()->GetTy(), covMatrix, TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass());
		
		globalnTracksReco[part]++;
		//globalmtavReco[part] += tmpmt;
		//globalmt2avReco[part] += tmpmt * tmpmt;
		globalmtavReco[part] += tempCrit(tmprapid, sqrt(tmpmt*tmpmt-TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()*TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()), TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass());
		globalmt2avReco[part] += tempCrit(tmprapid, sqrt(tmpmt*tmpmt-TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()*TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()), TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()) * tempCrit(tmprapid, sqrt(tmpmt*tmpmt-TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()*TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()), TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass());
		globalfavReco[part] += chi2func(tmprapid, sqrt(tmpmt*tmpmt-TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()*TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()), TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass());
		globalf2avReco[part] += chi2func(tmprapid, sqrt(tmpmt*tmpmt-TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()*TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()), TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()) * chi2func(tmprapid, sqrt(tmpmt*tmpmt-TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()*TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()), TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass());
		globalmtmomerrReco[part] += getMtErrorSquare(vRTracks[iTrs].GetParamFirst()->GetQp(), vRTracks[iTrs].GetParamFirst()->GetTx(), vRTracks[iTrs].GetParamFirst()->GetTy(), covMatrix, TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass());
		
		hfyReco[part]->Fill(tmprapid);
		hfdndydptReco[part]->Fill(tmprapid, sqrt(tmpmt*tmpmt-TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()*TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()));
  
		hfyRecoCor[part]->Fill(tmprapid);
		hfdndydptRecoCor[part]->Fill(tmprapid, sqrt(tmpmt*tmpmt-TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()*TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()));
	  }
	}
      }
    }
  }
  
  
  for(int part=0;part<p_sz;++part) {
    mtavMC[part] /= nTracksPaAllMC[part];
    mt2avMC[part] /= nTracksPaAllMC[part];
	}
  
  for(int part=0;part<p_sz;++part) {
    //if (part==0) 
	{
      double tmpT = getTemperatureAll(mtavMC[part], part, 20);
      hTempFullMC[part]->Fill(tmpT);
      hTempErrStatFullMC[part]->Fill(getTemperatureDerivAll(mtavMC[part], part, 20) * sqrt((mt2avMC[part]-mtavMC[part]*mtavMC[part]) / (nTracksPaAllMC[part]-1)));
      hTempErrMomFullMC[part]->Fill(getTemperatureDerivAll(mtavMC[part], part, 20) * sqrt(mtmomerrMC[part]) / nTracksPaAllMC[part]);
      
      hRadFullMC[part]->Fill(getRadius(tmpT, nTracksPaAllMC[part], TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()));
      double tmpTerr = getTemperatureDerivAll(mtavMC[part], part, 20) * sqrt((mt2avMC[part]-mtavMC[part]*mtavMC[part]) / (nTracksPaAllMC[part]-1));
      double tmp1 = 0., tmp2 = 0.;
      double tmpNerr = sqrt(nTracksPaAllMC[part]);
      tmp1 = getRadiusDerivT(tmpT, nTracksPaAllMC[part], TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass());
      tmp2 = getRadiusDerivN(tmpT, nTracksPaAllMC[part], TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass());
      double tmpRerr = sqrt(tmp1*tmp1*tmpTerr*tmpTerr + tmp2*tmp2*tmpNerr*tmpNerr);
      hRadErrStatFullMC[part]->Fill(tmpRerr);
      
      tmpTerr = getTemperatureDerivAll(mtavMC[part], part, 20) * sqrt(mtmomerrMC[part]) / nTracksPaAllMC[part];
      tmpRerr = sqrt(tmp1*tmp1*tmpTerr*tmpTerr + tmp2*tmp2*tmpNerr*tmpNerr*0.);
      hRadErrMomFullMC[part]->Fill(tmpRerr);
    }
  }
  
  //for(int rl=0;rl<recoLevels;++rl) {
    for(int part=0;part<p_sz;++part) {
	  mtavReco[part] /= nTracksReco[part];
	  mt2avReco[part] /= nTracksReco[part];
	}
    
    for(int part=0;part<p_sz;++part) {
	  //if (part==0) 
	  {
		  mtTacc[part] = mtTsts[part]; Npart[part] = Npartsts[part];
		  if (fusePID==2 && fRecoLevel>=1) { mtTacc[part] = mtTststof[part]; Npart[part] = Npartststof[part]; }  
		  double tmpT = getTemperatureAll(mtavMC[part], part, 20);
		  //hTempFull[part]->Fill(tmpT);
		  //hRadFull[part]->Fill(getRadius(tmpT, nTracksPaAll[part], TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()));
		  tmpT = getTemperatureAll(mtavReco[part], part, 20);
		  hTempFullReco[part]->Fill(tmpT);
		  hRadFullReco[part]->Fill(getRadius(tmpT, nTracksReco[part], TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()));
		  
		  double tmpTerr = getTemperatureDerivAll(mtavReco[part], part, 20) * sqrt((mt2avReco[part]-mtavReco[part]*mtavReco[part]) / (nTracksReco[part]-1));
		  double tmp1 = 0., tmp2 = 0.;
		  double tmpNerr = sqrt(nTracksReco[part]);
		  tmp1 = getRadiusDerivT(tmpT, nTracksReco[part], TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass());
		  tmp2 = getRadiusDerivN(tmpT, nTracksReco[part], TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass());
		  double tmpRerr = sqrt(tmp1*tmp1*tmpTerr*tmpTerr + tmp2*tmp2*tmpNerr*tmpNerr);
		  
		  hTempErrStatFullReco[part]->Fill(tmpTerr);
		  hRadErrStatFullReco[part]->Fill(tmpRerr);
		  
		  tmpTerr = getTemperatureDerivAll(mtavReco[part], part, 20) * sqrt(mtmomerrReco[part]) / nTracksReco[part];
		  tmpRerr = sqrt(tmp1*tmp1*tmpTerr*tmpTerr + tmp2*tmp2*tmpNerr*tmpNerr);
		  
		  hTempErrMomFullReco[part]->Fill(tmpTerr);
		  hRadErrMomFullReco[part]->Fill(tmpRerr);
		  
		  tmpT = getTemperatureAllCor(mtavReco[part], part, 20, mtTacc[part]);
		  hTempFullRecoCor[part]->Fill(tmpT);
		  hRadFullRecoCor[part]->Fill(getRadiusCor(tmpT, nTracksReco[part], TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass(), Npart[part]));
		  
		  tmpTerr = getTemperatureDerivAllCor(mtavReco[part], part, 20, mtTacc[part]) * sqrt((mt2avReco[part]-mtavReco[part]*mtavReco[part]) / (nTracksReco[part]-1));
		  tmp1 = 0., tmp2 = 0.;
		  tmpNerr = sqrt(nTracksReco[part]);
		  tmp1 = getRadiusDerivTCor(tmpT, nTracksReco[part], TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass(), Npart[part]);
		  tmp2 = getRadiusDerivNCor(tmpT, nTracksReco[part], TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass(), Npart[part]);
		  tmpRerr = sqrt(tmp1*tmp1*tmpTerr*tmpTerr + tmp2*tmp2*tmpNerr*tmpNerr);
		  
		  hTempErrStatFullRecoCor[part]->Fill(tmpTerr);
		  hRadErrStatFullRecoCor[part]->Fill(tmpRerr);
		  
		  tmpTerr = getTemperatureDerivAllCor(mtavReco[part], part, 20, mtTacc[part]) * sqrt(mtmomerrReco[part]) / nTracksReco[part];
		  tmpRerr = sqrt(tmp1*tmp1*tmpTerr*tmpTerr + tmp2*tmp2*tmpNerr*tmpNerr*0.);
		  
		  hTempErrMomFullRecoCor[part]->Fill(tmpTerr);
		  hRadErrMomFullRecoCor[part]->Fill(tmpRerr);
	  }
    }
  //}
    
  delete[] MCTrackSortedArray;
  delete[] TrRecons;
}

// Function to test method of moments
double CbmThermalModelNoFlow::chi2func(double y, double pt, double m) {
	return sqrt(pt*pt+m*m);//y*y;//pt*pt + m*m;
}

// Function to determine temperature
double CbmThermalModelNoFlow::tempCrit(double y, double pt, double m) {
	return sqrt(pt*pt+m*m);//*cosh(y);
}

void CbmThermalModelNoFlow::ComputeThermalDependence(Int_t part)
{
  TString work      = getenv("VMCWORKDIR");
  TString dir  = work + "/KF/KFModelParameters/";//ThermalModel.mT-t.sts_v13.txt";
  ReadAcceptanceFunction(AcceptanceSTS, dir + "pty_acc_sts_v13.txt");
  ReadAcceptanceFunction(AcceptanceSTSTOF, dir + "pty_acc_ststof_v13.txt");
  
  std::vector<double> mtall, Tvecall;

  std::cout << "Computing dependencies of ThermalModel...";
  
  fflush(stdout);
  
  std::vector<double> Tvec, mtacc, NPart;
  mtall.resize(0);
  double dT = 0.002;
  double Tmax = 0.5;
  for(int i=0;;++i) {
	  double tmpT = dT*(0.5+i);
	  if (tmpT>Tmax) break;
	  //cout << tmpT << "\n";
	  Tvec.push_back(tmpT);
	  mtall.push_back(ThermalMt(tmpT, TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()));
	  mtacc.push_back(ThermalMtAcc(tmpT, TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass(), AcceptanceSTS));
	  NPart.push_back(NFracAcc(tmpT, TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass(), AcceptanceSTS));
	  //cout << tmpT << " " << mtall[mtall.size()-1] << "\n";
  }
  
  //mtTall = new TSpline3("mtTall", &Tvecall[0], &mtall[0], Tvecall.size());
  mtTall[part] = new TSpline3("mtTall", &Tvec[0], &mtall[0], Tvec.size());
  mtTsts[part] = new TSpline3("mtTacc", &Tvec[0], &mtacc[0], Tvec.size());
  Npartsts[part] = new TSpline3("Npart", &Tvec[0], &NPart[0], Tvec.size());
  
  Tvec.resize(0); 
  //mtall.resize(0);
  mtacc.resize(0);
  NPart.resize(0);

  dT = 0.002;
  Tmax = 0.5;
  for(int i=0;;++i) {
	  double tmpT = dT*(0.5+i);
	  if (tmpT>Tmax) break;
	  //cout << tmpT << "\n";
	  Tvec.push_back(tmpT);
	  //mtall.push_back(ThermalMt(tmpT, TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()));
	  mtacc.push_back(ThermalMtAcc(tmpT, TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass(), AcceptanceSTSTOF));
	  NPart.push_back(NFracAcc(tmpT, TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass(), AcceptanceSTSTOF));
  }

  mtTststof[part] = new TSpline3("mtTacc", &Tvec[0], &mtacc[0], Tvec.size());
  Npartststof[part] = new TSpline3("Npart", &Tvec[0], &NPart[0], Tvec.size());
  
  if (fusePID==1) { mtTacc[part] = mtTsts[part]; Npart[part] = Npartsts[part]; }
  else { mtTacc[part] = mtTststof[part]; Npart[part] = Npartststof[part]; }
  
  std::cout << " Done!\n";
}

Double_t CbmThermalModelNoFlow::ThermalMt(double T, double m) {
  double ret1 = 0., ret2 = 0.;
  vector<double> xlag, wlag, xleg, wleg;
  //GetCoefs2DLaguerre32Legendre32(0., 6., xlag, wlag, xleg, wleg);
  GetCoefs2DLegendre32Legendre32(0., 3., -3., 6., xlag, wlag, xleg, wleg);
  for(Int_t i = 0 ; i < 32 ; i++){
		for(Int_t j = 0 ; j < 32 ; j++){
		  //if (xlag[i]>3.) break;
		  //cout << xlag[i] << " " << xleg[j] << " " << accfunc.getAcceptance(xleg[j], xlag[i]) << "\t";
		  double tmpf = 0.;
		  tmpf = xlag[i] * sqrt(xlag[i]*xlag[i]+ m*m) * TMath::CosH(xleg[j]-ycm) * exp(-sqrt(xlag[i]*xlag[i] + m*m)*TMath::CosH(xleg[j]-ycm)/T);
		  ret1 += wlag[i]*wleg[j]*tmpf;
		  ret2 += wlag[i]*wleg[j]*tmpf * tempCrit(xleg[j], xlag[i], m);
		}
	  }
   return ret2 / ret1;
}

Double_t CbmThermalModelNoFlow::ThermalMt2(double T, double m) {
  double ret1 = 0., ret2 = 0.;
  double ymin = -3., ymax = 6.;
  int itery = 1500;
  double dy = (ymax - ymin) / itery;
  double ptmin = 0., ptmax = 3.;
  int iterpt = 1200;
  double dpt = (ptmax - ptmin) / iterpt;
  double ty = 0., tpt = 0., tmp = 0.;
  for(int iy=0;iy<itery;++iy) {
	ty = ymin + (iy+0.5)*dy;
	for(int ipt=0;ipt<iterpt;++ipt) {
		tpt = ptmin + (ipt+0.5)*dpt;
		tmp = tpt * sqrt(tpt*tpt + m*m) * cosh(ty-ycm) * exp(-sqrt(tpt*tpt + m*m)*cosh(ty-ycm)/T);
		ret1 += tmp;
		//ret2 += tmp * (tpt*tpt + m*m);
		ret2 += tmp * chi2func(ty, tpt, m);
	}
  }
  //std::cout << "T = " << T << "\tmT = " << ret2 / ret1 << endl;
  return ret2 / ret1;
}

Double_t CbmThermalModelNoFlow::RecEfficiency(Double_t p) {
  if (fTrackNumber==0) return 0.99 - 0.98 * exp(-p*p/2./0.135/0.135);
  else return 0.98 - 0.88 * exp(-p*p/2./0.2/0.2);
}

Double_t CbmThermalModelNoFlow::ThermalMtAcc(double T, double m, const AcceptanceFunction & accfunc) {
  double ret1 = 0., ret2 = 0.;
  vector<double> xlag, wlag, xleg, wleg;
  //GetCoefs2DLaguerre32Legendre32(0., 6., xlag, wlag, xleg, wleg);
  GetCoefs2DLegendre32Legendre32(0., 2.5, 0., 6., xlag, wlag, xleg, wleg);
  //cout << ycm << "\n";
  for(Int_t i = 0 ; i < 32 ; i++){
		for(Int_t j = 0 ; j < 32 ; j++){
		  //if (xlag[i]>3.) break;
		  //cout << xlag[i] << " " << xleg[j] << " " << accfunc.getAcceptance(xleg[j], xlag[i]) << "\t";
		  double tmpf = 0.;
		  double tp = sqrt(xlag[i]*xlag[i] + m*m) * TMath::CosH(xleg[j]);
		  tp = sqrt(tp*tp - m*m);
		  tmpf = xlag[i] * sqrt(xlag[i]*xlag[i]+ m*m) * TMath::CosH(xleg[j]-ycm) * exp(-sqrt(xlag[i]*xlag[i] + m*m)*TMath::CosH(xleg[j]-ycm)/T) 
			* accfunc.getAcceptance(xleg[j], xlag[i]) * RecEfficiency(tp);
		  ret1 += wlag[i]*wleg[j]*tmpf;
		  ret2 += wlag[i]*wleg[j]*tmpf * tempCrit(xleg[j], xlag[i], m);
		}
	  }
   return ret2 / ret1;
}

Double_t CbmThermalModelNoFlow::ThermalMt2Acc(double T, double m, const AcceptanceFunction & accfunc) {
  if (accfunc.ys.size()==0) return -1.;
  double ret1 = 0., ret2 = 0.;
  double tmp = 0., tp = 0.;
  for(unsigned int i=0;i<accfunc.ys.size();++i) {
	tp = sqrt(accfunc.pts[i]*accfunc.pts[i] + m*m) * cosh(accfunc.ys[i]);
	tp = sqrt(tp*tp - m*m);
	tmp = accfunc.pts[i] * sqrt(accfunc.pts[i]*accfunc.pts[i]+ m*m) * cosh(accfunc.ys[i]-ycm) * exp(-sqrt(accfunc.pts[i]*accfunc.pts[i] + m*m)*cosh(accfunc.ys[i]-ycm)/T) 
	  * accfunc.probs[i] * RecEfficiency(tp);
	ret1 += tmp;
	//ret2 += tmp * (accfunc.pts[i]*accfunc.pts[i] + m*m);
	ret2 += tmp * chi2func(accfunc.ys[i], accfunc.pts[i], m);
  }
  //std::cout << "T = " << T << "\tmT = " << ret2 / ret1 << endl;
  return ret2 / ret1;
}

Double_t CbmThermalModelNoFlow::NFracAcc(double T, double m, const AcceptanceFunction & accfunc) {
  double ret1 = 0., ret2 = 0.;
  vector<double> xlag, wlag, xleg, wleg;
  //GetCoefs2DLaguerre32Legendre32(0., 6., xlag, wlag, xleg, wleg);
  GetCoefs2DLegendre32Legendre32(0., 2.5, 0., 6., xlag, wlag, xleg, wleg);
  for(Int_t i = 0 ; i < 32 ; i++){
		for(Int_t j = 0 ; j < 32 ; j++){
		  //if (xlag[i]>3.) break;
		  //cout << xlag[i] << " " << xleg[j] << " " << accfunc.getAcceptance(xleg[j], xlag[i]) << "\t";
		  double tmpf = 0.;
		  double tp = sqrt(xlag[i]*xlag[i] + m*m) * TMath::CosH(xleg[j]);
		  tp = sqrt(tp*tp - m*m);
		  tmpf = xlag[i] * sqrt(xlag[i]*xlag[i]+ m*m) * TMath::CosH(xleg[j]-ycm) * exp(-sqrt(xlag[i]*xlag[i] + m*m)*TMath::CosH(xleg[j]-ycm)/T)
			* accfunc.getAcceptance(xleg[j], xlag[i]) * RecEfficiency(tp);
		  ret2 += wlag[i]*wleg[j]*tmpf;
		  //ret2 += wlag[i]*wleg[j]*tmpf * tempCrit(xleg[j], xlag[i], m);
		}
	  }
   ret2 *= 1. / 2. / 2. / TMath::Pi() / TMath::Pi();
   ret1 = T * m * m * TMath::BesselK(2, m/T) / 2. / TMath::Pi() / TMath::Pi();
   return ret2 / ret1;
}

bool CbmThermalModelNoFlow::checkIfReconstructable(CbmKFTrErrMCPoints *inTrack)
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

double CbmThermalModelNoFlow::getRadius(double T, double N, double mo) {
  //std::cout << T << "\t" << N << endl;
  double V = (2*TMath::Pi()*TMath::Pi()*N)/T/mo/mo/TMath::BesselK(2, mo/T);
  return TMath::Power(3.*V/4./TMath::Pi(), 1./3.) / GeVtoifm; 
}

double CbmThermalModelNoFlow::getRadiusDerivT(double T, double N, double mo) {
  double dT = 0.005;
  return fabs(getRadius(T+dT, N, mo) - getRadius(T, N, mo)) / dT;
}

double CbmThermalModelNoFlow::getRadiusDerivN(double T, double N, double mo) {
  double dN = 0.1;
  return fabs(getRadius(T, N+dN, mo) - getRadius(T, N, mo)) / dN;
}

double CbmThermalModelNoFlow::getRadiusCor(double T, double N, double mo, TSpline3 *Np) {
  //std::cout << T << "\t" << N << "\t" <<  N/Npart->Eval(T) << endl ;
  double V = (2*TMath::Pi()*TMath::Pi()*N/Np->Eval(T))/T/mo/mo/TMath::BesselK(2, mo/T);
  return TMath::Power(3.*V/4./TMath::Pi(), 1./3.) / GeVtoifm; 
}

double CbmThermalModelNoFlow::getRadiusDerivTCor(double T, double N, double mo, TSpline3 *Np) {
  double dT = 0.005;
  return fabs(getRadiusCor(T+dT, N, mo, Np) - getRadiusCor(T, N, mo, Np)) / dT;
}

double CbmThermalModelNoFlow::getRadiusDerivNCor(double T, double N, double mo, TSpline3 *Np) {
  double dN = 0.1;
  return fabs(getRadiusCor(T, N+dN, mo, Np) - getRadiusCor(T, N, mo, Np)) / dN;
}

double CbmThermalModelNoFlow::getTemperature(double mt, double mo, int iters) {
  double left = 0., right = 1., center;
  double valleft = tempFunction(left, mt, mo);//, valright = tempFunction(right, mt, mo)
  double valcenter;
  for(int i=0;i<iters;++i) {
    center = (left+right)/2.;
    valcenter = tempFunction(center, mt, mo);
    if (valleft*valcenter>0) left = center;
    else right = center;
  }
  return (left+right)/2.;
}

double CbmThermalModelNoFlow::getTemperatureAll(double mt, int part, int iters) {
  double left = 0., right = 0.45, center;
  double valleft = mtTall[part]->Eval(left)-mt;//, valright = mtTall[part]->Eval(right)-mt, 
  double valcenter;
  for(int i=0;i<iters;++i) {
    center = (left+right)/2.;
    valcenter = mtTall[part]->Eval(center)-mt;
    if (valleft*valcenter>0) left = center;
    else right = center;
  }
  return (left+right)/2.;
}

double CbmThermalModelNoFlow::getTemperatureDerivAll(double mt, int part, int iters) {
  double dmt = 0.005;
  return fabs((getTemperatureAll(mt+dmt, part, iters) - getTemperatureAll(mt, part, iters))) / dmt;
}

double CbmThermalModelNoFlow::getTemperatureAllCor(double mt, int part, int iters, TSpline3 *mtT) {
  double left = 0., right = 0.45, center;
  double valleft = mtT->Eval(left)-mt;//, valright = mtT->Eval(right)-mt, 
  double valcenter;
  for(int i=0;i<iters;++i) {
    center = (left+right)/2.;
    valcenter = mtT->Eval(center)-mt;
    if (valleft*valcenter>0) left = center;
    else right = center;
  }
  return (left+right)/2.;
}

double CbmThermalModelNoFlow::getTemperatureDerivAllCor(double mt, int part, int iters, TSpline3 *mtT) {
  double dmt = 0.005;
  return fabs((getTemperatureAllCor(mt+dmt, part, iters, mtT) - getTemperatureAllCor(mt, part, iters, mtT))) / dmt;
}

double CbmThermalModelNoFlow::getTemperatureRapidity(double mt, double mo, int iters, double y) {
  double left = 0., right = 1., center;
  double valleft = tempFunction(left, mt, mo);//, valright = tempFunction(right, mt, mo), 
  double valcenter;
  for(int i=0;i<iters;++i) {
    center = (left+right)/2.;
    valcenter = tempFunction(center, mt, mo);
    if (valleft*valcenter>0) left = center;
    else right = center;
  }
  return (left+right) * 0.5 * cosh(y);
}

double CbmThermalModelNoFlow::getInverseSlope(double mt, double mo, int iters) {
  return 0.5*(0.5*mt-mo+sqrt((0.5*mt-mo)*(0.5*mt-mo)+2*mo*(mt-mo)));
}

double CbmThermalModelNoFlow::getDerivTemperature(double mt, double mo, double T, int iters) {
  double left = 0., right = 10., center;
  double valleft = tempDerivFunction(T, left, mt, mo);//, valright = tempDerivFunction(T, right, mt, mo), 
  double valcenter;
  for(int i=0;i<iters;++i) {
    center = (left+right)/2.;
    valcenter = tempDerivFunction(T, center, mt, mo);
    if (valleft*valcenter>0) left = center;
    else right = center;
  }
  return (left+right)/2.;
}

double CbmThermalModelNoFlow::getMtErrorSquare(double qp, double Tx, double Ty, double covMatrix[], double mo) {
  double tmpTxy = Tx*Tx+Ty*Ty;
  double mt = sqrt(mo*mo + tmpTxy/qp/qp/(tmpTxy+1.));
  double ddqp = -2./qp/qp/qp*tmpTxy/(tmpTxy+1.);
  double ddTx = 2.*Tx/qp/qp/(tmpTxy+1.)*(tmpTxy+1.);
  double ddTy = 2.*Ty/qp/qp/(tmpTxy+1.)*(tmpTxy+1.);
  //return (1./4./mt/mt) * ( ddqp*ddqp*covMatrix[14] + ddTx*ddTx*covMatrix[5] + ddTy*ddTy*covMatrix[9]
//			   + 2. * ( ddqp*ddTx*covMatrix[12] + ddqp*ddTy*covMatrix[13] + ddTx*ddTy*covMatrix[8] ) );
  return (1./4./mt/mt) * ( ddqp*ddqp*covMatrix[14] + ddTx*ddTx*covMatrix[9] + ddTy*ddTy*covMatrix[12]
			   + 2. * ( ddqp*ddTx*covMatrix[11] + ddqp*ddTy*covMatrix[13] + ddTx*ddTy*covMatrix[10] ) );
}

double CbmThermalModelNoFlow::tempFunction(double T, double mt, double mo) {
  return mt*(mo*mo+2.*mo*T+2*T*T) - (mo*mo*mo+3*mo*mo*T+6*mo*T*T+6*T*T*T);
}

double CbmThermalModelNoFlow::tempDerivFunction(double T, double dT, double mt, double mo) {
  //return mt*(mo*mo+2.*mo*T+2*T*T) - (mo*mo*mo+3*mo*mo*T+6*mo*T*T+6*T*T*T);
  return mt*(2*mo*dT + 4*T*dT) + mo*mo+2*mo*T+2*T*T - (3.*mo*mo*dT+12*mo*T*dT+18.*T*T*dT);
}

void CbmThermalModelNoFlow::Finish(){
  ofstream fout("ThermalModel.txt");
  for(int part=0;part<p_sz;++part) {
	globalmtavMC[part] /= globalnTracksMC[part];
	globalmt2avMC[part] /= globalnTracksMC[part];
	globalfavMC[part] /= globalnTracksMC[part];
	globalf2avMC[part] /= globalnTracksMC[part];
    globalmtavReco[part] /= globalnTracksReco[part]; globalmt2avReco[part] /= globalnTracksReco[part]; 
	globalfavReco[part] /= globalnTracksReco[part]; globalf2avReco[part] /= globalnTracksReco[part];
  }
  //if (!calcAcceptance) {
	//printf("Type\tAll\tReco\tReco cor\n");
	printf("%10s\t%10s\t%10s\t%10s\n", "Type", "All", "Reco", "Reco cor");
	fout << "MC Tracks:" << endl;
	fout << "Particle\tTemperature\tError\tRadius\tError\tchi2/ndf\n";
	for(int part=0;part<p_sz;++part) {
	  //if  (part==0) 
	  {
	      double tmpTMC = getTemperatureAll(globalmtavMC[part], part, 20);
	      double tmpRMC = getRadius(tmpTMC, globalnTracksMC[part] / static_cast<double>(events), TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass());
	      double tmpTMCerr = getTemperatureDerivAll(globalmtavMC[part], part, 20) * sqrt((globalmt2avMC[part]-globalmtavMC[part]*globalmtavMC[part]) / (globalnTracksMC[part]-1.));
	      double tmp1MC = 0., tmp2MC = 0.;
	      double tmpNMCerr = sqrt(globalnTracksMC[part]) / events;
	      tmp1MC = getRadiusDerivT(tmpTMC, static_cast<double>(globalnTracksMC[part]) / events, TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass());
	      tmp2MC = getRadiusDerivN(tmpTMC, static_cast<double>(globalnTracksMC[part]) / events, TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass());
	      double tmpRMCerr = sqrt(tmp1MC*tmp1MC*tmpTMCerr*tmpTMCerr + tmp2MC*tmp2MC*tmpNMCerr*tmpNMCerr);
		  double tmpmt2th = ThermalMt2(tmpTMC, TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass());
		  double tmpchi2ndf = (globalfavMC[part]-tmpmt2th) * (globalfavMC[part]-tmpmt2th) / ((globalf2avMC[part]-globalfavMC[part]*globalfavMC[part]) / (globalnTracksMC[part]-1.));
	      fout << p_names[part].Data() << "\t" << tmpTMC << "\t" << tmpTMCerr << "\t" << tmpRMC << "\t" << tmpRMCerr << "\t" << tmpchi2ndf << "\n";
		  //cout << globalmt2avMC[part] << " " << tmpmt2th << " " << (globalmt4avMC[part]-globalmt2avMC[part]*globalmt2avMC[part]) / (globalnTracksMC[part]-1.) << "\n";
	  }
	}
	//for(int rl=0;rl<recoLevels;++rl) {
	    //printf("%s\n", LevelNames[fRecoLevel].Data());
	    fout << "Reco level " << fRecoLevel << ":" << endl;
	    fout << "Particle\tTemperature\tError stat.\tError mom.\tError tot.\tRadius\tError stat.\tError mom.\tError tot.\tchi2/ndf\n";
	    for(int part=0;part<p_sz;++part) {
		//if  (part==0) 
		{
		
			mtTacc[part] = mtTsts[part]; Npart[part] = Npartsts[part];
			if (fusePID==2 && fRecoLevel>=1) { mtTacc[part] = mtTststof[part]; Npart[part] = Npartststof[part]; }
			printf("%s Temperature\t%10lf\t%10lf\t%10lf\n", p_names[part].Data(),
				  getTemperatureAll(globalmtavMC[part], part, 20),
				  getTemperatureAll(globalmtavReco[part], part, 20),
				  getTemperatureAllCor(globalmtavReco[part], part, 20, mtTacc[part]));
			double tmpT1 = getTemperatureAll(globalmtavMC[part], part, 20);
			double tmpT2 = getTemperatureAll(globalmtavReco[part], part, 20);
			double tmpT3 = getTemperatureAllCor(globalmtavReco[part], part, 20, mtTacc[part]);
			printf("%s Radius\t%10lf\t%10lf\t%10lf\n", p_names[part].Data(),
				  getRadius(tmpT1, globalnTracksMC[part] / static_cast<double>(events), TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()),
				  getRadius(tmpT2, globalnTracksReco[part] / static_cast<double>(events), TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()),
				  getRadiusCor(tmpT3, globalnTracksReco[part] / static_cast<double>(events), TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass(), Npart[part]));
			printf("%lf\t%lf\n", Npart[part]->Eval(tmpT3), globalnTracksReco[part] / static_cast<double>(globalnTracksMC[part]));
			printf("%lf\t%lf\n", globalmtavMC[part], globalmtavReco[part]);
			
			double tmpmt2thMC = ThermalMt2(tmpT1, TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass());
		    double tmpchi2ndfMC = (globalfavMC[part]-tmpmt2thMC) * (globalfavMC[part]-tmpmt2thMC) / ((globalf2avMC[part]-globalfavMC[part]*globalfavMC[part]) / (globalnTracksMC[part]-1.));
			
			double tmpTRe = getTemperatureAllCor(globalmtavReco[part], part, 20, mtTacc[part]);
			double tmpRRe = getRadiusCor(tmpTRe, globalnTracksReco[part] / static_cast<double>(events), TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass(), Npart[part]);
			double tmpTReerr = getTemperatureDerivAllCor(globalmtavReco[part], part, 20, mtTacc[part]) * sqrt((globalmt2avReco[part]-globalmtavReco[part]*globalmtavReco[part]) / (globalnTracksReco[part]-1.));
			double tmp1Re = 0., tmp2Re = 0.;
			double tmpNReerr = sqrt(globalnTracksReco[part]) / events;
			tmp1Re = getRadiusDerivTCor(tmpTRe, static_cast<double>(globalnTracksReco[part]) / events, TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass(), Npart[part]);
			tmp2Re = getRadiusDerivNCor(tmpTRe, static_cast<double>(globalnTracksReco[part]) / events, TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass(), Npart[part]);
			double tmpRReerr = sqrt(tmp1Re*tmp1Re*tmpTReerr*tmpTReerr + tmp2Re*tmp2Re*tmpNReerr*tmpNReerr);
			
			if (0) cout << sqrt(globalmtmomerrReco[part]) / globalnTracksReco[part] << endl;
			double tmpTReerrmom = getTemperatureDerivAllCor(globalmtavReco[part], part, 20, mtTacc[part]) * sqrt(globalmtmomerrReco[part]) / globalnTracksReco[part];
			double tmpRReerrmom = sqrt(tmp1Re*tmp1Re*tmpTReerrmom*tmpTReerrmom);
			
			double tmpmt2thRecoUn = ThermalMt2(tmpT2, TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass());
		    double tmpchi2ndfRecoUn = (globalfavReco[part]-tmpmt2thRecoUn) * (globalfavReco[part]-tmpmt2thRecoUn) / ((globalf2avReco[part]-globalfavReco[part]*globalfavReco[part]) / (globalnTracksReco[part]-1.));
			
			double tmpmt2th = ThermalMt2Acc(tmpTRe, TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass(), AcceptanceSTS);
		    double tmpchi2ndf = (globalfavReco[part]-tmpmt2th) * (globalfavReco[part]-tmpmt2th) / ((globalf2avReco[part]-globalfavReco[part]*globalfavReco[part]) / (globalnTracksReco[part]-1.));
			
			fout << p_names[part].Data() << "\t" << tmpTRe << "\t" << tmpTReerr << "\t" << tmpTReerrmom << "\t" 
			      << sqrt(tmpTReerr*tmpTReerr+tmpTReerrmom*tmpTReerrmom) << "\t" << tmpRRe << "\t" << tmpRReerr << "\t" << tmpRReerrmom << "\t" 
			      << sqrt(tmpRReerr*tmpRReerr+tmpRReerrmom*tmpRReerrmom) << "\t" << tmpchi2ndf << "\n";
				
			printf("%lf\t%lf\t%lf\n", tmpchi2ndfMC, tmpchi2ndfRecoUn, tmpchi2ndf);
			
			
			ThermalDistributionFunction pl(part, tmpT1, getRadius(tmpT1, globalnTracksMC[part] / static_cast<double>(events), TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()),
											ekin, NULL, NULL);
			  
			  for(int n = 0; n < hfyMC[part]->GetNbinsX(); n++) {
				  hfyMCmodel[part]->SetBinContent(n, pl.dndylab(hfyMC[part]->GetBinCenter(n)) * static_cast<double>(events) * (hfyMC[part]->GetXaxis()->GetBinUpEdge(n) - hfyMC[part]->GetXaxis()->GetBinLowEdge(n)));
				}
				
			  for(int nx = 0; nx < hfdndydptMC[part]->GetNbinsX(); nx++) {
				  for(int ny = 0; ny < hfdndydptMC[part]->GetNbinsY(); ny++) {
					  //hfdndydptMCmodel[part]->SetBinContent(nx, ny, pl.dndydpt(hfdndydptMC[part]->GetXaxis()->GetBinCenter(nx), hfdndydptMC[part]->GetYaxis()->GetBinCenter(ny)) * globalmtavMC[0]);
					  hfdndydptMCmodel[part]->SetBinContent(nx, ny, pl.dndydptbin(hfdndydptMC[part]->GetXaxis()->GetBinLowEdge(nx), hfdndydptMC[part]->GetXaxis()->GetBinUpEdge(nx), 
						10, hfdndydptMC[part]->GetYaxis()->GetBinLowEdge(ny), hfdndydptMC[part]->GetYaxis()->GetBinUpEdge(ny), 10) * (double)events 
						* hfdndydptMC[part]->GetXaxis()->GetBinWidth(nx) * hfdndydptMC[part]->GetYaxis()->GetBinWidth(ny) );
				  }
				}
				
			ThermalDistributionFunction plR(part, tmpT2, getRadius(tmpT2, globalnTracksReco[part] / static_cast<double>(events), TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()),
											ekin, NULL, NULL);
			  
			  for(int n = 0; n < hfyReco[part]->GetNbinsX(); n++) {
				  hfyRecomodel[part]->SetBinContent(n, plR.dndylab(hfyReco[part]->GetBinCenter(n)) * static_cast<double>(events) * (hfyReco[part]->GetXaxis()->GetBinUpEdge(n) - hfyReco[part]->GetXaxis()->GetBinLowEdge(n)));
				}
				
			  for(int nx = 0; nx < hfdndydptReco[part]->GetNbinsX(); nx++) {
				  for(int ny = 0; ny < hfdndydptReco[part]->GetNbinsY(); ny++) {
					  //hfdndydptMCmodel[part]->SetBinContent(nx, ny, pl.dndydpt(hfdndydptMC[part]->GetXaxis()->GetBinCenter(nx), hfdndydptMC[part]->GetYaxis()->GetBinCenter(ny)) * globalmtavMC[0]);
					  hfdndydptRecomodel[part]->SetBinContent(nx, ny, plR.dndydptbin(hfdndydptReco[part]->GetXaxis()->GetBinLowEdge(nx), hfdndydptReco[part]->GetXaxis()->GetBinUpEdge(nx), 
						10, hfdndydptReco[part]->GetYaxis()->GetBinLowEdge(ny), hfdndydptReco[part]->GetYaxis()->GetBinUpEdge(ny), 10) * static_cast<double>(events )
						* hfdndydptReco[part]->GetXaxis()->GetBinWidth(nx) * hfdndydptReco[part]->GetYaxis()->GetBinWidth(ny) );
				  }
				}
				
			ReconstructionEfficiencyFunction reff;
			if (fTrackNumber==0) reff = ReconstructionEfficiencyFunction(0.99, 0.98, 0.135);
			else reff = ReconstructionEfficiencyFunction(0.98, 0.88, 0.2);
				
			ThermalDistributionFunction plRC(part, tmpT3, getRadiusCor(tmpT3, globalnTracksReco[part] / static_cast<double>(events), TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass(), Npart[part]),
											ekin, &AcceptanceSTS, &reff);
			  
			  for(int n = 0; n < hfyRecoCor[part]->GetNbinsX(); n++) {
				  hfyRecoCormodel[part]->SetBinContent(n, plRC.dndylab(hfyRecoCor[part]->GetBinCenter(n)) * static_cast<double>(events) * (hfyRecoCor[part]->GetXaxis()->GetBinUpEdge(n) - hfyRecoCor[part]->GetXaxis()->GetBinLowEdge(n)));
				  //hfyRecoCormodel[part]->SetBinContent(n, plRC.dndybinlab(hfyRecoCor[part]->GetXaxis()->GetBinLowEdge(n), hfyRecoCor[part]->GetXaxis()->GetBinUpEdge(n), 10) * (double)events * (hfyRecoCor[part]->GetXaxis()->GetBinUpEdge(n) - hfyRecoCor[part]->GetXaxis()->GetBinLowEdge(n)));
				}
				
			  for(int nx = 0; nx < hfdndydptRecoCor[part]->GetNbinsX(); nx++) {
				  for(int ny = 0; ny < hfdndydptRecoCor[part]->GetNbinsY(); ny++) {
					  //hfdndydptMCmodel[part]->SetBinContent(nx, ny, pl.dndydpt(hfdndydptMC[part]->GetXaxis()->GetBinCenter(nx), hfdndydptMC[part]->GetYaxis()->GetBinCenter(ny)) * globalmtavMC[0]);
					  hfdndydptRecoCormodel[part]->SetBinContent(nx, ny, plRC.dndydptbin(hfdndydptRecoCor[part]->GetXaxis()->GetBinLowEdge(nx), hfdndydptRecoCor[part]->GetXaxis()->GetBinUpEdge(nx), 
						10, hfdndydptRecoCor[part]->GetYaxis()->GetBinLowEdge(ny), hfdndydptRecoCor[part]->GetYaxis()->GetBinUpEdge(ny), 10) * static_cast<double>(events) 
						* hfdndydptRecoCor[part]->GetXaxis()->GetBinWidth(nx) * hfdndydptRecoCor[part]->GetYaxis()->GetBinWidth(ny) );
				  }
				}
				
			ThermalChi2Func fFCN2(hfyMC[part], hfdndydptMC[part], static_cast<double>(events));
			cout << "MC chi2/ndf = " << fFCN2.chi2dndy(part, tmpT1, getRadius(tmpT1, globalnTracksMC[part] / static_cast<double>(events), TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()), ekin, NULL, NULL)  << "\n";
		    //fflush(stdout);
			cout << "MC YPtchi2/ndf = " << fFCN2.chi2ypt(part, tmpT1, getRadius(tmpT1, globalnTracksMC[part] / static_cast<double>(events), TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()), ekin, NULL, NULL)  << "\n";
			
			ThermalChi2Func fFCN2Reco(hfyReco[part], hfdndydptReco[part], static_cast<double>(events));
			cout << "Reco uncor chi2/ndf = " << fFCN2Reco.chi2dndy(part, tmpT2, getRadius(tmpT2, globalnTracksReco[part] / static_cast<double>(events), TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()), ekin, NULL, NULL, 0.05)  << "\n";
		    //fflush(stdout);
			cout << "Reco uncor YPtchi2/ndf = " << fFCN2Reco.chi2ypt(part, tmpT2, getRadius(tmpT2, globalnTracksReco[part] / static_cast<double>(events), TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass()), ekin, NULL, NULL, 0.05)  << "\n";
			
			ThermalChi2Func fFCN2RecoCor(hfyRecoCor[part], hfdndydptRecoCor[part], static_cast<double>(events));
			cout << "Reco chi2/ndf = " << fFCN2RecoCor.chi2dndy(part, tmpT3, getRadiusCor(tmpT3, globalnTracksReco[part] / static_cast<double>(events), TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass(), Npart[part]), ekin, &AcceptanceSTS, &reff, 0.05)  << "\n";
		    //fflush(stdout);
			cout << "Reco YPtchi2/ndf = " << fFCN2RecoCor.chi2ypt(part, tmpT3, getRadiusCor(tmpT3, globalnTracksReco[part] / static_cast<double>(events), TDatabasePDG::Instance()->GetParticle(pdgIds[part])->Mass(), Npart[part]), ekin, &AcceptanceSTS, &reff, 0.05)  << "\n";
			
			
		}
	    }
	//}
    //printf("Type\tRapidity\tAll\tReco\tReco cor\n");
  //}
  
}
