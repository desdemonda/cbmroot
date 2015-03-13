// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                             CbmTestSource                         -----
// -----                    Needed during CMake run to find definitions    -----   
// -----                    Created 24.02.2015 by F. Uhlig                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef CBMTESTSOURCE_H
#define CBMTESTSOURCE_H

#include "FairSource.h"

class CbmTestSource : public FairSource
{
  public:
    CbmTestSource();
    CbmTestSource(const CbmTestSource& source);
    virtual ~CbmTestSource();

    Bool_t Init();
    Int_t ReadEvent(UInt_t);
    void Close();

    void Reset();

  public:
    ClassDef(CbmTestSource, 0)
};


#endif
