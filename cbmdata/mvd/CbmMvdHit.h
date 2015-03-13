// -------------------------------------------------------------------------
// -----                      CbmMvdHit header file                    -----
// -----                 Created 07/11/06  by V. Friese                -----
// -------------------------------------------------------------------------


/** CbmMvdHit.h
 *@author V.Friese <v.friese@gsi.de>
 **
 ** Data class for hits in the CBM-MVD. 
 ** Data level RAW.
 ** Original source (CbmStsMapsHit) by M. Deveaux.
 ** 
 ** Hit flag not used up to now (will come later with real hit finding).
 **/


#ifndef CBMMVDHIT_H
#define CBMMVDHIT_H 1


#include "CbmPixelHit.h"
#include "CbmMvdDetectorId.h"
#include "TClonesArray.h"
#include <vector>

#include "TVector3.h"

class CbmMvdHit : public CbmPixelHit, public CbmMvdDetectorId 
{

 public:    

  /** Default constructor **/
  CbmMvdHit();


  /** Standard constructor 
  *@param statNr Station number
  *@param pos    Position coordinates [cm]
  *@param dpos   Errors in position coordinates [cm]
  *@param flag   Hit flag
  **/
  CbmMvdHit(Int_t statNr, TVector3& pos, TVector3& dpos, Int_t indexCentralX, Int_t indexCentralY=0, Int_t fClusterIndex=0, Int_t flag=0);



  /** Destructor **/
  virtual ~CbmMvdHit();    


  /** Output to screen **/
  virtual void Print(const Option_t* opt = 0) const;


  /** Accessors **/
  Int_t GetSystemId()   const { return SystemId(fDetectorID); };
  virtual Int_t GetStationNr() const { return StationNr(fDetectorID); };
  Int_t GetFlag()       const { return fFlag; };
  Int_t GetClusterIndex() const { return fClusterIndex;};
  Int_t GetIndexCentralX() const { return fIndexCentralX;}; // returns index of center of gravity
  Int_t GetIndexCentralY() const { return fIndexCentralY;}; // returns index of center of gravity
  void GetDigiIndexVector(TClonesArray* cbmMvdClusterArray, std::vector<Int_t>* digiIndexVector);
  Double_t GetTimeStamp() const { return fTimeStamp; }
  Double_t GetTimeStampError() const { return fTimeStampError; }
  
  Int_t GetRefIndex() { return fFlag; }
  

  void SetTimeStamp( Double_t time ){fTimeStamp = time;};
  void SetTimeStampError (Double_t timeError) {fTimeStampError = timeError;};

 protected:

  Int_t fFlag;     // Hit flag; to be used later
  Int_t fClusterIndex;
  Int_t fIndexCentralX;
  Int_t fIndexCentralY;
  

  Double_t fTimeStamp;
  Double_t fTimeStampError;  

  Int_t fDetectorID;
 


  ClassDef(CbmMvdHit,2);

};


#endif
