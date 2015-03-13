/** CbmGeoMuch class
 *
 * @author  A.Kiseleva
 * @version 0.0
 * @since   13.04.06
 * 
 *  Class for geometry of MUon CHambers
 *
 */

#ifndef CBMGEOMUCH_H
#define CBMGEOMUCH_H

#include "FairGeoSet.h"

class  CbmGeoMuch : public FairGeoSet {
protected:
  char modName[20];  // name of module
  char eleName[20];  // substring for elements in module
public:
  CbmGeoMuch();
  ~CbmGeoMuch() {}
  const char* getModuleName(Int_t);
  const char* getEleName(Int_t);
  inline Int_t getModNumInMod(const TString&);
  ClassDef(CbmGeoMuch,0) // Class for MUID
};

inline Int_t CbmGeoMuch::getModNumInMod(const TString& name) {
  // returns the module index from module name
  return (Int_t)(name[4]-'0')-1;
}

#endif  /* !CBMGEOMUCH_H */
