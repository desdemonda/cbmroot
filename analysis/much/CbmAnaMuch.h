/** CbmAnaMuch.h
 *@author E.Kryshen <e.kryshen@gsi.de>
 *@since 2009-09-08
 **
 ** Acceptance and efficiency correction framework for muons
 **/

#ifndef CBMANAMUCH_H_
#define CBMANAMUCH_H_ 1

#include "FairTask.h"
#include "TString.h"
#include <map>

class CbmMuchGeoScheme;
class CbmMuchTrack;
class CbmAnaMuch;
class TClonesArray;
class CbmTrackMatch;
class TH1D;
class TH2D;
using std::map;

class CbmAnaMuch : public FairTask{
public:
  /** Default constructor **/
  CbmAnaMuch();

  /** Standard constructor
  *@param name   Name of class
  *@param title  Task title
  **/
  CbmAnaMuch(const char* name, TString digiFileName, TString histoFileName);

  /** Destructor **/
  virtual ~CbmAnaMuch();

  /** Initialisation **/
  virtual InitStatus Init();

  /** Task execution **/
  virtual void Exec(Option_t* opt);

  /** Finish at the end **/
  virtual void Finish();

  /** SetParContainers **/
  virtual void SetParContainers();

  /** Return -1 if match->GetTrackId()<0
   *  Return -2 if trueHits/totalHits<quota
   */
  static Int_t GetTrackId(CbmTrackMatch* match, Double_t quota=0.7);
  
  // Set STS acceptance criteria as a number of STS points
  void SetStsPointsAccQuota(Int_t nPoints) { fStsPointsAccQuota = nPoints; }
  void SetStsTrueHitQuota(Double_t quota)  { fStsTrueHitQuota = quota; }
  
  Bool_t IsReconstructed(CbmMuchTrack* track);
private:
  TString fDigiFileName;            //!
  CbmMuchGeoScheme* fGeoScheme;     //!
  Int_t         fEvent;             //!
  TClonesArray* fMCTracks;          //!
  TClonesArray* fStsTracks;         //!
  TClonesArray* fStsPoints;         //!
  TClonesArray* fMuchPoints;        //!
  TClonesArray* fMuchHits;          //!
  TClonesArray* fMuchTracks;        //!
  TClonesArray* fMuchTrackMatches;  //!
  TClonesArray* fStsTrackMatches;   //!
  map<Int_t,Int_t> mapAccSts;       //!
  map<Int_t,Int_t> mapAccMuch;      //!
  map<Int_t,Int_t> mapRecSts;       //!
  map<Int_t,Int_t> mapRecMuch;      //!
  TH1D* hAllP;                      //!
  TH1D* hAccStsP;                   //!
  TH1D* hAccMuchP;                  //!
  TH1D* hRecStsP;                   //!
  TH1D* hRecMuchP;                  //!
  
  TH2D* hMuPlusYPt;                 //!
  TH2D* hMuMinusYPt;                //!
  TH1D* hJpsiM;                     //!
  TH2D* hJpsiYPt;                   //!
  
  Int_t    fStsPointsAccQuota;
  Double_t fStsTrueHitQuota;
  Int_t    fMuchPointsAccQuota;
  Double_t fMuchTrueHitQuota;
  TString fHistoFileName;

  CbmAnaMuch(const CbmAnaMuch&);
  CbmAnaMuch operator=(const CbmAnaMuch&);

  ClassDef(CbmAnaMuch,1);
};

#endif
