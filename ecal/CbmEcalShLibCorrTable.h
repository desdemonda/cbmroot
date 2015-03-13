#ifndef CBMECALSHLIBCORRTABLE_H
#define CBMECALSHLIBCORRTABLE_H

#include "CbmEcalShLibCorr.h"

#include "FairTask.h"

#include <list>

class CbmEcalShLibCorrRecord;

class CbmEcalShLibCorrTable : public CbmEcalShLibCorr
{
public:
  CbmEcalShLibCorrTable(const char* name, Int_t verb);
  void AddFile(const char* name) {fFiles.push_back(name);}
  CbmEcalShLibCorrRecord* GetRec(Int_t i) const;

  // Response to photon with energy e, polar angle phi (rad), impact angle
  // theta (rad). X, Y are position vs. photons impact point
  // Cell is a size os cell in cm.
  virtual Float_t GetResponse(Float_t x, Float_t y, Float_t phi, Float_t theta, Float_t e, Int_t celltype=0);
  virtual Float_t Correlation(Float_t x1, Float_t y1, Float_t x2, Float_t y2, Float_t phi, Float_t theta, Float_t e, Int_t celltype=0);
  virtual Float_t Correlation2(Float_t x1, Float_t y1, Float_t x2, Float_t y2, Float_t phi, Float_t theta, Float_t e, Int_t celltype=0);
  virtual InitStatus Init();
  void Exec(Option_t* exec) {};
  void Finish() {};
  virtual ~CbmEcalShLibCorrTable();
private:
  void Add(const char* name);

  std::list<TString> fFiles;
  CbmEcalShLibCorrRecord* fRec[10];

  CbmEcalShLibCorrTable(const CbmEcalShLibCorrTable&);
  CbmEcalShLibCorrTable& operator=(const CbmEcalShLibCorrTable&);

  ClassDef(CbmEcalShLibCorrTable, 1);
};

inline CbmEcalShLibCorrRecord* CbmEcalShLibCorrTable::GetRec(Int_t i) const
{
  if (i<0||i>9) return NULL;
  return fRec[i];
}

#endif
