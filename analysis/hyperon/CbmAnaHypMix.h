// -------------------------------------------------------------------------
// -----                  CbmAnaHypMix header file                     -----
// -----               Created 02/03/05  by E. Kryshen                 -----
// -------------------------------------------------------------------------

/** @class CbmAnaHypMix.h
 ** @author E.Kryshen  <e.kryshen@gsi.de>
 ** @date 22.06.2006
 ** @date 08.02.2007
 **
 ** Full reconstruction for cascade multistrange hyperons decay.
 **
 */

#ifndef CBMANAHYPMIX_H
#define CBMANAHYPMIX_H

#include "CbmAnaHypRecCascade.h"

class CbmAnaHypMix : public CbmAnaHypRecCascade{

public:

  /** Default constructor */
 CbmAnaHypMix() : CbmAnaHypRecCascade(), fNoMixedEv(0) {};

  /** Standard constructor */
  CbmAnaHypMix(Int_t nMixedEvents, Int_t iVerbose);

  /** Destructor */
  virtual ~CbmAnaHypMix(){};

  /** Execute task */
  void Exec(Option_t* option);

  // Setters
  void SetNoMixedEvents (Int_t nEv) { fNoMixedEv = nEv;}


protected:
  Int_t    fNoMixedEv;

ClassDef(CbmAnaHypMix,1);
};

#endif
