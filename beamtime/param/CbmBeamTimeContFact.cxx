/////////////////////////////////////////////////////////////
//
//  CbmBeamTimeContFact
//
//  Factory for the parameter containers in libBeamtime
//
/////////////////////////////////////////////////////////////

#include "CbmBeamTimeContFact.h"

#include "CbmBeamTimeFirstPar.h"

#include "FairRuntimeDb.h"

ClassImp(CbmBeamTimeContFact)

static CbmBeamTimeContFact gCbmBeamTimeContFact;

CbmBeamTimeContFact::CbmBeamTimeContFact() {
  // Constructor (called when the library is loaded)
  fName="CbmBeamTimeContFact";
  fTitle="Factory for parameter containers in libBeamtime";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void CbmBeamTimeContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the beamtime library.*/

    FairContainer* p= new FairContainer("CbmBeamTimeFirstPar",
                                          "First parameters for testing",
                                          "TestDefaultContext");
    p->addContext("TestNonDefaultContext");

    containers->Add(p);
}

FairParSet* CbmBeamTimeContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  std::cout << " -I conatiner name " << name << std::endl;
  FairParSet* p=0;
  if (strcmp(name,"CbmBeamTimeFirstPar")==0) {
      p=new CbmBeamTimeFirstPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}

