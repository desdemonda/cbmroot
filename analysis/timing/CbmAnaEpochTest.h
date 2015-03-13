/** CbmAnaEpochTest.h
 *@author E.Kryshen <e.kryshen@gsi.de>
 *@since 2010-09-07
 **
 **/

#ifndef CbmAnaEpochTest_H_
#define CbmAnaEpochTest_H_ 1

#include "FairTask.h"
#include "TString.h"
#include "CbmMCEpoch.h"
class TH3D;
class TH1D;

class CbmAnaEpochTest : public FairTask{
public:
  /** Default constructor **/
  CbmAnaEpochTest();

  /** Standard constructor
  *@param name   Name of class
  *@param title  Task title
  **/
  CbmAnaEpochTest(const char* name, TString histoName);

  /** Destructor **/
  virtual ~CbmAnaEpochTest();

  /** Initialisation **/
  virtual InitStatus Init();

  /** Task execution **/
  virtual void Exec(Option_t* opt);

  /** Finish at the end **/
  virtual void Finish();

  /** SetParContainers **/
  virtual void SetParContainers();

private:
  Int_t         fNepoch;            //!
  CbmMCEpoch*   fEpoch;             //!
  TString fHistoName;
  TH1D* fStsTimeFull;
  TH1D* fStsTimeCentral;
  TH1D* fMuchTimeFull;
  TH1D* fMuchTimeCentral;
  ClassDef(CbmAnaEpochTest,1);
};

#endif
