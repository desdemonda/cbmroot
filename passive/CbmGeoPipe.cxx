//*-- AUTHOR : Ilse Koenig
//*-- Created : 10/11/2003

/////////////////////////////////////////////////////////////
// CbmGeoPipe
//
// Class for geometry of support structure
//
/////////////////////////////////////////////////////////////

#include "CbmGeoPipe.h"

#include "FairGeoLoader.h"
#include "FairGeoInterface.h"
    
ClassImp(CbmGeoPipe)

CbmGeoPipe::CbmGeoPipe() 
: FairGeoSet(),
  modName(),
  eleName()
{
  // Constructor
  fName="pipe";
  strcpy(modName,"p");
  strcpy(eleName,"p");
  maxSectors=0;
  maxModules=1;
}
Bool_t  CbmGeoPipe::create ( FairGeoBuilder * build ) {
    Bool_t rc = FairGeoSet::create( build );
    if ( rc ) {
	FairGeoLoader *loader=FairGeoLoader::Instance();
	FairGeoInterface *GeoInterface =loader->getGeoInterface();

        GeoInterface->getMasterNodes()->Add( (TObject*)  getVolume("pipevac1") );

    }
 return rc;
}

CbmGeoPipe::~CbmGeoPipe() {

}

