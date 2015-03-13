#ifndef CBMGEOPASSIVEPAR_H
#define CBMGEOPASSIVEPAR_H

#include "FairParGenericSet.h"

class CbmGeoPassivePar : public FairParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  CbmGeoPassivePar(const char* name="CbmGeoPassivePar",
             const char* title="Passive Geometry Parameters",
             const char* context="TestDefaultContext");
  ~CbmGeoPassivePar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

 private:

  CbmGeoPassivePar(const CbmGeoPassivePar&);
  CbmGeoPassivePar& operator=(const CbmGeoPassivePar&);

  ClassDef(CbmGeoPassivePar,1)
};

#endif /* !CBMGEOPASSIVEPAR_H */
