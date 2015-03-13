#include <iostream>
#include <string>
#include <typeinfo>
#include <iostream>
#include <vector>
#include <list>

#include "TPrincipal.h"
#include "TMatrix.h"
#include "TClonesArray.h"
#include "CbmTrdDigiPar.h"
#include "CbmTrdModule.h"
#include "CbmTrdRawToDigiSpadic03.h"
#include "CbmTrdGeoHandler.h"

#include "TH1I.h"
#include "TH1D.h"
#include "TH2I.h"
#include "TH2D.h"

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TKey.h"


#include "TStopwatch.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "/mount/scr1/c_berg09/cbm/roc_2012/onlinemonitor/rocmonitor/TRocEvent.h"
#include "/mount/scr1/c_berg09/cbm/roc_2012/include/roc/Message.h"
#include "/mount/scr1/c_berg09/cbm/roc_2012/onlinemonitor/spadicmonitor/TSpadicEvent.h"
//#include "TEpicsEvent.h"
//#include "TFiberHodEvent.h"
#include "/mount/scr1/c_berg09/cbm/roc_2012/beamtime/cern-oct12/go4/TMbsCrateEvent.h"
#include "/mount/scr1/c_berg09/cbm/roc_2012/beamtime/cern-oct12/go4/TBeamMonitorEvent.h"
#include "/mount/scr1/c_berg09/cbm/roc_2012/beamtime/cern-oct12/go4/TCernOct12UnpackEvent.h"
//#include "TCernOct12DetectorEvent.h"

#include "/mount/scr1/c_berg09/cbm/go4_trunk/Go4Event/TGo4EventElement.h"
//#define NUM_SPADIC_CHA             8
//#define SPADIC_TRACE_SIZE         45
//#define SPADIC_ADC_MAX           255
//#define noBaselineTB               5 

CbmTrdRawToDigiSpadic03::CbmTrdRawToDigiSpadic03()
: FairTask("TrdSpadic03"),
    fNBaselineTB(5),
    fSusid(-1),
    fLowercorthreshold(),
    fMinimumAmplitudeThreshold(),
    fSusIds(),
    fFile(NULL),
    fPrincipal(NULL),
    fDigis(NULL),
    fDigiPar(NULL),
    fModuleInfo(NULL),

    baselineDistribution(),
    covaMatixValue(),
    covaMatixValueMaxAmplitude(),
    covaMatixValueHitTime(),
    maxAmplitudeHitTime(),
    maxAmplitudeValue(),
    noiseDistribution(),
    clusterSize(),
    signalChDistance(),
    averageNoise_2D(),
    averageSignal_2D(),
    covaMatixValueClusterSize()
{
}


CbmTrdRawToDigiSpadic03::CbmTrdRawToDigiSpadic03(const char *name, const char *title)
: FairTask(name),
    fNBaselineTB(5),
    fSusid(-1),
    fLowercorthreshold(),
    fMinimumAmplitudeThreshold(),
    fSusIds(),
    fFile(NULL),
    fPrincipal(NULL),
    fDigis(NULL),
    fDigiPar(NULL),
    fModuleInfo(NULL),

    baselineDistribution(),
    covaMatixValue(),
    covaMatixValueMaxAmplitude(),
    covaMatixValueHitTime(),
    maxAmplitudeHitTime(),  
    maxAmplitudeValue(),
    noiseDistribution(),
    clusterSize(),
    signalChDistance(),
    averageNoise_2D(),
    averageSignal_2D(),
    covaMatixValueClusterSize()
{
}

CbmTrdRawToDigiSpadic03::~CbmTrdRawToDigiSpadic03()
{
  FairRootManager *ioman =FairRootManager::Instance();
  ioman->Write();
  fDigis->Delete();
  delete fDigis;
  if(fDigiPar)
    delete fDigiPar;
  if(fModuleInfo)
    delete fModuleInfo;
}

void CbmTrdRawToDigiSpadic03::SetParContainers()
{
    cout<<" * CbmTrdRawToDigiSpadic03 * :: SetParContainers() "<<endl;


    // Get Base Container
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    fDigiPar = (CbmTrdDigiPar*)
               (rtdb->getContainer("CbmTrdDigiPar"));

}


// ---- ReInit  -------------------------------------------------------
InitStatus CbmTrdRawToDigiSpadic03::ReInit(){

  cout<<" * CbmTrdClusterizer * :: ReInit() "<<endl;


  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

  fDigiPar = (CbmTrdDigiPar*)
      (rtdb->getContainer("CbmTrdDigiPar"));
  
  return kSUCCESS;
}
// --------------------------------------------------------------------

// ---- Init ----------------------------------------------------------
InitStatus CbmTrdRawToDigiSpadic03::Init()
{

  cout<<" * CbmTrdRawToDigiSpadic03 * :: Init() "<<endl;

  FairRootManager *ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");

  fDigis = new TClonesArray("CbmTrdDigi", 100);
  ioman->Register("TrdDigi","TRD Digis",fDigis,kTRUE);

  //fGeoHandler->Init();

  return kSUCCESS;

}
// --------------------------------------------------------------------


// ---- Exec ----------------------------------------------------------
void CbmTrdRawToDigiSpadic03::Exec(Option_t * option)
{
  fPrincipal = new TPrincipal(NUM_SPADIC_CHA,"ND");
  TString name;

  TH1D *rawPulse[NUM_SPADIC_CHA];
  TH1D *baselinePulse[NUM_SPADIC_CHA];
  TH1D *baselineNoisePulse[NUM_SPADIC_CHA];
  for (Int_t ch = 0; ch < NUM_SPADIC_CHA; ch++) {
    name.Form("Ch%02i",ch);
    rawPulse[ch] = new TH1D("rawPulse"+name,"rawPulse"+name,SPADIC_TRACE_SIZE,0,SPADIC_TRACE_SIZE);
    baselinePulse[ch] = new TH1D("baselinePulse"+name,"baselinePulse"+name,SPADIC_TRACE_SIZE,0,SPADIC_TRACE_SIZE);
    baselineNoisePulse[ch] = new TH1D("baselineNoisePulse"+name,"baselineNoisePulse"+name,SPADIC_TRACE_SIZE,0,SPADIC_TRACE_SIZE);
  }
  Bool_t isPulser(false);
  TH1::SetDefaultSumw2(1);
  TStopwatch timer;
  timer.Start();
  if (fSusIds.empty()){
    cout << "CbmTrdRawToDigiSpadic03::Exec: no SusIds are set! " << endl;
    fSusIds.push_back(11);
    fSusIds.push_back(10);
    fSusIds.push_back(18);
  }
  for (std::vector<Int_t>::iterator it=fSusIds.begin(); it!=fSusIds.end(); it++){
    name.Form("_Sus%02i",(*it));
    clusterSize[(*it)] = new TH1I("UC_clusterSize"+name,"clusterSize"+name,NUM_SPADIC_CHA+1,-0.5,NUM_SPADIC_CHA+0.5);
    maxAmplitudeValue[(*it)] = new TH1I("UC_maxAmplitudeValue"+name,"maxAmplitudeValue"+name,256*2,0,256);
    maxAmplitudeHitTime[(*it)] = new TH2I("UC_maxAmplitudeHitTime"+name,"maxAmplitudeHitTime"+name,256,0,256,45,0,45);
    covaMatixValue[(*it)] = new TH1I("UC_covaMatixValue"+name,"covaMatixValue"+name,1000,-0.13,0.13);
    covaMatixValueMaxAmplitude[(*it)] = new TH2I("UC_covaMatixValueMaxAmplitude"+name,"covaMatixValueMaxAmplitude"+name,1000,-0.13,0.13,256,0,256);
    covaMatixValueHitTime[(*it)] = new TH2I("UC_covaMatixValueHitTime"+name,"covaMatixValueHitTime"+name,1000,-0.13,0.13,45,0,45);
    signalChDistance[(*it)] = new TH1I("UC_signalChDistance"+name,"signalChDistance"+name,2*NUM_SPADIC_CHA+1,-NUM_SPADIC_CHA-0.5,NUM_SPADIC_CHA+0.5);
    averageSignal_2D[(*it)] = new TH2I("UC_averageSignal_2D"+name,"averageSignal_2D"+name,SPADIC_TRACE_SIZE,0,SPADIC_TRACE_SIZE,356,-100,256);
    averageNoise_2D[(*it)] = new TH2I("UC_averageNoise_2D"+name,"averageNoise_2D"+name,SPADIC_TRACE_SIZE,0,SPADIC_TRACE_SIZE,356,-100,256);
    noiseDistribution[(*it)] = new TH1I("UC_noiseDistribution"+name,"noiseDistribution"+name,2*SPADIC_ADC_MAX,-SPADIC_ADC_MAX,SPADIC_ADC_MAX);
    baselineDistribution[(*it)] = new TH2I("UC_baselineDistribution"+name,"baselineDistribution"+name,NUM_SPADIC_CHA,-0.5,NUM_SPADIC_CHA-0.5,SPADIC_ADC_MAX,0,SPADIC_ADC_MAX);
    covaMatixValueClusterSize[(*it)] = new TH2I("covaMatixValueClusterSize"+name,"covaMatixValueClusterSize"+name,100,0,1,NUM_SPADIC_CHA,-0.5,NUM_SPADIC_CHA-0.5);
  }
  if (fFile->IsOpen()){
    TTree* theTree=NULL;
    TKey* kee=NULL;
    TIter iter(fFile->GetListOfKeys());
    while ( ( kee=dynamic_cast<TKey*>(iter()) ) !=0 ) {
      theTree = dynamic_cast<TTree*>(kee->ReadObj());
      if (theTree)
	break; // we take first Tree in file, disregarding its name...
    }
    if(theTree==NULL) {
      cout <<"[Error      ]  CbmTrdRawToDigiSpadic03::Exec: no Tree in file " << endl;
      return;
    }
    TCernOct12UnpackEvent* evnt = new TCernOct12UnpackEvent;
    TGo4EventElement* theBase=evnt;
    evnt->synchronizeWithTree(theTree, &theBase);
    Int_t entries = theTree->GetEntries();
    printf("Found %i events in TTree\n",entries);
    TMbsCrateEvent* fCrateInputEvent=NULL;
    TSpadicEvent* SpadicInputEvent=NULL;
    TSpadicData* theSpadic=NULL;
    Int_t iDigi = 0;
    for(Int_t i=0;i<entries;++i) {
      //cout << i << endl;
      theTree->GetEntry(i);
      SpadicInputEvent=dynamic_cast<TSpadicEvent*>(evnt->GetSubEvent("SPADIC"));
      isPulser = fCrateInputEvent->fIsPulser;
      if (isPulser) continue;
      for (std::vector<Int_t>::iterator it=fSusIds.begin(); it!=fSusIds.end(); it++){
	fSusid = (*it);
	theSpadic=NULL;
	theSpadic=dynamic_cast<TSpadicData*>(SpadicInputEvent->getEventElement(fSusid));
	if (!theSpadic) {cout << "no data found for Susibo " << fSusid << endl; continue;}
	for (Int_t ch = 0; ch < NUM_SPADIC_CHA; ch++) {
	  rawPulse[ch]->Reset();
	  baselinePulse[ch]->Reset();
	  baselineNoisePulse[ch]->Reset();
	  for (Int_t bin = 0; bin < SPADIC_TRACE_SIZE; bin++) {	
	    rawPulse[ch]->Fill(bin, (Double_t)theSpadic->fSpadicPulse[ch][bin]);
	  } 
	}
	BaselineCompensation(rawPulse, baselinePulse);
	Int_t clusterCh = CancelNoise_Cova(baselinePulse, baselineNoisePulse);
	 
	  
	for (Int_t ch = 0; ch < NUM_SPADIC_CHA; ch++){
	  Int_t digiAddress = 5;// CbmTrdAddressTestBeam::GetAddress(fSusid, ch); To be calculated by a CbmTrdDigiAddress class based on the channel id and susibo id
	  Double_t timeStamp = 0;
	  Double_t signal = 0;
	  for (Int_t bin = 1; bin <= baselineNoisePulse[ch]->GetNbinsX(); bin++){
	    if (baselineNoisePulse[ch]->GetBinContent(bin) > 0)
	      signal += baselineNoisePulse[ch]->GetBinContent(bin);
	  }

	  CbmTrdDigi* digi = new CbmTrdDigi(digiAddress, signal, timeStamp);
	  new ((*fDigis)[iDigi]) CbmTrdDigi(*digi);
	  delete digi;
	  iDigi++;
	}
      }
    }
  }

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();

  printf("\n\n******************** Reading Test  **********************\n");
  printf("   RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  printf("*********************************************************\n\n");

}

void CbmTrdRawToDigiSpadic03::SetInputFile(const TString fileName){
  fFile = new TFile(fileName,"READ");
}

void CbmTrdRawToDigiSpadic03::SetSusIds (std::vector<Int_t> susIds){
   for (std::vector<Int_t>::iterator it=susIds.begin(); it!=susIds.end(); it++)
     fSusIds.push_back(*it);
}

Int_t CbmTrdRawToDigiSpadic03::GetPadMax(TH1D* inPulse[NUM_SPADIC_CHA]) {
  Int_t maxCh = -1;
  Double_t maxIntegral = 0.0;
  Double_t tempIntegral = 0.0;
  for(Int_t ch = 0; ch < NUM_SPADIC_CHA; ch++){
    tempIntegral = 0.0;
    for (Int_t bin = 0; bin < SPADIC_TRACE_SIZE; bin++) {
      if (inPulse[ch]->GetBinContent(inPulse[ch]->FindBin(bin)) > 0.0)
	tempIntegral += inPulse[ch]->GetBinContent(inPulse[ch]->FindBin(bin));
    }
    if (tempIntegral > maxIntegral) {
      maxCh = ch;
      maxIntegral = tempIntegral;
    }
  }
  return maxCh;
}

 void CbmTrdRawToDigiSpadic03::SetNBaselineTB(Int_t NBaselineTB)
 {
   fNBaselineTB = NBaselineTB;
 }

 void CbmTrdRawToDigiSpadic03::BaselineCompensation(TH1D* inPulse[NUM_SPADIC_CHA], TH1D* outPulse[NUM_SPADIC_CHA])
  {
    Double_t baseline[NUM_SPADIC_CHA] = {0.0};
    for (Int_t ch = 0; ch < NUM_SPADIC_CHA; ch++){
      outPulse[ch]->Reset();
      for (Int_t bin = 0; bin < fNBaselineTB; bin++){
	baseline[ch] += inPulse[ch]->GetBinContent(inPulse[ch]->FindBin(bin));
      }
      baseline[ch] /= (Double_t)fNBaselineTB;
      if (baseline[ch] > 1)
	if(baselineDistribution.find(fSusid) != baselineDistribution.end())
	  baselineDistribution[fSusid]->Fill(ch,baseline[ch]);
    }
   
    for (Int_t ch = 0; ch < NUM_SPADIC_CHA; ch++){
      for (Int_t bin = 0; bin < SPADIC_TRACE_SIZE; bin++){
	outPulse[ch]->Fill(bin, inPulse[ch]->GetBinContent(inPulse[ch]->FindBin(bin)) - baseline[ch]);
      }
    }
  }


 void CbmTrdRawToDigiSpadic03::SetLowerCorThreshold (std::map<Int_t, Double_t> lowercorthreshold)
 {
   for (std::map<Int_t, Double_t>::iterator it=lowercorthreshold.begin(); it!=lowercorthreshold.end(); it++)
     fLowercorthreshold[it->first] = it->second;
 }

 void CbmTrdRawToDigiSpadic03::SetMinimumAmplitudeThreshold (std::map<Int_t, Double_t> minimumamplitudethreshold)
 {
 for (std::map<Int_t, Double_t>::iterator it=minimumamplitudethreshold.begin(); it!=minimumamplitudethreshold.end(); it++)
     fMinimumAmplitudeThreshold[it->first] = it->second;
 }

Int_t CbmTrdRawToDigiSpadic03::CancelNoise_Cova(TH1D* inPulse[NUM_SPADIC_CHA], TH1D* outPulse[NUM_SPADIC_CHA]
						/*, 
						  TH1I*covaMatixValue=NULL,
						  TH2I*covaMatixValueMaxAmplitude=NULL,
						  TH2I*covaMatixValueHitTime=NULL,
						  TH2I*maxAmplitudeHitTime=NULL,
						  TH1I*noiseDistribution=NULL,
						  TH1I*clusterSize=NULL,
						  TH1I*signalChDistance=NULL,
						  TH2I*averageNoise_2D=NULL,
						  TH2I*averageSignal_2D=NULL,
						  TH2I*covaMatixValueClusterSize=NULL,
						  Bool_t debug=false*/
						){
  Bool_t rawStatistic(false), debug(false);
  if(fLowercorthreshold.empty()){
    cout << "CbmTrdRawToDigiSpadic03::CancelNoise_Cova: fLowercorthreshold Thresholds not set!" << endl;
    fLowercorthreshold[11] = 0.120;//{0.121, 0.112, 0.111};
    fLowercorthreshold[10] = 0.112;
    fLowercorthreshold[18] = 0.111;
  }
  if(fLowercorthreshold.empty()){
    cout << "CbmTrdRawToDigiSpadic03::CancelNoise_Cova: fMinimumAmplitudeThreshold Thresholds not set!" << endl;
    fMinimumAmplitudeThreshold[11] = 30;
    fMinimumAmplitudeThreshold[10] = 16;
    fMinimumAmplitudeThreshold[18] = 20;
  }
  Double_t cortest[NUM_SPADIC_CHA];
  Double_t inputarray[SPADIC_TRACE_SIZE][NUM_SPADIC_CHA] = {{0.0}};
  Double_t minIntegral = 265*45;
  Double_t tempIntegral = 0.0;
  Int_t minCh = -1;//GetMinimumChannel(input);
  Int_t maxCh = -1;//GetPadMax(inPulse);
  Int_t noise_ch_counter=0;
  Int_t signal_ch_counter=0;

  for(Int_t ch = 0; ch < NUM_SPADIC_CHA; ch++){
    tempIntegral = 0.0;
    outPulse[ch]->Reset();
    for(Int_t bin = 0; bin < SPADIC_TRACE_SIZE; bin++){  
      inputarray[bin][ch] = inPulse[ch]->GetBinContent(inPulse[ch]->FindBin(bin));
      tempIntegral += inputarray[bin][ch];
    }
    if (tempIntegral < minIntegral){
      minIntegral = tempIntegral;
      minCh = ch;
    }
  }

  Double_t thisisnoise[SPADIC_TRACE_SIZE] = {0.0};

  for(int t=0;t<SPADIC_TRACE_SIZE;t++){
    fPrincipal->AddRow(inputarray[t]);
  }
  
  fPrincipal->MakePrincipals();
  TMatrixD* cova = (TMatrixD*)fPrincipal->GetCovarianceMatrix();
  
  Int_t lastSigCh = -1;
  std::list<Double_t> covaList;
  std::list<Double_t>::iterator it;
  for(int ch=0;ch<NUM_SPADIC_CHA;ch++){
    cortest[ch] = TMatrixDRow (*cova,minCh)(ch);
    if(covaMatixValueClusterSize.find(fSusid) != covaMatixValueClusterSize.end()){
      covaList.push_back(cortest[ch]);
    }
    if((cortest[ch] > fLowercorthreshold[fSusid]) 
       && (
	   (inPulse[ch]->GetBinContent(inPulse[ch]->GetMaximumBin()) < fMinimumAmplitudeThreshold[fSusid])
	   || ((inPulse[ch]->GetMaximumBin() < 5 || inPulse[ch]->GetMaximumBin() > 20))
	   )
       // && (fabs(inPulse[ch]->GetBinContent(inPulse[ch]->GetMinimumBin())) < fMinimumAmplitudeThreshold[fSusid])
       ){
      noise_ch_counter++;
      for(Int_t bin = 0; bin < SPADIC_TRACE_SIZE; bin++){
	thisisnoise[bin] += inputarray[bin][ch];
      }
    }
    else { //debug
      if (lastSigCh != -1){
	if(signalChDistance.find(fSusid) != signalChDistance.end()){
	  signalChDistance[fSusid]->Fill(ch - lastSigCh/*maxCh-ch*/); //debug
	}
      }
      lastSigCh = ch;
    } //debug
  }
  if(covaMatixValueClusterSize.find(fSusid) != covaMatixValueClusterSize.end()) {
    covaList.sort();
    Int_t counter = 0;//NUM_SPADIC_CHA-1;
    for (it = covaList.begin(); it != covaList.end(); it++){
      if(debug) cout << *it << " (" << counter << ") | ";
      covaMatixValueClusterSize[fSusid]->Fill(*it,counter);
      counter++;
    }
    if(debug) cout << endl;
    covaList.clear();
  }
  //if (noise_ch_counter > 0){ // have found a noise channel to subtract
  //if (noise_ch_counter < NUM_SPADIC_CHA && noise_ch_counter > 0){
  for(Int_t bin = 0; bin < SPADIC_TRACE_SIZE; bin++){
    thisisnoise[bin] /= (double)noise_ch_counter;
    if (noise_ch_counter < NUM_SPADIC_CHA && noise_ch_counter > 0){
      if(noiseDistribution.find(fSusid) != noiseDistribution.end()){
	noiseDistribution[fSusid]->Fill(thisisnoise[bin]);
      }
    }    
  } /*//else {cout << "very bad thing happened!!" << endl;}
    //} else 
    if (noise_ch_counter == 0){ // if no noise channels could be found just subtract the lowest channel to avoid underflows
    for(Int_t bin = 0; bin < SPADIC_TRACE_SIZE; bin++){
    thisisnoise[bin] = inputarray[bin][minCh];
    }
    }
    */

  for (int ch=0; ch<NUM_SPADIC_CHA; ch++){     
    for (int k=0; k<SPADIC_TRACE_SIZE; k++){  
      outPulse[ch]->Fill(k, inPulse[ch]->GetBinContent(inPulse[ch]->FindBin(k)) - thisisnoise[k]);
      if ( TString(inPulse[0]->GetTitle()).BeginsWith("baselinePulse")){
	if(averageSignal_2D.find(fSusid) != averageSignal_2D.end() && averageNoise_2D.find(fSusid) != averageNoise_2D.end()){
	  if (rawStatistic){
	    averageSignal_2D[fSusid]->Fill(k,inPulse[ch]->GetBinContent(inPulse[ch]->FindBin(k)));
	  } else {
	    averageSignal_2D[fSusid]->Fill(k,outPulse[ch]->GetBinContent(outPulse[ch]->FindBin(k)));
	  }
	  averageNoise_2D[fSusid]->Fill(k,thisisnoise[k]);
	}
      }
    }
    if ( TString(inPulse[ch]->GetTitle()).BeginsWith("baselinePulse")){
      if (rawStatistic){
	if (ch != minCh)
	  if (covaMatixValue.find(fSusid) != covaMatixValue.end())
	    covaMatixValue[fSusid]->Fill(cortest[ch]);
	if (ch != minCh)
	  if(covaMatixValueMaxAmplitude.find(fSusid) != covaMatixValueMaxAmplitude.end())
	    covaMatixValueMaxAmplitude[fSusid]->Fill(cortest[ch], inPulse[ch]->GetBinContent(inPulse[ch]->GetMaximumBin()));
	if (ch != minCh)
	  if(covaMatixValueHitTime.find(fSusid) != covaMatixValueHitTime.end())
	    covaMatixValueHitTime[fSusid]->Fill(cortest[ch], inPulse[ch]->GetMaximumBin());
	if(maxAmplitudeHitTime.find(fSusid) != maxAmplitudeHitTime.end())
	  if (inPulse[ch]->GetBinContent(inPulse[ch]->GetMaximumBin()) > 0.0){
	    maxAmplitudeHitTime[fSusid]->Fill(inPulse[ch]->GetBinContent(inPulse[ch]->GetMaximumBin()), inPulse[ch]->GetMaximumBin());
	  }
      } else {
	if (ch != minCh){
	  if (covaMatixValue.find(fSusid) != covaMatixValue.end()){
	    covaMatixValue[fSusid]->Fill(cortest[ch]);
	  }	
	  if(covaMatixValueMaxAmplitude.find(fSusid) != covaMatixValueMaxAmplitude.end()){
	    covaMatixValueMaxAmplitude[fSusid]->Fill(cortest[ch], outPulse[ch]->GetBinContent(outPulse[ch]->GetMaximumBin()));
	  }	
	  if(covaMatixValueHitTime.find(fSusid) != covaMatixValueHitTime.end()){
	    covaMatixValueHitTime[fSusid]->Fill(cortest[ch], outPulse[ch]->GetMaximumBin());
	  }
	}
	if(maxAmplitudeHitTime.find(fSusid) != maxAmplitudeHitTime.end()){
	  if (outPulse[ch]->GetBinContent(outPulse[ch]->GetMaximumBin()) > 0.0){
	    maxAmplitudeHitTime[fSusid]->Fill(outPulse[ch]->GetBinContent(outPulse[ch]->GetMaximumBin()), outPulse[ch]->GetMaximumBin());
	  }
	}	
      }
    }
  }
  if ( TString(inPulse[0]->GetTitle()).BeginsWith("baselinePulse")){
    if (clusterSize.find(fSusid) != clusterSize.end()){      
      if (rawStatistic){
	maxCh = GetPadMax(inPulse);
	for (Int_t iCh = maxCh; iCh < NUM_SPADIC_CHA; iCh++){
	  if (
	      (cortest[iCh] > fLowercorthreshold[fSusid]) 
	      && (
		  (inPulse[iCh]->GetBinContent(inPulse[iCh]->GetMaximumBin()) < fMinimumAmplitudeThreshold[fSusid])
		  || ((inPulse[iCh]->GetMaximumBin() < 5 || inPulse[iCh]->GetMaximumBin() > 20))	
		  )		
	      ) break;
	  signal_ch_counter++;
	}
	for (Int_t iCh = maxCh-1; iCh >= 0; iCh--){
	  if (
	      (cortest[iCh] > fLowercorthreshold[fSusid]) 
	      && (
		  (inPulse[iCh]->GetBinContent(inPulse[iCh]->GetMaximumBin()) < fMinimumAmplitudeThreshold[fSusid])
		  || ((inPulse[iCh]->GetMaximumBin() < 5 || inPulse[iCh]->GetMaximumBin() > 20))
		  )		
	      ) break;
	  signal_ch_counter++;
	} 
      } else {
	maxCh = GetPadMax(outPulse);
	for (Int_t iCh = maxCh; iCh < NUM_SPADIC_CHA; iCh++){
	  if (
	      (cortest[iCh] > fLowercorthreshold[fSusid]) 
	      && (
		  (inPulse[iCh]->GetBinContent(inPulse[iCh]->GetMaximumBin()) < fMinimumAmplitudeThreshold[fSusid])
		  || ((inPulse[iCh]->GetMaximumBin() < 5 || inPulse[iCh]->GetMaximumBin() > 20))	
		  )		
	      ) break;
	  signal_ch_counter++;
	}
	for (Int_t iCh = maxCh-1; iCh >= 0; iCh--){
	  if (
	      (cortest[iCh] > fLowercorthreshold[fSusid]) 
	      && (
		  (inPulse[iCh]->GetBinContent(inPulse[iCh]->GetMaximumBin()) < fMinimumAmplitudeThreshold[fSusid])
		  || ((inPulse[iCh]->GetMaximumBin() < 5 || inPulse[iCh]->GetMaximumBin() > 20))
		  )		
	      ) break;
	  signal_ch_counter++;
	} 
      }      
      clusterSize[fSusid]->Fill(signal_ch_counter); //debug
    }
  }
  fPrincipal->Clear();
  return signal_ch_counter;
}

void CbmTrdRawToDigiSpadic03::FinishTask() { 
  cout << "CbmTrdRawToDigiSpadic03::FinishTask" << endl;
  TString name;
  gDirectory->pwd();
  if (!gDirectory->Cd("CbmTrdRawToDigiSpadic03_QA")) 
    gDirectory->mkdir("CbmTrdRawToDigiSpadic03_QA");
  gDirectory->Cd("CbmTrdRawToDigiSpadic03_QA");
  for (std::vector<Int_t>::iterator it=fSusIds.begin(); it!=fSusIds.end(); it++){
    name.Form("Sus%02i",(*it));
    if (!gDirectory->Cd(name)) 
      gDirectory->mkdir(name);
    gDirectory->Cd(name);
    gDirectory->pwd();
    baselineDistribution[(*it)]->Write("", TObject::kOverwrite);
    covaMatixValue[(*it)]->Write("", TObject::kOverwrite);
    covaMatixValueMaxAmplitude[(*it)]->Write("", TObject::kOverwrite);
    covaMatixValueHitTime[(*it)]->Write("", TObject::kOverwrite);
    maxAmplitudeHitTime[(*it)]->Write("", TObject::kOverwrite);
    maxAmplitudeValue[(*it)]->Write("", TObject::kOverwrite);
    noiseDistribution[(*it)]->Write("", TObject::kOverwrite);
    clusterSize[(*it)]->Write("", TObject::kOverwrite);
    signalChDistance[(*it)]->Write("", TObject::kOverwrite);
    averageNoise_2D[(*it)]->Write("", TObject::kOverwrite);
    averageSignal_2D[(*it)]->Write("", TObject::kOverwrite);
    covaMatixValueClusterSize[(*it)]->Write("", TObject::kOverwrite);
    gDirectory->Cd("..");
  }
  gDirectory->Cd("..");
}

ClassImp(CbmTrdRawToDigiSpadic03)
