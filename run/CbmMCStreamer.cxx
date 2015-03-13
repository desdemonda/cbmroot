/** CbmMCStreamer.cxx
 **@author Volker Friese <v.friese@gsi.de>
 **@since 13.11.2009
 **version 1.0
 **/


#include "CbmMCStreamer.h"

#include "CbmMCEpoch.h"
#include "CbmRunAna.h"

#include "FairRootManager.h"

#include "TClonesArray.h"
#include "TMath.h"
#include "TRandom3.h"

#include <iostream>


using namespace std;



// -----   Default constructor   ---------------------------------------------
CbmMCStreamer::CbmMCStreamer() 
  : FairTask(),
    fEventRate(0.),
    fEventTau(0.),
    fBeamProfile(0),
    fPersistence(kFALSE),
    fEpochLength(0.),
    fEventId(-1),
    fEventTime(0.),
    fEpoch(NULL),
    fEpochIsChanged(kFALSE),
    fMarkNewEpoch(kFALSE),
    fInMvd(NULL),
    fStsPoints(NULL),
    fMuchPoints(NULL),
    stsBuffer(),
    muchBuffer()
{ 
}
// ---------------------------------------------------------------------------




// -----   Standard constructor   --------------------------------------------
CbmMCStreamer::CbmMCStreamer(Double_t eventRate, 
			     Int_t beamProfile,
			     Double_t epochLength,
			     Bool_t persistence, 
			     Int_t verbosity, 
			     const char* name) 
  : FairTask(name, verbosity), 
    fEventRate(eventRate),
    fEventTau(1.e9 / eventRate),
    fBeamProfile(beamProfile),
    fPersistence(persistence),
    fEpochLength(epochLength),
    fEventId(-1),
    fEventTime(0.),
    fEpoch(NULL),
    fEpochIsChanged(kFALSE),
    fMarkNewEpoch(kFALSE),
    fInMvd(NULL),
    fStsPoints(NULL),
    fMuchPoints(NULL),
    stsBuffer(),
    muchBuffer()
{
}
// ---------------------------------------------------------------------------




// -----   Destructor   ------------------------------------------------------
CbmMCStreamer::~CbmMCStreamer() { }
// ---------------------------------------------------------------------------




// -----   Activate or deactivate subtasks   ---------------------------------
void CbmMCStreamer::ActivateTasks(Bool_t activate) {

  TIter taskIter(GetListOfTasks());
  FairTask* task;
  while( ( task=dynamic_cast<FairTask*> ( taskIter() ) ) ) 
    task->SetActive(activate);

} 
// ---------------------------------------------------------------------------




// -----   Execution   -------------------------------------------------------
void CbmMCStreamer::Exec(Option_t* opt) {

  Int_t nPoints = 0;
  cout.precision(8);


  // Inactive subtasks (default, if no epoch is given to output)
  ActivateTasks(kFALSE);


  // Start new epoch if required (after old one was filled to output tree)
  if ( fMarkNewEpoch ) NextEpoch();


  // Unset epoch change marker and output fill marker
  fEpochIsChanged = kFALSE;
  ( (CbmRunAna*) CbmRunAna::Instance() )->MarkFill(kFALSE);
      

  // Imcrement event ID
  fEventId++;
  

  // Determine event time
  Double_t deltaT = 0;
  if ( ! fBeamProfile ) deltaT = fEventTau;
  else                  deltaT = TMath::Nint(gRandom->Exp(fEventTau));
  fEventTime += deltaT;
  cout << endl;
  cout << "-I- " << GetName() << ": New event " << fEventId << " at " 
       << fEventTime << " ns" << endl;


  // Read event into buffer. If empty, skip execution (next event)
  if ( ! ReadEvent() ) {
    cout << "-I- " << GetName() << ": No points in event; stop execution"
	 << endl;
    return;
  }
  PrintBuffer();

  
  // Execute this loop until something is filled into the current epoch
  while ( kTRUE ) {
\
    // Fill points from buffer into current epoch
    ProcessBuffer();
    PrintBuffer();

    // If epoch changed (points added): end event execution (go to next event)
    if ( fEpochIsChanged ) {
      cout << "-I- " << GetName() << ": Epoch was updated. End excution."
	   << endl;
      break;
    }

    // Nothing added to epoch: so we can end it
    if ( ! fEpoch->IsEmpty() ) {

      // There's something in the epoch, so mark it for output and run all subtasks
      cout << "-I- " << GetName() << ": Ending epoch" << endl;
      fEpoch->Print();

      // ---  Activate all subtasks
      ActivateTasks(kTRUE);

      // ---  Tell the run to fill the output tree at the end of all Execs
      ( (CbmRunAna*) CbmRunAna::Instance() )->MarkFill();

      // ---  Start new epoch at beginning of next event
      fMarkNewEpoch = kTRUE;
      
      // ---  End execution
      break;

    }

    // Points are in buffer, but epoch is empty: try next one
    NextEpoch();

  }

}
// ---------------------------------------------------------------------------




// -----   Finish run   ------------------------------------------------------
void CbmMCStreamer::Finish() {

  // Fill last epoch into output
  if ( ! fEpoch->IsEmpty() ) {
	cout << "-I- " << GetName() << ": Filling epoch to output" << endl;
	fEpoch->Print();
	FairRootManager::Instance()->Fill();
  }

}
// ---------------------------------------------------------------------------




// -----   Initialisation   --------------------------------------------------
InitStatus CbmMCStreamer::Init() {

  cout << endl;
  cout << "---------------------------------------------" << endl;
  cout << "-I- Initialising " << GetName() << " ...." << endl; 
  cout << "-I-" << endl;
  cout << "-I- Configuration: ";

  // Get input MCPoint arrays
  FairRootManager* ioman = FairRootManager::Instance();
  fStsPoints  = (TClonesArray*) ioman->GetObject("StsPoint");
  fMuchPoints = (TClonesArray*) ioman->GetObject("MuchPoint");
  

  // Register output array
  fEpoch = new CbmMCEpoch(0., fEpochLength);
  fEpochIsChanged = kFALSE;
  ioman->Register("MCEpoch.", "MC epoch", fEpoch, fPersistence); 

  cout << endl;
  cout << "-I-" << endl;
  cout << "-I- Epoch length " << fEpochLength << " ns" << endl;
  cout << "-I- Average event spacing " << fEventTau << " ns" << endl;
  if ( ! fBeamProfile ) cout << "-I- Beam model: constant" << endl;
  else                  cout << "-I- Beam model: exponential" << endl;
  cout << "---------------------------------------------" << endl;

  return kSUCCESS;

}
// ---------------------------------------------------------------------------




// -----   Start next epoch   ------------------------------------------------
void CbmMCStreamer::NextEpoch() {

    Double_t newEpochTime = fEpoch->GetStartTime() + fEpochLength;;
    fEpoch->Clear();
    fEpoch->SetStartTime(newEpochTime);
    fEpochIsChanged = kFALSE;
    cout << "-I- " << GetName() << ": New epoch at " << newEpochTime << endl;

}
// ---------------------------------------------------------------------------




// -----   Print buffer content (for debug)   --------------------------------
void CbmMCStreamer::PrintBuffer() {

  cout << "-I- " << GetName() << ": Entries in buffer: ";
  cout << "STS " << stsBuffer.size();
  cout << " MUCH " << muchBuffer.size() << endl;

}
// ---------------------------------------------------------------------------

  


// -----   Process buffer   --------------------------------------------------
void CbmMCStreamer::ProcessBuffer() {


  // --- STS
  map<Double_t,CbmStsPoint>::iterator stsIt;
  while ( ! stsBuffer.empty() ) {
    stsIt = stsBuffer.begin();
    
    // Error if point time is before current epoch
    if ( ( (*stsIt).second).GetTime() < fEpoch->GetStartTime() ) {
      cout << "-E- " << GetName() 
	   << "::Exec: MCPoint time before current epoch!" << endl;
      cout << "    " << "Point time: " << ((*stsIt).second).GetTime() 
	   << " ns" << endl;
      cout << "    " << "Current epoch time " << fEpoch->GetStartTime()
	   << " ns" << endl;
      Fatal("Exec", "Cannot treat MCPoint time");
    }

    // Exit loop if point time is after current epoch
    if ( ((*stsIt).second).GetTime() > 
	 ( fEpoch->GetStartTime() + fEpochLength ) ) {
      cout << "-I- " << GetName() << ": Point time " 
	   << (*stsIt).second.GetTime() << " outside epoch" << endl;
      break;
    }

    // Add (copy) point to current epoch. Time relative to epoch start is
    // calculated by CbmMCEpoch.
    fEpoch->AddPoint(kSTS, &((*stsIt).second));
    cout <<  "-I- " << GetName() << ": Adding STS point from event "
	 << ((*stsIt).second).GetEventID() << " to epoch at " 
	 << ((*stsIt).second).GetTime() << " ns" << endl;
    stsBuffer.erase(stsIt);
    fEpochIsChanged = kTRUE;

  }

  // --- MUCH
  /* I know it's not elegant copying the code, but do you have a better suggestion? */
  map<Double_t,CbmMuchPoint>::iterator muchIt;
  while ( ! muchBuffer.empty() ) {
    muchIt = muchBuffer.begin();
    
    // Error if point time is before current epoch
    if ( ( (*muchIt).second).GetTime() < fEpoch->GetStartTime() ) {
      cout << "-E- " << GetName() 
	   << "::Exec: MCPoint time before current epoch!" << endl;
      cout << "    " << "Point time: " << ((*muchIt).second).GetTime() 
	   << " ns" << endl;
      cout << "    " << "Current epoch time " << fEpoch->GetStartTime()
	   << " ns" << endl;
      Fatal("Exec", "Cannot treat MCPoint time");
    }

    // Exit loop if point time is after current epoch
    if ( ((*muchIt).second).GetTime() > 
	 ( fEpoch->GetStartTime() + fEpochLength ) ) {
      cout << "-I- " << GetName() << ": Point time " 
	   << (*muchIt).second.GetTime() << " outside epoch" << endl;
      break;
    }

    // Add (copy) point to current epoch. Time relative to epoch start is
    // calculated by CbmMCEpoch.
    fEpoch->AddPoint(kMUCH, &((*muchIt).second));
    cout <<  "-I- " << GetName() << ": Adding MUCH point from event "
	 << ((*muchIt).second).GetEventID() << " to epoch at " 
	 << ((*muchIt).second).GetTime() << " ns" << endl;
    muchBuffer.erase(muchIt);
    fEpochIsChanged = kTRUE;

  }


}
// ---------------------------------------------------------------------------

    


// -----   Read one event into the buffer   ----------------------------------
Int_t CbmMCStreamer::ReadEvent() {

  Int_t nPointsAdded = 0;

  if ( fStsPoints ) {
    Int_t    nStsPoints = 0;
    CbmStsPoint* stsPoint = NULL;
    nStsPoints = fStsPoints->GetEntriesFast();
    for (Int_t iPoint = 0; iPoint < nStsPoints; iPoint++) {
      stsPoint = (CbmStsPoint*) fStsPoints->At(iPoint);
      Double_t pTime = stsPoint->GetTime() + fEventTime;
      stsBuffer.insert( pair<Double_t,CbmStsPoint>
			(pTime, CbmStsPoint(*stsPoint, fEventId, 
					    fEventTime, 0.) ) );
      nPointsAdded++;
    }
  }

  if ( fMuchPoints ) {
    Int_t    nMuchPoints = 0;
    CbmMuchPoint* muchPoint = NULL;
    nMuchPoints = fMuchPoints->GetEntriesFast();
    for (Int_t iPoint = 0; iPoint < nMuchPoints; iPoint++) {
      muchPoint = (CbmMuchPoint*) fMuchPoints->At(iPoint);
      Double_t pTime = muchPoint->GetTime() + fEventTime;
      muchBuffer.insert( pair<Double_t,CbmMuchPoint>
			(pTime, CbmMuchPoint(*muchPoint, fEventId, 
					     fEventTime, 0.) ) );
      nPointsAdded++;
    }
  }

  cout << "-I- " << GetName() << ": " << nPointsAdded << 
    " points added to buffer" << endl;

  return nPointsAdded;
}
// ---------------------------------------------------------------------------




ClassImp(CbmMCStreamer)
