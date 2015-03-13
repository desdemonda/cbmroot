// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                             CbmRocSource                          -----
// -----                    Created 05.11.2013 by F. Uhlig                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef CBMROCSOURCE_H
#define CBMROCSOURCE_H

#include "FairSource.h"

#include "roc/Message.h"
#include "roc/Iterator.h"

#include <map>
#include "TString.h"

class CbmDaqMap;
class CbmHistManager;

class CbmRocSource : public FairSource
{
  public:
    CbmRocSource();
    CbmRocSource(const char *);
    CbmRocSource(const CbmRocSource& source);
    virtual ~CbmRocSource();

    virtual Bool_t Init();
    virtual Int_t ReadEvent();
    virtual void Close();

    virtual void Reset();

    void AddFile(const char * fname) { fInputFileName = fname;}

 private:

    TString  fInputFileName;        /** Name of input LMD file **/
    Int_t    fNofMessages;          /** Number of processed ROC messages **/
    std::map<Int_t, uint32_t> fCurrentEpoch;  /** Current epoch marker for each ROC **/
//    uint32_t fCurrentEpoch;         /** Current epoch marker **/
    ULong_t fCurrentEpochTime;     /** Time stamp of current epoch **/
    Int_t    fNofEpochs;            /** Number of epoch messages **/
    CbmDaqMap* fDaqMap;       /** Mapping from electronics to detectors **/

    ULong_t fTimeStart;
    ULong_t fTimeStop;

    roc::Iterator* fRocIter;             //! ROC iterator
    roc::Message*  fCurrentMessage;      //! Current ROC message

    Int_t fNofMessType[8];     // Number of message per message type
    Int_t fNofMessRoc[20][5];  // Number of messages per ROC and NXYTER

    void ProcessEpochMarker();
    void ProcessHodoMessage();
    void ProcessMuchMessage();
    void ProcessStsMessage();


    CbmHistManager* fHM;

    ClassDef(CbmRocSource, 0)
};


#endif
