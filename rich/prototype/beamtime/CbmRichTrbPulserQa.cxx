#include "CbmRichTrbPulserQa.h"
#include "FairRootManager.h"
#include "FairLogger.h"

#include "TClonesArray.h"

#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "CbmDrawHist.h"
#include "CbmRichHit.h"

#include <iostream>
#include <vector>

CbmRichTrbPulserQa::CbmRichTrbPulserQa()
  : FairTask(),
    fRichHits(NULL),
    fEventNum(0)
{
}

CbmRichTrbPulserQa::~CbmRichTrbPulserQa()
{
}


InitStatus CbmRichTrbPulserQa::Init()
{
	FairRootManager* manager = FairRootManager::Instance();
	fRichHits = (TClonesArray*)manager->GetObject("RichHit");
	if (NULL == fRichHits) { Fatal("CbmRichTrbPulserQa::Init","No RichHit array!"); }

	InitHist();

	return kSUCCESS;
}

void CbmRichTrbPulserQa::InitHist()
{
	//fhNofHitsInEvent = new TH1D("fhNofHitsInEvent", "fhNofHitsInEvent;Number of hits in event;Entries", 50, -0.5, 49.5);

}

void CbmRichTrbPulserQa::Exec(
		Option_t* option)
{
	fEventNum++;
	LOG(DEBUG2) << "CbmRichTrbPulserQa : Event #" << fEventNum << FairLogger::endl;


}


void CbmRichTrbPulserQa::DrawHist()
{
	TCanvas* c1 = new TCanvas("rich_trb_pulserqa_nof_hits_in_event", "rich_trb_pulserqa_nof_hits_in_event", 600, 600);
	//DrawH1(fhNofHitsInEvent);
}

void CbmRichTrbPulserQa::Finish()
{
	DrawHist();
}

ClassImp(CbmRichTrbPulserQa)
