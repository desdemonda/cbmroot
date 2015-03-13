// -------------------------------------------------------------------------
// -----             CbmAnaHypYPtGenerator header file                 -----
// -----              Created 03/10/04  by E. Kryshen                  -----
// -------------------------------------------------------------------------

/** CbmAnaHypYPtGenerator.h
 ** @author Evgeny Kryshen <e.kryshen@gsi.de>
 **
 ** The CbmAnaHypYPtGenerator generates particles with a given
 ** distribution over pt (dn/dpt = pt*exp(-mt/T)
 ** and rapidity (Gaussian) with fixed multiplicity per event.
 ** Derived from FairGenerator.
 */


#ifndef CBMANAHYPYPTGENERATOR_H
#define CBMANAHYPYPTGENERATOR_H

#include "FairGenerator.h"
#include "TF1.h"

using namespace std;

class FairPrimaryGenerator;

class CbmAnaHypYPtGenerator : public FairGenerator
{
public:

  /** Default constructor. */
  CbmAnaHypYPtGenerator();

  /** Constructor with PDG-ID, multiplicity
   **@param pdgid Particle type (PDG encoding)
   **@param mult  Multiplicity (default is 1)
   **/
  CbmAnaHypYPtGenerator(Int_t pdgid, Int_t mult=1);

  /** Destructor */
  virtual ~CbmAnaHypYPtGenerator() {};

  /** Modifiers */
  inline void SetPDGType        (Int_t pdg)  {fPDGType = pdg;  };
  inline void SetMultiplicity   (Int_t mult) {fMult    = mult; };
  inline void SetDistributionPt (Double_t T=0.154319) {fT=T;};
  inline void SetDistributionY  (Double_t y0=1.98604, Double_t sigma=0.617173) {fY0=y0;fSigma=sigma;};
  inline void SetRangePt        (Double_t ptMin=0, Double_t ptMax=3) {fPtMin=ptMin;fPtMax=ptMax;};

  /** Initializer */
  Bool_t Init();

  /** Creates an event with given type and multiplicity.
   **@param primGen  pointer to the CbmPrimaryGenerator
   */
  virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);

private:
  Int_t    fPDGType;             ///< Particle type (PDG encoding)
  Int_t    fMult;                ///< Multiplicity
  Double_t fT;                   ///< Temperature in the Pt distribution
  Double_t fY0;                  ///< Mean rapidity
  Double_t fSigma;               ///< Simga in the rapidity distribution
  Double_t fPtMin;               ///< Max value of Pt
  Double_t fPtMax;               ///< Min value of Pt
  Double_t fPDGMass;             ///< Particle mass [GeV]
  TF1*     fDistPt;              //! Pointer to the Pt function

  CbmAnaHypYPtGenerator(const CbmAnaHypYPtGenerator&);
  CbmAnaHypYPtGenerator operator=(const CbmAnaHypYPtGenerator&);

  ClassDef(CbmAnaHypYPtGenerator,1);
};

#endif
