#include "CbmStsIdealFindHitsEpoch.h"

#include "CbmGeoStsPar.h"
#include "CbmStsDigi.h"
#include "CbmStsDigiPar.h"
#include "CbmStsDigiScheme.h"
#include "CbmStsHit.h"
#include "CbmStsSector.h"
#include "CbmStsStation.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TMath.h"

#include <iomanip>

using std::cout;
using std::cerr;
using std::endl;
using std::flush;
using std::fixed;
using std::right;
using std::left;
using std::setw;
using std::setprecision;
using std::set;
using std::map;
using std::vector;
using std::multimap;
using std::pair;
#include <vector>
#include <map>

// -----   Default constructor   ------------------------------------------
CbmStsIdealFindHitsEpoch::CbmStsIdealFindHitsEpoch() 
  : FairTask("STS Ideal Hit Finder", 1), 
  fGeoPar(NULL),
  fDigiPar(NULL),
  fDigis(NULL),
  fHits(NULL),
  fDigiScheme(new CbmStsDigiScheme()),
  fNStations(0),
  fNEvents(0),
  fDigiMapF(),
  fDigiMapB(),
  fTimer(),
  fTime1(0.)
  ,fSliceSeparationTime(4)
{}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmStsIdealFindHitsEpoch::CbmStsIdealFindHitsEpoch(Int_t iVerbose) 
  : FairTask("STSIdealFindHits", iVerbose), 
  fGeoPar(NULL),
  fDigiPar(NULL),
  fDigis(NULL),
  fHits(NULL),
  fDigiScheme(new CbmStsDigiScheme()),
  fNStations(0),
  fNEvents(0),
  fDigiMapF(),
  fDigiMapB(),
  fTimer(),
  fTime1(0.)
  ,fSliceSeparationTime(4)
{}
// -------------------------------------------------------------------------



// -----   Constructor with name   -----------------------------------------
CbmStsIdealFindHitsEpoch::CbmStsIdealFindHitsEpoch(const char* name, Int_t iVerbose) 
  : FairTask(name, iVerbose), 
  fGeoPar(NULL),
  fDigiPar(NULL),
  fDigis(NULL),
  fHits(NULL),
  fDigiScheme(new CbmStsDigiScheme()),
  fNStations(0),
  fNEvents(0),
  fDigiMapF(),
  fDigiMapB(),
  fTimer(),
  fTime1(0.)
,fSliceSeparationTime(4)
{}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmStsIdealFindHitsEpoch::~CbmStsIdealFindHitsEpoch() {
  if ( fHits ) {
    fHits->Delete();
    delete fHits;
  }
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmStsIdealFindHitsEpoch::Exec(Option_t* opt) {
  fTimer.Start();

  Bool_t warn = kFALSE;

  // Check for digi scheme
  if ( ! fDigiScheme ) {
    cerr << "-E- " << fName << "::Exec: No digi scheme!" << endl;
    return;
  }

  // Clear output array
  //  fHits->Clear();
  fHits->Delete();

  // Sort STS digis with respect to sectors
  SortDigis();

  // Find hits in sectors
  Int_t nDigisF = 0;
  Int_t nDigisB = 0;
  Int_t nHits   = 0;
  Int_t nStations = fDigiScheme->GetNStations();
  CbmStsStation* station = NULL;
  for (Int_t iStation=0; iStation<nStations; iStation++) {
    station = fDigiScheme->GetStation(iStation);

    Int_t nDigisFInStation = 0;
    Int_t nDigisBInStation = 0;
    Int_t nHitsInStation   = 0;
    Int_t nSectors = station->GetNSectors();
    for (Int_t iSector=0; iSector<nSectors; iSector++) {
      CbmStsSector* sector = station->GetSector(iSector);
      set <Int_t> fSet, bSet;
      if ( fDigiMapF.find(sector) == fDigiMapF.end() ) {
        cout << "-E- " << fName << "::Exec: sector " 
             << sector->GetSectorNr() << " of station " 
             << station->GetStationNr() << "not found in front map!" 
             << endl;
        warn = kTRUE;
        continue;
      }
      fSet = fDigiMapF[sector];
      if ( sector->GetType() == 2 || sector->GetType() == 3 ) {
        if ( fDigiMapB.find(sector) == fDigiMapB.end() ) {
          cout << "-E- " << fName << "::Exec: sector " 
               << sector->GetSectorNr() << " of station " 
               << station->GetStationNr() << "not found in back map!" 
               << endl;
          warn = kTRUE;
          continue;
        }
      }
      bSet = fDigiMapB[sector];
      Int_t nDigisFInSector = fSet.size();
      Int_t nDigisBInSector = bSet.size();
      Int_t nHitsInSector   = FindHits(station, sector, fSet, bSet);
      if ( fVerbose > 2 ) 
        cout << "Sector " << sector->GetSectorNr() 
             << ", Digis front " << nDigisFInSector 
             << ", Digis Back " << nDigisBInSector
             << ", Hits " << nHitsInSector << endl;
      nHitsInStation   += nHitsInSector;
      nDigisFInStation += nDigisFInSector;
      nDigisBInStation += nDigisBInSector;      
    }      // Sector loop

    if ( fVerbose > 1 ) cout << "Total for station " 
                             << station->GetStationNr() << ": Digis front "
                             << nDigisFInStation << ", digis back "
                             << nDigisBInStation << ", hits "
                             << nHitsInStation << endl;
    nDigisB += nDigisBInStation;
    nDigisF += nDigisFInStation;
    nHits   += nHitsInStation;
    
  }       // Station loop

  fTimer.Stop();  
  if ( fVerbose > 1 ) {
    cout << endl;
    cout << "-I- " << fName << ":Event summary" << endl;
    cout << "    Active channels front side: " << nDigisF << endl;
    cout << "    Active channels back side : " << nDigisB << endl;
    cout << "    Hits created              : " << nHits   << endl;
    cout << "    Real time                 : " << fTimer.RealTime() 
         << endl;
  }
  if ( fVerbose == 1 ) {
    if ( warn ) cout << "- ";
    else        cout << "+ ";
    cout << setw(15) << left << fName << ": " << setprecision(4) << setw(8) 
         << fixed << right << fTimer.RealTime() 
         << " s, digis " << nDigisF << " / " << nDigisB << ", hits: " 
         << nHits << endl;
  }

  
}
// -------------------------------------------------------------------------

    


// -----   Private method SetParContainers   -------------------------------
void CbmStsIdealFindHitsEpoch::SetParContainers() {

  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  // Get STS geometry parameter container
  fGeoPar = (CbmGeoStsPar*) db->getContainer("CbmGeoStsPar");

  // Get STS digitisation parameter container
  fDigiPar = (CbmStsDigiPar*) db->getContainer("CbmStsDigiPar");

}
// -------------------------------------------------------------------------




// -----   Private method Init   -------------------------------------------
InitStatus CbmStsIdealFindHitsEpoch::Init() {
  
  // Get input array
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  fDigis = (TClonesArray*) ioman->GetObject("StsDigi");

  // Register output array
  fHits = new TClonesArray("CbmStsHit", 1000);
  ioman->Register("StsHit", "Hit in STS", fHits, kTRUE);

  // Build digitisation scheme
  Bool_t success = fDigiScheme->Init(fGeoPar, fDigiPar);
  if ( ! success ) return kERROR;

  // Create sectorwise digi sets
  MakeSets();

  // Control output
  if      (fVerbose == 1 || fVerbose == 2) fDigiScheme->Print(kFALSE);
  else if (fVerbose >  2) fDigiScheme->Print(kTRUE);
  cout << "-I- " << fName << "::Init: "
       << "STS digitisation scheme succesfully initialised" << endl;
  cout << "    Stations: " << fDigiScheme->GetNStations() 
       << ", Sectors: " << fDigiScheme->GetNSectors() << ", Channels: " 
       << fDigiScheme->GetNChannels() << endl;
  
  return kSUCCESS; 
}
// -------------------------------------------------------------------------




// -----   Private method ReInit   -----------------------------------------
InitStatus CbmStsIdealFindHitsEpoch::ReInit() {

  // Clear digitisation scheme
  fDigiScheme->Clear();

  // Build new digitisation scheme
  Bool_t success = fDigiScheme->Init(fGeoPar, fDigiPar);
  if ( ! success ) return kERROR;

  // Create sectorwise digi sets
  MakeSets();

  // Control output
  if      (fVerbose == 1 || fVerbose == 2) fDigiScheme->Print(kFALSE);
  else if (fVerbose >  2) fDigiScheme->Print(kTRUE);
  cout << "-I- " << fName << "::Init: "
       << "STS digitisation scheme succesfully reinitialised" << endl;
  cout << "    Stations: " << fDigiScheme->GetNStations() 
       << ", Sectors: " << fDigiScheme->GetNSectors() << ", Channels: " 
       << fDigiScheme->GetNChannels() << endl;
  
  return kSUCCESS;
}
// -------------------------------------------------------------------------




// -----   Private method MakeSets   ---------------------------------------
void CbmStsIdealFindHitsEpoch::MakeSets() {

  fDigiMapF.clear();
  fDigiMapB.clear();
  Int_t nStations = fDigiScheme->GetNStations();
  for (Int_t iStation=0; iStation<nStations; iStation++) {
    CbmStsStation* station = fDigiScheme->GetStation(iStation);
    Int_t nSectors = station->GetNSectors();
    for (Int_t iSector=0; iSector<nSectors; iSector++) {
      CbmStsSector* sector = station->GetSector(iSector);
      set<Int_t> a;
      fDigiMapF[sector] = a;
      if ( sector->GetType() == 2 || sector->GetType() ==3 ) {
        set<Int_t> b;
        fDigiMapB[sector] = b;
      }
    }
  }
  
}
// -------------------------------------------------------------------------




// -----   Private method SortDigis   --------------------------------------
void CbmStsIdealFindHitsEpoch::SortDigis() {
 
  // Check input array
  if ( ! fDigis ) {
    cout << "-E- " << fName << "::SortDigis: No input array!" << endl;
    return;
  }

  // Clear sector digi sets
  map<CbmStsSector*, set<Int_t> >::iterator mapIt;
  for (mapIt=fDigiMapF.begin(); mapIt!=fDigiMapF.end(); mapIt++)
    ((*mapIt).second).clear();
  for (mapIt=fDigiMapB.begin(); mapIt!=fDigiMapB.end(); mapIt++)
    ((*mapIt).second).clear();

  // Fill digis into sets
  CbmStsDigi* digi = NULL;
  CbmStsSector* sector = NULL;
  Int_t stationNr = -1;
  Int_t sectorNr  = -1;
  Int_t iSide     = -1;
  Int_t nDigis = fDigis->GetEntriesFast();
  for (Int_t iDigi=0; iDigi<nDigis; iDigi++) {
    digi = (CbmStsDigi*) fDigis->At(iDigi);
    stationNr = digi->GetStationNr();
    sectorNr  = digi->GetSectorNr();
    iSide     = digi->GetSide();
    sector = fDigiScheme->GetSector(stationNr, sectorNr);
    if (iSide == 0 ) {
      if ( fDigiMapF.find(sector) == fDigiMapF.end() ) {
        cerr << "-E- " << fName << "::SortDigits:: sector " << sectorNr
             << " of station " << stationNr 
             << " not found in digi scheme (F)!" << endl;
        continue;
      }
      fDigiMapF[sector].insert(iDigi);      
    }
    else if (iSide == 1 ) {
      if ( fDigiMapB.find(sector) == fDigiMapB.end() ) {
        cerr << "-E- " << fName << "::SortDigits:: sector " << sectorNr
             << " of station " << stationNr 
             << " not found in digi scheme (B)!" << endl;
        continue;
      }
      fDigiMapB[sector].insert(iDigi);      
    }
  }
  
}
// -------------------------------------------------------------------------


    

// -----   Private method FindHits   ---------------------------------------
Int_t CbmStsIdealFindHitsEpoch::FindHits(CbmStsStation* station,
                               CbmStsSector* sector, 
                               set<Int_t>& fSet, set<Int_t>& bSet) {

  // Counter
  Int_t nNew = 0;

  // Get sector parameters
  Int_t    detId  = sector->GetDetectorId();
  Int_t    iType  = sector->GetType();

  Double_t rot    = sector->GetRotation();
  Double_t dx     = sector->GetDx();
  Double_t dy     = sector->GetDy();
  Double_t stereoF = sector->GetStereoF();
  Double_t stereoB = sector->GetStereoB();

  //  Double_t z      = station->GetZ();
  Int_t stationNr = station->GetStationNr();
  Int_t sectorNr  = sector->GetSectorNr();

  // Some auxiliary values
  Double_t sinrot = TMath::Sin(rot);
  Double_t cosrot = TMath::Cos(rot);
  Double_t tanstrB = TMath::Tan(stereoB);
  Double_t tanstrF = TMath::Tan(stereoF);
  
  // Calculate error matrix in sector system
  Double_t vX, vY, vXY;
  if ( iType == 1 ) {
    vX  = dx / TMath::Sqrt(12.);
    vY  = dy / TMath::Sqrt(12.);
    vXY = 0.;
  }
  else if ( iType == 2 || iType == 3 ) {
    if (stereoF==0.) {
      vX  = dx / TMath::Sqrt(12.);
      vY  = dx / TMath::Sqrt(6.) / TMath::Abs(tanstrB);
      vXY = -1. * dx * dx / 12. / tanstrB;
    }
    else {
      vX  = dx / TMath::Sqrt(24.);
      vY  = dx / TMath::Sqrt(24.) / TMath::Abs(tanstrB);
      vXY = 0.;
    }
  }
  else {
    cerr << "-E- " << fName << "::FindHits: Illegal sector type "
         << iType << endl;
    return 0;
  }
    
  // Transform variances into global c.s.
  Double_t wX  = vX * vX  * cosrot * cosrot 
               - 2. * vXY * cosrot * sinrot
               + vY * vY  * sinrot * sinrot;
  Double_t wY  = vX * vX  * sinrot * sinrot
               + 2. * vXY * cosrot * sinrot
               + vY * vY  * cosrot * cosrot; 
  Double_t wXY = (vX*vX - vY*vY) * cosrot * sinrot
               + vXY * ( cosrot*cosrot - sinrot*sinrot );
  Double_t sigmaX = TMath::Sqrt(wX);
  Double_t sigmaY = TMath::Sqrt(wY);

  // Now perform the loop over active channels

  // ----- Type 1 : Pixel sector   ---------------------------------------
  if ( iType == 1 ) {
    Fatal("FindHits","Sorry, not implemented yet");
  }     // Pixel sensor
  // ---------------------------------------------------------------------

  // -----  Type 2: Strip sector OSU   -----------------------------------
  else if ( iType == 2 ) {
    Fatal("FindHits","Sorry, not implemented yet");
  }         // Strip OSU
  // ---------------------------------------------------------------------
      
  // -----  Type 3: Strip sector GSI   -----------------------------------
  else if (iType == 3 ) {
    Int_t iDigiF = -1;
    Int_t iDigiB = -1;
    Int_t iChanF = -1;
    Int_t iChanB = -1;
    Int_t nHits = fHits->GetEntriesFast();
    Double_t xHit;
    Double_t yHit;
    Double_t zHit;
    TVector3 pos, dpos;
    CbmStsDigi* digiF = NULL;
    CbmStsDigi* digiB = NULL;
    
    vector<Int_t> sliceF;
    vector<Int_t> sliceB;
    multimap<Double_t,Int_t> mapAll;
    for (set<Int_t>::iterator it1=fSet.begin(); it1!=fSet.end(); it1++) {
      CbmStsDigi* digi = (CbmStsDigi*) fDigis->At(*it1);
      Double_t t = digi->GetTimeStamp();
      mapAll.insert(pair<Double_t,Int_t>(t,2*(*it1)));
    }
    for (set<Int_t>::iterator it2=bSet.begin(); it2!=bSet.end(); it2++) {
      CbmStsDigi* digi = (CbmStsDigi*) fDigis->At(*it2);
      Double_t t = digi->GetTimeStamp();
      mapAll.insert(pair<Double_t,Int_t>(t,2*(*it2)+1));
    }
    Double_t last_time = -1.e+10;
    multimap<Double_t,Int_t>::iterator it=mapAll.begin();
    while (it!=mapAll.end() || sliceF.size()>0 || sliceB.size()>0){
      if (it==mapAll.end() ? 1 : last_time>0 ? it->first-last_time > fSliceSeparationTime : 0){
        if (!sliceF.size() && !sliceB.size()) continue;
        for (vector<Int_t>::iterator it1=sliceF.begin(); it1!=sliceF.end(); it1++) {
          iDigiF = (*it1);
          digiF  = (CbmStsDigi*) fDigis->At(iDigiF);
          if ( ! digiF ) {
            cout << "-W- " << GetName() << "::FindHits: Invalid digi index " 
                 << iDigiF << " in front set of sector " 
                 << sector->GetSectorNr() << ", station " 
                 << station->GetStationNr() << endl;
            continue;
          }
          iChanF = digiF->GetChannelNr();
          for (vector<Int_t>::iterator it2=sliceB.begin(); it2!=sliceB.end(); it2++ ) {
            iDigiB = (*it2);
            digiB = (CbmStsDigi*) fDigis->At(iDigiB);
            if ( ! digiB ) {
              cout << "-W- " << GetName() << "::FindHits: Invalid digi index " 
                   << iDigiB << " in back set of sector " 
                   << sector->GetSectorNr() << ", station " 
                   << station->GetStationNr() << endl;
              continue;
            }
            iChanB = digiB->GetChannelNr();

            Int_t sensorDetId = sector->Intersect(iChanF,iChanB,xHit,yHit,zHit);

            if ( sensorDetId == -1 ) continue;
            
            pos.SetXYZ(xHit, yHit, zHit);
            dpos.SetXYZ(sigmaX, sigmaY, 0.);

            Int_t statLayer = -1;
            for ( Int_t istatL = station->GetNofZ() ; istatL > 0 ; istatL-- ) 
              if ( TMath::Abs(zHit-station->GetZ(istatL-1)) < 0.00001 ) {
                statLayer = istatL-1;
                break;
              }
           
            if ( statLayer == -1 ) 
              cout << "unknown layer for hit at z = " << zHit << endl;
            
            Double_t time1   = digiF->GetTimeStamp();
            Double_t time2   = digiB->GetTimeStamp();
            Double_t dtime1  = digiF->GetTimeStampError();
            Double_t dtime2  = digiB->GetTimeStampError();

            Double_t time   = (time1+time2)/2.;
            Double_t dtime = TMath::Abs(time1-time2)+(dtime1+dtime2)/2.;
            new ((*fHits)[nHits++]) CbmStsHit(sensorDetId, pos, dpos, wXY, 
                                              iDigiF, iDigiB, iChanF, iChanB, statLayer);
            ((CbmStsHit*)fHits->At(nHits-1))->SetTimeStamp(time);
            ((CbmStsHit*)fHits->At(nHits-1))->SetTimeStampError(dtime);
            nNew++;
            if ( fVerbose > 3 ) cout << "New StsHit at (" << xHit << ", " << yHit
                                     << ", " << zHit << "), station " 
                                     << stationNr << ", sector " << sectorNr 
                                     << ", channel " << iChanF << " / " 
                                     << iChanB 
                                     << endl;
            
          }      // back side strip loop
        }        // front side strip loop
        sliceF.clear();
        sliceB.clear();
      }
      if (it==mapAll.end()) continue;
      last_time = it->first;
      Int_t iDigi = it->second/2;
      Bool_t side  = it->second%2;
      if (!side) sliceF.push_back(iDigi); else sliceB.push_back(iDigi);
      it++;
    }
    mapAll.clear();
    
    
  }          // strip GSI
  // ---------------------------------------------------------------------
 
  
  return nNew; 
}
// -------------------------------------------------------------------------

ClassImp(CbmStsIdealFindHitsEpoch)
