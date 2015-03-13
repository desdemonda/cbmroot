void create_histo_la(Int_t file_nr=0){
  Int_t iVerbose = 2;
  Double_t mMin = 1.10;
  Double_t mMax = 1.13;
  Double_t m0   = 1.115683;
  Double_t dm   = 0.00135;
  Double_t cutChi1 = 4.40;
  Double_t cutChi2 = 4.00;
  Double_t cutB1   = 0.00;
  Double_t cutB2   = 0.00;
  Double_t cutChi  = 8.20;
  Double_t cutDca  = 0.228;
  Double_t cutPca  = 2.00;
  Double_t cutBmo  = 0.256;
  Double_t cutMmin = m0-2*dm;
  Double_t cutMmax = m0+2*dm;
  Double_t cutPcaMax = 50;

  TString title;

  Double_t b1Max   = 0.5;
  Double_t b2Max   = 2.5;
  Double_t chi1Max = 20.0;
  Double_t chi2Max = 20.0;
  Double_t chiMax  = 10.0;
  Double_t dcaMax  = 0.40;
  Double_t pcaMax  = 20.;
  Double_t bmoMax  = 0.40;

  Char_t filenr[4];
  sprintf(filenr,"%04d",file_nr);
  printf("Filenr: %s\n", filenr);

  TString signal = "la";

  TString inDir = TString(filenr);
  inDir+="/";
  TString mcFile     = inDir+signal+".mc.root";
  TString parFile    = inDir+signal+".par.root";
  TString rcFile     = inDir+signal+".rc.root";
  TString anaFile    = inDir+signal+".ana.root";
  TString histoFile  = inDir+signal+".histo.root";

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/analysis/hyperon/analysislibs.C");
  analysislibs();

  TClonesArray* fMCTracks  = new TClonesArray("CbmMCTrack",10);
  TClonesArray* fRCTracks  = new TClonesArray("CbmStsTrack",10);
  TClonesArray* fRCMatch   = new TClonesArray("CbmStsTrackMatch",10);
  TClonesArray* fV0Tracks  = new TClonesArray("CbmAnaHypPair",10);

  TFile* f1 = TFile::Open(mcFile);
  TFile* f2 = TFile::Open(rcFile);
  TFile* f3 = TFile::Open(anaFile);
  if (!f1 || !f2 || !f3) return;
  TTree* t1 = (TTree*) f1->Get("cbmsim");
  TTree* t2 = (TTree*) f2->Get("cbmsim");
  TTree* t3 = (TTree*) f3->Get("cbmsim");
  if (!t1 || !t2 || !t3) return;

  t1->SetBranchAddress("MCTrack",&fMCTracks);
  t2->SetBranchAddress("STSTrack",&fRCTracks);
  t2->SetBranchAddress("STSTrackMatch",&fRCMatch);
  t3->SetBranchAddress("V0Candidates",&fV0Tracks);

  Int_t nEvents = t3->GetEntries();

  TH1D* hBgdK0M    = new TH1D("hBgdK0M"  ,"",50,0.48,0.52);
  TH1D* hSigK0M    = new TH1D("hSigK0M"  ,"",50,0.48,0.52);
  TH1D* hSigK0MLS  = new TH1D("hSigK0MLS","",60,0.30,0.60);
  TH2D* hAP        = new TH2D("hAP","",50,-1,1,50,0.,0.25);

  Int_t nBins = 100;
  title = ";b_{p} [cm];b_{#pi^{ -}} [cm]";
  TH2D* hBgdB1B2   = new TH2D("hBgdB1B2"  ,title,nBins,0.,b1Max,nBins,0.,b2Max);
  TH2D* hSigB1B2   = new TH2D("hSigB1B2"  ,title,nBins,0.,b1Max,nBins,0.,b2Max);
  title = ";#chi^{2}_{p};#chi^{2}_{#pi^{ -}}";
  TH2D* hBgdChi1Chi2   = new TH2D("hBgdChi1Chi2"  ,title,nBins,0.,chi1Max,nBins,0.,chi2Max);
  TH2D* hSigChi1Chi2   = new TH2D("hSigChi1Chi2"  ,title,nBins,0.,chi1Max,nBins,0.,chi2Max);
  title = ";#chi^{2};Distance of closest approach [cm]";
  TH2D*   hBgdChiDca = new TH2D("hBgdChiDca",title,nBins,0.,chiMax,nBins,0.,dcaMax);
  TH2D*   hSigChiDca = new TH2D("hSigChiDca",title,nBins,0.,chiMax,nBins,0.,dcaMax);
  title = ";z-vertex [cm];b_{#Xi^{ -}} [cm]";
  TH2D*   hBgdPcaBmo = new TH2D("hBgdPcaBmo",title,nBins,0.,pcaMax,nBins,0.,bmoMax);
  TH2D*   hSigPcaBmo = new TH2D("hSigPcaBmo",title,nBins,0.,pcaMax,nBins,0.,bmoMax);
  title = ";M_{inv} [GeV/c^{2}];pairs / 0.6 MeV";
  TH1D*   hAllM         = new TH1D("hAllM"        ,title,50,mMin,mMax);
  TH1D*   hBgdM         = new TH1D("hBgdM"        ,title,50,mMin,mMax);
  TH1D*   hSigM         = new TH1D("hSigM"        ,title,50,mMin,mMax);
  TH1D*   hAllB1B2M     = new TH1D("hAllB1B2M"    ,title,50,mMin,mMax);
  TH1D*   hBgdB1B2M     = new TH1D("hBgdB1B2M"    ,title,50,mMin,mMax);
  TH1D*   hSigB1B2M     = new TH1D("hSigB1B2M"    ,title,50,mMin,mMax);
  TH1D*   hAllChi1Chi2M = new TH1D("hAllChi1Chi2M",title,50,mMin,mMax);
  TH1D*   hBgdChi1Chi2M = new TH1D("hBgdChi1Chi2M",title,50,mMin,mMax);
  TH1D*   hSigChi1Chi2M = new TH1D("hSigChi1Chi2M",title,50,mMin,mMax);
  TH1D*   hAllChiDcaM   = new TH1D("hAllChiDcaM"  ,title,50,mMin,mMax);
  TH1D*   hBgdChiDcaM   = new TH1D("hBgdChiDcaM"  ,title,50,mMin,mMax);
  TH1D*   hSigChiDcaM   = new TH1D("hSigChiDcaM"  ,title,50,mMin,mMax);
  TH1D*   hAllPcaBmoM   = new TH1D("hAllPcaBmoM"  ,title,50,mMin,mMax);
  TH1D*   hBgdPcaBmoM   = new TH1D("hBgdPcaBmoM"  ,title,50,mMin,mMax);
  TH1D*   hSigPcaBmoM   = new TH1D("hSigPcaBmoM"  ,title,50,mMin,mMax);


  for (Int_t iEv=0;iEv<nEvents;iEv++){
    if (iVerbose>3 || iEv%1==0) printf("%4i\n",iEv);

  //  t1->GetEntry(iEv);
  //  t2->GetEntry(iEv);
    t3->GetEntry(iEv);

    for (Int_t i=0;i<fV0Tracks->GetEntriesFast();i++){
      CbmAnaHypPair* mo = (CbmAnaHypPair*) fV0Tracks->At(i);
      Bool_t   sig  = mo->fSig>=0;
      Double_t b1   = mo->fB1;
      Double_t b2   = mo->fB2;
      Double_t chi1 = mo->fChi1;
      Double_t chi2 = mo->fChi2;
      Double_t dca  = mo->fDca;
      Double_t chi  = mo->fChi;
      Double_t pca  = mo->fPca;
      Double_t bmo  = mo->fBmo;
      Double_t m    = mo->fMass;

      if (iVerbose>3 || sig) {
        if (sig) printf("+"); else printf(" ");
        printf("  (%4i,%4i) m=%7.3f",mo->fId1,mo->fId2,m);
        printf("  b1=%7.3f b2=%7.3f",b1*10,b2*10);
        printf("  chi1=%7.3f chi2=%7.3f",chi1,chi2);
        printf("  chi=%6.2f dca=%7.3f",chi,dca);
        printf("  pca=%6.2f bmo=%7.3f",pca,bmo);
        printf("\n");
      }

      if (sig) {
        hSigM->Fill(m);
        if (chi1>cutChi1 && chi2>cutChi2){
          hSigChi1Chi2M->Fill(m);
          if (b1>cutB1 && b2>cutB2){
            hSigB1B2M->Fill(m);
            if (dca<cutDca && chi<cutChi) {
              hSigChiDcaM->Fill(m);
              if (pca>cutPca && pca<cutPcaMax && bmo<cutBmo){
                hSigPcaBmoM->Fill(m);
              }
            }
          }
        }
      } else {
        hBgdM->Fill(m);
        if (chi1>cutChi1 && chi2>cutChi2){
          hBgdChi1Chi2M->Fill(m);
          if (b1>cutB1 && b2>cutB2){
            hBgdB1B2M->Fill(m);
            if (dca<cutDca && chi<cutChi) {
              hBgdChiDcaM->Fill(m);
              if (pca>cutPca && pca<cutPcaMax && bmo<cutBmo){
                hBgdPcaBmoM->Fill(m);
              }
            }
          }
        }
      }

      if (!(m>cutMmin && m<cutMmax)) continue;

      if (sig) hSigChi1Chi2->Fill(chi1,chi2);
      else     hBgdChi1Chi2->Fill(chi1,chi2);

      if (!(chi1>cutChi1 && chi2>cutChi2)) continue;

      if (sig) hSigB1B2->Fill(b1,b2);
      else     hBgdB1B2->Fill(b1,b2);

      if (!(b1>cutB1 && b2>cutB2)) continue;

      if (sig) hSigChiDca->Fill(chi,dca);
      else     hBgdChiDca->Fill(chi,dca);

      if (!(dca<cutDca && chi<cutChi)) continue;

      if (sig) hSigPcaBmo->Fill(pca,bmo);
      else     hBgdPcaBmo->Fill(pca,bmo);

      if (!(pca>cutPca && pca<cutPcaMax && bmo<cutBmo)) continue;
    }//V0 candidates
  }//events

  TFile* f = new TFile(histoFile,"recreate");
  hBgdB1B2     ->Write();
  hSigB1B2     ->Write();
  hBgdChi1Chi2 ->Write();
  hSigChi1Chi2 ->Write();
  hBgdChiDca   ->Write();
  hSigChiDca   ->Write();
  hBgdPcaBmo   ->Write();
  hSigPcaBmo   ->Write();
  hAllM        ->Write();
  hBgdM        ->Write();
  hSigM        ->Write();
  hBgdChi1Chi2M->Write();
  hSigChi1Chi2M->Write();
  hAllB1B2M    ->Write();
  hBgdB1B2M    ->Write();
  hSigB1B2M    ->Write();
  hAllChiDcaM  ->Write();
  hBgdChiDcaM  ->Write();
  hSigChiDcaM  ->Write();
  hAllPcaBmoM  ->Write();
  hBgdPcaBmoM  ->Write();
  hSigPcaBmoM  ->Write();
  f->Close();
}
