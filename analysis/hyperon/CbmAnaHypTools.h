#ifndef CBMANAHYPTOOLS_H
#define CBMANAHYPTOOLS_H

#include "TObject.h"
#include "CbmMCTrack.h"
#include <map>
#include <vector>
class CbmHit;
class CbmStsTrack;
class TClonesArray;
class CbmStsTrack;
class CbmKFTrack;
class CbmVertex;
class TVector3;
class TTree;
class CbmTrackMatch;

using std::vector;
using std::map;

class CbmAnaHypTools
{
public:
  CbmAnaHypTools()
    : fListMCTracks(NULL),
    fStsPointArray(NULL),
    fListRCTracks(NULL),
    fmArray(NULL),
    fsArray(NULL),
    fTrackMatch(NULL),
    fPV(NULL),
    pv(),
    fTree(NULL)
      {};

  ~CbmAnaHypTools(){};
  void Init();
  Int_t GetTrackId(CbmTrackMatch* match);
  Int_t GetTrackId(CbmHit* hit);
  void CopyTrack(CbmStsTrack* trackIn, CbmStsTrack* trackOut);
  void SetFlags();
  Double_t CalculateDca(CbmStsTrack* tr1, CbmStsTrack* tr2, CbmVertex vtx);
  Double_t CalculateCascadeDca (CbmStsTrack* la, CbmStsTrack* da, CbmVertex vtx);
  Int_t SignalPair(CbmStsTrack* tr1, CbmStsTrack* tr2);
  Int_t SignalCascade(CbmStsTrack* la, CbmStsTrack* da,
                Int_t motherPdg, Int_t sisterPdg);
  Double_t GetB(CbmStsTrack track);
  Int_t GetPidHypo(CbmStsTrack track);
  void SetFlagsOnly();
  Int_t IsL1accepted(Int_t trackId);

private:
  TClonesArray* fListMCTracks;
  TClonesArray* fStsPointArray;
  TClonesArray* fListRCTracks;
  TClonesArray* fmArray;
  TClonesArray* fsArray;
  TClonesArray* fTrackMatch;
  CbmVertex* fPV;
  TVector3 pv;
  TTree* fTree;

  CbmAnaHypTools(const CbmAnaHypTools&);
  CbmAnaHypTools operator=(const CbmAnaHypTools&);

};

class Lambda
{
 public:
  Lambda():laMC(NULL),piMC(NULL),prMC(NULL),piRC(NULL),prRC(NULL),piKF(NULL),prKF(NULL){}
  ~Lambda(){}
  CbmMCTrack* laMC;
  CbmMCTrack* piMC;
  CbmMCTrack* prMC;
  CbmStsTrack* piRC;
  CbmStsTrack* prRC;
  CbmKFTrack* piKF;
  CbmKFTrack* prKF;
 private:
  Lambda(const Lambda&);
  Lambda operator=(const Lambda&);
};

struct HitVector{
  vector<int> vMvd;
  vector<int> vSts;
  vector<int> vStationID;
  
HitVector() : vMvd(), vSts(), vStationID() {}
};

class Da{
 public:
  Da() : pdg(0), idMC(-1), idRC(-1), mc(NULL), rc(NULL), moId(-1), nPts(0),
    nHits(0), hits(), isL1AC(0) {}
 Da(Int_t IDMC, CbmMCTrack* MC) : pdg(MC->GetPdgCode()), idMC(IDMC), idRC(0), 
    mc(MC), rc(NULL), moId(MC->GetMotherId()), nPts(MC->GetNPoints(kMUCH)),
    nHits(0), hits(), isL1AC(0) {}
  ~Da(){}
  Int_t pdg;
  Int_t idMC;
  Int_t idRC;
  CbmMCTrack* mc;
  CbmStsTrack* rc;
  Int_t moId;
  Int_t nPts;
  Int_t nHits;
  HitVector hits;
  Bool_t isL1AC;

 private:
  Da(const Da&);
  Da operator=(const Da&);
};


#endif
