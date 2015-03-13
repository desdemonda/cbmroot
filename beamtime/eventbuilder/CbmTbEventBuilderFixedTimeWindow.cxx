// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                   CbmTbEventBuilderFixedTimeWindow                -----
// -----                    Created 14.12.2014 by F. Uhlig                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------
#include "CbmTbEventBuilderFixedTimeWindow.h"

#include "CbmTbEvent.h"
#include "CbmDigi.h"

#include "FairRunOnline.h"
#include "FairRootManager.h"
#include "FairLogger.h"

CbmTbEventBuilderFixedTimeWindow::CbmTbEventBuilderFixedTimeWindow()
  : CbmTbEventBuilder(),
    fEventTimeWindow(500.)
{
}

CbmTbEventBuilderFixedTimeWindow::~CbmTbEventBuilderFixedTimeWindow()
{
}

Bool_t CbmTbEventBuilderFixedTimeWindow::Init()
{
  // --- Get event header from Run
  fCurrentEvent = 
    dynamic_cast<CbmTbEvent*> (FairRunOnline::Instance()->GetEventHeader());
  if ( ! fCurrentEvent ) {
    LOG(FATAL) << "No event header in run!" << FairLogger::endl;
  }

  LOG(INFO) << "Init : event header at " << fCurrentEvent << FairLogger::endl;
  FairRootManager* ioman = FairRootManager::Instance();
  ioman->Register("EventHeader.", "Event", fCurrentEvent, kTRUE);

  return kTRUE;
}

Bool_t CbmTbEventBuilderFixedTimeWindow::IsInEvent(CbmDigi* digi)
{
  if ( ! fCurrentEvent ) {
    LOG(FATAL) << "No pointer to event header! " << fCurrentEvent 
	       << FairLogger::endl;
  }
  
  // If event is empty or time of digi matches to event, 
  // add digi to event.
  Double_t tDif = TMath::Abs(digi->GetTime() - fCurrentEvent->GetTime() );

  if ( fCurrentEvent->IsEmpty() || tDif < fEventTimeWindow ) {

    // --- Update event object
    fCurrentEvent->AddDigi(digi);
    return kTRUE;
  }

  return kFALSE;
}

void CbmTbEventBuilderFixedTimeWindow::Reset()
{
  // --- Clear event header
  fCurrentEvent->Clear();
}
ClassImp(CbmTbEventBuilderFixedTimeWindow)
