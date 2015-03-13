#include "TFile.h"
#include "TH3D.h"
#include "../analysis/hyperon/hyperon_style.C"
#include "TCanvas.h"

void rec_pol_histo(){

  gStyle->SetPaperSize(28.,19.5);

  static const Int_t nBinsPt=5;
  Double_t rangePt[nBinsPt+1]={1.,2.,4.,6.,8.,10.};

  TString genFileName = "data_pol_100000/polHisto.root";
  TString accFileName = "data_pol_100000/polHisto.root";
  TString fidFileName = "fid_jpsi10.root";
  TString recFileName = "rec_jpsi10.root";

  TH2D* hAndYPt[nBinsPt];
  TFile* fidFile = new TFile(fidFileName.Data());
  
  hAndYPt[0]=(TH2D*) fidFile->Get("hAndYPt4"); // |cos theta | < 0.8
  hAndYPt[1]=(TH2D*) fidFile->Get("hAndYPt3"); // |cos theta | < 0.7
  hAndYPt[2]=(TH2D*) fidFile->Get("hAndYPt4"); // |cos theta | < 0.8
  hAndYPt[3]=(TH2D*) fidFile->Get("hAndYPt4"); // |cos theta | < 0.8
  hAndYPt[4]=(TH2D*) fidFile->Get("hAndYPt4"); // |cos theta | < 0.8
  
  TH1D* hRecCos[nBinsPt];
  TH1D* hAccCos[nBinsPt];
  TH1D* hAllCos[nBinsPt];
  TH1D* haccRecCos[nBinsPt];
  TH1D* haccAllCos[nBinsPt];
  TH1D* hResCos[nBinsPt];
  
  char histoName[100];
  for (Int_t i=0;i<nBinsPt;i++){
    sprintf(histoName,"hRecCos%d",i);
    hRecCos[i] = new TH1D(histoName,histoName,20,-1,1);
    sprintf(histoName,"hAllCos%d",i);
    hAllCos[i] = new TH1D(histoName,histoName,20,-1,1);
    sprintf(histoName,"hAccCos%d",i);
    hAccCos[i] = new TH1D(histoName,histoName,20,-1,1);
    sprintf(histoName,"hResCos%d",i);
    hResCos[i] = new TH1D(histoName,histoName,20,-1,1);
    sprintf(histoName,"haccRecCos%d",i);
    haccRecCos[i] = new TH1D(histoName,histoName,20,-1,1);
    sprintf(histoName,"haccAllCos%d",i);
    haccAllCos[i] = new TH1D(histoName,histoName,20,-1,1);
  }
  
  TFile* accFile = new TFile(accFileName.Data());
  TH3D* haccAllYPtCos = (TH3D*) accFile->Get("hAllYPtCosH");
  TH3D* haccRecYPtCos = (TH3D*) accFile->Get("hAccYPtCosH");
  FillCos(haccRecCos,haccRecYPtCos,hAndYPt,rangePt,nBinsPt,0);
  FillCos(haccAllCos,haccAllYPtCos,hAndYPt,rangePt,nBinsPt,0);
  
  TFile* genFile = new TFile(genFileName.Data());
  TH3D* hRecYPtCos = (TH3D*) genFile->Get("hRecYPtCosH");
  TH3D* hAllYPtCos = (TH3D*) genFile->Get("hGenYPtCosH");
  cout << hRecYPtCos->GetEntries() << endl;
  FillCos(hRecCos,hRecYPtCos,hAndYPt,rangePt,nBinsPt);
  FillCos(hAllCos,hAllYPtCos,hAndYPt,rangePt,nBinsPt,0);
  

  
  for (Int_t i=0;i<nBinsPt;i++){
    hAccCos[i]->Divide(haccRecCos[i],haccAllCos[i]);
  }

  for (Int_t i=0;i<nBinsPt;i++){
    hResCos[i]->Divide(hRecCos[i],hAccCos[i]);
  }

  Int_t nEntries[nBinsPt];
  for (Int_t i=0;i<nBinsPt;i++){
    nEntries[i]=0;
    for (Int_t j=0;j<=20;j++){ nEntries[i]+=hRecCos[i]->GetBinContent(j);}
//    nEntries[i] = hRecCos[i]->Integral();
    cout << "bin: " << i+1 << "rec: " << nEntries[i] << endl;
  }

  TCanvas* c1 = new TCanvas("c1","",nBinsPt*240,2*240);
  c1->Divide(nBinsPt,2);
  Int_t minCosBin[nBinsPt]={ 3, 4, 3, 3, 3};
  Int_t maxCosBin[nBinsPt]={18,17,18,18,18};
  Double_t alpha[nBinsPt];
  Double_t error[nBinsPt];
  for (Int_t i=0;i<nBinsPt;i++){
/*    
    c1->cd(i+1+nBinsPt*0);
    sprintf(histoName,"%1.0f < pt < %1.0f; cos #theta;  Accepted",rangePt[i],rangePt[i+1]);
    haccAllCos[i]->SetTitle(histoName);
    haccAllCos[i]->Draw();
    haccAllCos[i]->SetMinimum(0);
    haccRecCos[i]->Draw("same");
*/    
    c1->cd(i+1+nBinsPt*0);
    
    sprintf(histoName,"Acceptance vs cos #theta: %1.0f < pt < %1.0f; cos #theta;  Acceptance",rangePt[i],rangePt[i+1]);
    hAccCos[i]->SetTitle(histoName);
    hAccCos[i]->Draw();
    c1->cd(i+1+nBinsPt*1);
    sprintf(histoName,"fPolar%i_pt",i+1);
    Float_t cosMin=-1+(minCosBin[i]-1)*0.1;
    Float_t cosMax=-1+maxCosBin[i]*0.1;
    TF1* fPolar = new TF1(histoName,"[0]*(1+[1]*x*x)",cosMin,cosMax);
    fPolar->SetLineColor(kRed);
    //fPolar->SetLineWidth(1);
    hResCos[i]->Fit(fPolar,"R");
    hAllCos[i]->SetMinimum(0);
    sprintf(histoName,"Reconstructed vs cos #theta: %1.0f < pt < %1.0f; cos #theta;",rangePt[i],rangePt[i+1]);
    hAllCos[i]->SetTitle(histoName);
    hAllCos[i]->Draw();
    hResCos[i]->SetLineColor(kBlue);
    hResCos[i]->SetLineWidth(1);
    hResCos[i]->Draw("e same");
    alpha[i] = fPolar->GetParameter(1);
    error[i] = fPolar->GetParError(1);
    TLegend* l1 = new TLegend(0.22,0.15,0.78,0.38);
    sprintf(histoName,"Rec. fit: #alpha=%4.3f #pm %4.3f", alpha[i], error[i]);
    l1->AddEntry(hAllCos[i],"Simulated, #alpha=-1.0");
    l1->AddEntry(hResCos[i],"Reconstructed");
    l1->AddEntry(fPolar,histoName);
    l1->Draw();
  }

  cout << "====  Alpha  ==================================" << endl;
  for (Int_t i=0;i<nBinsPt;i++)  printf(" %6.3f ",alpha[i]);
  cout << endl;
  cout << "===============================================" << endl;

  cout << "====  Error  ==================================" << endl;
  for (Int_t i=0;i<nBinsPt;i++)  printf(" %6.3f ",error[i]);
  cout << endl;
  cout << "===============================================" << endl;
  
  TH1D* hAlpha = new TH1D("hAlpha","#alpha vs p_{t};p_{t} [GeV/c];#alpha"
                           ,nBinsPt,rangePt);
  for (Int_t i=0;i<nBinsPt;i++) {
    hAlpha->SetBinContent(i+1,alpha[i]);
    hAlpha->SetBinError  (i+1,error[i]);
  }
  TCanvas* c2 = new TCanvas("c2");
  hAlpha->SetMinimum(-1);
  hAlpha->SetMaximum(1);
  hAlpha->SetLineColor(kBlue);
  hAlpha->SetLineWidth(1);
  hAlpha->GetXaxis()->SetRangeUser(0.,10.);
  hAlpha->Draw();
  
  TFile* recFile = new TFile(recFileName,"recreate");
  for (Int_t i=0;i<nBinsPt;i++){
    hAccCos[i]->Write();
    hAllCos[i]->Write();
    hRecCos[i]->Write();
    hResCos[i]->Write();
  }
    hAlpha->Write();
}


FillCos(TH1D** hCos, TH3* hYPtCos, TH2D** hAndYPt, 
        Double_t *rangePt,Int_t nBinsPt, Bool_t cutCos=1){
  Int_t minCosBin[5]={ 3, 4, 3, 3, 3};
  Int_t maxCosBin[5]={18,17,18,18,18};
  
  for (Int_t iBin=0;iBin<nBinsPt;iBin++){
    for (Int_t iz=1;iz<=hYPtCos->GetZaxis()->GetNbins(); iz++){//cos
      if (cutCos && (iz < minCosBin[iBin] || iz > maxCosBin[iBin])) continue;
      Double_t minPtBin=rangePt[iBin];
      Double_t maxPtBin=rangePt[iBin+1];
      Double_t signal=0;
      Double_t error2=0;
      for (Int_t ix=1;ix<=hYPtCos->GetXaxis()->GetNbins(); ix++){//y
        for (Int_t iy=minPtBin;iy<=maxPtBin;iy++){ //pt
          if (hAndYPt[iBin]->GetBinContent(ix,iy)==0) continue;
          signal += hYPtCos->GetBinContent(ix,iy,iz);
          error2 += pow(hYPtCos->GetBinError(ix,iy,iz),2);
        }
      }
      hCos[iBin]->SetBinContent(iz,signal);
      hCos[iBin]->SetBinError  (iz,sqrt(error2));
    }//Cos
  }

}
