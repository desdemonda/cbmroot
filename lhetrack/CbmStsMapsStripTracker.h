#ifndef CBM_STS_MAPS_STRIP_TRACKER_H
#define CBM_STS_MAPS_STRIP_TRACKER_H

#include "FairTask.h"
#include "FairDetector.h"
#include "CbmStsHit.h"
#include "CbmStsJointTrack.h"
#include "TClonesArray.h"
#include "TRefArray.h"
#include "CbmGeoStsPar.h"

class CbmStsMapsStripTracker : public FairTask {

    
protected:
  Int_t          fNHit;
  Int_t          fNTrack;

  TClonesArray  *fHits;          //! pointers to created hits
  TClonesArray  *fGeantTracks;   //! pointers to created Geant tracks
  TClonesArray  *fListSTSpts;    //! pointers to STS points
  TClonesArray  *fListStripHits;  //! pointers to strip points
  TClonesArray  *fListHybridHits;  //! pointers to hybrid points
  TClonesArray  *fListMAPSpts;   //! pointers to MAPS points
  TClonesArray  *fListHitInfo;   //! pointers to hit info 
  TClonesArray  *fListMCtracks;  //! pointers to MC tracks

  TRefArray *ref;

  TString fOption;     //  options to choose particles
  Double_t fDx;        //  error for X coordinate
  Double_t fDy;        //  error for Y coordinate

  Bool_t fSmearingXY;  // switch for smearing coordinates

  /** Geo Parameters **/
  CbmGeoStsPar*  fGeoPar; //!

public:    

  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual InitStatus ReInit();  
  virtual void Exec(Option_t * option);   //
  virtual void Finish();                  // 
  virtual void FinishTasks();             // 

  void Reset();
  void Register();
   
  CbmStsMapsStripTracker(const char *name, const char *title="CBM Task");
  CbmStsMapsStripTracker();
  virtual ~CbmStsMapsStripTracker();

  CbmStsHit *AddHit();
  TRefArray *GetRef(){return ref;} 
  CbmStsJointTrack *AddTrack(Int_t mTrackNumber);

  Int_t GetStation(Int_t det);

  void GetMapsHits();
  void GetStripHits();
  void GetHybridHits();
  void SetOption(Option_t *option=" ") {fOption = option;  fOption.ToLower();}
  void SetTrack(CbmStsHit *hit);
  void CheckTracks();
  void PrintTracks(Int_t n);
  void GetStripPoints();

  ClassDef(CbmStsMapsStripTracker,1) //CbmStsMapsStripTracker

};

#endif
