// -------------------------------------------------------------------------
// -----                         CbmTrdRadiator header file       -----
// -----                  Created 10/11/04  by M.Kalisky         -----
// -------------------------------------------------------------------------

/**  CbmTrdRadiator.h
 *@author M.Kalisky <m.kalisky@gsi.de>
 **
 ** Serves for Transition Radiation computation for regular radiator
 ** The output is energy loss of an electron via Transition Radiation
 **/

#ifndef CBMTRDRADIATOR_H
#define CBMTRDRADIATOR_H

#include "TVector3.h"

class TH1D;
class CbmTrdPoint;

class CbmTrdRadiator 
{
  
 public:
  
  /** Default constructor **/
  CbmTrdRadiator();

  /** Constructor **/
  CbmTrdRadiator(Bool_t SimpleTR, Int_t Nfoils, Float_t FoilThick, 
                 Float_t GapThick);

  /** Constructor **/
  CbmTrdRadiator(Bool_t SimpleTR, Int_t Nfoils, Float_t FoilThick, 
                 Float_t GapThick, TString material, TString prototype);

  /** Constructor 20.02.2013 to include external files with tr-spectra**/
  CbmTrdRadiator(Bool_t SimpleTR, TString prototype);
  /* // implemented prototypes are:
     "A"		ALICE like 7xfiber layer + 2x8mm Rohacell layer
     "Bshort"	        POKALON 24µm, 0.7mm gap,  100 foils
     "B"		POKALON 24µm, 0.7mm gap,  250 foils
     "B++"		POKALON 24µm, 0.7mm gap,  350 foils
     "C"		PE	15µm, 0.7mm gap,  200 foils
     "D"		PE	15µm, 0.5mm gap,  100 foils
     "E"		PE	20µm, 0.5mm gap,  120 foils
     "F"		PE	20µm, 0.25mm gap, 220 foils
     "G30"		30xfiber layer
     "H"		125x2mm PE foam foil layer
     "H++"		177x2mm PE foam foil layer
     "Kshort"	        POKALON 24µm, 0.7mm gap,  100 foils micro-structured
     "K"		POKALON 24µm, 0.7mm gap,  250 foils micro-structured
     "K++"		POKALON 24µm, 0.7mm gap,  350 foils micro-structured
  */

  /** Destructor **/
  virtual ~CbmTrdRadiator();
  
  /** Create the needed histograms **/
  void CreateHistograms();

  /** Init function **/
  void Init(Bool_t SimpleTR, Int_t Nfoils, Float_t FoilThick, Float_t GapTick, TString material);
  /** Init function **/
  void  Init(Bool_t SimpleTR, Int_t Nfoils, Float_t FoilThick, Float_t GapThick);
  /** Init function **/
  void Init();

//  void SetRadPrototype(TString prototype);

  /** Spectra production **/
  void ProduceSpectra();

  /** Final energy loss computation **/
  Int_t  ELoss(Int_t index);

  /** Main function for TR production **/
  void ProcessTR();

  /** Compute the TR spectrum **/
  Int_t TRspectrum();

  /** Compute the TR spectrum in the Window foil **/
  Int_t WinTRspectrum();

  /** Compute the TR spectrum in the Detector **/
  Int_t DetTRspectrum();

  /** Compute the gamma factor **/
  Float_t GammaF();

  /** Compute the sigma coeff. for one-gap-one-foil radiator **/
  Float_t Sigma(Float_t energykeV);

  /** Compute the sigma coeff. for the gas-.window foil **/
  Float_t SigmaWin(Float_t energykeV);

  /** Compute the sigma coeff. for the detector gas **/
  Float_t SigmaDet(Float_t energykeV);
  TString fWindowFoil;
  /** Computation of photon absorption cross sections taken from http://physics.nist.gov/PhysRefData/Xcom/html/xcom1.html**/
  Float_t      GetMuAl(Float_t energyMeV);
  Float_t      GetMuPo(Float_t energyMeV);
  Float_t      GetMuPok(Float_t energyMeV);
  Float_t      GetMuKa(Float_t energyMeV);
  Float_t      GetMuAir(Float_t energyMeV);
  Float_t      GetMuXe(Float_t energyMeV);
  Float_t      GetMuCO2(Float_t energyMeV);
  Float_t      GetMuMy(Float_t energyMeV);

  /** Calculate the TR for a given momentum **/
  Float_t      GetTR(TVector3 mom);

  /** Interpolate between given points of table **/
  Float_t Interpolate(Float_t energyMeV
		      , Float_t *en, Float_t *mu, Int_t n);

  /** Locate a point in 1-dim grid **/
  Int_t Locate(Float_t *xv, Int_t n, Float_t xval
               , Int_t &kl, Float_t &dx);


  /** Setters for private data memebers **/
  void          SetNFoils(Int_t n)      { fNFoils    = n; }
  void          SetFoilThick(Float_t t) { fFoilThick = t; }
  void          SetGapThick(Float_t t)  { fGapThick  = t; }
  void          SetSigma(Int_t SigmaT);

  Bool_t LatticeHit(const CbmTrdPoint* point);
 
 private:

  CbmTrdRadiator& operator=(const CbmTrdRadiator&);
  CbmTrdRadiator(const CbmTrdRadiator&);

  /* Input parameters to be set */
  TString   fRadType;
  Int_t     fDetType;     // 0: GSI  1: Muenster-Bucarest
  Bool_t    fFirstPass;   // used for MB-Chamber with two gas gaps
  Bool_t    fSimpleTR;    // mode of the TR production
  Int_t     fNFoils;      // Number of foils in the radiator stack
  Float_t   fFoilThick;   // Thickness of the foils (cm)
  Float_t   fGapThick;    // Thickness of gaps between the foils (cm)
  TString   fFoilMaterial;
  Float_t   fGasThick;    // Thickness of hte active gas volume.


  /* Parameters fixed in Init() */

  Float_t   fFoilDens;    // Density of the radiator foils (g/cm^3) 
  Float_t   fGapDens;     // Dens. of gas in the radiator gaps (g/cm^3)
  Float_t   fFoilOmega;   // Plasma frequency of the radiator foils
  Float_t   fGapOmega;    // Plasma freq. of gas
  Float_t fnPhotonCorr;   // Correction of number of photon production efficiency

  /* Parameters after correction of the angle of the particle */

  Float_t fFoilThickCorr;
  Float_t fGapThickCorr;
  Float_t fGasThickCorr;



  Float_t   fnTRprod;     // <nTR> produced
  //  Int_t     fSigmaT;      // set the absorption material

  Float_t   fWinDens;
  Float_t   fWinThick;
  
  Float_t   fCom1;        // first component of the gas
  Float_t   fCom2;        // second component of the gas

  static const Int_t     fSpNBins=100; // Number of Bins for histos 
  static const Int_t     fSpRange=50;  // Maximum (keV) of eloss spectrum 
  Float_t   fSpBinWidth;               // Bin width=fSpNBins/fSpRange

  Float_t*  fSigma;       // [fSpNBins] Array of sigma values for the foil of the radiator
  Float_t*  fSigmaWin;     // [fSpNBins] Array of sigma values for the entrance window of detector
  Float_t*  fSigmaDet;    // [fSpNBins] Array of sigma values for the active gas

  TH1D*     fSpectrum;    //! TR photon energy spectrum
  TH1D*     fWinSpectrum;  //! TR spectra in gas-window foil
  TH1D*     fDetSpectrumA;//! TR absorbed in Detector
  TH1D*     fDetSpectrum; //! TR passed through Detector

  static const Int_t fNMom=14; // number of momentum spectra  
  Double_t *fTrackMomentum;    // [fNMom] Track momenta for which spectra
                               // are available

  TH1D*     fFinal[fNMom];     //! Absorption spectra for different momenta
  Float_t   fnTRabs[fNMom];    // <nTR> absorbed for differnt momenta
  Float_t   fnTRab;            // <nTR> absorbed

  Float_t   fELoss;       // The real result - in GeV

  TVector3  fMom;         // momentum of the electron

  ClassDef(CbmTrdRadiator,1)
    };

#endif

