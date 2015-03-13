// --------------------------------------------------------------------------
// -----             Header for the CbmTofHitProducer                  ------
// -----           Created by E. Cordier 14/09/05                      ------
// -----           Modified by D. Gonzalez-Diaz 07/09/06               ------
// -----           Modified by D. Gonzalez-Diaz 02/02/07               ------
// -----           Modified by nh 23/10/12                             ------
// --------------------------------------------------------------------------

#ifndef CBMTOFHITPRODUCERNEW_H
#define CBMTOFHITPRODUCERNEW_H

#include "FairTask.h"

class CbmTofGeoHandler;
class CbmTofDigiPar;
class CbmTofCell;

class TVector3;
class TClonesArray;
class TString;                               


class CbmTofHitProducerNew : public FairTask {

 public:

  CbmTofHitProducerNew();

  CbmTofHitProducerNew(const char *name, Int_t verbose = 1);

  virtual ~CbmTofHitProducerNew();

  virtual InitStatus Init();
  virtual InitStatus ReInit();
  virtual void SetParContainers();
  virtual void Exec(Option_t * option);
  virtual void Finish();

  void AddHit(Int_t detID, TVector3 &posHit, TVector3 &posHitErr,
	      Int_t ref, Double_t tHit, Int_t flag, Int_t iChannel);

  void SetSigmaT(Double_t sigma);
  void SetSigmaEl(Double_t sigma);
  void SetSigmaXY(Double_t sigma);
  void SetSigmaY(Double_t sigma);
  void SetSigmaZ(Double_t sigma);
  void SetParFileName(const std::string& fnam) { fParFileName = fnam;}
  void SetInitFromAscii(Bool_t ascii) {fParInitFromAscii=ascii;}

  Double_t GetSigmaT();
  Double_t GetSigmaEl();
  Double_t GetSigmaXY();
  Double_t GetSigmaY();
  Double_t GetSigmaZ();

private:

  Int_t fVerbose;

  TString fParFileName;   // name of the file name with geometry parameters
  void InitParametersFromContainer();

  TClonesArray *fTofPoints;     // TOF MC points
  TClonesArray *fMCTracks;      // MC tracks
  TClonesArray *fHitCollection; // TOFhits

  //Temporary substitute of the parameter container. FIXME 

  static const Int_t maxSMtyp=6   ;
  static const Int_t maxnSM=255   ;
  static const Int_t maxnMod=10   ;
  static const Int_t maxnCell=255 ;
  
  Float_t X [maxSMtyp][maxnSM][maxnMod][maxnCell];  //X-Position in cm */
  Float_t Dx[maxSMtyp][maxnSM][maxnMod][maxnCell];  //Resolution in position in cm */
  Float_t Y [maxSMtyp][maxnSM][maxnMod][maxnCell];  //Y-Position in cm */
  Float_t Dy[maxSMtyp][maxnSM][maxnMod][maxnCell];  //Resolution in position in cm */
  Float_t Z [maxSMtyp][maxnSM][maxnMod][maxnCell];  //Z-Position in cm */
  Int_t   Ch[maxSMtyp][maxnSM][maxnMod][maxnCell];  //Channel number */
  
  Int_t ActSMtypMax;
  Int_t ActnSMMax[maxSMtyp];
  Int_t ActnModMax[maxSMtyp];
  Int_t ActnCellMax[maxSMtyp];

  //End of temporary data members

  //Members to store the hit during loop over all the points. FIXME

  Float_t tl[maxSMtyp][maxnSM][maxnMod][maxnCell];
  Float_t tr[maxSMtyp][maxnSM][maxnMod][maxnCell];
  Int_t trackID_left[maxSMtyp][maxnSM][maxnMod][maxnCell];
  Int_t trackID_right[maxSMtyp][maxnSM][maxnMod][maxnCell]; 
  Int_t point_left [maxSMtyp][maxnSM][maxnMod][maxnCell];
  Int_t point_right[maxSMtyp][maxnSM][maxnMod][maxnCell]; 

  //End of storing data members

  Double_t fSigmaT;       //Resolution in time in ns 
  Double_t fSigmaEl;      //Electronics time   in ns 
  Double_t fSigmaXY;      //Resolution in x/y  in cm
  Double_t fSigmaY;       //Resolution in y    in cm
  Double_t fSigmaZ;       //Resolution in z    in cm
  
  TString fVersion;       //
  Int_t  fNHits;          //Index of the CbmTofHit TClonesArray

  CbmTofGeoHandler* fGeoHandler;
  CbmTofDigiPar  *fDigiPar;
  CbmTofCell     *fCellInfo;

  Bool_t fParInitFromAscii;

  CbmTofHitProducerNew(const CbmTofHitProducerNew&);
  CbmTofHitProducerNew& operator=(const CbmTofHitProducerNew&);
  
  ClassDef(CbmTofHitProducerNew,1) //CbmTofHitProducer

};
#endif 

