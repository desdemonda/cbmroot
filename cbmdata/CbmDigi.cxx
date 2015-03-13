/** @file CbmDigi.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 30.04.2013
 **/

#include "CbmDigi.h"
#include "CbmMatch.h"

#include "FairMultiLinkedData.h"

#include <memory>

// -----   Default constructor   -------------------------------------------
CbmDigi::CbmDigi() 
 : TObject(), 
   fMatch(NULL) 
{
}
// -------------------------------------------------------------------------



// -----   Copy constructor (deep copy)   ----------------------------------
CbmDigi::CbmDigi(const CbmDigi& rhs) 
  : TObject(rhs),
    fMatch(NULL)
{
	if ( rhs.fMatch ) {
		fMatch = new CbmMatch();
		fMatch->AddLinks( *(rhs.fMatch) );
	}
} 
// -------------------------------------------------------------------------



// -----   Assignment operator (deep copy)  --------------------------------
CbmDigi& CbmDigi::operator=(const CbmDigi& rhs) 
{
  if (this != &rhs) {
    TObject::operator=(rhs);
    if ( rhs.fMatch ) {
  		fMatch = new CbmMatch();
  		fMatch->AddLinks( *(rhs.fMatch) );
    }
    else fMatch = NULL;
  }
  return *this;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmDigi::~CbmDigi() {
	if ( fMatch) delete fMatch;
}
// -------------------------------------------------------------------------




ClassImp(CbmDigi)
