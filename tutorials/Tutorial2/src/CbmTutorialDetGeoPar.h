#ifndef CBMTUTORIALDETGEOPAR_H
#define CBMTUTORIALDETGEOPAR_H

#include "FairParGenericSet.h"

class TObjArray;
class FairParamList;

class CbmTutorialDetGeoPar       : public FairParGenericSet {
public:
 
  /** List of FairGeoNodes for sensitive  volumes */
  TObjArray      *fGeoSensNodes; 

  /** List of FairGeoNodes for sensitive  volumes */
  TObjArray      *fGeoPassNodes; 

  CbmTutorialDetGeoPar(const char* name="CbmTutorialDetGeoPar",
		       const char* title="CbmTutorialDet Geometry Parameters",
		       const char* context="TestDefaultContext");
  ~CbmTutorialDetGeoPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
  TObjArray* GetGeoSensitiveNodes() {return fGeoSensNodes;}
  TObjArray* GetGeoPassiveNodes()   {return fGeoPassNodes;}

  ClassDef(CbmTutorialDetGeoPar,1)
};

#endif /* CBMTUTORIALDETGEOPAR_H */
