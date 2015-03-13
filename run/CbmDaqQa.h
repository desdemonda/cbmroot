/** @file CbmDaqQa.h
 ** @author Evgeny Kryshen <e.kryshen@gsi.de>
 ** @date 7 May 2013
 **/


#ifndef CBMDAQQA_H
#define CBMDAQQA_H 1

#include "FairTask.h"

class CbmTimeSlice;
class TH1D;
class TH2D;
class TClonesArray;
class TChain;
class CbmDaqQa : public FairTask{
 public:
  CbmDaqQa(TChain* mcChain);
  ~CbmDaqQa();
  virtual void Exec(Option_t* opt);
  virtual InitStatus Init();
  virtual void Finish();

  private:
  TChain* fMCChain;
  CbmTimeSlice* fTimeSlice;
  TClonesArray* fMuchHits;
  TClonesArray* fClusters;
  TH1D*  fTimeSTS;             //!
  TH1D*  fTimeMUCH;            //!
  TH1D** fSignalShape;         //!
  TH1D*  fTimeMUCHhits;        //!
  TH1D*  fTimeOverThreshold;   //!
  TH2D*  fChargeVsTOT;         //!
  TH2D*  fTotChargeVsTOT;      //!
  TH2D*  fTotChargeVsAMP;      //!
  TH1D*  fTimeMUCHmc;          //!
  TH1D*  fTimeDiff; //!
  TH2D*  fTimeDiff2; //!

  CbmDaqQa(const CbmDaqQa&);
  CbmDaqQa& operator=(const CbmDaqQa&);
  
  ClassDef (CbmDaqQa,1);
};

#endif
