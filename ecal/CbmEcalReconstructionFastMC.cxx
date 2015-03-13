// -------------------------------------------------------------------------
// -----         CbmEcalReconstructionFastMC source file               -----
// -----         Created 22/06/06  by Yu.Kharlov                       -----
// -------------------------------------------------------------------------

  /* $Id: CbmEcalReconstructionFastMC.cxx,v 1.1 2006/06/22 14:02:17 kharlov Exp $ */

  /* History of cvs commits:
   *
   * $Log: CbmEcalReconstructionFastMC.cxx,v $
   * Revision 1.1  2006/06/22 14:02:17  kharlov
   * First upload of reconstruction classes for Full MC
   *
   */

/**  CbmEcalReconstructionFastMC.cxx
 *@author Yu.Kharlov
 **
 ** Class for ECAL reconstruction, Fast MC version:
 ** calculate 4-momenta of reconstructed particles assuming
 ** that the particle comes from the target center
 **/
#include "CbmEcalReconstructionFastMC.h"

#include "CbmEcalRecParticle.h"
#include "CbmEcalHitFastMC.h"

#include "FairRootManager.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
CbmEcalReconstructionFastMC::CbmEcalReconstructionFastMC() 
  : FairTask(),
    fListECALhits(NULL),
    fListRecParticles(NULL),
    fNRecParticles(0),
    fEvent(0),
    fZEcal(1200.)
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   -------------------------------------------
CbmEcalReconstructionFastMC::CbmEcalReconstructionFastMC(const char *name, const Int_t iVerbose) 
  : FairTask(name,iVerbose),
    fListECALhits(NULL),
    fListRecParticles(NULL),
    fNRecParticles(0),
    fEvent(0),
    fZEcal(1200.)
{
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmEcalReconstructionFastMC::~CbmEcalReconstructionFastMC()
{
  FairRootManager *fManager =FairRootManager::Instance();
  fManager->Write();
}
// -------------------------------------------------------------------------


// -----   Initialization   ------------------------------------------------
InitStatus CbmEcalReconstructionFastMC::Init()
{
  FairRootManager* fManager = FairRootManager::Instance();

  // ECAL hits
  fListECALhits = (TClonesArray*)fManager->GetObject("EcalHitFastMC");
  // new list of ECAL reconstructed particles
  fListRecParticles = new TClonesArray("CbmEcalRecParticle",100);
  fManager->Register("EcalRecParticle","ECAL",fListRecParticles,kTRUE);
  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -----   Execution of Task   ---------------------------------------------
void CbmEcalReconstructionFastMC::Exec(Option_t* option)
{

  fEvent++;
  printf("\n=====> Event no. %d\n",fEvent);
  printf("Number of input ECAL hits: %d\n",fListECALhits->GetEntries());

  fListRecParticles->Clear();
  fNRecParticles = 0;
  CbmEcalHitFastMC *hit;
  Double_t x,y,px,py,pz,E;
  
  for(Int_t j=0; j<fListECALhits->GetEntries(); j++) {
    hit = (CbmEcalHitFastMC*)fListECALhits->At(j);
    E = hit->GetAmplitude();
    x = hit->GetX();
    y = hit->GetY();
    px = E*x/fZEcal;
    py = E*y/fZEcal;
    pz = TMath::Sqrt(E*E - px*px - py*py);
    AddRecParticle(px,py,pz,E);
  }   // loop over hits
//   for (Int_t i=0; i<fNRecParticles; i++) {
//     printf("Rec.particle %d\n",i);
//     ((CbmEcalRecParticle*)fListRecParticles->At(fNRecParticles))->Print();
//   }  
}
// -------------------------------------------------------------------------


// -----   Add Hit to HitCollection   --------------------------------------
void CbmEcalReconstructionFastMC::AddRecParticle(Double_t px, Double_t py,
					   Double_t pz, Double_t E)
{
  // Add this hit to the list of ECAL hits
  new((*fListRecParticles)[fNRecParticles]) CbmEcalRecParticle(px,py,pz,E);
  fNRecParticles++;
}
// -------------------------------------------------------------------------


// -----   Finish Task   ---------------------------------------------------
void CbmEcalReconstructionFastMC::Finish()
{
  fListRecParticles->Clear();
}
// -------------------------------------------------------------------------

ClassImp(CbmEcalReconstructionFastMC)
