/** @file CbmTbDaqBuffer.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 13 December 2013
 **/


#include <iostream>
#include <iomanip>

#include "TString.h"

#include "FairLogger.h"

#include "CbmTbDaqBuffer.h"
#include "CbmDetectorList.h"



// -----   Initialisation of static variables   ------------------------------
CbmTbDaqBuffer* CbmTbDaqBuffer::fgInstance = NULL;
// ---------------------------------------------------------------------------



// -----   Constructor   -----------------------------------------------------
CbmTbDaqBuffer::CbmTbDaqBuffer() : fData() {
}
// ---------------------------------------------------------------------------


// -----   Destructor   ------------------------------------------------------
CbmTbDaqBuffer::~CbmTbDaqBuffer() {
}
// ---------------------------------------------------------------------------




// -----   Access to next data   ---------------------------------------------
CbmDigi* CbmTbDaqBuffer::GetNextData(Double_t time) {


  // --- Check for empty buffer
  if ( ! fData.size() ) return NULL;

  // --- Get data from buffer
  CbmDigi* digi = NULL;
  multimap<Double_t, CbmDigi*>::iterator it = fData.begin();
  CbmDigi* test = it->second;
  if ( test->GetTime() < time ) {
    digi = test;
    fData.erase(it);
  }
  return digi;

}
// ---------------------------------------------------------------------------




// -----   Insert data into buffer   -----------------------------------------
void CbmTbDaqBuffer::InsertData(CbmDigi* digi) {

  if ( ! digi ) LOG(FATAL) << "DaqBuffer: invalid digi pointer"
                           << FairLogger::endl;

  pair<Double_t, CbmDigi*> value (digi->GetTime(), digi);
  fData.insert(value);

  LOG(DEBUG2) << "DaqBuffer: Inserting digi, detectorID "
              << digi->GetAddress() << ", time " << digi->GetTime()
              << FairLogger::endl;

}
// ---------------------------------------------------------------------------



// -----   Instance   --------------------------------------------------------
CbmTbDaqBuffer* CbmTbDaqBuffer::Instance() {
  if ( ! fgInstance ) fgInstance = new CbmTbDaqBuffer();
  return fgInstance;
}
// ---------------------------------------------------------------------------



// -----   Print status   ----------------------------------------------------
void CbmTbDaqBuffer::PrintStatus() const {
  TString sysName;
  Int_t size = GetSize();
  LOG(INFO) << "DaqBuffer: Status ";
  if ( ! size ) {
    LOG(INFO) << "empty" << FairLogger::endl;
    return;
  }
  LOG(INFO) << GetSize() << " digis from "
            << fixed << setprecision(9) << GetTimeFirst() * 1.e-9 << " s to "
            << GetTimeLast() *1.e-9 << " s" << FairLogger::endl;
}
// ---------------------------------------------------------------------------

