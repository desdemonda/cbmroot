#ifndef CBMRICHTRBUNPACK2_H
#define CBMRICHTRBUNPACK2_H

#include <TString.h>
#include "FairSource.h"

#include <vector>
#include <list>
#include <map>

class CbmRawSubEvent;
class TClonesArray;
class CbmTrbIterator;

class CbmRichTrbUnpack2 : public FairSource
{
public: // methods

   CbmRichTrbUnpack2();
   CbmRichTrbUnpack2(TString hldFileName);
   virtual ~CbmRichTrbUnpack2();

   Bool_t Init();
#ifdef _NewFairSource
   Int_t ReadEvent(UInt_t);
#else                    
   Int_t ReadEvent();    
#endif                    
   void Close();
   void Reset();

   // Add filename to the list of input files to be processed during the run
   void AddInputFile(TString filename) { fHldFiles.push_back(filename); }

private: // methods

   // Try to read the next input file. Return 0 in case of success, otherwize 1.
   UInt_t ReadNextInputFileToMemory();

   void ProcessSubEvent(CbmRawSubEvent* subEvent);
   void ProcessSubSubEvent(UInt_t* data, UInt_t size, UInt_t subEventId);

   //TODO implement or get rid
   CbmRichTrbUnpack2(const CbmRichTrbUnpack2&);
   CbmRichTrbUnpack2 operator=(const CbmRichTrbUnpack2&);

private: // data members

   // List of input HLD files to be processed
   std::list<TString> fHldFiles;

   // Counter of processed raw events
   UInt_t fNofRawEvents;

   // Limiter on the number of raw events to be processed
   UInt_t fMaxNofRawEvents;

   // Pointer and the size of the data in the memory (read from file) which is being processed
   Char_t* fDataPointer;
   UInt_t fDataSize;

   // Iterator over the CbmNet messages - ?
   CbmTrbIterator* fTrbIter;

   // Output raw hits
   TClonesArray* fTrbRawHits;

   ClassDef(CbmRichTrbUnpack2,1)
};

#endif // CBMRICHTRBUNPACK2_H
