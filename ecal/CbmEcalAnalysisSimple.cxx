#include "CbmEcalAnalysisSimple.h"

#include "FairRootManager.h"

#include "CbmEcalStructure.h"
#include "CbmEcalDrawer.h"

#include <iostream>
#include <fstream>

using namespace std;

/** Loop procedure **/
void CbmEcalAnalysisSimple::Exec(Option_t* option)
{
  fEv++;

  TString nm=""; nm+=fEv;
  Int_t i=nm.Length();
  for(;i<4;i++) nm="0"+nm;
  nm="ec"+nm; nm+=".png";
  fDr->InitMaximumEnergy();
  fDr->DrawEcal();
  fDr->GetCanvas()->SaveAs(nm);
}


CbmEcalAnalysisSimple::CbmEcalAnalysisSimple()
  : FairTask(),
    fDr(NULL),
    fStr(NULL),
    fName(""),
    fEv(0)
{
}

CbmEcalAnalysisSimple::CbmEcalAnalysisSimple(const char* name, const Int_t iVerbose, const char* fname)
  : FairTask(name, iVerbose),
    fDr(NULL),
    fStr(NULL),
    fName(fname),
    fEv(0)
{
}

/** Initing routine **/
InitStatus CbmEcalAnalysisSimple::Init()
{
  FairRootManager* fManager=FairRootManager::Instance();
  if (!fManager)
  {
    Fatal("Init", "Can't find a Root Manager.");
    return kFATAL;
  }

  fStr=(CbmEcalStructure*)fManager->GetObject("EcalStructure");
  if (!fStr) 
  {
    Fatal("Init()", "Can't find calorimeter structure in the system.");
    return kFATAL;
  }
  fDr=new CbmEcalDrawer(fStr);

  return kSUCCESS;
}

/** Finishing routine **/
void CbmEcalAnalysisSimple::Finish()
{
  ofstream f(fName);
  f << fEv << endl;
  f.close();
}

ClassImp(CbmEcalAnalysisSimple)
