#ifndef CBMGEOTRDPAR_H
#define CBMGEOTRDPAR_H


#include "FairParGenericSet.h"

class TObjArray;
class FairParamList;

class CbmGeoTrdPar : public FairParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

  CbmGeoTrdPar(const char* name="CbmGeoTrdPar",
             const char* title="Trd Geometry Parameters",
             const char* context="TestDefaultContext");
  ~CbmGeoTrdPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

 private:

  CbmGeoTrdPar(const CbmGeoTrdPar&);
  CbmGeoTrdPar& operator=(const CbmGeoTrdPar&);

  ClassDef(CbmGeoTrdPar,1)
};

#endif /* !CBMGEOTRDPAR_H */
