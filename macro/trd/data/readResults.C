#include "TString.h"
#include "TMath.h"
#include "plotResults.C"
#include <list>
#include "TLine.h"

void readResults(TString filename = "result.root", const Int_t detailLevel = 1){
  gROOT->Reset(); 
  gStyle->SetPalette(1,0);
  gROOT->SetStyle("Plain");
  gStyle->SetPadTickX(1);                        
  gStyle->SetPadTickY(1);  
  gStyle->SetPadGridX(1);                        
  gStyle->SetPadGridY(1);  
  // gStyle->SetOptStat(kFALSE);
  gStyle->SetOptTitle(kFALSE);
  //TH1::SetDefaultSumw2();

  Target = TFile::Open(filename, "READ");
  FindHistos(Target);
  const Double_t SpadicDataMax = 500;
  const Double_t ChannelTriggerMax = 1E5;
  const Int_t nEvents = fHistoMap["fEventCounter"]->GetEntries();
  const Double_t EventRate = 1e7;
  const Double_t central2minBias = 1./4.;
  const Double_t BitPerHit = 112 / (15./16.);// * 1.5; // (112 + 4*16) * 10 / 8.;   // 6 time samples, 8b/10b encoding, CBMnet header 
  //  const Double_t BitPerHit = 220; // (112 + 4*16) * 10 / 8.;   // 6 time samples, 8b/10b encoding, CBMnet header 
  //  const Double_t BitPerHit = 112;  // 6 time samples 3 + (9 * 6 + 3) / 15 = 7 words = 7 * 16 bit = 112 bits 
  Double_t triggerRate(0.), etriggerRate(0.), dataRate(0.), edataRate(0.), dataModuleRate(0.), edataModuleRate(0.), triggerModuleRate(0.), etriggerModuleRate(0.), triggerFebRate(0.);
  Int_t nTotalOptLinks(0), enTotalOptLinks(0);
  Double_t trdTotalDataRate(0.), etrdTotalDataRate(0.), trdTotalTriggerRate(0.), etrdTotalTriggerRate(0.);
  TString title;
  printf("Found %i events in file\n",nEvents);
  printf("# moduleAddress / Asic ID / FEB ID / trigger per 32ch Asic (or FEB) per second\n");
  std::map <TString, std::list<Double_t> > FebTypRateMap;
  std::map <TString, std::list<Double_t> >::iterator FebTypRateMapIt;
  std::map <TString, std::list<Double_t> > ModTypRateMap;
  std::map <TString, std::list<Double_t> >::iterator ModTypRateMapIt;


  //Char_t name[50];
  // set even bin size on logx scale
  const Int_t cnbins = 200;
  Double_t cxmin = 1e2;
  Double_t cxmax = 1e6;
  Double_t clogxmin = TMath::Log10(cxmin);
  Double_t clogxmax = TMath::Log10(cxmax);
  Double_t cbinwidth = (clogxmax-clogxmin)/cnbins;
  Double_t cxbins[cnbins+1];
  cxbins[0] = cxmin;
  for (Int_t i=1;i<=cnbins;i++) {
    cxbins[i] = cxmin + TMath::Power(10,clogxmin+i*cbinwidth);
  }
  TH1F *h1HitPad = new TH1F("HitPad","HitPad",cnbins,cxbins);

  //    HitPad = new TH1F(name,title,10000,1e02,1e06);
  h1HitPad->SetXTitle("Trigger / Pad [Hz]");
  h1HitPad->SetYTitle("count");
  h1HitPad->GetYaxis()->SetRangeUser(1,1e05);

 
  const Int_t anbins = 100;
  Double_t axmin = 10;     // Mbit
  Double_t axmax = 2000;  // Mbit
  Double_t alogxmin = TMath::Log10(axmin);
  Double_t alogxmax = TMath::Log10(axmax);
  Double_t abinwidth = (alogxmax-alogxmin)/anbins;
  Double_t axbins[anbins+1];
  axbins[0] = axmin;
  for (Int_t i=1;i<=anbins;i++) {
    axbins[i] = axmin + TMath::Power(10,alogxmin+i*abinwidth);
  }
  TH1D* h1HitAsic = new TH1D("h1HitAsic","h1HitAsic",anbins,axbins);  // Mbit

  if (BitPerHit == 1.)
    h1HitAsic->SetXTitle("Trigger / Asic [Hz]");
  else
    h1HitAsic->SetXTitle("Data / 32ch Asic [Mbit/s]");
  h1HitAsic->SetYTitle("count");
  //  h1HitAsic->GetYaxis()->SetRangeUser(0,20);

  //sprintf(name,"HM_S%d_L%d",fStation,fLayer);
  //  sprintf(title,"DataModule_Station %d, Layer %d",fStation,fLayer);
  //sprintf(name,"Data_per_Module");
  //  h1DataModule = new TH1F(name,title,50*BitPerHit,10,100*10*BitPerHit);
  //  h1DataModule = new TH1F(name,title,1000,10,100*2000);


  const Int_t dnbins = 100;
  Double_t dxmin = 100;     // Mbit
  Double_t dxmax = 20000;  // Mbit
  Double_t dlogxmin = TMath::Log10(dxmin);
  Double_t dlogxmax = TMath::Log10(dxmax);
  Double_t dbinwidth = (dlogxmax-dlogxmin)/dnbins;
  Double_t dxbins[dnbins+1];
  dxbins[0] = dxmin;
  for (Int_t i=1;i<=dnbins;i++) {
    dxbins[i] = dxmin + TMath::Power(10,dlogxmin+i*dbinwidth);
  }
  TH1D* h1HitFeb = new TH1D("h1HitFeb","h1HitFeb",dnbins,dxbins);  // Mbit

  if (BitPerHit == 1.)
    h1HitFeb->SetXTitle("Trigger / FEB [Hz]");
  else
    h1HitFeb->SetXTitle("Data / FEB [Mbit/s]");
  h1HitFeb->SetYTitle("count");
  //  h1HitFeb->GetYaxis()->SetRangeUser(0,20);

  //sprintf(name,"HM_S%d_L%d",fStation,fLayer);
  //  sprintf(title,"DataModule_Station %d, Layer %d",fStation,fLayer);
  //sprintf(name,"Data_per_Module");
  //  h1DataModule = new TH1F(name,title,50*BitPerHit,10,100*10*BitPerHit);
  //  h1DataModule = new TH1F(name,title,1000,10,100*2000);


  TH1F* h1HitPadModule = new TH1F("h1HitPadModule","h1HitPadModule",1200,0,120000);

  // set even bin size on logx scale
  const Int_t bnbins = 100;
  Double_t bxmin = 100;        // Mbit
  Double_t bxmax = 100*2000;  // Mbit
  Double_t blogxmin = TMath::Log10(bxmin);
  Double_t blogxmax = TMath::Log10(bxmax);
  Double_t bbinwidth = (blogxmax-blogxmin)/bnbins;
  Double_t bxbins[bnbins+1];
  bxbins[0] = bxmin;
  for (Int_t i=1;i<=bnbins;i++) {
    bxbins[i] = bxmin + TMath::Power(10,blogxmin+i*bbinwidth);
  }
  TH1F* h1DataModule = new TH1F("h1DataModule","h1DataModule",bnbins,bxbins);  // Mbit

  if (BitPerHit == 1.)
    h1DataModule->SetXTitle("Trigger / Module [Hz]");
  else
    h1DataModule->SetXTitle("Data / Module [Mbit/s]");
  h1DataModule->SetYTitle("count");
  //  h1DataModule->GetYaxis()->SetRangeUser(0,20);

  //sprintf(name,"HO_S%d_L%d",fStation,fLayer);
  //  sprintf(title,"OptLinksModule_Station %d, Layer %d",fStation,fLayer);
  //sprintf(title,"5_Gbps_optical_links_per_Module");
  TH1F* h1OptLinksModule = new TH1F("h1OptLinksModule","h1OptLinksModule",25,0.5,25.5);
  h1OptLinksModule->SetXTitle("5GBit/s optical links / Module");
  h1OptLinksModule->SetYTitle("count");
  //  h1OptLinksModule->GetYaxis()->SetRangeUser(0,20);


  Int_t HotSpadic(0), HotChannel(0);

  for (fHistoMapIt = fHistoMap.begin(); fHistoMapIt != fHistoMap.end(); fHistoMapIt++){
    title = (TString)fHistoMapIt->second->GetTitle();
    title.ReplaceAll("hd_","");
    title.ReplaceAll("_"," ");
    //fHistoMapIt->second->Scale(1. / nEvents);

    if (title.Contains("ASIC")){
      if (detailLevel > 0){
	//printf("  %s:\n",title.Data());
	title.ReplaceAll("ASIC","");
	title.ReplaceAll("Module ","");
	//cout << "# ModuleAddress: " << title.Data() << endl;

      }
      const Int_t nBins = fHistoMapIt->second->GetNbinsX();
    
      /*
	FEBs/Module   ASICs/FEB   ASICs/Module
	9    10    90
	5    10    50
	6     5    30
	4     5    20
	12    8    96
	6     8    48
	4     8    32
	3     8    24
      */
      TString modtype = "";
      Int_t ASIC_FEB(0), FEB_Module(0);
      if (nBins == 90){
	modtype = "S1";
	ASIC_FEB   = 10;
	FEB_Module =  9;
      } else if (nBins == 50){
	modtype = "S2";
	ASIC_FEB   = 10;
	FEB_Module =  5;
      } else if (nBins == 30){
	modtype = "S3";
	ASIC_FEB   = 5;
	FEB_Module = 6;
      } else if (nBins == 20){
	modtype = "S4";
	ASIC_FEB   = 5;
	FEB_Module = 4;
      } else if (nBins == 96){
	modtype = "L5";
	ASIC_FEB   =  8;
	FEB_Module = 12;
      } else if (nBins == 48){
	modtype = "L6";
	ASIC_FEB   = 8;
	FEB_Module = 6;
      } else if (nBins == 32){
	modtype = "L7";
	ASIC_FEB   = 8;
	FEB_Module = 4;
      } else if (nBins == 24){
	modtype = "L8";
	ASIC_FEB   = 8;
	FEB_Module = 3;
      } else {
	modtype = "type not known";
	printf("BAD SHIT\n");
      }

      //if (FebTypRateMap.find(modtype) == FebTypRateMap.end())
     
  
      printf("# %6s  Moduletype: %s NofAsics: %i FEBs/Module: %2i   ASICs/FEB: %2i\n",title.Data(),modtype.Data(),nBins,FEB_Module,ASIC_FEB);
      printf("\n");
      if (detailLevel > 1){
	cout << "# NofAsics     : " << nBins << endl;
	cout << "# moduleAddress / Asic ID / hits per 32ch Asic per second" << endl;
      }
    
      Int_t FEBcounter = 0;
      triggerFebRate = 0.;
      dataModuleRate = 0.;
      edataModuleRate = 0.;
      triggerModuleRate = 0.;
      etriggerModuleRate = 0.;
      for (Int_t iBin = 1; iBin <= nBins; iBin++){
	triggerRate  = fHistoMapIt->second->GetBinContent(iBin) * EventRate / Double_t(nEvents) * central2minBias;
	etriggerRate = TMath::Sqrt(fHistoMapIt->second->GetBinContent(iBin)) * EventRate / Double_t(nEvents) * central2minBias;


	triggerModuleRate += triggerRate;
	etriggerModuleRate += etriggerRate;
	dataRate = triggerRate * BitPerHit * 1e-6;
	h1HitAsic->Fill(dataRate);
	if (dataRate > SpadicDataMax)
	  HotSpadic++;
	edataRate = etriggerRate * BitPerHit * 1e-6;
	dataModuleRate += dataRate;
	edataModuleRate += edataRate;
	if ((iBin-1) % ASIC_FEB == 0 ){	 
	  if (iBin-1 > 0){
	    printf("                  %2i %9.3f MHz\n",FEBcounter,triggerFebRate * 1e-6);
	    FebTypRateMap[modtype].push_back(triggerFebRate);
	    h1HitFeb->Fill(triggerFebRate * BitPerHit * 1e-6);
	    //printf(" |%2i| of %2i\nFEB:%2i  %9.3f MHz\n",iBin-1,ASIC_FEB, FEBcounter,triggerFebRate * 1e-6);
	  } 	
	  triggerFebRate = 0.;
	  FEBcounter++;   
	}
	triggerFebRate += triggerRate;

	printf("          %4i          %6.3f MHz\n",iBin,triggerRate*1E-6/*,etriggerRate*1E-6*/);
      
	if (detailLevel > 1)
	  cout << title.Data() << " " << setfill('0') << setw(2) << iBin << " " 
	       << setiosflags(ios::fixed) << setprecision(0) << setfill(' ') << setw(8) 
	       << triggerRate  << " " 
	       << setiosflags(ios::fixed) << setprecision(0) << setfill(' ') << setw(8) 
	       << etriggerRate << endl;
	
	if (detailLevel > 1)
	  printf("       %4i  %6.3f +-%6.3f MHz  %9.3f+-%9.3f MBit/s\n",iBin,triggerRate*1E-6,etriggerRate*1E-6,dataRate,edataRate);
      }
      //printf("\nFEB:%2i  %9.3f MHz\nfound %2i FEBs of %2i\n",FEBcounter,triggerFebRate * 1e-6,FEBcounter+1,FEB_Module);
      //printf("FEB:%2i  %9.3f MHz\n",FEBcounter,triggerFebRate * 1e-6);
      printf("                  %2i %9.3f MHz\n",FEBcounter,triggerFebRate * 1e-6);
      FebTypRateMap[modtype].push_back(triggerFebRate);
      h1HitFeb->Fill(triggerFebRate * BitPerHit * 1e-6);
      printf("                     %9.3f MHz\n",triggerModuleRate * 1e-6);
      ModTypRateMap[modtype].push_back(triggerModuleRate);
      h1DataModule->Fill(dataModuleRate);
      Int_t    nOptLinks      = 1 + dataModuleRate / 4000.; // 5000.; // 1 link plus 1 for each 4 Gbps (fill links to 80% max)
      Int_t    enOptLinks     = 1 + (dataModuleRate + edataModuleRate) / 4000.;
      h1OptLinksModule->Fill(nOptLinks);
      if (detailLevel > 0){
	//printf("    ---->%12.3f +-%12.3f Mbit/s  nOptLinks: %4i +- %i    %12.3f +-%12.3f MHz\n",  dataModuleRate, edataModuleRate, nOptLinks, enOptLinks-nOptLinks, triggerModuleRate * 1e-6, etriggerModuleRate * 1e-6); 

      }
      nTotalOptLinks += nOptLinks;
      enTotalOptLinks += enOptLinks;
      trdTotalDataRate += dataModuleRate;
      etrdTotalDataRate += edataModuleRate;
      trdTotalTriggerRate += triggerModuleRate;
      etrdTotalTriggerRate += etriggerModuleRate;
    } else { // HIT PER PAD
      const Int_t nxBins = fHistoMapIt->second->GetNbinsX();
      const Int_t nyBins = fHistoMapIt->second->GetNbinsY();
      for (Int_t ixBin = 1; ixBin <= nxBins; ixBin++){
	for (Int_t iyBin = 1; iyBin <= nyBins; iyBin++){
	  Double_t triggerRat = fHistoMapIt->second->GetBinContent(ixBin,iyBin) * EventRate / Double_t(nEvents) * central2minBias;
	  h1HitPad->Fill(triggerRat);
	  if(triggerRat > ChannelTriggerMax)
	    HotChannel++;
	}
      }
    }
  }

  TCanvas* c = new TCanvas("c","c",2*800,2*600);
  c->Divide(3,2);
  c->cd(1)->SetLogx(1);
  c->cd(1)->SetLogy(1);
  h1HitPad->Draw();
  TLine* MaxHitRatePerPad = new TLine(ChannelTriggerMax,0,ChannelTriggerMax,1e05);  // 100.000 hits per pad
  MaxHitRatePerPad->SetLineColor(2);  // make it red
  MaxHitRatePerPad->SetLineWidth(8);  // make it thick
  MaxHitRatePerPad->Draw("same"); 
  h1HitPad->Draw("same");
  c->cd(2)->SetLogx(1);
  h1HitAsic->GetYaxis()->SetRangeUser(0,1500);
  h1HitAsic->Draw();
  TLine* MaxDataRatePerUplink = new TLine(500,0.7*1500,500,1500);//// 500 Mbit per Uplink
  // TLine* MaxDataRatePerUplink = new TLine(500,60,500,80);  // 500 Mbit per Uplink
  MaxDataRatePerUplink->SetLineColor(2);  // make it red
  MaxDataRatePerUplink->SetLineWidth(2);
  MaxDataRatePerUplink->SetLineStyle(2);
  MaxDataRatePerUplink->Draw("same");  // draw red line
  TLine* gbtx4 = new TLine(600,0.7*1500,600,1500);//max 600Mbit per GBTx connecting all 4 SPADIC ports to 4 ports of gbtx  -> 1SPADICS/GBTx //(500,0.7*1500,500,1500);  // 500 Mbit per Uplink
  // TLine* gbtx4 = new TLine(500,60,500,80);  // 500 Mbit per Uplink
  gbtx4->SetLineColor(2);  // make it red
  gbtx4->SetLineWidth(2);
  gbtx4->Draw("same");  // draw red line
  TLine* gbtx2 = new TLine(300,0.7*1500,300,1500);//max 300Mbit per GBTx connecting all 2 SPADIC ports to 2 ports of gbtx -> 2SPADICS/GBTx //(500,0.7*1500,500,1500);  // 500 Mbit per Uplink
  // TLine* gbtx2 = new TLine(500,60,500,80);  // 500 Mbit per Uplink
  gbtx2->SetLineColor(4);  // make it red
  gbtx2->SetLineWidth(2);
  gbtx2->Draw("same");
  TLine* gbtx1 = new TLine(150,0.7*1500,150,1500);//max 600Mbit per GBTx connecting all 1 SPADIC port to 1 ports of gbtx -> 4SPADICS/GBTx //(500,0.7*1500,500,1500);  // 500 Mbit per Uplink
  // TLine* gbtx1 = new TLine(500,60,500,80);  // 500 Mbit per Uplink
  gbtx1->SetLineColor(4);  // make it red
  gbtx1->SetLineWidth(2);
  gbtx1->Draw("same");
  TLine* doubleGbtx = new TLine(1200,0.7*1500,1200,1500);//max 600Mbit per GBTx connecting all 1 SPADIC port to 4 ports of 2 gbtx -> 1SPADICS/2GBTx //(500,0.7*1500,500,1500);  // 500 Mbit per Uplink
  // TLine* doubleGbtx = new TLine(500,60,500,80);  // 500 Mbit per Uplink
  doubleGbtx->SetLineColor(3);  // make it red
  doubleGbtx->SetLineWidth(2);
  doubleGbtx->Draw("same");
  h1HitAsic->Draw("same");
  c->cd(3)->SetLogx(1);
  h1HitFeb->Draw();

  c->cd(4)->SetLogx(1);
  h1DataModule->GetYaxis()->SetRangeUser(0,45);
  h1DataModule->Draw(); 
  TLine* MaxDataRatePerOptLink = new TLine(5000,0.7*45,5000,45);  // 500 Mbit per Uplink
  // TLine* MaxDataRatePerOptLink = new TLine(5000,5,5000,8);  // 500 Mbit per Uplink
  MaxDataRatePerOptLink->SetLineColor(2);  // make it red
  MaxDataRatePerOptLink->SetLineWidth(8);  // make it thick
  MaxDataRatePerOptLink->Draw("same");  // draw red line
  h1DataModule->Draw("same");
  c->cd(5);
  h1OptLinksModule->Draw();   
  c->SaveAs("HitRateLayerSpectrum.png");
  c->SaveAs("HitRateLayerSpectrum.pdf");
  printf("==============================================================================\n");
  for (ModTypRateMapIt =  ModTypRateMap.begin();  ModTypRateMapIt !=  ModTypRateMap.end();  ModTypRateMapIt++){
    printf("%s\n", (*ModTypRateMapIt).first.Data());
    (*ModTypRateMapIt).second.sort();
    for (std::list<Double_t>::iterator it = (*ModTypRateMapIt).second.begin(); it != (*ModTypRateMapIt).second.end(); it++)
      printf("     %9.3f MHz\n", (*it) * 1e-6);

  }

  printf("==============================================================================\n");
  for (FebTypRateMapIt =  FebTypRateMap.begin();  FebTypRateMapIt !=  FebTypRateMap.end();  FebTypRateMapIt++){
    printf("%s\n", (*FebTypRateMapIt).first.Data());
    (*FebTypRateMapIt).second.sort();
    for (std::list<Double_t>::iterator it = (*FebTypRateMapIt).second.begin(); it != (*FebTypRateMapIt).second.end(); it++)
      printf("     %9.3f MHz\n", (*it) * 1e-6);
  }
  printf("\n  %i too hot channels (%.1E Hz limit)\n  %i too hot SPADICs (%.1f MBit/s limit)\n",HotChannel,ChannelTriggerMax,HotSpadic,SpadicDataMax);
  printf("\nTotal:   %.2f +- %.2f MHz   %.2f +- %.2f Gbit/s  nOptLinks: %i +- %i\n___________________________________________________\n", trdTotalTriggerRate * 1e-6, etrdTotalTriggerRate * 1e-6, trdTotalDataRate * 1e-3, etrdTotalDataRate * 1e-3, nTotalOptLinks, enTotalOptLinks-nTotalOptLinks); 
}
