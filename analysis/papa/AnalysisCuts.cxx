/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 **************************************************************************/

// Base class for analysis cuts

#include <TObject.h>
#include "AnalysisCuts.h"


ClassImp(AnalysisCuts)


////////////////////////////////////////////////////////////////////////

AnalysisCuts::AnalysisCuts():
TNamed("Cuts",""), fFilterMask(0), fSelected(kFALSE)
{
  // Default constructor
}

AnalysisCuts::AnalysisCuts(const char* name, const char* title):
    TNamed(name, title), fFilterMask(0), fSelected(kFALSE)
{
  // Constructor
}

AnalysisCuts::AnalysisCuts(const AnalysisCuts& obj):
    TNamed(obj), fFilterMask(obj.fFilterMask), fSelected(obj.fSelected)
{
}

AnalysisCuts& AnalysisCuts::operator=(const AnalysisCuts& obj)
{
  // Assignment operator
  if ( this != &obj ) { 
    TNamed::operator=(obj);
    fFilterMask = obj.fFilterMask;
    fSelected = obj.fSelected;
  }
  return *this;
}

