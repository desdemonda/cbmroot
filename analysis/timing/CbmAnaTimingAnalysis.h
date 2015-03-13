/** CbmAnaTimingAnalysis.h
 *@author E.Kryshen <e.kryshen@gsi.de>
 *@since 2010-09-07
 **
 **/

#ifndef CbmAnaTimingAnalysis_H_
#define CbmAnaTimingAnalysis_H_ 1

#include "FairTask.h"
#include "TString.h"
class TH3D;
class TH1D;

class CbmAnaTimingAnalysis : public FairTask{
public:
  /** Default constructor **/
  CbmAnaTimingAnalysis();

  /** Standard constructor
  *@param name   Name of class
  *@param title  Task title
  **/
  CbmAnaTimingAnalysis(const char* name, TString histoName);

  /** Destructor **/
  virtual ~CbmAnaTimingAnalysis();

  /** Initialisation **/
  virtual InitStatus Init();

  /** Task execution **/
  virtual void Exec(Option_t* opt);

  /** Finish at the end **/
  virtual void Finish();

  /** SetParContainers **/
  virtual void SetParContainers();

private:
  Int_t         fEvent;             //!
  TClonesArray* fMCTracks;          //!
  TClonesArray* fStsPoints;         //!
  TClonesArray* fMuchPoints;        //!
  TClonesArray* fTofPoints;         //!
  TClonesArray* fTrdPoints;         //!
  TString fHistoName;
  TH3D* fDetEventTime;
  TH1D* fNPrimaryTracks;
  TH1D* fNPrimaryTracksInAcceptance;
  TH1D* fStsPointsZ;
  TH1D* fMuchPointsZ;
  TH1D* fTofPointsZ;
  TH1D* fTrdPointsZ;
  ClassDef(CbmAnaTimingAnalysis,1);
};

#endif
