/** CbmMuchLayerSide.h
 *@author  M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@version 1.0
 *@since   11.02.08
 **
 ** This class holds the transport geometry parameters
 ** of one MuCh tracking layer side.
 **
 **/


#ifndef CBMMUCHLAYERSIDE_H
#define CBMMUCHLAYERSIDE_H 1

#include "TObjArray.h"
#include "Gtypes.h"

class CbmMuchModule;

class CbmMuchLayerSide : public TObject
{

 public:

  /** Default constructor **/
  CbmMuchLayerSide();

  /** Standard constructor
  *@param detId     Detector ID
  *@param z         z position of layer side center [cm]
  **/
  CbmMuchLayerSide(Int_t detId, Double_t z);

  /** Standard constructor
   *@param iStation  Station index within the MUCH system.
   *@param iLayer    Layer index within the station.
   *@param iSide     Defines side (0 - Front, 1 - Back) within the layer.
   *@param z         z position of layer side center [cm].
   **/
  CbmMuchLayerSide(Int_t iStation, Int_t iLayer, Bool_t iSide, Double_t z);

  /** Destructor **/
  virtual ~CbmMuchLayerSide();

  /** Accessors **/
  Int_t      GetDetectorId()  const { return fDetectorId; }
  Int_t      GetNModules()    const { return fModules.GetEntriesFast(); }
  TObjArray* GetModules()            { return &fModules; }
  Double_t   GetZ()                   { return fZ;}
  void      SetZ(Double_t z)         { fZ = z; }

  CbmMuchModule* GetModule(Int_t iModule) const { return (CbmMuchModule*)fModules.At(iModule); }

  /** Adds given CbmMuchModuleGem to the internal list.
   *@param module  CbmMuchModule which should be added to the array. **/
  void AddModule(CbmMuchModule* module);

  void DrawModules(Color_t color=kYellow,
                   Bool_t modulesVisible=true, Bool_t sectorsVisible=true);

 protected:
  Int_t   fDetectorId;                      // Unique detector ID
  Double32_t fZ;                            // z position of layer side center (midplane) [cm] in global cs
  TObjArray  fModules;                      // Array of CbmMuchModuleGem objects

  ClassDef(CbmMuchLayerSide,1);
};
#endif
