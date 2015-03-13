/* $Id: CbmGeoEcalPar.h,v 1.4 2006/07/30 13:10:35 prokudin Exp $ */

/** CbmGeoEcalPar.h
 *@author Mikhail Prokudin
 **
 ** Parameter container with extended functionality
 **/

/*
 * $Log: &
 */
#ifndef CBMGEOECALPAR_H
#define CBMGEOECALPAR_H

#include "FairParGenericSet.h"

#include "TMap.h" 

class CbmGeoEcalPar : public FairParGenericSet {
public:
  virtual Bool_t Init() {return kTRUE;};
  CbmGeoEcalPar(const char* name="CbmGeoEcalPar",
             const char* title="Ecal Geometry Parameters",
             const char* context="TestDefaultContext");
  ~CbmGeoEcalPar(void);
  void clear(void);
  void putParams(FairParamList*);
  void dump();
  Bool_t getParams(FairParamList*);

  TObjArray* GetEcalStr() const;
  TMap* GetVariables() const;

  void SetEcalStr(TObjArray* EcalStr);
  void SetVariables(TMap* Variables);

  /** key must be lower case. For example, if have in
   ** geo file AaaA=90, then you should call 
   ** GetVariableStrict("aaaa").
   ** If variable not found, will return -1111 **/
  Double_t GetVariable(const char* key);
  /** key must be lower case. For example, if have in
   ** geo file AaaA=90, then you should call 
   ** GetVariableStrict("aaaa").
   ** If variable not found, will generate Fatal **/
  Double_t GetVariableStrict(const char* key);
private:
  /** Array of string, presents ECAL structure **/
  TObjArray* fEcalStr;
  /** Array of ECAL variables **/
  TMap* fVariables;

  CbmGeoEcalPar(const CbmGeoEcalPar&);
  CbmGeoEcalPar& operator=(const CbmGeoEcalPar&);

  ClassDef(CbmGeoEcalPar,1)
};

inline TObjArray* CbmGeoEcalPar::GetEcalStr() const
{
  return fEcalStr;
}

inline TMap* CbmGeoEcalPar::GetVariables() const
{
  return fVariables;
}

inline void CbmGeoEcalPar::SetEcalStr(TObjArray* EcalStr)
{
  fEcalStr=EcalStr;
}

inline void CbmGeoEcalPar::SetVariables(TMap* Variables)
{
  fVariables=Variables;
}

#endif /* !CBMGEOECALPAR_H */
