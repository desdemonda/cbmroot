#ifndef CBMNXFLIBFILESOURCE_H
#define CBMNXFLIBFILESOURCE_H 

#ifndef __CINT__
  #include "TimesliceSource.hpp"
  #include "Timeslice.hpp"
  #include "MicrosliceDescriptor.hpp"
  #include "Message.hpp"
#endif

#include "FairSource.h"

#include "TString.h"
#include "TClonesArray.h"

#include <memory>


class CbmNxFlibFileSource : public FairSource
{
  public:
    CbmNxFlibFileSource();
    CbmNxFlibFileSource(const CbmNxFlibFileSource& source);
    virtual ~CbmNxFlibFileSource();

    Bool_t Init();
    Int_t ReadEvent();
    void Close();
    void Reset();

    void SetFileName(TString name) { fFileName = name; }

  private:
  
    TString fFileName;

    Int_t fCurrEpoch;                  // Current epoch (first epoch in the stream initialises the variable sith sensible value)

    // --- Output arrays
    TClonesArray* fNxyterRaw;         ///< Output array of CbmNxyterRawMessage

#ifndef __CINT__
    fles::TimesliceSource* fSource; //!
    Bool_t CheckTimeslice(const fles::Timeslice& ts);
    void UnpackNxyterCbmNetMessage(const fles::Timeslice& ts, size_t component);
    void PrintMicroSliceDescriptor(const fles::MicrosliceDescriptor& mdsc);
#endif

    CbmNxFlibFileSource operator=(const CbmNxFlibFileSource&);

    ClassDef(CbmNxFlibFileSource, 1)
};


#endif // CBMNXFLIBFILESOURCE_H
