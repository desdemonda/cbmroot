/** CbmMuchStation.h
 *@author  M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@version 1.0
 *@since   15.03.07
 **
 ** This class holds the transport geometry parameters
 ** of one MuCh tracking station.
 **
 **/


#ifndef CBMMUCHSTATION_H
#define CBMMUCHSTATION_H 1

#include "TObjArray.h"
#include "CbmMuchLayer.h"

class CbmMuchStation : public TObject{
 public:

  /** Default constructor **/
  CbmMuchStation();


  /** Standard constructor
  *@param iStation  Station index
  *@param z         z position of station center in global cs [cm]
  **/
  CbmMuchStation(Int_t iStation, Double_t z);


  /** Destructor **/
  virtual ~CbmMuchStation();

  /** Setters **/
  void SetRmin(Double_t rMin) { fRmin = rMin; }
  void SetRmax(Double_t rMax) { fRmax = rMax; }
  void SetModuleDesign(Bool_t on)      { fModuleDesign = on;    }
  void SetTubeRmin(Double_t rMin) { fTubeRmin = rMin; }
  void SetTubeRmax(Double_t rMax) { fTubeRmax = rMax; }
  void SetTubeZ(Double_t z)       { fTubeZ    = z;    }


  /** Accessors **/
  Int_t GetDetectorId()   const { return fDetectorId; }
  Double_t GetZ()            const { return fZ; }
  Int_t    GetNLayers()      const { return fLayers.GetEntriesFast(); }
  Double_t GetRmin()         const { return fRmin;         }
  Double_t GetRmax()         const { return fRmax;         }
  Double_t GetTubeRmin()     const { return fTubeRmin;     }
  Double_t GetTubeRmax()     const { return fTubeRmax;     }
  Double_t GetTubeZ()        const { return fTubeZ;        }
  Bool_t   IsModuleDesign()  const { return fModuleDesign; }

  CbmMuchLayer* GetLayer(Int_t iLayer) const { return (CbmMuchLayer*)fLayers.At(iLayer); }

  /** Add one sector to the array **/
  void AddLayer(CbmMuchLayer* layer);

  /*
   * Automatic calculation of the half-thickness of the layer as
   * a maximum distance to layer module edges
   */
  Double_t GetTubeDz() const;

 protected:
  Int_t      fDetectorId;       // Unique detector ID
  Double32_t fZ;                // z position of station center (midplane) [cm] in global cs
  TObjArray  fLayers;           // Array of CbmMuchLayers
  Double_t   fRmin;             // Minimum radius of the station acceptance [cm]
  Double_t   fRmax;             // Maximum radius of the station acceptance [cm]
  Bool_t     fModuleDesign;     // 1 - detailed module design, 0 - simple 1-module design
  Double_t   fTubeRmin;         // Station tube Rmin
  Double_t   fTubeRmax;         // Station tube Rmax
  Double_t   fTubeZ;            // Station tube z position relative to much cave
  ClassDef(CbmMuchStation,1);
};
#endif
