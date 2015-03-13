//We are going t hack the architecture to be much faster
//Idea: we store tracks before the calorimeter and 
//energy deposition in calorimeter. Mix all stuff with 
//photon with known parameters and try to reconstruct it.
#ifndef CBMECALSTRUCTUREEXPORT_H
#define CBMECALSTRUCTUREEXPORT_H

#include "FairTask.h"

class TTree;
class CbmEcalStructure;

class CbmEcalStructureExport : public FairTask
{
public:
  /** Default constructor **/
  CbmEcalStructureExport();

  /** Standerd constructor **/
  CbmEcalStructureExport(const char* name, const Int_t verbose);

  /** Task initialization **/
  virtual InitStatus Init();

  /** Exec for task **/
  virtual void Exec(Option_t* opt);
  
  /** Finish task **/
  virtual void Finish();

  /** virtual destructor **/
  ~CbmEcalStructureExport();
private:
  Int_t fVerbose;
 
  Int_t fEventN;
  Float_t fX;
  Float_t fY;
  Float_t fE;

  TTree* fStructureOut;			//!

  CbmEcalStructure* fStr;		//!

  CbmEcalStructureExport(const CbmEcalStructureExport&);
  CbmEcalStructureExport& operator=(const CbmEcalStructureExport&);

  ClassDef(CbmEcalStructureExport, 1)
};

#endif

