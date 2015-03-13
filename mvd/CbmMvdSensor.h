// -------------------------------------------------------------------------
// -----                      CbmMvdSensor header file                -----
// -----                  Created 02/12/08  by M. Deveaux             -----
// -------------------------------------------------------------------------


/** CbmMvdSensor.h
 *@author M.Deveaux <deveaux@physik.uni-frankfurt.de>
 **
 ** Auxiliary class for the CbmMvdDigitise class.
 ** It holds the parameters of one MVD station and an array of its MvdPoints.
 **/


#ifndef CBMMVDSENSOR_H
#define CBMMVDSENSOR_H 1


#include "FairPrimaryGenerator.h"
#include "FairEventHeader.h"
#include "FairRunSim.h"
#include "FairRunAna.h"

#include <map>
#include <vector>
#include "TNamed.h"
#include "TString.h"
#include "SensorDataSheets/CbmMvdSensorDataSheet.h"
#include "TGeoMatrix.h"
#include "TGeoVolume.h"
#include "TGeoBBox.h"
#include "TClonesArray.h"
#include "TObjArray.h"
#include "CbmMvdDetectorId.h"
#include "CbmMvdCluster.h"

#include "plugins/CbmMvdSensorPlugin.h"

using std::vector;

// data classes
class CbmMvdPoint;
class CbmMvdDigi;

class CbmMvdSensor : public TNamed, CbmMvdDetectorId
{

 public:

  /** Default constructor **/
  CbmMvdSensor();


  
  CbmMvdSensor(const char* name, CbmMvdSensorDataSheet* dataSheet, TString volName, TString nodeName,
	       Int_t stationNr, Int_t volumeId, Double_t sensorStartTime);


  /** Destructor **/
  virtual ~CbmMvdSensor();


  /** Accessors **/
  Int_t    GetStationNr()  const { return fStationNr; }
  Int_t    GetVolumeId()   const { return fVolumeId; }
  Int_t    GetDetectorID() const { return fDetectorID;};
  Int_t    GetSensorNr() const { return fSensorNr; }
  
  Double_t GetDX(){return fShape->GetDX();};        
  Double_t GetDY(){return fShape->GetDY();};        
  Double_t GetDZ(){return fShape->GetDZ();};
  Double_t GetX(){return fSensorPosition[0];}; // returns x position for pixle 0/0
  Double_t GetY(){return fSensorPosition[1];}; // returns y position for pixle 0/0
  Double_t GetZ(){return fSensorPosition[2];}; // returns z position
  Double_t GetIntegrationtime() {return fSensorData->GetIntegrationTime();};
  Double_t GetCurrentEventTime() {return fcurrentEventTime;};
  
  
  TString  GetNodeName() {return fNodeName;};
  CbmMvdSensorDataSheet* GetDataSheet(){return fSensorData;}
  void     SetDataSheet(CbmMvdSensorDataSheet* sheet){fSensorData = sheet;}
  void     SetMap(std::map<Int_t, Int_t> SensorMap) 	{fSensorMap = SensorMap;} // Get Sensor Map to identify every Sensor
  void     SendInput(CbmMvdPoint* point);
  void     SendInputDigi(CbmMvdDigi* digi);
  void     SendInputCluster(CbmMvdCluster* cluster);
  void     SetStation(Int_t StationNumber){fStationNr = StationNumber;}  


  /** Coordinate transformations **/
  void LocalToTop	(Double_t* local, Double_t* lab);
  void TopToLocal	(Double_t* lab, Double_t* local);
  void LocalToPixel	(Double_t* local, Int_t &pixelNumberX, Int_t &pixelNumberY);
  void PixelToLocal	(Int_t pixelNumberX, Int_t pixelNumberY, Double_t* local);
  void PixelToTop	(Int_t pixelNumberX, Int_t pixelNumberY, Double_t* lab);
  void TopToPixel       (Double_t* lab, Int_t &pixelNumberX, Int_t &pixelNumberY);
  Int_t GetFrameNumber  (Int_t pixelNumberY, Double_t absoluteTime);
  Int_t GetDigiPlugin () {return fDigiPlugin;};
  Int_t GetHitPlugin () {return fHitPlugin;};
  Int_t GetClusterPlugin() {return fClusterPlugin;}
  
  void SetAlignment(TGeoHMatrix* alignmentMatrix);
  TGeoHMatrix* GetAlignmentCorr(){return fAlignmentCorr;};
  

  /** Initialization tools **/
  Int_t ReadSensorGeometry(TString volName, TString nodeName);
  
  void Init();
  void ShowDebugHistos();
  /** Plugin Controll */
  
  void SetPluginList(TObjArray* pluginArray){fPluginArray=pluginArray;};
  
  void AddPlugin(CbmMvdSensorPlugin* plugin){fPluginArray->AddLast((TObject*)plugin);};
  
  CbmMvdSensorPlugin* GetPlugin(UInt_t nPlugin){ return (CbmMvdSensorPlugin*) fPluginArray->At(nPlugin);}
  
  void SetMisalignment(Float_t misalignment[3]){for(Int_t i = 0; i<3; i++) epsilon[i]=misalignment[i];}
  
  /** Output to screen **/
  virtual void Print(Option_t* opt="") const;

  /** Run control */
 
  
  void ExecChain(); //runs chain of plugins
  void Exec(UInt_t nPlugin); //runs individual plugin
  void ExecTo(UInt_t nPlugin);
  void ExecFrom(UInt_t nPlugin);
  void Finish();
  
  /** Data control */
  
  void ClearInputArray(){;}
//  void AddInputObject(TObject* dataObject);
  
  TClonesArray* GetOutputBuffer();
  TClonesArray* GetOutputArray(Int_t nPlugin);
  TClonesArray* GetOutputMatch();
  Int_t         GetOutputArrayLen(Int_t nPlugin);
 
 
  
 
 protected:

  Int_t        fStationNr;        // Station identifier
  Int_t	       fSensorNr; 	  // Sensor identifier
  Int_t        fVolumeId;         // MC unique volume ID
  Int_t        fDetectorID;       // unique Detector ID
  Int_t        fDigiPlugin;
  Int_t        fHitPlugin;
  Int_t        fDigiLen;
  Int_t        fClusterPlugin;
  TString      fVolName;	  // Name of the volume attached to this sensor
  TString      fNodeName;	  // full name of the volume (including path) of the volume
  TClonesArray* foutputDigis;
  TClonesArray* foutputCluster;
  TClonesArray* foutputDigiMatch;
  TClonesArray* foutputBuffer;
  TClonesArray* fcurrentPoints;
  Double_t     fcurrentEventTime;
  Float_t     epsilon[3];

 
  /** Position of the sensor */
  TGeoBBox*   fShape; 	  	   // volume corresponding to the sensor in the TGeoManager
  TGeoHMatrix* fMCMatrix; 	   // coordinate transformation from Master to MC-Position of this volume
  TGeoHMatrix* fRecoMatrix;         // The reconstructed position of the sensor
  TGeoHMatrix* fAlignmentCorr;      // Correction of the sensor position according to alignment
  Double_t    fTempCoordinate[3];  // Just a vector to buffer temporarily position data.
  Double_t    fSensorPosition[3]; 
  
  /** Technical data of the sensor */
  CbmMvdSensorDataSheet* fSensorData;
  std::map<Int_t, Int_t> fSensorMap;
  
  /** Plugins */
  TObjArray* fPluginArray;
  
  /** Status of the sensor */
  
  Double_t fSensorStartTime;	   // Defines the start time of the sensor
				   // as expressed according to CBM time stamps
				   // Unit: [s]
  
  Bool_t initialized;

private:
  CbmMvdSensor(const CbmMvdSensor&);
  CbmMvdSensor operator=(const CbmMvdSensor&);

  ClassDef(CbmMvdSensor,1);

};



#endif
