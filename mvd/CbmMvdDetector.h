// -------------------------------------------------------------------------
// -----                      CbmMvdDetector header file              -----
// -----                  Created 02/12/08  by M. Deveaux             -----
// -------------------------------------------------------------------------


/** CbmMvdDetector.h
 *@author M.Deveaux <deveaux@physik.uni-frankfurt.de>
 **
 ** Singleton holding information on all sensors of the MVD.
 ** User interface to the MVD-Software
 **
 **/


#ifndef CBMMVDDETECTOR_H
#define CBMMVDDETECTOR_H 1



#include <vector>
#include "TNamed.h"
#include "TString.h"
#include "TGeoMatrix.h"
#include "TGeoVolume.h"
#include "TGeoBBox.h"
#include "TClonesArray.h"

#include "CbmMvdSensor.h"
#include "CbmMvdStationPar.h"

#include "plugins/CbmMvdSensorPlugin.h"
   #include "plugins/tasks/CbmMvdSensorTask.h"
      #include "plugins/tasks/CbmMvdSensorDigitizerTask.h"
      #include "plugins/tasks/CbmMvdSensorFindHitTask.h"
      #include "plugins/tasks/CbmMvdSensorClusterfinderTask.h"
      #include "plugins/tasks/CbmMvdSensorHitfinderTask.h"
   #include "plugins/buffers/CbmMvdSensorBuffer.h"
      #include "plugins/buffers/CbmMvdSensorFrameBuffer.h"
      #include "plugins/buffers/CbmMvdSensorTrackingBuffer.h"

using std::vector;

// data classes
class CbmMvdPoint;
class CbmMvdSensorDataSheet;



class CbmMvdDetector : public TNamed
{

 public:

   CbmMvdDetector();  
   CbmMvdDetector(const char* name);
   
   /**
   * static instance
   */
    static CbmMvdDetector* Instance();


  /** Destructor **/
  virtual ~CbmMvdDetector();

  /** Data interface */
  void SendInput(TClonesArray* input);
  void SendInputDigis(TClonesArray* digis);
  void SendInputCluster(TClonesArray* cluster);
  void SendClonesArray(TClonesArray* addedStructures, Int_t dataLevel=0){;}

  TClonesArray* GetOuput(){ return 0;}
  TClonesArray* GetOutputHits();
  TClonesArray* GetOutputDigis();
  TClonesArray* GetOutputDigiMatchs();
  TClonesArray* GetOutputCluster();
  TClonesArray* GetOutputArray(Int_t nPlugin);
  TClonesArray* GetClonesArray(Int_t dataLevel){ return 0;}
  Int_t GetSensorArraySize(){return(fSensorArray->GetEntriesFast());} 
  UInt_t GetPluginArraySize(){return fPluginCount-1;} 
  
  
  TClonesArray* GetCurrentEvent	();
  /** Initialisation */
  
  void AddSensor(TString clearName, TString fullName, TString nodeName, 
		 CbmMvdSensorDataSheet* sensorData, Int_t sensorNr, Int_t volumeId, Double_t sensorStartTime, Int_t stationNr);
  void AddPlugin(CbmMvdSensorPlugin* plugin);
  void BuildDebugHistograms(){;};
  void Init();
  void SetMisalignment(Float_t misalignment[3]){for(Int_t i = 0; i<3; i++) fepsilon[i] = misalignment[i];}; 
  void SetParameterFile(CbmMvdStationPar* parameter){fParameter = parameter;};
  void ShowDebugHistos();
  /** Data Processing */
  
  void ExecChain(); //Processes the full execution chain
  void Exec(UInt_t nLevel); //Processes Element nLevel of the chain
  void ExecTo(UInt_t nLevel){;}; // Processes Elements to a given Level of Plugins
  void ExecFrom(UInt_t nLevel); //Preocesses Elements from a given level till the end
  
  /** Finish */
  
  void Finish();
  void StoreDebugHistograms(TString fileName){;}
  
  /** Accessors */
  CbmMvdStationPar* GetParameterFile(){return fParameter;};  
  CbmMvdSensor* GetSensor(UInt_t nSensor){return (CbmMvdSensor*)fSensorArray->At(nSensor);};
  void PrintParameter(){fParameter->Print();};

private:
  TClonesArray* fSensorArray;
  TClonesArray* fSensorIDArray; //Array of Sensor ID
  UInt_t fPluginCount;
  TClonesArray* foutput;
  TClonesArray* foutputHits;
  TClonesArray* foutputDigis;
  TClonesArray* foutputCluster;
  TClonesArray* foutputDigiMatchs;
  TClonesArray* foutputHitMatchs;
  TClonesArray* fcurrentEvent;
  Float_t fepsilon[3];
 /** Data members */
  
  Int_t fDigiPlugin; 
  Int_t fHitPlugin;
  Int_t fClusterPlugin;
  
  
  static CbmMvdDetector* fInstance;
  Bool_t fSensorArrayFilled;
  Bool_t initialized;
  Bool_t fFinished;


  CbmMvdStationPar* fParameter;


  CbmMvdDetector(const CbmMvdDetector&);
  CbmMvdDetector operator=(const CbmMvdDetector&);
  
 ClassDef(CbmMvdDetector,1);
};



#endif
