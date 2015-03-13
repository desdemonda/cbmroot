/** CbmMuch.h
 *
 * @author  M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 * @version 1.0
 * @since   21.03.07
 *
 *  Class for MUon CHambers detector
 *
 */

#ifndef CBMMUCH_H
#define CBMMUCH_H


#include "TLorentzVector.h"
#include "FairDetector.h"


class TClonesArray;
class CbmMuchPoint;
class FairVolume;
class CbmGeoMuchPar;
class TGeoMedium;

class CbmMuch : public FairDetector
{

 public:

  /** Default constructor **/
  CbmMuch();


  /** Standard constructor.
   *@param name    detector name
   *@param active  sensitivity flag
   **/
  CbmMuch(const char* name, Bool_t active);


  /** Destructor **/
  virtual ~CbmMuch();


  /** Virtual method ProcessHits
   **
   ** Defines the action to be taken when a step is inside the
   ** active volume. Creates a CbmMuchPoint and adds it to the
   ** collection.
   *@param vol  Pointer to the active volume
   **/
  virtual Bool_t ProcessHits(FairVolume* vol = 0);


  /** Virtual method BeginEvent
   **
   ** If verbosity level is set, print hit collection at the
   ** end of the event and resets it afterwards.
   **/

  virtual void BeginEvent();

  /** Virtual method EndOfEvent
   **
   ** If verbosity level is set, print hit collection at the
   ** end of the event and resets it afterwards.
   **/

  virtual void EndOfEvent();



  /** Virtual method Register
   **
   ** Registers the hit collection in the ROOT manager.
   **/
  virtual void Register();


  /** Accessor to the hit collection **/
  virtual TClonesArray* GetCollection(Int_t iColl) const;


  /** Virtual method Print
   **
   ** Screen output of hit collection.
   **/
  virtual void Print() const;


  /** Virtual method Reset
   **
   ** Clears the hit collection
   **/
  virtual void Reset();


  /** Virtual method CopyClones
   **
   ** Copies the hit collection with a given track index offset
   *@param cl1     Origin
   *@param cl2     Target
   *@param offset  Index offset
   **/
  virtual void CopyClones(TClonesArray* cl1, TClonesArray* cl2,
			  Int_t offset);


  /** Virtaul method Construct geometry
   **
   ** Constructs the Much geometry
   **/
  virtual void ConstructGeometry();


  //  void SaveGeoParams();

 private:


  /** Track information to be stored until the track leaves the
      active volume. **/
  Int_t          fTrackID;           //!  track index
  Int_t          fVolumeID;          //!  volume id
  TLorentzVector fPosIn, fPosOut;    //!  position
  TLorentzVector fMomIn, fMomOut;    //!  momentum
  Double32_t     fTime;              //!  time
  Double32_t     fLength;            //!  length
  Double32_t     fELoss;             //!  energy loss

  Int_t          fPosIndex;          //!
  TClonesArray*  fMuchCollection;    //!  The hit collection
  Bool_t         kGeoSaved;          //!
  TList *flGeoPar; //!
  CbmGeoMuchPar* fPar;               //!  parameter container
  /** Private method AddHit
   **
   ** Adds a MuchPoint to the HitCollection
   **/
  CbmMuchPoint* AddHit(Int_t trackID, Int_t detID,
		       TVector3 posIn, TVector3 posOut,
		       TVector3 momIn, TVector3 momOut,
		       Double_t time, Double_t length, Double_t eLoss);


  /** Private method ResetParameters
   **
   ** Resets the private members for the track parameters
   **/
  void ResetParameters();
  Int_t GetDetId(FairVolume* vol);
private:
  Int_t Intersect(Float_t x, Float_t y, Float_t lx, Float_t ly, Float_t r);
  TGeoMedium* CreateMedium(const char* matName);

  CbmMuch(const CbmMuch&);
  CbmMuch& operator=(const CbmMuch&);

  ClassDef(CbmMuch,1);

};


inline void CbmMuch::ResetParameters() {
  fTrackID = fVolumeID = 0;
  fPosIn .SetXYZM(0.0, 0.0, 0.0, 0.0);
  fPosOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fMomIn .SetXYZM(0.0, 0.0, 0.0, 0.0);
  fMomOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fTime = fLength = fELoss = 0;
  fPosIndex = 0;
};


#endif
