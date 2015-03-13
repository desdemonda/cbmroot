// -------------------------------------------------------------------------
// -----                        CbmMvd header file                     -----
// -----                  Created 26/07/04  by V. Friese               -----
// -------------------------------------------------------------------------

/**  CbmMvd.h
 *@author V.Friese <v.friese@gsi.de>
 **
 ** Defines the active detector MVD. Constructs the geometry and
 ** registeres MCPoints.
 **/


#ifndef CBMMVD_H
#define CBMMVD_H 1


#include <map>

#include "TLorentzVector.h"
#include "TVector3.h"
#include "FairGeoMedia.h"
#include "FairGeoBuilder.h"
#include "FairDetector.h"
#include "TKey.h"
#include "TGeoVoxelFinder.h"

#include "CbmMvdDetector.h"
#include "SensorDataSheets/CbmMvdMimosa26AHR.h"


class TClonesArray;
class TList;
class FairVolume;
class CbmMvdPoint;

using std::cout;
using std::endl;




class CbmMvd : public FairDetector
{

 public:

  /** Default constructor **/
  CbmMvd();


  /** Standard constructor.
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
  CbmMvd(const char* name, Bool_t active);


  /** Destructor **/
  virtual ~CbmMvd();


  /** Virtual method ProcessHits
   **
   ** Defines the action to be taken when a step is inside the
   ** active volume. Creates a CbmMvdPoint and adds it to the
   ** collection.
   *@param vol  Pointer to the active volume
   **/
  virtual Bool_t ProcessHits(FairVolume* vol = 0);


  /** Virtual method BeginEvent **/
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


  /** Virtual method Construct geometry
   **
   ** Constructs the MVD geometry
   **/
  virtual void ConstructGeometry();

  virtual void ConstructAsciiGeometry();

  virtual void ConstructRootGeometry();

  virtual Bool_t CheckIfSensitive(std::string name);

  virtual std::map<Int_t, Int_t> GetMap() {return fStationMap;};
  
  private:


    /** Track information to be stored until the track leaves the
	active volume. **/
    Int_t          fTrackID;           //!  track index
    Int_t          fPdg;               //!  track particle type
    Int_t          fVolumeID;          //!  volume id
    TLorentzVector fPosIn, fPosOut;    //!  position
    TLorentzVector fMomIn, fMomOut;    //!  momentum
    Double32_t     fTime;              //!  time
    Double32_t     fLength;            //!  length
    Double32_t     fELoss;             //!  energy loss

    Int_t          fPosIndex;          //!
    TClonesArray*  fCollection;        //!  The hit collection
    Bool_t         kGeoSaved;          //!
    TList*         fGeoPar;            //!  List of geometry parameters
    std::map<Int_t, Int_t> fStationMap;  //! Map from MC volume ID to station number

    /** Private method AddHit
     **
     ** Adds a MvdPoint to the HitCollection
     **/
    CbmMvdPoint* AddHit(Int_t trackID, Int_t pdg, Int_t stationNr, 
			TVector3 posIn, TVector3 pos_out, 
			TVector3 momIn, TVector3 momOut, 
			Double_t time, Double_t length, Double_t eLoss);


    /** Private method ResetParameters
     **
     ** Resets the private members for the track parameters
     **/
    void ResetParameters();

    CbmMvd(const CbmMvd&);
    CbmMvd& operator=(const CbmMvd&);


    ClassDef(CbmMvd,1);

};


inline void CbmMvd::ResetParameters() {
  fTrackID = fVolumeID = 0;
  fPosIn.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fPosOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fMomIn.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fMomOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fTime = fLength = fELoss = 0;
  fPosIndex = 0;
};


#endif 
