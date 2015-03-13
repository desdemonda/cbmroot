// -------------------------------------------------------------------------
// -----                    CbmMvdHitProducer header file              -----
// -----                   Created 08/11/06  by V. Friese              -----
// -------------------------------------------------------------------------


/** CbmMvdHitProducer header file
 **
 ** Task class for producing MvdHits from MvdPoints.
 ** Mode = 0 : MAPS readout
 ** Mode = 1 : Ideal (copy of MCPoint properties)
 **
 ** Former CbmStsMapsHitProducer (M.Deveaux, 02/02/05)
 **
 **/


#ifndef CBMMVDHITPRODUCER_H
#define CBMMVDHITPRODUCER_H 1

#include "FairTask.h"

#include "TRandom3.h"
#include "TStopwatch.h"
#include "TString.h"

#include <list>
#include <map>

class TClonesArray;
class TRandom3;
class CbmMvdGeoPar;
class CbmMvdPileupManager;


class CbmMvdHitProducer : public FairTask
{
 
 public:

  /** Default constructor **/  
  CbmMvdHitProducer();


  /** Standard constructor 
  *@param name  Task name
  *@param mode  0 = MAPS, 1 = Ideal
  **/
  CbmMvdHitProducer(const char* name, 
		    Int_t mode = 0, Int_t iVerbose = 1);


  /** Destructor **/
  virtual ~CbmMvdHitProducer();


  /** Task execution **/
  virtual void Exec(Option_t* opt);


  /** Accessors **/
  Double_t GetSigmaX()        const { return fSigmaX;     };
  Double_t GetSigmaY()        const { return fSigmaY;     };
  Double_t GetEfficiency()    const { return fEfficiency; };
  Double_t GetMergeDistance() const { return fMergeDist;  };
  Double_t GetFakeRate()      const { return fFakeRate;   };
  Int_t    GetNPileup()       const { return fNPileup;    };
  TString  GetBranchName()    const { return fBranchName; };


  /** Modifiers **/
  void SetSigmaX(Double_t sigmaX) { fSigmaX = sigmaX; }
  void SetSigmaY(Double_t sigmaY) { fSigmaY = sigmaY; }
  void SetEfficiency(Double_t eff);
  void SetMergeDistance(Double_t dist);
  void SetFakeRate(Double_t rate);
  void SetNPileup(Int_t nPileup);
  void SetBgFileName(const char* fileName) {fBgFileName = fileName;}
  


 public:

  // ----------   Private data members  ------------------------------------

  /** Hit producer mode (0 = MAPS, 1 = Ideal) **/
  Int_t fMode;


  /** MAPS properties **/
  Double_t fSigmaX, fSigmaY;   // MAPS resolution in cm
  Double_t fEfficiency;        // MAPS detection efficiency
  Double_t fMergeDist;         // Merging distance
  Double_t fFakeRate;          // Fake hit rate
  Int_t    fNPileup;           // Number of pile-up background events


  /** Map of MC Volume Id to station number **/
  std::map<Int_t, Int_t> fStationMap;                  //!   
 
  /** Map of station number to pair of (rmin, rmax) **/
  std::map<Int_t, std::pair<Double_t, Double_t> > fRadiusMap;     //!


  /** IO arrays **/
  TString	fBranchName;     // Name of input branch (STSPoint)
  TString       fBgFileName;     // Name of background (pileup) file 
  TClonesArray* fPoints;         // Array of MCPoints (input)
  TClonesArray* fHits;           // Array of CbmMvdHit (output)
  TClonesArray* fMatches;        // Array of HitInfos (output)


  /** Random generator and Stopwatch **/
  TRandom3   fRandGen; 
  TStopwatch fTimer;
  

  /** Pileup manager **/
  CbmMvdPileupManager* fPileupManager;


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

  /** Set parameter containers **/
  virtual void SetParContainers();
  

  /** Intialisation **/
  virtual InitStatus Init();


  /** Reinitialisation **/
  virtual InitStatus ReInit();


  /** Exec (MAPS mode) **/
  void ExecMaps();


  /** Exec (Ideal mode) **/
  void ExecIdeal();


  /** Virtual method Finish **/
  virtual void Finish();


  /** Register the output arrays to the IOManager **/
  void Register();


  /** Clear the hit arrays **/
  void Reset();	 


  /** Print digitisation parameters **/
  void PrintParameters();


  /** Get digitisation parameters from database **/
  Bool_t GetParameters() ;
 

  /** Produce hits from MCPoints
   *@param pointBuffer Buffer of MCPoints
   *@param iFile  0 = data file, 1 = background file
   *@param iEvent 0 = data event, n = background event 
   *@param nInStations (return) Number of MvdPoints
   *@param nCreated    (return) Number of created MvdHits
   *@param nLost       (return) Number of lost hits (inefficiency)
   **/
  void ProduceHits(TClonesArray* pointBuffer, Int_t iFile, Int_t iEvent, 
		   Int_t& nPoints, Int_t& nCreated, Int_t& nLost);


  /** Produce fake hits 
   *@return  Number of fake hits created  
   **/
  Int_t ProduceFakes();


  /** Check for merged hits 
   *@return  Number of mergings
   **/
  Int_t MergeHits();

 private: 

  CbmMvdHitProducer(const CbmMvdHitProducer&);
  CbmMvdHitProducer& operator=(const CbmMvdHitProducer&);

  ClassDef(CbmMvdHitProducer,1);
    
};
    
    
#endif   		     
