// -------------------------------------------------------------------------
// -----                      CbmVisPoint header file                  -----
// -----                  Created 02/04/07  by E. Kryshen              -----
// -------------------------------------------------------------------------

/** CbmVisPoint
 *@author Evgeny Kryshen <e.kryshen@gsi.de>
 *@since 02.04.07
 *@version 1.0
 **
 ** Class for visualization of MC points with the electron spread
 ** projection in xy plane. The spread projection is drawn as rectangle
 ** around IN and OUT positions in xy plane with a given radius.
 ** Constructors from CbmStsPoint and CbmMuchPoint are implemented.
 ** Default electron spread radius in STS is 0.0005 cm, in MUCH - 0.03 cm
 **/


#ifndef CBMVISPOINT_H
#define CBMVISPOINT_H 1

#include "TGObject.h"
#include "TMarker.h"
#include "TPolyLine.h"
class FairMCPoint;
class CbmStsPoint;
class CbmMuchPoint;
class CbmMCTrack;
class CbmMuchStation;

class CbmVisPoint : public TPolyLine{
ClassDef(CbmVisPoint,1)

public:

  CbmVisPoint(Double_t xIn, Double_t yIn,
	       Double_t xOut, Double_t yOut, Double_t r);
  CbmVisPoint(CbmMuchPoint* pt, Double_t r=0.06);
  CbmVisPoint(CbmStsPoint* pt, Double_t r=0.0005);
  ~CbmVisPoint();

  void CreateSpread(Double_t xIn, Double_t yIn,
		    Double_t xOut, Double_t yOut, Double_t r);

  void SetStationNr(Int_t stationNr) {fStationNr = stationNr; }
  void SetMotherPdg(Int_t pdg)       {fMotherPdg = pdg;       }
  void SetTrack(CbmMCTrack* track);

  TString GetInfo();
  Int_t GetTrackPdg()  {return fTrackPdg;}
  Int_t GetMotherPdg() {return fMotherPdg;}
  Int_t GetTrackId()   {return fTrackId;}
  Int_t GetMotherId()  {return fMotherId;}

  TMarker* GetMarkerIn()  {return fMarkerIn;}
  TMarker* GetMarkerOut() {return fMarkerOut;}
  void DrawSpread();
  void SetVisible(Bool_t isVisible);
private:
  FairMCPoint *fPoint;       // MC point reference
  TMarker* fMarkerIn;       // marker at the IN position
  TMarker* fMarkerOut;      // marker at the OUT position
  Double_t fR;              // spread radius
  Double_t fX0,fY0;         // coordinates of the spread center

  Bool_t fVisible;
  Size_t fDefaultMarkerSize;

  Int_t fStationNr;
  Int_t fTrackId;
  Int_t fTrackPdg;
  Int_t fMotherId;
  Int_t fMotherPdg;
  CbmMCTrack* fTrack;
};
#endif
