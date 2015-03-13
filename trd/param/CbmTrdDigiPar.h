// -------------------------------------------------------------------------
// -----                    CbmTrdDigiPar header file                  -----
// -----                 Created 15/05/08  by F. Uhlig                 -----
// -------------------------------------------------------------------------

#ifndef CBMTRDDIGIPAR_H
#define CBMTRDDIGIPAR_H

#include "FairParGenericSet.h"

//#include "TObject.h"
//#include "TObjArray.h"
//#include "TArrayF.h"
#include "TArrayI.h"

#include <map>

class CbmTrdModule;
class FairParIo;
class FairParamList;


class CbmTrdDigiPar : public FairParGenericSet 
{

 public:

  /** Standard constructor **/
  CbmTrdDigiPar(const char* name    = "CbmTrdDigiPar",
		const char* title   = "TRD digi parameters",
		const char* context = "Default");


  /** Destructor **/
  virtual ~CbmTrdDigiPar();
  
  /** Reset all parameters **/
  virtual void clear();

  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);

  void SetNrOfModules(Int_t i) { fNrOfModules = i; }
  void SetMaxSectors(Int_t i) { fMaxSectors = i; }
  void SetModuleIdArray(TArrayI array) { fModuleIdArray = array; }
  void SetModuleMap(std::map<Int_t, CbmTrdModule*> map) 
                    { fModuleMap = map; }

  Int_t GetNrOfModules() { return fNrOfModules; }
  Int_t GetMaxSectors() { return fMaxSectors; }
  Int_t GetModuleId(Int_t i) { return fModuleIdArray[i]; }

  CbmTrdModule* GetModule(Int_t i) { return fModuleMap[i]; }

 private:

  /** Map of Unique Trd Module Id to corresponding TrdModule **/
  std::map<Int_t, CbmTrdModule*> fModuleMap;                  //!   
  std::map<Int_t, CbmTrdModule*>::iterator fModuleMapIt;      //! 

  TArrayI fModuleIdArray; // Array to hold the unique IDs for all modules
  Int_t fNrOfModules; // Total number of modules
  Int_t fMaxSectors; // Maximum number of sectors

  ClassDef(CbmTrdDigiPar,3);
};
#endif
