#ifndef CBMECALANALYSISSIMPLE_H
#define CBMECALANALYSISSIMPLE_H

#include "FairTask.h"

#include "TString.h"

#include <list>

class CbmEcalDrawer;
class CbmEcalStructure;

class CbmEcalAnalysisSimple : public FairTask
{
public:
  CbmEcalAnalysisSimple(const char* name, const Int_t iVerbose, const char* fname);
public:
  /** Default constructor **/
  CbmEcalAnalysisSimple();

  /** Initing routine **/
  virtual InitStatus Init();

  /** Loop procedure **/
  virtual void Exec(Option_t* option);

  /** Finishing routine **/
  virtual void Finish();

  /** Destructor **/
  virtual ~CbmEcalAnalysisSimple() {};
private:
  CbmEcalDrawer* fDr;
  CbmEcalStructure* fStr;
  TString fName;
  Int_t fEv;

  CbmEcalAnalysisSimple(const CbmEcalAnalysisSimple&);
  CbmEcalAnalysisSimple& operator=(const CbmEcalAnalysisSimple&);

  ClassDef(CbmEcalAnalysisSimple,1)
};

#endif 

