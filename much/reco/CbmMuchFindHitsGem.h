/* CbmMuchFindHitsGem.h
 *
 *@author  E.Kryshen <e.kryshen@gsi.de>
 *@version 2.0
 *@since   31.07.12
 *
 *@author  M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@version 1.0
 *@since   11.02.08
 *
 *  Produces hits in GEM-like modules using different
 *  clustering algorithms. Clustering algorithms can be chosen
 *  by the switch SetAlgorithm(Int_t iAlgorithm)
 *  Several clustering algorithms are supported
 *  0 - one hit per fired pad
 *  1 - one hit per cluster
 *  2 - simple cluster deconvolution with charge threshold
 *  3 - local maxima finder
 */

#ifndef CBMMUCHFINDHITSGEM_H
#define CBMMUCHFINDHITSGEM_H 1
#include "FairTask.h"
#include "TString.h"
#include <map>
#include <vector>
#include "CbmMuchDigi.h"
using std::map;
using std::pair;
using std::vector;

class CbmMuchPad;
class CbmMuchCluster;
class TClonesArray;
class CbmMuchGeoScheme;
class CbmTimeSlice;

class CbmMuchFindHitsGem: public FairTask {
  public:
    CbmMuchFindHitsGem(const char* digiFileName);
    virtual ~CbmMuchFindHitsGem() {}
    virtual void Exec(Option_t* opt);
    void SetAlgorithm(Int_t iAlgorithm)             { fAlgorithm = iAlgorithm;}
    void SetThresholdRatio(Double_t thresholdRatio) {fThresholdRatio = thresholdRatio; }
    void SetClusterSeparationTime(Double_t time)    { fClusterSeparationTime = time; }
    void SetDaq(Bool_t daq)                         { fDaq = daq; }

  private:
    virtual InitStatus Init();
    void FindClusters();
    void CreateCluster(CbmMuchPad* pad);
    void ExecClusteringSimple(CbmMuchCluster* cluster,Int_t iCluster);
    void ExecClusteringPeaks(CbmMuchCluster* cluster,Int_t iCluster);
    void CreateHits(CbmMuchCluster* cluster,Int_t iCluster);
    TString  fDigiFile;                     // Digitization file
    Int_t    fAlgorithm;                    // Defines which algorithm to use
    Double_t fClusterSeparationTime;        // Minimum required time between two clusters
    Double_t fThresholdRatio;               // Charge threshold ratio relative to max cluster charge
    Int_t fEvent;                           // Event counter
    TClonesArray* fDigis;                   // Input array of CbmMuchDigi
    vector<Int_t> fClusterCharges;          //!
    vector<Bool_t> fLocalMax;               //!
    vector<CbmMuchPad*> fClusterPads;       //!
    vector<vector<Int_t> >fNeighbours;      //!
    
    TClonesArray* fClusters;                // Output array of CbmMuchCluster objects
    TClonesArray* fHits;                    // Output array of CbmMuchHit
    CbmMuchGeoScheme* fGeoScheme;           // Geometry scheme
    // auxiliary maps and vectors
    vector<Int_t> fDigiIndices;             //!
    vector<CbmMuchPad*> fFiredPads;         //!
    
    Bool_t fDaq;                            // To daq or not to daq // TODO
    CbmTimeSlice* fTimeSlice;               // Time slice object in the DAQ approach
    vector<CbmMuchDigi> fDigiData;          // Vector of digis
    CbmMuchFindHitsGem(const CbmMuchFindHitsGem&);
    CbmMuchFindHitsGem operator=(const CbmMuchFindHitsGem&);

    ClassDef(CbmMuchFindHitsGem,1);
};

#endif
