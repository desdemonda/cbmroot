/** CbmMuchFindVectors.h
 *@author A.Zinchenko <Alexander.Zinchenko@jinr.ru>
 *@since 2014
 **
 ** Task class for vector finding in MUCH.
 ** Input: TClonesArray of CbmMuchHit
 ** Output: TClonesArray of CbmMuchTrack
 **
 **/

#ifndef CBMMUCHFINDVECTORS_H_
#define CBMMUCHFINDVECTORS_H_ 1

#include "CbmMuchGeoScheme.h"
#include "FairTask.h"
#include <TDecompLU.h>
#include <map>
#include <set>
#include <vector>

class CbmMuchStrawHit;
class CbmMuchTrack;
class TClonesArray;

class CbmMuchFindVectors : public FairTask
{
public:
  /** Default constructor **/
  CbmMuchFindVectors();
  
  /** Standard constructor
   *@param name   Name of class
   *@param title  Task title
   **/
  CbmMuchFindVectors(
		     const char* name,
		     const char* title = "FairTask");
  
  /** Destructor **/
  virtual ~CbmMuchFindVectors();
  
  /** Initialisation **/
  virtual InitStatus Init();
  
  /** Task execution **/
  virtual void Exec(
		    Option_t* opt);
  
  /** Finish at the end **/
  virtual void Finish();
  
  /** SetParContainers **/
  virtual void SetParContainers();
  
  /** Accessors **/
  Int_t GetNofTracks()           { return fNofTracks; };
  Int_t GetNofStat() const       { return fgkStat; }
  
 private:
  // Some constants
  static const Int_t fgkStat = 2;               // Number of stations
  static const Int_t fgkPlanes = 8;            // Number of straw planes per station

 private:
  CbmMuchGeoScheme* fGeoScheme;                 // Geometry scheme
  TClonesArray* fTrackArray;                    // Output array of CbmMuchVectors
  Int_t fNofTracks;                             // Number of tracks created
  TClonesArray* fHits;                          // Input array of CbmMuchHit
  TClonesArray* fPoints;                        // Input array of CbmMuchHit
  TClonesArray* fDigiMatches;                   // Input array of CbmMuchHit
  Int_t fStatFirst;                             // First straw station No.
  std::multimap<Int_t,Int_t> fHitPl[fgkStat][fgkPlanes]; // hit indices on planes vs tube No
  std::vector<CbmMuchTrack*> fVectors[fgkStat]; // track vectors for stations
  std::vector<CbmMuchTrack*> fVectorsHigh[fgkStat]; // track vectors for stations (high resolution)
  Double_t fUz[fgkPlanes][3];                   // hit float data
  Double_t fUzi[fgkPlanes][3];                  // hit int data
  Double_t fDz[fgkPlanes];                      // geometrical constants (Z-distances from layer 0)
  Double_t fCosa[fgkPlanes];                    // geometrical constants (cos of stereo angles)
  Double_t fSina[fgkPlanes];                    // geometrical constants (sin of stereo angles)
  std::map<Int_t,TDecompLU*> fLus;              // system matrices (for different hit layer patterns)
  Double_t fErrU;                               // hit measurement error
  Double_t fDiam;                               // tube diameter
  Double_t fCutChi2;                            // Chi2-cut
  Int_t fMinHits;                               // Min. number of hits on track to do fit 
  Double_t fZ0[fgkStat];                        // Z-positions of the first layers
  Double_t fRmin[fgkStat];                      // inner radii of stations
  Double_t fRmax[fgkStat];                      // outer radii of stations
  Double_t fDtubes[fgkStat][fgkPlanes];         // max. tube difference between views

  std::vector<pair<Int_t,Int_t> > fHit2d[fgkStat][fgkPlanes/2]; // Indx1,Indx2 of doublet hits

  void ComputeMatrix();
  void GetHits();
  Bool_t SelectHitId(const CbmMuchStrawHit *hit);
  void MakeVectors();
  void ProcessDouble(Int_t ista, Int_t lay2, Int_t patt, Int_t flag, Int_t tube0, Int_t segment0);
  Bool_t IntersectViews(Int_t ista, Int_t curLay, Int_t indx1, Int_t indx2, Int_t patt);
  void AddVector(Int_t ista, Int_t patt, Double_t chi2, Double_t *pars, Bool_t lowRes = kTRUE); 
  void SetTrackId(CbmMuchTrack *vec);
  Bool_t SelDoubleId(Int_t indx1, Int_t indx2);
  void FindLine(Int_t patt, Double_t *pars);
  Double_t FindChi2(Int_t ista, Int_t patt, Double_t *pars);
  void CheckParams();
  void HighRes();
  void ProcessSingleHigh(Int_t ista, Int_t plane, Int_t patt, Int_t flag, Int_t nok, Double_t uu[fgkPlanes][2]); 
  void MoveVectors();
  void RemoveClones();
  void RemoveShorts();
  void StoreVectors();
  Int_t CountBits(Int_t x); 

  CbmMuchFindVectors(const CbmMuchFindVectors&);
  CbmMuchFindVectors& operator=(const CbmMuchFindVectors&);
  
  ClassDef(CbmMuchFindVectors,1)
};

#endif
