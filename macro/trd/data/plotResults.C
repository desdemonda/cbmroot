#include <string.h>
#include "TChain.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include "TKey.h"
#include "Riostream.h"
#include "TString.h"
#include "TDirectory.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "Riostream.h"
#include "TRint.h"
#include "TROOT.h"
#include "TStyle.h"
#include <map>
#include "TBox.h"

TFile *Target;

std::map<TString, TH1*> fHistoMap;
std::map<TString, TH1*>::iterator fHistoMapIt;

void FindHistos(TDirectory *target);

void plotResults(TString filename = "result_0100Pi0.root", TString picsPath = "pics/"/*"pics0100pi0/"*/){
  gROOT->Reset(); 
  gStyle->SetPalette(1,0);
  gROOT->SetStyle("Plain");
  gStyle->SetPadTickX(1);                        
  gStyle->SetPadTickY(1);  
  gStyle->SetOptStat(kFALSE);
  gStyle->SetOptTitle(kFALSE);
  TH1::SetDefaultSumw2();
  Float_t max(0), min(1), nEntries(0);
  Bool_t foundAll = true;
 
  Target = TFile::Open(filename, "READ");
  FindHistos(Target);

  TBox *pi0 = new TBox(0.125,0,0.145,0.002);
  pi0->SetLineColor(2);
  pi0->SetFillStyle(0);
  TCanvas *c = new TCanvas("c","c",800,600);
  c->Divide(1,1);
  c->cd(1);
  TLegend *legUR = new TLegend(0.6,0.7,0.85,0.85);
  legUR->SetLineColor(0);
  legUR->SetLineStyle(0);
  legUR->SetFillStyle(0);
  legUR->SetTextSize(0.03);

  TLegend *legLR = new TLegend(0.6,0.15,0.85,0.30);
  legLR->SetLineColor(0);
  legLR->SetLineStyle(0);
  legLR->SetFillStyle(0);
  legLR->SetTextSize(0.03);
  TLegend *legUL = new TLegend(0.15,0.7,0.4,0.85);
  legUL->SetLineColor(0);
  legUL->SetLineStyle(0);
  legUL->SetFillStyle(0);
  legUL->SetTextSize(0.03);

  TLegend *legLL = new TLegend(0.15,0.15,0.4,0.30);
  legLL->SetLineColor(0);
  legLL->SetLineStyle(0);
  legLL->SetFillStyle(0);
  legLL->SetTextSize(0.03);
  
  c->cd(1)->SetLogx(0);
  c->cd(1)->SetLogy(0);
  c->cd(1)->SetLogz(0);
  /*
    fBirthGamma->Draw("");
    TString name = fBirthGamma->GetName();
    c->SaveAs(TString(picsPath + "" + name + ".pdf"));
    c->SaveAs(TString(picsPath + "" + name + ".png"));
    fBirthPi0->Draw("");
    name = fBirthPi0->GetName();
    c->SaveAs(TString(picsPath + "" + name + ".pdf"));
    c->SaveAs(TString(picsPath + "" + name + ".png"));
    fBirthPair->Draw("");
    name = fBirthPair->GetName();
    c->SaveAs(TString(picsPath + "" + name + ".pdf"));
    c->SaveAs(TString(picsPath + "" + name + ".png"));
  */
  legUR->AddEntry(fHistoMap["GT_STS_Eff"],"STS","LEP");
  legUR->AddEntry(fHistoMap["GT_RICH_Eff"],"RICH","LEP");
  legUR->AddEntry(fHistoMap["GT_TRD_Eff"],"TRD","LEP");
  legUR->AddEntry(fHistoMap["GT_TOF_Eff"],"TOF","LEP");
  fHistoMap["GT_STS_Eff"]->GetYaxis()->SetRangeUser(0.0001,1.);
  fHistoMap["GT_STS_Eff"]->Draw("PE1X0");
  fHistoMap["GT_RICH_Eff"]->Draw("PE1X0,same");
  fHistoMap["GT_TRD_Eff"]->Draw("PE1X0,same");
  fHistoMap["GT_TOF_Eff"]->Draw("PE1X0,same");
  legUR->Draw("same");
  c->SaveAs(picsPath + "SubDetectorEfficiency.pdf");
  c->SaveAs(picsPath + "SubDetectorEfficiency.png");
  legUR->Clear();

  fHistoMap["GT_STS_DeltaP"]->GetYaxis()->SetRangeUser(0.0001,100.);
  fHistoMap["GT_STS_DeltaP"]->Draw("PE1X0");
  c->SaveAs(picsPath + "STS_P_resulution.pdf");
  c->SaveAs(picsPath + "STS_P_resulution.png");

  c->cd(1)->SetLogy(1);
  //nEntries = fHistoMap["GT_Detector_Eff"]->GetBinContent(fHistoMap["GT_Detector_Eff"]->GetMaximumBin());//->GetEntries();
  fHistoMap["GT_Detector_Eff"]->SetYTitle("normalized counts");
  //fHistoMap["GT_Detector_Eff"]->Scale(1./nEntries);
  fHistoMap["GT_Detector_Eff"]->GetYaxis()->SetRangeUser(0.01,1.9);
  fHistoMap["GT_Detector_Eff"]->Draw("PE1X0");
  c->SaveAs(picsPath + "DetectorEfficiency.pdf");
  c->SaveAs(picsPath + "DetectorEfficiency.png");
  c->cd(1)->SetLogy(0);
  /*
    fHistoMap["TRD_GT_electronEff"] = (TH1F*)fHistoMap["TRD_GT_electron_found"]->Clone("TRD_GT_electronEff");
    fHistoMap["TRD_GT_electronEff"]->SetYTitle("Electron efficiency");
    fHistoMap["TRD_GT_electronEff"]->Divide((TH1F*)fHistoMap["TRD_GT_electron_all"]);
    fHistoMap["TRD_GT_contaminationEff"] = (TH1F*)fHistoMap["TRD_GT_electron_wrong"]->Clone("TRD_GT_contaminationEff");
    fHistoMap["TRD_GT_contaminationEff"]->SetYTitle("Contamination efficiency");
    fHistoMap["TRD_GT_all_found"] = (TH1F*)fHistoMap["TRD_GT_electron_found"]->Clone("TRD_GT_all_found");
    fHistoMap["TRD_GT_all_found"]->Add((TH1F*)fHistoMap["TRD_GT_electron_wrong"],1.0);
    if (foundAll)
    fHistoMap["TRD_GT_contaminationEff"]->Divide((TH1F*)fHistoMap["TRD_GT_all_found"]);
    else
    fHistoMap["TRD_GT_contaminationEff"]->Divide((TH1F*)fHistoMap["TRD_GT_electron_found"]);

    fHistoMap["RICH_GT_electronEff"] = (TH1F*)fHistoMap["RICH_GT_electron_found"]->Clone("RICH_GT_electronEff");
    fHistoMap["RICH_GT_electronEff"]->Divide((TH1F*)fHistoMap["RICH_GT_electron_all"]);
    fHistoMap["RICH_GT_contaminationEff"] = (TH1F*)fHistoMap["RICH_GT_electron_wrong"]->Clone("RICH_GT_contaminationEff");
    fHistoMap["RICH_GT_all_found"] = (TH1F*)fHistoMap["RICH_GT_electron_found"]->Clone("RICH_GT_all_found");
    fHistoMap["RICH_GT_all_found"]->Add((TH1F*)fHistoMap["RICH_GT_electron_wrong"],1.0);
    if (foundAll)
    fHistoMap["RICH_GT_contaminationEff"]->Divide((TH1F*)fHistoMap["RICH_GT_all_found"]);
    else
    fHistoMap["RICH_GT_contaminationEff"]->Divide((TH1F*)fHistoMap["RICH_GT_electron_found"]);

    fHistoMap["TOF_GT_electronEff"] = (TH1F*)fHistoMap["TOF_GT_electron_found"]->Clone("TOF_GT_electronEff");
    fHistoMap["TOF_GT_electronEff"]->Divide((TH1F*)fHistoMap["TOF_GT_electron_all"]);
    fHistoMap["TOF_GT_contaminationEff"] = (TH1F*)fHistoMap["TOF_GT_electron_wrong"]->Clone("TOF_GT_contaminationEff");
    fHistoMap["TOF_GT_all_found"] = (TH1F*)fHistoMap["TOF_GT_electron_found"]->Clone("TOF_GT_all_found");
    fHistoMap["TOF_GT_all_found"]->Add((TH1F*)fHistoMap["TOF_GT_electron_wrong"],1.0);
    if (foundAll)
    fHistoMap["TOF_GT_contaminationEff"]->Divide((TH1F*)fHistoMap["TOF_GT_all_found"]);
    else
    fHistoMap["TOF_GT_contaminationEff"]->Divide((TH1F*)fHistoMap["TOF_GT_electron_found"]);

    fHistoMap["TRD_RICH_GT_electronEff"] = (TH1F*)fHistoMap["TRD_RICH_GT_electron_found"]->Clone("TRD_RICH_GT_electronEff");
    fHistoMap["TRD_RICH_GT_electronEff"]->Divide((TH1F*)fHistoMap["TRD_RICH_GT_electron_all"]);
    fHistoMap["TRD_RICH_GT_contaminationEff"] = (TH1F*)fHistoMap["TRD_RICH_GT_electron_wrong"]->Clone("TRD_RICH_GT_contaminationEff");
    fHistoMap["TRD_RICH_GT_all_found"] = (TH1F*)fHistoMap["TRD_RICH_GT_electron_found"]->Clone("TRD_RICH_GT_all_found");
    fHistoMap["TRD_RICH_GT_all_found"]->Add((TH1F*)fHistoMap["TRD_RICH_GT_electron_wrong"],1.0);
    if (foundAll)
    fHistoMap["TRD_RICH_GT_contaminationEff"]->Divide((TH1F*)fHistoMap["TRD_RICH_GT_all_found"]);
    else
    fHistoMap["TRD_RICH_GT_contaminationEff"]->Divide((TH1F*)fHistoMap["TRD_RICH_GT_electron_found"]);

    fHistoMap["TRD_RICH_TOF_GT_electronEff"] = (TH1F*)fHistoMap["TRD_RICH_TOF_GT_electron_found"]->Clone("TRD_RICH_TOF_GT_electronEff");
    fHistoMap["TRD_RICH_TOF_GT_electronEff"]->Divide((TH1F*)fHistoMap["TRD_RICH_TOF_GT_electron_all"]);
    fHistoMap["TRD_RICH_TOF_GT_contaminationEff"] = (TH1F*)fHistoMap["TRD_RICH_TOF_GT_electron_wrong"]->Clone("TRD_RICH_TOF_GT_contaminationEff");
    fHistoMap["TRD_RICH_TOF_GT_all_found"] = (TH1F*)fHistoMap["TRD_RICH_TOF_GT_electron_found"]->Clone("TRD_RICH_TOF_GT_all_found");
    fHistoMap["TRD_RICH_TOF_GT_all_found"]->Add((TH1F*)fHistoMap["TRD_RICH_TOF_GT_electron_wrong"],1.0);
    if (foundAll)
    fHistoMap["TRD_RICH_TOF_GT_contaminationEff"]->Divide((TH1F*)fHistoMap["TRD_RICH_TOF_GT_all_found"]);
    else
    fHistoMap["TRD_RICH_TOF_GT_contaminationEff"]->Divide((TH1F*)fHistoMap["TRD_RICH_TOF_GT_electron_found"]);
  */
  legLR->AddEntry(fHistoMap["TRD_GT_electron_Eff"],"TRD","LEP");
  legLR->AddEntry(fHistoMap["RICH_GT_electron_Eff"],"RICH","LEP");
  legLR->AddEntry(fHistoMap["TOF_GT_electron_Eff"],"TOF","LEP");
  legLR->AddEntry(fHistoMap["TRD_RICH_GT_electron_Eff"],"RICH & TRD","LEP");
  legLR->AddEntry(fHistoMap["TRD_RICH_TOF_GT_electron_Eff"],"RICH & TRD & TOF","LEP");
  fHistoMap["TRD_GT_electron_Eff"]->GetYaxis()->SetRangeUser(0,1.05);
  fHistoMap["TRD_GT_electron_Eff"]->Draw("PE1X0");
  fHistoMap["RICH_GT_electron_Eff"]->Draw("PE1X0,same");
  fHistoMap["TOF_GT_electron_Eff"]->Draw("PE1X0,same");
  fHistoMap["TRD_RICH_GT_electron_Eff"]->Draw("PE1X0,same");
  fHistoMap["TRD_RICH_TOF_GT_electron_Eff"]->Draw("PE1X0,same");
  legLR->Draw("same");
  c->SaveAs(picsPath + "ElectronEfficiency.pdf");
  c->SaveAs(picsPath + "ElectronEfficiency.png");
  legLR->Clear();

  c->cd(1)->SetLogy(1);
  legLR->AddEntry(fHistoMap["TRD_GT_contamination_Eff"],"TRD","LEP");
  Float_t mintemp = 1e-6;
  if (fHistoMap["TRD_GT_contamination_Eff"]->GetBinContent(fHistoMap["TRD_GT_contamination_Eff"]->GetMinimumBin()) < mintemp && 
      fHistoMap["TRD_GT_contamination_Eff"]->GetBinContent(fHistoMap["TRD_GT_contamination_Eff"]->GetMinimumBin()) > 0)
    mintemp = fHistoMap["TRD_GT_contamination_Eff"]->GetBinContent(fHistoMap["TRD_GT_contaminationEff"]->GetMinimumBin());
  legLR->AddEntry(fHistoMap["RICH_GT_contamination_Eff"],"RICH","LEP");
  if (fHistoMap["RICH_GT_contamination_Eff"]->GetBinContent(fHistoMap["RICH_GT_contamination_Eff"]->GetMinimumBin()) < mintemp && 
      fHistoMap["RICH_GT_contamination_Eff"]->GetBinContent(fHistoMap["RICH_GT_contamination_Eff"]->GetMinimumBin()) > 0)
    mintemp = fHistoMap["RICH_GT_contamination_Eff"]->GetBinContent(fHistoMap["RICH_GT_contamination_Eff"]->GetMinimumBin());
  legLR->AddEntry(fHistoMap["TOF_GT_contamination_Eff"],"TOF","LEP");
  if (fHistoMap["TOF_GT_contamination_Eff"]->GetBinContent(fHistoMap["TOF_GT_contamination_Eff"]->GetMinimumBin()) < mintemp && 
      fHistoMap["TOF_GT_contamination_Eff"]->GetBinContent(fHistoMap["TOF_GT_contamination_Eff"]->GetMinimumBin()) > 0)
    mintemp = fHistoMap["TOF_GT_contamination_Eff"]->GetBinContent(fHistoMap["TOF_GT_contamination_Eff"]->GetMinimumBin());
  legLR->AddEntry(fHistoMap["TRD_RICH_GT_contamination_Eff"],"RICH & TRD","LEP");
  if (fHistoMap["TRD_RICH_GT_contamination_Eff"]->GetBinContent(fHistoMap["TRD_RICH_GT_contamination_Eff"]->GetMinimumBin()) < mintemp && 
      fHistoMap["TRD_RICH_GT_contamination_Eff"]->GetBinContent(fHistoMap["TRD_RICH_GT_contamination_Eff"]->GetMinimumBin()) > 0)
    mintemp = fHistoMap["TRD_RICH_GT_contamination_Eff"]->GetBinContent(fHistoMap["TRD_RICH_GT_contamination_Eff"]->GetMinimumBin());
  legLR->AddEntry(fHistoMap["TRD_RICH_TOF_GT_contamination_Eff"],"RICH & TRD & TOF","LEP");
  if (fHistoMap["TRD_RICH_TOF_GT_contamination_Eff"]->GetBinContent(fHistoMap["TRD_RICH_TOF_GT_contamination_Eff"]->GetMinimumBin()) < mintemp && 
      fHistoMap["TRD_RICH_TOF_GT_contamination_Eff"]->GetBinContent(fHistoMap["TRD_RICH_TOF_GT_contamination_Eff"]->GetMinimumBin()) > 0)
    mintemp = fHistoMap["TRD_RICH_TOF_GT_contamination_Eff"]->GetBinContent(fHistoMap["TRD_RICH_TOF_GT_contamination_Eff"]->GetMinimumBin());
  if (foundAll)
    fHistoMap["TRD_GT_contamination_Eff"]->GetYaxis()->SetRangeUser(mintemp,1.1);
  else
    fHistoMap["TRD_GT_contamination_Eff"]->GetYaxis()->SetRangeUser(mintemp,1.e2);
  fHistoMap["TRD_GT_contamination_Eff"]->Draw("PE1X0");
  fHistoMap["RICH_GT_contamination_Eff"]->Draw("PE1X0,same");
  fHistoMap["TOF_GT_contamination_Eff"]->Draw("PE1X0,same");
  fHistoMap["TRD_RICH_GT_contamination_Eff"]->Draw("PE1X0,same");
  fHistoMap["TRD_RICH_TOF_GT_contamination_Eff"]->Draw("PE1X0,same");
  legLR->Draw("same");
  c->SaveAs(picsPath + "ContaminationEfficiency.pdf");
  c->SaveAs(picsPath + "ContaminationEfficiency.png");
  legLR->Clear();
  c->cd(1)->SetLogy(0);


  legUR->AddEntry(fHistoMap["RICH_GT_radiusA_KF_P_pion"],"pion","LEP");
  legUR->AddEntry(fHistoMap["RICH_GT_radiusA_KF_P_electron"],"electron","LEP");
  legUR->AddEntry(fHistoMap["RICH_GT_radiusA_KF_P_proton"],"proton","LEP");
  legUR->AddEntry(fHistoMap["RICH_GT_radiusA_KF_P_myon"],"myon","LEP");
  fHistoMap["RICH_GT_radiusA_KF_P_pion"]->Draw(); 
  fHistoMap["RICH_GT_radiusA_KF_P_electron"]->Draw("same"); 
  fHistoMap["RICH_GT_radiusA_KF_P_proton"]->Draw("same");
  fHistoMap["RICH_GT_radiusA_KF_P_myon"]->Draw("same");
  legUR->Draw("same");
  c->SaveAs(picsPath + "RICH_GT_radiusA_KF_P_PID.pdf");
  c->SaveAs(picsPath + "RICH_GT_radiusA_KF_P_PID.png");
  fHistoMap["RICH_GT_radiusB_KF_P_pion"]->Draw();
  fHistoMap["RICH_GT_radiusB_KF_P_electron"]->Draw("same");
  fHistoMap["RICH_GT_radiusB_KF_P_proton"]->Draw("same");
  fHistoMap["RICH_GT_radiusB_KF_P_myon"]->Draw("same");
  legUR->Draw("same");
  c->SaveAs(picsPath + "RICH_GT_radiusB_KF_P_PID.pdf");
  c->SaveAs(picsPath + "RICH_GT_radiusB_KF_P_PID.png");
  fHistoMap["RICH_GT_radius_KF_P_pion"]->Draw();
  fHistoMap["RICH_GT_radius_KF_P_electron"]->Draw("same");
  fHistoMap["RICH_GT_radius_KF_P_proton"]->Draw("same");
  fHistoMap["RICH_GT_radius_KF_P_myon"]->Draw("same");
  legUR->Draw("same");
  c->SaveAs(picsPath + "RICH_GT_radius_KF_P_PID.pdf");
  c->SaveAs(picsPath + "RICH_GT_radius_KF_P_PID.png");
  legLR->AddEntry(fHistoMap["TOF_GT_time_KF_P_pion"],"pion","LEP");
  legLR->AddEntry(fHistoMap["TOF_GT_time_KF_P_electron"],"electron","LEP");
  legLR->AddEntry(fHistoMap["TOF_GT_time_KF_P_proton"],"proton","LEP");
  legLR->AddEntry(fHistoMap["TOF_GT_time_KF_P_myon"],"myon","LEP");
  fHistoMap["TOF_GT_time_KF_P_pion"]->Draw();
  fHistoMap["TOF_GT_time_KF_P_proton"]->Draw("same");  
  fHistoMap["TOF_GT_time_KF_P_electron"]->Draw("same");
  fHistoMap["TOF_GT_time_KF_P_myon"]->Draw("same");
  legLR->Draw("same");
  c->SaveAs(picsPath + "TOF_GT_time_KF_P_PID.pdf");
  c->SaveAs(picsPath + "TOF_GT_time_KF_P_PID.png");
  legUR->Clear();
  legLR->Clear();
  //nEntries = Float_t(fHistoMap["GT_MC_Tracks"]->GetEntries());
  //fHistoMap["GT_MC_Tracks"]->Scale(1./nEntries);
  fHistoMap["GT_MC_Tracks"]->SetYTitle("normalized counts");
  fHistoMap["GT_MC_Tracks"]->GetYaxis()->SetRangeUser(0,1);
  fHistoMap["GT_MC_Tracks"]->Draw("PE1X0");
  c->SaveAs(picsPath + "GT_MC_Tracks.pdf");
  c->SaveAs(picsPath + "GT_MC_Tracks.png");

  legUR->SetHeader("MC-#gamma pairs from MC-e^{+}e^{-} pairs");
  legUR->AddEntry(fHistoMap["InvMassSpectrumGammaEPPairsInMagnet"],"with vertex in magnet volume","L");
  legUR->AddEntry(fHistoMap["InvMassSpectrumGammaEPPairsInTarget"],"with vertex in target volume","L");
  legUR->AddEntry(fHistoMap["InvMassSpectrumGammaEPPairsOpenAngle"],"with vertex in target volume #Theta < 1.0","L");
  nEntries = fHistoMap["InvMassSpectrumGammaEPPairsInTarget"]->GetEntries();
  fHistoMap["InvMassSpectrumGammaEPPairsInTarget"]->Scale(1./nEntries);
  fHistoMap["InvMassSpectrumGammaEPPairsInTarget"]->Draw();
  nEntries = fHistoMap["InvMassSpectrumGammaEPPairsOpenAngle"]->GetEntries();
  fHistoMap["InvMassSpectrumGammaEPPairsOpenAngle"]->Scale(1./nEntries);
  fHistoMap["InvMassSpectrumGammaEPPairsOpenAngle"]->Draw("same");
  nEntries = fHistoMap["InvMassSpectrumGammaEPPairsInMagnet"]->GetEntries();
  fHistoMap["InvMassSpectrumGammaEPPairsInMagnet"]->Scale(1./nEntries);
  fHistoMap["InvMassSpectrumGammaEPPairsInMagnet"]->Draw("same");



  legUR->Draw("same");
  //pi0->Draw("same");
  c->SaveAs(picsPath + "MC_InvM_fromGamma_fromEPPair.pdf");
  c->SaveAs(picsPath + "MC_InvM_fromGamma_fromEPPair.png");
  legUR->Clear();
  nEntries = fHistoMap["InvMassSpectrumGammaAllPairs"]->GetEntries();
  fHistoMap["InvMassSpectrumGammaAllPairs"]->Scale(1./nEntries);
  fHistoMap["InvMassSpectrumGammaAllPairs"]->Draw();
  c->SaveAs(picsPath + "MC_InvM_fromGamma.pdf");
  c->SaveAs(picsPath + "MC_InvM_fromGamma.png");

  //fHistoMap["InvMassSpectrumGammaGTCandPairs"]->Draw();
  //fHistoMap["InvMassSpectrumGammaCandPairs"]->Draw("same");
  legUR->SetHeader("GT-#gamma pairs from GT-e^{+}e^{-} pairs");
  legUR->AddEntry(fHistoMap["InvMassSpectrumGammaCandPairs"],"without charge info.","L");
  legUR->AddEntry(fHistoMap["InvMassSpectrumGammaEPCandPairs"],"with charge info.","L");
  //legUR->AddEntry(fHistoMap["InvMassSpectrumGammaEPCandPairsOpenAngle"],"with charge info. #Theta < 1.0","L");
  legUR->AddEntry(fHistoMap["InvMassSpectrumGammaGTCandPairsMC"],"with MC info.","L");
  nEntries = fHistoMap["InvMassSpectrumGammaEPCandPairsOpenAngle"]->GetEntries();
  //fHistoMap["InvMassSpectrumGammaEPCandPairsOpenAngle"]->Scale(1./nEntries);
  //fHistoMap["InvMassSpectrumGammaEPCandPairsOpenAngle"]->Draw();
  nEntries = fHistoMap["InvMassSpectrumGammaGTCandPairsMC"]->GetEntries();
  fHistoMap["InvMassSpectrumGammaGTCandPairsMC"]->Scale(1./nEntries);
  fHistoMap["InvMassSpectrumGammaGTCandPairsMC"]->Draw("same"); 
  nEntries = fHistoMap["InvMassSpectrumGammaCandPairs"]->GetEntries();
  fHistoMap["InvMassSpectrumGammaCandPairs"]->Scale(1./nEntries);
  fHistoMap["InvMassSpectrumGammaCandPairs"]->Draw("same");
  nEntries = fHistoMap["InvMassSpectrumGammaEPCandPairs"]->GetEntries();
  fHistoMap["InvMassSpectrumGammaEPCandPairs"]->Scale(1./nEntries);
  fHistoMap["InvMassSpectrumGammaEPCandPairs"]->Draw("same");
 
  //pi0->Draw("same");
  legUR->Draw("same");
  c->SaveAs(picsPath + "KF_InvM_fromGamma_fromEPPair.pdf");
  c->SaveAs(picsPath + "KF_InvM_fromGamma_fromEPPair.png");
  legUR->Clear();
  /*
    CalcDeltaInvMassSpectrum("InvMassSpectrumGammaGTCandPairs");
    CalcDeltaInvMassSpectrum("InvMassSpectrumGammaCandPairs");
    CalcDeltaInvMassSpectrum("InvMassSpectrumGammaEPCandPairs");
    CalcDeltaInvMassSpectrum("InvMassSpectrumGammaEPCandPairsOpenAngle");
    legUR->SetHeader("GT-#gamma pairs from GT-e^{+}e^{-} pairs");
    legUR->AddEntry(fHistoMap["DeltaInvMassSpectrumGammaGTCandPairs"],"all GT","L");
    legUR->AddEntry(fHistoMap["DeltaInvMassSpectrumGammaCandPairs"],"without charge info.","L");
    legUR->AddEntry(fHistoMap["DeltaInvMassSpectrumGammaEPCandPairs"],"with charge info.","L");
    legUR->AddEntry(fHistoMap["DeltaInvMassSpectrumGammaEPCandPairsOpenAngle"],"with charge info. #Theta < 1.0","L");
    fHistoMap["DeltaInvMassSpectrumGammaGTCandPairs"]->Draw();
    fHistoMap["DeltaInvMassSpectrumGammaCandPairs"]->Draw("same");
    fHistoMap["DeltaInvMassSpectrumGammaEPCandPairs"]->Draw("same"); 
    fHistoMap["DeltaInvMassSpectrumGammaEPCandPairsOpenAngle"]->Draw("same");
    legUR->Draw("same");
    c->SaveAs(picsPath + "KF_DeltaInvM_fromGamma_fromEPPair.pdf");
    c->SaveAs(picsPath + "KF_DeltaInvM_fromGamma_fromEPPair.png");
    legUR->Clear();
  */
  fHistoMap["gammaAndGammaMother"]->Draw();
  c->SaveAs(picsPath + "MC_MotherPID_gamma_and_gamma.pdf");
  c->SaveAs(picsPath + "MC_MotherPID_gamma_and_gamma.png");

  fHistoMap["gammaMother"]->Draw();
  c->SaveAs(picsPath + "MC_MotherPID_gamma.pdf");
  c->SaveAs(picsPath + "MC_MotherPID_gamma.png");




  c->cd(1)->SetLogx(0);
  c->cd(1)->SetLogy(1);
  c->cd(1)->SetLogz(0);

  legUL->AddEntry(fHistoMap["PidWknEL"],"electron","L");
  legUL->AddEntry(fHistoMap["PidWknPI"],"pion","L");
  legUL->AddEntry(fHistoMap["PidWknelse"],"else","L");
  fHistoMap["PidWknEL"]->Draw();
  fHistoMap["PidWknPI"]->Draw("same");
  fHistoMap["PidWknelse"]->Draw("same");
  legUL->Draw("same");
  c->SaveAs(picsPath + "WKNLikelihood.pdf");
  c->SaveAs(picsPath + "WKNLikelihood.png");
  legUL->Clear();

  legUL->AddEntry(fHistoMap["PidLikeEL"],"electron","L");
  legUL->AddEntry(fHistoMap["PidLikePI"],"pion","L");
  legUL->AddEntry(fHistoMap["PidLikeKA"],"kaon","L");
  legUL->AddEntry(fHistoMap["PidLikePR"],"proton","L");
  legUL->AddEntry(fHistoMap["PidLikeMU"],"myon","L");
  fHistoMap["PidLikeEL"]->Draw();
  fHistoMap["PidLikePI"]->Draw("same");
  fHistoMap["PidLikeKA"]->Draw("same");
  fHistoMap["PidLikePR"]->Draw("same");
  fHistoMap["PidLikeMU"]->Draw("same");
  legUL->Draw("same");
  c->SaveAs(picsPath + "Likelihood.pdf");
  c->SaveAs(picsPath + "Likelihood.png");
  legUL->Clear();

  legUL->AddEntry(fHistoMap["PidANNelectron"],"electron","L");
  legUL->AddEntry(fHistoMap["PidANNpion"],"pion","L");
  legUL->AddEntry(fHistoMap["PidANNelse"],"else","L");
  fHistoMap["PidANNelectron"]->Draw();
  fHistoMap["PidANNpion"]->Draw("same");
  fHistoMap["PidANNelse"]->Draw("same");
  fHistoMap["PidANNelectron"]->Draw("same");
  legUL->Draw("same");
  c->SaveAs(picsPath + "ANNLikelihood.pdf");
  c->SaveAs(picsPath + "ANNLikelihood.png");
  legUL->Clear();


  legLL->AddEntry(fHistoMap["EPPairFromPi0DetectionEfficiencyAll"],"all global tracks","LEP");
  legLL->AddEntry(fHistoMap["EPPairFromPi0DetectionEfficiency"],"global tracks after PID cut","LEP");
  Float_t max1 = fHistoMap["EPPairFromPi0DetectionEfficiencyAll"]->GetBinContent(fHistoMap["EPPairFromPi0DetectionEfficiencyAll"]->GetMaximumBin());
  Float_t min1 = fHistoMap["EPPairFromPi0DetectionEfficiencyAll"]->GetBinContent(fHistoMap["EPPairFromPi0DetectionEfficiencyAll"]->GetMinimumBin());
  Float_t max2 = fHistoMap["EPPairFromPi0DetectionEfficiency"]->GetBinContent(fHistoMap["EPPairFromPi0DetectionEfficiency"]->GetMaximumBin());
  Float_t min2 = fHistoMap["EPPairFromPi0DetectionEfficiency"]->GetBinContent(fHistoMap["EPPairFromPi0DetectionEfficiency"]->GetMinimumBin());

  if (max2 >= max1)
    max = 5.0*max2;
  else 
    max = 5.0*max1;
  if (min2 <= min1 && min2 > 0)
    min = 0.1*min2;
  else if (min1 > 0)
    min = 0.1*min1;
  fHistoMap["EPPairFromPi0DetectionEfficiency"]->GetYaxis()->SetRangeUser(min,max);
  fHistoMap["EPPairFromPi0DetectionEfficiency"]->Draw("PE1");
  fHistoMap["EPPairFromPi0DetectionEfficiencyAll"]->Draw("PE1,same");
  legLL->Draw("same");
  c->SaveAs(picsPath + "EPPairFromPi0DetectionEfficiency.pdf");
  c->SaveAs(picsPath + "EPPairFromPi0DetectionEfficiency.png");
  legLL->Clear();
  fHistoMap["GT_MC_PID"]->SetMarkerStyle(24);
  fHistoMap["GT_MC_PID"]->SetMarkerSize(1.5);
  fHistoMap["GT_MC_PID_STS"]->SetMarkerStyle(24);
  fHistoMap["GT_MC_PID_STS"]->SetMarkerSize(2);
  fHistoMap["GT_MC_PID_RICH"]->SetMarkerStyle(24);
  fHistoMap["GT_MC_PID_RICH"]->SetMarkerSize(1);
  fHistoMap["GT_MC_PID_TRD"]->SetMarkerStyle(24);
  fHistoMap["GT_MC_PID_TRD"]->SetMarkerSize(2.5);
  fHistoMap["GT_MC_PID_TOF"]->SetMarkerStyle(24);
  fHistoMap["GT_MC_PID_TOF"]->SetMarkerColor(800);
  fHistoMap["GT_MC_PID_TOF"]->SetMarkerSize(3);
  legUR->AddEntry(fHistoMap["GT_MC_PID"],"pure track","LEP");
  legUR->AddEntry(fHistoMap["GT_MC_PID_STS"],"STS track","LEP");
  legUR->AddEntry(fHistoMap["GT_MC_PID_RICH"],"RICH track","LEP");
  legUR->AddEntry(fHistoMap["GT_MC_PID_TRD"],"TRD track","LEP");
  legUR->AddEntry(fHistoMap["GT_MC_PID_TOF"],"TOF track","LEP");
  fHistoMap["GT_MC_PID_RICH"]->SetYTitle("Normalized counts");
  nEntries = fHistoMap["GT_MC_PID_RICH"]->GetEntries();
  fHistoMap["GT_MC_PID_RICH"]->Scale(1./nEntries);
  fHistoMap["GT_MC_PID_RICH"]->GetYaxis()->SetRangeUser(0.1/nEntries,1.);
  fHistoMap["GT_MC_PID_RICH"]->DrawCopy("P");

  nEntries = fHistoMap["GT_MC_PID"]->GetEntries();
  fHistoMap["GT_MC_PID"]->Scale(1./nEntries);
  fHistoMap["GT_MC_PID"]->DrawCopy("P,same");

  nEntries = fHistoMap["GT_MC_PID_STS"]->GetEntries();
  fHistoMap["GT_MC_PID_STS"]->Scale(1./nEntries);
  fHistoMap["GT_MC_PID_STS"]->DrawCopy("P,same");

  nEntries = fHistoMap["GT_MC_PID_TRD"]->GetEntries();
  fHistoMap["GT_MC_PID_TRD"]->Scale(1./nEntries);
  fHistoMap["GT_MC_PID_TRD"]->DrawCopy("P,same");

  nEntries = fHistoMap["GT_MC_PID_TOF"]->GetEntries();
  fHistoMap["GT_MC_PID_TOF"]->Scale(1./nEntries);
  fHistoMap["GT_MC_PID_TOF"]->DrawCopy("P,same");
  fHistoMap["GT_MC_PID_TRD"]->DrawCopy("P,same");
  fHistoMap["GT_MC_PID_STS"]->DrawCopy("P,same");
  fHistoMap["GT_MC_PID_RICH"]->DrawCopy("P,same");
  fHistoMap["GT_MC_PID"]->DrawCopy("P,same");
  legUR->Draw("same");
  c->SaveAs(picsPath + "GT_MC_PID.pdf");
  c->SaveAs(picsPath + "GT_MC_PID.png");
  legUR->Clear();
  fHistoMap["gammaAndGammaMother"]->SetMarkerStyle(20);
  fHistoMap["gammaAndGammaMother"]->SetLineColor(2);
  fHistoMap["gammaAndGammaMother"]->SetMarkerColor(2);
  legUR->SetHeader("");
  legUR->AddEntry(fHistoMap["gammaAndGammaMother"],"#gamma pairs","LEP");
  legUR->AddEntry(fHistoMap["gammaMother"],"single #gamma","LEP");
  fHistoMap["gammaAndGammaMother"]->Draw("PE1");
  fHistoMap["gammaMother"]->Draw("PE1,same");
  legUR->Draw("same");
  c->SaveAs(picsPath + "MC_MotherPID_gamma_andOr_gamma.pdf");
  c->SaveAs(picsPath + "MC_MotherPID_gamma_andOr_gamma.png");
  legUR->Clear();

  fHistoMap["ePlusMinusMother"]->Draw("PE1");
  fHistoMap["ePlusAndMinusMother"]->SetMarkerStyle(20);
  fHistoMap["ePlusAndMinusMother"]->SetLineColor(2);
  fHistoMap["ePlusAndMinusMother"]->SetMarkerColor(2);
  legUR->SetHeader();
  legUR->AddEntry(fHistoMap["ePlusMinusMother"],"single e^{+} or e^{-}","LEP");
  legUR->AddEntry(fHistoMap["ePlusAndMinusMother"],"e^{+}e^{-} pairs","LEP");
  fHistoMap["ePlusAndMinusMother"]->Draw("PE1,same");
  legUR->Draw("same");
  c->SaveAs(picsPath + "MC_MotherPID_elec_andOr_posi.pdf");
  c->SaveAs(picsPath + "MC_MotherPID_elec_andOr_posi.png");
  legUR->Clear();

  fHistoMap["ePlusMinusMother"]->Draw();
  c->SaveAs(picsPath + "MC_MotherPID_elec_or_posi.pdf");
  c->SaveAs(picsPath + "MC_MotherPID_elec_or_posi.png");
  fHistoMap["ePlusAndMinusMother"]->Draw();
  c->SaveAs(picsPath + "MC_MotherPID_elec_and_posi.pdf");
  c->SaveAs(picsPath + "MC_MotherPID_elec_and_posi.png");

  fHistoMap["MCPid_global"]->SetMarkerStyle(24);
  fHistoMap["MCPid_global"]->SetMarkerSize(2);
  fHistoMap["MCPid_inMagnet"]->SetMarkerStyle(24);
  fHistoMap["MCPid_inMagnet"]->SetMarkerSize(1.5);
  fHistoMap["MCPid_inTarget"]->SetMarkerStyle(24);
  fHistoMap["MCPid_inTarget"]->SetMarkerSize(1);
  legUR->AddEntry(fHistoMap["MCPid_global"],"global","LEP");
  legUR->AddEntry(fHistoMap["MCPid_inMagnet"],"vertex within magnet volume","LEP");
  legUR->AddEntry(fHistoMap["MCPid_inTarget"],"vertex within target volume","LEP");
  fHistoMap["MCPid_global"]->Draw("PE1");
  fHistoMap["MCPid_inMagnet"]->Draw("PE1,same");
  fHistoMap["MCPid_inTarget"]->Draw("PE1,same");
  legUR->Draw("same");
  c->SaveAs(picsPath + "MC_PID.pdf");
  c->SaveAs(picsPath + "MC_PID.png");
  legUR->Clear();

  legUR->SetHeader("vertex of e^{+}e^{-} from");
  legUR->AddEntry(fHistoMap["PairGammaVertex_z"],"#gamma","LEP");
  legUR->AddEntry(fHistoMap["PairPi0Vertex_z"],"#pi^{0}","LEP");
  fHistoMap["PairGammaVertex_z"]->Draw();
  fHistoMap["PairPi0Vertex_z"]->Draw("same");
  legUR->Draw("same");
  c->SaveAs(picsPath + "MC_zBirth_ep_from_gamma_pi0.pdf");
  c->SaveAs(picsPath + "MC_zBirth_ep_from_gamma_pi0.png");
  legUR->Clear();

  legUR->AddEntry(fHistoMap["EPPairOpeningAngle"],"mixed MC-e^{+}e^{-} pairs","L");
  legUR->AddEntry(fHistoMap["EPPairOpeningAnglePi0"],"MC-e^{+}e^{-} pairs from #pi^{0}","L");
  legUR->AddEntry(fHistoMap["EPPairOpeningAngleGamma"],"MC-e^{+}e^{-} pairs from #gamma","L");
  fHistoMap["EPPairOpeningAngle"]->Draw();
  fHistoMap["EPPairOpeningAnglePi0"]->Draw("same");
  fHistoMap["EPPairOpeningAngleGamma"]->Draw("same");
  legUR->Draw("same");
  c->SaveAs(picsPath + "MC_openingAngle_sameMother_Gamma_Pi0.pdf");
  c->SaveAs(picsPath + "MC_openingAngle_sameMother_Gamma_Pi0.png");
  legUR->Clear();

  c->cd(1)->SetLogx(1);
  c->cd(1)->SetLogy(1);
  c->cd(1)->SetLogz(0);
  fHistoMap["ZBirthEPfromGamma"]->Draw();
  c->SaveAs(picsPath + "MC_zBirth_epPairs_motherGamma.pdf");
  c->SaveAs(picsPath + "MC_zBirth_epPairs_motherGamma.png");

  c->cd(1)->SetLogx(0);
  c->cd(1)->SetLogy(0);
  c->cd(1)->SetLogz(1);

  fHistoMap["RICH_PidANN_MC"]->Draw("colz");
  c->SaveAs(picsPath + "RICH_PidANN_MC.pdf");
  c->SaveAs(picsPath + "RICH_PidANN_MC.png");
  fHistoMap["PidANN_MC"]->Draw("colz");
  c->SaveAs(picsPath + "PidANN_MC.pdf");
  c->SaveAs(picsPath + "PidANN_MC.png");
  fHistoMap["PidWkn_MC"]->Draw("colz");
  c->SaveAs(picsPath + "PidWkn_MC.pdf");
  c->SaveAs(picsPath + "PidWkn_MC.png");
  fHistoMap["PidLikeEl_MC"]->Draw("colz");
  c->SaveAs(picsPath + "PidLikeEl_MC.pdf");
  c->SaveAs(picsPath + "PidLikeEl_MC.png");
  fHistoMap["PidLikePI_MC"]->Draw("colz");
  c->SaveAs(picsPath + "PidLikePI_MC.pdf");
  c->SaveAs(picsPath + "PidLikePI_MC.png");
  fHistoMap["PidLikePR_MC"]->Draw("colz");
  c->SaveAs(picsPath + "PidLikePR_MC.pdf");
  c->SaveAs(picsPath + "PidLikePR_MC.png");
  fHistoMap["PidLikeKA_MC"]->Draw("colz");
  c->SaveAs(picsPath + "PidLikeKA_MC.pdf");
  c->SaveAs(picsPath + "PidLikeKA_MC.png");
  fHistoMap["PidLikeMU_MC"]->Draw("colz");
  c->SaveAs(picsPath + "PidLikeMU_MC.pdf");
  c->SaveAs(picsPath + "PidLikeMU_MC.png");

  fHistoMap["Pt_global"]->Draw("colz");
  c->SaveAs(picsPath + "MC_PT_PID_global.pdf");
  c->SaveAs(picsPath + "MC_PT_PID_global.png");
  fHistoMap["P_global"]->Draw("colz");
  c->SaveAs(picsPath + "MC_P_PID_global.pdf");
  c->SaveAs(picsPath + "MC_P_PID_global.png");
  fHistoMap["Pt_inMagnet"]->Draw("colz");
  c->SaveAs(picsPath + "MC_PT_PID_Magnet.pdf");
  c->SaveAs(picsPath + "MC_PT_PID_Magnet.png");
  fHistoMap["P_inMagnet"]->Draw("colz");
  c->SaveAs(picsPath + "MC_P_PID_Magnet.pdf");
  c->SaveAs(picsPath + "MC_P_PID_Magnet.png");
  fHistoMap["Pt_inTarget"]->Draw("colz");
  c->SaveAs(picsPath + "MC_PT_PID_target.pdf");
  c->SaveAs(picsPath + "MC_PT_PID_target.png");
  fHistoMap["P_inTarget"]->Draw("colz");
  c->SaveAs(picsPath + "MC_P_PID_target.pdf");
  c->SaveAs(picsPath + "MC_P_PID_target.png");
  fHistoMap["MotherDaughter_global"]->Draw("colz");
  c->SaveAs(picsPath + "MC_MotherPID_DaughterPID.pdf");
  c->SaveAs(picsPath + "MC_MotherPID_DaughterPID.png");
  fHistoMap["MotherDaughter_inMagnet"]->Draw("colz");
  c->SaveAs(picsPath + "MC_MotherPID_DaughterPID_magnet.pdf");
  c->SaveAs(picsPath + "MC_MotherPID_DaughterPID_magnet.png");
  fHistoMap["MotherDaughter_inTarget"]->Draw("colz");
  c->SaveAs(picsPath + "MC_MotherPID_DaughterPID_target.pdf");
  c->SaveAs(picsPath + "MC_MotherPID_DaughterPID_target.png");
  fHistoMap["ZBirthAll"]->Draw("colz");
  c->SaveAs(picsPath + "MC_zBirth_PID.pdf");
  c->SaveAs(picsPath + "MC_zBirth_PID.png");

  fHistoMap["InvMPairMother"]->Draw("colz");
  c->SaveAs(picsPath + "MC_invM_elec_posi_MotherPID.pdf");
  c->SaveAs(picsPath + "MC_invM_elec_posi_MotherPID.png");
  fHistoMap["PtPairMother"]->Draw("colz");
  c->SaveAs(picsPath + "MC_PT_elec_posi_MotherPID.pdf");
  c->SaveAs(picsPath + "MC_PT_elec_posi_MotherPID.png");
  fHistoMap["PPairMother"]->Draw("colz");
  c->SaveAs(picsPath + "MC_P_elec_posi_MotherPID.pdf");
  c->SaveAs(picsPath + "MC_P_elec_posi_MotherPID.png");
  fHistoMap["OpenAnglePairMother"]->Draw("colz");
  c->SaveAs(picsPath + "MC_openingAngle_elec_posi_MotherPID.pdf");
  c->SaveAs(picsPath + "MC_openingAngle_elec_posi_MotherPID.png");

  fHistoMap["TRD_GT_dEdx_KF_P"]->Draw("colz");
  c->SaveAs(picsPath + "TRD_GT_dEdx_KF_P.pdf");
  c->SaveAs(picsPath + "TRD_GT_dEdx_KF_P.png");
  fHistoMap["RICH_GT_radiusA_KF_P"]->Draw("colz");
  c->SaveAs(picsPath + "RICH_GT_radiusA_KF_P.pdf");
  c->SaveAs(picsPath + "RICH_GT_radiusA_KF_P.png");
  fHistoMap["RICH_GT_radiusB_KF_P"]->Draw("colz");
  c->SaveAs(picsPath + "RICH_GT_radiusB_KF_P.pdf");
  c->SaveAs(picsPath + "RICH_GT_radiusB_KF_P.png");
  fHistoMap["RICH_GT_radius_KF_P"]->Draw("colz");
  c->SaveAs(picsPath + "RICH_GT_radius_KF_P.pdf");
  c->SaveAs(picsPath + "RICH_GT_radius_KF_P.png");
  fHistoMap["TOF_GT_time_KF_P"]->Draw("colz");
  c->SaveAs(picsPath + "TOF_GT_time_KF_P.pdf");
  c->SaveAs(picsPath + "TOF_GT_time_KF_P.png");

  fHistoMap["TRD_GT_dEdx_KF_P_true"]->Draw("colz");
  c->SaveAs(picsPath + "TRD_GT_dEdx_KF_P_true.pdf");
  c->SaveAs(picsPath + "TRD_GT_dEdx_KF_P_true.png");
  fHistoMap["RICH_GT_radiusA_KF_P_true"]->Draw("colz");
  c->SaveAs(picsPath + "RICH_GT_radiusA_KF_P_true.pdf");
  c->SaveAs(picsPath + "RICH_GT_radiusA_KF_P_true.png");
  fHistoMap["RICH_GT_radiusB_KF_P_true"]->Draw("colz");
  c->SaveAs(picsPath + "RICH_GT_radiusB_KF_P_true.pdf");
  c->SaveAs(picsPath + "RICH_GT_radiusB_KF_P_true.png");
  fHistoMap["RICH_GT_radius_KF_P_true"]->Draw("colz");
  c->SaveAs(picsPath + "RICH_GT_radius_KF_P_true.pdf");
  c->SaveAs(picsPath + "RICH_GT_radius_KF_P_true.png");
  fHistoMap["TOF_GT_time_KF_P_true"]->Draw("colz");
  c->SaveAs(picsPath + "TOF_GT_time_KF_P_true.pdf");
  c->SaveAs(picsPath + "TOF_GT_time_KF_P_true.png");
  fHistoMap["TOF_GT_mass2_KF_P"]->SetYTitle("m^{2} [(GeV/c^{2})^{2}]");
  fHistoMap["TOF_GT_mass2_KF_P"]->Draw("colz");
  c->SaveAs(picsPath + "TOF_GT_mass2_KF_P.pdf");
  c->SaveAs(picsPath + "TOF_GT_mass2_KF_P.png");
  fHistoMap["RICH_HitNr_PidMC"]->Draw("colz");
  c->SaveAs(picsPath + "RICH_HitNr_PidMC.pdf");
  c->SaveAs(picsPath + "RICH_HitNr_PidMC.png");
  fHistoMap["RICH_Radius_PidMC"]->Draw("colz");
  c->SaveAs(picsPath + "RICH_Radius_PidMC.pdf");
  c->SaveAs(picsPath + "RICH_Radius_PidMC.png");
  fHistoMap["RICH_Ring2TrackDist_PidMC"]->Draw("colz");
  c->SaveAs(picsPath + "RICH_Ring2TrackDist_PidMC.pdf");
  c->SaveAs(picsPath + "RICH_Ring2TrackDist_PidMC.png");


  fHistoMap["DeltaP"]->Draw("colz");
  c->SaveAs(picsPath + "DeltaP.pdf");
  c->SaveAs(picsPath + "DeltaP.png");
  fHistoMap["DeltaPt"] ->Draw("colz");
  c->SaveAs(picsPath + "DeltaPt.pdf");
  c->SaveAs(picsPath + "DeltaPt.png");
  fHistoMap["DeltaPGT_PKF"]->Draw("colz");
  c->SaveAs(picsPath + "DeltaPGT_PKF.pdf");
  c->SaveAs(picsPath + "DeltaPGT_PKF.png");
  fHistoMap["KF_PID_MC_PID"]->Draw("colz");
  c->SaveAs(picsPath + "KF_PID_MC_PID.pdf");
  c->SaveAs(picsPath + "KF_PID_MC_PID.png");
  fHistoMap["KF_PID_RICH_MC_PID"]->Draw("colz");
  c->SaveAs(picsPath + "KF_PID_RICH_MC_PID.pdf");
  c->SaveAs(picsPath + "KF_PID_RICH_MC_PID.png");
  fHistoMap["KF_PID_TRD_MC_PID"]->Draw("colz");
  c->SaveAs(picsPath + "KF_PID_TRD_MC_PID.pdf");
  c->SaveAs(picsPath + "KF_PID_TRD_MC_PID.png");
  fHistoMap["KF_PID_TOF_MC_PID"]->Draw("colz");
  c->SaveAs(picsPath + "KF_PID_TOF_MC_PID.pdf");
  c->SaveAs(picsPath + "KF_PID_TOF_MC_PID.png");

  c->Close();
}

    void FindHistos(TDirectory *target) {
    cout << "Target path: " << target->GetPath() << endl;
    TString path( (char*)strstr( target->GetPath(), ":" ) );
    path.Remove( 0, 2 );
    TDirectory *current_sourcedir = gDirectory;
    Bool_t status = TH1::AddDirectoryStatus();
    TH1::AddDirectory(kFALSE);
    //TChain *globChain = 0;
    TIter nextkey( current_sourcedir->GetListOfKeys() );
    TKey *key, *oldkey=0;
    while ( (key = (TKey*)nextkey())) {
      if (oldkey && !strcmp(oldkey->GetName(),key->GetName())) continue;
      TObject *obj = key->ReadObj();
      if ( obj->IsA()->InheritsFrom( TH1::Class() ) ) {
	fHistoMap[TString(obj->GetName())] = (TH1*)obj;
	if ( obj->IsA()->InheritsFrom( TH2::Class() ) ) {
	  fHistoMap[TString(obj->GetName())]->SetContour(99);
	}
      } else if ( obj->IsA()->InheritsFrom( TDirectory::Class() ) ) {
	cout << "Found subdirectory " << obj->GetName() << endl;
	target->cd(obj->GetName());
	TDirectory *newdir = target->CurrentDirectory();
	FindHistos(newdir);
      }
    }
    TH1::AddDirectory(status);
  }
