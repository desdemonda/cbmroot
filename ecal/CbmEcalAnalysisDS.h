// For calibrate calorimeter response for UrQMD events
// for photons, electrons, etc.
// by Mikhail Prokudin

/* $Id: CbmEcalAnalysisDS.h,v 1.5 2006/09/18 07:58:04 prokudin Exp $ */

/*
 * $Log: CbmEcalAnalysisDS.h,v $
 * Revision 1.5  2006/09/18 07:58:04  prokudin
 * First implementation of SCurve library
 *
 * Revision 1.2  2006/08/25 19:17:48  prokudin
 * Energy in cell added. Minor bug fixed.
 *
 * Revision 1.1  2006/08/17 18:16:35  prokudin
 * first version of UrqmdCalibrator
 *
 */

#ifndef CBMECALANALYSISDS_H
#define CBMECALANALYSISDS_H

#include "FairTask.h"

#include "TString.h"

#include <list>

class CbmEcalInf;
class CbmEcalStructure;
class CbmEcalPoint;
class CbmEcalCell;
class CbmEcalSCurveLib;
class TTree;
class TClonesArray;


class CbmEcalAnalysisDS : public FairTask
{
public:
  CbmEcalAnalysisDS(const char* name, const Int_t iVerbose=0, const char* fileGeo="ecal_FullMC.geo");

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

  /** Finds peaks on ECAL matrix **/
  void FindPeaks();

  /** Calculates ECAL response for cell **/
  void FillEnergies(CbmEcalCell* cell);

  /** Find nearest track **/
  void FindND();

  /** Returns incoming particle energy **/
  Float_t GetEnergy(Float_t e2, CbmEcalCell* cell);

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

  /** MC coordinates of hit **/
  Float_t fMCX;
  Float_t fMCY;

  /** Coordinates of cell center **/
  Float_t fCellX;
  Float_t fCellY;

  /** Calibration **/
  Float_t fP0a[5];
  Float_t fP0b[5];
  Float_t fP1a[5];
  Float_t fP1b[5];
  
  /** Incoming particle momentum **/
  Float_t fMCE;
  Float_t fMCPX;
  Float_t fMCPY;
  Float_t fMCPZ;

  Float_t fPSE;

  Float_t fE;

  /** Track energy depostion in 2x2 and 3x3 cells **/
  Float_t fE2x2;
  Float_t fE3x3;

  /** Total energy deposited by track in calorimeter **/
  Float_t fTotalTrackEnergy;
  /** Coordinates of cell with maximum energy deposition for this track **/
  Float_t fMaxCellX;
  Float_t fMaxCellY;

  /** Reconstructed energy**/
  Float_t fEReco;

  /** Assymetry **/
  Float_t fAX;
  Float_t fAY;

  /** Reconstructed X,Y **/
  Float_t fXReco;
  Float_t fYReco;

  /** Distance to nearest track **/
  Float_t fND;
  Int_t fNCode;

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
  std::list<CbmEcalCell*> fList; 

  /** SCurve library **/
  CbmEcalSCurveLib* fLib;
public:
  /** Default constructor **/
  CbmEcalAnalysisDS() 
    : FairTask(),
    fPDGType(0),
    fMinEnergy(-1.),
    fGeoFile(""),
    fTree(NULL),
    fEvent(-1),
    fMCX(0.),
    fMCY(0.),
    fCellX(0.),
    fCellY(0.),
    fP0a(),
    fP0b(),
    fP1a(),
    fP1b(),
    fMCE(0.),
    fMCPX(0.),
    fMCPY(0.),
    fMCPZ(0.),
    fPSE(0.),
    fE(0.),
    fE2x2(0.),
    fE3x3(0.),
    fTotalTrackEnergy(0.),
    fMaxCellX(0.),
    fMaxCellY(0.),
    fEReco(0.),
    fAX(0.),
    fAY(0.),
    fXReco(0.),
    fYReco(0.),
    fND(0.),
    fNCode(-1),
    fMotherCode(-1),
    fType(-1),
    fStr(NULL),
    fDiffType(-1),
    fInf(NULL),
    fList(),
    fLib(NULL),
    fLitePoints(NULL),
    fEcalPoints(NULL),
    fMCTracks(NULL)
      {};

  /** Initing routine **/
  virtual InitStatus Init();

  /** Loop procedure **/
  virtual void Exec(Option_t* option);

  /** Finishing routine **/
  virtual void Finish();

  /** Parameter container init **/
  virtual void SetParContainers();

  /** Destructor **/
  virtual ~CbmEcalAnalysisDS() {};
private:
  /** MCPoints in ECAL **/
  TClonesArray* fLitePoints;
  /** MCPoints in front of the ECAL **/
  TClonesArray* fEcalPoints;
  /** MCTracks array **/
  TClonesArray* fMCTracks;

  CbmEcalAnalysisDS(const CbmEcalAnalysisDS&);  
  CbmEcalAnalysisDS& operator=(const CbmEcalAnalysisDS&);  

  ClassDef(CbmEcalAnalysisDS,1)
};

inline void CbmEcalAnalysisDS::SetPDGType(Int_t type)
{
  fPDGType=type;
}

inline Int_t CbmEcalAnalysisDS::GetPDGType() const
{
  return fPDGType;
}

inline void CbmEcalAnalysisDS::SetMinEnergy(Float_t energy)
{
  fMinEnergy=energy;
}

inline Float_t CbmEcalAnalysisDS::GetMinEnergy() const
{
  return fMinEnergy;
}
#endif 

