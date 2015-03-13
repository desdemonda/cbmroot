// -------------------------------------------------------------------------
// -----                    CbmMvdHitProducer source file              -----
// -----                  Created 08/11/06  by V. Friese               -----
// -----           Based on CbmStsMapsHitProducer by M. Deveaux        -----
// -------------------------------------------------------------------------
// Includes from MVD
#include "CbmMvdHitProducer.h"

#include "CbmMvdGeoPar.h"
#include "CbmMvdHit.h"
#include "CbmMvdHitMatch.h"
#include "CbmMvdPileupManager.h"
#include "CbmMvdPoint.h"

// Includes from base
#include "FairGeoNode.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

// Includes from ROOT
#include "TArrayD.h"
#include "TClonesArray.h"
#include "TObjArray.h"
#include "TRandom3.h"
#include "TString.h"
#include "TVector3.h"

// Includes from C++
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>

using std::cout;
using std::endl;
using std::map;
using std::setw;
using std::left;
using std::right;
using std::fixed;
using std::pair;
using std::setprecision;
using std::ios_base;
using std::vector;


// -----   Default constructor   ------------------------------------------
CbmMvdHitProducer::CbmMvdHitProducer() 
  : FairTask("MVDHitProducer"),
    fMode(0),
    fSigmaX(0.0005),
    fSigmaY(0.0005),
    fEfficiency(0.99),
    fMergeDist(0.),
    fFakeRate(0.03),
    fNPileup(0),
    fStationMap(),
    fRadiusMap(),
    fBranchName("MvdPoint"),
    fBgFileName(""),
    fPoints(NULL),
    fHits(new TClonesArray("CbmMvdHit")),
    fMatches(new TClonesArray("CbmMvdHitMatch")),
    fRandGen(2736),
    fTimer(),
    fPileupManager(NULL),
    fGeoPar(NULL),
    fNEvents(0),
    fNPoints(0.),
    fNReal(0.),
    fNBg(0.),
    fNFake(0.),
    fNLost(0.),
    fNMerged(0.),
    fTime(0.)
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmMvdHitProducer::CbmMvdHitProducer(const char* name, Int_t iMode, 
				     Int_t iVerbose) 
  : FairTask(name, iVerbose),
    fMode(iMode),
    fSigmaX(0.0005),
    fSigmaY(0.0005),
    fEfficiency(0.99),
    fMergeDist(0.),
    fFakeRate(0.03),
    fNPileup(0),
    fStationMap(),
    fRadiusMap(),
    fBranchName("MvdPoint"),
    fBgFileName(""),
    fPoints(NULL),
    fHits(new TClonesArray("CbmMvdHit")),
    fMatches(new TClonesArray("CbmMvdHitMatch")),
    fRandGen(2736),
    fTimer(),
    fPileupManager(NULL),
    fGeoPar(NULL),
    fNEvents(0),
    fNPoints(0.),
    fNReal(0.),
    fNBg(0.),
    fNFake(0.),
    fNLost(0.),
    fNMerged(0.),
    fTime(0.)
{
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmMvdHitProducer::~CbmMvdHitProducer() {
  if ( fHits ) {
    fHits->Delete();
    delete fHits;
  }
  if ( fMatches ) {
    fMatches->Delete();
    delete fMatches;
  }
  if ( fPileupManager ) delete fPileupManager;
}
// -------------------------------------------------------------------------  



// -----   Virtual public method Exec   ------------------------------------
void CbmMvdHitProducer::Exec(Option_t* opt) {

  if ( fMode == 0 ) ExecMaps();
  else              ExecIdeal();

}
// ------------------------------------------------------------------------- 



// -----   Public modifiers   ----------------------------------------------
void CbmMvdHitProducer::SetEfficiency(Double_t eff) {
  if ( eff < 0. || eff > 1. ) cout << "-W- " << fName 
				   << "::SetEfficiency: Illegal value " 
				   << eff << endl;
  else fEfficiency = eff;
}

void CbmMvdHitProducer::SetMergeDistance(Double_t dist) {
  if ( dist < 0. ) cout << "-W- " << fName << "::SetMergeDistance: "
			<< "Illegal value " << dist << endl;
  else fMergeDist = dist;
}

void CbmMvdHitProducer::SetFakeRate(Double_t rate) {
  if ( rate < 0. ) cout << "-W- " << fName << "::SetFakeRate: "
			<< "Illegal value " << rate << endl;
  else fFakeRate = rate;
}

void CbmMvdHitProducer::SetNPileup(Int_t nPileup) {
  if ( nPileup < 0 ) cout << "-W- " << fName << "::SetNPileup: "
			  << "Illegal value " << nPileup << endl;
  else fNPileup = nPileup;
}
// ------------------------------------------------------------------------- 

    

// -----   Virtual private method SetParContainers   -----------------------
void CbmMvdHitProducer::SetParContainers() {
  FairRunAna*    ana  = FairRunAna::Instance();
  FairRuntimeDb* rtdb = ana->GetRuntimeDb();
  fGeoPar  = (CbmMvdGeoPar*)  (rtdb->getContainer("CbmMvdGeoPar"));
}
// -------------------------------------------------------------------------


			      
// -----    Virtual private method Init   ----------------------------------
InitStatus CbmMvdHitProducer::Init() {

  // Check for too many pileup events
  if (fNPileup > 200) {
    cout << "-E- CbmMvdHitProducer::Init:  Pileup of " << fNPileup 
	 << " too large; maximum buffer size is 1000 events." << endl;
    return kERROR;
  }

  // Register output arrays
  Register();

  // Get input array
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- " << GetName() << "::Init: No FairRootManager!" << endl;
    return kFATAL;
  }
  fPoints = (TClonesArray*) ioman->GetObject(fBranchName);

  // Create BgManager if necessary
  if (fNPileup > 1 && !(fPileupManager) && fMode == 0 ) {
    if ( fBgFileName == "" ) {
    cout << "-E- " << GetName() << "::Init: Pileup events needed, but no " 
	 << " background file name given! " << endl;
    return kERROR;
    }
    fPileupManager = new CbmMvdPileupManager(fBgFileName, 
					     fBranchName, 1000);
  }

  // Get parameters from database
  Bool_t success = GetParameters();
  if ( ! success ) {
    cout << "-E- " << GetName() << "::Init: "
	 << "Error in accessing parameters" << endl;
    return kERROR;
  }

  // Screen output
  cout << endl << "---------------------------------------------" << endl;
  cout << GetName() << " initialised with parameters: " << endl;
  PrintParameters();
  cout << "---------------------------------------------" << endl;

  return kSUCCESS;

}
// -------------------------------------------------------------------------



// -----   Virtual public method Reinit   ----------------------------------
InitStatus CbmMvdHitProducer::ReInit() {

  // Get parameters from database
  Bool_t success = GetParameters();
  if ( ! success ) {
    cout << "-E- " << GetName() << "::Reinit: "
	 << "Error in accessing parameters" << endl;
    return kERROR;
  }

  // Screen output
  cout << endl << "---------------------------------------------" << endl;
  cout << GetName() << " reinitialised with parameters: " << endl;
  PrintParameters();
  cout << "---------------------------------------------" << endl;

  return kSUCCESS;

}
// -------------------------------------------------------------------------



// -----   Private method ExecMaps   ---------------------------------------
void CbmMvdHitProducer::ExecMaps() {

  // Reset output arrays and counters
  fTimer.Start();
  Reset();
  Int_t nPoints = 0;
  Int_t nReal   = 0;
  Int_t nLost   = 0;
  Int_t nBg     = 0;
  Int_t nFake   = 0;
  Int_t nMerged = 0;

  // Process points from input file
  Int_t n1 = 0;   // Number of MvdPoints
  Int_t n2 = 0;   // Number of created hits
  Int_t n3 = 0;   // Number of lost hits
  ProduceHits(fPoints, 0, 0, n1, n2, n3);
  nPoints += n1;
  nReal   += n2;
  nLost   += n3;
  
  // Process points from background file (pileup)
  if (fNPileup > 1) {
    if ( ! fPileupManager ) Fatal("Exec",
				  "No Pileup Manager!");
    TClonesArray* bgArray = NULL;
    Int_t nEvents     = fPileupManager->GetNEvents();
    Int_t firstEvent  = fRandGen.Integer(nEvents-fNPileup+2);
    for (Int_t iBg=0; iBg<fNPileup-1; iBg++) {
      Int_t iEvent = firstEvent + iBg;
      bgArray = fPileupManager->GetEvent(iEvent);
      ProduceHits(bgArray, 1, iEvent, n1, n2, n3);
      nBg += n2;
    }
  }

  // Produce Fake Hits
  nFake   = ProduceFakes();

  // Merge hits
  nMerged = MergeHits();

  // Event summary
  fTimer.Stop();
  if ( fVerbose ) {
  cout << endl;
  cout << "======   CbmMvdHitProducer : Event summary   =============="
       << endl;
  cout << "MvdPoints           : " << nPoints << endl;
  cout << "True hits           : " << nReal   << endl;
  cout << "Background hits     : " << nBg << endl;
  cout << "Fake hits           : " << nFake << endl;
  cout << "Total hits created  : " << nReal+nBg+nFake << endl;
  cout << "Lost hits           : " << nLost << endl;
  cout << "Merged hits         : " << nMerged << endl;
  cout << "Real time " << fTimer.RealTime() << ", CPU time "
       << fTimer.CpuTime() << endl;
  cout << "==============================================================="
       << endl << endl;
  }
  else cout << "+ " << setw(15) << left<< fName << setprecision(4) << setw(8)
	    << fixed << right << fTimer.RealTime() 
	    << " s, points " << setw(5) << nPoints << ", hits " << setw(5)
	    << nReal+nBg+nFake << endl;


  // Statistics
  fNEvents++;
  fNPoints += Double_t(nPoints);
  fNReal   += Double_t(nReal);
  fNBg     += Double_t(nBg);
  fNFake   += Double_t(nFake);
  fNLost   += Double_t(nLost);
  fNMerged += Double_t(nMerged);
  fTime    += fTimer.RealTime();
  
}
// -------------------------------------------------------------------------



// -----   Private method ExecIdeal   --------------------------------------
void CbmMvdHitProducer::ExecIdeal() {

  // Reset output arrays and counters
  fTimer.Start();
  Reset();
  Int_t nPoints = 0;
  Int_t nReal   = 0;
  Int_t nLost   = 0;
  Int_t nBg     = 0;
  Int_t nFake   = 0;
  Int_t nMerged = 0;
  CbmMvdPoint* point = NULL;
  TVector3 pos, dpos;

  // Process point from input file
  nPoints = fPoints->GetEntriesFast();
  for (Int_t iPoint=0; iPoint<nPoints; iPoint++) {
    point = (CbmMvdPoint*) fPoints->At(iPoint);

    // Get station number
    Int_t volId = point->GetDetectorID();
    if ( fStationMap.find(volId) == fStationMap.end() ) {
      cout << "-W- " << fName << "::ExecIdeal: Volume ID " << volId
	   << " not found in sensitive nodes!" << endl;
      continue;
    }
    Int_t statNr = fStationMap[volId];
    Int_t detId = (statNr << 16) | (2 << 24);
    Int_t trackId = point->GetTrackID();

    // Take hit coordinates in the midplane of the station
    Double_t xpt = 0.5 * ( point->GetX() + point->GetXOut() );
    Double_t ypt = 0.5 * ( point->GetY() + point->GetYOut() );
    Double_t zpt = 0.5 * ( point->GetZ() + point->GetZOut() );
    pos.SetXYZ(xpt, ypt, zpt);
    dpos.SetXYZ(0.0001, 0.0001, 0.);

    // Create new hit and hitMatch
    Int_t nHits = fHits->GetEntriesFast();
    //new ((*fHits)[nHits]) CbmMvdHit(detId, pos, dpos, 0);
    new ((*fHits)[nHits]) CbmMvdHit(statNr, pos, dpos, 0); // cdritsa; 28apr09
    new ((*fMatches)[nHits]) CbmMvdHitMatch(0, 0, trackId, iPoint, 0);
    nReal++;

  }

  // Event summary
  fTimer.Stop();
  if ( fVerbose ) {
  cout << endl;
  cout << "======   CbmMvdHitProducer : Event summary   =============="
       << endl;
  cout << "MvdPoints           : " << nPoints << endl;
  cout << "True hits           : " << nReal   << endl;
  cout << "Background hits     : " << nBg << endl;
  cout << "Fake hits           : " << nFake << endl;
  cout << "Total hits created  : " << nReal+nBg+nFake << endl;
  cout << "Lost hits           : " << nLost << endl;
  cout << "Merged hits         : " << nMerged << endl;
  cout << "Real time " << fTimer.RealTime() << ", CPU time "
       << fTimer.CpuTime() << endl;
  cout << "==============================================================="
       << endl << endl;
  }
  else cout << "+ " << setw(15) << left<< fName << setprecision(4) << setw(8)
	    << fixed << right << fTimer.RealTime() 
	    << " s, points " << setw(5) << nPoints << ", hits " << setw(5)
	    << nReal+nBg+nFake << endl;

  // Statistics
  fNEvents++;
  fNPoints += Double_t(nPoints);
  fNReal   += Double_t(nReal);
  fNBg     += Double_t(nBg);
  fNFake   += Double_t(nFake);
  fNLost   += Double_t(nLost);
  fNMerged += Double_t(nMerged);
  fTime    += fTimer.RealTime();

}    
// -------------------------------------------------------------------------



// -----   Virtual method Finish   -----------------------------------------
void CbmMvdHitProducer::Finish() {
  cout << endl;
  cout << "============================================================"
       << endl;
  cout << "===== " << fName << ": Run summary " << endl;
  cout << "===== " << endl;
  cout << "===== Events processed          : " << setw(8) << fNEvents << endl;
  cout.setf(ios_base::fixed, ios_base::floatfield);
  cout << "===== Real time per event       : " 
       << setw(8) << setprecision(4) 
       << fTime / Double_t(fNEvents) << " s" << endl;
  cout << "===== MvdPoints per event       : " 
       << setw(8) << setprecision(2) 
       << fNPoints / Double_t(fNEvents) << endl;
  cout << "===== Real hits per event       : " 
       << setw(8) << setprecision(2) 
       << fNReal / Double_t(fNEvents) << " = "
       << setw(6) << setprecision(2) 
       << fNReal / (fNReal + fNBg + fNFake) * 100. << " %" << endl;
  cout << "===== Background hits per event : " 
       << setw(8) << setprecision(2) 
       << fNBg / Double_t(fNEvents) << " = "
       << setw(6) << setprecision(2) 
       << fNBg / (fNReal + fNBg + fNFake) * 100. << " %" << endl;
  cout << "===== Fake hits per events      : " 
       << setw(8) << setprecision(2) 
       << fNFake / Double_t(fNEvents) << " = " 
       << setw(6) << setprecision(2) 
       << fNFake / (fNReal + fNBg + fNFake) * 100. << " %" << endl;
  cout << "===== Lost hits per event       : " 
       << setw(8) << setprecision(2) 
       << fNLost / Double_t(fNEvents) << " = " 
       << setw(6) << setprecision(2) 
       << fNLost / fNPoints * 100. << " %" << endl;
  cout << "===== Merged hits per event     : " 
       << setw(8) << setprecision(2) 
       << fNMerged / Double_t(fNEvents) << " = " 
       << setw(6) << setprecision(2) 
       << fNMerged / (fNReal + fNBg + fNFake) * 100. << " %" << endl;
  cout << "===== Total hits per event      : " 
       << setw(8) << setprecision(2) 
       << (fNReal + fNBg + fNFake) / Double_t(fNEvents) << endl;
  cout << "============================================================"
       << endl;
	
}					       
// -------------------------------------------------------------------------



// -----   Private method Register   ---------------------------------------
void CbmMvdHitProducer::Register() {
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman) Fatal("Register",
		      "No FairRootManager");
  ioman->Register("MvdHit", "Mmv Hit", fHits, kTRUE);
  ioman->Register("MvdHitMatch", "MVD Hit Match", fMatches, kTRUE);
}
// -------------------------------------------------------------------------  



// -----   Private method Reset   ------------------------------------------
void CbmMvdHitProducer::Reset() {
  fHits->Clear("C");
  fMatches->Clear("C");
}  
// -------------------------------------------------------------------------  



// -----   Private method PrintParameters   --------------------------------
void CbmMvdHitProducer::PrintParameters() {
  if ( fMode == 0 ) {
    cout.setf(ios_base::fixed, ios_base::floatfield);
    cout << "Resolution in x  : " << setw(8) << setprecision(2)
	 << fSigmaX * 10000. << " mum" << endl;
    cout << "Resolution in y  : " << setw(8) << setprecision(2)
	 << fSigmaY * 10000. << " mum" << endl;
    cout << "Efficiency       : " << setw(8) << setprecision(2)
	 << fEfficiency * 100. << " %" << endl;
    cout << "Fake hit rate    : " << setw(8) << setprecision(2)
	 << fFakeRate * 100. << " %" << endl;
    cout << "Merging distance : " << setw(8) << setprecision(2)
	 << fMergeDist * 10. << " mm" << endl;
    cout << "Event pileup     : " << setw(5) << fNPileup << endl;
    cout << endl;
  }
  else cout << "Ideal hit producing mode" << endl << endl;
  cout << "Stations used: " << endl;
  map<Int_t, Int_t>::iterator mapIt;
  for (mapIt=fStationMap.begin(); mapIt!=fStationMap.end(); mapIt++)
    cout << "MVD Station " << (*mapIt).second << ", volume ID " 
	 << (*mapIt).first << endl;
}
// -------------------------------------------------------------------------  



// -----   Private method GetParameters   ----------------------------------
Bool_t CbmMvdHitProducer::GetParameters() {

  fStationMap.clear();
  fRadiusMap.clear();
  if ( ! fGeoPar ) return kFALSE;
  TObjArray* sensNodes = fGeoPar->GetGeoSensitiveNodes();
  Int_t nNodes = sensNodes->GetEntriesFast();
  for (Int_t iNode=0; iNode<nNodes; iNode++) {
    FairGeoNode* node = (FairGeoNode*) (sensNodes->At(iNode));
    Int_t volId = node->getMCid();
    TString nodeName = node->getName();
    char a[2];
    a[0] = nodeName[10];
    a[1] = nodeName[11];
    Int_t stationNr = atoi(a);
    fStationMap[volId] = stationNr;
    TArrayD* params = node->getParameters();
    Double_t rMin = params->At(0);
    Double_t rMax = params->At(1);
    pair<Double_t, Double_t> radii(rMin, rMax);
    fRadiusMap[stationNr] = radii;
  }   

  return kTRUE;
}
// -------------------------------------------------------------------------  



// -----   Private method ProduceHits   ------------------------------------
void CbmMvdHitProducer::ProduceHits(TClonesArray* pointArray,
				    Int_t iFile, Int_t iEvent,
				    Int_t& nPoints,
				    Int_t& nCreated, Int_t& nLost) {

  TClonesArray& hitArray   = *fHits;
  TClonesArray& matchArray = *fMatches;

  Int_t mCreated = 0;
  Int_t mLost    = 0;

  TVector3 pos, dpos;  

  CbmMvdPoint* point = NULL;
  nPoints = pointArray->GetEntriesFast();
  for (Int_t iPoint=0; iPoint<nPoints; iPoint++) {
    point = (CbmMvdPoint*) pointArray->At(iPoint);

    // Loss due to detector inefficiency
    if ( fRandGen.Rndm(1) > fEfficiency ) {
      mLost++;
      continue;
    }

    // Get station number
    Int_t statNr = point->GetStationNr();
    Int_t trackId = point->GetTrackID();
       
    // Point position in the middle of the station
    Double_t xpt = 0.5 * ( point->GetX() + point->GetXOut() );
    Double_t ypt = 0.5 * ( point->GetY() + point->GetYOut() );
    Double_t zpt = 0.5 * ( point->GetZ() + point->GetZOut() );

    // Hit position (Gaussian smearing)
    Double_t xHit = fRandGen.Gaus(xpt, fSigmaX);
    Double_t yHit = fRandGen.Gaus(ypt, fSigmaY);
    pos.SetXYZ(xHit, yHit, zpt);
    dpos.SetXYZ(fSigmaX, fSigmaY, 0.);

    // Create new hit and hitMatch
    Int_t nHits = hitArray.GetEntriesFast();
    new (hitArray[nHits])   CbmMvdHit(statNr, pos, dpos, 0);
    new (matchArray[nHits]) CbmMvdHitMatch(iFile, iEvent, trackId, 
					   iPoint, 0); 
    mCreated++;

  }        // Loop over MCPoints    
  
  nCreated = mCreated;
  nLost    = mLost;

}
// -------------------------------------------------------------------------



// -----   Private method ProduceFakes   -----------------------------------
Int_t CbmMvdHitProducer::ProduceFakes() {

  Int_t nHits  = fHits->GetEntriesFast();
  Int_t nFakes = TMath::Nint( Double_t(nHits) * fFakeRate );
  Int_t nFake  = 0;
 
  TClonesArray& hitArray    = *fHits;
  TClonesArray& matchArray  = *fMatches;

  TVector3 pos, dPos;
  Double_t twoPi  = 2. * TMath::Pi();

  for (Int_t iFake=0; iFake<nFakes; iFake++) {

    // Select a random hit from the array
    Int_t index = fRandGen.Integer(nHits);
    CbmMvdHit* trueHit = (CbmMvdHit*) fHits->At(index);
    Int_t statNr = trueHit->GetStationNr();
    trueHit->PositionError(dPos);

    // Get inner and outer radius of station
    if ( fRadiusMap.find(statNr) == fRadiusMap.end() ) {
      cout << "-E- " << GetName() << "::ProduceFakes: "
	   << "MVD station " << statNr << " not in radii map!" << endl;
      continue;
    }
    Double_t rMin = fRadiusMap[statNr].first;
    Double_t rMax = fRadiusMap[statNr].second;
    
    // Generate position of fake hit (flat in radius and azimuth)
    Double_t r   = fRandGen.Uniform(rMin, rMax);
    Double_t phi = fRandGen.Uniform(0., twoPi);
    pos.SetXYZ(r*TMath::Cos(phi), r*TMath::Sin(phi), trueHit->GetZ());

    // Create new fake hit and hit match
    Int_t size = hitArray.GetEntriesFast();
    new (hitArray[size])   CbmMvdHit(statNr, pos, dPos, 0);
    new (matchArray[size]) CbmMvdHitMatch(-1, -1, -1, -1, 0);
    nFake++;

  }

  return nFake;
}
// -------------------------------------------------------------------------  



// -----   Private method MergeHits   --------------------------------------
Int_t CbmMvdHitProducer::MergeHits() {

  Int_t nHits = fHits->GetEntriesFast();

  if ( fMergeDist == 0.) return 0;
  if ( nHits <= 1 )      return 0;

  // Sort hits with respect to stations
  map<Int_t, vector<Int_t>*> hitMap;
  map<Int_t, Int_t>::iterator mapIt;
  for (mapIt=fStationMap.begin(); mapIt!=fStationMap.end(); mapIt++) {
    Int_t statNr = (*mapIt).second;
    vector<Int_t>* hitVect = new vector<Int_t>;
    hitMap[statNr] = hitVect;
  }
  for (Int_t iHit=0; iHit<nHits; iHit++) {
    CbmMvdHit* hit = (CbmMvdHit*) fHits->At(iHit);
    Int_t iStation = hit->GetStationNr();
    if ( hitMap.find(iStation) == hitMap.end() ) {
      cout << "-E- " << GetName() << "::MergeHits: "
	   << "Station " << iStation << " not in hit map!" << endl;
      continue;
    }
    hitMap[iStation]->push_back(iHit);
  }

  // Loop over stations
  for (mapIt=fStationMap.begin(); mapIt!=fStationMap.end(); mapIt++) {
    Int_t iStation = (*mapIt).second;
    vector<Int_t>* hitVect = hitMap[iStation];
    Int_t mHits = hitVect->size();

    // Double loop over hits in station
    for (Int_t j1=0; j1<mHits; j1++) {
      Int_t iHit1 = (*hitVect)[j1];
      CbmMvdHit* hit1 = (CbmMvdHit*) fHits->At(iHit1);
      Double_t x1 = hit1->GetX();
      Double_t y1 = hit1->GetY();
      for (Int_t j2=j1+1; j2<mHits; j2++) {
	Int_t iHit2 = (*hitVect)[j2];
	CbmMvdHit* hit2 = (CbmMvdHit*) fHits->At(iHit2);
	Double_t x2 = hit2->GetX();
	Double_t y2 = hit2->GetY();

	// Check for distance
	Double_t dist = TMath::Sqrt( (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) );
	if ( dist < fMergeDist ) {
	  CbmMvdHitMatch* match1 = (CbmMvdHitMatch*) fMatches->At(iHit1);
	  CbmMvdHitMatch* match2 = (CbmMvdHitMatch*) fMatches->At(iHit2);
	  match1->AddMerged();
	  match2->AddMerged();
	}

      }    // second hit loop
    }      // first hit loop
  }        // station loop

  // Determine number of merged hits
  Int_t nMerged = 0;
  for (Int_t iMatch=0; iMatch<fMatches->GetEntriesFast(); iMatch++) {
    CbmMvdHitMatch* match = (CbmMvdHitMatch*) fMatches->At(iMatch);
    if ( match->GetNMerged() ) nMerged++;
  }

  return nMerged;
}
// -------------------------------------------------------------------------


ClassImp(CbmMvdHitProducer);
