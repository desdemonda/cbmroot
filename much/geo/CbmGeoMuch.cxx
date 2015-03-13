/** CbmGeoMuch class
 *
 * @author  A.Kiseleva
 * @version 0.0
 * @since   13.04.06
 * 
 *  Class for geometry of MUon CHambers
 *
 */

#include "CbmGeoMuch.h"
#include "FairGeoNode.h"

CbmGeoMuch::CbmGeoMuch() {
  // Constructor
  fName="much";
  maxSectors=0;
  maxModules=1;//99;
}

const char* CbmGeoMuch::getModuleName(Int_t m) {
  // Returns the module name of much number m
//   if ( m < 0 ) {
//   	cout <<"-E- CbmGeoMuch::getModuleName:: Module number "
//   	       << m << " not known!" << endl;
// 	 return "";
// 	 }
//   if ( m < 9 ) sprintf(modName,"muchstation0%i",m+1);
//   else  sprintf(modName,"muchstation%i",m+1);

  sprintf(eleName,"much%i",m+1);
//  cout << "DEBUG: modName(" << m << ")="<< modName << endl;
  //  CbmGeoMuchPar* fGeoPar = (CbmGeoMuchPar*) db->getContainer("CbmGeoMuchPar");
  
  return modName;
}

const char* CbmGeoMuch::getEleName(Int_t m) {
  // Returns the element name of sts number m
  sprintf(eleName,"much%i",m+1);
  return eleName;
}

ClassImp(CbmGeoMuch)

