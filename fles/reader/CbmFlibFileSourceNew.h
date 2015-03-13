// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                         CbmFlibFileSourceNew                      -----
// -----                    Created 07.11.2014 by F. Uhlig                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef CBMFLIBFILESOURCENEW_H
#define CBMFLIBFILESOURCENEW_H

#ifndef __CINT__
  #include "TimesliceSource.hpp"
  #include "Timeslice.hpp"
  #include "MicrosliceDescriptor.hpp"
  #include "Message.hpp"
#endif

#include "FairSource.h"
#include "CbmTSUnpack.h"

#include "TString.h"
#include "TClonesArray.h"
#include "TList.h"
#include "TObjString.h"
#include "TStopwatch.h"

#include <memory>
#include <map>

class CbmDaqBuffer;

class CbmFlibFileSourceNew : public FairSource
{
  public:
    CbmFlibFileSourceNew();
    CbmFlibFileSourceNew(const CbmFlibFileSourceNew& source);
    virtual ~CbmFlibFileSourceNew();

    Bool_t Init();
#ifdef _NewFairSource
    Int_t ReadEvent(UInt_t);
#else                
    Int_t ReadEvent();      
#endif    
    void Close();
    void Reset();

    void SetFileName(TString name) { fFileName = name; fInputFileList.Add(new TObjString(name));}
    void SetHostName(TString name) { fHost = name; }
    void SetPortNumber(Int_t port) { fPort = port; }

    void AddUnpacker(CbmTSUnpack* unpacker, Int_t id)
    { fUnpackers.insert ( std::pair<Int_t,CbmTSUnpack*>(id,unpacker) ); }    

    void AddFile(const char * name) {       
      fInputFileList.Add(new TObjString(name));
    }

  private:
  
    TString fFileName;
    TList   fInputFileList;    ///< List of input files
    Int_t   fFileCounter;
    TString fHost;
    Int_t   fPort;

    std::map<Int_t, CbmTSUnpack*> fUnpackers;

    CbmDaqBuffer* fBuffer;
 
    UInt_t fTSNumber;
    UInt_t fTSCounter;
    TStopwatch fTimer;

#ifndef __CINT__
    fles::TimesliceSource* fSource; //!
    Bool_t CheckTimeslice(const fles::Timeslice& ts);
    void PrintMicroSliceDescriptor(const fles::MicrosliceDescriptor& mdsc);
#endif

    CbmFlibFileSourceNew operator=(const CbmFlibFileSourceNew&);

    ClassDef(CbmFlibFileSourceNew, 1)
};


#endif
