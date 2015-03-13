/** CbmMuchContFact.cxx
 *
 * @author  M. Ryzhinskiy <m.ryzhinskiy@gsi.de>
 * @version 0.0
 * @since   15.03.07
 * 
 * Factory for the parameter containers for MUon CHambers detector
 *
 */
#include "CbmMuchContFact.h"

#include "CbmGeoMuchPar.h"

#include "FairRuntimeDb.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"

#include "TClass.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(CbmMuchContFact)

static CbmMuchContFact gCbmMuchContFact;

CbmMuchContFact::CbmMuchContFact() {
  // Constructor (called when the library is loaded)
  fName="CbmMuchContFact";
  fTitle="Factory for parameter containers in libMuch";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void CbmMuchContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the MuCh library.*/
//   FairContainer* p1= new FairContainer("CbmMuchDigiPar",
// 				     "Much Digitization Parameters",
// 				     "TestDefaultContext");
//   p1->addContext("TestNonDefaultContext");
  
  FairContainer* p2= new FairContainer("CbmGeoMuchPar",
				     "Much Geometry Parameters",
				     "TestDefaultContext");
  p2->addContext("TestNonDefaultContext");
  
  //  containers->Add(p1);
  containers->Add(p2);

}

FairParSet* CbmMuchContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  cout << " -I container name " << name << endl;
  FairParSet* p=0;
  if (strcmp(name,"CbmGeoMuchPar")==0) {
    p=new CbmGeoMuchPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;

}

void  CbmMuchContFact::activateParIo(FairParIo* io) {
  // activates the input/output class for the parameters
  // needed by the MuCh
//   if (strcmp(io->IsA()->GetName(),"FairParRootFileIo")==0) {
//     CbmMuchParRootFileIo* p=new CbmMuchParRootFileIo(((FairParRootFileIo*)io)->getParRootFile());
//     io->setDetParIo(p);
//   }
//   else if (strcmp(io->IsA()->GetName(),"FairParAsciiFileIo")==0) {
//     CbmMuchParAsciiFileIo* p=new CbmMuchParAsciiFileIo(((FairParAsciiFileIo*)io)->getFile());
//     io->setDetParIo(p);
//   }
}

