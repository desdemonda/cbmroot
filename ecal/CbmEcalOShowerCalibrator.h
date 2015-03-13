// Creater tree for shower calibration from MCPoints files.
// This is a source of raw information for shower library.
// Must be exactly one particle with resonably high energy
// (>LowEnergy) per event!
/* $Id: CbmEcalOShowerCalibrator.h,v 1.3 2006/09/07 18:19:27 prokudin Exp $*/

/*
 * $Log: CbmEcalOShowerCalibrator.h,v $
 * Revision 1.3  2006/09/07 18:19:27  prokudin
 * SCurveRun option added
 *
 * Revision 1.2  2006/08/22 22:31:42  prokudin
 * Optimization
 *
 * Revision 1.1  2006/08/15 13:06:06  prokudin
 * first release of CbmEcalOShowerCalibrator
 *
 */

#ifndef CBMECALOSHOWERCALIBRATOR_H
#define CBMECALOSHOWERCALIBRATOR_H

#include "FairTask.h"

#include <vector>

class CbmGeoEcalPar;
class TClonesArray;
class TTree;
class TVector3;

class CbmEcalOShowerCalibrator : public FairTask
{
public:
  /** Standart constructor **/
  CbmEcalOShowerCalibrator(const char* name, const Int_t iVerbose=1);

  void SetPDGCode(Int_t code=22);
  Int_t GetPDGCode() const;

  /** Set lowest energy to analize **/
  void SetLowEnergy(Float_t energy=0.1);
  Float_t GetLowEnergy() const;

  /** Z position of ECAL
   ** -1 for PS surface
   ** -2 for ECAL surface **/
  void SetZPosition(Float_t zpos=-2);
  Float_t GetZPosition() const;

  /** (x,y) -> local number. -1 if fails **/
  Int_t GetLocalNumber(Float_t x, Float_t y) const;

private:
  /** Z position of ECAL
   ** -1 for PS surface
   ** -2 for ECAL surface **/
  Float_t fZPosition; //=-2

  /** Pdg code of particles to analize **/
  Int_t fPDGCode; //=22

  /** Lowest energy to analize **/
  Float_t fLowEnergy; //=0.1

  /** MCPoints in ECAL **/
  TClonesArray* fLitePoints;	//!
  /** MCPoints in front of the ECAL **/
  TClonesArray* fEcalPoints;	//!
  /** MCTracks array **/
  TClonesArray* fMCTracks;	//!

  /** Output tree. It called ECALshower**/
  TTree* fTree;		//!

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

  /** Enent number**/
  Int_t fEvent;

  /** Energy in temporary cluster **/
  Float_t* fLocalEnergy;	//!
  Float_t* fLocalPSEnergy;	//!
  Int_t fLocalSize;
  Int_t fLocalL;

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
  CbmEcalOShowerCalibrator();

  /** Initing routine **/
  virtual InitStatus Init();

  /** Loop procedure **/
  virtual void Exec(Option_t* option);

  /** Finishing routine **/
  virtual void Finish();

  /** Parameter container init **/
  virtual void SetParContainers();

  /** Destructor **/
  virtual ~CbmEcalOShowerCalibrator() {};
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

  CbmGeoEcalPar* fPar;

  CbmEcalOShowerCalibrator(const CbmEcalOShowerCalibrator&);
  CbmEcalOShowerCalibrator& operator=(const CbmEcalOShowerCalibrator&);

  ClassDef(CbmEcalOShowerCalibrator,1)

};

inline void CbmEcalOShowerCalibrator::SetPDGCode(Int_t code)
{
  fPDGCode=code;
}

inline Int_t CbmEcalOShowerCalibrator::GetPDGCode() const
{
  return fPDGCode;
}

inline void CbmEcalOShowerCalibrator::SetLowEnergy(Float_t energy)
{
  fLowEnergy=energy;
}

inline Float_t CbmEcalOShowerCalibrator::GetLowEnergy() const
{
  return fLowEnergy;
}


inline void CbmEcalOShowerCalibrator::SetZPosition(Float_t zpos)
{
  fZPosition=zpos;
}

inline Float_t CbmEcalOShowerCalibrator::GetZPosition() const
{
  return fZPosition;
}

#endif

