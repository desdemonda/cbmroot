/** @file CbmStsMC.cxx
 **
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 17.02.2014
 **/


#include "CbmStsMC.h"

#include "TGeoManager.h"
#include "TGeoPhysicalNode.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TVector3.h"

#include "FairLogger.h"

#include "CbmDetectorList.h"
#include "CbmStack.h"
#include "CbmStsElement.h"
#include "CbmStsPoint.h"
#include "CbmStsSetup.h"




// -----   Constructor   ---------------------------------------------------
CbmStsMC::CbmStsMC(Bool_t active, const char* name)
  : FairDetector(name, active, kSTS),
    fStatusIn(),
    fStatusOut(),
    fEloss(0.),
    fAddressMap(),
    fStsPoints(NULL),
    fSetup(NULL)
{
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmStsMC::~CbmStsMC() {
  if (fStsPoints) {
    fStsPoints->Delete();
    delete fStsPoints;
  }
}
// -------------------------------------------------------------------------



// -----  ConstructGeometry  -----------------------------------------------
void CbmStsMC::ConstructGeometry() {

  TString fileName = GetGeometryFileName();

  // --- Only ROOT geometries are supported
  if (  ! fileName.EndsWith(".root") ) {
    LOG(FATAL) <<  GetName() << ": Geometry format of file "
    		       << fileName.Data() << " not supported." << FairLogger::endl;
  }

  LOG(INFO) << "Constructing " << GetName() << "  geometry from ROOT  file "
  		      << fileName.Data() << FairLogger::endl;
  ConstructRootGeometry();
}
// -------------------------------------------------------------------------



// -----   End of event action   -------------------------------------------
void CbmStsMC::EndOfEvent() {
	Print();                 // Status output
	Reset();                 // Reset the track status parameters
}
// -------------------------------------------------------------------------



// -----   Initialise   ----------------------------------------------------
void CbmStsMC::Initialize() {

	// --- Instantiate the output array
  fStsPoints = new TClonesArray("CbmStsPoint");

  // --- Get the CbmStsSetup instance and construct a map from full path
  // --- to address. This is needed to store the unique address of the
  // --- activated sensor in the CbmStsPoint class. Unfortunately, the
  // --- full geometry path (string) is the only way to unambiguously
  // --- identify the current active node during the transport.
  // --- It may seem that looking up a string in a map is not efficient.
  // --- I checked however, that the performance penalty is very small.
  fAddressMap.clear();
  fSetup = CbmStsSetup::Instance();
  	Int_t nStat = fSetup->GetNofDaughters();
  	for (Int_t iStat = 0; iStat < nStat; iStat++) {
  		CbmStsElement* stat = fSetup->GetDaughter(iStat);
  		Int_t nLadd = stat->GetNofDaughters();
  		for (Int_t iLadd = 0; iLadd < nLadd; iLadd++) {
  			CbmStsElement* ladd = stat->GetDaughter(iLadd);
  			Int_t nHlad = ladd->GetNofDaughters();
  			for (Int_t iHlad = 0; iHlad < nHlad; iHlad++) {
  				CbmStsElement* hlad = ladd->GetDaughter(iHlad);
  				Int_t nModu = hlad->GetNofDaughters();
  				for (Int_t iModu = 0; iModu < nModu; iModu++) {
  					CbmStsElement* modu = hlad->GetDaughter(iModu);
  					Int_t nSens = modu->GetNofDaughters();
  					for (Int_t iSens = 0; iSens < nSens; iSens++) {
  						CbmStsElement* sensor = modu->GetDaughter(iSens);
  						TString path = sensor->GetPnode()->GetName();
  						if ( ! path.BeginsWith("/") ) path.Prepend("/");
  						pair<TString, Int_t> a(path, sensor->GetAddress());
  						fAddressMap.insert(a);
  						TString test = sensor->GetPnode()->GetName();
  					}
  				}
  			}
  		}
  	}
  	LOG(INFO) << fName << ": Address map initialised with "
  			      << Int_t(fAddressMap.size()) << " sensors. "
  			      << FairLogger::endl;

  	// --- Call the Initialize method of the mother class
  	FairDetector::Initialize();
}
// -------------------------------------------------------------------------



// -----   ProcessHits  ----------------------------------------------------
Bool_t CbmStsMC::ProcessHits(FairVolume* vol) {


  // --- If track is entering the sensor:
  //     store track parameters and reset energy loss
  if ( gMC->IsTrackEntering() ) {
		SetStatus(fStatusIn);
		fEloss = 0.;
	}


  // --- For all steps within active volume: sum up differential energy loss
  fEloss += gMC->Edep();


  // --- If track is leaving: get track parameters and create CbmstsPoint
  if ( gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared()   ) {

  	SetStatus(fStatusOut);

  	// --- No action if no energy loss (neutral particles)
  	if (fEloss == 0. ) return kFALSE;

  	// --- Add a StsPoint to the output array. Increment stack counter.
  	CreatePoint();

    // --- Increment number of StsPoints for this track
    CbmStack* stack = (CbmStack*) gMC->GetStack();
    stack->AddPoint(kSTS);

    // --- Reset track status and accumulated energy loss
    fStatusIn.Reset();
    fStatusOut.Reset();
    fEloss = 0.;

  }  //? track is exiting or stopped


  return kTRUE;
}
// -------------------------------------------------------------------------



// -----   Reset output array and track status   ---------------------------
void CbmStsMC::Reset() {
  fStsPoints->Delete();
  fStatusIn.Reset();
  fStatusOut.Reset();
  fEloss = 0.;
}
// -------------------------------------------------------------------------



// -----   Screen log   ----------------------------------------------------
void CbmStsMC::Print(Option_t* opt) const {
  Int_t nHits = fStsPoints->GetEntriesFast();
  LOG(INFO) << fName << ": " << fStsPoints->GetEntriesFast()
            << " points registered in this event." << FairLogger::endl;
}
// -------------------------------------------------------------------------



// =========================================================================
// Private methods
// =========================================================================


// -----   Create STS point   ----------------------------------------------
CbmStsPoint* CbmStsMC::CreatePoint() {
	
  // --- Check detector address	
  if ( fStatusIn.fAddress != fStatusOut.fAddress ) {
    LOG(ERROR) << GetName() << ": inconsistent detector addresses "
               << fStatusIn.fAddress << " " << fStatusOut.fAddress 
               << FairLogger::endl;
    return NULL;
  }
  
  // --- Check track Id
  if ( fStatusIn.fTrackId != fStatusOut.fTrackId ) {
    LOG(ERROR) << GetName() << ": inconsistent track Id "
               << fStatusIn.fTrackId << " " << fStatusOut.fTrackId 
               << FairLogger::endl;
    return NULL;
  }
  
  // --- Check track PID
  if ( fStatusIn.fPid != fStatusOut.fPid ) {
    LOG(ERROR) << GetName() << ": inconsistent track PID "
               << fStatusIn.fPid << " " << fStatusOut.fPid
               << FairLogger::endl;
    return NULL;
  }

  // --- Entry position and momentum
  TVector3 posIn(fStatusIn.fX,  fStatusIn.fY,  fStatusIn.fZ);
  TVector3 momIn(fStatusIn.fPx, fStatusIn.fPy, fStatusIn.fPz);
  
  // --- Exit position and momentum
  TVector3 posOut(fStatusOut.fX,  fStatusOut.fY,  fStatusOut.fZ);
  TVector3 momOut(fStatusOut.fPx, fStatusOut.fPy, fStatusOut.fPz);
  
  // --- Time and track length (average between in and out)
  Double_t time   = 0.5 * ( fStatusIn.fTime   + fStatusOut.fTime );
  Double_t length = 0.5 * ( fStatusIn.fLength + fStatusOut.fLength);
  
  // --- Debug output
  LOG(DEBUG2) << GetName() << ": Creating point from track "
              << fStatusIn.fTrackId << " in sensor " 
              << fStatusIn.fAddress << ", position (" << posIn.X()
              << ", " << posIn.Y() << ", " << posIn.Z() 
              << "), energy loss " << fEloss << FairLogger::endl;
              
  // --- Add new point to output array
  return new ( (*fStsPoints)[fStsPoints->GetEntriesFast()] )
    CbmStsPoint(fStatusIn.fTrackId, fStatusIn.fAddress, posIn, posOut,
                momIn, momOut, time, length, fEloss, fStatusIn.fPid);
                
}
// -------------------------------------------------------------------------



// -----   Set the current track status   ----------------------------------
void CbmStsMC::SetStatus(CbmStsTrackStatus& status) {

  // --- Check for TVirtualMC and TGeomanager
  if ( ! (gMC  && gGeoManager) ) {
	LOG(ERROR) << fName << ": No TVirtualMC or TGeoManager instance!"
	           << FairLogger::endl;
		return;
  }

  // --- Address of current sensor
  // --- Use the geometry path from TVirtualMC; cannot rely on
  // --- TGeoManager here.
  TString path = gMC->CurrentVolPath();
  map<TString, UInt_t>::iterator it = fAddressMap.find(path);
  if ( it == fAddressMap.end() ) {
  	LOG(INFO) << fName << ": Path not found in address map! "
  			      << gGeoManager->GetPath() << FairLogger::endl;
  	status.fAddress = 0;
  }
  else status.fAddress = it->second;

  // --- Index and PID of current track
  status.fTrackId  = gMC->GetStack()->GetCurrentTrackNumber();
  status.fPid      = gMC->GetStack()->GetCurrentTrack()->GetPdgCode();

  // --- Position
  gMC->TrackPosition(status.fX, status.fY, status.fZ);

  // --- Momentum
  Double_t dummy;
  gMC->TrackMomentum(status.fPx, status.fPy, status.fPz, dummy);

  // --- Time and track length
  status.fTime   = gMC->TrackTime() * 1.e9;  // conversion into ns
  status.fLength = gMC->TrackLength();

}
// -------------------------------------------------------------------------


ClassImp(CbmStsMC)
