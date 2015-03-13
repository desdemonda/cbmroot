#ifndef CBMECALCONTFACT_H
#define CBMECALCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class CbmEcalContFact : public FairContFact {
private:
  void setAllContainers();
public:
  CbmEcalContFact();
  ~CbmEcalContFact() {}
  FairParSet* createContainer(FairContainer*);
  void  activateParIo(FairParIo* io);
  ClassDef( CbmEcalContFact,0) // Factory for all ECAL parameter containers
};

#endif  /* !CBMECALCONTFACT_H */
