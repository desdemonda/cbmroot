// -------------------------------------------------------------------------
// -----                  CbmEcalHitProducerFastMC header file               -----
// -----                  Created 20/06/06  by Yu.Kharlov              -----
// -------------------------------------------------------------------------

  /* $Id: CbmEcalHitProducerFastMC.h,v 1.1 2006/06/22 06:55:01 kharlov Exp $ */

  /* History of cvs commits:
   *
   * $Log: CbmEcalHitProducerFastMC.h,v $
   * Revision 1.1  2006/06/22 06:55:01  kharlov
   * Newly added classes for Fas-MC hit producer
   *
   */

/**  CbmEcalHitProducerFastMC.h
 *@author Yu.Kharlov
 **
 ** Class for producing ECAL hits from MCPoints for Fast MC
 **/


#ifndef CBMECALHITPRODUCERFASTMC_H
#define CBMECALHITPRODUCERFASTMC_H

#include "CbmEcalHit.h"

#include "FairTask.h"
#include "CbmStack.h"

#include "TClonesArray.h"

class CbmEcalHitProducerFastMC : public FairTask {

public:

  /** Default constructor **/
  CbmEcalHitProducerFastMC();

  /** Standard constructor **/
  CbmEcalHitProducerFastMC(const char *name, const Int_t iVerbose=1);

  /** Destructor **/
  virtual ~CbmEcalHitProducerFastMC();

  /** Initialization of the task **/  
  virtual InitStatus Init();

  /** Executed task **/ 
  virtual void Exec(Option_t* option);

  /** Finish task **/ 
  virtual void Finish();

  /** Set energy resolution **/ 
  void SetEResolution(Double_t a, Double_t b, Double_t c) {fA=a; fB=b; fC=c;}

  /** method AddHit
   **
   ** Adds a EcalHit to the HitCollection
   **/
  void AddHit(Double_t x, Double_t y, Double_t E);

protected:

private:

  Double_t Eresolution(Double_t E);
  Double_t HadronResponse(Double_t E);

private:

  // Digitization parameters

  Double_t fA;                  // dE/E=a/E
  Double_t fB;                  // dE/E=b/sqrt(E)
  Double_t fC;                  // dE/E=c
  Double_t fCellSize;           // supermodule size
  Double_t fEThreshold;         // Energy threshold
  Int_t fRegionXm[4];           // zone X-size in supermodules
  Int_t fRegionYm[4];           // zone Y-size in supermodules

  TClonesArray* fListECALpts;   // ECAL MC points
  TClonesArray* fHitCollection; // ECAL hits
  TClonesArray* fListStack;     // Tracks
  Int_t fNHits;                 // Number of hits
  Int_t fEvent;                 //! Internal event counter

  CbmEcalHitProducerFastMC(const CbmEcalHitProducerFastMC&);
  CbmEcalHitProducerFastMC& operator=(const CbmEcalHitProducerFastMC&);

  ClassDef(CbmEcalHitProducerFastMC,1)

};

#endif
