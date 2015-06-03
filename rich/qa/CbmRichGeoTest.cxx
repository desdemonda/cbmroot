/**
 * \file CbmRichGeoTest.cxx
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 **/

#include "CbmRichGeoTest.h"
#include "CbmRichGeoTestStudyReport.h"
#include "FairRootManager.h"
#include "CbmRichHit.h"
#include "CbmRichRing.h"
#include "CbmTrackMatchNew.h"
#include "CbmMCTrack.h"
#include "CbmRichPoint.h"
#include "CbmRichRingFitterCOP.h"
#include "CbmRichRingFitterEllipseTau.h"
#include "CbmGeoRichPar.h"
#include "FairGeoTransform.h"
#include "FairGeoNode.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "CbmRichHitProducer.h"
#include "CbmDrawHist.h"
#include "utils/CbmUtils.h"
#include "CbmRichConverter.h"
#include "CbmReport.h"
#include "CbmStudyReport.h"
#include "CbmHistManager.h"

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
#include "TLegend.h"

#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <iomanip>
#include <string>

#include <boost/assign/list_of.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;
using boost::assign::list_of;
using boost::property_tree::ptree;

CbmRichGeoTest::CbmRichGeoTest():
   FairTask("RichGeoTestQa"),
   fOutputDir(""),
   fRichHits(NULL),
   fRichRings(NULL),
   fRichPoints(NULL),
   fMCTracks(NULL),
   fRichRingMatches(NULL),
   fGP(),
   fCopFit(NULL),
   fTauFit(NULL),
   fCanvas(),
   fEventNum(0),
   fMinNofHits(0),
   fMinAaxis(0.),
   fMaxAaxis(0.),
   fMinBaxis(0.),
   fMaxBaxis(0.),
   fMinRadius(0.),
   fMaxRadius(0.),
   fNofDrawnRings(0),
   fHM(NULL)
{
	fEventNum = 0;
	fNofDrawnRings = 0;
	fMinNofHits = 7;

   fMinAaxis = 3.;
   fMaxAaxis = 7.;
   fMinBaxis = 3.;
   fMaxBaxis = 7.;
   fMinRadius = 3.;
   fMaxRadius = 7.;

}

CbmRichGeoTest::~CbmRichGeoTest()
{

}

InitStatus CbmRichGeoTest::Init()
{
   cout << "CbmRichGeoTest::Init"<<endl;
   FairRootManager* ioman = FairRootManager::Instance();
   if (NULL == ioman) { Fatal("CbmRichGeoTest::Init","RootManager not instantised!"); }

   fGP = CbmRichHitProducer::InitGeometry();

   fRichHits = (TClonesArray*) ioman->GetObject("RichHit");
   if ( NULL == fRichHits) { Fatal("CbmRichGeoTest::Init","No RichHit array!"); }

	fRichRings = (TClonesArray*) ioman->GetObject("RichRing");
	if ( NULL == fRichRings) { Fatal("CbmRichGeoTest::Init","No RichRing array!"); }

	fRichPoints = (TClonesArray*) ioman->GetObject("RichPoint");
	if ( NULL == fRichPoints) { Fatal("CbmRichGeoTest::Init","No RichPoint array!"); }

	fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
	if ( NULL == fMCTracks) { Fatal("CbmRichGeoTest::Init","No MCTrack array!"); }

	fRichRingMatches = (TClonesArray*) ioman->GetObject("RichRingMatch");
	if ( NULL == fRichRingMatches) { Fatal("CbmRichGeoTest::Init","No RichRingMatch array!"); }

   fCopFit = new CbmRichRingFitterCOP();
   fTauFit = new CbmRichRingFitterEllipseTau();

   InitHistograms();

	return kSUCCESS;
}

void CbmRichGeoTest::Exec(
      Option_t* option)
{
	fEventNum++;
	cout << "CbmRichGeoTest, event No. " <<  fEventNum << endl;
	FillMcHist();
	RingParameters();
	HitsAndPoints();
}

void CbmRichGeoTest::InitHistograms()
{
   double xMin = -110.;
   double xMax = 110.;
   int nBinsX = 50;
   double yMin = -300;
   double yMax = 300.;
   int nBinsY = 80;

   fHM = new CbmHistManager();

   fHM->Create2<TH2D>("fhHitsXY", "fhHitsXY;X [cm];Y [cm];Counter", nBinsX, xMin, xMax, nBinsY, yMin, yMax);
   fHM->Create2<TH2D>("fhPointsXY", "fhPointsXY;X [cm];Y [cm];Counter", 3*nBinsX, xMin, xMax, 3*nBinsY, yMin, yMax);

   for (Int_t i = 0; i < 2; i++){
      stringstream ss;
      if (i == 0) ss << "_hits";
      if (i == 1) ss << "_points";
      string t = ss.str();
      fHM->Create1<TH1D>("fhNofHits"+t, "fhNofHits"+t+";Nof hits in ring;Yield", 300, -.5, 299.5);
      // ellipse fitting parameters
      fHM->Create1<TH1D>("fhBoverA"+t, "fhBoverA"+t+";B/A;Yield", 50, 0, 1);
      fHM->Create2<TH2D>("fhXcYcEllipse"+t, "fhXcYcEllipse"+t+";x [cm];y [cm];Yield", nBinsX, xMin, xMax, nBinsY, yMin, yMax);
      fHM->Create2<TH2D>("fhBaxisVsMom"+t, "fhBaxisVsMom"+t+";p [GeV/c];B axis [cm];Yield", 100, 0., 10, 200, 0., 10.);
      fHM->Create2<TH2D>("fhAaxisVsMom"+t, "fhAaxisVsMom"+t+";p [GeV/c];A axis [cm];Yield", 100, 0., 10, 200, 0., 10.);
      fHM->Create2<TH2D>("fhChi2EllipseVsMom"+t, "fhChi2EllipseVsMom"+t+";p [GeV/c];#Chi^{2};Yield", 100, 0., 10., 50, 0., 0.5);
      // circle fitting parameters
      fHM->Create2<TH2D>("fhXcYcCircle"+t, "fhXcYcCircle"+t+";x [cm];y [cm];Yield", nBinsX, xMin, xMax, nBinsY, yMin, yMax);
      fHM->Create2<TH2D>("fhRadiusVsMom"+t, "fhRadiusVsMom"+t+";p [GeV/c];Radius [cm];Yield", 100, 0., 10, 200, 0., 10.);
      fHM->Create2<TH2D>("fhChi2CircleVsMom"+t, "fhChi2CircleVsMom"+t+";p [GeV/c];#Chi^{2};Yield", 100, 0., 10., 50, 0., .5);
      fHM->Create2<TH2D>("fhDRVsMom"+t, "fhDRVsMom"+t+";p [GeV/c];dR [cm];Yield", 100, 0, 10, 200, -2., 2.);
   }

   fHM->Create1<TH1D>("fhNofPhotonsPerHit", "fhNofPhotonsPerHit;Number of photons per hit;Yield", 10, -0.5, 9.5);

   // Difference between Mc Points and Hits fitting.
   fHM->Create2<TH2D>("fhDiffAaxis", "fhDiffAaxis;Nof hits in ring;A_{point}-A_{hit} [cm];Yield", 40, 0., 40., 100, -1.5, 1.5);
   fHM->Create2<TH2D>("fhDiffBaxis", "fhDiffBaxis;Nof hits in ring;B_{point}-B_{hit} [cm];Yield", 40, 0., 40., 100, -1.5, 1.5);
   fHM->Create2<TH2D>("fhDiffXcEllipse", "fhDiffXcEllipse;Nof hits in ring;Xc_{point}-Xc_{hit} [cm];Yield", 40, 0., 40., 100, -1.5, 1.5);
   fHM->Create2<TH2D>("fhDiffYcEllipse", "fhDiffYcEllipse;Nof hits in ring;Yc_{point}-Yc_{hit} [cm];Yield", 40, 0., 40., 100, -1.5, 1.5);
   fHM->Create2<TH2D>("fhDiffXcCircle", "fhDiffXcCircle;Nof hits in ring;Xc_{point}-Xc_{hit} [cm];Yield", 40, 0., 40., 100, -1.5, 1.5);
   fHM->Create2<TH2D>("fhDiffYcCircle", "fhDiffYcCircle;Nof hits in ring;Yc_{point}-Yc_{hit} [cm];Yield", 40, 0., 40., 100, -1.5, 1.5);
   fHM->Create2<TH2D>("fhDiffRadius", "fhDiffRadius;Nof hits in ring;Radius_{point}-Radius_{hit} [cm];Yield", 40, 0., 40., 100, -1.5, 1.5);

   // R, A, B distribution for different number of hits from 0 to 40.
   fHM->Create2<TH2D>("fhRadiusVsNofHits", "fhRadiusVsNofHits;Nof hits in ring;Radius [cm];Yield", 40, 0., 40., 100, 0., 10.);
   fHM->Create2<TH2D>("fhAaxisVsNofHits", "fhAaxisVsNofHits;Nof hits in ring;A axis [cm];Yield", 40, 0., 40., 100, 0., 10.);
   fHM->Create2<TH2D>("fhBaxisVsNofHits", "fhBaxisVsNofHits;Nof hits in ring;B axis [cm];Yield", 40, 0., 40., 100, 0., 10.);

   // Hits and points.
   fHM->Create1<TH1D>("fhDiffXhit", "fhDiffXhit;Y_{point}-Y_{hit} [cm];Yield", 200, -1., 1.);
   fHM->Create1<TH1D>("fhDiffYhit", "fhDiffYhit;Y_{point}-Y_{hit} [cm];Yield", 200, -1., 1.);

   // Fitting efficiency.
   fHM->Create1<TH1D>("fhNofHitsAll", "fhNofHitsAll;Nof hits in ring;Yield", 50, 0., 50.);
   fHM->Create1<TH1D>("fhNofHitsCircleFit", "fhNofHitsCircleFit;Nof hits in ring;Yield", 50, 0., 50.);
   fHM->Create1<TH1D>("fhNofHitsEllipseFit", "fhNofHitsEllipseFit;Nof hits in ring;Yield", 50, 0., 50.);
   fHM->Create1<TH1D>("fhNofHitsCircleFitEff", "fhNofHitsCircleFitEff;Nof hits in ring;Efficiency [%]", 50, 0., 50.);
   fHM->Create1<TH1D>("fhNofHitsEllipseFitEff", "fhNofHitsEllipseFitEff;Nof hits in ring;Efficiency [%]", 50, 0., 50.);

   // Detector acceptance efficiency vs. (pt,y) and p
   fHM->Create1<TH1D>("fhMcMomEl", "fhMcMomEl;p [GeV/c];Yield", 24, 0., 12.);
   fHM->Create2<TH2D>("fhMcPtyEl", "fhMcPtyEl;Rapidity;P_{t} [GeV/c];Yield", 25, 0., 4., 20, 0., 3.);
   fHM->Create1<TH1D>("fhAccMomEl", "fhAccMomEl;p [GeV/c];Yield", 24, 0., 12.);
   fHM->Create2<TH2D>("fhAccPtyEl", "fhAccPtyEl;Rapidity;P_{t} [GeV/c];Yield",25, 0., 4., 20, 0., 3.);

   fHM->Create1<TH1D>("fhMcMomPi", "fhMcMomPi;p [GeV/c];Yield", 24, 0., 12.);
   fHM->Create2<TH2D>("fhMcPtyPi", "fhMcPtyPi;Rapidity;P_{t} [GeV/c];Yield", 25, 0., 4., 20, 0., 3.);
   fHM->Create1<TH1D>("fhAccMomPi", "fhAccMomPi;p [GeV/c];Yield", 24, 0., 12.);
   fHM->Create2<TH2D>("fhAccPtyPi", "fhAccPtyPi;Rapidity;P_{t} [GeV/c];Yield", 25, 0., 4., 20, 0., 3.);

   // Numbers in dependence on XY position onto the photodetector.
   fHM->Create3<TH3D>("fhNofHitsXYZ", "fhNofHitsXYZ;X [cm];Y [cm];Nof hits in ring", nBinsX, xMin, xMax, nBinsY, yMin, yMax, 50, 0., 50);
   fHM->Create3<TH3D>("fhNofPointsXYZ", "fhNofPointsXYZ;X [cm];Y [cm];Nof points in ring", nBinsX, xMin, xMax, nBinsY, yMin, yMax, 50, 100., 300.);
   fHM->Create3<TH3D>("fhBoverAXYZ", "fhBoverAXYZ;X [cm];Y [cm];B/A", nBinsX, xMin, xMax, nBinsY, yMin, yMax, 50, 0., 1.);
   fHM->Create3<TH3D>("fhBaxisXYZ", "fhBaxisXYZ;X [cm];Y [cm];B axis [cm]", nBinsX, xMin, xMax, nBinsY, yMin, yMax, 80, 3., 7.);
   fHM->Create3<TH3D>("fhAaxisXYZ", "fhAaxisXYZ;X [cm];Y [cm];A axis [cm]", nBinsX, xMin, xMax, nBinsY, yMin, yMax, 80, 3., 7.);
   fHM->Create3<TH3D>("fhRadiusXYZ", "fhRadiusXYZ;X [cm];Y [cm];Radius [cm]", nBinsX, xMin, xMax, nBinsY, yMin, yMax, 80, 3., 7.);
   fHM->Create3<TH3D>("fhdRXYZ", "fhdRXYZ;X [cm];Y [cm];dR [cm]", nBinsX, xMin, xMax, nBinsY, yMin, yMax, 100, -1., 1.);
}

void CbmRichGeoTest::FillMcHist()
{
   for (Int_t i = 0; i < fMCTracks->GetEntriesFast(); i++){
      CbmMCTrack* mcTrack = (CbmMCTrack*)fMCTracks->At(i);
      if (!mcTrack) continue;
      Int_t motherId = mcTrack->GetMotherId();
      Int_t pdg = TMath::Abs(mcTrack->GetPdgCode());

      if (pdg == 11 && motherId == -1){
    	  fHM->H1("fhMcMomEl")->Fill(mcTrack->GetP());
    	  fHM->H2("fhMcPtyEl")->Fill(mcTrack->GetRapidity(), mcTrack->GetPt());
      }

      if (pdg == 211 && motherId == -1){
    	  fHM->H1("fhMcMomPi")->Fill(mcTrack->GetP());
    	  fHM->H2("fhMcPtyPi")->Fill(mcTrack->GetRapidity(), mcTrack->GetPt());
      }
   }
}

void CbmRichGeoTest::RingParameters()
{
	Int_t nofRings = fRichRings->GetEntriesFast();
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


		if (ring->GetNofHits() >= fMinNofHits){
	      if (pdg == 11 && motherId == -1){
	    	  fHM->H1("fhAccMomEl")->Fill(momentum);
	    	  fHM->H2("fhAccPtyEl")->Fill(rapidity, pt);
	      }
	      if (pdg == 211 && motherId == -1){
	    	  fHM->H1("fhAccMomPi")->Fill(momentum);
	    	  fHM->H2("fhAccPtyPi")->Fill(rapidity, pt);
	      }
		}

		if (pdg != 11 || motherId != -1) continue; // only primary electrons

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
		      TVector3 posPoint;
		      richPoint->Position(posPoint);
		      TVector3 detPoint;
		      CbmRichHitProducer::TiltPoint(&posPoint, &detPoint, fGP.fPmtPhi, fGP.fPmtTheta, fGP.fPmtZOrig);
		      CbmRichHitLight hit(detPoint.X(), detPoint.Y());
		      ringPoint.AddHit(hit);
		   }
		}
		fHM->H1("fhNofHitsAll")->Fill(ring->GetNofHits());

	   CbmRichRingLight ringHit;
	   CbmRichConverter::CopyHitsToRingLight(ring, &ringHit);

      FitAndFillHistCircle(0, &ringHit, momentum); //hits
      FitAndFillHistCircle(1, &ringPoint, momentum); // points
      FillMcVsHitFitCircle(&ringHit, &ringPoint);

      double r = ringHit.GetRadius();
      double xc = ringHit.GetCenterX();
      double yc = ringHit.GetCenterY();

      if (ringHit.GetRadius() > fMinRadius && ringHit.GetRadius() < fMaxRadius){
    	  fHM->H1("fhNofHitsCircleFit")->Fill(ringHit.GetNofHits());
      }
	   if (fNofDrawnRings < 10 && ringHit.GetNofHits() <= 500){
	      DrawRing(&ringHit, &ringPoint);
	   }

		FitAndFillHistEllipse(0, &ringHit, momentum);// hits
		FitAndFillHistEllipse(1, &ringPoint, momentum); // points
		FillMcVsHitFitEllipse(&ringHit, &ringPoint);

	   if (ringHit.GetAaxis() > fMinAaxis && ringHit.GetAaxis() < fMaxAaxis
	       &&  ringHit.GetBaxis() > fMinBaxis && ringHit.GetAaxis() < fMaxBaxis ){
		   fHM->H1("fhNofHitsEllipseFit")->Fill(ringHit.GetNofHits());

         double np = ringPoint.GetNofHits();
         double a = ringHit.GetAaxis();
         double b = ringHit.GetBaxis();
         double nh = ring->GetNofHits();

         fHM->H3("fhNofHitsXYZ")->Fill(xc, yc, nh);
         fHM->H3("fhNofPointsXYZ")->Fill(xc, yc, np);
         fHM->H3("fhBoverAXYZ")->Fill(xc, yc, b/a);
         fHM->H3("fhBaxisXYZ")->Fill(xc, yc, b);
         fHM->H3("fhAaxisXYZ")->Fill(xc, yc, a);
         fHM->H3("fhRadiusXYZ")->Fill(xc, yc, r);

         for (int iH = 0; iH < ringHit.GetNofHits(); iH++){
            double xh = ringHit.GetHit(iH).fX;
            double yh = ringHit.GetHit(iH).fY;
            double dr = r - sqrt((xc - xh)*(xc - xh) + (yc - yh)*(yc - yh));
            fHM->H3("fhdRXYZ")->Fill(xc, yc, dr);
         }
	   }
	}// iR
}

void CbmRichGeoTest::FitAndFillHistEllipse(
      int histIndex,
      CbmRichRingLight* ring,
      double momentum)
{
   fTauFit->DoFit(ring);
   double axisA = ring->GetAaxis();
   double axisB = ring->GetBaxis();
   double xcEllipse = ring->GetCenterX();
   double ycEllipse = ring->GetCenterY();
   int nofHitsRing = ring->GetNofHits();
   string t = "";
   if (histIndex == 0){
	   t = "_hits";
   } else if (histIndex == 1) {
	   t = "_points";
   }
   if (axisA > fMinAaxis && axisA < fMaxAaxis &&  axisB > fMinBaxis && axisB < fMaxBaxis ){
	   fHM->H1("fhBoverA"+t)->Fill(axisB/axisA);
	   fHM->H2("fhXcYcEllipse"+t)->Fill(xcEllipse, ycEllipse);
   }
   fHM->H1("fhNofHits"+t)->Fill(nofHitsRing);
   fHM->H2("fhBaxisVsMom"+t)->Fill(momentum, axisB);
   fHM->H2("fhAaxisVsMom"+t)->Fill(momentum, axisA);
   fHM->H2("fhChi2EllipseVsMom"+t)->Fill(momentum, ring->GetChi2()/ring->GetNofHits());
   if (histIndex == 0){ // only hit fit
	   fHM->H2("fhAaxisVsNofHits")->Fill(nofHitsRing, axisA);
	   fHM->H2("fhBaxisVsNofHits")->Fill(nofHitsRing, axisB);
   }
}

void CbmRichGeoTest::FitAndFillHistCircle(
      int histIndex,
      CbmRichRingLight* ring,
      double momentum)
{
   fCopFit->DoFit(ring);
   double radius = ring->GetRadius();
   double xcCircle = ring->GetCenterX();
   double ycCircle = ring->GetCenterY();
   int nofHitsRing = ring->GetNofHits();
   string t = "";
   if (histIndex == 0){
	   t = "_hits";
   } else if (histIndex == 1) {
   	   t = "_points";
   }
   fHM->H1("fhXcYcCircle"+t)->Fill(xcCircle, ycCircle);
   fHM->H1("fhRadiusVsMom"+t)->Fill(momentum, radius);
   fHM->H1("fhChi2CircleVsMom"+t)->Fill(momentum, ring->GetChi2()/ring->GetNofHits());

   for (int iH = 0; iH < ring->GetNofHits(); iH++){
      double xh = ring->GetHit(iH).fX;
      double yh = ring->GetHit(iH).fY;
      double dr = radius - sqrt((xcCircle - xh)*(xcCircle - xh) + (ycCircle - yh)*(ycCircle - yh));
      fHM->H1("fhDRVsMom"+t)->Fill(momentum, dr);

      //if (histIndex == 0) {
      //   fhdRXYZ->Fill(xcCircle, ycCircle, dr);
      //}
   }

   if (histIndex == 0){ // only hit fit
	   fHM->H2("fhRadiusVsNofHits")->Fill(nofHitsRing, radius);
   }
}

void CbmRichGeoTest::FillMcVsHitFitEllipse(
      CbmRichRingLight* ring,
      CbmRichRingLight* ringMc)
{
	fHM->H2("fhDiffAaxis")->Fill(ring->GetNofHits(), ringMc->GetAaxis() - ring->GetAaxis());
	fHM->H2("fhDiffBaxis")->Fill(ring->GetNofHits(),ringMc->GetBaxis() - ring->GetBaxis());
	fHM->H2("fhDiffXcEllipse")->Fill(ring->GetNofHits(),ringMc->GetCenterX() - ring->GetCenterX());
	fHM->H2("fhDiffYcEllipse")->Fill(ring->GetNofHits(),ringMc->GetCenterY() - ring->GetCenterY());
}

void CbmRichGeoTest::FillMcVsHitFitCircle(
      CbmRichRingLight* ring,
      CbmRichRingLight* ringMc)
{
	fHM->H2("fhDiffXcCircle")->Fill(ring->GetNofHits(),ringMc->GetCenterX() - ring->GetCenterX());
	fHM->H2("fhDiffYcCircle")->Fill(ring->GetNofHits(),ringMc->GetCenterY() - ring->GetCenterY());
	fHM->H2("fhDiffRadius")->Fill(ring->GetNofHits(),ringMc->GetRadius() - ring->GetRadius());
}

void CbmRichGeoTest::HitsAndPoints()
{
   Int_t nofHits = fRichHits->GetEntriesFast();
   for (Int_t iH = 0; iH < nofHits; iH++){
      CbmRichHit *hit = static_cast<CbmRichHit*>(fRichHits->At(iH));
      if ( hit == NULL ) continue;
      Int_t pointInd =  hit->GetRefId();
      if (pointInd < 0) continue;
      CbmRichPoint *point = static_cast<CbmRichPoint*>(fRichPoints->At(pointInd));
      if ( point == NULL ) continue;

      fHM->H1("fhNofPhotonsPerHit")->Fill(hit->GetNPhotons());

      TVector3 inPos(point->GetX(), point->GetY(), point->GetZ());
      TVector3 outPos;
      CbmRichHitProducer::TiltPoint(&inPos, &outPos, fGP.fPmtPhi, fGP.fPmtTheta, fGP.fPmtZOrig);
      fHM->H2("fhHitsXY")->Fill(hit->GetX(), hit->GetY());
      fHM->H1("fhDiffXhit")->Fill(hit->GetX() - outPos.X());
   	  fHM->H1("fhDiffYhit")->Fill(hit->GetY() - outPos.Y());
   }

   Int_t nofPoints = fRichPoints->GetEntriesFast();
   for (Int_t iP = 0; iP < nofPoints; iP++){
      CbmRichPoint *point = (CbmRichPoint*) fRichPoints->At(iP);
      if ( point == NULL ) continue;
      TVector3 inPos(point->GetX(), point->GetY(), point->GetZ());
      TVector3 outPos;
      CbmRichHitProducer::TiltPoint(&inPos, &outPos, fGP.fPmtPhi, fGP.fPmtTheta, fGP.fPmtZOrig);
      fHM->H2("fhPointsXY")->Fill(outPos.X(), outPos.Y());
   }
}

void CbmRichGeoTest::DrawRing(
      CbmRichRingLight* ringHit,
      CbmRichRingLight* ringPoint)
{
   stringstream ss;
   ss << "Event" << fNofDrawnRings;
   fNofDrawnRings++;
   TCanvas *c = CreateCanvas(ss.str().c_str(), ss.str().c_str(), 500, 500);
   c->SetGrid(true, true);
   TH2D* pad = new TH2D("pad", ";X [cm];Y [cm]", 1, -15., 15., 1, -15., 15);
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
   TEllipse* center = new TEllipse(ringHit->GetCenterX() - xCur, ringHit->GetCenterY() - yCur, .5);
   center->Draw();

   // Draw hits
   for (int i = 0; i < ringHit->GetNofHits(); i++){
      TEllipse* hitDr = new TEllipse(ringHit->GetHit(i).fX - xCur, ringHit->GetHit(i).fY - yCur, .5);
      hitDr->SetFillColor(kRed);
      hitDr->Draw();
   }

   // Draw MC Points
   for (int i = 0; i < ringPoint->GetNofHits(); i++){
      TEllipse* pointDr = new TEllipse(ringPoint->GetHit(i).fX - xCur, ringPoint->GetHit(i).fY - yCur, 0.15);
      pointDr->SetFillColor(kBlue);
      pointDr->Draw();
   }

   //Draw information
   stringstream ss2;
   ss2 << "(r, n)=(" << setprecision(3) << ringHit->GetRadius() << ", " << ringHit->GetNofHits()<<")";
   TLatex* latex = new TLatex(-8., 8., ss2.str().c_str());
   latex->Draw();
}

TH1D* CbmRichGeoTest::CreateAccVsMinNofHitsHist()
{
   Int_t nofMc = (Int_t)fHM->H1("fhMcMomEl")->GetEntries();
   TH1D* hist = (TH1D*)fHM->H1("fhNofHits_hits")->Clone("fhAccVsMinNofHitsHist");
   hist->GetXaxis()->SetTitle("Required min nof hits in ring");
   hist->GetYaxis()->SetTitle("Detector acceptance [%]");
   Double_t sum = 0.;
   for (int i = hist->GetNbinsX(); i > 1 ; i--){
      sum += fHM->H1("fhNofHits_hits")->GetBinContent(i);
      hist->SetBinContent(i, 100. * sum / nofMc);
   }
   return hist;
}

void CbmRichGeoTest::CreateH2MeanRms(
      TH2D* hist,
      TH1D** meanHist,
      TH1D** rmsHist)
{
   *meanHist = (TH1D*)hist->ProjectionX( (string(hist->GetName()) + "_mean").c_str() )->Clone();
   (*meanHist)->GetYaxis()->SetTitle( ("Mean and RMS. " + string(hist->GetYaxis()->GetTitle()) ).c_str());
   *rmsHist = (TH1D*)hist->ProjectionX((string(hist->GetName()) + "_rms").c_str() )->Clone();
   (*rmsHist)->GetYaxis()->SetTitle( ("RMS. "+ string(hist->GetYaxis()->GetTitle()) ).c_str());
   for (Int_t i = 1; i <= hist->GetXaxis()->GetNbins(); i++){
      stringstream ss;
      ss << string(hist->GetName()) << "_py" << i;
      TH1D* pr = hist->ProjectionY(ss.str().c_str(), i, i);
      if (*meanHist == NULL || pr == NULL) continue;
      (*meanHist)->SetBinContent(i, pr->GetMean());
      (*meanHist)->SetBinError(i, pr->GetRMS());
      (*rmsHist)->SetBinContent(i, pr->GetRMS());
   }
}

void CbmRichGeoTest::DrawH2MeanRms(
      TH2* hist,
      const string& canvasName)
{
   TH1D* mean, *rms;
   CreateH2MeanRms((TH2D*)hist, &mean, &rms);
   TCanvas *c = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 1200, 1200);
   c->Divide(2, 2);
   c->cd(1);
   DrawH2(hist);
   c->cd(2);
   TH1D* py = (TH1D*)hist->ProjectionY( (string(hist->GetName())+ "_py" ).c_str() )->Clone();
   py->Scale(1./py->Integral());
   py->GetYaxis()->SetTitle("Yield");
   DrawH1(py);
   string txt1 = Cbm::NumberToString<Double_t>(py->GetMean(), 2) + " / "
         + Cbm::NumberToString<Double_t>(py->GetRMS(), 2);
   TLatex text;
   text.SetTextAlign(21);
   text.SetTextSize(0.05);
   text.DrawTextNDC(0.5, 0.92, txt1.c_str());
   gPad->SetLogy(true);
   c->cd(3);
   DrawH1(mean);
   c->cd(4);
   DrawH1(rms);
}

void CbmRichGeoTest::DrawH1andFit(
      TH1D* hist)
{
  // TH1D* py = hist->ProjectionY( (string(hist->GetName())+ "_py" ).c_str() );
   hist->GetYaxis()->SetTitle("Yield");
   DrawH1(hist);
   hist->Scale(1./ hist->Integral());
   hist->GetXaxis()->SetRangeUser(2., 8.);
   hist->Fit("gaus", "Q");
   TF1* func = hist->GetFunction("gaus");
   if (func == NULL) return;
   func->SetLineColor(kBlack);
   double m = func->GetParameter(1);
   double s = func->GetParameter(2);
   string txt1 = Cbm::NumberToString<double>(m, 2) + " / " + Cbm::NumberToString<double>(s, 2);
   TLatex text;
   text.SetTextAlign(21);
   text.SetTextSize(0.06);
   text.DrawTextNDC(0.5, 0.92, txt1.c_str());
  // gPad->SetLogy(true);
}

void CbmRichGeoTest::DrawHist()
{
   SetDefaultDrawStyle();

   TCanvas *cHitsXY = CreateCanvas("richgeo_hits_xy", "richgeo_hits_xy", 800, 800);
   DrawH2(fHM->H2("fhHitsXY"));

   TCanvas *cPointsXY = CreateCanvas("richgeo_points_xy", "richgeo_points_xy", 800, 800);
   DrawH2(fHM->H2("fhPointsXY"));

   for (int i = 0; i < 2; i++){
      string t;
      if (i == 0) {
    	  t = "_hits";
      } else if (i == 1) {
    	  t = "_points";
      }
      TCanvas *cEllipseBoa = CreateCanvas(("richgeo" + t +"_ellipse_boa").c_str(), ("richgeo" + t +"_ellipse_boa").c_str(), 800, 800);
      fHM->H1("fhBoverA"+t)->Scale(1./fHM->H1("fhBoverA"+t)->Integral());
      DrawH1(fHM->H1("fhBoverA"+t));
      string txt1 = Cbm::NumberToString<Double_t>(fHM->H1("fhBoverA"+t)->GetMean(), 2) + " / " + Cbm::NumberToString<Double_t>(fHM->H1("fhBoverA"+t)->GetRMS(), 2);
      TLatex text;
      text.SetTextAlign(21);
      text.SetTextSize(0.05);
      text.DrawTextNDC(0.5, 0.92, txt1.c_str());
      //cout << ("richgeo" + t + "_ellipse_boa_mean") << fHM->H1("fhBoverA"+t)->GetMean() << endl;
      gPad->SetLogy(true);

      TCanvas *cEllipseXcYc = CreateCanvas(("richgeo" + t +"_ellipse_xc_yc").c_str(), ("richgeo" + t + "_ellipse_xc_yc").c_str(), 800, 800);
      DrawH2(fHM->H2("fhXcYcEllipse"+t));

      DrawH2MeanRms((TH2D*)fHM->H2("fhChi2EllipseVsMom"+t),  "richgeo" + t + "_chi2_ellipse_vs_mom");
      DrawH2MeanRms((TH2D*)fHM->H2("fhAaxisVsMom"+t), "richgeo" + t + "_a_vs_mom");
      DrawH2MeanRms((TH2D*)fHM->H2("fhBaxisVsMom"+t), "richgeo" + t + "_b_vs_mom");

      TCanvas *cCircle = CreateCanvas(("richgeo" + t + "_circle").c_str(), ("richgeo" + t + "_circle").c_str(), 800, 400);
      cCircle->Divide(2,1);
      cCircle->cd(1);
      DrawH1(fHM->H1("fhNofHits"+t));
      cout << "Number of hits/points = "  << fHM->H1("fhNofHits"+t)->GetMean() << endl;
      gPad->SetLogy(true);
      cCircle->cd(2);
      DrawH2(fHM->H2("fhXcYcCircle"+t));

      DrawH2MeanRms((TH2D*)fHM->H2("fhChi2CircleVsMom"+t), "richgeo" + t + "_chi2_circle_vs_mom");
      DrawH2MeanRms((TH2D*)fHM->H2("fhRadiusVsMom"+t), "richgeo" + t + "_r_vs_mom");
      DrawH2MeanRms((TH2D*)fHM->H2("fhDRVsMom"+t), "richgeo" + t + "_dr_vs_mom");
   }

   TCanvas* cNofPhotons = CreateCanvas("richgeo_nof_photons_per_hit", "richgeo_nof_photons_per_hit", 800, 800);
   fHM->H1("fhNofPhotonsPerHit")->Scale(1./fHM->H1("fhNofPhotonsPerHit")->Integral());
   DrawH1(fHM->H1("fhNofPhotonsPerHit"));

   TCanvas *cDiffEllipse = CreateCanvas("richgeo_diff_ellipse", "richgeo_diff_ellipse", 1200, 600);
   cDiffEllipse->Divide(4,2);
   cDiffEllipse->cd(1);
   DrawH2(fHM->H2("fhDiffAaxis"));
   cDiffEllipse->cd(2);
   DrawH2(fHM->H2("fhDiffBaxis"));
   cDiffEllipse->cd(3);
   DrawH2(fHM->H2("fhDiffXcEllipse"));
   cDiffEllipse->cd(4);
   DrawH2(fHM->H2("fhDiffYcEllipse"));
   cDiffEllipse->cd(5);
   DrawH1(fHM->H2("fhDiffAaxis")->ProjectionY());
   gPad->SetLogy(true);
   cDiffEllipse->cd(6);
   DrawH1(fHM->H2("fhDiffBaxis")->ProjectionY());
   gPad->SetLogy(true);
   cDiffEllipse->cd(7);
   DrawH1(fHM->H2("fhDiffXcEllipse")->ProjectionY());
   gPad->SetLogy(true);
   cDiffEllipse->cd(8);
   DrawH1(fHM->H2("fhDiffYcEllipse")->ProjectionY());
   gPad->SetLogy(true);

   TCanvas *cDiffCircle = CreateCanvas("richgeo_diff_circle", "richgeo_diff_circle", 900, 600);
   cDiffCircle->Divide(3,2);
   cDiffCircle->cd(1);
   DrawH2(fHM->H2("fhDiffXcCircle"));
   cDiffCircle->cd(2);
   DrawH2(fHM->H2("fhDiffYcCircle"));
   cDiffCircle->cd(3);
   DrawH2(fHM->H2("fhDiffRadius"));
   cDiffCircle->cd(4);
   DrawH1(fHM->H2("fhDiffXcCircle")->ProjectionY());
   gPad->SetLogy(true);
   cDiffCircle->cd(5);
   DrawH1(fHM->H2("fhDiffYcCircle")->ProjectionY());
   gPad->SetLogy(true);
   cDiffCircle->cd(6);
   DrawH1(fHM->H2("fhDiffRadius")->ProjectionY());
   gPad->SetLogy(true);

   TCanvas *cHits = CreateCanvas("richgeo_hits", "richgeo_hits", 1200, 600);
   cHits->Divide(2,1);
   cHits->cd(1);
   DrawH1(fHM->H1("fhDiffXhit"));
   cHits->cd(2);
   DrawH1(fHM->H1("fhDiffYhit"));

   TCanvas *cFitEff = CreateCanvas("richgeo_fit_eff", "richgeo_fit_eff", 1200, 400);
   cFitEff->Divide(3,1);
   cFitEff->cd(1);
   DrawH1( list_of((TH1D*)fHM->H1("fhNofHitsAll")->Clone())((TH1D*)fHM->H1("fhNofHitsCircleFit")->Clone())((TH1D*)fHM->H1("fhNofHitsEllipseFit")->Clone()),
         list_of("All")("Circle fit")("Ellipse fit"), kLinear, kLog, true, 0.7, 0.7, 0.99, 0.99);
   TH1D* fhNofHitsCircleFitEff = Cbm::DivideH1(fHM->H1("fhNofHitsCircleFit"), fHM->H1("fhNofHitsAll"));
   TH1D* fhNofHitsEllipseFitEff = Cbm::DivideH1(fHM->H1("fhNofHitsEllipseFit"), fHM->H1("fhNofHitsAll"));
   cFitEff->cd(2);
   DrawH1(fhNofHitsCircleFitEff);
   TLatex* circleFitEffTxt = new TLatex(15, 0.5, CalcEfficiency(fHM->H1("fhNofHitsCircleFit"), fHM->H1("fhNofHitsAll")).c_str());
   circleFitEffTxt->Draw();
   cFitEff->cd(3);
   DrawH1(fhNofHitsEllipseFitEff);
   TLatex* ellipseFitEffTxt = new TLatex(15, 0.5, CalcEfficiency(fHM->H1("fhNofHitsEllipseFit"), fHM->H1("fhNofHitsAll")).c_str());
   ellipseFitEffTxt->Draw();

   TCanvas *cAccEff = CreateCanvas("richgeo_acc_eff_el", "richgeo_acc_eff_el", 1200, 800);
   cAccEff->Divide(3,2);
   cAccEff->cd(1);
   DrawH1(list_of((TH1D*)fHM->H1("fhMcMomEl"))((TH1D*)fHM->H1("fhAccMomEl")), list_of("MC")("ACC"), kLinear, kLog, true, 0.8, 0.8, 0.99, 0.99);
   cAccEff->cd(2);
   DrawH2(fHM->H2("fhMcPtyEl"));
   cAccEff->cd(3);
   DrawH2(fHM->H2("fhAccPtyEl"));


   TH1D* pxEff = Cbm::DivideH1((TH1D*)fHM->H1("fhAccMomEl")->Clone(), (TH1D*)fHM->H1("fhMcMomEl")->Clone(), "", 100., "Geometrical acceptance [%]");
   TH2D* pyzEff = Cbm::DivideH2((TH2D*)fHM->H1("fhAccPtyEl")->Clone(), (TH2D*)fHM->H1("fhMcPtyEl")->Clone(), "", 100., "Geometrical acceptance [%]");
   //cAccEff->cd(4);
   TCanvas *cAccEff_mom = CreateCanvas("richgeo_acc_eff_el_mom", "richgeo_acc_eff_el_mom", 800, 800);
   DrawH1(pxEff);
   //TLatex* pxEffTxt = new TLatex(3, 0.5, CalcEfficiency(fh_acc_mom_el, fh_mc_mom_el).c_str());
   //pxEffTxt->Draw();
   TCanvas *cAccEff_pty = CreateCanvas("richgeo_acc_eff_el_pty", "richgeo_acc_eff_el_pty", 800, 800);
   //cAccEff->cd(5);
   DrawH2(pyzEff);

   TCanvas *cAccEffPi = CreateCanvas("richgeo_acc_eff_pi", "richgeo_acc_eff_pi", 1200, 800);
   cAccEffPi->Divide(3,2);
   cAccEffPi->cd(1);
   DrawH1(list_of((TH1D*)fHM->H1("fhMcMomPi"))((TH1D*)fHM->H1("fhAccMomPi")), list_of("MC")("ACC"), kLinear, kLog, true, 0.8, 0.8, 0.99, 0.99);
   cAccEffPi->cd(2);
   DrawH2(fHM->H2("fhMcPtyPi"));
   cAccEffPi->cd(3);
   DrawH2(fHM->H2("fhAccPtyPi"));
   TH1D* pxPiEff = Cbm::DivideH1((TH1D*)(TH1D*)fHM->H1("fhAccMomPi")->Clone(), (TH1D*)(TH1D*)fHM->H1("fhMcMomPi")->Clone(), "", 100., "Geometrical acceptance [%]");
   TH2D* pyzPiEff = Cbm::DivideH2((TH2D*)(TH1D*)fHM->H1("fhAccPtyPi")->Clone(), (TH2D*)(TH1D*)fHM->H1("fhMcPtyPi")->Clone(), "", 100., "Geometrical acceptance [%]");
  // cAccEffPi->cd(4);
   TCanvas *cAccEffPi_mom = CreateCanvas("richgeo_acc_eff_pi_mom", "richgeo_acc_eff_pi_mom", 800, 800);
   DrawH1(pxPiEff);
  // TLatex* pxEffTxt = new TLatex(3, 0.5, CalcEfficiency(fh_acc_mom_pi, fh_mc_mom_pi).c_str());
 //  pxEffTxt->Draw();
   //cAccEffPi->cd(5);
   TCanvas *cAccEffPi_pty = CreateCanvas("richgeo_acc_eff_pi_pty", "richgeo_acc_eff_pi_pty", 800, 800);
   DrawH2(pyzPiEff);
   pyzPiEff->GetZaxis()->SetRangeUser(0, 100);

   string effPi = CalcEfficiency((TH1D*)fHM->H1("fhAccMomPi")->Clone(), (TH1D*)fHM->H1("fhMcMomPi")->Clone());
   string effEl = CalcEfficiency((TH1D*)fHM->H1("fhAccMomEl")->Clone(), (TH1D*)fHM->H1("fhMcMomEl")->Clone());
   TCanvas *cAccEffElPi_mom = CreateCanvas("richgeo_acc_eff_el_pi_mom", "richgeo_acc_eff_el_pi_mom", 800, 800);
   DrawH1(list_of(pxEff)(pxPiEff), list_of("e^{#pm} ("+effEl+"%)")("#pi^{#pm} ("+effPi+"%)"), kLinear, kLinear, true, 0.7, 0.5, 0.87, 0.7);


   TCanvas *cAccEffZoom = CreateCanvas("richgeo_acc_eff_el_zoom", "richgeo_acc_eff_el_zoom", 1000, 500);
   cAccEffZoom->Divide(2,1);
   cAccEffZoom->cd(1);
   TH1D* fhMcMomElClone = (TH1D*)fHM->H1("fhMcMomEl")->Clone();
   TH1D* fhAccMomElClone = (TH1D*)fHM->H1("fhAccMomEl")->Clone();
   fhMcMomElClone->GetXaxis()->SetRangeUser(0., 3.);
   fhAccMomElClone->GetXaxis()->SetRangeUser(0., 3.);
   fhMcMomElClone->SetMinimum(0.);
   DrawH1(list_of(fhMcMomElClone)(fhAccMomElClone), list_of("MC")("ACC"), kLinear, kLog, true, 0.8, 0.8, 0.99, 0.99);
   gPad->SetLogy(false);
   cAccEffZoom->cd(2);
   TH1D* px_eff_clone = (TH1D*) pxEff->Clone();
   px_eff_clone->GetXaxis()->SetRangeUser(0., 3.);
   px_eff_clone->SetMinimum(0.);
   DrawH1(px_eff_clone);

   // Draw number vs position onto the photodetector plane
   DrawH3(fHM->H3("fhNofHitsXYZ"), "richgeo_numbers_vs_xy_hits", "Number of hits", 10, 30);
   DrawH3(fHM->H3("fhNofPointsXYZ"), "richgeo_numbers_vs_xy_points", "Number of points", 100., 300.);
   DrawH3(fHM->H3("fhBoverAXYZ"), "richgeo_numbers_vs_xy_boa", "B/A", 0.75, 1.0);
   DrawH3(fHM->H3("fhBaxisXYZ"), "richgeo_numbers_vs_xy_b", "B [cm]", 4., 5.);
   DrawH3(fHM->H3("fhAaxisXYZ"), "richgeo_numbers_vs_xy_a", "A [cm]", 4.4, 5.7);
   DrawH3(fHM->H3("fhRadiusXYZ"), "richgeo_numbers_vs_xy_r", "Radius [cm]", 4.2, 5.2);
   DrawH3(fHM->H3("fhdRXYZ"), "richgeo_numbers_vs_xy_dr", "dR [cm]", -.1, .1);

   TCanvas *cAccVsMinNofHits = CreateCanvas("richgeo_acc_vs_min_nof_hits", "richgeo_acc_vs_min_nof_hits", 600, 600);
   TH1D* h = CreateAccVsMinNofHitsHist();
   h->GetXaxis()->SetRangeUser(0., 40.0);
   DrawH1(h);

   DrawH2MeanRms(fHM->H2("fhRadiusVsNofHits"), "richgeo_hits_r_vs_nof_hits");
   DrawH2MeanRms(fHM->H2("fhAaxisVsNofHits"), "richgeo_hits_a_vs_nof_hits");
   DrawH2MeanRms(fHM->H2("fhBaxisVsNofHits"), "richgeo_hits_b_vs_nof_hits");

   TCanvas* cHitsRAB = CreateCanvas("richgeo_hits_rab", "richgeo_hits_rab", 1500, 600);
   cHitsRAB->Divide(3, 1);
   cHitsRAB->cd(1);
   DrawH1andFit(fHM->H2("fhRadiusVsNofHits")->ProjectionY( (string(fHM->H2("fhRadiusVsNofHits")->GetName())+ "_py" ).c_str() ));
   cHitsRAB->cd(2);
   DrawH1andFit(fHM->H2("fhAaxisVsNofHits")->ProjectionY( (string(fHM->H2("fhAaxisVsNofHits")->GetName())+ "_py" ).c_str() ));
   cHitsRAB->cd(3);
   DrawH1andFit(fHM->H2("fhBaxisVsNofHits")->ProjectionY( (string(fHM->H2("fhBaxisVsNofHits")->GetName())+ "_py" ).c_str() ));
}

void CbmRichGeoTest::DrawH3(
      TH3* h,
      const string& cName,
      const string& zAxisTitle,
      double zMin,
      double zMax,
      bool doFit)
{
   int nBinsX = h->GetNbinsX();
   int nBinsY = h->GetNbinsY();
   TH2D* h2Mean = (TH2D*)h->Project3D("yx")->Clone();
   TH2D* h2Rms = (TH2D*)h->Project3D("yx")->Clone();

   TCanvas *canvas;
   if (doFit) canvas = new TCanvas ("fit_func", "fit_func");

   for (int x = 1; x <= nBinsX; x++){
      for (int y = 1; y <= nBinsY; y++){
         stringstream ss;
         ss << h->GetName() << "_z_" << x << "_" << y;
         TH1D* hz = h->ProjectionZ(ss.str().c_str(), x, x, y, y);
         double m = 0.;
         double s = 0.;
         if (doFit) {
            hz->Fit("gaus", "QO");
            TF1* func = hz->GetFunction("gaus");
            if (func != NULL) {
               m = func->GetParameter(1);
               s = func->GetParameter(2);
            }
         } else {
            m = hz->GetMean();
            s = hz->GetRMS();
         }
         h2Mean->SetBinContent(x, y, m);
         h2Rms->SetBinContent(x, y, s);
      }
   }

   TCanvas *meanRmsCan = CreateCanvas(string(cName+"_mean_rms").c_str(), string(cName+"_mean_rms").c_str(), 1200, 600);
   meanRmsCan->Divide(2, 1);
   meanRmsCan->cd(1);
   h2Mean->GetZaxis()->SetTitle(string("Mean."+zAxisTitle).c_str());
   h2Mean->GetZaxis()->SetRangeUser(zMin, zMax);
   DrawH2(h2Mean);
   meanRmsCan->cd(2);
   h2Rms->GetZaxis()->SetTitle(string("RMS."+zAxisTitle).c_str());
   DrawH2(h2Rms);


 /*  TCanvas* cHitsRAB = CreateCanvas(string(cName+"_local_xy").c_str(), string(cName+"_local_xy").c_str(), 800, 800);
   int x = h->GetXaxis()->FindBin(20);
   int y = h->GetYaxis()->FindBin(120);
   TH1D* hz = h->ProjectionZ((string(h->GetName())+"_local_xy").c_str(), x, x, y, y);
   //if (hz->GetEntries() > 2) {
      DrawH1andFit(hz);
   //}
*/
   if (canvas != NULL) delete canvas;

}

void CbmRichGeoTest::CreateStudyReport(
      const string& title,
      const vector<string>& fileNames,
      const vector<string>& studyNames,
      const string& outputDir)
{
   if (outputDir != "") gSystem->mkdir(outputDir.c_str(), true);

   CbmStudyReport* report = new CbmRichGeoTestStudyReport();
   fTitle = title;
   cout << "Report can be found here: " << outputDir << endl;
   report->Create(fileNames, studyNames, outputDir);
   delete report;
}

void CbmRichGeoTest::Finish()
{
   DrawHist();
   //fHM->WriteToFile();
   SaveCanvasToImage();
}

string CbmRichGeoTest::CalcEfficiency(
   TH1* histRec,
   TH1* histAcc)
{
   if (histAcc->GetEntries() == 0) { return "0"; }
   else {
      Double_t eff = 100. * Double_t(histRec->GetEntries()) / Double_t(histAcc->GetEntries());
      cout << "Efficiency:" << eff << endl;
      return Cbm::NumberToString(eff, 2);

    /*  stringstream ss;

      ss.precision(4);
      ss << eff;
      return ss.str();*/
   }
}

TCanvas* CbmRichGeoTest::CreateCanvas(
      const string& name,
      const string& title,
      int width,
      int height)
{
   TCanvas* c = new TCanvas(name.c_str(), title.c_str(), width, height);
   fCanvas.push_back(c);
   return c;
}

void CbmRichGeoTest::SaveCanvasToImage()
{
   for (int i = 0; i < fCanvas.size(); i++)
   {
      Cbm::SaveCanvasAsImage(fCanvas[i], fOutputDir, "png");
   }
}

ClassImp(CbmRichGeoTest)

