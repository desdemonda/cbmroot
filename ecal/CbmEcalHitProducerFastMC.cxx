// -------------------------------------------------------------------------
// -----               CbmEcalHitProducerFastMC source file                  -----
// -----               Created 11/07/05  by Yu.Kharlov                 -----
// -------------------------------------------------------------------------

  /* $Id: CbmEcalHitProducerFastMC.cxx,v 1.1 2006/06/22 06:55:01 kharlov Exp $ */

  /* History of cvs commits:
   *
   * $Log: CbmEcalHitProducerFastMC.cxx,v $
   * Revision 1.1  2006/06/22 06:55:01  kharlov
   * Newly added classes for Fas-MC hit producer
   *
   */

/**  CbmEcalHitProducerFastMC.cxx
 *@author Yu.Kharlov
 **
 ** Class for producing ECAL hits from MCPoints
 ** It takes a list of EcalPoints produced by CbmEcal and
 ** converts it to a list of EcalHits with smeared energy
 ** and position.
 **/
#include "CbmEcalHitProducerFastMC.h"

#include "CbmEcalPoint.h"
#include "CbmEcalHitFastMC.h"

#include "FairRootManager.h"
#include "CbmMCTrack.h"

#include "TVector3.h"
#include "TRandom.h"

#include <iostream>

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmEcalHitProducerFastMC::CbmEcalHitProducerFastMC() 
  : FairTask(), 
    fA(0.),
    fB(0.),
    fC(0.),
    fCellSize(0.),
    fEThreshold(0.),
    fRegionXm(),
    fRegionYm(),
    fListECALpts(NULL),
    fHitCollection(NULL),
    fListStack(NULL),
    fNHits(0),
    fEvent(0)
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   -------------------------------------------
CbmEcalHitProducerFastMC::CbmEcalHitProducerFastMC(const char *name, const Int_t iVerbose) 
  : FairTask(name,iVerbose),
    fA(0.02),
    fB(0.05),
    fC(0.005),
    fCellSize(12.),
    fEThreshold(0.5),
    fRegionXm(),
    fRegionYm(),
    fListECALpts(NULL),
    fHitCollection(NULL),
    fListStack(NULL),
    fNHits(0),
    fEvent(0)
{
  // Set energy resolution parameters, zone sizes, energy threshold
  fRegionXm[0] = 5;     // ECAL hole 5x5 modules
  fRegionYm[0] = 5;
  fRegionXm[1] = 33;    // ECAL inner zone 33x27 modules
  fRegionYm[1] = 27;
  fRegionXm[2] = 57;    // ECAL middle zone 57x49 modules
  fRegionYm[2] = 49;
  fRegionXm[3] = 99;    // ECAL outer zone 99x79 modules
  fRegionYm[3] = 79;
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmEcalHitProducerFastMC::~CbmEcalHitProducerFastMC()
{
  FairRootManager *fManager =FairRootManager::Instance();
  fManager->Write();
}
// -------------------------------------------------------------------------


// -----   Initialization   ------------------------------------------------
InitStatus CbmEcalHitProducerFastMC::Init()
{
  FairRootManager* fManager = FairRootManager::Instance();

  // ECAL MC points
  fListECALpts = (TClonesArray*)fManager->GetObject("EcalPoint");
  // all tracks
  fListStack = (TClonesArray *)fManager->GetObject("MCTrack");
  // new list of ECAL hits
  fHitCollection = new TClonesArray("CbmEcalHitFastMC",100);
  fManager->Register("EcalHitFastMC","ECAL",fHitCollection,kTRUE);
  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -----   Execution of Task   ---------------------------------------------
void CbmEcalHitProducerFastMC::Exec(Option_t* option)
{
  // Convert EcalPoints for photons into hits:
  // energy is smeared according to energy resolution,
  // position is smeared with 3 different position resolution for each sone
  // only photons with smeared energy are accepted

  CbmEcalPoint* pt=NULL;

  fEvent++;
  printf("\n\n=====> Event no. %d\n",fEvent);

  cout<<"Number of input MC points: "<<fListECALpts->GetEntries()<<endl;

  fHitCollection->Clear();
  fNHits = 0;

  TVector3 posPoint;
  Double_t x,y,E, xResolution;
  for(Int_t j=0; j<fListECALpts->GetEntries(); j++) {
    pt = (CbmEcalPoint*)fListECALpts->At(j);
    pt->Position(posPoint);
    Int_t trackID = pt->GetTrackID(); 
    CbmMCTrack *track= (CbmMCTrack*) fListStack->At(trackID);
    Int_t pdgCode = track->GetPdgCode();
    E = pt->GetEnergyLoss();
    x = posPoint.X();
    y = posPoint.Y();
    Int_t cellX = (Int_t)(TMath::Abs(x)/fCellSize);
    Int_t cellY = (Int_t)(TMath::Abs(y)/fCellSize);
    if (cellX<=fRegionXm[0]/2 && cellY<=fRegionYm[0]/2 ||
	cellX> fRegionXm[3]/2 || cellY> fRegionYm[3]/2 ) continue; // in the hole

    if (pdgCode == 22 || TMath::Abs(pdgCode) == 11) { // photon or e+-
      xResolution = 0.0;
      if      (cellX<=fRegionXm[1]/2 && cellY<=fRegionXm[1]/2) xResolution=0.3;
      else if (cellX<=fRegionXm[2]/2 && cellY<=fRegionXm[2]/2) xResolution=0.6;
      else if (cellX<=fRegionXm[3]/2 && cellY<=fRegionXm[3]/2) xResolution=2.0;
      x = gRandom->Gaus(x,xResolution);
      y = gRandom->Gaus(y,xResolution);
      E = gRandom->Gaus(E,Eresolution(E));
    }
    else {                                            // hadron
      xResolution = 0.0;
      if      (cellX<=fRegionXm[1]/2 && cellY<=fRegionXm[1]/2) xResolution=1.0;
      else if (cellX<=fRegionXm[2]/2 && cellY<=fRegionXm[2]/2) xResolution=2.0;
      else if (cellX<=fRegionXm[3]/2 && cellY<=fRegionXm[3]/2) xResolution=10.0;
      x = gRandom->Gaus(x,xResolution);
      y = gRandom->Gaus(y,xResolution);
      E = HadronResponse(E);
    }
    if (E>fEThreshold) AddHit(x,y,E);
  }   // loop over input points
}
// -------------------------------------------------------------------------


// -----   Add Hit to HitCollection   --------------------------------------
void CbmEcalHitProducerFastMC::AddHit(Double_t x, Double_t y, Double_t E)
{
  // Add this hit to the list of ECAL hits
  new((*fHitCollection)[fNHits]) CbmEcalHitFastMC(x,y,E);
  fNHits++;
}
// -------------------------------------------------------------------------


// -----   Finish Task   ---------------------------------------------------
void CbmEcalHitProducerFastMC::Finish()
{
  fHitCollection->Clear();
}
// -------------------------------------------------------------------------

// -----   Energy resolution  ----------------------------------------------
Double_t CbmEcalHitProducerFastMC::Eresolution(Double_t E)
{
  // energy resolution vs energy
  return TMath::Sqrt(fA*fA + fB*fB*E + fC*fC*E*E);
}
// -------------------------------------------------------------------------

// -----   Energy deposited in ECAL by hadrons  ----------------------------
Double_t CbmEcalHitProducerFastMC::HadronResponse(Double_t E)
{
  // energy resolution vs energy
  Double_t energy = 0.2;
  return energy;
}
// -------------------------------------------------------------------------


ClassImp(CbmEcalHitProducerFastMC)
