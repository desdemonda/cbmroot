/** CbmMuchDigiMatch.h
 **@author  M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 **@since   23.03.07
 **@version 1.0
 **
 ** Data class for matching CbmMuchDigi with CbmMuchPoint.
 ** The indices of CbmMuchDigi and CbmMuchDigiMatch correspond to that
 ** in their respective arrays.
 ** The class holds the indices of all MuchPoints corresponding to
 ** the MuchDigi.
 **/


#ifndef CBMMUCHDIGIMATCH_H
#define CBMMUCHDIGIMATCH_H 1


#include "CbmMatch.h"
#include "TArrayI.h"
#include "TArrayD.h"
class TClonesArray;

static const Double_t gkResponseBin    = 1.;

class CbmMuchDigiMatch : public CbmMatch
{

 public:

  /** Default constructor **/
  CbmMuchDigiMatch();

  /** Standard constructor. Initializes this CbmMuchDigiMatch object from the given one. **/
  CbmMuchDigiMatch(CbmMuchDigiMatch* match);

  /** Destructor **/
  virtual ~CbmMuchDigiMatch();

  /**
   * Add a MuchPoint index
   * @param iPoint Point index
   * @value Number of indices present
   */
  //Int_t AddPoint(Int_t iPoint);

  /**
   * Add a charge from a MC point. Index of the point can
   * be accessed through GetRefIndex(i).
   * @param iCharge Charge from the MuchPoint [electrons]
   * @value Number of points the digi contains
   */
  //UInt_t AddCharge(UInt_t iCharge);

  /**
   * Accessor to MuchPoint index.
   * @param i MuchPoint index in the internal array of the digi
   * @value   MuchPointIndex in the output array of MC transport file
   */
  //Int_t GetRefIndex(Int_t i = 0) const;

  /**
   * Gets number of points corresponding to the digi.
   * @value Number of MuchPoints contained in the digi
   */
 // Int_t GetNPoints() const {return fRefIndex.GetSize();}

  /**
   * Gets charge from a MC point. Index of the point can
   * be accessed through GetRefIndex(i).
   * @param i MuchPoint index in the internal array of the digi
   * @value Charge from the corresponding MuchPoint [electrons]
   */
  //UInt_t GetCharge(Int_t i) const;
  //UInt_t GetTotalCharge() const;
  void SortPointsInTime(TClonesArray* points);
  
  void Reset();

  /**
   * Add a charge from a MC point. Index of the point can
   * be accessed through GetRefIndex(i).
   * @param iCharge Charge from the MuchPoint [electrons]
   * @value Number of points the digi contains
   */
  //UInt_t AddCharge(Int_t iPoint, UInt_t iCharge);

  void AddCharge(Int_t iPoint, UInt_t iCharge, Double_t driftTime, TArrayD shape, Double_t mcTime);
  
  Int_t GetNoPrimaryElectrons() { return fTimePerPrimaryElectron.GetSize(); }
  Int_t    GetRefIndexPerPrimaryElectron(Int_t i)  { return fRefIndexPerPrimaryElectron[i]; }
  Int_t    GetChargePerPrimaryElectron(Int_t i)    { return fChargePerPrimaryElectron[i]; }
  Double_t GetTimePerPrimaryElectron(Int_t i)      { return fTimePerPrimaryElectron[i]; }
  Double_t GetMCtimePerPrimaryElectron(Int_t i)    { return fMCtimePerPrimaryElectron[i]; }
  TArrayD GetSignalShape() {return fSignalShape; }
  Double_t GetT0() {return fT0; }
  void AddNoise(Double_t meanNoise);
  Double_t GetMaxCharge();
  Double_t GetTimeStamp(Double_t threshold);
  Double_t GetTimeOverThreshold(Double_t threshold);
  Double_t GetDeadTime() const {return fDeadTime; }
  void SetDeadTime(Double_t deadTime) {fDeadTime = deadTime; }
 private:

 // TArrayI fRefIndex;                    // Array of MC point indices
  TArrayD fMCtimePerPrimaryElectron;    // Array of MC point times
 // TArrayI fCharge;                      // Array of charges from each MC point
  TArrayI fRefIndexPerPrimaryElectron;  // Array of ref indices per primary electron 
  TArrayI fChargePerPrimaryElectron;    // Array of charges per primary electron
  TArrayD fTimePerPrimaryElectron;      // Array of arrival times per primary electron 
  TArrayD fSignalShape;                 // Array of time bins
  Double_t fT0;                         // Start time for the signal shape array
  Double_t fDeadTime;                   // Deadtime
  ClassDef(CbmMuchDigiMatch,1);

};

#endif
