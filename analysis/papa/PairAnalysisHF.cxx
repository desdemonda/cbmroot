/*************************************************************************
* Copyright(c) 1998-2009, ALICE Experiment at CERN, All rights reserved. *
**************************************************************************/

///////////////////////////////////////////////////////////////////////////
//                PairAnalysis HF                                  //
//                                                                       //
//                                                                       //
/*
Detailed description


*/
//                                                                       //
///////////////////////////////////////////////////////////////////////////

#include <TVectorD.h>
#include <TH1.h>
#include <TH1F.h>
#include <TH2.h>
#include <TH3.h>
#include <TProfile.h>
#include <TProfile2D.h>
#include <TProfile3D.h>
#include <THnSparse.h>
#include <TAxis.h>
#include <TString.h>
#include <TObjString.h>
#include <TObjArray.h>
#include <TKey.h>

#include "CbmMCTrack.h"

#include "PairAnalysis.h"
#include "PairAnalysisHelper.h"
#include "PairAnalysisStyler.h"
#include "PairAnalysisMC.h"
#include "PairAnalysisPair.h"
#include "PairAnalysisSignalMC.h"

#include "PairAnalysisHistos.h"
#include "PairAnalysisHF.h"

#include "PairAnalysisTrack.h"

ClassImp(PairAnalysisHF)

PairAnalysisHF::PairAnalysisHF() :
//TNamed(),
  PairAnalysisHistos(),
  fUsedVars(new TBits(PairAnalysisVarManager::kNMaxValuesMC)),
  fArrDielHistos(),
  fSignalsMC(0x0),
  fVarCutType(new TBits(kMaxCuts)),
  fAxes(kMaxCuts)
{
  //
  // Default Constructor
  //
  for (Int_t i=0; i<kMaxCuts; ++i){
    fVarCuts[i]=0;
    //    fVarCutType[i]=0;
  }
  fAxes.SetOwner(kTRUE);
  fArrDielHistos.SetOwner(kTRUE);
}

//______________________________________________
PairAnalysisHF::PairAnalysisHF(const char* name, const char* title) :
  //  TNamed(name, title),
  PairAnalysisHistos(name, title),
  fUsedVars(new TBits(PairAnalysisVarManager::kNMaxValuesMC)),
  fArrDielHistos(),
  fSignalsMC(0x0),
  fVarCutType(new TBits(kMaxCuts)),
  fAxes(kMaxCuts)
{
  //
  // Named Constructor
  //
  for (Int_t i=0; i<kMaxCuts; ++i){
    fVarCuts[i]=0;
    //    fVarCutType[i]=0;
  }
  fAxes.SetOwner(kTRUE);
  fArrDielHistos.SetOwner(kTRUE);
}

//______________________________________________
PairAnalysisHF::~PairAnalysisHF()
{
  //
  // Default Destructor
  //
  if(fUsedVars)   delete fUsedVars;
  if(fVarCutType) delete fVarCutType;
  fAxes.Delete();
  fArrDielHistos.Delete(); //TODO: better Clear?
}

//________________________________________________________________
void PairAnalysisHF::AddCutVariable(PairAnalysisVarManager::ValueTypes type,
				  TVectorD * binLimits, Bool_t leg)
{
  //
  // Add a variable to the histogram array with a vector
  // the TVectorD is assumed to be surplus after the creation and will be deleted!!!
  //

  // limit number of variables to kMaxCuts
  if (fAxes.GetEntriesFast()>=kMaxCuts) return;

  if (!binLimits) return;

  Int_t size=fAxes.GetEntriesFast();
  fVarCuts[size]=(UShort_t)type;
  //  fVarCutType[size]=leg;
  fVarCutType->SetBitNumber(size,leg);
  fAxes.Add(binLimits);
  fUsedVars->SetBitNumber(type,kTRUE);
}

//_____________________________________________________________________________
void PairAnalysisHF::FillClass(const char* histClass, const Double_t *values)
{
  //
  // Fill the histograms if cuts are passed
  //

  // find cell described by values
  Int_t cell = FindCell(values);
  //  printf("cell: %d \n",cell);
  if (cell<0) return;
  //  printf("  --> %s \n",fArrDielHistos.UncheckedAt(cell)->GetName());

  // do NOT set the ownership otherwise you will delete all histos!!
  SetHistogramList(*static_cast<THashList*>(fArrDielHistos.UncheckedAt(cell)), kFALSE);
  PairAnalysisHistos::FillClass(histClass,values);

}

//______________________________________________
void PairAnalysisHF::ReadFromFile(const char* file, const char *task, const char *config)
{
  //
  // Read histos from file
  //
  // TODO: to be tested!
  TFile f(file);
  TIter nextKey(f.GetListOfKeys());
  TKey *key=0;
  while ( (key=(TKey*)nextKey()) ){
    TString name=key->GetName();
    if(!name.Contains("PairAnalysisHistos"))    continue;
    if(!strlen(task) && !name.Contains(task)) continue;
    TObject *o=f.Get(key->GetName());
    TList *list=dynamic_cast<TList*>(o);
    if (!list) continue;
    //////NEEDED?    else fList=list;
    TObjArray *listCfg=dynamic_cast<TObjArray*>(list->FindObject(Form("%s_HF",config)));
    if (!listCfg) continue;
    ////    SetHistogramList(*listCfg);
    break;
  }
  f.Close();
  // load style
  PairAnalysisStyler::LoadStyle();
}

//______________________________________________
void PairAnalysisHF::Fill(Int_t label1, Int_t label2, Int_t nSignal)
{
  return; //TODO: OBSOLETE?
  /*
  //
  // fill the pure MC part of the container starting from a pair of 2 particles (part1 and part2 are legs)
  //
  // fill only if we have asked for these steps
  if(!fStepGenerated || fEventArray) return;

  CbmMCTrack* part1 = PairAnalysisMC::Instance()->GetMCTrackFromMCEvent(label1);
  CbmMCTrack* part2 = PairAnalysisMC::Instance()->GetMCTrackFromMCEvent(label2);
  if(!part1 || !part2) return;

  PairAnalysisMC* papaMC = PairAnalysisMC::Instance();

  Int_t mLabel1 = papaMC->GetMothersLabel(label1);
  Int_t mLabel2 = papaMC->GetMothersLabel(label2);

  // check the same mother option
  PairAnalysisSignalMC* sigMC = (PairAnalysisSignalMC*)fSignalsMC->At(nSignal);
  if(sigMC->GetMothersRelation()==PairAnalysisSignalMC::kSame      && mLabel1!=mLabel2) return;
  if(sigMC->GetMothersRelation()==PairAnalysisSignalMC::kDifferent && mLabel1==mLabel2) return;

  PairAnalysisVarManager::SetFillMap(fUsedVars);
  // fill the leg variables
  Double_t valuesLeg1[PairAnalysisVarManager::kNMaxValuesMC];
  Double_t valuesLeg2[PairAnalysisVarManager::kNMaxValuesMC];
  PairAnalysisVarManager::Fill(part1,valuesLeg1);
  PairAnalysisVarManager::Fill(part2,valuesLeg2);

  // fill the pair and event variables
  Double_t valuesPair[PairAnalysisVarManager::kNMaxValuesMC];
  //TODO PairAnalysisVarManager::Fill(papaMC->GetMCEvent(), valuesPair);
  PairAnalysisVarManager::FillVarMCParticle(part1,part2,valuesPair);

  // if pair types are filled, fill mc sources at the end
  Int_t istep=0;
  if(fPairType!=kMConly) istep=PairAnalysis::kSEPMRot+1;

  // only OS at the moment
  if(part1->GetCharge()*part2->GetCharge()<0) {
    Fill(istep+nSignal+fSignalsMC->GetEntries(), valuesPair,  valuesLeg1, valuesLeg2);
  }

  return;
  */
}
//______________________________________________
void PairAnalysisHF::Fill(Int_t pairIndex, const PairAnalysisPair *particle)
{
  //
  // fill histograms for event, pair and daughter cuts and pair types
  //
  return; //TODO: OBSOLETE?
  /*
  // only OS pairs in case of MC
  //////////////////////////////  if(fHasMC && pairIndex!=PairAnalysis::kSEPM) return;

  // only selected pair types in case of data
  if(!IsPairTypeSelected(pairIndex) || fEventArray) return;

  // get event and pair variables
  Double_t valuesPair[PairAnalysisVarManager::kNMaxValuesMC];
  PairAnalysisVarManager::SetFillMap(fUsedVars);
  PairAnalysisVarManager::Fill(particle,valuesPair);

  // get leg variables (TODO: do not fill for the moment since leg cuts are not opened)
  Double_t valuesLeg1[PairAnalysisVarManager::kNMaxValuesMC]={0};
  if(fVarCutType->CountBits())  PairAnalysisVarManager::Fill(particle->GetFirstDaughter(),valuesLeg1);
  Double_t valuesLeg2[PairAnalysisVarManager::kNMaxValuesMC]={0};
  if(fVarCutType->CountBits())  PairAnalysisVarManager::Fill(particle->GetSecondDaughter(),valuesLeg2);

  // fill

  // if pair types are filled, fill mc sources at the end
  Int_t istep = 0;
  if(fPairType!=kMConly) istep=PairAnalysis::kSEPMRot+1;

  // mc source steps (only OS SE pairs)
  if(fHasMC && fSignalsMC && pairIndex==PairAnalysis::kSEPM) {
    for(Int_t i=0; i<fSignalsMC->GetEntries(); i++) {
      ////////////      if(PairAnalysisMC::Instance()->IsMCTruth(particle, (PairAnalysisSignalMC*)fSignalsMC->At(i)))
	Fill(istep+i, valuesPair,  valuesLeg1, valuesLeg2);
    }
  }

  // all pair types w/o use of mc information
  if(fPairType==kMConly) return;

  // remove comments
  //// select correct step if we are looking at signals too
  ////  if(fHasMC && fSignalsMC) pairIndex += ( fSignalsMC->GetEntries() * (fStepGenerated ? 2 : 1) );
  Fill(pairIndex, valuesPair,  valuesLeg1, valuesLeg2); 

  return;
  */
}

//______________________________________________
void PairAnalysisHF::Fill(Int_t index, Double_t * const valuesPair, Double_t * const valuesLeg1, Double_t * const valuesLeg2)
{
  //
  // main fill function using index and values as input
  //
  return; //TODO: OBSOLETE?
  /*
  TObjArray *histArr = static_cast<TObjArray*>(fArrDielHistos.At(index));
  if(!histArr) return;

  Int_t size  = GetNumberOfBins();
  // loop over all histograms
  for(Int_t ihist=0; ihist<size; ihist++) {

    Int_t sizeAdd   = 1;
    Bool_t selected = kTRUE;
    
    // loop over all cut variables
    Int_t nvars = fAxes.GetEntriesFast();
    for(Int_t ivar=0; ivar<nvars; ivar++) {
      
      // get bin limits
      TVectorD *bins = static_cast<TVectorD*>(fAxes.At(ivar));
      Int_t nbins    = bins->GetNrows()-1;

      // bin limits for current ivar bin
      Int_t ibin   = (ihist/sizeAdd)%nbins;
      Double_t lowEdge = (*bins)[ibin];
      Double_t upEdge  = (*bins)[ibin+1];
      switch(fBinType[ivar]) {
      case kStdBin:     upEdge=(*bins)[ibin+1];     break;
      case kBinToMax:   upEdge=(*bins)[nbins];      break;
      case kBinFromMin: lowEdge=(*bins)[0];         break;
      case kSymBin:     upEdge=(*bins)[nbins-ibin];
	if(ibin>=((Double_t)(nbins+1))/2) upEdge=(*bins)[nbins]; // to avoid low>up
	break;
      }

      // leg variable
      if(fVarCutType->TestBitNumber(ivar)) {
	if( (valuesLeg1[fVarCuts[ivar]] < lowEdge || valuesLeg1[fVarCuts[ivar]] >= upEdge) ||
	    (valuesLeg2[fVarCuts[ivar]] < lowEdge || valuesLeg2[fVarCuts[ivar]] >= upEdge) ) {
	  selected=kFALSE;
	  break;
	}
      }
      else { // pair and event variables
	if( (valuesPair[fVarCuts[ivar]] < lowEdge || valuesPair[fVarCuts[ivar]] >= upEdge) ) {
	  selected=kFALSE;
	  break;
	}
      }

      sizeAdd*=nbins;
    } //end of var cut loop

    // do not fill the histogram
    if(!selected) continue;

    // fill the object with Pair and event values
    TObjArray *tmp = (TObjArray*) histArr->At(ihist);
    TString title = tmp->GetName();
    ////    Debug(10,title.Data());
    for(Int_t i=0; i<tmp->GetEntriesFast(); i++) {
      PairAnalysisHistos::FillValues(tmp->At(i), valuesPair);
    }
    //    Debug(10,Form("Fill var %d %s value %f in %s \n",fVar,PairAnalysisVarManager::GetValueName(fVar),valuesPair[fVar],tmp->GetName()));
  } //end of hist loop
  */
}

//______________________________________________
void PairAnalysisHF::Init()
{
  //
  // initialise the HF array
  //

  // create an TObjArray of 'size' with PairAnalysisHistos objects
  Int_t size  = GetNumberOfBins();

  fArrDielHistos.SetName(Form("%s_HF",GetName()));
  fArrDielHistos.Expand(size);
  ////  Debug(10,Form("Creating a histo array with size %d \n",size));

  // add 'PairAnalysisHistos'-list to each cell
  for(Int_t icell=0; icell<size; icell++) {
    fArrDielHistos.AddAt(fHistoList.Clone(""), icell);
  }

  // loop over all cut variables and do the naming according to its bin cell
  Int_t sizeAdd  = 1; // counter for processed cells
  Int_t nvars    = fAxes.GetEntriesFast();
  for(Int_t ivar=0; ivar<nvars; ivar++) {

    // get bin limits for variabvle ivar
    TVectorD *bins = static_cast<TVectorD*>(fAxes.At(ivar));
    Int_t nbins    = bins->GetNrows()-1;

    // loop over all cells
    // Add 'variable' name and current 'bin limits' to
    // the title of the 'PairAnalysisHistos'-list title
    // which is unique
    for(Int_t icell=0; icell<size; icell++) {

      // get the lower limit for current ivar bin
      Int_t ibin       = (icell/sizeAdd)%nbins;
      Double_t lowEdge = (*bins)[ibin];
      Double_t upEdge  = (*bins)[ibin+1];

      // modify title of hashlist
      TString title = fArrDielHistos.UncheckedAt(icell)->GetName();
      if(!ivar)       title ="";
      else            title+=":";                        // delimiter for new variable
      if(fVarCutType->TestBitNumber(ivar)) title+="Leg"; // for leg variable Identification
      title += PairAnalysisVarManager::GetValueName(fVarCuts[ivar]);
      title += Form("#%.2f#%.2f",lowEdge,upEdge);        // TODO: precision enough?
      static_cast<THashList*>(fArrDielHistos.UncheckedAt(icell))->SetName(title.Data());
      ///      Debug(10,title.Data());

    } // end: array of cells

    sizeAdd*=nbins;

  } //end: loop variables

  // clean up
  // if(fArrDielHistos) {
  //   fArrDielHistos.Delete();
  //   fArrDielHistos=0;
  // }

}

//______________________________________________
Int_t PairAnalysisHF::GetNumberOfBins() const
{
  //
  // return the number of bins this histogram grid has
  //
  Int_t size=1;
  for (Int_t i=0; i<fAxes.GetEntriesFast(); ++i)
    size*=((static_cast<TVectorD*>(fAxes.At(i)))->GetNrows()-1);
  return size;
}

//______________________________________________
Int_t PairAnalysisHF::FindCell(const Double_t *values)
{
  //
  // cell described by 'values'
  // if the values are outside the binning range -1 is returned
  //

  if (fAxes.GetEntriesFast()==0) return 0;

  Int_t sizeAdd=1;
  Int_t cell=0;
  for (Int_t i=0; i<fAxes.GetEntriesFast(); ++i){
    Double_t val=values[fVarCuts[i]];
    TVectorD *bins=static_cast<TVectorD*>(fAxes.At(i));
    Int_t nRows=bins->GetNrows();
    if ( (val<(*bins)[0]) || (val>(*bins)[nRows-1]) ) {
      return -1;
    }

    Int_t pos=TMath::BinarySearch(nRows,bins->GetMatrixArray(),val);
    cell+=sizeAdd*pos;
    // printf(" \t %s: %.2f-%.2f %d \n",
    // 	   PairAnalysisVarManager::GetValueName(fVarCuts[i]), (*bins)[pos], (*bins)[pos+1], pos);
    sizeAdd*=(nRows-1);
  }

  return cell;
}
