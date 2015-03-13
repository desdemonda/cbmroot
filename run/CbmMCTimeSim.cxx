/** @file CbmMCTimeSim.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 8 February 2012
 **/


#include <iomanip>

#include "TClonesArray.h"
#include "TRandom3.h"

#include "CbmDetectorList.h"
#include "CbmMCBuffer.h"
#include "CbmMCEventHeader.h"
#include "CbmMCTimeSim.h"


using namespace std;


// -----   Default constructor   ---------------------------------------------
CbmMCTimeSim::CbmMCTimeSim() 
 : FairTask(),
   fEventRate(0.),
   fBeamProfile(0),
   fMaxBufferSize(0.),
   fEventId(0),
   fEventTime(0.),
   fNofEvents(0),
   fTimeEventFirst(-1.),
   fTimeEventLast(-1.),
   fTimer(),
   fEvent(NULL),
   fPointArrays()
{ 
  gLogger->Fatal(MESSAGE_ORIGIN, 
		 "Default constructor should not be used; specify interaction rate!");
}
// ---------------------------------------------------------------------------



// -----   Standard constructor   --------------------------------------------
CbmMCTimeSim::CbmMCTimeSim(Double_t rate, Int_t profile, const char* name) 
  : FairTask(name), 
    fEventRate(rate),
    fBeamProfile(profile),
    fMaxBufferSize(1000.),
    fEventId(-1),
    fEventTime(0.),
    fNofEvents(0),
    fTimeEventFirst(-1.),
    fTimeEventLast(-1.),
    fTimer(),
    fEvent(NULL),
    fPointArrays()
{
  for (Int_t iDet = 0; iDet < kNOFDETS; iDet++) fPointArrays.push_back(NULL);
}
// ---------------------------------------------------------------------------



// -----   Destructor   ------------------------------------------------------
CbmMCTimeSim::~CbmMCTimeSim() { }
// ---------------------------------------------------------------------------



// -----   CreateEventTime   -------------------------------------------------
Double_t CbmMCTimeSim::CreateEventTime() {
  
  Double_t deltaT = 0.;
  Double_t tau = 1.e9 / fEventRate;  // average time between events in [ns]
  switch ( fBeamProfile ) {
  case 1 :   // Constant time interval 
    deltaT = tau; 
    break;
  case 2 :   // Poisson profile: exponential distribution of time interval
    deltaT = gRandom->Exp(tau);
    break;
  default :
    gLogger->Error(MESSAGE_ORIGIN, 
		   "Beam profile type %d not defined!", fBeamProfile);
  }

  return ( fEventTime + deltaT );
}
// ---------------------------------------------------------------------------



// -----   Exec   ------------------------------------------------------------
void CbmMCTimeSim::Exec(Option_t* opt) {

	// Start time and point counter
	fTimer.Start();
	Int_t nPointsAll = 0;

  // Get CbmMCBuffer instance and remove already used points
  CbmMCBuffer* buffer = CbmMCBuffer::Instance();
  buffer->Clear();

  // Check for overflow
  if ( buffer->GetSize() > fMaxBufferSize ) 
    gLogger->Fatal(MESSAGE_ORIGIN, 
		   "Buffer size %.2f MB exceeds maximum (%.2f)",
		   buffer->GetSize(), fMaxBufferSize);

  // Get event ID
  fEventId = fEvent->GetEventID();

  // Create and set the new event time
  fEventTime = CreateEventTime();
  if ( ! fNofEvents ) fTimeEventFirst = fEventTime;
  fTimeEventLast = fEventTime;

  // Log output
  cout << endl;
  LOG(DEBUG) << fName
  		      << ": event " << fNofEvents
            << ", event ID " << fEventId
            << ", event time " << fixed << setprecision(3) << fEventTime
            << " ns, points ";

  // Read MCPoints from event into buffers
  for (Int_t iDet = 0; iDet < kNOFDETS; iDet++) {

  	// --- For the time being, use only STS and MUCH to avoid overflow
  	 if ( iDet != kSTS && iDet != kMUCH ) continue;

    Int_t nPoints = 0;
    if ( fPointArrays[iDet] ) {
      nPoints = buffer->Fill(fPointArrays[iDet], iDet,
                             fNofEvents, fEventTime);
      TString sysName;
      CbmDetectorList::GetSystemNameCaps(iDet, sysName);
      LOG(DEBUG) << sysName << " " << nPoints << "  ";
      nPointsAll += nPoints;
    }

  }
  LOG(DEBUG) << FairLogger::endl;

  // Log buffer status
  LOG(DEBUG) << CbmMCBuffer::Instance()->ToString() << FairLogger::endl;

  // Event log
  fTimer.Stop();
  LOG(INFO) << "+ " << setw(20) << GetName() << ": event " << setw(6)
  		      << right << fNofEvents << ", time " << fixed << setprecision(6)
  		      << fTimer.RealTime() << " s, points: " << nPointsAll
  		      << ", event time " << setprecision(3) << fEventTime << " ns"
            << FairLogger::endl;
  fNofEvents++;

}
// ---------------------------------------------------------------------------




// -----   Finish   ----------------------------------------------------------
void CbmMCTimeSim::Finish() {

  cout << endl;
  LOG(INFO) << fName << ": End of run" << FairLogger::endl;

  // --- Clear MCBuffer and flag end of run
  CbmMCBuffer::Instance()->Clear();
  CbmMCBuffer::Instance()->SetEndOfRun();

	std::cout << std::endl;
	LOG(INFO) << "=====================================" << FairLogger::endl;
	LOG(INFO) << GetName() << ": Run summary" << FairLogger::endl;
  LOG(INFO) << GetName() << ": Events processed: " << fNofEvents
  		      << " from " << fixed << setprecision(3) << fTimeEventFirst
  		      << " ns to " << fTimeEventLast << " ns" << FairLogger::endl;
	LOG(INFO) << "=====================================" << FairLogger::endl
			      << FairLogger::endl;

}
// ---------------------------------------------------------------------------




// -----   Initialisation   --------------------------------------------------
InitStatus CbmMCTimeSim::Init() {

	std::cout << std::endl;
  LOG(INFO) << "=========================================================="
		        << FairLogger::endl;
 	LOG(INFO) << GetName() << ": Initialisation" << FairLogger::endl;
	LOG(INFO) << FairLogger::endl;
	LOG(INFO) << GetName() << ": average interaction rate is " << fEventRate
			      << " Hz" << FairLogger::endl;
	LOG(INFO) << GetName() << ": beam profile is ";
	switch (fBeamProfile) {
		case 1:  LOG(INFO)  << "constant"; break;
		case 2:  LOG(INFO)  << "Poisson"; break;
		default: LOG(INFO)  << "unknown"; break;
	}
	LOG(INFO) << FairLogger::endl;
	LOG(INFO) << GetName() << ": buffer size is " << fMaxBufferSize << " MB"
			      << FairLogger::endl;

	// Get FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) LOG(FATAL) << GetName() << ": No FairRootManager!"
		                        << FairLogger::endl;

  // Get event header
  fEvent = (CbmMCEventHeader*) ioman->GetObject("MCEventHeader.");

 // Get MCPoint arrays
  fPointArrays[kMVD]  = (TClonesArray*) ioman->GetObject("MvdPoint");
  fPointArrays[kSTS]  = (TClonesArray*) ioman->GetObject("StsPoint");
  fPointArrays[kRICH] = (TClonesArray*) ioman->GetObject("RichPoint");
  fPointArrays[kMUCH] = (TClonesArray*) ioman->GetObject("MuchPoint");
  fPointArrays[kTRD]  = (TClonesArray*) ioman->GetObject("TrdPoint");
  fPointArrays[kTOF]  = (TClonesArray*) ioman->GetObject("TofPoint");
  fPointArrays[kECAL] = (TClonesArray*) ioman->GetObject("EcalPoint");
  fPointArrays[kPSD]  = (TClonesArray*) ioman->GetObject("PsdPoint");

  // Control output for configuration
  LOG(INFO) << GetName() << ": configuration ";
  TString config = "";
  for (Int_t iDet = 1; iDet < kNOFDETS; iDet++) {
    if ( fPointArrays[iDet] ) {
      TString sysName;
      CbmDetectorList::GetSystemNameCaps(iDet, sysName);
      LOG(INFO) << sysName << " ";
      config += " " + sysName;
    }
  }
  LOG(INFO) << FairLogger::endl;

  // Clear CbmMCBuffer
  CbmMCBuffer::Instance()->Clear();

  LOG(INFO) << GetName() << ": Initialisation successful"
		    << FairLogger::endl;
  LOG(INFO) << "=========================================================="
		        << FairLogger::endl;
	std::cout << std::endl;

  return kSUCCESS;
}
// ---------------------------------------------------------------------------

   


  

ClassImp(CbmMCTimeSim)
