/** CbmMuchContFact.h
 *
 * @author  M. Ryzhinskiy <m.ryzhinskiy@gsi.de>
 * @version 0.0
 * @since   15.03.07
 * 
 *  Factory for the parameter containers for MUon CHambers detector
 *
 */

#ifndef CBMMUCHCONTFACT_H
#define CBMMUCHCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class CbmMuchContFact : public FairContFact {
private:
  void setAllContainers();
public:
  CbmMuchContFact();
  ~CbmMuchContFact() {}
  FairParSet* createContainer(FairContainer*);
  void  activateParIo(FairParIo* io);
  ClassDef( CbmMuchContFact,0) // Factory for all Much parameter containers
};

#endif  /* !CBMMUCHCONTFACT_H */
