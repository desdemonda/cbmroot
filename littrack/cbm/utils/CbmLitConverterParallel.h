/**
 * \file CbmLitConverterParallel.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 * \brief Class for conversion between CBMROOT data classes and littrack parallel data classes.
 */

#ifndef CBMLITCONVERTERPARALLEL_H_
#define CBMLITCONVERTERPARALLEL_H_

#include "parallel/LitTrackParam.h"
#include "parallel/LitScalPixelHit.h"
#include "parallel/LitScalTrack.h"

class FairTrackParam;
class TClonesArray;
class CbmPixelHit;
class CbmTrack;

class CbmLitConverterParallel {
public:
   /**
    * \brief Constructor.
    */
   CbmLitConverterParallel();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitConverterParallel();

   /**
    * \brief Convert FairTrackParam to LitTrackParamScal.
    * \param[in] par Pointer to FairTrackParam.
    * \param[out] lpar Pointer to LitTrackParamScal.
    */
   static void FairTrackParamToLitTrackParamScal(
      const FairTrackParam* par,
      lit::parallel::LitTrackParamScal* lpar);

   /**
    * \brief Convert LitTrackParamScal to FairTrackParam.
    * \param[in] lpar Pointer to LitTrackParamScal.
    * \param[out] par Pointer to FairTrackParam.
    */
   static void LitTrackParamScalToFairTrackParam(
      const lit::parallel::LitTrackParamScal* lpar,
      FairTrackParam* par);

   /**
    * \brief Convert CbmPixelHit to LitScalPixelHit.
    * \param[in] hit Pointer to CbmPixelHit.
    * \param[out] lhit Pointer to LitScalPixelHit.
    */
   static void CbmPixelHitToLitScalPixelHit(
         const CbmPixelHit* hit,
         lit::parallel::LitScalPixelHit* lhit);

   /**
    * \brief Convert CbmPixelHit array to LitScalPixelHit arra.
    * \param[in] hits Pointer to TClonesArray of CbmPixelHit.
    * \param[out] lhits Pointer to vector of LitScalPixelHit.
    */
   static void CbmPixelHitArrayToLitScalPixelHitArray(
         const TClonesArray* hits,
         vector<lit::parallel::LitScalPixelHit*>& lhits);

   /**
    * \brief Convert CbmTrack to LitScalTrack.
    * \param[in] track Pointer to CbmTrack.
    * \param[in] lhits Vector of LitScalPixelHit pointers.
    * \param[out] ltrack Pointer to LitScalTrack.
    */
   static void CbmTrackToLitScalTrack(
         const CbmTrack* track,
         const vector<lit::parallel::LitScalPixelHit*>& lhits,
         lit::parallel::LitScalTrack* ltrack);

   /**
    * \brief Convert array of CbmTrack to array of LitScalTrack.
    * \param[in] tracks TClonesArray of CbmTrack.
    * \param[in] lhits Vector of LitScalPixelHit pointers.
    * \param[out] ltracks Vector of LitScalTrack.
    */
   static void CbmTrackArrayToLitScalTrackArray(
         const TClonesArray* tracks,
         const vector<lit::parallel::LitScalPixelHit*>& lhits,
         vector<lit::parallel::LitScalTrack*>& ltracks);

   /**
    * \brief Convert LitScalTrack to CbmTrack.
    * \param[in] ltrack Pointer to LitScalTrack.
    * \param[out] track Pointer to CbmTrack.
    */
   static void LitScalTrackToCbmTrack(
         const lit::parallel::LitScalTrack* ltrack,
         CbmTrack* track);

   /**
    * \brief Convert array of LitScalTrack to array of CbmTrack.
    * \param[in] ltracks Vector of LitScalTrack.
    * \param[out] tracks TClonesArray of CbmTrack.
    */
   static void LitScalTrackArrayToCbmTrdTrackArray(
         const vector<lit::parallel::LitScalTrack*>& ltracks,
         TClonesArray* tracks);

   /**
    * \brief Convert array of LitScalTrack to array of CbmTrack.
    * \param[in] ltracks Vector of LitScalTrack.
    * \param[out] tracks TClonesArray of CbmTrack.
    */
   static void LitScalTrackArrayToCbmMuchTrackArray(
         const vector<lit::parallel::LitScalTrack*>& ltracks,
         TClonesArray* tracks);
};

#endif /* CBMLITCONVERTERPARALLEL_H_ */
