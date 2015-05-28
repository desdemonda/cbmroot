//
// TObject bits are used to flag the MC matching between detector tracklets
// bits used are >14 and used according to CbmDetectorList.h -> DetectorId
//
//

//#include <TObjArray.h>

#include <TDatabasePDG.h>
#include <TLorentzVector.h>

#include "CbmDetectorList.h"

#include "CbmTrack.h"
#include "CbmGlobalTrack.h"
#include "CbmStsTrack.h"
#include "CbmTrdTrack.h"
#include "CbmRichRing.h"
#include "CbmMCTrack.h"
#include "CbmTrackMatchNew.h"

#include "PairAnalysisTrack.h"

ClassImp(PairAnalysisTrack)

PairAnalysisTrack::PairAnalysisTrack() :
  TNamed(),
  fGlblTrack(0x0),
  fStsTrack(0x0),
  fTrdTrack(0x0),
  fRichRing(0x0),
  fMCTrack(0x0),
  fStsTrackMatch(0x0),
  fTrdTrackMatch(0x0),
  fRichRingMatch(0x0),
  fMomentum(),
  fPosition(),
  fCharge(0),
  fPdgCode(-11),
  fLabel(-1),
  fWeight(1.),
  fMultiMatch(0)
{
  //
  // Default Constructor
  //

}

//______________________________________________
PairAnalysisTrack::PairAnalysisTrack(const char* name, const char* title) :
  TNamed(name, title),
  fGlblTrack(0x0),
  fStsTrack(0x0),
  fTrdTrack(0x0),
  fRichRing(0x0),
  fMCTrack(0x0),
  fStsTrackMatch(0x0),
  fTrdTrackMatch(0x0),
  fRichRingMatch(0x0),
  fMomentum(),
  fPosition(),
  fCharge(0),
  fPdgCode(0),
  fLabel(-1),
  fWeight(1.),
  fMultiMatch(0)
{
  //
  // Named Constructor
  //

}

//______________________________________________
PairAnalysisTrack::PairAnalysisTrack(CbmGlobalTrack *gtrk, 
				 CbmStsTrack *ststrk, 
				 CbmTrdTrack *trdtrk, 
				 CbmRichRing *richring, 
				 CbmMCTrack *mctrk,
				 CbmTrackMatchNew *stsmatch,
				 CbmTrackMatchNew *trdmatch,
				 CbmTrackMatchNew *richmatch
				 ) :
  TNamed(),
  fGlblTrack(gtrk),
  fStsTrack(ststrk),
  fTrdTrack(trdtrk),
  fRichRing(richring),
  fMCTrack(mctrk),
  fStsTrackMatch(stsmatch),
  fTrdTrackMatch(trdmatch),
  fRichRingMatch(richmatch),
  fMomentum(),
  fPosition(),
  fCharge(0),
  fPdgCode(0),
  fLabel(-1),
  fWeight(1.),
  fMultiMatch(0)
{
  //
  // Constructor
  //
  //TODO: check which track components should go into it, do fitting?
  TVector3 mom;
  ststrk->GetParamFirst()->Momentum(mom);
  fMomentum.SetVect(mom);
  Double_t mass = TDatabasePDG::Instance()->GetParticle(11)->Mass();
  fMomentum.SetT( TMath::Sqrt(mom.Mag2()+mass*mass) );
  TVector3 pos;
  ststrk->GetParamFirst()->Position(pos);
  fPosition.SetVect(pos);

  fCharge  = (ststrk->GetParamFirst()->GetQp()>0. ? +1. : -1. );
  if(mctrk) fPdgCode = mctrk->GetPdgCode(); 
}

//______________________________________________
PairAnalysisTrack::PairAnalysisTrack(const PairAnalysisTrack& track) :
  TNamed(track.GetName(), track.GetTitle()),
  fGlblTrack(track.GetGlobalTrack()),
  fStsTrack(track.GetStsTrack()),
  fTrdTrack(track.GetTrdTrack()),
  fRichRing(track.GetRichRing()),
  fMCTrack(track.GetMCTrack()),
  fStsTrackMatch(track.GetTrackMatch(kSTS)),
  fTrdTrackMatch(track.GetTrackMatch(kTRD)),
  fRichRingMatch(track.GetTrackMatch(kRICH)),
  fMomentum(), //TODO: copy
  fPosition(), //TODO: copy
  fCharge(track.Charge()),
  fPdgCode(track.PdgCode()),
  fLabel(track.GetLabel()),
  fWeight(track.GetWeight()),
  fMultiMatch(0)
{
  //
  // Copy Constructor
  //

}

//______________________________________________
PairAnalysisTrack::~PairAnalysisTrack()
{
  //
  // Default Destructor
  //

}

//______________________________________________
CbmTrackMatchNew* PairAnalysisTrack::GetTrackMatch(DetectorId det) const
{
  //
  // get track match depending on detector id
  //
  switch(det) {
  case kSTS:  return fStsTrackMatch;
  case kTRD:  return fTrdTrackMatch;
  case kRICH: return fRichRingMatch;
  default:   return 0x0;
  }

}

//______________________________________________
CbmTrack* PairAnalysisTrack::GetTrack(DetectorId det) const
{
  //
  // get track match depending on detector id
  //
  switch(det) {
  case kSTS:  return fStsTrack;
  case kTRD:  return fTrdTrack;
  case kRICH: return 0x0;
  default:   return 0x0;
  }

}
