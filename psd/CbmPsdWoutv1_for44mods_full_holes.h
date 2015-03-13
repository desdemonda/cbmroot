// -------------------------------------------------------------------------
// -----                         CbmPsdv1 header file                 -----
// -----                  Created 04/02/05  by Yu.Kharlov              -----
// -------------------------------------------------------------------------

/**  CbmPsd.h
 *@author Yuri Kharlov <Yuri.Kharlov@ihep.ru>
 **
 ** Defines the active detector ECAL with geometry coded here.
 **/



#ifndef CBMPSDWOUTV1_FOR44MODS_FULL_HOLES_H
#define CBMPSDWOUTV1_FOR44MODS_FULL_HOLES_H


#include "CbmPsd.h"

#include "TLorentzVector.h"

class CbmPsdPoint; 
class FairVolume;
class TClonesArray;


class CbmPsdWoutv1_for44mods_full_holes : public CbmPsd
{

 public:

  /** Default constructor **/
  CbmPsdWoutv1_for44mods_full_holes();


  /** Standard constructor.
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
  CbmPsdWoutv1_for44mods_full_holes(const char* name, Bool_t active);


  /** Destructor **/
  virtual ~CbmPsdWoutv1_for44mods_full_holes();


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

  CbmPsdWoutv1_for44mods_full_holes(const CbmPsdWoutv1_for44mods_full_holes&);
  CbmPsdWoutv1_for44mods_full_holes operator=(const CbmPsdWoutv1_for44mods_full_holes&);

  ClassDef(CbmPsdWoutv1_for44mods_full_holes,3)

};

inline void CbmPsdWoutv1_for44mods_full_holes::ResetParameters() {
  fTrackID = fVolumeID = fModuleID = 0;
  fPos.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fMom.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fEloss = 0;
  fPosIndex = 0;
};


#endif
