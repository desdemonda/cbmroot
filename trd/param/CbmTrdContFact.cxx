//*-- AUTHOR : Ilse Koenig
//*-- Created : 25/10/2004

/////////////////////////////////////////////////////////////
//
//  CbmTrdContFact
//
//  Factory for the parameter containers in libTrd
//
/////////////////////////////////////////////////////////////

#include "CbmTrdContFact.h"

#include "CbmGeoTrdPar.h"
#include "CbmTrdDigiPar.h"

#include "FairRuntimeDb.h"

ClassImp(CbmTrdContFact)

static CbmTrdContFact gCbmTrdContFact;

CbmTrdContFact::CbmTrdContFact() {
  // Constructor (called when the library is loaded)
  fName="CbmTrdContFact";
  fTitle="Factory for parameter containers in libTrd";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void CbmTrdContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the STS library.*/

    FairContainer* p= new FairContainer("CbmGeoTrdPar",
                                          "Trd Geometry Parameters",
                                          "TestDefaultContext");
    p->addContext("TestNonDefaultContext");

    containers->Add(p);
 
    FairContainer* p1= new FairContainer("CbmTrdDigiPar",
                                          "Trd Digi Parameters",
                                          "TestDefaultContext");
    p1->addContext("TestNonDefaultContext");

    containers->Add(p1);
}

FairParSet* CbmTrdContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
//  cout << " -I conatiner name " << name << endl;
  FairParSet* p=0;
  if (strcmp(name,"CbmGeoTrdPar")==0) {
      p=new CbmGeoTrdPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  if (strcmp(name,"CbmTrdDigiPar")==0) {
      p=new CbmTrdDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}

