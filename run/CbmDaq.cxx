/** @file CbmDaq.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 20 July 2012
 **/


#include <iomanip>

#include "CbmMCBuffer.h"
#include "CbmDaq.h"
#include "CbmDaqBuffer.h"
#include "CbmTimeSlice.h"



// =====   Constructor   =====================================================
CbmDaq::CbmDaq(Double_t timeSliceSize) : FairTask("Daq"),
                   fCurrentStartTime (0.),
                   fDuration (timeSliceSize),
                   fStoreEmptySlices(kTRUE),
                   fTimer(),
                   fNofSteps(0),
                   fNofDigis(0),
                   fNofTimeSlices(0),
                   fNofTimeSlicesEmpty(0),
                   fTimeDigiFirst(-1.),
                   fTimeDigiLast(-1.),
                   fTimeSliceFirst(-1.),
                   fTimeSliceLast(-1.),
                   fTimeSlice(NULL),
                   fBuffer(NULL) {
}
// ===========================================================================



// =====   Destructor   ======================================================
CbmDaq::~CbmDaq() {
}
// ===========================================================================



// =====   Close the current time slice and fill it to the tree   ============
void CbmDaq::CloseTimeSlice() {

  // --- Time slice status
	if ( fTimeSlice->IsEmpty() ) {
	  LOG(DEBUG) << GetName() << ": closing " << fTimeSlice->ToString()
	  		       << FairLogger::endl;
	  fNofTimeSlicesEmpty++;
	} //? empty time slice
	else
		LOG(INFO) << GetName() << ": closing " << fTimeSlice->ToString()
  		       << FairLogger::endl;

  // --- Fill current time slice into tree (if required)
  if ( fStoreEmptySlices || ( ! fTimeSlice->IsEmpty()) )
  	FairRootManager::Instance()->Fill();
  fTimeSliceLast = fTimeSlice->GetEndTime();
  fNofTimeSlices++;
  if ( gLogger->IsLogNeeded(DEBUG) ) fTimeSlice->SelfTest();

  // --- Reset time slice with new time interval
  fCurrentStartTime += fDuration;
  fTimeSlice->Reset(fCurrentStartTime, fDuration);

}
// ===========================================================================



// =====   Task execution   ==================================================
void CbmDaq::Exec(Option_t* opt) {

	// Start timer and digi counter
	fTimer.Start();
	Int_t nDigis = 0;

  // --- DaqBuffer and time slice info
	LOG(DEBUG) << GetName() << ": " << fBuffer->ToString()
			       << FairLogger::endl;
	LOG(DEBUG) << GetName() << ": " << fTimeSlice->ToString()
			       << FairLogger::endl;

	// --- Fill the data from the buffer into the current time slice.
  while ( kTRUE ) {        /* Loop until time slice cannot be closed */

    nDigis += FillTimeSlice();

    // --> Exit if current time slice cannot yet be closed
    if ( CbmMCBuffer::Instance()->GetTime() <
         fTimeSlice->GetEndTime() + 2. * fDuration ) {
    	LOG(DEBUG) << GetName() << ": System time " << fixed
    			       << setprecision(3) <<CbmMCBuffer::Instance()->GetTime()
    			       << " ns; waiting for data." << FairLogger::endl;
    	break;
    }

  	LOG(DEBUG) << GetName() << ": System time " << fixed
  			       << setprecision(3) << CbmMCBuffer::Instance()->GetTime()
  			       << " ns" << FairLogger::endl;
    CloseTimeSlice();

  }     /* Loop over time slices */

  // --- DaqBuffer and time slice info
	LOG(DEBUG) << GetName() << ": " << fBuffer->ToString()
			       << FairLogger::endl;

  // --- Event log
  fTimer.Stop();
  LOG(INFO) << "+ " << setw(20) << GetName() << ": step  " << setw(6)
  		      << right << fNofSteps << ", time " << fixed << setprecision(6)
  		      << fTimer.RealTime() << " s, " << nDigis << " digis transported"
            << FairLogger::endl;

  // --- Increase exec counter
  fNofSteps++;

}
// ===========================================================================



// =====   Fill current time slice with data from buffers   ==================
Int_t CbmDaq::FillTimeSlice() {

	// --- Digi counter
	Int_t nDigis = 0;

  // --- Loop over all detector systems
  for (Int_t iDet = kREF; iDet < kNOFDETS; iDet++) {

    // --- Loop over digis from DaqBuffer and fill them into current time slice
    CbmDigi* digi = fBuffer->GetNextData(iDet, fTimeSlice->GetEndTime());
    while ( digi ) {

    	LOG(DEBUG2) << fName << ": Inserting digi with detector ID "
                  << digi->GetAddress() << " at time " << digi->GetTime()
                  << " into timeslice [" << fixed << setprecision(3)
                  << fTimeSlice->GetStartTime() << ", "
                  << fTimeSlice->GetEndTime() << ") ns"
                  << FairLogger::endl;
      fTimeSlice->InsertData(digi);
      nDigis++;
      if ( ! fNofDigis ) fTimeDigiFirst = digi->GetTime();
      fTimeDigiLast = TMath::Max(fTimeDigiLast, digi->GetTime());
      delete digi;
      digi = fBuffer->GetNextData(iDet, fTimeSlice->GetEndTime());
    }  //? Valid digi from buffer

  }  // Detector loop

  LOG(DEBUG) << GetName() << ": filled " << nDigis << " digis into "
  		       << fTimeSlice->ToString() << FairLogger::endl;
  fNofDigis += nDigis;

  return nDigis;
}
// ===========================================================================



// =====   End-of-run action   ===============================================
void CbmDaq::Finish() {

  LOG(INFO) << FairLogger::endl;
  LOG(INFO) << fName << ": End of run" << FairLogger::endl;

  while ( fBuffer->GetSize() ) {  // time slice loop until buffer is emptied

    FillTimeSlice();
    CloseTimeSlice();

    // --- DaqBuffer and time slice info
  	LOG(DEBUG) << GetName() << ": " << fBuffer->ToString()
  			       << FairLogger::endl;
  	LOG(DEBUG) << GetName() << ": " << fTimeSlice->ToString()
  			       << FairLogger::endl << FairLogger::endl;

  }

	LOG(DEBUG) << GetName() << ": " << fBuffer->ToString()
			       << FairLogger::endl;
  LOG(INFO)  << GetName() << ": run finished." << FairLogger::endl;
	std::cout << std::endl;
	LOG(INFO) << "=====================================" << FairLogger::endl;
	LOG(INFO) << GetName() << ": Run summary" << FairLogger::endl;
	LOG(INFO) << "Steps:        " << setw(10) << right << fNofSteps
						<< FairLogger::endl;
	LOG(INFO) << "Digis:        " << setw(10) << right << fNofDigis
			      << " from " << setw(10) << right << fixed << setprecision(1)
			      << fTimeDigiFirst << " ns  to " << setw(10) << right << fixed
			      << setprecision(1) << fTimeDigiLast << " ns" << FairLogger::endl;
	LOG(INFO) << "Time slices:  " << setw(10) << right << fNofTimeSlices
			      << " from " << setw(10) << right << fixed << setprecision(1)
			      << fTimeSliceFirst << " ns  to " << setw(10) << right << fixed
			      << setprecision(1) << fTimeSliceLast << " ns" << FairLogger::endl;
	LOG(INFO) << "Empty slices: " << setw(10) << right << fNofTimeSlicesEmpty
			      << FairLogger::endl;
	LOG(INFO) << "=====================================" << FairLogger::endl;

}
// ===========================================================================



// =====   Task initialisation   =============================================
InitStatus CbmDaq::Init() {

	std::cout << std::endl;
  LOG(INFO) << "=========================================================="
		        << FairLogger::endl;
 	LOG(INFO) << GetName() << ": Initialisation" << FairLogger::endl;
	LOG(INFO) << FairLogger::endl;
	LOG(INFO) << GetName() << ": Time slice interval is " << fDuration
			      << " ns." << FairLogger::endl;

	// Set initial start time
  fCurrentStartTime = 0.;
  fTimeSliceFirst = fCurrentStartTime;

  // Register output array TimeSlice
  fTimeSlice = new CbmTimeSlice(fCurrentStartTime, fDuration);
  fTimeSliceFirst = fTimeSlice->GetStartTime();
  FairRootManager::Instance()->Register("TimeSlice.",
      "DAQ", fTimeSlice, kTRUE);

  // Get Daq Buffer
  fBuffer = CbmDaqBuffer::Instance();

  LOG(INFO) << GetName() << ": Initialisation successful"
		    << FairLogger::endl;
  LOG(INFO) << "=========================================================="
		        << FairLogger::endl;
	std::cout << std::endl;

  return kSUCCESS;
}
// ===========================================================================


ClassImp(CbmDaq)

