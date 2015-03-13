#ifndef CBMBEAMTIMECONTFACT_H
#define CBMBEAMTIMECONTFACT_H

#include "FairContFact.h"

class FairContainer;
class FairParSet;

class CbmBeamTimeContFact : public FairContFact {
private:
  void setAllContainers();
  CbmBeamTimeContFact(const CbmBeamTimeContFact&);
  CbmBeamTimeContFact& operator=(const CbmBeamTimeContFact&);
public:
  CbmBeamTimeContFact();
  ~CbmBeamTimeContFact() {}
  FairParSet* createContainer(FairContainer*);
  ClassDef(CbmBeamTimeContFact,0) // Factory for all TRD parameter containers
};

#endif  /* !CBMTRDCONTFACT_H */
