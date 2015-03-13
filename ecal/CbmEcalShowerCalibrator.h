// Creater tree for shower calibration from MCPoints files.
// This is a source of raw information for shower library.
// Must be exactly one particle with resonably high energy
// (>LowEnergy) per event!
/* $Id: CbmEcalShowerCalibrator.h,v 1.3 2006/09/07 18:19:27 prokudin Exp $*/

/*
 * $Log: CbmEcalShowerCalibrator.h,v $
 * Revision 1.3  2006/09/07 18:19:27  prokudin
 * SCurveRun option added
 *
 * Revision 1.2  2006/08/22 22:31:42  prokudin
 * Optimization
 *
 * Revision 1.1  2006/08/15 13:06:06  prokudin
 * first release of CbmEcalShowerCalibrator
 *
 */

#ifndef CBMECALSHOWERCALIBRATOR_H
#define CBMECALSHOWERCALIBRATOR_H

#include "FairTask.h"

#include <vector>

class CbmGeoEcalPar;
class TClonesArray;
class TTree;
class TVector3;

class CbmEcalShowerCalibrator : public FairTask
{
public:
  /** Standart constructor **/
  CbmEcalShowerCalibrator(const char* name, const Int_t iVerbose=1);

  /** Output cell size **/
  void SetOutCellSize(Float_t cellsize=3.0);
  Float_t GetOutCellSize() const;

  void SetPDGCode(Int_t code=22);
  Int_t GetPDGCode() const;

  /** Set lowest energy to analize **/
  void SetLowEnergy(Float_t energy=0.1);
  Float_t GetLowEnergy() const;

  /** if true generate only showers with dy==0 **/
  void SetSCurveRun(Bool_t run=kTRUE);
  Bool_t GetSCurveRun() const;

  /** Z position of ECAL
   ** -1 for PS surface
   ** -2 for ECAL surface **/
  void SetZPosition(Float_t zpos=-2);
  Float_t GetZPosition() const;

  /** (x,y) -> local number. -1 if fails **/
  Int_t GetLocalNumber(Float_t x, Float_t y) const;

  /** local number -> (x,y) **/
  void GetLocalXY(Int_t num, Float_t& x, Float_t& y) const;

  /** (x,y) -> out number. -1 is fails **/
  Int_t GetOutNumber(Float_t x, Float_t y) const;
private:
  /** Z position of ECAL
   ** -1 for PS surface
   ** -2 for ECAL surface **/
  Float_t fZPosition; //=-2

  /** Pdg code of particles to analize **/
  Int_t fPDGCode; //=22

  /** Lowest energy to analize **/
  Float_t fLowEnergy; //=0.1

  /** Output cellsize. Calculated from fType**/
  Float_t fOutCellSize; //=3.0

  /** if true generate only showers with dy==0 **/
  Bool_t fSCurveRun; //=kFALSE

  /** MCPoints in ECAL **/
  TClonesArray* fLitePoints;
  /** MCPoints in front of the ECAL **/
  TClonesArray* fEcalPoints;
  /** MCTracks array **/
  TClonesArray* fMCTracks;

  /** Output tree. It called ECALshower**/
  TTree* fTree;

  /** MC coordinates of impact point **/
  Float_t fOutMCX;
  Float_t fOutMCY;

  /** Delta vs. real MC X,Y**/
  Int_t fOutDX;
  Int_t fOutDY;

  /** Cell size in simulation. From parameter container **/
  Float_t fCellSize;

  /** MC parameters of particle **/
  Float_t fOutMCE;
  Float_t fOutMCPx;
  Float_t fOutMCPy;
  Float_t fOutMCPz;

  /** Energy in 5x5 output cluster.
   ** If you want co change 25 to something else
   ** please correct CbmEcalShowerCalibrator::InitTree
   ** method **/
  Float_t fOutEnergy[25];
  Float_t fOutPSEnergy[25];

  /** Enent number**/
  Int_t fEvent;

  /** Energy in temporary cluster **/
  Float_t* fLocalEnergy;
  Float_t* fLocalPSEnergy;
  Float_t* fLocalXArr;
  Float_t* fLocalYArr;
  Int_t fLocalSize;

  /** Size of ECAL module **/
  Float_t fModuleSize;

  /** X/Y sizes of local array**/
  Int_t fLocalXSize;
  Int_t fLocalYSize;

  /** Local MC coordinates of ECAL plane hit **/
  Float_t fLocalMCX;
  Float_t fLocalMCY;

  /** MC coordinates of ECAL plane hit **/
  Float_t fMCX;
  Float_t fMCY;

  /** Number of found good particles **/
  Int_t fCount;
public:
  /** Default constructor **/
  CbmEcalShowerCalibrator();

  /** Initing routine **/
  virtual InitStatus Init();

  /** Loop procedure **/
  virtual void Exec(Option_t* option);

  /** Finishing routine **/
  virtual void Finish();

  /** Parameter container init **/
  virtual void SetParContainers();

  /** Destructor **/
  virtual ~CbmEcalShowerCalibrator() {};
private:
  /** Inits fTree and branches **/
  void InitTree();

  /** Get MC parameters of incoming particle **/
  void GetMCParameters();

  /** Get MC coordinates **/
  void GetMCXY(TVector3& mom);

  /** Fill fLocalEnergy and fLocalPSEnergy **/
  void FillLocal();

  /** Clear fLocalEnergy and fLocalPSEnergy **/
  void ClearLocal();

  /** Clear fOutEnergy and fOutPSEnergy **/
  void ClearOut();

  CbmGeoEcalPar* fPar;

  CbmEcalShowerCalibrator(const CbmEcalShowerCalibrator&);
  CbmEcalShowerCalibrator& operator=(const CbmEcalShowerCalibrator&);

  ClassDef(CbmEcalShowerCalibrator,1)

};

inline void CbmEcalShowerCalibrator::SetPDGCode(Int_t code)
{
  fPDGCode=code;
}

inline Int_t CbmEcalShowerCalibrator::GetPDGCode() const
{
  return fPDGCode;
}

inline void CbmEcalShowerCalibrator::SetLowEnergy(Float_t energy)
{
  fLowEnergy=energy;
}

inline Float_t CbmEcalShowerCalibrator::GetLowEnergy() const
{
  return fLowEnergy;
}


inline void CbmEcalShowerCalibrator::SetOutCellSize(Float_t cellsize)
{
  fOutCellSize=cellsize;
}

inline Float_t CbmEcalShowerCalibrator::GetOutCellSize() const
{
  return fOutCellSize;
}


inline void CbmEcalShowerCalibrator::SetZPosition(Float_t zpos)
{
  fZPosition=zpos;
}

inline Float_t CbmEcalShowerCalibrator::GetZPosition() const
{
  return fZPosition;
}

/** if true generate only showers with dy==0 **/
inline void CbmEcalShowerCalibrator::SetSCurveRun(Bool_t run)
{
  fSCurveRun=run;
}

inline  Bool_t CbmEcalShowerCalibrator::GetSCurveRun() const
{
  return fSCurveRun;
} 
/** Clear fOutEnergy and fOutPSEnergy **/
/** Only for perfomance reasons! **/
inline void CbmEcalShowerCalibrator::ClearOut()
{
  for(Int_t i=0;i<25;i++)
  {
    fOutEnergy[i]=0;
    fOutPSEnergy[i]=0;
  }
}


#endif

