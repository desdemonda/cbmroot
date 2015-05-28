#ifndef ANALYSISFILTER_H
#define ANALYSISFILTER_H
 
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

#include <TNamed.h>

class AnalysisCuts;
class TList;

class AnalysisFilter : public TNamed
{
 public:
    AnalysisFilter();
    AnalysisFilter(const char* name, const char* title = "AnalysisFilter");
    AnalysisFilter(const AnalysisFilter& obj);
    AnalysisFilter& operator=(const AnalysisFilter& other);
    virtual ~AnalysisFilter();
    virtual UInt_t IsSelected(TObject* obj);
    virtual UInt_t IsSelected(TList* obj);
    virtual Bool_t IsSelected(char* name);
    virtual void AddCuts(AnalysisCuts* cuts);
    virtual void Init();
    TList*  GetCuts() const {return fCuts;}

    //
    //    virtual void     Print(Option_t *option="") const;

 private:
    TList* fCuts;    // List of cuts
    ClassDef(AnalysisFilter, 1); // Manager class for filter decisions
};
 
#endif
