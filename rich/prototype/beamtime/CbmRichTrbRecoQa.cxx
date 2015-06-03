#include "CbmRichTrbRecoQa.h"
#include "FairRootManager.h"
#include "FairLogger.h"

#include "TClonesArray.h"

#include "TROOT.h"
#include "TFolder.h"
#include "TSystem.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TFile.h"
#include "CbmDrawHist.h"
#include "CbmRichHit.h"
#include "CbmRichHitInfo.h"
#include "TEllipse.h"
#include "TLatex.h"
#include "CbmRichRingLight.h"
#include "CbmRichRingFitterCOP.h"
#include "CbmRichRingFitterEllipseTau.h"
#include "CbmRichConverter.h"
#include "CbmUtils.h"
#include "TStyle.h"


#include <iostream>
#include <vector>

CbmRichTrbRecoQa::CbmRichTrbRecoQa()
  : FairTask(),
    fRichHits(NULL),
    fRichRings(NULL),
    fRichHitInfos(NULL),
    fHM(NULL),
    fEventNum(0),
    fNofDrawnEvents(0),
    fMaxNofEventsToDraw(0),
    fOutputDir(""),
    fRunTitle(""),
    fDrawHist(kFALSE),
    fCopFit(NULL),
    fTauFit(NULL)
{
}

CbmRichTrbRecoQa::~CbmRichTrbRecoQa()
{
}

InitStatus CbmRichTrbRecoQa::Init()
{
	FairRootManager* manager = FairRootManager::Instance();
	fRichHits = (TClonesArray*)manager->GetObject("RichHit");
	if (NULL == fRichHits) { Fatal("CbmRichTrbRawQa::Init","No RichHit array!"); }

	fRichRings = (TClonesArray*)manager->GetObject("RichRing");
	if (NULL == fRichRings) { Fatal("CbmRichTrbRawQa::Init","No RichRing array!"); }

	fRichHitInfos = (TClonesArray*)manager->GetObject("RichHitInfo");
	if (NULL == fRichHitInfos) { Fatal("CbmRichTrbRawQa::Init","No RichHitInfo array!"); }

	fCopFit = new CbmRichRingFitterCOP();
	fTauFit = new CbmRichRingFitterEllipseTau();
	CbmRichConverter::Init();

	InitHist();

	return kSUCCESS;
}

void CbmRichTrbRecoQa::InitHist()
{
	fHM = new CbmHistManager();

	fHM->Create1<TH1D>("fhNofHitsInEvent", "fhNofHitsInEvent;Number of hits in event;Yield", 40, 0.5, 40.5);
	fHM->Create1<TH1D>("fhNofHitsInRing", "fhNofHitsInRing;Number of hits in ring;Yield", 40, -0.5, 39.5);
	fHM->Create2<TH2D>("fhHitsXYPixel", "fhHitsXYPixel;X [pixel];Y [pixel];Hits per pixel", 32, 0.5, 32.5, 32, 0.5, 32.5);
	fHM->Create1<TH1D>("fhNofRingsInEvent", "fhNofRingsInEvent;# rings per event;Yield", 5, -0.5, 4.5);
	fHM->Create1<TH1D>("fhBoverAEllipse", "fhBoverAEllipse;B/A;Yield", 200, 0.0, 1.0);
	fHM->Create2<TH2D>("fhXcYcEllipse", "fhXcYcEllipse;X [cm];Y [cm];Yield", 200, 0., 21., 100, 0., 21.);
	fHM->Create1<TH1D>("fhBaxisEllipse", "fhBaxisEllipse;B axis [cm];Yield", 200, 3.5, 5.5);
	fHM->Create1<TH1D>("fhAaxisEllipse", "fhAaxisEllipse;A axis [cm];Yield", 200, 3.5, 5.5);
	fHM->Create1<TH1D>("fhChi2Ellipse", "fhChi2Ellipse;#Chi^{2};Yield", 200, 0.0, 0.5);
	fHM->Create1<TH1D>("fhPhiEllipse", "fhPhiEllipse;Ellipse rotation angle [rad];Yield", 100, -1.7, 1.7);
	fHM->Create2<TH2D>("fhXcYcCircle", "fhXcYcCircle;X [cm];Y [cm];Yield", 200, 0., 21., 100, 0., 21.);
	fHM->Create1<TH1D>("fhRadiusCircle", "fhRadiusCircle;Radius [cm];Yield", 200, 4.0, 5.0);
	fHM->Create1<TH1D>("fhChi2Circle", "fhChi2Circle;#Chi^{2};Yield", 200, 0.0, 0.5);
	fHM->Create1<TH1D>("fhDrCircle", "fhDrCircle;dR [cm];Yield", 200, -1.0, 1.0);
	fHM->Create2<TH2D>("fhHitsXYPmt", "fhHitsXYPmt;PMT X;PMT Y;Hits in event per PMT", 4, 0.5, 32.5, 4, 0.5, 32.5);
	fHM->Create1<TH1D>("fhNofHitsInEventAll", "fhNofHitsInEventAll;Number of hits in event;Yield", 40, 0.5, 40.5);
	fHM->Create1<TH1D>("fhNofHitsInEventWithRing", "fhNofHitsInEventWithRing;Number of hits in event;Yield", 40, 0.5, 40.5);

	for (int i = 0; i < 16; i++) {
		TString name, title;
		// number of hits in reconstructed ring per PMT
		name.Form("fhNofHitsInRingPmt_%i", i);
		title.Form("fhNofHitsInRingPmt_%i;Number of hits in ring in PMT%i;Yield", i, i);
		fHM->Create1<TH1D>(name.Data(), title.Data(), 15, -0.5, 14.5);

		// number of hits in event per PMT
		name.Form("fhNofHitsInEventPmt_%i", i);
		title.Form("fhNofHitsInEventPmt_%i;Number of hits in event in PMT%i;Yield", i, i);
		fHM->Create1<TH1D>(name.Data(), title.Data(), 15, -0.5, 14.5);
	}
}

void CbmRichTrbRecoQa::Exec(
		Option_t* option)
{
	fEventNum++;
	LOG(DEBUG2) << "CbmRichTrbRecoQa : Event #" << fEventNum << FairLogger::endl;

	Int_t nofHitsInEvent = fRichHits->GetEntries();
	fHM->H1("fhNofHitsInEvent")->Fill(nofHitsInEvent);

	Int_t nofHitInfosInEvent = fRichHitInfos->GetEntries();
	for (Int_t iH = 0; iH < nofHitInfosInEvent; iH++) {
		CbmRichHitInfo* hit = static_cast<CbmRichHitInfo*>(fRichHitInfos->At(iH));
		fHM->H2("fhHitsXYPixel")->Fill(hit->GetXPixel(), hit->GetYPixel());
		fHM->H2("fhHitsXYPmt")->Fill(hit->GetXPixel(), hit->GetYPixel());
	}

	Int_t nofRingsInEvent = fRichRings->GetEntries();
	fHM->H1("fhNofRingsInEvent")->Fill(nofRingsInEvent);
	vector<CbmRichRingLight> fitCircleRing;
	vector<CbmRichRingLight> fitEllipseRing;
	for (Int_t iR = 0; iR < nofRingsInEvent; iR++) {
		CbmRichRing* ring = static_cast<CbmRichRing*>(fRichRings->At(iR));

		CbmRichRingLight ringL;
		CbmRichConverter::CopyHitsToRingLight(ring, &ringL);
		fTauFit->DoFit(&ringL);
		FillHistEllipse(&ringL);
		fitEllipseRing.push_back(ringL);
		fCopFit->DoFit(&ringL);
		FillHistCircle(&ringL);
		fitCircleRing.push_back(ringL);
	}

	FillNofHitsPmt();

	fHM->H1("fhNofHitsInEventAll")->Fill(nofHitsInEvent);
	if (nofRingsInEvent >= 1) fHM->H1("fhNofHitsInEventWithRing")->Fill(nofHitsInEvent);


	if (nofHitsInEvent >= 5 && fNofDrawnEvents < fMaxNofEventsToDraw ) {
		DrawEvent(fitCircleRing, fitEllipseRing);
	}


}

void CbmRichTrbRecoQa::FillHistEllipse(
      CbmRichRingLight* ring)
{
   Double_t axisA = ring->GetAaxis();
   Double_t axisB = ring->GetBaxis();
   Double_t xcEllipse = ring->GetCenterX();
   Double_t ycEllipse = ring->GetCenterY();
   Int_t nofHitsRing = ring->GetNofHits();
   Double_t phi = ring->GetPhi();

   fHM->H1("fhBoverAEllipse")->Fill(axisB/axisA);
   fHM->H2("fhXcYcEllipse")->Fill(xcEllipse, ycEllipse);
   fHM->H1("fhNofHitsInRing")->Fill(nofHitsRing);
   fHM->H1("fhBaxisEllipse")->Fill(axisB);
   fHM->H1("fhAaxisEllipse")->Fill(axisA);
   fHM->H1("fhChi2Ellipse")->Fill(ring->GetChi2()/nofHitsRing);
   fHM->H1("fhPhiEllipse")->Fill(phi);
}

void CbmRichTrbRecoQa::FillHistCircle(
      CbmRichRingLight* ring)
{
   Double_t radius = ring->GetRadius();
   Double_t xcCircle = ring->GetCenterX();
   Double_t ycCircle = ring->GetCenterY();
   Int_t nofHitsRing = ring->GetNofHits();
   fHM->H2("fhXcYcCircle")->Fill(xcCircle, ycCircle);
   fHM->H1("fhRadiusCircle")->Fill(radius);
   fHM->H1("fhChi2Circle")->Fill(ring->GetChi2()/ring->GetNofHits());

   for (Int_t iH = 0; iH < ring->GetNofHits(); iH++){
      Double_t xh = ring->GetHit(iH).fX;
      Double_t yh = ring->GetHit(iH).fY;
      Double_t dr = radius - sqrt((xcCircle - xh)*(xcCircle - xh) + (ycCircle - yh)*(ycCircle - yh));
      fHM->H1("fhDrCircle")->Fill(dr);
   }
}

void CbmRichTrbRecoQa::FillNofHitsPmt()
{
    //calculate number of hits in ring per PMT
	Int_t nofRingsInEvent = fRichRings->GetEntries();
	for (Int_t iR = 0; iR < nofRingsInEvent; iR++) {
		CbmRichRing* ring = static_cast<CbmRichRing*>(fRichRings->At(iR));
		Int_t nofHitsInRingPmt[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		for (Int_t iH = 0; iH < ring->GetNofHits(); iH++){
			Int_t hitInd = ring->GetHit(iH);
			CbmRichHit* hit = static_cast<CbmRichHit*>(fRichHits->At(hitInd));
			Int_t pmtId = hit->GetPmtId() - 1;
			if (pmtId < 0 || pmtId >= 16) {
				LOG(ERROR) << "CbmRichTrbRecoQa::FillNofHitsPmt rings pmtId < 0 || pmtId > 16, pmtId:"<< pmtId << FairLogger::endl;
				continue;
			}
			nofHitsInRingPmt[pmtId]++;
		}//iH

		for (int i = 0; i < 16; i++) {
			TString name;
			name.Form("fhNofHitsInRingPmt_%i", i);
			fHM->H1(name.Data())->Fill(nofHitsInRingPmt[i]);
		}
	}//iR

	// number of hits in event perPMT
	Int_t nofHitsInEventPmt[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	Int_t nofHits = fRichHits->GetEntriesFast();
	for (Int_t iH = 0; iH < nofHits; iH++) {
		CbmRichHit* hit = static_cast<CbmRichHit*>(fRichHits->At(iH));
		Int_t pmtId = hit->GetPmtId() - 1;
		if (pmtId < 0 || pmtId >= 16) {
			LOG(ERROR) << "CbmRichTrbRecoQa::FillNofHitsPmt events pmtId < 0 || pmtId > 16, pmtId:"<< pmtId << FairLogger::endl;
			continue;
		}
		nofHitsInEventPmt[pmtId]++;
	}

	for (int i = 0; i < 16; i++) {
		TString name;
		name.Form("fhNofHitsInEventPmt_%i", i);
		fHM->H1(name.Data())->Fill(nofHitsInEventPmt[i]);
	}
}

void CbmRichTrbRecoQa::DrawHist()
{
	{
		TCanvas* c = new TCanvas(fRunTitle + "_rich_trb_recoqa_nof_hits_in_event", fRunTitle + "_rich_trb_recoqa_nof_hits_in_event", 600, 600);
		DrawH1(fHM->H1("fhNofHitsInEvent"));
		FitGaussAndDrawResults(fHM->H1("fhNofHitsInEvent"));
		Cbm::SaveCanvasAsImage(c, string(fOutputDir.Data()), "png");
	}

	{
		TCanvas* c = new TCanvas(fRunTitle + "_rich_trb_recoqa_nof_rings_per_event", fRunTitle + "_rich_trb_recoqa_nof_rings_per_event", 600, 600);
		DrawH1(fHM->H1("fhNofRingsInEvent"));
		Cbm::SaveCanvasAsImage(c, string(fOutputDir.Data()), "png");
	}

	{
		TCanvas* c = new TCanvas(fRunTitle + "_rich_trb_recoqa_nof_hits_per_ring", fRunTitle + "_rich_trb_recoqa_nof_hits_per_ring", 600, 600);
		DrawH1(fHM->H1("fhNofHitsInRing"));
		FitGaussAndDrawResults(fHM->H1("fhNofHitsInRing"));
		Cbm::SaveCanvasAsImage(c, string(fOutputDir.Data()), "png");
	}

	{
		TCanvas* c = new TCanvas(fRunTitle + "_rich_trb_recoqa_ellipse_param", fRunTitle + "_rich_trb_recoqa_ellipse_param", 1200, 800);
		c->Divide(3, 2);
		c->cd(1);
		DrawH1(fHM->H1("fhBaxisEllipse"), kLinear, kLog);
		FitGaussAndDrawResults(fHM->H1("fhBaxisEllipse"));
		c->cd(2);
		DrawH1(fHM->H1("fhAaxisEllipse"), kLinear, kLog);
		FitGaussAndDrawResults(fHM->H1("fhAaxisEllipse"));
		c->cd(3);
		DrawH1(fHM->H1("fhBoverAEllipse"));
		c->cd(4);
		DrawH1(fHM->H1("fhChi2Ellipse"));
		c->cd(5);
		DrawH2(fHM->H2("fhXcYcEllipse"));
		c->cd(6);
		DrawH1(fHM->H1("fhPhiEllipse"));
		Cbm::SaveCanvasAsImage(c, string(fOutputDir.Data()), "png");
	}

	{
		TCanvas* c = new TCanvas(fRunTitle + "_rich_trb_recoqa_circle_param", fRunTitle + "_rich_trb_recoqa_circle_param", 800, 800);
		c->Divide(2, 2);
		c->cd(1);
		DrawH2(fHM->H2("fhXcYcCircle"));
		c->cd(2);
		DrawH1(fHM->H1("fhRadiusCircle"), kLinear, kLog);
		FitGaussAndDrawResults(fHM->H1("fhRadiusCircle"));
		c->cd(3);
		DrawH1(fHM->H1("fhChi2Circle"));
		c->cd(4);
		DrawH1(fHM->H1("fhDrCircle"));
		FitGaussAndDrawResults(fHM->H1("fhDrCircle"));
		Cbm::SaveCanvasAsImage(c, string(fOutputDir.Data()), "png");
	}

	{
		TCanvas* c = new TCanvas(fRunTitle + "_rich_trb_recoqa_hits_xy_pixel", fRunTitle + "_rich_trb_recoqa_hits_xy_pixel", 800, 600);
		DrawH2(fHM->H2("fhHitsXYPixel"));
		Cbm::SaveCanvasAsImage(c, string(fOutputDir.Data()), "png");
	}

	{
		TCanvas* c = new TCanvas(fRunTitle + "_rich_trb_recoqa_hits_xy_pmt", fRunTitle + "_rich_trb_recoqa_hits_xy_pmt", 800, 600);
		TH2D* h = (TH2D*)fHM->H2("fhHitsXYPmt")->Clone();
		gStyle->SetPaintTextFormat("4.1f");
		h->Scale(100. / h->GetEntries());
		h->SetMarkerSize(3);
		h->GetZaxis()->SetTitle("Hits in event per PMT [%]");
		DrawH2(h, kLinear, kLinear, kLinear, "text COLZ");
		Cbm::SaveCanvasAsImage(c, string(fOutputDir.Data()), "png");
	}

	{
		TCanvas* c = new TCanvas(fRunTitle + "_rich_trb_recoqa_efficiency", fRunTitle + "_rich_trb_recoqa_efficiency", 800, 600);
		TH1D* eff = Cbm::DivideH1(fHM->H1("fhNofHitsInEventWithRing"), fHM->H1("fhNofHitsInEventAll"));
		DrawH1(eff);
		Cbm::SaveCanvasAsImage(c, string(fOutputDir.Data()), "png");
	}

	{ //dashboard histogram
		TCanvas* c = new TCanvas(fRunTitle + "_rich_trb_recoqa_dashboard", fRunTitle + "_rich_trb_recoqa_dashboard", 1200, 600);
		c->Divide(3, 2);
		c->cd(1);
		DrawH1(fHM->H1("fhNofHitsInEvent"));
		c->cd(2);
		DrawH1(fHM->H1("fhNofHitsInRing"));
		c->cd(3);
		DrawH1(fHM->H1("fhRadiusCircle"), kLinear, kLog);
		c->cd(4);
		DrawH2(fHM->H2("fhHitsXYPixel"));
		c->cd(5);
		TH2D* h = (TH2D*)fHM->H2("fhHitsXYPmt")->Clone();
	    gStyle->SetPaintTextFormat("4.1f");
		h->Scale(100. / h->GetEntries());
		h->SetMarkerSize(3);
		h->GetZaxis()->SetTitle("Hits in event per PMT [%]");
		DrawH2(h, kLinear, kLinear, kLinear, "text COLZ");
		c->cd(6);
		TString str;
		TLatex* latex = new TLatex();
		latex->DrawLatex(0.01, 0.9, "Run title: "+fRunTitle);
		str.Form("Total number of events %d", (Int_t)fHM->H1("fhNofHitsInEvent")->GetEntries());
		latex->DrawLatex(0.01, 0.8, str.Data());
		TH1* hREv = fHM->H1("fhNofRingsInEvent");
		Int_t nofEvWithRing = hREv->GetEntries() - hREv->GetBinContent(1);
		str.Form("Total number of events with ring %d (%.1f%%)", nofEvWithRing, 100. * nofEvWithRing / (Double_t)fHM->H1("fhNofHitsInEvent")->GetEntries());
		latex->DrawLatex(0.01, 0.7, str.Data());
		str.Form("Total number of rings %i", (Int_t)fHM->H1("fhNofHitsInRing")->GetEntries());
		latex->DrawLatex(0.01, 0.6, str.Data());
		str.Form("Total number of hits %i", (Int_t)fHM->H2("fhHitsXYPixel")->GetEntries());
		latex->DrawLatex(0.01, 0.5, str.Data());
		Cbm::SaveCanvasAsImage(c, string(fOutputDir.Data()), "png");
	}

	{
		TCanvas* c = new TCanvas(fRunTitle + "_rich_trb_nofhits_in_ring_pmt_all", fRunTitle + "_rich_trb_nofhits_in_ring_pmt_all", 1200, 1200);
		c->Divide(4, 4);
		int count = 0;
		for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 4; i++) {
			c->cd(4 - j + 4 * i);
			TString name;
			name.Form("fhNofHitsInRingPmt_%i", count);
			fHM->H1(name.Data());
			DrawH1(fHM->H1(name.Data()), kLinear, kLinear);
	        FitGaussAndDrawResults(fHM->H1(name.Data()));
	        count++;
		}
		}
		Cbm::SaveCanvasAsImage(c, string(fOutputDir.Data()), "png");
	}
}


void CbmRichTrbRecoQa::FitGaussAndDrawResults(
		TH1* h)
{
	h->Fit("gaus", "Q");
	//h->Fit("gaus");
	TF1* fit = h->GetFunction("gaus");

	Double_t sigma = (NULL != fit) ? fit->GetParameter(2) : 0.;
	Double_t mean = (NULL != fit) ? fit->GetParameter(1) : 0.;
	TString str;
	str.Form("m=%.2f  #sigma=%.2f", mean, sigma);

	h->SetTitle(str.Data());
	fit->SetLineColor(kBlack);
}

void CbmRichTrbRecoQa::DrawHistFromFile(TString fileName)
{
	fHM = new CbmHistManager();
	TFile* file = new TFile(fileName, "READ");
	fHM->ReadFromFile(file);
	DrawHist();
}

void CbmRichTrbRecoQa::DrawEvent(
    		const vector<CbmRichRingLight>& fitRingCircle,
    		const vector<CbmRichRingLight>& fitRingEllipse)
{
   TString ss;
   ss.Form("rich_trb_recoqa_eventdisplay_%d", fNofDrawnEvents);
   fNofDrawnEvents++;
   TCanvas *c = new TCanvas(ss.Data(), ss.Data(), 800, 800);
   c->SetGrid(true, true);
   TH2D* pad = new TH2D("pad", ";X [cm];Y [cm]", 1, 0.0, 21., 1, 0.0, 21.);
   DrawH2(pad);
   //gPad->SetLeftMargin(0.17);
   gPad->SetRightMargin(0.12);
  // gPad->SetBottomMargin(0.15);
   pad->SetStats(false);
   //pad->Draw();

   //Draw event hits
   Int_t nofHitsInEvent = fRichHits->GetEntries();
   for (Int_t iH = 0; iH < nofHitsInEvent; iH++){
	   CbmRichHit* hit = static_cast<CbmRichHit*>(fRichHits->At(iH));
	   TEllipse* hitDr = new TEllipse(hit->GetX(), hit->GetY(), 0.25);
	   hitDr->SetFillColor(kGreen);
	   hitDr->Draw();
   }

   //Draw all found rings and centers
   if (fitRingCircle.size() != fitRingEllipse.size()) {
	   LOG(ERROR) << "CbmRichTrbRecoQa::DrawEvent fitRingCircle.size() != fitRingEllipse.size()" << FairLogger::endl;
   }

   Int_t nofRings = fitRingCircle.size();
   for (Int_t iR = 0; iR < nofRings; iR++) {
	   CbmRichRingLight rC = fitRingCircle[iR];
	   TEllipse* circle = new TEllipse(rC.GetCenterX(), rC.GetCenterY(), rC.GetRadius());
	   circle->SetFillStyle(0);
	   circle->SetLineWidth(3);
	   circle->Draw();
	   TEllipse* cCircle = new TEllipse(rC.GetCenterX(), rC.GetCenterY(), 0.15);
	   cCircle->Draw();

	   CbmRichRingLight rE = fitRingEllipse[iR];
	   Double_t ePhi = rE.GetPhi() * 180 / TMath::Pi();
	   TEllipse* ellipse = new TEllipse(rE.GetCenterX(), rE.GetCenterY(), rE.GetAaxis(), rE.GetBaxis(), 0., 360., ePhi);
	   ellipse->SetFillStyle(0);
	   ellipse->SetLineWidth(3);
	   ellipse->SetLineColor(kBlue);
	   ellipse->Draw();
	   TEllipse* cEllipse = new TEllipse(rE.GetCenterX(), rE.GetCenterY(), 0.15);
	   cEllipse->Draw();

	   // Draw hits from the ring
	   for (int iH = 0; iH < rC.GetNofHits(); iH++){
		   CbmRichHitLight hit = rC.GetHit(iH);
		   TEllipse* hitDr = new TEllipse(hit.fX, hit.fY, 0.15);
		   hitDr->SetFillColor(kRed);
		   hitDr->Draw();
	   }
	   //Draw information
	   ss.Form("(x, y, A, B, #phi, n, B/A)=(%.1f, %.1f, %.2f, %.2f, %.2f, %i, %.2f)",
			   	 rE.GetCenterX(), rE.GetCenterY(), rE.GetAaxis(), rE.GetBaxis(), rE.GetPhi(), rE.GetNofHits(), rE.GetBaxis()/rE.GetAaxis());
	   TLatex* latexEllipse = new TLatex(0.05, 22.5 - 1.0 * iR, ss.Data());
	   latexEllipse->SetTextSize(0.03);
	   latexEllipse->Draw();

	   ss.Form("(x, y, R, n)=(%.1f, %.1f, %.2f, %i)", rC.GetCenterX(), rC.GetCenterY(), rC.GetRadius(), rE.GetNofHits());
	   TLatex* latexCircle = new TLatex(0.05, 21.5 - 1.0 * iR, ss.Data());
	   latexCircle->SetTextSize(0.03);
	   latexCircle->Draw();
   }
}

void CbmRichTrbRecoQa::Finish()
{
	if (fDrawHist) {
		DrawHist();
	}
	fHM->WriteToFile();
}

ClassImp(CbmRichTrbRecoQa)
