void DrawParticlesFitQA(int iParticle = 0) {

  TStopwatch timer;
  timer.Start();

//  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
//  basiclibs();

  TStyle *plain = new TStyle("Plain","Plain Style(no colors/fill areas)");
  //plain->SetCanvasBorderMode(0);
  //plain->SetPadBorderMode(0);
  plain->SetPadColor(0);
  plain->SetCanvasColor(0);
  plain->SetTitleColor(0);
  plain->SetStatColor(0);
  plain->SetOptStat(110010);
  plain->SetOptFit(0011);
  plain->SetStatW(0.225);
  plain->SetStatH(0.06);
  //plain->SetStatX(0.7);
  plain->SetOptTitle(0);
  plain->cd();

  TString name = "CbmKFParticleFinderQA.root";
  //TFile *dir = new TFile(name.Data(),"read");
  TFile *f = new TFile(name.Data(),"read");
  
  const int NParameters = 8;
  string ParNames[NParameters] = {"X","Y","Z","Px","Py","Pz","E","M"};
  
  KFPartEfficiencies eff;
  TString particleTitle = eff.partName[iParticle];
  
  TString chiDirName = "KFTopoReconstructor/KFParticlesFinder/Particles/";
          chiDirName += particleTitle;
          chiDirName += "/Parameters/Signal";
  TString dirName[2] = {"KFTopoReconstructor/KFParticlesFinder/Particles/",
                        "KFTopoReconstructor/KFParticlesFinder/Particles/"};
          dirName[0] += particleTitle + "/FitQA";
          dirName[1] += particleTitle + "/DaughtersQA";
  TString outname[2] = {particleTitle, particleTitle+TString("Daughters")};
  for(int iDir=0; iDir<2; iDir++)
  {
    TString fullDirPath = "";
    fullDirPath += dirName[iDir];
    TDirectory *dir = f->GetDirectory(fullDirPath);
    
    std::cout << "Fit of plots form:" << std::endl << fullDirPath.Data() << std::endl;
    
    
    TH1D *hRes[NParameters];
    TH1D *hPull[NParameters];
    TF1  *fRes[NParameters];
    TF1  *fPull[NParameters];

    string sRes[NParameters];
    string sPull[NParameters];
    string sResFit[NParameters];
    string sPullFit[NParameters];

    string AxisNameRes[NParameters];
    string AxisNamePull[NParameters];

    AxisNameRes[0] = "Residual (x^{reco} - x^{mc}) [cm]";
    AxisNameRes[1] = "Residual (y^{reco} - y^{mc}) [cm]";
    AxisNameRes[2] = "Residual (z^{reco} - z^{mc}) [cm]";
    AxisNameRes[3] = "Residual (P_{x}^{reco} - P_{x}^{mc}) [GeV/c]";
    AxisNameRes[4] = "Residual (P_{y}^{reco} - P_{y}^{mc}) [GeV/c]";
    AxisNameRes[5] = "Residual (P_{z}^{reco} - P_{z}^{mc}) [GeV/c]";
    AxisNameRes[6] = "Residual (E^{reco} - E^{mc}) [GeV/c^{2}]";
    AxisNameRes[7] = "Residual (M^{reco} - M^{mc}) [GeV/c^{2}]"; 
    
    AxisNamePull[0] = "Pull x";
    AxisNamePull[1] = "Pull y";
    AxisNamePull[2] = "Pull z";
    AxisNamePull[3] = "Pull P_{x}";
    AxisNamePull[4] = "Pull P_{y}";
    AxisNamePull[5] = "Pull P_{z}";
    AxisNamePull[6] = "Pull E";
    AxisNamePull[7] = "Pull M";
    
    string res = "res";
    string pull = "pull";

    for(int i=0; i<NParameters; i++)
    {
      sRes[i]=res+ParNames[i];
      sPull[i]=pull+ParNames[i];
      sResFit[i]=sRes[i]+"GaussFit"+dirName[iDir].Data();
      sPullFit[i]=sPull[i]+"GaussFit"+dirName[iDir].Data();
    }

    for(int i=0; i<NParameters; i++)
    {
      cout <<sRes[i].data()<<endl;
  
      hRes[i] = (TH1D*) dir->Get(sRes[i].data());
      std::cout << hRes[i] << std::endl;

      fRes[i] = new TF1(sResFit[i].data(),"gaus");
      fRes[i]->SetLineColor(2);
      hRes[i]->Fit(sResFit[i].data(),"","",hRes[i]->GetXaxis()->GetXmin(),hRes[i]->GetXaxis()->GetXmax());
      
      hPull[i] = (TH1D*) dir->Get(sPull[i].data());
      fPull[i] = new TF1(sPullFit[i].data(),"gaus");
      fPull[i]->SetLineColor(2);
      hPull[i]->Fit(sPullFit[i].data(),"","",hPull[i]->GetXaxis()->GetXmin(),hPull[i]->GetXaxis()->GetXmax());
  //    hPull[i]->Fit(sPullFit[i].data(),"","",-2,2);
    }

    TCanvas *c1;
    TString canvName1 = "position_";
    canvName1 += dirName[iDir];
    c1 = new TCanvas(canvName1,canvName1,0,0,1200,800);
    c1 -> Divide(3,2);
    c1->UseCurrentStyle();
    
    TCanvas *c2;
    TString canvName2 = "momentum_";
    canvName2 += dirName[iDir];
    c2 = new TCanvas(canvName2,canvName2,0,0,1200,800);
    c2 -> Divide(NParameters-3,2);
    c2->UseCurrentStyle();

    TCanvas *c3;
    TString canvName3 = "chi2_";
    canvName3 += dirName[iDir];
    c3 = new TCanvas(canvName3,canvName3,0,0,1200,400);
    c3 -> Divide(2,1);
    c3->UseCurrentStyle();

    
    int NPad=0;

    for(int i=0; i<3; i++)
    {
  //if(i == 1 || i == 3) continue;
      NPad++;
      c1->cd(NPad);
      hRes[i]->GetXaxis()->SetTitle(AxisNameRes[i].data());
      hRes[i]->GetXaxis()->SetTitleOffset(1);
      hRes[i]->Draw();
      fRes[i]->Draw("same");
    }
    for(int i=0; i<3; i++)
    {
  //if(i == 1 || i == 3) continue;
      NPad++;
      c1->cd(NPad);
      hPull[i]->GetXaxis()->SetTitle(AxisNamePull[i].data());
      hPull[i]->GetXaxis()->SetTitleOffset(1);
      hPull[i]->Draw();
      fPull[i]->Draw("same");
    }

    
    NPad=0;
    for(int i=3; i<NParameters; i++)
    {
  //if(i == 1 || i == 3) continue;
      NPad++;
      c2->cd(NPad);
      hRes[i]->GetXaxis()->SetTitle(AxisNameRes[i].data());
      hRes[i]->GetXaxis()->SetTitleOffset(1);
      hRes[i]->Draw();
      fRes[i]->Draw("same");
    }
    for(int i=3; i<NParameters; i++)
    {
  //if(i == 1 || i == 3) continue;
      NPad++;
      c2->cd(NPad);
      hPull[i]->GetXaxis()->SetTitle(AxisNamePull[i].data());
      hPull[i]->GetXaxis()->SetTitleOffset(1);
      hPull[i]->Draw();
      fPull[i]->Draw("same");
    }
    
    if(iDir==0)
    {
      TDirectory *chidir = f->GetDirectory(chiDirName);
      TH1D* hChi[2];
      {
        c3->cd(1);
        hChi[0] =  (TH1D*) chidir->Get("chi2ndf");
        hChi[0]->Draw();
        c3->cd(2);
        hChi[1] =  (TH1D*) chidir->Get("prob");
        hChi[1]->Draw();      
      }
    }
    
    
    TCanvas *c4;
    c4 = new TCanvas("c4","c4",0,0,1200,400);
    c4 -> cd();
    
    TString outFileName = "FitQAx_";
    outFileName += outname[iDir];
    outFileName += ".pdf";
    c1->SaveAs(outFileName);
    outFileName = "FitQAp_";
    outFileName += outname[iDir];
    outFileName += ".pdf";
    c2->SaveAs(outFileName);
    outFileName = "FitChi2_";
    outFileName += outname[iDir];
    outFileName += ".pdf";
    c3->SaveAs(outFileName);
  }
 
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);

}
