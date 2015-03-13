#include "CbmTrdHitDensityQa.h"

#include "CbmTrdDigiPar.h"
#include "CbmTrdDigi.h"
#include "CbmTrdPoint.h"
#include "CbmTrdCluster.h"
#include "CbmTrdHit.h"
#include "CbmTrdModule.h"
#include "CbmTrdGeoHandler.h"
#include "CbmTrdUtils.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "TGeoManager.h"
#include "TClonesArray.h"
#include "TDirectory.h"
#include "TArray.h"
#include "TF1.h"
#include "TH2F.h"
#include "TH2I.h"
#include "TH2D.h"
#include "TColor.h"
#include "TMath.h"
#include "TDatime.h"

#include "TProfile.h"
#include "TLine.h"
#include "TCanvas.h"
#include "TImage.h"
#include "TLegend.h"
#include "TStopwatch.h"

//#include <map>
#include <iostream>
#include <iomanip>
#include <cmath>

#include "CbmTrdDigitizer.h"
#include "CbmTrdDigitizerPRF.h"
#include "CbmTrdClusterFinderFast.h"
#include "CbmTrdHitProducerCluster.h"

using std::cout;
using std::endl;
using std::setfill;
using std::setw;
using std::ios;
using std::setprecision;
using std::setiosflags;

// ---- Default constructor -------------------------------------------
CbmTrdHitDensityQa::CbmTrdHitDensityQa()
  :FairTask("CbmTrdHitDensityQa",1),
   myfile(),
   fmin(1E3), 
   fmax(2.5E5),
   flogScale(false),
   fBitPerHit(1.),
   h1DataModule(NULL),
   h1OptLinksModule(NULL),
   fNeighbourTrigger(true),
   fPlotResults(false),
   fRatioTwoFiles(false),
   fDigis(NULL),
   fClusters(NULL),
   fDigiPar(NULL),
   fModuleInfo(NULL),
   fGeoHandler(new CbmTrdGeoHandler()),
   fStation(-1),
   fLayer(-1), 
   fModuleID(-1),
   fEventCounter(NULL),
   fTriggerThreshold(1e-6),
   fEventRate(1e7),
   fScaleCentral2mBias(1./4.),
   fUsedDigiMap(),
   fModuleHitMap(),
   fModuleHitMapIt(),
   fModuleHitASICMap(),
   fModuleHitASICMapIt()
{
}

// ---- Destructor ----------------------------------------------------
CbmTrdHitDensityQa::~CbmTrdHitDensityQa()
{
  if(fDigis){
    fDigis->Delete();
    delete fDigis;
  }     
  if(fClusters){
    fClusters->Delete();
    delete fClusters;
  }
  if(fDigiPar){
    delete fDigiPar;
  }
  if(fModuleInfo){
    delete fModuleInfo;
  }
}

// ----  Initialisation  ----------------------------------------------
void CbmTrdHitDensityQa::SetParContainers()
{
  // Get Base Container
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  cout << " * CbmTrdHitDensityQa * :: SetParContainers()" << endl;
  fDigiPar = (CbmTrdDigiPar*)(rtdb->getContainer("CbmTrdDigiPar"));
}
// ---- ReInit  -------------------------------------------------------
InitStatus CbmTrdHitDensityQa::ReInit()
{  
  cout << " * CbmTrdHitDensityQa * :: ReInit()" << endl;
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  
  //fDigiPar = (CbmTrdDigiPar*)(rtdb->getContainer("CbmTrdDigiPar"));
  
  return kSUCCESS;
}
// ---- Init ----------------------------------------------------------
InitStatus CbmTrdHitDensityQa::Init()
{
  cout << " * CbmTrdHitDensityQa * :: Init()" << endl;
  FairRootManager *ioman = FairRootManager::Instance();
  fDigis =(TClonesArray *)  ioman->GetObject("TrdDigi");
  if ( ! fDigis ) {
    cout << "-W CbmTrdHitDensityQa::Init: No TrdDigi array!" << endl;
    cout << "                             Task will be inactive" << endl;
    return kERROR;
  }
  fClusters =(TClonesArray *)  ioman->GetObject("TrdCluster");
  if ( ! fClusters ) {
    cout << "-W CbmTrdHitDensityQa::Init: No TrdCluster array!" << endl;
    cout << "                             Task will be inactive" << endl;
    //return kERROR;
  } 
  /* 
  TString origpath = gDirectory->GetPath();
  printf ("\n%s\n",origpath.Data());
  TString newpath = origpath;
  TFile *results = NULL;
  results->Open("data/result.root","read");
  *//*
  if (gSystem->AccessPathName("/data/result.root")){//results->IsOpen()){
    //gDirectory = results.CurrentDirectory();
    //gDirectory->pwd();
    fPlotResults = true;
    //results.Close();
  } else {
    fPlotResults = false;
  }
    */
  //gDirectory->Cd(origpath);
  //gDirectory->pwd();
  // Extract information about the number of TRD stations and
  // the number of layers per TRD station from the geomanager.
  // Store the information about the number of layers at the entrance
  // of subsequent stations in a vector. 
  // This allows to calculate the layer number starting with 1 for the
  // first layer of the first station at a later stage by only adding 
  // the layer number in the station to the number of layers in 
  // previous stations 

  fGeoHandler->Init();

  printf("\nCbmTrdHitDensityQa::Init: NeighbourTrigger %i\n\n",fNeighbourTrigger);

  fEventCounter = new TH1I("fEventCounter","fEventCounter",1,-0.5,0.5);

  return kSUCCESS;
  
}
void CbmTrdHitDensityQa::SetScaleCentral2mBias(Double_t scaling){
  fScaleCentral2mBias = scaling;
}
void CbmTrdHitDensityQa::SetPlotResults(Bool_t plotResults){
  fPlotResults = plotResults;
}
void CbmTrdHitDensityQa::SetNeighbourTrigger(Bool_t trigger){
  fNeighbourTrigger = trigger;
}
void CbmTrdHitDensityQa::SetTriggerMaxScale(Double_t max){
  fmax = max;
}
void CbmTrdHitDensityQa::SetTriggerMinScale(Double_t min){
  fmin = min;
}
void CbmTrdHitDensityQa::SetLogScale(Bool_t logScale){
  flogScale = logScale;
}

void CbmTrdHitDensityQa::SetRatioTwoFiles(Bool_t ratioPlot){
  fRatioTwoFiles = ratioPlot;
  fPlotResults = true;
}

void CbmTrdHitDensityQa::Exec(Option_t * option)
{
 

  printf("=================CbmTrdHitDensityQa====================\n");
  TString title;
  TStopwatch timer;
  timer.Start();



  fBitPerHit = 112 * 1.5; // (112 + 4*16) * 10 / 8.;   // 6 time samples, 8b/10b encoding, CBMnet header 
  //  fBitPerHit = 220; // (112 + 4*16) * 10 / 8.;   // 6 time samples, 8b/10b encoding, CBMnet header 
  //  fBitPerHit = 112;  // 6 time samples 3 + (9 * 6 + 3) / 15 = 7 words = 7 * 16 bit = 112 bits 


  fEventCounter->Fill(0);
  printf("\n  Event: %i\n\n",(Int_t)fEventCounter->GetEntries());
  //fNeighbourTrigger = CbmTrdClusterFinderFast::GetTriggerThreshold();
  CbmTrdDigi *digi = NULL;
  CbmTrdCluster *cluster = NULL;
  if (NULL != fClusters && fNeighbourTrigger == true){
    Int_t nCluster = fClusters->GetEntries();
    for (Int_t iCluster = 0; iCluster < nCluster; iCluster++) {
      //cout << iCluster << endl;
      cluster = (CbmTrdCluster*) fClusters->At(iCluster);//pointer to the acvit cluster  
      Int_t nDigisInCluster = cluster->GetNofDigis();
      for (Int_t iDigi = 0; iDigi < nDigisInCluster; iDigi++){  
	digi = (CbmTrdDigi*)fDigis->At(cluster->GetDigi(iDigi));
	Int_t digiAddress = digi->GetAddress();
	Int_t moduleAddress = CbmTrdAddress::GetModuleAddress(digiAddress);
	Int_t moduleId = CbmTrdAddress::GetModuleId(moduleAddress);// TODO
	fLayer    = CbmTrdAddress::GetLayerId(moduleAddress);
	fModuleInfo = fDigiPar->GetModule(moduleAddress);
	if (fModuleHitMap.find(moduleAddress) == fModuleHitMap.end()){
	  title.Form("hd_Module_%i",moduleAddress);
	  Int_t nRows = fModuleInfo->GetNofRows();
	  Int_t nCols = fModuleInfo->GetNofColumns();
	  fModuleHitMap[moduleAddress] = new TH2I(title,title,nCols,-0.5,nCols-0.5,nRows,-0.5,nRows-0.5);
	  fModuleHitMap[moduleAddress]->SetContour(99);
	  fModuleHitMap[moduleAddress]->SetXTitle("Column Id");
	  fModuleHitMap[moduleAddress]->SetYTitle("Row Id");
	  fModuleHitMap[moduleAddress]->SetZTitle("Trigger counter");
	}
	if (fModuleHitASICMap.find(moduleAddress) == fModuleHitASICMap.end()){
	  title.Form("hd_Module_%i_ASIC",moduleAddress);
	  fModuleHitASICMap[moduleAddress] = new TH1D(title,title,fModuleInfo->GetNofAsics(),-0.5,fModuleInfo->GetNofAsics()-0.5);
	  fModuleHitASICMap[moduleAddress]->SetXTitle("ASIC Address");
	  fModuleHitASICMap[moduleAddress]->SetYTitle("Trigger counter");
	}
	if (!fPlotResults){
	  Int_t iCol(CbmTrdAddress::GetColumnId(digiAddress)), local_Row(CbmTrdAddress::GetRowId(digiAddress)), iSec(CbmTrdAddress::GetSectorId(digiAddress));
	  Int_t iRow = fModuleInfo->GetModuleRow(iSec, local_Row);
	  if (fUsedDigiMap.find(digiAddress) == fUsedDigiMap.end()){ // Cluster include already neighbour trigger read-out. Two clusters can share associated neighbour digis, therefore test if digi is already used
	    fModuleHitMap[moduleAddress]->Fill(iCol, iRow);
	    fUsedDigiMap[digiAddress] = iDigi;
	  } 
	}
      }
    }  
  } else {
    Int_t nDigis = fDigis->GetEntries();
    for (Int_t iDigi=0; iDigi < nDigis; iDigi++ ) {
      digi = (CbmTrdDigi*) fDigis->At(iDigi);
      Int_t digiAddress = digi->GetAddress();
      Int_t moduleAddress = CbmTrdAddress::GetModuleAddress(digiAddress);
      Int_t moduleId = CbmTrdAddress::GetModuleId(moduleAddress);// TODO
      fLayer    = CbmTrdAddress::GetLayerId(moduleAddress);
      fModuleInfo = fDigiPar->GetModule(moduleAddress);
      if (digi->GetCharge() > fTriggerThreshold) {
	if (fModuleHitMap.find(moduleAddress) == fModuleHitMap.end()){
	  title.Form("hd_Module_%i",moduleAddress);
	  Int_t nRows = fModuleInfo->GetNofRows();
	  Int_t nCols = fModuleInfo->GetNofColumns();
	  fModuleHitMap[moduleAddress] = new TH2I(title,title,nCols,-0.5,nCols-0.5,nRows,-0.5,nRows-0.5);
	  fModuleHitMap[moduleAddress]->SetContour(99);
	  fModuleHitMap[moduleAddress]->SetXTitle("Column Id");
	  fModuleHitMap[moduleAddress]->SetYTitle("Row Id");
	  fModuleHitMap[moduleAddress]->SetZTitle("Trigger counter");
	}
	if (fModuleHitASICMap.find(moduleAddress) == fModuleHitASICMap.end()){
	  title.Form("hd_Module_%i_ASIC",moduleAddress);
	  fModuleHitASICMap[moduleAddress] = new TH1D(title,title,fModuleInfo->GetNofAsics(),-0.5,fModuleInfo->GetNofAsics()-0.5);
	  fModuleHitASICMap[moduleAddress]->SetXTitle("ASIC Address");
	  fModuleHitASICMap[moduleAddress]->SetYTitle("Trigger counter");
	}
	if (!fPlotResults){
	  Int_t iCol(CbmTrdAddress::GetColumnId(digiAddress)), local_Row(CbmTrdAddress::GetRowId(digiAddress)), iSec(CbmTrdAddress::GetSectorId(digiAddress));
	  Int_t iRow = fModuleInfo->GetModuleRow(iSec, local_Row);
	  if (fUsedDigiMap.find(digiAddress) == fUsedDigiMap.end()){
	    fModuleHitMap[moduleAddress]->Fill(iCol, iRow);
	    fUsedDigiMap[digiAddress] = iDigi;
	  } 
	}
	/*
	  Int_t neighbourAddress = 0;
	  if (iRow > 0){
	  if (local_Row > 0)
	  neighbourAddress = CbmTrdAddress::GetAddress(fLayer, CbmTrdAddress::GetModuleId(moduleAddress), iSec, local_Row-1, iCol);
	  else if (iSec > 0)
	  neighbourAddress = CbmTrdAddress::GetAddress(fLayer, CbmTrdAddress::GetModuleId(moduleAddress), iSec-1, fModuleInfo->GetNofRowsInSector(iSec-1)-1, iCol);
	  if (fUsedDigiMap.find(neighbourAddress) == fUsedDigiMap.end()){
	  fModuleHitMap[moduleAddress]->Fill(iCol, iRow-1);
	  fUsedDigiMap[neighbourAddress] = iDigi;
	  }
	  }
	  if (iRow < fModuleInfo->GetNofRows()-1){ // Only cross like neighbour trigger 
	  if (local_Row+1 > fModuleInfo->GetNofRowsInSector(iSec)-1)
	  neighbourAddress = CbmTrdAddress::GetAddress(fLayer, CbmTrdAddress::GetModuleId(moduleAddress), iSec+1, 0, iCol);
	  else
	  neighbourAddress = CbmTrdAddress::GetAddress(fLayer, CbmTrdAddress::GetModuleId(moduleAddress), iSec, local_Row+1, iCol);
	  if (fUsedDigiMap.find(neighbourAddress) == fUsedDigiMap.end()){
	  fModuleHitMap[moduleAddress]->Fill(iCol, iRow+1);
	  fUsedDigiMap[neighbourAddress] = iDigi;
	  }
	  }
	  if (iCol > 0){
	  neighbourAddress = CbmTrdAddress::GetAddress(fLayer, CbmTrdAddress::GetModuleId(moduleAddress), iSec, local_Row, iCol-1);
	  if (fUsedDigiMap.find(neighbourAddress) == fUsedDigiMap.end()){
	  fModuleHitMap[moduleAddress]->Fill(iCol-1, iRow);
	  fUsedDigiMap[neighbourAddress] = iDigi;
	  }
	  }
	  if (iCol < fModuleInfo->GetNofColumns()-1){
	  neighbourAddress = CbmTrdAddress::GetAddress(fLayer, CbmTrdAddress::GetModuleId(moduleAddress), iSec, local_Row, iCol+1);
	  if (fUsedDigiMap.find(neighbourAddress) == fUsedDigiMap.end()){
	  fModuleHitMap[moduleAddress]->Fill(iCol+1, iRow);
	  fUsedDigiMap[neighbourAddress] = iDigi;
	  }
	  }
	*/
      }
    }
  }
}
  // ---- Register ------------------------------------------------------
  void CbmTrdHitDensityQa::Register()
  {
    cout << " * CbmTrdHitDensityQa * :: Register()" << endl;

  }
  // --------------------------------------------------------------------


  // ---- Finish --------------------------------------------------------
void CbmTrdHitDensityQa::Finish()
{ 
  std::map<Int_t, std::pair<Int_t, TH2D*> > AsicTriggerMap;
  std::map<Int_t, std::pair<Int_t, TH2D*> >::iterator AsicTriggerMapIt;
  CbmTrdUtils* util = new CbmTrdUtils();

  //Bool_t logScale = false;
  Int_t nTotalAsics = 0;
  Int_t nTotalOptLinks = 0;
  Double_t trdTotalDataRate = 0.;
  Double_t ratePerModule = 0;
  TDatime time;
  TString outfile;
  outfile.Form("hits_per_asic.%i:%i:%i_%i.%i.%i.txt",time.GetHour(),time.GetMinute(),time.GetSecond(),time.GetDay(),time.GetMonth(),time.GetYear());
  if (fPlotResults)
    myfile.open(outfile,std::fstream::out);
  //myfile << "#" << endl;
  //myfile << "##   TRD data per ASIC" << endl;
  //myfile << "#" << endl;
  //myfile << "#" << endl;
  //myfile << "#--------------------------" << endl;
  
  //Double_t min(1E3), max(max = 6E5); // Is not scaled from central to minbias
  //if (fPlotResults) {min = 1E3; max = 2.5E5;} // scaling parameter is applied
  std::vector<Int_t> fColors;
  std::vector<Double_t> fZLevel;
  for (Int_t i = 0; i < TColor::GetNumberOfColors(); i++){
    fColors.push_back(TColor::GetColorPalette(i));
    if (flogScale)
      fZLevel.push_back(fmin + TMath::Power(10, TMath::Log10(fmax-fmin) / Double_t(TColor::GetNumberOfColors()) * i));
    else
      fZLevel.push_back(fmin + ((fmax-fmin) / Double_t(TColor::GetNumberOfColors()) * i));
  }
  TString title, name;
  std::map< Int_t, TCanvas*> LayerMap;
  std::map< Int_t, TCanvas*>::iterator LayerMapIt;
  TString origpath = gDirectory->GetPath();
  printf ("\n%s\n",origpath.Data());
  TString newpath = origpath;
  printf("fPlotResults: %i\n",(Int_t)fPlotResults);
  if (fPlotResults) {
    newpath = "data/result.root";
  } else {
    newpath.ReplaceAll("eds","hd_qa");
    newpath.ReplaceAll(":/","");
  }
  printf ("\n%s\n",newpath.Data());
  TFile *tempFile = NULL;
  TFile *tempFileNumerator = NULL;
  TFile *tempFileDenominator = NULL;
  if (fPlotResults) {
    tempFile = new TFile(newpath,"update");
    if (fRatioTwoFiles) {
      tempFileNumerator   = new TFile("data/result_Numerator.root",  "read");
      if (NULL == tempFileNumerator)
	LOG(ERROR) << "CbmTrdHitRateFastQa:: data/result_Numerator.root not found"<< FairLogger::endl;
      tempFileDenominator = new TFile("data/result_Denominator.root","read");
      if (NULL == tempFileDenominator)
	LOG(ERROR) << "CbmTrdHitRateFastQa:: data/result_Denominator.root not found"<< FairLogger::endl;
    }
  } else
    tempFile = new TFile(newpath,"recreate");
  gDirectory = tempFile->CurrentDirectory();
  gDirectory->pwd();
  
  if (fPlotResults)
    fEventCounter = (TH1I*)tempFile->Get("fEventCounter");
  else
    fEventCounter->Write("", TObject::kOverwrite);
  if (!gDirectory->Cd("TrdHitDensityQa")) 
    gDirectory->mkdir("TrdHitDensityQa");
  gDirectory->Cd("TrdHitDensityQa");
  if (!gDirectory->Cd("Module")) 
    gDirectory->mkdir("Module");
  gDirectory->Cd("Module");

  
  Int_t moduleAddress = -1;
  for (fModuleHitMapIt = fModuleHitMap.begin();
       fModuleHitMapIt != fModuleHitMap.end(); ++fModuleHitMapIt) {
    TString histName = fModuleHitMapIt->second->GetName();
    //cout << histName << endl;
    if (fPlotResults){
      if (fRatioTwoFiles) {
	fModuleHitMapIt->second = (TH2I*)tempFileNumerator->Get("TrdHitDensityQa/Module/" + histName)->Clone(histName+"_numerator");
	// 	if (NULL == fModuleHitMapIt->second)
	// 	  LOG(ERROR) << "CbmTrdHitRateFastQa:: data/result_Numerator.root " << histName.Data() << " not found" << FairLogger::endl;
	fModuleHitMapIt->second->Scale(100);
	// 	//	fModuleHitMapIt->second->Scale(2);
	fModuleHitMapIt->second->Divide((TH2I*)tempFileDenominator->Get("TrdHitDensityQa/Module/" + histName)->Clone(histName+"_denominator"));

//	fModuleHitMapIt->second->Divide(
//					(TH2I*)tempFileNumerator->Get("TrdHitDensityQa/Module/" + histName)->Clone(histName+"_numerator"),
//					(TH2I*)tempFileDenominator->Get("TrdHitDensityQa/Module/" + histName)->Clone(histName+"_denominator"),
//					1.,1.);  // need to be in the kHz range, therefore 100 * 1000
	//					1000.,1.);  // need to be in the kHz range, therefore 100 * 1000

        cout << histName << " " << fModuleHitMapIt->second->GetBinContent(fModuleHitMapIt->second->GetMaximumBin()) << endl;

	//	if (NULL == fModuleHitMapIt->second)
	//	  LOG(ERROR) << "CbmTrdHitRateFastQa:: data/result_Denominator.root " << histName.Data() << " not found" << FairLogger::endl;
      } else 
	fModuleHitMapIt->second = (TH2I*)tempFile->Get("TrdHitDensityQa/Module/" + histName);
    } else
      fModuleHitMapIt->second->Write("", TObject::kOverwrite);
    moduleAddress = fModuleHitMapIt->first;
    //printf("# ModuleAddress:     %8i",moduleAddress);
    //myfile << "# ModuleAddress: " <<  moduleAddress  << endl;
    //if (fPlotResults){
    //util = new CbmTrdUtils();
    //myfile << CbmTrdAddress::GetModuleId(moduleAddress) << " " << util->GetModuleType(moduleAddress,fModuleInfo,fDigiPar) << " ";
    // }
    ratePerModule = 0.;
    Int_t LayerId = CbmTrdAddress::GetLayerId(moduleAddress);
    if (LayerMap.find(LayerId) == LayerMap.end()){
      fStation =  LayerId / 4 + 1;  // OK for SIS100 and SIS300
      fLayer   =  LayerId % 4 + 1;  // 
      name.Form("hd_S%d_L%d",fStation,fLayer);
      title.Form("hd Station %d, Layer %d",fStation,fLayer);
      LayerMap[LayerId] = new TCanvas(name,title,1000,900);
      name.Form("hdH_S%d_L%d",fStation,fLayer);
      title.Form("hdH Station %d, Layer %d",fStation,fLayer);
      TH2I *Layer = new TH2I(name,title,1,-6000,6000,1,-6000,6000);
      Layer->SetContour(99);
      Layer->SetXTitle("x-Coordinate [mm]");
      Layer->SetYTitle("y-Coordinate [mm]");
      Layer->SetZTitle("Trigger/Channel [kHz]");
      if (fRatioTwoFiles)
	Layer->SetZTitle("Trigger ratio [%]");
      Layer->SetStats(kFALSE);
      Layer->GetXaxis()->SetLabelSize(0.02);
      Layer->GetYaxis()->SetLabelSize(0.02);
      Layer->GetZaxis()->SetLabelSize(0.02);
      Layer->GetXaxis()->SetTitleSize(0.02);
      Layer->GetXaxis()->SetTitleOffset(1.5);
      Layer->GetYaxis()->SetTitleSize(0.02);
      Layer->GetYaxis()->SetTitleOffset(2);
      Layer->GetZaxis()->SetTitleSize(0.02);
      Layer->GetZaxis()->SetTitleOffset(-2);
      Layer->GetZaxis()->SetRangeUser(fmin/1000,fmax/1000);
      if (fRatioTwoFiles)
	Layer->GetZaxis()->SetRangeUser(fmin,fmax);
      LayerMap[LayerId]->cd()->SetLogz(flogScale);
      Layer->Fill(0.,0.,0);
      Layer->Draw("colz");
    }
    const Int_t nModules = fDigiPar->GetNrOfModules();
    fModuleInfo = fDigiPar->GetModule(fModuleHitMapIt->first);
    gGeoManager->FindNode(fModuleInfo->GetX(), fModuleInfo->GetY(), fModuleInfo->GetZ());
    std::vector<Int_t> AsicAddresses = fModuleInfo->GetAsicAddresses();
    Int_t nofAsics = fModuleInfo->GetNofAsics();
    if (fPlotResults){
      if (AsicTriggerMap.find(nofAsics) == AsicTriggerMap.end()){
	name.Form("hd_ASICs%03i",nofAsics);
	title.Form("hd_ASICs%03i_%.1fcmx%.1fcm_%03ix%03ipads", nofAsics, 2*fModuleInfo->GetSizeX(), 2*fModuleInfo->GetSizeY(), fModuleInfo->GetNofRows(), fModuleInfo->GetNofColumns());
	AsicTriggerMap[nofAsics] = std::make_pair(1, new TH2D(name,title,nofAsics,-0.5,nofAsics-0.5,nModules,-0.5,nModules-0.5));
	AsicTriggerMap[nofAsics].second->SetContour(99);
	AsicTriggerMap[nofAsics].second->SetXTitle("ASIC Id");
	AsicTriggerMap[nofAsics].second->SetYTitle("module count");
	AsicTriggerMap[nofAsics].second->SetZTitle("trigger per ASIC");
      } else {
	AsicTriggerMap[nofAsics].first += 1;
      }
    }
    //printf("     NofAsics:     %3i\n",nofAsics);
    //myfile << "# NofAsics     : " << nofAsics << endl;
    //myfile << "# moduleAddress / Asic ID / hits per 32ch Asic per second" << endl;
    nTotalAsics += nofAsics;
    std::map<Int_t, Double_t> ratePerAsicMap;
    for (Int_t iAsic = 0; iAsic < nofAsics; iAsic++){
      ratePerAsicMap[AsicAddresses[iAsic]] = 0.;
    }
    const Int_t nSec = fModuleInfo->GetNofSectors();
    Int_t global_Row = 0;
    TVector3 padPos;
    TVector3 padSize;
    TBox *pad = NULL;
    TBox *module = NULL;
    //printf("Module: %6i   Maximum Trigger Rate: %EHz/Channel\n",fModuleHitMapIt->first,fModuleHitMapIt->second->GetBinContent(fModuleHitMapIt->second->GetMaximumBin()) / Double_t(fEventCounter->GetEntries()) * fEventRate);
    for (Int_t s = 0; s < nSec; s++){
      const Int_t nRow = fModuleInfo->GetNofRowsInSector(s);
      const Int_t nCol = fModuleInfo->GetNofColumnsInSector(s);
      for (Int_t r = 0; r < nRow; r++){
	for (Int_t c = 0; c < nCol; c++){
	  fModuleInfo->GetPosition(fModuleHitMapIt->first, s, c, r, padPos, padSize);// padPos local or global???
	  Int_t channelAddress = CbmTrdAddress::GetAddress(CbmTrdAddress::GetLayerId(moduleAddress),
							   CbmTrdAddress::GetModuleId(moduleAddress), 
							   s, r, c);
	  Double_t local_min[3] = {padPos[0]-0.5*padSize[0], padPos[1]-0.5*padSize[1], padPos[2]};
	  Double_t local_max[3] = {padPos[0]+0.5*padSize[0], padPos[1]+0.5*padSize[1], padPos[2]};
	  if (fModuleInfo->GetOrientation() == 1 || fModuleInfo->GetOrientation() == 3){ // Pad size is in local coordinate system where position is in global coordinate system
	    local_min[0] = padPos[0]-0.5*padSize[1];
	    local_min[1] = padPos[1]-0.5*padSize[0];
	    local_max[0] = padPos[0]+0.5*padSize[1];
	    local_max[1] = padPos[1]+0.5*padSize[0];
	  }
	  for (Int_t i = 0; i < 3; i++) {
	    //global_min[i] *= 10.;
	    //global_max[i] *= 10.;
	    local_min[i] *= 10.;
	    local_max[i] *= 10.;
	  }
	  //Double_t rate = Double_t(fModuleHitMapIt->second->GetBinContent(c+1,global_Row+1)) / Double_t(fEventCounter->GetEntries()) * fEventRate;// * 
	  Double_t rate = TriggerCount2TriggerRate(Double_t(fModuleHitMapIt->second->GetBinContent(c+1,global_Row+1))); 
	  ratePerModule += rate;
	  Int_t AsicAddress = fModuleInfo->GetAsicAddress(channelAddress);
	  ratePerAsicMap[AsicAddress] += rate;
	  if (AsicAddress < 0) 
	    LOG(ERROR) << "CbmTrdHitRateFastQa::ScanModulePlane: Channel address:" << channelAddress << " is not initialized in module " << moduleAddress << "(s:" << s << ", r:" << r << ", c:" << c << ")" << FairLogger::endl;

	  pad = new TBox(local_min[0], local_min[1], local_max[0], local_max[1]);
	  //printf("    %i %i %i  (%f, %f)   (%f, %f)   %f\n",s,r,c,local_min[0],local_min[1],global_min[0],global_min[1],rate);
	  pad->SetLineColor(0);
	  pad->SetLineWidth(0);	
	  Int_t color(0), j(0);
	  rate *= fScaleCentral2mBias;
	  //if (rate > min && rate <= max){
	  while ((rate > fZLevel[j]) && (j < (Int_t)fZLevel.size())){
	    //printf ("              %i<%i %i    %E <= %E\n",j,(Int_t)fZLevel.size(),fColors[j], rate, fZLevel[j]);
	    j++;
	  }
	  //printf ("%i<%i %i    %E <= %E\n\n",j,(Int_t)fZLevel.size(),fColors[j], rate, fZLevel[j]);
	  pad->SetFillColor(fColors[j]);
	  if (j == (Int_t)fZLevel.size())
	    pad->SetFillColor(12);
	  if (rate < fmin)
	    pad->SetFillColor(17);
	 
	  LayerMap[LayerId]->cd();
	  pad->Draw("same");

	 
	}
	global_Row++;
      }
    }

    module = new TBox(fModuleInfo->GetX()*10-fModuleInfo->GetSizeX()*10,
		      fModuleInfo->GetY()*10-fModuleInfo->GetSizeY()*10,
		      fModuleInfo->GetX()*10+fModuleInfo->GetSizeX()*10,
		      fModuleInfo->GetY()*10+fModuleInfo->GetSizeY()*10);
    module->SetFillStyle(0);
    LayerMap[LayerId]->cd();
    module->Draw("same");
    delete fModuleHitMapIt->second;
    for (Int_t iAsic = 0; iAsic < nofAsics; iAsic++){
      //fModuleHitASICMap[moduleAddress]->Fill(iAsic, ratePerAsicMap[AsicAddresses[iAsic]] * Double_t(fEventCounter->GetEntries()) / fEventRate);
      fModuleHitASICMap[moduleAddress]->Fill(iAsic, TriggerRate2TriggerCount(ratePerAsicMap[AsicAddresses[iAsic]]));
      if (fPlotResults){
	if (ratePerAsicMap[AsicAddresses[0]] > ratePerAsicMap[AsicAddresses[nofAsics-1]])
	  AsicTriggerMap[nofAsics].second->SetBinContent(iAsic+1, AsicTriggerMap[nofAsics].first, ratePerAsicMap[AsicAddresses[iAsic]] * fScaleCentral2mBias);
	else 
	  AsicTriggerMap[nofAsics].second->SetBinContent(nofAsics-iAsic, AsicTriggerMap[nofAsics].first, ratePerAsicMap[AsicAddresses[iAsic]] * fScaleCentral2mBias);
      }
      if (fPlotResults){
	util = new CbmTrdUtils();
	myfile << setfill(' ') << setw(5) << moduleAddress << " " 
	       << setfill(' ') << setw(2) << CbmTrdAddress::GetModuleId(moduleAddress) << " " << util->GetModuleType(moduleAddress,fModuleInfo,fDigiPar) << " " 
	       << setfill('0') << setw(2) << iAsic << " " 
	       << setiosflags(ios::fixed) << setprecision(0) << setfill(' ') << setw(8) 
	       << ratePerAsicMap[AsicAddresses[iAsic]] << endl;
      }
      //Double_t dataPerAsic = ratePerAsicMap[AsicAddresses[iAsic]]  * 1e-6 * fBitPerHit;  // Mbit, incl. neighbor
      Double_t dataPerAsic = TriggerRate2DataRate(ratePerAsicMap[AsicAddresses[iAsic]])  * 1e-6;  // Mbit, incl. neighbor
      //HitAsic->Fill(dataPerAsic);
    }
    //if (fPlotResults)
    //AsicTriggerMap[nofAsics].first +=1;
    if (!fPlotResults)
      fModuleHitASICMap[moduleAddress]->Write("", TObject::kOverwrite);
    delete fModuleHitASICMap[moduleAddress];
  }
  //Double_t dataPerModule = ratePerModule * 1e-6 * fBitPerHit * fScaleCentral2mBias;  // Mbit, incl. neighbor
  Double_t dataPerModule = TriggerRate2DataRate(ratePerModule) * 1e-6 * fScaleCentral2mBias;  // Mbit, incl. neighbor
  Int_t    nOptLinks     = (Int_t)(1 + dataPerModule / 4000.); // 5000.; // 1 link plus 1 for each 4 Gbps (fill links to 80% max)
  //h1DataModule->Fill(dataPerModule);
  //h1OptLinksModule->Fill(nOptLinks);

  // global statistics
  nTotalOptLinks += nOptLinks;
  trdTotalDataRate += dataPerModule;

  printf("     --------------------------\n");
  printf("     total number of ASICs        : %d\n", nTotalAsics);
  printf("     total number of optical links: %d\n", nTotalOptLinks);
  printf("     total TRD data rate          : %.2f (Gbit/s)\n", trdTotalDataRate * 1e-3 );
  printf("     --------------------------\n");
  printf("     --------------------------\n");
  /*
    myfile << "# total number of ASICs: " << nTotalAsics << endl;
    myfile << "#--------------------------" << endl;
    myfile << "#--------------------------" << endl;
  */
  if (fPlotResults)
    myfile.close();
  fModuleHitASICMap.clear();
  fModuleHitMap.clear();
  gDirectory->Cd("..");
  if (!gDirectory->Cd("ASIC")) 
    gDirectory->mkdir("ASIC");
  gDirectory->Cd("ASIC");
  if (fPlotResults){
    for (AsicTriggerMapIt = AsicTriggerMap.begin();
	 AsicTriggerMapIt != AsicTriggerMap.end(); ++AsicTriggerMapIt) {
      AsicTriggerMapIt->second.second->GetYaxis()->SetRangeUser(-0.5,AsicTriggerMapIt->second.first-1.5);
      AsicTriggerMapIt->second.second->GetZaxis()->SetRangeUser(0,8.0E6);
      AsicTriggerMapIt->second.second->Write("", TObject::kOverwrite);
    }
  }
  gDirectory->Cd("..");
  for (LayerMapIt = LayerMap.begin();
       LayerMapIt != LayerMap.end(); ++LayerMapIt) {
    LayerMapIt->second->Write("", TObject::kOverwrite);
    fStation =  LayerMapIt->first / 4 + 1;  // OK for SIS100 and SIS300
    fLayer   =  LayerMapIt->first % 4 + 1;  // 
    if (fPlotResults){
      name.Form("pics/CbmTrdHitDensityQaFinal_S%i_L%i.png",fStation,fLayer);
      if (fRatioTwoFiles)
	name.Form("pics/CbmTrdHitDensityQaRatio_S%i_L%i.png",fStation,fLayer);
    } else
      name.Form("pics/CbmTrdHitDensityQa_S%i_L%i.png",fStation,fLayer);
    LayerMapIt->second->SaveAs(name);
    name.ReplaceAll("png","pdf");
    LayerMapIt->second->SaveAs(name);
  }
  
  gDirectory->Cd("..");
  tempFile->Close();
  gDirectory->Cd(origpath);
  gDirectory->pwd();
  delete util;
}

  // -----   Public method EndOfEvent   --------------------------------------
  void CbmTrdHitDensityQa::FinishEvent() {
    if (fDigis){
      fDigis->Clear("C");
      fDigis->Delete();
    }
    if (fClusters){
      fClusters->Clear("C");
      fClusters->Delete();
    }
    fUsedDigiMap.clear();
  }
  // -------------------------------------------------------------------------

  void CbmTrdHitDensityQa::SetTriggerThreshold(Double_t triggerthreshold){
    fTriggerThreshold = triggerthreshold;
  }

  Double_t CbmTrdHitDensityQa::TriggerRate2DataRate(Double_t triggerrate){
    return triggerrate * fBitPerHit;
  }
  Double_t CbmTrdHitDensityQa::DataRate2TriggerRate(Double_t datarate){
    return datarate / fBitPerHit;
  }
  Double_t CbmTrdHitDensityQa::TriggerCount2TriggerRate(Double_t count){
//    if (fRatioTwoFiles)
//      return count / Double_t(fEventCounter->GetEntries());
    return count / Double_t(fEventCounter->GetEntries()) * fEventRate;
  }
  Double_t CbmTrdHitDensityQa::TriggerRate2TriggerCount(Double_t rate){
    return rate * Double_t(fEventCounter->GetEntries()) / fEventRate;
  }
  ClassImp(CbmTrdHitDensityQa)
