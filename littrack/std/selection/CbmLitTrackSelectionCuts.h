/* CbmLitTrackSelectionCuts.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2011
 * @version 1.0
 *
 * Removes tracks based on the cuts.
 */
#ifndef CBMLITTRACKSELECTIONCUTS_H_
#define CBMLITTRACKSELECTIONCUTS_H_

#include "interface/CbmLitTrackSelection.h"

class CbmLitTrackSelectionCuts : public CbmLitTrackSelection
{
public:
   /* Constructor */
   CbmLitTrackSelectionCuts();

   /* Constructor */
   CbmLitTrackSelectionCuts(
      litfloat maxChiSq,
      int minLastPlaneId,
      int minNofHits,
      litfloat minMomentum);

   /* Destructor */
   virtual ~CbmLitTrackSelectionCuts();

   /* Derived from CbmLitTrackSelection */
   virtual LitStatus DoSelect(
      TrackPtrIterator itBegin,
      TrackPtrIterator itEnd);

   /* Derived from CbmLitTrackSelection */
   virtual LitStatus DoSelect(
      TrackPtrVector& tracks);

   /* Return chi square cut */
   litfloat GetMaxChiSq() const {return fMaxChiSq;}

   /* Returns last plane index cut */
   int GetMinLastPlaneId() const {return fMinLastPlaneId;}

   /* Returns number of hits cut */
   int GetMinNofHits() const {return fMinNofHits;}

   /* Returns momentum cut */
   litfloat GetMinMomentum() const {return fMinMomentum;}

   /* Sets chi square cut */
   void SetMaxChiSq(litfloat maxChiSq) {fMaxChiSq = maxChiSq;}

   /* Sets last plane index cut */
   void SetMinLastPlaneId(int minLastPlaneId) {fMinLastPlaneId = minLastPlaneId;}

   /* Sets number of hits cut */
   void SetMinNofHits(int minNofHits) {fMinNofHits = minNofHits;}

   /* Sets momentum cut */
   void SetMinMomentum(litfloat minMomentum) {fMinMomentum = minMomentum;}

private:
   /* Chi square cut */
   litfloat fMaxChiSq;
   /* Last plane index cut */
   int fMinLastPlaneId;
   /* Number of hits cut */
   int fMinNofHits;
   /* Momentum cut in [GeV/c] */
   litfloat fMinMomentum;
};

#endif /* CBMLITTRACKSELECTIONCUTS_H_ */
