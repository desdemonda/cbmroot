#ifndef PAIRANALYSISEVENT_H
#define PAIRANALYSISEVENT_H

//#############################################################
//#                                                           #
//#      Class PairAnalysisEvent                                #
//#      Class for event structure and references to tracks   #
//#                                                           #
//#  Authors:                                                 #
//#   Julian    Book,     Uni Ffm / Julian.Book@cern.ch       #
//#                                                           #
//#############################################################
#include <TNamed.h>
#include <TClonesArray.h>
#include <TObjArray.h>

#include "CbmDetectorList.h"
#include "CbmVertex.h"

class FairRootManager;
class PairAnalysisTrack;

class PairAnalysisEvent : public TNamed {
public:
  PairAnalysisEvent();
  PairAnalysisEvent(const char*name, const char* title);

  PairAnalysisEvent(const PairAnalysisEvent& event);
  virtual ~PairAnalysisEvent();

  void SetInput(FairRootManager *man);
  void Init();


  // getters
  CbmVertex *GetPrimaryVertex()       const { return fPrimVertex; }
  Int_t      GetNumberOfTracks()      const { return (fGlobalTracks ? fGlobalTracks->GetEntriesFast() : -1); }
  Int_t      GetNumberOfMCTracks()    const { return (fMCTracks ? fMCTracks->GetEntriesFast() : -1); }
  Int_t      GetNumberOfVageMatches() const { return fMultiMatch; }
  Int_t      GetNumberOfMatches(DetectorId det) const;

  TClonesArray *GetHits(DetectorId det) const;

  TClonesArray *GetTrdHits()          const { return fTrdHits; }
  TClonesArray *GetTrdHitMatches()    const { return fTrdHitMatches; }
  TClonesArray *GetTrdPoints()        const { return fTrdPoints; }

  TObjArray    *GetTracks()           const { return fTracks; }
  PairAnalysisTrack *GetTrack(UInt_t pos);

  virtual void Clear(Option_t *opt="C");


private:
 TClonesArray *fMCTracks;       //mc tracks
 TClonesArray *fStsMatches;     //STS matches
 TClonesArray *fTrdMatches;     //TRD matches
 TClonesArray *fRichMatches;    //RICH matches

 TClonesArray *fGlobalTracks;   //global tracks
 TClonesArray *fTrdTracks;      //TRD tracks
 TClonesArray *fStsTracks;      //STS tracks
 TClonesArray *fRichRings;     //RICH rings

 TClonesArray *fStsHits;        //STS hits
 TClonesArray *fTrdHits;        //TRD hits
 TClonesArray *fRichHits;       //RICH hits

 TClonesArray *fTrdHitMatches;        //TRD hits
 TClonesArray *fTrdPoints;        //TRD points

 CbmVertex    *fPrimVertex;     //primary vertex
 TObjArray    *fTracks;         //papa tracks

 Int_t         fMultiMatch;     // number of mutiple matched tracks 


 ClassDef(PairAnalysisEvent,1)         // PairAnalysis Event
};



#endif
