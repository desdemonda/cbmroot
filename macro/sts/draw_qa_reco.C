void draw_qa_reco(TString inFileName="reco.qa.root")
{
  gStyle->SetPalette(1);

  Int_t fShowStation1 = 1;
  Int_t fShowStation2 = 5;
  Int_t nofStations = 6;

  TFile* inFile = new TFile(inFileName.Data());
  TH1F* fhMomEffAll  = (TH1F*)inFile->Get("STSReconstructionQA/hMomEffAll");
  TH1F* fhMomEffPrim = (TH1F*)inFile->Get("STSReconstructionQA/hMomEffPrim");
  TH1F* fhMomEffSec  = (TH1F*)inFile->Get("STSReconstructionQA/hMomEffSec");
  TH1F* fhMomRecAll  = (TH1F*)inFile->Get("STSReconstructionQA/hMomRecAll");
  TH1F* fhMomRecPrim = (TH1F*)inFile->Get("STSReconstructionQA/hMomRecPrim");
  TH1F* fhMomRecSec  = (TH1F*)inFile->Get("STSReconstructionQA/hMomRecSec");
  TH1F* fhMomAccAll  = (TH1F*)inFile->Get("STSReconstructionQA/hMomAccAll");
  TH1F* fhMomAccPrim = (TH1F*)inFile->Get("STSReconstructionQA/hMomAccPrim");
  TH1F* fhMomAccSec  = (TH1F*)inFile->Get("STSReconstructionQA/hMomAccSec");
  TH2F* fhHitPointCorrelation[10];
  for ( Int_t ist = 0 ; ist < nofStations ; ist++ ) 
    fhHitPointCorrelation[ist] = (TH2F*)inFile->Get(Form("STSReconstructionQA/hHitPointCorrelation%i",ist+1));
  TH2F* fhMomResAll  = (TH2F*)inFile->Get("STSReconstructionQA/hMomResAll");
  TH2F* fhMomResPrim = (TH2F*)inFile->Get("STSReconstructionQA/hMomResPrim");
  TH1F* fhNhGhosts   = (TH1F*)inFile->Get("STSReconstructionQA/hNhGhosts");
  TH1F* fhNhClones   = (TH1F*)inFile->Get("STSReconstructionQA/hNhClones");
  TH1F* fhRefTracks  = (TH1F*)inFile->Get("STSReconstructionQA/hRefTracks");
  TH1F* fhRecRefTracks = (TH1F*)inFile->Get("STSReconstructionQA/hRecRefTracks");

  Int_t fNEvents    = fhRefTracks->GetEntries();
  //  Int_t fNStsTracks = fhMomAccAll->GetEntries();
  //  Int_t fNStsTracks = fhMomAccPrim->GetEntries();
  Int_t fNStsTracks  = fhMomRecAll ->GetEntries();
  Int_t fNPrimTracks = fhMomRecPrim->GetEntries();
  Int_t fNSecTracks  = fhMomRecSec->GetEntries();
  Int_t fNGhosts    = fhNhGhosts->GetEntries();
  Int_t fNClones    = fhNhClones->GetEntries();

  TCanvas* recoCanvas = new TCanvas("StsRecoCanvas","Sts reconstruction",10,10,600,900);
  TPad* recoPad[10];

  recoPad[0] = new TPad("titlePad",   "Title pad"                     ,0.00,0.90,1.00,1.00);
  recoPad[1] = new TPad("efficiencyPad","Efficiency pad"              ,0.00,0.60,0.40,0.90);
  recoPad[2] = new TPad("resolutionPad","Momentum resolution pad"     ,0.00,0.30,0.40,0.60);
  recoPad[7] = new TPad("ghostsPad"    ,"Ghosts nof hits pad"         ,0.00,0.00,0.40,0.30);
  recoPad[3] = new TPad("hitVspoint3Pad","Hit vs point on st.3 pad"   ,0.40,0.65,0.70,0.90);
  recoPad[4] = new TPad("hitVspoint6Pad","Hit vs point on st.6 pad"   ,0.70,0.65,1.00,0.90);
  recoPad[5] = new TPad("hitVspntXY3Pad","Hit vs point XY on st.3 pad",0.40,0.40,0.70,0.65);
  recoPad[6] = new TPad("hitVspntXY6Pad","Hit vs point XY on st.6 pad",0.70,0.40,1.00,0.65);
  recoPad[8] = new TPad("printoutPad","Print information pad"         ,0.40,0.00,1.00,0.40);
  recoPad[2]->SetLogz();
  for ( Int_t ipad = 0 ; ipad < 9 ; ipad++ ) {
    recoPad[ipad]->SetFillColor(0);
    recoPad[ipad]->SetBorderMode(0);
    recoPad[ipad]->Draw();
  }
  
  recoPad[0]->cd();
  TLegend* brp = new TLegend(0.1,0.1,0.9,0.9,"STS reconstruction");
  brp->SetTextAlign(22);
  brp->SetTextSize(0.6);
  brp->SetTextColor(1);
  brp->SetBorderSize(0);
  brp->SetFillColor(0);
  brp->Draw();
  recoPad[0]->Update();

  recoPad[1]->cd();
  fhMomEffAll->SetAxisRange(0.,1.1,"Y");
  fhMomEffAll->SetLineWidth(2);
  fhMomEffAll->SetLineColor(1);
  fhMomEffAll->SetTitle("Efficiency");
  fhMomEffAll->Draw();
  fhMomEffPrim->SetLineWidth(2);
  fhMomEffPrim->SetLineColor(2);
  fhMomEffPrim->Draw("same");
  fhMomEffSec->SetLineWidth(2);
  fhMomEffSec->SetLineColor(3);
  fhMomEffSec->Draw("same");
  TLegend* effLeg = new TLegend(0.3,0.15,0.48,0.4);
  effLeg->SetBorderSize(0);
  effLeg->SetFillColor(0);
  effLeg->AddEntry(fhMomEffAll, "all" ,"pl");
  effLeg->AddEntry(fhMomEffPrim,"prim","pl");
  effLeg->AddEntry(fhMomEffSec, "sec" ,"pl");
  effLeg->Draw();
  TLine* oneLine = new TLine(0.0,1.0,10.0,1.0);
  oneLine->SetLineStyle(2);
  oneLine->Draw();
  recoPad[1]->Update();
  recoPad[2]->cd();
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
  fhHigBand->SetMarkerSize(0.1);
  fhHigBand->SetLineWidth(2);
  fhLowBand->Draw("Psame");
  fhHigBand->Draw("Psame");
  recoPad[2]->Update();
  recoPad[7]->cd();
  fhNhGhosts->Draw();
  recoPad[7]->Update();

  TH1F* projX[10];
  TH1F* projY[10];
  TF1*   fitX[10];
  TF1*   fitY[10];
  Double_t resolution[2][10];

  for ( Int_t ist = 0 ; ist < nofStations ; ist++ ) {
    projX[ist] = (TH1F*)fhHitPointCorrelation[ist]->ProjectionX(Form("projX%i",ist+1));
    projY[ist] = (TH1F*)fhHitPointCorrelation[ist]->ProjectionY(Form("projY%i",ist+1));
    fitX[ist] = new TF1(Form("fitX%i",ist+1),"gaus",-0.02,0.02);
    fitY[ist] = new TF1(Form("fitY%i",ist+1),"gaus",-0.02,0.02);
    projX[ist]->SetAxisRange(-0.02,0.02,"X");
    projY[ist]->SetAxisRange(-0.02,0.02,"X");
    projX[ist]->Fit(fitX[ist],"QN","",-0.02,0.02);
    projY[ist]->Fit(fitY[ist],"QN","",-0.02,0.02);
    resolution[0][ist] = 10000.*fitX[ist]->GetParameter(2);
    resolution[1][ist] = 10000.*fitY[ist]->GetParameter(2);
  }
  
  recoPad[3]->cd();
  fhHitPointCorrelation[fShowStation1]->Draw("col");
  recoPad[3]->Update();
  recoPad[4]->cd();
  fhHitPointCorrelation[fShowStation2]->Draw("col");
  recoPad[4]->Update();
  
  recoPad[5]->cd();
  projX[fShowStation1]->SetLineWidth(2);
  projY[fShowStation1]->SetLineWidth(2);
  projX[fShowStation1]->SetLineColor(2);
  projY[fShowStation1]->SetLineColor(3);
  projY[fShowStation1]->SetXTitle("#Delta x, y [cm]");
  projX[fShowStation1]->Draw();
  projY[fShowStation1]->Draw("same");
  //  fitY[fShowStation1]->Draw("same");
  //  fitX[fShowStation1]->Draw("same");
  TLegend* legend1 = new TLegend(0.55,0.6,1.0,0.94);
  legend1->SetBorderSize(0);
  legend1->SetFillColor(0);
  legend1->AddEntry(projX[fShowStation1],
		    Form("X,#sigma=%3.2f#mum",resolution[0][fShowStation1]),"l");
  legend1->AddEntry(projY[fShowStation1],
		    Form("Y,#sigma=%3.2f#mum",resolution[1][fShowStation1]),"l");
  legend1->Draw();
  recoPad[5]->Update();

  recoPad[6]->cd();
  projX[fShowStation2]->SetLineWidth(2);
  projY[fShowStation2]->SetLineWidth(2);
  projX[fShowStation2]->SetLineColor(2);
  projY[fShowStation2]->SetLineColor(3);
  projX[fShowStation2]->SetXTitle("#Delta x, y [cm]");
  projX[fShowStation2]->Draw();
  projY[fShowStation2]->Draw("same");
  //  fitY[fShowStation2]->Draw("same");
  //  fitX[fShowStation2]->Draw("same");
  TLegend* legend2 = new TLegend(0.55,0.6,1.0,0.94);
  legend2->SetBorderSize(0);
  legend2->SetFillColor(0);
  legend2->AddEntry(projX[fShowStation2],
		    Form("X,#sigma=%3.2f#mum",resolution[0][fShowStation2]),"l");
  legend2->AddEntry(projY[fShowStation2],
		    Form("Y,#sigma=%3.2f#mum",resolution[1][fShowStation2]),"l");
  legend2->Draw();
  recoPad[6]->Update();
  
  recoPad[3]->SetLogz();
  recoPad[4]->SetLogz();

  TF1* allEffFit = new TF1 ("allEffFit","pol0",1.,10.);
  fhMomEffAll->Fit(allEffFit,"QN","",1,10);
  Double_t effAll = fhMomRecAll->Integral()/fhMomAccAll->Integral();
  Double_t allEff = allEffFit->GetParameter(0);
  TF1* primEffFit = new TF1 ("primEffFit","pol0",1.,10.);
  fhMomEffPrim->Fit(primEffFit,"QN","",1,10);
  Double_t effPrim = fhMomRecPrim->Integral()/fhMomAccPrim->Integral();
  Double_t primEff = primEffFit->GetParameter(0);
  TF1* secEffFit = new TF1 ("secEffFit","pol0",1.,10.);
  fhMomEffSec->Fit(secEffFit,"QN","",1,10);
  Double_t effSec = fhMomRecSec->Integral()/fhMomAccSec->Integral();
  Double_t secEff = secEffFit->GetParameter(0);
  Double_t effRef = fhRecRefTracks->Integral()/fhRefTracks->Integral();

  TF1*  momentumResFuncPrim = new TF1("momentumResFuncPrim","gaus",-10.,10.);
  TH1F* momentumResHistPrim = (TH1F*)fhMomResPrim->ProjectionY("momentumResHistPrim");
  momentumResHistPrim->Fit(momentumResFuncPrim,"QN","",-10.,10.);
  Double_t momentumResolutionPrim = momentumResFuncPrim->GetParameter(2);
  TF1*  momentumResFuncAll = new TF1("momentumResFuncAll","gaus",-10.,10.);
  TH1F* momentumResHistAll = (TH1F*)fhMomResAll->ProjectionY("momentumResHistAll");
  momentumResHistAll->Fit(momentumResFuncAll,"QN","",-10.,10.);
  Double_t momentumResolutionAll = momentumResFuncAll->GetParameter(2);

  recoPad[8]->cd();
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
  for ( Int_t ist = 0 ; ist < nofStations ; ist++ )
    if ( resolution[0][ist] > 0.01 )
      printoutPave->AddText(Form("st#%i,#sigma_{x}=%3.2f#mum,#sigma_{y}=%3.2f#mum",
				 ist+1,resolution[0][ist],resolution[1][ist]));
  printoutPave->AddText("Tracking efficiencies (p>1.0 GeV/c):");
  printoutPave->AddText(Form("all = %2.2f%%(%2.2f%%)",100.*effAll,100.*allEff));
  printoutPave->AddText(Form("vertex = %2.2f%%(%2.2f%%)",100.*effPrim,100.*primEff));
  printoutPave->AddText(Form("reference = %2.2f%%",100.*effRef));
  printoutPave->AddText(Form("non-vertex = %2.2f%%(%2.2f%%)",100.*effSec,100.*secEff));
  printoutPave->AddText(Form("Momentum resolution = %3.2f%%(%3.2f%%)",momentumResolutionAll,momentumResolutionPrim));
  recoPad[8]->Clear();
  printoutPave->Draw();
  recoPad[8]->Update();
}
