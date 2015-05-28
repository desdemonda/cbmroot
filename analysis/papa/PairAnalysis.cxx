/*************************************************************************
* Copyright(c) 1998-2009, ALICE Experiment at CERN, All rights reserved. *
**************************************************************************/

///////////////////////////////////////////////////////////////////////////
//                PairAnalysis Analysis Main class                         //
//                                                                       //
/*
Framework to perform event selectoin, single track selection and track pair
selection.

Convention for the signs of the pair in fPairCandidates:
The names are available via the function PairClassName(Int_t i)

0: SE+ SE+  (same event like sign +)
1: SE+ SE-  (same event unlike sign)
2: SE- SE-  (same event like sign -)

3: SE+ ME+  (mixed event like sign +)
4: SE- ME+  (mixed event unlike sign -+)
5: SE+ ME-  (mixed event unlike sign +-)
6: SE- ME-  (mixed event like sign -)

7: SE+ SE- (same event track rotation)

*/
//                                                                       //
///////////////////////////////////////////////////////////////////////////

#include <TString.h>
#include <TList.h>
#include <TMath.h>
#include <TObject.h>
#include <TGrid.h>

#include "FairMCPoint.h"

#include "CbmTrack.h"
#include "CbmRichRing.h"
#include "CbmMatch.h"
#include "CbmHit.h"
#include "CbmMCTrack.h"

#include "PairAnalysisEvent.h"
#include "PairAnalysisTrack.h"

#include "PairAnalysisPair.h"
#include "PairAnalysisPairLV.h"
#include "PairAnalysisHistos.h"
///#include "PairAnalysisCF.h"
#include "PairAnalysisMC.h"
#include "PairAnalysisVarManager.h"
#include "PairAnalysisTrackRotator.h"
//#include "PairAnalysisDebugTree.h"
#include "PairAnalysisSignalMC.h"
///#include "PairAnalysisMixingHandler.h"
#include "PairAnalysisPairLegCuts.h"
//#include "PairAnalysisV0Cuts.h"
#include "PairAnalysisHistos.h"

#include "PairAnalysis.h"

ClassImp(PairAnalysis)

const char* PairAnalysis::fgkTrackClassNames[2] = {
  "+",
  "-"
};

const char* PairAnalysis::fgkPairClassNames[8] = {
  "SE++",
  "SE+-",
  "SE--",
  "ME++",
  "ME-+",
  "ME+-",
  "ME--",
  "SE+-_TR"
};

//________________________________________________________________
PairAnalysis::PairAnalysis() :
  TNamed("PairAnalysis","PairAnalysis"),
  fCutQA(kFALSE),
  fQAmonitor(0x0),
  fEventFilter("EventFilter"),
  fTrackFilter("TrackFilter"),
  fPairPreFilter("PairPreFilter"),
  fPairPreFilterLegs("PairPreFilterLegs"),
  fPairFilter("PairFilter"),
  fEventPlanePreFilter("EventPlanePreFilter"),
  fEventPlanePOIPreFilter("EventPlanePOIPreFilter"),
  fTrackFilterMC("TrackFilterMC"),
  fPairFilterMC("PairFilterMC"),
  fPdgMother(443),
  fPdgLeg1(11),
  fPdgLeg2(11),
  fSignalsMC(0x0),
  fNoPairing(kFALSE),
  fProcessLS(kTRUE),
  fUseKF(kTRUE),
  fHistoArray(0x0),
  fHistos(0x0),
  fUsedVars(new TBits(PairAnalysisVarManager::kNMaxValuesMC)),
  fPairCandidates(new TObjArray(8)),
  ///  fCfManagerPair(0x0),
  fTrackRotator(0x0),
  //  fDebugTree(0x0),
  ///  fMixing(0x0),
  fPreFilterEventPlane(kFALSE),
  fLikeSignSubEvents(kFALSE),
  fPreFilterUnlikeOnly(kFALSE),
  fPreFilterAllSigns(kFALSE),
  fHasMC(kFALSE),
  fStoreRotatedPairs(kFALSE),
  fDontClearArrays(kFALSE),
  fEventProcess(kTRUE)
{
  //
  // Default constructor
  //

}

//________________________________________________________________
PairAnalysis::PairAnalysis(const char* name, const char* title) :
  TNamed(name,title),
  fCutQA(kFALSE),
  fQAmonitor(0x0),
  fEventFilter("EventFilter"),
  fTrackFilter("TrackFilter"),
  fPairPreFilter("PairPreFilter"),
  fPairPreFilterLegs("PairPreFilterLegs"),
  fPairFilter("PairFilter"),
  fEventPlanePreFilter("EventPlanePreFilter"),
  fEventPlanePOIPreFilter("EventPlanePOIPreFilter"),
  fTrackFilterMC("TrackFilterMC"),
  fPairFilterMC("PairFilterMC"),
  fPdgMother(443),
  fPdgLeg1(11),
  fPdgLeg2(11),
  fSignalsMC(0x0),
  fNoPairing(kFALSE),
  fProcessLS(kTRUE),
  fUseKF(kTRUE),
  fHistoArray(0x0),
  fHistos(0x0),
  fUsedVars(new TBits(PairAnalysisVarManager::kNMaxValuesMC)),
  fPairCandidates(new TObjArray(8)),
  ///  fCfManagerPair(0x0),
  fTrackRotator(0x0),
  //  fDebugTree(0x0),
  ///  fMixing(0x0),
  fPreFilterEventPlane(kFALSE),
  fLikeSignSubEvents(kFALSE),
  fPreFilterUnlikeOnly(kFALSE),
  fPreFilterAllSigns(kFALSE),
  fHasMC(kFALSE),
  fStoreRotatedPairs(kFALSE),
  fDontClearArrays(kFALSE),
  fEventProcess(kTRUE)
{
  //
  // Named constructor
  //
  
}

//________________________________________________________________
PairAnalysis::~PairAnalysis()
{
  //
  // Default destructor
  //
  if (fQAmonitor) delete fQAmonitor;
  if (fHistos) delete fHistos;
  if (fUsedVars) delete fUsedVars;
  if (fPairCandidates && fEventProcess) delete fPairCandidates;
  //  if (fDebugTree) delete fDebugTree;
  ///  if (fMixing) delete fMixing;
  if (fSignalsMC) delete fSignalsMC;
  //  if (fCfManagerPair) delete fCfManagerPair;
  if (fHistoArray) delete fHistoArray;
}

//________________________________________________________________
void PairAnalysis::Init()
{
  //
  // Initialise objects
  //

  //  if(GetHasMC()) PairAnalysisMC::Instance()->SetHasMC(GetHasMC());

  if(fEventProcess) InitPairCandidateArrays();

  /*
  if (fCfManagerPair) {
    fCfManagerPair->SetSignalsMC(fSignalsMC);
    fCfManagerPair->InitialiseContainer(fPairFilter);
  }
  */

  //  if (fDebugTree) fDebugTree->SetPairAnalysis(this);

  ///  if (fMixing) fMixing->Init(this);
  if (fHistoArray) {
    //    fHistoArray->SetSignalsMC(fSignalsMC);
    fHistoArray->Init();
  }

  /*
  if(!fEventProcess) {
    PairAnalysisPairLegCuts *trk2leg = new PairAnalysisPairLegCuts("trk2leg","trk2leg");
    // move all track cuts (if any) into pair leg cuts
    TIter listIterator(fTrackFilter.GetCuts());
    while (AnalysisCuts *thisCut = (AnalysisCuts*) listIterator()) {
      trk2leg->GetLeg1Filter().AddCuts((AnalysisCuts*)thisCut->Clone());
      trk2leg->GetLeg2Filter().AddCuts((AnalysisCuts*)thisCut->Clone());
    }
    // add pair leg cuts to pair filter
    fPairFilter.AddCuts(trk2leg);
  }
  */

  // initialize simple cut qa
  if (fCutQA) {
    fQAmonitor = new PairAnalysisCutQA(Form("QAcuts_%s",GetName()),"QAcuts");
    fQAmonitor->AddTrackFilter(&fTrackFilter);
    if(!fNoPairing) fQAmonitor->AddPairFilter(&fPairFilter);
    fQAmonitor->AddEventFilter(&fEventFilter);
    fQAmonitor->Init();
  }

  if(fHistos) {
    (*fUsedVars)|= (*fHistos->GetUsedVars());
  }

}

//________________________________________________________________

void PairAnalysis::Process(TObjArray *arr)
{
  //
  // Process the pair array
  //

  // set pair arrays
  fPairCandidates = arr;

  // TODO: think of the following filling
  //fill debug tree if a manager is attached
  //  if (fDebugTree) FillDebugTree();
  //in case there is a histogram manager, fill the QA histograms
  //  if (fHistos && fSignalsMC) FillMCHistograms(ev1);

  // apply cuts and fill output
  if (fHistos) FillHistogramsFromPairArray();

  // never clear arrays !!!!

}

//________________________________________________________________
Bool_t PairAnalysis::Process(PairAnalysisEvent *ev1)
{
  //
  // Process the events
  //

  //at least first event is needed!
  if (!ev1->GetPrimaryVertex()){
    Fatal("PairAnalysis::Process","At least first event/vertex must be set!");
    return kFALSE;
  }

  // modify event numbers in MC so that we can identify new events
  // in PairAnalysisV0Cuts (not neeeded for collision data)
  /*
  if(GetHasMC()) {
    ev1->SetBunchCrossNumber(1);
    ev1->SetOrbitNumber(1);
    ev1->SetPeriodNumber(1);
  }
  */

  // set event
  PairAnalysisVarManager::SetFillMap(fUsedVars);
  PairAnalysisVarManager::SetEvent(ev1);
  /*
  if (fMixing){
    //set mixing bin to event data
    Int_t bin=fMixing->FindBin(PairAnalysisVarManager::GetData());
    PairAnalysisVarManager::SetValue(PairAnalysisVarManager::kMixingBin,bin);
  }
  */

  //in case we have MC load the MC event and process the MC particles
  // why do not apply the event cuts first ????
  //  if (PairAnalysisMC::Instance()->ConnectMCEvent()){
  if (PairAnalysisMC::Instance()->HasMC()){
    ProcessMC();
  }

  //if candidate array doesn't exist, create it
  if (!fPairCandidates->UncheckedAt(0)) {
    InitPairCandidateArrays();
  } else {
    ClearArrays();
  }

  //mask used to require that all cuts are fulfilled
  UInt_t selectedMask=(1<<fEventFilter.GetCuts()->GetEntries())-1;

  //apply event cuts
  UInt_t cutmask = fEventFilter.IsSelected(ev1);
  if (fCutQA) fQAmonitor->FillAll(ev1);
  if (fCutQA) fQAmonitor->Fill(cutmask,ev1);
  if (ev1 && cutmask!=selectedMask) return kFALSE;

  //fill track arrays for the first event
  FillTrackArrays(ev1);

  // prefilter track
  // if ( (fPreFilterAllSigns||fPreFilterUnlikeOnly) && (fPairPreFilter.GetCuts()->GetEntries()>0) )
  //   PairPreFilter(0, 1, fTracks[0], fTracks[1]);

  // event plane corrections
  // if ( fPreFilterEventPlane && (fEventPlanePreFilter.GetCuts()->GetEntries()>0 || fEventPlanePOIPreFilter.GetCuts()->GetEntries()>0) )
  //   EventPlanePreFilter(0, 1, fTracks[0], fTracks[1], ev1);

  // create SE pairs and fill pair candidate arrays
  if (!fNoPairing) {
    for (Int_t itrackArr1=0; itrackArr1<2; ++itrackArr1){
      for (Int_t itrackArr2=itrackArr1; itrackArr2<2; ++itrackArr2){
	if(!fProcessLS && GetPairIndex(itrackArr1,itrackArr2)!=kSEPM) continue;
	FillPairArrays(itrackArr1, itrackArr2);
      }
    }
    // add track rotated pairs
    if (fTrackRotator) {
      FillPairArrayTR();
    }
  }

  //process event mixing
  /*
  if (fMixing) {
    fMixing->Fill(ev1,this);
    //     FillHistograms(0x0,kTRUE);
  }
  */

  //fill debug tree if a manager is attached
  //  if (fDebugTree) FillDebugTree();

  // fill candidate variables
  Double_t ntracks = fTracks[0].GetEntriesFast() + fTracks[1].GetEntriesFast();
  Double_t npairs  = PairArray(PairAnalysis::kSEPM)->GetEntriesFast();
  PairAnalysisVarManager::SetValue(PairAnalysisVarManager::kTracks, ntracks);
  PairAnalysisVarManager::SetValue(PairAnalysisVarManager::kPairs,  npairs);

  //in case there is a histogram manager, fill the QA histograms
  if (fHistos)               FillHistograms(ev1);
  // fill histo array with event information only
  if (fHistoArray)
    fHistoArray->Fill(0,const_cast<Double_t *>(PairAnalysisVarManager::GetData()),0x0,0x0);

  // clear arrays
  if (!fDontClearArrays) ClearArrays();

  return kTRUE;
}

//________________________________________________________________
void PairAnalysis::ProcessMC()
{
  //
  // Process the MC data
  //
  PairAnalysisMC *papaMC=PairAnalysisMC::Instance();

  // fill mc true and rec event
  //  if (fHistos) FillHistogramsMC(papaMC->GetMCEvent(), ev1);

  // there are mc tracks
  if(!papaMC->GetNMCTracks()) return;

  // signals to be stupapad
  if(!fSignalsMC) return;
  Int_t nSignals = fSignalsMC->GetEntries();
  if(!nSignals) return;

  //loop over all MC data and Fill the HF, CF containers and histograms if they exist
  //  if(fCfManagerPair) fCfManagerPair->SetPdgMother(fPdgMother);

  Bool_t bFillCF   = kFALSE;//(fCfManagerPair ? fCfManagerPair->GetStepForMCtruth()  : kFALSE);
  Bool_t bFillHF   = kFALSE;
  Bool_t bFillHist = kFALSE;
  for(Int_t isig=0;isig<nSignals;isig++) {
    TString sigName = fSignalsMC->At(isig)->GetName();
    if(fHistos && !bFillHist) {
      bFillHist |= fHistos->HasHistClass(Form("Pair_%s_MCtruth",sigName.Data()));
      bFillHist |= fHistos->HasHistClass(Form("Track_Leg_%s_MCtruth",sigName.Data()));
      bFillHist |= fHistos->HasHistClass(Form("Track_%s_%s_MCtruth",fgkPairClassNames[1],sigName.Data()));
    }
    if(fHistoArray && !bFillHF) {
      bFillHF |= fHistoArray->HasHistClass(Form("Pair_%s_MCtruth",sigName.Data()));
      bFillHF |= fHistoArray->HasHistClass(Form("Track_Leg_%s_MCtruth",sigName.Data()));
      bFillHF |= fHistoArray->HasHistClass(Form("Track_%s_%s_MCtruth",fgkPairClassNames[1],sigName.Data()));
    }
  }

  // check if there is anything to fill
  if(!bFillCF && !bFillHF && !bFillHist) return;

  // initialize 2D arrays of labels for particles from each MC signal
  Int_t** labels1;      // labels for particles satisfying branch 1
  Int_t** labels2;      // labels for particles satisfying branch 2
  Int_t** labels12;     // labels for particles satisfying both branches
  labels1 = new Int_t*[nSignals];
  labels2 = new Int_t*[nSignals];
  labels12 = new Int_t*[nSignals];
  Int_t* indexes1=new Int_t[nSignals];
  Int_t* indexes2=new Int_t[nSignals];
  Int_t* indexes12=new Int_t[nSignals];
  for(Int_t isig=0;isig<nSignals;++isig) {
    *(labels1+isig) = new Int_t[papaMC->GetNMCTracks()];
    *(labels2+isig) = new Int_t[papaMC->GetNMCTracks()];
    *(labels12+isig) = new Int_t[papaMC->GetNMCTracks()];
    for(Int_t ip=0; ip<papaMC->GetNMCTracks();++ip) {
      labels1[isig][ip] = -1;
      labels2[isig][ip] = -1;
      labels12[isig][ip] = -1;
    }
    indexes1[isig]=0;
    indexes2[isig]=0;
    indexes12[isig]=0;
  }

  Bool_t truth1=kFALSE;
  Bool_t truth2=kFALSE;
  // loop over the MC tracks
  for(Int_t ipart=0; ipart<papaMC->GetNMCTracks(); ++ipart) {

    // selection of particles
    UInt_t selectedMask=(1<<fTrackFilterMC.GetCuts()->GetEntries())-1;
    //apply track cuts
    UInt_t cutmask=fTrackFilterMC.IsSelected(papaMC->GetMCTrackFromMCEvent(ipart));
    if (cutmask!=selectedMask) continue;

    // loop over signals
    for(Int_t isig=0; isig<nSignals; ++isig) {
      // Proceed only if this signal is required in the pure MC step
      // NOTE: Some signals can be satisfied by many particles and this leads to high
      //       computation times (e.g. secondary electrons from the GEANT transport). Be aware of this!!
      if(!((PairAnalysisSignalMC*)fSignalsMC->At(isig))->GetFillPureMCStep()) continue;

      truth1 = papaMC->IsMCTruth(ipart, (PairAnalysisSignalMC*)fSignalsMC->At(isig), 1);
      truth2 = papaMC->IsMCTruth(ipart, (PairAnalysisSignalMC*)fSignalsMC->At(isig), 2);

      // particles satisfying both branches are treated separately to avoid double counting during pairing
      if(truth1 && truth2) {
	labels12[isig][indexes12[isig]] = ipart;
	++indexes12[isig];
      }
      else {
	if(truth1) {
	  labels1[isig][indexes1[isig]] = ipart;
	  ++indexes1[isig];
	}
	if(truth2) {
	  labels2[isig][indexes2[isig]] = ipart;
	  ++indexes2[isig];
	}
      }
    }
  }  // end loop over MC particles

  // Do the pairing and fill the CF container with pure MC info
  // selection of MCtruth pairs
  UInt_t selectedMask=(1<<fPairFilterMC.GetCuts()->GetEntries())-1;
  // loop over signals
  for(Int_t isig=0; isig<nSignals; ++isig) {
    //    printf("INDEXES: %d-%d both%d\n",indexes1[isig],indexes2[isig],indexes12[isig]);

    // mix the particles which satisfy only one of the signal branches
    for(Int_t i1=0;i1<indexes1[isig];++i1) {
      CbmMCTrack* part1 = papaMC->GetMCTrackFromMCEvent(labels1[isig][i1]);
      Int_t mLabel1 = part1->GetMotherId();
      // (e.g. single electrons only, no pairs)
      if(!indexes2[isig]) FillMCHistograms(labels1[isig][i1], -1, isig);
      for(Int_t i2=0;i2<indexes2[isig];++i2) {

	//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
	// TODO: do pairing before filling anything, should be much faster
	// CbmMCTrack* part1 = papaMC->GetMCTrackFromMCEvent(labels1[isig][i1]);
	CbmMCTrack* part2 = papaMC->GetMCTrackFromMCEvent(labels2[isig][i2]);
	PairAnalysisPair* pair=0x0;
	CbmMCTrack*   mother=0x0;
	//	Int_t mLabel1 = part1->GetMotherId();
	Int_t mLabel2 = part2->GetMotherId();
	if(mLabel1==mLabel2) mother = papaMC->GetMCTrackFromMCEvent(mLabel1);

	// selection of MCtruth pairs
	UInt_t cutmask=0;
	if(mother)  cutmask=fPairFilterMC.IsSelected(mother);
	else {
	  pair = new PairAnalysisPairLV();
	  pair->SetMCTracks(part1,part2);
	  cutmask=fPairFilterMC.IsSelected(pair);
	  delete pair;
	}
	//apply MC truth pair cuts
	if (cutmask!=selectedMask) continue;
	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

	//if(bFillCF) fCfManagerPair->FillMC(labels1[isig][i1], labels2[isig][i2], isig);
	if(bFillHF)  fHistoArray->Fill(labels1[isig][i1], labels2[isig][i2], isig);
	if(bFillHist)FillMCHistograms(labels1[isig][i1], labels2[isig][i2], isig);

      }
    }
    // mix the particles which satisfy both branches
    for(Int_t i1=0;i1<indexes12[isig];++i1) {
      for(Int_t i2=0; i2<i1; ++i2) {
	// TODO: add pair cuts on mc truth level (SEE above)
	//if(bFillCF) fCfManagerPair->FillMC(labels12[isig][i1], labels12[isig][i2], isig);
	if(bFillHF) fHistoArray->Fill(labels12[isig][i1], labels12[isig][i2], isig);
	FillMCHistograms(labels12[isig][i1], labels12[isig][i2], isig);
      }
    }
  }    // end loop over signals

  // release the memory
  for(Int_t isig=0;isig<nSignals;++isig) {
    delete [] *(labels1+isig);
    delete [] *(labels2+isig);
    delete [] *(labels12+isig);
  }
  delete [] labels1;
  delete [] labels2;
  delete [] labels12;
  delete [] indexes1;
  delete [] indexes2;
  delete [] indexes12;

}

//________________________________________________________________
void PairAnalysis::FillHistogramsTracks(TObjArray **tracks)
{
  //
  // Fill Histogram information for tracks after prefilter
  // ignore mixed events - for prefilter, only single tracks +/- are relevant 
  //
  
  TString  className,className2;
  Double_t values[PairAnalysisVarManager::kNMaxValuesMC];
  PairAnalysisVarManager::SetFillMap(fUsedVars);

  //Fill track information, separately for the track array candidates
  for (Int_t i=0; i<2; ++i){
    className.Form("Pre_%s",fgkTrackClassNames[i]);
    if (!fHistos->GetHistogramList()->FindObject(className.Data())) continue;
    Int_t ntracks=tracks[i]->GetEntriesFast();
    for (Int_t itrack=0; itrack<ntracks; ++itrack){
      PairAnalysisVarManager::Fill(tracks[i]->UncheckedAt(itrack), values);
      fHistos->FillClass(className, values);
    }
  }
}


//________________________________________________________________
void PairAnalysis::FillHistogramsMC(const PairAnalysisEvent *ev, PairAnalysisEvent *ev1)
{
  //
  // Fill Histogram information for MCEvents
  //
  return;
  /*
  Double_t values[PairAnalysisVarManager::kNMaxValuesMC]={0.};
  PairAnalysisVarManager::SetFillMap(fUsedVars);

  // Fill event information
  PairAnalysisVarManager::Fill(ev1, values);    // ESD/AOD information
  PairAnalysisVarManager::Fill(ev, values);     // MC truth info
  if (fHistos->GetHistogramList()->FindObject("MCEvent"))
    fHistos->FillClass("MCEvent", values);
  */
}


//________________________________________________________________
void PairAnalysis::FillHistograms(const PairAnalysisEvent *ev, Bool_t pairInfoOnly)
{
  //
  // Fill Histogram information for event, pairs, tracks, hits
  //
  
  TString  className,className2,className3;
  TString  sigName;
  Double_t *values=PairAnalysisVarManager::GetData(); //NEW CHANGED
  /////////////
  PairAnalysisVarManager::SetFillMap(fUsedVars);

  //Fill event information
  if (ev){
    if (fHistos && fHistos->HasHistClass("Event"))
      fHistos      ->FillClass("Event", values);
    if (fHistoArray && fHistoArray->HasHistClass("Event"))
      fHistoArray ->FillClass("Event", values);
  }

  //Fill track information, separately for the track array candidates
  Int_t nsig = (fSignalsMC ? fSignalsMC->GetEntriesFast() : 0);
  TBits trkClassMC(  nsig);
  TBits trkClassMChf(nsig);
  TBits hitClassMC(  nsig);
  TBits hitClassMChf(nsig);
  TBits fillMC(  nsig);
  PairAnalysisMC *mc = (nsig ? PairAnalysisMC::Instance() : 0x0);
  PairAnalysisSignalMC *sigMC=0x0;
  if (!pairInfoOnly){
    className2.Form("Track_%s",fgkPairClassNames[1]);  // unlike sign, SE only
    Bool_t mergedtrkClass  =  fHistos->HasHistClass(className2);
    Bool_t mergedtrkClass2 = (fHistoArray && fHistoArray->HasHistClass(className2));
    // check mc signal filling
    for(Int_t isig=0; isig<nsig; isig++) {
      sigName = className2 + "_" + fSignalsMC->At(isig)->GetName();
      trkClassMC  .SetBitNumber(isig, fHistos->HasHistClass(sigName));
      trkClassMChf.SetBitNumber(isig, fHistoArray && fHistoArray->HasHistClass(sigName));
    }
    // loop over both track arrays
    for (Int_t i=0; i<2; ++i){
      className.Form("Track_%s",fgkTrackClassNames[i]);
      Bool_t trkClass  =  fHistos->HasHistClass(className);
      Bool_t trkClass2 = (fHistoArray && fHistoArray->HasHistClass(className));
      Bool_t fill = (mergedtrkClass || mergedtrkClass2 || trkClass || trkClass2 );
      // skip stuff not to be filled
      if (!fill && !trkClassMC.CountBits() && !trkClassMChf.CountBits()) continue;
      Int_t ntracks=fTracks[i].GetEntriesFast();
      for (Int_t itrack=0; itrack<ntracks; ++itrack){
	PairAnalysisTrack *track = static_cast<PairAnalysisTrack*>(fTracks[i].UncheckedAt(itrack));
        PairAnalysisVarManager::Fill(track, values);
        if(trkClass)	    fHistos    ->FillClass(className, values);
        if(trkClass2)	    fHistoArray->FillClass(className, values);
        if(mergedtrkClass)  fHistos    ->FillClass(className2, values);
        if(mergedtrkClass2) fHistoArray->FillClass(className2, values); //TODO: check only SE?
	// check and do mc signal filling
	for(Int_t isig=0; isig<nsig; isig++) {
	  if( !trkClassMC.TestBitNumber(isig) && !trkClassMChf.TestBitNumber(isig) ) continue;
	  // test if track does correspond to the signal
	  fillMC.SetBitNumber(isig,
			      mc->IsMCTruth(track, (PairAnalysisSignalMC*)fSignalsMC->At(isig), 1) ||
			      mc->IsMCTruth(track, (PairAnalysisSignalMC*)fSignalsMC->At(isig), 2) );
	  sigName = className2 + "_" + fSignalsMC->At(isig)->GetName();
	  // fill histos
	  if(fillMC.TestBitNumber(isig)) {
	    if(trkClassMC.TestBitNumber(isig))   fHistos     ->FillClass(sigName, values);
	    if(trkClassMChf.TestBitNumber(isig)) fHistoArray ->FillClass(sigName, values);
	  }
	}

	//Fill tracks hit information
	for (Int_t idet=kREF; idet<kNOFDETS; ++idet){
	  className3="Hit_"+PairAnalysisHelper::GetDetName(static_cast<DetectorId>(idet));  // detector hit
	  Bool_t hitClass  =  fHistos->HasHistClass(className3);
	  Bool_t hitClass2 = (fHistoArray && fHistoArray->HasHistClass(className3));
	  // check mc signal filling
	  for(Int_t isig=0; isig<nsig; isig++) {
	    sigName = className3 + "_" + fSignalsMC->At(isig)->GetName();
	    hitClassMC  .SetBitNumber(isig, fHistos->HasHistClass(sigName));
	    hitClassMChf.SetBitNumber(isig, fHistoArray && fHistoArray->HasHistClass(sigName));
	  }
	  if(!hitClass && !hitClass2 && !hitClassMC.CountBits() && !hitClassMChf.CountBits()) continue;

	  // get hit array
	  TClonesArray *hits = ev->GetHits(static_cast<DetectorId>(idet));

	  CbmTrack    *trkl = 0x0;
	  CbmRichRing *ring = 0x0;
	  switch(idet) {
	  case kSTS:
	  case kTRD:  trkl = track->GetTrack(static_cast<DetectorId>(idet)); break;
	  case kRICH: ring = track->GetRichRing();   break;
	  default:
	    continue;
	  }
	  // protection
	  if( (!trkl && !ring) || !hits) continue;

	  // loop over all hits
	  CbmHit   *hit  = 0x0;
	  Int_t nhits = (trkl ? trkl->GetNofHits() : ring->GetNofHits() );
	  for (Int_t ihit=0; ihit < nhits; ihit++){
	    if(trkl) hit = dynamic_cast<CbmHit*>(hits->At( trkl->GetHitIndex(ihit) ) );
	    else     hit = dynamic_cast<CbmHit*>(hits->At( ring->GetHit(ihit) ) );
	    if(!hit) continue;
	    // fill variables
	    PairAnalysisVarManager::Fill(hit, values);
	    if(hitClass)	    fHistos    ->FillClass(className3, values);
	    if(hitClass2)	    fHistoArray->FillClass(className3, values);
	    // check and fill mc signal histos
	    for(Int_t isig=0; isig<nsig; isig++) {
	      sigName = className3 + "_" + fSignalsMC->At(isig)->GetName();
	      if(fillMC.TestBitNumber(isig)) {
		if(hitClassMC.TestBitNumber(isig))   fHistos     ->FillClass(sigName, values);
		if(hitClassMChf.TestBitNumber(isig)) fHistoArray ->FillClass(sigName, values);
	      }
	    }

	    // only TRD for the moment
	    if(idet!=kTRD) continue;
	    // true or false hit
	    CbmTrackMatchNew *tmatch = track->GetTrackMatch(static_cast<DetectorId>(idet)); //mc
	    if(!tmatch) continue;
	    //	  Printf("Track match found");
	    Bool_t thitClass  =  fHistos->HasHistClass(className3+"_true");
	    Bool_t thitClass2 = (fHistoArray && fHistoArray->HasHistClass(className3+"_true"));
	    if(!thitClass && !thitClass2) continue;
	    const CbmMatch* hmatch = static_cast<CbmMatch*>(ev->GetTrdHitMatches()->At(trkl->GetHitIndex(ihit)));
	    //CbmMatch    *hmatch = hit->GetMatch(); // this somehow doesnot work
	    if(!hmatch) continue;
	    //Printf("Hit match found");
	    for (Int_t iLink = 0; iLink < hmatch->GetNofLinks(); iLink++) {
	      const FairMCPoint* point = static_cast<const FairMCPoint*>(ev->GetTrdPoints()->At(hmatch->GetLink(iLink).GetIndex())); //mc
	      if(!point) continue;
	      // fill mc truth information
	      PairAnalysisVarManager::Fill(point, values);
	      //	    Printf("                mc point  found");
	      // true hit or fake
	      if (point->GetTrackID() == tmatch->GetMatchedLink().GetIndex()) {
		if(thitClass)	    fHistos    ->FillClass(className3+"_true", values);
		if(thitClass2)	    fHistoArray->FillClass(className3+"_true", values);
	      }
	      else {
		if(thitClass)	    fHistos    ->FillClass(className3+"_fake", values);
		if(thitClass2)	    fHistoArray->FillClass(className3+"_fake", values);
	      }
	    } // link loop
	  } // hit loop
	} //det loop
      }
    }
  }
  
  //Fill Pair information, separately for all pair candidate arrays and the legs
  TBits legClassMC(  nsig);
  TBits legClassMChf(nsig);
  TBits pairClassMC(  nsig);
  TBits pairClassMChf(nsig);
  TObjArray arrLegs(100);
  for (Int_t i=0; i<7; ++i){
    className.Form("Pair_%s",fgkPairClassNames[i]);
    className2.Form("Track_Legs_%s",fgkPairClassNames[i]);
    Bool_t pairClass  =  fHistos->HasHistClass(className);
    Bool_t pairClass2 = (fHistoArray && fHistoArray->HasHistClass(className));
    Bool_t legClass   =  fHistos->HasHistClass(className2);
    Bool_t legClass2  = (fHistoArray && fHistoArray->HasHistClass(className2));

    // check mc signal filling
    if(i==kSEPM) {
      for(Int_t isig=0; isig<nsig; isig++) {
	sigName =  Form("Pair_%s",fSignalsMC->At(isig)->GetName());
	pairClassMC  .SetBitNumber(isig, fHistos->HasHistClass(sigName));
	pairClassMChf.SetBitNumber(isig, fHistoArray && fHistoArray->HasHistClass(sigName));
	//	Printf("fill %s: %d histos %d",sigName.Data(),pairClassMC.TestBitNumber(isig),fHistos->HasHistClass(sigName));
	sigName =  Form("Track_Legs_%s",fSignalsMC->At(isig)->GetName());
	legClassMC  .SetBitNumber(isig, fHistos->HasHistClass(sigName));
	legClassMChf.SetBitNumber(isig, fHistoArray && fHistoArray->HasHistClass(sigName));
      }
    }

    Bool_t fill = (pairClass || pairClass2 || legClass || legClass2 );
    if (!fill && !legClassMC.CountBits() && !legClassMChf.CountBits() &&
	!pairClassMC.CountBits() && !pairClassMChf.CountBits()) continue;

    // loop over all pairs
    Int_t npairs=PairArray(i)->GetEntriesFast();
    for (Int_t ipair=0; ipair<npairs; ++ipair){
      PairAnalysisPair *pair=static_cast<PairAnalysisPair*>(PairArray(i)->UncheckedAt(ipair));

      //fill pair information
      if (pairClass || pairClass2 || pairClassMC.CountBits() || pairClassMChf.CountBits()){
        PairAnalysisVarManager::Fill(pair, values);
        if(pairClass)  fHistos    ->FillClass(className, values);
        if(pairClass2) fHistoArray->FillClass(className, values);
	// check mc filling
	fillMC.ResetAllBits();
	for(Int_t isig=0; isig<nsig; isig++) {
	  sigMC = (PairAnalysisSignalMC*) fSignalsMC->At(isig);
	  Bool_t isMCtruth = mc->IsMCTruth(pair, sigMC);
	  fillMC.SetBitNumber(isig,isMCtruth);
	  if(!isMCtruth) continue;
	  sigName =  Form("Pair_%s",sigMC->GetName());
	  //	  Printf("fill %s: %d ",sigName.Data(),pairClassMC.TestBitNumber(isig));
	  PairAnalysisVarManager::SetValue(PairAnalysisVarManager::kWeight, sigMC->GetWeight());
	  if(pairClassMC.TestBitNumber(isig))   fHistos     ->FillClass(sigName, values);
	  if(pairClassMChf.TestBitNumber(isig)) fHistoArray ->FillClass(sigName, values);
	}
      }

      //fill leg information, don't fill the information twice
      if (legClass || legClass2 || legClassMC.CountBits() || legClassMChf.CountBits()){
        PairAnalysisTrack *d1=pair->GetFirstDaughter();
        PairAnalysisTrack *d2=pair->GetSecondDaughter();
        if (!arrLegs.FindObject(d1)){
          PairAnalysisVarManager::Fill(d1, values);
          if(legClass)  fHistos    ->FillClass(className2, values);
          if(legClass2) fHistoArray->FillClass(className2, values);
	  // mc signal filling
	  for(Int_t isig=0; isig<nsig; isig++) {
	    if(!fillMC.TestBitNumber(isig)) continue;
	    sigMC = (PairAnalysisSignalMC*) fSignalsMC->At(isig);
	    sigName = Form("Track_Legs_%s",sigMC->GetName());
	    PairAnalysisVarManager::SetValue(PairAnalysisVarManager::kWeight, sigMC->GetWeight());
	    if(legClassMC.TestBitNumber(isig))   fHistos     ->FillClass(sigName, values);
	    if(legClassMChf.TestBitNumber(isig)) fHistoArray ->FillClass(sigName, values);
	  }
	arrLegs.Add(d1);
        }
        if (!arrLegs.FindObject(d2)){
          PairAnalysisVarManager::Fill(d2, values);
          if(legClass)  fHistos    ->FillClass(className2, values);
          if(legClass2) fHistoArray->FillClass(className2, values);
	  // mc signal filling
	  for(Int_t isig=0; isig<nsig; isig++) {
	    if(!fillMC.TestBitNumber(isig)) continue;
	    sigMC = (PairAnalysisSignalMC*) fSignalsMC->At(isig);
	    sigName = Form("Track_Legs_%s",sigMC->GetName());
	    PairAnalysisVarManager::SetValue(PairAnalysisVarManager::kWeight, sigMC->GetWeight());
	    if(legClassMC.TestBitNumber(isig))   fHistos     ->FillClass(sigName, values);
	    if(legClassMChf.TestBitNumber(isig)) fHistoArray ->FillClass(sigName, values);
	  }
          arrLegs.Add(d2);
        }
      }
    }
    if (legClass || legClass2) arrLegs.Clear();
  }

}
//________________________________________________________________
void PairAnalysis::FillHistogramsPair(PairAnalysisPair *pair,Bool_t fromPreFilter/*=kFALSE*/)
{
  //
  // Fill Histogram information for pairs and the track in the pair
  // NOTE: in this funtion the leg information may be filled multiple
  //       times. This funtion is used in the track rotation pairing
  //       and those legs are not saved!
  //
  TString  className,className2;
  Double_t values[PairAnalysisVarManager::kNMaxValuesMC];
  PairAnalysisVarManager::SetFillMap(fUsedVars);

  //Fill Pair information, separately for all pair candidate arrays and the legs
  TObjArray arrLegs(100);
  const Int_t type=pair->GetType();
  if (fromPreFilter) {
    className.Form("RejPair_%s",fgkPairClassNames[type]);
    className2.Form("RejTrack_%s",fgkPairClassNames[type]);
  } else {
    className.Form("Pair_%s",fgkPairClassNames[type]);
    className2.Form("Track_Legs_%s",fgkPairClassNames[type]);
  }

  Bool_t pairClass  =  fHistos->HasHistClass(className);
  Bool_t pairClass2 = (fHistoArray && fHistoArray->HasHistClass(className));
  Bool_t legClass   =  fHistos->HasHistClass(className2);
  Bool_t legClass2  = (fHistoArray &&  fHistoArray->HasHistClass(className2));

  //fill pair information
  if (pairClass || pairClass2){
    PairAnalysisVarManager::Fill(pair, values);
    if(pairClass)   fHistos     ->FillClass(className, values);
    if(pairClass2)  fHistoArray ->FillClass(className, values);
  }

  if (legClass || legClass2){
    PairAnalysisTrack *d1=pair->GetFirstDaughter();
    PairAnalysisVarManager::Fill(d1, values);
    if(legClass)    fHistos     ->FillClass(className2, values);
    if(legClass2)   fHistoArray ->FillClass(className2, values);

    PairAnalysisTrack *d2=pair->GetSecondDaughter();
    PairAnalysisVarManager::Fill(d2, values);
    if(legClass)    fHistos     ->FillClass(className2, values);
    if(legClass2)   fHistoArray ->FillClass(className2, values);
  }

}

//________________________________________________________________
void PairAnalysis::FillTrackArrays(PairAnalysisEvent * const ev)
{
  //
  // select tracks and fill track candidate arrays
  // use track arrays 0 and 1
  //

  Int_t ntracks=ev->GetNumberOfTracks();

  UInt_t selectedMask=(1<<fTrackFilter.GetCuts()->GetEntries())-1;
  for (Int_t itrack=0; itrack<ntracks; ++itrack){
    //get particle
    PairAnalysisTrack *particle=ev->GetTrack(itrack);

    // store signal weights in the tracks - ATTENTION later signals should be more specific
    if(fHasMC && fSignalsMC) {
      PairAnalysisMC* papaMC = PairAnalysisMC::Instance();
      for(Int_t isig=0; isig<fSignalsMC->GetEntriesFast(); isig++) {
	PairAnalysisSignalMC *sig=(PairAnalysisSignalMC*)fSignalsMC->At(isig);
	if( papaMC->IsMCTruth(particle,sig,1) || papaMC->IsMCTruth(particle,sig,2) ) {
	  //   printf("signal weight for %s is %f \n",sig->GetName(),sig->GetWeight());
	  if(sig->GetWeight() != 1.0) particle->SetWeight( sig->GetWeight() );
	}
      }
    }

    //apply track cuts
    UInt_t cutmask=fTrackFilter.IsSelected(particle);
    //fill cut QA
    if(fCutQA) fQAmonitor->FillAll(particle);
    if(fCutQA) fQAmonitor->Fill(cutmask,particle);

    if (cutmask!=selectedMask) continue;

    //fill selected particle into the corresponding track arrays
    Short_t charge=particle->Charge();
    if (charge>0)      fTracks[0].Add(particle); // positive tracks
    else if (charge<0) fTracks[1].Add(particle); // negative tracks

  }
}

//________________________________________________________________
void PairAnalysis::PairPreFilter(Int_t arr1, Int_t arr2, TObjArray &arrTracks1, TObjArray &arrTracks2)
{
  //
  // Prefilter tracks from pairs
  // Needed for datlitz rejections
  // remove all tracks from the Single track arrays that pass the cuts in this filter
  //

  Int_t ntrack1=arrTracks1.GetEntriesFast();
  Int_t ntrack2=arrTracks2.GetEntriesFast();
  PairAnalysisPair *candidate;
  //  if(fUseKF) candidate = new PairAnalysisPairKF();
  //else
  candidate = new PairAnalysisPairLV();
  candidate->SetKFUsage(fUseKF);
  // flag arrays for track removal
  Bool_t *bTracks1 = new Bool_t[ntrack1];
  for (Int_t itrack1=0; itrack1<ntrack1; ++itrack1) bTracks1[itrack1]=kFALSE;
  Bool_t *bTracks2 = new Bool_t[ntrack2];
  for (Int_t itrack2=0; itrack2<ntrack2; ++itrack2) bTracks2[itrack2]=kFALSE;

  UInt_t selectedMask=(1<<fPairPreFilter.GetCuts()->GetEntries())-1;
  UInt_t selectedMaskPair=(1<<fPairFilter.GetCuts()->GetEntries())-1;

  Int_t nRejPasses = 1; //for fPreFilterUnlikeOnly and no set flag 
  if (fPreFilterAllSigns) nRejPasses = 3;

  for (Int_t iRP=0; iRP < nRejPasses; ++iRP) {
	Int_t arr1RP=arr1, arr2RP=arr2;
	TObjArray *arrTracks1RP=&arrTracks1;
	TObjArray *arrTracks2RP=&arrTracks2;
	Bool_t *bTracks1RP = bTracks1;
	Bool_t *bTracks2RP = bTracks2;
	switch (iRP) {
		case 1: arr1RP=arr1;arr2RP=arr1;
				arrTracks1RP=&arrTracks1;
				arrTracks2RP=&arrTracks1;
				bTracks1RP = bTracks1;
				bTracks2RP = bTracks1;
				break;
		case 2: arr1RP=arr2;arr2RP=arr2;
				arrTracks1RP=&arrTracks2;
				arrTracks2RP=&arrTracks2;
				bTracks1RP = bTracks2;
				bTracks2RP = bTracks2;
				break;
		default: ;//nothing to do
	}
	Int_t ntrack1RP=(*arrTracks1RP).GetEntriesFast();
	Int_t ntrack2RP=(*arrTracks2RP).GetEntriesFast();

	Int_t pairIndex=GetPairIndex(arr1RP,arr2RP);

	for (Int_t itrack1=0; itrack1<ntrack1RP; ++itrack1){
	  Int_t end=ntrack2RP;
	  if (arr1RP==arr2RP) end=itrack1;
	  for (Int_t itrack2=0; itrack2<end; ++itrack2){
		TObject *track1=(*arrTracks1RP).UncheckedAt(itrack1);
		TObject *track2=(*arrTracks2RP).UncheckedAt(itrack2);
		if (!track1 || !track2) continue;
		//create the pair
		candidate->SetTracks(static_cast<PairAnalysisTrack*>(track1), fPdgLeg1,
				     static_cast<PairAnalysisTrack*>(track2), fPdgLeg2);

		candidate->SetType(pairIndex);
		candidate->SetLabel(PairAnalysisMC::Instance()->GetLabelMotherWithPdg(candidate,fPdgMother));
		//relate to the production vertex
		//       if (PairAnalysisVarManager::GetKFVertex()) candidate->SetProductionVertex(*PairAnalysisVarManager::GetKFVertex());

		//pair cuts
		UInt_t cutMask=fPairPreFilter.IsSelected(candidate);

		//apply cut
		if (cutMask!=selectedMask) continue;
		//		if (fCfManagerPair) fCfManagerPair->Fill(selectedMaskPair+1 ,candidate);
		if (fHistos) FillHistogramsPair(candidate,kTRUE);
		//set flags for track removal
		bTracks1RP[itrack1]=kTRUE;
		bTracks2RP[itrack2]=kTRUE;
	  }
	}
  }
  //clear
  delete candidate;

  //remove the tracks from the Track arrays
  for (Int_t itrack1=0; itrack1<ntrack1; ++itrack1){
    if(bTracks1[itrack1]) arrTracks1.AddAt(0x0, itrack1);
  }
  for (Int_t itrack2=0; itrack2<ntrack2; ++itrack2){
    if(bTracks2[itrack2]) arrTracks2.AddAt(0x0, itrack2);
  }

  // clean up
  delete [] bTracks1;
  delete [] bTracks2;

  //compress the track arrays
  arrTracks1.Compress();
  arrTracks2.Compress();
  
  //apply leg cuts after the pre filter
  if ( fPairPreFilterLegs.GetCuts()->GetEntries()>0 ) {
    selectedMask=(1<<fPairPreFilterLegs.GetCuts()->GetEntries())-1;
    //loop over tracks from array 1
    for (Int_t itrack=0; itrack<arrTracks1.GetEntriesFast();++itrack){
      //test cuts
      UInt_t cutMask=fPairPreFilterLegs.IsSelected(arrTracks1.UncheckedAt(itrack));
      
      //apply cut
      if (cutMask!=selectedMask) arrTracks1.AddAt(0x0,itrack);
    }
    arrTracks1.Compress();
    
    //in case of like sign don't loop over second array
    if (arr1==arr2) {
      arrTracks2=arrTracks1;
    } else {
      
      //loop over tracks from array 2
      for (Int_t itrack=0; itrack<arrTracks2.GetEntriesFast();++itrack){
      //test cuts
        UInt_t cutMask=fPairPreFilterLegs.IsSelected(arrTracks2.UncheckedAt(itrack));
      //apply cut
        if (cutMask!=selectedMask) arrTracks2.AddAt(0x0,itrack);
      }
      arrTracks2.Compress();
      
    }
  }
  //For unlike-sign monitor track-cuts:
  if (arr1!=arr2&&fHistos) {
    TObjArray *unlikesignArray[2] = {&arrTracks1,&arrTracks2};
    FillHistogramsTracks(unlikesignArray);
  }
}

//________________________________________________________________
void PairAnalysis::FillPairArrays(Int_t arr1, Int_t arr2)
{
  //
  // select pairs and fill pair candidate arrays
  //

  TObjArray arrTracks1=fTracks[arr1];
  TObjArray arrTracks2=fTracks[arr2];

  //process pre filter if set
  //  if ((!fPreFilterAllSigns) && (!fPreFilterUnlikeOnly) && ( fPairPreFilter.GetCuts()->GetEntries()>0 ))  PairPreFilter(arr1, arr2, arrTracks1, arrTracks2);
  
  Int_t pairIndex=GetPairIndex(arr1,arr2);

  Int_t ntrack1=arrTracks1.GetEntriesFast();
  Int_t ntrack2=arrTracks2.GetEntriesFast();

  PairAnalysisPair *candidate;
  //  if(fUseKF) candidate = new PairAnalysisPairKF();
  //  else
  candidate = new PairAnalysisPairLV();
  candidate->SetKFUsage(fUseKF);

  UInt_t selectedMask=(1<<fPairFilter.GetCuts()->GetEntries())-1;
  
  for (Int_t itrack1=0; itrack1<ntrack1; ++itrack1){
    Int_t end=ntrack2;
    if (arr1==arr2) end=itrack1;
    for (Int_t itrack2=0; itrack2<end; ++itrack2){
      //create the pair (direct pointer to the memory by this daughter reference are kept also for ME)
      candidate->SetType(pairIndex);
      candidate->SetTracks(&(*static_cast<PairAnalysisTrack*>(arrTracks1.UncheckedAt(itrack1))), fPdgLeg1,
       			   &(*static_cast<PairAnalysisTrack*>(arrTracks2.UncheckedAt(itrack2))), fPdgLeg2);
      // TODO: maybe set here the mother pdg code and remove fPdgMother
      Int_t label=PairAnalysisMC::Instance()->GetLabelMotherWithPdg(candidate,fPdgMother);
      candidate->SetLabel(label);
      if (label>-1) candidate->SetPdgCode(fPdgMother);
      else candidate->SetPdgCode(0);

      //pair cuts
      UInt_t cutMask=fPairFilter.IsSelected(candidate);

      //CF manager for the pair
      //      if (fCfManagerPair) fCfManagerPair->Fill(cutMask,candidate);

      // cut qa
      if(pairIndex==kSEPM && fCutQA) {
	fQAmonitor->FillAll(candidate);
	fQAmonitor->Fill(cutMask,candidate);
      }

      //apply cut
      if (cutMask!=selectedMask) continue;

      //histogram array for the pair
      ////TODO: still needed?  if (fHistoArray) fHistoArray->Fill(pairIndex,candidate);

      //add the candidate to the candidate array
      PairArray(pairIndex)->Add(candidate);
      //get a new candidate
      //      if(fUseKF) candidate = new PairAnalysisPairKF();
      //else
      candidate = new PairAnalysisPairLV();
      candidate->SetKFUsage(fUseKF);
    }
  }
  //delete the surplus candidate
  delete candidate;
}

void PairAnalysis::FillPairArrayTR()
{
  //
  // rotate pairs and fill into pair candidate arrays
  //

  Int_t ntrack1=fTracks[0].GetEntriesFast();
  Int_t ntrack2=fTracks[1].GetEntriesFast();

  PairAnalysisPair *candidate;
  //  if(fUseKF) candidate = new PairAnalysisPairKF();
  //  else
  candidate = new PairAnalysisPairLV();
  candidate->SetKFUsage(fUseKF);
  candidate->SetType(kSEPMRot);

  UInt_t selectedMask=(1<<fPairFilter.GetCuts()->GetEntries())-1;
  // loop over track arrays
  for (Int_t itrack1=0; itrack1<ntrack1; ++itrack1){
    for (Int_t itrack2=0; itrack2<ntrack2; ++itrack2){

      // loop over iterations
      for (Int_t irot=0; irot<fTrackRotator->GetIterations(); ++irot){
	// build candidate
	candidate->SetTracks(&(*static_cast<PairAnalysisTrack*>(fTracks[0].UncheckedAt(itrack1))), fPdgLeg1,
			     &(*static_cast<PairAnalysisTrack*>(fTracks[1].UncheckedAt(itrack2))), fPdgLeg2);
	// rotate the candidates daughter track
	candidate->RotateTrack(fTrackRotator);

	//pair cuts
	UInt_t cutMask=fPairFilter.IsSelected(candidate);

	//apply cut
	if (cutMask!=selectedMask) continue;

	//histogram array for the pair
	if (fHistoArray) fHistoArray->Fill((Int_t)kSEPMRot,candidate);

	if(fHistos) FillHistogramsPair(candidate);
	if(fStoreRotatedPairs) {
	  //	  if(fUseKF) PairArray(kSEPMRot)->Add(static_cast<PairAnalysisPairKF*>(candidate->Clone()));
	  //	  else
	  PairArray(kSEPMRot)->Add(static_cast<PairAnalysisPairLV*>(candidate->Clone()));
	  // if(fUseKF) PairArray(kSEPMRot)->Add(new PairAnalysisPairKF(*candidate);
	  // else       PairArray(kSEPMRot)->Add(new PairAnalysisPairLV(*candidate);
	}

      }	// end of iterations
    } //arr0
  } //arr1

  //delete the surplus candidate
  delete candidate;

}


//________________________________________________________________
void PairAnalysis::FillDebugTree()
{
  //
  // Fill Histogram information for tracks and pairs
  //
  return;
  /*
  //Fill Debug tree
  for (Int_t i=0; i<7; ++i){
    Int_t ntracks=PairArray(i)->GetEntriesFast();
    for (Int_t ipair=0; ipair<ntracks; ++ipair){
      fDebugTree->Fill(static_cast<PairAnalysisPair*>(PairArray(i)->UncheckedAt(ipair)));
    }
  }
  */
}

//________________________________________________________________
void PairAnalysis::SaveDebugTree()
{
  //
  // delete the debug tree, this will also write the tree
  //
  //  if (fDebugTree) fDebugTree->DeleteStreamer();
}


//________________________________________________________________
void PairAnalysis::AddSignalMC(PairAnalysisSignalMC* signal) {
  //
  //  Add an MC signal to the signals list
  //
  if(!fSignalsMC) {
    fSignalsMC = new TObjArray();
    fSignalsMC->SetOwner();
  }
  fSignalsMC->Add(signal);
}

//________________________________________________________________
void PairAnalysis::FillMCHistograms(Int_t label1, Int_t label2, Int_t nSignal) {
  //
  // fill QA MC TRUTH histograms for pairs and legs of all added mc signals
  //

  TString className,className2,className3;
  className.Form("Pair_%s_MCtruth",       fSignalsMC->At(nSignal)->GetName());
  className2.Form("Track_Legs_%s_MCtruth",fSignalsMC->At(nSignal)->GetName());
  className3.Form("Track_%s_%s_MCtruth",fgkPairClassNames[1],fSignalsMC->At(nSignal)->GetName());
  Bool_t pairClass=fHistos->HasHistClass(className.Data());
  Bool_t legClass =fHistos->HasHistClass(className2.Data());
  Bool_t trkClass =fHistos->HasHistClass(className3.Data());
  //  printf("fill signal %d: pair %d legs %d trk %d \n",nSignal,pairClass,legClass,trkClass);
  if(!pairClass && !legClass && !trkClass) return;

  PairAnalysisMC* papaMC = PairAnalysisMC::Instance();
  CbmMCTrack* part1 = papaMC->GetMCTrackFromMCEvent(label1);
  CbmMCTrack* part2 = papaMC->GetMCTrackFromMCEvent(label2);
  if(!part1 && !part2) return;
  if(part1&&part2) {
    // fill only unlike sign (and only SE)
    if(part1->GetCharge()*part2->GetCharge()>=0) return;
  }

  Int_t mLabel1 = papaMC->GetMothersLabel(label1);
  Int_t mLabel2 = papaMC->GetMothersLabel(label2);
  //  printf("leg/mother labels: %d/%d %d/%d \n",label1,mLabel1,label2,mLabel2);

  // check the same mother option
  PairAnalysisSignalMC* sigMC = (PairAnalysisSignalMC*)fSignalsMC->At(nSignal);
  if(sigMC->GetMothersRelation()==PairAnalysisSignalMC::kSame      && mLabel1!=mLabel2) return;
  if(sigMC->GetMothersRelation()==PairAnalysisSignalMC::kDifferent && mLabel1==mLabel2) return;

  // fill event values
  Double_t *values = PairAnalysisVarManager::GetData(); //NEW CHANGED
  PairAnalysisVarManager::SetFillMap(fUsedVars);
  ///TODO:  PairAnalysisVarManager::Fill(papaMC->GetMCEvent(), values); // get event informations
  // fill the leg variables
  if (legClass || trkClass) {
    if(part1) PairAnalysisVarManager::Fill(part1,values);
    PairAnalysisVarManager::SetValue(PairAnalysisVarManager::kWeight, sigMC->GetWeight());
    if(part1 && trkClass)          fHistos->FillClass(className3, values);
    if(part1 && part2 && legClass) fHistos->FillClass(className2, values);
    if(part2) PairAnalysisVarManager::Fill(part2,values);
    PairAnalysisVarManager::SetValue(PairAnalysisVarManager::kWeight, sigMC->GetWeight());
    if(part2 && trkClass)          fHistos->FillClass(className3, values);
    if(part1 && part2 && legClass) fHistos->FillClass(className2, values);
  }

  //fill pair information
  if (pairClass && part1 && part2) {
    PairAnalysisVarManager::FillVarMCParticle(part1,part2,values);
    PairAnalysisVarManager::SetValue(PairAnalysisVarManager::kWeight, sigMC->GetWeight());
    fHistos->FillClass(className, values);
  }

}

//________________________________________________________________
void PairAnalysis::FillHistogramsFromPairArray(Bool_t pairInfoOnly/*=kFALSE*/)
{
  //
  // Fill Histogram information for tracks and pairs
  //

  TString  className,className2;
  Double_t values[PairAnalysisVarManager::kNMaxValuesMC]={0.};
  PairAnalysisVarManager::SetFillMap(fUsedVars);

  //Fill event information
  if(!pairInfoOnly) {
    if(fHistos->GetHistogramList()->FindObject("Event")) {
      fHistos->FillClass("Event", PairAnalysisVarManager::GetData());
    }
  }

  UInt_t selectedMask=(1<<fPairFilter.GetCuts()->GetEntries())-1;

  //Fill Pair information, separately for all pair candidate arrays and the legs
  TObjArray arrLegs(100);
  for (Int_t i=0; i<7; ++i){ // ROT pairs??
    Int_t npairs=PairArray(i)->GetEntriesFast();
    if(npairs<1) continue;

    className.Form("Pair_%s",fgkPairClassNames[i]);
    className2.Form("Track_Legs_%s",fgkPairClassNames[i]);
    Bool_t pairClass=fHistos->HasHistClass(className);
    Bool_t legClass=fHistos->HasHistClass(className2);

    //    if (!pairClass&&!legClass) continue;
    for (Int_t ipair=0; ipair<npairs; ++ipair){
      PairAnalysisPair *pair=static_cast<PairAnalysisPair*>(PairArray(i)->UncheckedAt(ipair));

      // apply cuts
      UInt_t cutMask=fPairFilter.IsSelected(pair);

      // cut qa
      if(i==kSEPM && fCutQA) {
	fQAmonitor->FillAll(pair);
	fQAmonitor->Fill(cutMask,pair);
      }

      //CF manager for the pair (todo: check steps and if they are properly filled)
      //      if (fCfManagerPair) fCfManagerPair->Fill(cutMask,pair);

      //apply cut
      if (cutMask!=selectedMask) continue;

      //histogram array for the pair
      if (fHistoArray) fHistoArray->Fill(i,pair);

      // fill map
      PairAnalysisVarManager::SetFillMap(fUsedVars);

      //fill pair information
      if (pairClass){
        PairAnalysisVarManager::Fill(pair, values);
        fHistos->FillClass(className, values);
      }

      //fill leg information, don't fill the information twice
      if (legClass){
        PairAnalysisTrack *d1=pair->GetFirstDaughter();
        PairAnalysisTrack *d2=pair->GetSecondDaughter();
        if (!arrLegs.FindObject(d1)){
          PairAnalysisVarManager::Fill(d1, values);
          fHistos->FillClass(className2, values);
          arrLegs.Add(d1);
        }
        if (!arrLegs.FindObject(d2)){
          PairAnalysisVarManager::Fill(d2, values);
          fHistos->FillClass(className2, values);
          arrLegs.Add(d2);
        }
      }
    }
    if (legClass) arrLegs.Clear();
  }

}

