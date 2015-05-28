///////////////////////////////////////////////////////////////////////////
//                PairAnalysisEvent                                        //
//                                                                       //
//                                                                       //
/*
Add Detailed description


*/
//                                                                       //
///////////////////////////////////////////////////////////////////////////
#include <Rtypes.h>
#include <TObjArray.h>
#include <TArrayS.h>


#include "FairRootManager.h"
#include "CbmDetectorList.h"
#include "CbmVertex.h"
#include "CbmGlobalTrack.h"
#include "CbmStsTrack.h"
#include "CbmTrdTrack.h"
#include "CbmRichRing.h"
#include "CbmMCTrack.h"
#include "CbmTrackMatchNew.h"

#include "PairAnalysisTrack.h"
#include "PairAnalysisEvent.h"

ClassImp(PairAnalysisEvent)

PairAnalysisEvent::PairAnalysisEvent() :
  TNamed(),
  fMCTracks(0x0),       //mc tracks
  fStsMatches(0x0),     //STS matches
  fTrdMatches(0x0),     //TRD matches
  fRichMatches(0x0),     //RICH matches
  fGlobalTracks(0x0),   //global tracks
  fTrdTracks(0x0),      //TRD tracks
  fStsTracks(0x0),      //STS tracks
  fRichRings(0x0),      //RICH rings
  fStsHits(0x0),      //STS hits
  fTrdHits(0x0),      //TRD hits
  fRichHits(0x0),      //RICH hits
  fTrdHitMatches(0x0),      //TRD hits
  fTrdPoints(0x0),      //TRD points
  fPrimVertex(0x0),     //primary vertices
  fTracks(new TObjArray(1)), // array of papa tracks
  fMultiMatch(0)
{
  //
  // Default Constructor
  //
  fTracks->SetOwner(kTRUE);

}

//______________________________________________
PairAnalysisEvent::PairAnalysisEvent(const char* name, const char* title) :
  TNamed(name, title),
  fMCTracks(0x0),       //mc tracks
  fStsMatches(0x0),     //STS matches
  fTrdMatches(0x0),     //TRD matches
  fRichMatches(0x0),     //RICH matches
  fGlobalTracks(0x0),   //global tracks
  fTrdTracks(0x0),      //TRD tracks
  fStsTracks(0x0),      //STS tracks
  fRichRings(0x0),      //RICH rings
  fStsHits(0x0),      //STS hits
  fTrdHits(0x0),      //TRD hits
  fRichHits(0x0),      //RICH hits
  fTrdHitMatches(0x0),      //TRD hits
  fTrdPoints(0x0),      //TRD points
  fPrimVertex(0x0),     //primary vertices
  fTracks(new TObjArray(1)), // array of papa tracks
  fMultiMatch(0)
{
  //
  // Named Constructor
  //
  fTracks->SetOwner(kTRUE);
}

//______________________________________________
PairAnalysisEvent::PairAnalysisEvent(const PairAnalysisEvent& event) :
  TNamed(event.GetName(), event.GetTitle()),
  fMCTracks(0x0),
  fStsMatches(0x0),
  fTrdMatches(0x0),
  fRichMatches(0x0),
  fGlobalTracks(0x0),
  fTrdTracks(0x0),
  fStsTracks(0x0),
  fRichRings(0x0),
  fStsHits(event.GetHits(kSTS)),
  fTrdHits(event.GetHits(kTRD)),
  fRichHits(event.GetHits(kRICH)),
  fTrdHitMatches(event.GetTrdHitMatches()),
  fTrdPoints(event.GetTrdPoints()),
  fPrimVertex(event.GetPrimaryVertex()),
  fTracks(event.GetTracks()),
  fMultiMatch(event.GetNumberOfVageMatches())
{
  //
  // Named Constructor
  //
  fTracks->SetOwner(kTRUE);
}

//______________________________________________
PairAnalysisEvent::~PairAnalysisEvent()
{
  //
  // Default Destructor
  //
  fTracks->Clear("C");
  fGlobalTracks->Delete();   //global tracks
  fMCTracks->Delete();       //mc tracks

  fTrdTracks->Delete();      //TRD tracks
  fStsTracks->Delete();      //STS tracks
  fRichRings->Delete();      //RICH rings

  fStsMatches->Delete();     //STS matches
  fTrdMatches->Delete();     //STS matches
  fRichMatches->Delete();     //RICH matches

  fStsHits->Delete();      //STS hits
  fTrdHits->Delete();      //TRD hits
  fRichHits->Delete();      //RICH hits

  fTrdHitMatches->Delete();      //TRD hits
  fTrdPoints->Delete();      //TRD hits
}

//______________________________________________
void PairAnalysisEvent::SetInput(FairRootManager *man)
{
  //
  // setup the track/hit branches
  //
  fGlobalTracks = (TClonesArray*) man->GetObject("GlobalTrack");
  fTrdTracks    = (TClonesArray*) man->GetObject("TrdTrack");
  fStsTracks    = (TClonesArray*) man->GetObject("StsTrack");
  fRichRings    = (TClonesArray*) man->GetObject("RichRing");
  fPrimVertex   = (CbmVertex*)    man->GetObject("PrimaryVertex");
  // MC matches and tracks
  fMCTracks     = (TClonesArray*) man->GetObject("MCTrack");
  fStsMatches   = (TClonesArray*) man->GetObject("StsTrackMatch");
  fTrdMatches   = (TClonesArray*) man->GetObject("TrdTrackMatch");
  fRichMatches  = (TClonesArray*) man->GetObject("RichRingMatch");
  // hits
  fStsHits      = (TClonesArray*) man->GetObject("StsHit");
  fTrdHits      = (TClonesArray*) man->GetObject("TrdHit");
  fRichHits      = (TClonesArray*) man->GetObject("RichHit");
  fTrdHitMatches = (TClonesArray*) man->GetObject("TrdHitMatch");
  // mc points
  fTrdPoints    = (TClonesArray*) man->GetObject("TrdPoint");
  //  if(fMCTracks)   printf("PairAnalysisEvent::SetInput: size of mc array: %04d \n",fMCTracks->GetSize());
}

//______________________________________________
void PairAnalysisEvent::Init()
{
  //
  // initialization of track arrays
  //
  fTracks->Clear("C");
  if(!fGlobalTracks) return;

  TArrayS matches;
  if(fMCTracks) matches.Set(fMCTracks->GetEntriesFast());

  // loop over all glbl tracks
  for (Int_t i=0; i<fGlobalTracks->GetEntriesFast(); i++) {
    // global track
    CbmGlobalTrack *gtrk=static_cast<CbmGlobalTrack*>(fGlobalTracks->UncheckedAt(i));
    if(!gtrk) continue;

    Int_t itrd  = gtrk->GetTrdTrackIndex();
    Int_t ists  = gtrk->GetStsTrackIndex();
    Int_t irich = gtrk->GetRichRingIndex();
    // Int_t itof  = gtrk->GetTofHitIndex();

    // reconstructed tracks
    CbmTrdTrack *trdTrack=0x0;
    if(fTrdTracks && itrd>=0) trdTrack=static_cast<CbmTrdTrack*>(fTrdTracks->At(itrd));
    CbmStsTrack *stsTrack=0x0;
    if(fStsTracks && ists>=0) stsTrack=static_cast<CbmStsTrack*>(fStsTracks->At(ists));
    CbmRichRing *richRing=0x0;
    if(fRichRings && irich>=0) richRing=static_cast<CbmRichRing*>(fRichRings->At(irich));
    // track matches
    CbmTrackMatchNew *stsMatch = 0x0;
    if(stsTrack) stsMatch = static_cast<CbmTrackMatchNew*>( fStsMatches->At(ists) );
    ////    printf("%p %p %d \n",stsMatch,&stsMatch->GetMatchedLink(),stsMatch->GetMatchedIndex());
    //TODO: investigate why the hell there could be 0xffffffffffffffe0 for stsMatch->GetMatchedLink() for these the fMatchedIndex is -1
    //    Int_t istsMC = (stsMatch && (&stsMatch->GetMatchedLink()&0xffffffffffffffe0) ? stsMatch->GetMatchedLink().GetIndex() : -1 );
    Int_t istsMC = (stsMatch && /*stsMatch->GetMatchedIndex()>=0 ?*/ stsMatch->GetMatchedLink().GetIndex() /*: -1*/ );
    CbmTrackMatchNew *trdMatch = 0x0;
    if(trdTrack) trdMatch = static_cast<CbmTrackMatchNew*>( fTrdMatches->At(itrd) );
    Int_t itrdMC = (trdMatch ? trdMatch->GetMatchedLink().GetIndex() : -1 );
    CbmTrackMatchNew *richMatch = 0x0;
    if(richRing) richMatch = static_cast<CbmTrackMatchNew*>( fRichMatches->At(irich) );
    Int_t irichMC = (richMatch ? richMatch->GetMatchedLink().GetIndex() : -1 );

    // monte carlo track based on the STS match!!!
    Int_t iMC = istsMC;
    CbmMCTrack *mcTrack=0x0;
    if(fMCTracks && iMC>=0) mcTrack=static_cast<CbmMCTrack*>(fMCTracks->At(iMC));

    // increment position in matching array
    if(mcTrack && fMCTracks) matches[istsMC]++;
    // build papa track
    fTracks->AddAtAndExpand(new PairAnalysisTrack(gtrk, stsTrack,trdTrack,richRing, mcTrack, stsMatch,trdMatch,richMatch),
			    i);

    // set MC label and matching bits
    if(iMC>=0) {
      PairAnalysisTrack *tr = static_cast<PairAnalysisTrack*>(fTracks->UncheckedAt(i));
      tr->SetLabel(iMC);
      tr->SetBit(BIT(14+kRICH), (iMC==irichMC) );
      tr->SetBit(BIT(14+kTRD),  (iMC==itrdMC)  );
      tr->SetBit(BIT(14+kSTS),  (iMC==istsMC)  );
    }

  }

  // number of multiple matched tracks
  for(Int_t i=0; i<matches.GetSize(); i++)
    fMultiMatch += (matches[i] > 1 ? matches[i] : 0);

}


PairAnalysisTrack *PairAnalysisEvent::GetTrack(UInt_t pos)
{
  //
  // intialize the papa track and return it
  //

  // check intitialisation
  if(fTracks->GetSize()<=pos || !fTracks->At(pos))
    Fatal("PairAnalysisEvent::GetTrack","Event initialisation failed somehow !!!");

  return static_cast<PairAnalysisTrack*>(fTracks->At(pos));

}

//______________________________________________
Int_t PairAnalysisEvent::GetNumberOfMatches(DetectorId det) const
{
  //
  // number of track matches
  //
  switch(det) {
  case kSTS:  return (fStsMatches  ? fStsMatches->GetEntriesFast()  : 0);
  case kTRD:  return (fTrdMatches  ? fTrdMatches->GetEntriesFast()  : 0);
  case kRICH: return (fRichMatches ? fRichMatches->GetEntriesFast() : 0);
  default:   return 0;
  }

}

//______________________________________________
TClonesArray *PairAnalysisEvent::GetHits(DetectorId det) const {
  //
  // get hits array for certain detector
  //
  switch(det) {
  case kSTS: return fStsHits;
  case kTRD: return fTrdHits;
  case kRICH:return fRichHits;
  default:   return 0x0;
  }

}

//______________________________________________
void PairAnalysisEvent::Clear(Option_t *opt)
{
  //
  // clear arrays
  //
  fTracks->Clear("C");
  // fGlobalTracks->Delete();   //global tracks
  // fTrdTracks->Delete();      //TRD tracks
  // fStsTracks->Delete();      //STS tracks
  // fMCTracks->Delete();       //mc tracks
  // fStsMatches->Delete();     //STS matches

}
