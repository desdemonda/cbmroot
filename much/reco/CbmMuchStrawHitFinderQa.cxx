// -------------------------------------------------------------------------
// -----                  CbmMuchStrawHitFinderQa source file          -----
// -----                   Created 26/05/09 by A. Zinchenko            -----
// -------------------------------------------------------------------------
#include "CbmMuchStrawHitFinderQa.h"
#include "CbmMuchPoint.h"
#include "CbmMuchStrawDigi.h"
#include "CbmMuchDigiMatch.h"
#include "CbmMuchCluster.h"
#include "CbmMuchStrawHit.h"

//#include "CbmRunAna.h"
//#include "CbmRuntimeDb.h"
#include "CbmMuchAddress.h"
#include "CbmMuchGeoScheme.h"
#include "CbmMuchStation.h"
#include "CbmMuchSector.h"
#include "CbmMuchPad.h"

#include "FairMCPoint.h"
#include "FairRootManager.h"

#include "TCanvas.h"
#include "TH1D.h"
#include "TMath.h"

using std::cout;
using std::endl;

ClassImp(CbmMuchStrawHitFinderQa)

// -------------------------------------------------------------------------
CbmMuchStrawHitFinderQa::CbmMuchStrawHitFinderQa(const char *name, const char* digiFileName, Int_t verbose)
: FairTask(name,verbose),
  fVerbose(verbose),
  fEvent(0),
  fPoints(NULL),
  fDigis(NULL),
  fDigiMatches(NULL),
  fHits(NULL),
  fDigiFile(digiFileName),
  fGeoScheme(CbmMuchGeoScheme::Instance()),
  fMirror(0),
  fRadIn(),
  fhOccup(),
  fhOccTub(),
  fhMult(),
  fhDx(),
  fhAngTrue(),
  fhAngWrong(),
  fhAngMin(),
  fhAngDif(),
  fhAngDif2(),
  fhAngDifW(),
  fhAngDifW2(),
  fhDtube(),
  fhComb(),
  fhAll(NULL),
  fhPoolX(NULL),
  fhPoolY(NULL)
{
  for (Int_t i = 0; i < 6; ++i) fhOccup[i] = 0x0;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
CbmMuchStrawHitFinderQa::~CbmMuchStrawHitFinderQa(){}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
InitStatus CbmMuchStrawHitFinderQa::Init()
{
  FairRootManager* fManager = FairRootManager::Instance();
  fPoints      = (TClonesArray*) fManager->GetObject("MuchPoint");
  fHits        = (TClonesArray*) fManager->GetObject("MuchStrawHit");
  fDigis       = (TClonesArray*) fManager->GetObject("MuchStrawDigi");
  fDigiMatches = (TClonesArray*) fManager->GetObject("MuchStrawDigiMatch");
  //fClusters    = (TClonesArray*) fManager->GetObject("MuchCluster");

  // Initialize GeoScheme
  TFile* oldfile=gFile;
  TFile* file=new TFile(fDigiFile);
  TObjArray* stations = (TObjArray*) file->Get("stations");
  file->Close();
  file->Delete();
  gFile=oldfile;
  fGeoScheme->Init(stations);
  Int_t nSt = fGeoScheme->GetNStations();

  // Get inner radia of stations and book histos
  //TObjArray* sensNodes = fGeoPar->GetGeoSensitiveNodes();
  Double_t xup = 0, diam[6] = {0.42, 0.42, 0.42, 0.42, 0.42, 0.42}; // tube diameters
  Bool_t sameD = kFALSE;
  if (diam[5] == diam[4] && diam[4] == diam[3]) sameD = kTRUE; // the same tube diam. in all stations
  Int_t nBins = 1;

  for (Int_t i = nSt-1; i > -1; --i) {
    CbmMuchStation *st = fGeoScheme->GetStation(i);
    fRadIn[i] = st->GetRmin();
    if (i == nSt-1 || !sameD) {
      nBins = (Int_t) (st->GetRmax() / diam[i]) + 1;
      nBins = (nBins / 50 + 1) * 50;
      xup = nBins * diam[i];
    }
    Int_t statNo = i + 1; //4;
    //fhOccup[i] = new TH1D(Form("hOccup%i",statNo),Form("Tube occupancy: station %i",statNo),
    //		  nBins*2,-xup,xup);
    fhOccup[i] = new TH1D(Form("hOccup%i",statNo),Form("Tube occupancy: station %i",statNo),
			  nBins,0,xup);
    fhOccTub[i] = new TH1D(Form("hOccTub%i",statNo),Form("Tube occupancy: station %i",statNo),
			   nBins,0,nBins);
    fhMult[i] = new TH1D(Form("hMult%i",statNo),Form("Hit multiplicity: station %i",statNo),10,0,10);
    fhDx[i] = new TH1D(Form("hDx%i",statNo),Form("Hit residual: station %i",statNo),100,-0.5,0.5);
    fhAngTrue[i] = new TH1D(Form("hAngTrue%i",statNo),Form("Doublet angle: station %i",statNo),100,-1.,1.);
    fhAngWrong[i] = new TH1D(Form("hAngWrong%i",statNo),Form("Doublet angle: station %i",statNo),100,-1.,1.);
    fhAngMin[i] = new TH1D(Form("hAngMin%i",statNo),Form("Doublet min. angle: station %i",statNo),100,-1.,1.);
    fhAngDif[i] = new TH1D(Form("hAngDif%i",statNo),Form("Diff of angles: station %i",statNo),100,-1.,1.);
    fhAngDif2[i] = new TH2D(Form("hAngDif2%i",statNo),Form("Diff of angles vs angMin: station %i",statNo),
			    100,-1.,1.,100,-1,1);
    fhAngDifW[i] = new TH1D(Form("hAngDifW%i",statNo),Form("Diff of angles: station %i",statNo),100,-1.,1.);
    fhAngDifW2[i] = new TH2D(Form("hAngDifW2%i",statNo),Form("Diff of angles vs angMin: station %i",statNo),
			     100,-1.,1.,100,-1,1);
    fhDtube[i] = new TH1D(Form("hDtube%i",statNo),Form("Tube difference: station %i",statNo),10,-5,5);
    fhComb[i] = new TH1D(Form("hComb%i",statNo),Form("Combination flag: station %i",statNo),2,0,2);
  }
  fhAll = new TH1D("hAll","Number of hits",4,0,4);

  /*
  fNstations = fDigiScheme->GetNStations();
  fhOccupancyR = new TH1D*[fNstations];
  fhPadsTotalR = new TH1D*[fNstations];
  fhPadsFiredR = new TH1D*[fNstations];
  for (Int_t i=0; i<fNstations; i++){
    CbmMuchStation* station = fDigiScheme->GetStation(i);
    Double_t rMax = station->GetRmax();
    fhPadsTotalR[i] = new TH1D(Form("hPadsTotal%i",i+1),Form("Number of  pads vs radius: station %i",i+1),100,0,1.2*rMax);
    fhPadsFiredR[i] = new TH1D(Form("hPadsFired%i",i+1),Form("Number of fired pads vs radius: station %i",i+1),100,0,1.2*rMax);
    fhOccupancyR[i] = new TH1D(Form("hOccupancy%i",i+1),Form("Occupancy vs radius: station %i",i+1),100,0,1.2*rMax);
    Int_t nSectors = station->GetNSectors();
    for (Int_t j=0;j<nSectors;j++){
      CbmMuchSector* sector = station->GetSectorByIndex(j);
      for (Int_t iPad=0;iPad<sector->GetNChannels();iPad++){
        CbmMuchPad* pad = sector->GetPad(iPad);
        Double_t x0 = pad->GetX0();
        Double_t y0 = pad->GetY0();
	Double_t r0 = TMath::Sqrt(x0*x0+y0*y0);
        fhPadsTotalR[i]->Fill(r0);
      }
    }
  }

  fhPoolX = new TH1D("hPoolX","Pool distribution;(x_{RC} - x_{MC}) / dx_{RC};Entries",100,-5,5);
  fhPoolY = new TH1D("hPoolY","Pool distribution;(y_{RC} - y_{MC}) / dy_{RC};Entries",100,-5,5);
  */
  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchStrawHitFinderQa::SetParContainers() {
  // Get run and runtime database
  /*
  CbmRunAna* run = CbmRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  CbmRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  // Get MUCH geometry parameter container
  fGeoPar = (CbmGeoMuchPar*) db->getContainer("CbmGeoMuchPar");

  // Get MUCH digitisation parameter container
  fDigiPar = (CbmMuchDigiPar*) db->getContainer("CbmMuchDigiPar");
  */
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchStrawHitFinderQa::Exec(Option_t * option)
{
  Double_t diam[6] = {0.42, 0.42, 0.42, 0.42, 0.42, 0.42}; // tube diameters

  Bool_t verbose = (fVerbose>1);
  fEvent++;
  printf("CbmMuchStrawHitFinderQa::Exec()");
  printf("  Event:%i\n",fEvent);

  // Filling occupancy plots
  Int_t nDigis = fDigis->GetEntriesFast();
  for (Int_t i = 0; i < nDigis; ++i){
    CbmMuchStrawDigi* digi = (CbmMuchStrawDigi*) fDigis->UncheckedAt(i);
    Int_t detId =  digi->GetAddress();
    Int_t station3 = CbmMuchAddress::GetStationIndex(detId);
    Double_t x = digi->GetX();
    Double_t tubeNo = x / diam[station3];
    if (TMath::Abs(x) > fRadIn[station3]) {
      fhOccup[station3]->Fill(x,1);
      fhOccTub[station3]->Fill(tubeNo,1);
    } else {
      fhOccup[station3]->Fill(x,1); // around beam hole
      fhOccTub[station3]->Fill(tubeNo,1);
    }
  }

  Int_t nHits = fHits->GetEntriesFast();
  Double_t xz[2] = {0};
  for (Int_t i = 0; i < nHits; ++i){
    CbmMuchStrawHit* hit = (CbmMuchStrawHit*) fHits->UncheckedAt(i);
    CbmMuchStrawDigi* digi = (CbmMuchStrawDigi*) fDigis->UncheckedAt(hit->GetRefId());
    Int_t detId =  digi->GetAddress();
    Int_t station3 = CbmMuchAddress::GetStationIndex(detId);
    fhMult[station3]->Fill(hit->GetFlag()>>1); // overlap multiplicity
    Double_t x = hit->GetX();
    Int_t rot = CbmMuchAddress::GetLayerIndex(detId); // view
    Int_t layer = CbmMuchAddress::GetLayerSideIndex(detId); // layer of the doublet
    if (rot == 0 && layer == 0) fhDx[station3]->Fill(hit->GetU()-x); // hit residual
    if (rot != 0 || layer != 0) continue;
    CbmMuchDigiMatch *digiM = (CbmMuchDigiMatch*) fDigiMatches->UncheckedAt(hit->GetRefId());
    FairMCPoint *p = (FairMCPoint*) fPoints->UncheckedAt(digiM->GetMatchedLink().GetIndex());
    Int_t id = p->GetTrackID();
    xz[0] = hit->GetU();
    xz[1] = hit->GetZ();

    // Fill histo of angle between hits from doublet layers
    for (Int_t j = 0; j < nHits; ++j){
      if (j == i) continue;
      CbmMuchStrawHit* hit1 = (CbmMuchStrawHit*) fHits->UncheckedAt(j);
      CbmMuchStrawDigi* digi1 = (CbmMuchStrawDigi*) fDigis->UncheckedAt(hit1->GetRefId());
      Int_t detId1 =  digi1->GetAddress();
      Int_t stat3 = CbmMuchAddress::GetStationIndex(detId1);
      if (stat3 != station3) continue;
      Int_t rot1 = CbmMuchAddress::GetLayerIndex(detId1); // view
      Int_t lay = CbmMuchAddress::GetLayerSideIndex(detId1); // layer of the doublet
      if (rot1 != 0 || lay == 0) continue;
      digiM = (CbmMuchDigiMatch*) fDigiMatches->UncheckedAt(hit1->GetRefId());
      p = (FairMCPoint*) fPoints->UncheckedAt(digiM->GetMatchedLink().GetIndex());
      Int_t id1 = p->GetTrackID();
      Double_t ang = TMath::ATan2 (hit1->GetU()-xz[0], hit1->GetZ()-xz[1]);
      //cout << id << " " << id1 << " " << stat3 << " " << ang << " " << hit->GetCluster() << " " << hit1->GetCluster() << endl;
      if (id1 == id && hit->GetFlag()%2 == 0 && hit1->GetFlag()%2 == 0) {
	// True combination of hits from adjacent layers 
	fhAngTrue[station3]->Fill(ang);
	fhDtube[station3]->Fill(hit->GetTube()-hit1->GetTube()+0.01); // tube numbers
	//if (TMath::Abs(hit->GetCluster()-hit1->GetCluster()) > 1) cout << " ***!!! dTube: " << hit->GetCluster()-hit1->GetCluster() << endl;
      }
      else fhAngWrong[station3]->Fill(ang);
    }
  }
  if (fMirror) CheckMirrors();
  
  // Filling residuals and pools for hits at the second station
  /*
  for (Int_t i=0;i<fHits->GetEntriesFast();i++){
    CbmMuchHit* hit = (CbmMuchHit*) fHits->At(i);
    // Select hits from the second station only
    Int_t stationNr = hit->GetStationNr();
    if (stationNr<2) continue;
    if (stationNr>2) break;
    if (verbose) printf("   Hit %i, station %i ",i,stationNr);
    // Select hits which are unique in the corresponding cluster
    Bool_t hit_unique=1;
    Int_t clusterId = hit->GetCluster();
    for (Int_t j=i+1;j<fHits->GetEntriesFast();j++){
      CbmMuchHit* hit1 = (CbmMuchHit*) fHits->At(j);
      if (hit1->GetStationNr()>stationNr) break;
      if (hit1->GetCluster()==clusterId) { hit_unique=0; break;}
    }
    if (verbose) printf("hit_unique=%i",hit_unique);
    if (!hit_unique) {if (verbose) printf("\n"); continue;}

    // Select hits with clusters formed by only one MC Point
    CbmMuchCluster* cluster = (CbmMuchCluster*) fClusters->At(clusterId);
    Bool_t point_unique=1;
    Int_t  pointId=-1;
    for(Int_t digiId=0;digiId<cluster->GetNDigis();digiId++){
      Int_t index = cluster->GetDigiIndex(digiId);
      CbmMuchDigiMatch* match = (CbmMuchDigiMatch*) fDigiMatches->At(index);
      // Not unique if the pad has several mcPoint references
      if (verbose) printf(" n=%i",match->GetNPoints());
      if (match->GetNPoints()>1) { point_unique=0; break; }
      Int_t currentPointId = match->GetRefIndex(0);
      if (digiId==0) { pointId=currentPointId; continue; }
      // Not unique if mcPoint references differ for diferent digis
      if (currentPointId!=pointId) {point_unique=0; break; }
    }
    if (verbose) printf(" point_unique=%i",point_unique);
    if (!point_unique) {if (verbose) printf("\n"); continue;}

    if (verbose) printf(" pointId=%i",pointId);
    CbmMuchPoint* point = (CbmMuchPoint*) fPoints->At(pointId);

    Double_t xMC  = 0.5*(point->GetXIn()+point->GetXOut());
    Double_t yMC  = 0.5*(point->GetYIn()+point->GetYOut());

    Double_t xRC  = hit->GetX();
    Double_t yRC  = hit->GetY();
    Double_t dx   = hit->GetDx();
    Double_t dy   = hit->GetDy();

    if (dx<1.e-10) { printf("Anomalously small dx\n"); continue;}
    if (dy<1.e-10) { printf("Anomalously small dy\n"); continue;}
    fhPoolX->Fill((xRC-xMC)/dx);
    fhPoolY->Fill((yRC-yMC)/dy);
    if (verbose) printf("\n");
  }
  */
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMuchStrawHitFinderQa::CheckMirrors()
{
  // Check mirror hits (left-right ambiguity)

  // Find hit and its mirror
  Int_t nHits = fHits->GetEntriesFast();
  CbmMuchStrawHit* hits[2] = {0x0};
  CbmMuchStrawHit **tmp = new CbmMuchStrawHit* [nHits]; // temporary storage
  for (Int_t i = 0; i < nHits; ++i) {
    CbmMuchStrawHit* hit = (CbmMuchStrawHit*) fHits->UncheckedAt(i);
    tmp[i] = hit;
  }

  for (Int_t i = 0; i < nHits; ++i) {
    //CbmMuchHit* hit = (CbmMuchHit*) fHits->UncheckedAt(i);
    CbmMuchStrawHit* hit = tmp[i];
    if (!hit) continue;
    CbmMuchStrawDigi* digi = (CbmMuchStrawDigi*) fDigis->UncheckedAt(hit->GetRefId());
    Int_t detId =  digi->GetAddress();
    Int_t station3 = CbmMuchAddress::GetStationIndex(detId);
    Int_t rot = CbmMuchAddress::GetLayerIndex(detId); // view
    Int_t layer = CbmMuchAddress::GetLayerSideIndex(detId); // layer of the doublet
    //fhMult[station3]->Fill(hit->GetFlag()>>1); // overlap multiplicity
    if (rot != 0 || layer != 0) continue;
    Int_t n2 = 0;
    hits[n2++] = hit;
    CbmMuchDigiMatch *digiM = (CbmMuchDigiMatch*) fDigiMatches->UncheckedAt(hit->GetRefId());
    Int_t nP = digiM->GetNofLinks();
    Int_t id = 999999, id1 = 0;
    for (Int_t i1 = 0; i1 < nP; ++i1) {
      FairMCPoint *p = (FairMCPoint*) fPoints->UncheckedAt(digiM->GetLink(i1).GetIndex());
      id = TMath::Min (id,p->GetTrackID());
      if (i1 == 0) id1 = id;
    }
    for (Int_t i1 = i+1; i1 < nHits; ++i1) {
      //hit = (CbmMuchHit*) fHits->UncheckedAt(i1);
      hit = tmp[i1];
      if (!hit) continue;
      digi = (CbmMuchStrawDigi*) fDigis->UncheckedAt(hit->GetRefId());
      Int_t detId1 =  digi->GetAddress();
      Int_t stat3 = CbmMuchAddress::GetStationIndex(detId1);
      Int_t rot1 = CbmMuchAddress::GetLayerIndex(detId1); // view
      Int_t layer1 = CbmMuchAddress::GetLayerSideIndex(detId1); // layer of the doublet
      //cout << stat3 << " " << hit->GetCluster() << endl;
      if (stat3 != station3) continue;
      if (rot1 != 0 || layer1 != 0) continue;
      // Check if the second hit is the mirror of the first
      if (hit->GetTube() != hits[0]->GetTube()) continue; // different tube
      if (hit->GetSegment() != hits[0]->GetSegment()) continue; // different segments
      digiM = (CbmMuchDigiMatch*) fDigiMatches->UncheckedAt(hit->GetRefId());
      FairMCPoint *p = (FairMCPoint*) fPoints->UncheckedAt(digiM->GetMatchedLink().GetIndex());
      if (p->GetTrackID() != id1) continue; // different track ID
      hits[n2++] = hit;
      tmp[i1] = 0x0;
      if (n2 == 2) break;
    }
    if (n2 != 2 || hits[0]->GetFlag()%2 == 0 && hits[1]->GetFlag()%2 == 0 || 
	hits[0]->GetFlag()%2 > 0 && hits[1]->GetFlag()%2 > 0) {
      cout << " *** CheckMirrors: Strange " << n2 << " " <<  hits[0]->GetFlag()%2 << " " <<  hits[1]->GetFlag()%2 << " " << id << " " << hits[0]->GetTube() << endl;
      exit(0);
    }
    if (id < 2) fhAll->Fill(station3+1.1);

    // Check combinations with hits from adjacent layer
    Int_t iMin = 0, iMin1 = 0;
    Double_t angMin = 99999, angMin1 = 99999;
    CbmMuchStrawHit* hitMin = 0x0, *hitMin1 = 0x0;
    for (Int_t i1 = 0; i1 < nHits; ++i1) {
      //CbmMuchHit* hit = (CbmMuchHit*) fHits->UncheckedAt(i1);
      CbmMuchStrawHit* hit1 = tmp[i1];
      if (!hit1) continue;
      if (hit1 == hits[0] || hit1 == hits[1]) continue;
      CbmMuchStrawDigi* digi1 = (CbmMuchStrawDigi*) fDigis->UncheckedAt(hit1->GetRefId());
      Int_t detId1 =  digi1->GetAddress();
      Int_t stat3 = CbmMuchAddress::GetStationIndex(detId1);
      if (stat3 != station3) continue;
      Int_t rot1 = CbmMuchAddress::GetLayerIndex(detId1); // view
      Int_t layer1 = CbmMuchAddress::GetLayerSideIndex(detId1); // layer of the doublet
      if (rot1 != 0 || layer1 == 0) continue;
      if (hit1->GetSegment() != hits[0]->GetSegment()) continue; // different segments
      digiM = (CbmMuchDigiMatch*) fDigiMatches->UncheckedAt(hit1->GetRefId());
      nP = digiM->GetNofLinks();
      for (Int_t i2 = 0; i2 < nP; ++i2) {
	FairMCPoint *p = (FairMCPoint*) fPoints->UncheckedAt(digiM->GetLink(i2).GetIndex());
	if (p->GetTrackID() != id) continue;
	if (id < 2 && hit1->GetFlag()%2 > 0) fhAll->Fill(station3+1.1);
	break;
      }
      if (TMath::Abs(hit1->GetTube()-hits[0]->GetTube()) > 2) continue; // tubes far away
      Double_t angPos = TMath::ATan2 (hit1->GetU(), hit1->GetZ()); // angular position
      for (Int_t j = 0; j < 2; ++j) {
	Double_t ang = TMath::ATan2 (hit1->GetU()-hits[j]->GetU(), hit1->GetZ()-hits[j]->GetZ());
	//cout << ang << " " << hits[j]->GetFlag() << " " << hit1->GetFlag() << " " << hits[j]->GetTube() << " " << hit1->GetTube() << " " << hits[j]->GetU() << " " << hit1->GetU() << " " << TMath::ATan2(hits[j]->GetU(),hits[j]->GetZ()) << " " << TMath::ATan2(hit1->GetU(),hit1->GetZ()) << endl;
	if (TMath::Abs(ang-angPos) < TMath::Abs(angMin)) {
	  angMin1 = angMin; 
	  iMin1 = iMin;
	  hitMin1 = hitMin;
	  iMin = j;
	  angMin = ang-angPos;
	  hitMin = hit1;
	} else if (TMath::Abs(ang-angPos) < TMath::Abs(angMin1)) {
	  angMin1 = ang - angPos;
	  iMin1 = j;
	  hitMin1 = hit1;
	}
      }
    } // for (Int_t i1 = 0; i1 < nHits;
    if (hitMin == 0x0) continue;
    digiM = (CbmMuchDigiMatch*) fDigiMatches->UncheckedAt(hitMin->GetRefId());
    Int_t ok = 1;
    id1 = 99999;
    nP = digiM->GetNofLinks();
    for (Int_t i1 = 0; i1 < nP; ++i1) {
      FairMCPoint *p = (FairMCPoint*) fPoints->UncheckedAt(digiM->GetLink(i1).GetIndex());
      id1 = TMath::Min (id1,p->GetTrackID());
    }
    if (id1 != id) ok = 0;
    if (hits[iMin]->GetFlag()%2 > 0 || hitMin->GetFlag()%2 > 0) ok = 0; // mirror hit
    if (id < 2 || id1 < 2) {
      // Only dimuons
      fhAngMin[station3]->Fill(angMin);
      //fhComb[station3]->Fill(ok+0.1);
      if (id > 1 || hits[iMin]->GetFlag()%2 > 0) fhComb[station3]->Fill(0.1);
      else fhComb[station3]->Fill(1.1);
      if (id1 > 1 || hitMin->GetFlag()%2 > 0) fhComb[station3]->Fill(0.1);
      else fhComb[station3]->Fill(1.1);
      if (ok) {
	fhAngDifW[station3]->Fill(angMin1-angMin);
	fhAngDifW2[station3]->Fill(angMin,angMin1-angMin);
      } else {
	fhAngDif[station3]->Fill(angMin1-angMin);
	fhAngDif2[station3]->Fill(angMin,angMin1-angMin);
      }
    }
    // Take another hit combination
    if (hitMin1 == 0x0) continue;
    //if (TMath::Abs(angMin1-angMin) > 0.2) continue;
    if (TMath::Abs(angMin1-angMin) > -0.1) continue;
    digiM = (CbmMuchDigiMatch*) fDigiMatches->UncheckedAt(hitMin1->GetRefId());
    id1 = 99999;
    nP = digiM->GetNofLinks();
    for (Int_t i1 = 0; i1 < nP; ++i1) {
      FairMCPoint *p = (FairMCPoint*) fPoints->UncheckedAt(digiM->GetLink(i1).GetIndex());
      id1 = TMath::Min (id1,p->GetTrackID());
    }
    if (hits[iMin]->GetFlag()%2 > 0 || hitMin->GetFlag()%2 > 0) ok = 0; // mirror hit
    if (id < 2 || id1 < 2) {
      // Only dimuons
      if (iMin1 != iMin) {
	if (id > 1 || hits[iMin1]->GetFlag()%2 > 0) fhComb[station3]->Fill(0.1);
	else fhComb[station3]->Fill(1.1);
      }
      if (hitMin1 != hitMin) {
	if (id1 > 1 || hitMin1->GetFlag()%2 > 0) fhComb[station3]->Fill(0.1);
	else fhComb[station3]->Fill(1.1);
      }
    }
    cout << " *** " << station3 << " " << id << " " << angMin << " " << angMin1 << " " << hits[iMin]->GetFlag() << " " << hitMin->GetFlag() << " " << ok << endl;
  } // for (Int_t i = 0; i < nHits; ++i)

  delete [] tmp;
}

// -------------------------------------------------------------------------
void CbmMuchStrawHitFinderQa::Finish()
{
  // Write histograms
  gDirectory->mkdir("MuchStrawHitsQA");
  gDirectory->cd("MuchStrawHitsQA");

  for (Int_t i = 0; i < 6; ++i) {
    if (fhOccup[i] == 0x0) continue;
    //fhPadsTotalR[i]->Sumw2();
    //fhPadsFiredR[i]->Sumw2();
    //fhPadsFiredR[i]->Scale(1./fEvent);
    //fhOccup[i]->Scale(1./fEvent/6);
    fhOccup[i]->Scale(1./fEvent/12);
    fhOccup[i]->Write();
    fhOccTub[i]->Scale(1./fEvent/12);
    fhOccTub[i]->Write();
    fhMult[i]->Write();
    fhDx[i]->Write();
    fhAngTrue[i]->Write();
    fhAngWrong[i]->Write();
    fhAngMin[i]->Write();
    fhAngDif[i]->Write();
    fhAngDif2[i]->Write();
    fhAngDifW[i]->Write();
    fhAngDifW2[i]->Write();
    fhDtube[i]->Write();
    fhComb[i]->Write();
  }
  fhAll->Write();
  /*
  fhPoolX->Sumw2();
  fhPoolY->Sumw2();
  fhPoolX->Fit("gaus","q");
  fhPoolY->Fit("gaus","q");


  if (fVerbose>1) {
    TCanvas* c1 = new TCanvas("c1","All pad distributions",1500,900);
    c1->Divide(5,3);
    for (Int_t i=0;i<15;i++) {
      c1->cd(5*(i%3)+i/3+1);
      fhPadsTotalR[i]->Draw();
    }
    c1->cd();

    TCanvas* c2 = new TCanvas("c2","Fired pad distributions",1500,900);
    c2->Divide(5,3);
    for (Int_t i=0;i<15;i++) {
      c2->cd(5*(i%3)+i/3+1);
      fhPadsFiredR[i]->Draw();
    }
    c2->cd();

    TCanvas* c3 = new TCanvas("c3","Occupancy plots",1500,900);
    c3->Divide(5,3);
    for (Int_t i=0;i<15;i++) {
      c3->cd(5*(i%3)+i/3+1);
      fhOccupancyR[i]->Draw();
    }
    c3->cd();

    TCanvas* c4 = new TCanvas("c4","Pools",800,400);
    c4->Divide(2,1);
    c4->cd(1);
    fhPoolX->Draw();
    c4->cd(2);
    fhPoolY->Draw();
    c4->cd();
  }
  */
  gDirectory->cd("..");
}
// -------------------------------------------------------------------------
