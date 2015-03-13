/**
 * \file CbmLitNearestHitTofMerger.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 * \brief Hit-to-track merging in TOF detector using nearest hit approach.
 **/

#ifndef CBMLITNEARESTHITTOFMERGER_H_
#define CBMLITNEARESTHITTOFMERGER_H_

#include "interface/CbmLitHitToTrackMerger.h"
#include "base/CbmLitPtrTypes.h"

class CbmLitTrackPropagator;
class CbmLitTrackUpdate;

class CbmLitNearestHitTofMerger : public CbmLitHitToTrackMerger
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitNearestHitTofMerger();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitNearestHitTofMerger();

   /**
    * \brief Inherited from CbmLitHitToTrackMerger */
   virtual LitStatus DoMerge(
      HitPtrVector& hits, //TODO: add const here
      TrackPtrVector& tracks,
      TofTrackPtrVector& tofTracks);

   /** Setters **/
   void SetFieldPropagator(TrackPropagatorPtr propagator) { fFieldPropagator = propagator; }
   void SetLinePropagator(TrackPropagatorPtr propagator) { fLinePropagator = propagator; }
   void SetFilter(TrackUpdatePtr filter) { fFilter = filter; }
   void SetPDG(Int_t pdg) { fPDG = pdg; }
   void SetChiSqCut(litfloat chiSqCut) { fChiSqCut = chiSqCut; }

private:
   TrackPropagatorPtr fFieldPropagator; // Field track propagation tool
   TrackPropagatorPtr fLinePropagator; // Line track propagation tool
   TrackUpdatePtr fFilter; // Track update tool
   Int_t fPDG; // PDG hypothesis
   litfloat fChiSqCut; // Chi square cut for hit to be attached to track.
};

#endif /* CBMLITNEARESTHITTOTRACKMERGER_H_ */
