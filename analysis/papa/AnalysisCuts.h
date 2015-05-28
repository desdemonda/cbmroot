#ifndef ANALYSISCUTS_H
#define ANALYSISCUTS_H
 
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

#include <TNamed.h>

class TList;
class TCollection;

class AnalysisCuts : public TNamed
{
 public:
    AnalysisCuts();
    AnalysisCuts(const char* name, const char* title);
    AnalysisCuts(const AnalysisCuts& obj);
    AnalysisCuts& operator=(const AnalysisCuts& obj);
    virtual ~AnalysisCuts() {;}
    virtual Bool_t IsSelected(TObject* /* obj  */ )  {return kFALSE;}
    virtual Bool_t IsSelected(TList*   /* list */ ) = 0;
    virtual void   Init() {;}
    virtual void   SetFilterMask(UInt_t mask) {fFilterMask = mask;}
    virtual UInt_t GetFilterMask()   const    {return fFilterMask;}
    virtual void   SetSelected(Bool_t dec)    {fSelected = dec;}
    virtual UInt_t Selected()        const    {return fSelected;}

    // vvvvv NEEDED ?? vvvvv
    virtual Long64_t Merge(TCollection* /* list */)      { return 0; }
    virtual TList* GetOutput()                { return 0; }
    virtual TObject *GetStatistics(Option_t *) const {return 0;} //TODO: maybe use to pass histogram with statistics
 private:
    UInt_t fFilterMask;        // Mask to use one of the previous decisions inside a filter
    Bool_t fSelected;          // Final decision on selction
    ClassDef(AnalysisCuts, 1); // Base class for filter decisions
};
 
#endif
