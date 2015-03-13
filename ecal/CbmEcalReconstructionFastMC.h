// -------------------------------------------------------------------------
// -----         CbmEcalReconstructionFastMC header file               -----
// -----         Created 22/06/06  by Yu.Kharlov                       -----
// -------------------------------------------------------------------------

  /* $Id: CbmEcalReconstructionFastMC.h,v 1.1 2006/06/22 14:02:17 kharlov Exp $ */

  /* History of cvs commits:
   *
   * $Log: CbmEcalReconstructionFastMC.h,v $
   * Revision 1.1  2006/06/22 14:02:17  kharlov
   * First upload of reconstruction classes for Full MC
   *
   */

/**  CbmEcalReconstructionFastMC.h
 *@author Yu.Kharlov
 **
 ** Class for ECAL reconstruction, Fast MC version:
 ** calculate 4-momenta of reconstructed particles assuming
 ** that the particle comes from the target center
 **/


#ifndef CBMECALRECONSTRUCTIONFASTMC_H
#define CBMECALRECONSTRUCTIONFASTMC_H

#include "FairTask.h"

#include "TClonesArray.h"

class CbmEcalReconstructionFastMC : public FairTask {

public:

  /** Default constructor **/
  CbmEcalReconstructionFastMC();

  /** Standard constructor **/
  CbmEcalReconstructionFastMC(const char *name, const Int_t iVerbose=1);

  /** Destructor **/
  virtual ~CbmEcalReconstructionFastMC();

  /** Initialization of the task **/  
  virtual InitStatus Init();

  /** Executed task **/ 
  virtual void Exec(Option_t* option);

  /** Finish task **/ 
  virtual void Finish();

  /** method AddHit
   **
   ** Adds a EcalHit to the HitCollection
   **/
  void AddRecParticle(Double_t px, Double_t py, Double_t pz, Double_t E);

private:

  TClonesArray* fListECALhits;     // ECAL hits
  TClonesArray* fListRecParticles; // ECAL reconstructed particles
  Int_t fNRecParticles;            // Number of reconstructed particles
  Int_t fEvent;                    //! Internal event counter
  Double_t fZEcal;                 // distance from target to ECAL

  CbmEcalReconstructionFastMC(const CbmEcalReconstructionFastMC&);
  CbmEcalReconstructionFastMC& operator=(const CbmEcalReconstructionFastMC&);

  ClassDef(CbmEcalReconstructionFastMC,1)

};

#endif
