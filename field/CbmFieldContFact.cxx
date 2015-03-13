// -------------------------------------------------------------------------
// -----                    CbmFieldContFact source file               -----
// -----                   Created 20/02/06  by V. Friese              -----
// -------------------------------------------------------------------------
#include "CbmFieldContFact.h"

#include "CbmFieldPar.h"

#include "FairParSet.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include <iostream>

using std::cout;
using std::endl;


static CbmFieldContFact gCbmFieldContFact;


// -----   Constructor   ---------------------------------------------------
CbmFieldContFact::CbmFieldContFact() {
  fName = "CbmFieldContFact";
  fTitle = "Factory for field parameter containers";
  SetAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmFieldContFact::~CbmFieldContFact() { }
// -------------------------------------------------------------------------



// -----   Create containers   ---------------------------------------------
FairParSet* CbmFieldContFact::createContainer(FairContainer* container) {


  const char* name = container->GetName();
  LOG(INFO) << "create CbmFieldPar container " << name << FairLogger::endl;
  FairParSet* set = NULL;
  if ( strcmp(name, "CbmFieldPar") == 0 ) 
    set = new CbmFieldPar( container->getConcatName().Data(),
			   container->GetTitle(),
			   container->getContext() );
  return set;
}
// -------------------------------------------------------------------------



// -----   Set all containers (private)   ----------------------------------
void CbmFieldContFact::SetAllContainers() {
  FairContainer* container = new FairContainer("CbmFieldPar",
					     "Field parameter container",
					     "Default field");
  containers->Add(container);
}
// -------------------------------------------------------------------------


ClassImp(CbmFieldContFact)
