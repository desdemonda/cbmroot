// -------------------------------------------------------------------------
// -----                  CbmAnaHypSecFinderQa source file             -----
// -----                  Created 24/11/06  by E. Kryshen              -----
// -------------------------------------------------------------------------

#include "CbmAnaHypSecFinderQa.h"

#include "TH1D.h"
#include "TH2D.h"
#include "TVector3.h"
#include "TClonesArray.h"
#include "TCanvas.h"

#include "FairRootManager.h"
#include "CbmAnaHypTools.h"
#include "CbmMCTrack.h"
#include "CbmStsPoint.h"

ClassImp(CbmAnaHypSecFinderQa)

// -------------------------------------------------------------------------
CbmAnaHypSecFinderQa::CbmAnaHypSecFinderQa()
: CbmAnaHypSecFinder(),
  fTools(NULL),
  fTrackMatch(NULL),
  hTxB(),
  hTyB(),
  hTxS(),
  hTyS(),
  hDtxB(),
  hDtyB(),
  hDtxS(),
  hDtyS(),
  hChi2S(NULL),
  hChi2B(NULL),
  hPS(NULL),
  hPB(NULL),
  hDx(),
  hDy(),
  hDxPull(),
  hDyPull(),
  hPullX(NULL),
  hPullY(NULL),
  hPullQp(NULL),
  hQpDtx(NULL),
  fEvent(0),
  fFinder(0),
  fFound(0),
  fAll(0),
  fSig(0),
  fSigD(0),
  fFakeD(0),
  fL1AC(0.),
  fRC(0),
  fRC_good(0),
  fRC_fake(0),
  fSR(0),
  fSR_good(0),
  fSR_fake(0),
  fAR(0),
  fAR_good(0),
  fAR_fake(0)
{
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
CbmAnaHypSecFinderQa::CbmAnaHypSecFinderQa(char* name, Int_t iVerbose)
  : CbmAnaHypSecFinder(name,iVerbose),
    fTools(NULL),
    fTrackMatch(NULL),
    hTxB(),
    hTyB(),
    hTxS(),
    hTyS(),
    hDtxB(),
    hDtyB(),
    hDtxS(),
    hDtyS(),
    hChi2S(NULL),
    hChi2B(NULL),
    hPS(NULL),
    hPB(NULL),
    hDx(),
    hDy(),
    hDxPull(),
    hDyPull(),
    hPullX(NULL),
    hPullY(NULL),
    hPullQp(NULL),
    hQpDtx(NULL),
    fEvent(0),
    fFinder(0),
    fFound(0),
    fAll(0),
    fSig(0),
    fSigD(0),
    fFakeD(0),
    fL1AC(0.),
    fRC(0),
    fRC_good(0),
    fRC_fake(0),
    fSR(0),
    fSR_good(0),
    fSR_fake(0),
    fAR(0),
    fAR_good(0),
    fAR_fake(0)
{
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
CbmAnaHypSecFinderQa::~CbmAnaHypSecFinderQa() {}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
InitStatus CbmAnaHypSecFinderQa::Init() {
  CbmAnaHypSecFinder::Init();

  FairRootManager* ioman = FairRootManager::Instance();
  fTrackMatch = (TClonesArray*) ioman->GetObject("StsTrackMatch");


  fTools = new CbmAnaHypTools();
  fTools->Init();
  fFound=0;
  fL1AC=0;
  fAll=0;
  fSig=0;
  fEvent=0;
  fFakeD=0;
  fSigD=0;
  fRC = fRC_good = fRC_fake = 0;
  fSR = fSR_good = fSR_fake = 0;
  fAR = fAR_good = fAR_fake = 0;

//  hChi2S = new TH1D("hChi2S","hChi2S",100,0,1000);
//  hChi2B = new TH1D("hChi2B","hChi2B",100,0,1000);

  hChi2S = new TH1D("hChi2S","hChi2S",100,0,20);
  hChi2B = new TH1D("hChi2B","hChi2B",100,0,20);

  hQpDtx = new TH2D("hQpDtx","hQpDtx",100,-10.,10.,100,-0.4,0.4);

  hPS = new TH1D("hPS","hPS",100,0.,1.);
  hPB = new TH1D("hPB","hPB",100,0.,1.);
  hChi2S->SetLineColor(kBlue);
  hPS->SetLineColor(kBlue);

  hPullX  = new TH1D("hPullX" ,"hPullX" ,100,-10.,10.);
  hPullY  = new TH1D("hPullY" ,"hPullY" ,100,-10.,10.);
  hPullQp = new TH1D("hPullQp","hPullQp",100,-10.,10.);

  for (Int_t i=0;i<10;i++){
    char s[10];
    sprintf(s,"hTxS%i",i);
    hTxS[i] = new TH1D(s,s,120,0,3);
    sprintf(s,"hTyS%i",i);
    hTyS[i] = new TH1D(s,s,120,0,3);
    sprintf(s,"hTxB%i",i);
    hTxB[i] = new TH1D(s,s,120,0,3);
    sprintf(s,"hTyB%i",i);
    hTyB[i] = new TH1D(s,s,120,0,3);
    sprintf(s,"hDtxS%i",i);
    hDtxS[i] = new TH1D(s,s,100,0,2);
    sprintf(s,"hDtyS%i",i);
    hDtyS[i] = new TH1D(s,s,100,0,1);
    sprintf(s,"hDtxB%i",i);
    hDtxB[i] = new TH1D(s,s,100,0,2);
    sprintf(s,"hDtyB%i",i);
    hDtyB[i] = new TH1D(s,s,100,0,1);
    hTxS[i]->SetLineColor(kBlue);
    hTyS[i]->SetLineColor(kBlue);
    hDtxS[i]->SetLineColor(kBlue);
    hDtyS[i]->SetLineColor(kBlue);
    hTxB[i]->SetMinimum(0.5);
    hTyB[i]->SetMinimum(0.5);
    hDtxB[i]->SetMinimum(0.5);
    hDtyB[i]->SetMinimum(0.5);
    sprintf(s,"hDx%i",i);
    hDx[i] = new TH1D(s,s,40,-0.2,0.2);
    sprintf(s,"hDy%i",i);
    hDy[i] = new TH1D(s,s,40,-0.2,0.2);
    sprintf(s,"hDxPull%i",i);
    hDxPull[i] = new TH1D(s,s,50,-10,10);
    sprintf(s,"hDyPull%i",i);
    hDyPull[i] = new TH1D(s,s,50,-10,10);

  }

  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmAnaHypSecFinderQa::DrawHisto(){
  TCanvas* c = new TCanvas("c","",600,300);
  c->Divide(2,1);
  c->cd(1);
  hTxS[0]->Draw();
  c->cd(2);
  hTyS[0]->Draw();

  TH1D* hTxEf[10];
  TH1D* hTyEf[10];
  TH1D* hDtxEf[10];
  TH1D* hDtyEf[10];
  TH1D* hChi2Ef = new TH1D("hChi2Ef","hChi2Ef",100,0,1000);
  TH1D* hPEf    = new TH1D("hPEf","hPEf",100,0.,1.);

  Double_t tot;
  tot = hChi2S->Integral(0,101);
  for (Int_t bin = 1; bin<101; bin++)
    hChi2Ef->SetBinContent(bin,hChi2S->Integral(0,bin)/tot);

  tot = hPS->Integral(0,101);
  for (Int_t bin = 1; bin<101; bin++)
    hPEf->SetBinContent(bin,hPS->Integral(bin,101)/tot);

  for (Int_t i=0;i<fNstations-3;i++) {
    char s[10];
    sprintf(s,"hTxEf%i",i);
    hTxEf[i]  = new TH1D(s,s,120,0,3);
    sprintf(s,"hTyEf%i",i);
    hTyEf[i]  = new TH1D(s,s,120,0,3);
    sprintf(s,"hDtxEf%i",i);
    hDtxEf[i] = new TH1D(s,s,100,0,2);
    sprintf(s,"hDtyEf%i",i);
    hDtyEf[i] = new TH1D(s,s,100,0,1);

    tot = hTxS[i]->Integral(0,121);
    for (Int_t bin = 1; bin<121; bin++)
      hTxEf[i]->SetBinContent(bin,hTxS[i]->Integral(0,bin)/tot);

    tot = hTyS[i]->Integral(0,121);
    for (Int_t bin = 1; bin<121; bin++)
      hTyEf[i]->SetBinContent(bin,hTyS[i]->Integral(0,bin)/tot);

    tot = hDtxS[i]->Integral(0,101);
    for (Int_t bin = 1; bin<101; bin++)
      hDtxEf[i]->SetBinContent(bin,hDtxS[i]->Integral(0,bin)/tot);

    tot = hDtyS[i]->Integral(0,101);
    for (Int_t bin = 1; bin<101; bin++)
      hDtyEf[i]->SetBinContent(bin,hDtyS[i]->Integral(0,bin)/tot);

  }


  TCanvas* c1 = new TCanvas("c1","",300*4,300*(fNstations-3));
  c1->Divide(4,(fNstations-3));
  for (Int_t i=0;i<fNstations-3;i++){
    c1->cd(4*i+1);
    gPad->SetLogy();
    if (hTxB[i]->GetMaximum()>hTxS[i]->GetMaximum())
         {  hTxB[i]->Draw();  hTxS[i]->Draw("same");  }
    else {  hTxS[i]->Draw();  hTxB[i]->Draw("same");  }
    c1->cd(4*i+3);
    gPad->SetLogy();
    if (hTyB[i]->GetMaximum()>hTyS[i]->GetMaximum())
         {  hTyB[i]->Draw();  hTyS[i]->Draw("same");  }
    else {  hTyS[i]->Draw();  hTyB[i]->Draw("same");  }
    c1->cd(4*i+2);
    hTxEf[i]->Draw();
    c1->cd(4*i+4);
    hTyEf[i]->Draw();
  }
  TCanvas* c2 = new TCanvas("c2","",300*4,300*(fNstations-3));
  c2->Divide(4,(fNstations-3));
  for (Int_t i=0;i<fNstations-3;i++){
    c2->cd(4*i+1);
    gPad->SetLogy();
    if (hDtxB[i]->GetMaximum()>hDtxS[i]->GetMaximum())
         {  hDtxB[i]->Draw();  hDtxS[i]->Draw("same");  }
    else {  hDtxS[i]->Draw();  hDtxB[i]->Draw("same");  }
    c2->cd(4*i+3);
    gPad->SetLogy();
    if (hDtyB[i]->GetMaximum()>hDtyS[i]->GetMaximum())
         {  hDtyB[i]->Draw();  hDtyS[i]->Draw("same");  }
    else {  hDtyS[i]->Draw();  hDtyB[i]->Draw("same");  }
    c2->cd(4*i+2);
    hDtxEf[i]->Draw();
    c2->cd(4*i+4);
    hDtyEf[i]->Draw();
  }
  TCanvas* c3 = new TCanvas("c3","",300*2,300*2);
  c3->Divide(2,2);
  c3->cd(1);
  gPad->SetLogy();
  hChi2S->Draw();
  hChi2B->Draw("same");
  c3->cd(2);
  hChi2Ef->Draw();
  c3->cd(3);
  gPad->SetLogy();
  hPS->Draw();
  hPB->Draw("same");
  c3->cd(4);
  hPEf->Draw();

}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmAnaHypSecFinderQa::WriteHisto(TString fileName){

  TFile* f = new TFile(fileName,"recreate");
  hChi2S->Write();
  hChi2B->Write();
  hPS->Write();
  hPB->Write();
  for (Int_t i=0;i<fNstations-3;i++){
    hTxS[i]->Write();
    hTxB[i]->Write();
    hTyS[i]->Write();
    hTyB[i]->Write();
    hDtxS[i]->Write();
    hDtxB[i]->Write();
    hDtyS[i]->Write();
    hDtyB[i]->Write();
  }



  f->Close();

  TCanvas* cc = new TCanvas("cc","",400,400);
  cc->cd();
  hQpDtx->Draw();

  TCanvas* cc1 = new TCanvas("cc1","",1000,1000);
  cc1->Divide(4,4);
  cc1->cd(1);
  hDx[3]->Draw();
  cc1->cd(2);
  hDy[3]->Draw();
  cc1->cd(3);
  hDxPull[3]->Fit("gaus");
  hDxPull[3]->Draw();
  cc1->cd(4);
  hDyPull[3]->Fit("gaus");
  hDyPull[3]->Draw();
  cc1->cd(5);
  hDx[4]->Draw();
  cc1->cd(6);
  hDy[4]->Draw();
  cc1->cd(7);
  hDxPull[4]->Fit("gaus");
  hDxPull[4]->Draw();
  cc1->cd(8);
  hDyPull[4]->Fit("gaus");
  hDyPull[4]->Draw();
  cc1->cd(9);
  hDx[5]->Draw();
  cc1->cd(10);
  hDy[5]->Draw();
  cc1->cd(11);
  hDxPull[5]->Fit("gaus");
  hDxPull[5]->Draw();
  cc1->cd(12);
  hDyPull[5]->Fit("gaus");
  hDyPull[5]->Draw();
  cc1->cd(13);
  hDx[6]->Draw();
  cc1->cd(14);
  hDy[6]->Draw();
  cc1->cd(15);
  hDxPull[6]->Fit("gaus");
  hDxPull[6]->Draw();
  cc1->cd(16);
  hDyPull[6]->Fit("gaus");
  hDyPull[6]->Draw();


  TCanvas* cc2 = new TCanvas("cc2","cc2",900,300);
  cc2->Divide(3,1);
  cc2->cd(1);
  hPullX->Fit("gaus");
  hPullX->Draw();
  cc2->cd(2);
  hPullY->Fit("gaus");
  hPullY->Draw();
  cc2->cd(3);
  hPullQp->Fit("gaus");
  hPullQp->Draw();
}
// -------------------------------------------------------------------------




// -------------------------------------------------------------------------
void CbmAnaHypSecFinderQa::Exec(Option_t* option){
  printf("Event:%i\n",fEvent++);
//  fTools->SetFlagsOnly();
//  PrintEventStatistics();
  ReadHits();
//  AddHitRefTracks();
  //PrintHitInfo();
  //AnalyseDoubleStrips();
//  DoFind();
  Analyse();
//  Performance();
  //AnalysePulls();
}
// -------------------------------------------------------------------------



// -------------------------------------------------------------------------
void CbmAnaHypSecFinderQa::Analyse(){
  newTrackArray->Clear();
  fRCTracks = 0;

  Double_t txm,txr,tym,tyr,dtx,dty;
  Double_t chi2,p,qp;
  Double_t* C;
  Double_t* T;
  vector<Hit>::iterator iBest,it;
  Double_t s2Best;
  Int_t iAll;

  Double_t x,y,dx,dy,x_min,x_max,y_min,y_max,x0,y0;
  Double_t qp0;
  Double_t z;

  if (fVerbose) printf("Study information:\n");

  Int_t n=0;
  for (Int_t stl=0;stl<fNstations-3;stl++){
//  for (Int_t stl=0;stl<1;stl++){
    if (fVerbose) printf("  Station:%i",stl);
    if (fVerbose) printf("  lhits:%lu\n",vHits[stl].size());
    for (unsigned il=0;il<vHits[stl].size();il++){
      Hit &hl = vHits[stl][il];
      if (hl.used) continue;

      Int_t stm=stl+1;
      for (unsigned im=0;im<vHits[stm].size();im++){
        if (hl.used) break;
        Hit &hm = vHits[stm][im];
        if (hm.used) continue;

        //Cut on the slope
        txm = (hl.x-hm.x)/(hl.z-hm.z);
        tym = (hl.y-hm.y)/(hl.z-hm.z);

        Bool_t sig = (hl.tr==hm.tr && hl.tr!=-1);
        sig = (sig && fTools->IsL1accepted(hl.tr)>3);

        if (sig) {
          hTxS[stl]->Fill(TMath::Abs(txm));
          hTyS[stl]->Fill(TMath::Abs(tym));
        } else {
          hTxB[stl]->Fill(TMath::Abs(txm));
          hTyB[stl]->Fill(TMath::Abs(tym));
        }// if true track

        if (TMath::Abs(tym)>fTyMax[stl]) continue;
        if (TMath::Abs(txm)>fTxMax[stl]) continue;

        if (fVerbose>1) {
          printf("  ");
          if (sig) printf("+"); else printf(" ");
          printf("(%4i,%4i)",hl.id,hm.id);
          printf("tx=%6.2f,ty=%6.2f",TMath::Abs(txm),TMath::Abs(tym));
          printf("\n");
        }

        if (stm>=fNstations-2) continue;

        Int_t str = stm+1;
        for (unsigned ir=0;ir<vHits[str].size();ir++){
          if (hl.used || hm.used) break;
          Hit &hr = vHits[str][ir];
          if (hr.used) continue;

          //Cut on the slope
          txr = (hm.x-hr.x)/(hm.z-hr.z);
          tyr = (hm.y-hr.y)/(hm.z-hr.z);

          // Slope difference
          dtx = TMath::Abs(txr-txm);
          dty = TMath::Abs(tyr-tym);

          Bool_t sig_triplet = (sig && hm.tr==hr.tr);

          if (sig_triplet) {
            hDtxS[stl]->Fill(dtx);
            hDtyS[stl]->Fill(dty);
          } else {
            hDtxB[stl]->Fill(dtx);
            hDtyB[stl]->Fill(dty);
          }

          if (fVerbose>1) {
            printf("    ");
            if (sig_triplet) printf("+"); else printf(" ");
            printf("(%4i,%4i,%4i)",hl.id,hm.id,hr.id);
            printf("  tx=%5.2f ty=%5.2f",TMath::Abs(txr),TMath::Abs(tyr));
            printf("  dtx=%5.2f dty=%5.2f",dtx,dty);
            printf("\n");
          }

          if (TMath::Abs(tyr)>fTyMax[stm]) continue;
          if (TMath::Abs(txr)>fTxMax[stm]) continue;

          if (dtx>fdTxMax[stl]) continue;
          if (dty>fdTyMax[stl]) continue;

          if (stl==0) fAll++;
          if (sig_triplet && stl==0) fSig++;

         sig = (hl.tr==hm.tr && hm.tr==hr.tr && hm.tr!=-1 && fTools->IsL1accepted(hl.tr)>3);
         //if (!sig) continue;
         CbmKFTrack track;
         C = track.GetCovMatrix();
         T = track.GetTrack();

         for (Int_t i=0;i<5;i++) track.GetTrack()[i]=0;
         track.fHits.clear();
         track.fHits.push_back(hl.GetKFHit());
         track.fHits.push_back(hm.GetKFHit());
         track.fHits.push_back(hr.GetKFHit());
         track.Fit(1);
         chi2 = track.GetRefChi2()/track.GetRefNDF();
         if (chi2>1000) continue;
         track.Fit(1);
         chi2 = track.GetRefChi2()/track.GetRefNDF();
         if (chi2>10) { continue; }

//           printf("4. chi2=%10.2f",chi2);
//         printf(" dx=%6.4f dy=%6.4f dp=%6.4f",sqrt(C[0]),sqrt(C[2]),sqrt(C[14]));
//         printf(" dtx=%6.4f dty=%6.4f",sqrt(C[5]),sqrt(C[9]));
//         printf(" qp=%6.2f",T[4]);

         qp0 = T[4];
         track.Propagate(fSt_z[3],qp0);

//         printf(" dx=%6.4f dy=%6.4f",sqrt(C[0]),sqrt(C[2]));
         x   = T[0];
         y   = T[1];
         dx =  sqrt(C[0]);
         dy =  sqrt(C[2]);
         if (dx>0.16) {
           //printf("\n");
           continue; }
         if (dy>0.10) {
           //printf("\n");
           continue; }

         y_min = y-fNsigma*dy;
         y_max = y+fNsigma*dy;
         x_min = x-fNsigma*dx;
         x_max = x+fNsigma*dx;


         iBest = vHits[3].end();
         s2Best=1000.;

         it = find_if(vHits[3].begin(),vHits[3].end(),GreaterY(y_min));

         iAll=0;

         for (Int_t i=0;i<vHits[3].size();i++){
           Hit h = vHits[3][i];
           if (h.tr!=hl.tr || !sig) continue;
           x0=h.x;
           y0=h.y;
           hDx[3]->Fill(x-x0);
           hDy[3]->Fill(y-y0);
           hDxPull[3]->Fill((x-x0)/dx);
           hDyPull[3]->Fill((y-y0)/dy);
//           printf(" sx=%6.2f sy=%6.2f",(x-x0)/dx,(y-y0)/dy);
         }

         for (vector<Hit>::iterator i = it; i!=vHits[3].end(); i++){
           Hit &h = *i;
           if ( h.y > y_max ) break;
           if ( h.x < x_min ) continue;
           if ( h.x > x_max ) continue;
           Double_t s2 = pow(h.x-x,2)+pow(h.y-y,2);
           if (s2<s2Best) {s2Best=s2;iBest=i;}
           iAll++;
         }

         if (iAll){
           Hit &h = *iBest;
           track.fHits.push_back(h.GetKFHit());
           track.Fit(0);
           track.Fit(1);
           chi2 = track.GetRefChi2()/track.GetRefNDF();
           p=TMath::Abs(1./T[4]);
           printf("Step 1: ");
           if (sig && h.tr==hl.tr) printf("+"); else printf(" ");
           printf(" iAll=%i ",iAll);
           printf("5. chi2=%10.2f",chi2);
           printf(" p= %6.2f",p);
            if ( (chi2>0 && chi2<3 && p>0.20) ) {
             printf("   !");
             n++;
             printf("%4i %4i %4i %4i",hl.tr,hm.tr,hr.tr,h.tr);
           }
           else { printf("\n");continue;}
         printf("\n");
         } else {continue;}

         printf("Step 2: ");

         qp0 = T[4];
         track.Propagate(fSt_z[4],qp0);
//         printf(" dx=%6.4f dy=%6.4f",sqrt(C[0]),sqrt(C[2]));
         x   = T[0];
         y   = T[1];
         z   = T[5];
         dx =  sqrt(C[0]);
         dy =  sqrt(C[2]);
         printf(" x=%6.2f y=%6.2f z=%6.2f",x,y,z);
         if (0 && dx>0.16) {
           //printf("\n");
           continue; }
         if (0 && dy>0.10) {
           //printf("\n");
           continue; }

         y_min = y-fNsigma*dy;
         y_max = y+fNsigma*dy;
         x_min = x-fNsigma*dx;
         x_max = x+fNsigma*dx;


         iBest = vHits[4].end();
         s2Best=1000.;

         it = find_if(vHits[4].begin(),vHits[4].end(),GreaterY(y_min));

         iAll=0;

         for (Int_t i=0;i<vHits[4].size();i++){
           Hit h = vHits[4][i];
           if (h.tr!=hl.tr || !sig) continue;
           x0=h.x;
           y0=h.y;
           hDx[4]->Fill(x-x0);
           hDy[4]->Fill(y-y0);
           hDxPull[4]->Fill((x-x0)/dx);
           hDyPull[4]->Fill((y-y0)/dy);
           printf(" sx=%6.2f sy=%6.2f",(x-x0)/dx,(y-y0)/dy);
         }

         for (vector<Hit>::iterator i = it; i!=vHits[4].end(); i++){
           Hit &h = *i;
           if ( h.y > y_max ) break;
           if ( h.x < x_min ) continue;
           if ( h.x > x_max ) continue;
           Double_t s2 = pow(h.x-x,2)+pow(h.y-y,2);
           if (s2<s2Best) {s2Best=s2;iBest=i;}
           iAll++;
         }

         if (iAll){
           Hit &h = *iBest;
           track.fHits.push_back(h.GetKFHit());
           track.Fit(0);
           track.Fit(1);
           chi2 = track.GetRefChi2()/track.GetRefNDF();
           p=TMath::Abs(1./T[4]);
            if (sig && h.tr==hl.tr) printf("+"); else printf(" ");
             printf(" iAll=%i ",iAll);
           printf("5. chi2=%10.2f",chi2);
           printf(" p= %6.2f",p);
            if ( (chi2>0 && chi2<3 && p>0.20) ) {
             printf("   !");
             printf("%4i %4i %4i %4i",hl.tr,hm.tr,hr.tr,h.tr);
           MarkUsed(hl);
           MarkUsed(hm);
           MarkUsed(hr);
           MarkUsed(*iBest);

           }
         }
         printf("\n");
         printf("\n");

        }// third hit
      }// second hit
    }//first hit
  }//station

  printf("-I- CbmHyperonsFinder: %i tracks\n",fRCTracks);
  printf("nAll=%i\n",fAll);
  printf("nSig=%i\n",fSig);
  printf("n=%i\n",n);
}


// -------------------------------------------------------------------------
void CbmAnaHypSecFinderQa::PrintEventStatistics(){
  Int_t nAC=0;

  for (Int_t i=0;i<mcTrackArray->GetEntriesFast();i++){
    if (fTools->IsL1accepted(i)>3) nAC++;
  }
  printf("Event statistics: ");
  printf("  L1 tracks: %i",nAC);
  printf("  RC tracks: %i",rcTrackArray->GetEntriesFast());
  printf("\n");
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmAnaHypSecFinderQa::PrintMCTrackInfo(){
  printf("MCTracks info:\n");

  for (Int_t i=0;i<mcTrackArray->GetEntriesFast();i++){
    CbmMCTrack* track = (CbmMCTrack*) mcTrackArray->At(i);
    Int_t pdg = track->GetPdgCode();
    TVector3 mom;
    track->GetMomentum(mom);
    Int_t ac = (fTools->IsL1accepted(i)>3);
    if (!ac) continue;
    if (ac) printf("+"); else printf(" ");
    printf("No:%4i  %8i  %6.2f",i,pdg,mom.Mag());
    printf("\n");
  }

}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmAnaHypSecFinderQa::PrintHitInfo(){
  Int_t nUsed[10];

  printf("Hit info:\n");
  for (Int_t st=0;st<fNstations;st++){
    nUsed[st]=0;
    for (unsigned i=0;i<vHits[st].size();i++){
      Hit &h = vHits[st][i];
      //printf("  station:  %i   id= %i",st,h.id);
      //printf("  mcTrack: %2i",h.tr);
      //printf("\n");
      if (h.used==true) nUsed[st]++;
    }
  }


  printf("Hit statistics:\n");
  for (Int_t st=0;st<fNstations;st++){
    Int_t unused=vHits[st].size()-nUsed[st];
    printf("  station: %i  total(used,unused) = %4i(%4i,%4i)\n",
           st,(int) vHits[st].size(),nUsed[st],unused);
  }

}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmAnaHypSecFinderQa::AddHitRefTracks(){
  for (Int_t st=0;st<fNstations;st++){
    for (unsigned i=0;i<vHits[st].size();i++){
      Hit &h = vHits[st][i];
      h.tr=fTools->GetTrackId(h.pt);
    }
  }
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmAnaHypSecFinderQa::Performance(){
  Int_t nAC=0;

  for (Int_t i=0;i<mcTrackArray->GetEntriesFast();i++){
    if (fTools->IsL1accepted(i)>3) nAC++;
  }
  fL1AC+=nAC;


  Int_t nRC = rcTrackArray->GetEntriesFast();
  Int_t nRC_good = 0;
  for (Int_t i=0;i<nRC;i++){
    CbmTrackMatch* match = (CbmTrackMatch*) fTrackMatch->At(i);
    if (fTools->GetTrackId(match)>=0) nRC_good++;
  }
  Int_t nRC_fake = nRC-nRC_good;

  Int_t nSR = newTrackArray->GetEntriesFast();
  Int_t nSR_good = 0;
  for (Int_t i=0;i<nSR;i++){
    CbmStsTrack* track = (CbmStsTrack*) newTrackArray->At(i);
    CbmTrackMatch* match = (CbmTrackMatch*) fTrackMatch->At(i);
    printf("chi2=%8.2f",track->GetChi2()/track->GetNDF());
    if (fTools->GetTrackId(match)>=0) {
      nSR_good++;
      printf("+\n");
    }
    else printf("\n");
  }
  Int_t nSR_fake = nSR-nSR_good;

  Int_t nAR      = nRC+nSR;
  Int_t nAR_good = nRC_good+nSR_good;
  Int_t nAR_fake = nAR-nAR_good;

  fRC+=nRC;  fRC_good+=nRC_good; fRC_fake+=nRC_fake;
  fSR+=nSR;  fSR_good+=nSR_good; fSR_fake+=nSR_fake;
  fAR+=nAR;  fAR_good+=nAR_good; fAR_fake+=nAR_fake;


  printf("=====================\n");
  printf("L1 acc tracks: %5.0f\n",fL1AC);
  printf("L1 track finder info:\n");
  printf("  reco tracks: %5i (%5.3f)\n",fRC,fRC/fL1AC);
  printf("  good tracks: %5i (%5.3f)\n",fRC_good,fRC_good/fL1AC);
  printf("  fake tracks: %5i (%5.3f)\n",fRC_fake,fRC_fake/fL1AC);
  printf("SecFinder info:\n");
  printf("  reco tracks: %5i (%5.3f)\n",fSR,fSR/fL1AC);
  printf("  good tracks: %5i (%5.3f)\n",fSR_good,fSR_good/fL1AC);
  printf("  fake tracks: %5i (%5.3f)\n",fSR_fake,fSR_fake/fL1AC);
  printf("All Finder info:\n");
  printf("  reco tracks: %5i (%5.3f)\n",fAR,fAR/fL1AC);
  printf("  good tracks: %5i (%5.3f)\n",fAR_good,fAR_good/fL1AC);
  printf("  fake tracks: %5i (%5.3f)\n",fAR_fake,fAR_fake/fL1AC);
  printf("=====================\n");
}
// -------------------------------------------------------------------------

void CbmAnaHypSecFinderQa::AnalyseDoubleStrips(){

  for (unsigned i=0;i<vHits[0].size();i++){
    Hit &h0 = vHits[0][i];
    if (h0.tr==-1) { continue; }
    if (fTools->IsL1accepted(h0.tr)<4) continue;
    for (unsigned j=0;j<vHits[1].size();j++){
      Hit &h1 = vHits[1][j];
      if (h0.tr!=h1.tr) continue;
      hTxS[0]->Fill(TMath::Abs((h0.x-h1.x)/(h0.z-h1.z)));
      hTyS[0]->Fill(TMath::Abs((h0.y-h1.y)/(h0.z-h1.z)));
    }
  }

  Double_t txMax=0.5;
  Double_t tyMax=0.5;
  Double_t dx = txMax*fSt_dz[0];
  Double_t dy = txMax*fSt_dz[0];

  printf("dx=%6.4f\n",dx);

  Int_t nSig=0;
  Int_t nFake=0;


  printf("nHits:%lu\n",vHits[0].size());
  for (unsigned i=0;i<vHits[0].size();i++){
    Hit &h0 = vHits[0][i];

    for (unsigned j=0;j<vHits[1].size();j++){
      Hit &h1 = vHits[1][j];

      if (TMath::Abs(h0.x-h1.x) > dx) continue;
      if (TMath::Abs(h0.y-h1.y) > dy) continue;
      if (h0.tr==h1.tr && h0.tr!=-1) nSig++;
      else nFake++;
    }
  }
  printf("nSig:  %4i\n",nSig);
  printf("nFake: %4i\n",nFake);

  fFakeD+=nFake;
  fSigD+=nSig;

  printf("fSigD:  %4i\n",fSigD);
  printf("fFakeD: %4i\n",fFakeD);

}


void CbmAnaHypSecFinderQa::AddNextStationAna(CbmKFTrackSec &track, Int_t st){
  if (st>=fNstations) return;
  CbmKFTrackSec tr(track);
  Double_t qp0 = tr.GetTrack()[4];
  tr.Propagate(fSt_z[st],qp0);
  Double_t x   = tr.GetTrack()[0];
  Double_t y   = tr.GetTrack()[1];
  Double_t dx =  fNsigma*sqrt(tr.GetCovMatrix()[0]);
  Double_t dy =  fNsigma*sqrt(tr.GetCovMatrix()[2]);
/*
  Int_t sig = track.Sig();
  Int_t nHits = track.hits.size();

  if (0 && sig) printf("nHits: %i\n",nHits);
  if (0 && sig){
    printf(" tr=%4i ",track.hits[0]->tr);
    printf(" x=%6.3f y=%6.3f dx=%6.3f dy=%6.3f ",x,y,dx,dy);
    Int_t iSig = -1;
    for (unsigned i=0;i<vHits[st].size();i++){
      if (track.hits[0]->tr==vHits[st][i].tr) iSig=i;
    }
    if (iSig>=0){
      Double_t x0 = vHits[st][iSig].x;
      Double_t y0 = vHits[st][iSig].y;
      printf(" x0=%6.3f y0=%6.3f",x0,y0);
      hDx[nHits]->Fill(x-x0);
      hDy[nHits]->Fill(y-y0);
      hDxPull[nHits]->Fill((x-x0)*fNsigma/dx);
      hDyPull[nHits]->Fill((y-y0)*fNsigma/dy);
    }
    printf("\n");
  }
*/
  Double_t y_min = y-dy;
  Double_t y_max = y+dy;
  Double_t x_min = x-dx;
  Double_t x_max = x+dx;

  vector<Hit>::iterator iBest = vHits[st].end();
  Double_t s2Best=1000.;

  vector<Hit>::iterator it =
    find_if(vHits[st].begin(),vHits[st].end(),GreaterY(y_min));

  Int_t iAll=0;
  for (vector<Hit>::iterator i = it; i!=vHits[st].end(); i++){
    Hit &h = *i;
    if ( h.y > y_max ) break;
    if ( h.x < x_min ) continue;
    if ( h.x > x_max ) continue;
    Double_t s2 = pow(h.x-x,2)+pow(h.y-y,2);
    if (s2<s2Best) {s2Best=s2;iBest=i;}
    iAll++;
  }

  if (iBest==vHits[st].end()) return;
/*
  h.Filter(track,1);
*/
  Hit &h = *iBest;
  track.fHits.push_back(h.GetKFHit());
  track.hits.push_back(&h);
  track.Fit(1);
  Double_t chi2=track.GetRefChi2()/track.GetRefNDF();
  printf("iAll:%i  ",iAll);
  printf("nHits:%lu  ",track.hits.size());
  if   (track.Sig()) printf("+Fit!!! chi2=%6.2f\n",chi2);
  else printf(" Fit!!! chi2=%6.2f\n",chi2);
  h.used=true;
  //printf("%i",track.hits.size());
  //if (track.Sig()) track.Print();
  //AddNextStationAna(track,st+1);
}
// -------------------------------------------------------------------------

void CbmAnaHypSecFinderQa::AnalysePulls(){
//TODO AnalysePulls
//  Double_t x , y, qp;
//  Double_t x0,y0,qp0;
//  Double_t dx,dy,dqp;
//  TVector3 mom;
//
//  Int_t nSR = newTrackArray->GetEntriesFast();
//  for (Int_t i=0;i<nSR;i++){
//    CbmStsTrack* track = (CbmStsTrack*) newTrackArray->At(i);
//    if (fTools->GetTrackId(track)<0) continue;
//    Int_t mcTrackId = fTools->GetTrackId(track);
//    CbmMCTrack* mcTrack = (CbmMCTrack*) mcTrackArray->At(mcTrackId);
//    Int_t iLast = -1+track->GetNStsHits();
//    CbmHit* hit = (CbmHit*) sHitArray->At(track->GetStsHitIndex(iLast));
//
//    Int_t mcPointId = hit->GetRefIndex();
//    if (mcPointId<0) continue;
//    CbmStsPoint* mcPoint = (CbmStsPoint*) mcPointArray->At(mcPointId);
//
//    /*
//    x0 = mcPoint->GetXOut();
//    y0 = mcPoint->GetYOut();
//    mcPoint->MomentumOut(mom);
//    */
//
//    x0 = hit->GetX();
//    y0 = hit->GetY();
//    mcPoint->MomentumOut(mom);
//
//    qp0 = 1./mom.Mag();
//
//    FairTrackParam* par = track->GetParamLast();
//    x  = par->GetX();
//    y  = par->GetY();
//    qp = TMath::Abs(par->GetQp());
//    dx  = sqrt(par->GetCovariance(0,0));
//    dy  = sqrt(par->GetCovariance(1,1));
//    dqp = sqrt(par->GetCovariance(4,4));
//
//    hPullX ->Fill((x  -  x0)/dx);
//    hPullY ->Fill((y  -  y0)/dy);
//    hPullQp->Fill((qp - qp0)/dqp);
//  }
//
}

// -------------------------------------------------------------------------
Int_t CbmAnaHypSecFinderQa::CreateStsTrackAna(CbmKFTrackSec track){
/*
  track.Fit(0);
  track.Fit(1);
  track.Fit(0);
  track.Fit(1);
*/

  CbmKFTrackSec tr;
  for (unsigned i=0;i<track.hits.size();i++){
    tr.fHits.push_back(track.hits[i]->GetKFHit());
    tr.hits.push_back(track.hits[i]);
  }
  tr.Fit( 1 ); // fit downstream
  tr.Fit( 0 ); // fit upstream
  tr.Fit( 1 ); // fit downstream

  Int_t sig = tr.Sig();
  Int_t nHits = tr.hits.size();

  if (sig && nHits==6){
    Int_t iSig = -1;
    for (unsigned i=0;i<vHits[5].size();i++){
      if (track.hits[0]->tr==vHits[5][i].tr) iSig=i;
    }
    if (iSig>=0){
      Double_t qp0 = tr.GetTrack()[4];
      tr.Propagate(vHits[5][iSig].z,qp0);
      Double_t x   = tr.GetTrack()[0];
      Double_t y   = tr.GetTrack()[1];
      Double_t dx =  sqrt(tr.GetCovMatrix()[0]);
      Double_t dy =  sqrt(tr.GetCovMatrix()[2]);

      Double_t x0 = vHits[5][iSig].x;
      Double_t y0 = vHits[5][iSig].y;
      printf(" x0=%6.3f y0=%6.3f",x0,y0);
      hDx[6]->Fill(x-x0);
      hDy[6]->Fill(y-y0);
      hDxPull[6]->Fill((x-x0)/dx);
      hDyPull[6]->Fill((y-y0)/dy);
    }
    printf("\n");
  }

  CbmStsTrack stsTrack;
  stsTrack.SetPidHypo(track.GetTrack()[4]>=0 ? 211 : -211);
  for (unsigned i=0;i<track.hits.size();i++){
    Hit h = *track.hits[i];
//TODO    if      (h.type==0) stsTrack.AddMvdHit(h.id,(CbmMvdHit*)  h.pt);
    if (h.type==1) stsTrack.AddStsHit(h.id,(CbmStsHit*)h.pt);
  }
  stsTrack.SortHits();
  fFitter->DoFit(&stsTrack);


/*
  CbmStsTrack stsTrack1;
  stsTrack1.SetPidHypo(track.GetTrack()[4]>=0 ? 211 : -211);
  for (unsigned i=0;i<track.hits.size();i++){
    Hit h = *track.hits[i];
    if      (h.type==0) stsTrack1.AddMapsHit  (h.id,(CbmStsMapsHit*)  h.pt);
    else if (h.type==1) stsTrack1.AddHybridHit(h.id,(CbmStsHybridHit*)h.pt);
    else if (h.type==2) stsTrack1.AddStripHit (h.id,(CbmStsStripHit*) h.pt);
  }
  stsTrack1.SortHits();
  T.GetTrackParam( *stsTrack1.GetParamLast() ); // store fitted track & cov.matrix
  T.Fit( 0 ); // fit upstream
  T.GetStsTrack( stsTrack1, 1 );          // store fitted track & cov.matrix & chi2 &
*/


  if (stsTrack.GetChi2()/stsTrack.GetNDF()>5) return 0;
  new((*newTrackArray)[fRCTracks++]) CbmStsTrack(stsTrack);
  return 1;
}
// -------------------------------------------------------------------------

