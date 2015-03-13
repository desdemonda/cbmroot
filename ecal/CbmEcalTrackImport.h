/** A second part of hack for fast tracking **/

#ifndef CBMECALTRACKIMPORT_H
#define CBMECALTRACKIMPORT_H

#include "FairTask.h"

class TChain;
class TClonesArray;

class CbmEcalTrackImport : public FairTask
{
public:
  /** Only to comply with framework **/
  CbmEcalTrackImport();
  /** Standard constructor **/
  CbmEcalTrackImport(const char* name, const Int_t verbose);

  /** Task initialization **/
  virtual InitStatus Init();

  /** Exec for task **/
  virtual void Exec(Option_t* opt);
  
  /** Finish task **/
  virtual void Finish();

  /** virtual destructor **/
  ~CbmEcalTrackImport();
  
  /** add a root file with tracks to chain **/
  void AddFile(const char* name);
private:
  /** A chain with tracks **/
  TChain* fChain;		//!
  /** An event number in file **/
  Int_t fEventN;
  /** X coordinate of track **/
  Float_t fX;
  /** Y coordinate of track **/
  Float_t fY;
  /** Z coordinate of track **/
  Float_t fZ;
  /** Tx (ask tracking people for name) **/
  Float_t fTx;
  /** Ty (ask tracking people for name) **/
  Float_t fTy;
  /** Qp (ask tracking people for name) **/
  Float_t fQp;
  /** A number of records in file **/
  Int_t fN;
  /** A current entry in chain **/
  Int_t fEntry;

  /** A current event **/
  Int_t fEv;
  /** An output array of tracks **/
  TClonesArray* fTracks;	//!

  CbmEcalTrackImport(const CbmEcalTrackImport&);
  CbmEcalTrackImport& operator=(const CbmEcalTrackImport&);

  ClassDef(CbmEcalTrackImport, 1)
};

#endif
