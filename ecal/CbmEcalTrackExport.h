//We are going t hack the architecture to be much faster
//Idea: we store tracks before the calorimeter and 
//energy deposition in calorimeter. Mix all stuff with 
//photon with known parameters and try to reconstruct it.
#ifndef CBMECALTRACKEXPORT_H
#define CBMECALTRACKEXPORT_H

#include "FairTask.h"

class TTree;
class TClonesArray;

class CbmEcalTrackExport : public FairTask
{
public:
  /** Default constructor **/
  CbmEcalTrackExport();

  /** Standerd constructor **/
  CbmEcalTrackExport(const char* name, const Int_t verbose);

  /** Task initialization **/
  virtual InitStatus Init();

  /** Exec for task **/
  virtual void Exec(Option_t* opt);
  
  /** Finish task **/
  virtual void Finish();

  /** virtual destructor **/
  ~CbmEcalTrackExport();
private:
  Int_t fVerbose;
 
  Int_t fEventN;
  Float_t fX;
  Float_t fY;
  Float_t fZ;
  Float_t fTx;
  Float_t fTy;
  Float_t fQp;

  TTree* fTracksOut;			//!

  TClonesArray* fTrackPar;

  CbmEcalTrackExport(const CbmEcalTrackExport&);
  CbmEcalTrackExport& operator=(const CbmEcalTrackExport&);

  ClassDef(CbmEcalTrackExport, 1)
};

#endif
    
