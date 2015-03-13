#ifndef CBM_STS_POLY_FITTER_H
#define CBM_STS_POLY_FITTER_H

#include "FairTask.h"
#include "CbmStsJointTrack.h"
#include "FairField.h"
#include "TRefArray.h"

//#include "fstream.h"

class CbmStsPolyFitter : public FairTask {

protected:

  Int_t fNHit;              //  number of hits in the track
  Int_t qTrack;             //  = 0 -> fit into vertex
                            //  = 1 -> fit into 1-st hit
                            //  =<0 -> not fitted

  FairField *fField;   //  

  TClonesArray *fHits;      //  array of hits on the track
  TClonesArray *fTracks;    //
  TClonesArray *fOutputTracks;    //!
  TRefArray *TrHits;
  TString fOption;          //  options to choose branches

  //  TList *histlist;

//---- Geometry
  Int_t iRegFit; // regime of fit
  Double_t zPlane[7];  
  Double_t sigX; // res. in X (mkm)
  Double_t sigY; // res. in Y (mkm)

//--------- Only for test

  Int_t numOfTrackPro;  // Num of tracks to be processed for each event

  Double_t Pin;
  Double_t BETAin;
  Double_t TGAin;

  Int_t fNFitPoint;

//---- Parabolica (0-step) -----

  Int_t numOfTrack;
  Int_t pID;
  Int_t iHit[10];
  Double_t Xhit[10];
  Double_t Yhit[10];
  Double_t Zhit[10];
  Double_t Hx[10];
  Double_t Hy[10];
  Double_t Hz[10];

  Double_t Rparab;
  Double_t sinPhi1;
  Double_t cosPhi1;
  Double_t FI0;
  Double_t fP0;
  Double_t fBETA0;
  Double_t fTGA0;

  Double_t chi2;   // chi^2 in X0Z plane
  Double_t w2;     // chi^2 in Y0Z plane

//---- nonuHandIonLoss (1-step) ----
  Double_t fP1;
  Double_t fP1f[4];     // Momentum: [0] e+/-, [1] pi+/-, [2] K+/-, [3] p  
  Double_t fBETA1;      // Azimutal angle of the track
  Double_t fTGA1;       // tg of deep angle of the track
  Double_t tgBeta1;
  Double_t Sp[10];      // segment lengths
  Double_t StSi;
  Double_t Strack;      // Length of track

//----- Data for end point --

  Double_t P1fN[4];
  Double_t P1fNx[4];
  Double_t P1fNy[4];
  Double_t P1fNz[4];
  Double_t xN;
  Double_t yN;
  Double_t zN;
  Double_t fBETAN;
  Double_t fTGAN;
//--------------------------

public:

  virtual void Exec(Option_t * option);
  virtual InitStatus Init();                         // *MENU*
  virtual void Finish();                       // *MENU*

  void  Register();                 //

  CbmStsPolyFitter(const char *name, const char *title="Cbm Task");
  CbmStsPolyFitter();
  virtual ~CbmStsPolyFitter();

  Bool_t Parabolica();
  void nonuniHandEloss();
  void getHtrack(Double_t *H0, Double_t *dHdS, Double_t *d2HdS2);
  Double_t dPdS(Double_t P, Int_t IP, Int_t IM);
  Double_t d2PdS2(Double_t P, Int_t IP, Int_t IM);
  void SetOption(Option_t *option=" ") {fOption = option;  fOption.ToLower();}
  void parEndPoint();
  void Output(CbmStsJointTrack* track );
  void OutputForEval(CbmStsJointTrack* track );

ClassDef(CbmStsPolyFitter,1) // CbmStsPolyFitter

};

#endif //CBM_STS_POLY_FITTER_H
