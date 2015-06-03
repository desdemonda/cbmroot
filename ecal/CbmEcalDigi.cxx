#include "CbmEcalDigi.h"

#include "CbmEcalStructure.h"

#include "FairRootManager.h"

#include "TRandom.h"

#include <iostream>
#include <list>

using namespace std;

/** --- Default constructor --------------------------------------------------- **/
CbmEcalDigi::CbmEcalDigi() 
  : FairTask(),
    fPedestal(80),
    fADCMax(4096),
    fADCNoise(0.5e-3),
    fADCChannel(0.5e-3),
    fStr(NULL)
{
}

/** --- Standard constructor. Use this ---------------------------------------- **/
CbmEcalDigi::CbmEcalDigi(const char* name, Int_t iVerbose)
  : FairTask(name, iVerbose),
    fPedestal(80),
    fADCMax(4096),
    fADCNoise(0.5e-3),
    fADCChannel(0.5e-3),
    fStr(NULL)

{
}

/** --- Initialization of the task -------------------------------------------- **/  
InitStatus CbmEcalDigi::Init()
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
void CbmEcalDigi::Exec(Option_t* option)
{
  CbmEcalCell* cell;
  list<CbmEcalCell*> cells;
  fStr->GetCells(cells);
  list<CbmEcalCell*>::const_iterator p=cells.begin();
  Short_t adc;

  for(;p!=cells.end();++p)
  {
    cell=(*p);
    adc=((Short_t)(gRandom->Gaus(cell->GetEnergy(), fADCNoise)/fADCChannel))+fPedestal;
    if (adc>fADCMax) adc=fADCMax;
    cell->SetEnergy(-1111);
    cell->SetADC(adc);
  }
}

/** --- Finish task ----------------------------------------------------------- **/ 
void CbmEcalDigi::Finish()
{
  ;
}
/** --- Destructor ------------------------------------------------------------ **/
CbmEcalDigi::~CbmEcalDigi()
{
  ;
}

ClassImp(CbmEcalDigi)
