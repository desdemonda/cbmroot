void draw_qa_sim(TString inFileName="sim.qa.root")
{
  gStyle->SetPalette(1);

  TFile* inFile = new TFile(inFileName.Data());

  TH1F* fhNofEvents = (TH1F*)inFile->Get("STSSimulationQA/hNofEvents");
  TH1F* fhNofStsStations = (TH1F*)inFile->Get("STSSimulationQA/hNofStsStations");

  Int_t nofStations = fhNofStsStations->GetBinContent(1);
  Int_t fShowStation = 4;
  Int_t fNEvents = fhNofEvents->GetBinContent(1);

  TH1F* fhMomAll  = (TH1F*)inFile->Get("STSSimulationQA/hMomAll");
  TH1F* fhMomRec  = (TH1F*)inFile->Get("STSSimulationQA/hMomRec");
  TH2F* fhYPtMapRec  = (TH2F*)inFile->Get("STSSimulationQA/hYPtMapRec");
  TH1F* fhStsPointsRec  = (TH1F*)inFile->Get("STSSimulationQA/hStsPointsRec");
  TH3F* fhStsPointsPosition  = (TH3F*)inFile->Get("STSSimulationQA/hStsPointsPosition");
  TH2F* fhStationPoints[10];
  for ( Int_t ist = 0 ; ist < nofStations ; ist++ ) 
    fhStationPoints[ist] = (TH2F*)inFile->Get(Form("STSSimulationQA/hStationPoints%i",ist+1));

  Double_t tracksPerEvent = (Double_t)(fhMomAll->GetEntries())/((Double_t)fNEvents);
  Double_t pointsPerEvent = (Double_t)(fhStsPointsPosition->GetEntries())/((Double_t)fNEvents);


  TCanvas* simCanvas = new TCanvas("StsSim","Sts simulation",10,10,600,900);
  TPad* simPad[10];
  simPad[0] = new TPad("titlePad",   "Title pad"                ,0.01,0.91,0.99,0.99);
  simPad[1] = new TPad("momentumPad","Momentum pad"             ,0.01,0.46,0.49,0.89);
  simPad[2] = new TPad("yptPad",     "y-pt pad"                 ,0.01,0.01,0.49,0.44);
  simPad[3] = new TPad("pointPad",   "Points per track pad"     ,0.51,0.61,0.99,0.89);
  simPad[4] = new TPad("stationPad", "Points on 1st station pad",0.51,0.31,0.99,0.59);
  simPad[5] = new TPad("printoutPad","Print information pad    ",0.51,0.01,0.99,0.29);
  simPad[1]->SetLogy();
  for ( Int_t ipad = 0 ; ipad < 6 ; ipad++ ) {
    simPad[ipad]->SetFillColor(0);
    simPad[ipad]->SetBorderMode(0);
    simPad[ipad]->Draw();
  }
  
  simPad[0]->cd();
  TLegend* brp = new TLegend(0.1,0.1,0.9,0.9,"STS simulation");
  brp->SetTextAlign(22);
  brp->SetTextSize(0.6);
  brp->SetTextColor(1);
  brp->SetBorderSize(0);
  brp->SetFillColor(0);
  brp->Draw();
  simPad[0]->Update();
  
  simPad[1]->cd();
  //  fhMomAll->Draw();
  fhMomRec->Draw();
  simPad[1]->Update();
  simPad[2]->cd();
  fhYPtMapRec->Draw("colz");
  simPad[2]->Update();
  simPad[3]->cd();
  fhStsPointsRec->Draw();
  simPad[3]->Update();
  
  simPad[4]->cd();
  fhStationPoints[0]->Draw();
  fhStationPoints[0]->Draw("colz");
  simPad[4]->Update();
  
  simPad[5]->cd();
  TPaveText* printoutPave = new TPaveText(0.1,0.1,0.9,0.9);
  printoutPave->SetTextAlign(22);
  printoutPave->SetTextSize(0.1);
  printoutPave->SetTextColor(1);
  printoutPave->SetBorderSize(0);
  printoutPave->SetFillColor(0);
  printoutPave->AddText(Form("%i events",fNEvents));
  printoutPave->AddText(Form("tracks/event = %3.2f",tracksPerEvent));
  printoutPave->AddText(Form("points/event = %3.2f",pointsPerEvent));
  simPad[5]->Clear();
  printoutPave->Draw();
  simPad[5]->Update();
}
