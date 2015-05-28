#include "CbmBuildEventsSimple.h"

#include "CbmTimeSlice.h"
#include "CbmDigi.h"

#include "FairRootManager.h"
#include "FairLogger.h"

#include "TClonesArray.h"

#include <iostream>
#include <list>

using namespace std;

CbmBuildEventsSimple::CbmBuildEventsSimple()
  : FairTask(), fSlice(NULL), fSTSDigi(NULL),
    fNSTSDigis(0), fEv(0), fSliceN(0),
    fStepLength(30), fEventStartThreshold(4000), fSliceLength(200),
    fPrevT(-1111), fPrevN(-1111), fCurT(-1111), fCurN(-1111),
    fStartT(-1111), fEndT(-1111), fOpenEvent(0)
{
}

CbmBuildEventsSimple::CbmBuildEventsSimple(const char* name, Int_t iVerbose)
  : FairTask(name, iVerbose), fSlice(NULL), fSTSDigi(NULL),
    fNSTSDigis(0), fEv(0), fSliceN(0),
    fStepLength(30), fEventStartThreshold(4000), fSliceLength(200),
    fPrevT(-1111), fPrevN(-1111), fCurT(-1111), fCurN(-1111),
    fStartT(-1111), fEndT(-1111), fOpenEvent(0)
{
}

void CbmBuildEventsSimple::Exec(Option_t* opt)
{
  Int_t i=0;
  Int_t n=0;
  Int_t nsts=fSlice->GetDataSize(kSTS);
  Double_t t;
  CbmDigi* digi;

  LOG(INFO) << "CbmBuildEventsSimple:	Sts events in slice " << nsts << ". Slice start: " << fSlice->GetStartTime() << FairLogger::endl;
  if (fOpenEvent<0) i=SkipEvents(0);
  if (fOpenEvent>0) ConstructEvent();

  for(;i<nsts;i++)
  {
    digi=fSlice->GetData(kSTS, i);
    if (digi==NULL) continue;
    t=digi->GetTime();
//    cout << t << endl;
    if (t-fCurT>fStepLength)
    {
      fPrevT=fCurT; fPrevN=fCurN;
      fCurT=t; fCurN=i; n=0;
    }
    n++;
    if (n>=fEventStartThreshold)
    {
      i=ConstructEvent();
      n=0;
    }
  }
  fSliceN++;
}

Int_t CbmBuildEventsSimple::ConstructEvent()
{
  Int_t i;
  Int_t is;
  Int_t ir=-1111;
  Double_t stt;	// Start time
  Int_t nsts=fSlice->GetDataSize(kSTS);
  CbmDigi* digi;
  Int_t n=-111111111;
  Double_t t;
  //TODO: New global variable required
  Double_t startt=fCurT+fStepLength*2.0;
 
  if (fOpenEvent==0)
  {
    if (fCurT-fPrevT<fStepLength*1.5)
    {
      i=is=fPrevN;
      stt=fPrevT;
    }
    else
    {
      i=is=fCurN;
      stt=fCurT;
    }
    fEndT=stt+fSliceLength;
    fStartT=stt;
    LOG(INFO) << "CbmBuildEventsSimple:	Construction of event " << fEv << " from " <<  stt << " to " << fEndT << " ns stared. SliceN=" << fSliceN << FairLogger::endl;
  }
  else
  {
    i=is=0;
    stt=fStartT;
    LOG(INFO) << "CbmBuildEventsSimple:	Construction of event " << fEv << " from " <<  stt << " to " << fEndT << " ns continued. SliceN=" << fSliceN << FairLogger::endl;
  }


  for(;i<nsts;i++)
  {
    digi=fSlice->GetData(kSTS, i);
    t=digi->GetTime();
    if (t>fEndT) break;
    if (t>startt&&t-fCurT>fStepLength)
    {
      fPrevT=fCurT; fPrevN=fCurN;
      fCurT=t; fCurN=i; n=0;
    }
    n++;
    if (n>=fEventStartThreshold) break;
  }

  if (n>=fEventStartThreshold)
  {
    if (fCurT+fStepLength>fEndT) { ir=i-1; }
    else
    {
      LOG(ERROR) << "CbmBuildEventsSimple:	Overlapping events at " << stt << "  and " << fCurT << ". Skipping both." << FairLogger::endl;
      fEndT=fCurT+fSliceLength;
      i=SkipEvents(i);
      return i-1;
    }
  }
  if (ir<0) ir=i-1;

  FillStsInfo(is, ir);
  if (i==nsts)
  {
    fOpenEvent++;
    return nsts;
  }
  LOG(INFO) << "CbmBuildEventsSimple:	Event " << fEv << " constructed. STS digis: " <<  fSTSDigi->GetEntriesFast() << FairLogger::endl;
  fEv++; fOpenEvent=0; fEndT=-1111; fStartT=-1111;
  FairRootManager::Instance()->Fill();
  fNSTSDigis=0; fSTSDigi->Delete();

  return i-1;
}

/** Fills Stsdigis array. STS separate, because start and end] end digi is known. **/
void CbmBuildEventsSimple::FillStsInfo(Int_t st, Int_t end)
{
  Int_t i;
  CbmStsDigi* digi;

  for(i=st;i<=end;i++)
  {
    digi=(CbmStsDigi*)fSlice->GetData(kSTS, i);
    new ((*fSTSDigi)[fNSTSDigis])CbmStsDigi(*digi);
    fNSTSDigis++;
//    cout << fSTSDigi->GetEntries() << " " << fNSTSDigis << endl;
  }
}

Int_t CbmBuildEventsSimple::SkipEvents(Int_t ists)	/** Skip events until fEndT reached **/
{
  Int_t i;
  CbmDigi* digi;
  Int_t n=0;
  Int_t nsts=fSlice->GetDataSize(kSTS);
  Double_t t;

  if (fOpenEvent<=0) fOpenEvent--; else fOpenEvent=-1;
  fSTSDigi->Delete(); fNSTSDigis=0;
  for(i=ists;i<nsts;i++)
  {
    digi=fSlice->GetData(kSTS, i);
    if (digi==NULL) continue;
    t=digi->GetTime();
    if (t>fEndT) break;
    if (t-fCurT>fStepLength)
    {
      fPrevT=fCurT; fPrevN=fCurN;
      fCurT=t; fCurN=i; n=0;
    }
    n++;
    if (n>=fEventStartThreshold)
    {
      if (fCurT-fPrevT<fStepLength*1.5)
	fEndT=fPrevT+fSliceN;
      else 
	fEndT=fCurT+fSliceN;
    }
  }

  if (i!=nsts||t>fEndT) { fEndT=-1111; fOpenEvent=0; } 

  return i;
}

void CbmBuildEventsSimple::Finish()
{
  LOG(INFO) << "CbmBuildEventsSimple:	Events constructed: " << fEv << ". Slices processed: " << fSliceN << FairLogger::endl;
}

InitStatus CbmBuildEventsSimple::Init()
{
  FairRootManager* mgr=FairRootManager::Instance();

  if (mgr==NULL)
  {
    LOG(FATAL) << "Can't find root manager in the system." << FairLogger::endl;
    return kFATAL;
  }
  fSlice=(CbmTimeSlice*)mgr->GetObject("TimeSlice.");
  if (fSlice==NULL)
  {
    LOG(FATAL) << "No timeslice object in the system" << FairLogger::endl;
    return kFATAL;
  }

  fSTSDigi=new TClonesArray("CbmStsDigi", 10000);
  mgr->Register("StsDigi", "Digital response in STS" ,fSTSDigi, kTRUE);
  fSTSDigi->Delete(); fNSTSDigis=0;

  return kSUCCESS;
}

CbmBuildEventsSimple::~CbmBuildEventsSimple()
{
  ;
}

ClassImp(CbmBuildEventsSimple)
