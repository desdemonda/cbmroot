// -------------------------------------------------------------------------
// -----                      CbmMvdStationPar header file             -----
// -----                  Created 28/10/14 by P.Sitzmann               -----
// -------------------------------------------------------------------------


/** CbmMvdStationPar.h
 *@author P.Sitzmann <p.sitzmann@gsi.de>
 **
 ** Parameter class for the CbmMvdDetector class.
 ** It holds the parameters necessary for tracking.
 **/


#ifndef CBMMVDSTATIONPAR_H
#define CBMMVDSTATIONPAR_H 1



#include <vector>
#include <map>
#include "TNamed.h"

using std::map;

class CbmMvdStationPar : public TNamed
{

 public:

  /** Default constructor **/
  CbmMvdStationPar();

  CbmMvdStationPar(Int_t stationCount);
  
  /** Destructor **/
  virtual ~CbmMvdStationPar();

  Bool_t Init();

  /** Accessors **/
  Int_t    GetStationCount() const { return fStationCount; };
  Double_t GetZPosition(Int_t stationNumber);
  Double_t GetThickness(Int_t stationNumber);
  Double_t GetHeight(Int_t stationNumber);
  Double_t GetWidth(Int_t stationNumber);
  Double_t GetXRes(Int_t stationNumber);
  Double_t GetYRes(Int_t stationNumber);
  Double_t GetRadLength(Int_t stationNumber);
  Double_t GetBeamHeight(Int_t stationNumber);
  Double_t GetBeamWidth(Int_t stationNumber);


  /** Data interface */
  void SetStationCount(Int_t count) { fStationCount = count; };
  void SetZPosition(Int_t stationNumber, Double_t z);
  void SetThickness(Int_t stationNumber, Double_t thickness);
  void SetHeight(Int_t stationNumber, Double_t height);
  void SetWidth(Int_t stationNumber, Double_t width);
  void SetXRes(Int_t stationNumber, Double_t xres);
  void SetYRes(Int_t stationNumber, Double_t yres);
  void SetRadLength(Int_t stationNumber, Double_t length);
  void SetBeamHeight(Int_t stationNumber, Double_t beamheight);
  void SetBeamWidth(Int_t stationNumber, Double_t beamwidth);


  /** Output to screen **/
  void Print(Option_t* opt="") const;



 protected:

  Int_t                   fStationCount;        // Number of Stations, station numbering starts at 0!!!

  map<Int_t , Int_t >  fZPositions;          // map of the z positions of all Stations 
  map<Int_t , Double_t >  fThicknesses;      // in cm 
  map<Int_t , Double_t >  fHeights;          // in cm
  map<Int_t , Double_t >  fWidths;           // in cm
  map<Int_t , Double_t >  fXResolutions;     // in mu m
  map<Int_t , Double_t >  fYResolutions;     // in mu m
  map<Int_t , Double_t >  fRadiationLength;  // in %x0
  map<Int_t , Double_t >  fBeamHeights;      // in cm 
  map<Int_t , Double_t >  fBeamWidths;       // in cm


  ClassDef(CbmMvdStationPar,1);

};



#endif
