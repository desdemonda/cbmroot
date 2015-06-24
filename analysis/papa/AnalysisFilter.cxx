/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 **************************************************************************/

//
// Manager class for filter decisions based on cuts
// The filter contains a list of sets of cuts.
// A bit field is filled in order to store the decision of each cut-set. 

#include <TObject.h>
#include <TList.h>
#include "AnalysisFilter.h"
#include "AnalysisCuts.h"


ClassImp(AnalysisFilter)


////////////////////////////////////////////////////////////////////////

AnalysisFilter::AnalysisFilter():
    TNamed(),
    fCuts(0)
{
  // Default constructor
}

AnalysisFilter::AnalysisFilter(const char* name, const char* title):
    TNamed(name, title),
    fCuts(new TList())
{
  // Constructor
}

AnalysisFilter::AnalysisFilter(const AnalysisFilter& obj):
    TNamed(obj),
    fCuts(0)
{
// Copy constructor
    fCuts = obj.fCuts;
}

AnalysisFilter::~AnalysisFilter()
{
// Destructor
   if (fCuts) fCuts->Delete("slow");
   delete fCuts;
}   

AnalysisFilter& AnalysisFilter::operator=(const AnalysisFilter& other)
{
// Assignment
   if (&other != this) {
	   TNamed::operator=(other);
	   fCuts = other.fCuts;
   }
   return *this;
   }
   
UInt_t AnalysisFilter::IsSelected(TObject* obj)
{
    //
    // Loop over all set of cuts
    // and store the decision
    UInt_t result = 0;
    UInt_t filterMask;
    
    TIter next(fCuts);
    AnalysisCuts *cuts;
    Int_t iCutB = 1;
	
    while((cuts = (AnalysisCuts*)next())) {
	Bool_t acc = cuts->IsSelected(obj);
	if ((filterMask = cuts->GetFilterMask()) > 0) {
	    acc = (acc && (filterMask == result));
	}
	cuts->SetSelected(acc);
	if (acc) {result |= iCutB & 0x00ffffff;}
	iCutB *= 2;
    }  

    return result;
}

UInt_t AnalysisFilter::IsSelected(TList* list)
{
    //
    // Loop over all set of cuts
    // and store the decision
    UInt_t result = 0;
    UInt_t filterMask;

    TIter next(fCuts);
    AnalysisCuts *cuts;
    Int_t iCutB = 1;
	
    while((cuts = (AnalysisCuts*)next())) {
	Bool_t acc = cuts->IsSelected(list);
	if ((filterMask = cuts->GetFilterMask()) > 0) {
	    acc = (acc && (filterMask & result));
	}
	cuts->SetSelected(acc);
	if (acc) {result |= iCutB & 0x00ffffff;}
	iCutB *= 2;
    }  

    return result;
}

void AnalysisFilter::Init()
{
    //
    // Loop over all set of cuts and call Init
    TIter next(fCuts);
    AnalysisCuts *cuts;
    while((cuts = (AnalysisCuts*)next())) cuts->Init();
}

void AnalysisFilter::AddCuts(AnalysisCuts* cuts)
{
    // Add a set of cuts
    fCuts->Add(cuts);
}

Bool_t AnalysisFilter::IsSelected(char* name)
{
    //
    // Returns current result for cut with name
    AnalysisCuts* cut = (AnalysisCuts*) (fCuts->FindObject(name));
    if (cut) {
      return (cut->Selected());
    } else  {
      return 0;
    }
}
