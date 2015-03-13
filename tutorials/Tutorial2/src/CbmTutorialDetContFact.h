#ifndef CBMTUTORIALDETCONTFACT_H
#define CBMTUTORIALDETCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class CbmTutorialDetContFact : public FairContFact {
 private:
  void setAllContainers();
 public:
  CbmTutorialDetContFact();
  ~CbmTutorialDetContFact() {}
  FairParSet* createContainer(FairContainer*);
  ClassDef( CbmTutorialDetContFact,0) // Factory for all MyDet parameter containers
};

#endif  /* !CBMTUTORIALDETCONTFACT_H */
