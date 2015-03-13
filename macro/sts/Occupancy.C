// example.C
#include <TGClient.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TRandom.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TRootEmbeddedCanvas.h>
#include <RQ_OBJECT.h>
{
 gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libCbmBase");
  gSystem->Load("libCbmData");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libEcal");
  gSystem->Load("libKF");
  gSystem->Load("libMvd");
  gSystem->Load("libSts");
  gSystem->Load("libLittrack");
  gSystem->Load("libRich");
  gSystem->Load("libTrd");
  gSystem->Load("libTof");
  gSystem->Load("libGlobal");
  gSystem->Load("libL1");
  gSystem->Load("libMinuit2"); // Nedded for rich ellipse fitter
}

class MyMainFrame {

  RQ_OBJECT("MyMainFrame")


 private:
  TGMainFrame *fMain;
  TRootEmbeddedCanvas *fEcanvas;
 
  Int_t fSelView;
  Int_t fNofViews;
  TString fView[10];

  TPave* sensor[2000];

  TString fAdditString;
  TString fSettingString;
  TString specialString;

  Int_t fSelStation;
  Int_t fNofStations;
  TString fStation[10];

  TString fGeoFileName;

  Int_t    fNofStations;

  TObjArray* fStations;

  Double_t fZPos              [50];
  Int_t    fStationNumber     [20];
  Int_t    fNofSectors        [200];
//   Int_t    fNofSensors        [20];
//   Int_t    fNofSensorsInSector[20][1000];
//   Int_t    fSensorSectorNr    [20][1000];
//   Float_t  fSensorPosition [3][20][1000];
//   Float_t  fSensorDimension[3][20][1000];
//   Float_t  fSensorAngle       [20][1000];
  Float_t  fSensorArea        [20][1500][10];
//   Float_t  fSensorPitch       [20][1000][10];
//   Float_t  fSensorStereo      [20][1000];
  Int_t    fSectorNChannels   [200][1500];

  Int_t qaRecoToPlot;

  Int_t fNofMCEvents;
  Int_t fNofRecoEvents;

  Bool_t fUseMvd;

  TGTextBuffer  *fTbmsg;
  TGTextBuffer  *fTbmsg2;

  Float_t fHorizontalIncrease[10];

  TH1F* fhMomEffAll;
  TH1F* fhMomEffPrim;
  TH1F* fhMomEffSec;
  TH1F* fhMomRecAll;
  TH1F* fhMomRecPrim;
  TH1F* fhMomRecSec;
  TH1F* fhMomAccAll;
  TH1F* fhMomAccPrim;
  TH1F* fhMomAccSec;
  TH2F* fhHitPointCorrelation[10];
  TH2F* fhMomResAll;
  TH2F* fhMomResPrim;
  TH2F* fhMomResPrimClone;
  TH1F* fhNhGhosts;
  TH1F* fhNhClones;
  TH1F* fhRefTracks;
  TH1F* fhRecRefTracks;

  TH1F* fhHitFindingEffComb;

  Int_t fNEvents;
  //  Int_t fNStsTracks = fhMomAccAll->GetEntries();
  //  Int_t fNStsTracks = fhMomAccPrim->GetEntries();
  Int_t fNStsTracks;
  Int_t fNPrimTracks;
  Int_t fNSecTracks;
  Int_t fNGhosts;
  Int_t fNClones;

  Double_t effAll;
  Double_t allEff;
  Double_t effPrim;
  Double_t primEff;
  Double_t effSec;
  Double_t secEff;
  Double_t effRef;
  Double_t momentumResolutionPrim;
  Double_t momentumResolutionAll;

  TString fCbmroot;

  TString beingDrawn;
  
  TString qaFileName;
  TH1F* fhNofHits [10][1000][10];
  TH1F* fhFNofDigis[10][1000];
  TH1F* fhFNofDigisPerChip[10][1000][10];
  TH1F* fhBNofDigis[10][1000];
  TH1F* fhFClusterL[10][1000];
  TH1F* fhBClusterL[10][1000];
  Float_t fMaxHitDens;
  Float_t fMaxDigiOcc;
  Float_t fMaxCluster;
  Float_t fMinCluster;
  Float_t fMaxHFE;
  Float_t fMinHFE;

  TH2F* fhHitFindingEfficiency[10];
  
  TString qasimFileName;
  TH2F* fhELoss[10];
  Float_t fMaxELoss;
  TH2F* fhMrad[10];
  Float_t fMaxMrad;
  TH2F* fhMips[10];
  Float_t fMaxMips;

  Int_t fMaxNofDigis;
  
  Bool_t fGeoRead;
  Bool_t qaRead;
  Bool_t qasimRead;
  
  TGStatusBar *fStatusBar;
  
  void RecognizeStsLayout();
  
public:
  MyMainFrame(const TGWindow *p,UInt_t w,UInt_t h);
  virtual ~MyMainFrame();
  void DoDraw();
  void ChangeStation(Int_t itemp);
  
  void ChangeQaRecoPlot(Int_t itemp) { qaRecoToPlot = itemp; DoDraw(); }
  
  void ChangeView(Int_t itemp);

  void ChangeAdditString   (char* carr) {// cout << "addit string = " << carr << endl;
                                          fAdditString  = Form("%s",carr);
                                          ChangeQaFileName(); }

  void ChangeSettingString   (char* carr) {// cout << "setting string = " << carr << endl;
                                          fSettingString  = Form("%s",carr);
                                          ChangeQaFileName(); }
  

  void ChangeGeoFileName() { fGeoFileName = "sts_v11b_0-8.digi.par";}
  void ChangeQaFileName () { qaFileName  = "sts.reco.test.root";}
  void ChangeQaSimFileName () { qasimFileName  = "sts.mc.root";}

  void ChangeNofMCEvents (char* carr) { fNofMCEvents = atoi(carr); ChangeGeoFileName(); ChangeQaFileName();ChangeQaSimFileName(); }
  void ChangeNofRecoEvents (char* carr) { fNofRecoEvents = atoi(carr); ChangeGeoFileName(); ChangeQaFileName();ChangeQaSimFileName(); }
  
  void ReadGeometry();
  void ReadOccupancy();
  void ReadELoss();
  
  void SaveCanvas();
  
  void HECa(Int_t event, Int_t x, Int_t y, TObject *sel);
};

MyMainFrame::MyMainFrame(const TGWindow *p,UInt_t w,UInt_t h) {
  // Create a main frame
  
  fMain = new TGMainFrame(p,w,h);

  fGeoRead = kFALSE;
  qaRead = kFALSE;
  qasimRead = kFALSE;

  beingDrawn="";

  fSelView = 0;
  fNofViews = 6;
  fView[0] = "simple";
  fView[1] = "hit dens.";
  fView[2] = "sens occ.";
  fView[3] = "cluster l";
  fView[4] = "hit find eff";
  fView[5] = "chip occ.";

  qaRecoToPlot = 0;

  fSelStation = 0;
  fNofStations = 8;
  fStation[0] = "1";
  fStation[1] = "2";
  fStation[2] = "3";
  fStation[3] = "4";
  fStation[4] = "5";
  fStation[5] = "6";
  fStation[6] = "7";
  fStation[7] = "8";

  fHorizontalIncrease[0] = 1.65;
  fHorizontalIncrease[1] = 1.65;
  fHorizontalIncrease[2] = 1.50;
  fHorizontalIncrease[3] = 1.40;
  fHorizontalIncrease[4] = 1.20;
  fHorizontalIncrease[5] = 1.10;
  fHorizontalIncrease[6] = 1.00;
  fHorizontalIncrease[7] = 1.00;

  fNofMCEvents = 1000;
  fNofRecoEvents = 200;
  fUseMvd = kFALSE;

  fCbmroot = getenv("VMCWORKDIR");
  fCbmroot.ReplaceAll("/u/kotynia/",12,"",0);
  fCbmroot.ReplaceAll("/cbmroot",8,"",0);
  cout << "fcbmroot is in " << fCbmroot.Data() << endl;

  ChangeGeoFileName(); 
  ChangeQaFileName();
  ChangeQaSimFileName();

  // Create canvas widget
  fEcanvas = new TRootEmbeddedCanvas("Ecanvas",fMain,700,700);
  fMain->AddFrame(fEcanvas, new TGLayoutHints(kLHintsExpandX| kLHintsExpandY,
					      10,10,10,1));
  // Create a horizontal frame widget with buttons
  TGHorizontalFrame *hframe = new TGHorizontalFrame(fMain,200,40);

  TGButtonGroup *bview = new TGButtonGroup(hframe,"View",kHorizontalFrame);
  TGRadioButton *fV[8];
  for ( Int_t itemp = 0 ; itemp < fNofViews ; itemp++ )
    fV[itemp] = new TGRadioButton(bview,fView[itemp].Data());
  fV[fSelView]->SetState(kButtonDown);
  bview->Connect("Pressed(Int_t)","MyMainFrame",this,"ChangeView(Int_t)");
  hframe->AddFrame(bview,new TGLayoutHints(kLHintsCenterX,5,5,3,4));

//   TGButtonGroup *br = new TGButtonGroup(hframe,"Station to draw",kHorizontalFrame);
//   TGRadioButton *fR[8];
//   for ( Int_t itemp = 0 ; itemp < fNofStations ; itemp++ )
//     fR[itemp] = new TGRadioButton(br,fStation[itemp].Data());
//   fR[fSelStation]->SetState(kButtonDown);
//   br->Connect("Pressed(Int_t)","MyMainFrame",this,"ChangeStation(Int_t)");
//   hframe->AddFrame(br,new TGLayoutHints(kLHintsCenterX,5,5,3,4));
  TGComboBox *statCombo = new TGComboBox(hframe,100);
  for (Int_t istat = 0; istat < fNofStations ; istat++) {
    statCombo->AddEntry(Form("station %d",istat+1), istat);
  }
  statCombo->Connect("Selected(Int_t)","MyMainFrame",this,"ChangeStation(Int_t)");
  hframe->AddFrame(statCombo, new TGLayoutHints(kLHintsTop | kLHintsLeft,5,5,5,5));
  statCombo->Resize(100, 20);

  // combo box widget
  const Int_t nofRecoQaPlots = 6;
  TString recoQaPlots[nofRecoQaPlots] = {"efficiency",
					 "mom. resolution",
					 "nof hits",
					 "hits-pnts 2D",
					 "hits-pnts 1D",
					 "summary"};
  TGComboBox *fCombo = new TGComboBox(hframe,100);
  for (Int_t iqap = 0; iqap < nofRecoQaPlots; iqap++) {
    fCombo->AddEntry(recoQaPlots[iqap], iqap);
  }
  fCombo->Resize(150, 20);
  // Entry3 is selected as current
  fCombo->Select(qaRecoToPlot);
  fCombo->Connect("Selected(Int_t)","MyMainFrame",this,"ChangeQaRecoPlot(Int_t)");
  hframe->AddFrame(fCombo, new TGLayoutHints(kLHintsTop | kLHintsLeft,5,5,5,5));
  fMain->AddFrame(hframe, new TGLayoutHints(kLHintsCenterX,2,2,2,2));

  TGHorizontalFrame *h2frame = new TGHorizontalFrame(fMain,200,40);

  TGTextButton *draw = new TGTextButton(h2frame,"&Draw");
  draw->Connect("Clicked()","MyMainFrame",this,"DoDraw()");
  h2frame->AddFrame(draw, new TGLayoutHints(kLHintsCenterX,5,5,3,4));

  TGTextButton *readG = new TGTextButton(h2frame,"&Read geometry");
  readG->Connect("Clicked()","MyMainFrame",this,"ReadGeometry()");
  h2frame->AddFrame(readG, new TGLayoutHints(kLHintsCenterX,5,5,3,4));

  TGTextButton *readQA = new TGTextButton(h2frame,"&Read occupancy");
  readQA->Connect("Clicked()","MyMainFrame",this,"ReadOccupancy()");
  h2frame->AddFrame(readQA, new TGLayoutHints(kLHintsCenterX,5,5,3,4));

  TGTextButton *readEL = new TGTextButton(h2frame,"&Read energy loss");
  readEL->Connect("Clicked()","MyMainFrame",this,"ReadELoss()");
  h2frame->AddFrame(readEL, new TGLayoutHints(kLHintsCenterX,5,5,3,4));

  TGTextButton *saveCanv = new TGTextButton(h2frame,"&Save canvas");
  saveCanv->Connect("Clicked()","MyMainFrame",this,"SaveCanvas()");
  h2frame->AddFrame(saveCanv, new TGLayoutHints(kLHintsCenterX,5,5,3,4));

  TGTextButton *exit = new TGTextButton(h2frame,"&Exit",
					"gApplication->Terminate(0)");
  h2frame->AddFrame(exit, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
  fMain->AddFrame(h2frame, new TGLayoutHints(kLHintsCenterX,2,2,2,2));

  TGHorizontalFrame *tframe = new TGHorizontalFrame(fMain,600,40);

  TGTextEntry *spStText = new TGTextEntry(tframe,specialString.Data());
  tframe->AddFrame(spStText, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
  spStText->Connect("TextChanged(char*)","MyMainFrame",this,"ChangeSpecialString(char*)");
  
  TGTextEntry *adStText = new TGTextEntry(tframe,fTbmsg = new TGTextBuffer(10));
  fTbmsg->AddText(0, fAdditString.Data());
  tframe->AddFrame(adStText, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
  adStText->Connect("TextChanged(char*)","MyMainFrame",this,"ChangeAdditString(char*)");

  TGTextEntry *setStText = new TGTextEntry(tframe,fTbmsg2 = new TGTextBuffer(10));
  fTbmsg2->AddText(0, fSettingString.Data());
  tframe->AddFrame(setStText, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
  setStText->Connect("TextChanged(char*)","MyMainFrame",this,"ChangeSettingString(char*)");

  cout << fNofMCEvents << " MC events" << endl;
  cout << fNofRecoEvents << " reco events" << endl;
  TGNumberEntryField *spNEEntry = new TGNumberEntryField(tframe,-1,fNofMCEvents,
							 TGNumberFormat::kNESInteger);
  tframe->AddFrame(spNEEntry, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
  spNEEntry->Connect("TextChanged(char*)","MyMainFrame",this,"ChangeNofMCEvents(char*)");

  TGNumberEntryField *spREEntry = new TGNumberEntryField(tframe,-1,fNofRecoEvents,
							 TGNumberFormat::kNESInteger);
  tframe->AddFrame(spREEntry, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
  spREEntry->Connect("TextChanged(char*)","MyMainFrame",this,"ChangeNofRecoEvents(char*)");

//   TGTextEntry *geoFText = new TGTextEntry(tframe,geoFileName.Data());
//   tframe->AddFrame(geoFText, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
//   geoFText->Connect("TextChanged(char*)","MyMainFrame",this,"ChangeGeoFileName(char*)");
//   TGTextEntry *qaFText = new TGTextEntry(tframe,qaFileName.Data());
//   tframe->AddFrame(qaFText, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
//   qaFText->Connect("TextChanged(char*)","MyMainFrame",this,"ChangeQaFileName(char*)");

  fMain->AddFrame(tframe, new TGLayoutHints(kLHintsCenterX,2,2,2,2));


  Int_t parts[] = {8, 16, 36, 10, 10, 10, 10};
  fStatusBar = new TGStatusBar(fMain,50,10,kHorizontalFrame);
  fStatusBar->SetParts(parts,7);
  fMain->AddFrame(fStatusBar,new TGLayoutHints(kLHintsBottom | kLHintsLeft |
					       kLHintsExpandX,0,0,2,0));


  // Set a name to the main frame
  fMain->SetWindowName("Simple Example");
  // Map all subwindows of main frame
  fMain->MapSubwindows();
  // Initialize the layout algorithm
  fMain->Resize(fMain->GetDefaultSize());
  // Map main frame
  fMain->MapWindow();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void MyMainFrame::ChangeStation(Int_t itemp) {
  // Draws function graphics in randomly choosen interval
  //  cout << "ale jazda" << endl;
  fSelStation = itemp;

  if ( fGeoRead ) {//&& qaRead ) 
    //    cout << "before draw " << endl;
    DoDraw();
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void MyMainFrame::ChangeView(Int_t itemp) {
  // Draws function graphics in randomly choosen interval
  fSelView = itemp-1;

  if ( fGeoRead && ( qaRead || qasimRead ) ) 
    DoDraw();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void MyMainFrame::DoDraw() {
  //  cout << "drawing" << endl;
  // Draws function graphics in randomly choosen interval
  TCanvas *fCanvas = fEcanvas->GetCanvas();

  gStyle->SetOptStat(1);

  fEcanvas->SetName(specialString.Data());

  fCanvas->Clear();

  //  cout << "sel view = " << fSelView << endl;

  //  cout << "drawing1" << endl;
  if ( fSelView == 5 && qaRead ) {
    gStyle->SetPalette(1);

    if ( qaRecoToPlot == 0 ) fCanvas->SetLogz();

    fCanvas->SetFillColor(0);
    fCanvas->SetFillStyle(4000);
    fCanvas->SetBorderMode(0);
    
    if ( qaRecoToPlot == 0 ) {
      beingDrawn="momEff";

      fhMomEffAll->SetAxisRange(0.,1.1,"Y");
      fhMomEffAll->SetLineWidth(2);
      fhMomEffAll->SetLineColor(1);
      fhMomEffAll->SetTitle("Efficiency");
      fhMomEffAll->Draw();
      fhMomEffPrim->SetLineWidth(2);
      fhMomEffPrim->SetLineColor(2);
      fhMomEffPrim->Draw("same");
      fhMomEffSec->SetLineWidth(2);
      fhMomEffSec->SetLineColor(4);
      fhMomEffSec->Draw("same");
      
      TLegend* effLegDet = new TLegend(0.3,0.15,0.68,0.4);
      effLegDet->SetBorderSize(0);
      effLegDet->SetFillColor(0);
      effLegDet->AddEntry(fhMomEffPrim,Form("primaries     (%2.2f%%)",100.*primEff) ,"pl");
      effLegDet->AddEntry(fhMomEffAll, Form("all                (%2.2f%%)",100.*allEff),"pl");
      effLegDet->AddEntry(fhMomEffSec, Form("secondaries (%2.2f%%)",100.*secEff) ,"pl");
      effLegDet->Draw();

      TLine* oneLine = new TLine(0.0,1.0,10.0,1.0);
      oneLine->SetLineStyle(2);
      oneLine->Draw();
    }

    if ( qaRecoToPlot == 1 ) {
      beingDrawn="momRes";
 
      fhMomResPrim->SetAxisRange(0.,3.,"Y");
      fhMomResPrim->Draw("cont0");
      TH1F* fhLowBand = new TH1F("fhLowBand","Low Band",
				 fhMomResPrim->GetXaxis()->GetNbins(),
				 fhMomResPrim->GetXaxis()->GetXmin(),
				 fhMomResPrim->GetXaxis()->GetXmax());
      TH1F* fhHigBand = new TH1F("fhHigBand","Hig Band",
				 fhMomResPrim->GetXaxis()->GetNbins(),
				 fhMomResPrim->GetXaxis()->GetXmin(),
				 fhMomResPrim->GetXaxis()->GetXmax());
      for ( Int_t ibin = fhMomResPrim->GetXaxis()->GetNbins() ; ibin > 1 ; ibin-- ) {
	TF1* gausFit = new TF1("gausFit","gaus");
	TH1F* tempProjY = (TH1F*)fhMomResPrim->ProjectionY("tempProjY",ibin,ibin);
	tempProjY->Fit("gausFit","QN","",-5.,5.);
	fhLowBand->SetBinContent(ibin,gausFit->GetParameter(1)-gausFit->GetParameter(2));
	fhLowBand->SetBinError(ibin,0.01);
	fhHigBand->SetBinContent(ibin,gausFit->GetParameter(2));
	fhHigBand->SetBinError(ibin,gausFit->GetParError(2));
      }
      fhLowBand->SetMarkerSize(0.2);
      fhLowBand->SetLineWidth(2);
      //  fhHigBand->SetMarkerSize(0.1);
      fhHigBand->SetLineWidth(2);
      fhLowBand->Draw("Psame");
      fhHigBand->Draw("Psame");

      TLegend* effLegMR = new TLegend(0.3,0.15,0.68,0.23);
      effLegMR->SetBorderSize(0);
      effLegMR->SetFillColor(0);
      effLegMR->AddEntry(fhHigBand,Form("momentum resolution (%2.2f%%)",momentumResolutionPrim) ,"pl");
      effLegMR->Draw();
    }

    if ( qaRecoToPlot == 2 ) {
      beingDrawn="nofGhosts";
 
      fhNhGhosts->Draw();
    }

    if ( qaRecoToPlot == 3 ) {
      beingDrawn="hitC2";
      fhHitPointCorrelation[fSelStation]->Draw("col");
    }

    if ( qaRecoToPlot == 4 ) {
      beingDrawn="hitC1";
      TH1F* projX;
      TH1F* projY;
      TF1*   fitX;
      TF1*   fitY;
      Double_t resolution[2];
      
      projX = (TH1F*)fhHitPointCorrelation[fSelStation]->ProjectionX(Form("projX%i",fSelStation+1));
      projY = (TH1F*)fhHitPointCorrelation[fSelStation]->ProjectionY(Form("projY%i",fSelStation+1));
      fitX = new TF1(Form("fitX%i",fSelStation+1),"gaus",-0.02,0.02);
      fitY = new TF1(Form("fitY%i",fSelStation+1),"gaus",-0.02,0.02);
      projX->SetAxisRange(-0.02,0.02,"X");
      projY->SetAxisRange(-0.02,0.02,"X");
      projX->Fit(fitX,"QN","",-0.02,0.02);
      projY->Fit(fitY,"QN","",-0.02,0.02);
      resolution[0] = 10000.*fitX->GetParameter(2);
      resolution[1] = 10000.*fitY->GetParameter(2);
      
      projX->SetLineWidth(2);
      projY->SetLineWidth(2);
      projX->SetLineColor(2);
      projY->SetLineColor(3);
      projY->SetXTitle("#Delta x, y [cm]");
      projX->Draw();
      projY->Draw("same");
      TLegend* legend1 = new TLegend(0.6,0.6,1.0,0.94);
      legend1->SetBorderSize(0);
      legend1->SetFillColor(0);
      legend1->AddEntry(projX,
			Form("X,#sigma=%3.2f#mum",resolution[0]),"l");
      legend1->AddEntry(projY,
			Form("Y,#sigma=%3.2f#mum",resolution[1]),"l");
      legend1->Draw();
    }

    if ( qaRecoToPlot == 5 ) {
      beingDrawn="summary";

      fCanvas->Range(0.,0.,1.,1.);

      TPaveText* printoutPave = new TPaveText(0.0,0.0,1.0,1.0);
      printoutPave->SetTextAlign(23);
      printoutPave->SetTextSize(0.05);
      printoutPave->SetTextColor(1);
      printoutPave->SetBorderSize(0);
      printoutPave->SetFillColor(0);
      printoutPave->AddText(Form("%i events",fNEvents));
      printoutPave->AddText(Form("%3.2f prim, %3.2f sec, %3.2f gh, %3.2f cl",
				 Double_t (fNPrimTracks)/Double_t (fNEvents),
				 Double_t (fNSecTracks)/Double_t (fNEvents),
				 Double_t (fNGhosts)   /Double_t (fNEvents),
				 Double_t (fNClones)   /Double_t (fNEvents)));
      printoutPave->AddText("Single Hit Resolutions:");
      /*      for ( Int_t ist = 0 ; ist < nofStations ; ist++ )
	if ( resolution[0][ist] > 0.01 )
	  printoutPave->AddText(Form("st#%i,#sigma_{x}=%3.2f#mum,#sigma_{y}=%3.2f#mum",
	  ist+1,resolution[0][ist],resolution[1][ist]));*/
      printoutPave->AddText("Tracking efficiencies (p>1.0 GeV/c):");
      printoutPave->AddText(Form("all = %2.2f%%(%2.2f%%)",100.*effAll,100.*allEff));
      printoutPave->AddText(Form("vertex = %2.2f%%(%2.2f%%)",100.*effPrim,100.*primEff));
      printoutPave->AddText(Form("reference = %2.2f%%",100.*effRef));
      printoutPave->AddText(Form("non-vertex = %2.2f%%(%2.2f%%)",100.*effSec,100.*secEff));
      printoutPave->AddText(Form("Momentum resolution = %3.2f%%(%3.2f%%)",momentumResolutionAll,momentumResolutionPrim));
      printoutPave->Draw();
    }
 
    fCanvas->cd();
    fCanvas->Update();
    return;
  }

  Double_t xmax=55;
  Double_t ymax=55;
  //  cout << "drawing2" << endl;

  if ( fSelView >= 5 && qasimRead ) {
    fCanvas->SetFillColor(0);
    fCanvas->SetBorderSize(0);
    fCanvas->SetFrameFillColor(0);
    fCanvas->SetGrid(0,0);
    fCanvas->SetLogy(0);

    if ( fSelView == 5 ) 
      beingDrawn="eLoss";
   if ( fSelView == 6 ) 
      beingDrawn="mips";
    if ( fSelView == 7 ) 
      beingDrawn="mrads";

    gStyle->SetOptStat(0);
    gPad->SetLogz();
    gPad->SetRightMargin(0.15);

    const Int_t colNum = 10;
    Int_t palette[colNum];
    
    Float_t cMin = 0.0;        // 0.1
    Float_t cMax = 1.0;       // 0.95 
    Float_t dc = (cMax-cMin)/(Float_t)colNum;
    for( Int_t ic = 50; ic < (50+colNum); ic++ ) {
      TColor *color = (TColor*) (gROOT->GetListOfColors()->At(ic));
      Float_t c = cMin + (ic-50) * dc;
      //printf("%d   %f   %f\n",ic,c,dc);
      //    if(PlotInColor) 
      color->SetRGB(cMax-c,0,c);  // in color      blue -> red
      //    else color->SetRGB(c,c,c);                  // in grey
      palette[colNum-1-(ic-50)] = ic;
    }
    
    if ( fSelView == 5 ) {
      gStyle->SetPalette(colNum,palette);
      fhELoss[fSelStation]->SetTitle("");
      fhELoss[fSelStation]->SetAxisRange(-xmax,xmax,"X");
      fhELoss[fSelStation]->SetAxisRange(-ymax,ymax,"Y");
      fhELoss[fSelStation]->SetAxisRange(fMaxELoss/5000.,fMaxELoss*1.5,"Z");
      fhELoss[fSelStation]->Draw("colz");
    }
    if ( fSelView == 6 ) {
      fhMips[fSelStation]->SetTitle("");
      fhMips[fSelStation]->SetAxisRange(-xmax,xmax,"X");
      fhMips[fSelStation]->SetAxisRange(-ymax,ymax,"Y");
      fhMips[fSelStation]->SetAxisRange(fMaxMips/5000.,fMaxMips*1.5,"Z");
      fhMips[fSelStation]->Draw("colz");
    }
    if ( fSelView == 7 ) {
      gStyle->SetPalette(1);
//       fhMrad[fSelStation]->SetAxisRange(-xmax,xmax,"X");
//       fhMrad[fSelStation]->SetAxisRange(-ymax,ymax,"Y");
      gPad->SetBorderMode(0);
      fhMrad[fSelStation]->SetTitle("");
      fhMrad[fSelStation]->SetAxisRange(-30,30,"X");
      fhMrad[fSelStation]->SetAxisRange(-20,20,"Y");
      //      fhMrad[fSelStation]->SetAxisRange(maxMrad/5000.,maxMrad*1.5,"Z");
      fhMrad[fSelStation]->Draw("colz");
    }


    for ( Int_t iwaf = 0 ; iwaf < nofSensors[fSelStation] ; iwaf++ ) {
      Double_t phi=sensorAngle[fSelStation][iwaf]*3.14159/180.;
      
      Double_t x1 = sensorPosition[0][fSelStation][iwaf]-sensorDimension[0][fSelStation][iwaf]/2*cos(phi)-sensorDimension[1][fSelStation][iwaf]/2*sin(phi);
      Double_t y1 = sensorPosition[1][fSelStation][iwaf]-sensorDimension[0][fSelStation][iwaf]/2*sin(phi)-sensorDimension[1][fSelStation][iwaf]/2*cos(phi);
      Double_t x2 = sensorPosition[0][fSelStation][iwaf]+sensorDimension[0][fSelStation][iwaf]/2*cos(phi)+sensorDimension[1][fSelStation][iwaf]/2*sin(phi);
      Double_t y2 = sensorPosition[1][fSelStation][iwaf]+sensorDimension[0][fSelStation][iwaf]/2*sin(phi)+sensorDimension[1][fSelStation][iwaf]/2*cos(phi);

      TLine* up = new TLine(x1,y1,x2,y1); up->Draw();
      TLine* bo = new TLine(x1,y2,x2,y2); bo->Draw();
      TLine* ri = new TLine(x1,y1,x1,y2); ri->Draw();
      TLine* le = new TLine(x2,y1,x2,y2); le->Draw();
    }

    TLegend* unitL;
    if ( fSelView == 5 )
      unitL = new TLegend(.8,.9,1.,1.,"GeV/cm^{3}");
    if ( fSelView == 6 )
      unitL = new TLegend(.8,.9,1.,1.,"#MIPs/6yr");
    if ( fSelView == 7 )
      unitL = new TLegend(.8,.9,1.,1.,"Mrad/6yr");
    unitL->SetFillColor(0);
    unitL->SetBorderSize(0);
    unitL->Draw();

    fCanvas->cd();
    fCanvas->Update();
    return;
  }


  //  cout << "drawing3" << endl;
  TLine* line;
  Double_t maxx = (int)xmax/10*10;
  Double_t maxy = (int)ymax/10*10;
  
  fCanvas->Range(-xmax,-ymax,xmax,ymax);
  fCanvas->SetFillColor(0);
  fCanvas->SetBorderSize(2);
  fCanvas->SetFrameFillColor(0);
  fCanvas->SetGrid(0,0);
  fCanvas->SetLogy(0);

  fCanvas->SetBottomMargin(0.);
  fCanvas->SetRightMargin (0.);
  fCanvas->SetLeftMargin  (0.);
  fCanvas->SetTopMargin   (0.);
  
  if ( fSelView == 2 || fSelView == 0 ) {
    fCanvas->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
		     "MyMainFrame", this,"HECa(Int_t,Int_t,Int_t,TObject*)");
  }
    
  line = new TLine(0,-ymax,0,ymax); line->Draw();
  line = new TLine(-xmax,0,xmax,0); line->Draw();
  // draw axis
  Double_t maxx = (int)xmax/10*10;
  Double_t maxy = (int)ymax/10*10;
  TGaxis *axis1 = new TGaxis(-maxx,0,maxx,0,-maxx,maxx,412,"-+");    axis1->SetName("axis1");    axis1->SetLabelSize(0.03);    axis1->Draw();
  TGaxis *axis2 = new TGaxis(0,-maxy,0,maxy,-maxy,maxy,412,"-+");    axis2->SetName("axis2");    axis2->SetLabelSize(0.03);    axis2->Draw();
  // draw axis text 
  TLatex* tx = new TLatex(maxx-maxx/12,4,"X/cm");     tx->SetTextSize(0.03);    tx->Draw();
  TLatex* ty = new TLatex(4.2,maxy-1.3,"Y/cm");       ty->SetTextSize(0.03);    ty->Draw();
  // draw Station text

  fStatusBar->SetText(Form("Station %d",fSelStation+1),0);
  //  cout << "drawing4" << endl;

  /*  if ( fSelView == 4 ) {
    fhHitFindingEfficiency[fSelStation]->SetAxisRange(-xmax,xmax,"X");
    fhHitFindingEfficiency[fSelStation]->SetAxisRange(-ymax,ymax,"Y");
    fhHitFindingEfficiency[fSelStation]->SetAxisRange(0.,100.,"Z");
    fhHitFindingEfficiency[fSelStation]->Draw("acolsame");
    }*/

  if ( fGeoRead ) {
    
    beingDrawn=Form("st%d",fSelStation+1);
    if ( qaRead && fSelView != 0 ) {
      if ( fSelView == 1 ) beingDrawn = Form("%shitDens",beingDrawn.Data());
      if ( fSelView == 2 ) beingDrawn = Form("%ssecOccup",beingDrawn.Data());
      if ( fSelView == 3 ) beingDrawn = Form("%sclustLen",beingDrawn.Data());
      if ( fSelView == 4 ) beingDrawn = Form("%shitFEff",beingDrawn.Data());
    }
    else {
      if ( fSelView == 0 ) beingDrawn = Form("%sLayout",beingDrawn.Data());
    }

    CbmStsStation* station = (CbmStsStation*)fStations->At(fSelStation);

    TLatex* ztxt = new TLatex(maxx/3,maxy-1.3,Form("STS station %i, z=%2.1fcm",station->GetStationNr(),station->GetZ())); 
    ztxt->SetTextSize(0.03);
    ztxt->Draw();
    
    const Int_t colNum = 10;
    Int_t palette[colNum];
    
    if ( qaRead && fSelView != 0 ) {
      // create a color palette of colNum colors, starting from position nb. 50. 
      Float_t cMin = 0.0;        // 0.1
      Float_t cMax = 1.0;       // 0.95 
      Float_t dc = (cMax-cMin)/(Float_t)colNum;
      for( Int_t ic = 50; ic < (50+colNum); ic++ ) {
	TColor *color = (TColor*) (gROOT->GetListOfColors()->At(ic));
	Float_t c = cMin + (ic-50) * dc;
	//printf("%d   %f   %f\n",ic,c,dc);
	//    if(PlotInColor) 
	color->SetRGB(cMax-c,0,c);  // in color      blue -> red
	//    else color->SetRGB(c,c,c);                  // in grey
	palette[colNum-1-(ic-50)] = ic;
      }
      
      //      gStyle->SetPalette(colNum,palette);
      
      // draw occupancy legend
      Float_t xstart=xmax*0.3;
      Float_t dx=xmax*0.05;
      Float_t ystart=ymax*0.85;
      Float_t dy=ymax*0.05;
      for(Int_t i=0;i<10;i++) {
	pave = new TPave(xstart+dx*i,-ystart,xstart+dx*(i+1),-(ystart+dy),1); 
	//	if ( fSelStation < 5 )
	pave->Draw();      
	//      Int_t color = (float)COLORSTART-((float)COLORSTART-(float)COLORSTOP)/10*(float)i;
	pave->SetFillColor(palette[i]);
	//	if ( i == 0 ) 	pave->SetFillColor(3);
      }
      
      Float_t maxOccup;
      Float_t minOccup = 0.;
      if ( fSelView == 1 ) maxOccup = fMaxHitDens;
      if ( fSelView == 2 ) maxOccup = fMaxDigiOcc;
      if ( fSelView == 3 ) {
	maxOccup = fMaxCluster;
	minOccup = fMinCluster;
      }
      if ( fSelView == 4 ) {
	maxOccup = fMaxHFE;
	minOccup = fMinHFE; 
      }

      // draw label text
      char upper[100];
      sprintf(upper,"%3.2f",maxOccup);
      TLatex* tup = new TLatex(xstart+9*dx,-(ystart+dy*2),upper);
      tup->SetTextSize(0.015);
      //	if ( fSelStation < 5 )
      tup->Draw();
      char lower[100];
      sprintf(lower,"%3.2f",minOccup);
      TLatex* tlow = new TLatex(xstart+0.5*dx,-(ystart+dy*2),"0.1");
      if ( fSelView == 3 || fSelView == 4 ) 
	tlow->SetText(xstart+0.5*dx,-(ystart+dy*2),lower);
      tlow->SetTextSize(0.015);
      //	if ( fSelStation < 5 )
      tlow->Draw();
      char middle[100];
      sprintf(middle,"%3.2f",(maxOccup+minOccup)/2.);
      TLatex* tmid = new TLatex(xstart+4.5*dx,-(ystart+dy*2),middle);
      tmid->SetTextSize(0.015);
      //	if ( fSelStation < 5 )
      tmid->Draw();
      char ttext[100];
      if ( fSelView == 1 ) sprintf(ttext,"hit density per cm^{2}");
      if ( fSelView == 2 ) sprintf(ttext,"sensor occupancy [%%]");
      if ( fSelView == 3 ) sprintf(ttext,"cluster length [channels]");
      if ( fSelView == 4 ) sprintf(ttext,"hit finding efficiency [%%]");
      TLatex* ttex = new TLatex(xstart+7.5*dx,-(ystart-0.5*dy),ttext);
      ttex->SetTextSize(0.015);
      //      if ( fSelStation < 5 )
	ttex->Draw();

    }
    
    Int_t isector = 0;
    CbmStsSector* sector = NULL;
    CbmStsSensor* sensor = NULL;
    for ( Int_t isect = 0 ; isect < station->GetNSectors() ; isect++ ) {
      sector = station->GetSector(isect);

      for ( Int_t isens = 0 ; isens < sector->GetNSensors() ; isens++ ) {
	sensor = sector->GetSensor(isens);

	Double_t phi=sensor->GetRotation();
      
	Double_t xc = sensor->GetX0();
	Double_t yc = sensor->GetY0();
	Double_t lx = sensor->GetLx();
	Double_t ly = sensor->GetLy();
	
	Double_t phi=sensor->GetRotation();
	
	Double_t x1 = xc-lx/2.*cos(phi)-ly/2.*sin(phi);
	Double_t y1 = yc-lx/2.*sin(phi)-ly/2.*cos(phi);
	Double_t x2 = xc+lx/2.*cos(phi)+ly/2.*sin(phi);
	Double_t y2 = yc+lx/2.*sin(phi)+ly/2.*cos(phi); 

	//	TPave* sensorPave = new TPave(x1,y1,x2,y2,0);
	TPaveText* sensorPave = new TPaveText(x1,y1,x2,y2,0);
        for (Int_t ichip=1; ichip<9; ichip++){
          Double_t chipx = lx/8;
          TPaveText* chipPave   = new TPaveText(x1,y1,x1+(chipx*ichip),y2,0);
          chipPave->SetBorderSize(0);
	  chipPave  ->SetName(Form("Sector%03dSensor%03d%03d",isect+1,isens+1,ichip));
	}
        
	sensorPave->SetBorderSize(0);

	/*	if ( TMath::Abs(y1) > 1. ) 
	  sensorPave->AddText(0.,0.,Form("%2.1f",y2));
	else 
	sensorPave->AddText(0.,0.,Form("%2.1f",x1));*/

	TPave* backSide;
	sensorPave->SetName(Form("Sector%03dSensor%03d",isect+1,isens+1));
	
	Double_t sensColor = 50.+70.*(sensor->GetZ0() - station->GetZ()+.35);
	sensorPave->SetFillColor(sensColor);
	chipPave->SetFillColor(sensColor);
	//	cout << "occupancy" << endl;
	Float_t occupancy;
	Float_t occupancy1;
	if ( qaRead && fSelView != 0 ) {
	  //	  cout << "do i really come here?" << endl;
	  
	  if ( fSelView == 1 ) {
	    occupancy = fhNofHits[fSelStation][isect][isens]->GetMean()/fSensorArea[fSelStation][isect][isens];
	    sensorPave->SetFillColor(palette[Int_t(occupancy*10./fMaxHitDens)]);
	    //	  if ( occupancy < 0.1 ) sensor[iwaf]->SetFillColor(3);
	  }
	  //	  cout << "ehllo" << endl;
	  if ( fSelView == 2 ) {

	    occupancy = 100.*fhFNofDigis[fSelStation][isect]->GetMean()/((Float_t)fSectorNChannels[fSelStation][isect]);
cout<<occupancy<<endl;
	    sensorPave->SetFillColor(palette[Int_t(occupancy*10./fMaxDigiOcc)]);
	    occupancy = 100.*fhBNofDigis[fSelStation][isect]->GetMean()/((Float_t)fSectorNChannels[fSelStation][isect]);
	    if ( yc > 0 )
	      backSide = new TPave((x1+x2)/2.,(y1+y2)/2.,x2,y2,0);
	    else
	      backSide = new TPave(x1,y1,(x1+x2)/2.,(y1+y2)/2.,0);
	    backSide->SetName(Form("Sector%03dSensor%03dBack",isect+1,isens+1));
	    backSide->SetFillColor(palette[Int_t(occupancy*10./fMaxDigiOcc)]);
	    //	  if ( occupancy < 0.1 ) sensor[iwaf]->SetFillColor(3);
	  }

	  if ( fSelView == 3 ) {
	    occupancy = fhFClusterL[fSelStation][isect]->GetMean();
	    sensorPave->SetFillColor(palette[Int_t((occupancy-fMinCluster)*10./(fMaxCluster-fMinCluster))]);
	    occupancy = fhBClusterL[fSelStation][isect]->GetMean();
	    if ( yc > 0 )
	      backSide = new TPave((x1+x2)/2.,(y1+y2)/2.,x2,y2,0);
	    else
	      backSide = new TPave(x1,y1,(x1+x2)/2.,(y1+y2)/2.,0);
	    backSide->SetName(Form("Sector%03dSensor%03dBack",isect+1,isens+1));
	    backSide->SetFillColor(palette[Int_t((occupancy-fMinCluster)*10./(fMaxCluster-fMinCluster))]);
	    //	  if ( occupancy < 0.1 ) sensor[iwaf]->SetFillColor(3);
	  }

	  if ( fSelView == 4 ) {
	    for (Int_t IChip=1; IChip<9; IChip++){
// cout<<(Int_t)(fhFNofDigisPerChip[fSelStation][isect][IChip])<<endl;
	      occupancy1 = 100.*fhFNofDigisPerChip[fSelStation][isect][IChip]->GetMean();
cout<<"1  "<<occupancy1<<endl;
	      //	    cout << "occupancy = " << occupancy << " so color = " << Int_t((occupancy-fMinHFE)*10./(fMaxHFE-fMinHFE)) << endl;
//  	      chipPave->SetFillColor(palette[Int_t(occupancy1*10./0.1)]);
	      //	  if ( occupancy < 0.1 ) sensor[iwaf]->SetFillColor(3);
	    }
	chipPave->Draw();
	  }
	  //	  cout << "end" << endl;
	}
	//	cout << "what the heck?" << endl;

	sensorPave->Draw();

	if ( backSide ) backSide->Draw();    

	TLine* ri = new TLine(x1,y1,x1,y2); ri->SetLineWidth(2); ri->Draw();
	TLine* le = new TLine(x2,y1,x2,y2); le->SetLineWidth(2); le->Draw();

	if ( isens == 0 ) {
	  TLine* bo = new TLine(x1,y1,x2,y1); bo->SetLineWidth(2); bo->Draw();
	}
	if ( isens == sector->GetNSensors()-1 ) {
	  TLine* up = new TLine(x1,y2,x2,y2); up->SetLineWidth(2); up->Draw();
	}
      }
    }
    
    if ( fSelView == 0 ) {
      const Int_t colZPos = 50;
      Int_t paletteZPos[colZPos];
      
      for( Int_t ic = 0; ic < colZPos ; ic++ ) {
	//    TColor *color = (TColor*) (gROOT->GetListOfColors()->At(50+ic));
	paletteZPos[ic] = 51+ic;
      }
      // draw occupancy legend
      Float_t xstart=xmax*0.3;
      Float_t dx=xmax*0.01;
      Float_t ystart=ymax*0.85;
      Float_t dy=ymax*0.05;
      for(Int_t i=0;i<50;i++) {
	pave = new TPave(xstart+dx*i,-ystart,xstart+dx*(i+1),-(ystart+dy),1); 
	pave->SetBorderSize(0);
	pave->Draw();      
	pave->SetFillColor(paletteZPos[i]);
      }
      cout << "station at " << station->GetZ() << endl;
      
      TLatex* tlow = new TLatex(xstart+0.5*dx,-(ystart+dy*2),Form("%.2f",-0.35));
      tlow->SetTextSize(0.02);
      tlow->Draw();
      TLatex* tmid = new TLatex(xstart+24.5*dx,-(ystart+dy*2),Form("%.0f",station->GetZ()));
      tmid->SetTextSize(0.02);
      tmid->Draw();
      TLatex* tup = new TLatex(xstart+45*dx,-(ystart+dy*2),Form("%.2f",station->GetZ()+0.35));
      tup->SetTextSize(0.02);
      tup->Draw();
      TLatex* ttex = new TLatex(xstart+35*dx,-(ystart-0.5*dy),"z [cm]");
      ttex->SetTextSize(0.03);
      ttex->Draw();
    }

    //    if ( fSelView == 0 ) {
    if ( 1 == 0 ) {
      TEllipse* innerAccLimit = new TEllipse(0.,0.,
					     station->GetZ()*TMath::Tan(2.5*TMath::DegToRad()),
					     station->GetZ()*TMath::Tan(2.5*TMath::DegToRad()));
      innerAccLimit->SetLineWidth(2);
      innerAccLimit->Draw();
      
      TEllipse* outerAccLimit = new TEllipse(0.,0.,
					     station->GetZ()*TMath::Tan(25.*TMath::DegToRad()),
					     station->GetZ()*TMath::Tan(25.*TMath::DegToRad()));
      outerAccLimit->SetLineWidth(2);
      outerAccLimit->SetLineStyle(2);
      outerAccLimit->Draw();

      TEllipse* ellipAccLimit = new TEllipse(0.,0.,
					     fHorizontalIncrease[fSelStation]*station->GetZ()*TMath::Tan(25.*TMath::DegToRad()),
					     station->GetZ()*TMath::Tan(25.*TMath::DegToRad()));
      ellipAccLimit->SetLineWidth(2);
      ellipAccLimit->Draw();

      // Beam pipe size:
      // at z= 27.cm radius is  1.05 cm
      // at z=160.cm radius is  3.25 cm

      TEllipse* beamPAccLimit = new TEllipse(0.,0.,
					     1.05+(station->GetZ()-27.)*0.0165414,
					     1.05+(station->GetZ()-27.)*0.0165414);
      beamPAccLimit->SetFillColor(2);
      beamPAccLimit->SetLineWidth(0);
      beamPAccLimit->Draw("f");

    }

    /*    if ( fSelView == 0 && fSelStation == 4 ) {
      Float_t tx1 = 12.; Float_t tx2 = 18.;
      Float_t ty1 =  0.; Float_t ty2 = 30.;
      TLine* up = new TLine(tx1,ty1,tx2,ty1); up->SetLineWidth(3); up->SetLineColor(1); up->Draw();
      TLine* bo = new TLine(tx1,ty2,tx2,ty2); bo->SetLineWidth(3); bo->SetLineColor(1); bo->Draw();
      TLine* ri = new TLine(tx1,ty1,tx1,ty2); ri->SetLineWidth(3); ri->SetLineColor(1); ri->Draw();
      TLine* le = new TLine(tx2,ty1,tx2,ty2); le->SetLineWidth(3); le->SetLineColor(1); le->Draw();

      Float_t ry1 = 30.; Float_t ry2 = 34.;
      TPave* ropav = new TPave(tx1,ry1,tx2,ry2,1);
      ropav->SetFillColor(1);
      ropav->SetLineColor(1);
      ropav->Draw();
      }*/
  }

  fCanvas->cd();
  fCanvas->Update();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void MyMainFrame::RecognizeStsLayout() {

  TString cutFileName = fGeoFileName.Data();


//   while ( cutFileName.Contains("_s") ) 
//     cutFileName.Remove(0,1);
//   cutFileName.Remove(0,1);
// 
//   while ( cutFileName.Contains(".") ) 
//     cutFileName.Chop();

  TString firstStationZ = 30;
//   firstStationZ.Remove(2,firstStationZ.Length()-2);

  fZPos[0] = 30;
  fZPos[1] = 40;
  fZPos[2] = 50;
  fZPos[3] = 60;
  fZPos[4] = 70;
  fZPos[5] = 80;
  fZPos[6] = 90;
  fZPos[7] = 100;
  
//   Int_t is = 1;

//   if ( fGeoFileName.Contains(".SS") ) {
//     fNofStations = 2*fNofStations;
//     fZPos[1] = fZPos[0]+0.5;
//     is = 2;
//   }
// 
//   Int_t jump = is;
// 
//   for ( Int_t ichar = 2 ; ichar < cutFileName.Length() ; ichar++ ) {
//     if ( (Int_t)cutFileName[ichar] >= 48 && (Int_t)cutFileName[ichar] < 58 ) 
//       fZPos[is] = fZPos[is-jump]+(Int_t)cutFileName[ichar]-48;
//     if ( (Int_t)cutFileName[ichar] >= 65 && (Int_t)cutFileName[ichar] < 91 ) 
//       fZPos[is] = fZPos[is-jump]+10+(Int_t)cutFileName[ichar]-65;
//     if ( (Int_t)cutFileName[ichar] >= 97 && (Int_t)cutFileName[ichar] < 123 ) 
//       fZPos[is] = fZPos[is-jump]+10+26+(Int_t)cutFileName[ichar]-97;
//     is++;
//     if ( fGeoFileName.Contains(".SS") ) {
//       fZPos[is] = fZPos[is-1]+0.5;
//       is++;
//     }
//   }
  //cout << " -> " << cutFileName[ichar] << " -> " <<  << endl;

  cout << "seems that there are " << fNofStations << " stations at z = " << flush;
  for ( Int_t istat = 0 ; istat < fNofStations ; istat++ ) 
    cout << ( istat?", ":"" ) << fZPos[istat] << flush;
  cout << " cm." << endl;

}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void MyMainFrame::ReadGeometry() {

  RecognizeStsLayout();

  if ( fGeoRead ) {
    cout << " Geometry already read" << endl;
    return;
  }

  fStations = new TObjArray(10);

  ifstream fin("../../parameters/sts/sts_v11b_0-8.digi.par");

  string dummyText;         //dummy text

  if(!fin) { cout << "sorry, no file \"" << fGeoFileName.Data() << "\"" << endl; return; }

//   do {
//     getline(fin,dummyText);
//     TString check = Form ("%s",dummyText);
//   }
//   while ( check.Contains("#") );

  Float_t tempFloat1 = 0.0;
  Float_t tempFloat2 = 0.0;
  Float_t tempInt = 0;
  Float_t tempInt2 = 0;

  CbmStsStation* station;
  CbmStsSector*  sector;
  CbmStsSensor*  sensor;
//    fNofStations = 8;
  //if ( fGeoFileName.Contains(".SS") ) {
  //  fNofStations = 8;

// fin >> a >> b >> c;
// cout<<"   "<<a<<"   "<<b<<"   "<<c<<endl;
  Int_t nofss = 0;
// cout<<fNofStations<<"KKKKKK"<<endl;
  for ( Int_t istat = 0 ; istat < fNofStations ; istat++ ) {
    fin >> fStationNumber[istat] >> tempFloat1 >> fNofSectors[istat];
    
    Int_t stationNr = istat+1;

    cout << "STATION number " << fStationNumber[istat] << " has " << fNofSectors[istat] << " sectors" << endl;

    //  fNofSensors[istat] = 0;

    TString stationName = Form("stat%02d",stationNr);
    station = new CbmStsStation(stationName.Data(), stationNr, fZPos[istat],
				-777., -777., 0., 
				100., tempFloat1);
    fStations->Add(station);    

    for ( Int_t isect = 0 ; isect < fNofSectors[istat] ; isect++ ) {
      fin >> tempInt >> nofss;
      Int_t sectorNr = isect+1;
      
      Int_t sectorDetId  = 2 << 24 | stationNr << 16 | sectorNr << 4;

      TString sectorName = Form("stat%02dsect%d",stationNr,sectorNr);
      sector  = new CbmStsSector(sectorName.Data(), sectorDetId);

      for ( Int_t isens = 0 ; isens < nofss ; isens++ ) {
	Double_t sensorX, sensorY, sensorZ;
	Double_t sensorRot;
	Double_t sensorXDim, sensorYDim, sensorZDim;
	Double_t sensorDX, sensorDY, sensorStereoF, sensorStereoB;
	fin >> tempInt >> tempInt2 >> sensorX >> sensorY >> sensorZ >> sensorRot
	    >> sensorXDim >> sensorYDim >> sensorZDim
	    >> sensorDX >> sensorDY >> sensorStereoF >> sensorStereoB;
	fSensorArea     [istat][isect][isens] = sensorXDim*sensorYDim;
	if ( isens == 0 ) 
	  fSectorNChannels[istat][isect] = TMath::Ceil(sensorXDim/sensorDX);

	sensorStereoF = sensorStereoF * TMath::Pi() / 180.;
	sensorStereoB = sensorStereoB * TMath::Pi() / 180.;
	sensorRot    = sensorRot    * TMath::Pi() / 180.;

	Int_t sensorNr = isens+1;
	Int_t    detId  = 2 << 24 | stationNr << 16 | sectorNr << 4 | sensorNr << 1;
	
	TString sensorName = Form("stat%02dsect%dsens%d",stationNr,sectorNr,sensorNr);
	sensor = new CbmStsSensor(sensorName.Data(), detId, 
				  tempInt2, 
				  sensorX, sensorY, sensorZ,
				  sensorRot, 
				  sensorXDim, sensorYDim, sensorZDim, 
				  sensorDX, sensorDY, sensorStereoF, sensorStereoB);

	sector->AddSensor(sensor);
// 	cout << "helloZ!! " << 	fSensorPosition[2][istat][iwaf] << endl;
      }
      station->AddSector(sector);
    }
  }

  cout << "read geometry successful" << endl;
  fGeoRead = kTRUE;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void MyMainFrame::ReadOccupancy() {

  if ( !fGeoRead ) {
    cout << " Geometry not read" << endl;
    return;
  }

  fMaxHitDens = 0.;
  fMaxDigiOcc = 0.;
  fMaxCluster = 0.;
  fMinCluster = 100.;
  fMaxHFE = 0.;
  fMinHFE = 100.;

  qaFileName = "sts.reco.test.root";

  cout << "Occupancy read from file: \"" << qaFileName.Data() << "\"" << endl;

  TFile* occuF = TFile::Open(qaFileName.Data());   

  if ( !occuF )
    { cout << "sorry, no file" << endl; return; }

  //  TString directoryName = "STSReconstructionQA";
  TString directoryName = "STSFindHitsQA";
  
  fhHitFindingEffComb = (TH1F*)occuF->Get(Form("%s/hHitFindingEfficiency",directoryName.Data()));

  Int_t nofStations = fStations->GetEntriesFast();
  CbmStsStation* station = NULL;
  CbmStsSector*  sector  = NULL;
  CbmStsSensor*  sensor  = NULL;
  for ( Int_t istat = 0 ; istat < nofStations ; istat++ ) {
    station = (CbmStsStation*)fStations->At(istat);

    fhHitFindingEfficiency[istat  ] = (TH2F*)occuF->Get(Form("%s/Station%d/hRecoPoints%d",directoryName.Data(),istat+1,istat+1));
    //    fhHitFindingEfficiency[istat  ]->Rebin2D(2,2);
    fhHitFindingEfficiency[istat+1] = (TH2F*)occuF->Get(Form("%s/Station%d/hPoints%d",directoryName.Data(),istat+1,istat+1));
    cout << "STATION #" << istat+1 << " : " << fhHitFindingEfficiency[istat  ]->Integral() 
	 << " points of " << fhHitFindingEfficiency[istat+1]->Integral() 
	 << " found -> hfe = " 
	 << fhHitFindingEfficiency[istat  ]->Integral()/fhHitFindingEfficiency[istat+1]->Integral() << endl;
    //    fhHitFindingEfficiency[istat+1]->Rebin2D(2,2);
    fhHitFindingEfficiency[istat  ]->Divide(fhHitFindingEfficiency[istat  ],fhHitFindingEfficiency[istat+1]);
    fhHitFindingEfficiency[istat  ]->Scale(100.);
cout<<"KKKKKKK"<<station->GetNSectors()<<endl;
    for ( Int_t isect = 0 ; isect < station->GetNSectors() ; isect++ ) {

      sector = station->GetSector(isect);
      Float_t occupancy;

      for ( Int_t isens = 0 ; isens < sector->GetNSensors() ; isens++ ) {
	TString tempStr = Form("%s/Station%d/hNofHitsSt%dSect%dSens%d",directoryName.Data(),istat+1,istat+1,isect+1,isens+1);
	//	cout << "looking in \"" << tempStr.Data() << "\"" << endl;
	fhNofHits[istat][isect][isens] = (TH1F*)occuF->Get(Form("%s/Station%d/hNofHitsSt%dSect%dSens%d",directoryName.Data(),istat+1,istat+1,isect+1,isens+1));
	if ( !fhNofHits[istat][isect][isens] ) 
	  cout << "no hits table for " << istat << " " << isect << " " << isens << endl;

	occupancy = fhNofHits[istat][isect][isens]->GetMean()/fSensorArea[istat][isect][isens];
	if ( fMaxHitDens < occupancy ) fMaxHitDens = occupancy;

	occupancy = fhHitFindingEffComb->GetBinContent(((istat+1)<<16|(isect+1)<<4|(isens+1)<<1)+1);
	//	cout << "hfe = " << occupancy << endl;
	if ( fMaxHFE < occupancy ) fMaxHFE = occupancy;
	if ( fMinHFE > occupancy ) fMinHFE = occupancy;
	//	cout << " --> " << fMinHFE << " to " << fMaxHFE << endl;
      }
      fhFNofDigis[istat][isect] = (TH1F*)occuF->Get(Form("%s/Station%d/hNofFiredDigisFSt%dSect%d",directoryName.Data(),istat+1,istat+1,isect+1));
      for (Int_t chiip=1; chiip<9; chiip++){
	fhFNofDigisPerChip[istat][isect][chiip] = (TH1F*)occuF->Get(Form("%s/Station%d/hNofFiredDigisFSt%dSect%dChip%d",directoryName.Data(),istat+1,istat+1,isect+1,chiip));
      }
      fhFClusterL[istat][isect] = (TH1F*)occuF->Get(Form("%s/Station%d/hClusterLengthFSt%dSect%d",directoryName.Data(),istat+1,istat+1,isect+1));
      fhBNofDigis[istat][isect] = (TH1F*)occuF->Get(Form("%s/Station%d/hNofFiredDigisBSt%dSect%d",directoryName.Data(),istat+1,istat+1,isect+1));
      fhBClusterL[istat][isect] = (TH1F*)occuF->Get(Form("%s/Station%d/hClusterLengthBSt%dSect%d",directoryName.Data(),istat+1,istat+1,isect+1));
      occupancy = 100.*fhFNofDigis[istat][isect]->GetMean()/((Float_t)fSectorNChannels[istat][isect]);

      if ( fMaxDigiOcc < occupancy ) fMaxDigiOcc = occupancy;
      occupancy = fhFClusterL[istat][isect]->GetMean();
      if ( fMaxCluster < occupancy ) fMaxCluster = occupancy;
      if ( fMinCluster > occupancy ) fMinCluster = occupancy;
    }
  }

  fMaxHitDens = TMath::Ceil(fMaxHitDens);
  fMaxDigiOcc = 0.1*TMath::Ceil(fMaxDigiOcc*10);
  fMaxCluster = 0.1*TMath::Ceil(fMaxCluster*10);
  fMinCluster = 0.1*TMath::Floor(fMinCluster*10);
  fMaxHFE     = 0.01*TMath::Ceil(fMaxHFE*100.);
  fMinHFE     = 0.01*TMath::Floor(fMinHFE*100.);

  cout << "Maximum hit density = " << fMaxHitDens << " hits per square centimeter" << endl;
  cout << "Maximum digi occupancy is " << fMaxDigiOcc << " percent" << endl;
  cout << "Minimum cluster length is " << fMinCluster << " channels" << endl;
  cout << "Maximum cluster length is " << fMaxCluster << " channels" << endl;
  cout << "Minimum hit finding eff is " << fMinHFE << " " << endl;
  cout << "Maximum hit finding eff is " << fMaxHFE << " " << endl;

  qaRead = kTRUE;

  TString trackingDirName = "STSReconstructionQA";

  fhMomEffAll  = (TH1F*)occuF->Get(Form("%s/hMomEffAll",trackingDirName.Data()));
  fhMomEffPrim = (TH1F*)occuF->Get(Form("%s/hMomEffPrim",trackingDirName.Data()));
  fhMomEffSec  = (TH1F*)occuF->Get(Form("%s/hMomEffSec",trackingDirName.Data()));
  fhMomRecAll  = (TH1F*)occuF->Get(Form("%s/hMomRecAll",trackingDirName.Data()));
  fhMomRecPrim = (TH1F*)occuF->Get(Form("%s/hMomRecPrim",trackingDirName.Data()));
  fhMomRecSec  = (TH1F*)occuF->Get(Form("%s/hMomRecSec",trackingDirName.Data()));
  fhMomAccAll  = (TH1F*)occuF->Get(Form("%s/hMomAccAll",trackingDirName.Data()));
  fhMomAccPrim = (TH1F*)occuF->Get(Form("%s/hMomAccPrim",trackingDirName.Data()));
  fhMomAccSec  = (TH1F*)occuF->Get(Form("%s/hMomAccSec",trackingDirName.Data()));
  fhHitPointCorrelation[10];
  for ( Int_t ist = 0 ; ist < nofStations ; ist++ ) 
    fhHitPointCorrelation[ist] = (TH2F*)occuF->Get(Form("%s/Station%i/hHitPointCorrelation%i",directoryName.Data(),ist+1,ist+1));
  fhMomResAll  = (TH2F*)occuF->Get(Form("%s/hMomResAll",trackingDirName.Data()));
  fhMomResPrim = (TH2F*)occuF->Get(Form("%s/hMomResPrim",trackingDirName.Data()));
//  fhMomResPrimClone = (TH2F*)fhMomResPrim->Clone();
  fhNhGhosts   = (TH1F*)occuF->Get(Form("%s/hNhGhosts",trackingDirName.Data()));
  fhNhClones   = (TH1F*)occuF->Get(Form("%s/hNhClones",trackingDirName.Data()));
  fhRefTracks  = (TH1F*)occuF->Get(Form("%s/hRefTracks",trackingDirName.Data()));
  fhRecRefTracks = (TH1F*)occuF->Get(Form("%s/hRecRefTracks",trackingDirName.Data()));
  
  fNEvents    = fhRefTracks->GetEntries();
  //  fNStsTracks = fhMomAccAll->GetEntries();
  //  fNStsTracks = fhMomAccPrim->GetEntries();
  fNStsTracks  = fhMomRecAll ->GetEntries();
  fNPrimTracks = fhMomRecPrim->GetEntries();
  fNSecTracks  = fhMomRecSec->GetEntries();
  fNGhosts    = fhNhGhosts->GetEntries();
  fNClones    = fhNhClones->GetEntries();

  TF1* allEffFit = new TF1 ("allEffFit","pol0",1.,10.);
  fhMomEffAll->Fit(allEffFit,"QN","",1,10);
  effAll = fhMomRecAll->Integral()/fhMomAccAll->Integral();
  allEff = allEffFit->GetParameter(0);
  TF1* primEffFit = new TF1 ("primEffFit","pol0",1.,10.);
  fhMomEffPrim->Fit(primEffFit,"QN","",1,10);
  effPrim = fhMomRecPrim->Integral()/fhMomAccPrim->Integral();
  primEff = primEffFit->GetParameter(0);
  TF1* secEffFit = new TF1 ("secEffFit","pol0",1.,10.);
  fhMomEffSec->Fit(secEffFit,"QN","",1,10);
  effSec = fhMomRecSec->Integral()/fhMomAccSec->Integral();
  secEff = secEffFit->GetParameter(0);
  effRef = fhRecRefTracks->Integral()/fhRefTracks->Integral();
  
  TF1*  momentumResFuncPrim = new TF1("momentumResFuncPrim","gaus",-10.,10.);
  TH1F* momentumResHistPrim = (TH1F*)fhMomResPrim->ProjectionY("momentumResHistPrim");
  momentumResHistPrim->Fit(momentumResFuncPrim,"QN","",-10.,10.);
  momentumResolutionPrim = momentumResFuncPrim->GetParameter(2);
  TF1*  momentumResFuncAll = new TF1("momentumResFuncAll","gaus",-10.,10.);
  TH1F* momentumResHistAll = (TH1F*)fhMomResAll->ProjectionY("momentumResHistAll");
  momentumResHistAll->Fit(momentumResFuncAll,"QN","",-10.,10.);
  momentumResolutionAll = momentumResFuncAll->GetParameter(2);

  qaRead = kTRUE;
  
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void MyMainFrame::ReadELoss() {

  if ( !fGeoRead ) {
    cout << " Geometry not read" << endl;
    return;
  }

  fMaxELoss = 0.;

  TFile* elossF = TFile::Open(qasimFileName.Data());   

  if ( !elossF )
    { cout << "sorry, no file" << endl; return; }

  Double_t stationThickness = 0.0400; // in cm
  Double_t siliconDensity = 2.329e-3; // in kg/cm3

  nofStations = 8;
  for ( Int_t iwst = 0 ; iwst < nofStations ; iwst++ ) {
    fhELoss[iwst] = (TH2F*)elossF->Get(Form("STSSimulationQA/STSOccupancy/hEnergyLossSt%i",iwst+1));
    
    fhELoss[iwst]->SetXTitle("x [cm]");
    fhELoss[iwst]->SetYTitle("y [cm]");

    fhMrad[iwst] = (TH2F*)fhELoss[iwst]->Clone();      
    fhMrad[iwst]->SetTitle(Form("%s, z=%dcm",fhMrad[iwst]->GetTitle(),(Int_t)(station->GetZ())));

    fhMrad[iwst]->Scale(1./fNofMCEvents);       // for 1 event
    fhMrad[iwst]->Scale(1./stationThickness); // for GeV/cm3 energy loss (each bin is cm2)
    fhMrad[iwst]->Scale(1./siliconDensity);   // for GeV/kg energy loss
    fhMrad[iwst]->Scale(1.e9);                // for eV/kg energy loss
    fhMrad[iwst]->Scale(1.6e-19);             // for Gy=J/kg energy loss
    fhMrad[iwst]->Scale(1.e2);                // for rad=.01Gy energy loss
    fhMrad[iwst]->Scale(1.e-6);               // for Mrad energy loss

    fhMrad[iwst]->Scale(.25);                 // going from central to mbias
    fhMrad[iwst]->Scale(1.e7);                // going for fluence per second
    fhMrad[iwst]->Scale(5.26e6);              // going for fluence per 2 months of const. running
    fhMrad[iwst]->Scale(6.);                  // six years of running at this luminocity/rate

    fhMips[iwst] = (TH2F*)fhMrad[iwst]->Clone();
    fhMips[iwst]->Scale(1.47e13);             // for mips

    if ( fMaxELoss < fhELoss[iwst]->GetMaximum() ) {
      fMaxELoss = fhELoss[iwst]->GetMaximum();
      fMaxMrad = fhMrad[iwst]->GetMaximum();
      fMaxMips = fhMips[iwst]->GetMaximum();
    }
  }
  cout << "maximum energy loss = " << fMaxELoss << endl;  
  cout << "maximum Mrad        = " << fMaxMrad << endl;  
  cout << "maximum mips        = " << fMaxMips << endl;  

  qasimRead = kTRUE;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void MyMainFrame::HECa(Int_t event, Int_t x, Int_t y,
		       TObject *sel) {
  TString selName = sel->GetName();
  Int_t isect;
  Int_t isens;
  
  if ( selName.Contains("Sector") ) {
    //    cout << "selName = " << selName.Data() << endl;
    TString sectorNr = selName;
    if ( selName.Contains("Back") ) sectorNr.Remove(sectorNr.Length()-4,4);
    sectorNr.Remove(0,6);
    sectorNr.Remove(3,9);
    //    cout << selName.Data() << " -> " << sectorNr.Data() << endl;
    isect = sectorNr.Atoi()-1;
    TString sensorNr = selName;
    if ( selName.Contains("Back") ) sensorNr.Remove(sensorNr.Length()-4,4);
    sensorNr.Remove(0,15);
    isens = sensorNr.Atoi()-1;
    //    cout << " -> sector " << isect << " sensor " << isens << endl;
    //  cout << "sensor = " << secNo << " in sector " << isector << endl;
    if (event == kButton1Down) {
      if ( qaRead && fSelView != 0 ) {
	TCanvas* canvas = new TCanvas("sensor","sensor",10,10,600,400);
	if ( fSelView == 1 ) {
	  canvas->SetLogy();
	  fhNofHits [fSelStation][isect][isens]->SetAxisRange(-0.5,400.5,"X");
	  fhNofHits [fSelStation][isect][isens]->Draw();
	}
	if ( fSelView == 2 ) { 
	  canvas->SetLogy();
	  fhFNofDigis[fSelStation][isect]->SetAxisRange(-0.5,100.5,"X");
	  fhFNofDigis[fSelStation][isect]->Draw();
	  fhBNofDigis[fSelStation][isect]->SetLineColor(2);
	  fhBNofDigis[fSelStation][isect]->Draw("same");

	}
	if ( fSelView == 3 ) { 
	  canvas->SetLogy();
	  fhFClusterL[fSelStation][isect]->SetAxisRange(-0.5,20.5,"X");
	  fhFClusterL[fSelStation][isect]->Draw();
	  fhBClusterL[fSelStation][isect]->SetLineColor(2);
	  fhBClusterL[fSelStation][isect]->Draw("same");
	}
	if ( fSelView == 4 ) { 
	  for (Int_t chip=1; chip<9; chip++){
	    canvas->SetLogy();
	    fhFNofDigisPerChip[fSelStation][isect][chip]->SetAxisRange(-0.5,100.5,"X");
	    fhFNofDigisPerChip[fSelStation][isect][chip]->Draw();
	    fhFNofDigisPerChip[fSelStation][isect][chip]->SetLineColor(2);
	    fhFNofDigisPerChip[fSelStation][isect][chip]->Draw("same");
	  }
	}
      }
    }
  }
  
  if (event == kMouseMotion){
    if ( selName.Contains("Sector") ) {
      CbmStsStation* station = (CbmStsStation*)fStations->At(fSelStation);
      CbmStsSector*  sector  = station->GetSector(isect);
      CbmStsSensor*  sensor  = sector ->GetSensor(isens);
      fStatusBar->SetText(selName.Data(),1);
      fStatusBar->SetText(Form("%.2f / %.2f / %.2f || %.2f X %.2f = %.2f || %d str.",
			       sensor->GetX0(),
			       sensor->GetY0(),
			       sensor->GetZ0(),
			       sensor->GetLx(),
			       sensor->GetLy(),
			       fSensorArea        [fSelStation][isect][isens],
			       fSectorNChannels   [fSelStation][isect]),2);
      fStatusBar->SetText("",3);
      fStatusBar->SetText("",4);
      fStatusBar->SetText("",5);
      fStatusBar->SetText("",6);
      if ( fSelView > 0 ) {
	fStatusBar->SetText(Form("%.2f hit/cm^2",
				 fhNofHits[fSelStation][isect][isens]->GetMean()/fSensorArea[fSelStation][isect][isens]),3);
	if ( !selName.Contains("Back") ) {
	  fStatusBar->SetText(Form("occ = %.2f %%",
				   100.*fhFNofDigis[fSelStation][isect]->GetMean()/((Float_t)fSectorNChannels[fSelStation][isect])),4);
	  fStatusBar->SetText(Form("clu %.2f ch.",
				   fhFClusterL[fSelStation][isect]->GetMean()),5);
	}
	if ( selName.Contains("Back") ) {
	  fStatusBar->SetText(Form("occ = %.2f %%",
				   100.*fhBNofDigis[fSelStation][isect]->GetMean()/((Float_t)fSectorNChannels[fSelStation][isect])),4);
	  fStatusBar->SetText(Form("clu %.2f ch.",
				   fhBClusterL[fSelStation][isect]->GetMean()),5);
	}

	fStatusBar->SetText(Form("hfe = %.2f ch.",
				 fhHitFindingEffComb->GetBinContent(((fSelStation+1)<<16|(isect+1)<<4|(isens+1)<<1)+1)),6);
      }
    }
    else {
      fStatusBar->SetText("",1);
      fStatusBar->SetText("",2);
      fStatusBar->SetText("",3);
      fStatusBar->SetText("",4);
      fStatusBar->SetText("",5);
      fStatusBar->SetText("",6);
    }
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void MyMainFrame::SaveCanvas() {
  TCanvas *fCanvas = fEcanvas->GetCanvas();

  TString saveToFile = Form("%s.png ",beingDrawn.Data());
  cout << "will save canvas as \"" << saveToFile.Data() << "\"" << endl;

  fCanvas->SaveAs(saveToFile.Data());

}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
MyMainFrame::~MyMainFrame() {
  // Clean up used widgets: frames, buttons, layouthints
  fMain->Cleanup();
  delete fMain;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void Occupancy() {
  // Popup the GUI...
  new MyMainFrame(gClient->GetRoot(),800,1000);
}
//-----------------------------------------------------------------------------
