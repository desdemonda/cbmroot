//*--Author: Denis Bertini
//   modified 21/06/2005 

#include "CbmGeoEcalPar.h"

#include "FairParamList.h"

#include "TObjArray.h"
#include "TObjString.h"

#include <iostream>
#include <stdlib.h>

using std::cout;
using std::cerr;
using std::endl;

ClassImp(CbmGeoEcalPar)

/** key must be lower case. For example, if have in
 ** geo file AaaA=90, then you should call 
 ** GetVariableStrict("aaaa").
 ** If variable not found, will return -1111 **/
Double_t CbmGeoEcalPar::GetVariable(const char* key)
{
  if (fVariables==NULL) return -1111;
  TObjString* value=(TObjString*)fVariables->GetValue(key);
  if (value==NULL)
    return -1111;
  Double_t val;
  char* err=NULL;
  val=strtod(value->GetString(),&err);
  if (err[0]!='\0')
    return -1111;
  return val;
}
/** key must be lower case. For example, if have in
 ** geo file AaaA=90, then you should call 
 ** GetVariableStrict("aaaa").
 ** If variable not found, will generate Fatal **/
Double_t CbmGeoEcalPar::GetVariableStrict(const char* key)
{
  TObjString* value=(TObjString*)fVariables->GetValue(key);
  if (value==NULL)
  {
    cerr << "Can't find variable named \"" << key << "\"";
    cerr << ". Exiting..." << endl;
    Fatal("","");
  }
  Double_t val;
  char* err=NULL;
  val=strtod(value->GetString(),&err);
  if (err[0]!='\0')
  {
    cerr << "Can't convert variable named \"" << key ;
    cerr << "\" to floating point. Value is \"";
    cerr << value->GetString() << "\"." << endl;
    cerr << "Exiting..." << endl;
    Fatal("","");
  }
  return val;
}

CbmGeoEcalPar::CbmGeoEcalPar(const char* name,const char* title,const char* context)
  : FairParGenericSet(name,title,context), 
    fEcalStr(NULL),
    fVariables(NULL)
{
}

CbmGeoEcalPar::~CbmGeoEcalPar(void) {
}

void CbmGeoEcalPar::dump()
{
  if (fVariables)
  {
    TObjString* key;
    TIterator* iter=fVariables->MakeIterator();
    while((key=(TObjString*)iter->Next())!=NULL)
    {
      TObjString* str=(TObjString*)fVariables->GetValue(key);
      cout << key->String() << "=" << str->String() << endl;
    }
  }
  if (fEcalStr)
  {
    TObjString* key;
    TIterator* iter=fEcalStr->MakeIterator();
    while((key=(TObjString*)iter->Next())!=NULL)
    {
      cout << key->String() << endl;
    }
  }

}

void CbmGeoEcalPar::clear(void)
{
  if (fEcalStr) delete fEcalStr;
  if (fVariables) delete fVariables;
}

void CbmGeoEcalPar::putParams(FairParamList* l) {
  if (!l) return;
  if (fEcalStr)
   l->addObject("ECAL Structure", fEcalStr);
   l->addObject("ECAL Variables", fVariables);
}

Bool_t CbmGeoEcalPar::getParams(FairParamList* l) {
    if (!l) return kFALSE;
    if (!l->fillObject("ECAL Structure", fEcalStr))
      fEcalStr=NULL;
    if (!l->fillObject("ECAL Variables", fVariables))
      return kFALSE;

  return kTRUE;
}
