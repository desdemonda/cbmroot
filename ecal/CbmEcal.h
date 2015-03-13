// -------------------------------------------------------------------------
// -----                         CbmEcal header file                 -----
// -----                  Created  04/02/05  by Yu.Kharlov             -----
// -----                  Modified 11/04/05  by B.Zagreev              -----
// -------------------------------------------------------------------------

/**  CbmEcal.h
 *@author Yuri Kharlov
 **
 ** Defines the active detector ECAL with geometry coded here.
 **/

/* $Id: CbmEcal.h,v 1.12 2006/07/29 00:33:53 prokudin Exp $ */

/* History of cvs commits:
 *
 * $Log: CbmEcal.h,v $
 * Revision 1.12  2006/07/29 00:33:53  prokudin
 * Commenting. Bringing code to coding conventions. New way of parameter file processing
 *
 * Revision 1.11  2006/07/24 21:45:02  prokudin
 * Now we write information to parameter file
 *
 * Revision 1.10  2006/07/17 13:59:06  prokudin
 * fMat1 removed. At last.
 *
 * Revision 1.9  2006/07/14 15:09:18  prokudin
 * New functionality added
 *
 * Revision 1.8  2006/07/13 14:42:16  prokudin
 * GetCellCoord now static
 *
 * Revision 1.7  2006/07/12 08:41:38  prokudin
 * String compare removed. Using FairVolume instead.
 *
 * Revision 1.6  2006/06/30 13:04:22  kharlov
 * Added a function SetSpecialPhysicsCuts()
 *
 * Revision 1.5  2006/06/30 06:17:17  kharlov
 * Optimization, commenting
 *
 * Revision 1.4  2006/06/12 11:59:47  kharlov
 * Passing geometry file name via the constructor argument
 *
 * Revision 1.3  2006/04/25 06:52:17  kharlov
 * Remove CbmEcalv[1,2] and leave the only class CbmEcal
 *
 * Revision 1.4  2006/04/02 16:55:23  kharlov
 * Merging full and fast MC
 *
 * Revision 1.3  2006/01/31 17:07:17  kharlov
 * Correction for CbmEcal
 *
 * Revision 1.2  2006/01/27 17:26:30  kharlov
 * First adaptation of ECAL detailed geometry to cbmroot2
 *
 */


#ifndef CBMECALV1_H
#define CBMECALV1_H


#include "CbmEcalPointLite.h"
#include "CbmEcalStructure.h"
#include "CbmEcalInf.h"

#include "FairDetector.h"

#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TVector3.h"

class CbmEcalPoint; 
class FairVolume;

#define kNumberOfECALSensitiveVolumes 6

class CbmEcal : public FairDetector
{

public:

  /** Default constructor **/
  CbmEcal();


  /** Standard constructor.
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
  CbmEcal(const char* name, Bool_t active, 
	  const char* fileGeo="ecal_FastMC.geo");


  /** Destructor **/
  virtual ~CbmEcal();


  /** Virtual method ProcessHits
   **
   ** Defines the action to be taken when a step is inside the
   ** active volume. Creates CbmEcalPoints and adds them to the
   ** collection.
   *@param vol  Pointer to the active volume
   **/
  virtual Bool_t  ProcessHits(FairVolume* vol = NULL);


  /** Virtual method Construct geometry
   **
   ** Constructs the ECAL geometry
   **/
  virtual void ConstructGeometry();

  virtual void EndOfEvent();
  virtual void BeginEvent();
  virtual void Reset();
  virtual void Print() const;
  virtual void CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset);
  virtual void Register();
  virtual void ChangeHit(CbmEcalPointLite* oldHit=NULL);
  virtual void FinishPrimary();

  virtual void Initialize();

  /** Accessor to the hit collection **/                       
  virtual TClonesArray* GetCollection(Int_t iColl) const;
  virtual void SetSpecialPhysicsCuts();
  
  /** Get maximum volume id according
   ** to parameter container **/
  static Int_t GetVolIdMax();
  /** Get maximum volume id according to Inf file **/
  static Int_t GetVolIdMaxInf();

  /** Get cell coordinates according
   ** to parameter container **/
  static Bool_t GetCellCoord(Int_t fVolumeID, Float_t &x, Float_t &y, Int_t& tenergy);
  /** Get cell coordinates according to Inf file **/
  static Bool_t GetCellCoordInf(Int_t fVolumeID, Float_t &x, Float_t &y, Int_t& tenergy);
  CbmEcalInf* GetInf() const {return fInf;}

  /** size in Mb (2^20)**/
  void SetMaximumFileSize(Long64_t size);  

protected:
  CbmEcalPoint* AddHit(Int_t trackID, Int_t detID, TVector3 pos,
                       TVector3 mom, Double_t time, Double_t length,
                       Double_t eLoss);
  CbmEcalPointLite* AddLiteHit(Int_t trackID, Int_t detID, Double32_t time, Double32_t eLoss);

private:
  Bool_t FindVolumeID(Int_t volnum);
  Bool_t FillLitePoint(Int_t volnum);
  void FillWallPoint();
  /** Private method ResetParameters
   **
   ** Resets the private members for the track parameters
   **/
  void ResetParameters();
  void SetEcalCuts(Int_t medium);
  CbmEcalPointLite* FindHit(Int_t VolId, Int_t TrackId);

private:
  CbmEcalInf*  fInf;	//!
  Option_t* fDebug;	//!

  /** returns type of volume **/
  Int_t GetVolType(Int_t volnum);
  /** Track information to be stored until the track leaves the
      active volume. **/
  /**  track index **/
  Int_t          fTrackID;           //!  
  /** volume id **/
  Int_t          fVolumeID;          //!  
  /** position **/
  TLorentzVector fPos;               //!  
  /** momentum **/
  TLorentzVector fMom;               //!  
  /** time **/
  Double32_t     fTime;              //!
  /** length **/
  Double32_t     fLength;            //!
  /** energy loss **/
  Double32_t     fELoss;             //!
  /** **/ 
  Int_t          fPosIndex;          //!

  /** MC point collection on ECAL wall **/ 
  TClonesArray*  fEcalCollection;    //! 
  /** MC point collection inside ECAL **/ 
  TClonesArray*  fLiteCollection;    //! 
  /** ECAL geometry parameters **/
  /** x,y,z size of outer ECAL box [cm] **/
  Float_t fEcalSize[3];	//!
  /** x,y,z size of inner ECAL beam hole [cm] **/
  Float_t fEcalHole[3];	//!
  /** transverce cell size [cm] **/
  Float_t fCellSize;	//!
  /** Z-position of ECAL from the target [cm] **/
  Float_t fZEcal;	//!
  /** thickness of one lead layer [cm] **/
  Float_t fThicknessLead;	//!
  /** thickness of one scintillator layer [cm] **/
  Float_t fThicknessScin;	//!
  /** thickness of one tyvek layer [cm] **/
  Float_t fThicknessTyvk;	//!
  /** total thickness of one layer [cm] **/
  Float_t fThicknessLayer;	//!
  /** number of layers per cell **/
  Int_t   fNLayers;	//!
  /** scall factor for fCellSize and fThicknessLayer **/
  Float_t fGeoScale;	//!
  /** Number of columns in ECAL1 **/
  Int_t   fNColumns1;	//!
  /** Number of rows in ECAL1 **/
  Int_t   fNRows1;	//!
  /** Number of columns in ECAL2 **/
  Int_t   fNColumns2;	//!
  /** Number of rows in ECAL2 **/
  Int_t   fNRows2;	//!
  /** thickness of preshower lead [cm] **/
  Float_t fThicknessPSlead;	//!
  /** thickness of preshower scintillator [cm] **/
  Float_t fThicknessPSscin;	//!
  /** gap between ECAL cell and preshower [cm] **/
  Float_t fEcalPSgap;	//!
  /** max number of columns in ECAL1 or ECAL2 **/
  Int_t   fNColumns;	//!
  /** max number of rows in ECAL1 or ECAL2 **/
  Int_t   fNRows;	//!
  /** Max number of ECAL cells **/
  Int_t   fVolIdMax;	//!
  /** Number of first hit for current primary **/
  Int_t fFirstNumber;	//!
  /** Material of ther absorber **/
  TString fAbsorber;	//!
  /** Map of volumes in ECAL
   ** fVolArr[0]==code of sensivite wall
   ** fVolArr[1]==code of PS Lead
   ** fVolArr[2]==code of PS Scin
   ** fVolArr[4]==code of Lead
   ** fVolArr[3]==code of Tyveec
   ** fVolArr[5]==code of scintillator
   **/
  Int_t fVolArr[kNumberOfECALSensitiveVolumes];

  CbmEcal(const CbmEcal&);
  CbmEcal& operator=(const CbmEcal&);

  ClassDef(CbmEcal,1)

};

inline void CbmEcal::ResetParameters()
{
  fTrackID = fVolumeID = 0;
  fPos.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fMom.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fTime = fLength = fELoss = 0;
  fPosIndex = 0;
};


#endif
