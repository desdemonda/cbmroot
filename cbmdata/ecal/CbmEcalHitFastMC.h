// -------------------------------------------------------------------------
// -----               CbmEcalHitFastMC header file                    -----
// -----               Created 21/06/06  by Yu.Kharlov                 -----
// -------------------------------------------------------------------------

  /* $Id: CbmEcalHitFastMC.h,v 1.1 2006/06/22 06:55:01 kharlov Exp $ */

  /* History of cvs commits:
   *
   * $Log: CbmEcalHitFastMC.h,v $
   * Revision 1.1  2006/06/22 06:55:01  kharlov
   * Newly added classes for Fas-MC hit producer
   *
   */

/**  CbmEcalHitFastMC.h
 *@author Yu.Kharlov
 **
 ** ECAL hit which is a photon with the total energy and
 ** coordinate position. To be used in Fast MC.
 **/

#ifndef CBMECALHITFASTMC_H
#define CBMECALHITFASTMC_H

#include "FairHit.h"

class CbmEcalHitFastMC : public FairHit {

public:    
  
  /** Default constructor **/
  CbmEcalHitFastMC();

  /** Constructor **/
  CbmEcalHitFastMC(Double_t x, Double_t y, Double_t E);

  /** Destructor **/
  virtual ~CbmEcalHitFastMC();
  
  /** Output to screen **/
  virtual void Print(const Option_t* opt ="") const;
  
  /** Modifiers **/
  void SetAmplitude(Double_t amp) {fAmplitude = amp;}
  
  /** Accessors **/
  Double_t GetAmplitude() {return fAmplitude;}
  
protected:

  Double32_t fAmplitude;    // hit amplitude

  ClassDef(CbmEcalHitFastMC,1)
};

#endif //CBMRICHHITFASTMC_H
