// -------------------------------------------------------------------------
// -----                     CbmUnigenGenerator                        -----
// -----              Created 2006/Jul/04  by D. Kresan                -----
// -------------------------------------------------------------------------
#ifndef CBM_UNIGEN_GENERATOR
#define CBM_UNIGEN_GENERATOR

#include "TString.h"
#include "FairGenerator.h"

class TFile;
class TTree;
class UEvent;
class FairPrimaryGenerator;


class CbmUnigenGenerator : public FairGenerator {

private:
    Int_t         fEvents;           //! Current event number
    TFile        *fInputFile;        //! Input file
    TString       fFileName;         //! Input file Name
    TTree        *fInTree;           //! Input tree
    UEvent       *fEvent;            //! Input event
    Bool_t        fCM;               //! CM flag
    Double_t      fBetaCM;           //! CM velocity
    Double_t      fGammaCM;          //! CM gamma factor

    Double32_t fPhiMin, fPhiMax;          // Limits of event plane angle
    Bool_t     fEventPlaneSet;            // Flag whether event plane angle is used

    void CloseInput();

    CbmUnigenGenerator(const CbmUnigenGenerator&);
    CbmUnigenGenerator& operator=(const CbmUnigenGenerator&);

public:
    CbmUnigenGenerator();
    CbmUnigenGenerator(TString fileName);
    virtual ~CbmUnigenGenerator();

  /** Public method SetEventPlane 
   **@param phiMin   Lower limit for event plane angle [rad]
   **@param phiMax   Upper limit for event plane angle [rad]
   **If set, an event plane angle will be generated with flat
   **distrtibution between phiMin and phiMax. 
   **/
  void SetEventPlane(Double_t phiMin, Double_t phiMax);

    virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);

    ClassDef(CbmUnigenGenerator,2);
};


#endif
