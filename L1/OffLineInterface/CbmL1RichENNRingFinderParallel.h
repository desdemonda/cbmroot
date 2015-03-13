/*
 *====================================================================
 *
 *  CBM Level 1 Reconstruction 
 *  
 *  Authors: I.Kisel,  S.Gorbunov
 *
 *  e-mail : ikisel@kip.uni-heidelberg.de 
 *
 *====================================================================
 *
 *  Standalone RICH ring finder based on the Elastic Neural Net
 *
 *====================================================================
 */

#ifndef _CBM_L1_RICH_ENN_RING_FINDER2_H_
#define _CBM_L1_RICH_ENN_RING_FINDER2_H_


#include "CbmRichRingFinder.h"
#include "CbmL1Def.h"
//#include "../vectors/PSEUDO_F32vec4.h" // for check
//#include "../vectors/PSEUDO_F32vec1.h" // for check

#include "TString.h"
#include "TStopwatch.h"

#include <vector>

class ENNHit;
class ENNRing;


class CbmL1RichENNRingFinderParallel :public CbmRichRingFinder
{

  typedef unsigned short THitIndex;
  struct ENNHit {
    ENNHit():x(0),y(0),quality(0),localIndex(0){};
    virtual ~ENNHit(){};
    
    float x, y;             // coordinates
    int quality;            // quality of the best ring with this hit
    THitIndex localIndex;   // index in local copy of Clone array

    static bool Compare( const ENNHit &h1, const ENNHit &h2 ){
      return ( h1.x < h2.x );
    };
  };
  
  
  struct ENNRingHit: public ENNHit { /// hit to use in ENNRings
    ENNRingHit():outIndex(0){};
    
    THitIndex outIndex; // index in Clone Array
  };

  struct ENNHitV {
    ENNHitV():x(0),y(0),quality(0),localIndex(0){};
    virtual ~ENNHitV(){};
    
    fvec x, y;             // coordinates
    fvec quality;            // quality of the best ring with this hit
    fvec localIndex;   // index in local copy of Clone array
    void CopyHit(ENNHit& a, int i) {
      localIndex[i] = a.localIndex; x[i] = a.x; y[i] = a.y; quality[i] = a.quality;
    }
    void CopyHit(ENNHitV& a, int j, int i) {
      localIndex[i] = a.localIndex[j]; x[i] = a.x[j]; y[i] = a.y[j]; quality[i] = a.quality[j];
    };
  };

  struct ENNRingHitV: public ENNHitV {
    ENNRingHitV():outIndex(0){};
    
    fvec outIndex;   // index in local copy of Clone array

    void CopyHit(ENNRingHit& a, int i) {
      outIndex[i] = a.outIndex;  ENNHitV::CopyHit(a, i);
    }
    void CopyHit(ENNRingHitV& a, int j, int i) {
      outIndex[i] = a.outIndex[j]; ENNHitV::CopyHit(a, j, i);
    };
  };

  struct ENNSearchHitV: public ENNHitV { /// hit to use for ringfinding
    ENNSearchHitV():
      lx(0), ly(0), lr2(0),        // local coordinates
      S0(0), S1(0), S2(0), S3(0), S4(0), // coefficients for calculation of E
      C(0), Cx(0), Cy(0),          // coefficients for the parameter space
      on_ring(0)    
    {};

    // variables for local search:
    fvec lx, ly, lr2;        // local coordinates
    fvec S0, S1, S2, S3, S4; // coefficients for calculation of E
    fvec C, Cx, Cy;          // coefficients for the parameter space
    fvec on_ring;              // is the hit close to the current ring
  };


  
 
  struct ENNRing {
    ENNRing():
      on(0),                   // is the ring selected?
      x(0), y(0), r(0),            // parameters
      chi2(0),               // chi^2
      NHits(0),                 // number of ring hits
      NOwn(0),                  // number of its own hits
      skip(0),                 // skip the ring during selection
      localIHits()
    {};
    
    bool on;                   // is the ring selected?
    float x, y, r;            // parameters
    float chi2;               // chi^2
    // variables for the selection procedure:
    THitIndex NHits;                 // number of ring hits
    THitIndex NOwn;                  // number of its own hits
    bool skip;                 // skip the ring during selection
    std::vector<THitIndex> localIHits; // indexes of hits in local array

    static bool CompareENNHRings( const ENNRing &r1, const ENNRing &r2 ){
      if (r1.NHits != r2.NHits) return ( r1.NHits > r2.NHits );
      else return ( r1.chi2 < r2.chi2 );
    };
  };

  struct ENNRingV {
    ENNRingV():
      x(0), y(0), r(0),            // parameters
      chi2(0),               // chi^2
      NHits(0),                 // number of ring hits
      localIHits()
    {};
    
    fvec x, y, r;            // parameters
    fvec chi2;               // chi^2

    fvec NHits;                 // number of ring hits
    std::vector<fvec> localIHits; // indexes of hits in local array
  };
  

  void ENNRingFinder( const int NHits, nsL1vector<ENNHitV>::TSimd &HitsV, std::vector<ENNRing> &Rings,
		      float HitSize = 1., THitIndex MinRingHits = 5, 
		      fvec RMin = 2., fvec RMax = 6. );

 public:
  
  /** Standard constructor **/
  CbmL1RichENNRingFinderParallel( Int_t verbose = 0 );

   /** Destructor **/
  ~CbmL1RichENNRingFinderParallel();

  /** Initialisation **/
  void Init();

  /** Ring finding algorithm
   *
   *@param hitArray   Array of rich hits
   *@param projArray  Array of projected tracks (not needed here)
   *@param ringArray  Array of CbmRichRing
   **
   *@value Number of tracks created
   **/
  Int_t DoFind(TClonesArray* hitArray, TClonesArray* projArray,
	       TClonesArray* ringArray);

 private:

  Float_t fRecoTime;
  Int_t fNEvents;

  enum { NTimers = 11 };
  TStopwatch fTimers[NTimers];    // timers for different parts of algorithm
  TString fTimersNames[NTimers]; // names which are correspond to the timers.
  TStopwatch& GetTimer( TString t );

 public:
  
};

#endif
