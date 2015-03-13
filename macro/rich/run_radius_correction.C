/*  Description: This macro calculate MAP for radius correction
    To use this macro, first run CbmRichRingQa task.
    It sets recFlag to the rings.

    Author : Simeon Lebedev
    E-mail : S.Lebedev@gsi.de
*/
#include <vector>
#include "../../littrack/utils/CbmLitDrawHist.cxx"
#include "../../littrack/utils/CbmLitUtils.cxx"
void run_radius_correction ()
{
    TStopwatch timer;
    timer.Start();

    gStyle->SetPalette(1,0);
    gStyle->SetHistLineWidth(2);

    // ----  Load libraries   -------------------------------------------------
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gROOT->LoadMacro("$VMCWORKDIR/macro/rich/cbmlibs.C");
    cbmlibs();

   // gROOT->LoadMacro("$VMCWORKDIR/macro/rich/setstyle.C");
   // setphdStyle();
    SetStyles();

    char fileMC[200], fileRec[200];

    sprintf(fileMC,"/d/cbm02/slebedev/rich/JUL09/correction/mc.00.root");
    cout<<fileMC<<endl;
    TFile *f1 = new TFile(fileMC,"R");
    TTree* t1 = f1->Get("cbmsim");
    TFolder *fd1 = f1->Get("cbmroot");
    TClonesArray* fMCTracks = (TClonesArray*) fd1->FindObjectAny("MCTrack");
    t1->SetBranchAddress(fMCTracks->GetName(),&fMCTracks);

    sprintf(fileRec, "/d/cbm02/slebedev/rich/JUL09/correction/reco.00.root");
    TFile *f = new TFile(fileRec,"R");
    TTree* t = f->Get("cbmsim");
    TFolder *fd = f->Get("cbmout");
    TClonesArray *fRichRings = (TClonesArray*) fd->FindObjectAny("RichRing");
    t->SetBranchAddress(fRichRings->GetName(),&fRichRings);
    TClonesArray *fRichMatches = (TClonesArray*) fd->FindObjectAny("RichRingMatch");
    t->SetBranchAddress(fRichMatches->GetName(),&fRichMatches);

    //Int_t fNofBinsX = 40;
    //Int_t fNofBinsY = 50;
    Int_t fNofBinsX = 25;
    Int_t fNofBinsY = 25;
    ///A axis
    TH2D* fh_axisAXYCount;
    TH2D* fh_axisAXYW;
    TH2D* fh_axisAXY;
    TH2D* fh_axisASigma;
    TH2D* mapaxisAXY;


    ///B axis
    TH2D* fh_axisBXYCount;
    TH2D* fh_axisBXYW;
    TH2D* fh_axisBXY;
    TH2D* fh_axisBSigma;
    TH2D* mapaxisBXY;

    mapaxisAXY = new TH2D("fh_mapaxisAXY","dA distribution (x,y);X, [cm];Y, [cm]",fNofBinsX,-200,200,fNofBinsY,-250,250);
    mapaxisBXY = new TH2D("fh_mapaxisBXY","dB distribution (x,y);X, [cm];Y, [cm]",fNofBinsX,-200,200,fNofBinsY,-250,250);
    fh_axisAXYCount = new TH2D("fh_axisAXYCount","A Count",fNofBinsX,-200,200,fNofBinsY,-250,250);
    fh_axisAXYW = new TH2D("fh_axisAXYW","",fNofBinsX,-200,200,fNofBinsY,-250,250);
    fh_axisBXYCount = new TH2D("fh_axisBXYCount","B Count",fNofBinsX,-200,200,fNofBinsY,-250,250);
    fh_axisBXYW = new TH2D("fh_axisBXYW","",fNofBinsX,-200,200,fNofBinsY,-250,250);
    fh_axisAXY = new TH2D("fh_axisAXY","A distribution (x,y);X, [cm];Y, [cm]",fNofBinsX,-200,200,fNofBinsY,-250,250);
    fh_axisBXY = new TH2D("fh_axisBXY","B distribution (x,y);X, [cm];Y, [cm]",fNofBinsX,-200,200,fNofBinsY,-250,250);

    Double_t fMinAaxis = 4.5;
    Double_t fMaxAaxis = 7.5;

    ///Set Mean value of A and B axeses, Compact RICH
    //Double_t fMeanAaxis = 5.06;
    //Double_t fMeanBaxis = 4.65;

    ///Set Mean value of A and B axeses, Large RICH
    Double_t fMeanAaxis = 6.17;
    Double_t fMeanBaxis = 5.6;

    Int_t nEvents=t->GetEntries();
    cout<<" nEvents ="<<nEvents<<endl;
    for(Int_t ievent=0;ievent<nEvents; ievent++ ) {
        cout<<"ievent = "<<ievent;
        CbmRichRing *ring=NULL;
        CbmRichRingMatch *match=NULL;
        t->GetEntry(ievent);
        t1->GetEntry(ievent);
        Int_t nofRings = fRichRings->GetEntries();
        cout<<"  nofRings = "<<nofRings;
        cout<<"  nofMatches = "<< fRichMatches->GetEntries() ;
        cout<<"  nofMCTracks = "<<fMCTracks->GetEntries() << endl;

        for(Int_t iRing=0; iRing < nofRings; iRing++){
            ring = (CbmRichRing*)fRichRings->At(iRing);
            if (!ring) continue;
            match = (CbmRichRingMatch*)fRichMatches->At(iRing);
            if (!match) continue;

            Int_t trackId = match->GetMCTrackID();
            if (trackId == -1) continue;
            if (trackId > fMCTracks->GetEntries()) continue;

            CbmMCTrack* mcTrack = (CbmMCTrack*)fMCTracks->At(trackId);
            if (!mcTrack) continue;
            Int_t pdg = TMath::Abs(mcTrack->GetPdgCode());
            Int_t motherId = mcTrack->GetMotherId();
            if (pdg != 11) continue;
            if (motherId != -1) continue;

            Double_t radius = ring->GetRadius();
            Double_t axisA = ring->GetAaxis();
            Double_t axisB = ring->GetBaxis();
            Double_t centerX = ring->GetCenterX();
            Double_t centerY = ring->GetCenterY();

            if (axisA > fMaxAaxis || axisB > fMaxAaxis) continue;
            if (axisA < fMinAaxis || axisB < fMinAaxis) continue;

            fh_axisAXYW->Fill(centerX, centerY, axisA);
            fh_axisAXYCount->Fill(centerX, centerY);

            fh_axisBXYW->Fill(centerX, centerY, axisB);
            fh_axisBXYCount->Fill(centerX, centerY);
        } //iRing
    } //iEvent

    fh_axisAXY->Divide(fh_axisAXYW,fh_axisAXYCount);
    fh_axisBXY->Divide(fh_axisBXYW,fh_axisBXYCount);


///create two correction maps
    for (Int_t iX = 1; iX < mapaxisAXY->GetNbinsX() + 1; iX++){
        for (Int_t iY = 1; iY < mapaxisAXY->GetNbinsY() + 1; iY++){
        	if (fh_axisAXYCount->GetBinContent(iX, iY) != 0){
        		mapaxisAXY->SetBinContent(iX, iY, fMeanAaxis - fh_axisAXY->GetBinContent(iX, iY) );
        	} else {
        		mapaxisAXY->SetBinContent(iX, iY, -99999999.);
        	}

        	if (fh_axisBXYCount->GetBinContent(iX, iY) != 0){
        		mapaxisBXY->SetBinContent(iX, iY, fMeanBaxis - fh_axisBXY->GetBinContent(iX, iY) );
        	} else {
        		mapaxisBXY->SetBinContent(iX, iY, -99999999.);
        	}
        }
    }

    c1_0 = new TCanvas("c1_0","c1_0",10,10,600,600);
    c1_0->Divide(1,2);
    c1_0->cd(1);
    fh_axisAXYCount->Draw("COLZ");
    c1_0->cd(2);
    fh_axisBXYCount->Draw("COLZ");

    c1 = new TCanvas("c1","c1",10,10,600,600);
    c1->Divide(1,2);
    c1->cd(1);
    fh_axisAXY->SetMinimum(5.0);
    fh_axisAXY->SetMaximum(6.4);
    fh_axisAXY->Draw("COLZ");
    c1->cd(2);
    fh_axisBXY->SetMinimum(5.0);
    fh_axisBXY->SetMaximum(6.0);
    fh_axisBXY->Draw("COLZ");

    c2 = new TCanvas("c2","c2",10,10,600,600);
    c2->Divide(1,2);
    c2->cd(1);
    mapaxisAXY->SetMinimum(-0.5);
    mapaxisAXY->SetMaximum(0.5);
    mapaxisAXY->Draw("COLZ");
    c2->cd(2);
    mapaxisBXY->SetMinimum(-0.5);
    mapaxisBXY->SetMaximum(0.5);
    mapaxisBXY->Draw("COLZ");


///// Check correction procedure
    TH1D* fh_Abefore = new TH1D("fh_Abefore","A before;A, [cm];yield", 300, 0., 9.);;
    TH1D* fh_Bbefore= new TH1D("fh_Bbefore","B before;B, [cm];yield", 300, 0., 9.);;

    TH1D* fh_A = new TH1D("fh_A","A after;A, [cm];yield", 300, 0., 9.);;
    TH1D* fh_B = new TH1D("fh_B","B after;B, [cm];yield", 300, 0., 9.);;

    cout <<"Check correction procedure......" << endl;
    for(Int_t ievent=0;ievent<nEvents;ievent++ ) {
        CbmRichRing *ring=NULL;
       // if (ievent % 100 == 0) cout << ievent << "   ";
        //t1->GetEntry(ievent);
        t->GetEntry(ievent);
        t1->GetEntry(ievent);
        Int_t nofRings = fRichRings->GetEntries();

        for(Int_t iRing=0; iRing < nofRings; iRing++){

            ring = (CbmRichRing*)fRichRings->At(iRing);
            if (!ring) continue;
            match = (CbmRichRingMatch*)fRichMatches->At(iRing);
            if (!match) continue;

            Int_t trackId = match->GetMCTrackID();
            if (trackId == -1) continue;
            if (trackId > fMCTracks->GetEntries()) continue;

            CbmMCTrack* mcTrack = (CbmMCTrack*)fMCTracks->At(trackId);
            if (!mcTrack) continue;
            Int_t pdg = TMath::Abs(mcTrack->GetPdgCode());
            Int_t motherId = mcTrack->GetMotherId();
            if (pdg != 11) continue;
            if (motherId != -1) continue;

            Double_t axisA = ring->GetAaxis();
            Double_t axisB = ring->GetBaxis();
            if (axisA > fMaxAaxis || axisB > fMaxAaxis) continue;
            if (axisA < fMinAaxis || axisB < fMinAaxis) continue;

            Double_t radius = ring->GetRadius();
            Double_t centerX = ring->GetCenterX();
            Double_t centerY = ring->GetCenterY();
            Double_t axisAbefore = ring->GetAaxis();
            Double_t axisBbefore = ring->GetBaxis();
            fh_Abefore->Fill(axisAbefore);
            fh_Bbefore->Fill(axisBbefore);

            Double_t axisA = ring->GetAaxis();
            Double_t axisB = ring->GetBaxis() ;

            axisA += mapaxisAXY->GetBinContent(mapaxisAXY->FindBin(centerX,centerY));
            axisB += mapaxisBXY->GetBinContent(mapaxisBXY->FindBin(centerX,centerY));

            fh_A->Fill(axisA);
            fh_B->Fill(axisB);
        } //iRing
    }//iEvent


  //  gStyle->SetOptStat(0);
    c3 = new TCanvas("c3","c3",10,10,600,600);
    c3->Divide(2,2);
    c3->cd(1);
    fh_Abefore->Scale(1./fh_Abefore->Integral());
    fh_Abefore->SetMaximum(fh_Abefore->GetMaximum()*1.3);
    fh_Abefore->Draw();
    fh_Abefore->SetAxisRange(fMinAaxis, fMaxAaxis);
    fh_Abefore->Fit("gaus");
    Double_t sigmaAb = fh_Abefore->GetFunction("gaus")->GetParameter("Sigma");
    char sigmaTxtAb[30];
    sprintf(sigmaTxtAb,"sigma = %.3f",sigmaAb);
    TText* txtAb = new TText(4.3, fh_Abefore->GetMaximum()*0.85, sigmaTxtAb);
    txtAb->SetTextSize(0.1);
    txtAb->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c3->cd(2);
    fh_Bbefore->Scale(1./fh_Bbefore->Integral());
    fh_Bbefore->SetMaximum(fh_Bbefore->GetMaximum()*1.3);
    fh_Bbefore->Draw();
    fh_Bbefore->SetAxisRange(fMinAaxis, fMaxAaxis);
    fh_Bbefore->Fit("gaus");
    Double_t sigmaBb = fh_Bbefore->GetFunction("gaus")->GetParameter("Sigma");
    char sigmaTxtBb[30];
    sprintf(sigmaTxtBb,"sigma = %.3f",sigmaBb);
    TText* txtBb = new TText(4.3, fh_Bbefore->GetMaximum()*0.85, sigmaTxtBb);
    txtBb->SetTextSize(0.1);
    txtBb->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c3->cd(3);
    fh_A->Scale(1./fh_A->Integral());
    fh_A->SetMaximum(fh_A->GetMaximum()*1.3);
    fh_A->SetAxisRange(fMinAaxis, fMaxAaxis);
    fh_A->Draw();
    fh_A->Fit("gaus");
    Double_t sigmaA = fh_A->GetFunction("gaus")->GetParameter("Sigma");
    char sigmaTxtA[30];
    sprintf(sigmaTxtA,"sigma = %.3f",sigmaA);
    TText* txtA = new TText(4.3, fh_A->GetMaximum()*0.85, sigmaTxtA);
    txtA->SetTextSize(0.1);
    txtA->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);

    c3->cd(4);
    fh_B->Scale(1./fh_B->Integral());
    fh_B->SetMaximum(fh_B->GetMaximum()*1.3);
    fh_B->SetAxisRange(fMinAaxis, fMaxAaxis);
    fh_B->Draw();
    fh_B->Fit("gaus");
    Double_t sigmaB = fh_B->GetFunction("gaus")->GetParameter("Sigma");
    char sigmaTxtB[30];
    sprintf(sigmaTxtB,"sigma = %.3f",sigmaB);
    TText* txtB = new TText(4.3, fh_B->GetMaximum()*0.85, sigmaTxtB);
    txtB->SetTextSize(0.1);
    txtB->Draw();
    gPad->SetGridx(true);
    gPad->SetGridy(true);


/// Write correction map to the file
    TFile *file = new TFile("radius_correction_map.root", "recreate");
    mapaxisAXY->Write();
    mapaxisBXY->Write();
    file->Close();

}
