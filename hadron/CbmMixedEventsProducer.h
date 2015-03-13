// ------------------------------------------------------------------
// -----                  CbmMixedEventsProducer                -----
// -----             Created 2004-Sep-23 by D.Kresan            -----
// ------------------------------------------------------------------
#ifndef CBM_MIXED_EVENTS_PRODUCER
#define CBM_MIXED_EVENTS_PRODUCER

#include <map>
using namespace std;

#include "FairTask.h"

class TObjArray;
class TClonesArray;
class TH2F;
class TRandom;


class CbmMixedEventsProducer : public FairTask {

private:
    Int_t             fLevel;                // Level
    UInt_t            fSeed;                 // Initial seed for random gen.
    TRandom          *fRnd;                  // Random number generator
    TObjArray        *fTrackPool;            // Track pool
    TObjArray        *fTofPointPool;         // TOF point pool
    TObjArray        *fHadronPool;           // Hadron pool

    TClonesArray     *fInputArrayMCTrack;    // MC tracks
    TClonesArray     *fInputArrayTofPoint;   // TOF points
    TClonesArray     *fInputArrayHadron;     // Hadrons
    TClonesArray     *fOutputArrayMCTrack;   // MC tracks - output
    TClonesArray     *fOutputArrayTofPoint;  // TOF points - output
    TClonesArray     *fOutputArrayHadron;    // Hadrons - output

    map<Int_t, Int_t> fMapTracks;            // Map from event # to the number of tracks
    map<Int_t, Int_t> fMapTofPoint[1000];    // Array of maps from track number to tof point index
    map<Int_t, Int_t> fMapHadrons;
    Int_t             fEvents;               // Number of events processed

    CbmMixedEventsProducer(const CbmMixedEventsProducer&);
    CbmMixedEventsProducer operator=(const CbmMixedEventsProducer&);

public:
    CbmMixedEventsProducer();
    CbmMixedEventsProducer(const char *name, Int_t verbose = 1);
    virtual ~CbmMixedEventsProducer();

    virtual InitStatus Init();
    virtual void Exec(Option_t *option = "") {}

    void CreateMixer();
    void Generate(Int_t nev);
    void WriteOutput();

    inline Int_t  GetLevel() const      {  return fLevel;  }
    inline UInt_t GetSeed()  const      {  return fSeed;   }

    inline void SetLevel(Int_t level)   {  fLevel = level; }
    inline void SetSeed(UInt_t seed)    {  fSeed = seed;   }

    ClassDef(CbmMixedEventsProducer,1);
};


#endif



