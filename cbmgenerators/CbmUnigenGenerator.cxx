// -------------------------------------------------------------------------
// -----                     CbmUnigenGenerator                        -----
// -----              Created 2006/Jul/04  by D. Kresan                -----
// -----              Switch to FairLogger 2013/Oct/31 by DE           -----
// -------------------------------------------------------------------------

#include "CbmUnigenGenerator.h"

#include "CbmMCEventHeader.h"

#include "FairPrimaryGenerator.h"
#include "FairLogger.h"

#include "URun.h"
#include "UEvent.h"
#include "UParticle.h"

#include "TFile.h"
#include "TTree.h"
#include "TRandom.h"

#include <iostream>
using namespace std;


// ------------------------------------------------------------------------
CbmUnigenGenerator::CbmUnigenGenerator()
  : FairGenerator(),
    fEvents(0),
    fInputFile(NULL),
    fFileName(""),
    fInTree(NULL),
    fEvent(NULL),
    fCM(kFALSE),
    fBetaCM(0.),
    fGammaCM(1.),
    fPhiMin(0.), 
    fPhiMax(0.),
    fEventPlaneSet(kFALSE) 
{
}
// ------------------------------------------------------------------------


// ------------------------------------------------------------------------
CbmUnigenGenerator::CbmUnigenGenerator(TString fileName)
  : FairGenerator(),
    fEvents(0),
    fInputFile(NULL),
    fFileName(fileName),
    fInTree(NULL),
    fEvent(NULL),
    fCM(kFALSE),
    fBetaCM(0.),
    fGammaCM(0.),
    fPhiMin(0.), 
    fPhiMax(0.),
    fEventPlaneSet(kFALSE) 
{
    LOG(INFO) << "CbmUnigenGenerator: Opening input file "
              << fFileName.Data() << FairLogger::endl;
    fInputFile = new TFile(fFileName);
    if (NULL == fInputFile) {
        LOG(FATAL) << "CbmUnigenGenerator: Cannot open input file." << FairLogger::endl;
    }

    // Get run description
    URun *run = (URun*) fInputFile->Get("run");
    if(NULL==run) {
        LOG(FATAL) << "CbmUnigenGenerator: No run description in input file." << FairLogger::endl;
    }
    fCM = kFALSE;
    LOG(DEBUG) << "Target Momentum: " << run->GetPTarg() << endl;
    if(TMath::Abs(run->GetPTarg()) > 0.001) fCM = kTRUE;
    fBetaCM = 0.;
    fGammaCM = 1.;
    if(fCM) {
        LOG(INFO) << "Input data is in CM frame" << FairLogger::endl;
	Double_t elab = (TMath::Power(run->GetNNSqrtS(),2)-
			 2*TMath::Power(0.938271998,2))/(2*0.938271998);
	Double_t plab = TMath::Sqrt(elab*elab - TMath::Power(0.938271998,2));
        LOG(INFO) << "CbmUnigenGenerator: Plab = " << plab << " AGeV" << FairLogger::endl;
        fBetaCM = plab / (elab + 0.938271998);
	fGammaCM = 1./TMath::Sqrt(1. - fBetaCM*fBetaCM);
        LOG(INFO) << "CbmUnigenGenerator: boost parameters: " 
                  << "betaCM = "    << fBetaCM
	          << ", gammaCM = " << fGammaCM << FairLogger::endl;
    } else {
        LOG(INFO) << "Input data is in LAB frame" << FairLogger::endl;
    }

    delete run;
    fInTree = (TTree*) fInputFile->Get("events");
    if(NULL == fInTree) {
        LOG(FATAL) << "CbmUnigenGenerator: No event tree in input file." << FairLogger::endl;
    }
    fEvent = new UEvent();
    fInTree->SetBranchAddress("event", &fEvent);
}
// ------------------------------------------------------------------------


// ------------------------------------------------------------------------
CbmUnigenGenerator::~CbmUnigenGenerator()
{
    CloseInput();
}
// ------------------------------------------------------------------------


// ------------------------------------------------------------------------
Bool_t CbmUnigenGenerator::ReadEvent(FairPrimaryGenerator* primGen)
{
    // Check for input file
    if(NULL==fInputFile || NULL==fInTree) {
        LOG(ERROR) << "CbmUnigenGenerator: Input file is not opened!" << FairLogger::endl;
	return kFALSE;
    }

    // If end of input file is reached : close it and abort run
    if(fEvents >= fInTree->GetEntries()) {
        LOG(INFO) << "CbmUnigenGenerator: End of input file reached" << FairLogger::endl;
	CloseInput();
	return kFALSE;
    }

    // Read entry
    fInTree->GetEntry(fEvents);

    LOG(INFO) << "CbmUnigenGenerator: Event " << fEvent->GetEventNr()
              << ", multiplicity " << fEvent->GetNpa() << FairLogger::endl;

    UParticle *particle;
    Double_t pz;
    Double_t pz1;
    Double_t phi = 0.;

  // ---> Generate rotation angle  D
  if ( fEventPlaneSet ) phi = gRandom->Uniform(fPhiMin, fPhiMax);

  // Set event id and impact parameter in MCEvent if not yet done
  CbmMCEventHeader* event = dynamic_cast<CbmMCEventHeader*>(primGen->GetEvent());
  if ( event && (! event->IsSet()) ) {
    event->SetEventID(fEvent->GetEventNr());
    event->SetB(fEvent->GetB());
    event->SetPhi(phi);
    event->SetNPrim(fEvent->GetNpa());
    event->MarkSet(kTRUE);
  }

    // Loop over tracks in the current event
    for (Int_t itrack = 0; itrack < fEvent->GetNpa(); itrack++) {
	// Get particle
	particle = fEvent->GetParticle(itrack);
	// Boost
	pz = particle->Pz();
	if(fCM) {
	    pz1 = fGammaCM*(pz + fBetaCM*particle->E());
	} else {
            pz1 = pz;
	}
	Double_t px = particle->Px();
	Double_t py = particle->Py();
	LOG(DEBUG2) << "Px before: "<< px << FairLogger::endl;
	LOG(DEBUG2) << "Py before: "<< py << FairLogger::endl;
	// Rotate momenta by event plane angle
	if ( fEventPlaneSet ) {
	  Double_t pt = TMath::Sqrt(px*px + py*py);
	  Double_t azim = TMath::ATan2(py,px);
	  azim += phi;
	  px = pt * TMath::Cos(azim);
	  py = pt * TMath::Sin(azim);
	  LOG(DEBUG2) << "Px after: "<< px << FairLogger::endl;
	  LOG(DEBUG2) << "Py after: "<< py << FairLogger::endl;
	}

	// Give track to PrimaryGenerator
	primGen->AddTrack(particle->GetPdg(),
			  px,
			  py,
			  pz1,
			  0., 0., 0.);
    }

    fEvents += 1;

    return kTRUE;
}

// -----   Public method SetEventPlane   ----------------------------------
void CbmUnigenGenerator::SetEventPlane(Double_t phiMin, Double_t phiMax) {
  fPhiMin = phiMin;
  fPhiMax = phiMax;
  fEventPlaneSet = kTRUE;
}

// ------------------------------------------------------------------------
void CbmUnigenGenerator::CloseInput()
{
    if(NULL != fInputFile) {
        LOG(INFO) << "CbmUnigenGenerator: Closing input file "
                  << fFileName.Data() << FairLogger::endl;
	fInputFile->Close();
	fInputFile = NULL;
    }
}
// ------------------------------------------------------------------------


ClassImp(CbmUnigenGenerator);

