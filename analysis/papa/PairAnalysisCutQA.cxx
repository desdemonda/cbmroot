/*************************************************************************
 * Copyright(c) 1998-2009, ALICE Experiment at CERN, All rights reserved. *
 **************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                           CutQA                                      //
//                                                                      //
/*
   Allow to monitor how many tracks,pair,events pass the selection criterion 
   in any of the cuts added to the corresponding filters. All you need to 
   add to your config is the following:

   papa->SetCutQA();


*/
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "PairAnalysisCutQA.h"

#include <TList.h>
#include <TCollection.h>

#include "PairAnalysisCutGroup.h"
#include "AnalysisCuts.h"

#include "PairAnalysisEvent.h"
//#include "AliVParticle.h"
#include "PairAnalysisTrack.h"
#include "PairAnalysisPair.h"


ClassImp(PairAnalysisCutQA)


PairAnalysisCutQA::PairAnalysisCutQA() :
  TNamed(),
  fQAHistArray()
{
  //
  // Default constructor
  //
  for(Int_t itype=0; itype<kNtypes; itype++) {
    fCutQA[itype]=0x0;
    fNCuts[itype]=1;
    for(Int_t i=0; i<20; i++) {
      fCutNames[i][itype]="";
    }
  }
  fTypeKeys[kTrack] = "Track";
  fTypeKeys[kPair]  = "Pair";
  fTypeKeys[kEvent] = "Event";
  fQAHistArray.SetOwner();
}

//_____________________________________________________________________
PairAnalysisCutQA::PairAnalysisCutQA(const char* name, const char* title) :
  TNamed(name, title),
  fQAHistArray()
{
  //
  // Named Constructor
  //
  for(Int_t itype=0; itype<kNtypes; itype++) {
    fCutQA[itype]=0x0;
    fNCuts[itype]=1;
    for(Int_t i=0; i<20; i++) {
      fCutNames[i][itype]="";
    }
  }
  fTypeKeys[kTrack] = "Track";
  fTypeKeys[kPair]  = "Pair";
  fTypeKeys[kEvent] = "Event";
  fQAHistArray.SetOwner();
}

//_____________________________________________________________________
PairAnalysisCutQA::~PairAnalysisCutQA() 
{
  //
  //Default Destructor
  //
  fQAHistArray.Delete();
}

//_____________________________________________________________________
void PairAnalysisCutQA::Init()
{

  fQAHistArray.SetName(Form("%s",GetName()));

  // loop over all types
  for(Int_t itype=0; itype<kNtypes; itype++) {
    //    printf("\n type: %d\n",itype);
    fCutNames[0][itype]="no cuts";

    // create histogram based on added cuts
    fCutQA[itype] = new TH1F(fTypeKeys[itype],
			     Form("%sQA;cuts;# passed %ss",fTypeKeys[itype],fTypeKeys[itype]),
			     fNCuts[itype],0,fNCuts[itype]);
    // loop over all cuts
    for(Int_t i=0; i<fNCuts[itype]; i++) {
      fCutQA[itype]->GetXaxis()->SetBinLabel(i+1,fCutNames[i][itype]);
      //      printf(" %s \n",fCutNames[i][itype]);
    }
    fQAHistArray.AddLast(fCutQA[itype]);
  }

}

//_____________________________________________________________________
void PairAnalysisCutQA::AddTrackFilter(AnalysisFilter *trackFilter)
{
  //
  // add track filter cuts to the qa histogram
  //


  TIter listIterator(trackFilter->GetCuts());
  while (AnalysisCuts *thisCut = (AnalysisCuts*) listIterator()) {
    Bool_t addCut=kTRUE;

    // add new cut class to the array
    if(addCut) {
      fCutNames[fNCuts[kTrack]][kTrack]=thisCut->GetTitle();
      //      printf("add cut %s to %d \n",thisCut->GetTitle(),fNCuts[kTrack]);
      fNCuts[kTrack]++;
    }

  } // pair filter loop

}


//_____________________________________________________________________
void PairAnalysisCutQA::AddPairFilter(AnalysisFilter *pairFilter)
{
  //
  // add track filter cuts to the qa histogram
  //


  TIter listIterator(pairFilter->GetCuts());
  while (AnalysisCuts *thisCut = (AnalysisCuts*) listIterator()) {
    Bool_t addCut=kTRUE;

    // add new cut class to the array
    if(addCut) {
      fCutNames[fNCuts[kPair]][kPair]=thisCut->GetTitle();
      //  printf("add cut %s to %d \n",thisCut->GetTitle(),fNCuts[kPair]);
      fNCuts[kPair]++;
    }

  } // trk filter loop

}

//_____________________________________________________________________
void PairAnalysisCutQA::AddEventFilter(AnalysisFilter *eventFilter)
{
  //
  // add track filter cuts to the qa histogram
  //


  TIter listIterator(eventFilter->GetCuts());
  while (AnalysisCuts *thisCut = (AnalysisCuts*) listIterator()) {
    Bool_t addCut=kTRUE;

    // add new cut class to the array
    if(addCut) {
      fCutNames[fNCuts[kEvent]][kEvent]=thisCut->GetTitle();
      //      printf("add cut %s to %d \n",thisCut->GetTitle(),fNCuts[kEvent]);
      fNCuts[kEvent]++;
    }

  } // trk filter loop

}

//_____________________________________________________________________
void PairAnalysisCutQA::Fill(UInt_t mask, TObject *obj)
{
  //
  // fill the corresponding step in the qa histogram
  //

  UInt_t idx = GetObjIndex(obj);

  Int_t cutstep=1;
  for (Int_t iCut=0; iCut<fNCuts[idx]-1;++iCut) {
    //    UInt_t cutMask=1<<iCut;         // for each cut
    UInt_t cutMask=(1<<(iCut+1))-1; // increasing cut match

    if ((mask&cutMask)==cutMask) {
      fCutQA[idx]->Fill(cutstep);
      ++cutstep;
    }

  }

}

//_____________________________________________________________________
void PairAnalysisCutQA::FillAll(TObject *obj)
{
  //
  // fill the corresponding step in the qa histogram
  //

  UInt_t idx = GetObjIndex(obj);
  fCutQA[idx]->Fill(0);

}

//______________________________________________________________________
UInt_t PairAnalysisCutQA::GetObjIndex(TObject *obj)
{
  //
  // return the corresponding idex
  //
  //  printf("INFO: object type is a %s \n", obj->IsA()->GetName());
  if(obj->InheritsFrom(PairAnalysisPair::Class())    )  return kPair;
  if(obj->InheritsFrom(PairAnalysisTrack::Class())   )  return kTrack;
  if(obj->InheritsFrom(PairAnalysisEvent::Class())   )  return kEvent;
  //  printf("FATAL: object type %s not yet supported, please let the author know\n", obj->IsA()->GetName());
  return -1;

}




