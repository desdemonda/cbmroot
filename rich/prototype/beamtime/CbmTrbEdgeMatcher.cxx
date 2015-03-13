#include "CbmTrbEdgeMatcher.h"


//TODO check which headers are really needed
#include <cstdlib>

#include <TClonesArray.h>
#include <TH1D.h>
#include <TCanvas.h>
#include "FairRootManager.h"
#include "FairLogger.h"

#include "CbmRichTrbParam.h"
#include "CbmRichTrbDigi.h"
#include "CbmTrbRawMessage.h"
#include "CbmTrbCalibrator.h"

CbmTrbEdgeMatcher::CbmTrbEdgeMatcher() 
  : FairTask(),
    fTrbRawHits(NULL), 
    fRichTrbDigi(new TClonesArray("CbmRichTrbDigi", 10)),
    BUFSIZE(16),
    tdcIdToStoredEdges(),
    circularCounters(),
    fDrawHist(kFALSE),
    fhTtimeMinusLtime(),
    fhTtimeMinusLtimeCH()
{
}

CbmTrbEdgeMatcher::~CbmTrbEdgeMatcher()
{
}

InitStatus CbmTrbEdgeMatcher::Init()
{
   FairRootManager* manager = FairRootManager::Instance();
   fTrbRawHits = (TClonesArray*)manager->GetObject("CbmTrbRawMessage");
   if (NULL == fTrbRawHits) { Fatal("CbmTrbEdgeMatcher","No CbmTrbRawMessage array!"); }

   manager->Register("CbmRichTrbDigi", "RICH TRB leading-trailing edge pairs", fRichTrbDigi, kTRUE);

   std::map<UInt_t, UInt_t*>::iterator tdcIdToStoredEdgesI;
   for (tdcIdToStoredEdgesI = tdcIdToStoredEdges.begin(); tdcIdToStoredEdgesI != tdcIdToStoredEdges.end(); ++tdcIdToStoredEdgesI) {
      if (tdcIdToStoredEdgesI->second) delete [] tdcIdToStoredEdgesI->second;
   }

   if (fDrawHist)
   {
      CbmRichTrbParam* param = CbmRichTrbParam::Instance();

      TString histoName;
      TString histoTitle;

      for (UInt_t i=0; i<68; i++) {
         histoName.Form("fhTtimeMinusLtime - TDC %04x", param->IntegerToTDCid(i));
         histoTitle.Form("(Trailing time - Leading time) for TDC %04x", param->IntegerToTDCid(i));
         fhTtimeMinusLtime[i] = new TH1D(histoName, histoTitle, 4000, -50., 50.);
      }

      for (UInt_t i=0; i<16; i++) {
         histoName.Form("fhTtimeMinusLtime - TDC %04x CH%d - CH%d", DEBUGTDCID, i*2+2, i*2+1);
         histoTitle.Form("(Trailing time - Leading time) for TDC %04x CH%d - CH%d", DEBUGTDCID, i*2+2, i*2+1);
         fhTtimeMinusLtimeCH[i] = new TH1D(histoName, histoTitle, 4000, -50., 50.);
      }
   }

   return kSUCCESS;
}

void CbmTrbEdgeMatcher::Exec(Option_t* option)
{
   CbmRichTrbParam* param = CbmRichTrbParam::Instance();

   for (UInt_t i=0; i<fTrbRawHits->GetEntries(); i++) { // for loop over the input raw hits
      CbmTrbRawMessage* curTrbRawHit = static_cast<CbmTrbRawMessage*>(fTrbRawHits->At(i));

      UInt_t tdcId = curTrbRawHit->GetSourceAddress();
      UInt_t channel = curTrbRawHit->GetChannelID();
      UInt_t fine = curTrbRawHit->GetTDCfine();
      UInt_t coarse = curTrbRawHit->GetTDCcoarse();
      UInt_t epoch = curTrbRawHit->GetEpochMarker();

      if (param->IsSyncChannel(channel)) {                   // SYNCH MESSAGE PROCESSING

         //TODO implement

         Double_t timestamp = GetFullTime(tdcId, channel, epoch, coarse, fine);

         // This is channel zero, we suppose that here only leading edge is coming, so write an 'only-leading-edge' RichTrbDigi.
         // Set channel to 0xffffffff if the edge is unavailable

         new( (*fRichTrbDigi)[fRichTrbDigi->GetEntriesFast()] )
            CbmRichTrbDigi(tdcId, kTRUE, kFALSE, channel, 0xffffffff, timestamp, 0.0);

      } else if (param->IsLeadingEdgeChannel(channel)) {     // LEADING EDGE PROCESSING

         // MONKEY CODE. 'edge' bit is not filled. 'reserved' bits are set to 0.
         UInt_t tdcData = 0x80000000;
         tdcData += (channel << 22);
         tdcData += (fine << 12);
         tdcData += coarse;

         //possLeadEdges->AddPossibleLeadingEdge(tdcId, channel, epoch, tdcData);
         this->AddPossibleLeadingEdge(tdcId, channel, epoch, tdcData);

      } else {                                              // TRAILING EDGE PROCESSING

         // LEADING-TRAILING edges matching

         UInt_t searchedLchannel = param->GetCorrespondingLeadingEdgeChannel(channel);
         UInt_t foundLepoch;
         UInt_t leadWord;
         //Bool_t edgeFound = possLeadEdges->FindLeadingEdge(tdcId, searchedLchannel, epoch, coarse, &foundLepoch, &leadWord);
         Bool_t edgeFound = this->FindLeadingEdge(tdcId, searchedLchannel, epoch, coarse, &foundLepoch, &leadWord);

         if (edgeFound)  // corresponding leading edge found
         {
            UInt_t lchannel = (leadWord >> 22) & 0x7f;
            UInt_t lfine = (leadWord >> 12) & 0x3ff;
            UInt_t ledge = (leadWord >> 11) & 0x1;
            UInt_t lcoarse = (leadWord) & 0x7ff;

            Double_t tfullTime = GetFullTime(tdcId, channel, epoch, coarse, fine);
            Double_t lfullTime = GetFullTime(tdcId, lchannel, foundLepoch, lcoarse, lfine);

            if (fDrawHist) {
               fhTtimeMinusLtime[param->TDCidToInteger(tdcId)]->Fill(tfullTime - lfullTime);
               if (tdcId == DEBUGTDCID) fhTtimeMinusLtimeCH[(channel/2)-1]->Fill(tfullTime - lfullTime);
            }

            if (tfullTime - lfullTime < 0.) {
               // negative time-over-threshold
            }

            new( (*fRichTrbDigi)[fRichTrbDigi->GetEntriesFast()] )
               CbmRichTrbDigi(tdcId, kTRUE, kTRUE, lchannel, channel, lfullTime, tfullTime);

         } else {  // corresponding leading edge not found

            Double_t timestamp = GetFullTime(tdcId, channel, epoch, coarse, fine);

            new( (*fRichTrbDigi)[fRichTrbDigi->GetEntriesFast()] )
               CbmRichTrbDigi(tdcId, kFALSE, kTRUE, 0xffffffff, channel, 0.0, timestamp);

         }

      } // TYPE OF THE EDGE

   } // for loop over the input raw hits

}

void CbmTrbEdgeMatcher::FinishEvent()
{
   fRichTrbDigi->Clear();
}

void CbmTrbEdgeMatcher::FinishTask()
{
   this->DrawDebugHistos();
}

void CbmTrbEdgeMatcher::AddPossibleLeadingEdge(UInt_t tdcId, UInt_t lChannel, UInt_t lEpoch, UInt_t lDataWord)
{
   std::map<UInt_t, UInt_t*>::iterator tdcIdToStoredEdgesI;

   tdcIdToStoredEdgesI = tdcIdToStoredEdges.find(tdcId);

   if (tdcIdToStoredEdgesI != tdcIdToStoredEdges.end())
   {
      tdcIdToStoredEdgesI->second[lChannel*BUFSIZE*2 + circularCounters[tdcId]*2 + 0] = lEpoch;
      tdcIdToStoredEdgesI->second[lChannel*BUFSIZE*2 + circularCounters[tdcId]*2 + 1] = lDataWord;
      circularCounters[tdcId] += 1;
      if (circularCounters[tdcId] == BUFSIZE) circularCounters[tdcId] = 0;     
   }
   else
   {
      tdcIdToStoredEdges[tdcId] = new UInt_t[33*BUFSIZE*2];

      for (UInt_t i=0; i<33*BUFSIZE*2; i++) {  // initialize with 0xffffffff
         tdcIdToStoredEdges[tdcId][i] = 0xffffffff;
      }

      circularCounters[tdcId] = 0;
      tdcIdToStoredEdges[tdcId][lChannel*BUFSIZE*2 + circularCounters[tdcId]*2 + 0] = lEpoch;
      tdcIdToStoredEdges[tdcId][lChannel*BUFSIZE*2 + circularCounters[tdcId]*2 + 1] = lDataWord;
      circularCounters[tdcId] += 1;
   }
}

// return timeWord, epoch should coincide, the found instance is cleared
Bool_t CbmTrbEdgeMatcher::FindLeadingEdge(UInt_t tdcId, UInt_t lChannel, UInt_t tEpoch, UInt_t tCoarse, UInt_t* lEpoch, UInt_t* lWord)
{
   std::map<UInt_t, UInt_t*>::iterator tdcIdToStoredEdgesI;
   tdcIdToStoredEdgesI = tdcIdToStoredEdges.find(tdcId);

   if (tdcIdToStoredEdgesI != tdcIdToStoredEdges.end())
   {
      UInt_t foundEpoch = 0xffffffff;
      UInt_t foundWord = 0xffffffff;
      UInt_t numOfStoredEdges=0; // number of edges stored for the given channel
      UInt_t foundWordIndex=0;

      for (UInt_t i=0; i<BUFSIZE; i++) {  // count how many edges are saved for this channel
         if (tdcIdToStoredEdgesI->second[lChannel*BUFSIZE*2 + i*2 + 1] != 0xffffffff) {
            foundEpoch = tdcIdToStoredEdgesI->second[lChannel*BUFSIZE*2 + i*2 + 0];  // store epoch
            foundWord =  tdcIdToStoredEdgesI->second[lChannel*BUFSIZE*2 + i*2 + 1];  // store data word
            numOfStoredEdges++;     // count
            foundWordIndex = i;     // store
         }
      }

      if (numOfStoredEdges == 0) { // nothing found for the given channel - no need to create a 'leading-edge-only' digi
         *lEpoch = 0xffffffff;
         *lWord = 0xffffffff;
         return kFALSE;
      } else if (numOfStoredEdges == 1) { // only one edge is stored for the given channel - clean the entry and return found data
         tdcIdToStoredEdgesI->second[lChannel*BUFSIZE*2 + foundWordIndex*2 + 0] = 0xffffffff;  // clean epoch
         tdcIdToStoredEdgesI->second[lChannel*BUFSIZE*2 + foundWordIndex*2 + 1] = 0xffffffff;  // clean data word

         // if the time-over-threshold is more than 1 epoch (10 microseconds) then do not use this leading edge, but still delete it!
         if ((signed int)tEpoch - (signed int)foundEpoch > 1) {
            //printf("\nonly one edge is stored for the given channel (huge     TtimeOvTh) %08x - %08x = %d\n", tEpoch, foundEpoch, tEpoch - foundEpoch);

            // Create a digi with leading edge only
            this->CreateLeadingEdgeOnlyDigi(tdcId, foundEpoch, foundWord);
            *lEpoch = 0xffffffff;
            *lWord = 0xffffffff;
            return kFALSE;
         } else if ((signed int)tEpoch - (signed int)foundEpoch < 0) {
            //printf("\nonly one edge is stored for the given channel (negative TtimeOvTh) %08x - %08x = %d\n", tEpoch, foundEpoch, tEpoch - foundEpoch);

            // Create a digi with leading edge only
            this->CreateLeadingEdgeOnlyDigi(tdcId, foundEpoch, foundWord);
            *lEpoch = 0xffffffff;
            *lWord = 0xffffffff;
            return kFALSE;
         } else {
            *lEpoch = foundEpoch;  // return epoch
            *lWord = foundWord;      // return data word
            return kTRUE;
         }

      } else { // more than one word found - find the nearest leading to the trailing
         UInt_t lCoarse;
         UInt_t numSameEpoch=0;

         for (UInt_t i=0; i<BUFSIZE; i++) {  // count how many edges within the same epoch are saved for this channel
            if (tdcIdToStoredEdgesI->second[lChannel*BUFSIZE*2 + i*2 + 1] != 0xffffffff &&
                  tdcIdToStoredEdgesI->second[lChannel*BUFSIZE*2 + i*2 + 0] == tEpoch) {
               foundEpoch = tdcIdToStoredEdgesI->second[lChannel*BUFSIZE*2 + i*2 + 0];  // store epoch
               foundWord =  tdcIdToStoredEdgesI->second[lChannel*BUFSIZE*2 + i*2 + 1];  // store data word
               numSameEpoch++;
               foundWordIndex = i;     // store
            }
         }

         if (numSameEpoch==1) { // only one edge within the same epoch is stored - clean the entry and return found data
            tdcIdToStoredEdgesI->second[lChannel*BUFSIZE*2 + foundWordIndex*2 + 0] = 0xffffffff;  // clean epoch
            tdcIdToStoredEdgesI->second[lChannel*BUFSIZE*2 + foundWordIndex*2 + 1] = 0xffffffff;  // clean data word
            *lEpoch = foundEpoch;  // return epoch
            *lWord = foundWord;      // return data word
            return kTRUE;
         } else {  // zero or more than one edge within the same epoch found


            UInt_t coarseDiff = 0;
            Bool_t atLeastOneFound = kFALSE;
            for (UInt_t i=0; i<BUFSIZE; i++) {

               UInt_t epoch1 = tdcIdToStoredEdgesI->second[lChannel*BUFSIZE*2 + i*2 + 0];
               UInt_t tword1 = tdcIdToStoredEdgesI->second[lChannel*BUFSIZE*2 + i*2 + 1];
               if (tword1 != 0xffffffff && epoch1 == tEpoch) { // found edge within the same epoch

                  if (!atLeastOneFound) {
                     atLeastOneFound = kTRUE;
                     coarseDiff = abs((signed int)(tword1 & 0x7ff) - (signed int)tCoarse);  // fist conicidence - store it
                     foundEpoch = tdcIdToStoredEdgesI->second[lChannel*BUFSIZE*2 + i*2 + 0];  // store epoch
                     foundWord =  tdcIdToStoredEdgesI->second[lChannel*BUFSIZE*2 + i*2 + 1];  // store data word
                     foundWordIndex = i;     // store

                  } else {
                     if (coarseDiff > abs((signed int)(tword1 & 0x7ff) - (signed int)tCoarse)) { // further coincidences - find minimum
                        coarseDiff = abs((signed int)(tword1 & 0x7ff) - (signed int)tCoarse);  // less than the prev - store this as minimum
                        foundEpoch = tdcIdToStoredEdgesI->second[lChannel*BUFSIZE*2 + i*2 + 0];  // store epoch
                        foundWord =  tdcIdToStoredEdgesI->second[lChannel*BUFSIZE*2 + i*2 + 1];  // store data word
                        foundWordIndex = i;     // store
                     }
                  }

               }
            }

            tdcIdToStoredEdgesI->second[lChannel*BUFSIZE*2 + foundWordIndex*2 + 0] = 0xffffffff;  // clean epoch
            tdcIdToStoredEdgesI->second[lChannel*BUFSIZE*2 + foundWordIndex*2 + 1] = 0xffffffff;  // clean data word

            if ((signed int)tEpoch - (signed int)foundEpoch > 1) {
               //printf("\n============== zero or more than one edge within the same epoch (huge     TtimeOvTh) %08x - %08x = %d\n", tEpoch, foundEpoch, tEpoch - foundEpoch);
               // Create a digi with leading edge only
               this->CreateLeadingEdgeOnlyDigi(tdcId, foundEpoch, foundWord);
               *lEpoch = 0xffffffff;
               *lWord = 0xffffffff;
               return kFALSE;
            } else if ((signed int)tEpoch - (signed int)foundEpoch < 0) {
               //printf("\n============== zero or more than one edge within the same epoch (negative TtimeOvTh) %08x - %08x = %d\n", tEpoch, foundEpoch, tEpoch - foundEpoch);
               // Create a digi with leading edge only
               this->CreateLeadingEdgeOnlyDigi(tdcId, foundEpoch, foundWord);
               *lEpoch = 0xffffffff;
               *lWord = 0xffffffff;
               return kFALSE;
            } else {
               *lEpoch = foundEpoch;  // return epoch
               *lWord = foundWord;      // return data word
               return kTRUE;
            }

         }

      }
   }
   else // no entries for the given tdc is found - no need to create a 'leading-edge-only' digi
   {
      *lEpoch = 0xffffffff;
      *lWord = 0xffffffff;
      return kFALSE;
   }
}

void CbmTrbEdgeMatcher::CreateLeadingEdgeOnlyDigi(UInt_t tdcId, UInt_t lepoch, UInt_t lword)
{
   // Create a digi with leading edge only
   UInt_t lchannel = (lword >> 22) & 0x7f;
   UInt_t lfine = (lword >> 12) & 0x3ff;
   UInt_t lcoarse = (lword) & 0x7ff;
   Double_t timestamp = GetFullTime(tdcId, lchannel, lepoch, lcoarse, lfine);
   new( (*fRichTrbDigi)[fRichTrbDigi->GetEntriesFast()] )
      CbmRichTrbDigi(tdcId, kTRUE, kFALSE, lchannel, 0xffffffff, timestamp, 0.0);
}

Double_t CbmTrbEdgeMatcher::GetFullTime(UInt_t tdcId, UInt_t channel, UInt_t epoch, UInt_t coarse, UInt_t fine)
{
	Double_t coarseUnit = 5.;
	Double_t epochUnit = coarseUnit * 0x800;
	
   UInt_t trb_index = (tdcId >> 4) & 0x00FF - 1;
   UInt_t tdc_index = (tdcId & 0x000F);
	
	Double_t time = epoch * epochUnit + coarse * coarseUnit -
	               CbmTrbCalibrator::Instance()->GetFineTimeCalibrated(trb_index, tdc_index, channel, fine);

   return time;
}

void CbmTrbEdgeMatcher::DrawDebugHistos()
{
   if (!fDrawHist) return;

   CbmRichTrbParam* param = CbmRichTrbParam::Instance();

   TCanvas* c[17];
   for (UInt_t i=1; i<18; i++) {

      TString canvasName;
      TString canvasTitle;
      canvasName.Form("fhTtimeMinusLtime - TDCs %04x, %04x, %04x, %04x", param->IntegerToTDCid(i*4+0),
                                                                         param->IntegerToTDCid(i*4+1),
                                                                         param->IntegerToTDCid(i*4+2),
                                                                         param->IntegerToTDCid(i*4+3));
      canvasTitle.Form("(Trailing time - Leading time) for TDCs %04x, %04x, %04x, %04x",
                                                                         param->IntegerToTDCid(i*4+0),
                                                                         param->IntegerToTDCid(i*4+1),
                                                                         param->IntegerToTDCid(i*4+2),
                                                                         param->IntegerToTDCid(i*4+3));

      c[i-1] = new TCanvas(canvasName, canvasTitle, 800, 800);
      c[i-1]->Divide(2, 2);
      c[i-1]->cd(1);
      c[i-1]->GetPad(1)->SetLogy(1);
      fhTtimeMinusLtime[(i-1)*4+0]->Draw();
      c[i-1]->cd(2);
      c[i-1]->GetPad(2)->SetLogy(1);
      fhTtimeMinusLtime[(i-1)*4+1]->Draw();
      c[i-1]->cd(3);
      c[i-1]->GetPad(3)->SetLogy(1);
      fhTtimeMinusLtime[(i-1)*4+2]->Draw();
      c[i-1]->cd(4);
      c[i-1]->GetPad(4)->SetLogy(1);
      fhTtimeMinusLtime[(i-1)*4+3]->Draw();
   }

   TCanvas* c2[16];
   TCanvas* c3;

   TString canvasName;
   TString canvasTitle;

   canvasName.Form("fhTtimeMinusLtime - TDC %04x", DEBUGTDCID);
   canvasTitle.Form("(Trailing time - Leading time) for TDC %04x", DEBUGTDCID);
   c3 = new TCanvas(canvasName, canvasTitle, 800, 800); 
   gPad->SetLogy(1);
   fhTtimeMinusLtime[param->TDCidToInteger(DEBUGTDCID)]->Draw();


   for (UInt_t i=0; i<16; i++) {
      canvasName.Form("fhTtimeMinusLtime - TDC %04x CH%d - CH%d", DEBUGTDCID, i*2+2, i*2+1);
      canvasTitle.Form("(Trailing time - Leading time) for TDC %04x CH%d - CH%d", DEBUGTDCID, i*2+2, i*2+1);

      c2[i] = new TCanvas(canvasName, canvasTitle, 800, 800);
      gPad->SetLogy(1);
      fhTtimeMinusLtimeCH[i]->Draw();

   }
}

ClassImp(CbmTrbEdgeMatcher)
