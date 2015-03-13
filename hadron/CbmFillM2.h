// ------------------------------------------------------------------
// -----                         CbmFillM2                      -----
// -----               Created 24-09-2004 by D.Kresan           -----
// ------------------------------------------------------------------
#ifndef CBM_FILL_M2_H
#define CBM_FILL_M2_H

#include "FairTask.h"

#include "TH2F.h"

class TClonesArray;
class TRandom;


class CbmFillM2 : public FairTask {

private:
    TRandom      *fRnd;              // Random number generator
    Int_t         fEvents;           // Number of processed events
    Int_t         fCoutBunch;        // Size of the output bunch
    Double_t      fTimeResolution;   // Time resolution
    Int_t         fLevel;            // Level: 1-MC, 2-RECO
    Int_t         fNoTrdStations;    // Number of TRD stations
    Int_t         fNoTrdPerStation;  // Number of layers per station

    TClonesArray *fArrayMCTrack;     // MC tracks
    TClonesArray *fArrayTofPoint;    // TOF points
    TClonesArray *fArrayHadron;      // Hadron tracks

    TH2F         *fh_m2mom_hadron_all;   // m2 vs. mom for all hadrons
    TH2F         *fh_m2mom_hadron;   // m2 vs. mom for prim. hadrons
    TH2F         *fh_m2mom_hadron_true;   // m2 vs. mom for hadrons
    TH2F         *fh_m2mom_hadron_10m;   // m2 vs. mom for hadrons

    Int_t         fNbinsM2;          // Number of bins in m2
    Double_t      fMinM2;            // Min m2
    Double_t      fMaxM2;            // Max m2
    Double_t      fBinSizeM2;        // Binsize in m2
    Int_t         fNbinsMom;         // Number of bins in momentum
    Double_t      fMinMom;           // Min momentum
    Double_t      fMaxMom;           // Max momentum
    Int_t         fNbinsMomCharge;   // Number of bins in mom*charge
    Double_t      fBinSizeMom;       // Binsize in momentum

    void CreateHistogramms();
    CbmFillM2(const CbmFillM2&);
    CbmFillM2 operator=(const CbmFillM2&);

public:
    CbmFillM2();
    CbmFillM2(const char *name, Int_t verbose=1,
	      Int_t nBinsM2=90,
	      Double_t minM2=-1.5, Double_t maxM2=3.0,
	      Int_t nBinsMom=9,
	      Double_t minMom=1., Double_t maxMom=10.);
    virtual ~CbmFillM2();

    virtual InitStatus Init();
    virtual void Exec(Option_t *option);
    virtual void Finish();
    void Reset();

    void WriteHistogramms();

    inline Double_t GetTimeResolution() const {  return fTimeResolution; }
    inline Int_t    GetLevel()          const {  return fLevel;          }
    inline Double_t GetMinMom()         const {  return fMinMom;         }
    inline Double_t GetMaxMom()         const {  return fMaxMom;         }
    inline Int_t    GetNbinsMom()       const {  return fNbinsMom;       }
    inline Double_t GetMinM2()          const {  return fMinM2;          }
    inline Double_t GetMaxM2()          const {  return fMaxM2;          }
    inline Int_t    GetNbinsM2()        const {  return fNbinsM2;        }

    inline void SetTimeResolution(Double_t timeResolution) {  fTimeResolution = timeResolution; }
    inline void SetCoutBunch(Int_t coutBunch)              {  fCoutBunch = coutBunch;           }
    inline void SetLevel    (Int_t level)                  {  fLevel = level;                   }

    ClassDef(CbmFillM2,1);
};

#endif

