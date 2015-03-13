#ifndef CBM_STS_CANDID_TRACKER_H
#define CBM_STS_CANDID_TRACKER_H

#include "FairTask.h"
#include "FairDetector.h"
#include "CbmStsHit.h"
#include "CbmStsJointTrack.h"
#include "TClonesArray.h"
#include "TRefArray.h"


class CbmStsCandidTracker : public FairTask {

    
protected:
  Int_t          fNHit;
  Int_t          fNTrack;

  TClonesArray  *fHits;          //! pointers to created hits
  TClonesArray  *fGeantTracks;   //! pointers to created Geant tracks
  TClonesArray  *fListSTSpts;    //! pointers to STS points
  TClonesArray  *fListMCtracks;  //! pointers to MC tracks

  TRefArray *ref;

  TString fOption;     //  options to choose particles
  Double_t fDx;        //  error for X coordinate
  Double_t fDy;        //  error for Y coordinate

  Bool_t fSmearingXY;  // switch for smearing coordinates

public:    

  virtual InitStatus Init();                    // 
  virtual void Exec(Option_t * option);   //
  virtual void Finish();                  // 

  void Reset();
  void Register();
   
  virtual ~CbmStsCandidTracker();
  
  CbmStsCandidTracker(const char *name, const char *title="CBM Task");
  CbmStsCandidTracker();
  CbmStsHit *AddHit();
  TRefArray *GetRef(){return ref;} 
  CbmStsJointTrack *AddTrack(Int_t mTrackNumber);

  void SetOption(Option_t *option=" ") {fOption = option;  fOption.ToLower();}
  void SetTrack(CbmStsHit *hit);
  void CheckTracks();
  void PrintTracks(Int_t n);
  void MakeFakes(Int_t nf, Int_t nstation);

  ClassDef(CbmStsCandidTracker,1) //CbmStsCandidTracker

};

#endif
