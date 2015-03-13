//*-- AUTHOR : Denis Bertini
//*-- Created : 20/06/2005

/////////////////////////////////////////////////////////////
//
//  CbmStsContFact
//
//  Factory for the parameter containers in libSts
//
/////////////////////////////////////////////////////////////

#include "CbmMvdContFact.h"

#include "CbmMvdGeoPar.h"

#include "FairRuntimeDb.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"
#include "FairParIo.h"
#include "FairParGenericSet.h"
#include "FairLogger.h"

#include "TClass.h"

#include <iostream>
#include <iomanip>

using std::cout;
using std::endl;

ClassImp(CbmMvdContFact)

static CbmMvdContFact gCbmMvdContFact;

CbmMvdContFact::CbmMvdContFact() {
  // Constructor (called when the library is loaded)
  fName="CbmMvdContFact";
  fTitle="Factory for parameter containers in libMvd";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void CbmMvdContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the STS library.*/
    FairContainer* p= new FairContainer("CbmMvdGeoPar",
                                          "Mvd Geometry Parameters",
                                          "Default");
    p->addContext("TestNonDefaultContext");

    containers->Add(p);
}

FairParSet* CbmMvdContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  LOG(DEBUG) << "container name " << name << FairLogger::endl;
  FairParSet* p=0;
  if (strcmp(name,"CbmMvdGeoPar")==0) {
    p=new CbmMvdGeoPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}
