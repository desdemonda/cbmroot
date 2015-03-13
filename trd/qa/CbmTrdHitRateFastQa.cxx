#include "CbmTrdHitRateFastQa.h"

#include "CbmTrdDigiPar.h"
#include "CbmTrdAddress.h"
#include "CbmTrdModule.h"
#include "CbmTrdRadiator.h"
#include "CbmTrdGeoHandler.h"

#include "CbmTrdUtils.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TColor.h"
#include "TStyle.h"
#include "TBox.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TImage.h"
#include "TLine.h"
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TPRegexp.h"
#include "TPolyMarker.h"
#include "TMath.h"

#include <vector>
#include <iostream>
#include "Riostream.h"
#include <fstream>
#include <cmath>

#define winsize 6000 // 1500 // 5500 // 6000

using std::cout;
using std::cin;
using std::endl;
using std::pair;
using std::vector;
using std::ios;
using std::setw;
using std::setfill;
using std::setiosflags;
using std::setprecision;

// ---- Default constructor -------------------------------------------
CbmTrdHitRateFastQa::CbmTrdHitRateFastQa()
  : FairTask("TrdCluster"),
    myfile(),
    nTotalAsics(0),
    nTotalOptLinks(0),
    trdTotalDataRate(0),
    h1DataModule(NULL),
    h1OptLinksModule(NULL),
    Digicounter(-1),
    tFile(NULL),
    fDraw(kFALSE),
    fPlane(-1),
    fStation(-1),
    fLayer(-1),
    fCol_mean(-1),
    fCol_in(-1),
    fCol_out(-1),
    fRow_mean(-1),
    fRow_in(-1),
    fRow_out(-1),
    fModuleID(-1),
    fMCindex(-1),
    local_meanLL(),
    local_meanC(),
    global_meanLL(),
    global_meanC(),
    local_inLL(),
    local_inC(),
    global_inLL(),
    global_inC(),
    local_outLL(),
    local_outC(),
    global_outLL(),
    global_outC(),
    fx_in(0.), 
    fx_out(0.), 
    fy_in(0.), 
    fy_out(0.), 
    fz_in(0.), 
    fz_out(0.), 
    fx_mean(0.), 
    fy_mean(0.), 
    fz_mean(0.),
    fSector(-1),
    padsize(),
    modulesize(),
    fELoss(-1.),
    fELossdEdX(-1.),
    fELossTR(-1.),
    fPosXLL(0.),
    fPosYLL(0.),
    fPadPosxLL(0.), 
    fPadPosyLL(0.),
    fPadPosxC(0.), 
    fPadPosyC(0.),
    fDeltax(0.), 
    fDeltay(0.),
    fPadCharge(),
    fPRFHitPositionLL(0.), 
    fPRFHitPositionC(0.),
    fEfficiency(1.),
    fTrdPoints(NULL),
    fDigiCollection(NULL),
    fDigiMatchCollection(NULL),
    fMCStacks(NULL),
    fDigiPar(NULL),
    fModuleInfo(NULL),
    //fRadiators(NULL),
    fGeoHandler(new CbmTrdGeoHandler()),
    fColors(),
    fZLevel(),
    fBitPerHit(0),
    fDigiMap(),
    fDigiMapIt()
{
}
// --------------------------------------------------------------------

// ---- Constructor ----------------------------------------------------
CbmTrdHitRateFastQa::CbmTrdHitRateFastQa(const char *name, const char *title)
  : FairTask(name),
    myfile(),
    nTotalAsics(0),
    nTotalOptLinks(0),
    trdTotalDataRate(0),
    h1DataModule(NULL),
    h1OptLinksModule(NULL),
    Digicounter(-1),
    tFile(NULL),
    fDraw(kFALSE),
    fPlane(-1),
    fStation(-1),
    fLayer(-1),
    fCol_mean(-1),
    fCol_in(-1),
    fCol_out(-1),
    fRow_mean(-1),
    fRow_in(-1),
    fRow_out(-1),
    fModuleID(-1),
    fMCindex(-1),
    local_meanLL(),
    local_meanC(),
    global_meanLL(),
    global_meanC(),
    local_inLL(),
    local_inC(),
    global_inLL(),
    global_inC(),
    local_outLL(),
    local_outC(),
    global_outLL(),
    global_outC(),
    fx_in(0.), 
    fx_out(0.), 
    fy_in(0.), 
    fy_out(0.), 
    fz_in(0.), 
    fz_out(0.), 
    fx_mean(0.), 
    fy_mean(0.), 
    fz_mean(0.),
    fSector(-1),
    padsize(),
    modulesize(),
    fELoss(-1.),
    fELossdEdX(-1.),
    fELossTR(-1.),
    fPosXLL(0.),
    fPosYLL(0.),
    fPadPosxLL(0.), 
    fPadPosyLL(0.),
    fPadPosxC(0.), 
    fPadPosyC(0.),
    fDeltax(0.), 
    fDeltay(0.),
    fPadCharge(),
    fPRFHitPositionLL(0.), 
    fPRFHitPositionC(0.),
    fEfficiency(1.),
    fTrdPoints(NULL),
    fDigiCollection(NULL),
    fDigiMatchCollection(NULL),
    fMCStacks(NULL),
    fDigiPar(NULL),
    fModuleInfo(NULL),
    //fRadiators(NULL),
    fGeoHandler(new CbmTrdGeoHandler()),
    fColors(),
    fZLevel(),
    fBitPerHit(0),
    fDigiMap(),
    fDigiMapIt()
{
}
// --------------------------------------------------------------------

// ---- Destructor ----------------------------------------------------
CbmTrdHitRateFastQa::~CbmTrdHitRateFastQa()
{
  //    FairRootManager *ioman =FairRootManager::Instance();
  //ioman->Write();
  //fDigiCollection->Clear("C");
  //delete fDigiCollection;
  //fDigiMatchCollection->Clear("C");
  //delete fDigiMatchCollection;

}
// --------------------------------------------------------------------

// ----  Initialisation  ----------------------------------------------
void CbmTrdHitRateFastQa::SetParContainers()
{
    cout<<" * CbmTrdHitRateFastQa * :: SetParContainers() "<<endl;


    // Get Base Container
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    fDigiPar = (CbmTrdDigiPar*)
               (rtdb->getContainer("CbmTrdDigiPar"));

}
// --------------------------------------------------------------------

// ---- ReInit  -------------------------------------------------------
InitStatus CbmTrdHitRateFastQa::ReInit(){

  cout<<" * CbmTrdHitRateFastQa * :: ReInit() "<<endl;


  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

  fDigiPar = (CbmTrdDigiPar*)
      (rtdb->getContainer("CbmTrdDigiPar"));
  
  return kSUCCESS;
}
// --------------------------------------------------------------------

// ---- Init ----------------------------------------------------------
InitStatus CbmTrdHitRateFastQa::Init()
{

    cout<<" * CbmTrdHitRate * :: Init() "<<endl;

    FairRootManager *ioman = FairRootManager::Instance();
    if ( ! ioman ) Fatal("Init", "No FairRootManager");
    
    fTrdPoints=(TClonesArray *)ioman->GetObject("TrdPoint"); 
    if ( ! fTrdPoints ) {
      cout << "-W CbmTrdCluster::Init: No TrdPoints array!" << endl;
      cout << "                            Task will be inactive" << endl;
      return kERROR;
    }

    fMCStacks = (TClonesArray*)ioman->GetObject("MCTrack");
  if ( ! fMCStacks ) {
      cout << "-W CbmTrdCluster::Init: No MCTrack array!" << endl;
      cout << "                            Task will be inactive" << endl;
      return kERROR;
    }
  /*
    fDigiCollection = new TClonesArray("CbmTrdDigi", 100);
    ioman->Register("TrdDigi","TRD Digis",fDigiCollection,kTRUE);

    fDigiMatchCollection = new TClonesArray("CbmTrdDigiMatch", 100);
    ioman->Register("TrdDigiMatch","TRD Digis",fDigiMatchCollection,kTRUE);
  */
    fGeoHandler->Init();

    //fRadiators->Init();

    return kSUCCESS;

}
// --------------------------------------------------------------------


// ---- Exec ----------------------------------------------------------
void CbmTrdHitRateFastQa::Exec(Option_t * option)
{
  myfile.open("hits_per_asic.txt",std::fstream::out);
  myfile << "#" << endl;
  myfile << "##   TRD data per ASIC" << endl;
  myfile << "#" << endl;
  myfile << "#" << endl;
  myfile << "#--------------------------" << endl;

  fBitPerHit = 112 * 1.5; // (112 + 4*16) * 10 / 8.;   // 6 time samples, 8b/10b encoding, CBMnet header 
  //  fBitPerHit = 220; // (112 + 4*16) * 10 / 8.;   // 6 time samples, 8b/10b encoding, CBMnet header 
  //  fBitPerHit = 112;  // 6 time samples 3 + (9 * 6 + 3) / 15 = 7 words = 7 * 16 bit = 112 bits 
  
  //TStyle::SetNumberContours(99); 
  gStyle->SetNumberContours(99); 
  //TH2::SetContour(99); 
  /*
    const Int_t Number = 11;
    Double_t r[Number] =      {1.00, 0.50, 0.00, 0.00, 0.00, 0.00, 0.00, 0.50, 1.00, 0.50, 1.00};
    Double_t g[Number] =      {0.00, 0.00, 0.00, 0.50, 1.00, 1.00, 1.00, 1.00, 1.00, 0.50, 1.00};
    Double_t b[Number] =      {1.00, 1.00, 1.00, 1.00, 1.00, 0.50, 0.00, 0.00, 0.00, 0.50, 1.00};
    Double_t length[Number] = {0.00, 0.10, 0.20, 0.30, 0.40, 0.50, 0.60, 0.70, 0.80, 0.90, 1.00};
    Int_t nb = 99;
    Int_t a = TColor::CreateGradientColorTable(Number,length,r,g,b,nb);
  */
  printf("Introduction:\n");
  HitRateGeoPara2 *GeoPara = new HitRateGeoPara2;
  Bool_t Lines;
  Bool_t Fast = true; // false; // true; // false;  // will not fill the root file (in Histo)!!
  Bool_t firstLayer = false;
  fDraw = true; // false;
  Double_t ZRangeL = 1;
  Double_t ZRangeU = 1e05;
  Double_t mm2bin = 2.5; // 20.0; // 10.0; // 5.0; // 2.5;
 
  fStation = 0;
  fLayer = 0;
  tFile = new TFile("CbmTrdHitRateFastQa.root","RECREATE"," ROOT file with histograms");

  Char_t name[50];
  Char_t title[50];


  sprintf(name,"HA_S%d_L%d",fStation,fLayer);
  //  sprintf(title,"DataAsic_Station %d, Layer %d",fStation,fLayer);
  sprintf(title,"Data_per_Asic");
  //  TH1F* h1HitAsic = new TH1F(name,title,50*fBitPerHit,1,10*fBitPerHit);
  //  TH1F* h1HitAsic = new TH1F(name,title,1000,1,2000);  // Mbit

  // set even bin size on logx scale
  const Int_t anbins = 100;
  Double_t axmin = 1;     // Mbit
  Double_t axmax = 2000;  // Mbit
  Double_t alogxmin = TMath::Log10(axmin);
  Double_t alogxmax = TMath::Log10(axmax);
  Double_t abinwidth = (alogxmax-alogxmin)/anbins;
  Double_t axbins[anbins+1];
  axbins[0] = axmin;
  for (Int_t i=1;i<=anbins;i++) {
    axbins[i] = axmin + TMath::Power(10,alogxmin+i*abinwidth);
  }
  TH1F* h1HitAsic = new TH1F(name,title,anbins,axbins);  // Mbit

  if (fBitPerHit == 1.)
    h1HitAsic->SetXTitle("Hits/Asic [Hz]");
  else
    h1HitAsic->SetXTitle("Data/32ch Asic [Mbit/s]");
  h1HitAsic->SetYTitle("count");
  //  h1HitAsic->GetYaxis()->SetRangeUser(0,20);

  sprintf(name,"HM_S%d_L%d",fStation,fLayer);
  //  sprintf(title,"DataModule_Station %d, Layer %d",fStation,fLayer);
  sprintf(title,"Data_per_Module");
  //  h1DataModule = new TH1F(name,title,50*fBitPerHit,10,100*10*fBitPerHit);
  //  h1DataModule = new TH1F(name,title,1000,10,100*2000);

  // set even bin size on logx scale
  const Int_t bnbins = 100;
  Double_t bxmin = 10;        // Mbit
  Double_t bxmax = 100*2000;  // Mbit
  Double_t blogxmin = TMath::Log10(bxmin);
  Double_t blogxmax = TMath::Log10(bxmax);
  Double_t bbinwidth = (blogxmax-blogxmin)/bnbins;
  Double_t bxbins[bnbins+1];
  bxbins[0] = bxmin;
  for (Int_t i=1;i<=bnbins;i++) {
    bxbins[i] = bxmin + TMath::Power(10,blogxmin+i*bbinwidth);
  }
  h1DataModule = new TH1F(name,title,bnbins,bxbins);  // Mbit

  if (fBitPerHit == 1.)
    h1DataModule->SetXTitle("Hits/Module [Hz]");
  else
    h1DataModule->SetXTitle("Data/Module [Mbit/s]");
  h1DataModule->SetYTitle("count");
  //  h1DataModule->GetYaxis()->SetRangeUser(0,20);

  sprintf(name,"HO_S%d_L%d",fStation,fLayer);
  //  sprintf(title,"OptLinksModule_Station %d, Layer %d",fStation,fLayer);
  sprintf(title,"5_Gbps_optical_links_per_Module");
  h1OptLinksModule = new TH1F(name,title,20,0.5,20.5);
  h1OptLinksModule->SetXTitle("optical links");
  h1OptLinksModule->SetYTitle("count");
  //  h1OptLinksModule->GetYaxis()->SetRangeUser(0,20);

  TH1F* h1HitPad = NULL;
  TH2F* h2Layer  = NULL;
  TH2F* h2Topview[3] = {NULL,NULL,NULL};
  TCanvas* c1 = NULL;
  TCanvas* c3 = NULL;
  TCanvas* c2 = NULL;
  TCanvas* c0 = NULL;
  if(fDraw) {
    c0 = new TCanvas("c0","c0",1800,450);	
    c0->Divide(3,1);
  }
  const Int_t z_cave = 12000;
  h2Topview[0] = new TH2F("TopView","TopView",    int(2* winsize /mm2bin), -winsize, winsize, int(z_cave /mm2bin),    0, z_cave);
  h2Topview[0]->SetXTitle("x-Coordinate [mm]");
  h2Topview[0]->SetYTitle("z-Coordinate [mm]");
  h2Topview[0]->SetZTitle("#");
  h2Topview[1] = new TH2F("FrontView","FrontView",int(2* winsize /mm2bin), -winsize, winsize, int(2* winsize /mm2bin), -winsize, winsize);
  h2Topview[1]->SetXTitle("x-Coordinate [mm]");
  h2Topview[1]->SetYTitle("y-Coordinate [mm]");
  h2Topview[1]->SetZTitle("#");
  h2Topview[2] = new TH2F("SideView","SideView",  int(z_cave /mm2bin),    0, z_cave, int(2* winsize /mm2bin), -winsize, winsize);
  h2Topview[2]->SetXTitle("z-Coordinate [mm]");
  h2Topview[2]->SetYTitle("y-Coordinate [mm]");
  h2Topview[2]->SetZTitle("#");
  for (Int_t i = 0; i < 3; i++) {
    h2Topview[i]->SetContour(99);
    h2Topview[i]->SetStats(kFALSE);
    h2Topview[i]->GetXaxis()->SetLabelSize(0.02);
    h2Topview[i]->GetYaxis()->SetLabelSize(0.02);
    h2Topview[i]->GetZaxis()->SetLabelSize(0.02);
    h2Topview[i]->GetXaxis()->SetTitleSize(0.02);
    h2Topview[i]->GetXaxis()->SetTitleOffset(1.5);
    h2Topview[i]->GetYaxis()->SetTitleSize(0.02);
    h2Topview[i]->GetYaxis()->SetTitleOffset(2);
    h2Topview[i]->GetZaxis()->SetTitleSize(0.02);
    h2Topview[i]->GetZaxis()->SetTitleOffset(-2);
    if(fDraw) {
      c0->cd(i+1);
      h2Topview[i]->Draw("colz");
    }
  }
  /*
    for (Int_t i = 0; i < 8; i++){
    fColors.push_back(19-i);
    fZLevel.push_back(ZRangeL + (i+1) * ((ZRangeU - ZRangeL) / 9));
    printf("%2i %3i %f\n",i,fColors[i],fZLevel[i]);
    }
    fColors.push_back(1);
    fZLevel.push_back(ZRangeL + (9) * ((ZRangeU - ZRangeL) / 9));
  */ 
  for (Int_t i = 0; i < TColor::GetNumberOfColors(); i++){
    fColors.push_back(TColor::GetColorPalette(i));
    fZLevel.push_back(ZRangeL + TMath::Power(10, TMath::Log10(ZRangeU) / TColor::GetNumberOfColors() * i));
    //fZLevel.push_back(ZRangeL + (i+1) * ((ZRangeU - ZRangeL) / TColor::GetNumberOfColors()));
    //printf("%2i %3i %f\n",i,TColor::GetColorPalette(i),ZRangeL + (i+1) * ((ZRangeU - ZRangeL) / TColor::GetNumberOfColors()));
  }
  //h2Topview->GetZaxis()->SetRangeUser(ZRangeL,ZRangeU);
  // return;
  
  vector<int> Plane01;
  vector<int> Plane02;
  vector<int> Plane03;
  vector<int> Plane04;
  vector<int> Plane05;
  vector<int> Plane06;
  vector<int> Plane07;
  vector<int> Plane08;
  vector<int> Plane09;
  vector<int> Plane10;

  Int_t ModuleID;
  Int_t Sector = 0;
  const Int_t NofModules = fDigiPar->GetNrOfModules();

  //const Int_t NofModules = 10; // DE
  //TH2F *Module[NofModules];

  for (Int_t i = 0; i < NofModules; i++) {  

    ModuleID = fDigiPar->GetModuleId(i);
    fModuleInfo = fDigiPar->GetModule(ModuleID);
    //printf("%d:\n  %.1f %.1f %.1f\n",ModuleID,fModuleInfo->GetX(), fModuleInfo->GetY(),fModuleInfo->GetZ());

    fPlane   = CbmTrdAddress::GetLayerId(ModuleID);
    fStation = fPlane / 4 + 1;  // OK for SIS100 and SIS300
    fLayer   = fPlane % 4 + 1;  // OK for SIS100 and SIS300

    //cout << "module " << i+1 << ": ID " << ModuleID << " - P" << fPlane << " S" << fStation << " L" << fLayer << endl;
    //printf("module %4i: ID $6i - P%2i S%2i L%2i\n",i+1,ModuleID,fPlane,fStation,fLayer);
    // fill plane vectors
    if (fPlane == 0)
      Plane01.push_back (ModuleID);
    if (fPlane == 1)
      Plane02.push_back (ModuleID);
    if (fPlane == 2)
      Plane03.push_back (ModuleID);
    if (fPlane == 3)
      Plane04.push_back (ModuleID);
    if (fPlane == 4)
      Plane05.push_back (ModuleID);
    if (fPlane == 5)
      Plane06.push_back (ModuleID);
    if (fPlane == 6)
      Plane07.push_back (ModuleID);
    if (fPlane == 7)
      Plane08.push_back (ModuleID);
    if (fPlane == 8)
      Plane09.push_back (ModuleID);
    if (fPlane == 9)
      Plane10.push_back (ModuleID);
  }

  vector< vector<int> > LiSi;
  //LiSi.push_back(Plane01);
  //LiSi.push_back(Plane02);
  //LiSi.push_back(Plane05);
  //LiSi.push_back(Plane06);
  //LiSi.push_back(Plane09);
  //LiSi.push_back(Plane10);
  LiSi.push_back(Plane01);
  LiSi.push_back(Plane02);
  LiSi.push_back(Plane03);
  LiSi.push_back(Plane04);
  LiSi.push_back(Plane05);
  LiSi.push_back(Plane06);
  LiSi.push_back(Plane07);
  LiSi.push_back(Plane08);
  LiSi.push_back(Plane09);
  LiSi.push_back(Plane10);

  TString OutFile1;
  TString OutFile2;

  TImage *Outimage1;
  TImage *Outimage2;

  TLine* MaxHitRatePerPad = new TLine(1e05,0,1e05,1e04);  // 100.000 hits per pad
  MaxHitRatePerPad->SetLineColor(2);  // make it red
  MaxHitRatePerPad->SetLineWidth(8);  // make it thick

  /*
    h2Layer  = new TH2F("dummy1","dummy1",1,-0.5,0.5,1,-0.5,0.5);
    h1HitPad = new TH1F("dummy2","dummy2",1,-0.5,0.5);
    c1 = new TCanvas("dummy3","dummy3",10,10);
    c2 = new TCanvas("dummy4","dummy4",10,10);
  */
  //**************************************************************************************
  
  if ( (Int_t)LiSi.size() > 0)
    for (vector< vector<Int_t> >::size_type j = 0; j < LiSi.size(); j++){
      // fix layer number
      Int_t nModulesInThisLayer = LiSi[j].size();
      if (nModulesInThisLayer == 0) break;
      fPlane   = CbmTrdAddress::GetLayerId(LiSi[j][0]);
      fStation = fPlane / 4 + 1;  // OK for SIS100 and SIS300
      fLayer   = fPlane % 4 + 1;  // OK for SIS100 and SIS300
    
      printf("Station: %i  Layer: %i   nModules: %i\n",fStation,fLayer,nModulesInThisLayer);
 
      h2Topview[0]->Reset();
      h2Topview[1]->Reset();
      h2Topview[2]->Reset();

      OutFile1.Form("pics/HitRateLayerPadView_S%d_L%d.png", fStation,fLayer);
      OutFile2.Form("pics/HitRateLayerSpectrum_S%d_L%d.png",fStation,fLayer);

      HistoInit(c1, c2, c3, h2Layer, h1HitPad, ZRangeL, ZRangeU, mm2bin);

      if(fDraw){
	c2->cd(1);
	MaxHitRatePerPad->Draw("same");  // draw red line
      }
      // generate HitPerPadSpectra
      // generate png files
      Lines = false;
      //c3 = (TCanvas*)c1->Clone("c3");
      if (nModulesInThisLayer > 0)
	for (vector<int>::size_type i = 0; i < nModulesInThisLayer; i++){
	  printf("     ModuleAddress: %i\n",LiSi[j][i]);
          myfile << "# ModuleAddress: " << LiSi[j][i] << endl;
	  ScanModulePlane(LiSi[j][i], c1, c3, h1HitPad, h1HitAsic);
	  
	}
      /*
        GetModuleInformationFromDigiPar(GeoPara, Fast, Lines, LiSi[j][i], h2Layer ,c1, h1HitPad, c2, h2Topview, c0, mm2bin);

	if(fDraw)
	{
	c1->cd(1)->SetLogz(1);
	h2Layer->Draw("colz");  // draw layer view
	}

	Lines = true;
	for (vector<int>::size_type i = 0; i < nModulesInThisLayer; i++)
	ScanModulePlane();
	GetModuleInformationFromDigiPar(GeoPara, Fast, Lines, LiSi[j][i], h2Layer ,c1, h1HitPad, c2, h2Topview, c0, mm2bin);
      */
      if(fDraw)  // dump png file for this layer
	{
	  Outimage1 = TImage::Create();
	  Outimage1->FromPad(c1);
	  Outimage1->WriteImage(OutFile1);
	  OutFile1.ReplaceAll("png","pdf");
	  c1->SaveAs(OutFile1);
	  OutFile1.ReplaceAll("Pad","ASIC");
	  if (c1)
	    delete c1;
	  c3->SaveAs(OutFile1);
	  OutFile1.ReplaceAll("pdf","png");
	  c3->SaveAs(OutFile1);
	  if (c3)
	    delete c3;
	    
	  c2->cd(1);
	  h1HitPad->Draw("same"); 

          c2->cd(2);
	  h1HitAsic->Draw();   
	  h1HitAsic->Write("", TObject::kOverwrite);
          c2->Update();
          TLine* MaxDataRatePerUplink = new TLine(500,0.7*gPad->GetUymax(),500,gPad->GetUymax());  // 500 Mbit per Uplink
          // TLine* MaxDataRatePerUplink = new TLine(500,60,500,80);  // 500 Mbit per Uplink
          MaxDataRatePerUplink->SetLineColor(2);  // make it red
          MaxDataRatePerUplink->SetLineWidth(8);  // make it thick
          MaxDataRatePerUplink->Draw("same");  // draw red line

	  c2->cd(3);
	  h1DataModule->Draw();   
	  h1DataModule->Write("", TObject::kOverwrite);
          c2->Update();

          TLine* MaxDataRatePerOptLink = new TLine(5000,0.7*gPad->GetUymax(),5000,gPad->GetUymax());  // 500 Mbit per Uplink
          // TLine* MaxDataRatePerOptLink = new TLine(5000,5,5000,8);  // 500 Mbit per Uplink
          MaxDataRatePerOptLink->SetLineColor(2);  // make it red
          MaxDataRatePerOptLink->SetLineWidth(8);  // make it thick
          MaxDataRatePerOptLink->Draw("same");  // draw red line

	  c2->cd(4);
	  h1OptLinksModule->Draw();   
	  h1OptLinksModule->Write("", TObject::kOverwrite);

	  Outimage2 = TImage::Create();
	  Outimage2->FromPad(c2);
	  Outimage2->WriteImage(OutFile2);
	  OutFile1.ReplaceAll("png","pdf");
	  c2->SaveAs(OutFile2);  
	  OutFile1.ReplaceAll("Pad","ASIC");
	  c2->SaveAs(OutFile2);

	  h1HitAsic->Reset();
	  h1DataModule->Reset();   
	  h1OptLinksModule->Reset();   

	  if (c2)
	    delete c2;
	  if (Outimage1)
	    delete Outimage1; 
	  if (Outimage2)
	    delete Outimage2; 
	}
      if (h2Layer)
	delete h2Layer;
      if (h1HitPad)
	delete h1HitPad;
      if(fDraw)
	c0->Update();
    }
  if(fDraw)
    c0->Update();
  h1HitAsic->Write("", TObject::kOverwrite);

  printf("     --------------------------\n");
  printf("     total number of ASICs        : %d\n", nTotalAsics);
  printf("     total number of optical links: %d\n", nTotalOptLinks);
  printf("     total TRD data rate          : %.2f (Gbit/s)\n", trdTotalDataRate * 1e-3 );
  printf("     --------------------------\n");
  printf("     --------------------------\n");

  myfile << "# total number of ASICs: " << nTotalAsics << endl;
  myfile << "#--------------------------" << endl;
  myfile << "#--------------------------" << endl;
  myfile.close();
}


void CbmTrdHitRateFastQa::ScanModulePlane(const Int_t moduleAddress, TCanvas*& c1, TCanvas*& c2, TH1F*& HitPad, TH1F*& HitAsic)
{
  c1->cd(1);
  Double_t ratePerModule = 0;  // sum of data from this module
  TVector3 padPos;
  TVector3 padSize;
  fModuleInfo = fDigiPar->GetModule(moduleAddress);
  gGeoManager->FindNode(fModuleInfo->GetX(), fModuleInfo->GetY(), fModuleInfo->GetZ());
  /*gGeoManager->FindNode(fModuleInfo->GetX(),
    fModuleInfo->GetY(),
    fModuleInfo->GetZ());
    printf("%s\n",TString(gGeoManager->GetPath()).Data());
  */
  std::vector<Int_t> AsicAddresses = fModuleInfo->GetAsicAddresses();
  Int_t nofAsics = fModuleInfo->GetNofAsics();
  printf("     NofAsics:     %3i\n",nofAsics);
  myfile << "# NofAsics     : " << nofAsics << endl;
  myfile << "# moduleAddress / Asic ID / hits per 32ch Asic per second" << endl;
  nTotalAsics += nofAsics;
  std::map<Int_t, Double_t> ratePerAsicMap;
  for (Int_t iAsic = 0; iAsic < nofAsics; iAsic++){
    ratePerAsicMap[AsicAddresses[iAsic]] = 0.;
  }
  const Int_t nSec = fModuleInfo->GetNofSectors();
  for (Int_t s = 0; s < nSec; s++){
    const Int_t nRow = fModuleInfo->GetNofRowsInSector(s);
    const Int_t nCol = fModuleInfo->GetNofColumnsInSector(s);
    for (Int_t r = 0; r < nRow; r++){
      for (Int_t c = 0; c < nCol; c++){
	fModuleInfo->GetPosition(moduleAddress, s, c, r, padPos, padSize);// padPos local or global???
	Int_t channelAddress = CbmTrdAddress::GetAddress(CbmTrdAddress::GetLayerId(moduleAddress),
							 CbmTrdAddress::GetModuleId(moduleAddress), 
							 s, r, c);
	Double_t local_min[3] = {padPos[0]-0.5*padSize[0], padPos[1]-0.5*padSize[1], padPos[2]};
	Double_t local_max[3] = {padPos[0]+0.5*padSize[0], padPos[1]+0.5*padSize[1], padPos[2]};
	//Double_t master_min[3];
	//Double_t master_max[3];
	//gGeoManager->LocalToMaster(local_min, master_min);
	//gGeoManager->LocalToMaster(local_max, master_max);
	if (fModuleInfo->GetOrientation() == 1 || fModuleInfo->GetOrientation() == 3){ // Pad size is in local coordinate system where position is in global coordinate system
	  local_min[0] = padPos[0]-0.5*padSize[1];
	  local_min[1] = padPos[1]-0.5*padSize[0];
	  local_max[0] = padPos[0]+0.5*padSize[1];
	  local_max[1] = padPos[1]+0.5*padSize[0];
	}
	//Double_t global_min[3];
	//Double_t global_max[3];
	//Double_t x_min(padPos[0]-0.5*padSize[0]), x_max(padPos[0]+0.5*padSize[0]), y_min(padPos[1]-0.5*padSize[1]), y_max(padPos[1]+0.5*padSize[1]);
	//gGeoManager->LocalToMaster(local_min, global_min);
	//gGeoManager->LocalToMaster(local_max, global_max);
	//TBox *pad = new TBox(x_min, y_min, x_max, y_max);
	for (Int_t i = 0; i < 3; i++) {
	  //master_min[i] *= 10.;
	  //master_max[i] *= 10.;
	  local_min[i] *= 10.;
	  local_max[i] *= 10.;
	}
	//Double_t rate = CalcHitRatePad(global_min[0], global_max[0], global_min[1], global_max[1], global_max[2]);
	Double_t rate = CalcHitRatePad(local_min[0], local_max[0], local_min[1], local_max[1], local_max[2]);
	HitPad->Fill(rate);
        ratePerModule += rate;  // increase sum by rate of this asic

	//TBox *pad = new TBox(global_min[0], global_min[1], global_max[0], global_max[1]);
	TBox *pad = new TBox(local_min[0], local_min[1], local_max[0], local_max[1]);
	//printf("    %i %i %i  (%f, %f)   (%f, %f)   %f\n",s,r,c,local_min[0],local_min[1],global_min[0],global_min[1],rate);
	pad->SetLineColor(0);
	pad->SetLineWidth(0);	
	Int_t color(0), j(0);
	while ((rate > fZLevel[j]) && (j < (Int_t)fZLevel.size())){
	  //printf ("              %i<%i %i    %E <= %E\n",j,(Int_t)fZLevel.size(),fColors[j], rate, fZLevel[j]);
	  j++;
	}
	//printf ("%i<%i %i    %E <= %E\n\n",j,(Int_t)fZLevel.size(),fColors[j], rate, fZLevel[j]);
	pad->SetFillColor(fColors[j]);
	if (j >= fZLevel.size() || rate > 1E05)
	  pad->SetFillColor(12);
	pad->Draw("same");

	//delete pad;
	Int_t AsicAddress = fModuleInfo->GetAsicAddress(channelAddress);
	if (AsicAddress < 0) 
	  LOG(ERROR) << "CbmTrdHitRateFastQa::ScanModulePlane: Channel address:" << channelAddress << " is not initialized in module " << moduleAddress << "(s:" << s << ", r:" << r << ", c:" << c << ")" << FairLogger::endl;
	ratePerAsicMap[AsicAddress] += rate;
      }
    }
    //TLine *sec = new TLine();
    //sec->SetLineStyle(2);
    //sec->Draw("same");
  }
  TBox *module = new TBox(fModuleInfo->GetX()*10-fModuleInfo->GetSizeX()*10,
			  fModuleInfo->GetY()*10-fModuleInfo->GetSizeY()*10,
			  fModuleInfo->GetX()*10+fModuleInfo->GetSizeX()*10,
			  fModuleInfo->GetY()*10+fModuleInfo->GetSizeY()*10);
  module->SetFillStyle(0);
  module->Draw("same");
  c1->Update();
  c2->cd(1);
  module->Draw("same");
  c2->Update();
  CbmTrdAsic *Asic = NULL; 
  TBox *asic = NULL;
  CbmTrdUtils *utils = new CbmTrdUtils();
  utils->InitColorVector(true, 0., 4e6);
  for (Int_t iAsic = 0; iAsic < nofAsics; iAsic++){
    Asic = fModuleInfo->GetAsic(AsicAddresses[iAsic]);

    Double_t local_min[3]; 
    Double_t local_max[3];
    Double_t master_min[3];
    Double_t master_max[3];
    local_min[0] = Asic->GetX() - 0.5 * Asic->GetSizeX();
    local_min[1] = Asic->GetY() - 0.5 * Asic->GetSizeY();
    local_min[2] = fModuleInfo->GetZ();
    local_max[0] = Asic->GetX() + 0.5 * Asic->GetSizeX();
    local_max[1] = Asic->GetY() + 0.5 * Asic->GetSizeY();
    local_max[2] = fModuleInfo->GetZ();
    gGeoManager->LocalToMaster(local_min, master_min);
    gGeoManager->LocalToMaster(local_max, master_max);
    //printf("moduleAddress:%10i O:%i Asic:%3i  rate:%E\n",moduleAddress,fModuleInfo->GetOrientation(),AsicAddresses[iAsic],ratePerAsicMap[AsicAddresses[iAsic]]);
    asic = new TBox(10.*master_min[0],
		    10.*master_min[1],
		    10.*master_max[0],
		    10.*master_max[1]);
    c2->cd(1);   
    if (ratePerAsicMap[AsicAddresses[iAsic]] > 4e6)
      asic->SetFillColor(kBlack);
    else
      asic->SetFillColor( utils->GetColorCode(ratePerAsicMap[AsicAddresses[iAsic]]));

    //    myfile << iAsic << " " << AsicAddresses[iAsic] << " " << ratePerAsicMap[AsicAddresses[iAsic]] << endl;
    myfile << moduleAddress << " " << setfill('0') << setw(2) << iAsic << " " 
           << setiosflags(ios::fixed) << setprecision(0) << setfill(' ') << setw(8) 
           << ratePerAsicMap[AsicAddresses[iAsic]] << endl;

    Double_t dataPerAsic = ratePerAsicMap[AsicAddresses[iAsic]] * 3 * 1e-6 * fBitPerHit;  // Mbit, incl. neighbor
    HitAsic->Fill(dataPerAsic);
    asic->Draw("same");
    //c2->Update();
  }

  Double_t dataPerModule = ratePerModule * 3 * 1e-6 * fBitPerHit;  // Mbit, incl. neighbor
  Int_t    nOptLinks     = 1 + dataPerModule / 4000.; // 5000.; // 1 link plus 1 for each 4 Gbps (fill links to 80% max)
  h1DataModule->Fill(dataPerModule);
  h1OptLinksModule->Fill(nOptLinks);

  // global statistics
  nTotalOptLinks += nOptLinks;
  trdTotalDataRate += dataPerModule;

  printf("     data rate: %11.4f (Gbit/s)\n", dataPerModule * 1e-3 );
  printf("     opt links: %6i\n", nOptLinks );
  printf("     --------------------------\n");

  myfile << "#--------------------------" << endl;
}


void CbmTrdHitRateFastQa::HistoInit(TCanvas*& c1, TCanvas*& c2, TCanvas*& c3, TH2F*& Layer,TH1F*& HitPad, Double_t ZRangeL, Double_t ZRangeU, Double_t mm2bin)
  {
    Char_t name[50];
    Char_t title[50];


    sprintf(name,"HP_S%d_L%d",fStation,fLayer);
    sprintf(title,"HitPad_Station %d, Layer %d",fStation,fLayer);

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
    HitPad = new TH1F(name,title,cnbins,cxbins);

    //    HitPad = new TH1F(name,title,10000,1e02,1e06);
    HitPad->SetXTitle("Hits/Pad [Hz]");
    HitPad->SetYTitle("count");
    HitPad->GetYaxis()->SetRangeUser(1,1e04);

    sprintf(name,"c2_S%d_L%d",fStation,fLayer);
    sprintf(title,"c2 Station %d, Layer %d",fStation,fLayer);
    if(fDraw){
      c2 = new TCanvas(name,title,1600,900);	
      c2->Divide(2,2); // (3,1);
      c2->cd(1)->SetLogx(1);
      c2->cd(1)->SetLogy(1);
      c2->cd(1)->SetGridx(1);
      c2->cd(1)->SetGridy(1);  
      HitPad->Draw();

      // h1HitAsic
      c2->cd(2)->SetLogx(1);  // Data per 32ch Asic lin/log scale
      c2->cd(2)->SetLogy(0);  // will be overwritten later
      c2->cd(2)->SetGridx(1);
      c2->cd(2)->SetGridy(1); 

      // h1DataModule
      c2->cd(3)->SetLogx(1);  // Data per Module lin/log scale
      c2->cd(3)->SetLogy(0);  // will be overwritten later
      c2->cd(3)->SetGridx(1);
      c2->cd(3)->SetGridy(1); 

      // h1OptLinksModule
      c2->cd(4)->SetLogx(0);  // Data per Module lin/log scale
      c2->cd(4)->SetLogy(0);  // will be overwritten later
      c2->cd(4)->SetGridx(1);
      c2->cd(4)->SetGridy(1); 
    }

    sprintf(name,"S%d_L%d",fStation,fLayer);
    sprintf(title,"Station %d, Layer %d",fStation,fLayer);
    printf("%s\n",title);

    Layer = new TH2F(name,title,int(2* winsize /mm2bin), -winsize, winsize, int(2* winsize /mm2bin), -winsize, winsize);
    Layer->SetContour(99);
    Layer->SetXTitle("x-Coordinate [mm]");
    Layer->SetYTitle("y-Coordinate [mm]");
    Layer->SetZTitle("Hits/Pad [Hz]");
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
    Layer->GetZaxis()->SetRangeUser(ZRangeL,ZRangeU);
    Layer->Fill(0.,0.,0.);

    sprintf(name,"c1_S%d_L%d",fStation,fLayer);
    sprintf(title,"c1 Station %d, Layer %d",fStation,fLayer);
    if(fDraw){
      c1 = new TCanvas(name,title,1000,900);	
      c1->Divide(1,1);
      c1->cd(1)->SetLogz(1);
      Layer->DrawCopy("colz");  
      sprintf(name,"c3_S%d_L%d",fStation,fLayer);
      sprintf(title,"c3 Station %d, Layer %d",fStation,fLayer);
      c3 = new TCanvas(name,title,1000,900);	
      c3->Divide(1,1);
      c3->cd(1)->SetLogz(1);
      Layer->GetZaxis()->SetRangeUser(1.,4e6);
      Layer->SetZTitle("Hits/Asic [Hz]");
      Layer->DrawCopy("colz");
    }
  }

  // --------------------------------------------------------------------

  // ---- FinishTask-----------------------------------------------------
  void CbmTrdHitRateFastQa::FinishEvent()
  {
    fDigiMap.clear();
    if ( fDigiCollection ) fDigiCollection->Clear();
    if ( fDigiMatchCollection ) fDigiMatchCollection->Clear();
  }

  // --------------------------------------------------------------------
  // ----GetModuleInformation ------------------------------------------
  void CbmTrdHitRateFastQa::GetModuleInformation()
  {
    // Extract the information about station, layer, module type
    // and cpoy number of the module from the full path to the
    // node.
    // The full path is tokenized at the "/" which diide the different
    // levels of the geometry.
    // Knowing the nameing scheme of the volumes one gets the required
    // information with simple string manipulation.
    // This is probably not the fastes way, but the speed has to be checked.
    // The methode works only for versions of Root > 5.20.0, before the
    // class TStringTocken is not implemented

    TString path = gGeoManager->GetPath();
    cout<<"Path: "<<path<<endl;
    TStringToken* bla = new TStringToken(path,"/");

    while (bla->NextToken()) {
      if (bla->Contains("layer")) {
	TString bla3 = (TString) *bla;
	Ssiz_t pos = bla3.Last('_');
	Ssiz_t substringLength=bla3.Length()-pos-1;
	TString bla2 = bla3((bla3.Last('_')+1),substringLength);
	TString bla1 = bla3(3,1);
	fStation=bla1.Atoi();
	fLayer=bla2.Atoi();
      }
      if (bla->Contains("mod")){
	TString bla3 = (TString) *bla;
	Ssiz_t pos = bla3.Last('_');
	Ssiz_t substringLength=bla3.Length()-pos-1;
	TString bla2 = bla3(pos+1,substringLength);
	substringLength=pos-7;
	TString bla1 = bla3(7,substringLength);     
	break;
      } 
    }

  }

  // --------------------------------------------------------------------
  // ----GetModuleInformationFromDigiPar ------------------------------------------
  void CbmTrdHitRateFastQa::GetModuleInformationFromDigiPar(HitRateGeoPara2 *GeoPara, Bool_t Fast, Bool_t Lines, Int_t VolumeID, TH2F* Layer, TCanvas* c1, TH1F* HitPad, TCanvas* c2, TH2F* Topview[3], TCanvas* c0, Double_t mm2bin)
  {
    // fPos is >0 for x and y and not rotated
    // origin of the local coordinate system in 
    // the lower left corner of the chamber, 
    // x to the right (small side of the pads), y up
    //cout << "GetModuleInformationFromDigiPar" << endl;

    fModuleInfo = fDigiPar->GetModule(VolumeID);
    if (fModuleInfo != NULL)
      {

	// fill GeoPara
	TVector3 padPos;
	TVector3 padSize;
     
	GeoPara->nSec      = fModuleInfo->GetNofSectors();  // is always 3
	GeoPara->moduleId  = VolumeID;
	GeoPara->layerId   = fLayer;
	GeoPara->stationId = fStation;

	GeoPara->mPos[0] = fModuleInfo->GetX() * 10;
	GeoPara->mPos[1] = fModuleInfo->GetY() * 10;
	GeoPara->mPos[2] = fModuleInfo->GetZ() * 10; // == z(station) ??
    
	GeoPara->mSize[0] = fModuleInfo->GetSizeX() * 10;
	GeoPara->mSize[1] = fModuleInfo->GetSizeY() * 10;
	GeoPara->mSize[2] = 0; 

	GeoPara->nCol = 0;  // reset total number of columns
	GeoPara->nRow = 0;  // reset total number of rows   

	for (Int_t s = 0; s < GeoPara->nSec; s++)  // for all (3) sectors
	  {
	    GeoPara->sCol[s] = 0;  // reset number of columns in sector
	    GeoPara->sRow[s] = 0;  // reset number of rows    in sector

	    fModuleInfo->GetPosition(VolumeID, s, 0, 0, padPos, padSize);
	    GeoPara->pSize[s][2] = 0;

	    for (Int_t i = 0; i < 2; i++)  // for x and y
	      {
		GeoPara->pSize[s][i] = padSize[i] * 10;  // convert to mm

		if (i == 0) // x direction
		  {
		    GeoPara->sSize[s][i] = fModuleInfo->GetSectorSizeX(s) * 10;
		    if (GeoPara->sSize[s][i] < 2 * GeoPara->mSize[i]) // if sector smaller than module
		      {
			GeoPara->sCol[s] = round(GeoPara->sSize[s][i] / GeoPara->pSize[s][i]); // calculate number of pads
		      }
		    else // only one sector
		      {
			if (s == 0) 
			  GeoPara->sCol[s] = round(GeoPara->sSize[s][i] / GeoPara->pSize[s][i]); // calculate number of pads
			else        
			  GeoPara->sCol[s] = 0; // set other sectors 0
		      }
		    GeoPara->nCol += GeoPara->sCol[s];  // sum up columns
		  }

		if (i == 1) // y direction
		  {
		    GeoPara->sSize[s][i] = fModuleInfo->GetSectorSizeY(s) * 10;
		    if (GeoPara->sSize[s][i] < 2 * GeoPara->mSize[i]) // if sector smaller than module
		      {
			GeoPara->sRow[s] = round(GeoPara->sSize[s][i] / GeoPara->pSize[s][i]);
		      }
		    else // only one sector
		      {
			if (s == 0) 
			  GeoPara->sRow[s] = round(GeoPara->sSize[s][i] / GeoPara->pSize[s][i]); // calculate number of pads
			else        
			  GeoPara->sRow[s] = 0; // set other sectors 0
		      }
		    GeoPara->nRow += GeoPara->sRow[s];  // sum up columns
		  }

	      }
	  }

	// swap, if required
	if (GeoPara->nRow > GeoPara->nCol)  // if mofre rows than columns
	  {
	    Int_t    itemp;
	    Double_t dtemp;

	    itemp = GeoPara->nRow;
	    GeoPara->nRow = GeoPara->nCol;
	    GeoPara->nCol = itemp;

	    for (Int_t s = 0; s < GeoPara->nSec; s++)  // for all (3) sectors
	      {
		dtemp = GeoPara->sRow[s];  
		GeoPara->sRow[s] = GeoPara->sCol[s];
		GeoPara->sCol[s] = dtemp;

		dtemp = GeoPara->sSize[s][0];  
		GeoPara->sSize[s][0] = GeoPara->sSize[s][1];
		GeoPara->sSize[s][1] = dtemp;

		dtemp = GeoPara->pSize[s][0];  
		GeoPara->pSize[s][0] = GeoPara->pSize[s][1];
		GeoPara->pSize[s][1] = dtemp;
	      }
	    cout << "swapped x and y" << endl;
	  }

	// get origin
	fModuleInfo->GetPosition(VolumeID, 0, 0, 0, padPos, padSize);
	GeoPara->vOrigin[0] = padPos[0] * 10;
	GeoPara->vOrigin[1] = padPos[1] * 10; 
	GeoPara->vOrigin[2] = padPos[2] * 10;
    
	// get col offset
	fModuleInfo->GetPosition(VolumeID, 0, 1, 0, padPos, padSize);
	GeoPara->vX[0]      = padPos[0] * 10 - GeoPara->vOrigin[0]; 
	GeoPara->vX[1]      = padPos[1] * 10 - GeoPara->vOrigin[1];			   
	GeoPara->vX[2]      = padPos[2] * 10 - GeoPara->vOrigin[2];

	// get row offset
	//    fModuleInfo->GetPosition(VolumeID, 0, 0, 1, padPos, padSize); // rather take next of 3 sectors
	fModuleInfo->GetPosition(VolumeID, 1, 0, 0, padPos, padSize);
	GeoPara->vY[0]      = padPos[0] * 10 - GeoPara->vOrigin[0]; 
	GeoPara->vY[1]      = padPos[1] * 10 - GeoPara->vOrigin[1]; 
	GeoPara->vY[2]      = padPos[2] * 10 - GeoPara->vOrigin[2];

	// normal vector
	GeoPara->vN[0]      = GeoPara->vX[1]*GeoPara->vY[2] - GeoPara->vX[2]*GeoPara->vY[1];
	GeoPara->vN[1]      = GeoPara->vX[2]*GeoPara->vY[0] - GeoPara->vX[0]*GeoPara->vY[2];
	GeoPara->vN[2]      = GeoPara->vX[0]*GeoPara->vY[1] - GeoPara->vX[1]*GeoPara->vY[0];

	// inclination angle
	GeoPara->lambda = (GeoPara->vOrigin[0] * GeoPara->vN[0] +
			   GeoPara->vOrigin[1] * GeoPara->vN[1] +
			   GeoPara->vOrigin[2] * GeoPara->vN[2]);

	/*x-direction*/
	GeoPara->cosX = (GeoPara->vX[0] * 1 + GeoPara->vX[1] * 0 + GeoPara->vX[2] * 0) / sqrt(pow(GeoPara->vX[0],2) + pow(GeoPara->vX[1],2) + pow(GeoPara->vX[2],2));
	/*y-direction*/
	GeoPara->cosY = (GeoPara->vY[0] * 0 + GeoPara->vY[1] * 1 + GeoPara->vY[2] * 0) / sqrt(pow(GeoPara->vY[0],2) + pow(GeoPara->vY[1],2) + pow(GeoPara->vY[2],2));

	// check and fix cos values (were larger than 1)
	//    cout << "a cosX " << GeoPara->cosX << " cosY " << GeoPara->cosY << endl;

	if (GeoPara->cosX >  1) GeoPara->cosX =  1;
	if (GeoPara->cosX < -1) GeoPara->cosX = -1;
	if (GeoPara->cosY >  1) GeoPara->cosY =  1;
	if (GeoPara->cosY < -1) GeoPara->cosY = -1;

	//    cout << "b cosX " << GeoPara->cosX << " cosY " << GeoPara->cosY << endl;

	if (GeoPara->vX[0] != 0)
	  GeoPara->stepDirection[0] = fabs(GeoPara->vX[0]) / (GeoPara->vX[0]); // is the next pad (1,0,0) on the left  or right side?
	else
	  GeoPara->stepDirection[0] =-fabs(GeoPara->vX[1]) / (GeoPara->vX[1]); // is the next pad (1,0,0) on the left  or right side?

	if (GeoPara->vY[1] != 0)
	  GeoPara->stepDirection[1] = fabs(GeoPara->vY[1]) / (GeoPara->vY[1]); // is the next pad (0,1,0) on the upper or lower side?
	else
	  GeoPara->stepDirection[1] =-fabs(GeoPara->vY[0]) / (GeoPara->vY[0]); // is the next pad (0,1,0) on the upper or lower side?

	// transfer values to old variables
	Double_t Mpos[3];
	Double_t Msize[3];
	for (Int_t i = 0; i < 3; i++)  // for x, y and z
	  {
	    Mpos[i]  = GeoPara->mPos[i];   // set module position
	    Msize[i] = GeoPara->mSize[i];  // set module size
	  }

	cout.setf(ios::fixed);
	cout.precision(0);
	if (Lines)
	  cout << "pos "   << setw(6) << Mpos[0]  << setw(6) << Mpos[1]  << setw(6) << Mpos[2] 
	       << " size " << setw(6) << Msize[0] << setw(6) << Msize[1] << setw(6) << Msize[2] << endl;


	const Int_t nSec = fModuleInfo->GetNofSectors();  // is always 3
	//      cout << nSec << " nSec" << endl;
	Double_t Ssize[3*nSec];  // sector size
	Double_t Psize[3*nSec];  // pad size
	for (Int_t iSec = 0; iSec < nSec; iSec++)// iSec = Sector
	  {
	    Ssize[0+iSec*nSec] = GeoPara->sSize[iSec][0];
	    Ssize[1+iSec*nSec] = GeoPara->sSize[iSec][1];
	    Ssize[2+iSec*nSec] = 0;
      
	    Psize[0+iSec*nSec] = GeoPara->pSize[iSec][0];
	    Psize[1+iSec*nSec] = GeoPara->pSize[iSec][1];
	    Psize[2+iSec*nSec] = 0;
	  }

	if (Lines)
	  cout << "sec   " << setw(10) << Ssize[0]  << setw(10) << Ssize[1]  
	       << setw(10) << Ssize[3]  << setw(10) << Ssize[4]  
	       << setw(10) << Ssize[6]  << setw(10) << Ssize[7] << endl; 

	cout.precision(2);
	if (Lines)
	  cout << "pad   " << setw(10) << Psize[0]  << setw(10) << Psize[1] 
	       << setw(10) << Psize[3]  << setw(10) << Psize[4] 
	       << setw(10) << Psize[6]  << setw(10) << Psize[7] << endl;

	// nCol and nRow
	Int_t nCol = GeoPara->nCol;  // columns per module - or - pads per row
	Int_t nRow = GeoPara->nRow;  // rows    per module

	const Int_t mCol = fModuleInfo->GetNofColumns(); 
	const Int_t mRow = fModuleInfo->GetNofRows();

	if (Lines)
	  {
	    cout << "col0  " << setw(10) << GeoPara->sCol[0] << "  row0  "<< setw(10) << GeoPara->sRow[0] << endl;
	    cout << "col1  " << setw(10) << GeoPara->sCol[1] << "  row1  "<< setw(10) << GeoPara->sRow[1] << endl;
	    cout << "col2  " << setw(10) << GeoPara->sCol[2] << "  row2  "<< setw(10) << GeoPara->sRow[2] << endl;
	    cout << "col   " << setw(10) << nCol             << "  row   "<< setw(10) << nRow << endl;

	    cout << "mcol  " << setw(10) << mCol             << "  mrow  "<< setw(10) << mRow << endl;
	    cout << "0col  " << setw(10) << fModuleInfo->GetNofColumnsInSector(0) 
		 << "  0row  " << setw(10) << fModuleInfo->GetNofRowsInSector(0) << endl;
	    cout << "1col  " << setw(10) << fModuleInfo->GetNofColumnsInSector(1) 
		 << "  1row  " << setw(10) << fModuleInfo->GetNofRowsInSector(1) << endl;
	    cout << "2col  " << setw(10) << fModuleInfo->GetNofColumnsInSector(2) 
		 << "  2row  " << setw(10) << fModuleInfo->GetNofRowsInSector(2) << endl;
	  }

       
	//// nCol and nRow
	//      Int_t nCol = 0;  // columns per module - or - pads per row
	//      Int_t nRow = 0;  // rows    per module
	//
	//      for (Int_t i = 0; i < fnSec; i++)
	//	{
	//	  if (Ssize[0+i*nSec] < 2 * Msize[0] && Ssize[0+i*nSec] > 0)  // sector size within module size
	//	    {
	//	      nCol += int(Ssize[0+i*nSec]/Psize[0+i*nSec]); 
	//	    }
	//	  else
	//	    {
	//	      nCol  = int(Ssize[0+i*nSec]/Psize[0+i*nSec]); 
	//	    }
	//
	//	  if (Ssize[1+i*nSec] < 2 * Msize[1] && Ssize[1+i*nSec] > 0)  // sector size within module size
	//	    {
	//	      nRow += int(Ssize[1+i*nSec]/Psize[1+i*nSec]); 
	//	    }
	//	  else
	//	    {
	//	      nRow  = int(Ssize[1+i*nSec]/Psize[1+i*nSec]); 
	//	    }
	//	}
	//
	//      if (Lines)
	//        cout << "nCol  " << setw(10) << nCol  << "  nRow  "<< setw(10) << nRow << endl;

	if (Lines)
	  {
	    cout << "vX0   " << setw(10) << GeoPara->vX[0] << endl;
	    cout << "vX1   " << setw(10) << GeoPara->vX[1] << endl;
	    cout << "vY0   " << setw(10) << GeoPara->vY[0] << endl;
	    cout << "vY1   " << setw(10) << GeoPara->vY[1] << endl;
	    cout << "step  " << setw(10) << GeoPara->stepDirection[0] << setw(10) << GeoPara->stepDirection[1] << endl;
	    cout << "cos   " << setw(10) << GeoPara->cosX  << setw(10) << GeoPara->cosY << endl;
	    cout << "v00xy " << setw(10) << GeoPara->mPos[0]-GeoPara->vOrigin[0] 
		 << setw(10) << GeoPara->mPos[1]-GeoPara->vOrigin[1] << endl;

	    //      cout << "------------------------------------------------------" << endl;
	  }

	// angles are relative to y direction
	if ((GeoPara->stepDirection[0] ==  1) && (GeoPara->stepDirection[1] ==  1)) // OK - vert up
	  GeoPara->rot_angle = 0;
	if ((GeoPara->stepDirection[0] == -1) && (GeoPara->stepDirection[1] ==  1)) // OK - hori left
	  GeoPara->rot_angle = 1;
	if ((GeoPara->stepDirection[0] == -1) && (GeoPara->stepDirection[1] == -1)) // OK - vert down
	  GeoPara->rot_angle = 2;
	if ((GeoPara->stepDirection[0] ==  1) && (GeoPara->stepDirection[1] == -1)) // OK - hori right
	  GeoPara->rot_angle = 3;

	//    GeoPara->ori       = fModuleInfo->GetOrientation();  // is 0,1,2,3
	if (Lines)
	  cout << "ori   " << setw(10) << fModuleInfo->GetOrientation() << "  rot   " << setw(10) << GeoPara->rot_angle << endl;

	//    if (GeoPara->rot_angle == 3)
	//    {
	//      fModuleInfo->GetPosition(VolumeID, 2, GeoPara->sCol[0], GeoPara->sRow[2], padPos, padSize);
	//      if (Lines)
	//	{
	////      cout << "v--xy " << setw(10) << GeoPara->sCol[0] 
	////                       << setw(10) << GeoPara->sRow[0] << endl;
	////      cout << "v--xy " << setw(10) << GeoPara->sCol[1] 
	////                       << setw(10) << GeoPara->sRow[1] << endl;
	////      cout << "v--xy " << setw(10) << GeoPara->sCol[2] 
	////                       << setw(10) << GeoPara->sRow[2] << endl;
	//      cout << "v11xy " << setw(10) << padPos[0] * 10 - GeoPara->vOrigin[0] 
	//                       << setw(10) << padPos[1] * 10 - GeoPara->vOrigin[1] << endl;
	//        }
	//      GeoPara->vOrigin[0] = padPos[0] * 10;
	//      GeoPara->vOrigin[1] = padPos[1] * 10; 
	//    }

	if (Lines)
	  cout << "------------------------------------------------------" << endl;

	Topview[0]->Fill(GeoPara->mPos[0],GeoPara->mPos[2]);
	Topview[1]->Fill(GeoPara->mPos[0],GeoPara->mPos[1]);
	Topview[2]->Fill(GeoPara->mPos[2],GeoPara->mPos[1]);
    
	if (Lines)
	  {
	    DrawPads(GeoPara, Layer, c1);
	    DrawBorders(GeoPara, Layer, c1);
	  }
	else
	  {
	    Histo(GeoPara, Fast, Layer, c1, HitPad, c2, Topview, c0, mm2bin);
	  }

	if (Lines)
	  for (Int_t s = 0; s < GeoPara->nSec; s++)  // for all (3) sectors
	    {
	      fModuleInfo->GetPosition(VolumeID, s, 0, 0, padPos, padSize);
  
	      TPolyMarker* start = new TPolyMarker(1);
	      start->SetPoint(0, padPos(0)*10, padPos(1)*10);
	      start->SetMarkerStyle(22);
	      start->SetMarkerSize(.8);
    
	      if(fDraw)
		{
		  c1->cd(1);
		  start->Draw("same");
		}
  
	      fModuleInfo->GetPosition(VolumeID, s, GeoPara->sCol[0]-1, GeoPara->sRow[s]-1, padPos, padSize);
  
	      TPolyMarker* end = new TPolyMarker(1);
	      end->SetPoint(0, padPos(0)*10, padPos(1)*10);
	      end->SetMarkerStyle(20);
	      end->SetMarkerSize(.8);
    
	      if(fDraw)
		{
		  c1->cd(1);
		  end->Draw("same");
		}
  
	    }

      }

    else
      printf("fModuleInfo == NULL\n");
  }
  Double_t CbmTrdHitRateFastQa::CalcHitRatePad(const Double_t x_min, const Double_t x_max, const Double_t y_min, const Double_t y_max, const Double_t z){ //[mm]
    Double_t a[3] = { 7.66582e+00,  6.97712e+00,  6.53780e+00};
    Double_t b[3] = {-2.72375e-03, -1.85168e-03, -1.42673e-03};
    Double_t r(0), alpha(0), HitRate(0);
    Double_t xStepWidth(1), yStepWidth(1); // [mm]
    const Int_t xSteps = Int_t(fabs(x_max - x_min)/xStepWidth)-1;
    const Int_t ySteps = Int_t(fabs(y_max - y_min)/yStepWidth)-1;
    Double_t x(x_min + 0.5), y(y_min + 0.5);
    const Double_t padArea = fabs(x_max - x_min) * fabs(y_max - y_min);
    for (Int_t yStep = 0; yStep < ySteps; yStep++) {
      x = x_min + 0.5;
      for (Int_t xStep = 0; xStep < xSteps; xStep++) {
	r = sqrt( pow(x, 2) + pow(y,2));
	alpha = atan(r/z)*1000.; //[mrad]
	HitRate += // fit including errors
	  exp(4.536355e00 + -8.393716e-03 * alpha) + 
	  exp(2.400547e01 + -1.208306e-02 * alpha) /
	  (z * z);
	x += xStepWidth;
      }
      y += yStepWidth;
    }
    return (HitRate);// [Hz/mm^2]
  }

  Double_t CbmTrdHitRateFastQa::CalcHitRate(HitRateGeoPara2 *GeoPara, Double_t StartX, Double_t StopX, Int_t xSteps, Double_t StartY, Double_t StopY, Int_t ySteps, Double_t* Mpos, TH2F* Topview[3], TCanvas* c0)
  {
    //cout << "CalcHitRate" << endl;
    Double_t HitRate = 0;//1. / sqrt( pow( StartX,2) + pow( StartY,2));
    Double_t r = 0;
    Double_t alpha = 0;
    Int_t counter = 0;
    Double_t a[3] = { 7.66582e+00,  6.97712e+00,  6.53780e+00};
    Double_t b[3] = {-2.72375e-03, -1.85168e-03, -1.42673e-03};

    Double_t xStepWidth = GeoPara->cosX;
    Double_t yStepWidth = GeoPara->cosY;
    Double_t x = StartX + 0.5 * GeoPara->cosX; // don't start on the pad border line
    Double_t y = StartY + 0.5 * GeoPara->cosY;

    //  Double_t xStepWidth = fabs(GeoPara->cosX);
    //  Double_t yStepWidth = fabs(GeoPara->cosY);
    //  Double_t x = StartX + 0.5 * fabs(GeoPara->cosX); // don't start on the pad border line
    //  Double_t y = StartY + 0.5 * fabs(GeoPara->cosY);
    Double_t z = (GeoPara->lambda - (x * GeoPara->vN[0] + y * GeoPara->vN[1])) / GeoPara->vN[2];
    for (Int_t yStep = 0; yStep < ySteps; yStep++) {
      x = StartX + 0.5 * GeoPara->cosX;
      //    x = StartX + 0.5 * fabs(GeoPara->cosX);
      for (Int_t xStep = 0; xStep < xSteps; xStep++) {
	z = (GeoPara->lambda - (x * GeoPara->vN[0] + y * GeoPara->vN[1])) / GeoPara->vN[2];
	r = sqrt( pow(x/1.5, 2) + pow(y,2));
	alpha = atan(r/z)*1000.;
	/* //Fit without errors
	   HitRate += 
	   exp(4.54156e00 + -8.47377e-03 * alpha) + 
	   exp(2.40005e01 + -1.19541e-02 * alpha) /
	   (z * z)
	   ;
	*/
	HitRate += // fit including errors
	  exp(4.536355e00 + -8.393716e-03 * alpha) + 
	  exp(2.400547e01 + -1.208306e-02 * alpha) /
	  (z * z)
	  ;
	/*
	  Topview[0]->Fill(x,z);
	  Topview[1]->Fill(x,y);
	  Topview[2]->Fill(z,y);
	*/
	x += xStepWidth;
      }
      y += yStepWidth;
    }
    return (HitRate/*/(counter/100.)*/);/*Convertes Hits/Pad -> Hits/cm² on each Pad*/
  }


void CbmTrdHitRateFastQa::Histo(HitRateGeoPara2 *GeoPara, Bool_t Fast, TH2F* h2Layer, TCanvas* c1, TH1F* h1HitPad, TCanvas* c2, TH2F* Topview[3], TCanvas* c0, Double_t mm2bin)
{
  Double_t ZRangeL = 1e00;//1e05;
  Double_t ZRangeU = 1e05;//1e06;
  TString name;

  // show only current module name
  name.Form("ModuleID %5d",GeoPara->moduleId);
  //  cout << "      " << name << "\r" << flush;
  cout << "      " << name << "\n" << flush;

  name.Form("Module%05d",GeoPara->moduleId);  
  TH2F* h2Module; 
  if (GeoPara->mSize[0] < 500)  // small module
    h2Module = new TH2F(name,name, 600/mm2bin+1,-300.5,300.5, 600/mm2bin+1,-300.5,300.5);   //  501 x  501 bins
  else  // large module
    h2Module = new TH2F(name,name,1000/mm2bin+1,-500.5,500.5, 1000/mm2bin+1,-500.5,500.5);  // 1001 x 1001 bins
  //  TH2F *h2Module = new TH2F(name,name,1500/mm2bin+1,-750.5,750.5,1500/mm2bin+1,-750.5,750.5);
  h2Module->GetZaxis()->SetRangeUser(ZRangeL,ZRangeU);

  name.Form("Module%05dHitPad",GeoPara->moduleId);
  TH1F* h1HitPadModule = new TH1F(name,name,1200,0,120000);
  //  TH1F* h1HitPadModule = new TH1F(name,name,10000,1e00,1e06);

  //cout << "Histo" << endl;

  //  printf(" Xp %6d Xs %6d Yp %6d Ys %6d\n", (Int_t)GeoPara->mPos[0], (Int_t)GeoPara->mSize[0], (Int_t)GeoPara->mPos[1], (Int_t)GeoPara->mSize[1]);
  //  cout << endl << "Xp " << GeoPara->mPos[0] << " Xs " << GeoPara->mSize[0] << " Yp " << GeoPara->mPos[1] << " Ys " << GeoPara->mSize[1] << endl;

  Double_t HiteRate = 0;

  Int_t iSecX = 0;
  Int_t iSecY = 0;

  Double_t planeStartX = GeoPara->mPos[0]-GeoPara->mSize[0];
  Double_t planeStopX  = planeStartX + GeoPara->pSize[iSecX][0];

  Double_t planeStartY = GeoPara->mPos[1]-GeoPara->mSize[1];
  Double_t planeStopY  = planeStartY + GeoPara->pSize[iSecY][1];

  Double_t StartX = GeoPara->vOrigin[0] + 0.5 * GeoPara->pSize[iSecX][0] * GeoPara->cosX;  // vOrigin points to the center of pad (0,0,0)
  Double_t StopX  = StartX + GeoPara->pSize[iSecX][0] * GeoPara->cosX;

  Double_t StartY = GeoPara->vOrigin[1] + 0.5 * GeoPara->pSize[iSecY][1] * GeoPara->cosY;
  Double_t StopY  = StartY + GeoPara->pSize[iSecY][1] * GeoPara->cosY;

  Int_t xSteps = 0;
  Int_t ySteps = 0;

  //  cout << "x " << GeoPara->pSize[iSecX][0] << " y " <<  GeoPara->pSize[iSecX][1] << endl;

  //  printf("stX %6d spX %6d stY %6d spY %6d\n", (Int_t)StartX, (Int_t)StopX, (Int_t)StartY, (Int_t)StopY);

  //Int_t xStepDirection = GeoPara->vX[0] / fabs(GeoPara->vX[0]); // is the next pad (1,0,0) on the left or right side?
  //Int_t yStepDirection = GeoPara->vY[1] / fabs(GeoPara->vY[1]); // is the next pad (0,1,0) on the upper or lower side?

  //----------------------------------------------------------------------------------------

  //if (GeoPara->rot_angle == 1)
  //if (GeoPara->rot_angle != 3)
  for (Int_t iR = 0; iR < GeoPara->nRow; iR++)  // rows
    {
      StartX = GeoPara->vOrigin[0] + 0.5 * GeoPara->pSize[iSecX][0] * GeoPara->cosX; // why to the left?
      StopX  = StartX + GeoPara->pSize[iSecX][0] * GeoPara->cosX;
      planeStartX = GeoPara->mPos[0] - GeoPara->mSize[0];
      planeStopX  = planeStartX + GeoPara->pSize[iSecX][0];

      for (Int_t iC = 0; iC < GeoPara->nCol; iC++)  // for all columns
	{
	  xSteps = GeoPara->pSize[iSecX][0] - 1; // 1 Step / mm
	  ySteps = GeoPara->pSize[iSecY][1] - 1;

	  if (Fast)
	    {
	      //DE
	      //	      if (GeoPara->mPos[0] > -1 && GeoPara->mPos[1] > -1)
	      if (GeoPara->mPos[0] < 1 && GeoPara->mPos[1] < 1)  // copy the bottom left quadrant
		{
		  HiteRate = CalcHitRate(GeoPara, StartX, StopX, xSteps, StartY, StopY, ySteps, GeoPara->mPos, Topview, c0);
		  h1HitPad->Fill(HiteRate,4);  // weight 4 for each quadrant
		}
	    }

	  else  // slow, for each module
	    {
	      HiteRate = CalcHitRate(GeoPara, StartX, StopX, xSteps, StartY, StopY, ySteps, GeoPara->mPos, Topview, c0);
	      h1HitPad->Fill(HiteRate);
	      h1HitPadModule->Fill(HiteRate);  // single module, goes into root file
	    }

	  //   fill HitRate into h2Layer histo

	  Int_t mStepX = Int_t((planeStartX-GeoPara->mPos[0]));
	  Int_t mStepY = Int_t((planeStartY-GeoPara->mPos[1]));

	  for (Int_t stepY = int(planeStartY/mm2bin); stepY < int(planeStopY/mm2bin); stepY++)
	    {
	      //	      mStepX = Int_t((planeStartX-GeoPara->mPos[0]));

	      for (Int_t stepX = int(planeStartX/mm2bin); stepX < int(planeStopX/mm2bin); stepX++)
		{

		  if (Fast)
		    {
		      //DE
		      //		      if (GeoPara->mPos[0]/*GeoPara->mPos[0]*/ > -1 && GeoPara->mPos[1]/*GeoPara->mPos[1]*/ > -1)
		      if (GeoPara->mPos[0]/*GeoPara->mPos[0]*/ < 1 && GeoPara->mPos[1]/*GeoPara->mPos[1]*/ < 1)
			{
			  
			  h2Layer->SetBinContent(     stepX + int( winsize / mm2bin ),      stepY + int( winsize / mm2bin ), HiteRate); // bin coordiantes
			  h2Layer->SetBinContent(-1 * stepX + int( winsize / mm2bin ),      stepY + int( winsize / mm2bin ), HiteRate); // bin coordiantes
			  h2Layer->SetBinContent(     stepX + int( winsize / mm2bin ), -1 * stepY + int( winsize / mm2bin ), HiteRate); // bin coordiantes
			  h2Layer->SetBinContent(-1 * stepX + int( winsize / mm2bin ), -1 * stepY + int( winsize / mm2bin ), HiteRate); // bin coordiantes
			  											      
			  if (GeoPara->moduleId == 5)
			    {
			      //                            cout << "filling 5 " << mStepX << " " << mStepY << " " << HiteRate << endl;
			      h2Module->Fill(mStepX,mStepY,HiteRate);  // single module, goes into root file
			    }

			}											      
		    }												      
														      
		  else  // slow											      
		    {												      
														      
		      h2Layer ->SetBinContent(stepX + int( winsize / mm2bin ), 
					      stepY + int( winsize / mm2bin ), HiteRate); // bin coordinates

		      //                      cout << "StepX " << stepX << " StepY " << stepY << endl;

		      if (GeoPara->moduleId == 5)
			{
			  cout << "filling 5 " << mStepX << " " << mStepY << " " << HiteRate << endl;
			  h2Module->Fill(mStepX,mStepY,HiteRate);  // single module, goes into root file
			}
		      //                      cout << "mStepX " << mStepX << " mStepY " << mStepY << endl;

		    }

		  mStepX += mm2bin;
		}

	      mStepY += mm2bin;
	    }

	  //printf("Sx Sy (%d,%d)     nC nR (%d,%d) iC iR (%d,%d) SC SR (%d,%d)             Px Py (%.1f,%.1f) StartX StartY (%.1f,%.1f)\n",iSecX,iSecY,nC,nR,iC,iR,SecCol,SecRow,Psize[0+iSecX*nSec],Psize[1+iSecY*nSec],StartX,StartY);

	  //	  if (iC == GeoPara->sCol[iSecX]-1)  // why -1 ??
	  if (iC == GeoPara->sCol[iSecX])
	    { 
	      //	    cout << "iC " << iC << " " << GeoPara->nCol << " " << iSecX << endl;
	      iSecX++;
	    }  
	  StartX      += GeoPara->stepDirection[0] * GeoPara->pSize[iSecX][0] * GeoPara->cosX;
	  StopX       += GeoPara->stepDirection[0] * GeoPara->pSize[iSecX][0] * GeoPara->cosX;
	  planeStartX += GeoPara->stepDirection[0] * GeoPara->pSize[iSecX][0];
	  planeStopX  += GeoPara->stepDirection[0] * GeoPara->pSize[iSecX][0];
	  //	  StartX      += GeoPara->pSize[iSecX][0] * GeoPara->cosX;
	  //	  StopX       += GeoPara->pSize[iSecX][0] * GeoPara->cosX;
	  //	  planeStartX += GeoPara->pSize[iSecX][0];
	  //	  planeStopX  += GeoPara->pSize[iSecX][0];
	}

      iSecX = 0;
   
      //      if (iR == GeoPara->sRow[iSecY]-1)  // why -1 ??
      if (iR == GeoPara->sRow[iSecY])
	{
	  //        cout << "iR " << iR << " " << GeoPara->nRow << " " << iSecY << endl;
	  iSecY++;
	}

      StartY      += GeoPara->stepDirection[1] * GeoPara->pSize[iSecY][1] * GeoPara->cosY;
      StopY       += GeoPara->stepDirection[1] * GeoPara->pSize[iSecY][1] * GeoPara->cosY;
      planeStartY += GeoPara->stepDirection[1] * GeoPara->pSize[iSecY][1];
      planeStopY  += GeoPara->stepDirection[1] * GeoPara->pSize[iSecY][1];
      //    StartY      += GeoPara->pSize[iSecY][1] * GeoPara->cosY;
      //    StopY       += GeoPara->pSize[iSecY][1] * GeoPara->cosY;
      //    planeStartY += GeoPara->pSize[iSecY][1];
      //    planeStopY  += GeoPara->pSize[iSecY][1];
    }

  //----------------------------------------------------------------------------------------

  for (Int_t i = 0; i < 3; i++)
    Topview[i]->Write("", TObject::kOverwrite);

  h2Layer->Write("", TObject::kOverwrite);
  h1HitPad->Write("", TObject::kOverwrite);
  
  h2Module->Write("", TObject::kOverwrite);
  h1HitPadModule->Write("", TObject::kOverwrite);
  if (h2Module)
    delete h2Module;
  if (h1HitPadModule)
    delete h1HitPadModule;

}


  void CbmTrdHitRateFastQa::DrawBorders(HitRateGeoPara2 *GeoPara, TH2F* Layer, TCanvas* c1)
  {
    //----------------------Sector--------------------------------------
    Double_t SecXStart = 0.0;
    Double_t SecYStart = 0.0;
    Double_t SecXStop  = 0.0;
    Double_t SecYStop  = 0.0;

    for (Int_t iSec = 0; iSec < GeoPara->nSec-1; iSec++)  // would be enough to iterate up to nSec-1
      {

	if (GeoPara->sSize[iSec][0] < 2 * GeoPara->mSize[0] && GeoPara->sSize[iSec][0] > 0)  // in x direction
	  {
	    SecXStart += GeoPara->sSize[iSec][0];
	    SecXStop   = SecXStart;
	  }
	else
	  {
	    SecXStart = 0.0;
	    SecXStop  = GeoPara->sSize[iSec][0];
	  }

	if (GeoPara->sSize[iSec][1] < 2 * GeoPara->mSize[1] && GeoPara->sSize[iSec][1] > 0)  // in x direction
	  {
	    SecYStart += GeoPara->sSize[iSec][1];
	    SecYStop = SecYStart;
	  }
	else
	  {
	    SecYStart = 0.0;
	    SecYStop  = GeoPara->sSize[iSec][1];
	  }


	TLine* sector = new TLine(GeoPara->mPos[0]-GeoPara->mSize[0]+SecXStart,
				  GeoPara->mPos[1]-GeoPara->mSize[1]+SecYStart,
				  GeoPara->mPos[0]-GeoPara->mSize[0]+SecXStop,
				  GeoPara->mPos[1]-GeoPara->mSize[1]+SecYStop);
	sector->SetLineColor(15);
	sector->SetLineStyle(2);

	TPolyMarker* corner = new TPolyMarker(1);
	corner->SetPoint(0, GeoPara->mPos[0] - GeoPara->stepDirection[0] * GeoPara->mSize[0],
			 GeoPara->mPos[1] - GeoPara->stepDirection[1] * GeoPara->mSize[1]);
	corner->SetMarkerStyle(21);
	corner->SetMarkerSize(.8);

	if(fDraw)
	  {
	    c1->cd(1);
	    sector->Draw("same");
	    corner->Draw("same");
	  }

      }
  
  
    //----------------------Module--------------------------------------
    TBox *module  = new TBox(GeoPara->mPos[0]-GeoPara->mSize[0],
			     GeoPara->mPos[1]-GeoPara->mSize[1],
			     GeoPara->mPos[0]+GeoPara->mSize[0],
			     GeoPara->mPos[1]+GeoPara->mSize[1]);
    module->SetFillStyle(0);
    module->SetLineColor(1);

    TBox *M_inner = new TBox(GeoPara->mPos[0]-100,     GeoPara->mPos[1]-100,     GeoPara->mPos[0]+100,     GeoPara->mPos[1]+100);
    //  M_inner->SetUniqueID(ModuleId);
    M_inner->SetFillColor(kWhite);

    if(fDraw)
      {
	c1->cd(1);
	module->Draw("same");         // black module frame
	//    M_inner->Draw("same");   // white box in module center // do not draw fot the time being
	c1->Write("", TObject::kOverwrite);
      }

  }
  // --------------------------------------------------------------------


  void CbmTrdHitRateFastQa::DrawPads(HitRateGeoPara2 *GeoPara, TH2F* Layer, TCanvas* c1)
  {
    //----------------------Pad--------------------------------------
    //  const Int_t nR = GeoPara->nRow;
    //  const Int_t nC = GeoPara->nCol;

    Int_t iSecX = 0;
    Int_t iSecY = 0;

    Double_t StartX = GeoPara->mPos[0]-GeoPara->mSize[0];
    Double_t StartY = GeoPara->mPos[1]-GeoPara->mSize[1];

    Int_t SecCol = GeoPara->sCol[iSecX];
    Int_t SecRow = GeoPara->sRow[iSecY];

    for (Int_t iR = 0; iR < GeoPara->nRow; iR++)  // rows
      {
	StartX = GeoPara->mPos[0]-GeoPara->mSize[0];
	for (Int_t iC = 0; iC < GeoPara->nCol; iC++)  // cols
	  {
	    TLine* Pa = new TLine(StartX,                           // - low
				  StartY,
				  StartX+GeoPara->pSize[iSecX][0],
				  StartY);
	    TLine* Pb = new TLine(StartX,                           // - high
				  StartY+GeoPara->pSize[iSecY][1],
				  StartX+GeoPara->pSize[iSecX][0],
				  StartY+GeoPara->pSize[iSecY][1]);
	    TLine* Pc = new TLine(StartX,                           // | left
				  StartY,
				  StartX,
				  StartY+GeoPara->pSize[iSecY][1]);
	    TLine* Pd = new TLine(StartX+GeoPara->pSize[iSecX][0],  // | right
				  StartY,
				  StartX+GeoPara->pSize[iSecX][0],
				  StartY+GeoPara->pSize[iSecY][1]);
	    c1->cd(1);
	    Pa->SetLineColor(17);
	    //Pa->SetLineStyle(3);
	    Pa->Draw("same");

	    Pb->SetLineColor(17);
	    //Pb->SetLineStyle(3);
	    Pb->Draw("same");

	    Pc->SetLineColor(17);
	    //Pc->SetLineStyle(3);
	    Pc->Draw("same");

	    Pd->SetLineColor(17);
	    //Pd->SetLineStyle(3);
	    Pd->Draw("same");

	    //printf("Sx Sy (%d,%d)     nC nR (%d,%d) iC iR (%d,%d) SC SR (%d,%d)              Px Py (%.1f,%.1f) StartX StartY (%.1f,%.1f)\n",iSecX,iSecY,GeoPara->nCol,GeoPara->nRow,iC,iR,SecCol,SecRow,GeoPara->pSize[iSecX][0],GeoPara->pSize[iSecY][1],StartX,StartY);
      
	    if (iC == SecCol-1)
	      {
		iSecX++;
		SecCol += GeoPara->sCol[iSecX];
	      }
	    StartX += GeoPara->pSize[iSecX][0];
	  }
	iSecX = 0;
	if (iR == SecRow-1)
	  {          
	    iSecY++;
	    SecRow += GeoPara->sRow[iSecY];
	  }
	StartY += GeoPara->pSize[iSecY][1];
      }

  }
  // --------------------------------------------------------------------


  // ------DrawDigi------------------------------------------------------
  void CbmTrdHitRateFastQa::DrawDigi()
  {
  }
  // --------------------------------------------------------------------


  // ---- Register ------------------------------------------------------
  void CbmTrdHitRateFastQa::Register()
  {
    //FairRootManager::Instance()->Register("TrdDigi","Trd Digi", fDigiCollection, kTRUE);
    //FairRootManager::Instance()->Register("TrdDigiMatch","Trd Digi Match", fDigiMatchCollection, kTRUE);
  }
  // --------------------------------------------------------------------

  ClassImp(CbmTrdHitRateFastQa)
