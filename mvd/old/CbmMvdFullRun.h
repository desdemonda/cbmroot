// ------------------------------------------------------------------------------
// -----                    CbmMvdFullRun header file                    -----
// -----                    Created by P.Sitzmann (2014)	                  -----
// -----                    							  -----
// ------------------------------------------------------------------------------


/** CbmMvdFullRun header file
 ** 
 **
 **/


#ifndef CBMMVDFULLRUN_H
#define CBMMVDFULLRUN_H 1

//#include "omp.h"
#include "FairTask.h"
#include "CbmMvdGeoPar.h"
#include "CbmMvdPoint.h"
#include "CbmMvdDigi.h"
#include "CbmMvdDigiMatch.h"
#include "SensorDataSheets/CbmMvdMimosa26AHR.h"
#include "CbmMvdDetector.h"
//#include "MyG4UniversalFluctuationForSi.h"

#include "TGeoManager.h"


#include "TRandom3.h"
#include "TStopwatch.h"
#include "TString.h"
#include "TMath.h"
#include "TH1.h"
#include "TH2.h"
#include "TRefArray.h"
#include "TObjArray.h"
#include <vector>
#include <list>
#include <map>
#include <utility>
#include "CbmMvdPixelCharge.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"




class TClonesArray;
class TRandom3;
class CbmMvdGeoPar;
class CbmMvdPileupManager;
class CbmMvdSensor;

using namespace std;


class CbmMvdFullRun : public FairTask
{
 
 public:

  /** Default constructor **/  
  CbmMvdFullRun();


  /** Standard constructor 
  *@param name  Task name
  *@param mode  0 = MAPS, 1 = Ideal
  **/
  CbmMvdFullRun(const char* name, 
		    Int_t mode = 0, Int_t iVerbose = 1);


  /** Destructor **/
  virtual ~CbmMvdFullRun();


  /** Task execution **/
  virtual void Exec(Option_t* opt);


  /** Accessors **/
  void SetWriteOutput(Bool_t bFlag)	{bWriteOutput = bFlag;};


//protected:
public:

    Bool_t bWriteOutput; // set to write Sensor output to file.
    // ----------   Protected data members  ------------------------------------

private:

  
  CbmMvdDetector* fDetector;
  
  /** Hit producer mode (0 = MAPS, 1 = Ideal) **/
  Int_t fMode;


  

  /** IO arrays **/

  TClonesArray* fInputPoints;    // Array of MCPoints (input)
  TClonesArray* fHits;     // Array of Points produced by included Plugins, e.g FrameBuffer, digitizer...
  TClonesArray* fDigis;
  TClonesArray* fdetectorOutput;
  
 

  /** Counters **/ 
  Int_t    fNEvents;
  Double_t fNPoints;
  Double_t fNReal;
  Double_t fNBg;
  Double_t fNFake;
  Double_t fNLost;
  Double_t fNMerged;
  Double_t fTime;
  
  // -----   Private methods   ---------------------------------------------

  

  /** Intialisation **/
  virtual InitStatus Init();


  /** Reinitialisation **/
  virtual InitStatus ReInit();


  /** Virtual method Finish **/
  virtual void Finish();


  /** Register the output arrays to the IOManager **/
  void Register();


  /** Clear the hit arrays **/
  void Reset();	 

  void GetMvdGeometry();


  /** Print digitisation parameters **/
  void PrintParameters();

private:
  CbmMvdFullRun(const CbmMvdFullRun&);
  CbmMvdFullRun operator=(const CbmMvdFullRun&);

  ClassDef(CbmMvdFullRun,1);

    
};
    
    
#endif   		     
