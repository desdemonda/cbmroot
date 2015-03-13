#ifndef CBMECALSHLIBNN_H
#define CBMECALSHLIBNN_H

#include "CbmEcalShLib.h"

#include "FairTask.h"

#include <list>

class TTree;
class TMultiLayerPerceptron;

class CbmEcalShLibNN : public CbmEcalShLib
{
public:
  CbmEcalShLibNN(const char* name, Int_t verb);
  void AddRecord(const char* name, const char* fname, Int_t celltype);
  TMultiLayerPerceptron* GetRec(Int_t i) const;

  // Response to photon with energy e, polar angle phi (rad), impact angle
  // theta (rad). X, Y are position vs. photons impact point
  // Cell is a size os cell in cm.
  virtual Float_t GetResponse(Float_t x, Float_t y, Float_t phi, Float_t theta, Float_t e, Int_t celltype=0);
  virtual InitStatus Init();
  void Exec(Option_t* exec) {};
  void Finish() {};
  virtual ~CbmEcalShLibNN();
private:
  TMultiLayerPerceptron* fRec[10];	//!
  TTree* fTr[10];			//!

  CbmEcalShLibNN(const CbmEcalShLibNN&);
  CbmEcalShLibNN& operator=(const CbmEcalShLibNN&);

  ClassDef(CbmEcalShLibNN, 1);
};

inline TMultiLayerPerceptron* CbmEcalShLibNN::GetRec(Int_t i) const
{
  if (i<0||i>9) return NULL;
  return fRec[i];
}

#endif
