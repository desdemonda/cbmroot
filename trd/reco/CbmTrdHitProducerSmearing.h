/**
 * \file CbmTrdHitProducerSmearing.h
 * \brief Smearing hit producer for TRD.
 * \author M.Kalisky <m.kalisky@gsi.de>
 * \date 2005
 *
 * Renamed from CbmTrdHitProducer 04.04.09 F. Uhlig
 * Cleaning and refactoring 07.09.12 A. Lebedev
 */

#ifndef CBMTRDHITPRODUCERSMEARING_H_
#define CBMTRDHITPRODUCERSMEARING_H_

#include "CbmTrdDigiPar.h"
#include "CbmTrdModule.h"
#include "FairTask.h"

#include <vector>

class TClonesArray;
class TVector3;
class CbmTrdHit;
class CbmTrdRadiator;

using std::vector;

/**
 * \class CbmTrdHitProducerSmearing
 * \brief Smearing hit producer for TRD.
 * \author M.Kalisky <m.kalisky@gsi.de>
 * \date 2005
 */
class CbmTrdHitProducerSmearing: public FairTask {
public:

   /**
    * \brief Standard constructor.
    * \param[in] radiator Pointer to TRD radiator class.
    */
   CbmTrdHitProducerSmearing(
         CbmTrdRadiator* radiator = NULL);

   /**
    * \brief Destructor.
    */
   virtual ~CbmTrdHitProducerSmearing();

   /**
    * \brief Inherited from FairTask.
    */
   virtual InitStatus Init();

   /**
    * \brief Inherited from FairTask.
    */
   virtual void SetParContainers();

   /**
    * \brief Inherited from FairTask.
    */
   virtual void Exec(Option_t * option);

   /**
    * \brief Inherited from FairTask.
    */
   virtual void Finish();

   /* Setters */
   void SetSigmaX(const vector<Double_t>& sigmaX) { fSigmaX = sigmaX; }
   void SetSigmaY(const vector<vector<Double_t> >& sigmaY) { fSigmaY = sigmaY; }
   void SetEfficency(Double_t eff) { fEfficency = eff; }
   void SetHitMergingDistance(Double_t dist) { fHitMergingDistance = dist; }
   void SetGhostRate(Double_t ghost) { fGhostRate = ghost; }
   void SetGhostDistance(Double_t ghostDistance) { fGhostDistance = ghostDistance; }
   void SetUseDigiPar(Bool_t useDigiPar) { fUseDigiPar = useDigiPar; }

private:

   /**
    * \brief Create and return CbmTrdHit for MC point with pointId.
    * \param[in] pointId Mc point index in TClonesArray.
    * \return CbmTrdHit for MC point with pointId.
    */
   CbmTrdHit* CreateHit(
         Int_t pointId);

   /**
    * \brief Create and return ghost hit for specified real hit.
    * \param[in] hit Pointer to the real hit.
    * \return Ghost hit for specified real hit.
    */
   CbmTrdHit* CreateGhostHit(
         const CbmTrdHit* hit);

   /**
    * \brief Return gauss error.
    * \param[in] sigma Sigma for gauss distribution.
    * \return Gauss error.
    */
   Double_t GetHitErr(
         Double_t sigma);

   /**
    * \brief Hit merging procedure based on minimum allowed distances between two hits.
    * \param[in,out] Array of hits for merging.
    */
   void MergeHits(
         vector<CbmTrdHit*>& hits);

   /**
    * \brief Return sigma error.
    * \param[in] layerId Index of the TRD layer.
    * \return Sigma error.
    */
   Double_t GetSigmaX(
         Int_t layerId) const;

   /**
    * \brief Return sigma error.
    * \param[in] teta Theta angle.
    * \param[in] layerId Index of the TRD layer.
    * \return Sigma error.
    */
   Double_t GetSigmaY(
         Double_t teta,
         Int_t layerId) const;

   CbmTrdHitProducerSmearing(const CbmTrdHitProducerSmearing&);
   CbmTrdHitProducerSmearing operator=(const CbmTrdHitProducerSmearing&);

   TClonesArray* fTrdPoints; //! CbmTrdPoint
   TClonesArray* fTrdHits; //! CbmTrdHit
   TClonesArray* fMCTracks; //! CbmMCTrack

   CbmTrdDigiPar* fDigiPar; //! TRD digitization parameters
   CbmTrdModule* fModuleInfo; //! TRD module information
   CbmTrdRadiator* fRadiator; //!

   vector<Double_t> fSigmaX; //! Errors for X coordinate
   vector<vector<Double_t> > fSigmaY; //! Errors for Y coordinate
   Double_t fEfficency; //! Hit finding efficiency (0-1)
   Double_t fHitMergingDistance; //! Minimum distance between hit which can be separated [cm]. corresponds to 2 pad width dimensions. if == 0 all hits can be separted
   Double_t fGhostRate; //! Rate to produce ghost hits per real hit
   Double_t fGhostDistance; //! average distance between real hit and ghost hit [cm]
   Bool_t fUseDigiPar; // True if digi parameters has to be used

ClassDef(CbmTrdHitProducerSmearing, 3)
};
#endif // CBMTRDHITPRODUCERSMEARING_H_
