/**
 * \file CbmMvdGeoHandler.h
 * \brief Helper class to extract information from the GeoManager. Addapted from TrdGeoHandler byFlorian Uhlig <f.uhlig@gsi.de>
 * \author Philipp Sitzmann <p.sitzmann@gsi.de>
 * \date 30/10/2014
 *
 */

#ifndef CbmMvdGeoHandler_H_
#define CbmMvdGeoHandler_H_ 1

#include <map>

#include "TObject.h"
#include "CbmMvdDetector.h"
#include "SensorDataSheets/CbmMvdMimosa26AHR.h"
#include "SensorDataSheets/CbmMvdMimosa34.h"
#include "CbmMvdStationPar.h"

using std::map;

class TGeoBBox;
class TGeoVolume;
class TGeoHMatrix;

class CbmMvdGeoHandler : public TObject
{
public:

   /**
   * \brief Constructor.
   **/
   CbmMvdGeoHandler();

   /**
   * \brief Destructor.
   **/
   ~CbmMvdGeoHandler();

   void Init(Bool_t isSimulation = kFALSE);

   /**
   * \brief Return module address calculated based on the current node in the TGeoManager.
   */
   Int_t GetSensorAddress();

   /**
   * \brief Navigate to node.
   */
   Int_t GetSensorAddress(const TString& path);
          
   Double_t GetSizeX(const TString& path);
   Double_t GetSizeY(const TString& path);
   Double_t GetSizeZ(const TString& path);
   Double_t GetX(const TString& path);
   Double_t GetY(const TString& path);
   Double_t GetZ(const TString& path);
   Int_t    GetStation(const TString& path);
   void     Fill();
   map<Int_t, Int_t> GetMap() {return fStationMap;};

private:

   void NavigateTo(const TString& sensor);
   void GetPipe();
   void GetGeometryTyp();
   void FillParameter();
   void FillDetector();
   void FillStationMap(); 


   CbmMvdDetector* fDetector;
   CbmMvdStationPar* fStationPar;

   map<Int_t, Int_t> fStationMap; 

   Bool_t fIsSimulation; //!

   UInt_t       fGeoPathHash;   //!
   TGeoVolume*  fCurrentVolume; //!
   TGeoBBox*    fVolumeShape;   //!
   Double_t     fGlobal[3];     //! Global center of volume
   TGeoHMatrix* fGlobalMatrix;  //!
   Int_t        fLayerId;       //!
   Int_t        fModuleId;      //!
   Int_t        fModuleType;    //!
   Int_t        fStation;       //! StationTypeID, 1..3
   TString      fMother;
   Int_t	fGeoTyp;
   Int_t	fVolId;
   Int_t        fStationNumber;

   Double_t     fWidth;
   Double_t     fHeight;
   Double_t     fRadLength;
   Double_t     fBeamwidth;
   Double_t     fBeamheight;
   Double_t     fThickness;
   Double_t     fXres;
   Double_t     fYres;

   TString	fStationName;
   TString	fDetectorName;
   TString	fSectorName;
   TString	fQuadrantName;
   TString	fSensorHolding;
   TString	fSensorName;
   TString	fnodeName;

   CbmMvdGeoHandler(const CbmMvdGeoHandler&);
   CbmMvdGeoHandler operator=(const CbmMvdGeoHandler&);

   ClassDef(CbmMvdGeoHandler, 4)
};

#endif //CbmMvdGeoHandler_H

