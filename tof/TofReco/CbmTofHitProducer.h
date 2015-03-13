// --------------------------------------------------------------------------
// -----             Header for the CbmTofHitProducer                  ------
// -----           Created by E. Cordier 14/09/05                      ------
// -----           Modified by D. Gonzalez-Diaz 07/09/06               ------
// -----           Modified by D. Gonzalez-Diaz 02/02/07               ------
// --------------------------------------------------------------------------



#ifndef CBMTOFHITPRODUCER_H
#define CBMTOFHITPRODUCER_H

#include "FairTask.h"

class CbmTofGeoHandler;
class CbmTofDigiPar;
class CbmTofCell;

class TVector3;
class TClonesArray;
class TString;                               


class CbmTofHitProducer : public FairTask {

 public:

  CbmTofHitProducer();

  CbmTofHitProducer(const char *name, Int_t verbose = 1);

  virtual ~CbmTofHitProducer();

  virtual InitStatus Init();
  virtual InitStatus ReInit();
  virtual void SetParContainers();
  virtual void Exec(Option_t * option);
  virtual void Finish();

  void AddHit(Int_t detID, TVector3 &posHit, TVector3 &posHitErr,
	      Int_t ref, Double_t tHit, Int_t flag);

  void SetSigmaT(Double_t sigma);
//  void SetSigmaEl(Double_t sigma);
  void SetSigmaXY(Double_t sigma);
  void SetSigmaY(Double_t sigma);
  void SetSigmaZ(Double_t sigma);
  void SetInitFromAscii(Bool_t ascii) {fParInitFromAscii=ascii;}

  Double_t GetSigmaT();
//  Double_t GetSigmaEl();
  Double_t GetSigmaXY();
  Double_t GetSigmaY();
  Double_t GetSigmaZ();

private:

  void ReadTofZPosition();

  void InitParametersFromAscii();
  void InitParametersFromContainer();

  Int_t fVerbose;

  TClonesArray *fTofPoints;     // TOF MC points
  TClonesArray *fMCTracks;      // MC tracks
  TClonesArray *fTofHits; // TOFhits

  //Temporary substitute of the parameter container. FIXME 
 
  Float_t X [10][500][500];  //X-Position in cm */
  Float_t Dx[10][500][500];  //Resolution in position in cm */
  Float_t Y [10][500][500];  //Y-Position in cm */
  Float_t Dy[10][500][500];  //Resolution in position in cm */
  Float_t Z [10][500][500];  //Z-Position in cm */

  TString type[10][500][500]; //Type of cell [pad, strip]

  //End of temporary data members

  //Members to store the hit during loop over all the points. FIXME

  Float_t tl[10][500][500];
  Float_t tr[10][500][500];
  Int_t trackID_left[10][500][500];
  Int_t trackID_right[10][500][500]; 
  Int_t point_left[10][500][500];
  Int_t point_right[10][500][500]; 

  //End of storing data members

  Double_t fSigmaT;       //Resolution in time in ns 
  Double_t fSigmaEl;      //Electronics time   in ns 
  Double_t fSigmaXY;      //Resolution in x/y  in cm
  Double_t fSigmaY;       //Resolution in y    in cm
  Double_t fSigmaZ;       //Resolution in z    in cm
  
  TString fVersion;       //
  Int_t  fNHits;          //Index of the CbmTofHit TClonesArray

  Double_t fTofZPosition; // entry Z position of the TOF wall (NOT hit)

  CbmTofGeoHandler* fGeoHandler;
  CbmTofDigiPar  *fDigiPar;
  CbmTofCell   *fCellInfo;

  Bool_t fParInitFromAscii;

  CbmTofHitProducer(const CbmTofHitProducer&);
  CbmTofHitProducer& operator=(const CbmTofHitProducer&);

  ClassDef(CbmTofHitProducer,3) //CBMTOFHitProducer

};
#endif 

