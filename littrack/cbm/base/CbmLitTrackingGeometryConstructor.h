/**
 * \file CbmLitTrackingGeometryConstructor.h
 * \brief Tracking geometry constructor.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 **/

#ifndef CBMLITTRACKINGGEOMETRYCONSTRUCTOR_H_
#define CBMLITTRACKINGGEOMETRYCONSTRUCTOR_H_

#include "TObject.h"
#include "cbm/base/CbmLitDetectorSetup.h"
#include "parallel/LitDetectorLayout.h"
#include "parallel/LitMaterialGrid.h"

#include <string>

class TGeoManager;
class CbmTrdGeoHandler;
class TProfile2D;

class CbmLitTrackingGeometryConstructor
{
public:
   /**
    * \brief Return pointer to singleton object.
    * \return Pointer to singleton object.
    */
   static CbmLitTrackingGeometryConstructor* Instance();

   /**
    * \brief Return MUCH detector layout for parallel MUCH tracking in SIMD format.
    * \param[out] layout MUCH detector layout.
    */
   void GetMuchLayoutVec(lit::parallel::LitDetectorLayoutVec& layout);

   /**
    * \brief Return MUCH detector layout for parallel MUCH tracking in scalar format.
    * \param[out] layout MUCH detector layout.
    */
   void GetMuchLayoutScal(lit::parallel::LitDetectorLayoutScal& layout);

   /**
    * \brief Return MUCH detector layout for parallel MUCH tracking.
    * \param[out] layout MUCH detector layout.
    */
   template<class T> void GetMuchLayout(lit::parallel::LitDetectorLayout<T>& layout);

   /**
    * \brief Return TRD detector layout for TRD parallel tracking in SIMD format.
    * \param[out] layout TRD detector layout.
    */
   void GetTrdLayoutVec(lit::parallel::LitDetectorLayoutVec& layout);

   /**
    * \brief Return TRD detector layout for TRD parallel tracking in scalar format.
    * \param[out] layout TRD detector layout.
    */
   void GetTrdLayoutScal(lit::parallel::LitDetectorLayoutScal& layout);

   /**
    * \brief Return TRD detector layout for TRD parallel tracking.
    * \param[out] layout TRD detector layout.
    */
   template<class T> void GetTrdLayout(lit::parallel::LitDetectorLayout<T>& layout);

   void GetRichMaterial(
         lit::parallel::LitMaterialGrid* material);

   void ConvertTProfile2DToLitMaterialGrid(
         const TProfile2D* profile,
         lit::parallel::LitMaterialGrid* grid,
         Double_t maximumValue = 0);

   /**
    * \brief Return number of stations in TRD.
    * \return Number of stations in TRD.
    */
   Int_t GetNofTrdStations();

   /**
    * \brief Return number of stations in MUCH.
    * \return Number of stations in MUCH.
    */
   Int_t GetNofMuchStations();

   /**
    * \brief Return number of stations in MUCH + TRD.
    * \return Number of stations in MUCH + TRD.
    */
   Int_t GetNofMuchTrdStations();

   /**
    * \brief Return number of stations in MVD.
    * \return Number of stations in MVD.
    */
   Int_t GetNofMvdStations();

   /**
    * \brief Return number of stations in STS.
    * \return Number of stations in STS.
    */
   Int_t GetNofStsStations();

   /**
    * \brief Return number of MUCH absorbers.
    * \return Number of MUCH absorbers.
    */
   Int_t GetNofMuchAbsorbers();

   /**
    *
    */
//   Int_t ConvertTrdToAbsoluteStationNr(
//         Int_t station,
//         Int_t layer);

   /**
    *
    */
   Int_t ConvertMuchToAbsoluteStationNr(
         Int_t station,
         Int_t layer);

private:
   /**
    * \brief Constructor.
    * Constructor is protected since singleton pattern is used.
    * Pointer to object is returned by static Instance() method.
    */
   CbmLitTrackingGeometryConstructor();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitTrackingGeometryConstructor();

   /**
    * \brief Copy constructor.
    */
   CbmLitTrackingGeometryConstructor(const CbmLitTrackingGeometryConstructor&);

   /**
    * \brief Assignment operator.
    */
   const CbmLitTrackingGeometryConstructor& operator=(const CbmLitTrackingGeometryConstructor&);

   TGeoManager* fGeo; // Pointer to full geometry
   Int_t fNofTrdStations; // Number of TRD stations
   Int_t fNofMuchStations; // Number of MUCH stations
   Int_t fNofMvdStations; // Number of MVD stations
   Int_t fNofStsStations; // Number of STS stations
   Int_t fNofMuchAbsorbers; // Number of MUCH absorbers
   CbmLitDetectorSetup fDet; //
};

#endif /* CBMLITTRACKINGGEOMETRYCONSTRUCTOR_H_ */
