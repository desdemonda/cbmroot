/* $Id: CbmEcalPhotonCalibrator.h,v 1.3 2006/07/30 13:10:35 prokudin Exp $ */
/* CbmEcalPhotonCalibrator.h by Mikhail Prokudin
 * Class provides geometry information for correct
 * photon(or other particles) generation and replaces
 * hit producer, to draw all necessary histograms */
/*
 * $Log: CbmEcalPhotonCalibrator.h,v $
 * Revision 1.3  2006/07/30 13:10:35  prokudin
 * Bug hunting
 *
 * Revision 1.2  2006/07/25 20:23:31  prokudin
 * Thresholds and noise added
 *
 * Revision 1.1  2006/07/19 09:34:45  prokudin
 * First version
 *
 */

#ifndef CBMECALPHOTONCALIBRATOR_H
#define CBMECALPHOTONCALIBRATOR_H

#include "FairTask.h"

#include <vector>

class CbmEcalInf;
class CbmEcalStructure;
class TClonesArray;
class TTree;

class CbmEcalPhotonCalibrator : public FairTask
{
public:
  CbmEcalPhotonCalibrator(const char* name, const Int_t iVerbose=1, const char* fileGeo="ecal_FullMC.geo");

  TString GetGeoFileName();

  /** Must be called before GetLowTheta, GetHighTheta,
   ** GetMaxX, GetMaxY**/
  void InitForGenerator();

  /** Returns maximum X for cells of given type**/
  Float_t GetMaxX(Int_t type);
  /** Returns maximum Y for cells of given type**/
  Float_t GetMaxY(Int_t type);
  /** Returns a low boundary for theta **/
  Float_t GetLowTheta(Int_t type);
  /** Returns a high boundary for theta **/
  Float_t GetHighTheta(Int_t type);

  /** Set energy of particles to analize **/
  void SetEnergy(Float_t energy);
  /** Get energy of particles to analize **/
  Float_t GetEnergy() const;

  /** Set noise for ECAL (GeV) **/
  void SetNoise(Float_t noise);

  Float_t GetNoise() const;

  /** Set threshold for ECAL (GeV)**/
  void SetThreshold(Float_t threshold);

  Float_t GetThreshold() const;
 
  /** Set PDG code of particles to analize **/
  void SetPDGCode(Int_t code);
  /** Get PDG code of particles to analize **/
  Int_t GetPDGCode() const;

  virtual InitStatus Init();
  virtual void Exec(Option_t* option);
  virtual void Finish();
  /** Destructor must be virtual because of the virtual
   ** methods **/
  virtual ~CbmEcalPhotonCalibrator();

  /** Set photon birth point **/
  void SetPhotonXYZ(Float_t x=0, Float_t y=0, Float_t z=0);
  /** Set algorithm of cluster construction **/
  void SetAlgo(Int_t algo) {fAlgo=algo;}
private:
  /** Init procedure for fLowTheta, fHiTheta, fMaxX, fMaxY**/
  void InitArrays(UInt_t size=15, UInt_t start=0);
  void InitStructure();
  Float_t GetAngle(Float_t x, Float_t y, Float_t z);
  /** Structure container **/
  CbmEcalStructure* fStr;
  /** Event number **/
  UInt_t fEvent;
  /** Energy of particles to analize **/
  Float_t fEnergy;
  /** PDG code of particles to analize **/
  Int_t fPDGCode;
  /** Name of geometry container **/
  TString fGeoFile;

  /** Lite MC points **/
  TClonesArray* fLitePoints;
  /** ECAL MC points at the begin of the ECAL**/
  TClonesArray* fEcalPoints;
  /** Array of MC tracks **/
  TClonesArray* fMCTracks;

  /** X coordinate of photon birth point **/
  Float_t fBirthX;
  /** Y coordinate of photon birth point **/
  Float_t fBirthY;
  /** Z coordinate of photon birth point **/
  Float_t fBirthZ;

  /** Threshold for ECAL **/
  Float_t fThreshold;
  /** Noise in ECAL **/
  Float_t fNoise;

  CbmEcalInf* fInf;

  /** output tree **/
  TTree* fTree;

  /** Full energy deposited in ECAL **/
  Float_t fFullEnergy;
  /** Energy deposited in ECAL, not in PS **/
  Float_t fEcalEnergy;

  /** Energy deposited in ECAL in 2x2 cluster **/
  Float_t f2x2EcalEnergy;
  /** Energy deposited in ECAL in 3x3 cluster **/
  Float_t f3x3EcalEnergy;
  /** Energy deposited in ECAL in 5x5 cluster **/
  Float_t f5x5EcalEnergy;

  /** Type of ECAL cells used in analysis **/
  Int_t fType;
  /** Coordinates of ECAL hit **/
  Float_t fX;
  Float_t fY;
  Float_t fZ;
  /** Energy and momentum of incoming particle **/
  Float_t fE;
  Float_t fPx;
  Float_t fPy;
  Float_t fPz;
  /** Numeber of steps to find maximum cell **/
  Int_t fSteps;
  /** Algorithm of cluster construction **/
  Int_t fAlgo;

  std::vector<Float_t> fLowTheta;
  std::vector<Float_t> fHighTheta;
  std::vector<Float_t> fMaxX;
  std::vector<Float_t> fMaxY;

  CbmEcalPhotonCalibrator(const CbmEcalPhotonCalibrator&);
  CbmEcalPhotonCalibrator& operator=(const CbmEcalPhotonCalibrator&);

  ClassDef(CbmEcalPhotonCalibrator, 1)
};

/** Set energy of particles to analize **/
inline void CbmEcalPhotonCalibrator::SetEnergy(Float_t energy)
{
  fEnergy=energy;
}

/** Get energy of particles to analize **/
inline Float_t CbmEcalPhotonCalibrator::GetEnergy() const
{
  return fEnergy;
}

/** Set PDG code of particles to analize **/
inline void CbmEcalPhotonCalibrator::SetPDGCode(Int_t code)
{
  fPDGCode=code;
}

/** Get PDG code of particles to analize **/
inline Int_t CbmEcalPhotonCalibrator::GetPDGCode() const
{
  return fPDGCode;;
}

/** Returns maximum X for cells of given type**/
inline Float_t CbmEcalPhotonCalibrator::GetMaxX(Int_t type)
{
  return fMaxX[type];
}

/** Returns maximum Y for cells of given type**/
inline Float_t CbmEcalPhotonCalibrator::GetMaxY(Int_t type)
{
  return fMaxY[type];
}

/** Returns a low boundary for theta **/
inline Float_t CbmEcalPhotonCalibrator::GetLowTheta(Int_t type)
{
  return fLowTheta[type];
}

/** Returns a high boundary for theta **/
inline Float_t CbmEcalPhotonCalibrator::GetHighTheta(Int_t type)
{
  return fHighTheta[type];
}

inline TString CbmEcalPhotonCalibrator::GetGeoFileName() 
{
  return fGeoFile;
}

inline void CbmEcalPhotonCalibrator::SetPhotonXYZ(Float_t x, Float_t y, Float_t z)
{
  fBirthX=x;
  fBirthY=y;
  fBirthZ=z;
}

/** Set noise for ECAL (GeV) **/
inline void CbmEcalPhotonCalibrator::SetNoise(Float_t noise)
{
	fNoise=noise;
}

inline Float_t CbmEcalPhotonCalibrator::GetNoise() const
{
	return fNoise;
}

/** Set threshold for ECAL (GeV)**/
inline void CbmEcalPhotonCalibrator::SetThreshold(Float_t threshold)
{
	fThreshold=threshold;
}

inline Float_t CbmEcalPhotonCalibrator::GetThreshold() const
{
	return fThreshold;
}

#endif
