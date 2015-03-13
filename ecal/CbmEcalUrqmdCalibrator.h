// For calibrate calorimeter response for UrQMD events
// for photons, electrons, etc.
// by Mikhail Prokudin

/* $Id: CbmEcalUrqmdCalibrator.h,v 1.2 2006/08/25 19:17:48 prokudin Exp $ */

/*
 * $Log: CbmEcalUrqmdCalibrator.h,v $
 * Revision 1.2  2006/08/25 19:17:48  prokudin
 * Energy in cell added. Minor bug fixed.
 *
 * Revision 1.1  2006/08/17 18:16:35  prokudin
 * first version of UrqmdCalibrator
 *
 */

#ifndef CBMECALURQMDCALIBRATOR_H
#define CBMECALURQMDCALIBRATOR_H

#include "FairTask.h"

#include "TString.h"

#include <list>

class CbmEcalInf;
class CbmEcalStructure;
class CbmEcalPoint;
class CbmEcalCell;
class TTree;
class TClonesArray;


class CbmEcalUrqmdCalibrator : public FairTask
{
public:
  CbmEcalUrqmdCalibrator(const char* name, const Int_t iVerbose=0, const char* fileGeo="ecal_FullMC.geo");

  //TString GetGeoFileName();

  /** Setter and getter for type of particle to
   ** calibrate **/
  void SetPDGType(Int_t type=22);
  Int_t GetPDGType() const;

  /** Setter and getter for minimum energy cut **/
  void SetMinEnergy(Float_t energy=0.5);
  Float_t GetMinEnergy() const;

private:
  /** Fills ECAL structure **/
  void FillStructure();

  /** Initializes tree **/
  void InitTree();

  /** Populates fList with particles **/
  void FillList();

  /** Do everything for one ECAL point **/
  void ProcessHit(CbmEcalPoint* point);

  /** Calculates ECAL response for cell **/
  void FillEnergies(CbmEcalCell* cell, Int_t trackid);

  /** Calculates ECAL response for track **/
  void FillTotalEnergy(Int_t tracknum);

  /** Finds cell with more energy deposition than given **/
  CbmEcalCell* FindNextCell(CbmEcalCell* cell);

  /** Type of particle for calibration **/
  Int_t fPDGType;

  /** Minimum energy for particle to be considered **/
  Float_t fMinEnergy;

  /** Name of Geo file **/
  TString fGeoFile;

  /** Output tree named ECALcal **/
  TTree* fTree;

  /** Event number **/
  Int_t fEvent;

  /** Number of steps before maximum reached **/
  Int_t fSteps;

  /** MC coordinates of hit **/
  Float_t fMCX;
  Float_t fMCY;

  /** Coordinates of cell center **/
  Float_t fCellX;
  Float_t fCellY;

  /** Incoming particle momentum **/
  Float_t fMCE;
  Float_t fMCPX;
  Float_t fMCPY;
  Float_t fMCPZ;


  Float_t fE;

  /** Track energy depostion in 2x2 and 3x3 cells **/
  Float_t fE2x2;
  Float_t fE3x3;

  Float_t fTrackE2x2;
  Float_t fTrackE3x3;
  /** Total energy deposited by track in calorimeter **/
  Float_t fTotalTrackEnergy;
  /** Coordinates of cell with maximum energy deposition for this track **/
  Float_t fMaxCellX;
  Float_t fMaxCellY;

  /** PDG code of mother particle. 0 if none. **/
  Int_t fMotherCode;

  /** Type of cell with maximum energy deposition **/
  Int_t fType;

  /** Structure container **/
  CbmEcalStructure* fStr;

  /** 1 if 3x3 cluster contains cells
   ** with different sizes**/
  Int_t fDiffType;

  /** Structure information **/
  CbmEcalInf* fInf;

  /** List of ECAL points to process **/
  std::list<CbmEcalPoint*> fList; 
public:
  /** Default constructor **/
  CbmEcalUrqmdCalibrator();

  /** Initing routine **/
  virtual InitStatus Init();

  /** Loop procedure **/
  virtual void Exec(Option_t* option);

  /** Finishing routine **/
  virtual void Finish();

  /** Parameter container init **/
  virtual void SetParContainers();

  /** Destructor **/
  virtual ~CbmEcalUrqmdCalibrator() {};
private:
  /** MCPoints in ECAL **/
  TClonesArray* fLitePoints;
  /** MCPoints in front of the ECAL **/
  TClonesArray* fEcalPoints;
  /** MCTracks array **/
  TClonesArray* fMCTracks;

  CbmEcalUrqmdCalibrator(const CbmEcalUrqmdCalibrator&);
  CbmEcalUrqmdCalibrator& operator=(const CbmEcalUrqmdCalibrator&);

  ClassDef(CbmEcalUrqmdCalibrator,1)
};

inline void CbmEcalUrqmdCalibrator::SetPDGType(Int_t type)
{
  fPDGType=type;
}

inline Int_t CbmEcalUrqmdCalibrator::GetPDGType() const
{
  return fPDGType;
}

inline void CbmEcalUrqmdCalibrator::SetMinEnergy(Float_t energy)
{
  fMinEnergy=energy;
}

inline Float_t CbmEcalUrqmdCalibrator::GetMinEnergy() const
{
  return fMinEnergy;
}
#endif 

