#include "CbmEcalPrepare.h"

#include "CbmEcalStructure.h"

#include "FairRootManager.h"

#include "TRandom.h"

#include <iostream>
#include <list>

using namespace std;

/** --- Default constructor --------------------------------------------------- **/
CbmEcalPrepare::CbmEcalPrepare() 
  : FairTask(),
    fPedestal(80),
    fADCMax(4096),
    fADCChannel(0.5e-3),
    fStr(NULL)
{
}

/** --- Standard constructor. Use this ---------------------------------------- **/
CbmEcalPrepare::CbmEcalPrepare(const char* name, Int_t iVerbose)
  : FairTask(name, iVerbose),
    fPedestal(80),
    fADCMax(4096),
    fADCChannel(0.5e-3),
    fStr(NULL)
{
}

/** --- Initialization of the task -------------------------------------------- **/  
InitStatus CbmEcalPrepare::Init()
{
  FairRootManager* manager=FairRootManager::Instance();
  if (!manager)
  {
    Fatal("Init()", "Can't find IOManager.");
    return kFATAL;
  }

  fStr=(CbmEcalStructure*)manager->GetObject("EcalStructure");
  if (!fStr) 
  {
    Fatal("Init()", "Can't find calorimeter structure in the system.");
    return kFATAL;
  }
  
  return kSUCCESS;
}

/** --- Executed task --------------------------------------------------------- **/ 
void CbmEcalPrepare::Exec(Option_t* option)
{
  CbmEcalCell* cell;
  list<CbmEcalCell*> cells;
  fStr->GetCells(cells);
  list<CbmEcalCell*>::const_iterator p=cells.begin();
  Short_t adc;

  for(;p!=cells.end();++p)
  {
    cell=(*p);
    adc=cell->GetADC();
    adc-=fPedestal; if (adc<0) adc=0;
    cell->SetEnergy(adc*fADCChannel);
  }
}

/** --- Finish task ----------------------------------------------------------- **/ 
void CbmEcalPrepare::Finish()
{
  ;
}
/** --- Destructor ------------------------------------------------------------ **/
CbmEcalPrepare::~CbmEcalPrepare()
{
  ;
}

ClassImp(CbmEcalPrepare)
