// -------------------------------------------------------------------------
// -----                         CbmPsdv1 header file                 -----
// -----                  Created 04/02/05  by Yu.Kharlov              -----
// -------------------------------------------------------------------------

/**  CbmPsd.h
 *@author Yuri Kharlov <Yuri.Kharlov@ihep.ru>
 **
 ** Defines the active detector ECAL with geometry coded here.
 **/



#ifndef CBMPSDWOUTV1_FOR48MODS_SUB3_H
#define CBMPSDWOUTV1_FOR48MODS_SUB3_H


#include "CbmPsd.h"

#include "TLorentzVector.h"

class CbmPsdPoint; 
class FairVolume;
class TClonesArray;


class CbmPsdWoutv1_for48mods_sub3 : public CbmPsd
{

 public:

  /** Default constructor **/
  CbmPsdWoutv1_for48mods_sub3();


  /** Standard constructor.
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
  CbmPsdWoutv1_for48mods_sub3(const char* name, Bool_t active);


  /** Destructor **/
  virtual ~CbmPsdWoutv1_for48mods_sub3();


  /** Virtual method ProcessHits
   **
   ** Defines the action to be taken when a step is inside the
   ** active volume. Creates CbmPsdPoints and adds them to the
   ** collection.
   *@param vol  Pointer to the active volume
   **/
  virtual Bool_t  ProcessHits(FairVolume* vol = 0);


  /** Virtual method Construct geometry
   **
   ** Constructs the PSD geometry
   **/
  virtual void CreateMaterial();
  virtual void ConstructGeometry();

  void SetXshift(Float_t shift) { fXshift = shift;}
  void SetZposition(Float_t position) {fZposition = position; }
  void SetRotYAngle(Float_t rotation) {fRotYAngle = rotation; }//[deg.] //marina
  void SetGeoFile(TString geoFile) { fGeoFile = geoFile; }  

 private:

  /** Track information to be stored until the track leaves the
      active volume. **/
  Int_t          fTrackID;           //!  track index
  Int_t          fVolumeID;          //!  volume id
  Int_t          fModuleID;          //!  module id
  TLorentzVector fPos;               //!  position
  TLorentzVector fMom;               //!  momentum
  Double32_t     fTime;              //!  time
  Double32_t     fLength;            //!  length
   Double32_t     fEloss;             //!  energy loss
 
  Int_t          fPosIndex;          //!
  TClonesArray*  fPsdCollection;     //! Hit collection
  
  Float_t   fXshift;                 //  shift on X axis to have ion beam inside the hole
  Float_t   fZposition;              //  Z position PSD surface
  Float_t   fRotYAngle;              //  PSD rotation around Y axis [deg.] //marina
  TString fGeoFile;  
  

  /** Private method ResetParameters
   **
   ** Resets the private members for the track parameters
   **/
  void ResetParameters();


  ClassDef(CbmPsdWoutv1_for48mods_sub3,3)

};

inline void CbmPsdWoutv1_for48mods_sub3::ResetParameters() {
  fTrackID = fVolumeID = fModuleID = 0;
  fPos.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fMom.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fEloss = 0;
  fPosIndex = 0;
};


#endif
