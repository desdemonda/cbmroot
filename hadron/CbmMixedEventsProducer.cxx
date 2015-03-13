// ------------------------------------------------------------------
// -----                  CbmMixedEventsProducer                -----
// -----             Created 2004-Sep-23 by D.Kresan            -----
// ------------------------------------------------------------------
#include <iostream>
#include <map>
using namespace std;

#include "TClonesArray.h"
#include "TH2F.h"
#include "TRandom.h"
#include "TMath.h"

#include "FairRootManager.h"
#include "CbmMCTrack.h"
#include "CbmTofPoint.h"
#include "CbmHadron.h"
#include "CbmMixedEventsProducer.h"

#define MPION 0.1395700
#define MKAON 0.493677
#define MPROT 0.939272


//___________________________________________________________________
//
// CbmMixedEventsProducer
//
// Task for event mixing. In each mixed event there is no two tracks
// coming from one real event. The total multiplicity is kept the sa-
// me. Each track is used only once.
//


// ------------------------------------------------------------------
CbmMixedEventsProducer::CbmMixedEventsProducer()
  : FairTask("MixedEventProducer"),
    fLevel(1),
    fSeed(0),
    fRnd(NULL),
    fTrackPool(new TObjArray()),
    fTofPointPool(new TObjArray()),
    fHadronPool(new TObjArray()),
    fInputArrayMCTrack(NULL),
    fInputArrayTofPoint(NULL),
    fInputArrayHadron(NULL),
    fOutputArrayMCTrack(new TClonesArray("CbmMCTrack")),
    fOutputArrayTofPoint(new TClonesArray("CbmTofPoint")),
    fOutputArrayHadron(new TClonesArray("CbmHadron")),
    fMapTracks(),
    fMapTofPoint(),
    fMapHadrons(),
    fEvents(0)
{
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
CbmMixedEventsProducer::CbmMixedEventsProducer(const char *name,
					       Int_t verbose)
  : FairTask(name, verbose),
    fLevel(1),
    fSeed(0),
    fRnd(NULL),
    fTrackPool(new TObjArray()),
    fTofPointPool(new TObjArray()),
    fHadronPool(new TObjArray()),
    fInputArrayMCTrack(NULL),
    fInputArrayTofPoint(NULL),
    fInputArrayHadron(NULL),
    fOutputArrayMCTrack(new TClonesArray("CbmMCTrack")),
    fOutputArrayTofPoint(new TClonesArray("CbmTofPoint")),
    fOutputArrayHadron(new TClonesArray("CbmHadron")),
    fMapTracks(),
    fMapTofPoint(),
    fMapHadrons(),
    fEvents(0)
{
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
CbmMixedEventsProducer::~CbmMixedEventsProducer()
{
    // Destructor
    delete fRnd;
    delete fTrackPool;
    delete fTofPointPool;
    delete fHadronPool;
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
InitStatus CbmMixedEventsProducer::Init()
{
    // Task initialisation
    FairRootManager* rootMgr = FairRootManager::Instance();
    if(NULL == rootMgr) {
	cout << "-E- CbmMixedEventsProducer::Init : "
	    << "ROOT manager is not instantiated!" << endl;
        return kERROR;
    }

    // Activate input branches
    if(1 == fLevel || 2 == fLevel) {
	fInputArrayMCTrack = (TClonesArray*) rootMgr->GetObject("MCTrack");
	if(NULL == fInputArrayMCTrack) {
	    cout << "-W- CbmMixedEventsProducer::Init : "
		<< "no MC track array!" << endl;
	}
	fInputArrayTofPoint = (TClonesArray*) rootMgr->GetObject("TOFPoint");
	if(NULL == fInputArrayTofPoint) {
	    cout << "-W- CbmMixedEventsProducer::Init : "
		<< "no TOF point array!" << endl;
	}
    } else {
	fInputArrayHadron = (TClonesArray*) rootMgr->GetObject("Hadron");
	if(NULL == fInputArrayHadron) {
	    cout << "-W- CbmMixedEventsProducer::Init : "
		<< "no Hadron array!" << endl;
	}
    }

    // Register the output
    if(1 == fLevel || 2 == fLevel) {
	rootMgr->Register("MCTrack", "Stack", fOutputArrayMCTrack, kTRUE);
	rootMgr->Register("TOFPoint", "Tof", fOutputArrayTofPoint, kTRUE);
    } else {
	rootMgr->Register("Hadron", "Hadrons", fOutputArrayHadron, kTRUE);
    }

    // Create random number generator
    fRnd = new TRandom(fSeed);

    cout << "-I- CbmMixedEventsProducer : "
	<< "initialisation completed." << endl;

    return kSUCCESS;
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
void CbmMixedEventsProducer::CreateMixer()
{
    // Copy MC tracks and TOF points from input arrays to the pools

    if(1 == fLevel) {
	if(NULL == fInputArrayMCTrack || NULL == fInputArrayTofPoint) return;

	CbmMCTrack* mcTrack;
	Int_t pdgCode;
	Int_t tofPointIndex;
	CbmTofPoint* tofPoint;
	CbmTofPoint* outTofPoint;
	Int_t trackID;
	TClonesArray* mcTracks;
	TClonesArray* tofPoints;
	Int_t nMCTrack;
	Int_t nTofPoint;
	Int_t nOutMCTrack;
	Int_t nOutTofPoint;

	// Map from track ID to TOF point index
	map<Int_t, Int_t> mapTofPoint;

	for(Int_t iev = 0; iev < FairRootManager::Instance()->GetInChain()->GetEntries(); iev++) {
	    FairRootManager::Instance()->GetInChain()->GetEntry(iev);

	    // Create containers for this event
	    mcTracks = new TClonesArray("CbmMCTrack");
	    tofPoints = new TClonesArray("CbmTofPoint");

	    // Get number of MC tracks and TOF points
	    nMCTrack = fInputArrayMCTrack->GetEntriesFast();
	    nTofPoint = fInputArrayTofPoint->GetEntriesFast();

	    nOutMCTrack = 0;
	    nOutTofPoint = 0;

	    // Loop over TOF points
	    Int_t temp = -3;
	    for(Int_t iTof = 0; iTof < nTofPoint; iTof++) {
		// Get pointer to the TOF point
		tofPoint = (CbmTofPoint*) fInputArrayTofPoint->At(iTof);
		if(NULL == tofPoint) continue;
		// Get track ID
		trackID = tofPoint->GetTrackID();
		if(trackID < 0) continue;
		if(trackID == temp) continue;
		temp = trackID;
		// Store in map
		mapTofPoint[trackID] = iTof;
	    }

	    // Loop over MC tracks
	    for(Int_t iTrack = 0; iTrack < nMCTrack; iTrack++) {
		// Get the pointer to the track
		mcTrack = (CbmMCTrack*) fInputArrayMCTrack->At(iTrack);
		if(NULL == mcTrack) continue;
		// Skip secondaries
		if(-1 != mcTrack->GetMotherId()) continue;
		// Process only hadrons
		pdgCode = mcTrack->GetPdgCode();
		if( (211 != TMath::Abs(pdgCode)) &&
		   (321 != TMath::Abs(pdgCode)) &&
		   (2212 != TMath::Abs(pdgCode)) ) {
		    continue;
		}
		// Copy this track to the track pool
		new ((*mcTracks)[nOutMCTrack]) CbmMCTrack(*mcTrack);
		// Look for corresponding TOF point
		if(mapTofPoint.find(iTrack) != mapTofPoint.end()) {
		    // Get pointer to the tofPoint
		    tofPointIndex = mapTofPoint[iTrack];
		    tofPoint = (CbmTofPoint*) fInputArrayTofPoint->At(tofPointIndex);
		    if(NULL == tofPoint) continue;
		    if(tofPoint->GetTrackID() != iTrack) {
			Fatal("CreateMixer", "Getter error");
		    }
		    new ((*tofPoints)[nOutTofPoint]) CbmTofPoint(*tofPoint);
		    outTofPoint = (CbmTofPoint*) tofPoints->At(nOutTofPoint);
		    outTofPoint->SetTrackID(nOutMCTrack);
		    (fMapTofPoint[iev])[nOutMCTrack] = nOutTofPoint;
		    nOutTofPoint += 1;
		}
		nOutMCTrack += 1;
	    }// Loop over MC tracks

	    fMapTracks[iev] = nOutMCTrack;

	    // Save containers in the event array
	    fTrackPool->Add(mcTracks);
	    fTofPointPool->Add(tofPoints);

	    if(0 == (iev % 100)) {
		cout << "-I- CbmMixedEventsProducer : "
		    << "event : " << iev
		    << ",  tracks copied : " << nOutMCTrack
		    << ",  TOF points copied : " << nOutTofPoint << "."
		    << endl;
	    }

	    mapTofPoint.clear();
	}
    } else if(2 == fLevel) {
	if(NULL == fInputArrayMCTrack || NULL == fInputArrayTofPoint) return;

	CbmMCTrack* mcTrack;
	Int_t pdgCode;
	Int_t tofPointIndex;
	CbmTofPoint* tofPoint;
	CbmTofPoint* outTofPoint;
	Int_t trackID;
	TClonesArray* mcTracks;
	TClonesArray* tofPoints;
	Int_t nMCTrack;
	Int_t nTofPoint;
	Int_t nOutMCTrack;
	Int_t nOutTofPoint;

	// Map from track ID to TOF point index
	map<Int_t, Int_t> mapTofPoint;

	for(Int_t iev = 0; iev < FairRootManager::Instance()->GetInChain()->GetEntries(); iev++) {
	    FairRootManager::Instance()->GetInChain()->GetEntry(iev);

	    // Create containers for this event
	    mcTracks = new TClonesArray("CbmMCTrack");
	    tofPoints = new TClonesArray("CbmTofPoint");

	    // Get number of MC tracks and TOF points
	    nMCTrack = fInputArrayMCTrack->GetEntriesFast();
	    nTofPoint = fInputArrayTofPoint->GetEntriesFast();

	    nOutMCTrack = 0;
	    nOutTofPoint = 0;

	    // Loop over TOF points
	    Int_t temp = -3;
	    for(Int_t iTof = 0; iTof < nTofPoint; iTof++) {
		// Get pointer to the TOF point
		tofPoint = (CbmTofPoint*) fInputArrayTofPoint->At(iTof);
		if(NULL == tofPoint) continue;
		// Get track ID
		trackID = tofPoint->GetTrackID();
		if(trackID < 0) continue;
		if(trackID == temp) continue;
		temp = trackID;
		// Store in map
		mapTofPoint[trackID] = iTof;
	    }

	    // Loop over MC tracks
	    for(Int_t iTrack = 0; iTrack < nMCTrack; iTrack++) {
		// Get the pointer to the track
		mcTrack = (CbmMCTrack*) fInputArrayMCTrack->At(iTrack);
		if(NULL == mcTrack) continue;
		// Skip secondaries
		if(-1 != mcTrack->GetMotherId()) continue;
		// Process only hadrons
		pdgCode = mcTrack->GetPdgCode();
		if( (211 != TMath::Abs(pdgCode)) &&
		   (321 != TMath::Abs(pdgCode)) &&
		   (2212 != TMath::Abs(pdgCode)) ) {
		    continue;
		}
		// Look for corresponding TOF point
		if(mapTofPoint.find(iTrack) != mapTofPoint.end()) {
		    // Copy this track to the track pool
		    new ((*mcTracks)[nOutMCTrack]) CbmMCTrack(*mcTrack);
		    // Get pointer to the tofPoint
		    tofPointIndex = mapTofPoint[iTrack];
		    tofPoint = (CbmTofPoint*) fInputArrayTofPoint->At(tofPointIndex);
		    if(NULL == tofPoint) continue;
		    if(tofPoint->GetTrackID() != iTrack) {
			Fatal("CreateMixer", "Getter error");
		    }
		    new ((*tofPoints)[nOutTofPoint]) CbmTofPoint(*tofPoint);
		    outTofPoint = (CbmTofPoint*) tofPoints->At(nOutTofPoint);
		    outTofPoint->SetTrackID(nOutMCTrack);
		    (fMapTofPoint[iev])[nOutMCTrack] = nOutTofPoint;
		    nOutMCTrack += 1;
		    nOutTofPoint += 1;
		}
	    }// Loop over MC tracks

	    fMapTracks[iev] = nOutMCTrack;

	    // Save containers in the event array
	    fTrackPool->Add(mcTracks);
	    fTofPointPool->Add(tofPoints);

	    if(0 == (iev % 100)) {
		cout << "-I- CbmMixedEventsProducer : "
		    << "event : " << iev
		    << ",  tracks copied : " << nOutMCTrack
		    << ",  TOF points copied : " << nOutTofPoint << "."
		    << endl;
	    }

	    mapTofPoint.clear();
	}
    } else {
	if(NULL == fInputArrayHadron) return;

	TClonesArray *hadrons;
	Int_t nHadron;
        Int_t nOutHadron;
	CbmHadron *hadron;

	for(Int_t iev = 0; iev < FairRootManager::Instance()->GetInChain()->GetEntries(); iev++) {
	    FairRootManager::Instance()->GetInChain()->GetEntry(iev);

	    hadrons = new TClonesArray("CbmHadron");
            nOutHadron = 0;

	    nHadron = fInputArrayHadron->GetEntriesFast();

	    for(Int_t iHadron = 0; iHadron < nHadron; iHadron++) {
		hadron = (CbmHadron*) fInputArrayHadron->At(iHadron);
		if(NULL == hadron) continue;

		new ((*hadrons)[nOutHadron]) CbmHadron(*hadron);
                nOutHadron += 1;
	    }

	    fHadronPool->Add(hadrons);
            fMapHadrons[iev] = nOutHadron;

	    if(0 == (iev % 100)) {
		cout << "-I- CbmMixedEventsProducer : "
		    << "event : " << iev
		    << ",  hadrons copied : " << nOutHadron << "."
		    << endl;
	    }
	}
    }
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
void CbmMixedEventsProducer::Generate(Int_t nev)
{
    if(1 == fLevel || 2 == fLevel) {
	// Generation of mixed events from the track-point pulls
	if(NULL == fInputArrayMCTrack || NULL == fInputArrayTofPoint) return;

	// map from the event number to boolean flag
	map<Int_t, Bool_t> mapEventUsed;
	// map from the event number to number of used tracks
	map<Int_t, Int_t> mapUsedTracks;
	// Array of maps from track numbers to boolean flag
	map<Int_t, Bool_t> mapTrackUsed[1000];
	for(Int_t i = 0; i < 1000; i++) {
	    mapEventUsed[i] = kFALSE;
	    mapUsedTracks[i] = 0;
	    for(Int_t j = 0; j < 1000; j++) {
		mapTrackUsed[i][j] = kFALSE;
	    }
	}

	TClonesArray *mcTracks = NULL;
	TClonesArray *tofPoints = NULL;
	CbmMCTrack *mcTrack = NULL;
	Int_t tofPointIndex = 0;
	CbmTofPoint *tofPoint = NULL;
	Int_t nMCTrack;
	Int_t rEvent;
	Int_t rTrack;
	Int_t nOutTofPoint;
	Int_t nFreeEvents;
	Int_t nOutEvent = 0;


	for(Int_t iev = 0; iev < nev; iev++) {
	    // Get number of MC tracks in this event
	    nMCTrack = fMapTracks[iev];
	    nOutTofPoint = 0;

	    if( nMCTrack >= fTrackPool->GetEntries() ) continue;

	    nFreeEvents = 0;
	    for(Int_t jEvent = 0; jEvent < fTrackPool->GetEntries(); jEvent++) {
		if(jEvent == iev) continue;
		if(mapUsedTracks[jEvent] < fMapTracks[jEvent])
		    nFreeEvents += 1;
	    }

	    if(nMCTrack > nFreeEvents) continue;

	    // Loop over MC tracks
	    for(Int_t iMCTrack = 0; iMCTrack < nMCTrack; iMCTrack++) {
		// Randomly select not used event
		rEvent = iev;
		while((rEvent == iev) ||
		      (mapEventUsed[rEvent]) ||
		      (mapUsedTracks[rEvent]>=fMapTracks[rEvent])) {
		    rEvent = Int_t(fRnd->Uniform(0., fTrackPool->GetEntries()));
		}
		// Randomly select not used track from this event
		rTrack = Int_t(fRnd->Uniform(0., fMapTracks[rEvent]));
		while(mapTrackUsed[rEvent][rTrack]) {
		    rTrack = Int_t(fRnd->Uniform(0., fMapTracks[rEvent]));
		}
		mapTrackUsed[rEvent][rTrack] = kTRUE;
		mapEventUsed[rEvent] = kTRUE;
		mapUsedTracks[rEvent] += 1;
		// Get the containers
		mcTracks = (TClonesArray*) fTrackPool->At(rEvent);
		tofPoints = (TClonesArray*) fTofPointPool->At(rEvent);
		// Get pointer to the MC track
		mcTrack = (CbmMCTrack*) mcTracks->At(rTrack);
		if(NULL == mcTrack) continue;
		// Copy the track to the output
		new ((*fOutputArrayMCTrack)[iMCTrack]) CbmMCTrack(*mcTrack);
		// Copy corresponding TOF point
		if(fMapTofPoint[rEvent].find(rTrack) != fMapTofPoint[rEvent].end()) {
		    tofPointIndex = (fMapTofPoint[rEvent])[rTrack];
		    tofPoint = (CbmTofPoint*) tofPoints->At(tofPointIndex);
		    if(NULL == tofPoint) continue;
		    if(tofPoint->GetTrackID() != rTrack) {
			cout << "Event: " << rEvent << ",  track: " << rTrack << ",  tofPoint: "
			    << tofPointIndex << ",  with trackID: " << tofPoint->GetTrackID() << endl;
			return;
		    }
		    new ((*fOutputArrayTofPoint)[nOutTofPoint]) CbmTofPoint(*tofPoint);
		    tofPoint = (CbmTofPoint*) fOutputArrayTofPoint->At(nOutTofPoint);
		    if(NULL == tofPoint) continue;
		    tofPoint->SetTrackID(iMCTrack);
		    nOutTofPoint += 1;
		}
	    } // Loop over MC tracks

	    mapEventUsed.clear();

	    // Fill output tree
	    FairRootManager::Instance()->Fill();

	    if(0 == (iev % 100)) {
		cout << "-I- CbmMixedEventsProducer : "
		    << "events: " << iev << ",  MC tracks: " << fOutputArrayMCTrack->GetEntriesFast()
		    << ",  TOF points: " << fOutputArrayTofPoint->GetEntriesFast()
		    << endl;
	    }

	    fOutputArrayMCTrack->Clear();
	    fOutputArrayTofPoint->Clear();

	    nOutEvent += 1;
	}// Loop over events

	cout << "-I- CbmMixedEventsProducer::Generate : "
	    << nOutEvent << " mixed events created." << endl;
    } else {
	if(NULL == fInputArrayHadron) return;

	// map from the event number to boolean flag
	map<Int_t, Bool_t> mapEventUsed;
	// map from the event number to number of used tracks
	map<Int_t, Int_t> mapUsedTracks;
	// Array of maps from track numbers to boolean flag
	map<Int_t, Bool_t> mapTrackUsed[1000];
	for(Int_t i = 0; i < 1000; i++) {
	    mapEventUsed[i] = kFALSE;
	    mapUsedTracks[i] = 0;
	    for(Int_t j = 0; j < 1000; j++) {
		mapTrackUsed[i][j] = kFALSE;
	    }
	}

	TClonesArray *hadrons = NULL;
	CbmHadron *hadron = NULL;
	Int_t nHadron;
	Int_t rEvent;
	Int_t rTrack;
	Int_t nFreeEvents;
	Int_t nOutEvent = 0;


	for(Int_t iev = 0; iev < nev; iev++) {
	    // Get number of hadrons in this event
	    nHadron = fMapHadrons[iev];

	    if( nHadron >= fHadronPool->GetEntries() ) continue;

	    nFreeEvents = 0;
	    for(Int_t jEvent = 0; jEvent < fHadronPool->GetEntries(); jEvent++) {
		if(jEvent == iev) continue;
		if(mapUsedTracks[jEvent] < fMapHadrons[jEvent])
		    nFreeEvents += 1;
	    }

	    if(nHadron > nFreeEvents) continue;

	    // Loop over MC tracks
	    for(Int_t iHadron = 0; iHadron < nHadron; iHadron++) {
		// Randomly select not used event
		rEvent = iev;
		while((rEvent == iev) ||
		      (mapEventUsed[rEvent]) ||
		      (mapUsedTracks[rEvent]>=fMapHadrons[rEvent])) {
		    rEvent = Int_t(fRnd->Uniform(0., fHadronPool->GetEntries()));
		}
		// Randomly select not used track from this event
		rTrack = Int_t(fRnd->Uniform(0., fMapHadrons[rEvent]));
		while(mapTrackUsed[rEvent][rTrack]) {
		    rTrack = Int_t(fRnd->Uniform(0., fMapHadrons[rEvent]));
		}
		mapTrackUsed[rEvent][rTrack] = kTRUE;
		mapEventUsed[rEvent] = kTRUE;
		mapUsedTracks[rEvent] += 1;
		// Get the containers
		hadrons = (TClonesArray*) fHadronPool->At(rEvent);
		// Get pointer to the hadron
		hadron = (CbmHadron*) hadrons->At(rTrack);
		if(NULL == hadron) continue;
		// Copy the track to the output
		new ((*fOutputArrayHadron)[iHadron]) CbmHadron(*hadron);
	    } // Loop over hadrons

	    mapEventUsed.clear();

	    // Fill output tree
	    FairRootManager::Instance()->Fill();

	    if(0 == (iev % 100)) {
		cout << "-I- CbmMixedEventsProducer : "
		    << "events: " << iev << ",  hadrons: " << fOutputArrayHadron->GetEntriesFast()
		    << endl;
	    }

	    fOutputArrayHadron->Clear();

	    nOutEvent += 1;
	}
	cout << "-I- CbmMixedEventsProducer::Generate : "
	    << nOutEvent << " mixed events created." << endl;
    }
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
void CbmMixedEventsProducer::WriteOutput()
{
    // Write the output to the file
    FairRootManager::Instance()->Write();
}
// ------------------------------------------------------------------


ClassImp(CbmMixedEventsProducer);


