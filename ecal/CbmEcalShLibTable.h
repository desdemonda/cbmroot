#ifndef CBMECALSHLIBTABLE_H
#define CBMECALSHLIBTABLE_H

#include "CbmEcalShLib.h"

#include "FairTask.h"

#include <list>

class CbmEcalShLibRecord;

class CbmEcalShLibTable : public CbmEcalShLib
{
public:
  CbmEcalShLibTable(const char* name, Int_t verb);
  void AddFile(const char* name) {fFiles.push_back(name);}
  CbmEcalShLibRecord* GetRec(Int_t i) const;

  // Response to photon with energy e, polar angle phi (rad), impact angle
  // theta (rad). X, Y are position vs. photons impact point
  // Cell is a size os cell in cm.
  virtual Float_t GetResponse(Float_t x, Float_t y, Float_t phi, Float_t theta, Float_t e, Int_t celltype=0);
  virtual InitStatus Init();
  void Exec(Option_t* exec) {};
  void Finish() {};
  virtual ~CbmEcalShLibTable();
private:
  void Add(const char* name);

  std::list<TString> fFiles;
  CbmEcalShLibRecord* fRec[10];

  CbmEcalShLibTable(const CbmEcalShLibTable&);
  CbmEcalShLibTable& operator=(const CbmEcalShLibTable&);

  ClassDef(CbmEcalShLibTable, 1);
};

inline CbmEcalShLibRecord* CbmEcalShLibTable::GetRec(Int_t i) const
{
  if (i<0||i>9) return NULL;
  return fRec[i];
}

#endif
