// -------------------------------------------------------------------------
// -----                      CbmMvdStation header file                -----
// -----                  Created 02/12/08  by V. Friese               -----
// -------------------------------------------------------------------------


/** CbmMvdStation.h
 *@author V.Friese <v.friese@gsi.de>
 **
 ** Auxiliary class for the CbmMvdDigitise class.
 ** It holds the parameters of one MVD station and an array of its MvdPoints.
 **/


#ifndef CBMMVDSTATION_H
#define CBMMVDSTATION_H 1



#include <vector>
#include "TNamed.h"


using std::vector;


class CbmMvdPoint;



class CbmMvdStation : public TNamed
{

 public:

  /** Default constructor **/
  CbmMvdStation();


  /** Standard constructor 
  *@param name      Volume name of station
  *@param stationNr Station number
  *@param volumeId  Unique volume identifier 
  *@param z         z position of station centre [cm]
  *@param d         thickness of station [cm]
  *@param rmin      Inner radius [cm]
  *@param rmax      Outer radius [cm]
  **/
  CbmMvdStation(const char* name, Int_t stationNr, Int_t volumeId,
		Double_t z, Double_t d, Double_t rmin, Double_t rmax);


  /** Destructor **/
  virtual ~CbmMvdStation();


  /** Accessors **/
  Int_t    GetStationNr()  const { return fStationNr; }
  Int_t    GetVolumeId()   const { return fVolumeId; }
  Double_t GetZ()          const { return fZ; }
  Double_t GetD()          const { return fD; }
  Double_t GetRmin()       const { return fRmin; }
  Double_t GetRmax()       const { return fRmax; }
  Int_t    GetNPoints()    const { return fPoints.size(); }
  CbmMvdPoint* GetPoint(Int_t iPoint) const { return fPoints[iPoint]; }


  /** Add one MvdPoint to the vector **/
  void AddPoint(CbmMvdPoint* point) { fPoints.push_back(point); }


  /** Clear MvdPoint vector **/
  void Clear() { fPoints.clear(); }


  /** Output to screen **/
  virtual void Print(Option_t* opt="") const;



 protected:

  Int_t        fStationNr;        // Station identifier
  Int_t        fVolumeId;         // MC unique volume ID
  Double32_t   fZ;                // z position of station centre [cm]
  Double32_t   fD;                // Thickness of station
  Double32_t   fRmin;             // Inner radius [cm]
  Double32_t   fRmax;             // Outer radius [cm]
  vector<CbmMvdPoint*>  fPoints;  // Vector of MvdPoints
  


  ClassDef(CbmMvdStation,1);

};



#endif
