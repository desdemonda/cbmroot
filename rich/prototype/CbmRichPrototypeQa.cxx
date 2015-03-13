/**
 * \file CbmRichPrototypeQa.cxx
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 **/

#include "CbmRichPrototypeQa.h"
#include "FairRootManager.h"
#include "CbmRichHit.h"
#include "CbmRichRing.h"
#include "CbmTrackMatchNew.h"
#include "CbmMCTrack.h"
#include "CbmRichPoint.h"
#include "CbmRichRingFitterCOP.h"
#include "CbmRichRingFitterEllipseTau.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "CbmRichHitProducer.h"
#include "CbmDrawHist.h"
#include "CbmUtils.h"
#include "CbmRichConverter.h"

#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TCanvas.h"
#include "TEllipse.h"
#include "TClonesArray.h"
#include "TMath.h"
#include "TPad.h"
#include "TLatex.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TF1.h"
#include "TLine.h"
#include "TLegend.h"

#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <iomanip>
#include <string>

#include <boost/assign/list_of.hpp>
//#include <boost/property_tree/ptree.hpp>
//#include <boost/property_tree/json_parser.hpp>

using namespace std;
using boost::assign::list_of;

CbmRichPrototypeQa::CbmRichPrototypeQa():
   FairTask("RichGeoTestQa"),
   fOutputDir(""),
   fRichHits(NULL),
   fRichRings(NULL),
   fRichPoints(NULL),
   fMCTracks(NULL),
   fRichRingMatches(NULL),
   fCopFit(NULL),
   fTauFit(NULL),
   fCanvas(),
   fEventNum(0),
   fMinNofHits(7),
   fhHitsXY(),
   fhPointsXY(),
   fhNofPhotonsPerHit(),
   fhNofPointsEv(),
   fhNofHitsEv(),   
   fhNofHitsRing(),
   fhAaxis(),
   fhBaxis(),
   fhBoverA(),
   fhXcYcCircle(),
   fhRadius(),
   fhDR(),
   fMinAaxis(1.5),
   fMaxAaxis(7.),
   fMinBaxis(1.5),
   fMaxBaxis(7.),
   fMinRadius(1.5),
   fMaxRadius(7.),
   fhNofHitsAll(NULL),
   fhNofHitsCircleFit(NULL),
   fhNofHitsEllipseFit(NULL),
   fhNofHitsCircleFitEff(NULL),
   fhNofHitsEllipseFitEff(NULL),
   fHists(),
   fNofDrawnRings(0)
{
}

CbmRichPrototypeQa::~CbmRichPrototypeQa()
{

}

InitStatus CbmRichPrototypeQa::Init()
{
   cout << "CbmRichPrototypeQa::Init"<<endl;
   FairRootManager* ioman = FairRootManager::Instance();
   if (NULL == ioman) { Fatal("CbmRichPrototypeQa::Init","RootManager not instantised!"); }

   fRichHits = (TClonesArray*) ioman->GetObject("RichHit");
   if ( NULL == fRichHits) { Fatal("CbmRichPrototypeQa::Init","No RichHit array!"); }

	fRichRings = (TClonesArray*) ioman->GetObject("RichRing");
	if ( NULL == fRichRings) { Fatal("CbmRichPrototypeQa::Init","No RichRing array!"); }

	fRichPoints = (TClonesArray*) ioman->GetObject("RichPoint");
	if ( NULL == fRichPoints) { Fatal("CbmRichPrototypeQa::Init","No RichPoint array!"); }

	fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
	if ( NULL == fMCTracks) { Fatal("CbmRichPrototypeQa::Init","No MCTrack array!"); }

	fRichRingMatches = (TClonesArray*) ioman->GetObject("RichRingMatch");
	if ( NULL == fRichRingMatches) { Fatal("CbmRichPrototypeQa::Init","No RichRingMatch array!"); }

   fCopFit = new CbmRichRingFitterCOP();
   fTauFit = new CbmRichRingFitterEllipseTau();

   InitHistograms();

	return kSUCCESS;
}

void CbmRichPrototypeQa::Exec(
      Option_t* option)
{
	fEventNum++;
	cout << "CbmRichPrototypeQa, event No. " <<  fEventNum << endl;
	RingParameters();
}

void CbmRichPrototypeQa::InitHistograms()
{
   double xMin = -15.;
   double xMax = 15.;
   int nBinsX = 60;
   double yMin = 20.;
   double yMax = 50.;
   int nBinsY = 60;

   fhHitsXY.resize(2);
   fhPointsXY.resize(2);
   fhNofPhotonsPerHit.resize(2);
   fhNofPointsEv.resize(2);
   fhNofHitsEv.resize(2);
   fhNofHitsRing.resize(2);
   fhAaxis.resize(2);
   fhBaxis.resize(2);
   fhBoverA.resize(2);
   fhXcYcCircle.resize(2);
   fhRadius.resize(2);
   fhDR.resize(2);

   string s;
   for (int i = 0; i < 2; i++){
      if (i == 0) s = "electron";
      if (i == 1) s = "pion";

      fhHitsXY[i] = new TH2D( ("fhHitsXY"+s).c_str(), ("fhHitsXY"+s+";X [cm];Y [cm];Counter").c_str(), nBinsX, xMin, xMax, nBinsY, yMin, yMax);
      fHists.push_back(fhHitsXY[i]);
      fhPointsXY[i] = new TH2D(("fhPointsXY"+s).c_str(), ("fhPointsXY"+s+";X [cm];Y [cm];Counter").c_str(), nBinsX, xMin, xMax, nBinsY, yMin, yMax);
      fHists.push_back(fhPointsXY[i]);
      fhNofPointsEv[i] = new TH1D(("fhNofPointsEv"+s).c_str(), ("fhNofPointsEv"+s+";Nof points in event;Yield").c_str(), 200, 99.5, 299.5);
      fHists.push_back(fhNofPointsEv[i]);
      fhNofHitsRing[i] = new TH1D(("fhNofHitsRing"+s).c_str(), ("fhNofHitsRing"+s+";Nof hits in ring;Yield").c_str(), 55, -.5, 54.5);
      fHists.push_back(fhNofHitsRing[i]);
      fhNofHitsEv[i] = new TH1D(("fhNofHitsEv"+s).c_str(), ("fhNofHitsEv"+s+";Nof hits in event;Yield").c_str(), 55, -.5, 54.5);
      fHists.push_back(fhNofHitsEv[i]);

      // ellipse fitting parameters
      fhBoverA[i] = new TH1D(("fhBoverA"+s).c_str(), ("fhBoverA"+s+";B/A;Yield").c_str(), 50, 0, 1);
      fHists.push_back(fhBoverA[i]);
      fhBaxis[i] = new TH1D(("fhBaxis"+s).c_str(), ("fhBaxis"+s+";B axis [cm];Yield").c_str(), 2000, 2.5, 5.5);
      fHists.push_back(fhBaxis[i]);
      fhAaxis[i] = new TH1D(("fhAaxis"+s).c_str(), ("fhAaxis"+s+";A axis [cm];Yield").c_str(), 2000, 2.5, 5.5);
      fHists.push_back(fhAaxis[i]);

      // circle fitting parameters
      fhXcYcCircle[i] = new TH2D(("fhXcYcCircle"+s).c_str(), ("fhXcYcCircle"+s+";x [cm];y [cm];Yield").c_str(), nBinsX, xMin, xMax, nBinsY, yMin, yMax);
      fHists.push_back(fhXcYcCircle[i]);
      fhRadius[i] = new TH1D(("fhRadius"+s).c_str(), ("fhRadius"+s+";Radius [cm];Yield").c_str(), 2000, 2.5, 5.5);
      fHists.push_back(fhRadius[i]);
      fhDR[i] = new TH1D(("fhDR"+s).c_str(), ("fhDR"+s+";dR [cm];Yield").c_str(), 100, -1., 1.);
      fHists.push_back(fhDR[i]);

      fhNofPhotonsPerHit[i] = new TH1D(("fhNofPhotonsPerHit"+s).c_str(), ("fhNofPhotonsPerHit"+s+";Number of photons per hit;Yield").c_str(), 10, -0.5, 9.5);
      fHists.push_back(fhNofPhotonsPerHit[i]);

   }

   // Fitting efficiency.
   fhNofHitsAll = new TH1D("fhNofHitsAll", "fhNofHitsAll;Nof hits in ring;Yield", 50, 0., 50.);
   fHists.push_back(fhNofHitsAll);
   fhNofHitsCircleFit = new TH1D("fhNofHitsCircleFit", "fhNofHitsCircleFit;Nof hits in ring;Yield", 50, 0., 50.);
   fHists.push_back(fhNofHitsCircleFit);
   fhNofHitsEllipseFit = new TH1D("fhNofHitsEllipseFit", "fhNofHitsEllipseFit;Nof hits in ring;Yield", 50, 0., 50.);
   fHists.push_back(fhNofHitsEllipseFit);
   fhNofHitsCircleFitEff = new TH1D("fhNofHitsCircleFitEff", "fhNofHitsCircleFitEff;Nof hits in ring;Efficiency [%]", 50, 0., 50.);
   fHists.push_back(fhNofHitsCircleFitEff);
   fhNofHitsEllipseFitEff = new TH1D("fhNofHitsEllipseFitEff", "fhNofHitsEllipseFitEff;Nof hits in ring;Efficiency [%]", 50, 0., 50.);
   fHists.push_back(fhNofHitsEllipseFitEff);
}

void CbmRichPrototypeQa::RingParameters()
{
	Int_t nofRings = fRichRings->GetEntriesFast();
   int hIndex = 0;
	for (Int_t iR = 0; iR < nofRings; iR++){
		CbmRichRing *ring = (CbmRichRing*) fRichRings->At(iR);
		if (NULL == ring) continue;
		CbmTrackMatchNew* ringMatch = (CbmTrackMatchNew*) fRichRingMatches->At(iR);
		if (NULL == ringMatch) continue;

		Int_t mcTrackId = ringMatch->GetMatchedLink().GetIndex();
		if (mcTrackId < 0) continue;
		CbmMCTrack* mcTrack = (CbmMCTrack*)fMCTracks->At(mcTrackId);
		if (!mcTrack) continue;
		Int_t motherId = mcTrack->GetMotherId();
		Int_t pdg = TMath::Abs(mcTrack->GetPdgCode());
		Double_t momentum = mcTrack->GetP();
      Double_t pt = mcTrack->GetPt();
      Double_t rapidity = mcTrack->GetRapidity();

      if (motherId != -1) continue; // only primary

		CbmRichRingLight ringPoint;
		int nofRichPoints = fRichPoints->GetEntriesFast();
		for (int iPoint = 0; iPoint < nofRichPoints; iPoint++){
		   CbmRichPoint* richPoint = (CbmRichPoint*) fRichPoints->At(iPoint);
		   if (NULL == richPoint) continue;
		   Int_t trackId = richPoint->GetTrackID();
		   if (trackId < 0) continue;
	      CbmMCTrack* mcTrackRich = (CbmMCTrack*)fMCTracks->At(trackId);
	      if (NULL == mcTrackRich) continue;
	      int motherIdRich = mcTrackRich->GetMotherId();
		   if (motherIdRich == mcTrackId){
		      CbmRichHitLight hit(richPoint->GetX(), richPoint->GetY());
		      ringPoint.AddHit(hit);
		   }
		}
	   fhNofHitsAll->Fill(ring->GetNofHits());

	   CbmRichRingLight ringHit;
	   CbmRichConverter::CopyHitsToRingLight(ring, &ringHit);

	   if (pdg == 11) hIndex = 0;
	   if (pdg == 211) hIndex = 1;
      FitAndFillHistCircle(&ringHit, hIndex); //hits
      double r = ringHit.GetRadius();
      double xc = ringHit.GetCenterX();
      double yc = ringHit.GetCenterY();

      if (ringHit.GetRadius() > fMinRadius && ringHit.GetRadius() < fMaxRadius){
         fhNofHitsCircleFit->Fill(ringHit.GetNofHits());
      }
	   if (fNofDrawnRings < 10 && ringHit.GetNofHits() <= 100){
	      DrawRing(&ringHit, &ringPoint);
	   }

		FitAndFillHistEllipse(&ringHit, hIndex);// hits
	}// iR

	if (nofRings == 1) HitsAndPoints(hIndex);
}

void CbmRichPrototypeQa::FitAndFillHistEllipse(
      CbmRichRingLight* ring,
      int hIndex)
{
   fTauFit->DoFit(ring);
   double axisA = ring->GetAaxis();
   double axisB = ring->GetBaxis();
   double xcEllipse = ring->GetCenterX();
   double ycEllipse = ring->GetCenterY();
   int nofHitsRing = ring->GetNofHits();
   if (axisA > fMinAaxis && axisA < fMaxAaxis &&  axisB > fMinBaxis && axisB < fMaxBaxis ){
      fhBoverA[hIndex]->Fill(axisB/axisA);
   }
   fhNofHitsRing[hIndex]->Fill(nofHitsRing);
   fhBaxis[hIndex]->Fill(axisB);
   fhAaxis[hIndex]->Fill(axisA);
}

void CbmRichPrototypeQa::FitAndFillHistCircle(
      CbmRichRingLight* ring,
      int hIndex)
{
   fCopFit->DoFit(ring);
   double radius = ring->GetRadius();
   double xcCircle = ring->GetCenterX();
   double ycCircle = ring->GetCenterY();
   int nofHitsRing = ring->GetNofHits();
   fhXcYcCircle[hIndex]->Fill(xcCircle, ycCircle);
   fhRadius[hIndex]->Fill(radius);

   for (int iH = 0; iH < ring->GetNofHits(); iH++){
      double xh = ring->GetHit(iH).fX;
      double yh = ring->GetHit(iH).fY;
      double dr = radius - sqrt((xcCircle - xh)*(xcCircle - xh) + (ycCircle - yh)*(ycCircle - yh));
      fhDR[hIndex]->Fill(dr);
   }
}

void CbmRichPrototypeQa::HitsAndPoints(
      int hIndex)
{
   Int_t nofHits = fRichHits->GetEntriesFast();
   fhNofHitsEv[hIndex]->Fill(nofHits);
   for (Int_t iH = 0; iH < nofHits; iH++){
      CbmRichHit *hit = (CbmRichHit*) fRichHits->At(iH);
      if ( hit == NULL ) continue;

      fhNofPhotonsPerHit[hIndex]->Fill(hit->GetNPhotons());
      fhHitsXY[hIndex]->Fill(hit->GetX(), hit->GetY());
   }

   Int_t nofPoints = fRichPoints->GetEntriesFast();
   fhNofPointsEv[hIndex]->Fill(nofPoints);
   for (Int_t iP = 0; iP < nofPoints; iP++){
      CbmRichPoint *point = (CbmRichPoint*) fRichPoints->At(iP);
      if ( point == NULL ) continue;
      fhPointsXY[hIndex]->Fill(point->GetX(), point->GetY());
   }
}

void CbmRichPrototypeQa::DrawRing(
      CbmRichRingLight* ringHit,
      CbmRichRingLight* ringPoint)
{
   stringstream ss;
   ss << "Event" << fNofDrawnRings;
   fNofDrawnRings++;
   TCanvas *c = CreateCanvas(ss.str().c_str(), ss.str().c_str(), 500, 500);
   c->SetGrid(true, true);
   TH2D* pad = new TH2D("pad", ";X [cm];Y [cm]", 1, -10., 10., 1, -10., 10);
   pad->SetStats(false);
   pad->Draw();

   // find min and max x and y positions of the hits
   // in order to shift drawing
   double xmin = 99999., xmax = -99999., ymin = 99999., ymax = -99999.;
   for (int i = 0; i < ringHit->GetNofHits(); i++){
      double hitX = ringHit->GetHit(i).fX;
      double hitY = ringHit->GetHit(i).fY;
      if (xmin > hitX) xmin = hitX;
      if (xmax < hitX) xmax = hitX;
      if (ymin > hitY) ymin = hitY;
      if (ymax < hitY) ymax = hitY;
   }
   double xCur = (xmin + xmax) / 2.;
   double yCur = (ymin + ymax) / 2.;

   //Draw circle and center
   TEllipse* circle = new TEllipse(ringHit->GetCenterX() - xCur,
         ringHit->GetCenterY() - yCur, ringHit->GetRadius());
   circle->SetFillStyle(0);
   circle->SetLineWidth(3);
   circle->Draw();
   TEllipse* center = new TEllipse(ringHit->GetCenterX() - xCur, ringHit->GetCenterY() - yCur, .25);
   center->SetFillColor(kBlack);
   center->Draw();

   //Draw hits in event
   int nofHitsInEvent = fRichHits->GetEntriesFast();
   for (int i = 0; i < nofHitsInEvent; i++){
      CbmRichHit* hit = (CbmRichHit*)fRichHits->At(i);
      if (NULL == hit) continue;
      TEllipse* hitDr = new TEllipse(hit->GetX() - xCur, hit->GetY() - yCur, .5);
      hitDr->SetFillColor(kGreen);
      if (hit->GetRefId() == -1) hitDr->SetFillColor(kOrange-2);
      hitDr->Draw();
   }

   // Draw hits in the ring
   for (int i = 0; i < ringHit->GetNofHits(); i++){
      TEllipse* hitDr = new TEllipse(ringHit->GetHit(i).fX - xCur, ringHit->GetHit(i).fY - yCur, .37);
      hitDr->SetFillColor(kRed);
      hitDr->Draw();
   }

   // Draw MC Points
   for (int i = 0; i < ringPoint->GetNofHits(); i++){
      TEllipse* pointDr = new TEllipse(ringPoint->GetHit(i).fX - xCur, ringPoint->GetHit(i).fY - yCur, 0.1);
      pointDr->SetFillColor(kBlue);
      pointDr->Draw();
   }

   //Draw information
   stringstream ss2;
   ss2 << "(r, n)=(" << setprecision(3) << ringHit->GetRadius() << ", " << ringHit->GetNofHits()<<")";
   TLatex* latex = new TLatex(-8., 8., ss2.str().c_str());
   latex->Draw();
}

void CbmRichPrototypeQa::DrawH1andFit(
      TH1D* hist)
{
  // TH1D* py = hist->ProjectionY( (string(hist->GetName())+ "_py" ).c_str() );
   hist->GetYaxis()->SetTitle("Yield");
   DrawH1(hist);
   double scale = hist->Integral();
   hist->Scale(1./ scale);
  // hist->GetXaxis()->SetRangeUser(2., 8.);
   hist->Fit("gaus", "Q");
   hist->GetFunction("gaus")->SetLineColor(kBlack);
   double m = hist->GetFunction("gaus")->GetParameter(1);
   double s = hist->GetFunction("gaus")->GetParameter(2);
   string txt1 = Cbm::NumberToString<double>(m, 2) + " / " + Cbm::NumberToString<double>(s, 2);
   TLatex text;
   text.SetTextAlign(21);
   text.SetTextSize(0.06);
   text.DrawTextNDC(0.5, 0.92, txt1.c_str());
  // gPad->SetLogy(true);
}

void CbmRichPrototypeQa::CreatePionSuppression()
{
   double m = fhRadius[0]->GetFunction("gaus")->GetParameter(1);
   double s = fhRadius[0]->GetFunction("gaus")->GetParameter(2);
   double cut2s = m - 2*s;
   double cut3s = m - 3*s;
   cout << "Radius_mean = " << m << ", Radius_sigma = " << s << endl;

   int nBinsX = fhRadius[0]->GetNbinsX();
   TH1D* ps = new TH1D("fhRadiusPionSupp", "fhRadiusPionSupp;Radius [cm];Pion suppression", nBinsX , fhRadius[0]->GetXaxis()->GetXmin(), fhRadius[0]->GetXaxis()->GetXmax());
   TH1D* elEff = new TH1D("fhRadiusElEff", "fhRadiusElEff;Radius [cm];Efficiency, electrons [%]", nBinsX , fhRadius[0]->GetXaxis()->GetXmin(), fhRadius[0]->GetXaxis()->GetXmax());

   double nofPions = fhRadius[1]->Integral();
   double nofElectrons = fhRadius[1]->Integral();
   for (int i = 1; i <= nBinsX; i++){
      double np = nofPions / (nofPions - fhRadius[1]->Integral(1, i));
      if (np < 9e7) ps->SetBinContent(i, np);

      double ee = 100. * (nofElectrons - fhRadius[0]->Integral(1, i)) / nofElectrons;
      elEff->SetBinContent(i, ee);

      if (i != 1 && ps->GetBinCenter(i) >= cut2s && ps->GetBinCenter(i-1) <= cut2s) {
         cout << "2sigma cut : pion suppression = " << np << " at " << ee << "% electron efficiency" << endl;
      }
      if (i != 1 && ps->GetBinCenter(i) >= cut3s && ps->GetBinCenter(i-1) <= cut3s) {
         cout << "3sigma cut : pion suppression = " << np << " at " << ee << "% electron efficiency" << endl;
      }
   }

   TCanvas *cPiSupp = CreateCanvas("protqa_pion_suppression", "protqa_pion_suppression", 1500, 600);
   cPiSupp->Divide(3,1);
   cPiSupp->cd(1);
   DrawH1(list_of(fhRadius[0])(fhRadius[1]), list_of("el")("pi"), kLinear, kLinear, true, 0.8,0.8, 0.99, 0.99, "");
   TLine* lineR = new TLine(cut2s, 0, cut2s, fhRadius[0]->GetMaximum());
   lineR->SetLineWidth(3);
   lineR->SetLineColor(kGreen+2);
   lineR->Draw();
   gPad->SetLogy(true);

   cPiSupp->cd(2);
   ps->GetXaxis()->SetRangeUser(3.7, 5.3);
   DrawH1(ps);
   TLine* linePs = new TLine(cut2s, 0, cut2s, ps->GetMaximum());
   linePs->SetLineWidth(3.);
   linePs->SetLineColor(kGreen+2);
   linePs->Draw();

   cPiSupp->cd(3);
   elEff->GetXaxis()->SetRangeUser(3.7, 5.3);
   DrawH1(elEff);
   TLine* lineEff = new TLine(cut2s, 0, cut2s, elEff->GetMaximum());
   lineEff->SetLineWidth(3);
   lineEff->SetLineColor(kGreen+2);
   lineEff->Draw();

}

void CbmRichPrototypeQa::DrawHist()
{
   SetDefaultDrawStyle();

   string s;
   for (int i = 0; i < 2; i++) {
      if (i == 0) s = "_electrons";
      if (i == 1) s = "_pions";
      TCanvas *cHitsXY = CreateCanvas(("protqa_hits_xy"+s).c_str(), ("protqa_hits_xy"+s).c_str(), 800, 800);
      DrawH2(fhHitsXY[i]);

      TCanvas *cPointsXY = CreateCanvas(("protqa_points_xy"+s).c_str(), ("protqa_points_xy"+s).c_str(), 800, 800);
      DrawH2(fhPointsXY[i]);

      TCanvas *cEllipseBoa = CreateCanvas(("protqa_ellipse_boa"+s).c_str(), ("protqa_ellipse_boa"+s).c_str(), 800, 800);
      fhBoverA[i]->Scale(1./fhBoverA[i]->Integral());
      DrawH1(fhBoverA[i]);
      cout << "B/A mean = " << fhBoverA[i]->GetMean() << endl;
      gPad->SetLogy(true);

      TCanvas *cCircle = CreateCanvas(("protqa_nofhits"+s).c_str(), ("protqa_nofhits"+s).c_str(), 1200, 600);
      cCircle->Divide(2,1);
      cCircle->cd(1);
      DrawH1andFit(fhNofHitsEv[i]);
      gPad->SetLogy(true);
      cCircle->cd(2);
      DrawH1andFit(fhNofHitsRing[i]);
      gPad->SetLogy(true);

      TCanvas *cNofPoints = CreateCanvas(("protqa_nofpoints"+s).c_str(), ("protqa_nofpoints"+s).c_str(), 800, 800);
      DrawH1(fhNofPointsEv[i]);
      cout << "Number of points in event = "  << fhNofPointsEv[i]->GetMean() << endl;
      //gPad->SetLogy(true);

      TCanvas *cCircleXY = CreateCanvas(("protqa_circle_xc_yc"+s).c_str(), ("protqa_circle_xc_yc"+s).c_str(), 800, 800);
      DrawH2(fhXcYcCircle[i]);

      TCanvas *cCircleDr = CreateCanvas(("protqa_dr"+s).c_str(), ("protqa_dr"+s).c_str(), 800, 800);
      DrawH1andFit(fhDR[i]);

      TCanvas* cNofPhotons = CreateCanvas(("protqa_nof_photons_per_hit"+s).c_str(), ("protqa_nof_photons_per_hit"+s).c_str(), 800, 800);
      fhNofPhotonsPerHit[i]->Scale(1./fhNofPhotonsPerHit[i]->Integral());
      DrawH1(fhNofPhotonsPerHit[i]);

      TCanvas* cHitsRAB = CreateCanvas(("protqa_hits_rab"+s).c_str(), ("protqa_hits_rab"+s).c_str(), 1500, 600);
      cHitsRAB->Divide(3, 1);
      cHitsRAB->cd(1);
      DrawH1andFit(fhRadius[i]);
      gPad->SetLogy(true);
      cHitsRAB->cd(2);
      DrawH1andFit(fhAaxis[i]);
      gPad->SetLogy(true);
      cHitsRAB->cd(3);
      DrawH1andFit(fhBaxis[i]);
      gPad->SetLogy(true);
   }

   CreatePionSuppression();

   TCanvas *cFitEff = CreateCanvas("protqa_fit_eff", "protqa_fit_eff", 1200, 400);
   cFitEff->Divide(3,1);
   cFitEff->cd(1);
   DrawH1( list_of((TH1D*)fhNofHitsAll->Clone())((TH1D*)fhNofHitsCircleFit->Clone())((TH1D*)fhNofHitsEllipseFit->Clone()),
         list_of("All")("Circle fit")("Ellipse fit"), kLinear, kLog, true, 0.7, 0.7, 0.99, 0.99);
   fhNofHitsCircleFitEff = DivideH1(fhNofHitsCircleFit, fhNofHitsAll, "", "", "Nof hits in ring", "Efficiency");
   fhNofHitsEllipseFitEff = DivideH1(fhNofHitsEllipseFit, fhNofHitsAll, "", "", "Nof hits in ring", "Efficiency");
   cFitEff->cd(2);
   DrawH1(fhNofHitsCircleFitEff);
   TLatex* circleFitEffTxt = new TLatex(15, 0.5, CalcEfficiency(fhNofHitsCircleFit, fhNofHitsAll).c_str());
   circleFitEffTxt->Draw();
   cFitEff->cd(3);
   DrawH1(fhNofHitsEllipseFitEff);
   TLatex* ellipseFitEffTxt = new TLatex(15, 0.5, CalcEfficiency(fhNofHitsEllipseFit, fhNofHitsAll).c_str());
   ellipseFitEffTxt->Draw();

}


void CbmRichPrototypeQa::Finish()
{
   DrawHist();
   for (Int_t i = 0; i < fHists.size(); i++){
   //   if (NULL != fHists[i]) fHists[i]->Write();
   }
   SaveCanvasToImage();
}

string CbmRichPrototypeQa::CalcEfficiency(
   TH1* histRec,
   TH1* histAcc)
{
   if (histAcc->GetEntries() == 0) { return "0.0"; }
   else {
      Double_t eff = Double_t(histRec->GetEntries()) / Double_t(histAcc->GetEntries());
      stringstream ss;
      ss.precision(3);
      ss << eff;
      return ss.str();
   }
}


TH1D* CbmRichPrototypeQa::DivideH1(
   TH1D* h1,
   TH1D* h2,
   const string& name,
   const string& title,
   const string& axisX,
   const string& axisY)
{
   h1->Sumw2();
   h2->Sumw2();
   TH1D* h3 = new TH1D(name.c_str(), string(title +";"+axisX+";"+ axisY).c_str(),
                       h1->GetNbinsX(), h1->GetXaxis()->GetXmin(),h1->GetXaxis()->GetXmax());
   h3->Divide(h1, h2, 100., 1., "B");
   return h3;
}

TCanvas* CbmRichPrototypeQa::CreateCanvas(
      const string& name,
      const string& title,
      int width,
      int height)
{
   TCanvas* c = new TCanvas(name.c_str(), title.c_str(), width, height);
   fCanvas.push_back(c);
   return c;
}

void CbmRichPrototypeQa::SaveCanvasToImage()
{
   for (int i = 0; i < fCanvas.size(); i++)
   {
      Cbm::SaveCanvasAsImage(fCanvas[i], fOutputDir);
   }
}

ClassImp(CbmRichPrototypeQa)

