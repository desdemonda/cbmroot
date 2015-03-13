
#ifndef CBMSTTPOINT_H
#define CBMSTTPOINT_H 1


#include "FairMCPoint.h"

class TVector3;

class CbmSttPoint : public FairMCPoint
{

 public:

  /** Default constructor **/
  CbmSttPoint();

  /** Constructor with arguments
   *@param trackID  Index of MCTrack
   *@param detID    Detector ID
   *@param pos      Ccoordinates at entrance to active volume [cm]
   *@param mom      Momentum of track at entrance [GeV]
   *@param tof      Time since event start [ns]
   *@param length   Track length since creation [cm]
   *@param eLoss    Energy deposit [GeV]
   **/
  CbmSttPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom,
               Double_t tof, Double_t length, Double_t eLoss);

  /** Destructor **/
  virtual ~CbmSttPoint();

  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;

  /** Getters **/
  Int_t GetStationNo() const {return fStation; }
  Int_t GetLayerNo() const   {return fLayer; }
  Int_t GetChamberNo() const {return fChamber; }

  /** Setters **/
  void  SetLayerNo(Int_t layer)      {fLayer = layer; }
  void  SetStationNo(Int_t station)  {fStation = station; }
  void  SetChamberNo(Int_t chamber)  {fChamber = chamber; }

private:

  Int_t fStation;    // STT station Number
  Int_t fLayer;      // STT layer Number
  Int_t fChamber;    // STT chamber Number

  ClassDef(CbmSttPoint,1)

};


#endif
