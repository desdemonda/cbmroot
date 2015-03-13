// -------------------------------------------------------------------------
// -----                      CbmVisHit header file                    -----
// -----                  Created 14/03/07  by E. Kryshen              -----
// -------------------------------------------------------------------------

/** CbmVisHit
 *@author Evgeny Kryshen <e.kryshen@gsi.de>
 *@since 14.03.07
 *@version 1.0
 **
 **/
#ifndef CBMVISHIT_H
#define CBMVISHIT_H 1

#include "TObject.h"
#include "TString.h"

class CbmMCTrack;

class CbmVisHit {
public:
  CbmVisHit();
  virtual ~CbmVisHit(){};
  void SetPointId(Int_t id)         {fPointId   = id;     }
  void SetTrackId(Int_t id)         {fTrackId   = id;     }
  void SetMotherPdg(Int_t pdg)      {fMotherPdg = pdg;    }
  void SetTrack(CbmMCTrack* track);
  virtual void SetVisible(Bool_t isVisible){};

  virtual TString GetInfo() {};
  Bool_t GetVisible() {return fVisible;}
  Int_t GetTrackPdg()  {return fTrackPdg;}
  Int_t GetMotherPdg() {return fMotherPdg;}
  Int_t GetTrackId()   {return fTrackId;}
  Int_t GetMotherId()  {return fMotherId;}

protected:
  Int_t        fPointId;
  Int_t        fTrackId;
  Int_t        fTrackPdg;
  Int_t        fMotherId;
  Int_t        fMotherPdg;
  Double_t     fDefaultSize;
  Bool_t       fVisible;
  CbmMCTrack*  fTrack;

  ClassDef(CbmVisHit,1);
};
#endif
