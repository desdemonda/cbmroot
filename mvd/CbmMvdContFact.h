#ifndef CBMMVDCONTFACT_H
#define CBMMVDCONTFACT_H

#include "FairContFact.h"

class FairContainer;
class FairParIo;

class CbmMvdContFact : public FairContFact {
private:
  void setAllContainers();
public:
  CbmMvdContFact();
  ~CbmMvdContFact() {}
  FairParSet* createContainer(FairContainer*);
  ClassDef( CbmMvdContFact,0) // Factory for all STS parameter containers
};

#endif  /* !CBMSTSCONTFACT_H */
