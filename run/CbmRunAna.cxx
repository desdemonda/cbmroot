/** CbmRunAna.cxx
 *@author Volker Friese v.friese@gsi.de
 *@since 10.12.2009
 *@version 1.0
 **/


#include "CbmRunAna.h"

#include "FairEventHeader.h"
#include "FairTask.h"
#include "FairTrajFilter.h"

#include <iostream>

using std::cout;
using std::endl;



// -----   Constructor   -----------------------------------------------------
CbmRunAna::CbmRunAna() 
  : FairRunAna(),
    fAsync(kFALSE),
    fMarkFill(kFALSE)
{
}
// ---------------------------------------------------------------------------



// -----   Destructor   ------------------------------------------------------
CbmRunAna::~CbmRunAna() {
}
// ---------------------------------------------------------------------------




// -----   Run execution   ---------------------------------------------------
void CbmRunAna::ExecRun(Int_t iStart, Int_t iStop) {

  // Check maximal event number
  Int_t maxEvt = fRootManager->GetInChain()->GetEntries();

  if ( iStart > maxEvt ) {
    cout << "-W- CbmRunAna: File contains only " << maxEvt << " events!" 
	 << endl;
    cout << "    No run execution." << endl << endl;
    return;
  }

  if ( iStop > maxEvt-1 ) {
    cout << "-W- CbmRunAna: Last event requested is " << iStop-1 << endl;
    cout << "               Last event in file   is " << maxEvt-1 << endl;
    iStop = maxEvt;
    cout << "               Last event is set    to " << iStop-1 << endl;
    cout << endl;
  }

  if ( iStart < 0 ) iStart = 0;


  // Event loop
  UInt_t tmpId = 0;
  for (Int_t iEvent=iStart; iEvent<iStop; iEvent++) {

    // If input file is present: read one event
    if ( fRootManager->GetInFile() ) {
      fRootManager->ReadEvent(iEvent);
      tmpId = fEvtHeader->GetRunId();

      // Re-initialisation if necessary
      if ( fEvtHeader->GetRunId() != fRunId ) {
	fRunId = fEvtHeader->GetRunId();
	if ( ! fStatic ) {
	  cout << " -I- FairRunAna : reinitialization done for RunID: "
	       << fRunId << endl;
	  Reinit( fRunId );
	  fTask->ReInitTask();
	}
      }

    } // Input file present


    // Execute master task
    fTask->ExecuteTask("");


    // In normal mode: mark output tree for filling
    if ( ! fAsync ) MarkFill();

    // If output marked for filling: do so
    if ( fMarkFill ) {
      fRootManager->Fill();
      fTask->FinishEvent();
    }

    // Reset trajectory filter if present
    if ( FairTrajFilter::Instance() ) FairTrajFilter::Instance()->Reset();

  }

  // End of run 
  fTask->FinishTask();
  fRootManager->Write();

}
// ---------------------------------------------------------------------------


    
ClassImp(CbmRunAna)
