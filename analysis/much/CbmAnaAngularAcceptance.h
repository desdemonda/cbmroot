/** CbmAnaAngularAcceptance.h
 *@author E.Kryshen <e.kryshen@gsi.de>
 *@since 2010-03-28
 **
 **/

#ifndef CBMANAANGULARACCEPTANCE_H_
#define CBMANAANGULARACCEPTANCE_H_ 1

#include "FairTask.h"
class TClonesArray;
class TH1D;
class CbmAnaMuonCandidate;

class CbmAnaAngularAcceptance : public FairTask{
public:
  /** Default constructor **/
  CbmAnaAngularAcceptance();

  /** Standard constructor
  *@param name   Name of class
  **/
  CbmAnaAngularAcceptance(const char* name);

  /** Destructor **/
  virtual ~CbmAnaAngularAcceptance();

  /** Initialisation **/
  virtual InitStatus Init();

  /** Task execution **/
  virtual void Exec(Option_t* opt);

  /** Finish at the end **/
  virtual void Finish();

  /** SetParContainers **/
  virtual void SetParContainers();

private:
  CbmAnaMuonCandidate* GetMu(Int_t trackId);

private:
  Int_t         fEvent;             //!
  TClonesArray* fMCTracks;          //!
  TClonesArray* fStsTracks;         //!
  TClonesArray* fStsPoints;         //!
  TClonesArray* fMuchPoints;        //!
  TClonesArray* fStsTrackMatches;   //!
  Int_t fSignalPairs;               //!
  TClonesArray* fDimuonCandidates;  //!
  TH1D* fTanMin;
  TH1D* fTanMax;
  
  CbmAnaAngularAcceptance(const CbmAnaAngularAcceptance&);
  CbmAnaAngularAcceptance operator=(const CbmAnaAngularAcceptance&);

  ClassDef(CbmAnaAngularAcceptance,1);
};

#endif
