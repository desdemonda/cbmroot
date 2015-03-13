// -------------------------------------------------------------------------
// -----                  CbmMuchHitProducerQa source file             -----
// -----                   Created 16/11/07 by E. Kryshen              -----
// -------------------------------------------------------------------------
#include "CbmMuchHitFinderQa.h"
#include "FairRootManager.h"
#include "CbmMuchPoint.h"
#include "CbmMuchDigi.h"
#include "CbmMuchDigiMatch.h"
#include "CbmMuchCluster.h"
#include "CbmMuchPixelHit.h"

#include "CbmMuchAddress.h"
#include "CbmMuchStation.h"
#include "CbmMuchModuleGem.h"
#include "CbmMuchSector.h"
#include "CbmMuchPad.h"

#include "CbmMCTrack.h"
#include "TParticlePDG.h"
#include "TDatabasePDG.h"
#include "FairLogger.h"

#include "TCanvas.h"
#include "TF1.h"
#include "TFile.h"
#include "CbmMuchPointInfo.h"
#include "CbmMuchDigitizeGem.h"
#include "TStyle.h"
#include "TArrayI.h"
#include "CbmMuchGeoScheme.h"
#include "TObjArray.h"


#include "FairRuntimeDb.h"
#include "CbmGeoMuchPar.h"
#include "TGraph.h"

#include <map>
#include <vector>
#include <algorithm>

using std::map;
using std::vector;

// -------------------------------------------------------------------------
CbmMuchHitFinderQa::CbmMuchHitFinderQa(const char* name, Int_t verbose)
  :FairTask(name,verbose),
    fGeoScheme( CbmMuchGeoScheme::Instance()),
    fGeoFileName("much.digi.root"),
    fFileName("performance.root"),
    fSignalPoints(0),
    fSignalHits(0),
    fVerbose(verbose),
    fEvent(0),
    fPoints(NULL),
    fDigis(NULL),
    fDigiMatches(NULL),
    fClusters(NULL),
    fHits(NULL),
    fMCTracks(NULL),
    fPointInfos(new TClonesArray("CbmMuchPointInfo",10)),
    fNstations(0),
    fChargeHistos(NULL),
    fhChargeEnergyLog(NULL),
    fhChargeEnergyLogPi(NULL),
    fhChargeEnergyLogPr(NULL),
    fhChargeEnergyLogEl(NULL),
    fhChargeTrackLength(NULL),
    fhChargeTrackLengthPi(NULL),
    fhChargeTrackLengthPr(NULL),
    fhChargeTrackLengthEl(NULL),
    fhCharge(NULL),
    fhChargeLog(NULL),
    fhChargePr_1GeV_3mm(NULL),
    fhNpadsVsS(NULL),
    fhOccupancyR(NULL),
    fhPadsTotalR(NULL),
    fhPadsFiredR(NULL),
    fhPullXpads1(NULL),
    fhPullYpads1(NULL),
    fhPullXpads2(NULL),
    fhPullYpads2(NULL),
    fhPullXpads3(NULL),
    fhPullYpads3(NULL),
    fnPadSizesX(0),
    fnPadSizesY(0),
    fhPullT(NULL),
    fNTimingPulls(8),
    fhPullX(NULL),
    fhPullY(NULL),
    fhPointsInCluster(NULL),
    fhDigisInCluster(NULL),
    fhHitsInCluster(NULL),
    fNall(NULL),
    fNpr(NULL),
    fNpi(NULL),
    fNel(NULL),
    fNmu(NULL),
    fNka(NULL),
    fNprimary(NULL),
    fNsecondary(NULL),
    fPointsTotal(0),
    fPointsUnderCounted(0),
    fPointsOverCounted(0),
    fOccupancyQaOn(kTRUE),
    fPullsQaOn(kTRUE),
    fDigitizerQaOn(kTRUE),
    fStatisticsQaOn(kTRUE),
    fClusterDeconvQaOn(kTRUE),
    fPrintToFileOn(kTRUE),
    fPadMinLx(0.),
    fPadMinLy(0.),
    fPadMaxLx(0.),
    fPadMaxLy(0.),
    pointsFile(NULL),
    padsFile(NULL)
{
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
CbmMuchHitFinderQa::~CbmMuchHitFinderQa()
{
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
InitStatus CbmMuchHitFinderQa::Init()
{
  FairRootManager* fManager = FairRootManager::Instance();
  fMCTracks    = (TClonesArray*) fManager->GetObject("MCTrack");
  fPoints      = (TClonesArray*) fManager->GetObject("MuchPoint");
  fHits        = (TClonesArray*) fManager->GetObject("MuchPixelHit");
  fDigis       = (TClonesArray*) fManager->GetObject("MuchDigi");
  fDigiMatches = (TClonesArray*) fManager->GetObject("MuchDigiMatch");
  fClusters    = (TClonesArray*) fManager->GetObject("MuchCluster");

//  printf(" %i",fMCTracks);
//  printf(" %i",fPoints);
//  printf(" %i",fHits);
//  printf(" %i",fDigis);
//  printf(" %i",fDigiMatches);
//  printf(" %i",fClusters);
//  printf("\n");
  
  TFile* f = new TFile(fGeoFileName,"R");
  TObjArray* stations = (TObjArray*) f->Get("stations");
  fGeoScheme->Init(stations);
  fNstations = fGeoScheme->GetNStations();
  printf("Init: fNstations = %i\n", fNstations);

  fNall       = new Int_t[fNstations];
  fNpr        = new Int_t[fNstations];
  fNpi        = new Int_t[fNstations];
  fNel        = new Int_t[fNstations];
  fNmu        = new Int_t[fNstations];
  fNka        = new Int_t[fNstations];
  fNprimary   = new Int_t[fNstations];
  fNsecondary = new Int_t[fNstations];

  // Reset counters
  for (Int_t i=0;i<fNstations;i++){
    fNall[i] = 0;
    fNpr[i] = 0;
    fNpi[i] = 0;
    fNel[i] = 0;
    fNmu[i] = 0;
    fNka[i] = 0;
    fNprimary[i] = 0;
    fNsecondary[i] = 0;
  }

  fPointsTotal=0;
  fPointsUnderCounted=0;
  fPointsOverCounted=0;

  #define BINNING_CHARGE        100,    0,   3.0
  #define BINNING_LENGTH        100,    0,   2.5
  #define BINNING_CHARGE_LOG    100,    4,     8
  #define BINNING_ENERGY_LOG    100, -0.5,   4.5
  #define BINNING_ENERGY_LOG_EL 100, -0.5,   4.5

  fhCharge              = new TH1D("hCharge",
                                   "Charge distribution from tracks",
                                   BINNING_CHARGE);

  fhChargeLog           = new TH1D("hChargeLog",
                                   "Charge (log.) distribution from tracks",
                                   BINNING_CHARGE_LOG);

  fhChargeEnergyLog     = new TH2D("hChargeEnergyLog"    ,
                                   "Charge vs energy (log.) for all tracks",
                                   BINNING_ENERGY_LOG,BINNING_CHARGE);

  fhChargeEnergyLogPi   = new TH2D("hChargeEnergyLogPi",
                                   "Charge vs energy (log.) for pions",
                                   BINNING_ENERGY_LOG,BINNING_CHARGE);

  fhChargeEnergyLogPr   = new TH2D("hChargeEnergyLogPr",
                                   "Charge vs energy (log.) for protons",
                                   BINNING_ENERGY_LOG,BINNING_CHARGE);

  fhChargeEnergyLogEl   = new TH2D("hChargeEnergyLogEl",
                                   "Charge vs energy (log.) for electrons",
                                   BINNING_ENERGY_LOG_EL,BINNING_CHARGE);

  fhChargeTrackLength   = new TH2D("hChargeTrackLength",
                                   "Charge vs length for all tracks",
                                   BINNING_LENGTH,BINNING_CHARGE);

  fhChargeTrackLengthPi = new TH2D("hChargeTrackLengthPi",
                                   "Charge vs length for pions",
                                   BINNING_LENGTH,BINNING_CHARGE);
  fhChargeTrackLengthPr = new TH2D("hChargeTrackLengthPr",
                                   "Charge vs length for proton",
                                   BINNING_LENGTH,BINNING_CHARGE);

  fhChargeTrackLengthEl = new TH2D("hChargeTrackLengthEl",
                                   "Charge vs length for electrons",
                                   BINNING_LENGTH,BINNING_CHARGE);

  fhChargePr_1GeV_3mm = new TH1D("hChargePr_1GeV_3mm",
                                 "Charge for 1 GeV protons",
                                 BINNING_CHARGE);

  fhPointsInCluster = new TH1I("hPointsInCluster",
                               "Number of tracks in cluster",
                                10,0,10);

  fhDigisInCluster  = new TH1I("hDigisInCluster",
                               "Number of digis in cluster",
                               10,0,10);

  fhHitsInCluster   = new TH1I("hHitsInCluster",
                               "Number of hits in cluster",
                               10,0,10);

  
  fhCharge   ->GetXaxis()->SetTitle("Charge [10^{6} electrons]");
  fhChargeLog->GetXaxis()->SetTitle("Lg (Charge) [Number of electrons]");
  fhChargePr_1GeV_3mm->GetXaxis()->SetTitle("Charge [10^{6} electrons]");


  fChargeHistos = new TObjArray();
  fChargeHistos->Add(fhChargeEnergyLog    );
  fChargeHistos->Add(fhChargeEnergyLogEl  );
  fChargeHistos->Add(fhChargeEnergyLogPi  );
  fChargeHistos->Add(fhChargeEnergyLogPr  );
  fChargeHistos->Add(fhChargeTrackLength  );
  fChargeHistos->Add(fhChargeTrackLengthEl);
  fChargeHistos->Add(fhChargeTrackLengthPi);
  fChargeHistos->Add(fhChargeTrackLengthPr);
  fChargeHistos->Add(fhCharge);
  fChargeHistos->Add(fhChargeLog);

  for (Int_t i=0;i<8;i++){
    TH2D* histo = (TH2D*) fChargeHistos->At(i);
    histo->SetStats(0);
    histo->GetYaxis()->SetDecimals(kTRUE);
    histo->GetYaxis()->SetTitleOffset(1.4);
    histo->GetYaxis()->CenterTitle();
    histo->GetYaxis()->SetTitle("Charge [10^{6} electrons]");
    if (i<4) histo->GetXaxis()->SetTitle("Lg E_{kin} [MeV]");
    else     histo->GetXaxis()->SetTitle("Track length [cm]");
  }

  fhOccupancyR = new TH1D*[fNstations];
  fhPadsTotalR = new TH1D*[fNstations];
  fhPadsFiredR = new TH1D*[fNstations];

  for (Int_t i=0; i<fNstations; i++){
    CbmMuchStation* station = fGeoScheme->GetStation(i);
    Double_t rMax = station->GetRmax();
    fhPadsTotalR[i] = new TH1D(Form("hPadsTotal%i",i+1),Form("Number of  pads vs radius: station %i;Radius [cm]",i+1),100,0,1.2*rMax);
    fhPadsFiredR[i] = new TH1D(Form("hPadsFired%i",i+1),Form("Number of fired pads vs radius: station %i;Radius [cm]",i+1),100,0,1.2*rMax);
    fhOccupancyR[i] = new TH1D(Form("hOccupancy%i",i+1),Form("Occupancy vs radius: station %i;Radius [cm];Occupancy, %%",i+1),100,0,1.2*rMax);
  }

  vector<CbmMuchModule*> modules = fGeoScheme->GetModules();
  for(vector<CbmMuchModule*>::iterator im = modules.begin(); im!=modules.end(); im++){
    CbmMuchModule* mod = (*im);
    if(mod->GetDetectorType() == 1 || mod->GetDetectorType()==3) {
      CbmMuchModuleGem* module = (CbmMuchModuleGem*) mod;
      vector<CbmMuchPad*> pads = module->GetPads();
      for (Int_t ip=0; ip<pads.size(); ip++) {
        CbmMuchPad* pad = pads[ip];
        Int_t stationId = CbmMuchAddress::GetStationIndex(pad->GetAddress());
        Double_t x0 = pad->GetX();
        Double_t y0 = pad->GetY();
        Double_t r0 = TMath::Sqrt(x0*x0+y0*y0);
        fhPadsTotalR[stationId]->Fill(r0);
      }
    }
  }
  /*
  fPadMinLx = std::numeric_limits<Double_t>::max();
  fPadMinLy = std::numeric_limits<Double_t>::max();
  fPadMaxLx = std::numeric_limits<Double_t>::min();
  fPadMaxLy = std::numeric_limits<Double_t>::min();
  
  Int_t nTotSectors = 0;
  Int_t nTotChannels = 0;
  printf("=========================================================================================\n");
  printf(" Station Nr.\t| Sectors\t| Channels\t| Pad min size\t\t| Pad max length\t \n");
  printf("-----------------------------------------------------------------------------------------\n");
  */
  Int_t nTotChannels = 0;
  for (Int_t iStation=0;iStation<fNstations;iStation++){
/*    
    Double_t padMinLx = GetMinPadSize(iStation).X();
    Double_t padMinLy = GetMinPadSize(iStation).Y();
    Double_t padMaxLx = GetMaxPadSize(iStation).X();
    Double_t padMaxLy = GetMaxPadSize(iStation).Y();
*/
    Int_t nChannels  = GetNChannels(iStation);
/*
    Int_t nSectors   = GetNSectors(iStation);
    if (fPadMinLx>padMinLx) fPadMinLx = padMinLx;
    if (fPadMinLy>padMinLy) fPadMinLy = padMinLy;
    if (fPadMaxLx<padMaxLx) fPadMaxLx = padMaxLx;
    if (fPadMaxLy<padMaxLy) fPadMaxLy = padMaxLy;
    printf("%i\t\t| %i\t\t| %i\t| %5.4fx%5.4f\t\t| %5.4fx%5.4f\n",iStation+1, nSectors, nChannels, padMinLx, padMinLy, padMaxLx, padMaxLy);
*/
//    nTotSectors += nSectors;
    printf("%i\t\t| %i\t\t\n",iStation+1, nChannels);
    nTotChannels += nChannels;
  }
  printf("-----------------------------------------------------------------------------------------\n");
  printf(" Total:\t\t| %i\t\t\n", nTotChannels);
  printf("=========================================================================================\n");
/*
  fnPadSizesX = TMath::CeilNint(TMath::Log2(fPadMaxLx/fPadMinLx)+1);
  fnPadSizesY = TMath::CeilNint(TMath::Log2(fPadMaxLy/fPadMinLy)+1);
  Info("Init","nPadSizesX=%i",fnPadSizesX);
  Info("Init","nPadSizesY=%i",fnPadSizesY);
  fhPullXpads1 = new TH1D*[fnPadSizesX];
  fhPullYpads1 = new TH1D*[fnPadSizesY];
  fhPullXpads2 = new TH1D*[fnPadSizesX];
  fhPullYpads2 = new TH1D*[fnPadSizesY];
  fhPullXpads3 = new TH1D*[fnPadSizesX];
  fhPullYpads3 = new TH1D*[fnPadSizesY];
  fhPullT      = new TH1D*[fNTimingPulls];
  
  for (Int_t i=0;i<fnPadSizesX;i++){
    fhPullXpads1[i] = new TH1D(Form("hPullXpads1%i",i),Form("Pull distribution X. Npads = 1 Size =%i; (x_{RC} - x_{MC}) / dx_{RC}",i),100,-5,5);
    fhPullXpads2[i] = new TH1D(Form("hPullXpads2%i",i),Form("Pull distribution X. Npads = 2 Size =%i; (x_{RC} - x_{MC}) / dx_{RC}",i),100,-5,5);
    fhPullXpads3[i] = new TH1D(Form("hPullXpads3%i",i),Form("Pull distribution X. Npads = 3 Size =%i; (x_{RC} - x_{MC}) / dx_{RC}",i),100,-5,5);
  }

  for (Int_t i=0;i<fnPadSizesY;i++){
    fhPullYpads1[i] = new TH1D(Form("hPullYpads1%i",i),Form("Pull distribution Y. Npads = 1 Size =%i; (y_{RC} - y_{MC}) / dy_{RC}",i),100,-5,5);
    fhPullYpads2[i] = new TH1D(Form("hPullYpads2%i",i),Form("Pull distribution Y. Npads = 2 Size =%i; (y_{RC} - y_{MC}) / dy_{RC}",i),100,-5,5);
    fhPullYpads3[i] = new TH1D(Form("hPullYpads3%i",i),Form("Pull distribution Y. Npads = 3 Size =%i; (y_{RC} - y_{MC}) / dy_{RC}",i),100,-5,5);
  }

  fhPullT[0] = new TH1D("hPullT","Pull distribution T, all pads; (t_{RC} - t_{MC}) / dt_{RC}",100,-5,5);
  for (Int_t i=1;i<fNTimingPulls;i++){
    fhPullT[i] = new TH1D(Form("hPullT%i",i),Form("Pull distribution T. Npads = %i; (t_{RC} - t_{MC}) / dt_{RC}",i),100,-5,5);
  }
  */

  fhPullX = new TH1D("hPullX","Pull distribution X;(x_{RC} - x_{MC}) / dx_{RC}",100,-5,5);
  fhPullY = new TH1D("hPullY","Pull distribution Y;(y_{RC} - y_{MC}) / dy_{RC}",100,-5,5);

  fhNpadsVsS = new TH2D("hNpadsVsS","Number of fired pads vs pad area:area:n pads",10,-5,0,10,0.5,10.5);
  pointsFile = fopen ("points.txt","w");
  padsFile = fopen ("pads.txt","w");
  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchHitFinderQa::SetParContainers() {
  // Get run and runtime database
  // FairRuntimeDb* db = FairRuntimeDb::instance();
  // if ( ! db ) Fatal("SetParContainers", "No runtime database");
  // Get MUCH geometry parameter container
  // fGeoPar = (CbmGeoMuchPar*) db->getContainer("CbmGeoMuchPar");
}
// -------------------------------------------------------------------------




// -------------------------------------------------------------------------x
void CbmMuchHitFinderQa::Exec(Option_t * option){
  fEvent++;
  gLogger->Info(MESSAGE_ORIGIN,"Event:%i",fEvent);
  fprintf(pointsFile,"Event %i\n",fEvent);
  fprintf(padsFile,"Event %i\n",fEvent);
  
  if (fPullsQaOn) PullsQa();
  if (fOccupancyQaOn) OccupancyQa();
  if (fDigitizerQaOn) DigitizerQa();
  if (fStatisticsQaOn) StatisticsQa();
  if (fClusterDeconvQaOn) ClusterDeconvQa();
  
  return;
  for (int i=0;i<fPoints->GetEntriesFast();i++){
    CbmMuchPoint* point = (CbmMuchPoint*) fPoints->At(i);
    Int_t stId = CbmMuchAddress::GetStationIndex(point->GetDetectorID());
    if (stId!=0) continue;
    Int_t layerId = CbmMuchAddress::GetLayerIndex(point->GetDetectorID());
    if (layerId!=0) continue;
    printf("point %4i xin=%6.2f yin=%6.2f xout=%6.2f yout=%6.2f zin=%6.2f\n",i,point->GetXIn(),point->GetYIn(),point->GetXOut(),point->GetYOut(),point->GetZIn());
    fprintf (pointsFile, "%7.3f %7.3f %7.3f %7.3f\n",point->GetXIn(),point->GetYIn(),point->GetXOut(),point->GetYOut());
  }

  for (Int_t i=0;i<fDigis->GetEntriesFast();i++){
    CbmMuchDigi* digi = (CbmMuchDigi*) fDigis->At(i);
    UInt_t address = digi->GetAddress();
    Int_t stId = CbmMuchAddress::GetStationIndex(address);
    if (stId!=0) continue;
    Int_t layerId = CbmMuchAddress::GetLayerIndex(address);
    if (layerId!=0) continue;
    CbmMuchModuleGem* module = (CbmMuchModuleGem*)fGeoScheme->GetModuleByDetId(address);
    if(!module) continue;
    CbmMuchPad* pad = module->GetPad(address);
    Double_t x0 = pad->GetX();
    Double_t y0 = pad->GetY();
    UInt_t charge = digi->GetAdc();
    printf("digi %4i x0=%5.1f y0=%5.1f charge=%3i\n",i,x0,y0,charge);
    fprintf(padsFile,"%5.1f %5.1f %3i\n",x0,y0,charge);
  }
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchHitFinderQa::FinishTask(){
  printf("FinishTask\n");
  fclose (pointsFile);
  fclose (padsFile);

  gStyle->SetPaperSize(20,20);

  Double_t errors[100];
  for (Int_t i=0;i<100;i++) errors[i]=0;

  for (Int_t i=0;i<fNstations;i++) {
    fhPadsTotalR[i]->Sumw2();
    fhPadsTotalR[i]->SetError(errors);
    fhPadsFiredR[i]->Sumw2();
    fhPadsFiredR[i]->Scale(1./fEvent);
    fhOccupancyR[i]->Divide(fhPadsFiredR[i],fhPadsTotalR[i]);
    fhOccupancyR[i]->Scale(100);
  }

  if (fPullsQaOn && fVerbose>1){
    printf("===================================\n");
    printf("PullsQa:\n");

//    TCanvas* cTiming = new TCanvas("cTiming","Timing pulls",250*4,250*2);
//    cTiming->Divide(4,2);
//    for (Int_t i=0;i<fNTimingPulls;i++){
//      cTiming->cd(i+1);
//      fhPullT[i]->Fit("gaus");
//      fhPullT[i]->Draw("e");
//    }
//    
    TCanvas* c4 = new TCanvas("c4","Pulls",800,400);
    c4->Divide(2,1);
    c4->cd(1);
    fhPullX->Sumw2();
    fhPullX->Fit("gaus","Q");
    fhPullX->GetFunction("gaus")->SetLineWidth(2);
    fhPullX->GetFunction("gaus")->SetLineColor(kBlue);
    fhPullX->Draw();
    if (fPrintToFileOn) gPad->Print(".gif");
    if (fPrintToFileOn) gPad->Print(".eps");
    c4->cd(2);
    fhPullY->Sumw2();
    fhPullY->Fit("gaus","Q");
    fhPullY->GetFunction("gaus")->SetLineWidth(2);
    fhPullY->GetFunction("gaus")->SetLineColor(kBlue);
    fhPullY->Draw();
    if (fPrintToFileOn) gPad->Print(".gif");
    if (fPrintToFileOn) gPad->Print(".eps");
    c4->cd();

    TCanvas* c_alone = new TCanvas("c_alone","Pulls",400,400);
    fhPullX->Draw();
    gPad->Print(".gif");
//
//    TCanvas* c4x = new TCanvas("c4x","X-pulls vs pad size and cluster size",fnPadSizesX*300,3*300);
//    c4x->Divide(fnPadSizesX,3);
//    for (Int_t i=0;i<fnPadSizesX;i++){
//      c4x->cd(i+1);
//      fhPullXpads1[i]->Sumw2();
//      fhPullXpads1[i]->Fit("gaus","Q");
//      fhPullXpads1[i]->GetFunction("gaus")->SetLineWidth(2);
//      fhPullXpads1[i]->GetFunction("gaus")->SetLineColor(kBlue);
//      fhPullXpads1[i]->Draw();
//      if (fPrintToFileOn) gPad->Print(".gif");
//      if (fPrintToFileOn) gPad->Print(".eps");
//
//      c4x->cd(fnPadSizesX+i+1);
//      fhPullXpads2[i]->Sumw2();
//      fhPullXpads2[i]->Fit("gaus","Q");
//      fhPullXpads2[i]->GetFunction("gaus")->SetLineWidth(2);
//      fhPullXpads2[i]->GetFunction("gaus")->SetLineColor(kBlue);
//      fhPullXpads2[i]->Draw();
//      if (fPrintToFileOn) gPad->Print(".gif");
//      if (fPrintToFileOn) gPad->Print(".eps");
//
//      c4x->cd(2*fnPadSizesX+i+1);
//      fhPullXpads3[i]->Sumw2();
//      fhPullXpads3[i]->Fit("gaus","Q");
//      fhPullXpads3[i]->GetFunction("gaus")->SetLineWidth(2);
//      fhPullXpads3[i]->GetFunction("gaus")->SetLineColor(kBlue);
//      fhPullXpads3[i]->Draw();
//      if (fPrintToFileOn) gPad->Print(".gif");
//      if (fPrintToFileOn) gPad->Print(".eps");
//    }

//    TCanvas* c4y = new TCanvas("c4y","Y-pulls vs pad size and cluster size",fnPadSizesY*300,3*300);
//    c4y->Divide(fnPadSizesY,3);
//    for (Int_t i=0;i<fnPadSizesY;i++){
//      c4y->cd(i+1);
//      fhPullYpads1[i]->Sumw2();
//      fhPullYpads1[i]->Fit("gaus","Q");
//      fhPullYpads1[i]->GetFunction("gaus")->SetLineWidth(2);
//      fhPullYpads1[i]->GetFunction("gaus")->SetLineColor(kBlue);
//      fhPullYpads1[i]->Draw();
//      if (fPrintToFileOn) gPad->Print(".gif");
//      if (fPrintToFileOn) gPad->Print(".eps");
//
//      c4y->cd(fnPadSizesY+i+1);
//      fhPullYpads2[i]->Sumw2();
//      fhPullYpads2[i]->Fit("gaus","Q");
//      fhPullYpads2[i]->GetFunction("gaus")->SetLineWidth(2);
//      fhPullYpads2[i]->GetFunction("gaus")->SetLineColor(kBlue);
//      fhPullYpads2[i]->Draw();
//      if (fPrintToFileOn) gPad->Print(".gif");
//      if (fPrintToFileOn) gPad->Print(".eps");
//
//      c4y->cd(2*fnPadSizesY+i+1);
//      fhPullYpads3[i]->Sumw2();
//      fhPullYpads3[i]->Fit("gaus","Q");
//      fhPullYpads3[i]->GetFunction("gaus")->SetLineWidth(2);
//      fhPullYpads3[i]->GetFunction("gaus")->SetLineColor(kBlue);
//      fhPullYpads3[i]->Draw();
//      if (fPrintToFileOn) gPad->Print(".gif");
//      if (fPrintToFileOn) gPad->Print(".eps");
//    }
  }


  if (fOccupancyQaOn && fVerbose>1) {
    printf("===================================\n");
    printf("OccupancyQa:\n");
    TCanvas* c1 = new TCanvas("c1","All pad distributions",1200,800);
    c1->Divide(3,2);
    for (Int_t i=0;i<fNstations;i++) {
      c1->cd(i+1);
      fhPadsTotalR[i]->SetStats(0);
      fhPadsTotalR[i]->Draw("hist");
      if (fPrintToFileOn) gPad->Print(".gif");
      if (fPrintToFileOn) gPad->Print(".eps");
    }
    c1->cd();

    TCanvas* c2 = new TCanvas("c2","Fired pad distributions",1200,800);
    c2->Divide(3,2);
    for (Int_t i=0;i<fNstations;i++) {
      c2->cd(i+1);
      fhPadsFiredR[i]->SetStats(0);
      fhPadsFiredR[i]->Draw();
      if (fPrintToFileOn) gPad->Print(".gif");
      if (fPrintToFileOn) gPad->Print(".eps");
    }
    c2->cd();

    TCanvas* c3 = new TCanvas("c3","Occupancy plots",2400,1600);
    c3->Divide(3,2);
    for (Int_t i=0;i<fNstations;i++) {
      c3->cd(i+1);
      fhOccupancyR[i]->SetStats(0);
      fhOccupancyR[i]->Draw();
      if (fPrintToFileOn) gPad->Print(".gif");
      if (fPrintToFileOn) gPad->Print(".eps");
    }
    c3->cd();
    
    TCanvas* oc_1 = new TCanvas("oc_1","",1200,1000);
    fhOccupancyR[0]->GetYaxis()->SetTitleOffset(1.6);
    fhOccupancyR[0]->Draw();
    if (fPrintToFileOn) gPad->Print(".gif");
    TCanvas* oc_2 = new TCanvas("oc_2","",1200,1000);
    fhOccupancyR[1]->GetYaxis()->SetTitleOffset(1.6);
    fhOccupancyR[1]->Draw();
    if (fPrintToFileOn) gPad->Print(".gif");
    TCanvas* oc_3 = new TCanvas("oc_3","",1200,1000);
    fhOccupancyR[2]->GetYaxis()->SetTitleOffset(1.6);
    fhOccupancyR[2]->Draw();
    if (fPrintToFileOn) gPad->Print(".gif");
  }

  if (fDigitizerQaOn && fVerbose>1) {
    printf("===================================\n");
    printf("DigitizerQa:\n");

    TF1* fit_el = new TF1("fit_el",LandauMPV,-0.5,4.5,1);
    fit_el->SetParameter(0,0.511);
    fit_el->SetLineWidth(2);
    fit_el->SetLineColor(kBlack);

    TF1* fit_pi = new TF1("fit_pi",LandauMPV,-0.5,4.5,1);
    fit_pi->SetParameter(0,139.57);
    fit_pi->SetLineWidth(2);
    fit_pi->SetLineColor(kBlack);
    
    TF1* fit_pr = new TF1("fit_pr",LandauMPV,-0.5,4.5,1);
    fit_pr->SetParameter(0,938.27);
    fit_pr->SetLineWidth(2);
    fit_pr->SetLineColor(kBlack);

    TH1D* hLength   = fhChargeTrackLength  ->ProjectionX();
    TH1D* hLengthEl = fhChargeTrackLengthEl->ProjectionX();;
    TH1D* hLengthPi = fhChargeTrackLengthPi->ProjectionX();;
    TH1D* hLengthPr = fhChargeTrackLengthPr->ProjectionX();;
    hLength  ->SetTitle("Track length for all tracks");
    hLengthEl->SetTitle("Track length for electrons");
    hLengthPi->SetTitle("Track length for pions");
    hLengthPr->SetTitle("Track length for protons");

    fChargeHistos->Add(hLength);
    fChargeHistos->Add(hLengthEl);
    fChargeHistos->Add(hLengthPi);
    fChargeHistos->Add(hLengthPr);

    TCanvas* c5 = new TCanvas("c5","Charge vs energy and length",2100,1300);
    c5->Divide(4,3);
    for (Int_t i=0;i<8;i++){
      c5->cd(i+1);
      gPad->Range(0,0,200,200);
      gPad->SetBottomMargin(0.11);
      gPad->SetRightMargin(0.14);
      gPad->SetLeftMargin(0.12);
      gPad->SetLogz();
      TH2D* histo = (TH2D*) fChargeHistos->At(i);
      histo->Draw("colz");
      if (i==1) fit_el->Draw("same");
      if (i==2) fit_pi->Draw("same");
      if (i==3) fit_pr->Draw("same");
      if (fPrintToFileOn) gPad->Print(".gif");
      if (fPrintToFileOn) gPad->Print(".eps");
    }
    
    TCanvas* ch_1 = new TCanvas("ch_1","",1200,1000);
    gPad->SetBottomMargin(0.11);
    gPad->SetRightMargin(0.14);
    gPad->SetLeftMargin(0.12);
    gPad->SetLogz();
    TH2D* histo1 = (TH2D*) fChargeHistos->At(1);
    histo1->Draw("colz");
    fit_el->Draw("same");
    if (fPrintToFileOn) gPad->Print(".gif");
    
    TCanvas* ch_2 = new TCanvas("ch_2","",1200,1000);
    gPad->SetBottomMargin(0.11);
    gPad->SetRightMargin(0.14);
    gPad->SetLeftMargin(0.12);
    gPad->SetLogz();
    TH2D* histo2 = (TH2D*) fChargeHistos->At(2);
    histo2->Draw("colz");
    fit_pi->Draw("same");
    if (fPrintToFileOn) gPad->Print(".gif");

    TCanvas* ch_3 = new TCanvas("ch_3","",1200,1000);
    gPad->SetBottomMargin(0.11);
    gPad->SetRightMargin(0.14);
    gPad->SetLeftMargin(0.12);
    gPad->SetLogz();
    TH2D* histo3 = (TH2D*) fChargeHistos->At(3);
    histo3->Draw("colz");
    fit_pr->Draw("same");
    if (fPrintToFileOn) gPad->Print(".gif");

    
    for (Int_t i=10;i<14;i++){
      c5->cd(i-1);
      gPad->SetLogy();
      gStyle->SetOptStat(1110);
      TH1D* histo = (TH1D*) fChargeHistos->At(i);
      histo->Draw();
      if (fPrintToFileOn) gPad->Print(".gif");
      if (fPrintToFileOn) gPad->Print(".eps");
    }

    TCanvas* c6 = new TCanvas("c6","Charge distribution",1200,400);
    c6->Divide(3,1);
    c6->cd(1);
    fhCharge->Draw();
    if (fPrintToFileOn) gPad->Print(".gif");
    if (fPrintToFileOn) gPad->Print(".eps");

    c6->cd(2);
    fhChargeLog->Draw();
    if (fPrintToFileOn) gPad->Print(".gif");
    if (fPrintToFileOn) gPad->Print(".eps");

    c6->cd(3);
    fhChargePr_1GeV_3mm->Draw();
    if (fPrintToFileOn) gPad->Print(".gif");
    if (fPrintToFileOn) gPad->Print(".eps");

    TCanvas* c8 = new TCanvas("c8","Square vs nPads",800,400);
    c8->Divide(2,1);
    c8->cd(1);
    fhNpadsVsS->Draw("colz");
    Double_t nMean[11];
    Double_t s[11];
    for (Int_t iS=1;iS<=10;iS++){
      nMean[iS]=0;
      s[iS]=-5.25+0.5*iS;
      Double_t total=0;
      for (Int_t iN=1;iN<=10;iN++){
        nMean[iS]+=iN*fhNpadsVsS->GetBinContent(iS,iN);
        total+=fhNpadsVsS->GetBinContent(iS,iN);
      }
      if (total>0) nMean[iS]/=total;
      //printf("%f %f\n",s[iS],nMean[iS]);
    }
    c8->cd(2);
    TGraph* gNvsS = new TGraph(11,s,nMean);
    //gNvsS->DrawClone();


    printf("All tracks: ;");
    for (Int_t i=0;i<fNstations;i++) printf("%8i;",fNall[i]);
    printf("\n");
    printf("------------;");
    for (Int_t i=0;i<fNstations;i++) printf("---------");
    printf("\n");
    printf("Primary:    ;");
    for (Int_t i=0;i<fNstations;i++) printf("%8i;",fNprimary[i]);
    printf("\n");
    printf("Secondary:  ;");
    for (Int_t i=0;i<fNstations;i++) printf("%8i;",fNsecondary[i]);
    printf("\n");
    printf("-------------");
    for (Int_t i=0;i<fNstations;i++) printf("---------");
    printf("\n");
    printf("Protons:    ;");
    for (Int_t i=0;i<fNstations;i++) printf("%8i;",fNpr[i]);
    printf("\n");
    printf("Pions:      ;");
    for (Int_t i=0;i<fNstations;i++) printf("%8i;",fNpi[i]);
    printf("\n");
    printf("Electrons:  ;");
    for (Int_t i=0;i<fNstations;i++) printf("%8i;",fNel[i]);
    printf("\n");
    printf("Muons:      ;");
    for (Int_t i=0;i<fNstations;i++) printf("%8i;",fNmu[i]);
    printf("\n");
    printf("Kaons:      ;");
    for (Int_t i=0;i<fNstations;i++) printf("%8i;",fNka[i]);
    printf("\n");

  }


  if (fStatisticsQaOn){
    printf("===================================\n");
    printf("StatisticsQa:\n");
    TCanvas* c7 = new TCanvas("c7","Cluster statistics",1200,400);
    c7->Divide(3,1);
    c7->cd(1);
    gStyle->SetOptStat(1110);
    gPad->SetLogy();
    fhPointsInCluster->Draw();
    if (fPrintToFileOn) gPad->Print(".gif");
    if (fPrintToFileOn) gPad->Print(".eps");
    c7->cd(2);
    gStyle->SetOptStat(1110);
    gPad->SetLogy();
    fhDigisInCluster->Draw();
    if (fPrintToFileOn) gPad->Print(".gif");
    if (fPrintToFileOn) gPad->Print(".eps");
    c7->cd(3);
    gStyle->SetOptStat(1110);
    gPad->SetLogy();
    fhHitsInCluster->Draw();
    if (fPrintToFileOn) gPad->Print(".gif");
    if (fPrintToFileOn) gPad->Print(".eps");

    printf("Total number of points: %i\n",fPointsTotal);
    printf("Points overcounted: %i\n",fPointsOverCounted);
    printf("Points undercounted: %i\n",fPointsUnderCounted);
  }

  if (fClusterDeconvQaOn){
    printf("Signal points: %i\n", fSignalPoints);
    printf("Signal hits: %i\n", fSignalHits);
  }

//  TFile* performanceFile = new TFile(fFileName, "recreate");
//
//   for (Int_t i=0;i<fNstations;i++) {
//     fhPadsTotalR[i]->Write();
//     fhPadsFiredR[i]->Write();
//     fhOccupancyR[i]->Write();
//   }
//
//   fhPullX->Write();
//   fhPullY->Write();
//
//   fhPointsInCluster->Write();
//   fhDigisInCluster->Write();
//   fhHitsInCluster->Write();
//
//   for (Int_t i=0;i<10;i++) fChargeHistos->At(i)->Write();
//
//   fhNpadsVsS->Write();
//   performanceFile->Close();

}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
Double_t LandauMPV(Double_t *lg_x, Double_t *par) {
  Double_t gaz_gain_mean=1.7e+4;
  Double_t scale=1.e+6;
  gaz_gain_mean/=scale;
  Double_t mass = par[0]; // mass in MeV
  Double_t x = TMath::Power(10,lg_x[0]);
  return gaz_gain_mean*CbmMuchDigitizeGem::MPV_n_e(x,mass);
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchHitFinderQa::DigitizerQa(){
  Bool_t verbose = (fVerbose>2);
  TVector3 vIn;   // in  position of the track
  TVector3 vOut;  // out position of the track
  TVector3 p;     // track momentum

  fPointInfos->Clear();

  for (Int_t i=0;i<fPoints->GetEntriesFast();i++){
    CbmMuchPoint* point = (CbmMuchPoint*) fPoints->At(i);
    Int_t stId = CbmMuchAddress::GetStationIndex(point->GetDetectorID());

    // Check if the point corresponds to a certain  MC Track
    Int_t trackId = point->GetTrackID();
    if (trackId < 0) {
        new ((*fPointInfos)[i]) CbmMuchPointInfo(0,0,0,0,0);
        continue;
    }
    CbmMCTrack* mcTrack = (CbmMCTrack*)fMCTracks->At(trackId);
    if (!mcTrack) {
        new ((*fPointInfos)[i]) CbmMuchPointInfo(0,0,0,0,0);
        continue;
    }

    Int_t motherId = mcTrack->GetMotherId();

    // Get mass
    Int_t pdgCode = mcTrack->GetPdgCode();
    TParticlePDG* particle = TDatabasePDG::Instance()->GetParticle(pdgCode);
    if(!particle ||
       pdgCode == 22 ||  // photons
       pdgCode == 2112)  // neutrons
    {
       new ((*fPointInfos)[i]) CbmMuchPointInfo(0,0,0,0,0);
       continue;
    }

    if (CbmMuchAddress::GetLayerIndex(point->GetDetectorID())==0){
      fNall[stId]++;

      if      (pdgCode==2212)                 fNpr[stId]++;
      else if (pdgCode==-211 || pdgCode==211) fNpi[stId]++;
      else if (pdgCode==-11  || pdgCode==11 ) fNel[stId]++;
      else if (pdgCode==-13  || pdgCode==13 ) fNmu[stId]++;
      else if (pdgCode==-321 || pdgCode==321) fNka[stId]++;

      if (motherId==-1) fNprimary[stId]++;
      else              fNsecondary[stId]++;
    }

    Double_t mass = particle->Mass();

    point->PositionIn(vIn);
    point->PositionOut(vOut);
    point->Momentum(p);
    Double_t length = (vOut-vIn).Mag();              // Track length
    Double_t kine = sqrt(p.Mag2()+mass*mass) - mass; // Kinetic energy
    // Get mother pdg code
    Int_t motherPdg = 0;
    CbmMCTrack* motherTrack = NULL;
    if(motherId != -1) motherTrack = (CbmMCTrack*)fMCTracks->At(motherId);
    if(motherTrack) motherPdg = motherTrack->GetPdgCode();
    new ((*fPointInfos)[i]) CbmMuchPointInfo(pdgCode,motherPdg,kine,length,stId);
  }


  // Filling generated charge for each point
  for (Int_t i=0;i<fDigiMatches->GetEntriesFast();i++){
    CbmMuchDigiMatch* match = (CbmMuchDigiMatch*) fDigiMatches->At(i);
    // Get pad area
    CbmMuchDigi* digi = (CbmMuchDigi*) fDigis->At(i);
    CbmMuchModule* module = fGeoScheme->GetModuleByDetId(digi->GetAddress());
    if(!module) continue;
    Double_t area=0;
    if (module->GetDetectorType()!=1 && module->GetDetectorType()!=3) continue;
    CbmMuchModuleGem* module1 = (CbmMuchModuleGem*) module;
    CbmMuchPad* pad = module1->GetPad(digi->GetAddress());
    area = pad->GetDx()*pad->GetDy();
    Int_t nofLinks = match->GetNofLinks();
    for (Int_t pt = 0; pt < nofLinks; pt++) {
      Int_t pointId = match->GetLink(pt).GetIndex();
      Int_t charge  = match->GetLink(pt).GetWeight();
      CbmMuchPointInfo* info = (CbmMuchPointInfo*) fPointInfos->At(pointId);
      if (info->GetPdgCode()==0) continue;
      info->AddCharge(charge);
      info->AddArea(area);
    }
  }

  //Filling digitizer performance plots
  for (Int_t i=0;i<fPointInfos->GetEntriesFast();i++){
    CbmMuchPointInfo* info = (CbmMuchPointInfo*) fPointInfos->At(i);
    if (verbose) {printf("%i",i); info->Print();}
    Double_t length     = info->GetLength();
    Double_t kine       = 1000*(info->GetKine());
    Double_t charge     = info->GetCharge();
    Int_t    pdg        = info->GetPdgCode();
    if (pdg==0) continue;
    if (charge<=0) continue;
    Double_t log_kine   = TMath::Log10(kine);
    Double_t log_charge = TMath::Log10(charge);
    charge = charge/1.e+6;  // measure charge in 10^6 electrons

    Int_t    nPads = info->GetNPads();
    Double_t area  = info->GetArea()/nPads;
    //printf("%f %i\n",TMath::Log2(area),nPads);
    fhNpadsVsS->Fill(TMath::Log2(area),nPads);
    fhCharge->Fill(charge);
    fhChargeLog->Fill(log_charge);
    fhChargeEnergyLog->Fill(log_kine,charge);
    fhChargeTrackLength->Fill(length,charge);

    if (pdg == 2212)                   fhChargeEnergyLogPr->Fill(log_kine,charge);
    else if (pdg == 211 || pdg ==-211) fhChargeEnergyLogPi->Fill(log_kine,charge);
    else if (pdg == 11  || pdg ==-11 ) fhChargeEnergyLogEl->Fill(log_kine,charge);

    if (pdg == 2212)                   fhChargeTrackLengthPr->Fill(length,charge);
    else if (pdg == 211 || pdg ==-211) fhChargeTrackLengthPi->Fill(length,charge);
    else if (pdg == 11  || pdg ==-11 ) fhChargeTrackLengthEl->Fill(length,charge);

    if (pdg == 2212 && length>0.3 && length<0.32 && kine>1000 && kine < 1020)
      fhChargePr_1GeV_3mm->Fill(charge);
  }

}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchHitFinderQa::OccupancyQa(){
  Bool_t verbose = (fVerbose>2);
  // Filling occupancy plots
  for (Int_t i=0;i<fDigis->GetEntriesFast();i++){
    CbmMuchDigi* digi = (CbmMuchDigi*) fDigis->At(i);
    UInt_t address = digi->GetAddress();
    CbmMuchModule* module = fGeoScheme->GetModuleByDetId(address);
    if(!module) continue;
    Double_t r0=0;
    if (module->GetDetectorType()!=1 && module->GetDetectorType()!=3)  continue;
    CbmMuchModuleGem* module1 = (CbmMuchModuleGem*)module;
    CbmMuchPad* pad = module1->GetPad(address);//fGeoScheme->GetPadByDetId(detectorId, channelId);
    Double_t x0 = pad->GetX();
    Double_t y0 = pad->GetY();
    r0 = TMath::Sqrt(x0*x0+y0*y0);
    fhPadsFiredR[CbmMuchAddress::GetStationIndex(address)]->Fill(r0);
  }
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchHitFinderQa::StatisticsQa(){
  Bool_t verbose = (fVerbose>2);
  Int_t nClusters = fClusters->GetEntriesFast();
  TArrayI hitsInCluster;
  TArrayI pointsInCluster;
  hitsInCluster.Set(nClusters);
  pointsInCluster.Set(nClusters);

  for (Int_t i=0;i<nClusters;i++) hitsInCluster[i]=0;
  for (Int_t i=0;i<nClusters;i++) pointsInCluster[i]=0;

  for (Int_t i=0;i<fHits->GetEntriesFast();i++){
    CbmMuchPixelHit* hit = (CbmMuchPixelHit*) fHits->At(i);
    Int_t clusterId = hit->GetRefId();
    hitsInCluster[clusterId]++;
  }

  for (Int_t i=0;i<nClusters;i++){
    CbmMuchCluster* cluster = (CbmMuchCluster*) fClusters->At(i);
    map<Int_t,Int_t> map_points;
    Int_t nDigis = cluster->GetNofDigis();
    fhDigisInCluster->Fill(nDigis);
    for(Int_t digiId=0;digiId<nDigis;digiId++){
      Int_t index = cluster->GetDigi(digiId);
      CbmMuchDigiMatch* match = (CbmMuchDigiMatch*) fDigiMatches->At(index);
      Int_t nPoints = match->GetNofLinks();
      for (Int_t iRefPoint=0;iRefPoint<nPoints;iRefPoint++){
        Int_t pointId = match->GetLink(iRefPoint).GetIndex();
        map_points[pointId]=1;
      }
    }
    pointsInCluster[i]=map_points.size();
    map_points.clear();
  }


  for (Int_t i=0;i<nClusters;i++){
    Int_t nPts  = pointsInCluster[i];
    Int_t nHits = hitsInCluster[i];
    fhPointsInCluster->Fill(nPts);
    fhHitsInCluster->Fill(nHits);
    if (nPts>nHits) fPointsUnderCounted+=(nPts-nHits);
    if (nHits>nPts) fPointsOverCounted+=(nHits-nPts);
    fPointsTotal+=nPts;
  }
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchHitFinderQa::PullsQa(){
  Bool_t verbose = (fVerbose>2);
  // Filling residuals and pools for hits at the first layer
  for (Int_t i=0;i<fHits->GetEntriesFast();i++){
    CbmMuchPixelHit* hit = (CbmMuchPixelHit*) fHits->At(i);
    // Select hits from the first station only
    Int_t iStation = CbmMuchAddress::GetStationIndex(hit->GetAddress());
    Int_t iLayer   = CbmMuchAddress::GetLayerIndex(hit->GetAddress());
    if(!(iStation == 0)) continue;
//    if(!(iStation == 3 && iLayer == 0)) continue;
    if (verbose) printf("   Hit %i, station %i, layer %i ",i,iStation, iLayer);

    Int_t clusterId = hit->GetRefId();
    Bool_t hit_unique=1;
    for (Int_t j=i+1;j<fHits->GetEntriesFast();j++){
      CbmMuchPixelHit* hit1 = (CbmMuchPixelHit*) fHits->At(j);
      //if (hit1->GetStationNr()>stationNr) break;
      if (hit1->GetRefId()==clusterId) { hit_unique=0; break;}
    }
    if (verbose) printf("hit_unique=%i",hit_unique);
    if (!hit_unique) {if (verbose) printf("\n"); continue;}

    // Select hits with clusters formed by only one MC Point
    CbmMuchCluster* cluster = (CbmMuchCluster*) fClusters->At(clusterId);
    Bool_t point_unique=1;
    Int_t  pointId=-1;

    Double_t xmax = 0;
    Double_t xmin = 0;
    Double_t ymax = 0;
    Double_t ymin = 0;
    Double_t dxmin = 0;
    Double_t dymin = 0;

//    if (cluster->GetNDigis()>1) {if (verbose) printf("\n"); continue;}
    for(Int_t digiId=0;digiId<cluster->GetNofDigis();digiId++){
      Int_t index = cluster->GetDigi(digiId);
//      printf("%i\n",index);
      CbmMuchDigiMatch* match = (CbmMuchDigiMatch*) fDigiMatches->At(index);
      // Not unique if the pad has several mcPoint references
      if (verbose) printf(" n=%i",match->GetNofLinks());
      if (match->GetNofLinks()==0) {
        printf(" noise hit");
        point_unique=0;
        break;
      }
      if (match->GetNofLinks()>1) { point_unique=0; break; }
      Int_t currentPointId = match->GetLink(0).GetIndex();
      CbmMuchDigi* digi = (CbmMuchDigi*) fDigis->At(index);
      CbmMuchModuleGem* module = (CbmMuchModuleGem*)fGeoScheme->GetModuleByDetId(digi->GetAddress());
      if(!module) continue;
      CbmMuchPad* pad = module->GetPad(digi->GetAddress());//fGeoScheme->GetPadByDetId(digi->GetDetectorId(), digi->GetChannelId());
      Double_t x = pad->GetX();
      Double_t y = pad->GetY();
      Double_t dx = pad->GetDx();
      Double_t dy = pad->GetDy();
      if (digiId==0 || dxmin>dx   ) dxmin=dx;
      if (digiId==0 || dymin>dy   ) dymin=dy;
      if (digiId==0 || xmin>x-dx/2) xmin=x-dx/2;
      if (digiId==0 || xmax<x+dx/2) xmax=x+dx/2;
      if (digiId==0 || ymin>y-dy/2) ymin=y-dy/2;
      if (digiId==0 || ymax<y+dy/2) ymax=y+dy/2;
      if (digiId==0) { pointId=currentPointId; continue; }
      // Not unique if mcPoint references differ for different digis
      if (currentPointId!=pointId) {point_unique=0; break; }
    }


    if (verbose) printf(" point_unique=%i",point_unique);
    if (!point_unique) {if (verbose) printf("\n"); continue;}
    //printf(" %f %f %f %f %f %f\n",xmin,xmax,ymin,ymax,dxmin,dymin);
    Int_t nPadsX=Int_t((xmax-xmin)/dxmin);
    Int_t nPadsY=Int_t((ymax-ymin)/dymin);
    //printf("nPadsX=%i nPadsY=%i\n",nPadsX,nPadsY);

    if (verbose) printf(" pointId=%i",pointId);
    CbmMuchPoint* point = (CbmMuchPoint*) fPoints->At(pointId);

    Double_t xMC  = 0.5*(point->GetXIn()+point->GetXOut());
    Double_t yMC  = 0.5*(point->GetYIn()+point->GetYOut());
    Double_t tMC  = point->GetTime();
    
    Double_t xRC  = hit->GetX();
    Double_t yRC  = hit->GetY();
    Double_t dx   = hit->GetDx();
    Double_t dy   = hit->GetDy();
    
    Double_t tRC  = hit->GetTime();
    Double_t dt   = hit->GetDTime();
    
    
    if (dx<1.e-10) { printf("Anomalously small dx\n"); continue;}
    if (dy<1.e-10) { printf("Anomalously small dy\n"); continue;}
    fhPullX->Fill((xRC-xMC)/dx);
    fhPullY->Fill((yRC-yMC)/dy);
//    fhPullT[0]->Fill((tRC-tMC)/dt);
//    Int_t nDigis = cluster->GetNDigis();
//   if (nDigis<=fNTimingPulls) fhPullT[nDigis]->Fill((tRC-tMC)/dt);
    
    if (verbose) printf("\n");

//    Int_t index = cluster->GetDigiIndex(0);
//
//    // printf("index=%i\n",index);
//    CbmMuchDigi* digi = (CbmMuchDigi*) fDigis->At(index);
//
//
//    Int_t padSizeX = Int_t(TMath::Log2(dxmin/fPadMinLx));
//    Int_t padSizeY = Int_t(TMath::Log2(dymin/fPadMinLy));
//    if (padSizeX>=fnPadSizesX || padSizeX<0) { printf("wrong x pad size\n"); continue; }
//    if (padSizeY>=fnPadSizesY || padSizeY<0) { printf("wrong y pad size\n"); continue; }
//    if (nPadsX==1 && nPadsY==1) fhPullXpads1[padSizeX]->Fill((xRC-xMC)/dx);
//    if (nPadsY==1 && nPadsX==1) fhPullYpads1[padSizeY]->Fill((yRC-yMC)/dy);
//    if (nPadsX==2 && nPadsY==1) fhPullXpads2[padSizeX]->Fill((xRC-xMC)/dx);
//    if (nPadsY==2 && nPadsX==1) fhPullYpads2[padSizeY]->Fill((yRC-yMC)/dy);
//    if (nPadsX==3 && nPadsY==1) fhPullXpads3[padSizeX]->Fill((xRC-xMC)/dx);
//    if (nPadsY==3 && nPadsX==1) fhPullYpads3[padSizeY]->Fill((yRC-yMC)/dy);
  }

}
// -------------------------------------------------------------------------




// -------------------------------------------------------------------------
void CbmMuchHitFinderQa::ClusterDeconvQa(){
  Int_t nPoints = fPoints->GetEntriesFast();
  Int_t nMatches = fDigiMatches->GetEntriesFast();
  Int_t nClusters = fClusters->GetEntriesFast();
  vector<Int_t> pIndices;
  vector<Int_t>::iterator it;

  for(Int_t iPoint = 0; iPoint < nPoints; ++iPoint){
    if(IsSignalPoint(iPoint)) fSignalPoints++;
  }

  for(Int_t iCluster = 0; iCluster < nClusters; ++iCluster){
    CbmMuchCluster* cluster = (CbmMuchCluster*)fClusters->At(iCluster);
    if(!cluster) continue;
    Int_t nDigis = cluster->GetNofDigis();
    for(Int_t id=0; id < nDigis; ++id){
      Int_t iDigi = cluster->GetDigi(id);
      CbmMuchDigiMatch* match = (CbmMuchDigiMatch*) fDigiMatches->At(iDigi);
      if(!match) continue;
      for(Int_t ip=0; ip<match->GetNofLinks();++ip){
        Int_t iPoint = match->GetLink(ip).GetIndex();
        it = find(pIndices.begin(), pIndices.end(), iPoint);
        if(it != pIndices.end()) continue;
        pIndices.push_back(iPoint);
        if(IsSignalPoint(iPoint)) fSignalHits++;
      }
    }
  }
}
// -------------------------------------------------------------------------

Bool_t CbmMuchHitFinderQa::IsSignalPoint(Int_t iPoint){
  Int_t nPoints = fPoints->GetEntriesFast();
  Int_t nTracks = fMCTracks->GetEntriesFast();
  CbmMuchPoint* point = (iPoint<0 || iPoint>nPoints-1) ? NULL : (CbmMuchPoint*) fPoints->At(iPoint);
  if(!point) return kFALSE;
  Int_t iTrack = point->GetTrackID();
  CbmMCTrack* track = (iTrack<0 || iTrack>nTracks-1) ? NULL : (CbmMCTrack*)fMCTracks->At(iTrack);
  if(!track) return kFALSE;
  if(iTrack != 0 && iTrack != 1) return kFALSE; // Signal tracks must be fist ones
  // Verify if it is a signal muon
  if(track->GetMotherId() < 0){                 // No mother for signal
    Int_t pdgCode = track->GetPdgCode();
    if(TMath::Abs(pdgCode) == 13){              // If it is a muon
      return kTRUE;
    }
  }
  return kFALSE;
}

// -------------------------------------------------------------------------
Int_t CbmMuchHitFinderQa::GetNChannels(Int_t iStation){
  Int_t nChannels = 0;
  vector<CbmMuchModule*> modules = fGeoScheme->GetModules(iStation);
  for(Int_t im = 0; im<modules.size(); im++){
    CbmMuchModule* mod = modules[im];
    if(mod->GetDetectorType() != 1 && mod->GetDetectorType() != 3) continue;
    CbmMuchModuleGem* module = (CbmMuchModuleGem*) mod;
    if(!module) continue;
    nChannels += module->GetNPads();
  }
  return nChannels;
}

// -------------------------------------------------------------------------
Int_t CbmMuchHitFinderQa::GetNSectors(Int_t iStation){
  Int_t nSectors = 0;
  vector<CbmMuchModule*> modules = fGeoScheme->GetModules(iStation);
  for(Int_t im = 0; im<modules.size(); im++){
    CbmMuchModule* mod = modules[im];
    if(mod->GetDetectorType() != 1 && mod->GetDetectorType() != 3) continue;
    CbmMuchModuleGem* module = (CbmMuchModuleGem*) mod;
    if(!module) continue;
    nSectors += module->GetNSectors();
  }
  return nSectors;
}

// -------------------------------------------------------------------------
TVector2 CbmMuchHitFinderQa::GetMinPadSize(Int_t iStation){
  Double_t padMinLx = std::numeric_limits<Double_t>::max();
  Double_t padMinLy = std::numeric_limits<Double_t>::max();
  vector<CbmMuchModule*> modules = fGeoScheme->GetModules(iStation);
  for(Int_t im = 0; im<modules.size(); im++){
    CbmMuchModule* mod = modules[im];
    if(mod->GetDetectorType() != 1) continue;
    CbmMuchModuleGem* module = (CbmMuchModuleGem*) mod;
    vector<CbmMuchPad*> pads = module->GetPads();
    for (Int_t ip=0; ip<pads.size(); ip++) {
      CbmMuchPad* pad = pads[ip];
      if(pad->GetDx() < padMinLx) padMinLx = pad->GetDx();
      if(pad->GetDy() < padMinLy) padMinLy = pad->GetDy();
    }
  }
  return TVector2(padMinLx, padMinLy);
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
TVector2 CbmMuchHitFinderQa::GetMaxPadSize(Int_t iStation){
  Double_t padMaxLx = std::numeric_limits<Double_t>::min();
  Double_t padMaxLy = std::numeric_limits<Double_t>::min();
  vector<CbmMuchModule*> modules = fGeoScheme->GetModules(iStation);
  for(Int_t im = 0; im<modules.size(); im++){
    CbmMuchModule* mod = modules[im];
    if(mod->GetDetectorType() != 1) continue;
    CbmMuchModuleGem* module = (CbmMuchModuleGem*) mod;
    vector<CbmMuchPad*> pads = module->GetPads();
    for (Int_t ip=0; ip<pads.size(); ip++) {
      CbmMuchPad* pad = pads[ip];
      if(pad->GetDx() > padMaxLx) padMaxLx = pad->GetDx();
      if(pad->GetDy() > padMaxLy) padMaxLy = pad->GetDy();
    }
  }
  return TVector2(padMaxLx, padMaxLy);
}
// -------------------------------------------------------------------------


ClassImp(CbmMuchHitFinderQa)
