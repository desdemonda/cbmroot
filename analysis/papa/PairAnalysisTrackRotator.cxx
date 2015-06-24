
///////////////////////////////////////////////////////////////////////////
//                PairAnalysis TrackRotator                              //
//                                                                       //
//                                                                       //
/*
Detailed description:
this class keeps only the configuration of the TrackRotator, the
actual track rotation is done in PairAnalysisPair::RotateTrack

*/
//                                                                       //
///////////////////////////////////////////////////////////////////////////

#include <TMath.h>
#include <TRandom3.h>

#include "PairAnalysisTrackRotator.h"

ClassImp(PairAnalysisTrackRotator)

PairAnalysisTrackRotator::PairAnalysisTrackRotator() :
  TNamed(),
  fIterations(1),
  fRotationType(kRotateBothRandom),
  fStartAnglePhi(TMath::Pi()),
  fConeAnglePhi(TMath::Pi()/6.)
{
  //
  // Default Constructor
  //
  gRandom->SetSeed();
}

//______________________________________________
PairAnalysisTrackRotator::PairAnalysisTrackRotator(const char* name, const char* title) :
  TNamed(name, title),
  fIterations(1),
  fRotationType(kRotateBothRandom),
  fStartAnglePhi(TMath::Pi()),
  fConeAnglePhi(TMath::Pi()/6.)
{
  //
  // Named Constructor
  //
  gRandom->SetSeed();
}

//______________________________________________
PairAnalysisTrackRotator::~PairAnalysisTrackRotator()
{
  //
  // Default Destructor
  //

}


