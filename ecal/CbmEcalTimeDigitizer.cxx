#include "CbmEcalTimeDigitizer.h"

#include "FairRootManager.h"

#include "TFormula.h"

#include "CbmEcalParam.h"
#include "CbmEcalCellMC.h"
#include "CbmEcalStructure.h"
#include "CbmEcalCalibration.h"

#include <iostream>

using namespace std;

/** Loop procedure **/
void CbmEcalTimeDigitizer::Exec(Option_t* option)
{
  if (fVerbose>0)
    Info("Exec", "Event %d.", ++fEvent);
  if (fCells.size()==0)
    fStr->GetCells(fCells);
  list<CbmEcalCell*>::const_iterator p=fCells.begin();
  for(;p!=fCells.end();++p)
    CalculateTime((CbmEcalCellMC*)(*p));
}

/** Calculate time in given cell **/
void CbmEcalTimeDigitizer::CalculateTime(CbmEcalCellMC* cell)
{
  std::map<Int_t, Float_t>::const_iterator p=cell->GetTrackEnergyBegin();
  Int_t n=0;
  Int_t i;
  Double_t e=0;
  for(;p!=cell->GetTrackEnergyEnd();++p)
  {
    Double_t time=cell->GetTrackTime(p->first);
    if (time!=-1111)
    {
      fE[n]=p->second;
      fT[n]=time;
      n++;
    }
    else
    {
      Info("CalculateTime","Can't find time information for track %d.", p->first);
      return;
    }
  }
  TMath::Sort(n, fT, fI);
  for(i=0;i<n;i++)
  {
    e+=fE[fI[i]];
    if (fCal->GetEnergy(e, cell)>fThreshold)
      break;
  }
  if (i!=n)
  {
    fResponse->SetParameter(2, fT[fI[i]]);
    fResponse->SetParameter(1, fCal->GetEnergy(fE[fI[i]], cell));
    fResponse->SetParameter(0, fCal->GetEnergy(cell->GetEnergy(), cell));
    cell->SetTime(fResponse->Eval(0));
  }
  else
  {
//    cout << "CellTimeCalc: " << e << "	" << fCal->GetEnergy(e, cell) << "	" << cell->GetCenterX() << "	" << cell->GetCenterY() << endl;
    cell->SetTime(-1111);
  }
}

void CbmEcalTimeDigitizer::SetArraySize(Int_t size)
{
  if (fSize>size) return;
  if (fE) delete fE;
  if (fT) delete fT;
  if (fI) delete fI;
  fSize=size;
  fE=new Double_t[fSize];
  fT=new Double_t[fSize];
  fI=new Int_t[fSize];
}

CbmEcalTimeDigitizer::CbmEcalTimeDigitizer()
  : FairTask(),
    fStr(NULL),
    fCells(),
    fResponse(NULL),
    fConfigName(""),
    fThreshold(0.),
    fEvent(0),
    fCal(NULL),
    fSize(0),
    fE(NULL),
    fT(NULL),
    fI(NULL)

{
}


CbmEcalTimeDigitizer::CbmEcalTimeDigitizer(const char* name, const Int_t iVerbose, const char* config)
  : FairTask(name, iVerbose),
    fStr(NULL),
    fCells(),
    fResponse(NULL),
    fConfigName(config),
    fThreshold(0.),
    fEvent(0),
    fCal(NULL),
    fSize(0),
    fE(NULL),
    fT(NULL),
    fI(NULL)

{
}

/** Initing routine **/
InitStatus CbmEcalTimeDigitizer::Init()
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
    Fatal("Init", "Can't find calorimeter structure in the system");
    return kFATAL;
  }
  fCal=(CbmEcalCalibration*)fManager->GetObject("EcalCalibration");
  if (!fCal)
  {
    Fatal("Init", "Can't find EcalCalibration");
    return kFATAL;
  }

  CbmEcalParam* par=new CbmEcalParam("EcalTimeDigitizer", fConfigName);
  fThreshold=par->GetDouble("energythreshold");
  TString st=par->GetString("response");
  Info("Init","Using formula %s.", st.Data());
  st.ReplaceAll("Ecell","[0]");
  st.ReplaceAll("Eparticle","[1]");
  st.ReplaceAll("Tparticle","[2]");
  Info("Init","Using formula %s.", st.Data());
  fResponse=new TFormula("EcalTimeDigitizerFormula", st);
  delete par;

  fEvent=0;
  fCells.clear();
  SetArraySize(6000);

  return kSUCCESS;
}

/** Finishing routine **/
void CbmEcalTimeDigitizer::Finish()
{
  if (fE) delete fE;
  if (fT) delete fT;
  if (fI) delete fI;
}

ClassImp(CbmEcalTimeDigitizer)
