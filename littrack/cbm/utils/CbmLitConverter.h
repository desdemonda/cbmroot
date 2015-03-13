#ifndef CBMLITCONVERTER_H_
#define CBMLITCONVERTER_H_

#include "base/CbmLitTypes.h"
#include "base/CbmLitEnums.h"

class CbmLitPixelHit;
class CbmLitStripHit;
class CbmLitTrack;
class CbmLitTrackParam;
class CbmLitTofTrack;
class CbmTrack;
class CbmTofTrack;
class CbmBaseHit;
class CbmPixelHit;
class CbmStripHit;
class CbmHit;
class FairTrackParam;
class CbmStsTrack;
class CbmGlobalTrack;
class TClonesArray;

class CbmLitConverter
{
public:

   static void FairTrackParamToCbmLitTrackParam(
      const FairTrackParam* par,
      CbmLitTrackParam* litPar);

   static void CbmLitTrackParamToFairTrackParam(
      const CbmLitTrackParam* litPar,
      FairTrackParam* par);

   static void CbmPixelHitToCbmLitPixelHit(
      const CbmPixelHit* hit,
      Int_t index,
      CbmLitPixelHit* litHit);

   static void CbmStripHitToCbmLitStripHit(
      const CbmStripHit* hit,
      Int_t index,
      CbmLitStripHit* litHit);

   static void CbmHitToCbmLitPixelHit(
      const CbmHit* hit,
      Int_t index,
      CbmLitPixelHit* litHit,
      LitSystemId sysId);

   static void CbmStsTrackToCbmLitTrack(
      const CbmStsTrack* stsTrack,
      CbmLitTrack* litTrack);

   static void CbmTrackToCbmLitTrack(
      const CbmTrack* track,
      const HitPtrVector& lhits,
      CbmLitTrack* ltrack);

   static void CbmTrackArrayToCbmLitTrackArray(
      const TClonesArray* tracks,
      const HitPtrVector& lhits,
      TrackPtrVector& ltracks);

   static void CbmLitTrackToCbmTrack(
      const CbmLitTrack* litTrack,
      CbmTrack* track,
      LitSystemId systemId);

   static void LitTrackVectorToGlobalTrackArray(
      const TrackPtrVector& litTracks,
      const TofTrackPtrVector& littofTracks,
      TClonesArray* globalTracks,
      TClonesArray* stsTracks,
      TClonesArray* trdTracks,
      TClonesArray* muchTracks,
      TClonesArray* tofTracks);

   static void HitArrayToHitVector(
      const TClonesArray* hits,
      HitPtrVector& litHits);

   static void MvdHitArrayToHitVector(
      const TClonesArray* hits,
      HitPtrVector& litHits);

   static void StsTrackArrayToTrackVector(
      const TClonesArray* tracks,
      TrackPtrVector& litTracks);
};

#endif /*CBMLITCONVERTER_H_*/
