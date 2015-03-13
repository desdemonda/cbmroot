// ------------------------------------------------------------------------
// -----                  CbmMvdSensorDigitizerTask header file       -----
// -----                   Created 02/02/12  by M. Deveaux            -----
// ------------------------------------------------------------------------

/**  CbmMvdSensorDigitizerTask.h
 **  @author M.Deveaux <M.Deveaux@gsi.de>
 **  Acknowlegments to: C.Dritsa
 **
 **
 **/

#ifndef CBMMVDSENSORDIGITIZERTASK_H
#define CBMMVDSENSORDIGITIZERTASK_H 1


#include "FairTask.h"
#include "CbmMvdSensor.h"
//#include "omp.h"
#include "FairTask.h"
#include "CbmMvdPoint.h"
#include "CbmMvdDigi.h"
#include "CbmMatch.h"
#include "CbmMvdSensorTask.h"
#include "CbmMvdPileupManager.h"
#include "CbmMvdGeoPar.h"

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


class CbmMvdSensorDigitizerTask : public CbmMvdSensorTask
{

 public:

  /** Default constructor **/
  CbmMvdSensorDigitizerTask();
  CbmMvdSensorDigitizerTask(const char* name, Int_t iMode, Int_t iVerbose);

  /** Destructor **/
  virtual ~CbmMvdSensorDigitizerTask();
  
    /** Intialisation **/
  virtual void Init(CbmMvdSensor* mySensor);
  
  /** fill buffer **/
  void SetInputArray (TClonesArray* inputStream);
  void SetInput (CbmMvdPoint* point);
  
  /** Execute **/
  void Exec();
  void ExecChain();
  
  TClonesArray* GetOutputArray() {return fOutputBuffer;};
  TClonesArray* GetMatchArray() {return fDigiMatch;};
  TClonesArray* GetWriteArray() {return fDigis;};
  
  InitStatus ReadSensorInformation();
  void ProduceIonisationPoints(CbmMvdPoint* point);
  void ProducePixelCharge(CbmMvdPoint* point);
//  void PositionWithinCell(Double_t x, Double_t y,  Int_t & ix, Int_t & iy, Double_t & xCell, Double_t & yCell);

  /** Modifiers **/
  void SetSegmentLength(Double_t segmentLength)     { fSegmentLength = segmentLength;     }
  void SetDiffusionCoef(Double_t diffCoeff)         { fDiffusionCoefficient = diffCoeff;  }
  void SetElectronsPerKeV(Double_t electronsPerKeV) { fElectronsPerKeV = electronsPerKeV; }
  void SetWidthOfCluster(Double_t widthOfCluster)   { fWidthOfCluster = widthOfCluster;   }
  void SetCutOnDeltaRays(Double_t cutOnDeltaRays)   { fCutOnDeltaRays = cutOnDeltaRays;   }
  void SetChargeThreshold(Float_t chargeThreshold) { fChargeThreshold = chargeThreshold; }

  

//protected:
public:

    // ----------   Protected data members  ------------------------------------

    Int_t fcurrentFrameNumber;
  
    Double_t fEpiTh;
    Double_t fSegmentLength;

    Double_t fDiffusionCoefficient;
    Double_t fElectronsPerKeV;
    Double_t fWidthOfCluster;
    Double_t fPixelSizeX;
    Double_t fPixelSizeY;
    Double_t fCutOnDeltaRays;
    Float_t fChargeThreshold;
    Double_t fFanoSilicium;
   
    Double_t fEsum;
    Double_t fSegmentDepth;
    Double_t fCurrentTotalCharge;
    Double_t fCurrentParticleMass;
    Double_t fCurrentParticleMomentum;
    Int_t    fCurrentParticlePdg;

    TH1F* fRandomGeneratorTestHisto;
    TH2F* fPosXY;
    TH1F* fpZ;
    TH1F* fPosXinIOut;
    TH1F* fAngle;
    TH1F* fSegResolutionHistoX;
    TH1F* fSegResolutionHistoY;
    TH1F* fSegResolutionHistoZ;
    TH1F* fTotalChargeHisto;
    TH1F* fTotalSegmentChargeHisto;

    

    Double_t fLorentzY0;
    Double_t fLorentzXc; 
    Double_t fLorentzW;
    Double_t fLorentzA;
    Double_t fLorentzNorm;
  
    Double_t fLandauMPV;
    Double_t fLandauSigma;
    Double_t fLandauGain;
    TRandom3* fLandauRandom;

    Double_t fPixelSize;
    Double_t fPar0;
    Double_t fPar1;
    Double_t fPar2;

    Double_t fCompression;

    TH1F* fResolutionHistoX;
    TH1F* fResolutionHistoY;

    Int_t fNumberOfSegments;
    Int_t fCurrentLayer;
    Int_t fEvent;
    Int_t fVolumeId;
    Int_t fNPixelsX;
    Int_t fNPixelsY;
   
    TClonesArray* fPixelCharge;
    
    TClonesArray* fDigis;
   
    TClonesArray* fDigiMatch;
    
  

    std::vector<CbmMvdPixelCharge*> fPixelChargeShort;

    TObjArray* fPixelScanAccelerator;
    std::map<pair<Int_t, Int_t>, CbmMvdPixelCharge*> fChargeMap;
    std::map<pair<Int_t, Int_t>, CbmMvdPixelCharge*>::iterator fChargeMapIt;
    
    
    


private:

 
  CbmMvdSensorDataSheet* fsensorDataSheet;
  
  /** Hit producer mode (0 = MAPS, 1 = Ideal) **/
  Int_t fMode;


  /** MAPS properties **/
  Double_t fSigmaX, fSigmaY;   // MAPS resolution in [cm]
  Double_t fReadoutTime;       // MAPS readout time in [s]
  Double_t fEfficiency;        // MAPS detection efficiency
  Double_t fMergeDist;         // Merging distance
  Double_t fFakeRate;          // Fake hit rate
  Int_t    fNPileup;           // Number of pile-up background events
  Int_t    fNDeltaElect;       // Number of delta electron events
  Int_t    fDeltaBufferSize;
  Int_t    fBgBufferSize;


 /** IO arrays **/
  TString	fBranchName;     // Name of input branch (STSPoint)
  TString       fBgFileName;     // Name of background (pileup) file 
  TString       fDeltaFileName;  // Name of the file containing delta electrons
  TClonesArray* fInputPoints;     // Array of MCPoints (input) 
  
  TRefArray* fPoints;            // Array of all MCPoints (including background files)


  /** Random generator and Stopwatch **/
  TRandom3   fRandGen; 
  TStopwatch fTimer;
  

  /** Pileup manager **/
  CbmMvdPileupManager* fPileupManager;
  CbmMvdPileupManager* fDeltaManager;


  /** Parameters **/
  CbmMvdGeoPar*  fGeoPar;


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

  struct SignalPoint {
      double x;
      double y;
      double z;
      double sigmaX;
      double sigmaY;
      double charge;
      double eloss;

  };


  typedef std::vector<SignalPoint> SignalPointVec; //!
  
  SignalPointVec fSignalPoints;//!


  /** Set parameter containers **/
  virtual void SetParContainers();
  




  /** Reinitialisation **/
  virtual void ReInit(CbmMvdSensor* mySensor);


  /** Virtual method Finish **/
  virtual void Finish();


  /** Register the output arrays to the IOManager **/
  void Register();


  /** Clear the hit arrays **/
  void Reset();	 


  /** Print digitisation parameters **/
  void PrintParameters();


 
  /** Get MVD geometry parameters from database 
   **@value Number of MVD stations
   **/
  Int_t GetMvdGeometry();
 

  TH1F* h_trackLength;
  TH1F* h_numSegments;
  TH2F* h_LengthVsAngle;
  TH2F* h_LengthVsEloss;
  TH2F* h_ElossVsMomIn;
  
  
  
  
  
  
  
  private:

  CbmMvdSensorDigitizerTask(const CbmMvdSensorDigitizerTask&);
  CbmMvdSensorDigitizerTask operator=(const CbmMvdSensorDigitizerTask&);
    
  ClassDef(CbmMvdSensorDigitizerTask,1);

};


#endif
