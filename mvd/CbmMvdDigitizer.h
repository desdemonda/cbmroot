// ----------------------------------------------------------------------------
// -----                    CbmMvdDigitizer header file                   -----
// -----                   Created by C. Dritsa (2009)                    -----
// -----                   Maintained by M.Deveaux (m.deveaux(att)gsi.de) -----
// -----                   Update by P.Sitzmann (p.sitzmann(att)gsi.de    -----
// ----------------------------------------------------------------------------

#ifndef CBMMVDDIGITIZER_H
#define CBMMVDDIGITIZER_H 1

#include "FairTask.h"
#include "CbmMvdDetector.h"
#include "TGeoManager.h"

#include "TRandom3.h"
#include "TString.h"
#include "TMath.h"

using namespace std;


class CbmMvdDigitizer : public FairTask
{
 
 public:

  /** Default constructor **/  
  CbmMvdDigitizer();


  /** Standard constructor 
  *@param name  Task name
  *@param mode  0 = MAPS, 1 = Ideal
  **/
  CbmMvdDigitizer(const char* name, 
		    Int_t mode = 0, Int_t iVerbose = 1);


  /** Destructor **/
  ~CbmMvdDigitizer();

  void Exec(Option_t* opt);

  void SetPileUp(Int_t pileUp)           { fNPileup         = pileUp;      }
  void SetDeltaEvents(Int_t deltaEvents) { fNDeltaElect     = deltaEvents; }
  void SetBgFileName(TString fileName)   { fBgFileName      = fileName;    }
  void SetDeltaName(TString fileName)    { fDeltaFileName   = fileName;    }
  void SetBgBufferSize(Int_t nBuffer)    { fBgBufferSize    = nBuffer;     }
  void SetDeltaBufferSize(Int_t nBuffer) { fDeltaBufferSize = nBuffer;     }
  void SetMisalignment(Float_t misalignment[3]){for(Int_t i = 0; i<3; i++) epsilon[i]=misalignment[i];} // set the misalignment in cm
  void BuildEvent();
  void ShowDebugHistograms() {fShowDebugHistos = kTRUE;}
protected:
 
 


private:
/** Hit producer mode (0 = MAPS, 1 = Ideal) **/
  Int_t fMode;

Bool_t fShowDebugHistos;

 CbmMvdDetector* fDetector;
 
 TClonesArray* fInputPoints;
 TClonesArray* fDigis;
 TClonesArray* fDigiMatch;
 pair<Float_t, Int_t> fPerformanceDigi;

 UInt_t fDigiPluginNr;

 Double_t fFakeRate;          // Fake hit rate
  Int_t    fNPileup;           // Number of pile-up background events
  Int_t    fNDeltaElect;       // Number of delta electron events
  Int_t    fDeltaBufferSize;
  Int_t    fBgBufferSize;
  Float_t epsilon[3];

  TString	fBranchName;   // Name of input branch (MvdPoint)
  TString       fBgFileName;     // Name of background (pileup) file 
  TString       fDeltaFileName;  // Name of the file containing delta electrons
  

  TStopwatch     fTimer;        ///< ROOT timer
    /** Random generator and Stopwatch **/
  TRandom3   fRandGen; 
  
  /** Pileup manager **/
  CbmMvdPileupManager* fPileupManager;
  CbmMvdPileupManager* fDeltaManager;



// -----   Private methods   ---------------------------------------------
 /** Intialisation **/
  virtual InitStatus Init();


  /** Reinitialisation **/
  virtual InitStatus ReInit();


  /** Virtual method Finish **/
  virtual void Finish();


  /** Register the output arrays to the IOManager **/
  void Register();

  void GetMvdGeometry();


  /** Clear the hit arrays **/
  void Reset();	 


  /** Print digitisation parameters **/
  void PrintParameters();

private:
  CbmMvdDigitizer(const CbmMvdDigitizer&);
  CbmMvdDigitizer operator=(const CbmMvdDigitizer&);  

ClassDef(CbmMvdDigitizer,1);    
};
    
    
#endif   		     
