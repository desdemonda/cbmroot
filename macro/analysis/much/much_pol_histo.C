#include "TFile.h"
#include "TH3D.h"
#include "TH2D.h"
#include "../analysis/hyperon/hyperon_style.C"
#include "TCanvas.h"
#include "TF1.h"
#include "TLegend.h"

static const Int_t nBinsPt=3;
static Double_t rangePt[nBinsPt+1]={0.5,1.0,2.0,3.0};
static Int_t minCosBin[nBinsPt]={ 5, 5, 5};
static Int_t maxCosBin[nBinsPt]={16,16,16};

void FillCos(TH1D** hCos, TH3* hYPtCos, TH2D** hAndYPt, Bool_t cutCos=1);


void much_pol_histo(){
  TString dir = "data/";
  TString accFileName = dir+"polHisto.root";
  TString genFileName = dir+"polHisto.root";  
  TString recFileName = dir+"recHisto.root";
  hyperon_style();

  TFile* f = new TFile(accFileName,"read");
  TH3D* hJpsiAllYPtCost = (TH3D*) f->Get("fhJpsiAllYPtCost");
  TH3D* hJpsiRecYPtCost = (TH3D*) f->Get("fhJpsiRecYPtCost");
  hJpsiAllYPtCost->Sumw2();
  hJpsiRecYPtCost->Sumw2();
  TH3D* hJpsiEffYPtCost = (TH3D*) hJpsiRecYPtCost->Clone();
  hJpsiEffYPtCost->Divide(hJpsiAllYPtCost);
  
  Int_t binsY   = hJpsiAllYPtCost->GetXaxis()->GetNbins();
  Double_t minY = hJpsiAllYPtCost->GetXaxis()->GetXmin();
  Double_t maxY = hJpsiAllYPtCost->GetXaxis()->GetXmax();
  
  Int_t binsPt   = hJpsiAllYPtCost->GetYaxis()->GetNbins();
  Double_t minPt = hJpsiAllYPtCost->GetYaxis()->GetXmin();
  Double_t maxPt = hJpsiAllYPtCost->GetYaxis()->GetXmax();
  
  Int_t binsCost   = hJpsiAllYPtCost->GetZaxis()->GetNbins();
  Double_t minCost = hJpsiAllYPtCost->GetZaxis()->GetXmin();
  Double_t maxCost = hJpsiAllYPtCost->GetZaxis()->GetXmax();

  TH2D** hFidYPt = new TH2D*[binsCost];
  TH2D** hAndYPt = new TH2D*[binsCost];
  
  // Create fiducial regions
  for (Int_t icost=1;icost<=binsCost;icost++){
    hFidYPt[icost-1] = new TH2D(Form("hFidYPt%2i",icost),Form("hFidYPt%2i",icost),binsY,minY,maxY,binsPt,minPt,maxPt);
    hAndYPt[icost-1] = new TH2D(Form("hAndYPt%2i",icost),Form("hAndYPt%2i",icost),binsY,minY,maxY,binsPt,minPt,maxPt);
    for (Int_t iy=1;iy<=binsY;iy++){
      for (Int_t ipt=1;ipt<=binsPt;ipt++){
        Double_t rec = hJpsiRecYPtCost->GetBinContent(iy,ipt,icost);
        hFidYPt[icost-1]->SetBinContent(iy,ipt,1);
        hAndYPt[icost-1]->SetBinContent(iy,ipt,1);
        if (rec>200) continue;
        hJpsiEffYPtCost->SetBinContent(iy,ipt,icost,0.);
        hFidYPt[icost-1]->SetBinContent(iy,ipt,0);
      }
    }
  }
  
  for (Int_t icost=1;icost<=binsCost/2;icost++){
    for (Int_t i=icost;i<=binsCost+1-icost;i++)
      hAndYPt[icost-1]->Multiply(hFidYPt[i-1]);
  }

  TCanvas* cFiducial = new TCanvas("cFiducial","cFiducial",200*5,200*4);
  cFiducial->Divide(5,4);
  for (Int_t icost=1;icost<=binsCost;icost++){
    cFiducial->cd(icost);
    gPad->SetRightMargin(0.1);
    hFidYPt[icost-1]->Draw("colz");
  }

  TCanvas* cAnd = new TCanvas("cAnd","cAnd",200*5,200*2);
  cAnd->Divide(5,2);
  for (Int_t icost=1;icost<=binsCost/2;icost++){
    cAnd->cd(icost);
    gPad->SetRightMargin(0.1);
    hAndYPt[icost-1]->Draw("colz");
  }

  
  TH3D* hJpsiRecEffCorrectedYPtCost = (TH3D*) hJpsiRecYPtCost->Clone();
  hJpsiRecEffCorrectedYPtCost->Divide(hJpsiEffYPtCost);
  
//  TH1D* hJpsiAllCost = hJpsiAllYPtCost->ProjectionZ();
  
  
//  TH1D* hJpsiRecEffCorrectedCost = hJpsiRecEffCorrectedYPtCost->ProjectionZ();
//  TH1D** hJpsiRecEffCorrectedCost = new TH1D*[nBinsPt];
  
  
//  hJpsiAllCost->SetMinimum(0);
//  hJpsiAllCost->SetLineColor(kBlue);
//  hJpsiRecEffCorrectedCost->SetLineColor(kRed);
//  
//  TCanvas* cCost = new TCanvas("cCost","Cos #theta distributions",400,400);
//  hJpsiAllCost->Draw();
//  hJpsiRecEffCorrectedCost->Draw("same");


  TH1D* hRecCost[nBinsPt];
  TH1D* hAccCost[nBinsPt];
  TH1D* hAllCost[nBinsPt];
  TH1D* haccRecCost[nBinsPt];
  TH1D* haccAllCost[nBinsPt];
  TH1D* hResCost[nBinsPt];
  
  for (Int_t i=0;i<nBinsPt;i++){
    hRecCost[i] = new TH1D(Form("hRecCost%d",i),Form("hRecCost%d",i),binsCost,minCost,maxCost);
    hAllCost[i] = new TH1D(Form("hAllCost%d",i),Form("hAllCost%d",i),binsCost,minCost,maxCost);
    hAccCost[i] = new TH1D(Form("hAccCost%d",i),Form("hAccCost%d",i),binsCost,minCost,maxCost);
    hResCost[i] = new TH1D(Form("hResCost%d",i),Form("hResCost%d",i),binsCost,minCost,maxCost);
    haccRecCost[i] = new TH1D(Form("haccRecCost%d",i),Form("haccRecCost%d",i),binsCost,minCost,maxCost);
    haccAllCost[i] = new TH1D(Form("haccAllCost%d",i),Form("haccAllCost%d",i),binsCost,minCost,maxCost);
  }
  
  TH2D* vhAndYPt[nBinsPt];
//  TH2D* vhAndGenYPt[nBinsPt];
  for (Int_t iBin=0;iBin<nBinsPt;iBin++){
    vhAndYPt[iBin]=hAndYPt[minCosBin[iBin]];
//    vhAndGenYPt[iBin]=hAndGenYPt[minCosBin[iBin]];
  }
  
  TFile* accFile = new TFile(accFileName.Data());
  TH3D* haccAllYPtCost = (TH3D*) accFile->Get("fhJpsiAllYPtCost");
  TH3D* haccRecYPtCost = (TH3D*) accFile->Get("fhJpsiRecYPtCost");
  FillCos(haccRecCost,haccRecYPtCost,vhAndYPt,0);
  FillCos(haccAllCost,haccAllYPtCost,vhAndYPt,0);
  
  TFile* genFile = new TFile(genFileName.Data());
  TH3D* hAllYPtCost = (TH3D*) genFile->Get("fhJpsiAllYPtCost");
  TH3D* hRecYPtCost = (TH3D*) genFile->Get("fhJpsiRecYPtCost");
  FillCos(hRecCost,hRecYPtCost,vhAndYPt,1);
  FillCos(hAllCost,hAllYPtCost,vhAndYPt,0);
  
  for (Int_t i=0;i<nBinsPt;i++){
    hAccCost[i]->Divide(haccRecCost[i],haccAllCost[i]);
    hResCost[i]->Divide(hRecCost[i],hAccCost[i]);
  }
  
  Int_t nEntries[nBinsPt];
  for (Int_t i=0;i<nBinsPt;i++){
    nEntries[i]=0;
    for (Int_t j=0;j<=20;j++){ nEntries[i]+= (Int_t) hRecCost[i]->GetBinContent(j);}
//    nEntries[i] = hRecCos[i]->Integral();
//    cout << "bin: " << i+1 << "rec: " << nEntries[i] << endl;
  }

  TCanvas* c1 = new TCanvas("c1","",nBinsPt*240,2*240);
  c1->Divide(nBinsPt,2);
  Double_t alpha[nBinsPt];
  Double_t error[nBinsPt];
  for (Int_t i=0;i<nBinsPt;i++){
    c1->cd(i+1+nBinsPt*0);
    hAccCost[i]->SetTitle(Form("Acceptance vs cos #theta: %3.1f < pt < %3.1f; cos #theta;  Acceptance",rangePt[i],rangePt[i+1]));
    hAccCost[i]->Draw();
    c1->cd(i+1+nBinsPt*1);
    Float_t cosMin=-1+(minCosBin[i]-1)*0.1;
    Float_t cosMax=-1+maxCosBin[i]*0.1;
    TF1* fPolar = new TF1(Form("fPolar%i_pt",i+1),"[0]*(1+[1]*x*x)",cosMin,cosMax);
    fPolar->SetLineColor(kRed);
    //fPolar->SetLineWidth(1);
    hResCost[i]->Fit(fPolar,"R");
    hAllCost[i]->SetMinimum(0);
    hAllCost[i]->SetTitle(Form("Reconstructed vs cos #theta: %3.1f < pt < %3.1f; cos #theta;",rangePt[i],rangePt[i+1]));
    hAllCost[i]->Draw();
    hResCost[i]->SetLineColor(kBlue);
    hResCost[i]->SetLineWidth(1);
    hResCost[i]->Draw("e same");
    alpha[i] = fPolar->GetParameter(1);
    error[i] = fPolar->GetParError(1);
    TLegend* l1 = new TLegend(0.22,0.15,0.78,0.38);
    l1->AddEntry(hAllCost[i],"Simulated, #alpha=-1.0");
    l1->AddEntry(hResCost[i],"Reconstructed");
    l1->AddEntry(fPolar,Form("Rec. fit: #alpha=%4.3f #pm %4.3f", alpha[i], error[i]));
    l1->Draw();
    gPad->Print(Form("%i.png",i));
  }

  printf("====  Alpha  ==================================\n");
  for (Int_t i=0;i<nBinsPt;i++)  printf(" %6.3f ",alpha[i]);
  printf("\n");
  printf("===============================================\n");

  printf("====  Error  ==================================\n");
  for (Int_t i=0;i<nBinsPt;i++)  printf(" %6.3f ",error[i]);
  printf("\n");
  printf("===============================================\n");
  
  TH1D* hAlpha = new TH1D("hAlpha","#alpha vs p_{t};p_{t} [GeV/c];#alpha",nBinsPt,rangePt);
  for (Int_t i=0;i<nBinsPt;i++) {
    hAlpha->SetBinContent(i+1,alpha[i]);
    hAlpha->SetBinError  (i+1,error[i]);
  }
  TCanvas* cAlpha = new TCanvas("cAlpha");
  cAlpha->cd();
  hAlpha->SetMinimum(-1);
  hAlpha->SetMaximum(1);
  hAlpha->SetLineColor(kBlue);
  hAlpha->SetLineWidth(1);
  hAlpha->GetXaxis()->SetRangeUser(0.,10.);
  hAlpha->Draw();
  
  TFile* recFile = new TFile(recFileName,"recreate");
  for (Int_t i=0;i<nBinsPt;i++){
    hAccCost[i]->Write();
    hAllCost[i]->Write();
    hRecCost[i]->Write();
    hResCost[i]->Write();
  }
  hAlpha->Write();
  recFile->Close();
}

void FillCos(TH1D** hCos, TH3* hYPtCos, TH2D** hAndYPt, Bool_t cutCos){
  for (Int_t iBin=0;iBin<nBinsPt;iBin++){
    for (Int_t icost=1;icost<=hYPtCos->GetZaxis()->GetNbins(); icost++){//cos
      if (cutCos && (icost < minCosBin[iBin] || icost > maxCosBin[iBin])) continue;
      Int_t minPtBin=hYPtCos->GetYaxis()->FindBin(rangePt[iBin]);
      Int_t maxPtBin=hYPtCos->GetYaxis()->FindBin(rangePt[iBin+1]);
      Double_t signal=0;
      Double_t error2=0;
      for (Int_t iy=1;iy<=hYPtCos->GetXaxis()->GetNbins(); iy++){//y
        for (Int_t ipt=minPtBin;ipt<=maxPtBin;ipt++){ //pt
          if (hAndYPt[iBin]->GetBinContent(iy,ipt)==0) continue;
          signal += hYPtCos->GetBinContent(iy,ipt,icost);
          error2 += pow(hYPtCos->GetBinError(iy,ipt,icost),2);
        }
      }
      hCos[iBin]->SetBinContent(icost,signal);
      hCos[iBin]->SetBinError  (icost,sqrt(error2));
    }//Cos
  }

}
