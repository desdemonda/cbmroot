/***************************************************************
* $Id: CbmRichTestHits.C,v 1.3 2006/06/22 07:18:07 hoehne Exp $
*
* This macro gives typical plots for RICH HitProducer
* in order to make a fast check whether the Rich HitProducer
* ran correctly
*
* C. Hoehne (c.hoehne@gsi.de)
*
*
**************************************************************/


{

// ========================================================================
  //          Adjust this part according to your requirements

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;

  // Number of events to process
  Int_t nEvents = 10;

  // Input file (MC events)
  TString inFile = "../run/data/test.mc.root";
  
  // Input file (Hits or reconstructed data)
  TString hitFile = "../run/data/test.eds.root";

  // Parameter file
  TString parFile = "../run/data/params.root";

  // Output file
  TString outFile = "TestRichHits.root";

  // Output file with most important histograms and numbers for printout
  TString psFile = "TestRichHits.ps";

  // In general, the following parts need not be touched
  // ========================================================================

  // timer
  TStopwatch timer;
  timer.Start();

    // ----  Load libraries   -------------------------------------------------
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
  gSystem->Load("libMvd");
  gSystem->Load("libSts");
  gSystem->Load("libRich");
  gSystem->Load("libTrd");
  gSystem->Load("libTof");
  gSystem->Load("libEcal");
  gSystem->Load("libGlobal");
  gSystem->Load("libKF");
  gSystem->Load("libL1");
  // ------------------------------------------------------------------------


  FairRunAna *fRun = new FairRunAna();
  fRun->SetInputFile(inFile);
  //fRun->AddFile(inFile2);
  fRun->AddFriend(hitFile);
  fRun->SetOutputFile(outFile);

  FairRuntimeDb *rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo *io1 = new FairParRootFileIo();
  io1->open(parFile.Data());
  rtdb->setFirstInput(io1);

  // ---- define TestHits task to run ---------
  CbmRichTestHits *tsim= new CbmRichTestHits("RichTestHits","RichTestHits",iVerbose);
  fRun->AddTask(tsim);

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0,nEvents);

  delete tsim;

  //------------ draw Histos, save plot ------------------------------------------------

  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0000);
  gStyle->SetPalette(1,0);

  gStyle->SetPadBorderMode(0);
//  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  //gStyle->SetStatW(0.3);
  //gStyle->SetStatH(0.01);
  gStyle->SetTitleW(0.);
  gStyle->SetStatColor(10);
  gStyle->SetStatBorderSize(1);
  gStyle->SetStatX(0.9);
  gStyle->SetStatY(0.90);
  gStyle->SetTitleOffset(0.5,"x");
  gStyle->SetTitleOffset(0.5,"y");
  gStyle->SetTitleOffset(0.5,"z");
  gSystem->Load("libPostscript.so");

//  TLatex *t1 = new TLatex(220,7.2,"#frac{dN}{d#lambda}=#frac{2#pi#alpha}{#lambda ^{2}} (1-#frac{1}{#beta^{2}n^{2}}) L");
//  t1->Draw();


  TCanvas *c1 = new TCanvas("c1","Canvas",768,1024);
  ps = new TPostScript(psFile,-111);
//  ps->Range(768,1024);

  c1-> SetBottomMargin(0.11);
  c1-> SetTopMargin(0.11);
  c1->SetTicks(0,0);
  c1->SetFillColor(10);
  c1->SetGrid(0,0);

  TPaveLabel *title = new TPaveLabel(0.1,0.92,0.9,0.98,"Test RICH HitProducer for file  " + hitFile);
  title->SetFillColor(10);
  title->SetLineStyle(1);
  title->Draw();

  TPad *pad1 = new TPad("pad1","This is pad1",0.02,0.6,0.48,0.9);
  TPad *pad2 = new TPad("pad2","This is pad2",0.52,0.6,0.98,0.9);
  TPad *pad3 = new TPad("pad3","This is pad3",0.02,0.3,0.48,0.6);
  TPad *pad4 = new TPad("pad4","This is pad4",0.52,0.3,0.98,0.6);
  TPad *pad5 = new TPad("pad5","This is pad5",0.02,0.0,0.48,0.3);
  TPad *pad6 = new TPad("pad6","This is pad6",0.52,0.0,0.98,0.3);

  pad1->Draw();
  pad2->Draw();
  pad3->Draw();
  pad4->Draw();
  pad5->Draw();
  pad6->Draw();

  pad1->cd();
  gPad->SetFillColor(10);
  fh_Det1ev->GetXaxis()->SetTitle("x [cm]");
  fh_Det1ev->GetYaxis()->SetTitle("y [cm]");
  fh_Det1ev->Draw();

  pad2->cd();
  gPad->SetFillColor(10);
  pad2->SetLogz();
  fh_Detall->GetXaxis()->SetTitle("x [cm]");
  fh_Detall->GetYaxis()->SetTitle("y [cm]");
  fh_Detall->Draw("colz");
  
  pad3->cd();
  gPad->SetFillColor(10);
  pad3->SetLogz();
  fh_Detall_zoom->GetXaxis()->SetTitle("x [cm]");
  fh_Detall_zoom->GetYaxis()->SetTitle("y [cm]");
  fh_Detall_zoom->Draw("colz");

  pad4->cd();
  gPad->SetFillColor(0);
  Double_t ymax = fh_Nhits->GetMaximum();
  fh_Nhits->GetXaxis()->SetTitle("Nhits");
  fh_Nhits->Fit("gaus","Q","R",0,60);             // Protvino
  //fh_Nhits->Fit("gaus","Q","R",10,30);             // CsI
  //fh_Nhits->Fit("gaus","Q","R",6,20);                // Hamamatsu
  fh_Nhits->Draw("same");
  fh_Nhits->GetFunction("gaus");
  Double_t par[3];
  gaus->GetParameters(par);

  char t1[300];
  Double_t y=ymax*0.95;
  sprintf(t1,"\t Mean number of hits/rings %3.2f\n",par[1]);
  TLatex *t2 = new TLatex(2,y,t1);
  t2->Draw();
  sprintf(t1,"\t Sigma                         %3.2f\n",par[2]);
  TLatex *t2 = new TLatex(2,y*0.9,t1);
  t2->Draw();
  

  pad5->cd();
  gPad->SetFillColor(10);
  pad5->SetLogz();
  fh_n_vs_p->GetXaxis()->SetTitle("p [GeV/c]");
  fh_n_vs_p->GetYaxis()->SetTitle("Nhits");
  fh_n_vs_p->Draw("colz");

  pad6->cd();
  gPad->SetFillColor(10);

  Double_t Nall    = fh_Nall    ->GetMean();
  Double_t Nel     = fh_Nel     ->GetMean();
  Double_t Nelprim = fh_Nelprim ->GetMean();
  Double_t Npi     = fh_Npi     ->GetMean();
  Double_t Nk      = fh_Nk      ->GetMean();
  Double_t Nhad    = fh_Nhad    ->GetMean();
  Double_t Nnoise  = fh_Nnoise  ->GetMean();

  TPaveText *info = new TPaveText(0.1,0.1,0.9,0.9);
  info->SetFillColor(10);
  info->SetBorderSize(0);
  info->SetTextAlign(1);
  info->Draw();

  char t1[300];
  sprintf(t1,"\t Mean number of rings/event for ");
  info->AddText(0.05,0.9,t1);
  sprintf(t1,"\t  all                             %3.2f\n",Nall);
  info->AddText(0.05,0.7,t1);
  sprintf(t1,"\t  electrons                   %3.2f\n",Nel);
  info->AddText(0.05,0.6,t1);
  sprintf(t1,"\t  electrons (STS>6)     %3.2f\n",Nelprim);
  info->AddText(0.05,0.5,t1);
  sprintf(t1,"\t  pions                          %3.2f\n",Npi);
  info->AddText(0.05,0.4,t1);
  sprintf(t1,"\t  Kaons                          %3.2f\n",Nk);
  info->AddText(0.05,0.3,t1);
  sprintf(t1,"\t  Mean number of crossing hadrons     %3.2f\n",Nhad);
  info->AddText(0.05,0.1,t1);
  sprintf(t1,"\t  Mean number of noise hits     %3.2f\n",Nnoise);
  info->AddText(0.05,0.,t1);
  info->Draw();

  ps->Close();



  // ------------------------------------------------------------------------

  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------



}// macro ends here
