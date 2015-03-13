#ifndef CBMTRBEDGEMATCHER_H
#define CBMTRBEDGEMATCHER_H

#include <map>

#include "FairTask.h"

// The TDC channels of which are draw for debug
#define DEBUGTDCID 0x0081

class TH1D;
class TClonesArray;

class CbmTrbEdgeMatcher : public FairTask
{
public: // methods

   CbmTrbEdgeMatcher();
   ~CbmTrbEdgeMatcher();

   virtual InitStatus Init();
   virtual void Exec(Option_t* option);
   virtual void FinishEvent();
   virtual void FinishTask();

   void SetDrawHits(Bool_t toDraw = kTRUE) { fDrawHist = toDraw; }

private: // methods

   // Add the leadgin edge into the buffer
   void AddPossibleLeadingEdge(UInt_t tdcId, UInt_t lChannel, UInt_t lEpoch, UInt_t lDataWord);

   // Return true if the corresponding leading edge has been found in the buffer
   Bool_t FindLeadingEdge(UInt_t tdcId, UInt_t lChannel,
                                 UInt_t tEpoch, UInt_t tCoarse,
                                 UInt_t* lEpoch, UInt_t* lWord);

   // Create an output digi with only leading edge
   void CreateLeadingEdgeOnlyDigi(UInt_t tdcId, UInt_t lepoch, UInt_t lword);

   //TODO move this method somewhere else? z.b. Calibrator?
   Double_t GetFullTime(UInt_t tdcId, UInt_t channel, UInt_t epoch, UInt_t coarse, UInt_t fine);

   void DrawDebugHistos();

private: // data members

   // Input raw hits from the unpacker
   TClonesArray* fTrbRawHits;

   // Output pairs (if possible) of leading and trailing edges
   TClonesArray* fRichTrbDigi;

   // The size of the buffer for the leading edges
   UInt_t BUFSIZE;

   // 33 channels for each tdc * BUFSIZE edges stored for each channel * 2 (e+tw)
   std::map<UInt_t, UInt_t*> tdcIdToStoredEdges;
   std::map<UInt_t, UInt_t> circularCounters;

   // --- Debug histograms

   // If true - draw debug histograms
   Bool_t fDrawHist;

   // For each possible TDCid
   TH1D* fhTtimeMinusLtime[68];

   // For all the channels of a single certain TDC with id DEBUGTDCID
   TH1D* fhTtimeMinusLtimeCH[16];

   CbmTrbEdgeMatcher(const CbmTrbEdgeMatcher&);
   CbmTrbEdgeMatcher operator=(const CbmTrbEdgeMatcher&);

   ClassDef(CbmTrbEdgeMatcher,1)
};

#endif // CBMTRBEDGEMATCHER_H
